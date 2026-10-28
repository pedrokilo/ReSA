// Microtest autonomo de 32 bits (herramienta de prueba, NO codigo de produccion; __asm permitido aqui): valida contra el HARDWARE x87 la funcion HeliIsFarFromPlayer de Heli.cpp (produccion TAL CUAL,
// recortada por dist_hw.py en heli_slice.inc), que sustituye a la secuencia x87 del EXE 0x6C7B5D..0x6C7B95 (distancia jugador - helicoptero > 170):
//   fld [jugador.x]; fsub [heli.x]; fld [jugador.y]; fsub [heli.y]; fld [jugador.z]; fsub [heli.z]; (suma z*z + y*y, luego + x*x); fsqrt; fabs; fcomp [170.0]; fnstsw ax; test ah,41h; jne (no lejos)
// Estado de coma flotante como el juego: palabra de control x87 a 24 bits (redondeo al mas cercano, excepciones enmascaradas); se repite con y sin FTZ/DAZ en MXCSR.
// Combina TODOS los patrones de un conjunto (NaN silenciosos y de senal, ceros, desnormales, normales, grandes que desbordan el cuadrado en float, infinitos) en las 6 entradas, y millones de combinaciones
// aleatorias cerca de la frontera 170. Solo se compara el resultado booleano (es lo unico que consume la funcion).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <float.h>
#include <intrin.h>
struct CVector { float x, y, z; };
#include "heli_slice.inc"

typedef uint32_t u32;
typedef uint64_t u64;
static u64 rngState = 0x9E3779B97F4A7C15ull;
static u32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return u32(rngState >> 16); }
static const float k170 = 170.0f;
static float F(u32 b) { float f; memcpy(&f, &b, 4); return f; }

static __declspec(noinline) bool HwFar(const float* player, const float* heli) {
    int far_ = 0;
    __asm {
        mov ecx, player
        mov edx, heli
        fld dword ptr [ecx]
        fsub dword ptr [edx]
        fld dword ptr [ecx + 4]
        fsub dword ptr [edx + 4]
        fld dword ptr [ecx + 8]
        fsub dword ptr [edx + 8]
        fld st(0)
        fmul st(0), st(1)
        fld st(2)
        fmul st(0), st(3)
        faddp st(1), st(0)
        fld st(3)
        fmul st(0), st(4)
        faddp st(1), st(0)
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fabs
        fcomp dword ptr [k170]
        fnstsw ax
        test ah, 41h
        jne notfar
        mov far_, 1
    notfar:
    }
    return far_ != 0;
}

static const u32 kPat[] = {0x00000000u, 0x80000000u, 0x00000001u, 0x807FFFFFu, 0x1E3CE508u, 0x3F800000u, 0x42C80000u, 0x432A0000u, 0xC32A0000u, 0x60AD78ECu, 0x7F7FFFFFu,
    0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0xFFC12345u};

int main() {
    u64 combos = 0, mismatch = 0;
    for (int pass = 0; pass < 2; ++pass) {
        _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);
        _controlfp(pass ? _DN_FLUSH : _DN_SAVE, _MCW_DN);
        const size_t n = sizeof(kPat) / sizeof(kPat[0]);
        // todas las combinaciones de seis patrones sobre una rejilla reducida (el jugador y el helicoptero usan el mismo conjunto en cada componente)
        for (size_t a = 0; a < n; ++a) for (size_t b = 0; b < n; ++b) for (size_t c = 0; c < n; ++c) for (size_t d = 0; d < n; ++d) for (size_t e = 0; e < n; ++e) for (size_t f = 0; f < n; ++f) {
            const float p[3] = {F(kPat[a]), F(kPat[b]), F(kPat[c])}, h[3] = {F(kPat[d]), F(kPat[e]), F(kPat[f])};
            const CVector pc{p[0], p[1], p[2]}, hc{h[0], h[1], h[2]};
            ++combos;
            if (HwFar(p, h) != HeliIsFarFromPlayer(pc, hc)) { if (mismatch++ < 10) printf("DIFIERE %08x %08x %08x %08x %08x %08x\n", kPat[a], kPat[b], kPat[c], kPat[d], kPat[e], kPat[f]); }
        }
        // aleatorias: desplazamiento de longitud 170 +- unos ulp, con posiciones de distintas magnitudes
        for (int i = 0; i < 1500000; ++i) {
            float v[3] = {F((Rnd() & 0x807FFFFFu) | (u32(120 + Rnd() % 16) << 23)), F((Rnd() & 0x807FFFFFu) | (u32(120 + Rnd() % 16) << 23)), F((Rnd() & 0x807FFFFFu) | (u32(120 + Rnd() % 16) << 23))};
            const double len = std::sqrt(double(v[0]) * v[0] + double(v[1]) * v[1] + double(v[2]) * v[2]);
            const double k = len == 0.0 ? 0.0 : 170.0 / len;
            const float base[3] = {F(Rnd()) , F(Rnd()), F(Rnd())};
            float h[3], p[3];
            const int mode = Rnd() % 4;
            for (int j = 0; j < 3; ++j) {
                const float off = float(v[j] * k);
                const float origin = mode == 0 ? 0.0f : mode == 1 ? float(int(Rnd() % 4001) - 2000) : mode == 2 ? float(int(Rnd() % 41) - 20) : float(int(Rnd() % 200001) - 100000) * 0.01f;
                h[j] = origin;
                p[j] = origin + off;
                if (Rnd() % 3 == 0) { u32 b; memcpy(&b, &p[j], 4); b += int(Rnd() % 7) - 3; memcpy(&p[j], &b, 4); }
            }
            (void)base;
            const CVector pc{p[0], p[1], p[2]}, hc{h[0], h[1], h[2]};
            ++combos;
            if (HwFar(p, h) != HeliIsFarFromPlayer(pc, hc)) { if (mismatch++ < 10) printf("DIFIERE aleatorio %g %g %g / %g %g %g\n", p[0], p[1], p[2], h[0], h[1], h[2]); }
        }
    }
    printf("combinaciones=%llu discrepancias=%llu\n", (unsigned long long)combos, (unsigned long long)mismatch);
    return mismatch ? 1 : 0;
}
