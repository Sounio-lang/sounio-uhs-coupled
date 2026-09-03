# Phase 0 — language feature maturity

Every number and every verdict on this page was produced by a command printed
beside it, against `Sounio-lang/sounio` at `origin/main` @ `57f87da54f`, on
2026-09-02, on Linux x86-64 with `SOUNIO_SOUC_ENGINE=lean_single`.

The columns **"what it caught"**, **"false positives"** and **"what was
missing"** were deliberately left empty until the end. They are now FILLED, by
measurement rather than expectation -- see "The measured columns" below. A
feature whose measured value turned out to be zero is reported with zero.

---

## 1. Registry tiers, as declared

Source: `docs/serious-language/public-claim-registry.v1.tsv` (52 rows; columns
`claim_id / claim_level / closure_status / evidence_kind / evidence_ref /
spec_refs / public_wording`).

```sh
awk -F'\t' 'NR>1{printf "%-30s %-20s %s\n",$1,$2,$3}' docs/serious-language/public-claim-registry.v1.tsv
```

| claim_id | claim_level | closure_status |
|---|---|---|
| `units.measure` | **prototype** | **downgraded** |
| `ontology` | validated_research | closed |
| `epistemic.knowledge` / `.gum` / `.observe` / `.boundary` | validated_research | closed |
| `native_v2.qd128_core` / `native_v2.qd128_mul` | validated_research | closed |
| `generics.structs` / `.functions` / `.traits` | prototype | downgraded |
| `closures.lambdas` | **stale_conflicting** | downgraded |
| `refinement.types` | prototype | downgraded |
| `platform.linux_x86_64` | stable | closed |
| `formal.lean` | validated_research | closed |

Distribution over all 52 rows: 13 `stable`, 28 `validated_research`, 9
`prototype`, 1 `stale_conflicting`; 41 `closed`, 10 `downgraded`.

---

## 2. Where the registry and the measured behaviour disagree

### 2.1 `units.measure` — the tier understates, and the cited evidence is weak

The registry's evidence pointer for `units.measure` is
`tests/run-pass/units.sio`. That file says, in its own header:

```
// Units of measure test (simplified - full units not yet implemented)
```

and contains no units at all — plain `f64` arithmetic. On that evidence,
`prototype/downgraded` is generous.

The behaviour is better than the pointer suggests, but **not for the reason the
test corpus claims**. Measured:

```sh
./bin/souc check tests/compile-fail/unit_mismatch_call_arg.sio
```
→ `error: unit mismatch in call argument at <main>:11` — clean, single
diagnostic. Dimensional checking at call boundaries is real.

**Derived units work, and they are what makes the geochemistry typing possible:**

```sh
# unit molality = mol / kg;  unit molarity = mol / L;  then  b + c
./bin/souc check /tmp/uhsprobe/molal.sio
```
→ rc=1, with **two** diagnostics:
```
error: unit mismatch at <main>:11
error: unit dimension mismatch at <main>:11
```
Molality (`amount·mass⁻¹`) and molarity (`amount·length⁻³`) are distinct packed
dimensions and are refused against each other. **No new machinery is needed for
that case.**

### 2.2 Two corpus tests do not prove what their filenames say

```sh
./bin/souc check tests/compile-fail/unit_literal_clinical_reject_mass_as_amount_concentration.sio
```
→ rc=1, but the diagnostic is
```
error[E200]: undefined identifier `mg_dL` at <main>:13
```
The test asserts `//@ error-pattern: unit mismatch in call argument`. It fails
for **name resolution**, not for a dimensional reason: the literal form
`140.0<mg_dL>` is not implemented by any lexer in the tree (the implemented form
is the underscore suffix, `500_mg`). `unit_literal_suffix_reject_length_as_mass.sio`
emits `error[E200]: undefined identifier \`m\`` alongside the expected message.

These two are recorded here as a **defect in the test corpus**, not as evidence.
Nothing in this study cites them.

### 2.3 `ontology` — the gate named in the registry validates a different layer

The registry's evidence for `ontology` is
`scripts/ci/run_ontology_validation.sh`. That script exercises the weak nominal
taxonomy layer (`stdlib/ontology/reasoner.sio`: subclass closure, Wu-Palmer
similarity). The EL+ engine (`stdlib/ontology/elplus.sio`) is **not compiled by
it**; EL+ is gated separately at `.github/workflows/ci.yml:492-494`. One of the
six bundled sub-gates (`ontology_cli_smoke_gate.sh`) is a self-documented vacuous
gate whose subject function does not exist in the tree.

The registry's public wording — *"Claim rebuilt ontology validation surfaces
only"* — is load-bearing and correct. The tier is not wrong; the evidence pointer
points elsewhere.

### 2.4 There are two unit engines, and they disagree

