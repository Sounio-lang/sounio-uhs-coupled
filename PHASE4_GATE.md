# Phase 4 gate: STOPPED

The protocol makes the mesocosm-vs-field comparison a hard stop — *"Reproduzir
PRIMEIRO a diferença mesocosmo vs campo do Hellerschmied (4:1 vs 52:1 em H₂:CO₂)
— se o modelo não explica essa razão de ~30× na taxa, não está pronto para o
resto"*, and *"Se a Fase 4 não reproduzir 4:1 vs 52:1, parar e reportar."*

**The gate fails. Work on the rest of Phase 4 has stopped.** This is that report.

---

## 1. The ~30× figure is not in the source

Searched the article and every supplementary file for fold-changes, factors and
rate comparisons between mesocosm and field. The only match is *"three times
higher in energy density per unit volume"* for CH₄ against H₂ — unrelated.

There is also **no measured field methanation rate** to compare against. The
0.26 mmol l⁻¹ h⁻¹ MER is stated as *"the calculated average MER (equation (7))
for the test reservoir operated at **mesocosm** productivity"* — a projection of
mesocosm behaviour onto the field's brine volume, not a field measurement (see
`CORRECTIONS.md` C2).

Under the rule that no number is invented, interpolated or inherited, **this
reading of the gate cannot be tested and the figure is not carried** — the same
disposition already applied to the "4–13 % Monte Carlo recovery".

## 2. What the source actually claims

The paper attributes the difference to *"several factors"*:

> *"**First**, in contrast to the field trial, the mesocosm experiments were
> conducted with a substrate gas mixture at optimal stoichiometry for
> hydrogenotrophic methanogenesis (mesocosms H₂:CO₂ = 4:1; field H₂:CO₂ = 52:1)
> to explore the process potential at near-optimal conditions. **Second**, the
> difference in scale of several orders of magnitude translates to greater
> heterogeneity of the environment for water saturation, porosity and gas
> permeability **and unknown factors**, which may have impeded the
> geo-methanation process in the field."*

The stoichiometry is listed first among several, and **"unknown factors" are
named explicitly**. The paper does not claim the ratio explains the difference.

## 3. The testable version of the gate, and it fails

Does the H₂:CO₂ ratio produce a rate difference through dual-Monod kinetics?
Computed with Strobel's constants (K_H2 = 2.0e-05, K_CO2 = 1.1e-05 molal, both
taken from the literature rather than fitted) and Henry constants from the pinned
`phreeqc.dat`:

| | P_H2 | P_CO2 | H₂(aq) | CO₂(aq) | H₂/K | CO₂/K | Monod product |
|---|---|---|---|---|---|---|---|
| mesocosm 10 % / 2.5 %, 40 bar | 4.00 | 1.00 | 2.981e-03 | 2.371e-02 | **149** | **2155** | 0.9928754 |
| field 9.89 % / 0.19 %, 78 bar | 7.71 | 0.148 | 5.749e-03 | 3.514e-03 | **287** | **319** | 0.9934235 |

**Ratio field / mesocosm = 1.000552.**

The model predicts the field to be **0.055 % faster**, not thirty times slower.

The reason is structural, not a tuning problem: **both substrates sit 150 to
2150 times above their half-saturation constants in both cases.** Monod kinetics
are saturated at both conditions and are therefore insensitive to the ratio. And
the field, at 78 bar, carries *more* dissolved H₂ than the mesocosm at 40 bar,
which is why it comes out marginally ahead.

For the CO₂ term to fall to one half, dissolved CO₂ in the field would have to
drop by a factor of **319**.

## 4. This converges with two earlier measurements

| condition | effect of the CO₂ half-saturation term |
|---|---|
| USGS reference model at Aux Vases | **none** — KA = 0, the term is identically 1 |
| Strobel K_CO2 applied at Aux Vases | **1.9 %** |
| mesocosm vs field, this gate | **0.055 %** |

Three independent conditions, and the CO₂ half-saturation constant is never a
limiting mechanism at any of them.

## 5. What this does and does not say about H1a

**It does not falsify H1a.** H1a claims calcite-derived CO₂ *limits*
methanogenesis, and there are two distinct channels by which CO₂ can limit it:

- the **kinetic** channel, the Monod term — measured here at 0.055 %, at Aux
  Vases at 1.9 %, and switched off entirely in the reference model;
- the **mass-balance** channel, carbon availability — which Phase 3 measured as
  worth a factor of **3.7** in methane when replenishment from the total carbon
  pool is removed.

**H1a, if it survives, must run through mass balance and not through the CO₂
Monod constant.** The kinetic channel is closed by these three measurements. That
is a sharper hypothesis than the one that was pre-registered, and it is reachable
only because the gate was run rather than assumed.

