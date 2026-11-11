"""Exercise source-audit gaps that the Doxygen XML inventory cannot detect."""
from __future__ import annotations

import contextlib
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock

_SCRIPT = Path(__file__).resolve().parents[1] / "check_source_doxygen.py"
_SPEC = importlib.util.spec_from_file_location("check_source_doxygen", _SCRIPT)
_MODULE = importlib.util.module_from_spec(_SPEC)
sys.modules[_SPEC.name] = _MODULE
_SPEC.loader.exec_module(_MODULE)


class SourceDoxygenAuditTest(unittest.TestCase):
    """Require source comments and reject stale or overbroad manual review records."""

    def setUp(self) -> None:
        """Provide isolated source and XML directories for each audit."""
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.xml = self.root / "xml"
        self.xml.mkdir()
        (self.xml / "index.xml").write_text("<doxygenindex/>")

    def run_gate(self, manifest: Path) -> int:
        """Run the CLI entry point while keeping expected diagnostic output quiet."""
        args = [str(_SCRIPT), str(self.root), "--xml-dir", str(self.xml),
                "--review-manifest", str(manifest)]
        with mock.patch.object(sys, "argv", args), contextlib.redirect_stdout(io.StringIO()):
            return _MODULE.main()

    def review(self, source: Path) -> Path:
        """Record a review of one exact source version, without waiving comments."""
        manifest = self.root / "reviews.json"
        manifest.write_text(json.dumps({str(source): {
            "sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
            "reason": "The placeholder condition is inside the inventoried function.",
        }}))
        return manifest

    def test_missing_cpp_and_java_apis_are_reported(self) -> None:
        """Do not skip private methods, conversions, free functions or qualified definitions."""
        (self.root / "missing.h").write_text(
            "class Missing { Missing(); ~Missing(); operator bool() const; void method(); };\n"
            "void Free();\nvoid Missing::method() {}\n")
        (self.root / "Missing.java").write_text(
            "class MissingJava { MissingJava() {} void method() {} }\n")
        entities, _, _ = _MODULE.inventory(self.root)
        missing = [entity.name for entity in entities if not entity.documented]
        self.assertEqual(len(missing), 10)
        self.assertTrue({"Missing", "Missing::method", "operator bool", "MissingJava", "Free"}
                        <= set(missing))

    def test_defaulted_definition_requires_exact_scoped_declaration(self) -> None:
        """A documented defaulted destructor covers only the corresponding class."""
        (self.root / "present.h").write_text(
            "/** @brief Documented type. */\nclass Present { public:\n"
            "/** @brief Release instance state. */\n~Present();\n};\n"
            "Present::~Present() = default;\nOther::~Other() = default;\n")
        entities, _, _ = _MODULE.inventory(self.root)
        _MODULE.mapped_definitions(self.xml, entities)
        missing = [entity.name for entity in entities if not entity.documented]
        self.assertEqual(missing, ["Other::~Other"])

    def test_xml_links_to_documented_declaration(self) -> None:
        """Use exact XML locations when the definition uses different parameter names."""
        header = self.root / "api.h"
        implementation = self.root / "api.cc"
        header.write_text("/** @brief Return the input. */\nint identity(int input);\n")
        implementation.write_text("int identity(int value) { return value; }\n")
        (self.xml / "index.xml").write_text('<doxygenindex><compound refid="api"/></doxygenindex>')
        (self.xml / "api.xml").write_text(
            '<doxygen><compounddef><sectiondef><memberdef kind="function">'
            f'<location file="{implementation}" line="1" bodyfile="{implementation}" '
            f'bodystart="1" declfile="{header}" declline="2"/>'
            '</memberdef></sectiondef></compounddef></doxygen>')
        entities, _, _ = _MODULE.inventory(self.root)
        _MODULE.mapped_definitions(self.xml, entities)
        self.assertTrue(all(entity.documented for entity in entities))

    def test_changed_source_invalidates_parser_review(self) -> None:
        """A valid review stops applying after any change to the reviewed source."""
        source = self.root / "placeholder.h"
        source.write_text("/** @brief Return a placeholder. */\nint run() { if () {} return 0; }\n")
        manifest = self.review(source)
        self.assertEqual(self.run_gate(manifest), 0)
        source.write_text(source.read_text() + "// A later edit requires review.\n")
        self.assertEqual(self.run_gate(manifest), 1)

    def test_review_never_waives_missing_api_comment(self) -> None:
        """Even a current manual review cannot hide an undocumented function."""
        source = self.root / "missing.h"
        source.write_text("int run() { if () {} return 0; }\n")
        self.assertEqual(self.run_gate(self.review(source)), 1)

    def test_python_ast_ignores_fixture_strings(self) -> None:
        """Audit Python classes and methods without treating source fixtures as code."""
        (self.root / "helpers.py").write_text(
            'class Helper:\n    """A documented helper."""\n'
            '    def method(self):\n        """! @brief Return one."""\n        return 1\n'
            '    async def missing(self):\n        """! @brief"""\n        pass\n'
            'fixture = "class Pretend: pass\\ndef pretend(): pass"\n')
        entities, errors, _ = _MODULE.inventory(self.root)
        self.assertEqual(errors, [])
        self.assertEqual([entity.name for entity in entities], ["Helper", "method", "missing"])
        self.assertEqual([entity.name for entity in entities if not entity.documented], ["missing"])

    def test_doxygen_markers_alone_are_not_documentation(self) -> None:
        """Require prose after commands instead of accepting delimiters or parameter names."""
        for text in ("/** */", "/** @brief */", "/** @param value */",
                     "/** @param[in] value @return */", "/** @copydoc Other::method */",
                     "! @brief", "/// @brief"):
            with self.subTest(text=text):
                self.assertFalse(_MODULE.has_prose(text))
        self.assertTrue(_MODULE.has_prose("/** @brief Return the stored value. */"))
        self.assertTrue(_MODULE.has_prose("/** @param value The value to retain. */"))
        (self.root / "empty.h").write_text("/** @brief */\nvoid missing();\n")
        entities, _, _ = _MODULE.inventory(self.root)
        self.assertFalse(entities[0].documented)


if __name__ == "__main__":
    unittest.main()