| | `self-hosted/check/units.sio` (Madaros) | `self-hosted/compiler/lean_single.sio` (production) |
|---|---|---|
| dimension representation | `[i64; 7]` + rational scale | one packed `i64`, 4-bit two's-complement per exponent |
| derived units (`unit v = m / s;`) | absent | **present** |
| `f64<m/s>` annotations | absent | **present** |
| dim propagation through `*` `/` | computed, then discarded | **real** |

`g + kg` is an error on Madaros (nominal `unit_id` comparison) and accepted on
lean_single (packed-dim comparison, with a source comment permitting it).
Dual-engine divergence is tracked upstream as an open issue and is noted here
because any unit claim must say which engine it was measured on. **Everything on
this page was measured on lean_single.**

---

## 3. Feature-by-feature viability for this study

### U — units

| requirement | verdict | evidence |
|---|---|---|
| molality ≠ molarity | **works today** | §2.1, two diagnostics |
| specific (per-biomass) vs volumetric rates | expected to work (distinct dimensions) | to be measured during the port |
| m³ at STP vs m³ at reservoir | **GAP** | below |
| cal vs J | **GAP** — `cal` is not in the unit table; an unregistered name resolves to dimensionless, which switches checking off | |

The same-dimension brand case, measured:

```sh
# unit m3_stp = m / m;  unit m3_res = m / m;  takes_reservoir(stp)
./bin/souc check /tmp/uhsprobe/stp.sio
```
→ **rc=0, accepted.** `unit_call_arg_mismatch` short-circuits when the packed
dimensions are equal, so two distinct named units of the same dimension are
interchangeable at a call boundary. Under `+` the same pair yields only a
`warning: unit mismatch`, with the hard error coming from a generic numeric-type
rule rather than a unit rule.

This is the real gap for feature U, and the study needs it: the mission requires
gas volume to carry its condition (STP vs reservoir) in the type.

### O — ontology

- **O1/O2 — viable, with a hard ceiling.** Role composition (`r ∘ s ⊑ t`) is the
  strongest part of the module: rule `(RC) roleComp` is executable and is proved
  **sound and complete** in Lean with zero `sorry`. `hasElectronAcceptor ∘
  requiredBy ⊑ admits` is exactly that shape. Ceiling: **64 concepts, 8 roles, 8
  chains** in the dense engine; the sparse variant (4096 classes) cannot do
  composition at all. The 518 Thaysen strains therefore enter as **metabolic
  groups**, never as individual concepts.
- **O3 — not available as a feature.** There are no concrete domains / datatype
  properties in the DL, so "reference pressure = 1 bar" is not an expressible
  axiom. The missing extension is named precisely in `LANGUAGE_GAPS.md`.

### F — extended precision

`f128`/`f256` exist as **type descriptors only**. The compiler is explicit:

```
self-hosted/check/check.sio: "f128/f256 value conversion is not implemented; wide-float casts fail closed"
self-hosted/check/check.sio: "f128/f256 is reserved for compiler-owned format identity; source values are unavailable in V0-A"
```

That the language **fails closed** rather than silently degrading to `f64` is
itself a result worth recording, and it is the behaviour this study wants.

