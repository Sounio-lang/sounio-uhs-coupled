# Language gaps

What this model forced the language to need. Each entry says what was missing,
what it cost, and what was done instead. Nothing here was worked around in
silence — that is the point of the file.

Measured against `Sounio-lang/sounio` @ `origin/main` `57f87da54f`, 2026-09-02,
engine `lean_single`.

Status legend: **OPEN** — gap stands, fallback in use. **IN PROGRESS** — being
implemented in this study. **CLOSED** — implemented and gated. **HALF CLOSED** —
one of several named routes is implemented and gated, the rest still stand; the
entry says which is which.

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

**Status: IN PROGRESS — item 1 closed, item 2 half, item 3 one cap of four. Per
item, with what is still open, under "Where the three stand" below.**

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

### Where the three stand (2026-09-03)

Measured on `feat/w4b-e072-allocation` (`26dcff2e42`), which carries both lanes.

**1 — done.** `Sub` and `Div` are gated. A signed noise-symbol domain makes the
coefficient signs part of the transfer, so `E230` now separates `x - x`
(same-sign reuse: the naive variance overstates, safe) from `(p - a) - a`
(opposite-sign: it understates). `ns_union_opaque` is the default transfer and
`ns_union` the `OpAdd`-only exception, so any unmodelled operator marks its
result sign-unknown and every later sign query fails closed. Gated by
`ns_antigarbling_gate.sh` and `ns_dataflow_trace_gate.sh`, both green.

**2 — half.** Two different surfaces compute a quadrature, and only one of them
now costs a proof.

- `stdlib/epistemic/graded_effects.sio` — **closed.** `indep_proven()` is gone: a
  zero-argument constructor that handed out the tight bound to anyone willing to
  *assert* independence is the same as not requiring it. Its replacement,
  `indep_dsep(proof)`, only accepts the result of `d_separated(A, B | Z)`, which
  does not compile unless a `causal graph` declared in source actually
  d-separates them. True on **both** engines — the type-level obligation and the
  proof token were carried to Madaros, which is the default and had neither.
- `stdlib/epistemic/gum.sio` — **untouched.** `gum_combine2`, `gum_add` and
  `gum_sub` still take bare components with no source identity and still compute
  plain root-sum-of-squares; `gum_sub` is literally `gum_combine2` on the
  difference, so `u(x − x) = √2·u(x)` still holds exactly as this entry
  describes. Nothing above changes that: the two surfaces are unrelated, and
  closing one is not closing the other.

**3 — one of four.** The independent-noise-source cap moved 64 → 256. The other
three stand, and the W4 capacity audit's contribution was to make them *named*
rather than merely true: `covariance.sio` now carries "KNOWN DEFECT … The clamp
below is SILENT" over the 24-variable clamp, `correlation.sio` says the fifth
source is absorbed into `residual_u2` with the total preserved and the identity
lost, and `lean_single.sio` records why the 8 forward-sensitivity channels are
not a constant anyone can edit — they are hand-unrolled across 1221
`EXPR_HSHADOW_jk`, 252 `VAR_HSHADOW_jk` and 458 `SSHADOW_n` references, because
the language has no closures and no way to index a family of globals by number.
Documented, not raised: the clamps are still silent at run time.

**Fallback, still in force for what is not closed:** first-order coherent band by
forward sensitivities, verified against Monte Carlo N ≥ 10⁴ in the oracle, with
the independence assumption stated explicitly at every combination site. Item 2's
`gum.sio` half and item 3's three remaining caps are what it still covers.

---

## G7 — No closure literals; helpers must precede callers

**Status: OPEN (accepted).**

`closures.lambdas` is `stale_conflicting/downgraded`. Named function references
are used throughout. Forward references are not allowed, so module layout is
dictated by call order. Cost to this study: stylistic only. Recorded for
completeness because it shapes every file.

---

## G8 — A `lean_single` change is not verifiable through `bin/souc`

**Status: HALF CLOSED. The harness half is done on `feat/w6-neg-f64-global`
(`4e92710d20`), not yet merged; the wrapper half stays open by this study's own
decision not to modify `bin/souc`. Affects how every language-side claim in this
study is checked. See "A correction to this entry's premise" below: a lagging
seed is a CI merge blocker, not the tolerated condition this entry first
described.**

`bin/souc` resolves the engine with a fallback:

