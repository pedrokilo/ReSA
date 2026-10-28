// Microtest autonomo de 32 bits (herramienta de prueba, NO codigo de produccion; __asm permitido aqui): valida contra el HARDWARE x87 las cuatro ayudantes aritmeticas de CarNodes75
// (X87Math.cpp, bloque «Lote 75 (CarNodes75)»): Nodes75RegionCpp (0x42BD85..0x42BE56), Nodes75InRangeCpp (0x42BEB0..0x42BF44), Nodes75Z3Cpp (0x42C029..0x42C06C) y Nodes75ScoreCpp (0x42C12F..0x42C172),
// cada una frente a una transcripcion con __asm de la secuencia x87 del EXE, bit a bit, con NaN (silenciosos y de senal, de ambos signos y varias cargas), infinitos, desnormales, ceros con signo y maximos.
// El texto de produccion se incluye TAL CUAL desde nodes_slice.inc, que genera nodes_nan_hw.py cortando X87Math.cpp (bloque Lote 75 de CarNodes75) y X87Math.h.
// Estado de coma flotante como el juego: palabra de control x87 a 24 bits (redondeo al mas cercano, excepciones enmascaradas) y MXCSR con FTZ.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <float.h>
#include "nodes_slice.inc"

typedef uint32_t u32;
typedef uint64_t u64;

static u64 rngState = 0x9E3779B97F4A7C15ull;
static u32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return u32(rngState >> 16); }

static const u32 k200 = 0x43480000u, k550 = 0x44098000u, k125 = 0x3E000000u, k150 = 0x43160000u, k3 = 0x40400000u, k1 = 0x3F800000u, k5 = 0x40A00000u;

// 0x42BD85..0x42BE56 : ventana (minX, minY, maxX, maxY)
static __declspec(noinline) void RegionHw(u32 posx, u32 posy, u32 xc, u32 yc, int rx, int ry, int* out) {
    int hiX = 0, hiY = 0, minX = 0, minY = 0;
    u32 dxr = 0;
    __asm {
        mov edi, rx
        mov esi, ry
        fld dword ptr [xc]
        fsubr dword ptr [posx]
        fstp dword ptr [dxr]
        fld dword ptr [yc]
        fsubr dword ptr [posy]
        fld dword ptr [dxr]
        mov edx, edi
        fcomp dword ptr [k200]
        mov dword ptr [hiX], edi
        mov ecx, esi
        mov dword ptr [hiY], esi
        fnstsw ax
        test ah, 5
        jp l1
        lea edx, [edi-1]
    l1:
        fcom dword ptr [k200]
        fnstsw ax
        test ah, 5
        jp l2
        lea ecx, [esi-1]
    l2:
        fld dword ptr [dxr]
        fcomp dword ptr [k550]
        fnstsw ax
        test ah, 41h
        jne l3
        inc edi
        mov dword ptr [hiX], edi
    l3:
        fcomp dword ptr [k550]
        fnstsw ax
        test ah, 41h
        jne l4
        inc esi
        mov dword ptr [hiY], esi
    l4:
        xor eax, eax
        test edx, edx
        setl al
        dec eax
        and eax, edx
        xor edx, edx
        test ecx, ecx
        setl dl
        dec edx
        and edx, ecx
        mov dword ptr [minX], eax
        mov dword ptr [minY], edx
        mov ecx, 7
        cmp dword ptr [hiX], ecx
        jle l5
        mov dword ptr [hiX], ecx
    l5:
        cmp dword ptr [hiY], ecx
        jle l6
        mov dword ptr [hiY], ecx
    l6:
    }
    out[0] = minX; out[1] = minY; out[2] = hiX; out[3] = hiY;
}