What does exist and is usable: `stdlib/math/qd128.sio`, a **quad-double**
(`Qd128 { x0, x1, x2, x3 }`, ~212-bit mantissa, `f64` exponent range) with
add/sub/mul/div/sqrt/abs/cmp and its own CI gates. It has **no transcendentals**,
which the abiotic model requires (Van't Hoff, Arrhenius, log K, pH).

---

## 4. Environment

```
Linux x86-64, 64 cores, 188 GB RAM
Sounio: origin/main @ 57f87da54f, SOUNIO_SOUC_ENGINE=lean_single
make build (boot → gen1 → gen2 → gen3 + fixed point): 6 s, ✓ FIXED POINT OK (0f3aa2c9dd3be4e407ce546130f7614c)
one self-compile of self-hosted/compiler/lean_single.sio (39,372 lines): 2 s
test corpus: 1692 tests/run-pass + 289 tests/compile-fail; ~0.22 s/file
```

---

# The measured columns

Filled at the end of the study, by running the compiler against errors a
scientist would actually make — not by expectation. Every row below is a
command that was run, against `gen3.elf` md5 `0f3aa2c9dd3be4e407ce546130f7614c`.
**A feature whose measured value is zero is reported with zero.**

## 3. What each feature caught, measured

| feature | tier | what it caught **(measured)** | false positives | what was missing |
|---|---|---|---|---|
| **effects** (`with IO, Mut, …`) | stable | **Works.** Calling `println` from a function declared `with Mut` only → `error[E035]: effect not declared in function signature`. The one advertised guarantee that fired in a live test. | **0 observed** | nothing found |
| **units — dimensional form** `unit molal = mol / kg;` | prototype / downgraded | **Works, and is stronger than the registry implies.** Passing a `molar` value to a `molal` parameter → `error: unit mismatch in call argument`. Mixing in arithmetic → `error: arithmetic operands must have matching numeric types`. | **0 observed** | **same-dimension, different-scale is NOT caught** at a call boundary: `mg` → `kg` passes silently (G4/G5). The `1.0<molal>` literal form does not parse — it is read as the comparison `1.0 < molal`, yielding `expected f64, got bool`. |
| **units — nominal form** `unit molal;` | prototype / downgraded | **Weaker.** Catches arithmetic mixing, but a `molar` passed to a `molal` parameter **compiles**. | 0 observed | call-boundary dimension checking, which the dimensional form has |
| **reaction literals** (`feat/w5-reaction-literals`, unmerged) | — | **Works.** `error[E188]: reaction 'obvious_imbalance' does not balance for element O`. | 0 observed | **not on `main`** — unavailable to this study |
| **epistemic** (`Knowledge<T>`, GUM) | validated_research | **0.** Every compile printed `knowledge_subtype: 0 sites, 0 violations` and `knowledge_units: 0 sites with dimensional Knowledge<T>`. Nothing was expressed in it, so nothing was checked. | 0 | monomorphic (f64 only), so the study's `f64`-typed chemistry could not adopt it without rewriting |
| **refinement types** | prototype / downgraded | **0 caught.** `refined: 29 in 2 passes` appears in every compile; no refinement rejected anything in this study. | 0 | — |
| **qd128 / f128** | validated_research | **0.** Not used for any reported number. | 0 | **no native IEEE binary128 with source values** (G1). The instrument is quad-double, ~212-bit mantissa, which is *not* binary128 and is declared as such. |
| **EL+ ontology** | validated_research | **0.** Not used in any computation. | 0 | no concrete domains (G2); silent truncation and a verified out-of-bounds write (G3) |
| **closures** | stale_conflicting | **0.** None exist; every callback is a named `fn`. | 0 | cost readability throughout; caught nothing |

## 4. The uncomfortable result, stated plainly

**The type system caught nothing in this study — because the study did not use
it.** Every module's signatures are bare `f64`. `sio/h2_solubility.sio` declares

```sio
unit molal = mol / kg;
unit molar = mol / L;
```

— the **strong, dimensional** form, measured above to catch exactly the
mg/L-of-solution versus mmol/kg-of-water confusion the module's own header warns
about — and then annotates **no value with either**. Its own header claimed *"the
checker refuses to substitute one for the other."* **Measured: it would have, and
it was never asked to.** The declarations were decorative. That is recorded as
`CORRECTIONS.md` C20, because it is the study overclaiming a feature, not the
feature failing.

## 5. What actually cost time: three compiler defects

| defect | how it surfaced | cost |
|---|---|---|
| **G9** — module-level `let` of a negative `f64` evaluates to the wrong value | the Chabab port disagreed with its C++ replica as `exp(0.00947·m²)`; traced to `let ZETA: f64 = 0.0 - 0.00947` reading as **0** | one debugging cycle; forced every negative constant in the study to be `var`. **Re-confirmed present at the end of the study.** |
| **G8** — `bin/souc` silently falls back to a committed seed binary that predates the source | a compiler change appeared to have no effect; it had, but was not being run | one false "feature is inert" conclusion, reported to the user and retracted |
| **G11** — a string literal of **127–199** characters passed as a function argument segfaults; 126 works, 200 works | the deliverable figure crashed after writing 71 correct lines | one debugging cycle and one wrong first diagnosis, committed to a comment before being corrected |

**None of the 19 corrections in `CORRECTIONS.md` could have been caught by a type
system.** They are provenance failures, arithmetic inconsistencies inside
published sources, and one rule violation of this study's own. What the type
system is good at is not what went wrong.

**The inverse did happen, twice.** G9 was found *by* the study's
cross-validation discipline — a Sounio module disagreeing with an independent
C++ implementation — not by any compiler check. The science caught the language's
bug, not the other way round.

## 6. The measured recommendation

For the stated goal that everything be carried on units and ontologies, the
measurement says the mechanism is closer than the registry's `prototype /
downgraded` tier suggests, and the obstacles are specific:

1. **Use the dimensional form.** `unit x = mol / kg;` catches call-boundary
   mismatches; `unit x;` does not. The tier does not distinguish them; this
   measurement does.
2. **G4/G5 is the gap that matters most for chemistry** — `mg` and `kg` are
   interchangeable at a call boundary, and casts do not convert scale. Dose
   arithmetic is exactly where that bites.
3. **The literal syntax is broken** (`1.0<molal>` parses as a comparison), so
   values must be introduced by annotation, `let x: molal = 3.5`. That is
   workable and was simply not known when this study's modules were written.
4. **`Knowledge<T>` is monomorphic**, so uncertainty cannot ride on the same
   values as units without the struct-generics work.

None of that is a reason the study's modules are bare `f64`. They are bare `f64`
because nobody checked what the feature could do before deciding not to use it —
which is the same failure mode, in miniature, as inheriting a number without
reading its source.
