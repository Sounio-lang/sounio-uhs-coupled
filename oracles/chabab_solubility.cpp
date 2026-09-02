// H2 solubility in NaCl brine — Chabab et al., Model 2 (Duan-type / Pitzer).
//
// An INDEPENDENT check on the IPhreeqc oracle's dissolved-hydrogen result. This
// is a different model family fitted to different data: PHREEQC combines a
// Henry's law constant with a Debye-Hückel activity model, while this is a
// Pitzer-type virial correlation fitted directly to H2-brine solubility
// measurements. Agreement between them corroborates both; disagreement is a
// finding either way. Neither is a replica of the other.
//
// Coefficients read from Table 4 of the accepted manuscript (HAL hal-04623907,
// sha256 6e1d87124013fda00ef362932bcf10c071eab1b442e87606793ac518e896a979),
// verified against the PDF rather than taken from a summary. Version of record
// is paywalled; these are pre-copyedit values.
//
// Equation 8, rearranged. The published form is
//     ln(y_H2 * P / m_H2) = mu0/RT - ln(phi_H2) + sum 2*lambda*m + sum sum zeta*m*m
// and since the fugacity is f_H2 = y_H2 * phi_H2 * P, the fugacity coefficient
// cancels:
//     ln(m_H2) = ln(f_H2) - mu0/RT - sum 2*lambda*m - sum sum zeta*m*m
// so the result depends on the imposed fugacity alone, which is exactly what the
// IPhreeqc run imposes. No phi model is needed and none is invented.
//
// Equation 11 gives each parameter's T,P dependence:
//     Par(T,P) = C1 + C2*T + C3/T + C4*T^2 + C5*P + C6*P/T^2
//                + C7/P + C8*T/P + C9*T^2/P + C10*T^3/P
//
// Units, as published: P in bar, m in mol/kg water, T in K.
//
// The paper states NO explicit validity envelope. Its measurements span
// T 298-373 K, P <= 200 bar, m_NaCl 0-4 mol/kgw, and the 200 bar ceiling is an
// autoclave limit rather than a physical one. This program refuses to
// extrapolate outside that measured range rather than returning a number whose
// standing the source does not support.
//
// Build: g++ -std=c++17 -O2 -o chabab_solubility chabab_solubility.cpp
// Usage: ./chabab_solubility --temp-k T --pressure-bar P --nacl-molal M --fugacity-bar F

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

// Table 4, model 2. Index 0..9 = C1..C10.
const double MU0_RT[10] = {
     41.8266086, -8.24713967e-2, -4.60318630e+3,  6.03537635e-5, 4.12979459e-4,
     1.82081207e+1, 3.73478602e+1, -3.87633253e-1, 1.34370747e-3, -1.55621990e-6};

const double LAMBDA_H2_C[10] = {
    -7.74829265312071, 0.0226221702021589, 923.092396500207,
    -2.21140172559128e-5, 7.40868321886585e-5, -12.3509724808910,
    -47.3816790140829, 0.469165009435218, -0.0015626314758, 1.75015662317748e-6};

// Table 4 leaves C2..C10 blank for these two: no T or P dependence at all.
const double LAMBDA_H2_A = 0.0;             // H2-Cl-, exactly zero
const double ZETA_H2_C_A = -0.009470244669; // constant

// Measured range of the fit. Not an authors' statement of validity — the paper
// gives none — so it is labelled as what it is at the point of use.
const double T_MIN = 298.0, T_MAX = 373.0;
const double P_MAX = 200.0;
const double M_MAX = 4.0;

double par(const double c[10], double T, double P) {
    return c[0] + c[1]*T + c[2]/T + c[3]*T*T + c[4]*P + c[5]*P/(T*T)
         + c[6]/P + c[7]*T/P + c[8]*T*T/P + c[9]*T*T*T/P;
}

[[noreturn]] void die(const std::string& m) {
    std::cerr << "FAIL: " << m << "\n";
    std::exit(1);
}

double need(const char* s, const char* n) {
    if (!s) die(std::string("missing value for ") + n);
    char* e = nullptr;
    double v = std::strtod(s, &e);
    if (e == s || *e) die(std::string(n) + " is not a number: " + s);
    return v;
}

}  // namespace

int main(int argc, char** argv) {
    double T = 0, P = 0, m = -1, f = 0;
    bool allow_extrap = false;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto nxt = [&]() -> const char* { return (i + 1 < argc) ? argv[++i] : nullptr; };
        if (a == "--temp-k")           T = need(nxt(), "--temp-k");
        else if (a == "--pressure-bar") P = need(nxt(), "--pressure-bar");
        else if (a == "--nacl-molal")   m = need(nxt(), "--nacl-molal");
        else if (a == "--fugacity-bar") f = need(nxt(), "--fugacity-bar");
        else if (a == "--allow-extrapolation") allow_extrap = true;
        else die("unknown argument: " + a);
    }
    if (T <= 0 || P <= 0 || m < 0 || f <= 0)
        die("need --temp-k, --pressure-bar, --nacl-molal, --fugacity-bar");

    if (!allow_extrap) {
        if (T < T_MIN || T > T_MAX)
            die("T = " + std::to_string(T) + " K is outside the fitted range "
                + std::to_string(T_MIN) + "-" + std::to_string(T_MAX)
                + " K; pass --allow-extrapolation to proceed and label the result");
        if (P > P_MAX)
            die("P is above the " + std::to_string(P_MAX) + " bar fitted ceiling");
        if (m > M_MAX)
            die("m_NaCl is above the " + std::to_string(M_MAX) + " mol/kgw fitted ceiling");
    }

    const double mu0 = par(MU0_RT, T, P);
    const double lam_c = par(LAMBDA_H2_C, T, P);

    // NaCl: m_Na+ = m_Cl- = m
    const double sum_lambda = 2.0 * lam_c * m + 2.0 * LAMBDA_H2_A * m;
    const double sum_zeta   = ZETA_H2_C_A * m * m;

    const double ln_m = std::log(f) - mu0 - sum_lambda - sum_zeta;
    const double m_h2 = std::exp(ln_m);

    std::cout << std::setprecision(10)
              << "T_K=" << T << " P_bar=" << P << " m_NaCl=" << m
              << " f_H2_bar=" << f << "\n"
              << "mu0_RT=" << mu0 << " lambda_H2_c=" << lam_c << "\n"
              << "m_H2_mol_per_kgw=" << m_h2 << "\n";
    if (allow_extrap && (T < T_MIN || T > T_MAX || P > P_MAX || m > M_MAX))
        std::cout << "EXTRAPOLATED beyond the fitted range; not supported by the source\n";
    return 0;
}
