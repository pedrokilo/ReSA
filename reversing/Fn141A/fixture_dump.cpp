// Vuelca las filas `input` del banco Fn141A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F141AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F141AFix::kCases; ++n) std::printf("%s\n", F141AFix::JsonC(F141AFix::MakeC(fn, n)).c_str());
    return 0;
}
