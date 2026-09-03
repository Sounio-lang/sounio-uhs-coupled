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
