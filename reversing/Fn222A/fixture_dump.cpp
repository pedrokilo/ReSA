// Vuelca las filas `input` del banco Fn222A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F222AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F222AFix::kCases; ++n) std::printf("%s\n", F222AFix::JsonC(F222AFix::MakeC(fn, n)).c_str());
    return 0;
}
