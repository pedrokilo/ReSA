// Vuelca las filas `input` del banco Fn167A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F167AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F167AFix::kCases; ++n) std::printf("%s\n", F167AFix::JsonC(F167AFix::MakeC(fn, n)).c_str());
    return 0;
}
