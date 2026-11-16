// Vuelca las filas `input` del banco Fn200A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F200AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F200AFix::kCases; ++n) std::printf("%s\n", F200AFix::JsonC(F200AFix::MakeC(fn, n)).c_str());
    return 0;
}
