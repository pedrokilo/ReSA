// Vuelca las filas `input` del banco Fn156A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F156AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F156AFix::kCases; ++n) std::printf("%s\n", F156AFix::JsonC(F156AFix::MakeC(fn, n)).c_str());
    return 0;
}
