// Vuelca las filas `input` del banco Fn232A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F232AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F232AFix::kCases; ++n) std::printf("%s\n", F232AFix::JsonC(F232AFix::MakeC(fn, n)).c_str());
    return 0;
}
