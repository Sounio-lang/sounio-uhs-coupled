# Results

**F1 passes for a CO₂-poor feed and is inert otherwise. F2 and F3 are both not evaluable as pre-registered; against the one usable field point F2 is satisfied and F3 cannot be settled. H1a survives — as a stoichiometric claim, not the kinetic one it was pre-registered as.**

Every number carries its producer in parentheses. `PREREGISTRATION.md` is
unaltered and its sha256 unchanged. `PHASE4_GATE.md` is the running log,
including a result published there and later retracted; this file summarises and
does not tidy that away.

---

## 1. The hypothesis changed shape under test

H1a as registered: calcite dissolution is the CO₂ source limiting hydrogenotrophic
methanogenesis, and therefore controls biotic H₂ loss. That has two mechanically
distinct channels, and they answer differently.

**The kinetic channel is closed.** The dual-Monod product differs between the 4:1
mesocosm and 52:1 field compositions by **1.000552** (`sio/gate_4to1_52to1.sio`),
because both substrates sit 149–2155× above their half-saturation constants in
both cases (`sio/gate_4to1_52to1.sio`). The same constant is worth **1.9 %** at
Aux Vases conditions and **zero** in the reference model, whose methanogenesis
`KA` is literally 0 (`sio/microbial.sio`). Three conditions, no kinetic effect.

**The mass-balance channel is real and large.** Carbon limits by *extent* — how
much H₂ can convert before the co-injected CO₂ runs out — not by rate.

---

## 2. F1 — remove calcite; under a factor of 2 and H1a dies

**Verdict: passes for a CO₂-poor feed; inert for a stoichiometric or CO₂-rich one.**

Biotic H₂ loss with calcite over without, 40 °C (`sio/coupled_gasphase.sio`).
`V_gas` maps to gas saturation with porosity cancelling, `Sg = V/(1+V)`
(`sio/f3_abiotic_band.sio`), so `V=1` is 50 % gas saturation.

| condition | H₂:CO₂ | V=0.1 | **V=1** | V=10 | V=100 | asymptote |
|---|---|---|---|---|---|---|
| mesocosm 10 %/2.5 % | 4:1 | *starved* | **1.000** | 1.000 | 1.000 | **1.000** |
| **field 9.89 %/0.19 %** | **52:1** | 1.000 | **4.321** | 10.803 | 12.752 | **13.013** |
| Lobodice 54 %/12 % | 4.5:1 | 1.000 | 1.000 | 1.058 | 1.118 | **1.125** |

The limit is exact and stoichiometric — **no rate constant, surface area or
half-saturation constant enters it.** As the reservoir grows, the ratio approaches
**y_H₂/(4·y_CO₂)**, the excess of stored hydrogen over the CO₂ stored with it.

**Confirmed twice independently.** The integrator reaches **4.320759** against the
analytic **4.320784** — 6×10⁻⁶, consistent with Euler truncation
(`sio/coupled_finite.sio`). And the cap from the field's **measured gas volumes**,
4·CO₂ᵢₙⱼ/H₂ᵢₙⱼ = **7.776 %**, matches the asymptote from **reported mole
fractions**, 4·y_CO₂/y_H₂ = **7.685 %**, to **1.2 %** (`sio/field_mass_balance.sio`).

**Where H1a is false:** at a stoichiometric feed the co-injected CO₂ covers the
hydrogen it arrives with, and calcite changes nothing (1.000 at every gas volume);
at Lobodice's CO₂-rich town gas, nearly nothing (1.000–1.118). H1a holds **only**
for a CO₂-poor feed — which is what the field trial was.

*Caveat carried:* the with-calcite branch assumes the mineral is in excess and does
not model its mass. `sio/abiotic_kinetics.sio` measured calcite equilibration as
fast relative to the microbial clock, which supports the assumption without
establishing the inventory.

---

## 3. F2 — the band must encompass both field points, nothing tuned

**Verdict: not evaluable as pre-registered. Satisfied against Sun Storage alone.**
Reported both ways, as `CORRECTIONS.md` C12 required before either was measured.

**Sun Storage — encompassed, nothing tuned.** The no-calcite branch has a hard cap
no kinetics can exceed; the with-calcite branch has none (`sio/coupled_finite.sio`):
minimum recovery **0 %** at V=0.1, **76.86 %** at V=1, **90.74 %** at V=10,
**92.16 %** at V=100. Observed recovery is **84.397 %** on one volume basis and
**83.579 %** on the other, unreconciled, with the paper's own 84.3 % reproducing
from neither (`CORRECTIONS.md` C3). **Both fall inside the no-calcite range**,
between V=1 and V=10 — ordinary gas saturations. The sweep was fixed before the
comparison.