```sh
[[ -x "$LEAN_SINGLE" ]] || LEAN_SINGLE="$ROOT_DIR/bin/souc-linux-x86_64"
```

`bin/souc-linux-x86_64` is the **committed seed binary**. So when a change is made
to `self-hosted/compiler/lean_single.sio` and `make build` is run, the tree now
holds two different compilers:

| | md5 (measured on the reaction-literal branch) |
|---|---|
| committed seed, used by `bin/souc` | `c7d5e8388494f6b753111943182df7ba` |
| freshly built `gen3.elf` | `21bfdf12bf923220f6c98ba88d42bd0c` |

**`./bin/souc check` exercises the seed, not what was just built.** A new
front-end feature is therefore invisible to it, and to the test harness, which
invokes the same wrapper.

Two corrections to the above, from measuring the wrapper rather than reading it:

**It is the `lean_single` path only. Madaros — the default engine — already does
the right thing and says so.** `bin/souc` resolves a local
`artifacts/self-hosted/madaros` *ahead* of the committed ELF, and `--version`
reports it:

```
provenance: elf=artifacts/self-hosted/madaros md5=e0c471f8 tree=cbafc5547d
provenance: this ELF is a LOCAL BUILD (...), NOT the committed
            bin/madaros-linux-x86_64 (md5=ff69dae4); it was resolved ahead of it.
provenance: set SOUNIO_REQUIRE_COMMITTED_MADAROS=1 to refuse local builds
```

The `lean_single` path prints **no provenance at all** — it execs the raw ELF,
which does not understand `--version` and answers with a usage line. That
asymmetry is what made the hole invisible: the engine that reports is the one
that did not need to.

**There are two seeds, and the one named above is the second choice.**
`LEAN_SINGLE` is `bin/souc-lean-single-x86_64`, and `bin/souc-linux-x86_64` is
only its fallback. On `feat/w6-neg-f64-global` the first is present and is
`0f3aa2c9…` — the fixed point of unmodified `main`.

This was found the hard way. The reaction-literal feature (`docs/CHEMICAL_SYNTAX.md`,
item 1) was measured through `bin/souc` and pronounced inert: three compile-fail
fixtures compiled cleanly, and so did deliberately malformed syntax. Run against
`gen3.elf` directly, the same fixtures give:

```
error[E188]: reaction `sulfate_reduction_perturbed` does not balance for element H
error[E189]: reaction `iron_oxidation_unbalanced` does not balance in charge
error[E190]: unknown element symbol `Xx` in reaction `bogus_element`
```

The feature was correct the whole time; the measurement was pointed at the wrong
binary.

**Why this belongs in this file.** It is the same failure this project has
documented four times in CI gates — a claim resting on a proxy that no longer
measures what it names — occurring in the verification path itself, and it caught
us. `✓ FIXED POINT OK` says the bootstrap chain converged. It does **not** say
that any subsequent test exercised the compiler that was just built.

**Practical consequence.** Any language-side result in this study must state which
binary produced it. Verification of a `lean_single` change runs `gen3.elf`
directly, or refreshes the seed first. A green test suite after a compiler change,
obtained through `bin/souc`, is not evidence.

**What would close it.** `bin/souc` preferring a freshly built `gen3.elf` over the
seed when one is present and newer, or the harness taking an explicit engine
binary and refusing to guess.

### The second route, done

`scripts/dev/run_sio_test_suite_v2.sh` now refuses to run when a locally built
`gen3.elf` is newer than the `lean_single` binary the run would use. It prints
both paths with their md5s and makes the caller say which one they mean:

```
harness: refusing to run -- a locally built gen3.elf is newer than the
         lean_single binary this run would use, so the result would
         describe the committed seed, not the tree.
           would run:   bin/souc-lean-single-x86_64  (md5 0f3aa2c9)
           local build: gen3.elf                     (md5 fe4d4915)
```

```sh
SOUNIO_TEST_SOUC_BIN=$PWD/gen3.elf  ...   # test the build
SOUNIO_TEST_ALLOW_STALE_ENGINE=1    ...   # test the seed, on purpose
```

The hole was live on that branch while G9 was being fixed: the seed was
`0f3aa2c9` — unmodified `main` — and `gen3.elf` was `fe4d4915`, the tree with the
fix. A suite run would have reported that the fix does nothing.

