// Vuelca las filas `input` del banco Fn219A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F219AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F219AFix::kCases; ++n) std::printf("%s\n", F219AFix::JsonC(F219AFix::MakeC(fn, n)).c_str());
    return 0;
}
