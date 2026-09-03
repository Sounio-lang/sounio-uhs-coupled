# Corrections

`PREREGISTRATION.md` is frozen. Its own rule says a defect in it becomes a new
section elsewhere, naming the file and the defect, never an edit. This is that
file; `RESULTS.md` will reference it once results exist.

Everything here was found during Phase 1 data acquisition, **before any model was
written**. Each entry says what was believed, what the source actually says, and
what it changes.

---

## C1 — The Sun Storage site is Lehen, not Gampern

**Believed:** the Hellerschmied field trial reservoir is at Gampern, Upper
Austria. This appeared in an early scoping note in this project.

**Source:** the word "Gampern" **does not appear anywhere in the paper**. Methods,
"Test reservoir": *"a DHR named **Lehen** (48° 01′ 45.0″ N 13° 41′ 29.6″ E,
**Unterpilsbach**, Austria)"*.

The error came from a public page about the wider Underground Sun Storage
programme, which describes a different reservoir. **Provenance of a site name
must come from the paper, not from a project website.**

Verified reservoir parameters (Hellerschmied et al. 2024, Methods):

| | |
|---|---|
| lithology | porous **2 m thick sandstone**, Hall formation, Upper Austrian Molasse Basin |
| depth | **1027 m** TVD |
| temperature | **40 °C** |
| pressure during trial | 35.1 → 78.4 bar abs. injection; 28.3 bar final |
| brine volume | 40 000 m³ (estimated) |

The lithology finding that drove the H1a/H1b split in `PREREGISTRATION.md`
**stands** — sandstone, confirmed from the paper itself. Only the place name was
wrong.

---

## C2 — 0.26 mmol L⁻¹ h⁻¹ is not a measured mesocosm rate

**Believed:** "mesocosm rate 0.26 mmol/L/h", as a measurement.

**Source:** Discussion — *"the calculated average MER (equation (7)) of 0.26 mmol
l⁻¹ h⁻¹ **for the test reservoir operated at mesocosm productivity**"*. Methods
defines `MER = Δn_CH4 / (Δt · V_b)` with **V_b = 40 000 m³ of Lehen reservoir
brine** and Δn_CH4 from the ideal gas law at 22.4 L/mol.

So it is a **methane production rate**, computed by extrapolating mesocosm
productivity onto the field reservoir's brine volume. It is neither a mesocosm
measurement nor a hydrogen rate. Any use of it as a measured kinetic datum would
be wrong.

---

## C3 — 84.3 % does not reproduce from the table it cites, and F2 depends on it

**Believed:** 84.3 % H₂ recovery at 285 days is a directly reported measurement.
`PREREGISTRATION.md` criterion **F2** is stated in terms of that number.

**Source:** main text — *"We successfully recovered 84.3% of the injected H₂
(Supplementary Table 2)."* Supplementary Table 2 gives injection **118 196 Sm³**
and production **99 754 Sm³**.

Computed here:

```sh
python3 -c "print(99754/118196*100, 99754/119353*100)"
```

| quotient | result |
|---|---|
| 99 754 / 118 196 (metered injection) | **84.397 %** |
| 99 754 / 119 353 (abstract figure) | **83.579 %** |
| claimed | **84.3 %** |

Neither reproduces 84.3 %. The paper does not show the arithmetic.

**Consequence for F2.** The criterion is retained as written — the
pre-registration is frozen and is not tuned after the fact. But the target is
recorded as an **interval, 83.58 % – 84.40 %**, spanning the two defensible
quotients, and the model band is judged against that interval as well as against
the literal 84.3 %. Both verdicts are reported. Judging only against a figure
that its own source table does not reproduce would be false precision.

---

## C4 — Two injected-volume figures differ by ~1 %, unreconciled

| value | where |
|---|---|
| 119 353 m³ | **abstract only** |
| **118 196 Sm³** | **Supplementary Table 2, column `Volume [Sm3]`** — metered |

119 353 is a nominal recomputation: 9.89 % × 1 206 802 = 119 352.7 (verified
here). The metered figure is 9.7942 % of the same total. Difference **1157 Sm³
(0.98 %)**. The paper never reconciles them. The **metered** value is used, and
the abstract value is recorded as derived.

Related, smaller: annual CH₄ yield is 114 648 m³ in the abstract and 114 646 m³
in the Discussion.

**Volume basis is resolved, and favourably.** Methods, "Field trial operation":
*"All volumetric values are given in standard m³."* Supplementary Table 2 labels
its column `[Sm3]`. So **standard conditions, not reservoir conditions** — which
is exactly the distinction the unit typing is meant to carry, and it is now
sourced rather than assumed.

