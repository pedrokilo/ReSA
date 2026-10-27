// Microtest autonomo de 32 bits (herramienta de prueba, NO codigo de produccion; __asm permitido aqui): valida contra el HARDWARE x87 la emulacion de NaN de
// HeliSwat74WorldPosCpp (X87Math.cpp, bloque «Lote 74», que reutiliza HeatFire73Raw / HeatFire73Op del bloque «Lote 73»). El texto de produccion se incluye TAL CUAL desde
// nan_rules_slice.inc, que genera nan_rules_hw.py cortando X87Math.cpp desde «// --- Lote 73» hasta el final (sin copiar ni modificar la logica).
// Estado de coma flotante como el juego: palabra de control x87 a 24 bits (redondeo al mas cercano, excepciones enmascaradas) y MXCSR con FTZ.
//
// Parte 1 (reglas de operacion): fadd/fsub/fmul con el operando destino en registro (cargado con fld, que silencia un NaN de senal) y el operando fuente en
//   registro (RR) o en memoria float (RM, puede ser NaN de senal), para TODAS las parejas de un conjunto de patrones: NaN silenciosos y de senal x signo x cargas
//   (menor/mayor/igual, solo difiere el signo) y otros operandos (ceros, desnormales, normales, maximos, infinitos). Se compara el resultado bit a bit con
//   HeatFire73Op(hwNan, Raw(x,true), Raw(y,RR)) y la bandera IE del hardware con la regla (fuente de senal en memoria, o NaN generado por 0*inf / inf-inf).
// Parte 2 (cadena completa): HeliSwat74WorldPosCpp frente a una transcripcion con __asm de la secuencia x87 del EXE (0x59C790 rotacion + 0x411A00 suma), bit a bit en las tres salidas,
//   con NaN en una entrada (15 entradas x todos los patrones), en dos entradas (todas las parejas x todas las parejas de patrones) y aleatorio denso, y sin NaN (camino numerico).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <float.h>
#include "nan_rules_slice.inc"

typedef uint32_t u32;
typedef uint64_t u64;

static u64 rngState = 0x9E3779B97F4A7C15ull;
static u32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return u32(rngState >> 16); }

static bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
static bool IsSNanBits(u32 b) { return IsNanBits(b) && (b & 0x400000u) == 0; }

// ------------------------------------------------------------------------------------------------ primitivas del hardware
#define RR_OP(NAME, INSTR) \
    static __declspec(noinline) void NAME(const u32* x, const u32* y, u32* out, u32* fsw) { \
        __asm { mov esi, x } __asm { mov edi, y } __asm { mov ebx, out } __asm { mov edx, fsw } \
        __asm { fld dword ptr [esi] } __asm { fld dword ptr [edi] } __asm { fnclex } __asm { INSTR st(1), st } \
        __asm { fnstsw ax } __asm { movzx eax, ax } __asm { mov [edx], eax } __asm { fstp st(0) } __asm { fstp dword ptr [ebx] } }
#define RM_OP(NAME, INSTR) \
    static __declspec(noinline) void NAME(const u32* x, const u32* y, u32* out, u32* fsw) { \
        __asm { mov esi, x } __asm { mov edi, y } __asm { mov ebx, out } __asm { mov edx, fsw } \
        __asm { fld dword ptr [esi] } __asm { fnclex } __asm { INSTR dword ptr [edi] } \
        __asm { fnstsw ax } __asm { movzx eax, ax } __asm { mov [edx], eax } __asm { fstp dword ptr [ebx] } }
RR_OP(RrAdd, fadd) RR_OP(RrSub, fsub) RR_OP(RrMul, fmul)
RM_OP(RmAdd, fadd) RM_OP(RmSub, fsub) RM_OP(RmMul, fmul)
typedef void (*HwOp)(const u32*, const u32*, u32*, u32*);

