// Microtest autonomo de 32 bits (herramienta de prueba, NO codigo de produccion; __asm permitido aqui): valida contra el HARDWARE x87 la aritmetica y la emulacion de NaN de las funciones
// Heli75*Cpp (X87Math.cpp, bloque Lote 75; usan HeatFire73Raw / HeatFire73Op del bloque Lote 73, validadas aparte por HeatFire73/nan_rules_hw). El texto de produccion se incluye TAL CUAL desde
// heli_slice.inc, que genera heli_nan_hw.py cortando X87Math.cpp desde Lote 73 hasta el final (sin copiar ni modificar la logica).
// Estado de coma flotante como el juego: palabra de control x87 a 24 bits (redondeo al mas cercano, excepciones enmascaradas) y MXCSR con FTZ.
//
// Cada funcion se compara con la transcripcion __asm de la secuencia x87 del EXE (desensamblado 0x6C4E60..0x6C541A y 0x6C5420..0x6C58D3), bit a bit:
//   Heli75DecayCpp   0x6C4EBD..0x6C4EE4   Heli75NearCpp   0x6C5261..0x6C52B7   Heli75DotCpp     0x6C5342..0x6C53BD (con Multiply3x3 0x59C790)   Heli75RatioCpp 0x6C5524..0x6C554D
//   Heli75WheelCpp   0x6C555A..0x6C55A6   Heli75AngleACpp 0x6C562C..0x6C56E9   Heli75AngleBCpp 0x6C568A..0x6C56E9   Heli75AddPosCpp 0x6C5729..0x6C5751
// con TODAS las combinaciones de un conjunto de patrones (NaN silenciosos y de senal x signo x cargas, ceros, desnormales, normales, maximos e infinitos) y con millones de combinaciones aleatorias densas en NaN.
// Para los angulos solo se usan patrones sin infinitos ni magnitudes grandes (el bucle de envolvente no termina con ellos ni en el EXE).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <float.h>
#include "heli_slice.inc"

typedef uint32_t u32;
typedef uint64_t u64;

static u64 rngState = 0x9E3779B97F4A7C15ull;
static u32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return u32(rngState >> 16); }

static const u32 kOne = 0x3F800000u, kZero = 0u, kK75 = 0x3F400000u, kLow = 0xC0C90FDBu, kTwoPi = 0x40C90FDBu, kK23 = 0x40133333u, kK400 = 0x43C80000u, kMin = 0x3C23D70Au, kK16 = 0x3FD47AE1u, kDotLimit = 0x3F733333u;