---

## C5 — Lobodice gas composition: three of five components were wrong

`PREREGISTRATION.md` records the Lobodice confounder as town gas carrying
"~10 % CO₂ + ~10 % CO".

**Source** (Tremosa, Jakobsen & Le Gallo 2023, *Front. Energy Res.* 11:1145978,
CC BY, quoting Šmigáň 1990 and Buzek 1994 jointly):

> *"The stored gas initially composed of 54% H2, 22% CH4, 12% CO2, 9% CO and
> 2.5% N2 evolved after being stored during 7 months to 40% CH4, 37% H2, 9% CO2,
> 9% N2 and 3% CO."*

| component | pre-registered | actual |
|---|---|---|
| H₂ | 54 → 37 % | 54 → 37 % ✓ |
| CH₄ | ~30 % | **22 → 40 %** |
| CO₂ | ~10 % | **12 → 9 %** |
| CO | ~10 % | **9 → 3 %** |
| N₂ | ~2 % | **2.5 → 9 %** |

**The confounder itself is unaffected and if anything strengthened**: 12 % CO₂
plus 9 % CO was co-injected, so the carbon available to methanogenesis at
Lobodice did not have to come from calcite. The numbers are corrected here.

**Citation chain.** Buzek et al. 1994 (*Fuel* 73:747–752) and Šmigáň et al. 1990
(*FEMS Microbiol. Lett.* 73:221–224) are **both inaccessible** — ScienceDirect
and OUP return HTTP 403, Unpaywall reports `closed`, no OA copy. The secondary
cites them **jointly**, so the 54 → 37 % figure **cannot be attributed to Buzek
1994 specifically** from anything readable. It is recorded as
`secondary-quoted`. The dates of the 7-month episode are **not stated in any
accessible source**; only the 1965–1991 operating window is known.

---

## C6 — Lehen salinity was never reported, and the model needs it

H₂ solubility in brine (Chabab) is a function of salinity. For Lehen there is
**no TDS, no salinity figure, and no sodium measurement**. Supplementary Table 8
lists Ca, Co, Cu, Fe, K, Mg, Mn, Mo, Ni, P, S, Zn, TN — **Na is absent**.
Electrical conductivity was measured (Suppl. Note 4, HACH CDC401) but **the value
is never reported**.

What does exist: Cl⁻ **7487 mg L⁻¹**, pH **8.7**, acetate 206 mg L⁻¹, NO₃⁻ 11.5,
SO₄²⁻ below quantification (Suppl. Table 9, filtered brine, day −13).

**Consequence.** Lehen ionic strength cannot be taken from the paper. Either it
is carried as an uncertain parameter bounded by the Cl⁻ measurement, or the site
is modelled at a stated assumed salinity that is labelled as an assumption. It is
**not** filled in from a nearby field. Recorded as a gap.

---

## C7 — Ghaedi's result cuts against the study's own framing

The mission framed Ghaedi et al. 2025 as: calcite dissolution is *abiotically
negligible above ~70 °C*. The abstract (publicly visible via Crossref) says that,
and also says the converse:

> *"considerable hydrogen consumption due to geochemical reactions could occur at
> low temperatures (25–50°C)"*

**Both field sites are inside that band** — Lehen 40 °C, Lobodice 25–45 °C.
Neither is above 70 °C. So the regime where geochemical H₂ consumption is claimed
to be *considerable* is precisely the regime this study validates in.

This matters for **F3** (if the abiotic band alone encompasses both field points,
the coupling is unnecessary). Ghaedi's own abstract predicts a non-negligible
abiotic contribution at our temperatures, which makes F3 a live possibility
rather than a formality. Recorded before any model was run.

Three further qualifications on that source:
1. The word **"abiotic" never appears** — the claim is about *"geochemical
   interactions in the hydrogen-brine-calcite system"*.
2. It is a **modelling** result (PHREEQC kinetic simulations) explicitly
   conditioned on *"this experimentally calibrated model"*, not a direct
   measurement.
3. Also stated: *"the overall hydrogen loss due to dissolution in the brine is
   generally far less than 1% (molar)"*.

Paywall re-confirmed independently (Wiley and doi.org both HTTP 403). The
calibrated analytical K(T) expression is **not publicly available** — confirmed,
not assumed. The Van't Hoff reconstruction path in `PREREGISTRATION.md` stands.

---

## C8 — The abiotic validation source changes