static __declspec(noinline) void ChainHw(const u32* m, const u32* rel, const u32* pos, u32* out) {
    u32 t0 = 0, t1 = 0, t2 = 0;
    __asm {
        mov ecx, m
        mov eax, rel
        mov edx, pos
        fld dword ptr [ecx+20h]
        fmul dword ptr [eax+8]
        fld dword ptr [ecx+10h]
        fmul dword ptr [eax+4]
        faddp st(1), st
        fld dword ptr [ecx]
        fmul dword ptr [eax]
        faddp st(1), st
        fstp dword ptr [t0]
        fld dword ptr [ecx+24h]
        fmul dword ptr [eax+8]
        fld dword ptr [ecx+4]
        fmul dword ptr [eax]
        faddp st(1), st
        fld dword ptr [ecx+14h]
        fmul dword ptr [eax+4]
        faddp st(1), st
        fstp dword ptr [t1]
        fld dword ptr [ecx+28h]
        fmul dword ptr [eax+8]
        fld dword ptr [ecx+8]
        fmul dword ptr [eax]
        faddp st(1), st
        fld dword ptr [ecx+18h]
        fmul dword ptr [eax+4]
        faddp st(1), st
        fstp dword ptr [t2]
        mov ebx, out
        fld dword ptr [edx]
        fadd dword ptr [t0]
        fstp dword ptr [ebx]
        fld dword ptr [edx+4]
        fadd dword ptr [t1]
        fstp dword ptr [ebx+4]
        fld dword ptr [edx+8]
        fadd dword ptr [t2]
        fstp dword ptr [ebx+8]
    }
}

// ------------------------------------------------------------------------------------------------ patrones
static u32 nanPatterns[64];
static unsigned nNan;
static u32 otherPatterns[64];
static unsigned nOther;
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
    for (u32 sign = 0; sign < 2; ++sign)
        for (u32 m : mags) otherPatterns[nOther++] = (sign << 31) | m;
    for (unsigned i = 0; i < nNan; ++i) allPatterns[nAll++] = nanPatterns[i];
    for (unsigned i = 0; i < nOther; ++i) allPatterns[nAll++] = otherPatterns[i];
}

// ------------------------------------------------------------------------------------------------ parte 1
static u64 combos1, fail1, flagFail1;
static void CheckOp(HwOp op, bool rm, int kind, u32 x, u32 y) {
    u32 out = 0xDEADBEEFu, fsw = 0;
    op(&x, &y, &out, &fsw);
    const bool hwNan = IsNanBits(out);
    const auto dest = HeatFire73Raw(x, true);
    const auto src = HeatFire73Raw(y, !rm);
    const auto r = HeatFire73Op(hwNan, dest, src);
    ++combos1;
    bool ok = true;
    if (r.nan != hwNan) ok = false;
    else if (hwNan && r.bits != out) ok = false;
    if (!ok) {
        if (++fail1 <= 10) std::printf("DISCREPANCIA op=%d rm=%d x=%08x y=%08x hw=%08x emul=%08x(nan=%d)\n", kind, int(rm), x, y, out, r.bits, int(r.nan));
    }
    const bool generated = !IsNanBits(x) && !IsNanBits(y) && hwNan;
    const bool expectIe = (rm && IsSNanBits(y)) || generated;
    if (((fsw & 1u) != 0) != expectIe) {
        if (++flagFail1 <= 10) std::printf("BANDERA IE op=%d rm=%d x=%08x y=%08x fsw=%04x esperada=%d\n", kind, int(rm), x, y, fsw, int(expectIe));
    }
}
static void Part1() {
    struct { HwOp op; bool rm; int kind; } ops[] = {{RrAdd, false, 0}, {RrSub, false, 1}, {RrMul, false, 2}, {RmAdd, true, 0}, {RmSub, true, 1}, {RmMul, true, 2}};
    for (const auto& o : ops)
        for (unsigned i = 0; i < nAll; ++i)
            for (unsigned j = 0; j < nAll; ++j) CheckOp(o.op, o.rm, o.kind, allPatterns[i], allPatterns[j]);
    // aleatorio: patrones de 32 bits cualesquiera, la mitad forzados a NaN (carga y signo aleatorios)
    for (unsigned n = 0; n < 6000000u; ++n) {
        u32 x = Rnd() ^ (Rnd() << 16), y = Rnd() ^ (Rnd() << 16);
        if (Rnd() % 2) x = 0x7F800000u | (x & 0x807FFFFFu) | 1u;
        if (Rnd() % 2) y = 0x7F800000u | (y & 0x807FFFFFu) | 1u;
        if (Rnd() % 4 == 0) y = (x & 0xFFC00000u) | (y & 0x3FFFFFu) | 1u;   // misma carga alta, para forzar empates y casi empates
        if (Rnd() % 8 == 0) y = x ^ 0x80000000u;                            // solo difiere el signo
        const auto& o = ops[Rnd() % 6];
        CheckOp(o.op, o.rm, o.kind, x, y);
    }
}

