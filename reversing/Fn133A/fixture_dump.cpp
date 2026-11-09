// Vuelca las filas `input` del banco Fn133A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F133AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F133AFix::kCases; ++n) std::printf("%s\n", F133AFix::JsonC(F133AFix::MakeC(fn, n)).c_str());
    return 0;
}
