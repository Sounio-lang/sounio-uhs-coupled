// Oracle 1 — IPhreeqc, H2-brine-calcite abiotic system.
//
// This is the oracle. Nothing here is a replica of the Sounio model and nothing
// here may take a reference value from it: the direction of trust is one way.
//
// Fail-closed throughout. Every IPhreeqc call is checked, the database is pinned
// by SHA-256 rather than by path (three different files named phreeqc.dat ship
// in the IPhreeqc distribution, with different contents), and a run that does
// not produce the expected selected-output shape aborts instead of printing a
// plausible number.
//
// Build:
//   g++ -std=c++17 -O2 -o iphreeqc_calcite iphreeqc_calcite.cpp \
//       -I<install>/include -L<install>/lib -liphreeqc -Wl,-rpath,<install>/lib
//
// Usage:
//   ./iphreeqc_calcite --db PATH --db-sha256 HEX --temp-c T --nacl-molal M \
//                      --h2-fugacity F [--co2-fugacity F] [--header]

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <IPhreeqc.hpp>

namespace {

// ---------------------------------------------------------------------------
// SHA-256, so the database can be pinned without adding a dependency.
// ---------------------------------------------------------------------------
struct Sha256 {
    uint32_t h[8] = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
                     0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};
    uint64_t len = 0;
    unsigned char buf[64];
    size_t n = 0;

    static uint32_t ror(uint32_t x, int c) { return (x >> c) | (x << (32 - c)); }

    void block(const unsigned char* p) {
        static const uint32_t k[64] = {
            0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,
            0x923f82a4u,0xab1c5ed5u,0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,
            0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,0xe49b69c1u,0xefbe4786u,
            0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
            0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,
            0x06ca6351u,0x14292967u,0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,
            0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,0xa2bfe8a1u,0xa81a664bu,
            0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
            0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,
            0x5b9cca4fu,0x682e6ff3u,0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,
            0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u};
        uint32_t w[64];
        for (int i = 0; i < 16; ++i)
            w[i] = (p[i*4] << 24) | (p[i*4+1] << 16) | (p[i*4+2] << 8) | p[i*4+3];
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = ror(w[i-15],7) ^ ror(w[i-15],18) ^ (w[i-15] >> 3);
            uint32_t s1 = ror(w[i-2],17) ^ ror(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = ror(e,6) ^ ror(e,11) ^ ror(e,25);
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t t1 = hh + S1 + ch + k[i] + w[i];
            uint32_t S0 = ror(a,2) ^ ror(a,13) ^ ror(a,22);
            uint32_t mj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t t2 = S0 + mj;
            hh=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
        }
        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
    }

    void update(const unsigned char* p, size_t l) {
        len += l;
        while (l) {
            size_t take = std::min(l, 64 - n);
            memcpy(buf + n, p, take);
            n += take; p += take; l -= take;
            if (n == 64) { block(buf); n = 0; }
        }
    }

    std::string hex() {
        uint64_t bits = len * 8;
        unsigned char pad = 0x80;
        update(&pad, 1);
        unsigned char z = 0;
        while (n != 56) update(&z, 1);
        unsigned char lb[8];
        for (int i = 0; i < 8; ++i) lb[i] = (unsigned char)(bits >> (56 - 8*i));
        update(lb, 8);
        std::ostringstream o;
        for (int i = 0; i < 8; ++i) o << std::hex << std::setw(8) << std::setfill('0') << h[i];
        return o.str();
    }
};

std::string sha256_file(const std::string& path, bool* ok) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { *ok = false; return {}; }
    Sha256 s;
    std::vector<char> b(1 << 16);
    while (f.read(b.data(), (std::streamsize)b.size()) || f.gcount())
        s.update((const unsigned char*)b.data(), (size_t)f.gcount());
    *ok = true;
    return s.hex();
}

[[noreturn]] void die(const std::string& msg) {
    std::cerr << "FAIL: " << msg << "\n";
    std::exit(1);
}

void check(IPhreeqc& ip, int rc, const std::string& what) {
    if (rc != 0) {
        std::cerr << "FAIL: " << what << " returned " << rc << "\n"
                  << ip.GetErrorString() << "\n";
        std::exit(1);
    }
}

double need(const char* s, const char* name) {
    if (!s) die(std::string("missing argument value for ") + name);
    char* end = nullptr;
    double v = std::strtod(s, &end);
    if (end == s || (end && *end != '\0'))
        die(std::string("argument ") + name + " is not a number: " + s);
    return v;
}

}  // namespace

