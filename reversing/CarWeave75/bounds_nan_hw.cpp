// Microtest autonomo de 32 bits (herramienta de prueba, NO codigo de produccion; __asm permitido aqui): valida contra el HARDWARE x87 las dos funciones aritmeticas de CarWeave75
// (X87Math.cpp, bloque «Lote 75 (CarWeave75)»): Weave75BoundsCpp (rectangulo de busqueda y celdas del sector, 0x4325C7..0x4327AD, incluidas las reglas de NaN de las cuatro cotas)
// y Weave75FinalCpp (eleccion del rumbo, 0x432922..0x432B0A), cada una frente a una transcripcion con __asm de la secuencia x87 del EXE, bit a bit. El texto de produccion se incluye
// TAL CUAL desde weave_slice.inc, que genera bounds_nan_hw.py cortando X87Math.cpp (Lote 73 hasta PlaneFiring74, la ayuda CarLink74Pos y el bloque Lote 75 de CarWeave75) y X87Math.h.
// Estado de coma flotante como el juego: palabra de control x87 a 24 bits (redondeo al mas cercano, excepciones enmascaradas) y MXCSR con FTZ.
//
//  a) Weave75BoundsCpp: vehiculos con velocidad (x, y), escala y posicion tomadas de un conjunto de patrones (NaN silenciosos y de senal, de ambos signos y varias cargas; ceros, desnormales,
//     normales, maximos, infinitos), con y sin matriz, en todas las combinaciones (velocidad x escala x posicion) y 20 millones de combinaciones aleatorias densas en especiales.
//  b) Weave75FinalCpp: cuatro entradas (arg3, arg4, barrido 50, barrido 40) de patrones moderados y especiales (sin infinitos ni magnitudes enormes: los bucles de normalizacion del EXE no
//     terminarian), todas las combinaciones de un conjunto de patrones y 20 millones aleatorias.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <float.h>
#include "weave_slice.inc"

typedef uint32_t u32;
typedef uint64_t u64;

static u64 rngState = 0x9E3779B97F4A7C15ull;
static u32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return u32(rngState >> 16); }

// floor (CRT 0x8219F0, RC hacia abajo) y _ftol2 (0x821B40, truncamiento a 64 bits; fuera de rango o NaN = 0x8000000000000000) en x87 real
static __declspec(noinline) double FloorHw(double v) {
    unsigned short cw = 0, cw2 = 0;
    double r = 0;
    __asm {
        fld qword ptr [v]
        fnstcw word ptr [cw]
        mov ax, word ptr [cw]
        or ax, 0400h
        mov word ptr [cw2], ax
        fldcw word ptr [cw2]
        frndint
        fldcw word ptr [cw]
        fstp qword ptr [r]
    }
    return r;
}
static __declspec(noinline) int FtolHw(double v) {
    unsigned short cw = 0, cw2 = 0;
    long long r = 0;
    __asm {
        fld qword ptr [v]
        fnstcw word ptr [cw]
        mov ax, word ptr [cw]
        or ax, 0C00h
        mov word ptr [cw2], ax
        fldcw word ptr [cw2]
        fistp qword ptr [r]
        fldcw word ptr [cw]
    }
    return int(r & 0xFFFFFFFFll);
}

// 0x43265E..0x4326F3 (cada eje): fld limite; fmul 0.02; fadd 60.0; fst dword; fstp qword -> floor -> ftol2; si hace falta, otra vez con el float guardado
static __declspec(noinline) void SectorHw(u32 stored, int* first, int* second) {
    static const u32 k002 = 0x3CA3D70Au, k60 = 0x42700000u;
    float f = 0;
    double d = 0;
    __asm {
        fld dword ptr [stored]
        fmul dword ptr [k002]
        fadd dword ptr [k60]
        fst dword ptr [f]
        fstp qword ptr [d]
    }
    *first = FtolHw(FloorHw(d));
    const double d2 = double(f);
    *second = FtolHw(FloorHw(d2));
}

