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

---

## Result, 2026-09-03 (continued): the targeted sweep, and it does not move

The previous section flagged three untested conditions that could, in
principle, put carbon back in reach: salinity, a genuinely CO2-rich gas
stream, and a resident rather than cold-start biomass. All three are now
tested, each against a source-grounded bound rather than an invented number,
and none changes the verdict.

`sio/coupled_kinetics.sio` gained `f1_condition` (a parameterised version of
the existing F1 test) and three sweep drivers. `init_coupled`/`run_coupled`
took a new `biomass_init` parameter; every existing call site was updated to
pass the same `1.0e-6` cold start used before, so the original two-condition
result is unchanged, not just re-run.

### Sweep 1 — salinity, at the organisms' own viability limits

Not a Lehen number: Lehen's salinity was never reported (CORRECTIONS.md C6).
Tremosa et al. 2023 — the Lobodice source already in `data/tremosa-lobodice`
— states the bound directly: *"most methanogens may grow at salinities up to
0.77 M NaCl and most sulfate-reducers and acetogens at between 0 and 0.4 M
NaCl."* Both bounds were run, at both the mesocosm and field conditions, over
12 700 years.

**Ratio: 1.000000, all four runs.** Total dissolved carbon moves by 0.03–0.07%
between the 0.40 M and 0.77 M cases (0.026% mesocosm, 0.068% field, computed
from the run's own initial-DIC diagnostics — activity-coefficient effects at
these ionic strengths are real but second order) and H2 is still fully
exhausted in the no-calcite branch in every case. Raising ionic strength
toward the edge of what these organisms can survive at all does not touch the
mechanism: H2, not carbon, is still what runs out.

### Sweep 2 — Lobodice's actual injected composition

Not a synthetic condition: 54% H2, 22% CH4, **12% CO2**, 9% CO, 2.5% N2 at 4
MPa (40 bar), 25–45 °C, is what Tremosa et al. 2023 report was actually
injected at Lobodice (CORRECTIONS.md C5; T taken at 40 °C, mid-range). At 12%,
this is the most CO2-rich condition available anywhere in this study — about
63 times the field gate condition's CO2 fraction (0.19%, from that
condition's own P_CO2 = 0.1482 atm at 78 bar total).

**Ratio: 1.000000**, at the 12 700-year horizon and, separately, at
**Lobodice's own actual storage duration, seven months** — real, not
extrapolated. At seven months aqueous H2 sits at 1.6098e-2 molal, close to its
own initial charge under this richer H2 fraction (54% vs. mesocosm's 10%) —
the same barely-moved pattern already seen at 285 days for the mesocosm
condition (2.9812e-3 → 2.9812e-3 to displayed precision), just at a higher
concentration because more H2 was dissolved to begin with. This reading is
time-limited, the same way the operational-horizon test above was, and says
nothing about carbon either.

### Sweep 3 — established rather than cold-start biomass

No source used in this study gives a resident cell density for any of these
reservoirs. What is available: the USGS Aux Vases archive's own sensitivity
sweep for this exact input
(`data/usgs-auxvases/sensitivity_analysis_biomass.csv`), run at 1e-8 through
1e-4 mg/kg. 1e-4 — the archive's own upper bound, 100x the cold start used
everywhere else in this study — was run at the operational (285 d) horizon,
at both mesocosm and field conditions, and at the geological horizon for
completeness.

**Ratio: 1.000000**, all three. Even at 100x the starting population, aqueous
H2 still moves by only 0.00001–0.00002% over 285 days (0.0000232% mesocosm,
0.0000121% field): the doubling times this study already established (103 yr
SRB, 610 yr MET) put any starting population two
orders of magnitude above cold-start still deep in the early-exponential
regime at under a year. This is not evidence that biomass never matters — it
is evidence that the *specific* upper bound this study can point to, without
inventing one, is still far too small to matter at operational timescales.

### Verdict

Every condition this study has data or a defensible, source-grounded bound
for — two labelled gas compositions, two salinities at the edge of microbial
viability, the one real CO2-rich composition available, and the one
literature-grounded biomass upper bound available — returns the same number:
**ratio = 1.000000.** Not close to the factor-of-2 bar F1 sets; exactly at the
point of no effect, everywhere checked.

**H1a is falsified on both readings this study can test, at every condition
this study has data for.** The kinetic channel failed at ratio 1.000552
(`PHASE4_GATE.md`, section 3). The mass-balance channel fails at ratio
1.000000, robustly, across the salinity range these organisms can survive, the
most CO2-rich real composition on record for a site like this, and the
largest biomass this study can point to without guessing. What remains
genuinely untested is outside this study's data: H1b (pure carbonate, in
silico only, pre-registered as such) and any condition beyond what a real,
cited source supports. Within that boundary, this sweep is not a spot-check
that happened to miss the effect — it is the effect's own most favourable
directions, checked, and it is not there.

