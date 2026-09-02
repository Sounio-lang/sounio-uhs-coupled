#!/usr/bin/env python3
"""Fetch the USGS Aux Vases microbial-reaction model archive.

Source of truth for the file list is the ScienceBase item itself, so the set
downloaded is whatever the archive actually holds on the retrieval date, not a
list transcribed by hand.

Item : 698b487ab66b011dc17eb873
DOI  : 10.5066/P13GJC6Y
Right: CC0-1.0 (public domain dedication)

Writes files under data/usgs-auxvases/ and prints one TSV row per file:
    relpath <TAB> bytes <TAB> sha256
"""
import hashlib
import json
import os
import sys
import urllib.request

ITEM = "698b487ab66b011dc17eb873"
API = f"https://www.sciencebase.gov/catalog/item/{ITEM}?format=json"
OUT = os.path.join("data", "usgs-auxvases")


def get(url, timeout=180):
    req = urllib.request.Request(url, headers={"User-Agent": "sounio-uhs-coupled/0.1"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        return r.read()


def main():
    os.makedirs(OUT, exist_ok=True)
    meta = json.loads(get(API).decode("utf-8"))
    files = meta.get("files") or []
    if not files:
        print("FAIL: item reports no files", file=sys.stderr)
        return 1

    failures = []
    for f in files:
        name, url = f.get("name"), f.get("url")
        declared = f.get("size")
        if not name or not url:
            failures.append(f"malformed file entry: {f!r}")
            continue
        dest = os.path.join(OUT, name)
        try:
            blob = get(url)
        except Exception as e:  # report, never substitute
            failures.append(f"{name}: download failed: {e}")
            continue
        # A size mismatch means the archive moved under us; fail closed on it
        # rather than recording a hash for content we did not expect.
        if declared is not None and len(blob) != declared:
            failures.append(
                f"{name}: size mismatch, declared {declared} got {len(blob)}")
            continue
        with open(dest, "wb") as fh:
            fh.write(blob)
        print(f"{dest}\t{len(blob)}\t{hashlib.sha256(blob).hexdigest()}")

    for msg in failures:
        print(f"FAIL\t{msg}", file=sys.stderr)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
