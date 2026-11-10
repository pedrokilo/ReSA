// Vuelca las filas `input` del banco Fn144A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F144AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F144AFix::kCases; ++n) std::printf("%s\n", F144AFix::JsonC(F144AFix::MakeC(fn, n)).c_str());
    return 0;
}
