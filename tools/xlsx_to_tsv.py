#!/usr/bin/env python3
"""Extract a sheet of an .xlsx workbook to TSV using only the standard library.

Marshalling only: it moves published values into a readable form and computes
nothing. Written rather than pulled in as a dependency so the extraction path is
auditable and reproducible with no environment setup.

Handles what this workbook uses: shared strings, inline strings, numbers,
booleans, and blank cells. It deliberately does NOT evaluate formulas — a cell
carrying a formula is emitted as its cached value if one is stored, and as
FORMULA_NO_CACHED_VALUE otherwise, so a missing value can never be mistaken for
an empty one.

Usage:
    python3 tools/xlsx_to_tsv.py BOOK.xlsx --list
    python3 tools/xlsx_to_tsv.py BOOK.xlsx --sheet "all info" > out.tsv
"""
import argparse
import re
import sys
import xml.etree.ElementTree as ET
import zipfile

NS = "{http://schemas.openxmlformats.org/spreadsheetml/2006/main}"
RNS = "{http://schemas.openxmlformats.org/officeDocument/2006/relationships}"

# The workbook is downloaded from the internet, so it is untrusted input.
# defusedxml is not available in this environment, so entity-bearing documents
# are refused outright rather than parsed: a spreadsheet part has no legitimate
# reason to carry a DTD or an entity declaration, and refusing is cheap.
_DANGEROUS = re.compile(rb"<!DOCTYPE|<!ENTITY", re.IGNORECASE)
# Guard against a zip bomb in the archive members we expand.
MAX_PART_BYTES = 256 << 20


def safe_xml(z, name):
    info = z.getinfo(name)
    if info.file_size > MAX_PART_BYTES:
        sys.exit(f"FAIL: {name} expands to {info.file_size} bytes; refusing")
    blob = z.read(name)
    if _DANGEROUS.search(blob):
        sys.exit(f"FAIL: {name} contains a DTD or entity declaration; refusing")
    return ET.fromstring(blob)


def col_index(ref):
    """'BC12' -> 54 (0-based column)."""
    letters = re.match(r"[A-Z]+", ref).group(0)
    n = 0
    for ch in letters:
        n = n * 26 + (ord(ch) - 64)
    return n - 1


def shared_strings(z):
    if "xl/sharedStrings.xml" not in z.namelist():
        return []
    out = []
    for si in safe_xml(z, "xl/sharedStrings.xml"):
        out.append("".join(t.text or "" for t in si.iter(NS + "t")))
    return out


def sheet_map(z):
    """sheet name -> archive path, resolved through the workbook rels."""
    rels = {}
    for r in safe_xml(z, "xl/_rels/workbook.xml.rels"):
        rels[r.get("Id")] = r.get("Target")
    out = {}
    for sh in safe_xml(z, "xl/workbook.xml").iter(NS + "sheet"):
        target = rels.get(sh.get(RNS + "id"), "")
        target = target[1:] if target.startswith("/") else "xl/" + target.lstrip("/")
        out[sh.get("name")] = target.replace("xl/xl/", "xl/")
    return out


def cell_text(c, strings):
    t = c.get("t")
    if t == "inlineStr":
        return "".join(x.text or "" for x in c.iter(NS + "t"))
    v = c.find(NS + "v")
    has_formula = c.find(NS + "f") is not None
    if v is None or v.text is None:
        return "FORMULA_NO_CACHED_VALUE" if has_formula else ""
    if t == "s":
        i = int(v.text)
        if i >= len(strings):
            return "SHARED_STRING_OUT_OF_RANGE"
        return strings[i]
    if t == "b":
        return "TRUE" if v.text == "1" else "FALSE"
    if t == "e":
        return "ERROR:" + v.text
    return v.text


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("book")
    ap.add_argument("--sheet")
    ap.add_argument("--list", action="store_true")
    a = ap.parse_args()

    z = zipfile.ZipFile(a.book)
    sheets = sheet_map(z)
    if a.list or not a.sheet:
        for name, path in sheets.items():
            print(f"{name}\t{path}")
        return 0
    if a.sheet not in sheets:
        sys.exit(f"FAIL: no sheet named {a.sheet!r}; have {list(sheets)}")

    strings = shared_strings(z)
    root = safe_xml(z, sheets[a.sheet])
    for row in root.iter(NS + "row"):
        cells = {}
        for c in row.iter(NS + "c"):
            ref = c.get("r")
            if not ref:
                continue
            cells[col_index(ref)] = cell_text(c, strings).replace("\t", " ").replace("\n", " ")
        if not cells:
            continue
        width = max(cells) + 1
        print("\t".join(cells.get(i, "") for i in range(width)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