**Believed:** Fatah & Al-Yaseri 2024 and Monash/CSIRO "Yarrabee" work would
supply an abiotic limestone–H₂ time series for Phase 2.

**Found:** all four relevant Al-Yaseri/Fatah papers are `oa_status: closed` with
no OA location; ACS and ScienceDirect return HTTP 403; Crossref carries no
abstract for the two most relevant. **"Yarrabee formation" is not confirmed** —
no matching record found. The premise is unverified and nothing was inferred
from it.

**Replacement, and it is better.** Hellerschmied et al. 2024's own **abiotic
controls** are an open abiotic rock–H₂ time series under **CC BY 4.0** with
numerical data: Source Data Fig. 5, sheet `Fig5A_pabio`, **10 abiotic cycles ×
10 days, 482 points each**, alongside 14 biotic cycles. Cores are sandstone whose
XRD (Suppl. Table 7) is **8 % calcite + 20 % dolomite = 28 % carbonate**, at
40 °C, ~40–45 bar, in reservoir brine.

Reproduced here from the source file:

```sh
python3 tools/xlsx_to_tsv.py data/hellerschmied-lehen/Hellerschmied2024_SourceData_Fig5_pressure.xlsx --sheet Fig5A_pabio > pabio.tsv
python3 tools/xlsx_to_tsv.py data/hellerschmied-lehen/Hellerschmied2024_SourceData_Fig5_pressure.xlsx --sheet Fig5A_pbio  > pbio.tsv
python3 tools/abiotic_check.py pabio.tsv pbio.tsv
```

| | cycles | loss at day 10 | mean p_t/p₀ |
|---|---|---|---|
| abiotic | 10 | **1.22 % – 2.20 %** | 0.9848 |
| biotic | 14 | **10.54 % – 11.62 %** | 0.8905 |

**Caveat, load-bearing:** this is **total pressure, not speciated H₂**. It bounds
total abiotic gas loss; it does **not** isolate calcite dissolution. The paper
states *"We observed no conversion in the abiotic controls."*

A labelling inconsistency in the source, recorded: Fig. 5's caption names the
abiotic controls **M4 and M5**, while the Suppl. Data 1 sheet is
`Pressure_data_M4_M6` with columns **M5 and M6**.

---

## C9 — Chabab: wrong year, wrong author list

The brief cites "Chabab et al. 2023" with an author list that belongs to a
**different paper**.

| | DOI | authors |
|---|---|---|
| intended target | **10.1016/j.ijhydene.2023.10.290** (IJHE 50:648–658; online Nov 2023, print Jan 2024) | Chabab, Kerkache, Bouchkira, Poulain, Baudouin, Moine, Ducousso, Hoang, Galliero, Cézac |
| the brief's list | 10.1016/j.ijhydene.2020.08.192 (IJHE 45:32206–32220, 2020) | Chabab, Théveneau, Coquelet, Corvisier, Paricaud |

The 2020 paper is also the one **Hellerschmied et al. cite** for H₂ solubility
(their ref. 36).

The version of record is paywalled (ScienceDirect 403). The **accepted
manuscript** is green OA on HAL (`hal-04623907`, HTTP 200) under the HAL
depositor authorisation — **not** a Creative Commons licence, and pre-copyedit.
Coefficients read from it are labelled as accepted-manuscript values pending a
version-of-record check.

**The 2023 paper states no explicit validity envelope.** Its measurements span
T 298–373 K, P ≤ 200 bar, m_NaCl 0–4 mol/kgw, and the 200 bar ceiling is an
autoclave limit rather than a physical one. Any envelope quoted is therefore
**inferred from the fitting range, not stated by the authors**, and is labelled
so. The 2020 paper does state ranges explicitly.

The e-NRTL model's τ and α parameters are **not tabulated anywhere** — they live
in a commercial software database. Recorded as a gap.

---

## C10 — "Yarrabee formation" does not exist, and the Al-Yaseri paper is not what the brief describes

**Believed:** Fatah & Al-Yaseri 2024 plus Monash/CSIRO work on the "Yarrabee
formation" would supply an abiotic limestone–H₂ time series.

Three separate errors in that premise:

**1. There is no Yarrabee formation.** The only formal Australian unit of that
name is the **Yarrabee Tuff**, an upper Permian volcanic ash marker bed in the
Bowen Basin, Queensland — not a limestone, not a carbonate, not a storage target.
No matching record was found for a Yarrabee-formation H₂ storage dataset. Any
protocol text referring to it should be struck.

