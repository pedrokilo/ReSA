// Programa autonomo (sin el juego): ejecuta los ayudantes x87 PedCtl* de X87Math.cpp (y FightCtlElapsedCpp) con el FCW de 24 bits sobre las entradas que le pasa selftest_helpers.py
// (una linea por caso: «nombre arg arg ...» con todo en hexadecimal) e imprime el resultado de cada uno en una linea. Se compila con las opciones de X87Math.cpp (/O2 /Oi /arch:IA32 /fp:fast).
#include <cfloat>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include "X87Math.h"

static float F(uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }
static uint32_t B(float f) { uint32_t b; std::memcpy(&b, &f, 4); return b; }

int main() {
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    char name[64];
    unsigned a[10];
    char line[512];
    while (std::fgets(line, sizeof line, stdin)) {
        std::memset(a, 0, sizeof a);
        int n = std::sscanf(line, "%63s %x %x %x %x %x %x %x %x %x %x", name, &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7], &a[8], &a[9]);
        if (n < 1) continue;
        const std::string fn = name;
        if (fn == "sqmag") {
            float v[3]{F(a[0]), F(a[1]), F(a[2])};
            std::printf("%u\n", PedCtlSqMagBelowCpp(v, F(a[3])) ? 1u : 0u);
        } else if (fn == "decay") {
            float v[3]{F(a[0]), F(a[1]), F(a[2])};
            PedCtlDecaySpeedCpp(v, F(a[3]));
            std::printf("%08x %08x %08x\n", B(v[0]), B(v[1]), B(v[2]));
        } else if (fn == "raised") {
            std::printf("%u\n", PedCtlRaisedBelowCpp(F(a[0]), F(a[1])) ? 1u : 0u);
        } else if (fn == "near") {
            float p[3]{F(a[0]), F(a[1]), F(a[2])};
            float c[3]{F(a[3]), F(a[4]), F(a[5])};
            std::printf("%u\n", PedCtlBloodNearCpp(p, c) ? 1u : 0u);
        } else if (fn == "spawn") {
            float pos[3]{F(a[3]), F(a[4]), F(a[5])};
            float size = 0.f, out[3]{};
            PedCtlBloodSpawnCpp(int(a[0]), int(a[1]), int(a[2]), pos, &size, out);
            std::printf("%08x %08x %08x %08x\n", B(size), B(out[0]), B(out[1]), B(out[2]));
        } else if (fn == "slow") {
            float v[3]{F(a[0]), F(a[1]), F(a[2])};
            std::printf("%u\n", PedCtlStepsSlowCpp(v, F(a[3])) ? 1u : 0u);
        } else if (fn == "steps") {
            float v[3]{F(a[0]), F(a[1]), F(a[2])};
            std::printf("%08x\n", PedCtlStepsCpp(v, F(a[3]), int(a[4])));
        } else if (fn == "elapsed") {
            std::printf("%08x\n", FightCtlElapsedCpp(F(a[0])));
        } else {
            std::printf("?\n");
        }
    }
    return 0;
}
