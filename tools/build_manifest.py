#!/usr/bin/env python3
"""Regenerate data/MANIFEST.tsv from what is actually on disk.

Hashes are computed from the files themselves, never transcribed, so the
manifest cannot drift from the tree it describes. Per-source metadata (DOI,
licence, retrieval date, convention axioms) lives in data/SOURCES.tsv, which is
authored by hand; this tool joins the two and fails closed if a file on disk has
no source entry, or a source entry names a directory that does not exist.

Usage:  python3 tools/build_manifest.py [--check]
        --check verifies the committed manifest still matches disk (exit 1 on
        drift) without rewriting it.
"""
import hashlib
import os
import sys

DATA = "data"
SOURCES = os.path.join(DATA, "SOURCES.tsv")
MANIFEST = os.path.join(DATA, "MANIFEST.tsv")
COLUMNS = ["path", "bytes", "sha256", "source_id", "doi", "licence",
           "retrieved", "convention_axioms"]


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def read_sources():
    if not os.path.exists(SOURCES):
        sys.exit(f"FAIL: {SOURCES} is missing; no file may enter without a source")
    rows, seen = {}, set()
    with open(SOURCES, encoding="utf-8") as fh:
        header = fh.readline().rstrip("\n").split("\t")
        need = ["source_id", "subdir", "doi", "licence", "retrieved",
                "convention_axioms"]
        if header != need:
            sys.exit(f"FAIL: {SOURCES} header is {header}, expected {need}")
        for n, line in enumerate(fh, start=2):
            line = line.rstrip("\n")
            if not line or line.startswith("#"):
                continue
            f = line.split("\t")
            if len(f) != len(need):
                sys.exit(f"FAIL: {SOURCES}:{n}: {len(f)} fields, expected {len(need)}")
            sid, subdir = f[0], f[1]
            if sid in seen:
                sys.exit(f"FAIL: {SOURCES}:{n}: duplicate source_id {sid}")
            seen.add(sid)
            rows[subdir] = dict(zip(need, f))
    return rows


def main():
    check = "--check" in sys.argv
    sources = read_sources()

    for subdir in sources:
        d = os.path.join(DATA, subdir)
        if not os.path.isdir(d):
            sys.exit(f"FAIL: source names {d}, which does not exist")

    out, orphans = [], []
    for root, dirs, files in os.walk(DATA):
        dirs[:] = sorted(dirs)
        for name in sorted(files):
            path = os.path.join(root, name)
            rel = os.path.relpath(path, DATA)
            if rel in ("MANIFEST.tsv", "SOURCES.tsv", "README.md"):
                continue
            subdir = rel.split(os.sep)[0]
            src = sources.get(subdir)
            if src is None:
                orphans.append(path)
                continue
            out.append([os.path.join(DATA, rel), str(os.path.getsize(path)),
                        sha256(path), src["source_id"], src["doi"],
                        src["licence"], src["retrieved"],
                        src["convention_axioms"]])

    if orphans:
        for p in orphans:
            print(f"FAIL\tno source entry for {p}", file=sys.stderr)
        sys.exit(1)

    body = "\t".join(COLUMNS) + "\n" + "".join("\t".join(r) + "\n" for r in out)

    if check:
        if not os.path.exists(MANIFEST):
            sys.exit(f"FAIL: {MANIFEST} missing")
        if open(MANIFEST, encoding="utf-8").read() != body:
            sys.exit(f"FAIL: {MANIFEST} does not match disk")
        print(f"OK: {len(out)} files match the manifest")
        return
    with open(MANIFEST, "w", encoding="utf-8") as fh:
        fh.write(body)
    print(f"wrote {MANIFEST}: {len(out)} files")


if __name__ == "__main__":
    main()
