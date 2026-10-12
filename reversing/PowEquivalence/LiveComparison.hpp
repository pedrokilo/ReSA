#pragma once
// Banco dentro del motor: ExeCompatiblePowCpp (el pow que usan los helpers de X87Math, CRT estática de la DLL) frente al _CIpow del EXE (0x822130).
// Se comparan los bits del resultado Y las banderas de excepción de coma flotante (MXCSR y palabra de estado x87) que deja cada llamada.
// 0) rejilla de valores especiales a nivel de double (±0, subnormales, ±1, ±infinito, NaN silenciosos y de señalización, extremos);
// 1) todos los exponentes float (2^32) con la base de CTaskSimpleCarJumpOut (0.96f como double);
// 2) regiones: para cada par (signo, exponente binario de damp) x (signo, exponente binario del paso), 2048 pares de mantisas;
// 3) PowScaleVector3Cpp (CBouncingPanel::ProcessPanel) frente a la secuencia x87 original, con damp/paso/vector aleatorios;
// 4) PowDecaySpeedCpp (CTaskSimpleCarJumpOut::ProcessPed) frente a la secuencia x87 original.
// Cada "par" es un trozo de casos; original y candidato son el hash FNV-1a de los bits de todos los resultados (y banderas) del trozo.
// Antes se comprueba el entorno: banderas SSE2 del EXE y de la CRT de la DLL, MXCSR y palabra de control, que fijan la rama que usan las dos.
#include <fstream>
#include <bit>
#include <cstring>
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
extern "C" int __use_sse2_mathfcns;
namespace PowEquivalenceLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& j) { ReversingProbe::PumpMessages(); log << j.dump() << '\n'; log.flush(); }

// Llama al _CIpow del EXE y devuelve las banderas que deja (bits 0..5 de MXCSR, bits 8..13 = estado x87)
inline double ExePow(double base, double exponent, uint32& flags) {
    double r; uint32 m; uint16 s;
    __asm {
        fnclex
        stmxcsr m
        and   m, 0FFFFFFC0h
        ldmxcsr m
        fld   base
        fld   exponent
        mov   eax, 0x822130
        call  eax
        fstp  r
        stmxcsr m
        fnstsw s
    }
    flags = (m & 0x3F) | (uint32(s & 0x3F) << 8);
    return r;
}
inline double CandidatePow(double base, double exponent, uint32& flags) {
    uint32 m; uint16 s;
    __asm {
        fnclex
        stmxcsr m
        and   m, 0FFFFFFC0h
        ldmxcsr m
    }
    const double r = ExeCompatiblePowCpp(base, exponent);
    __asm {
        stmxcsr m
        fnstsw s
    }
    flags = (m & 0x3F) | (uint32(s & 0x3F) << 8);
    return r;
}
// Secuencia original de CBouncingPanel::MultiplyByPow (antes de la conversión)
inline void ExeScaleVector3(float* p, float damp, float step) {
    __asm {
        fld   damp
        fld   step
        mov   eax, 0x822130
        call  eax
        mov   edx, p
        fld   st(0)
        fmul  dword ptr [edx]
        fstp  dword ptr [edx]
        fld   st(0)
        fmul  dword ptr [edx+4]
        fstp  dword ptr [edx+4]
        fmul  dword ptr [edx+8]
        fstp  dword ptr [edx+8]
    }
}
// Secuencia original de CTaskSimpleCarJumpOut::ProcessPed (antes de la conversión)
inline void ExeDecaySpeed(float* speed, float* shift, float step) {
    const double base = 0.9599999785423279;
    __asm {
        fld   base
        fld   step
        mov   eax, 0x822130
        call  eax
        mov   edx, speed
        fmul  dword ptr [edx]
        fst   dword ptr [edx]
        fmul  step
        mov   edx, shift
        fadd  dword ptr [edx]
        fstp  dword ptr [edx]
    }
}

inline uint64 Fnv(uint64 h, uint64 v) { for (int i = 0; i < 8; ++i) { h ^= (v >> (i * 8)) & 0xFF; h *= 1099511628211ull; } return h; }
struct Rng {
    uint32 s;
    uint32 Next() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
    float Uniform(float a, float b) { return a + (b - a) * float(Next() % 1000000) / 1000000.f; }
    float Bits() { return std::bit_cast<float>(Next()); }
    // mitad en el dominio real, mitad patrones de bits cualquiera (NaN, infinitos, subnormales, enormes)
    float Mixed(float a, float b) { return (Next() & 1) ? Uniform(a, b) : Bits(); }
};

