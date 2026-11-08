// Vuelca las filas `input` del banco Fn124A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F124AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F124AFix::kCases; ++n) std::printf("%s\n", F124AFix::JsonC(F124AFix::MakeC(fn, n)).c_str());
    return 0;
}
