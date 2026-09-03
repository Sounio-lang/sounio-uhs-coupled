# The engine

Sounio modules. These are the model. They are neither oracles nor replicas, and
they never take a reference value from either.

Compiled with `gen3.elf` built from the qd128-transcendentals branch, **not**
through `bin/souc` — see `LANGUAGE_GAPS.md` G8 for why that distinction is not
pedantry.

| module | what it is | status |
|---|---|---|
| `h2_solubility.sio` | Chabab Model 2, H₂ in NaCl brine | matches its C++ replica to all printed digits; residual against the IPhreeqc oracle measured and judged against the oracle's resolution |
| `calcite_rate.sio` | Palandri–Kharaka Table 33 dissolution rate | k₂₅ reproduced from the table; affinity term and sentinels verified |

Every negative f64 constant is a module-level `var` or is assigned inside a
function. **None is a module-level `let`**, because that form is silently wrong
on this compiler (G9), and every `log10 k` in the rate law is negative.

## `calcite_rate.sio`

```
rate = SUM_i k_i(T) * a_j^n_i * (1 - Omega^p)^q      [mol m^-2 s^-1]
k_i(T) = k_i(25C) * exp( -Ea_i/R * (1/T - 1/298.15) )
```

Table 33 gives log₁₀k at 25 °C in mol m⁻² s⁻¹ and Ea in kJ/mol, so the tabulated
constant is k₂₅ and the Arrhenius factor is the conversion from it.

