// Vuelca las filas `input` del banco Fn158A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F158AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F158AFix::kCases; ++n) std::printf("%s\n", F158AFix::JsonC(F158AFix::MakeC(fn, n)).c_str());
    return 0;
}
