// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Las ultimas lineas: muestras del atan2 del falso (A) y constantes de disposicion (S).
#include <cstdio>
#include <cmath>
#include "Fixtures.hpp"
using namespace WS64Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    for (u32 i = 0; i < 40; ++i) {
        const float x = float(int(Rand(i, 1) % 4001) - 2000) / 64.0f, y = float(int(Rand(i, 2) % 4001) - 2000) / 64.0f;
        const float r = static_cast<float>(std::atan2(static_cast<double>(y), static_cast<double>(x)));
        std::printf("A\t[%u,%u,%u]\n", FBits(x), FBits(y), FBits(r));
    }
    std::printf("S\t{\"region_base\":%u,\"region_words\":%u,\"y_word\":%u,\"x_word\":%u,\"count_word\":%u,\"max_events\":%u,\"point_slots\":%u}\n",
                kRegionBase, kRegionWords, kYWord, kXWord, kCountWord, kMaxEvents, kPointSlots);
    return 0;
}
