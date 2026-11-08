// Vuelca las filas `input` del banco Fn130A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F130AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F130AFix::kCases; ++n) std::printf("%s\n", F130AFix::JsonC(F130AFix::MakeC(fn, n)).c_str());
    return 0;
}
