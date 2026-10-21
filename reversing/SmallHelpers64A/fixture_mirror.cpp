// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Ultima linea: constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace SH64Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    std::printf("S\t{\"atom\":%u,\"mi1\":%u,\"mi2\":%u,\"data\":%u,\"ps1\":%u,\"ps2\":%u,\"dev_a\":%u,\"dev_b\":%u,\"dev_c\":%u,\"g1\":%u,\"size\":%u,\"max_events\":%u,\"v_words\":%u,\"families\":%u}\n",
                oAtom, oMi1, oMi2, oData, oPs1, oPs2, oDevA, oDevB, oDevC, oG1, kArenaSize, kMaxEvents, kVWords, kFamilies);
    return 0;
}
