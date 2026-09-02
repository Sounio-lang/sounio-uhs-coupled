# Language gaps

What this model forced the language to need. Each entry says what was missing,
what it cost, and what was done instead. Nothing here was worked around in
silence — that is the point of the file.

Measured against `Sounio-lang/sounio` @ `origin/main` `57f87da54f`, 2026-09-02,
engine `lean_single`.

Status legend: **OPEN** — gap stands, fallback in use. **IN PROGRESS** — being
implemented in this study. **CLOSED** — implemented and gated.

---

## G1 — No native IEEE binary128; extended precision is quad-double

**Status: OPEN (by decision).**

`f128` / `f256` exist as type descriptors and **carry no values**. The compiler
refuses rather than degrading:

```
"f128/f256 value conversion is not implemented; wide-float casts fail closed"
"f128/f256 is reserved for compiler-owned format identity; source values are unavailable in V0-A"
"no f64 cast is a valid f128/f256 implementation"
```

Nothing named `f128`/`f256` appears in `ir/`, `native/`, `emit/`, `resolve/` or
`printer/`. There is no scalar FMA, no 128-bit XMM move, and no `i128` in the
native backend.

**Cost to close properly:** a from-scratch IEEE 754-2019 binary128 soft-float
with bit-identity against MPFR on adversarial rounding — estimated ~3 months.
Out of scope for this study.

**What is used instead:** `stdlib/math/qd128.sio`, a quad-double
(`Qd128 { x0,x1,x2,x3 }`, ≈212-bit mantissa, **f64 exponent range**), which is
the right instrument for the actual purpose — isolating accumulated rounding by
running the same trajectory twice — because that purpose needs *more precision
than f64*, not *IEEE binary128 conformance*.

**Stated plainly, wherever a "f128" result is reported in this study:** the
instrument is quad-double. It is **not** binary128; it does not have binary128's
exponent range, and it does not carry correctly-rounded semantics.