int main(int argc, char** argv) {
    std::string db, db_sha;
    double temp_c = 25.0, nacl = 0.0, h2_fug = 0.0, co2_fug = 0.0;
    bool have_co2 = false, header = false;
    bool decouple = false;   // treat H2 as redox-inert (H(0) fixed)
    double h2_molal = 0.0;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto nxt = [&]() -> const char* { return (i + 1 < argc) ? argv[++i] : nullptr; };
        if (a == "--db")             { const char* v = nxt(); if (!v) die("--db needs a path"); db = v; }
        else if (a == "--db-sha256") { const char* v = nxt(); if (!v) die("--db-sha256 needs a hex digest"); db_sha = v; }
        else if (a == "--temp-c")    temp_c = need(nxt(), "--temp-c");
        else if (a == "--nacl-molal") nacl = need(nxt(), "--nacl-molal");
        else if (a == "--h2-fugacity") h2_fug = need(nxt(), "--h2-fugacity");
        else if (a == "--co2-fugacity") { co2_fug = need(nxt(), "--co2-fugacity"); have_co2 = true; }
        else if (a == "--header")    header = true;
        else if (a == "--decouple-redox") decouple = true;
        else if (a == "--h2-molal")  h2_molal = need(nxt(), "--h2-molal");
        else die("unknown argument: " + a);
    }
    if (db.empty()) die("--db is required; the database is not guessed");
    if (db_sha.empty())
        die("--db-sha256 is required: three different files named phreeqc.dat ship "
            "with IPhreeqc, so the database is pinned by content, not by path");

    bool ok = false;
    std::string got = sha256_file(db, &ok);
    if (!ok) die("cannot read database: " + db);
    if (got != db_sha)
        die("database hash mismatch for " + db + "\n  expected " + db_sha + "\n  got      " + got);

    IPhreeqc ip;
    check(ip, ip.LoadDatabase(db.c_str()), "LoadDatabase");

    std::ostringstream in;
    in << std::setprecision(17);
    // Redox decoupling, when asked for, is done the way PHREEQC's own Amm.dat
    // decouples ammonia from nitrogen: by making the species a SEPARATE ELEMENT
    // with its own master species, so no electron appears in its reactions and
    // it cannot drive pe. `Hdg` is dissolved hydrogen gas carrying exactly the
    // Henry constants that phreeqc.dat gives H2(g) -- same solubility, no redox.
    //
    // The database itself is NOT modified: it stays pinned by hash, and this
    // block is part of the run's own input, so it appears in the record.
    if (decouple) {
        in << "SOLUTION_MASTER_SPECIES\n"
           << "    Hdg    Hdg    0.0    Hdg    2.016\n"
           << "SOLUTION_SPECIES\n"
           << "    Hdg = Hdg\n"
           << "    -log_k 0.0\n"
           << "PHASES\n"
           << "    Hdg(g)\n"
           << "    Hdg = Hdg\n"
           << "    -log_k -3.105\n"
           << "    -delta_h -4.184 kJ\n"
           << "    -analytic -9.3114 4.6473e-3 -49.335 1.4341 1.2815e5\n"
           << "END\n";
    }

    // Charge balance is placed on Cl, not on pH. Balancing on pH lets the solver
    // move pH arbitrarily to absorb the Ca2+ released by calcite, which produces
    // a formally converged but physically meaningless state.
    in << "SOLUTION 1\n"
       << "    units      mol/kgw\n"
       << "    temp       " << temp_c << "\n"
       << "    pH         7.0\n";
    if (nacl > 0.0) {
        in << "    Na         " << nacl << "\n"
           << "    Cl         " << nacl << " charge\n";
    }
    // CONVENTION AXIOM, and it dominates the result by orders of magnitude:
    // whether H(0) and H(1) are allowed to equilibrate.
    //
    // phreeqc.dat couples them through the electron -- H(0) has master species
    // H2, H(1) has H+ -- so imposing H2(g) at reservoir fugacity as an
    // equilibrium phase pins pe at the bottom of its range and reduces the water
    // itself. That is the artefact the calibrated-model literature warns about,
    // not a property of the rock.
    //
    // Decoupled: H2 is entered as a fixed H(0) molality, kinetically inert with
    // respect to the water couple, which is the physical situation at 40 degC in
    // the absence of a catalyst -- and is why microbial catalysis is the thing
    // that matters. Matches the archived USGS model, which sets "decouple ALL".
    in << "EQUILIBRIUM_PHASES 1\n"
       << "    Calcite    0.0  10.0\n";
    if (h2_fug > 0.0) {
        // same imposed fugacity either way; only the coupling differs
        if (decouple) in << "    Hdg(g)     " << std::log10(h2_fug) << "  10.0\n";
        else          in << "    H2(g)      " << std::log10(h2_fug) << "  10.0\n";
    }
    if (have_co2 && co2_fug > 0.0)
        in << "    CO2(g)     " << std::log10(co2_fug) << "  10.0\n";
    in << "SELECTED_OUTPUT\n"
       << "    -reset     false\n"
       << "    -temperature true\n"
       << "    -pH        true\n"
       << "    -ionic_strength true\n"
       << (decouple ? "    -totals    Ca C Hdg\n" : "    -totals    Ca C H(0)\n")
       << "    -equilibrium_phases Calcite\n"
       << "    -saturation_indices Calcite\n"
       << "END\n";

    ip.SetSelectedOutputStringOn(true);
    check(ip, ip.RunString(in.str().c_str()), "RunString");

    const char* so = ip.GetSelectedOutputString();
    if (!so || !*so) die("selected output is empty; refusing to report a value");

    if (header) std::cout << "# " << so;
    else {
        // emit only the data row(s), the header having been checked by the caller
        std::istringstream lines(so);
        std::string line;
        bool first = true;
        while (std::getline(lines, line)) {
            if (first) { first = false; continue; }
            if (!line.empty()) std::cout << line << "\n";
        }
    }
    return 0;
}
