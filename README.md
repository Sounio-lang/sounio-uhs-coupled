# Coupled abiotic + microbial model of H₂ loss in underground storage

A pre-registered attempt to **falsify** a hypothesis about what controls
hydrogen loss in underground hydrogen storage — not to confirm it.

**Status: Phase 0 complete. No scientific result exists yet.** `RESULTS.md` does
not appear in this tree until Phase 2 produces something to put in it. This
README, `PREREGISTRATION.md`, `FEATURES.md` and `LANGUAGE_GAPS.md` are the whole
of the current content, and that is deliberate.

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
| `probes/` | versioned, each named with the section it produces |
| `sio/` | the Sounio engine sources |
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
