// Vuelca las filas `input` del banco Fn123A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F123AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F123AFix::kCases; ++n) std::printf("%s\n", F123AFix::JsonC(F123AFix::MakeC(fn, n)).c_str());
    return 0;
}
