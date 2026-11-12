// Vuelca las filas `input` del banco Fn173A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F173AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F173AFix::kCases; ++n) std::printf("%s\n", F173AFix::JsonC(F173AFix::MakeC(fn, n)).c_str());
    return 0;
}
