#!/usr/bin/env python3
"""Internal-consistency check of the Thaysen strain table.

The rule needs no outside knowledge and imputes nothing: a strain's reported
optimum must lie within that same strain's reported lower and upper critical
values. A row that violates its own bounds is a defect in the source, and this
names it so it can be excluded explicitly rather than silently widening a group
envelope.

Excluded values are reported, never repaired. Where a value is needed, it must
come from the primary reference the row itself cites.

Exit 1 if any violation is found, so the check can gate a pipeline.

Usage:  python3 tools/thaysen_consistency.py ALLINFO.tsv
"""
import sys

TRIPLES = [
    ("Temp", "Temp_LOW", "Temp _OPT", "Temp_UP"),
    ("Salt", "Salt_LOW (g/L)", "Salt_OPT (g/L)", "Salt_UP (g/L)"),
    ("pH", "pH_LOW", "pH_OPT", "pH_UP"),
]


def num(s):
    """Strictly numeric, else None. A range like '40-45' is NOT a number and is
    deliberately not parsed into one."""
    try:
        return float((s or "").strip().replace(",", "."))
    except ValueError:
        return None


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "/tmp/allinfo.tsv"
    rows = [l.rstrip("\n").split("\t") for l in open(path, encoding="utf-8")]
    hdr = [h.strip() for h in rows[0]]
    idx = {h: i for i, h in enumerate(hdr)}
    for _, lo, opt, up in TRIPLES:
        for c in (lo, opt, up):
            if c not in idx:
                sys.exit(f"FAIL: column {c!r} missing; header is {hdr}")

    def cell(r, c):
        i = idx[c]
        return (r[i] if i < len(r) else "").strip()

    violations = []
    nonnumeric = 0
    checked = 0
    for r in rows[1:]:
        if not (r and r[0].strip()):
            continue
        for field, lo, opt, up in TRIPLES:
            o = num(cell(r, opt))
            if o is None:
                if cell(r, opt):
                    nonnumeric += 1
                continue
            l, u = num(cell(r, lo)), num(cell(r, up))
            if l is None and u is None:
                continue
            checked += 1
            if (l is not None and o < l) or (u is not None and o > u):
                violations.append((cell(r, "microbe"), r[0].strip(), field,
                                   cell(r, lo), cell(r, opt), cell(r, up),
                                   cell(r, "References")))

    print(f"checked {checked} optimum-within-bounds assertions")
    print(f"{nonnumeric} optimum cells are non-numeric (e.g. a range) and were not checked")
    print(f"violations: {len(violations)}\n")
    for grp, name, field, lo, opt, up, ref in violations:
        print(f"  {grp:5s} {name:34s} {field:5s} "
              f"low={lo!r} OPT={opt!r} up={up!r}   ref: {ref}")
    return 1 if violations else 0


if __name__ == "__main__":
    sys.exit(main())
