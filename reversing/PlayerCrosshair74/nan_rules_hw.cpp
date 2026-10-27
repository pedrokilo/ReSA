// Microtest de hardware del lote 74: compara las funciones Cross74*Cpp de produccion (X87Math.cpp, bloques «Lote 73» y «Lote 74» tal cual) con una transcripcion en ensamblador x87 de las
// secuencias del EXE (0x56ECA7.., 0x56ED85.., 0x56F0A0.., 0x56F0E0..) sobre millones de entradas con NaN (de senal y silenciosos, cargas distintas), infinitos, ceros con signo, desnormales y normales.
// Exige igualdad de bits (incluida la carga de los NaN) y la pila x87 equilibrada. Palabra de control del juego: 24 bits, redondeo al mas cercano, excepciones enmascaradas.
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <bit>
#include <float.h>
#include "nan_rules_slice.inc"

static uint32_t rngState = 0x12345678u;
static uint32_t Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 17; rngState ^= rngState << 5; return rngState; }

static const uint32_t kSpecial[] = {
    0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x00800000u, 0x80800000u,
    0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0x3F666666u, 0xBF666666u, 0x3F7FFFFFu, 0x42B40000u, 0x7F7FFFFFu, 0xFF7FFFFFu,
    0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu, 0x7FBFFFFFu};
static uint32_t PickF(bool allowHuge = true) {
    const uint32_t r = Rnd() % 16;
    uint32_t b;
    if (r < 7) b = kSpecial[Rnd() % (sizeof(kSpecial) / 4)];
    else if (r < 9) b = (Rnd() & 0x807FFFFFu) | (uint32_t(Rnd() % 60 + 70) << 23);
    else if (r < 11) b = (Rnd() & 0x807FFFFFu) | (uint32_t(Rnd() % 200 + 10) << 23);
    else if (r < 13) b = (Rnd() & 0x807FFFFFu) | (uint32_t(Rnd() % 20 + 118) << 23);
    else b = Rnd();
    if (!allowHuge && ((b >> 23) & 0xFF) > 0x80 && ((b >> 23) & 0xFF) != 0xFF) b = (b & 0x807FFFFFu) | (0x70u << 23);   // evita |x| >= 2^63 para fptan
    return b;
}
static int PickSteer() {
    const uint32_t r = Rnd() % 6;
    if (r == 0) return 0;
    if (r == 1) return -32768;
    if (r == 2) return 32767;
    if (r == 3) return int(Rnd() % 7) - 3;
    return int16_t(Rnd());
}
static unsigned fails;
static unsigned CheckTop() {
    unsigned short sw;
    __asm { fnstsw sw }
    return (sw >> 11) & 7;
}
static void Report(const char* what, unsigned long long i, uint32_t got, uint32_t want) {
    if (fails < 25) printf("DIFIERE %s combinacion %llu: obtenido %08x esperado %08x\n", what, i, got, want);
    ++fails;
}

static void RefStep(float* v, int steer, uint32_t ts, bool subtract) {
    const float k = std::bit_cast<float>(0x39AEC33Eu);
    const int subFlag = subtract ? 1 : 0;
    __asm {
        mov eax, v
        fild steer
        fmul dword ptr ts
        fmul dword ptr k
        cmp subFlag, 0
        jne subr
        fadd dword ptr [eax]
        jmp done
    subr:
        fsubr dword ptr [eax]
    done:
        fstp dword ptr [eax]
    }
}

static void RefAim(const uint32_t* in, uint32_t* muzzle, uint32_t* target) {
    uint32_t fov1 = in[0], fov2 = in[1], aspect = in[2], crossX = in[3], crossY = in[4];
    const uint32_t* right = in + 5;
    const uint32_t* forward = in + 8;
    const uint32_t* up = in + 11;
    const uint32_t* pos = in + 14;
    const float cc = std::bit_cast<float>(0x3C0EFA35u), k200 = 200.0f;
    float s0, s1, s2, rx, ry, p0, p1, f1, f2, g0, g1, g2, sc2, t0, t1, t2;
    __asm {
        mov esi, up
        mov edi, right
        mov ebx, forward
        mov edx, pos
        fld dword ptr fov1
        fmul dword ptr cc
        fptan
        fstp st(0)
        fdiv dword ptr aspect
        fmul dword ptr crossY
        fld dword ptr [esi]
        fmul st, st(1)
        fstp dword ptr s0
        fld dword ptr [esi+4]
        fmul st, st(1)
        fstp dword ptr s1
        fld dword ptr [esi+8]
        fmul st, st(1)
        fstp dword ptr s2
        fstp st(0)
        fld dword ptr fov2
        fmul dword ptr cc
        fld dword ptr crossX
        fld dword ptr [edi]
        fmul st, st(1)
        fstp dword ptr rx
        fld dword ptr [edi+4]
        fmul st, st(1)
        fstp dword ptr ry
        fmul dword ptr [edi+8]
        fxch st(1)
        fptan
        fstp st(0)
        fld dword ptr rx
        fmul st, st(1)
        fstp dword ptr p0
        fld dword ptr ry
        fmul st, st(1)
        fstp dword ptr p1
        fxch st(1)
        fmul st, st(1)
        fxch st(1)
        fstp st(0)
        fld dword ptr [ebx]
        fsub dword ptr p0
        fld dword ptr [ebx+4]
        fsub dword ptr p1
        fstp dword ptr f1
        fld dword ptr [ebx+8]
        fsub st, st(2)
        fstp dword ptr f2
        fsub dword ptr s0
        fstp dword ptr g0
        fstp st(0)
        fld dword ptr f1
        fsub dword ptr s1
        fstp dword ptr g1
        fld dword ptr f2
        fsub dword ptr s2
        fstp dword ptr g2
        fld dword ptr g0
        fmul dword ptr k200
        fld dword ptr g1
        fmul dword ptr k200
        fld dword ptr g2
        fmul dword ptr k200
        fstp dword ptr sc2
        fxch st(1)
        fadd dword ptr [edx]
        fstp dword ptr t0
        fld dword ptr [edx+4]
        fadd st, st(1)
        fstp dword ptr t1
        fstp st(0)
        fld dword ptr [edx+8]
        fadd dword ptr sc2
        fstp dword ptr t2
    }
    memcpy(&muzzle[0], &s0, 4); memcpy(&muzzle[1], &s1, 4); memcpy(&muzzle[2], &s2, 4);
    memcpy(&target[0], &t0, 4); memcpy(&target[1], &t1, 4); memcpy(&target[2], &t2, 4);
}

