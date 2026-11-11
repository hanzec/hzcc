#!/usr/bin/env python3
"""Fail when Doxygen XML contains an undocumented project class or function."""
from __future__ import annotations

import argparse
from pathlib import Path
import xml.etree.ElementTree as ET


def has_description(element: ET.Element) -> bool:
    """Return whether a brief or detailed description contains rendered text."""
    return any(
        "".join(description.itertext()).strip()
        for tag in ("briefdescription", "detaileddescription")
        for description in element.findall(tag)
    )


def audit(xml_dir: Path) -> tuple[int, list[str]]:
    """Audit entities referenced by the current index, rejecting incomplete XML.

    Doxygen can leave older compound files behind between runs. Only compounds
    referenced by index.xml belong to this extraction. This checks extracted
    declarations; it cannot account for source excluded by Doxygen preprocessing.
    """
    index_path = xml_dir / "index.xml"
    if not index_path.is_file():
        raise ValueError(f"Doxygen XML index not found: {index_path}")
    index = ET.parse(index_path).getroot()
    if index.tag != "doxygenindex":
        raise ValueError(f"Not a Doxygen XML index: {index_path}")
    records: dict[str, tuple[bool, str]] = {}
    missing: list[str] = []
    for reference in index.findall("compound"):
        refid = reference.get("refid", "")
        if not refid or refid in {".", ".."} or "/" in refid or "\\" in refid:
            raise ValueError(f"Invalid compound reference in {index_path}: {refid!r}")
        path = xml_dir / f"{refid}.xml"
        if not path.is_file():
            raise ValueError(f"Indexed Doxygen compound is missing: {path}")
        root = ET.parse(path).getroot()
        compounds = root.findall("compounddef")
        if len(compounds) != 1 or compounds[0].get("id") != refid:
            raise ValueError(f"Indexed Doxygen compound ID does not match: {path}")
        for compound in compounds:
            candidates = []
            if compound.get("kind") in {"class", "struct", "union", "interface"}:
                candidates.append((compound, compound.findtext("compoundname", "")))
            for member in compound.findall("sectiondef/memberdef"):
                kind = member.get("kind")
                # Inline friend functions have a distinct XML kind; friend class
                # declarations have no argument list and are not functions.
                is_friend_function = kind == "friend" and bool(member.findtext("argsstring"))
                if kind != "function" and not is_friend_function:
                    continue
                name = member.findtext("name", "")
                # GoogleTest generates its own classes from these invocation macros.
                if name in {"TEST", "TEST_F", "TEST_P", "TYPED_TEST", "TYPED_TEST_P"}:
                    continue
                candidates.append((member, member.findtext("qualifiedname") or name))
            for element, name in candidates:
                identity = element.get("id", "")
                if not identity:
                    raise ValueError(f"Extracted entity has no XML ID in {path}: {name}")
                location = element.find("location")
                filename = location.get("file", str(path)) if location is not None else str(path)
                line = location.get("line", "?") if location is not None else "?"
                message = f"{filename}:{line}: missing Doxygen documentation: {name}"
                documented = has_description(element)
                if identity in records:
                    documented = documented or records[identity][0]
                records[identity] = documented, message
    if not records:
        raise ValueError(f"No classes or functions were extracted in {index_path}")
    missing.extend(message for documented, message in records.values() if not documented)
    return len(records), missing


def main() -> int:
    """Check the XML directory supplied by CMake or a standalone Doxygen run."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("xml_dir", type=Path)
    args = parser.parse_args()
    try:
        total, missing = audit(args.xml_dir)
    except (ValueError, ET.ParseError, OSError) as error:
        parser.error(str(error))
    for message in missing:
        print(message)
    print(f"Doxygen coverage: {total - len(missing)}/{total} extracted classes and functions documented")
    return int(bool(missing))


if __name__ == "__main__":
    raise SystemExit(main())