**Lobodice — the data do not close.** Methanation consumes five moles of gas and
returns one, so 54 % → 37 % H₂ is not a 31 % hydrogen loss. **N₂ is the
instrument**: inert, so its mole fraction measures only the change in total moles.
Four checks fail (`sio/lobodice_massbalance.sio`): compositions sum to **99.5 %**
and **98.0 %** with no stated convention; the N₂ tracer gives **72.6 %** volume
loss against a stated **10–20 %**, off **3.6–7.3×**; CH₄ moles **fall 49.5 %**
against a measured microbial isotopic signature for methane *formation*; and
closure predicts a final CH₄ mole fraction of **107.2 %**, impossible.

The source's own model corroborates it — Trémosa et al. simulate Lobodice and get
a 46 % volume decrease, far above the stated 10–20 %. The primary sources (Smigan
1990, Buzek 1994) were never obtained, so the failure cannot be attributed.
Selecting whichever subset would permit a band around 37 % is fitting to a target,
which the protocol forbids — **so the probe refuses** (`CORRECTIONS.md` C13).

**Lobodice could not have discriminated H1a even with clean data**: its F1
asymptote is **1.125** (`sio/coupled_gasphase.sio`). The site is CO₂-rich, which
is exactly where the mechanism is inert. The pre-registered choice was aimed at
the wrong kind of site.

**The study therefore has one clean field constraint, not two.**

---

## 4. F3 — if the abiotic band alone encompasses both points, the coupling is superfluous

**Verdict: not evaluable as pre-registered; and against Sun Storage alone it
cannot be settled either. This reverses an earlier statement in this study.**

F3 needs both points, and Lobodice is unevaluable — so, like F2, it reduces to Sun
Storage. There it turns on the size of the physical loss, and that is not known
well enough to decide.

### What the abiotic channels are worth

| channel | status |
|---|---|
| **H₂ dissolution** | **1.879 %** of H₂ at Sg = 50 % (`sio/f3_abiotic_band.sio`); the dissolved *fraction* is pressure-independent — P cancels. **But it is reversible** (§4.2) |
| **mineral redox** | **bounded: < 0.033 %** over 103 d, **< 0.053 %** at 285 d (`sio/f3_abiotic_band.sio`), from Truche et al. 2010 run #P20 — a measurement, not an extrapolation |
| **abiotic methanation** | **refused.** Several measured nulls at UHS conditions, **none publishing a detection limit** |
| **caprock diffusion** | **ruled out by magnitude**: 0.006 % over 12 months (Mignard 2016), median 0.032 % and max 0.80 % per year (Battelle/USEA 2023), 0.36 % over 30 years in salt (Ghaedi & Gholami 2025) — **two to three orders of magnitude below the observed shortfall** |
| **capillary trapping + incomplete sweep** | **unbounded, and the term that matters** (§4.3) |

`sio/f3_abiotic_band.sio` still refuses to emit a total abiotic band: a null
without a detection limit is not a zero.

### 4.2 Dissolution is reversible — an over-count in this study's own decomposition

`sio/field_mass_balance.sio` counted dissolution (3 692 m³, 20.0 % of the
shortfall) as a physical **loss**. Tawil et al. (2024, gold OA, measured) state
plainly that it is not: *"in the absence of microbial activities, the gas quantity
that dissolves as the pressure increases during the injection period is **not
lost**. It is released as the pressure is gradually reduced during the withdrawal
phase."* Hellerschmied say the same mechanistically for CO₂. Dissolution is a
**reversible partitioning**, not a loss term, on cycle timescales — so the
physical share computed in that module is an **over-count**, and the honest
physical total at Lehen rests on cushion-gas migration alone.

### 4.3 The term that decides F3 is the one nobody has measured

Of the **18 442 Sm³** not recovered (15.60 % of injected), Hellerschmied's own
numbers give cushion-gas migration **9 310 m³ (50.5 %)**, dissolution ≤ **3 692 m³
(20.0 %, reversible)** and methanation **3 842 m³ (20.8 %)**
(`sio/field_mass_balance.sio`). But the authors state their cushion-gas estimate
*"could potentially lead to an **underestimation** of gas migration."*

An independent **reaction-free** reservoir simulation — Eckel et al. 2025, Ketzin,
open access, hysteretic relative permeability, 10-month cycles, **no microbial or
geochemical reactions of any kind** — reports H₂ recovery of **64 % (long
vertical well) to 91 % (long horizontal well)**, with 5–11 % residually trapped,
up to 25 % mobile-but-unrecovered and ≤ 3 % dissolved (all values as published).
**The observed recovery of 84.40 % / 83.58 % lies inside that published range.**
Physical trapping and incomplete sweep alone are numerically sufficient to
produce the Lehen shortfall.

And **composition change is not evidence of reaction**: the same reaction-free
model shifts produced-stream H₂ purity from 12–27 % to 68–84 % across ten cycles,
while at Lehen produced H₂ fell to 2.8 % by mixing into cushion gas alone.

**That term is the least constrained quantity in the whole comparison.** A
dedicated search of the residual-trapping literature returns three findings that
together make it undecidable:

- **No field measurement of H₂ capillary or residual trapping exists.** In the
  only published UHS field trial in porous media, the strings *capillar* and
  *trapp* do not appear anywhere in the results or discussion — capillary
  trapping is not among the mechanisms Hellerschmied invoke, and the paper offers
  no field constraint on residual saturation at all.
