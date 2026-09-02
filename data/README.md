# Data

Nothing enters this directory without an entry in `SOURCES.tsv`. `MANIFEST.tsv`
is generated from disk by `tools/build_manifest.py` — hashes are computed, never
transcribed — and `--check` verifies the committed manifest still matches the
tree.

**The manifest fails closed, and that was demonstrated rather than asserted:**

```sh
# an unsourced file
mkdir -p data/orphan-test && echo unsourced > data/orphan-test/nowhere.txt
python3 tools/build_manifest.py
#   FAIL	no source entry for data/orphan-test/nowhere.txt   (exit 1)

# a single appended byte
printf 'x' >> data/usgs-auxvases/data_dictionary.csv
python3 tools/build_manifest.py --check
#   FAIL: data/MANIFEST.tsv does not match disk               (exit 1)
```

Both refused; both passed again once reverted.

---

## `usgs-auxvases/` — USGS microbial reaction model archive

DOI `10.5066/P13GJC6Y` · **CC0-1.0** (public domain dedication) · published
2026-05-29 · retrieved 2026-09-02 · 14 files, all present, every declared size
matched on download.

Batch-reaction simulations built with **React** (Geochemist's Workbench) for
formation water of the Mississippian **Aux Vases** Formation, Illinois Basin
(sample EOR-B106). Two runs, 365 days and 12 700 years.

**The two input scripts are identical except for the time horizon and the output
suffix** — verified by `diff`: chemistry and kinetics are the same in both.

### Initial condition, as stated in the script

| | value |
|---|---|
| temperature | 28.2 °C, isothermal |
| porosity | 0.2 |
| pH | 6.1 |
| basis | 1 free kg H₂O, charge balance on Cl⁻ |
| Cl⁻ | 76 000 mg/L |
| Na⁺ | 45 010 mg/L |
| Ca²⁺ | 3 600 mg/L |
| Mg²⁺ | 1 540 mg/L |
| SO₄²⁻ | 750 mg/L |
| K⁺ | 216 mg/L |
| **HCO₃⁻** | **130 mg/L** |
| SiO₂(aq) | 6.2 mg/L as Si |
| HS⁻ | 1 × 10⁻⁶ mg/L |
| TDS | 127 744 |
| H₂(g) | fugacity 91 |
| CH₄(g) | fugacity 1 × 10⁻⁶ |

### Kinetics, verbatim from the script

Sulfate reduction — `4 H2(aq) + H+ + SO4-- -> HS- + 4 H2O`

```
biomass = 1e-6   rate_con = 4.27e-14   KA = 3.9e-5   KD = 1.1e-6
mpower(H2(aq)) = 1   mpower(SO4--) = 1
ATP_energy = -46.75   ATP_number = 1   growth_yield = 5000
order1 = .1666666666667   order2 = 0
```

Methanogenesis — `HCO3- + 4 H2(aq) + H+ -> CH4(aq) + 3 H2O`

```
biomass = 1e-6   rate_con = 2.88e-14   KA = 0   KD = 4.7e-6
mpower(H2(aq)) = 1   mpower(HCO3-) = 1
ATP_energy = -45   ATP_number = .25   growth_yield = 1250
order1 = .5   order2 = 0
```

### Two observations that matter for this study

**1. `KA = 0` for methanogenesis.** The half-saturation constant on the electron
*acceptor* — bicarbonate, i.e. the CO₂ source — is **zero** in the reference
model, while the sulfate reducer carries `KA = 3.9e-5`. So the archived model
does **not** impose Monod saturation on the carbon source; the HCO₃⁻ dependence
enters only through the rate-law power `mpower(HCO3-) = 1`.

This is directly relevant to **H1a**, which asserts that calcite-derived CO₂
*limits* methanogenesis. A coupled model that adds CO₂ limitation is therefore
adding a term the oracle does not contain. This is recorded now, before any
model is written, so the difference cannot later be mistaken for agreement or
for a porting error. It is not a defect in the USGS model — it is a modelling
choice, and it defines precisely where our model departs from its oracle.

**2. The concentration scale changes between input and output.** The script
states concentrations in **mg/L** (per litre of *solution*); the data dictionary
declares every aqueous output in **mmol/kg** — *molality*, per kg of water — and
calls them **activities**, not concentrations. At TDS ≈ 127 744 the solution
density is well above 1 kg/L, so mg/L and mg/kg differ by several percent, and
activity coefficients at this ionic strength are far from unity. Neither the
density used for the conversion nor the activity model is stated in the script;
both come from the `thermo.tdat` database.

This is the molality-vs-molarity hazard the unit typing is meant to catch, and
it is a live one in the reference data.

### Convention axioms

Recorded per source in `SOURCES.tsv`. For this one: `thermo.tdat` (GWB, with
`verify`), `conductivity-USGS.dat`, redox **fully decoupled** (`decouple ALL`),
28.2 °C isothermal, 1 free kg H₂O basis, input in mg/L of solution, output in
mmol/kg molality as activities, gas as **fugacity** rather than partial
pressure.

**One axiom is undeclared in the source**: the units of `ATP_energy` (−46.75 and
−45). The GWB convention is presumed to be kJ/mol, but the script does not say
so and this has **not** been verified. It is carried as undeclared, and is a
candidate case for the ontology convention check.

### What cannot be claimed

Geochemist's Workbench is not available here, so **React cannot be run**. Parity
in Phase 3 is claimed against the **archived output files** only. No claim of
parity with GWB itself is made anywhere in this repository.

---

## `thaysen-strains/` — environmental growth envelopes

DOI `10.17632/4dksb2x4zn.1` · **CC BY** · published 2021-05-11 · retrieved
2026-09-02 · Thaysen & Strobel · one workbook, 192 652 bytes, size matched on
download.

Three sheets: `all info`, `R input microbes strobel comple`, `IRB + uncertain`.
Extracted with `tools/xlsx_to_tsv.py`, a stdlib-only reader written rather than
pulled in as a dependency, which refuses any workbook part carrying a DTD or
entity declaration.

**518 strains, counted rather than inherited** — 286 SRB, 144 methanogens (METH),
88 homoacetogens (ACET):

```sh
python3 tools/xlsx_to_tsv.py data/thaysen-strains/*.xlsx --sheet "all info" > allinfo.tsv
python3 tools/thaysen_envelopes.py allinfo.tsv
```

Columns give, per strain, optimum and lower/upper critical values for
temperature, salinity and pH, plus pressure, references and a link. Salinity is
declared **g/L** and pressure **MPa**; the **temperature unit is not declared in
the header** and is carried as undeclared. The solute basis for salinity is also
not declared — it is not stated whether "Salt" means NaCl-equivalent or total
dissolved salts, which matters when it is compared against a formation water
reported as TDS.

### Six rows contradict their own bounds

```sh
python3 tools/thaysen_consistency.py allinfo.tsv     # exit 1
```

The rule needs no outside knowledge and imputes nothing: a strain's reported
optimum must lie between that same strain's own lower and upper critical values.
Of 706 such assertions, **6 fail**:

| group | strain | field | low | OPT | up | reference |
|---|---|---|---|---|---|---|
| SRB | *Desulfonatronum buryatense* Su2 | Salt | 2 | **1** | 100 | Ryzhmanova et al. 2013 |
| METH | *Methanofollis aquaemaris* | Salt | — | **52.6** | 5.84 | Imachi et al. 2009 |
| METH | *Methanogenium tatii* | Salt | 0 | **44173** | 70 | Zabel et al. 1984 |
| METH | *Methanosarcina spelaei* | pH | 4.1 | **66** | 9.9 | Ganzert et al. 2014 |
| METH | *Methanobacterium aarhusense* | pH | `7,5-8` | **9** | **5** | Ma et al. 2005 |
| METH | *Methanotorris igneus* | Salt | — | **78** | 54 | Takai et al. 2004b |

Two of these are also physically impossible on their face (pH 66; 44 173 g/L,
some 120× halite saturation). The other four are not — they are only detectable
against the row's own bounds, which is why that is the rule used. The
*M. aarhusense* row is worse than an outlier: its upper bound (5) is **below** its
lower bound (`7,5-8`, itself a comma-decimal range in a numeric field).

**These values are excluded and named. None is repaired.** An optimum that is
needed must come from the primary reference the row itself cites, and would enter
as a separate, separately-sourced datum. Silently clipping them to the bound
would have widened three group envelopes with fabricated numbers.

### A third of optima are not numbers

**369 optimum cells are non-numeric** — ranges such as `40-45`, or comma
decimals. The extractor refuses to parse these into a single value rather than
guessing a midpoint. Any statistic over optima therefore covers a subset, and the
count of what was skipped is reported alongside it.

### Pressure is not obtainable for two of three groups

The mission asks for envelopes in T, salinity, pH **and pressure**. Measured
coverage: pressure is reported by **3 SRB strains** (optimum) and **2** (tolerance),
and by **no methanogen and no homoacetogen at all**. A pressure envelope per group
cannot be built from this source. Reported as a gap, not filled from elsewhere.

### Envelopes as computed

Group envelope = the union of its strains' reported tolerance ranges, which is
the quantity the ontology's subsumption-derived envelope must reproduce in test
O1. Computed **before** exclusions, so the two defective salinity/pH maxima above
still inflate the METH row; the post-exclusion table is regenerated when the
envelopes are consumed.

| group | field | n reported | n missing | envelope low | envelope high |
|---|---|---|---|---|---|
| ACET | Temp | 56 | 32 | −2.5 | 72 |
| ACET | Salt (g/L) | 16 | 72 | 0 | 250 |
| ACET | pH | 60 | 28 | 3.6 | 10.7 |
| METH | Temp | 128 | 16 | 0 | 122 |
| METH | Salt (g/L) | 103 | 41 | 0 | 200 |
| METH | pH | 112 | 32 | 4.1 | 10.2 |
| SRB | Temp | 220 | 66 | −2 | 113 |
| SRB | Salt (g/L) | 165 | 121 | 0 | 250 |
| SRB | pH | 184 | 102 | 1 | 11.5 |

Missing counts are large — salinity is absent for 72 of 88 homoacetogens and 121
of 286 sulfate reducers — so an envelope is the range of what *was reported*, not
of the group.
