// Vuelca las filas `input` del banco Fn111A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F111AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F111AFix::kCases; ++n) std::printf("%s\n", F111AFix::JsonC(F111AFix::MakeC(fn, n)).c_str());
    return 0;
}
