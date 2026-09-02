#!/usr/bin/env python3
"""Total-pressure loss at the end of each mesocosm cycle, abiotic vs biotic.

Reads the normalised pressure series p_t/p_0 from Source Data Fig. 5 of
Hellerschmied et al. 2024 (CC BY 4.0) and reports, per cycle, the last value on
record and the implied loss. Descriptive only: it restates published values, and
computes nothing beyond 1 - p_t/p_0 and a min/max across cycles.

This is TOTAL pressure, not speciated H2. It bounds total abiotic gas loss; it
does not isolate calcite dissolution.

Usage:  python3 tools/abiotic_check.py PABIO.tsv PBIO.tsv
"""
import sys


def load(path):
    rows = [l.rstrip("\n").split("\t") for l in open(path, encoding="utf-8")]
    body = rows[1:]
    series = {}
    for r in body:
        if not r or not r[0].strip():
            continue
        try:
            day = float(r[0])
        except ValueError:
            continue
        for c in range(1, len(r)):
            v = r[c].strip()
            if not v:
                continue
            try:
                series.setdefault(c, []).append((day, float(v)))
            except ValueError:
                continue
    return series


def summarise(label, path):
    s = load(path)
    print(f"=== {label}: {len(s)} cycles ===")
    finals = []
    for c in sorted(s):
        pts = s[c]
        last_day, last_v = pts[-1]
        finals.append(last_v)
        print(f"  cycle {c:2d}  n={len(pts):3d}  last day={last_day:6.3f}  "
              f"p_t/p_0={last_v:.4f}  loss={100*(1-last_v):5.2f}%")
    if finals:
        lo, hi = min(finals), max(finals)
        print(f"  -> loss range {100*(1-hi):.2f}% .. {100*(1-lo):.2f}%   "
              f"mean p_t/p_0 = {sum(finals)/len(finals):.4f}")
    print()
    return finals


def main():
    a = summarise("ABIOTIC", sys.argv[1])
    b = summarise("BIOTIC", sys.argv[2])
    if a and b:
        print(f"abiotic mean loss {100*(1-sum(a)/len(a)):.2f}%  vs  "
              f"biotic mean loss {100*(1-sum(b)/len(b)):.2f}%")
    return 0


if __name__ == "__main__":
    sys.exit(main())
