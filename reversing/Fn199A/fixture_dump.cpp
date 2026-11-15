// Vuelca las filas `input` del banco Fn199A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F199AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F199AFix::kCases; ++n) std::printf("%s\n", F199AFix::JsonC(F199AFix::MakeC(fn, n)).c_str());
    return 0;
}