// 0x42BEB0..0x42BF44 : el nodo esta a menos de 150.0
static __declspec(noinline) int InRangeHw(const u32* pos, int ix, int iy, int iz) {
    u32 nz = 0, dxs = 0;
    int result = 0;
    __asm {
        mov eax, pos
        fild dword ptr [ix]
        fmul dword ptr [k125]
        fild dword ptr [iy]
        fmul dword ptr [k125]
        fild dword ptr [iz]
        fmul dword ptr [k125]
        fstp dword ptr [nz]
        fxch st(1)
        fsub dword ptr [eax]
        fstp dword ptr [dxs]
        fsub dword ptr [eax+4]
        fld dword ptr [nz]
        fsub dword ptr [eax+8]
        fld st(0)
        fmul st, st(1)
        fld st(2)
        fmul st, st(3)
        faddp st(1), st
        fld dword ptr [dxs]
        fmul dword ptr [dxs]
        faddp st(1), st
        fsqrt
        fstp st(2)
        fstp st(0)
        fcomp dword ptr [k150]
        fnstsw ax
        test ah, 5
        jp no
        mov dword ptr [result], 1
    no:
    }
    return result;
}

static __declspec(noinline) u32 Z3Hw(u32 z) {
    u32 out = 0;
    __asm {
        fld dword ptr [z]
        fmul dword ptr [k3]
        fstp dword ptr [out]
    }
    return out;
}

// 0x42C12F..0x42C172
static __declspec(noinline) int ScoreHw(const u32* n, const u32* f, u32 dist, u32* best) {
    int result = 0;
    __asm {
        mov esi, n
        mov edi, f
        mov ebx, best
        fld dword ptr [esi+8]
        fmul dword ptr [edi+8]
        fld dword ptr [esi+4]
        fmul dword ptr [edi+4]
        faddp st(1), st
        fld dword ptr [esi]
        fmul dword ptr [edi]
        faddp st(1), st
        fsubr dword ptr [k1]
        fmul dword ptr [k5]
        fadd dword ptr [dist]
        fcom dword ptr [ebx]
        fnstsw ax
        test ah, 5
        jp skip
        fstp dword ptr [ebx]
        mov dword ptr [result], 1
        jmp done
    skip:
        fstp st(0)
    done:
    }
    return result;
}

static u32 patterns[160];
static unsigned nPat;
static void BuildPatterns() {
    const u32 qPay[] = {0u, 1u, 0x1234u, 0x155555u, 0x3FFFFFu};
    const u32 sPay[] = {1u, 0x1234u, 0x3FFFFFu};
    for (u32 sign = 0; sign < 2; ++sign) {
        for (u32 p : qPay) patterns[nPat++] = (sign << 31) | 0x7F800000u | 0x400000u | p;
        for (u32 p : sPay) patterns[nPat++] = (sign << 31) | 0x7F800000u | p;
    }
    const u32 mags[] = {0u, 1u, 0x7FFFFFu, 0x800000u, 0x3F800000u, 0x3F000000u, 0x40400000u, 0x41400000u, 0x42C80000u, 0x43480000u, 0x44098000u, 0x43160000u, 0x447A0000u, 0x7149F2CAu, 0x7F7FFFFFu, 0x7F800000u,
                        0x4347FFFFu, 0x43480001u, 0x44097FFFu, 0x44098001u, 0x4315FFFFu, 0x43160001u, 0x461C4000u};
    for (u32 sign = 0; sign < 2; ++sign)
        for (u32 m : mags) patterns[nPat++] = (sign << 31) | m;
}
static u32 Background() {
    switch (Rnd() % 9) {
    case 0: return Rnd() % 2 ? 0u : 0x80000000u;
    case 1: return (Rnd() & 0x80000000u) | (Rnd() & 0x7FFFFFu);                                  // desnormal o cero
    case 2: return (Rnd() & 0x80000000u) | ((Rnd() % 40 + 40u) << 23) | (Rnd() & 0x7FFFFFu);   // pequeno
    case 3: return (Rnd() & 0x80000000u) | 0x7F800000u;                                          // infinito
    case 4: return (Rnd() & 0x80000000u) | ((Rnd() % 30 + 225u) << 23) | (Rnd() & 0x7FFFFFu);   // grande
    case 5: return (Rnd() & 0x80000000u) | ((Rnd() % 6 + 133u) << 23) | (Rnd() & 0x7FFFFFu);    // 32..2048: entorno de 200, 550, 150
    default: return (Rnd() & 0x80000000u) | ((Rnd() % 24 + 116u) << 23) | (Rnd() & 0x7FFFFFu);  // moderado
    }
}
static u32 Pat() { return Rnd() % 3 == 0 ? patterns[Rnd() % nPat] : Background(); }
static int RegionPick() {
    static const int sp[] = {0, 1, 7, 8, 9, -1, -2, 100, 2147483647, -2147483647 - 1, 6, 2};
    return Rnd() % 3 == 0 ? sp[Rnd() % (sizeof(sp) / sizeof(sp[0]))] : int(Rnd() % 12) - 2;
}

