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

---

## Redox decoupling: achieved, and the 400× was entirely the convention

The earlier attempt — fixing `H(0)` as a molality — did **not** decouple: 0.05 molal
in returned 7.3073e-07 out, so pe re-equilibrated and the H₂ reacted away. That is
now fixed.

**How.** The species is made a **separate element**, which is exactly how PHREEQC's
own `Amm.dat` decouples ammonia from nitrogen redox (`Amm  AmmH+  0  AmmH  17.031`).
`Hdg` is dissolved hydrogen gas carrying **the Henry constants `phreeqc.dat` gives
`H2(g)`** — `log_k -3.105`, `delta_h -4.184 kJ`, and the same `-analytic` row — so
its solubility is identical, but no electron appears in its reaction and it cannot
drive pe.

**The pinned database is not modified.** The definition is part of the run's own
input, emitted before `SOLUTION`, so it appears in the record and the hash gate
still holds.

### Measured, 40 °C, 1 molal NaCl, f(H₂) = 91

| formulation | pH | Ca (mol/kgw) | H₂ in solution (mol/kgw) | calcite dissolved (mol) |
|---|---|---|---|---|
| redox coupled | 13.7866 | 2.3922 | 9.9300e-05 | 2.5000 |
| **redox decoupled** | **9.6474** | **3.7824e-04** | **5.3858e-02** | **3.7824e-04** |
| decoupled, no H₂ at all | 9.64715 | 3.7830e-04 | 0 | 3.7830e-04 |

Two things follow, and they are the point of the exercise.

**1. Decoupled H₂ dissolves and stays.** 5.3858e-02 mol/kgw remains in solution,
against 9.93e-05 in the coupled case where it had reacted away. That residual is
Henry's law, and it is the abiotic loss mechanism.

**2. With H₂ inert, calcite dissolution is unchanged.** 3.7824e-04 mol with H₂
against 3.7830e-04 without — a difference of 6e-9 mol, **0.016 %**, and pH moves
from 9.64715 to 9.6474.

**So the factor of ~400 reported earlier was entirely an artefact of the redox
convention, not a property of the rock.** Under the physically appropriate
treatment for 40 °C without a catalyst, hydrogen does not measurably attack
calcite in this system. That is consistent with the direction of the calibrated-
model literature, which reports overall hydrogen loss to brine dissolution as far
below 1 % molar.

**What this does and does not license.** It licenses reporting equilibrium calcite
dissolution under decoupled H₂. It does **not** yet license an abiotic H₂-loss
percentage for a reservoir: that requires the gas-to-water ratio of the site, and
Lehen's salinity is not reported at all (see `CORRECTIONS.md` C6). Nor is this a
kinetic result — it is equilibrium. The Palandri–Kharaka rate law is not yet wired
in, so nothing here speaks to how fast.

---

## Oracle 2 — Chabab correlation, and where it disagrees with IPhreeqc

`oracles/chabab_solubility.cpp` evaluates Chabab et al.'s Model 2 (Duan-type /
Pitzer) for H₂ solubility in NaCl brine. It is an **independent** check, not a
replica: PHREEQC combines a Henry's law constant with a Debye–Hückel activity
model, while this is a virial correlation fitted directly to H₂–brine solubility
measurements. Agreement corroborates both; disagreement is a finding either way.

Coefficients are Table 4 of the accepted manuscript, read from the PDF and
verified (see `data/README.md`). The fugacity coefficient cancels out of the
published equation once fugacity rather than partial pressure is imposed, so no
φ model is invented.

The program **refuses to extrapolate** outside the fitted range by default:

```sh
./chabab_solubility --temp-k 400 --pressure-bar 91 --nacl-molal 1 --fugacity-bar 91
#   FAIL: T = 400 K is outside the fitted range 298-373 K; pass
#         --allow-extrapolation to proceed and label the result
```

The range is the *measured* range, not an authors' statement — the paper gives no
validity envelope, and that is recorded rather than glossed.

### Dissolved H₂ at 40 °C, f(H₂) = 91 bar (mol/kg water)

