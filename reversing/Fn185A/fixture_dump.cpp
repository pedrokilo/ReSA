// Vuelca las filas `input` del banco Fn185A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F185AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F185AFix::kCases; ++n) std::printf("%s\n", F185AFix::JsonC(F185AFix::MakeC(fn, n)).c_str());
    return 0;
}
