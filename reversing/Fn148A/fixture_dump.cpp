// Vuelca las filas `input` del banco Fn148A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F148AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F148AFix::kCases; ++n) std::printf("%s\n", F148AFix::JsonC(F148AFix::MakeC(fn, n)).c_str());
    return 0;
}