// 0x4325C7..0x43265A: limites del rectangulo (bits)
static __declspec(noinline) void BoundsHw(const unsigned char* vehicle, u32 scaleBits, u32* out) {
    static const u32 k25 = 0x40200000u, k1 = 0x3F800000u, k2 = 0x40000000u, k12 = 0x41400000u;
    u32 xmin = 0, ymin = 0, xmax = 0, ymax = 0;
    __asm {
        mov edi, vehicle
        fld dword ptr [edi+48h]
        fld dword ptr [edi+44h]
        fld st(0)
        fmul st, st(1)
        fld st(2)
        fmul st, st(3)
        faddp st(1), st
        fsqrt
        fstp st(2)
        fstp st(0)
        fmul dword ptr [k25]
        fadd dword ptr [k1]
        fcom dword ptr [k2]
        fnstsw ax
        test ah, 5
        jnp keep
        fstp st(0)
        fld dword ptr [k2]
    keep:
        mov eax, dword ptr [edi+14h]
        xor ebx, ebx
        cmp eax, ebx
        lea ecx, [eax+30h]
        jne p1
        lea ecx, [edi+4]
    p1:
        cmp eax, ebx
        fmul dword ptr [scaleBits]
        fmul dword ptr [k12]
        fld dword ptr [ecx]
        lea ecx, [eax+30h]
        fsub st, st(1)
        fstp dword ptr [xmin]
        jne p2
        lea ecx, [edi+4]
    p2:
        cmp eax, ebx
        fld st(0)
        fadd dword ptr [ecx]
        lea ecx, [eax+30h]
        fstp dword ptr [xmax]
        jne p3
        lea ecx, [edi+4]
    p3:
        cmp eax, ebx
        fld dword ptr [ecx+4]
        fsub st, st(1)
        fstp dword ptr [ymin]
        je p4
        add eax, 30h
        jmp p5
    p4:
        lea eax, [edi+4]
    p5:
        fadd dword ptr [eax+4]
        fstp dword ptr [ymax]
    }
    out[0] = xmin; out[1] = ymin; out[2] = xmax; out[3] = ymax;
}

static u64 combos, fails;
static void CheckBounds(const unsigned char* veh, u32 scale) {
    u32 hw[4] = {1, 2, 3, 4};
    BoundsHw(veh, scale, hw);
    int f, s, hwi[4];
    SectorHw(hw[0], &f, &s); hwi[0] = f <= 0 ? 0 : s;
    SectorHw(hw[1], &f, &s); hwi[1] = f <= 0 ? 0 : s;
    SectorHw(hw[2], &f, &s); hwi[2] = f >= 0x77 ? 0x77 : s;
    SectorHw(hw[3], &f, &s); hwi[3] = f >= 0x77 ? 0x77 : s;
    Weave75Bounds pr;
    memset(&pr, 0x5A, sizeof(pr));
    Weave75BoundsCpp(veh, std::bit_cast<float>(scale), &pr);
    ++combos;
    if (hw[0] != pr.xminBits || hw[1] != pr.yminBits || hw[2] != pr.xmaxBits || hw[3] != pr.ymaxBits || hwi[0] != pr.minX || hwi[1] != pr.minY || hwi[2] != pr.maxX || hwi[3] != pr.maxY) {
        if (++fails <= 10) {
            std::printf("DISCREPANCIA bounds hw=%08x %08x %08x %08x ints %d %d %d %d prod=%08x %08x %08x %08x ints %d %d %d %d\n  vel=%08x %08x scale=%08x mat=%08x pos=%08x %08x/%08x %08x\n",
                hw[0], hw[1], hw[2], hw[3], hwi[0], hwi[1], hwi[2], hwi[3], pr.xminBits, pr.yminBits, pr.xmaxBits, pr.ymaxBits, pr.minX, pr.minY, pr.maxX, pr.maxY,
                *(const u32*)(veh + 0x44), *(const u32*)(veh + 0x48), scale, *(const u32*)(veh + 0x14), *(const u32*)(veh + 4), *(const u32*)(veh + 8), *(const u32*)(veh + 0x1000 + 0x30), *(const u32*)(veh + 0x1000 + 0x34));
        }
    }
}

