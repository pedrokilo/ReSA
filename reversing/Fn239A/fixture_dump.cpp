// Vuelca las filas `input` del banco Fn239A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F239AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F239AFix::kCases; ++n) std::printf("%s\n", F239AFix::JsonC(F239AFix::MakeC(fn, n)).c_str());
    return 0;
}
