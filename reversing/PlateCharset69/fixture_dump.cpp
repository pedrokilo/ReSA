// Vuelca las filas `input` de los 16384 casos de Fixtures.hpp (el mismo archivo que usa el banco) y las constantes de arena, en JSON por lineas.
// Lo compila fixture_selfcheck.py con cl (solo C++ estandar).
#include <cstdio>
#include "Fixtures.hpp"

int main() {
    using namespace Pc69Fix;
    std::printf("{\"layout\":{\"arena\":%u,\"out_words\":%u,\"max_lines\":%u,\"line_buf\":%u}}\n", kArena, kOutWords, kMaxLines, kLineBuf);
    for (unsigned n = 0; n < kCases; ++n)
        std::printf("%s\n", JsonOf(Make(n)).c_str());
    return 0;
}
