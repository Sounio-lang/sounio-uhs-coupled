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