**2. Author order and institution.** The paper is **Al-Yaseri, Fatah**, Alsaif,
Sakthivel, Amao, Al-Qasim, Yousef (2024), *Energy & Fuels* 38(11):9923–9932,
doi `10.1021/acs.energyfuels.4c00742` — and the group is **KFUPM / Saudi Aramco,
not Australian**.

**3. It is not a time series.** Conditions per the abstract: limestone, 1500 psi,
75 °C, 6–13 months, reaction cells with SEM/GC/ICP-OES. Verbatim: *"suggesting
that abiotic reactions in carbonate rocks are unlikely to occur during the first
stages of UHS."* That is a **two-endpoint null result**. It can bound a
dissolution rate; it cannot calibrate one. Full text is closed (ACS HTTP 403,
`oa_status: closed`), so conditions, mineralogy, sterile controls and the data
statement are **not verified**.

Note the temperature: 75 °C, i.e. **above** the ~70 °C threshold where Ghaedi
reports geochemical interactions become negligible. A null result there is
consistent with Ghaedi and says little about our sites at 25–45 °C.

**The real Monash/CSIRO paper, for the record.** Dodangoda, Haque, Zeng, Yang &
Ranjith (2025), *Renewable Energy* **251**:123357, doi
`10.1016/j.renene.2025.123357`, Monash + CSIRO Manufacturing, **CC BY 4.0**.
Abstract-level: 2.8 MPa/28 °C and 12 MPa/70 °C, up to 180 days, ~1 % calcite
dissolution, up to 25 % H₂ loss within 10 days, up to 63 % porosity increase.
Elsevier's bot filter returned 403 to automated retrieval, so the Methods and the
data-availability statement were **not read**.

**Two cautions before anyone treats it as an abiotic control**: the only control
described is an **N₂ gas-type control, not a sterility control**, so it should not
be classified as abiotic-controlled without reading the Methods; and Crossref
shows no linked dataset, DataCite no deposit. The related Dodangoda & Ranjith
(2024) *Gas Sci. Eng.* paper is CC BY but is **PHREEQC modelling only and includes
methanogenesis terms** — it is not an abiotic measurement. The thesis that likely
holds raw series is embargoed until 2027-01-12.

**Conclusion unchanged from C8:** no confirmed, openly accessible, numerical
abiotic carbonate–H₂ time series was found. Hellerschmied's CC BY abiotic
controls remain the substitute, with the total-pressure caveat.

---

## C11 — Two automated extractions of the same source contradicted each other

Worth recording as a method note, because it nearly put an unverified table into
this repository.

The Chabab solubility coefficients were reported twice by the same automated
extraction. The first run returned full coefficient tables read from the HAL
deposit (`HTTP 200, 33 pp.`). The second run reported the same deposit
**unreachable behind an anti-bot proof-of-work challenge** and the coefficients
**not obtained**.

Both were set aside and the file was fetched directly:

```sh
curl -sL -o chabab.pdf https://univ-pau.hal.science/hal-04623907/document
#   200  1082113  application/pdf   -> PDF document, version 1.4, 33 pages
```

Accessible. Table 4 was then read from the PDF itself and every coefficient
confirmed. The first run was right; the second was wrong.

**The rule this enforces:** an extraction is a lead, not a source. No number
enters this repository on the strength of a report about a document — only on the
document. The same run also caught a search summariser fabricating twice, once
inventing a Creative Commons licence for a paywalled Wiley article and once
attributing a paper to a DOI belonging to an unrelated study. Neither reached
this repository.

---

## C12 — The Lobodice source itself says the site is not directly applicable

Reading the secondary in full turned up a stronger statement than the confounder
recorded in `PREREGISTRATION.md`. Tremosa, Jakobsen & Le Gallo (2023), *Front.
Energy Res.* 11:1145978 (CC BY), opens its Lobodice discussion with:

> *"Field observations from town gas storage sites are **not directly applicable
> to pure hydrogen storage, due to the large presence of co-injected carbon
> sources**. The most emblematic case is that of the town gas storage of Lobodice
> (Czech Republic), documented in two scientific articles (Smigan et al., 1990;
> Buzek et al., 1994). In Lobodice, town gas containing 54% H2 was stored in a
> sandstone reservoir at a depth of 500 m (pressure of 4 MPa and temperature of
> 25°C–45°C). After 7 months of storage, 10%–20% of the gas volume was lost and
> the composition of the gas changed, with the formation of methane and decreases
> in hydrogen, carbon dioxide and carbon monoxide."*