// 0x432922..0x432B0A: resultado (bits)
static __declspec(noinline) u32 FinalHw(u32 T, u32 H, u32 S50, u32 S40) {
    static const u32 kNegPi = 0xC0490FDBu, kPi = 0x40490FDBu, kTwoPi = 0x40C90FDBu, kHalf = 0x3F000000u, kHalfPi = 0x3FC90FDBu, kZero = 0u, kTol = 0x3DA3D70Au;
    u32 slotT = T;   // [esp+38]: el hueco de arg3, luego A
    u32 result = 0;
    u32 depth = 0;
    __asm {
        fld dword ptr [H]
        fsub dword ptr [slotT]
        fcom dword ptr [kNegPi]
        fnstsw ax
        test ah, 5
        jp a1
    a0:
        fadd dword ptr [kTwoPi]
        fcom dword ptr [kNegPi]
        fnstsw ax
        test ah, 5
        jnp a0
    a1:
        fcom dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        jne a2
    a1b:
        fsub dword ptr [kTwoPi]
        fcom dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        je a1b
    a2:
        fmul dword ptr [kHalf]
        fadd dword ptr [slotT]
        fcom dword ptr [kNegPi]
        fst dword ptr [slotT]
        fnstsw ax
        test ah, 5
        jp a3
    a2b:
        fadd dword ptr [kTwoPi]
        fcom dword ptr [kNegPi]
        fnstsw ax
        test ah, 5
        jnp a2b
        fst dword ptr [slotT]
    a3:
        fcom dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        jne a4
    a3b:
        fsub dword ptr [kTwoPi]
        fcom dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        je a3b
        fst dword ptr [slotT]
    a4:
        fsubr dword ptr [S50]
        fld dword ptr [S40]
        fsub dword ptr [slotT]
        fld st(1)
        fcomp dword ptr [kNegPi]
        fnstsw ax
        test ah, 5
        jp b1
    b0:
        fxch st(1)
        fadd dword ptr [kTwoPi]
        fxch st(1)
        fld st(1)
        fcomp dword ptr [kNegPi]
        fnstsw ax
        test ah, 5
        jnp b0
    b1:
        fld st(1)
        fcomp dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        jne b2
    b1b:
        fxch st(1)
        fsub dword ptr [kTwoPi]
        fxch st(1)
        fld st(1)
        fcomp dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        je b1b
    b2:
        fld st(1)
        fcomp dword ptr [kZero]
        fnstsw ax
        test ah, 5
        jp c1
        fxch st(1)
        fchs
        fxch st(1)
    c1:
        fcom dword ptr [kNegPi]
        fnstsw ax
        test ah, 5
        jp c3
    c2:
        fadd dword ptr [kTwoPi]
        fcom dword ptr [kNegPi]
        fnstsw ax
        test ah, 5
        jnp c2
    c3:
        fcom dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        jne c5
    c4:
        fsub dword ptr [kTwoPi]
        fcom dword ptr [kPi]
        fnstsw ax
        test ah, 41h
        je c4
    c5:
        fcom dword ptr [kZero]
        fnstsw ax
        test ah, 5
        jp d1
        fchs
    d1:
        fld st(1)
        fcomp dword ptr [kHalfPi]
        fnstsw ax
        test ah, 41h
        jne e1
        fcom dword ptr [kHalfPi]
        fnstsw ax
        test ah, 41h
        jne e1
        fstp st(0)
        fstp st(0)
        mov eax, dword ptr [slotT]
        mov dword ptr [result], eax
        jmp done
    e1:
        fld st(1)
        fsub st, st(1)
        fcom dword ptr [kZero]
        fnstsw ax
        test ah, 5
        jp e2
        fchs
    e2:
        fcomp dword ptr [kTol]
        fnstsw ax
        test ah, 5
        jp e4
    e3:
        fstp st(0)
        fstp st(0)
        mov eax, dword ptr [S40]
        mov dword ptr [result], eax
        jmp done
    e4:
        fxch st(1)
        fcomp st(1)
        fnstsw ax
        fstp st(0)
        test ah, 5
        jp e3
        mov eax, dword ptr [S50]
        mov dword ptr [result], eax
    done:
    }
    (void)depth;
    return result;
}