**Sub-gap G1a — no transcendentals (IN PROGRESS).** `qd128.sio` ships
add/sub/mul/div/sqrt/abs/cmp and no `exp`, `log`, `pow` or `log10`. The abiotic
model needs all four (Van't Hoff K(T), Arrhenius, log K, pH). Being implemented
in this study on top of the existing primitives.

---

## G2 — EL+ has no concrete domains, so convention axioms are not expressible

**Status: IN PROGRESS.**

Requirement O3: each dataset and each oracle declares its convention axioms
(reference pressure, energy unit, concentration scale, R), and the reasoner
**refuses the composition** when they are inconsistent — before anything runs.

The EL+ engine has no concrete domains and no datatype properties. Its concept
grammar is `atom | top | conjunction | existential` only. "Reference pressure =
101325 Pa" is therefore not an expressible axiom, and nothing in the tree derives
`1 bar ≠ 1 atm → conflict`. `guarded_add` genuinely rejects, and its rejection
semantics are proved — but it consumes a conflict matrix that the caller must
supply by hand.

**The missing extension, named precisely:** EL⁺⁺ with a **p-admissible concrete
domain**. The minimal fragment that suffices is
`D₌ = (ℚ, { =_q : q ∈ ℚ })` — unary equality on rational constants, feature paths
of length ≤ 1, no order, no arithmetic. It is trivially PTime and vacuously
convex, so the PTime completion result survives.

**Why it is cheap after all:** the 8 completion rules dispatch only on
`ckind == 2` (conjunction) and `ckind == 3` (existential), so a new
`ckind == 4` datum is **already inert** under every rule, and the role-layer
rules are additionally gated on `exid ≥ 0`, which is `-1` for non-existentials.
The whole extension lands in the conflict oracle as a derived-disjointness table,
not in the fixpoint. On the Lean side the proofs are parameterised over `Fin n` /
`Fin m` with no numeric literals, so the capacity is not baked in and a
TBox-level generator needs no re-proving of the existing development.

**Fallback if it does not land:** conventions hand-encoded as disjoint atomic
concepts. The negative control (re-injecting 1 bar vs 1 atm and the truncated
`R_cal`) remains mandatory either way.

---

## G3 — EL+ silently truncates, and one overflow path writes out of bounds

**Status: IN PROGRESS.**

Dense EL+ capacities are **64 concepts / 8 roles / 8 chains**. The sparse variant
(4096 classes) cannot do conjunctions, role hierarchies or composition, so
anything needing role chains is capped at 64 concepts. Consequence for this
study: the 518 Thaysen strains must be modelled as metabolic **groups**.

Worse than the cap is how it is reached. `stdlib/ontology/elplus.sio` has **no
capacity validation at all** — no guard on `nc`, `nb`, `nr` or `nchain` in
`elplus_seed`, `elplus_fixpoint`, `elplus_conflict` or
`elplus_derive_conflicts`. And axioms are dropped in silence:

- `snomed.sio` `stage`: `if a >= 0 && b >= 0 && self.n_stated < SNOMED_ELPLUS_MAX_STATED { … }`
  with **no else**. Stated subsumption axioms past the cap vanish. The closure is
  then computed over a **weaker TBox than the caller declared** — and the Lean
  soundness/completeness theorems are stated *for the TBox handed in*, so this
  silently voids their hypothesis. `elplus_conflict` misses conflicts and
  `guarded_add` accepts an inconsistent addition, defeating the module's own
  a-priori-consistency invariant.
- `snomed_elplus_add_role_chain`: `if self.nchain < 8 { … }` with no else — the
  9th chain vanishes.

**A verified memory-safety bug.** `snomed_elplus_add_role_chain` does **not**
guard its interned role ids, while its sibling `snomed_elplus_add_role_sub`
does (`if r1 >= 0 && r2 >= 0`). When the 8-role table is full, `intern_role`
returns `-1`, that `-1` is stored into `ch1/ch2/ch3`, and the `(RC)` loop then
evaluates `CLOSE_R[(r1 * 64 + c) * 64 + f]` with `r1 = -1` → index `-4096`
(out-of-bounds read), and with `r3 = -1` executes `CLOSE_R[idx] = true` — an
**out-of-bounds write**. `CLOSE_S: [bool; 4096]` is declared immediately before
`CLOSE_R: [bool; 32768]`, so this corrupts the subsumption matrix silently.

Being fixed in this study: fail-closed overflow everywhere, bounds-checked role
indices in **both** copies of the fixpoint (the library one and the one
hand-inlined in `snomed.sio`), and loud failure instead of degradation. **The cap
raise is deliberately deferred until fail-closed shows whether it is needed.**

---

## G4 — Same-dimension unit brands are interchangeable at call boundaries

**Status: OPEN.**

Distinct *dimensions* are enforced (molality vs molarity is refused with both a
name and a dimension diagnostic). Distinct *names of the same dimension* are not:
passing an `m3_stp` value to a parameter declared `m3_res` compiles with **rc=0**,
because the call-boundary check short-circuits when packed dimensions match. Under
`+`, the same pair produces only a `warning`.

This study needs the distinction — gas volume must carry its condition (STP vs
reservoir) in the type, and mass must distinguish solvent from rock.

**Cost:** the name tag exists (`EXPR_UNIT`) and is already enforced on `+`/`-`; it
is discarded on `*` and `/`, and bypassed at call boundaries. The cheap route is
an orthogonal **brand id** checked for exact equality at `+`/`-`/call/cast, with
`as` as the documented escape — roughly 1.5–2 weeks including scale conversion,
versus 3–5 weeks for widening the 7-slot dimension vector to admit user-declared
base dimensions.

**Fallback in use:** condition is carried in the *name* and enforced by
convention plus review, and every such site is listed in `RESULTS.md` as
unenforced. Any count of "rejections the type checker caught" excludes this class
and says so.

---

## G5 — No scale conversion is emitted on unit casts

**Status: OPEN.**

`mg as kg` type-checks and emits **no multiplication**. The production engine
stores only `(name-hash, dimension)` per unit — there is no scale field at all;
the other engine has `scale_num`/`scale_den` but they never reach codegen, and
compatibility explicitly ignores them. For a geochemistry model mixing mol/L,
mmol/L and µg/L this is not optional.

**Fallback in use:** every conversion is written explicitly as arithmetic in the
model source, and no `as` cast is used to change magnitude anywhere. Reviewed as
a class.

---

## G6 — Uncertainty: the coherent path exists, the naive path is unguarded

**Status: IN PROGRESS.**

Correlation-aware propagation is **already real** and compiled: forward-sensitivity
shadow slots in the production compiler, plus noise-symbol sets whose `E230`
"anti-garbling" rule refuses `Add`/`Mul` on `Knowledge` operands whose source sets
are not provably disjoint, with CI gates and Lean metatheory.

Three things are missing, and the study needs all three:

1. **`Sub` and `Div` are ungated** — a documented scope decision, not a safety
   one. Same-sign correlated subtraction overstates (safe), but an opposite-sign
   shared source makes `Sub` a genuine ungated anti-garbling.
2. **The stdlib quadrature is unguarded.** `gum_combine2`/`gum_add`/`gum_sub`
   take bare components with no source identity and compute plain root-sum-of-
   squares — so `u(x − x) = √2·u(x)` instead of 0. This is the surface that
   "quadrature must require a proof" is meant to close.
3. **Capacities bind.** 64 independent noise sources (a single `i64` mask; on
   overflow it degrades to *unknown*, which then refuses everything downstream),
   8 forward-sensitivity channels, 4 tracked sources per correlated value,
   covariance matrices capped at 24. A model with tens of uncertain parameters
   runs into these.

**Fallback if items 1–3 do not land:** first-order coherent band by forward
sensitivities, verified against Monte Carlo N ≥ 10⁴ in the oracle, with the
independence assumption stated explicitly at every combination site.

---

## G7 — No closure literals; helpers must precede callers

**Status: OPEN (accepted).**

`closures.lambdas` is `stale_conflicting/downgraded`. Named function references
are used throughout. Forward references are not allowed, so module layout is
dictated by call order. Cost to this study: stylistic only. Recorded for
completeness because it shapes every file.
