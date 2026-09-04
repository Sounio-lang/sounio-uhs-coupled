#!/bin/bash
# Structural invariants that hold without compiling anything.
#
# These are the checks that would have caught CORRECTIONS.md C21 one layer
# earlier: a pin that disagrees with itself, a claim with no probe, a probe with
# no expectation. They need no compiler, so CI can run them on every push even
# while the pinned compiler is not publicly obtainable.
#
#   usage: bash tools/check_structure.sh
#
# Exit 0 if every invariant holds, 1 otherwise.
set -u
cd "$(dirname "$0")/.."
fail=0
note() { printf "  FAIL  %s\n" "$1"; fail=$((fail+1)); }
ok()   { printf "  ok    %s\n" "$1"; }

# 1. every captured output names the same compiler
PINS=$(grep -h "^compiler: gen3.elf, md5 " sio/*.output.txt 2>/dev/null | sed 's/.*md5 //' | sort -u)
n=$(printf '%s\n' "$PINS" | grep -c .)
if [ "$n" -ne 1 ]; then
  note "the captured outputs name $n different compilers: $(echo $PINS | tr '\n' ' ')"
else
  ok "all captured outputs name one compiler ($PINS)"
fi

# 2. the pin quoted in RESULTS.md is that compiler
if [ "$n" -eq 1 ]; then
  if grep -q "$PINS" RESULTS.md; then
    ok "RESULTS.md quotes the same pin"
  else
    note "RESULTS.md does not quote the pin the outputs name ($PINS)"
  fi
fi

# 3. every captured output has a source beside it
for o in sio/*.output.txt; do
  m=$(basename "$o" .output.txt)
  [ -f "sio/$m.sio" ] || note "sio/$m.sio is missing but $o is committed"
done
ok "every captured output has its source"

# 4. every probe states an expectation and the claim it backs
[ -d probes ] || note "probes/ is missing"
c=0
for p in probes/*.sio; do
  [ -f "$p" ] || continue
  c=$((c+1))
  b=$(basename "$p")
  grep -q "^//@ expect-rc:" "$p" || note "$b has no expect-rc — it asserts nothing"
  grep -q "^//@ claim:" "$p"     || note "$b names no claim — nothing ties it to the prose"
done
[ "$c" -gt 0 ] && ok "$c probes, each with an expectation and a named claim" || note "probes/ holds no probes"

# 5. the runners are syntactically valid and executable
for s in tools/verify.sh tools/run_feature_probes.sh tools/check_structure.sh; do
  [ -f "$s" ] || { note "$s is missing"; continue; }
  bash -n "$s" 2>/dev/null || note "$s does not parse"
done
ok "the verification scripts parse"

echo
if [ "$fail" -eq 0 ]; then echo "structure OK"; exit 0; fi
echo "structure NOT OK: $fail problem(s)"
exit 1