static __declspec(noinline) u32 HwRatio(u32 comp, u32 spr, u32 line) {
    u32 out = 0;
    __asm {
        fld dword ptr [spr]
        fdiv dword ptr [line]
        fsubr dword ptr [kOne]
        fld dword ptr [comp]
        fsub st(0), st(1)
        fld dword ptr [kOne]
        fsub st(0), st(2)
        fdivp st(1), st(0)
        fstp dword ptr [out]
        fstp st(0)
    }
    return out;
}
static __declspec(noinline) u32 HwWheel(u32 ratio, u32 z, u32 h, u32 spr, u32 wpIn, int direct) {
    u32 w = wpIn;
    __asm {
        fld dword ptr [z]
        fadd dword ptr [h]
        fld dword ptr [ratio]
        fcomp dword ptr [kZero]
        fnstsw ax
        test ah, 41h
        jne skip
        fld dword ptr [ratio]
        fmul dword ptr [spr]
        fsubp st(1), st(0)
    skip:
        fcom dword ptr [w]
        fnstsw ax
        test ah, 41h
        je store
        cmp direct, 0
        jne store
        fsub dword ptr [w]
        fmul dword ptr [kK75]
        fadd dword ptr [w]
    store:
        fstp dword ptr [w]
    }
    return w;
}
static __declspec(noinline) u32 HwAngleA(u32 step, u32 rotor, int heavy, u32 aIn) {
    u32 a = aIn;
    __asm {
        fld dword ptr [step]
        fmul dword ptr [rotor]
        cmp heavy, 0
        je nh
        fmul dword ptr [kK16]
    nh:
        fsubr dword ptr [a]
        fstp dword ptr [a]
        fld dword ptr [a]
        fcomp dword ptr [kLow]
        fnstsw ax
        test ah, 5
        jp done
        fld dword ptr [a]
    again:
        fadd dword ptr [kTwoPi]
        fcom dword ptr [kLow]
        fnstsw ax
        test ah, 5
        jnp again
        fstp dword ptr [a]
    done:
    }
    return a;
}
static __declspec(noinline) u32 HwAngleB(u32 step, u32 rotor, int twice, u32 aIn) {
    u32 a = aIn;
    __asm {
        fld dword ptr [step]
        fmul dword ptr [rotor]
        cmp twice, 0
        je nt
        fadd st(0), st(0)
        jmp merged
    nt:
        fmul dword ptr [kK23]
    merged:
        fsubr dword ptr [a]
        fstp dword ptr [a]
        fld dword ptr [a]
        fcomp dword ptr [kTwoPi]
        fnstsw ax
        test ah, 41h
        jne done
        fld dword ptr [a]
    again:
        fsub dword ptr [kTwoPi]
        fcom dword ptr [kTwoPi]
        fnstsw ax
        test ah, 41h
        je again
        fstp dword ptr [a]
    done:
    }
    return a;
}
static __declspec(noinline) u32 HwAdd(u32 pos, u32 saved) {
    u32 out = 0;
    __asm {
        fld dword ptr [pos]
        fadd dword ptr [saved]
        fstp dword ptr [out]
    }
    return out;
}
static __declspec(noinline) int HwDecay(u32 step, u32 k, u32* rotor) {
    int returned = 0;
    __asm {
        mov edx, rotor
        fld dword ptr [step]
        fmul dword ptr [k]
        fcom dword ptr [edx]
        fnstsw ax
        test ah, 5
        jp zeroit
        fld dword ptr [edx]
        fsub st(0), st(1)
        fstp dword ptr [edx]
        fstp st(0)
        mov returned, 1
        jmp done
    zeroit:
        mov dword ptr [edx], 0
        fstp st(0)
    done:
    }
    return returned;
}
static __declspec(noinline) int HwNear(const u32* h, const u32* c, u32* out) {
    int isNear = 0;
    __asm {
        mov esi, c
        mov edi, h
        mov edx, out
        fld dword ptr [esi + 8]
        fsub dword ptr [edi + 8]
        fstp dword ptr [edx + 8]
        fld dword ptr [esi + 4]
        fsub dword ptr [edi + 4]
        fstp dword ptr [edx + 4]
        fld dword ptr [esi]
        fsub dword ptr [edi]
        fst dword ptr [edx]
        fmul dword ptr [edx]
        fld dword ptr [edx + 4]
        fmul dword ptr [edx + 4]
        faddp st(1), st(0)
        fld dword ptr [edx + 8]
        fmul dword ptr [edx + 8]
        faddp st(1), st(0)
        fst dword ptr [edx + 12]
        fcomp dword ptr [kK400]
        fnstsw ax
        test ah, 5
        jp no
        mov isNear, 1
    no:
    }
    return isNear;
}
static __declspec(noinline) int HwDot(const u32* m, const u32* v, const u32* d, u32 dist2) {
    u32 r[3];
    int fire = 0;
    __asm {
        mov ecx, m
        mov eax, v
        fld dword ptr [ecx + 20h]
        fmul dword ptr [eax + 8]
        fld dword ptr [ecx + 10h]
        fmul dword ptr [eax + 4]
        faddp st(1), st(0)
        fld dword ptr [ecx]
        fmul dword ptr [eax]
        faddp st(1), st(0)
        fstp dword ptr [r]
        fld dword ptr [ecx + 24h]
        fmul dword ptr [eax + 8]
        fld dword ptr [ecx + 4]
        fmul dword ptr [eax]
        faddp st(1), st(0)
        fld dword ptr [ecx + 14h]
        fmul dword ptr [eax + 4]
        faddp st(1), st(0)
        fstp dword ptr [r + 4]
        fld dword ptr [ecx + 28h]
        fmul dword ptr [eax + 8]
        fld dword ptr [ecx + 8]
        fmul dword ptr [eax]
        faddp st(1), st(0)
        fld dword ptr [ecx + 18h]
        fmul dword ptr [eax + 4]
        faddp st(1), st(0)
        fstp dword ptr [r + 8]
        fld dword ptr [dist2]
        fsqrt
        fld dword ptr [kMin]
        fcomp st(1)
        fnstsw ax
        test ah, 41h
        jne keep
        fstp st(0)
        fld dword ptr [kMin]
    keep:
        fdivr dword ptr [kOne]
        mov edx, d
        fld dword ptr [edx]
        fmul st(0), st(1)
        fld st(1)
        fmul dword ptr [edx + 4]
        fxch st(2)
        fmul dword ptr [edx + 8]
        fld dword ptr [r + 8]
        fmul st(0), st(1)
        fld dword ptr [r + 4]
        fmul st(0), st(4)
        faddp st(1), st(0)
        fld dword ptr [r]
        fmul st(0), st(3)
        faddp st(1), st(0)
        fxch st(3)
        fxch st(1)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fabs
        fcomp dword ptr [kDotLimit]
        fnstsw ax
        test ah, 41h
        jne no
        mov fire, 1
    no:
    }
    return fire;
}