---

## RETRACTION and correction, 2026-09-03: F1 was tested on a defective model

**The two sections above reporting F1 = 1.000000 are retracted.** The verdict
was produced by a model that could not express H1a's mechanism, and the reason
is a defect in that model, not a property of the system. This section states
the defect, the corrected test, and the new verdict.

### The defect

`sio/coupled_kinetics.sio` supplied H2 as a single Henry's-law snapshot of the
**aqueous film alone** — 2.98e-3 molal (mesocosm) to 5.75e-3 molal (field) —
while keeping the reference model's **sulfate** inventory of 8.269e-3 molal.
Sulfate reduction consumes 4 H2 per SO4, so exhausting that pool needs
3.31e-2 molal of H2:

| | H2 supplied | H2 sulfate alone needs | shortfall |
|---|---|---|---|
| mesocosm | 2.981e-3 molal | 3.308e-2 | **11.1×** |
| field | 5.749e-3 molal | 3.308e-2 | **5.8×** |

Sulfate reduction therefore consumed every H2 molecule in the model before
methanogenesis could touch carbon at all. The diagnostic that read "H2-limited"
was true *inside* that model and meaningless outside it: the H2 budget was
being spent on a reaction that has nothing to do with H1a, and carbon was
never approached for a reason unrelated to whether carbon is scarce.

**The oracle says so directly.** The archived input script declares
`swap H2(g) for H2(aq)` with `H2(g) = 91 fugacity`, giving H2(aq) = 4.226e-2
molal — **7 to 14 times more H2** than the coupled model contained. The
previous model inverted its own oracle's limiting reagent. That is the defect,
and it is mine.

A second, smaller error is corrected at the same time: the previous module
applied Henry's-law constants (which are per **atm**) to pressures stated in
**bar** without converting, a 1.3 % error in every partial pressure.

### The oracle's own trajectory identifies the limiting reagent

From `data/usgs-auxvases/output_microbial-reactions_EOR-B106_12700years.txt`:

| | start | end | change |
|---|---|---|---|
| H2(g) | 91.00 bar | 1.176 bar | ÷ 77 |
| **CO2(g)** | **0.02433 bar** | **7.328e-08 bar** | **÷ 332 000** |

CO2 collapses five and a half orders of magnitude and sits pinned at a floor
from the ninth reporting step onward — while H2 is still at 19.7 bar. And the
stoichiometry confirms it: 4 × (SO4 8.269e-3 + DIC 2.2565e-3) = 4.210e-2 molal
of H2 exhausts both pools, against 4.226e-2 available. **Balanced to 0.4 %.**
Carbon is a genuine limiting reagent in the reference model.

### The Phase 4 gate also asked the wrong question

Section 3 above tested whether the H2:CO2 ratio changes the **rate**, through
the dual-Monod term, and measured 1.000552. **That measurement stands** — Monod
is saturated at both compositions, and the kinetic channel is still closed.

But the source's own sentence is about stoichiometry: *"the mesocosm experiments
were conducted with a substrate gas mixture at optimal stoichiometry for
hydrogenotrophic methanogenesis (mesocosms H2:CO2 = 4:1; field H2:CO2 = 52:1)."*
Stoichiometry governs **how much** H2 can be converted before the co-injected
CO2 runs out. That is *extent*, not rate. Testing the rate reading and
concluding the stoichiometry "cannot be the mechanism" was a conclusion about
the wrong quantity.

### The corrected model

`sio/coupled_gasphase.sio`. Underground hydrogen storage stores **gas**; the
water is a connate film. So each species' inventory per kg of water is

    n_i = y_i * P * V_gas / (R T)   [gas]  +  KH_i * y_i * P   [aqueous]

