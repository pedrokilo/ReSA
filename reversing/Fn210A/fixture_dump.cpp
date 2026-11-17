// Vuelca las filas `input` del banco Fn210A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F210AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F210AFix::kCases; ++n) std::printf("%s\n", F210AFix::JsonC(F210AFix::MakeC(fn, n)).c_str());
    return 0;
}
