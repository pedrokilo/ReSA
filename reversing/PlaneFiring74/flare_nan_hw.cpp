// Microtest autonomo de 32 bits (herramienta de prueba, NO codigo de produccion; __asm permitido aqui): valida contra el HARDWARE x87 la emulacion de NaN y la aritmetica
// de PlaneFiring74FlareCpp (X87Math.cpp, bloque Lote 74; usa HeatFire73Raw / HeatFire73Op del bloque Lote 73, validadas aparte por HeatFire73/nan_rules_hw). El texto de
// produccion se incluye TAL CUAL desde flare_slice.inc, que genera flare_nan_hw.py cortando X87Math.cpp desde Lote 73 hasta el final (sin copiar ni modificar la logica).
// Estado de coma flotante como el juego: palabra de control x87 a 24 bits (redondeo al mas cercano, excepciones enmascaradas) y MXCSR con FTZ.
//
// Cadena completa por componente (0x6E3527..0x6E35D7): t1 = float(-2.5 * adelante) ; t2 = float(-1.0 * arriba) ; s = float(t2 + t1) ; salida = float(s + posicion), con la
// transcripcion __asm de la secuencia x87 del EXE (fld k; fmul [v]; fstp / fld [t2]; fadd [t1]; fstp / fld [s]; fadd [pos]; fstp), bit a bit, para:
//   a) TODAS las ternas (adelante, arriba, posicion) de un conjunto de 56 patrones (36 NaN: silenciosos y de senal x signo x cargas; 20 no NaN: ceros, desnormales, normales, maximos, infinitos),
//   b) 20 millones de ternas aleatorias densas en NaN (cargas, signos y empates aleatorios) y c) 6 millones sin NaN (desnormales, ceros con signo, infinitos, desbordamientos).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <float.h>
#include "flare_slice.inc"

typedef uint32_t u32;
typedef uint64_t u64;

static u64 rngState = 0x9E3779B97F4A7C15ull;
static u32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return u32(rngState >> 16); }

static __declspec(noinline) void ChainHw(const u32* fwd, const u32* up, const u32* pos, u32* out) {
    static const u32 kBack = 0xC0200000u, kDown = 0xBF800000u;
    u32 t1 = 0, t2 = 0, s = 0;
    for (int i = 0; i < 3; ++i) {
        const u32* f = fwd + i;
        const u32* u = up + i;
        const u32* p = pos + i;
        u32* o = out + i;
        __asm {
            mov esi, f
            mov edi, u
            mov ebx, p
            mov edx, o
            fld dword ptr [kBack]
            fmul dword ptr [esi]
            fstp dword ptr [t1]
            fld dword ptr [kDown]
            fmul dword ptr [edi]
            fstp dword ptr [t2]
            fld dword ptr [t2]
            fadd dword ptr [t1]
            fstp dword ptr [s]
            fld dword ptr [s]
            fadd dword ptr [ebx]
            fstp dword ptr [edx]
        }
    }
}

static u32 nanPatterns[64];
static unsigned nNan;
static u32 allPatterns[128];
static unsigned nAll;
static void BuildPatterns() {
    const u32 qPay[] = {0u, 1u, 2u, 3u, 0x100u, 0x1234u, 0x155555u, 0x200000u, 0x3FFFFEu, 0x3FFFFFu};
    const u32 sPay[] = {1u, 2u, 3u, 0x100u, 0x1234u, 0x155555u, 0x200000u, 0x3FFFFFu};
    for (u32 sign = 0; sign < 2; ++sign) {
        for (u32 p : qPay) nanPatterns[nNan++] = (sign << 31) | 0x7F800000u | 0x400000u | p;
        for (u32 p : sPay) nanPatterns[nNan++] = (sign << 31) | 0x7F800000u | p;
    }
    const u32 mags[] = {0u, 1u, 0x7FFFFFu, 0x800000u, 0x3F800000u, 0x3F000000u, 0x40400000u, 0x7149F2CAu, 0x7F7FFFFFu, 0x7F800000u};
    for (unsigned i = 0; i < nNan; ++i) allPatterns[nAll++] = nanPatterns[i];
    for (u32 sign = 0; sign < 2; ++sign)
        for (u32 m : mags) allPatterns[nAll++] = (sign << 31) | m;
}