static void CheckFinal(u32 T, u32 H, u32 S50, u32 S40) {
    const u32 hw = FinalHw(T, H, S50, S40);
    volatile float r = Weave75FinalCpp(std::bit_cast<float>(T), std::bit_cast<float>(H), std::bit_cast<float>(S50), std::bit_cast<float>(S40));
    u32 pr;
    memcpy(&pr, const_cast<const float*>(&r), 4);
    ++combos;
    // el EXE devuelve un float cargado con fld (un NaN de senal saldria silenciado): se compara tras silenciar en ambos lados
    auto quiet = [](u32 b) { return ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu)) ? (b | 0x400000u) : b; };
    if (quiet(hw) != quiet(pr)) {
        if (++fails <= 10) std::printf("DISCREPANCIA final hw=%08x prod=%08x T=%08x H=%08x S50=%08x S40=%08x\n", hw, pr, T, H, S50, S40);
    }
}

static u32 patterns[160];
static unsigned nPat;
static u32 finalPatterns[160];
static unsigned nFin;
static void BuildPatterns() {
    const u32 qPay[] = {0u, 1u, 0x1234u, 0x155555u, 0x3FFFFFu};
    const u32 sPay[] = {1u, 0x1234u, 0x3FFFFFu};
    for (u32 sign = 0; sign < 2; ++sign) {
        for (u32 p : qPay) patterns[nPat++] = (sign << 31) | 0x7F800000u | 0x400000u | p;
        for (u32 p : sPay) patterns[nPat++] = (sign << 31) | 0x7F800000u | p;
    }
    const u32 mags[] = {0u, 1u, 0x7FFFFFu, 0x800000u, 0x3F800000u, 0x3F000000u, 0x40400000u, 0x41400000u, 0x42C80000u, 0x447A0000u, 0x7149F2CAu, 0x7F7FFFFFu, 0x7F800000u};
    for (u32 sign = 0; sign < 2; ++sign)
        for (u32 m : mags) patterns[nPat++] = (sign << 31) | m;
    // para la eleccion final: sin infinitos ni magnitudes enormes; con pi, pi/2, 0.08 y vecinos
    const u32 nanFin[] = {0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFFA00000u, 0x7FC12345u};
    for (u32 v : nanFin) finalPatterns[nFin++] = v;
    const u32 num[] = {0u, 0x80000000u, 1u, 0x80000001u, 0x7FFFFFu, 0x800000u, 0x3F800000u, 0xBF800000u, 0x40490FDBu, 0xC0490FDBu, 0x40490FDAu, 0x40490FDCu, 0xC0490FDAu, 0xC0490FDCu, 0x40C90FDBu,
                       0xC0C90FDBu, 0x3FC90FDBu, 0xBFC90FDBu, 0x3FC90FDAu, 0x3FC90FDCu, 0x3DA3D70Au, 0x3DA3D709u, 0x3DA3D70Bu, 0x40000000u, 0x40400000u, 0x41200000u, 0xC1200000u, 0x42C80000u, 0xC2C80000u,
                       0x447A0000u, 0xC47A0000u, 0x3F000000u, 0xBF000000u, 0x40A00000u, 0xC0A00000u};
    for (u32 v : num) finalPatterns[nFin++] = v;
}
static u32 Background() {
    switch (Rnd() % 8) {
    case 0: return Rnd() % 2 ? 0u : 0x80000000u;
    case 1: return (Rnd() & 0x80000000u) | (Rnd() & 0x7FFFFFu);                                  // desnormal o cero
    case 2: return (Rnd() & 0x80000000u) | ((Rnd() % 40 + 40u) << 23) | (Rnd() & 0x7FFFFFu);   // pequeno
    case 3: return (Rnd() & 0x80000000u) | 0x7F800000u;                                          // infinito
    case 4: return (Rnd() & 0x80000000u) | ((Rnd() % 30 + 225u) << 23) | (Rnd() & 0x7FFFFFu);   // grande
    default: return (Rnd() & 0x80000000u) | ((Rnd() % 24 + 116u) << 23) | (Rnd() & 0x7FFFFFu);  // moderado
    }
}
static u32 FinalBackground() {
    switch (Rnd() % 6) {
    case 0: return Rnd() % 2 ? 0u : 0x80000000u;
    case 1: return (Rnd() & 0x80000000u) | (Rnd() & 0x7FFFFFu);
    case 2: return finalPatterns[Rnd() % nFin];
    case 3: return (Rnd() & 0x80000000u) | ((Rnd() % 12 + 120u) << 23) | (Rnd() & 0x7FFFFFu);   // [0.06, 250] aprox.
    default: return (Rnd() & 0x80000000u) | ((Rnd() % 6 + 122u) << 23) | (Rnd() & 0x7FFFFFu);   // [0.125, 8)
    }
}

