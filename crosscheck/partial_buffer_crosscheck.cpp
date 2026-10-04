// crosscheck/partial_buffer_crosscheck.cpp
//
// Independent C++23 cross-check of sio/partial_buffer.sio.
//
// What is independent: the model of the gas-buffered closed vessel (carbon,
// alkalinity, calcium, Henry, Omega = 1), the F1 accounting, and the solver --
// Brent's method on both levels instead of the .sio's nested bisections, and the
// carbon balance solved for pH rather than through a shared helper.
// What is inherited: the activity coefficients and equilibrium constants at
// 40 C and mu = 0.5, printed by the study's own validated carbonate_equilibria
// module (WATEQ Debye-Hueckel, phreeqc.dat). Re-deriving those is the study's
// 25 C oracle validation, not this file's job.
//
// Build: g++ -std=c++23 -O2 -o pbx partial_buffer_crosscheck.cpp && ./pbx
#include <cmath>
#include <cstdio>
#include <expected>
#include <functional>

namespace {
// Inherited at T = 313.15 K, mu = 0.5 (sio/carbonate_equilibria.sio, gen3 1aa4317f)
constexpr double g_h = 0.759778582710429, g_oh = 0.625555306085348;
constexpr double g_hco3 = 0.685935251910316, g_co3 = 0.221376996845678;
constexpr double g_ca = 0.250832986105578;
constexpr double k1 = 5.04202449629622e-7, k2 = 6.00213392006326e-11;
constexpr double kw = 2.92799027886569e-14, kh = 2.37109065891596e-2;
constexpr double ksp = 2.41979110615114e-9;
constexpr double alk0 = 1.08393509422944e-3, dic0 = 2.256521e-3;
// Declared inputs, as in the .sio
constexpr double TK = 313.15, R = 0.0820574, BAR2ATM = 0.986923, KH_H2 = 7.4530e-4;
constexpr double SO4 = 0.008269, P_BAR = 78.0, Y_H2 = 0.0989, Y_CO2 = 0.0019;
double CA0 = 0.0898;  // Aux Vases; swept below

struct State { double ctot, dic, ng, a_co3; };

// Total carbon at (pH, Alk); nullopt-like via NaN when infeasible.
State carbon(double ph, double alk, double v) {
  const double ah = std::pow(10.0, -ph);
  const double moh = kw / ah / g_oh, mh = ah / g_h;
  const double s = k1 / (ah * g_hco3) + 2.0 * k1 * k2 / (ah * ah * g_co3);
  const double num = alk - moh + mh;
  if (num <= 0) return {-1e30, 0, 0, 0};
  const double aco2 = num / s;
  const double dic = aco2 + aco2 * k1 / ah / g_hco3 + aco2 * k1 * k2 / (ah * ah) / g_co3;
  const double ng = aco2 / kh * v / (R * TK);
  return {dic + ng, dic, ng, aco2 * k1 * k2 / (ah * ah)};
}

std::expected<double, int> brent(const std::function<double(double)>& f, double a, double b) {
  double fa = f(a), fb = f(b);
  if (fa * fb > 0) return std::unexpected(1);
  if (std::fabs(fa) < std::fabs(fb)) { std::swap(a, b); std::swap(fa, fb); }
  double c = a, fc = fa, d = b - a, e = d;
  for (int it = 0; it < 300; ++it) {
    if (fb == 0) return b;
    if (fa * fb > 0) { a = c; fa = fc; d = e = b - c; }
    if (std::fabs(fa) < std::fabs(fb)) { c = b; b = a; a = c; fc = fb; fb = fa; fa = fc; }
    const double tol = 2e-16 * std::fabs(b) + 1e-18, m = 0.5 * (a - b);
    if (std::fabs(m) <= tol) return b;
    if (std::fabs(e) >= tol && std::fabs(fc) > std::fabs(fb)) {
      double p, q, r, s = fb / fc;
      if (a == c) { p = 2 * m * s; q = 1 - s; }
      else { q = fc / fa; r = fb / fa; p = s * (2 * m * q * (q - r) - (b - c) * (r - 1)); q = (q - 1) * (r - 1) * (s - 1); }
      if (p > 0) q = -q; else p = -p;
      if (2 * p < std::fmin(3 * m * q - std::fabs(tol * q), std::fabs(e * q))) { e = d; d = p / q; }
      else { d = m; e = m; }
    } else { d = m; e = m; }
    c = b; fc = fb;
    b += std::fabs(d) > tol ? d : (m > 0 ? tol : -tol);
    fb = f(b);
  }
  return b;
}

double omega(double xi, double psi, double c0, double v) {
  const double ctot = c0 - xi + psi, ca = CA0 + psi, alk = alk0 + 2 * psi;
  if (ctot <= 0 || ca <= 0) return -1;
  auto ph = brent([&](double p) { return carbon(p, alk, v).ctot - ctot; }, 2.0, 13.0);
  if (!ph) return -1;
  return ca * g_ca * carbon(*ph, alk, v).a_co3 / ksp;
}
}  // namespace

struct Row { double psi, arg, f1, f1fixed; };
Row run(double v);
int main() {
  std::printf("Aux Vases calcium, V_gas sweep\nV_gas    psi_max          argmax  F1_honoured   F1_fixed\n");
  for (double v : {0.1, 1.0, 10.0, 100.0}) { auto r = run(v); std::printf("%-8g %.10e %-7g %.10f %.10f\n", v, r.psi, r.arg, r.f1, r.f1fixed); }
  std::printf("\nV_gas = 1, calcium sweep\nCa0      F1_honoured\n");
  double fmax = 0;
  for (double ca : {1e-5, 1e-4, 1e-3, 1e-2, 0.0898, 0.3}) { CA0 = ca; auto r = run(1.0); fmax = std::fmax(fmax, r.f1); std::printf("%-8g %.10f\n", ca, r.f1); }
  std::printf("max F1 %.10f\n", fmax);
}
Row run(double v) {
  const double patm = P_BAR * BAR2ATM, rt = R * TK, sink = 4 * SO4;
    const double c0 = dic0 + Y_CO2 * patm * v / rt;
    const double nh2 = Y_H2 * patm * v / rt + KH_H2 * Y_H2 * patm, left = nh2 - sink;
    double best = 0, arg = 0;
    for (int i = 0; i < 200; ++i) {
      const double xi = c0 * i / 200.0;
      auto psi = brent([&](double p) { return omega(xi, p, c0, v) - 1.0; }, -0.9 * CA0, 5e-2);
      if (psi && *psi > best) { best = *psi; arg = i / 200.0; }
    }
    const double without = sink + 4 * std::fmin(c0, left / 4);
    const double with_c = sink + 4 * std::fmin(c0 + best, left / 4);
    return {best, arg, with_c / without, (sink + left) / without};
}
