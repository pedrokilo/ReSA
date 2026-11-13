// Vuelca las filas `input` del banco Fn184A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F184AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F184AFix::kCases; ++n) std::printf("%s\n", F184AFix::JsonC(F184AFix::MakeC(fn, n)).c_str());
    return 0;
}