static u32 nanPatterns[64];
static unsigned nNan;
static u32 allPatterns[160];
static unsigned nAll;
static u32 safePatterns[160];   // sin infinitos ni magnitudes grandes
static unsigned nSafe;
static void BuildPatterns() {
    const u32 qPay[] = {0u, 1u, 2u, 3u, 0x100u, 0x1234u, 0x155555u, 0x200000u, 0x3FFFFEu, 0x3FFFFFu};
    const u32 sPay[] = {1u, 2u, 3u, 0x100u, 0x1234u, 0x155555u, 0x200000u, 0x3FFFFFu};
    for (u32 sign = 0; sign < 2; ++sign) {
        for (u32 p : qPay) nanPatterns[nNan++] = (sign << 31) | 0x7F800000u | 0x400000u | p;
        for (u32 p : sPay) nanPatterns[nNan++] = (sign << 31) | 0x7F800000u | p;
    }
    const u32 mags[] = {0u, 1u, 0x7FFFFFu, 0x800000u, 0x3F800000u, 0x3F000000u, 0x40400000u, 0x3C23D70Au, 0x3F060A92u, 0x40C90FDBu, 0x41C80000u, 0x7149F2CAu, 0x7F7FFFFFu, 0x7F800000u};
    for (unsigned i = 0; i < nNan; ++i) { allPatterns[nAll++] = nanPatterns[i]; safePatterns[nSafe++] = nanPatterns[i]; }
    for (u32 sign = 0; sign < 2; ++sign)
        for (u32 m : mags) {
            allPatterns[nAll++] = (sign << 31) | m;
            if (m <= 0x41C80000u) safePatterns[nSafe++] = (sign << 31) | m;
        }
}
static u32 Background(bool safe) {
    switch (Rnd() % 8) {
    case 0: return Rnd() % 2 ? 0u : 0x80000000u;
    case 1: return (Rnd() & 0x80000000u) | (Rnd() & 0x7FFFFFu);                       // desnormal o cero
    case 2: return (Rnd() & 0x80000000u) | ((Rnd() % 40 + 40u) << 23) | (Rnd() & 0x7FFFFFu);   // pequeno
    case 3: return safe ? (Rnd() & 0x80000000u) | 0x3F800000u : (Rnd() & 0x80000000u) | 0x7F800000u;   // infinito
    case 4: return safe ? (Rnd() & 0x80000000u) | ((Rnd() % 8 + 124u) << 23) | (Rnd() & 0x7FFFFFu) : (Rnd() & 0x80000000u) | ((Rnd() % 30 + 225u) << 23) | (Rnd() & 0x7FFFFFu);   // grande
    default: return (Rnd() & 0x80000000u) | ((Rnd() % (safe ? 13u : 24u) + 116u) << 23) | (Rnd() & 0x7FFFFFu);   // moderado (con safe, menor que 2^2)
    }
}
static u32 Any(bool safe) { return Rnd() % 3 == 0 ? nanPatterns[Rnd() % nNan] : Background(safe); }

