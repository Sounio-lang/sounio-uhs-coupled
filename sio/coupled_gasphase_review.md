# Independent verification of `coupled_gasphase.sio` and the F1 retraction

An outside check of commit `9da4c3c` ("Retract the F1 falsification: the model
inverted its own oracle's limiting reagent"), run without assuming its
conclusions. Every claim below was re-derived or re-executed, not re-read.

Compiler: `gen3.elf`, md5 `0f3aa2c9dd3be4e407ce546130f7614c`, built from
`Sounio-lang/sounio` `feat/w1-qd128-transcend` @ `865dd6db87` — not through
`bin/souc` (G8). Checked against `sounio-uhs-coupled` @ `2eb2cc8`.

## 1. The units bug the retraction claims to fix

`sio/coupled_kinetics.sio:226` (pre-retraction): `h2_from_gas(p_total, y_h2) =
KH_H2 * y_h2 * p_total`, called with `p_total` in **bar**
(`h2_from_gas(40.0, 0.10)`), against `KH_H2 = 7.4530e-4` documented as
**molal atm⁻¹**. No conversion. `coupled_gasphase.sio` adds
`BAR_TO_ATM = 0.986923` and applies it before the Henry's-law term. Confirmed
real: `1/0.986923 - 1 = 1.33e-2`, matching the claimed "1.3% error in every
partial pressure."

## 2. `carbonate_equilibria.sio` is purely additive

`git diff 35359f4..9da4c3c -- sio/carbonate_equilibria.sio` shows only added
lines (the new `solve_nocalcite`/`speciate_nocalcite` pair and four `pub var`
declarations); nothing in the file above line 409 is touched. The "byte-
identical to before it" claim for the pre-existing closure holds by
inspection, not just assertion.

## 3. The extent-limit math, hand-derived independently

For the field condition at `V_gas = 1.0` (`y_H2=0.0989, y_CO2=0.0019,
P=78 bar, T=313.15 K`), computed by hand from the formulas in the file's own
header comment, not by reading its output:

```
p_atm = 78 * 0.986923 = 76.980
n_H2  = y_H2*p_atm*V/RT + KH_H2*y_H2*p_atm ≈ 0.30196 mol/kg
h2_left = n_H2 - 4*SO4_TOTAL = 0.30196 - 0.033076 = 0.26889
n_C   ≈ 0.00924 mol/kg  (gas term + NC_DIC from solve_nocalcite)
c_used = min(n_C, h2_left/4) = n_C   (carbon-limited)
ratio = (so4_sink + h2_left) / (so4_sink + 4*c_used) ≈ 0.30196 / 0.070052 ≈ 4.31
```

Matches the code's own printed value (4.3208) to the precision this hand
calculation supports. The mesocosm "starved at V=0.1" case was independently
re-derived the same way: at `V=0.1`, `n_H2 ≈ 0.01831 mol/kg < 4*SO4_TOTAL =
0.033076`, so `h2_left < 0` — the `EX_SULFATE_STARVED` branch is the correct
behavior here, not a special case papering over a bug.

The asymptote formula, `y_H2/(4·y_CO2)`, was re-derived from the code as
`V_gas → ∞`: the fixed sulfate term and the aqueous Henry's-law terms become
negligible against the `V_gas`-scaled gas terms, leaving
`ratio → (y_H2·P)/(4·y_CO2·P) = y_H2/(4·y_CO2)` — algebra, not curve-fitting.

## 4. Fresh re-run, compared against the commit's own numbers

```sh
cd sio && /workspace/uhs-feat/w1-qd128-transcend/gen3.elf coupled_gasphase.sio /tmp/gasphase_out
/tmp/gasphase_out
```

Compiled with zero warnings (cleaner than `coupled_kinetics.sio`, which has
pre-existing duplicate-name warnings). Printed ratios matched the commit's
`PHASE4_GATE.md` table to every displayed digit:

| condition | V=1 | V=10 | V=100 | asymptote | commit claims | match |
|---|---|---|---|---|---|---|
| mesocosm | 1.000 | 1.000 | 1.000 | 1.000 | same | exact |
| field | 4.3208 | 10.803 | 12.752 | 13.013 | same | exact |
| Lobodice | 1.000 | 1.058 | 1.118 | 1.125 | same | exact |

## 5. The oracle citation — the claim the whole retraction rests on

Checked directly against the archived files, not the commit message's
paraphrase of them:

- `data/usgs-auxvases/input_script_microbial-reactions_EOR-B106_12700years.txt`,
  lines 24–25: `swap H2(g) for H2(aq)` / `H2(g) = 91 fugacity`. Present,
  verbatim.
- `data/usgs-auxvases/output_microbial-reactions_EOR-B106_12700years.txt`:
  H2(g) 91.00 → 1.176 bar, CO2(g) 0.02433 → 7.328e-08 bar, first-step and
  last-step values. Matches the commit's cited trajectory exactly.

None of this was taken on the commit message's word; each number was located
in the archived file it claims to come from.

## Verdict

No defect found in `coupled_gasphase.sio` or in the retraction's supporting
claims. The units-bug diagnosis is real and independently confirmed, the
extent-limit model is algebraically sound and reproduces by hand, the
`carbonate_equilibria.sio` change is genuinely additive, and the oracle
citation checks out against the archive rather than resting on the commit's
own say-so.

**One note, not a defect.** In `extent_limit()`, the `else` branch that would
set `EX_RATIO = 1.0` when `EX_WITHOUT <= 0.0` is unreachable: past the
`EX_SULFATE_STARVED` check, `EX_WITHOUT` always includes `so4_sink =
4*SO4_TOTAL > 0`, so it can never be non-positive. No functional effect —
defensive code that never fires under these parameters.

**Not re-verified here:** `KH_H2 = 7.4530e-4 molal atm⁻¹` against `phreeqc.dat`
itself (taken as pinned, per the existing comment and its reuse from
`gate_4to1_52to1.sio`), and the "sulfate reduced first" ordering assumption,
which is a modelling choice the file discloses rather than derives.
