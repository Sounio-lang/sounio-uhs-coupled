# Coupled abiotic + microbial model of H₂ loss in underground storage

A pre-registered attempt to **falsify** a hypothesis about what controls
hydrogen loss in underground hydrogen storage — not to confirm it.

**Status: Phases 0-4 complete. All three pre-registered criteria have
verdicts, in [`RESULTS.md`](RESULTS.md), which opens with them.**
`PHASE4_GATE.md` is the running log behind it, written as a log rather than a
summary: it contains a wrong answer, the defect that produced it, and the
correction, in that order.

| criterion | verdict | where |
|---|---|---|
| **F1** — remove calcite; under a factor 2 and H1a dies | **passes at a CO2-poor feed** (4.3x at ordinary gas saturation, approaching 13x), **but is inert at a stoichiometric or CO2-rich feed** (1.00 to 1.13) | `sio/coupled_gasphase.sio` |
| **F2** — band must encompass both field points, nothing tuned | **not evaluable as pre-registered.** Lobodice's reported data fail four independent consistency checks (`CORRECTIONS.md` C13). **Satisfied against Sun Storage alone**, which falls inside the model's reachable range with nothing tuned | `sio/coupled_finite.sio`, `sio/lobodice_massbalance.sio` |
| **F3** — if the abiotic band alone encompasses both points, the coupling is superfluous | **not evaluable as pre-registered, and cannot be settled against Sun Storage alone.** A reaction-free simulation spans the observation; the dominant physical term is unmeasured in the field and spans a factor of 20 in the laboratory. But methanogenesis at the site is directly measured, and no physical process explains an isotopic signature | `sio/f3_abiotic_band.sio`, `sio/field_mass_balance.sio` |

**H1a survives, in a sharper and more falsifiable form than it was
pre-registered in.** The mechanism is not kinetic — the CO2 half-saturation term
is worth 0.055 % — but stoichiometric: the ratio of biotic H2 loss with calcite
to without approaches `y_H2 / (4 * y_CO2)`, the excess of stored hydrogen over
the CO2 stored with it. That is 13.0 for the 52:1 field gas and 1.0 for the 4:1
mesocosm feed, so the hypothesis is **true only for a CO2-poor feed** and this
study says where it is false.

**Two things are open, and both are data gaps rather than modelling ones.** No
microbial kinetic parameter set appropriate to a 285-day reservoir observation
exists in these sources — the available one is ~10 orders of magnitude too slow,
and the fast one is stated in units that cannot be converted without inventing a
mass per cell. And three of the four abiotic channels in F3 have no measured
rate, which is why that module refuses to emit a total band rather than
silently treating them as zero.

The engine is [Sounio](https://github.com/sounio-lang/sounio), a self-hosted
systems and scientific language. Harnesses are C++. Python appears only as an
oracle binding or as data marshalling, never as computation of our own.

## The contract

Every number in this repository carries **the command that produced it and the
commit it ran at**. `audit_provenance.py` checks that mechanically, three ways:
OK / INHERIT / FAIL — where INHERIT is a judgement left to a reader, not a pass.
`verify_snapshot.py` checks the things that rot silently in an archive: a missing
file, a path that only resolves in the upstream layout, a constant aligned on one
side of a comparison and not the other.

Further rules, inherited from the GRI-Mech cross-validation that preceded this
work:

- **The oracle is the oracle.** A replica is a diagnostic and never supplies a
  reference value.
- **A correction is never an edit in place.** It becomes a new release with a new
  DOI.
- **A reconstruction declares itself** in its own file header, with a date.
- **Probes fail closed on provenance.** If the provenance of the initial state
  cannot be established, the probe refuses rather than printing a plausible
  number.
- **A run that fails is reported as a failure.** No number is invented,
  interpolated, or inherited — including from our own earlier work.

## Layout

| path | what it is |
|---|---|
| `PREREGISTRATION.md` | hypotheses and falsification criteria, **frozen before any run** |
| `FEATURES.md` | Phase 0 — language feature maturity, measured |
| `LANGUAGE_GAPS.md` | what this model forced the language to need |
| `data/` | manifest: origin, DOI, licence, sha256, convention axioms per file |
| `abiotic/` `microbial/` `coupled/` | the models |
| `oracles/` | IPhreeqc parity harness; archived USGS outputs |
| `RESULTS.md` | **the verdicts**, on the first line, with the producer of every number |
| `PHASE4_GATE.md` | the running log behind them — including a retracted result and the defect that caused it |
| `CORRECTIONS.md` | 13 corrections to the brief's premises and to the sources, each with the measurement that exposed it |
| `sio/` | the Sounio engine sources -- validation and gate probes live here too, not in a separate `probes/` directory, because imports resolve same-directory first and the self-hosted stdlib fallback does not see this repo |
| `tools/` | data → `.sio` code generation (marshalling only, no numerics) |

## Compiler provenance

Measured against `Sounio-lang/sounio` at `origin/main` `57f87da54f`.

**This contradicts the upstream `CLAUDE.md`, which directs work at
`integration/sounio-dev-ready-base`, and the deviation is deliberate.** The two
branches have diverged (424 commits on one side, 211 on the other), and five
files of the EL+ ontology stack — `elplus.sio`, `evolve.sio`, `repair.sio`,
`closure.sio`, `temporal.sio` — **exist only on `main`**. The ontology layer this
study depends on is not present on the branch the instruction names. `units.sio`
is byte-identical on both.

## Why a separate repository

A path cited into a monorepo of several thousand commits with dozens of open pull
requests decays within days: files move, results are superseded, the prebuilt
compiler drifts behind its own source. A reproduction path that points into a
moving tree is not a reproduction path. Corrections happen upstream; if they
change a result here, that becomes a new release.

## Licence

Apache-2.0. Data files carry their own licences, recorded per file in
`data/MANIFEST.tsv` — the USGS model archive is CC0-1.0.