static u64 combos, fails;
static void Fail(const char* what, u32 a, u32 b, u32 c, u32 d, u32 hw, u32 pr) {
    if (++fails <= 20) std::printf("DISCREPANCIA %s in=%08x %08x %08x %08x hw=%08x prod=%08x\n", what, a, b, c, d, hw, pr);
}
static u32 Bi(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Los helpers reciben floats por valor: se llaman con los bits en la pila (como lo hace el codigo SSE de la DLL) para que un sNaN no pase por un fld (que lo silenciaria)
typedef float (__cdecl* RatioFn)(u32, u32, u32);
typedef void (__cdecl* WheelFn)(u32, u32, u32, u32, u32*, bool);
typedef void (__cdecl* AngleAFn)(u32, u32, bool, u32, u32*);
typedef void (__cdecl* AngleBFn)(u32, u32, bool, u32*);
typedef bool (__cdecl* DecayFn)(u32, u32, u32*);
typedef bool (__cdecl* DotFn)(const u32*, const u32*, const u32*, u32);

static void CheckRatio(u32 comp, u32 spr, u32 line) {
    const u32 hw = HwRatio(comp, spr, line), pr = Bi(reinterpret_cast<RatioFn>(&Heli75RatioCpp)(comp, spr, line));
    ++combos;
    if (hw != pr) Fail("ratio", comp, spr, line, 0, hw, pr);
}
static void CheckWheel(u32 ratio, u32 z, u32 h, u32 spr, u32 wp, int direct) {
    const u32 hw = HwWheel(ratio, z, h, spr, wp, direct);
    u32 w = wp;
    reinterpret_cast<WheelFn>(&Heli75WheelCpp)(ratio, z, h, spr, &w, direct != 0);
    ++combos;
    if (hw != w) Fail(direct ? "wheel(direct)" : "wheel", ratio, z, h, spr ^ (wp * 3u), hw, w);
}
static void CheckAngleA(u32 step, u32 rotor, int heavy, u32 a) {
    const u32 hw = HwAngleA(step, rotor, heavy, a);
    u32 v = a;
    reinterpret_cast<AngleAFn>(&Heli75AngleACpp)(step, rotor, heavy != 0, kK16, &v);
    ++combos;
    if (hw != v) Fail(heavy ? "angleA(heavy)" : "angleA", step, rotor, a, 0, hw, v);
}
static void CheckAngleB(u32 step, u32 rotor, int twice, u32 a) {
    const u32 hw = HwAngleB(step, rotor, twice, a);
    u32 v = a;
    reinterpret_cast<AngleBFn>(&Heli75AngleBCpp)(step, rotor, twice != 0, &v);
    ++combos;
    if (hw != v) Fail(twice ? "angleB(twice)" : "angleB", step, rotor, a, 0, hw, v);
}
static void CheckAdd(u32 pos, u32 saved) {
    u32 p[3] = {pos, saved, pos ^ saved};
    const u32 s[3] = {saved, pos, saved ^ 0x80000000u};
    Heli75AddPosCpp(reinterpret_cast<float*>(p), reinterpret_cast<const float*>(s));
    ++combos;
    const u32 hw0 = HwAdd(pos, saved), hw1 = HwAdd(saved, pos), hw2 = HwAdd(pos ^ saved, saved ^ 0x80000000u);
    if (hw0 != p[0] || hw1 != p[1] || hw2 != p[2]) Fail("addpos", pos, saved, 0, 0, hw0, p[0]);
}
static void CheckDecay(u32 step, u32 k, u32 rotor) {
    u32 hw = rotor;
    const int hr = HwDecay(step, k, &hw);
    u32 v = rotor;
    const bool pr = reinterpret_cast<DecayFn>(&Heli75DecayCpp)(step, k, &v);
    ++combos;
    if (hw != v || hr != (pr ? 1 : 0)) Fail("decay", step, k, rotor, 0, hw, v);
}
static void CheckNear(const u32* h, const u32* c) {
    u32 hwOut[4] = {1, 2, 3, 4};
    u32 prOut[4] = {0, 0, 0, 0};
    const int hr = HwNear(h, c, hwOut);
    const bool pr = Heli75NearCpp(reinterpret_cast<const float*>(h), reinterpret_cast<const float*>(c), reinterpret_cast<float*>(prOut));
    ++combos;
    if (hr != (pr ? 1 : 0) || (hr && (hwOut[0] != prOut[0] || hwOut[1] != prOut[1] || hwOut[2] != prOut[2] || hwOut[3] != prOut[3]))) Fail("near", h[0], c[0], hwOut[3], prOut[3], hwOut[0], prOut[0]);
}
static void CheckDot(const u32* m, const u32* v, const u32* d, u32 dist2) {
    const int hw = HwDot(m, v, d, dist2);
    const bool pr = reinterpret_cast<DotFn>(&Heli75DotCpp)(m, v, d, dist2);
    ++combos;
    if (hw != (pr ? 1 : 0)) Fail("dot", m[0], v[0], d[0], dist2, hw, pr);
}

int main() {
    BuildPatterns();
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    unsigned csr = 0;
    __asm { stmxcsr csr }
    csr |= 0x8000u;   // FTZ
    __asm { ldmxcsr csr }
    // a) todas las combinaciones de patrones
    for (unsigned i = 0; i < nAll; ++i)
        for (unsigned j = 0; j < nAll; ++j) {
            CheckAdd(allPatterns[i], allPatterns[j]);
            for (unsigned k = 0; k < nAll; ++k) {
                CheckRatio(allPatterns[i], allPatterns[j], allPatterns[k]);
                CheckDecay(allPatterns[i], allPatterns[j], allPatterns[k]);
            }
        }
    for (unsigned i = 0; i < nAll; i += 1)
        for (unsigned j = 0; j < nAll; j += 2)
            for (unsigned k = 0; k < nAll; k += 3)
                for (unsigned l = 0; l < nAll; l += 5) {
                    const u32 ratio = allPatterns[i], z = allPatterns[j], h = allPatterns[k], spr = allPatterns[l];
                    const u32 wp = allPatterns[(i * 7 + j * 3 + k + l) % nAll];
                    CheckWheel(ratio, z, h, spr, wp, 0);
                    CheckWheel(ratio, z, h, spr, wp, 1);
                }
    for (unsigned i = 0; i < nSafe; ++i)
        for (unsigned j = 0; j < nSafe; ++j)
            for (unsigned k = 0; k < nSafe; ++k) {
                CheckAngleA(safePatterns[i], safePatterns[j], 0, safePatterns[k]);
                CheckAngleA(safePatterns[i], safePatterns[j], 1, safePatterns[k]);
                CheckAngleB(safePatterns[i], safePatterns[j], 0, safePatterns[k]);
                CheckAngleB(safePatterns[i], safePatterns[j], 1, safePatterns[k]);
            }
    // b) aleatorio denso en NaN
    for (unsigned n = 0; n < 6000000u; ++n) {
        CheckRatio(Any(false), Any(false), Any(false));
        CheckWheel(Any(false), Any(false), Any(false), Any(false), Any(false), int(Rnd() % 2));
        CheckAngleA(Any(true), Any(true), Any(true), int(Rnd() % 2) ? Any(true) : Background(true));
        CheckAngleB(Any(true), Any(true), int(Rnd() % 2), Any(true));
        CheckAdd(Any(false), Any(false));
        CheckDecay(Any(false), Any(false), Any(false));
        u32 h[3], c[3], m[12], v[3], d[3];
        for (int i = 0; i < 3; ++i) { h[i] = Any(false); c[i] = Any(false); v[i] = Any(false); d[i] = Any(false); }
        for (int i = 0; i < 12; ++i) m[i] = Any(false);
        CheckNear(h, c);
        CheckDot(m, v, d, Any(false) & 0x7FFFFFFFu);
    }
    // c) sin NaN (desnormales, ceros con signo, desbordamientos); y la zona util del dot (vectores unitarios)
    for (unsigned n = 0; n < 3000000u; ++n) {
        CheckRatio(Background(false), Background(false), Background(false));
        CheckWheel(Background(false), Background(false), Background(false), Background(false), Background(false), int(Rnd() % 2));
        CheckAngleA(Background(true), Background(true), int(Rnd() % 2), Background(true));
        CheckAngleB(Background(true), Background(true), int(Rnd() % 2), Background(true));
        CheckAdd(Background(false), Background(false));
        CheckDecay(Background(false), Background(false), Background(false));
        u32 h[3], c[3], m[12], v[3], d[3];
        for (int i = 0; i < 3; ++i) { h[i] = Background(false); c[i] = Background(false); v[i] = Background(false); d[i] = Background(false); }
        for (int i = 0; i < 12; ++i) m[i] = Background(false);
        CheckNear(h, c);
        CheckDot(m, v, d, Background(false) & 0x7FFFFFFFu);
    }
    std::printf("combinaciones=%llu discrepancias=%llu\n", (unsigned long long)combos, (unsigned long long)fails);
    std::printf("patrones=%u (NaN %u, seguros %u)\n", nAll, nNan, nSafe);
    return fails ? 1 : 0;
}
