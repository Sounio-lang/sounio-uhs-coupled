# Pre-registration

**Frozen 2026-09-02, before any model was run and before any dataset was
downloaded.** This file is not edited after its commit. If a hypothesis or a
criterion below turns out to be badly posed, the correction becomes a *new*
section in `RESULTS.md` naming this file and the defect — never an edit here.

The purpose of this study is to **try to kill H1**, not to confirm it. A result
that falsifies H1 is a result, and the deliverable in that case is the figure
that kills it.

---

## Scope correction made BEFORE freezing

The hypothesis as originally posed spoke of "a carbonate reservoir". Checking
the two field validation points first:

| site | lithology | note |
|---|---|---|
| Sun Storage (Hellerschmied et al. 2024) | **porous sandstone**, ~1000 m, Gampern, Upper Austria | depleted hydrocarbon reservoir |
| Lobodice | **sandstone aquifer**, 500 m, 25–45 °C | injected town gas already contained ~10% CO₂ + ~10% CO |
| Aux Vases (USGS model archive) | **sandstone**, Illinois Basin | the microbial model's formation |

None of the three is a carbonate reservoir. Freezing a hypothesis about carbonate
reservoirs whose only field tests are siliciclastic would put the field
validation out of domain by construction. The hypothesis is therefore split, and
the split is pre-registered rather than discovered later.

---

## Hypotheses

**H1a (testable against field data).** In a *siliciclastic* reservoir bearing
calcite as cement or accessory phase, calcite dissolution is the CO₂ source that
limits hydrogenotrophic methanogenesis, and therefore controls biotic H₂ loss.

**H1b (in silico only, declared extrapolation).** The same mechanism in a pure
carbonate reservoir. **No field point tests H1b.** Its verdict comes only from S1
and S5. Any statement about carbonate reservoirs in the final report is labelled
an extrapolation.

**H2.** The pH → calcite precipitation → CO₂ cut loop makes the system
self-limiting over part of the parameter space.

---

## Falsification criteria

**F1.** If removing calcite from the coupled model reduces biotic H₂ loss by less
than a factor of 2 (band included), H1a dies.

**F2.** If the coupled model band does NOT encompass 84.3% recovery at 285 d (Sun
Storage) and the 54% → 37% drop (Lobodice), the model is not validated, and the
report says so. **No parameter is tuned to make it fit.**

**F3.** If the abiotic band alone already encompasses both field points, the
coupling is unnecessary and H1a is superfluous.

---

## Pre-registered confounders

Recorded now so they cannot later be presented as discoveries.

1. **Lobodice CO₂ provenance.** The injected town gas carried ~10% CO₂ and ~10%
   CO. At that site the CO₂ available to methanogenesis did not have to come from
   calcite at all. Lobodice therefore constrains the coupled model weakly with
   respect to H1a, and this is stated wherever the Lobodice point is used.

2. **Temperature domain.** Both field sites sit well below 70 °C (Lobodice
   25–45 °C). Any claim imported from a study whose regime is above ~70 °C is an
   extrapolation into a colder regime and is labelled as such.

3. **Calcite as accessory phase.** Calcite cement abundance in these sandstones is
   itself uncertain, and is treated as an uncertain parameter, not a constant.

---

## Numbers that may NOT be inherited

Under the rule "no number is invented, interpolated, or inherited":

- A "4–13% Monte Carlo recovery" figure was attributed to prior GRI-Mech
  cross-validation work. Exhaustive search of that repository found zero
  occurrences of "Monte Carlo", "4–13", "sampling" or "coverage". **It is not
  carried forward.** Either it is re-derived within this study, with its own
  producer, or it does not appear.

- The reference-pressure "1 bar vs 1 atm" defect is **not** a real defect of
  GRI-Mech 3.0: that work measured both sides already at 101325 Pa and recorded
  "no defect present". It is reused here only as a **synthetic counterfactual**
  for the ontology negative control, and is labelled as such wherever it appears.

---

## Analysis commitments

- Oracle resolution is measured **in regime**, over an ensemble of 10 perturbed
  states, and reported as an **interval**, never as a point.
- No number is reported to more significant digits than the oracle floor allows,
  regardless of internal precision.
- Step bisection is run in every regime; invariance is reported whether or not it
  holds.
- A run that fails is reported as a failure. A dataset that is inaccessible is
  reported as inaccessible. Nothing is synthesised.
- Probes fail closed on the provenance of the initial state: if provenance cannot
  be established, the probe refuses rather than printing a plausible number.
- The oracle is the oracle; a replica is a diagnostic and never supplies a
  reference value.

---

## Verdict line

`RESULTS.md` opens with the verdict on F1 / F2 / F3. If H1a is falsified, the
first line says "H1a falsified".