// vehiculo falso: posicion compacta en +4, matriz en +0x14 (apunta a un bloque con posicion en +0x30), velocidad en +0x44 / +0x48
static unsigned char vehMem[0x2000];
static void SetVehicle(bool hasMat, u32 vx, u32 vy, u32 px, u32 py) {
    memset(vehMem, 0, sizeof(vehMem));
    unsigned char* mat = vehMem + 0x1000;
    *(u32*)(vehMem + 0x44) = vx;
    *(u32*)(vehMem + 0x48) = vy;
    *(u32*)(vehMem + 4) = hasMat ? 0x3F800000u : px;
    *(u32*)(vehMem + 8) = hasMat ? 0x3F800000u : py;
    *(u32*)(vehMem + 0x14) = hasMat ? (u32)(uintptr_t)mat : 0u;
    *(u32*)(mat + 0x30) = px;
    *(u32*)(mat + 0x34) = py;
}

int main() {
    BuildPatterns();
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    unsigned csr = 0;
    __asm { stmxcsr csr }
    csr |= 0x8000u;   // FTZ
    __asm { ldmxcsr csr }
    // a) velocidad x escala x posicion (todas las combinaciones de un subconjunto representativo), con y sin matriz
    for (unsigned i = 0; i < nPat; i += 2)
        for (unsigned j = 0; j < nPat; ++j)
            for (unsigned k = 0; k < nPat; ++k) {
                SetVehicle((i + j + k) & 1, patterns[i], patterns[(i + 5) % nPat], patterns[k], patterns[(k + 3) % nPat]);
                CheckBounds(vehMem, patterns[j]);
            }
    // a2) aleatorio denso
    for (unsigned n = 0; n < 20000000u; ++n) {
        auto pick = [&]() { return Rnd() % 3 == 0 ? patterns[Rnd() % nPat] : Background(); };
        SetVehicle(Rnd() & 1, pick(), pick(), pick(), pick());
        CheckBounds(vehMem, pick());
    }
    // b) eleccion final: todas las combinaciones de un conjunto de patrones
    for (unsigned a = 0; a < nFin; ++a)
        for (unsigned b = 0; b < nFin; ++b)
            for (unsigned c = 0; c < nFin; c += 1)
                for (unsigned d = 0; d < nFin; d += 3)
                    CheckFinal(finalPatterns[a], finalPatterns[b], finalPatterns[c], finalPatterns[d]);
    for (unsigned n = 0; n < 20000000u; ++n) CheckFinal(FinalBackground(), FinalBackground(), FinalBackground(), FinalBackground());
    std::printf("combinaciones=%llu discrepancias=%llu\n", (unsigned long long)combos, (unsigned long long)fails);
    std::printf("patrones=%u finales=%u\n", nPat, nFin);
    return fails ? 1 : 0;
}
