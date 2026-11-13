// Vuelca las filas `input` del banco Fn176A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F176AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F176AFix::kCases; ++n) std::printf("%s\n", F176AFix::JsonC(F176AFix::MakeC(fn, n)).c_str());
    return 0;
}