## 6. What would reopen the gate

Any of these, and none is currently available:

1. **A measured field methanation rate.** Without one there is nothing to form a
   ratio against, whatever the model predicts.
2. **A CO₂ half-saturation constant much larger than Strobel's.** For the
   stoichiometry to matter at field concentrations, K_CO2 would have to be some
   two orders of magnitude higher. Thaysen gives environmental envelopes, not
   kinetic constants, so it cannot supply one.
3. **A carbon-poor regime.** H1a needs bicarbonate near K_CO2. Aux Vases is 53×
   above it and the field is 319× above it. Whether any real reservoir sits there
   is an open question, and Lehen's carbonate chemistry is among the things its
   paper does not report.

## 7. Status

Phase 4 is stopped at its gate, as the protocol requires. The coupling itself —
shared pH, calcite-sourced CO₂, H₂ solubility limiting aqueous H₂ — is **not**
built, because the protocol makes this gate the precondition for building it.

Phases 0 through 3 stand and are unaffected.

---

## Addendum, 2026-09-03: decision on how Phase 4 proceeds

Directed explicitly: **proceed with mass balance as the surviving version of
H1a.** The kinetic channel (CO2 Monod half-saturation) is dropped from the
coupled model. This addendum states, before any further code is written, what
that means mechanistically and how it will be tested — so the test is specified
before it is run, the same discipline `PREREGISTRATION.md` already applies.

### What "mass balance" as the mechanism requires

Not a rate-law change. Section 5 above already showed the Monod term is
saturated at every condition checked (mesocosm, field, Aux Vases). The
mechanism has to work through the **size of the total dissolved-carbon pool**,
not through how fast a saturated Monod term turns over.

Two chemically distinct sources of that pool, and the coupled model has to keep
them apart rather than blend them into one invented number:

1. **CO2 already dissolved from the stored gas stream itself**, via Henry's law
   — present whether or not calcite exists, sized by the gas composition alone.
2. **Carbon released by calcite dissolution**, 1:1 with dissolved Ca²⁺ by the
   stoichiometry CaCO₃ → Ca²⁺ + CO₃²⁻ — present only if calcite is present.

`sio/abiotic_kinetics.sio` already measured that at any swept surface area the
calcite system reaches its equilibrium ceiling in minutes to at most a few
years — far inside a step of the microbial integrator (~0.3 yr) and far inside
any storage horizon under study. **Calcite dissolution is fast relative to the
biotic clock.** That was measured, not assumed, and it licenses treating
calcite — when present — as continuously re-equilibrating: it does not just add
a one-time increment of carbon, it holds the pool near its equilibrium value as
methanogenesis draws it down, for as long as calcite mineral mass is not
exhausted (assumed in excess here; the mineral budget itself is not modelled).

### The two-scenario test this implies

**With calcite**: total dissolved carbon is clamped near the calcite-saturation
value at the running pH and P(CO2) — computed the same way
`carbonate_equilibria::solve_ph_pairs` already computes it for the abiotic
validation, not a new closure.

**Without calcite**: no mineral buffering exists. Total dissolved carbon is
whatever the injected CO2 alone puts into solution, speciated by an open
carbonic-acid charge balance with **no calcium term** — H⁺ = HCO₃⁻ + 2 CO₃²⁻ +
OH⁻, nothing else. This is a new, smaller closure than the calcite one, not a
subtraction from it, and needs its own derivation and its own sanity check
before it is trusted for anything. Building it is the next step.

**F1, restated concretely**: integrate biotic H2 consumption (4×(SRB rate + MET
rate), the same accounting already used in `sio/microbial_kinetics.sio`) under
both scenarios, same T, same gas composition, same horizon. If the with/without
ratio is under a factor of 2 (band included), H1a dies — now specifically as a
mass-balance claim, since section 5 already closed the kinetic reading.

### What is explicitly NOT done here

No number for calcite's reactive surface area, or for the reservoir's real
carbonate mineral content, exists in any source used by this study. Both stay
**swept inputs**, exactly as `abiotic_kinetics.sio` already treats surface area
— the F1 verdict will be reported as a function of that sweep, not as a single
point, and if the verdict changes sign across the swept range that is itself
part of the result.

---

## Result, 2026-09-03: F1 tested on the mass-balance channel, and it fails too

`sio/coupled_kinetics.sio` builds the two-branch model the addendum above
specified and runs F1 at the same two labelled conditions as the Phase 4 gate
(mesocosm 4:1, field 52:1), at two horizons.