// ------------------------------------------------------------------------------------------------ parte 2
static u64 combos2, fail2;
// in: 15 entradas = matriz right (0..2), forward (3..5), up (6..8), posicion relativa (9..11) y posicion del helicoptero (12..14)
static void CheckChain(const u32* in) {
    u32 m[16] = {};
    for (int i = 0; i < 3; ++i) { m[i] = in[i]; m[4 + i] = in[3 + i]; m[8 + i] = in[6 + i]; }
    u32 hw[3] = {1, 2, 3}, pr[3] = {4, 5, 6};
    ChainHw(m, in + 9, in + 12, hw);
    HeliSwat74WorldPosCpp(reinterpret_cast<const float*>(m), reinterpret_cast<const float*>(in + 9), reinterpret_cast<const float*>(in + 12), reinterpret_cast<float*>(pr));
    ++combos2;
    if (hw[0] != pr[0] || hw[1] != pr[1] || hw[2] != pr[2]) {
        if (++fail2 <= 10) {
            std::printf("DISCREPANCIA cadena hw=%08x %08x %08x prod=%08x %08x %08x\n  in:", hw[0], hw[1], hw[2], pr[0], pr[1], pr[2]);
            for (int i = 0; i < 15; ++i) std::printf(" %08x", in[i]);
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
    default: return (Rnd() & 0x80000000u) | ((Rnd() % 24 + 116u) << 23) | (Rnd() & 0x7FFFFFu);   // moderado
    }
}
static void Part2() {
    u32 in[15];
    for (unsigned p = 0; p < 15; ++p)
        for (unsigned q = 0; q < nNan; ++q)
            for (unsigned k = 0; k < 200; ++k) {
                for (int i = 0; i < 15; ++i) in[i] = Background();
                in[p] = nanPatterns[q];
                CheckChain(in);
            }
    for (unsigned p1 = 0; p1 < 15; ++p1)
        for (unsigned p2 = p1 + 1; p2 < 15; ++p2)
            for (unsigned q1 = 0; q1 < nNan; ++q1)
                for (unsigned q2 = 0; q2 < nNan; ++q2)
                    for (unsigned k = 0; k < 4; ++k) {
                        for (int i = 0; i < 15; ++i) in[i] = Background();
                        in[p1] = nanPatterns[q1]; in[p2] = nanPatterns[q2];
                        CheckChain(in);
                    }
    for (unsigned n = 0; n < 20000000u; ++n) {
        for (int i = 0; i < 15; ++i) in[i] = (Rnd() % 4 == 0) ? nanPatterns[Rnd() % nNan] : Background();
        CheckChain(in);
    }
    for (unsigned n = 0; n < 5000000u; ++n) {
        for (int i = 0; i < 15; ++i) in[i] = Background();
        CheckChain(in);
    }
}

int main() {
    BuildPatterns();
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    unsigned csr = 0;
    __asm { stmxcsr csr }
    csr |= 0x8000u;   // FTZ
    __asm { ldmxcsr csr }
    Part1();
    Part2();
    std::printf("parte1_combinaciones=%llu discrepancias=%llu discrepancias_IE=%llu\n", (unsigned long long)combos1, (unsigned long long)fail1, (unsigned long long)flagFail1);
    std::printf("parte2_combinaciones=%llu discrepancias=%llu\n", (unsigned long long)combos2, (unsigned long long)fail2);
    std::printf("patrones=%u (NaN %u, otros %u)\n", nAll, nNan, nOther);
    return (fail1 || flagFail1 || fail2) ? 1 : 0;
}
