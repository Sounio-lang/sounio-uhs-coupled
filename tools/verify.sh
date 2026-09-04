#!/bin/bash
# Verify this study reproduces: the pin, the captured numbers, and the measured
# claims — in that order, under one exit code.
#
#   usage: bash tools/verify.sh [path-to-gen3.elf]
#
# Until this existed there was no place in the repository that revalidated
# anything. The byte-identical check lived in a scratch harness outside version
# control, and the probes for the measured claims lived in /tmp; CORRECTIONS.md
# C21 is what that costs. Both now run from here.
#
# Exit 0 only if all three phases hold:
#   2  the compiler is not the one the captured outputs name
#   1  a number or a documented outcome no longer reproduces
#   0  the pin matches, every body is byte-identical, every probe reproduces
set -u
cd "$(dirname "$0")/.."
ROOT=$(pwd)

mkdir /tmp/uhs_verify.lock 2>/dev/null || {
  echo "FAIL: another verify.sh is running. Two runs share temp paths and will"
  echo "      race into results that look like regressions and are not."
  exit 2
}
trap 'rmdir /tmp/uhs_verify.lock 2>/dev/null; rm -rf "${T:-}"' EXIT
T=$(mktemp -d)

# ---------- phase 1: the pin ----------
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
  exit 2
fi
ACTUAL=$(md5sum "$COMPILER" | cut -d" " -f1)
echo "== pin =="
echo "  captured outputs name: $PIN"
echo "  compiler under test:   $ACTUAL"
if [ "$PIN" != "$ACTUAL" ]; then
  echo "  MISMATCH — re-run the capture and re-pin, or point this at that build."
  echo "  Carrying the numbers forward unmeasured is exactly CORRECTIONS.md C21."
  exit 2
fi
echo "  match"

# ---------- phase 2: the captured numbers ----------
echo
echo "== captured outputs (body must be byte-identical) =="
same=0; differ=0
for o in sio/*.output.txt; do
  m=$(basename "$o" .output.txt)
  [ -f "sio/$m.sio" ] || { echo "  MISSING SOURCE  $m"; differ=$((differ+1)); continue; }
  rm -f "$T/m.elf"
  if ! ( cd sio && "$COMPILER" "$m.sio" "$T/m.elf" ) > "$T/c.log" 2>&1; then
    echo "  DOES NOT COMPILE  $m"; differ=$((differ+1)); continue
  fi
  chmod +x "$T/m.elf"
  timeout 300 "$T/m.elf" > "$T/new.txt" 2>&1
  sed -i "/./,\$!d" "$T/new.txt"
  sed -n "/^date:/,\$p" "$o" | tail -n +2 | sed "/./,\$!d" > "$T/old.txt"
  if diff -q "$T/old.txt" "$T/new.txt" > /dev/null; then
    same=$((same+1))
  else
    echo "  DIFFERS  $m"; diff "$T/old.txt" "$T/new.txt" | head -4 | sed "s/^/      /"
    differ=$((differ+1))
  fi
done
echo "  identical=$same differs=$differ"

# ---------- phase 3: the measured claims ----------
echo
echo "== measured claims =="
bash tools/run_feature_probes.sh "$COMPILER" | sed "s/^/  /"
probe_rc=${PIPESTATUS[0]}

echo
if [ "$differ" -eq 0 ] && [ "$probe_rc" -eq 0 ]; then
  echo "VERIFIED: pin matches, $same outputs byte-identical, every probe reproduces."
  exit 0
fi
echo "NOT VERIFIED: $differ output(s) differ; probes exited $probe_rc."
exit 1