Every run now opens with `harness: engine=… elf=… md5=…`, which is the
**Practical consequence** above enforced rather than remembered.

Verified on all four paths: the default run refuses with `rc=2`; the explicit
build reports `engine=explicit md5=fe4d4915` and passes
`tests/run-pass/module_let_negative_f64.sio`; the explicit seed reports
`engine=lean_single md5=0f3aa2c9`; and with `artifacts/self-hosted/madaros`
present the guard stays silent and reports `engine=madaros`. That last one is
the test that mattered — the ordinary case must not start refusing.

### A correction to this entry's premise

The entry above says the tree "now holds two different compilers" as though that
were a tolerated condition. It is not. `scripts/ci/canonical_compiler_gate.sh`,
in the Contracts job, **requires** `bin/souc-lean-single-x86_64` to BE the fixed
point of `self-hosted/compiler/lean_single.sio`:

```
[canonical-compiler] FAIL: <seed> is NOT the fixed point of <src>.
[canonical-compiler]   The committed lean_single ELF has drifted from current source.
```

So a lagging seed is a **merge blocker**, not a sanctioned state. Measured
2026-09-03, both branches from this study are held by it:

| branch | committed seed | fixed point of its source |
|---|---|---|
| `feat/w6-neg-f64-global` | `0f3aa2c9…` | `fe4d4915…` |
| `feat/w4b-e072-allocation` | `0f3aa2c9…` | `c20a4490…` |

That sharpens what this gap actually is. The hazard is not that the repository
accepts two compilers — it forbids them. The hazard lives in the **window**
between editing `lean_single.sio` and refreshing the seed, which is every moment
of development on that file. CI closes the window at merge. Nothing closes it
while you work, and `bin/souc` says nothing, which is how the reaction-literal
feature was measured against the wrong binary and pronounced inert.

