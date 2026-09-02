# Chemical syntax and canonical ontologies

A design note produced by building this model, not before it. Every claim below
is grounded in a defect that actually occurred during Phase 1 and Phase 2, with
the measurement that exposed it.

---

## The observation

Not one of the defects found while assembling this study was arithmetic. Every
one was a **meaning that was not attached to its number**.

| defect | what it was | where it is recorded |
|---|---|---|
| redox coupled vs decoupled → **~400×** in calcite dissolved | a convention living in a `.dat` file, invisible to any type | `oracles/README.md` |
| three different `phreeqc.dat`, three different hashes | identity by path is not identity | `oracles/README.md` |
| `ATP_energy = -46.75` with no unit stated | undeclared convention | `data/SOURCES.tsv` |
| mg/L in, mmol/kg out, density conversion implicit | undeclared concentration scale | `data/README.md` |
| carbonate mechanism order is in **P(CO₂)**, not H⁺ | a different footnote from every other mineral class | `data/README.md` |
| `Temp_OPT` with no unit in the header | undeclared convention | `data/README.md` |

Six instances, one shape. **The numbers were right; their meanings were not
carried with them.** A language whose subject is chemistry should make that
shape unrepresentable, and none of it needs the compiler to be cleverer at
arithmetic.

A second observation, from writing the oracle harness. This reaction —

```
4 H2(aq) + H+ + SO4-- -> HS- + 4 H2O
```

— is, in the reference model this study must reproduce, **a string in a text
file**, parsed at run time. Nothing checks that it balances in mass or in charge
until it runs, and if it does not balance, what comes back is a wrong number
rather than an error. In a language written for a chemical engineer that should
be a typed term with its balance checked at compile time.

---

## Three proposals, in order of what the current compiler can carry

### 1. Reaction literals, balance checked at compile time — **built**

```sio
reaction sulfate_reduction {
    4 H2(aq) + H+ + SO4-- -> HS- + 4 H2O
}

reaction methanogenesis {
    HCO3- + 4 H2(aq) + H+ -> CH4(aq) + 3 H2O
}
```

The checker verifies, per reaction:

- **mass balance**, element by element;
- **charge balance**, left against right;
- **every element symbol is a real element symbol** — an unrecognised symbol is
  an error, never a silent zero. Without this a typo in a species name would let
  a reaction appear to balance.

Charge is written the way chemists and both reference engines write it —
trailing `+`/`-`, repeated for multiplicity (`SO4--`, `Ca++`) — so a reaction can
be transcribed from a paper or a GWB script without re-encoding. Phase is part of
the species, not decoration: `H2(aq)` and `H2(g)` are different species, which the
redox result above makes non-negotiable. A parenthesised group that is *not* a
phase is a multiplier group, so `CaMg(CO3)2` counts C 2 and O 6.

Implemented diagnostics, quoted as the compiler actually emits them — these are
the verbatim outputs of the three tests in `tests/compile-fail/reaction_*.sio` in
the Sounio repository.

The negative control is sulfate reduction, which balances exactly, with one
coefficient perturbed from `4 H2O` to `3 H2O`:

```
error[E188]: reaction `sulfate_reduction_perturbed` does not balance for element H at <main>:1 (bundle line 1)
  = note: element H: left 9, right 7
error[E188]: reaction `sulfate_reduction_perturbed` does not balance for element O at <main>:1 (bundle line 1)
  = note: element O: left 4, right 3
```

Dropping one water removes 2 H and 1 O, and the diagnostic says exactly that: it
names every element that fails and reports both counts. Sulfur still balances and
is not reported. A checker that said only "does not balance" would leave the
author to find the missing atom themselves, which is most of the work.

Charge, isolated from mass — iron is conserved, so no `E188` fires:

```
error[E189]: reaction `iron_oxidation_unbalanced` does not balance in charge at <main>:1 (bundle line 1)
  = note: charge left 2, right 3
```

An unrecognised element symbol:

