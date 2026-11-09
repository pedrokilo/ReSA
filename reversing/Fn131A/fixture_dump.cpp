// Vuelca las filas `input` del banco Fn131A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F131AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F131AFix::kCases; ++n) std::printf("%s\n", F131AFix::JsonC(F131AFix::MakeC(fn, n)).c_str());
    return 0;
}
