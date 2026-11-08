// Vuelca las filas `input` del banco Fn118A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F118AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F118AFix::kCases; ++n) std::printf("%s\n", F118AFix::JsonC(F118AFix::MakeC(fn, n)).c_str());
    return 0;
}