with `V_gas` the gas volume per kg water — a **swept input**, since no source
here reports one for these reservoirs. V_gas = 1 L/kg is what porosity 0.2 at
50 % gas saturation implies, and it is reported inside the sweep rather than
presented as the answer.

The extent limit is exact and **no rate constant, surface area or
half-saturation constant enters it**. Sulfate is reduced first, costing
4 × SO4_total. Then: *without* calcite, methanogenesis stops when the finite
gas + aqueous carbon inventory is gone; *with* calcite, carbon is resupplied by
the mineral and methanogenesis runs until H2 is gone.

### Result — F1 discriminates sharply, along exactly H1a's axis

Computed by the Sounio engine, cross-checked against the closed form
`y_H2 / (4·y_CO2)` that the ratio must approach as V_gas grows:

| condition | H2:CO2 | V=0.1 | **V=1.0** | V=10 | V=100 | asymptote |
|---|---|---|---|---|---|---|
| mesocosm | 4:1 | *starved* | **1.000** | 1.000 | 1.000 | 1.000 |
| **field** | **52:1** | 1.000 | **4.321** | 10.803 | 12.752 | **13.013** |
| Lobodice | 4.5:1 | 1.000 | 1.000 | 1.058 | 1.118 | 1.125 |

**F1 passes at the field condition.** The ratio is 4.32 at the reservoir-
plausible gas volume and rises to 12.75 — far above the factor-of-2 bar F1
sets. **H1a survives, as a mass-balance/extent claim.**

At V_gas = 0.1 the mesocosm case reports *sulfate alone exhausts the H2* and
refuses to emit a ratio, rather than printing the spurious 1.0 that the
arithmetic would otherwise give — the same fail-closed discipline used
elsewhere in this study.

### What this says, and where H1a is still false

The mesocosm feed sits **at** stoichiometry, so its co-injected CO2 covers the
H2 it arrives with and calcite changes nothing: ratio 1.000 at every gas
volume. The field feed carries **13 times more H2 than its own CO2 can
convert**, so carbon runs out first and calcite is what relieves it. Lobodice,
at 12 % CO2, is CO2-rich and calcite is again nearly irrelevant.

So **H1a is not universally true, and this test says precisely where it is
false**: at a stoichiometric or CO2-rich feed. It survives only for a CO2-poor
feed — which is what the field trial actually was. That is a sharper and more
falsifiable hypothesis than the one pre-registered, and it was reachable only
by correcting the model rather than accepting its verdict.

The asymptotic factor of 13 is what the reported gas compositions give. It is
**not** offered as a reproduction of the "~30×" of section 1, which remains
absent from the source and is still not carried.

### What remains open, and is not claimed

1. **This is the infinite-horizon bound.** Whether a real storage reaches it
   within a given horizon is kinetic, and is the next increment. The finite-
   horizon integration is not run here and no timescale is claimed.
2. **Calcite is assumed in excess.** The mineral mass is not modelled.
   `abiotic_kinetics.sio` already measured dissolution as fast relative to this
   clock, which supports the assumption but does not establish the inventory.
3. **F2 is untested.** Whether this model's band encompasses the 84.3 %
   recovery at Sun Storage and the 54 %→37 % drop at Lobodice is a separate
   criterion, and nothing here should be read as bearing on it.

---

## Finite-horizon integration, 2026-09-03: the kinetics decide, and the two available sources disagree by 2.8e5

`sio/coupled_gasphase.sio` gave the extent bound — exact, stoichiometric,
infinite-horizon. `sio/coupled_finite.sio` asks whether a real storage reaches
it inside a storage horizon. The answer turns on a choice of kinetic source,
and that choice matters far more than any chemistry in this study.

### The two sources are not describing the same process

| source | growth rate | doubling time | what it is |
|---|---|---|---|
| USGS Aux Vases archive | µ = yield × k = 3.60e-11 /s | **610 years** | geological, 12 700-yr reservoir model |
| Strobel et al. 2023, sp. 2 (37 °C) | µmax = 1.0e-5 /s | **19.25 hours** | engineered methanation, batch reactor |

