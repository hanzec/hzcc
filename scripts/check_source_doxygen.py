#!/usr/bin/env python3
"""Audit raw C++/Java/Python declarations omitted by Doxygen preprocessing.

Install tree-sitter, tree-sitter-cpp and tree-sitter-java to run this supplemental
check. Unlike the XML check, this parses every branch without preprocessing.
An out-of-line definition is covered only when the current XML explicitly maps
its source location to a documented declaration, or its scoped declarator is
identical to that declaration. Parser errors and local declaration/initializer
ambiguities require a manual review bound to the exact source SHA-256 hash.
Run from the repository root, for example:

    python scripts/check_source_doxygen.py hzcc scripts --xml-dir build/default/doxygen_xml \
        --review-manifest scripts/source_doxygen_reviews.json --report build/source-docs.json

This check inventories explicit source declarations, not compiler-generated
implicit special members, GoogleTest-generated classes, or unnamed lambda call
operators. Forward declarations are covered by their documented type definitions.
"""
from __future__ import annotations

import argparse
import ast
from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
import re
import xml.etree.ElementTree as ET

from tree_sitter import Language, Node, Parser
import tree_sitter_cpp
import tree_sitter_java

TEST_MACROS = {"TEST", "TEST_F", "TEST_P", "TYPED_TEST", "TYPED_TEST_P"}
# This macro expands to a statement block, not a declared callable. Its final
# block argument confuses the C++ grammar into emitting function_definition.
STATEMENT_MACROS = {"LIMIT_REGEX_CHECK"}
CLASS_KINDS = {"class_specifier", "struct_specifier", "union_specifier"}
JAVA_KINDS = {"class_declaration", "interface_declaration", "enum_declaration",
              "record_declaration", "method_declaration", "constructor_declaration"}


@dataclass
class Entity:
    """One explicit source declaration or definition and its documentation evidence."""

    file: str
    line: int
    kind: str
    name: str
    documented: bool
    evidence: str = ""
    signature: str = ""
    header_end: int = 0


def walk(node: Node):
    """Yield every named node, including inactive preprocessor branches."""
    yield node
    for child in node.named_children:
        yield from walk(child)


def source_text(node: Node | None, source: bytes) -> str:
    """Read a syntax node without normalizing source line endings."""
    return source[node.start_byte:node.end_byte].decode() if node else ""


def parser_source(source: bytes) -> bytes:
    """Blank declaration-only attribute macros while preserving every source offset.

    These macros contribute attributes/inline/constexpr, never declarations or
    function bodies. No conditional branch or project function is removed.
    """
    annotations = rb"\b(?:ALWAYS_INLINE|PACKED|NODISCARD|HZCC_MUST_USE_RESULT|HZCC_ATTRIBUTE_LIFETIME_BOUND)\b"
    return b"".join(line if line.lstrip().startswith(b"#") else
                    re.sub(annotations, lambda match: b" " * len(match[0]), line)
                    for line in source.splitlines(keepends=True))


def signature(node: Node, source: bytes) -> str:
    """Build a conservative exact signature for defaulted out-of-line members."""
    scopes = []
    parent = node.parent
    while parent:
        if parent.type in CLASS_KINDS | {"namespace_definition"}:
            scope = source_text(parent.child_by_field_name("name"), source)
            if scope:
                scopes.append(scope)
        parent = parent.parent
    declarator = node.child_by_field_name("declarator")
    text = source_text(declarator, source)
    return re.sub(r"\s+", "", "::".join([*reversed(scopes), text]))


def has_prose(text: str) -> bool:
    """Reject empty documentation containing only delimiters or Doxygen commands.

    Parameter names and copy/reference targets are markup arguments, not prose.
    This is a content-presence check, not a substitute for reviewing accuracy.
    """
    text = re.sub(r"/\*\*[<!]?|/\*![<]?|\*/", "", text)
    text = re.sub(r"(?m)^\s*(?:///<?|//!<?|\*|##<?|#)\s?", "", text)
    named_commands = (r"param|tparam|retval|throws|exception|copydoc|copybrief|copydetails|"
                      r"ref|see|sa|ingroup|relates|relatesalso|class|struct|union|enum|"
                      r"namespace|package|file|dir")
    text = re.sub(r"[@\\](?:" + named_commands + r")(?:\[[^\]]*\])?\s+[^\s@\\]+", "", text)
    text = re.sub(r"[@\\][A-Za-z_]+(?:\[[^\]]*\])?", "", text)
    return any(character.isalnum() for character in text)