The refresh is documented and executable — `scripts/dev/refresh_lean_seed.sh`,
`docs/ops/LEAN_SINGLE_SEED_REFRESH.md` — and it is deliberately not casual. §3
calls placement "a founder decision — consumes cluster", the driver's own header
says *"Agents must not run --execute unprompted"*, and §2 is a HARD STOP against
the obvious shortcut: an ELF that merely differs from the old seed is not a
refresh; M1 settle, M2 self-reproduction and M3 determinism must be recorded, and
`out/SETTLED.md5` must exist before anything is installed. *"Hand-derive without
M1 = off-recipe. STOP. Do not cp. Do not commit."* — because shipping gen1
(#1606) is worse than not refreshing at all.

**Still open:** the first route, and it is the one that closes the window rather
than the end state. The canonical gate already forbids a lagging seed at merge;
what nothing does is tell you, while you work, that the binary answering
`./bin/souc` is not the source you just edited. `bin/souc` preferring a freshly
built `gen3.elf` would say it — but it is a wrapper other projects depend on, and
this study does not modify it. Until that changes, a `lean_single` result
obtained through `bin/souc` outside this harness still carries the original
hazard, and must name its binary.

Worth noting that the wrapper already does exactly this for the OTHER engine:
Madaros resolves a local `artifacts/self-hosted/madaros` ahead of the committed
ELF and reports which one it used. The asymmetry is the whole gap.

---

## G9 — A module-level `let` holding a negative f64 is silently wrong

**Status: CLOSED in `lean_single` on `feat/w6-neg-f64-global` (`e2652bbba9`),
not yet merged. Found by porting a real correlation; it produced a wrong number
that looked entirely healthy.**

Minimal reproduction, compiled with `gen3.elf` at `origin/main` + the qd128
branch, printed as value × 100 so the sign and magnitude are both visible:

```sio
let  M_LET_EXPR: f64 = 0.0 - 1.25     //  0     WRONG, want -125
let  M_LET_LIT:  f64 = -1.25          // +125   WRONG, sign dropped
var  M_VAR_EXPR: f64 = 0.0 - 1.25     // -125   correct
var  M_VAR_LIT:  f64 = -1.25          // -125   correct
let  M_INT_EXPR: i64 = 0 - 7          //   -7   correct
let  M_INT_LIT:  i64 = -7             //   -7   correct
// and inside a function body:
let  local_expr: f64 = 0.0 - 1.25     // -125   correct
```

**The defect is exactly one combination: module scope, `let`, type f64, negative
value.** It fails in two different ways depending on how the value is written —
a computed expression collapses to `0.0`, and a leading-minus literal keeps the
magnitude and loses the sign. `var` is correct at module scope, `i64` is correct
at module scope, and `let` is correct inside a function.

**It is one engine, not the language.** The reproduction above was measured on
`lean_single` only. Run through Madaros — the default engine — all seven lines
are correct. Anything this file says about "the compiler" here means
`self-hosted/compiler/lean_single.sio`; the modular tree never had the defect
and was not touched by the fix.

Note also that `-1.25` is accepted at all. The language documents that it has no
unary minus and that one must write `0 - x`. Measured, the documentation is what
is wrong: unary minus on f64 is correct in a function body on **both** engines,
in a module-level `var`, and in a module-level `let` of `i64`. Module-scope
`let f64` was the only place it misbehaved, so the fix makes that case agree
with every other context rather than rejecting a form the rest of the language
accepts.

### How it surfaced

Porting Chabab's Model 2 for H₂ solubility. The Sounio result agreed with the C++
replica exactly at zero salinity and diverged as salinity rose:

| m_NaCl | Sounio (before) | C++ replica | ratio |
|---|---|---|---|
| 0 | 6.313448550e-02 | 6.31344855e-02 | 1.0000 |
| 1 | 5.058345798e-02 | 5.10647712e-02 | 1.00952 |
| 2 | 4.052755323e-02 | 4.20922e-02 | 1.03862 |
| 3 | 3.247074510e-02 | 3.53597e-02 | 1.08897 |

The ratios are exactly `exp(0.009470244669 · m²)`, and the correlation's
`ζ_H2−c−a` is `−0.009470244669`, declared as a module-level `let`. It had
evaluated to zero, so the ζ term vanished from the model. After changing that one
declaration to `var`, every point matches the replica to all printed digits.

### Why this one is serious

It is silent, and it lands where a scientific model keeps its constants.
Enthalpies of reaction, Gibbs energies, log K values, activation energies and the
`ATP_energy` figures in the reference microbial model are **all negative**, and
the idiomatic place to write them is a module-level constant. The compiler
reported no error, no warning, and a tier summary of `PLATINUM=1940 (98%)`.

Had the divergence not grown with a swept parameter, a single-point comparison
would have shown 5 % disagreement between the model and an oracle — a plausible
number to attribute to physics, since two genuinely different solubility models
already disagree by that much (see `oracles/README.md`). **The parameter sweep is
what separated a compiler defect from a modelling difference.**

### Mitigation in this study

Every negative f64 constant is a module-level `var` or is assigned inside a
function; none is a module-level `let`. This is stated in the header of each
`.sio` file that has one. That is a discipline, not a fix — it depends on
remembering, which is the wrong place to put a correctness argument.

**The discipline stays until the fix below is in whatever binary the study
builds with.** A `.sio` file that relies on it is correct under both.

### What closed it

Both halves of "either evaluating them correctly, or rejecting the forms it
cannot evaluate" — the first for the two forms that have a meaning, the second
for everything else.

The cause was one representation choice. A module-level `let` becomes a
compile-time constant, and for f64 the value is rebuilt at each **use site**
from the literal token's magnitude (`TV`/`TF`/`TD`/`TX`). The token text carries
no sign, so the negation was recorded into the integer field `CONST_VAL`, which
that path never reads: the sign was computed and then discarded. `0.0 - 1.25`
failed differently — the constant folder's entry test does not accept a float
literal as a first token, so the scanner took the literal path, read `0.0` as
the whole initializer, and walked away from `- 1.25`.

- `CONST_NEG` carries the sign the token cannot, applied at both use sites with
  the instruction each backend's own unary-minus path already emits
  (`btc rax, 63` on x86, `fneg d0, d0` on a64).
- `0.0 - LITERAL` is recognised as the idiom it is.
- Any arithmetic still standing after the literal is **refused**. That is the
  part worth keeping: this block's failure mode was to consume what it
  understood and drop the rest without a word.

Measured against `gen3.elf` `fe4d49152e4dbb1e5d505db88cb4f175`, built from that
branch — not through `bin/souc`, whose `lean_single` fallback is the committed
seed (G8). All seven lines of the reproduction now match Madaros, as do
scientific notation, `0.0 - 1.5e2`, `-1.5e2`, `0.0` and `-0.001`.

On the ζ constant that surfaced this, `-0.009470244669` as a module-level `let`,
both spellings now give `-9470244669` × 10⁻¹² — agreeing with the `var` form to
every printed digit. Against a `gen3.elf` without the change, the same file
gives `+9470244669` for the leading-minus literal and `0` for `0.0 - x`.

`tests/run-pass/module_let_negative_f64.sio` is the regression test. It passes
with the fix and **fails**, naming the case, against a `gen3.elf` built without
it — it compares against literals written inside a function body, the path that
was never broken, because comparing against module-level constants would have
compared the defect with itself.

No regression in the corpus: 53 of the first 60 `run-pass` files compile before
and after, and the same 7 fail on a baseline built from that tree with the
change stashed — cross-module identifiers and an `[i8;4]` vs `[i64;4]` mismatch,
neither related.

---

## G10 — No rate-law composition or catalyst-cycle representation

**Status: CLOSED, merged.** Implemented and merged to `Sounio-lang/sounio`
`main` via PR #2401 (merge commit `1d3eaa3f2f`, 2026-09-03) — see
"Update, 2026-09-03: implemented" below for what actually landed, what it
corrected in this entry's own original assumptions, and what is still not
done. The paragraph immediately below is the entry as originally written
and is kept for the record, not silently edited.

The commit first landed on `lane/cursor-1/20260826`, a long-running
integration lineage 7000+ commits diverged from `main` — too far diverged
to merge directly (the same reason `Sounio-lang/sounio` PR #2346 was closed
two days earlier for unrelated work on that same lineage). Cherry-picked
onto a clean branch off `main` instead. CI on the resulting PR caught two
real issues before merge, both fixed and pushed: a witness-census artifact
drifted by the one new `tests/run-pass/*.sio` fixture this suite adds
(resynced via the gate's own named fix command), and a dangling
`lean_lib «SounioZDCollapse»` registration accidentally carried over from
the source lineage's own lakefile — that file had been deliberately
deleted elsewhere in the lineage's history, but the registration survived
independently and broke `lake build` on `main`. Removed; unrelated to the
catalysis suite itself.

**Status, as originally written: OPEN (by decision) — anticipated, not yet model-forced.**

Every other entry in this file is scoped to what a run of this study actually
hit. This one is not: no phase here has yet needed a catalytic mechanism, so by
this file's own header ("what this model forced the language to need") it does
not belong. It is entered anyway, at a workstream-planning decision point,
because the cost is worth measuring before the decision to build it is made
rather than after — the same reasoning that put a number on G4 and G5 before
either was started. **What would move it to IN PROGRESS:** a phase of this or a
future study actually needing a catalytic mechanism — the mineral-surface-
catalyzed abiotic pathway is the candidate, since it is what Phase 4's stopped
coupling work would have touched next (`PHASE4_GATE.md` §7).

**The gap, in three parts.**

1. **Rate-law composability beyond mass-action.** The general CRN engine,
   `stdlib/chemistry/kinetics.sio:619` (`compute_rates_general`), takes an
   arbitrary stoichiometry matrix, but each species' rate order is derived only
   from the *negative* — net-consumed — entries of that matrix
   (`kinetics.sio:625`, `if coeff < 0.0`). A catalyst is net-zero stoichiometry
   by definition, so it cannot appear in a rate expression under this scheme at
   all, composable or otherwise. Non-mass-action laws exist only as one-off
   scalar formulas outside this engine: Michaelis–Menten as `mm_rate()`
   (`stdlib/chemistry/acids.sio:121`) and dual-Monod as `monod()` /
   `microbial_rate()` (`sio/microbial.sio:62,72` in this repo — the rate law
   `PHASE4_GATE.md` actually runs), the latter hardcoding two parameter sets
   behind a `which: i64` switch rather than a general mechanism. Neither plugs
   into `compute_rates_general`; nothing shares a rate-law shape.
2. **A catalyst/intermediate species-role tag.** No species-role typing exists
   anywhere in the language — reactant, product, catalyst and intermediate are
   all the same kind of thing wherever species appear. The one "enzyme" hit in
   the tree, `self-hosted/collections/epistemic_reaction.sio:106,182`
   (`enzyme_prov_l64`), is a Merkle-provenance/data-lineage concept, unrelated
   to kinetics. Reaction literals — `reaction NAME { ... }`, compile-time
   mass/charge balance checking — exist on `feat/w5-reaction-literals`
   (`self-hosted/compiler/lean_single.sio:4257`, not on `main`), and their own
   doc comment, verbatim in `docs/CHEMICAL_SYNTAX.md:103–107`, states they do
   "not check that a reaction is ... elementary, that its direction is right,
   or that its rate law is sound." A species-role tag is cheapest once that
   branch merges, since roles would naturally attach to the existing species
   position in that syntax rather than needing their own declaration form —
   but nothing about it is blocked on the rate-law-composability piece above.
3. **Turnover-number bookkeeping.** Absent everywhere in both repositories.
   It is not a separate primitive: it falls out of (1) and (2) once a rate law
   can be evaluated per-catalyst and a catalyst is a typed role, as moles
   product formed per mole of tagged catalyst per unit time.

**Cost to close properly:** in the same style as G4/G5 — weeks, not months. A
rate-law trait/interface pluggable into `compute_rates_general` so mass-action,
Michaelis–Menten and Monod-style laws share one composable shape; a
catalyst/intermediate species-role tag riding the reaction-literal syntax; and
turnover-number derivation on top of both. Comparable in size to one W-sized
workstream, with the explicit caveat that the species-role piece is additionally
blocked on `feat/w5-reaction-literals` merging first.

**What is used instead:** standalone hardcoded rate formulas per model
(`mm_rate()`, `monod()`/`microbial_rate()`), each hand-written outside any
shared abstraction and reviewed as a class — the same fallback discipline
already in force for G4 and G5.

---

### Update, 2026-09-03: implemented

New file, `stdlib/chemistry/catalysis.sio` (709 lines), on
`Sounio-lang/sounio` branch `lane/cursor-1/20260826` @ `21900a1dfe`. All
three parts of the gap are addressed:

1. **Rate-law composability.** `RateLaw` — a flat struct with an `i64 kind`
   discriminator (Sounio has no traits/generics beyond prototype tier) —
   covers 9 kinds: mass-action, MM irreversible/reversible, Hill, Langmuir–
   Hinshelwood single/dual-site, competitive/noncompetitive-inhibited MM,
   generalized dual-substrate Monod. `compute_rates_mechanistic`/
   `simulate_mechanistic_crn` plug this into the same `MatNM` stoichiometry
   machinery `compute_rates_general` already uses — `nu` still drives
   species bookkeeping, only the per-reaction rate *formula* is now
   pluggable. `compute_rates_general` itself is untouched, and is the
   trusted baseline the new path is cross-checked against.
2. **A catalyst/intermediate species-role tag.** `CatalyticSpecies { index,
   role, initial_amount }`, plus `catalyst_conserved` — a checkable
   invariant (net-zero stoichiometry across a minimal cycle) that used to be
   assumed, not tested. **This entry's original assumption about how to get
   here was wrong, discovered during implementation, not before**: it said
   a role tag would be "cheapest once `feat/w5-reaction-literals` merges,
   since roles would naturally attach to the existing species position in
   that syntax." Reaction literals turn out to be **100% compile-time-only**
   — species are folded into throwaway integer arrays for balance-checking
   and then discarded; nothing survives to codegen or produces a runtime
   value at all. A role tag cannot ride syntax that carries nothing to
   runtime. `catalysis.sio` has zero dependency on that branch.
3. **Turnover-number bookkeeping.** `turnover_number`/`turnover_frequency`
   plus `simulate_catalytic_cycle`, which runs a mechanism and derives
   TON/TOF from it directly.

**Validation, by explicit instruction, never Python, never Rust — not even
for validation.** Sounio-internal tests (conservation laws, algebraic
identities — e.g. Hill at `S=Km` gives exactly `Vmax/2` for any `n`, checked
both numerically in `.sio` and formally in Lean) plus three independent
oracles, none of them translations of the Sounio code: `stdlib/chemistry/
oracles/catalysis_oracle.cpp` (C++23, all 9 rate laws reimplemented
independently, 22-case self-test), `stdlib/chemistry/oracles/
catalysis_cycle.kk` (Koka, independent hand-rolled RK4 of the classic
E+S⇌ES→E+P cycle, division-by-zero surfaced as a typed effect), `formal/
lean4/SounioCatalysisKinetics.lean` (Lean4, Mathlib-free — this repo's own
default — proves enzyme conservation and Hill half-saturation, zero
`sorry`; a mandatory `xai/grok-4.5` math-review on that file caught a real
docstring/theorem mismatch — an unused `0 < n` hypothesis — before it
shipped).

**The independent Koka oracle caught something concrete.** Run at
`kinetics.sio::test_enzyme_crn`'s own parameters (`kf=1.2, kr=0.3, kcat=0.8,
E0=0.05, S0=2.0`), it independently computed P≈0.154, not the ≈0.7 that
test's own `check_near` expects. That expectation was never actually
verified: `kinetics.sio` parse-fails outright under `bin/souc run`, and its
external driver (`tests/stdlib/chemistry/test_kinetics_core.sio`) is
`//@ check-only` — type-checked, never executed. `catalysis.sio`'s own
equivalent test now checks the value both independent implementations
agree on, ~0.154054667769, to full displayed precision.

**Four real, previously-undocumented Sounio compiler bugs were found
building this — this is the "what this model forced the language to need"
this file's own header names, arriving after the fact rather than before
it, which is exactly why this update exists as a correction and not just a
status flip.** All four are fixed at their trigger point in
`catalysis.sio` itself: `extern "C" fn pow(x,y)` silently returns `0.0`
whenever `y` isn't a compile-time literal; sequential per-field mutation
across *different* struct-array indices aliases every write to one slot;
a function returning a struct or tuple that embeds a fixed-size array
corrupts under native linking; a tuple-destructured binding corrupts on its
second use in the same function. **One is not worked around**, and is a
standing limitation stated plainly rather than hidden: a function taking a
mutable array-reference parameter (`&![T;N]`) only links when called
directly from `main()`, not from a wrapper function — so
`simulate_catalytic_cycle`/its test is verified correct by isolated
extraction (matches the Koka oracle exactly) but the full 10-function test
suite cannot be linked as one binary. `test_catalysis_stdlib.sio` is
`//@ check-only` for the same reason `test_kinetics_core.sio` already is.

**Not done, stated plainly:** not applied back to this study's own models
(the mineral-surface-catalyzed abiotic pathway named above as the trigger
for IN PROGRESS is still untouched — this closes the *language* gap, not a
*study* phase). If a future phase of this study actually needs a catalytic
mechanism, the tool now exists to reach for rather than a further estimate
to make.

## G11 — A string literal of 127 to 199 characters, passed as a function argument, segfaults

**Status: OPEN. Found by the deliverable figure, not by a test suite.**

`sio/figure.sio` emits the study's figure as SVG from the engine itself. It
crashed at runtime after writing 71 correct lines, on the caption calls. The
first diagnosis — a string-length limit — **was wrong**, and is recorded as wrong
because the correct one is stranger.

**Reproduction**, against `gen3.elf` md5 `0f3aa2c9dd3be4e407ce546130f7614c`:

```sio
fn txt(x: f64, y: f64, anchor: string, size: f64, col: string, t: string) with IO, Mut, Div, Panic {
    print(anchor) print(col) print(t)
}
fn main() -> i32 with IO, Mut, Div, Panic {
    txt(30.0, 366.0, "start", 10.0, "#555555", "<N characters>")
    0
}
```

| literal length | result |
|---|---|
| ≤ 126 | compiles, runs, exit 0 |
| **127 – 199** | compiles, **SIGSEGV at runtime (exit 139)**, no output |
| ≥ 200 | compiles, runs, exit 0 |

**It is a window, not a threshold.** 126 works, 127 through 199 crash, 200 works
again. That rules out a simple buffer bound and points at a length-dependent
branch in argument passing or literal placement — something takes a different
code path at 200 that is correct, and the middle range takes one that is not.

**It is specific to passing the literal as an argument.** `println` with a
literal of any length tested (up to 257) is fine. The crash needs the literal to
travel through a function parameter.

**Not a compile error.** The compiler accepts it silently and emits a binary that
segfaults. That is the worst of the three available failure modes — worse than
rejecting it, and worse than miscomputing, because nothing in the build warns.

**What it cost here.** One debugging cycle and a wrong first diagnosis, which was
committed to a source comment before being corrected. The figure's captions are
split into sub-126-character literals: a cosmetic workaround with no effect on
any number in the figure.

**What would close it.** A codegen fix. Until then the practical rule for this
lane is: **keep string literals passed as arguments under 126 characters**, now
noted at the point of use in `sio/figure.sio`.

## G12 — Unit types type-check correctly and MISCOMPILE the values. The units rewrite is blocked.

**Status: OPEN, and it blocks the stated goal of carrying everything on units.**
Found by attempting exactly that rewrite, on the module `CORRECTIONS.md` C20
identified as the one that declared units and did not use them.

`FEATURES.md` measured the checker and it is genuinely good: it rejects assigning
one unit into another, adding different units, passing the wrong unit at any
argument position, and passing a bare `f64` or a bare literal where a unit is
expected. Every one of those was verified by a run.

**None of that matters, because the values are wrong.**

### Reproduction 1 — a module-level `var` with a unit type does not hold its initialiser

```sio
unit degK = K;
unit molal = mol / kg;
var T_MIN: degK = 298.0
var M_MAX: molal = 4.0
// ... print (T_MIN as f64) * 100 and (M_MAX as f64) * 100
```

| binding | expected ×100 | **actual** |
|---|---|---|
| `var T_MIN: degK = 298.0` | 29 800 | **9** |
| `var M_MAX: molal = 4.0` | 400 | **9** |

Both read back as ≈ 0.09, and **both read back as the same wrong value** despite
different initialisers. The same declarations typed `f64` are correct (29 800).

### Reproduction 2 — a unit-typed value arrives corrupted as a function argument

```sio
unit degK = K;
var T_MIN_PLAIN: f64 = 298.0            // plain f64 module var: correct
fn check(t: degK) -> bool with Mut, Div, Panic { let raw = t as f64
    raw < T_MIN_PLAIN }
fn main() -> i32 with IO, Mut, Div, Panic {
    let t: degK = 313.15
    let lo: degK = 290.0
    print("local t x100 = ") // -> 31314, CORRECT
    if check(t)  { /* 313.15 < 298 */ }   // FALSE, correct
    if check(lo) { /* 290    < 298 */ }   // FALSE, WRONG -- should be TRUE
}
```

The local `let lo: degK = 290.0` reads back correctly **in the scope where it is
declared**. Passed to `check`, it compares as though it were ≥ 298. So the
corruption happens at the call boundary — the same boundary the checker guards
so well.

### End-to-end consequence, measured on a real module

Annotating `sio/h2_solubility.sio` with the dimensional form — parameters,
module-level bounds, and typed bindings at every call site, with an explicit
`as f64` strip at the correlation boundary — **compiled cleanly** and produced:

| | original (`f64`) | unit-annotated |
|---|---|---|
| m_nacl = 0.0 | **6.313448550155170e-2** | **0** |
| m_nacl = 1.0 | 5.106477118630551e-2 | **1e200** |
| m_nacl = 2.0 | 4.209222515732879e-2 | **1e200** |
| m_nacl = 3.0 | 3.535966399150986e-2 | **1e200** |

Correct molalities became zero and 10²⁰⁰. The refusal sentinels still fired,
which would mask the failure from a smoke test.

**The annotated module was reverted.** It is not committed, because a module that
type-checks and returns 10²⁰⁰ is worse than one that carries no units at all.

### Why this is the worst failure mode available

The three ways a feature can fail are: reject valid code, be absent, or accept
code and compute the wrong answer. This is the third, and it is combined with a
**correct** type checker — so the feature actively invites the user to adopt it,
passes every check they write, and silently corrupts the numbers. A study that
adopted units on the strength of `FEATURES.md` §3 alone would have published
wrong results with a clean build.

### What this means for "everything on units and ontologies"

The mechanism is closer than the registry's `prototype / downgraded` tier
suggests — the *checking* half is real and would have caught the mg/L-of-solution
versus mmol/kg-of-water confusion this study worried about. **The codegen half is
unusable.** Until G12 is fixed, unit types can be used for documentation and for
compile-time checking of code that is then thrown away, and not for anything that
computes a number that will be reported.

**What would close it:** a codegen fix for unit-typed value representation in
module-level initialisers and at call boundaries. This is not a design gap like
G4/G5 (same-dimension scale) — the design works. It is a lowering bug.

### An accidental correctness, worth naming

`CORRECTIONS.md` C20 criticised this study for declaring units and never using
them, and for not checking what the feature could do before deciding not to use
it. **That criticism of the process stands.** But the outcome was accidentally
right: had the study used the feature, every number it reported would have been
corrupt. The right decision was reached for the wrong reason, which is not the
same as having been right.