**Ratio: 2.78e5.** Both are cited in this literature for "microbial H2 loss in
underground storage." Neither is wrong. They are not the same process, and
substituting one for the other changes a finite-horizon prediction by five
orders of magnitude — more than every parameter uncertainty quantified
elsewhere in this study combined.

### With the archive's kinetics, nothing happens at operational timescales

Field condition, V_gas = 1 L/kg, cold-start biomass, archive parameters:

| horizon | H2 recovery |
|---|---|
| 285 d | 99.999999998 % |
| 100 yr | 99.9999996 % |
| 1 000 yr | 99.99979 % |
| 12 700 yr | 87.12 % |

To match the observed Sun Storage loss of 15.7 % at 285 days, this parameter
set is short by roughly **ten orders of magnitude**. This is worth stating
plainly: **the only fully self-consistent microbial kinetic parameter set
available to this study cannot reproduce a field observation at operational
timescales.** It was built for, and validated against, a 12 700-year model.

The integration uses the archive's set rather than Strobel's because Strobel
states biomass in *cells* and yield in cells/mol; converting to this study's
mg/kg and molal requires a mass per cell that Strobel does not supply, and
inventing one is barred. Strobel therefore enters only through two unit-free
quantities: the growth-rate ratio above, and the washout threshold below.

### The integrator reproduces the analytic extent bound