def has_comment(node: Node, source: bytes) -> bool:
    """Find an attached Doxygen block through templates, friends and lint comments."""
    while node.parent and node.parent.type in {
        "template_declaration", "field_declaration", "declaration", "type_definition",
        "friend_declaration",
    }:
        node = node.parent
    previous = node.prev_named_sibling
    while previous and previous.type in {"comment", "block_comment", "line_comment"}:
        text = source_text(previous, source).lstrip()
        if text.startswith(("/**", "/*!", "///", "//!")):
            return has_prose(text)
        # Lint annotations do not sever an otherwise adjacent API comment.
        if "NOLINT" not in text:
            break
        previous = previous.prev_named_sibling
    return False


def function_name(node: Node, source: bytes) -> str | None:
    """Read a C++ function declarator without mistaking callback fields for methods."""
    declarator = node.child_by_field_name("declarator")
    while declarator and declarator.type in {"reference_declarator", "pointer_declarator"}:
        declarator = declarator.child_by_field_name("declarator") or next(
            (child for child in declarator.named_children if "declarator" in child.type), None)
    if declarator and declarator.type == "operator_cast":
        return "operator " + source_text(declarator.child_by_field_name("type"), source)
    if not declarator or declarator.type != "function_declarator":
        return None
    name = declarator.child_by_field_name("declarator")
    if not name or name.type in {"parenthesized_declarator", "abstract_parenthesized_declarator"}:
        return None
    return source_text(name, source)


def inventory(root: Path) -> tuple[list[Entity], list[str], dict[str, int]]:
    """Inventory C++/Java syntax and Python AST declarations, never fixture strings."""
    cpp_parser = Parser(Language(tree_sitter_cpp.language()))
    parsers = dict.fromkeys((".h", ".hh", ".hpp", ".hxx", ".c", ".cc", ".cpp", ".cxx"), cpp_parser)
    parsers[".java"] = Parser(Language(tree_sitter_java.language()))
    entities: list[Entity] = []
    errors: list[str] = []
    counts = {"files": 0, "test_macros": 0, "statement_macros": 0, "local_declarations": 0}
    for path in sorted(root.rglob("*")):
        if path.suffix == ".py":
            counts["files"] += 1
            try:
                tree = ast.parse(path.read_bytes(), filename=str(path))
            except SyntaxError as error:
                errors.append(f"{path}:{error.lineno}: Python syntax error: {error.msg}")
                continue
            for node in ast.walk(tree):
                if isinstance(node, (ast.ClassDef, ast.FunctionDef, ast.AsyncFunctionDef)):
                    documented = has_prose(ast.get_docstring(node) or "")
                    entities.append(Entity(str(path.resolve()), node.lineno,
                                           "python_class" if isinstance(node, ast.ClassDef) else "python_function",
                                           node.name, documented,
                                           "Python docstring" if documented else "",
                                           header_end=node.lineno))
            continue
        if path.suffix not in parsers:
            continue
        counts["files"] += 1
        source = path.read_bytes()
        for node in walk(parsers[path.suffix].parse(parser_source(source)).root_node):
            if node.type == "ERROR" or node.is_missing:
                excerpt = source_text(node, source).replace("\n", " ")[:120]
                errors.append(f"{path}:{node.start_point.row + 1}: {node.type}: {excerpt}")
            name = None
            if node.type in CLASS_KINDS and node.child_by_field_name("body"):
                name = source_text(node.child_by_field_name("name"), source) or "(anonymous)"
            elif path.suffix == ".java" and node.type in JAVA_KINDS:
                name = source_text(node.child_by_field_name("name"), source)
            elif node.type in {"function_definition", "declaration", "field_declaration"}:
                name = function_name(node, source)
                if name in TEST_MACROS:
                    counts["test_macros"] += 1
                    continue
                if name in STATEMENT_MACROS:
                    counts["statement_macros"] += 1
                    continue
                if name and node.type != "function_definition" and node.parent.type == "compound_statement":
                    counts["local_declarations"] += 1
                    excerpt = source_text(node, source).replace("\n", " ")[:120]
                    errors.append(f"{path}:{node.start_point.row + 1}: local declaration/initializer: {excerpt}")
                    continue
            if name:
                documented = has_comment(node, source)
                body = node.child_by_field_name("body")
                entities.append(Entity(str(path.resolve()), node.start_point.row + 1,
                                       node.type, name, documented,
                                       "attached Doxygen comment" if documented else "",
                                       signature(node, source),
                                       (body.start_point.row if body else node.end_point.row) + 1))
    return entities, errors, counts


