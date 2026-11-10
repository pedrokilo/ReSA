// Vuelca las filas `input` del banco Fn145A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F145AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F145AFix::kCases; ++n) std::printf("%s\n", F145AFix::JsonC(F145AFix::MakeC(fn, n)).c_str());
    return 0;
}
