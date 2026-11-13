// Vuelca las filas `input` del banco Fn181A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F181AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F181AFix::kCases; ++n) std::printf("%s\n", F181AFix::JsonC(F181AFix::MakeC(fn, n)).c_str());
    return 0;
}