// Diferencias de banderas conocidas y explicadas (los VALORES deben ser siempre idénticos):
//  1) desbordamiento a ±infinito con entradas finitas: el EXE carga el infinito sin más y la DLL lo calcula como máximo·máximo, por lo que
//     además enciende la bandera de desbordamiento (0x08) y la de inexacto (0x20); el resto de banderas son las mismas;
//  2) NaN elevado a ±0: ExeCompatiblePowCpp devuelve la base NaN silenciada sin pasar por el algoritmo (el EXE sí lo recorre y deja inexacto, 0x20).
// Cualquier otra diferencia de banderas hace fallar el banco. Las banderas son acumulativas y nadie las lee (ver README).
inline uint64 explainedOverflow{}, explainedNanZero{};
struct Chunk {
    uint64 ho = 14695981039346656037ull, hc = 14695981039346656037ull;
    bool eq = true, flagsEq = true;
    json first;
    void Add(double base, double exponent) {
        uint32 fe{}, fc{};
        const uint64 ab = std::bit_cast<uint64>(ExePow(base, exponent, fe)), bb = std::bit_cast<uint64>(CandidatePow(base, exponent, fc));
        const uint64 bBits = std::bit_cast<uint64>(base), eBits = std::bit_cast<uint64>(exponent), mag = 0x7FFFFFFFFFFFFFFFull, inf = 0x7FF0000000000000ull;
        bool explained = false;
        if (ab == bb && fe != fc) {
            const uint32 diff = fc ^ fe;
            const bool finiteInputs = (bBits & mag) < inf && (eBits & mag) < inf;
            if ((ab & mag) == inf && finiteInputs && (diff & ~0x28u) == 0 && (fc & fe) == fe) { explained = true; ++explainedOverflow; }
            else if ((bBits & mag) > inf && (eBits & mag) == 0 && fe == 0x20 && fc == 0) { explained = true; ++explainedNanZero; }
        }
        const uint32 hfe = explained ? (fe & fc) : fe, hfc = explained ? (fe & fc) : fc;   // las banderas explicadas no entran en el hash
        ho = Fnv(Fnv(ho, ab), hfe); hc = Fnv(Fnv(hc, bb), hfc);
        if ((ab != bb || (fe != fc && !explained)) && first.is_null()) {
            first = {{"base", bBits}, {"exp", eBits}, {"exe", ab}, {"dll", bb}, {"exe_flags", fe}, {"dll_flags", fc}};
        }
        eq &= ab == bb; flagsEq &= fe == fc || explained;
    }
    bool Equal() const { return eq && flagsEq; }
};

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "pow-equivalence-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(dir / "pow-equivalence-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("pow-equivalence");
        Write({{"event", "waiting_for_player"}});
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;

    int pairs = 0;
    bool pass = true;
    // Entorno que decide la rama: las dos implementaciones usan la ruta SSE2 sólo si su bandera está activa y MXCSR/control x87 son los normales
    {
        uint32 mxcsr{}; uint16 cw{};
        __asm { stmxcsr mxcsr
                fnstcw cw }
        const int exeFlag = *reinterpret_cast<const int*>(0xC9C400), dllFlag = __use_sse2_mathfcns;
        const bool ok = exeFlag != 0 && dllFlag != 0 && (mxcsr & 0x1F80) == 0x1F80 && (mxcsr & 0x7F80) == 0x1F80 && (cw & 0x7F) == 0x7F;
        pass &= ok;
        Write({{"event", "environment"}, {"exe_sse2_flag", exeFlag}, {"dll_sse2_flag", dllFlag}, {"mxcsr", mxcsr}, {"x87_control", cw}, {"sse2_path_selected_by_both", ok}});
    }
    // 0) rejilla de valores especiales
    {
        const char* name = "pow especiales (double)";
        std::vector<uint64> values;
        for (uint64 s = 0; s < 2; ++s)
            for (uint64 e : {0x000ull, 0x001ull, 0x3C0ull, 0x3FDull, 0x3FEull, 0x3FFull, 0x400ull, 0x401ull, 0x41Dull, 0x43Eull, 0x43Full, 0x7FDull, 0x7FEull, 0x7FFull})
                for (uint64 m : {0ull, 1ull, 0x8000000000000ull, 0xFFFFFFFFFFFFFull, 0x5A5A5A5A5A5A5ull, 0x4000000000000ull})
                    values.push_back((s << 63) | (e << 52) | m);
        int failures = 0;
        for (size_t bi = 0; bi < values.size(); ++bi) {
            Chunk c;
            for (size_t ei = 0; ei < values.size(); ++ei) c.Add(std::bit_cast<double>(values[bi]), std::bit_cast<double>(values[ei]));
            failures += !c.Equal(); pass &= c.Equal();
            json r{{"event", "comparison"}, {"function", name}, {"pair", pairs++}, {"case", int(bi)}, {"equal", c.Equal()}, {"accepted", c.Equal()}, {"original", c.ho}, {"candidate", c.hc}};
            if (!c.Equal()) r["first_difference"] = c.first;
            Write(r);
        }
        Write({{"event", "function_summary"}, {"function", name}, {"cases", int(values.size())}, {"failures", failures}, {"explained_overflow_flags", explainedOverflow}, {"explained_nan_zero_flags", explainedNanZero}});
    }
    // 1) todos los exponentes float con la base de CarJumpOut
    {
        const char* name = "pow(0.96f, todo float)";
        int failures = 0;
        for (uint32 chunk = 0; chunk < 4096; ++chunk) {
            Chunk c;
            for (uint32 j = 0; j < (1u << 20); ++j) c.Add(0.9599999785423279, double(std::bit_cast<float>((chunk << 20) | j)));
            failures += !c.Equal(); pass &= c.Equal();
            json r{{"event", "comparison"}, {"function", name}, {"pair", pairs++}, {"case", chunk}, {"equal", c.Equal()}, {"accepted", c.Equal()}, {"original", c.ho}, {"candidate", c.hc}};
            if (!c.Equal()) r["first_difference"] = c.first;
            Write(r);
        }
        Write({{"event", "function_summary"}, {"function", name}, {"cases", 4096}, {"failures", failures}, {"explained_overflow_flags", explainedOverflow}, {"explained_nan_zero_flags", explainedNanZero}});
    }
    // 2) regiones: todas las combinaciones (signo, exponente binario) de damp y de paso, con mantisas pseudoaleatorias y extremas
    {
        const char* name = "pow regiones (float)";
        int failures = 0;
        for (uint32 dr = 0; dr < 512; ++dr) {
            Chunk c;
            for (uint32 sr = 0; sr < 512; ++sr) {
                Rng rng{0x9E3779B9u * (dr * 512 + sr + 1)};
                for (uint32 k = 0; k < 2048; ++k) {
                    uint32 m1 = rng.Next() & 0x7FFFFF, m2 = rng.Next() & 0x7FFFFF;
                    if (k < 4) { constexpr uint32 edge[]{0u, 0x7FFFFFu, 0x400000u, 1u}; m1 = edge[k]; m2 = edge[(k + 1) & 3]; }
                    const float damp = std::bit_cast<float>(((dr >> 8) << 31) | ((dr & 0xFF) << 23) | m1);
                    const float step = std::bit_cast<float>(((sr >> 8) << 31) | ((sr & 0xFF) << 23) | m2);
                    c.Add(double(damp), double(step));
                }
            }
            failures += !c.Equal(); pass &= c.Equal();
            json r{{"event", "comparison"}, {"function", name}, {"pair", pairs++}, {"case", dr}, {"equal", c.Equal()}, {"accepted", c.Equal()}, {"original", c.ho}, {"candidate", c.hc}};
            if (!c.Equal()) r["first_difference"] = c.first;
            Write(r);
        }
        Write({{"event", "function_summary"}, {"function", name}, {"cases", 512}, {"failures", failures}, {"explained_overflow_flags", explainedOverflow}, {"explained_nan_zero_flags", explainedNanZero}});
    }
    // 3) PowScaleVector3Cpp y 4) PowDecaySpeedCpp con casos aleatorios
    for (int which = 0; which < 2; ++which) {
        const char* name = which == 0 ? "PowScaleVector3Cpp" : "PowDecaySpeedCpp";
        int failures = 0;
        for (uint32 chunk = 0; chunk < 1024; ++chunk) {
            Rng rng{0x9E3779B9u * (chunk + 1) + uint32(which) * 7919u};
            uint64 ho = 14695981039346656037ull, hc = ho;
            bool eq = true; json first;
            for (uint32 j = 0; j < 65536; ++j) {
                const float a0 = which == 0 ? rng.Mixed(0.f, 1.f) : 0.f;      // damp
                const float step = rng.Mixed(0.f, 8.f);
                float o[3], c[3];
                for (int k = 0; k < 3; ++k) o[k] = c[k] = rng.Mixed(-100.f, 100.f);
                if (which == 0) { ExeScaleVector3(o, a0, step); PowScaleVector3Cpp(c, a0, step); }
                else { ExeDecaySpeed(&o[0], &o[1], step); PowDecaySpeedCpp(&c[0], &c[1], step); }
                const int n = which == 0 ? 3 : 2;
                for (int k = 0; k < n; ++k) {
                    const uint32 ob = std::bit_cast<uint32>(o[k]), cb = std::bit_cast<uint32>(c[k]);
                    ho = Fnv(ho, ob); hc = Fnv(hc, cb);
                    if (ob != cb && eq) { eq = false; first = {{"damp_bits", std::bit_cast<uint32>(a0)}, {"step_bits", std::bit_cast<uint32>(step)}, {"component", k}, {"exe", ob}, {"dll", cb}}; }
                }
            }
            failures += !eq; pass &= eq;
            json r{{"event", "comparison"}, {"function", name}, {"pair", pairs++}, {"case", chunk}, {"equal", eq}, {"accepted", eq}, {"original", ho}, {"candidate", hc}};
            if (!eq) r["first_difference"] = first;
            Write(r);
        }
        Write({{"event", "function_summary"}, {"function", name}, {"cases", 1024}, {"failures", failures}});
    }
    Write({{"event", pass ? "complete" : "error"}, {"pairs", pairs}, {"state_restored", true}});
}
}