**The third carbonate mechanism is not the base mechanism.** For every other
mineral class in the report the columns are acid/neutral/**base** with the third
order in H⁺ (footnote c); for carbonates the third is the **carbonate**
mechanism with its order in **P(CO₂)** (footnote d). A port reusing the
base-mechanism form would be wrong in the variable, not merely the number — and
that term is where H1a lives, since it couples calcite to CO₂ in the rate law
itself rather than only through the carbonate equilibria.

### Verified

`k₂₅` recovered from the Arrhenius conversion at T = 298.15 K against the table:

| mechanism | computed | 10^(log₁₀k) |
|---|---|---|
| acid | 5.011872336272722e-01 | 10^−0.30, exact |
| neutral | 1.548816618912463e-06 | 10^−5.81, agrees to 14 digits |
| carbonate | 3.311311214825906e-04 | 10^−3.48, agrees to 15 digits |

The residual is f64 rounding through `exp(log₁₀k · ln10)`, ~1e-14 relative.

Affinity factor at p = q = 1: Ω = 0 → 1.0; Ω = 0.5 → 0.5; **Ω = 1 → 0** (rate
vanishes at equilibrium); Ω = 2 → **−1.0**, the sign carrying precipitation. The
supersaturated branch restores the sign explicitly rather than raising a negative
base to a real power, because guessing there would silently turn precipitation
into dissolution.

Neutral-mechanism rate constant against temperature: 1.5488e-06 at 25 °C,
**2.4390e-06 at 40 °C** (Lehen), 5.3691e-06 at 70 °C, 1.5305e-05 at 120 °C.

### The affinity exponents are an uncertain parameter, not a constant

Table 33 gives no p or q for calcite. Footnote e says they "default to unity if
not specified", and the report does not treat that as harmless — it says the
unit values are "assumed to be equal to unity, **which is likely incorrect**",
and that

> *"uncertainties in values for pᵢ and qᵢ are currently the source of much
> uncertainty in the length of time computed in models of water-rock
> interaction."*

The nearest carbonate with **measured** values is magnesite, at **p = 4.00**.
Measured here, at Ω = 0.9 — near equilibrium, where a reservoir sits:

| p | affinity factor |
|---|---|
| 1 (the default) | 0.1000 |
| 4 (magnesite, measured) | 0.3439 |

**A factor of 3.44 in the rate, from a parameter the table does not supply.** The
gap widens as equilibrium is approached. `set_affinity(p, q)` exists so the
sensitivity analysis can sweep it; it is not a knob to be tuned to fit anything.

## `carbonate_equilibria.sio`

The pH ↔ CO₂ loop, which is **H2**. Nothing here asserts the hypothesis; it makes
it computable.

All five constants come from the pinned `phreeqc.dat` (sha256 `59373961…`), in
PHREEQC's analytic form

```
log10 K(T) = A1 + A2*T + A3/T + A4*log10(T) + A5/T^2 + A6*T^2      (T in K)
```

**A declared tradeoff.** Sharing the thermodynamic database with the oracle means
a parity test against IPhreeqc isolates the **solver**, not the data — and says
nothing about whether the data are right. Measuring the data contribution means
swapping in `llnl.dat`, which is a separate experiment, not a variant of this one.

### Self-check: the analytic form against the database's own log_k at 298.15 K

| reaction | tabulated | computed | residual |
|---|---|---|---|
| CO₂(g) Henry | −1.468 | −1.468166248870 | −1.66e-04 |
| CO₃²⁻ + H⁺ | 10.329 | 10.32885437850 | −1.46e-04 |
| CO₃²⁻ + 2H⁺ | 16.681 | 16.68071852866 | −2.81e-04 |
| H₂O = OH⁻ + H⁺ | *(see below)* | −13.99475154220 | +5.25e-03 |
| calcite Ksp | −8.48 | −8.479964655643 | +3.53e-05 |

Residuals of order 1e-4 in log₁₀ units are **fit residuals, not errors**: `-log_k`
is the value at 25 °C while `-analytic` is a separate least-squares fit over a
temperature range, and the fit need not pass exactly through that point. Had the
form been misread, the residuals would be orders of magnitude larger. This is the
check that the reading is right.

**The water row is not like the others.** `phreeqc.dat`'s `H2O = OH- + H+` block
carries **no `-log_k` line at all** — only the analytic expression. The −14.0 it
is compared against is a textbook round number **supplied here, not by the
database**, so that residual measures an assumption rather than a fit. What the
database actually gives at 25 °C is **−13.99475**.

### Derived

| | computed | textbook |
|---|---|---|
| pK₁ (25 °C) | 6.351864150163 | 6.35 |
| pK₂ (25 °C) | 10.32885437850 | 10.33 |
| pK₁ (40 °C) | 6.297395048667 | |
| pK₂ (40 °C) | 10.22169431879 | |

### The H2 loop is visible

Calcite saturation at 40 °C, P(CO₂) = 0.01 atm, Ca = 1e-3 molal:

| pH | Ω |
|---|---|
| 7 | 2.965e-04 |
| 8 | 2.965e-02 |
| 9 | **2.965** |

Ω crosses 1 between pH 8 and 9: rising pH drives calcite from dissolving to
**precipitating**, which is exactly the leg of H2 that cuts the CO₂ supply. The
factor of 100 per pH unit is [CO₃²⁻] ∝ 1/[H⁺]², so the algebra is doing what it
should.

**What is not done here.** This speciates at a *given* pH. It does not solve for
pH — closing the system on charge balance is a separate step, and folding it in
silently would hide which constraint produced a number. Until that exists there
is no trajectory, and without a trajectory there is no step bisection and no
statement about truncation.

### Charge-balance closure, and the first engine↔oracle parity

The system closes on electroneutrality, `2[Ca²⁺] + [H⁺] = [HCO₃⁻] + 2[CO₃²⁻] + [OH⁻]`,
with calcite equilibrium supplying `[Ca²⁺] = Ksp/[CO₃²⁻]`. One equation, one
unknown, solved by **bisection** — chosen over Newton because it needs no
derivative, cannot diverge, and its error after n steps is exactly `(hi−lo)/2ⁿ`,
so convergence is arithmetic rather than hope. The iteration count and the
residual at the root are both reported: a root is exhibited, not claimed.

At 40 °C, tolerance 1e-12 over pH ∈ [4, 12]: **43 iterations** every time, which
is what `8/2⁴³ = 9.1e-13 < 1e-12` requires, with charge residuals of 6e-16 to
2e-14. An interval with no sign change (`[4.0, 4.5]`) returns the refusal
sentinel rather than an endpoint dressed as a root.

#### The engine reproduces the analytic limiting laws

For calcite open to a fixed P(CO₂) the classical result is `[Ca] ∝ P^(1/3)` and a
pH slope of −2/3 per decade.

| | Ca ratio per decade | pH slope per decade |
|---|---|---|
| **theory** | **2.1544** | **−0.6667** |
| Sounio (ideal solution) | 2.1517, 2.1538 | −0.6664, −0.6666 |
| IPhreeqc (Debye–Hückel) | 2.2937, 2.3674 | −0.6526, −0.6459 |

The engine lands on the limiting law to four digits. That is the correct
outcome and not a coincidence: **those laws are derived for the ideal case,
which is what the engine solves.**

#### The residual against the oracle is the activity model, and it was named first

| P(CO₂) | ΔpH | ΔCa | ionic strength (oracle) |
|---|---|---|---|
| 0.01 | −0.0296 | −13.59 % | 3.65e-03 |
| 0.1 | −0.0434 | −18.95 % | 8.32e-03 |
| 1.0 | −0.0641 | −26.26 % | 1.94e-02 |

The disagreement **grows monotonically with ionic strength**, which is the
signature of the term the engine omits. The direction is right too: with γ < 1
the ion activity product falls, so more calcite must dissolve to reach Ksp, and
the oracle's `[Ca]` is duly higher at every point. The oracle's departure from
the limiting laws grows in step.

So neither is wrong. They solve different problems, and the difference is the
**declared** one — `carbonate_equilibria.sio` states that activity coefficients
are unity and that a parity test therefore carries the activity model as a named
difference. It was named before it was measured, which is the only way that
statement is worth anything.

The residual is 13–26 %, against an oracle resolution measured at ~2 ppm, so it
is roughly **five orders of magnitude above the instrument** — real, not noise.

**What closes it**: a Debye–Hückel activity model in the engine. Until that
exists, engine and oracle are not comparable on absolute concentrations, and no
number from this closure is quoted as an absolute solubility.

### Debye–Hückel: the attribution tested, and it held

The ideal closure's residual against the oracle was attributed **in advance** to
the omitted activity model. The test of that attribution is to supply the *same*
activity model and see whether the residual collapses. If it had not, the
attribution was wrong and something else was going on.

The chain is PHREEQC's own, read from its source rather than a textbook —
Wagner & Pruss 2002 density, Bradley & Pitzer 1979 dielectric, then
`DH_B = sqrt(8πN_A·e²/DkT·ρ₀/1e3)/1e8` and `DH_A = DH_B·e²/DkT/(2 ln10)`, with
WATEQ `log γ = −A z²√μ/(1 + B a√μ) + b μ`. Using a *different* A(T) would have
introduced a second difference and destroyed the test.

Validated against values known independently of that source:

| | computed | expected |
|---|---|---|
| ρ₀ (25 °C, 1 atm) | 0.9970430117423 g/cm³ | 0.997047 |
| ε_r (25 °C) | 78.38441784056 | ~78.4 |
| **A (25 °C)** | **0.5100247894123** | ~0.5092 |
| **B (25 °C)** | **0.3284906339825** | ~0.3283 |

**One factor was wrong on the first attempt, and the validation is what caught
it.** `DH_B`'s `/1e3` reads like a kg/m³→g/cm³ conversion; it is not. PHREEQC's
caller overwrites the `rho_0` member with `calc_rho_0`'s *return* value, which is
already g/cm³, so the division is a genuine extra factor. Cancelling it as a unit
conversion put A and B out by exactly √1000 — 16.13 and 10.39 instead of 0.510
and 0.328. Nothing downstream would have looked odd; the activity coefficients
would simply have been absurd, and the residual would have moved the wrong way.

#### The collapse

| P(CO₂) | Ca error, ideal | Ca error, with activity | factor |
|---|---|---|---|
| 0.01 | −13.59 % | **−1.38 %** | 9.9× |
| 0.1 | −18.95 % | **−2.01 %** | 9.4× |
| 1.0 | −26.26 % | **−3.45 %** | 7.6× |

| P(CO₂) | ΔpH ideal | ΔpH with activity |
|---|---|---|
| 0.1 | −0.04341 | **−0.00223** |
| 1.0 | −0.06412 | **−0.00557** |

Ionic strength, which the engine now computes rather than ignores, agrees with
the oracle to **0.02 %** and **0.20 %**.

The attribution held. That is worth stating precisely: it was named before it was
measured, the prediction was that supplying the term would collapse the residual,
and it collapsed by roughly an order of magnitude.

#### What is left, named rather than shrugged at

**The remaining 1.4–3.5 % still grows with P(CO₂).** The obvious candidate is
**ion pairing** — PHREEQC speciates CaHCO₃⁺, CaCO₃⁰ and CaOH⁺, and this engine
does not. More dissolved carbonate means more pairing, which is the observed
direction. That is a named, testable next hypothesis, on the same footing the
activity model had before this section: it will be tested by adding the pairs,
not by asserting it now.

**The outer fixed point hit its iteration cap.** `solve_ph_activity` runs an
inner bisection on pH inside an outer fixed point on ionic strength, and the
outer loop reported **60 sweeps**, its ceiling, rather than meeting the 1e-14
tolerance it was asked for. The result is converged in practice — μ matches the
oracle to 0.02 % — but it did **not** meet its own stated criterion, and that is
a defect in the solver rather than a property of the chemistry. It is recorded
here rather than papered over by loosening the tolerance to whatever it happened
to reach.

### Ion pairs: the third named hypothesis, and the third collapse

The residual left after Debye–Hückel — 1.4 to 3.5 % in [Ca], still growing with
P(CO₂) — was attributed to **ion pairing**, on the grounds that PHREEQC speciates
CaHCO₃⁺, CaCO₃⁰ and CaOH⁺ and this engine did not, and that more dissolved
carbonate means more pairing. As before, the attribution was recorded before the
test.

There was a second, sharper reason to expect it: **PHREEQC's `Ca` column is
*total* calcium.** Comparing it against free Ca²⁺, as the previous section did,
is not like-for-like, and the pairs are exactly the difference.

Constants from the same pinned database: CaHCO₃⁺ (log K 10.91, `-gamma 6 0`),
CaCO₃⁰ (log K 3.224, uncharged with **no `-gamma` line**, so PHREEQC's gflag 0
with dhb = 0, i.e. γ ≡ 1), CaOH⁺ (log K −12.78, charged with no `-gamma` line, so
**Davies**, gflag 1).

#### The collapse

| P(CO₂) | ideal | + activity | **+ ion pairs** | total gain |
|---|---|---|---|---|
| 0.01 | −13.594 % | −1.376 % | **+0.0098 %** | **1384×** |
| 0.1 | −18.946 % | −2.015 % | **+0.0270 %** | **702×** |
| 1.0 | −26.261 % | −3.453 % | **+0.2127 %** | **123×** |

| P(CO₂) | ΔpH ideal | ΔpH + activity | **ΔpH + pairs** |
|---|---|---|---|
| 0.01 | −0.02962 | −0.00104 | **−0.000044** |
| 0.1 | −0.04341 | −0.00223 | **−0.000191** |
| 1.0 | −0.06412 | −0.00557 | **−0.001541** |

Ionic strength agrees with the oracle to **0.007 %, 0.025 % and 0.206 %**.

#### An internal check that was not planned for

`CaCO₃⁰` comes out **identical at all three P(CO₂)**: 5.868226950764e-06 molal.
It has to be. At calcite saturation `a_Ca · a_CO₃ = Ksp` is fixed, and
`CaCO₃⁰ = K · a_Ca · a_CO₃ = K · Ksp`, which does not depend on P(CO₂) at all.
The model reproduced that without being asked to, which is the kind of check
worth more than one that was designed in.

#### Where the residual now stands

Three named hypotheses, three tests, three collapses — ideal → activity → pairs,
each attributed in advance and each confirmed by supplying the term rather than
by fitting anything.

**The remainder still grows with P(CO₂)** — 0.0098 %, 0.027 %, 0.213 % — so one
small term is still missing, and the sign has flipped: the engine now reads
slightly high rather than low. At P(CO₂) = 1 atm the 0.213 % sits about **three
orders of magnitude above the oracle's measured 2 ppm resolution**, so it is
still real and not instrument. Naming the next candidate honestly: PHREEQC
applies molar-volume pressure corrections (`-Vm`) to these species and carries a
water activity that this engine holds at 1. Which of those it is has not been
tested, and it is not claimed.

### Abiotic methanation, and how wrong Van't Hoff is

**A labelled reconstruction**, as the pre-registration requires. The calibrated
analytic K(T) this stands in for is paywalled and confirmed unavailable, so what
is used is Van't Hoff with a constant enthalpy — precisely the "default"
treatment that the same study criticises. This module therefore also **measures
the approximation's error** rather than leaving the label to do the work.

Assembled from the pinned database's own half-reactions, so every number is
traceable:

```
R1  CO3-2 + 10 H+ + 8 e- = CH4 + 3 H2O    log K  41.071   dH -61.039 kcal
R2  2 H+ + 2 e- = H2                      log K  -3.15    dH  -1.759 kcal
R3  CO3-2 + 2 H+ = CO2 + H2O              log K  16.681   dH  -5.738 kcal
overall = R1 - R3 - 4 R2  ->  CO2 + 4 H2 = CH4 + 2 H2O
```

CO₃²⁻, 10 H⁺ and 8 e⁻ all cancel. Result:

| | |
|---|---|
| log₁₀ K (25 °C) | **36.99** |
| ΔH | **−201 940.76 J/mol = −201.94 kJ/mol** |

#### Thermodynamics is not the barrier, and that is the point

| T | log₁₀ K |
|---|---|
| 25 °C | 36.990 |
| 40 °C | 35.295 |
| 70 °C | 32.351 |
| 120 °C | 28.441 |

K falls with temperature, as an exothermic reaction must, and stays between
**10²⁸ and 10³⁷ across the entire range**. Methanation is overwhelmingly
favourable everywhere in this study's window.

And it does not happen. The field study's own abiotic controls report no
conversion, and the decoupled-redox oracle runs show hydrogen dissolving and
staying dissolved. **So the barrier is entirely kinetic, not thermodynamic** —
which is the mechanistic reason microbial catalysis is the operative process and
why a biotic term is not optional in the coupled model.

#### The Van't Hoff error, measured

Some database reactions carry **both** a `delta_h` and an `-analytic` fit, so
Van't Hoff can be checked against the database's own better answer for the *same*
reaction. Using `CO3-2 + 2 H+ = CO2 + H2O`:

| T | Van't Hoff | analytic | error (log K) | factor in K |
|---|---|---|---|---|
| 40 °C | 16.4795 | 16.5191 | −0.0396 | 1.10 |
| 70 °C | 16.1294 | 16.4365 | −0.3071 | 2.03 |
| 120 °C | 15.6647 | 16.7700 | **−1.1053** | **12.74** |

**At the field temperatures the approximation is defensible; at the top of the
sweep it is not.** Lehen is 40 °C and Lobodice 25–45 °C, where the error is under
0.04 log units. At 120 °C — the upper end of the temperature sweep the protocol
asks for — Van't Hoff is off by a factor of **12.7 in K**.

This is a measured version of the criticism the paywalled study makes of default
treatments, obtained without access to that study, using only the pinned
database's internal disagreement with itself.

#### ΔH sensitivity

The database states **no uncertainty** on `delta_h`, so this is a **sweep**, not a
propagated uncertainty: it says how far the answer moves, not how uncertain it is.

At 120 °C, ±10 % in ΔH moves log₁₀ K by ∓0.855 — a factor of **7.2 in K**, the
same order as the approximation error itself. Both belong in the band, and
neither can be reduced by choosing more carefully; closing them needs the
calibrated expression that is not available.

## `abiotic_kinetics.sio` — time, and the step-bisection test

The piece that turns point calculations into a trajectory. Until it existed there
was no step bisection, hence no statement about truncation, hence no quotable
number. It **imports** the verified modules rather than restating them —
`calcite_rate` for the rate law and `carbonate_equilibria` for speciation,
activity and ion pairs.

Calcite is no longer held at equilibrium: total dissolved calcium is the state
variable and Ω evolves toward 1.

```
d(Ca_total)/dt = rate(T, a_H, P_CO2, Omega) * A      [mol kg_w^-1 s^-1]
```

The calcium split closes analytically — every Ca species is proportional to the
free-ion activity, so `a_Ca = Ca_total / bracket` with the bracket collecting the
free ion and the three pairs. No second nested root-find is needed.

**Reactive surface area A is not a datum.** No source in this study supplies one
for these reservoirs, so it is a **swept input** and every trajectory is labelled
with the value used. Choosing one and calling it the answer would be inventing
the number that sets the entire timescale.

### The system is stiff, and that is chemistry rather than a defect

The first run used A = 1 m²/kgw with a one-day step and produced nonsense — Ca
overshooting to 1.4 molal, then oscillating to zero. The step was not slightly
too large; it was too large by a factor of about 1400.

At A = 1 m²/kgw the initial rate is 1.624e-05 mol m⁻² s⁻¹ and equilibrium sits at
1.234e-03 molal, so the characteristic time is

```
t_char ~ Ca_eq / (rate_0 * A) ~ 1.234e-3 / 1.624e-5 ~ 61 seconds
```

**Over any storage horizon, calcite at this surface area is simply at
equilibrium**, and the kinetics only matter when A is orders of magnitude
smaller. That is a property of the chemistry, and it is why the equilibrium
closure was a reasonable thing to build first.

### The kinetic model converges to the equilibrium model

40 °C, P(CO₂) = 0.01 atm, A = 1 m²/kgw, dt = 1 s:

| t (s) | Ca_total (molal) | pH | Ω |
|---|---|---|---|
| 10 | 1.0542e-04 | 6.238 | 0.0086 |
| 30 | 2.8891e-04 | 6.668 | 0.0162 |
| 60 | 5.4828e-04 | 6.940 | 0.1021 |
| 120 | 9.4363e-04 | 7.168 | 0.4740 |
| 300 | **1.2246e-03** | **7.277** | **0.9799** |

At 300 s the trajectory has reached Ω = 0.98 and Ca = 1.2246e-03 against the
**equilibrium closure's 1.2336e-03 at pH 7.2797**. Two independently written
paths — a root-find on charge balance at calcite saturation, and a time
integration that never mentions Ksp except through Ω — land on the same state.
That check was not designed in; it fell out.

pH rises from 6.24 to 7.28 as dissolution consumes H⁺, in the right direction.

### Step bisection: first order, but only once the step is small enough

Euler was chosen deliberately. Its truncation error is first order, so halving
the step must halve the error and successive differences must fall by exactly 2 —
an unambiguous expected result. A higher-order scheme would converge faster and
say less.

At t = 60 s, A = 1 m²/kgw:

| dt (s) | Ca_total | successive difference | ratio |
|---|---|---|---|
| 4 | 5.646451137163e-04 | | |
| 2 | 5.531512195925e-04 | 1.1494e-05 | |
| 1 | 5.482767592896e-04 | 4.8745e-06 | **2.358** |
| 0.5 | 5.463145346995e-04 | 1.9622e-06 | **2.484** |
| 0.25 | 5.454876232470e-04 | 8.2691e-07 | **2.373** |
| 0.125 | 5.451084218697e-04 | | |
| 0.0625 | 5.449262510051e-04 | | **2.082** |
| 0.03125 | 5.448368915090e-04 | | **2.039** |

**The first three ratios are not 2.** They sit at 2.36–2.48, and they were not
rounded to "about 2" and moved past. Pushing the bisection three levels further
resolved it: the ratio falls to **2.082 and then 2.039**, converging on the
first-order value as it must.

So the answer is that the scheme is first order and behaves correctly, **but the
larger steps were not in the asymptotic regime** — the region where the
convergence claim is even meaningful begins somewhere below dt ≈ 0.1 s for this
problem. A bisection stopped after three levels would have reported a ratio of
2.4 and left it ambiguous whether the integrator or the chemistry was at fault.

Richardson extrapolation on the last pair puts the converged value near
**5.4475e-04**, so dt = 1 s carries about **0.65 % truncation error** at this
horizon. That is now a measured quantity rather than an assumption, which is the
whole point of the exercise.

## `sweep.sio` — temperature, salinity, and what a band can honestly be

### The protocol asks for a band the sources cannot supply

The protocol asks for u(ΔH), u(log K) and u(k_calcite) **"from the literature"**,
propagated coherently. The literature this study actually holds does not supply
them:

| source | gives | states an uncertainty? |
|---|---|---|
| Palandri & Kharaka Table 33 | log k at 25 °C, Ea | **no** |
| `phreeqc.dat` | `log_k`, `delta_h` | **no** |

Assigning values would produce a band whose width is a decision of mine wearing
the costume of a datum. So this module separates two things that are often
conflated:

**Sensitivities are computed.** They are facts about the model and require no
knowledge of u(parameter): `S_i = ∂ln(rate)/∂ln(x_i)` by forward difference.
Multiply by `u_i/x_i` and the band exists the day someone supplies `u_i`.

**A band is computed for exactly one parameter** — the chemical-affinity exponent
p — because exactly one has a range the source supports.

### The sweep

P(CO₂) = 0.01 atm. The rate is evaluated at a **stated reference undersaturation
Ω = 0.5**, since at equilibrium it is identically zero and a sweep of zeros says
nothing.

| T | m_NaCl | pH | Ca_total (molal) | rate (mol m⁻² s⁻¹) |
|---|---|---|---|---|
| 25 °C | 0 | 7.2953 | 1.6228e-03 | 2.4428e-06 |
| 25 °C | 1 | 7.4034 | **2.9660e-03** | 2.4400e-06 |
| 25 °C | 2 | 7.3730 | 2.9095e-03 | 2.4407e-06 |
| 25 °C | 3 | 7.3312 | 2.7140e-03 | 2.4418e-06 |
| 40 °C | 0 | 7.2797 | 1.2336e-03 | 4.5181e-06 |
| 40 °C | 1 | 7.3949 | 2.3296e-03 | 4.5141e-06 |
| 120 °C | 1 | — | 5.6760e-04 | 5.9882e-05 |
| 120 °C | 3 | 7.3128 | 5.3721e-04 | 5.9889e-05 |

**Two things fall out that were not put in.**

*Calcite solubility is non-monotonic in salinity.* It rises from 1.62e-3 at zero
ionic strength to **2.97e-3 at 1 molal**, then falls to 2.91e-3 and 2.71e-3 at 2
and 3 molal. That is the WATEQ form doing its job: the Debye–Hückel term lowers
activity coefficients and raises solubility, until the `b·μ` term — +0.165 for
Ca²⁺ — takes over and turns the curve around. Nothing in the code was told to
produce a maximum.

*At fixed undersaturation, salinity barely touches the rate.* Across 0 to 3
molal the rate moves by **less than 0.12 %**. Salinity's influence is on *where
equilibrium sits*, not on the rate at a given distance from it — a distinction
that matters when reading any claim that brine composition slows or speeds
dissolution.

Rate rises by a factor of **24.5** from 25 °C to 120 °C.

### Sensitivities, and a check that they are right

At 40 °C, 1 molal, Ω = 0.5. Converting `∂ln(rate)/∂ln(log k)` into each
mechanism's share of the rate — which must sum to one:

| mechanism | ∂ln(rate)/∂(log₁₀k) | share of rate |
|---|---|---|
| acid | 0.0068 | **0.30 %** |
| neutral | 0.6220 | **27.02 %** |
| carbonate | 1.6737 | **72.69 %** |
| | | **100.00 %** |

The shares sum to 100.00 %, which is not something the finite differences were
told to do — it is the check that they are correct.

And the result matters for H1a: **the CO₂-dependent carbonate mechanism supplies
roughly three quarters of the dissolution rate** at these conditions. The
coupling between calcite and CO₂ is not a secondary path through the equilibria;
it is most of the rate law.

Activation-energy sensitivities at the same point: 8.2e-04 (acid), 0.123
(neutral), 0.497 (carbonate).

### The one band the sources support

Table 33 gives no p for calcite. The report says p and q "default to unity if not
specified", calls that default **"likely incorrect"**, and states that uncertainty
in them is "the source of much uncertainty in the length of time computed in
models of water–rock interaction". Magnesite, the nearest carbonate with a
**measured** value, has p = 4.00. So [1, 4] is a range the source supports.

| Ω | rate(p=4) / rate(p=1) |
|---|---|
| 0.5 | **1.875** |
| 0.9 | **3.439** |
| 0.99 | **3.940** |

Measured at Ω = 0.5: 1.875 at every temperature in the sweep, matching the
analytic `(1−0.5⁴)/(1−0.5)` exactly — the affinity term factorises out of the
temperature dependence.

**The band widens as equilibrium is approached**, toward the limiting ratio of 4.
A storage reservoir sits near equilibrium, which is precisely where this single
unsourced exponent is worth a factor approaching four in the rate — and therefore
in every timescale computed from it.

### What is still missing, plainly

A Monte-Carlo verification of a first-order band cannot be run, because there is
no band to verify: the inputs it would sample do not have stated distributions.
The honest sequence is to obtain u(log k), u(Ea) and u(ΔH) — from the primary
kinetics literature rather than from these compilations — and only then to
propagate. The sensitivities above are the half of that job which does not
depend on anyone supplying anything.

## Validation against the Hellerschmied abiotic series

### The measurement, reproduced independently

Source Data Fig. 5, sheet `Fig5A_pabio` (CC BY 4.0): ten abiotic cycles, ten days
each, 482 points per cycle, normalised total pressure. Reproduced here by
`tools/abiotic_check.py`:

| | cycles | loss at day 10 | mean p_t/p₀ |
|---|---|---|---|
| abiotic | 10 | **1.22 % – 2.20 %** | 0.9848 |
| biotic | 14 | 10.54 % – 11.62 % | 0.8905 |

These are genuine sterility controls, not gas-type controls: mesocosms M4 and M5
received a biocide and **35 kGy of γ-irradiation**.

### The prediction

In a sealed vessel, abiotic pressure loss is gas dissolving into brine. Per
component at equilibrium, `P_i = P_i0 / (1 + K_H,i · M_w · R · T / V_g)`, with
every Henry constant from the pinned `phreeqc.dat` and **no fitted parameter**.

At 40 °C: K_H(H₂) = 7.4530e-04, K_H(CH₄) = 1.2252e-03, K_H(CO₂) = 2.3711e-02
mol kgw⁻¹ atm⁻¹.

**A gap, handled by sweeping rather than guessing.** The paper gives a working
volume of 1.8 L and says the vessels hold drill cores plus reservoir brine, but
the **split** between brine, core and headspace is not reported anywhere
accessible — Supplementary Note 3 is instrumentation only. So the brine fraction
is swept, and the question becomes whether the measured loss lies inside what
dissolution can produce for a *plausible* fraction. That has two possible
answers and both are informative.

| brine fraction | predicted total-pressure loss |
|---|---|
| 0.10 | 0.485 % |
| 0.20 | 1.061 % |
| **0.30** | **1.764 %** |
| 0.40 | 2.647 % |
| 0.50 | 3.805 % |
| 0.60 | 5.419 % |

### The result

The measured 1.22 – 2.20 % implies a brine fraction of **0.223 – 0.349**, i.e.
**401 – 629 mL of brine** in a 1.8 L vessel, leaving 1171 – 1399 mL for gas plus
drill cores.

That is an ordinary loading for a vessel packed with cores and brine. **Gas
dissolution alone, with database Henry constants and nothing fitted, reproduces
the measured abiotic pressure loss.**

The test could have failed. Had the implied fraction come out at 0.02 or 0.95 —
outside anything a real vessel could hold — dissolution would have been
falsified as the explanation, and something else would have had to account for
the loss.

**And the biotic series fails it, correctly.** Reproducing 10.54 – 11.62 % by
dissolution would need a brine fraction far above 0.60 — more liquid than the
vessel contains. The biotic loss is *not* explicable by dissolution, which is
exactly what it should not be: that is the methanogenesis.

### Four independent lines now agree

1. The paper's own abiotic controls report no conversion.
2. The decoupled-redox oracle runs show H₂ dissolving and **staying** dissolved,
   with calcite dissolution unchanged to 0.016 % whether H₂ is present or not.
3. Methanation carries log K of 28–37 across the whole temperature range and
   still does not proceed, so the barrier is kinetic rather than thermodynamic.
4. This: dissolution alone quantitatively accounts for the abiotic pressure loss.

**Abiotic H₂ loss in this system is physical dissolution.** Everything beyond it
in the biotic series requires catalysis.

### What this still does not license

The comparison is on **total pressure**, not speciated H₂, because that is what
the source measured. It bounds total abiotic gas loss; it does not isolate a
calcite-dissolution contribution, and no such contribution is quoted from it.
The CO₂ term is a lower bound, since CO₂ also speciates at the brine's pH 8.7
and the calculation uses Henry's law alone — which makes the agreement
conservative rather than tuned.