static u64 combos, fails;
static void Report(const char* what, unsigned long long a, unsigned long long b, const u32* ctx, int nctx) {
    if (++fails <= 12) {
        std::printf("DISCREPANCIA %s hw=%llx prod=%llx ctx:", what, a, b);
        for (int i = 0; i < nctx; ++i) std::printf(" %08x", ctx[i]);
        std::printf("\n");
    }
}

int main() {
    BuildPatterns();
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    unsigned csr = 0;
    __asm { stmxcsr csr }
    csr |= 0x8000u;   // FTZ
    __asm { ldmxcsr csr }
    // region: todas las parejas de (posicion, coordenada) de los patrones y regiones especiales
    for (unsigned i = 0; i < nPat; ++i)
        for (unsigned j = 0; j < nPat; ++j) {
            for (int r = 0; r < 4; ++r) {
                const int rx = RegionPick(), ry = RegionPick();
                int hw[4], pr[4];
                RegionHw(patterns[i], patterns[(i + 3) % nPat], patterns[j], patterns[(j + 5) % nPat], rx, ry, hw);
                Nodes75Region w;
                Nodes75RegionCpp(std::bit_cast<float>(patterns[i]), std::bit_cast<float>(patterns[(i + 3) % nPat]), std::bit_cast<float>(patterns[j]), std::bit_cast<float>(patterns[(j + 5) % nPat]), rx, ry, &w);
                pr[0] = w.minX; pr[1] = w.minY; pr[2] = w.maxX; pr[3] = w.maxY;
                ++combos;
                if (memcmp(hw, pr, sizeof(hw))) { const u32 ctx[] = {patterns[i], patterns[(i + 3) % nPat], patterns[j], patterns[(j + 5) % nPat], u32(rx), u32(ry)}; Report("region", u64(u32(hw[0])) | (u64(u32(hw[1])) << 32), u64(u32(pr[0])) | (u64(u32(pr[1])) << 32), ctx, 6); }
            }
        }
    for (unsigned n = 0; n < 12000000u; ++n) {
        const u32 px = Pat(), py = Pat(), xc = Pat(), yc = Pat();
        const int rx = RegionPick(), ry = RegionPick();
        int hw[4], pr[4];
        RegionHw(px, py, xc, yc, rx, ry, hw);
        Nodes75Region w;
        Nodes75RegionCpp(std::bit_cast<float>(px), std::bit_cast<float>(py), std::bit_cast<float>(xc), std::bit_cast<float>(yc), rx, ry, &w);
        pr[0] = w.minX; pr[1] = w.minY; pr[2] = w.maxX; pr[3] = w.maxY;
        ++combos;
        if (memcmp(hw, pr, sizeof(hw))) { const u32 ctx[] = {px, py, xc, yc, u32(rx), u32(ry)}; Report("region", u64(u32(hw[0])) | (u64(u32(hw[1])) << 32), u64(u32(pr[0])) | (u64(u32(pr[1])) << 32), ctx, 6); }
    }
    // dentro de rango: posicion (3 floats) y nodo (enteros de 16 bits)
    for (unsigned n = 0; n < 20000000u; ++n) {
        u32 pos[3] = {Pat(), Pat(), Pat()};
        if (Rnd() % 4 == 0) { for (int k = 0; k < 3; ++k) pos[k] = std::bit_cast<u32>(float(int(Rnd() % 4000) - 2000) * 0.125f); }
        const int ix = Rnd() % 5 == 0 ? int(Rnd() % 65536) - 32768 : int(Rnd() % 4000) - 2000, iy = int(Rnd() % 4000) - 2000, iz = Rnd() % 6 == 0 ? int(Rnd() % 65536) - 32768 : int(Rnd() % 400) - 200;
        const int hw = InRangeHw(pos, ix, iy, iz);
        const int pr = Nodes75InRangeCpp(reinterpret_cast<const float*>(pos), ix, iy, iz) ? 1 : 0;
        ++combos;
        if (hw != pr) { const u32 ctx[] = {pos[0], pos[1], pos[2], u32(ix), u32(iy), u32(iz)}; Report("rango", hw, pr, ctx, 6); }
    }
    // cerca del borde de 150: nodo a 150 +- pasos de 1/8 de la posicion
    for (unsigned n = 0; n < 2000000u; ++n) {
        const int px = int(Rnd() % 2000) - 1000;
        u32 pos[3] = {std::bit_cast<u32>(float(px) * 0.125f), std::bit_cast<u32>(float(int(Rnd() % 2000) - 1000) * 0.125f), std::bit_cast<u32>(float(int(Rnd() % 2000) - 1000) * 0.125f)};
        const int dx = int(Rnd() % 3000) - 1500, dy = int(Rnd() % 3000) - 1500, dz = int(Rnd() % 100) - 50;
        const int ix = px + dx, iy = int(std::bit_cast<float>(pos[1]) * 8.0f) + dy, iz = int(std::bit_cast<float>(pos[2]) * 8.0f) + dz;
        const int hw = InRangeHw(pos, ix, iy, iz);
        const int pr = Nodes75InRangeCpp(reinterpret_cast<const float*>(pos), ix, iy, iz) ? 1 : 0;
        ++combos;
        if (hw != pr) { const u32 ctx[] = {pos[0], pos[1], pos[2], u32(ix), u32(iy), u32(iz)}; Report("rango-borde", hw, pr, ctx, 6); }
    }
    // z * 3.0
    for (unsigned i = 0; i < nPat; ++i) {
        ++combos;
        volatile float r = Nodes75Z3Cpp(std::bit_cast<float>(patterns[i]));
        u32 pr;
        memcpy(&pr, const_cast<const float*>(&r), 4);
        if (Z3Hw(patterns[i]) != pr) { const u32 ctx[] = {patterns[i]}; Report("z3", Z3Hw(patterns[i]), pr, ctx, 1); }
    }
    for (unsigned n = 0; n < 5000000u; ++n) {
        const u32 z = Pat();
        ++combos;
        volatile float r = Nodes75Z3Cpp(std::bit_cast<float>(z));
        u32 pr;
        memcpy(&pr, const_cast<const float*>(&r), 4);
        if (Z3Hw(z) != pr) { const u32 ctx[] = {z}; Report("z3", Z3Hw(z), pr, ctx, 1); }
    }
    // puntuacion: normal, adelante, distancia y mejor actual
    for (unsigned n = 0; n < 25000000u; ++n) {
        u32 nv[3] = {Pat(), Pat(), Pat()}, fv[3] = {Pat(), Pat(), Pat()};
        if (Rnd() % 3 == 0) { for (int k = 0; k < 3; ++k) { nv[k] = std::bit_cast<u32>(float(int(Rnd() % 2001) - 1000) * 0.001f); fv[k] = std::bit_cast<u32>(float(int(Rnd() % 2001) - 1000) * 0.001f); } }
        const u32 dist = Rnd() % 3 == 0 ? Pat() : std::bit_cast<u32>(float(Rnd() % 100000) * 0.01f);
        u32 bestHw = Rnd() % 4 == 0 ? Pat() : 0x47C34FF3u + (Rnd() % 5) - 2, bestPr = bestHw;
        const int hw = ScoreHw(nv, fv, dist, &bestHw);
        const int pr = Nodes75ScoreCpp(reinterpret_cast<const float*>(nv), reinterpret_cast<const float*>(fv), std::bit_cast<float>(dist), reinterpret_cast<float*>(&bestPr)) ? 1 : 0;
        ++combos;
        if (hw != pr || bestHw != bestPr) { const u32 ctx[] = {nv[0], nv[1], nv[2], fv[0], fv[1], fv[2], dist, bestHw, bestPr}; Report("puntuacion", hw | (u64(bestHw) << 32), pr | (u64(bestPr) << 32), ctx, 9); }
    }
    std::printf("combinaciones=%llu discrepancias=%llu\n", (unsigned long long)combos, (unsigned long long)fails);
    std::printf("patrones=%u\n", nPat);
    return fails ? 1 : 0;
}