The pre-registration recorded co-injected CO₂ and CO as a confounder that weakens
what Lobodice constrains. **The source goes further**: it states the class of site
is not directly applicable to pure-H₂ storage at all.

**Consequence for F2.** F2 requires the coupled band to encompass the Lobodice
54 % → 37 % drop. That criterion is frozen and is not being altered. But the
verdict on it will be reported **with this statement attached**, because a
validation point whose own source says it is not directly applicable cannot carry
the same weight as Sun Storage. Concretely: the F2 verdict is reported twice —
against both field points as pre-registered, and against Sun Storage alone — and
both are stated. Reporting only the combined verdict would hide that one of the
two points is contested by the literature it comes from.

This also sharpens **F3**. If the abiotic band alone encompasses both points, the
coupling is superfluous; but if Lobodice is not a clean pure-H₂ test, then F3
effectively turns on Sun Storage, and the study has **one** clean field
constraint, not two. That is a materially weaker validation base than the mission
assumed, and it is recorded now rather than discovered at the end.

Reservoir parameters confirmed from the same passage: 500 m depth, 4 MPa,
25–45 °C, sandstone, 7 months, 10–20 % of gas volume lost.

## C13 — The Lobodice composition data do not close, on four independent checks

C12 recorded that the Lobodice source itself calls the site not directly
applicable to pure-H₂ storage. This is worse: **the reported composition data
fail internal consistency before any model of ours touches them.** Measured by
`sio/lobodice_massbalance.sio`, which does the arithmetic in the engine.

The data, verbatim from Tremosa et al. (2023) §3:

> *"The stored gas initially composed of 54% H2, 22% CH4, 12% CO2, 9% CO and 2.5%
> N2 evolved after being stored during 7 months to 40% CH4, 37% H2, 9% CO2, 9% N2
> and 3% CO."* … *"During 7 months of storage, 10%–20% of the gas volume was lost
> … with an increase in methane."*

**The instrument is N₂.** Nitrogen takes no part in methanation, sulfate
reduction, acetogenesis or the water-gas shift, so its *moles* are conserved and
its *mole fraction* measures nothing but the change in total moles:
`total_final/total_initial = y_N₂(initial)/y_N₂(final)`. No model, no rate
constant, no assumption about which reaction ran.

| # | check | result |
|---|---|---|
| 0 | do the compositions sum to 100 %? | **99.5 % and 98.0 %**, and no normalisation convention is stated |
| 1 | N₂ tracer vs the stated volume loss | tracer says **72.6 %** lost; source says **10–20 %** — off by **3.6× to 7.3×** |
| 2 | did methane form? | CH₄ moles **fall 49.5 %**, yet formation of methane is a *measurement* there, carrying a microbial isotopic signature |
| 3 | stoichiometric closure | extents from CO₂ and CO predict a final CH₄ mole fraction of **107.2 %** — impossible |

Atom balances lose 66.4 % of carbon and 66.8 % of hydrogen from the gas. Carbon
can leave as dissolved carbonate, so a deficit is not by itself impossible — but
two thirds is a great deal, and it does not rescue checks 1–3.

**Independent corroboration, from the source's own model.** Tremosa et al.
simulate this site and report methane reaching 70 % against 40 % measured, CO₂
falling to 5.9 % against 8.8 % measured, and *"a decrease to less than the half
of the gas volume (46%)"*. **Their own model also produces a volume loss far
above the stated 10–20 %**, and much nearer this probe's tracer inference. On two
independent routes the 10–20 % figure is the outlier.

**Not attributed, because it cannot be.** The primary sources — Smigan et al.
(1990) and Buzek et al. (1994) — were never obtained. The failure may lie in the
primary measurement, in transmission through the secondary, or in a
normalisation convention nobody stated. Reported, not repaired.

**Consequence for F2.** F2 is **not evaluable at Lobodice.** Selecting whichever
subset of these numbers would permit a band to be drawn around 37 % is fitting to
a target, which the protocol forbids, so the probe refuses to emit a verdict
rather than choosing. Combined with C12, the study has **one** usable field
constraint — Sun Storage — not two, and F2 is reported against that one with the
Lobodice point declared unevaluable.

**And it would not have discriminated anyway.** At Lobodice's composition the F1
asymptote `y_H₂/(4·y_CO₂)` is **1.125** — the town gas carries very nearly its own
stoichiometric CO₂, so calcite changes almost nothing there whatever the data
say. 37 % H₂ is a value *both* model branches pass through on the way down. Even
a clean Lobodice dataset could not have separated H1a; the site is CO₂-rich,
which is exactly where the mechanism is inert.