static float RefPhase(uint32_t time) {
    int t = int(time & 0x3FF);
    const float k1 = std::bit_cast<float>(0x3BC90FDBu), k2 = std::bit_cast<float>(0x3E4CCCCDu), one = 1.0f;
    float outv;
    __asm {
        fild t
        fmul dword ptr k1
        fsin
        fmul dword ptr k2
        fadd dword ptr one
        fstp dword ptr outv
    }
    return outv;
}

static void RefTrail(uint32_t xBits, uint32_t yBits, int width, int height, float phase, float radius, int corner, uint32_t* out) {
    const float k1 = std::bit_cast<float>(0x3FC90FDBu), k2 = std::bit_cast<float>(0x3F490FDBu), one = 1.0f, half = 0.5f;
    float ox, oy;
    __asm {
        fild corner
        fmul dword ptr k1
        fadd dword ptr k2
        fld st(0)
        fcos
        fmul dword ptr radius
        fmul dword ptr phase
        fld dword ptr yBits
        fadd dword ptr one
        fimul dword ptr height
        fmul dword ptr half
        faddp st(1), st
        fstp dword ptr oy
        fsin
        fmul dword ptr radius
        fmul dword ptr phase
        fld dword ptr xBits
        fadd dword ptr one
        fimul dword ptr width
        fmul dword ptr half
        faddp st(1), st
        fstp dword ptr ox
    }
    memcpy(&out[0], &ox, 4);
    memcpy(&out[1], &oy, 4);
}

int main() {
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    unsigned long long total1 = 0, total2 = 0, total3 = 0, total4 = 0;
    const unsigned topBefore = CheckTop();
    for (unsigned long long i = 0; i < 600000; ++i) {   // 1. paso de la mira
        uint32_t cur = PickF(), ts = PickF();
        const int steer = PickSteer();
        const bool sub = Rnd() & 1;
        float a = std::bit_cast<float>(cur), b = a;
        Cross74StepCpp(&a, steer, ts, sub);
        RefStep(&b, steer, ts, sub);
        uint32_t ga, gb;
        memcpy(&ga, &a, 4); memcpy(&gb, &b, 4);
        if (ga != gb) Report("Step", i, ga, gb);
        if (CheckTop() != topBefore) { printf("pila x87 desequilibrada en Step\n"); ++fails; }
        ++total1;
    }
    for (unsigned long long i = 0; i < 600000; ++i) {   // 2. direccion de la mira
        uint32_t in[17];
        in[0] = PickF(false); in[1] = Rnd() % 3 == 0 ? in[0] : PickF(false); in[2] = PickF(); in[3] = PickF(); in[4] = PickF();
        for (int k = 5; k < 17; ++k) in[k] = PickF();
        uint32_t m1[3], t1[3], m2[3], t2[3];
        Cross74AimCpp(in, m1, t1);
        RefAim(in, m2, t2);
        for (int k = 0; k < 3; ++k) {
            if (m1[k] != m2[k]) Report("Aim.muzzle", i * 3 + k, m1[k], m2[k]);
            if (t1[k] != t2[k]) Report("Aim.target", i * 3 + k, t1[k], t2[k]);
        }
        if (CheckTop() != topBefore) { printf("pila x87 desequilibrada en Aim\n"); ++fails; }
        ++total2;
    }
    for (unsigned t = 0; t < 4096; ++t) {   // 3. fase de la estela
        const float a = Cross74PhaseCpp(t * 2654435761u + t), b = RefPhase(t * 2654435761u + t);
        uint32_t ga, gb;
        memcpy(&ga, &a, 4); memcpy(&gb, &b, 4);
        if (ga != gb) Report("Phase", t, ga, gb);
        ++total3;
    }
    for (unsigned long long i = 0; i < 600000; ++i) {   // 4. punto de la estela
        const uint32_t x = PickF(), y = PickF();
        const int width = Rnd() % 4 == 0 ? int(Rnd()) : int(Rnd() % 4000), height = Rnd() % 4 == 0 ? int(Rnd()) : int(Rnd() % 3000);
        const float phase = std::bit_cast<float>(0x3F800000u + (Rnd() & 0x7FFFFu) - 0x40000u);
        const float radius = float(20 + 10 * (Rnd() % 3));
        const int corner = int(Rnd() % 4);
        uint32_t o1[2], o2[2];
        Cross74TrailPointCpp(x, y, width, height, phase, radius, corner, o1);
        RefTrail(x, y, width, height, phase, radius, corner, o2);
        for (int k = 0; k < 2; ++k)
            if (o1[k] != o2[k]) Report("Trail", i * 2 + k, o1[k], o2[k]);
        if (CheckTop() != topBefore) { printf("pila x87 desequilibrada en Trail\n"); ++fails; }
        ++total4;
    }
    printf("paso=%llu direccion=%llu fase=%llu estela=%llu discrepancias=%u\n", total1, total2, total3, total4, fails);
    return fails ? 1 : 0;
}
