// Vuelca las filas `input` del banco Fn95A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F95Fix::kFunctions; ++fn)
        for (unsigned n = 0; n < F95Fix::kCases; ++n) std::printf("%s\n", F95Fix::JsonC(F95Fix::MakeC(fn, n)).c_str());
    return 0;
}
