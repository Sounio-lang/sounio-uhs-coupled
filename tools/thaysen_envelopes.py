#!/usr/bin/env python3
"""Empirical growth envelopes per metabolic group, computed from Thaysen et al.

Marshalling and descriptive statistics over published values only. Nothing is
imputed: a strain that does not report a field is counted as missing and named
in the coverage column, never filled in.

The envelope of a group is the union of its strains' reported tolerance ranges —
the widest low and the widest high actually observed — which is the quantity the
ontology's subsumption-derived envelope has to reproduce in test O1.

Usage:  python3 tools/thaysen_envelopes.py ALLINFO.tsv
"""
import sys
from collections import defaultdict

FIELDS = [
    ("Temp", "Temp_LOW", "Temp _OPT", "Temp_UP", "degC (UNDECLARED in source)"),
    ("Salt", "Salt_LOW (g/L)", "Salt_OPT (g/L)", "Salt_UP (g/L)", "g/L"),
    ("pH", "pH_LOW", "pH_OPT", "pH_UP", "unitless"),
]
PRESSURE = ("Pressure-OPT (Mpa)", "Pressure-tolerance (Mpa)")


def num(s):
    """Parse a cell to float, or None. Refuses anything ambiguous rather than
    guessing: ranges, inequalities and free text are missing, not values."""
    s = (s or "").strip().replace(",", ".")
    if not s:
        return None
    try:
        return float(s)
    except ValueError:
        return None


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "/tmp/allinfo.tsv"
    rows = [l.rstrip("\n").split("\t") for l in open(path, encoding="utf-8")]
    hdr = [h.strip() for h in rows[0]]
    idx = {h: i for i, h in enumerate(hdr)}

    for _, lo, opt, up, _u in FIELDS:
        for col in (lo, opt, up):
            if col not in idx:
                sys.exit(f"FAIL: column {col!r} not in header {hdr}")

    def cell(r, col):
        i = idx[col]
        return r[i] if i < len(r) else ""

    groups = defaultdict(list)
    for r in rows[1:]:
        if not any(c.strip() for c in r):
            continue
        if not (r and r[0].strip()):
            continue
        groups[cell(r, "microbe").strip()].append(r)

    print(f"# Thaysen et al. 2021, DOI 10.17632/4dksb2x4zn.1, sheet 'all info'")
    print(f"# strains: {sum(len(v) for v in groups.values())}")
    print()
    print("group\tfield\tunits\tn_reported\tn_missing\tenv_low\tenv_high\topt_min\topt_max")

    for g in sorted(groups):
        rs = groups[g]
        for name, lo, opt, up, units in FIELDS:
            los = [num(cell(r, lo)) for r in rs]
            ups = [num(cell(r, up)) for r in rs]
            opts = [num(cell(r, opt)) for r in rs]
            lo_v = [v for v in los if v is not None]
            up_v = [v for v in ups if v is not None]
            op_v = [v for v in opts if v is not None]
            # a strain counts as reported for the envelope if it gave either bound
            reported = sum(1 for a, b in zip(los, ups) if a is not None or b is not None)
            print("\t".join([
                g, name, units, str(reported), str(len(rs) - reported),
                f"{min(lo_v):g}" if lo_v else "NONE",
                f"{max(up_v):g}" if up_v else "NONE",
                f"{min(op_v):g}" if op_v else "NONE",
                f"{max(op_v):g}" if op_v else "NONE",
            ]))
        for col in PRESSURE:
            if col not in idx:
                continue
            vals = [num(cell(r, col)) for r in rs]
            v = [x for x in vals if x is not None]
            print("\t".join([
                g, col.replace(" ", "_"), "MPa", str(len(v)), str(len(rs) - len(v)),
                f"{min(v):g}" if v else "NONE", f"{max(v):g}" if v else "NONE",
                "", "",
            ]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
