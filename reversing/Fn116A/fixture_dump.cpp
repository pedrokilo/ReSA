// Vuelca las filas `input` del banco Fn116A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F116AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F116AFix::kCases; ++n) std::printf("%s\n", F116AFix::JsonC(F116AFix::MakeC(fn, n)).c_str());
    return 0;
}