At 1e-4 mg/kg biomass (the archive's own upper sensitivity bound) over
12 700 yr, the field condition reaches its bound and gives **F1 = 4.320759**,
against the analytic extent bound's **4.320784** — agreement to 6e-6 relative,
consistent with Euler truncation. The bound is confirmed by an independent
route, not merely restated.

But at cold-start biomass, even 12 700 years is not enough to get there:
3.889e-2 molal consumed against a no-calcite cap of 6.99e-2, so carbon never
becomes limiting and **F1 = 1.000 at every horizon**. H1a's mechanism is real
and large, and it is *not always reached*. Whether it operates is a question
about biomass and time, not only about chemistry.

### Strobel's decay term adds a washout threshold the archive lacks

Strobel gives a decay coefficient b = 3e-7 /s; the archived scripts give none.
With decay, a population sustains itself only while

    µmax · MonodH2 · MonodCO2 > b   →   Monod product > b/µmax = 0.030

With the H2 term near 1, that requires dissolved CO2 above **3.40e-7 molal**.
Below it methanogens **die off** rather than merely slowing.

This is the one place the CO2 half-saturation constant finally matters. Section
3 above measured it as worth 0.055 % on *rate* at high CO2, and that stands.
Near exhaustion it decides *survival* — and sustaining CO2 above that floor is
exactly what calcite resupply would do. The structural role of calcite in H1a
is therefore sharper than a carbon budget: it is the difference between a
throttled population and a dead one.

### F2 becomes checkable — and the answer cuts against H1a at this site

The no-calcite branch carries a **hard cap** on H2 loss that no kinetics can
exceed, because carbon runs out. The with-calcite branch has **no cap**: given
time, all H2 goes. Field condition, swept over gas volume:

| V_gas (L/kg) | max loss, no calcite | **min recovery, no calcite** | min recovery, with calcite |
|---|---|---|---|
| 0.1 | 100 % | 0 % | 0 % |
| 1.0 | 23.14 % | **76.86 %** | 0 % |
| 10 | 9.26 % | **90.74 %** | 0 % |
| 100 | 7.84 % | **92.16 %** | 0 % |

**The observed 84.3 % recovery falls inside the no-calcite branch's reachable
range**, between V_gas = 1 and 10 L/kg — both physically ordinary gas
saturations. Nothing was tuned to put it there; the sweep was fixed before the
comparison, and V_gas remains a declared swept input.

For the with-calcite branch, the bound is 0 % recovery at every gas volume, so
reproducing 84.3 % requires kinetics stopped at exactly 15.7 % of completion —
possible, but it is a coincidence the no-calcite branch does not need.

**So: H1a's mechanism is real and large in extent (F1 = 4.3 to 13), but the Sun
Storage observation does not require it.** The field number sits comfortably
where carbon limitation *without* mineral resupply puts it. That is evidence
about this site, not about the mechanism's existence — and it is the opposite
of what H1a would have predicted for it.

### Not claimed

1. **F2 is not decided.** It requires *both* field points, and the Lobodice
   54 %→37 % composition change is not computed here — it is a change in gas
   composition under a shrinking mole count (4 H2 + CO2 → CH4), not a recovery
   fraction, and needs its own treatment.
2. **The carbon gas/aqueous partition is linearised** at its initial DIC/P_CO2
   ratio rather than re-solving the carbonate closure each step, which would
   cost ~1e9 speciation calls per run. The linearisation makes carbon look
   slightly *more* available than it is near exhaustion, so it is conservative
   against H1a rather than favourable to it.
3. **No kinetic parameter set appropriate to a 285-day reservoir observation
   exists in this study's sources.** The archive is 1e10 too slow; Strobel is a
   batch reactor with units that cannot be converted without inventing a
   number. That gap is now the single largest obstacle to a quantitative F2
   verdict, and it is a data gap, not a modelling one.

---

## F2, 2026-09-03: one field point survives, the other is not evaluable

F2 requires the coupled band to encompass **both** field points, with no
parameter tuned to fit. Both have now been tested.

### Sun Storage — encompassed, nothing tuned

Reported in the finite-horizon section above. The no-calcite branch carries a
hard stoichiometric cap on H2 loss; minimum recovery runs 0 % at V_gas = 0.1
L/kg, 76.86 % at 1, 90.74 % at 10, 92.16 % at 100. **The observed 84.3 % falls
inside that range**, between V_gas = 1 and 10 L/kg, both ordinary gas
saturations. The sweep was fixed before the comparison and V_gas remains a
declared swept input.

### Lobodice — the data do not close, and the probe refuses

`sio/lobodice_massbalance.sio`, recorded in full as `CORRECTIONS.md` C13.

Methanation consumes five moles of gas and returns one, so mole fractions move
both because a species is consumed and because the denominator shrinks: a drop
from 54 % to 37 % H2 is *not* a 31 % hydrogen loss and cannot be compared to a
recovery fraction without closing the balance. Closing it requires an
instrument, and **N2 is one** — it takes no part in any reaction here, so its
mole fraction measures nothing but the change in total moles.

Four checks fail on the reported set alone:

| check | result |
|---|---|
| compositions sum to 100 %? | **99.5 %** and **98.0 %**, no convention stated |
| N2 tracer vs stated volume loss | tracer **72.6 %**, source **10–20 %** — off **3.6× to 7.3×** |
| did methane form? | CH4 moles **fall 49.5 %**, against a measured microbial isotopic signature for methane *formation* |
| stoichiometric closure | predicts a final CH4 mole fraction of **107.2 %** — impossible |

**The source's own model corroborates the discrepancy**: Tremosa et al. simulate
Lobodice and get a volume decrease of 46 %, far above the stated 10–20 % and
much nearer the tracer inference. On two independent routes the 10–20 % figure
is the outlier.

The primary sources (Smigan 1990, Buzek 1994) were never obtained, so the
failure cannot be attributed to measurement, transmission, or an unstated
convention. Selecting whichever subset of the numbers would let a band be drawn
around 37 % is fitting to a target, which the protocol forbids. **The probe
refuses rather than choosing, and F2 is declared not evaluable at Lobodice.**

### F2 verdict, reported both ways as C12 requires

- **Against both field points as pre-registered: F2 CANNOT BE EVALUATED.** One
  of its two required points rests on data that fail internal consistency.
- **Against Sun Storage alone: F2 is SATISFIED.** The observation lies inside
  the model's reachable range with nothing tuned to place it there.

This is a materially weaker validation base than the mission assumed — one clean
field constraint, not two — and C12 recorded that risk before this measurement
was made, rather than after.

### Lobodice could not have discriminated H1a even with clean data

At its composition the F1 asymptote `y_H2/(4·y_CO2)` is **1.125**: the town gas
carries very nearly its own stoichiometric CO2, so calcite changes almost
nothing there. 37 % H2 is a value **both** branches pass through on the way down.
The site is CO2-rich, which is exactly where this study has now shown the
mechanism to be inert — so the pre-registered choice of Lobodice as a
discriminating field point was, in hindsight, poorly matched to H1a regardless
of data quality. That is worth stating plainly: the criterion was not defeated
only by bad data, it was also aimed at the wrong kind of site.
