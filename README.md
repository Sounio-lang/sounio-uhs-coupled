# Coupled abiotic + microbial model of H₂ loss in underground storage

A pre-registered attempt to **falsify** a hypothesis about what controls
hydrogen loss in underground hydrogen storage — not to confirm it.

**Status: Phases 0-4 complete, and F1 settled on 2026-10-04 (`RESULTS.md` §2d).
[`RESULTS.md`](RESULTS.md) opens with the verdicts.** `PHASE4_GATE.md` is the
running log behind it, written as a log rather than a summary: it contains a
wrong answer, the defect that produced it, and the correction, in that order.

> **This table was stale until 2026-10-04.** It still read "F1 passes at a
> CO2-poor feed" after `RESULTS.md` §2b–2c had revised that to "not settled", and
> it called the mechanism a sharper form of H1a after `CORRECTIONS.md` C22 had
> found it is prior art. Both are corrected below (`CORRECTIONS.md` C23).

| criterion | verdict | where |
|---|---|---|
| **F1** — remove calcite; under a factor 2 and H1a dies | **fails.** For the field gas (0.19 % CO2), with H2's self-limiting loop honoured and the gas CO2 buffering it: **1.033** at ordinary gas saturation, **at most 1.21** across calcium 1e-5 to 0.3 molal. Pure hydrogen storage: 1.015. The 4.3–13 once reported here was a fixed-ceiling upper bound | `sio/partial_buffer.sio`, `sio/pure_h2.sio`, `sio/h2_selflimiting.sio` |
| **F2** — band must encompass both field points, nothing tuned | **not evaluable as pre-registered.** Lobodice's reported data fail four independent consistency checks (`CORRECTIONS.md` C13). **Satisfied against Sun Storage alone**, which falls inside the model's reachable range with nothing tuned | `sio/coupled_finite.sio`, `sio/lobodice_massbalance.sio` |
| **F3** — if the abiotic band alone encompasses both points, the coupling is superfluous | **not evaluable as pre-registered, and cannot be settled against Sun Storage alone.** A reaction-free simulation spans the observation; the dominant physical term is unmeasured in the field and spans a factor of 20 in the laboratory. But methanogenesis at the site is directly measured, and no physical process explains an isotopic signature | `sio/f3_abiotic_band.sio`, `sio/field_mass_balance.sio` |

**H1a does not survive F1.** Carbon limitation of hydrogenotrophic
methanogenesis, and calcite as a carbon source, are real and published
(`CORRECTIONS.md` C22). But in a closed vessel the methanogenesis they enable is
alkalinity-neutral, so pH rises and calcite turns from source to sink. With the
loop honoured, calcite's whole contribution is dissolved **before** the microbes
start (ψ_max is reached at zero carbon methanated at every gas volume and
calcium tested), and removing it changes biotic H2 loss by 3 %, not by a factor
of 2. The stoichiometric bound `y_H2 / (4 * y_CO2)` remains the right ceiling for
an *infinite* carbon buffer, and it is a formalisation of Hellerschmied et al.
2024's own 4:1 versus 52:1 remark, credited as such.

**What would overturn F1** is stated in `RESULTS.md` §2d: an open system with
CO2 resupplied from outside the store, or a field water chemistry far from the
Aux Vases proxy.

**Two things are open, and both are data gaps rather than modelling ones.** No
microbial kinetic parameter set appropriate to a 285-day reservoir observation
exists in these sources — the available one is ~10 orders of magnitude too slow,
and the fast one is stated in units that cannot be converted without inventing a
mass per cell. And three of the four abiotic channels in F3 have no measured
rate, which is why that module refuses to emit a total band rather than
silently treating them as zero.

**Reproduce everything:** `bash tools/verify.sh` (≈1.5 min). It uses the vendored
compiler `vendor/gen3.elf`, md5 `1aa4317f…`, which rebuilds bit-identically from
`Sounio-lang/sounio@654ba36260` with `make build` in about 8 s.

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
