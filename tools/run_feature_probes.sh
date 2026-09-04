#!/bin/bash
# Executable probes for the measured claims in FEATURES.md, CORRECTIONS.md and
# LANGUAGE_GAPS.md.
#
# Why this exists: CORRECTIONS.md C21. A claim measured under one compiler was
# carried across a re-pin without being re-run, and nothing caught it, because
# nothing in this repository tied a measured claim to the compiler that measured
# it. This script is that tie. It refuses to run unless the compiler you hand it
# is the one every captured output says produced them.
#
#   usage: bash tools/run_feature_probes.sh [path-to-gen3.elf]
#
# Exit 0 only if the pin matches AND every probe reproduces its documented
# outcome. Any drift is a hard failure: re-measure and correct the prose, or
# re-pin -- but do not carry the number forward.
set -u
cd "$(dirname "$0")/.."

PINS=$(grep -h "^compiler: gen3.elf, md5 " sio/*.output.txt | sed 's/.*md5 //' | sort -u)
if [ "$(printf '%s\n' "$PINS" | wc -l)" -ne 1 ]; then
  echo "FAIL: the captured outputs disagree about which compiler produced them:"
  printf '  %s\n' $PINS
  exit 2
fi
PIN=$PINS

COMPILER=${1:-}
if [ -z "$COMPILER" ]; then
  COMPILER=$(grep -h -m1 "^  cd sio && " sio/coupled_gasphase.output.txt | awk '{print $4}')
fi
if [ ! -x "$COMPILER" ]; then
  echo "FAIL: compiler not found or not executable: $COMPILER"
  echo "      pass the path as the first argument."
  exit 2
fi
ACTUAL=$(md5sum "$COMPILER" | cut -d" " -f1)
echo "pin (from sio/*.output.txt): $PIN"
echo "compiler under test:         $ACTUAL  $COMPILER"
if [ "$PIN" != "$ACTUAL" ]; then
  echo
  echo "FAIL: this is not the compiler the captured outputs were produced with."
  echo "      Every measured claim here is pinned to $PIN."
  echo "      Re-run the capture and re-pin, or point this script at that build."
  echo "      Carrying the claims forward unmeasured is exactly C21."
  exit 2
fi
echo

T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
pass=0; fail=0

for p in probes/*.sio; do
  name=$(basename "$p" .sio)
  want_rc=$(grep -m1 "^//@ expect-rc:" "$p" | sed "s|^//@ expect-rc: *||")
  want_n=$(grep -m1 "^//@ expect-diagnostic-count:" "$p" | sed "s|^//@ expect-diagnostic-count: *||")

  "$COMPILER" "$p" "$T/out.elf" > "$T/log.txt" 2>&1
  got_rc=$?
  got_n=$(grep -cE "^(error|warning)" "$T/log.txt")

  why=""
  if [ -n "$want_rc" ] && [ "$got_rc" != "$want_rc" ]; then
    why="rc=$got_rc, expected $want_rc"
  fi
  if [ -z "$why" ] && [ -n "$want_n" ] && [ "$got_n" != "$want_n" ]; then
    why="$got_n diagnostics, expected $want_n"
  fi
  if [ -z "$why" ]; then
    while IFS= read -r d; do
      [ -z "$d" ] && continue
      if ! grep -qF "$d" "$T/log.txt"; then why="missing diagnostic: $d"; break; fi
    done <<< "$(grep "^//@ expect-diagnostic:" "$p" | sed "s|^//@ expect-diagnostic: *||")"
  fi

  if [ -z "$why" ]; then
    printf "  PASS  %s\n" "$name"; pass=$((pass+1))
  else
    printf "  FAIL  %s -- %s\n" "$name" "$why"; fail=$((fail+1))
    grep -E "^(error|warning)" "$T/log.txt" | head -4 | sed "s/^/          /"
    grep -m3 "^//@ claim:" "$p" | sed "s|^//@ claim: *|          claim: |"
  fi
done

echo
echo "  probes: $pass passed, $fail failed"
[ "$fail" -eq 0 ] || exit 1
