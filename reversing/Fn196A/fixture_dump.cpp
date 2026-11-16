// Vuelca las filas `input` del banco Fn196A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F196AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F196AFix::kCases; ++n) std::printf("%s\n", F196AFix::JsonC(F196AFix::MakeC(fn, n)).c_str());
    return 0;
}
