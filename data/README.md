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