- **Laboratory residual saturations span `Sgr` < 0.02 to 0.44** — a factor above
  20 — across nominally similar water-wet quartz sandstones. That dispersion is
  larger than any modelled effect being argued about.
- **No published reservoir model separates residual trapping from operational
  constraints.** Gas remaining at cycle end mixes genuinely trapped gas with gas
  left in place by minimum bottomhole pressure, water-cut limits and cycle
  duration, and no accessible study resolves the two.

Three of the four core-scale datasets also used KI solution or tap water rather
than a reservoir brine — chosen for X-ray contrast — so the salinity dependence
of H₂ trapping is unconstrained as well.

### 4.4 Why it still cannot be called superfluous

Biotic activity at the site is **measured, not inferred**: mcrA transcripts, a
δ¹³C_CH₄ shift of **−2.7 ± 0.5 ‰ (p = 0.0002)**, and **960 m³** of CO₂ consumed.
No physical process explains an isotopic signature of microbial methanogenesis.

So F3 splits by observable. **For H₂ recovery, the coupling is not demonstrated
necessary** — physical routes may account for all of it, and the dominant physical
term is bounded only by a simulation of a different reservoir. **For the isotopic
and carbon observables, the coupling remains necessary**, and nothing abiotic
substitutes for it. The pre-registered criterion did not anticipate that split,
and it is reported rather than resolved by choosing the convenient half.

**This reverses what this study said earlier**, which was that F3 "does not fire."
That was stated before the reaction-free simulation and the reversibility of
dissolution were known, and it was twice too confident: first in comparing one
abiotic channel against a shortfall that is mostly migration, then in treating
reversible dissolution as a loss.

---

## 5. What is bounded rather than known

**A published F1 result was retracted.** `sio/coupled_kinetics.sio` reported
F1 = 1.000000 everywhere and concluded H1a falsified. It supplied H₂ as a
Henry's-law snapshot of the aqueous film alone (2.98e-3 to 5.75e-3 molal) while
keeping the reference sulfate inventory of 8.269e-3 molal, which needs 3.31e-2
molal of H₂ — so **sulfate reduction needed 5.8–11.1× more H₂ than the model
contained** and consumed all of it before methanogenesis reached carbon. The
oracle holds H₂ at 4.226e-2 molal because its input declares `H2(g) = 91
fugacity`. The model inverted its own oracle's limiting reagent.

**Three quantities are bounded, not known:**

1. **The biotic share is an upper bound.** The CO₂ decrease could be dissolution:
   the paper's own 40 000 m³ of brine holds **3 109–3 280 Sm³** of CO₂ against a
   **960.5 m³** decrease — three times over (`sio/field_mass_balance.sio`, C15).
2. **The sterile control cannot separate reaction from leakage.** A four-lab
   round-robin found H₂ loss in sterile controls of 26 %, negligible, 8.8 % and
   none — bottles alone (C17).
3. **No kinetic parameter set fits the timescale.** The USGS archive implies a
   methanogen doubling time of **610.1 years**; Strobel's species 2 at 37 °C,
   **19.25 hours**; ratio **2.78 × 10⁵** (`sio/coupled_finite.sio`). With the
   archive's parameters the field gives **99.999999998 %** recovery at 285 days
   (`sio/coupled_finite.sio`) — short by ten orders of magnitude. Independently,
   Trémosa et al. had to **divide laboratory methanogenesis rates by 4–50** to
   match Lobodice, so the microbial explanation there is calibrated, not
   rate-predictive.

**Consequence: this study predicts an extent, not a timescale.** Every F1 and F2
statement is a stoichiometric bound. Whether a reservoir reaches its bound within
a storage cycle is a kinetic question no available source can answer.

---

## 6. Seventeen corrections

`CORRECTIONS.md` records seventeen, each with the measurement that exposed it.
Bearing directly on the verdicts: **C3/C4/C14** — three arithmetic
inconsistencies in the single field mass balance on which F2 and F3 now rest;
**C13** — Lobodice failing four consistency checks; **C15** — the CO₂ attribution
not established; **C16** — the literature's quantitative anchor for abiotic
negligibility being an extrapolation from 250–280 °C restated downstream as a
measurement at 90 °C, its companion figure unverifiable because the primary source
is closed.

---

## 7. Reproduction

Engine is Sounio; harnesses are C++; Python appears only as data marshalling and
oracle binding. Modules compile with `gen3.elf` md5
`0f3aa2c9dd3be4e407ce546130f7614c`, from `Sounio-lang/sounio`
`feat/w1-qd128-transcend` @ `865dd6db87` — **not** through `bin/souc`
(`LANGUAGE_GAPS.md` G8).

```
cd sio && <gen3.elf> <module>.sio /tmp/out && /tmp/out
```

Each module's unedited stdout is committed beside it as `<module>.output.txt`
with compiler md5, repository commit, and capture date.