**With calcite**: total dissolved carbon held at the calcite-equilibrium
ceiling, `DIC_eq(T, P_CO2)`, computed by `carbonate_equilibria::solve_ph_pairs`
— the exact function already used and verified for the abiotic model, called
here without modification. **Without calcite**: total carbon fixed at t=0 by a
new closure, `solve_nocalcite`, structurally identical to `solve_ph_pairs` but
with every calcium term removed (pure H⁺ = HCO₃⁻ + 2 CO₃²⁻ + OH⁻), and left to
deplete under methanogenesis exactly as `microbial_kinetics.sio` already
depletes the Aux Vases carbon pool. `carbonate_equilibria.sio` was extended
**additively** — four new `pub var` outputs on the existing `speciate()`
function, `SP_M_CO2/SP_M_HCO3/SP_M_CO3/SP_DIC` — and every dependent file
(`abiotic_kinetics.sio`, `hellerschmied_validation.sio`, `sweep.sio`,
`sio/gate_4to1_52to1.sio`) was recompiled clean before this was trusted; the
module's own self-check output is byte-identical to before the change.

### Result

| horizon | condition | ratio (with / without calcite) |
|---|---|---|
| 12 700 yr (Aux Vases-comparable) | mesocosm | **1.000000** |
| 12 700 yr | field | **1.000000** |
| 285 d (Sun Storage-comparable) | mesocosm | **1.000000** |
| 285 d | field | **1.000000** |

Not "under 2" — **exactly 1**, to the precision printed, at every condition
checked. **F1 fails. H1a fails on the mass-balance channel too**, at both
labelled conditions, at both horizons.

### Why, and it is two different reasons, not one

**At the 12 700-year horizon, the system is H2-limited, not carbon-limited.**
Diagnostic state at the end of the no-calcite run: aqueous H2 is fully
exhausted (0), while total dissolved carbon has moved by under 0.0001% of its
starting value (mesocosm: 2.382160868617e-2 → 2.382159892980e-2 molal; field:
3.556375171758e-3 → 3.556364136641e-3 molal). Total H2 available at these gas
compositions (2.98e-3 to 5.75e-3 molal, from the same Henry's-law constant used
throughout) caps cumulative carbon consumption, by the 4:1 stoichiometry, at
roughly 7e-4 to 1.4e-3 molal — three to thirty times smaller than even the
**no-calcite** carbon pool. Calcite's extra buffering capacity is never drawn
on, with or without it, because carbon was never within reach of the ceiling
either branch would have imposed.

**At the 285-day horizon, the system is time-limited, not carbon-limited.**
Aqueous H2 barely moves at all (mesocosm final 2.9812e-3 against an initial
2.9812e-3; field 5.7494e-3 against 5.7494e-3). This is the seed-biomass
assumption inherited from the Aux Vases archive doing the work: biomass starts
at 1e-6 mg/kg and the archive's own doubling times are 103 yr (SRB) and 610 yr
(MET) — over 285 days neither population has grown enough to consume anything
measurable, so the ratio is 1 for a reason that has nothing to do with carbon
at all. **This horizon's result should be read as "cold-start biomass makes 285
days too short to see any biotic effect," not as evidence about calcite.** A
resident, geologically-established microbial population — which is what a real
reservoir has, and what Hellerschmied's actual field trial presumably started
from — is a different initial condition and is not tested here. No source used
in this study gives a resident biomass density, so this stays an open input,
flagged rather than guessed.

### The swept input turned out not to matter, which is itself the result

The addendum committed to reporting F1 as a function of the reactive-surface-
area sweep, since no source supplies that number. It was not swept here,
because it did not need to be: calcite's role is to raise the with-calcite
carbon ceiling, and in every branch checked the **without-calcite** ceiling was
already far above what either H2 supply or elapsed time allowed the system to
reach. A faster or slower dissolution rate cannot change a verdict that carbon
was never the constraint in the first place. If the verdict is to change, it
will not be through the surface-area sweep — it will require conditions where
carbon, not H2 or biomass, is actually scarce, which is a different experiment
than the one run here.

### Where this leaves H1a

Both pre-registered readings have now been tested at the mesocosm and field
conditions this study has access to, and both return the same verdict:

- **Kinetic channel** (`PHASE4_GATE.md`, section 3): CO2 Monod term saturated,
  ratio 1.000552.
- **Mass-balance channel** (this section): carbon never approaches either
  branch's ceiling, ratio 1.000000.

At these conditions, calcite's presence or absence changes nothing measurable
about biotic H2 loss, by either mechanism this study can test. That is not yet
a statement about all of parameter space — surface area was shown not to
matter here, but salinity, a genuinely CO2-rich gas stream, or a
geologically-resident biomass population are conditions this test has not
touched, and `PREREGISTRATION.md`'s own H1b (pure carbonate, in silico only)
was scoped for exactly this kind of extension. Proceeding to S1/S5's full swept
verdict, or accepting this as sufficient and reporting H1a falsified, is a
decision this report surfaces rather than makes.
