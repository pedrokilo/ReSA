// Vuelca las filas `input` del banco Fn94A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F94AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F94AFix::kCases; ++n) std::printf("%s\n", F94AFix::JsonC(F94AFix::MakeC(fn, n)).c_str());
    return 0;
}
