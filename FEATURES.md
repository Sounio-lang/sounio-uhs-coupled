# Phase 0 — language feature maturity

Every number and every verdict on this page was produced by a command printed
beside it, against `Sounio-lang/sounio` at `origin/main` @ `57f87da54f`, on
2026-09-02, on Linux x86-64 with `SOUNIO_SOUC_ENGINE=lean_single`.

The columns **"what it caught"**, **"false positives"** and **"what was
missing"** are deliberately empty. They are filled by measurement at the end of
the study, not by expectation at the start. A feature whose measured value turns
out to be zero is reported with zero.

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
