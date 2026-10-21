// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input` (usando Fixtures.hpp, solo C++ estandar)
// y, con argumentos `fn n`, vuelca las palabras clave de la imagen de ese caso (depuracion).
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace A63Fix;
int main(int argc, char** argv) {
    if (argc == 3) {
        const Spec s = MakeSpec(u32(std::atoi(argv[1])), u32(std::atoi(argv[2])));
        std::printf("fn %u n %u fam %u tgt %u hasMat %u %u %u arg2 %u time %u\n", s.fn, s.n, s.family, s.tgtKind, s.hasMat[0], s.hasMat[1], s.hasMat[2], s.arg2Kind, s.timeMs);
        auto dump = [&](const char* name, const u32* img, u32 from, u32 to) { std::printf("%s:", name); for (u32 o = from; o < to; o += 4) std::printf(" [%X]=%08X", o, img[o / 4]); std::printf("\n"); };
        dump("A pos/speed", s.a, 0x04, 0x50);
        dump("A dest", s.a, 0x3EC, 0x3F8); dump("A plane", s.a, 0x7D4, 0x7E4); dump("A ctl", s.a, 0x988, 0x9D0); dump("A flags", s.a, 0x3B8, 0x3C0); dump("A rseed", s.a, 0x20, 0x24); dump("A 42c", s.a, 0x428, 0x430);
        dump("A flag", s.a, 0x424, 0x428);
        dump("T pos/speed", s.t, 0x04, 0x50);
        dump("P pos/speed", s.p, 0x04, 0x50);
        for (int m = 0; m < 3; ++m) { std::printf("mat%d:", m); for (u32 i = 0; i < 16; ++i) std::printf(" %08X", s.mat[m][i]); std::printf("\n"); }
        std::printf("speedV: %08X %08X %08X\n", s.speedV[0], s.speedV[1], s.speedV[2]);
        std::printf("atan:"); for (int i = 0; i < 8; ++i) std::printf(" %08X", s.atan[i]); std::printf("\n");
        return 0;
    }
    for (u32 fn = 0; fn < kFnCount; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    return 0;
}
