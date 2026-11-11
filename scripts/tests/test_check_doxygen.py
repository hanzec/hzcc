"""Exercise coverage-gate failures without requiring a Doxygen installation."""
from __future__ import annotations

import importlib.util
from pathlib import Path
import tempfile
import unittest

_SPEC = importlib.util.spec_from_file_location(
    "check_doxygen", Path(__file__).resolve().parents[1] / "check_doxygen.py"
)
_MODULE = importlib.util.module_from_spec(_SPEC)
_SPEC.loader.exec_module(_MODULE)


class DoxygenAuditTest(unittest.TestCase):
    """Check current-index selection and reject absent or incomplete extraction."""

    def setUp(self) -> None:
        """Give each case a separate disposable XML output directory."""
        self._temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self._temporary.cleanup)
        self.xml_dir = Path(self._temporary.name)

    def write_index(self, *refids: str) -> None:
        """Describe the compound files produced by the simulated current run."""
        entries = "".join(f'<compound refid="{refid}"/>' for refid in refids)
        (self.xml_dir / "index.xml").write_text(f"<doxygenindex>{entries}</doxygenindex>")

    def write_compound(self, refid: str, members: str = "", documented: bool = True) -> None:
        """Write a class with optional member XML and a real prose description."""
        description = "<para>Owns the test resource.</para>" if documented else ""
        (self.xml_dir / f"{refid}.xml").write_text(
            f'<doxygen><compounddef id="{refid}" kind="class">'
            f"<compoundname>Example</compoundname><briefdescription>{description}</briefdescription>"
            f"<sectiondef>{members}</sectiondef></compounddef></doxygen>"
        )

    def test_documented_current_entities_ignore_stale_xml(self) -> None:
        """Old, even malformed, unindexed files must not affect current coverage."""
        self.write_index("current")
        self.write_compound("current")
        (self.xml_dir / "stale.xml").write_text("not XML from the current extraction")
        self.assertEqual(_MODULE.audit(self.xml_dir), (1, []))

    def test_missing_description_is_reported(self) -> None:
        """An extracted class without prose must fail the documentation gate."""
        self.write_index("current")
        self.write_compound("current", documented=False)
        total, missing = _MODULE.audit(self.xml_dir)
        self.assertEqual(total, 1)
        self.assertEqual(len(missing), 1)
        self.assertIn("Example", missing[0])

    def test_empty_extraction_is_not_success(self) -> None:
        """A successful-looking empty index must never produce a 0/0 pass."""
        self.write_index()
        with self.assertRaisesRegex(ValueError, "No classes or functions"):
            _MODULE.audit(self.xml_dir)

    def test_missing_indexed_file_is_rejected(self) -> None:
        """A partial extraction cannot borrow unrelated old files to pass."""
        self.write_index("missing")
        self.write_compound("old")
        with self.assertRaisesRegex(ValueError, "compound is missing"):
            _MODULE.audit(self.xml_dir)

    def test_mismatched_compound_id_is_rejected(self) -> None:
        """The index and referenced compound must describe the same entity."""
        self.write_index("current")
        self.write_compound("other")
        (self.xml_dir / "other.xml").rename(self.xml_dir / "current.xml")
        with self.assertRaisesRegex(ValueError, "ID does not match"):
            _MODULE.audit(self.xml_dir)

    def test_friend_function_is_audited(self) -> None:
        """Friend functions require comments while friend classes do not create methods."""
        self.write_index("current")
        self.write_compound(
            "current",
            '<memberdef id="operator" kind="friend"><name>operator==</name>'
            '<argsstring>(const Example &amp; rhs)</argsstring></memberdef>'
            '<memberdef id="friend_class" kind="friend"><name>OtherClass</name></memberdef>',
        )
        total, missing = _MODULE.audit(self.xml_dir)
        self.assertEqual(total, 2)
        self.assertEqual(len(missing), 1)
        self.assertIn("operator==", missing[0])

    def test_shared_function_ids_are_deduplicated(self) -> None:
        """File and namespace XML can describe the same documented function."""
        self.write_index("one", "two")
        member = '<memberdef id="shared" kind="function"><name>run</name>{}</memberdef>'
        self.write_compound("one", member.format(""))
        self.write_compound(
            "two", member.format("<briefdescription><para>Run the pass.</para></briefdescription>")
        )
        self.assertEqual(_MODULE.audit(self.xml_dir), (3, []))


if __name__ == "__main__":
    unittest.main()
