// Vuelca las filas `input` del banco Fn168A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F168AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F168AFix::kCases; ++n) std::printf("%s\n", F168AFix::JsonC(F168AFix::MakeC(fn, n)).c_str());
    return 0;
}
