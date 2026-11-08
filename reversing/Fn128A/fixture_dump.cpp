// Vuelca las filas `input` del banco Fn128A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F128AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F128AFix::kCases; ++n) std::printf("%s\n", F128AFix::JsonC(F128AFix::MakeC(fn, n)).c_str());
    return 0;
}
