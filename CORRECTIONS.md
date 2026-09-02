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
