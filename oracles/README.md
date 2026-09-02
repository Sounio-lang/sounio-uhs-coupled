# Oracles

**The oracle is the oracle.** Nothing in this directory is a replica of the
Sounio model, and nothing here may take a reference value from it. The direction
of trust is one way.

## Oracle 1 — IPhreeqc

IPhreeqc **3.8.6-17100**, USGS, built from source on Linux x86-64 with
`g++ (Ubuntu 13.3.0)`, `./configure && make -j16`.

| | |
|---|---|
| source tarball | `iphreeqc-3.8.6-17100.tar.gz`, sha256 `f5babcc9fb0c252e0c432786bbe9c265f02ca32791b096106f360cf1df1d9d94` |
| database | `database/phreeqc.dat`, sha256 `59373961d648dfbf68a40744060c1d64f57ecbec98f4f5fb89f3a1b4213ccd10` |

Build note: `PHRQ_exports.h` is **not installed** by `make install`; the compile
needs `-I<src>/phreeqcpp/common -I<src>` in addition to the install include dir.

### The database is pinned by content, not by path

**Three different files named `phreeqc.dat` ship in the IPhreeqc distribution**,
with different sizes and different hashes. `iphreeqc_calcite.cpp` therefore
requires `--db-sha256` and refuses to run on a mismatch. Demonstrated:

```sh
# a deliberately wrong digest
./iphreeqc_calcite --db <canonical> --db-sha256 000...0 --temp-c 40
#   FAIL: database hash mismatch ...  expected 000...0  got 59373961...

# the canonical digest against the OTHER phreeqc.dat in the tree
./iphreeqc_calcite --db examples/cpp/advect/phreeqc.dat --db-sha256 59373961... --temp-c 40
#   FAIL: database hash mismatch ...  got 317344c381a88ac66154367e520747cd2a698a492c797bb8e1dba1aec6ccab4d
```

Both refused. The second is the one that matters: the wrong file is a real file
that would have run and produced plausible numbers.

### Validated against an independent known value

Calcite-saturated pure water, closed to CO₂, 25 °C:

```sh
./iphreeqc_calcite --db <canonical> --db-sha256 59373961... --temp-c 25 --header
```

| quantity | oracle | textbook |
|---|---|---|
| pH | **9.90652** | ~9.9 |
| Ca | **1.2294e-04 mol/kgw** | ~1.2e-4 |
| C | 1.2294e-04 | equal to Ca, by CaCO₃ stoichiometry |
| SI calcite | 0.0000 | 0 at saturation |

At 40 °C it gives pH 9.53933 and Ca 1.3254e-04 — calcite is *less* soluble than
at 25 °C in the sense the retrograde solubility of CaCO₃ requires. Direction and
magnitude both check.

## The redox convention dominates the abiotic result, and it is not settled

`phreeqc.dat` defines `H(0)` with master species `H2` and `H(1)` with `H+`, so
the two are **coupled through the electron**. Imposing H₂(g) at reservoir
fugacity as an equilibrium phase therefore pins pe at the bottom of its range and
reduces the water itself.

Measured, 40 °C, 1 molal NaCl:

| formulation | pH | Ca (mol/kgw) | calcite dissolved (mol) |
|---|---|---|---|
| redox coupled, H₂(g) fugacity 91 | 13.79 | 2.3922 | **2.5000** |
| H(0) fixed at 0.05 molal | 11.40 | 6.2634e-03 | 6.2642e-03 |
| no H₂ at all | 9.647 | 3.7830e-04 | 3.7830e-04 |

**A factor of ~400 between the first two rows, and ~6600 between the first and
the third, from a convention choice alone.** The coupled row is also
suspicious on its own terms: Ca comes out at 2.3922 mol/kgw *identically* at 0, 1
and 3 molal NaCl, which is a saturation artefact rather than chemistry.

This reproduces the failure mode the calibrated-model literature describes —
that PHREEQC defaults with Van't Hoff over-predict calcite dissolution, hydrogen
consumption and methane production. It is an artefact of the redox convention,
not a property of the rock.

**What is NOT yet achieved, stated plainly.** Entering `H(0)` as a fixed molality
does **not** actually decouple the couple: the run above was given 0.05 molal and
returned **7.3073e-07**, so pe re-equilibrated and most of the H₂ reacted.
Genuine decoupling needs the H(0)/H(1) link removed at the database level, the
way the archived USGS model does with `decouple ALL`. Until that is done, no
abiotic H₂-loss number from this harness is quotable, and none is quoted.

## Status

| | |
|---|---|
| oracle builds and runs | yes |
| pinned by content, fail-closed | yes, demonstrated by negative control |
| validated against an independent value | yes, calcite-saturated water at 25 and 40 °C |
| redox decoupling | **not achieved** — next step, database-level |
| parity against the Sounio model | not started; the Sounio model does not exist yet |
| oracle resolution in regime, over 10 perturbed states | not started |
| step bisection | not started |
