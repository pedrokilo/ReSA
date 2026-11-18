// Vuelca las filas `input` del banco FnF1A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < FF1AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < FF1AFix::kCases; ++n) std::printf("%s\n", FF1AFix::JsonC(FF1AFix::MakeC(fn, n)).c_str());
    return 0;
}
