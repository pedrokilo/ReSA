// Autoprueba del generador y de los tamanos de arena (sin el juego): comprueba por static_assert que guarda + ventana + guarda == arena, genera los 6144 casos (sin excepciones, sin desbordar los
// 33 puntos de la arena de contacto) y imprime los recuentos de lo que las fixtures dicen buscar.
//   cl /nologo /EHsc /O2 /std:c++20 /Fe:dump_fixtures.exe dump_fixtures.cpp ; dump_fixtures.exe
#include <cstdio>
#include "Fixtures.hpp"
using namespace PedEntityCollision60Fix;
static_assert(kPedOff + kPedBytes + kPedOff == kPedArena && kEntOff + kEntBytes + kEntOff == kEntArena && kMatOff + kMatBytes + kMatOff == kMatArena && kCpOff + kCpBytes + kCpOff == kCpArena, "arenas");
static_assert(kMiOff + kMiBytes + kMiOff == kMiArena && kCmOff + kCmBytes + kCmOff == kCmArena && kCdOff + kCdBytes + kCdOff == kCdArena && kLnOff + kLnBytes + kLnOff == kLnArena, "arenas");
static_assert(kSpOff + kSpBytes + kSpOff == kSpArena && kTaskOff + kTaskBytes + kTaskOff == kTaskArena && kWiOff + kWiBytes + kWiOff == kWiArena && kTokOff + kTokBytes + kTokOff == kTokArena, "arenas");
int main() {
    unsigned touch0 = 0, touch1 = 0, ceil = 0, test = 0, uses = 0, lines = 0, types[8]{}, n1max = 0, n1zero = 0, entnull = 0, aim = 0, stuck = 0;
    for (unsigned n = 0; n < kCases; ++n) {
        const Case c = MakeCase(n);
        if (c.n1 > 32 || c.n2 > 32) { std::printf("n>32 en %u\n", n); return 1; }
        touch0 += c.w0 && U2F(c.touch0) < 1.0f; touch1 += c.w1 && U2F(c.touch1) < 1.0f; ceil += (c.ped478 >> 8) & 1; test += (c.ped474 >> 15) & 1; uses += c.pedF1C & 1;
        lines += !(c.pedPhys & 0x19000) && !c.attachedSel && (c.entType & 7) != 3; ++types[c.entType & 7]; n1max = c.n1 > n1max ? c.n1 : n1max; n1zero += c.n1 == 0; entnull += c.entMatNull; aim += c.aimSel == 2;
        stuck += (c.pedF1C >> 4) & 1;
        u8 cp[0x2C]; MakeCp(c, 0, 0, cp);
    }
    std::printf("cases=%u touch0<1=%u touch1<1=%u ceil=%u test=%u uses=%u lines=%u n1max=%u n1zero=%u entnull=%u aim2=%u stuck=%u types:", kCases, touch0, touch1, ceil, test, uses, lines, n1max, n1zero, entnull, aim, stuck);
    for (unsigned t : types) std::printf(" %u", t);
    std::printf("\nok\n");
    return 0;
}