```
error[E190]: unknown element symbol `Xx` in reaction `bogus_element` at <main>:2 (bundle line 2)
```

**What this deliberately does not claim.** It checks conservation, nothing more.
It does not check that a reaction is thermodynamically favourable, that it is
elementary, that its direction is right, or that its rate law is sound. A
balanced reaction can still be wrong chemistry. Claiming otherwise would be the
same category of error this document exists to prevent.

**Status: built.** `unit` was already a soft keyword registered in Pass 0a of the
production engine and skipped in the main pass, with `tc_linear_violation`
emitting the diagnostic. `reaction` follows that precedent exactly: registered in
Pass 0a of `self-hosted/compiler/lean_single.sio`, where the balance is checked,
and skipped by the main compile pass. Formula parsing and integer balance
arithmetic needed nothing the language did not already have.

That `reaction` is a *soft* keyword is a compatibility guarantee, not a detail.
It introduces a declaration only in the position `reaction NAME {` and stays an
ordinary identifier everywhere else, so `fn reaction(...)`, `let reaction = 7`
and `reaction + 1` all still compile. This is pinned by
`tests/run-pass/reaction_soft_keyword_shadowing.sio`, so the feature is not a
breaking change for programs that already use the word.

### 2. Species identity from the ontology, not from the formula — **fits what exists**

Calcite and aragonite are both CaCO₃. They have different solubility products.
Nothing today stops one being passed where the other is meant, because to a type
system built on formulas they are the same thing.

```sio
mineral Calcite   : CaCO3 is Carbonate, Trigonal
mineral Aragonite : CaCO3 is Carbonate, Orthorhombic
```

with subsumption doing the work: both are `Carbonate`, so a function over
carbonates accepts either, while a function that needs calcite's rate parameters
accepts only calcite. This is EL+ subsumption, which is implemented, gated, and
**proved sound and complete in Lean with zero `sorry`**. The ceiling is 64
concepts / 8 roles / 8 chains in the dense engine, which is comfortable for a
mineral set and is why the 518 Thaysen strains enter as metabolic groups rather
than as individuals.

It also gives Palandri–Kharaka's two dolomites — sedimentary disordered and
hydrothermal ordered, differing by ~12× in the neutral mechanism — distinct
identities instead of one ambiguous name.

### 3. Conventions as axioms attached to the value — **blocked on a known gap**

This is the one that would have caught the 400×.

```sio
// the database declares what it assumes
let db = Database::load("phreeqc.dat", sha256: "59373961...")
    // declares: redox = Coupled, concentration = Molal, gas = Fugacity

// the model declares what it requires
fn abiotic_h2_loss(db: Database<RedoxDecoupled>, ...) -> Knowledge<mol> { ... }
```

Passing a `Coupled` database to a model that requires `Decoupled` is refused
**before anything runs**. Same for reference pressure 1 bar against 1 atm, for
molality against molarity, and for an energy in cal against one in J.

**This is exactly the O3 requirement, and it is exactly what is missing.** The
EL+ engine has no concrete domains and no datatype properties, so "reference
pressure = 101325 Pa" is not an expressible axiom. The extension needed is
EL⁺⁺ with a p-admissible concrete domain; the minimal sufficient fragment is
equality on rational constants, which is trivially PTime and vacuously convex, so
the completion algorithm survives. That work is scoped in `LANGUAGE_GAPS.md` G2.

**The redox result is the strongest argument for it that has appeared**, because
the cost is measured rather than hypothetical: a factor of ~400 in a number that
would have gone into a report with nothing to flag it.

---

## What this is not

A wish list. Item 1 is implemented, with the diagnostics quoted above taken
from its tests; item 2 rests on
machinery that already exists and is verified; item 3 names precisely which
extension it needs and why the current profile cannot carry it. Where an item is
blocked, it says so and says what it would cost.

If item 1 turns out to catch nothing during the port of this model, that is
reported as catching nothing. A feature's value here is measured at the end, not
claimed at the start.