| m_NaCl | IPhreeqc (decoupled) | Chabab | difference |
|---|---|---|---|
| 0 | 6.78160e-02 | 6.31345e-02 | **+7.42 %** |
| 1 | 5.38580e-02 | 5.10648e-02 | **+5.47 %** |
| 2 | 4.27810e-02 | 4.20922e-02 | **+1.64 %** |
| 3 | 3.39830e-02 | 3.53597e-02 | **−3.89 %** |

**The disagreement is systematic, not scatter.** It runs monotonically from
+7.4 % to −3.9 % and crosses zero near 2.2 molal, which means the two models
differ in the *salting-out slope* rather than in a constant offset:

| salting-out ratio m(0 molal) / m(3 molal) | |
|---|---|
| IPhreeqc | **1.9956** |
| Chabab | **1.7855** |

PHREEQC salts H₂ out about **12 % more strongly** than the correlation fitted to
H₂–brine data. That direction is what one would expect: a Debye–Hückel/Davies
activity model is not fitted to the salting-out of a neutral dissolved gas,
whereas Chabab's virial terms are.

**Why this matters here, concretely.** Lehen's salinity was never measured or
reported (`CORRECTIONS.md` C6), so salinity enters the model as an uncertain
parameter — and across the plausible range the two oracles disagree by up to
7 % in a salinity-dependent way. That is a contribution to the uncertainty band
which is now **measured**, not assumed, and it cannot be reduced by choosing one
oracle: choosing is the assumption.

At a single point the agreement (5.5 % at 1 molal, against Chabab's own reported
AAD of 3.13 %) would have looked like corroboration and nothing more. Sweeping
the parameter is what exposed the structure.

---

## The oracle's resolution, measured in regime

A residual has no meaning until it is compared against the resolution of the
instrument that produced it, and that resolution is not a constant. It is
measured here at the setting actually used, over an ensemble of **10 states each
perturbed by ±1 ppm** in temperature, salinity and imposed fugacity, and reported
as an **interval**.

```sh
./iphreeqc_calcite --db <pinned> --db-sha256 59373961... \
    --temp-c 40 --nacl-molal 1 --decouple-redox --h2-fugacity 91 --resolution-probe
```

Dissolved H₂, 40 °C, f(H₂) = 91 bar:

| m_NaCl | nominal (mol/kgw) | spread | relative |
|---|---|---|---|
| 0 | 0.0678158708318 | 1.35632e-07 | **2.00e-06** |
| 1 | 0.0538579539494 | 1.14133e-07 | **2.11915e-06** |
| 2 | 0.0427807445777 | 1.00507e-07 | **2.34936e-06** |
| 3 | 0.0339826351945 | 8.76603e-08 | **2.57956e-06** |

**The resolution is not constant — it varies by 29 % across these four regimes**,
rising monotonically with salinity. A single measurement, reused, would have
misstated the instrument at three of the four points. This is why it is measured
每 regime rather than once.

### The first attempt measured the formatter, not the oracle

Run without `-high_precision`, the probe returned a spread of **exactly zero** at
every regime. That was not a result: PHREEQC's selected output carries six
significant digits by default, so a 1 ppm perturbation falls in the seventh and
is invisible. The measurement was of the print, not the quantity.

That error — a number's print resolution taken for the resolution of the thing
it names — is on record in the predecessor GRI-Mech cross-validation as the cause
of a misdiagnosed disagreement. It is recorded again here because it recurred,
in a different code, on the first attempt, and only a spread of *exactly* zero
gave it away.

### What this licenses

The Sounio engine's residual against this oracle, per `sio/h2_solubility.sio`:

| m_NaCl | residual | oracle resolution | ratio |
|---|---|---|---|
| 0 | +7.42 % | 2.00e-06 | **37 100×** |
| 1 | +5.47 % | 2.12e-06 | **25 800×** |
| 2 | +1.64 % | 2.35e-06 | **6 980×** |
| 3 | −3.89 % | 2.58e-06 | **15 100×** |

Every residual stands four orders of magnitude above the instrument, so the
disagreement between the engine's correlation and the oracle's physics is
**established as real, not attributed to it**. It is a difference between two
solubility models — one virial, fitted to H₂–brine data; one Henry's law with a
Debye–Hückel activity term — and not arithmetic, not implementation, and not
noise.

Had the residual come out near 2e-06, nothing could have been claimed either way,
and this is the measurement that would have said so.
