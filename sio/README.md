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
