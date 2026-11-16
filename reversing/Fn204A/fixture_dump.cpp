// Vuelca las filas `input` del banco Fn204A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F204AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F204AFix::kCases; ++n) std::printf("%s\n", F204AFix::JsonC(F204AFix::MakeC(fn, n)).c_str());
    return 0;
}
