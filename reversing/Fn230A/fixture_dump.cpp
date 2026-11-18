// Vuelca las filas `input` del banco Fn230A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F230AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F230AFix::kCases; ++n) std::printf("%s\n", F230AFix::JsonC(F230AFix::MakeC(fn, n)).c_str());
    return 0;
}
