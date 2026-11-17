// Vuelca las filas `input` del banco Fn216A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F216AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F216AFix::kCases; ++n) std::printf("%s\n", F216AFix::JsonC(F216AFix::MakeC(fn, n)).c_str());
    return 0;
}