def mapped_definitions(xml_dir: Path, entities: list[Entity]) -> None:
    """Cover definitions only through XML links to directly documented declarations."""
    by_location = {(entity.file, line): entity for entity in entities
                   for line in range(entity.line, entity.header_end + 1)}
    index = ET.parse(xml_dir / "index.xml").getroot()
    for reference in index.findall("compound"):
        refid = reference.get("refid", "")
        if not refid or "/" in refid or "\\" in refid or refid in {".", ".."}:
            raise ValueError(f"Invalid Doxygen compound ID: {refid!r}")
        compound = ET.parse(xml_dir / f"{refid}.xml").getroot()
        for member in compound.findall(".//memberdef"):
            if member.get("kind") not in {"function", "friend"}:
                continue
            location = member.find("location")
            if location is None or "bodystart" not in location.attrib:
                continue
            declaration = by_location.get((str(Path(location.get("declfile", location.get("file", ""))).resolve()),
                                           int(location.get("declline", location.get("line", "0")))))
            definition = by_location.get((str(Path(location.get("bodyfile", "")).resolve()),
                                          int(location.get("bodystart", "0"))))
            if (declaration and declaration.evidence == "attached Doxygen comment"
                    and definition and not definition.documented):
                definition.documented = True
                definition.evidence = f"documented declaration {declaration.file}:{declaration.line}"
    # Doxygen omits body locations for defaulted/destructed member definitions.
    # Require an identical fully scoped declarator, not merely a shared name.
    direct_signatures = {entity.signature: entity for entity in entities
                         if entity.evidence == "attached Doxygen comment"}
    for entity in entities:
        declaration = direct_signatures.get(entity.signature)
        if not entity.documented and entity.kind == "function_definition" and declaration:
            entity.documented = True
            entity.evidence = f"exact declared signature {declaration.file}:{declaration.line}"


def main() -> int:
    """Print source coverage and retain the full inventory for independent review."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source_roots", type=Path, nargs="+")
    parser.add_argument("--xml-dir", type=Path)
    parser.add_argument("--report", type=Path)
    parser.add_argument("--review-manifest", type=Path,
                        help="Manual parser-diagnostic reviews with exact source SHA-256 hashes")
    args = parser.parse_args()
    entities, errors, counts = [], [], {}
    for root in args.source_roots:
        if not root.is_dir():
            parser.error(f"Source directory not found: {root}")
        root_entities, root_errors, root_counts = inventory(root)
        entities.extend(root_entities)
        errors.extend(root_errors)
        for name, count in root_counts.items():
            counts[name] = counts.get(name, 0) + count
    if not entities:
        parser.error("No explicit classes or functions found in the source directories")
    if args.xml_dir:
        mapped_definitions(args.xml_dir, entities)
    missing = [entity for entity in entities if not entity.documented]
    reviews = json.loads(args.review_manifest.read_text()) if args.review_manifest else {}
    unreviewed_errors = []
    for error in errors:
        filename = error.split(":", 1)[0]
        review = reviews.get(filename, {})
        digest = hashlib.sha256(Path(filename).read_bytes()).hexdigest()
        if review.get("sha256") != digest or not review.get("reason"):
            unreviewed_errors.append(error)
    for entity in missing:
        print(f"{entity.file}:{entity.line}: missing source Doxygen comment: {entity.name}")
    print(f"Source coverage: {len(entities) - len(missing)}/{len(entities)} explicit declarations/definitions")
    print(f"Scanned {counts['files']} C++/Java/Python files; excluded {counts['test_macros']} GoogleTest macro invocations "
          f"and {counts['statement_macros']} statement macros; "
          f"{counts['local_declarations']} local declaration/initializer ambiguities checked against manual reviews.")
    print(f"Source review findings: {len(errors) - len(unreviewed_errors)} manually reviewed, "
          f"{len(unreviewed_errors)} requiring review")
    for error in unreviewed_errors:
        print(error)
    if args.report:
        args.report.write_text(json.dumps({"counts": counts, "entities": [vars(e) for e in entities],
                                          "parser_diagnostics": errors,
                                          "unreviewed_diagnostics": unreviewed_errors}, indent=2) + "\n")
    return int(bool(missing or unreviewed_errors))


if __name__ == "__main__":
    raise SystemExit(main())