static u64 combos, fails;
static void Check(const u32* fwd, const u32* up, const u32* pos) {
    u32 hw[3] = {1, 2, 3}, pr[3] = {4, 5, 6};
    ChainHw(fwd, up, pos, hw);
    PlaneFiring74FlareCpp(reinterpret_cast<const float*>(fwd), reinterpret_cast<const float*>(up), reinterpret_cast<const float*>(pos), reinterpret_cast<float*>(pr));
    ++combos;
    if (hw[0] != pr[0] || hw[1] != pr[1] || hw[2] != pr[2]) {
        if (++fails <= 10) {
            std::printf("DISCREPANCIA hw=%08x %08x %08x prod=%08x %08x %08x\n  fwd:", hw[0], hw[1], hw[2], pr[0], pr[1], pr[2]);
            for (int i = 0; i < 3; ++i) std::printf(" %08x", fwd[i]);
            std::printf(" up:");
            for (int i = 0; i < 3; ++i) std::printf(" %08x", up[i]);
            std::printf(" pos:");
            for (int i = 0; i < 3; ++i) std::printf(" %08x", pos[i]);
            std::printf("\n");
        }
    }
}
static u32 Background() {
    switch (Rnd() % 8) {
    case 0: return Rnd() % 2 ? 0u : 0x80000000u;
    case 1: return (Rnd() & 0x80000000u) | (Rnd() & 0x7FFFFFu);                       // desnormal o cero
    case 2: return (Rnd() & 0x80000000u) | ((Rnd() % 40 + 40u) << 23) | (Rnd() & 0x7FFFFFu);   // pequeno
    case 3: return (Rnd() & 0x80000000u) | 0x7F800000u;                                 // infinito
    case 4: return (Rnd() & 0x80000000u) | ((Rnd() % 30 + 225u) << 23) | (Rnd() & 0x7FFFFFu);   // grande (desborda al multiplicar por 2.5)
    default: return (Rnd() & 0x80000000u) | ((Rnd() % 24 + 116u) << 23) | (Rnd() & 0x7FFFFFu);   // moderado
    }
}

int main() {
    BuildPatterns();
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    unsigned csr = 0;
    __asm { stmxcsr csr }
    csr |= 0x8000u;   // FTZ
    __asm { ldmxcsr csr }
    u32 a[3], b[3], c[3];
    // a) todas las ternas de patrones en cada componente (las otras dos componentes con valores distintos mezclados)
    for (unsigned i = 0; i < nAll; ++i)
        for (unsigned j = 0; j < nAll; ++j)
            for (unsigned k = 0; k < nAll; ++k) {
                a[0] = allPatterns[i]; b[0] = allPatterns[j]; c[0] = allPatterns[k];
                a[1] = allPatterns[(i + 7) % nAll]; b[1] = allPatterns[(j + 11) % nAll]; c[1] = allPatterns[(k + 13) % nAll];
                a[2] = allPatterns[(i * 3 + 1) % nAll]; b[2] = allPatterns[(j * 5 + 2) % nAll]; c[2] = allPatterns[(k * 7 + 3) % nAll];
                Check(a, b, c);
            }
    // b) aleatorio denso en NaN
    for (unsigned n = 0; n < 20000000u; ++n) {
        for (int i = 0; i < 3; ++i) {
            a[i] = Rnd() % 3 == 0 ? nanPatterns[Rnd() % nNan] : Background();
            b[i] = Rnd() % 3 == 0 ? nanPatterns[Rnd() % nNan] : Background();
            c[i] = Rnd() % 3 == 0 ? nanPatterns[Rnd() % nNan] : Background();
            if (Rnd() % 6 == 0) b[i] = a[i] ^ (Rnd() % 2 ? 0x80000000u : 0u);       // misma carga, signo igual o distinto
            if (Rnd() % 6 == 0) c[i] = a[i] ^ (Rnd() % 2 ? 0x80000000u : 0u);
        }
        Check(a, b, c);
    }
    // c) sin NaN
    for (unsigned n = 0; n < 6000000u; ++n) {
        for (int i = 0; i < 3; ++i) { a[i] = Background(); b[i] = Background(); c[i] = Background(); }
        Check(a, b, c);
    }
    std::printf("combinaciones=%llu discrepancias=%llu\n", (unsigned long long)combos, (unsigned long long)fails);
    std::printf("patrones=%u (NaN %u)\n", nAll, nNan);
    return fails ? 1 : 0;
}
