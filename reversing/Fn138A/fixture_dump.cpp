// Vuelca las filas `input` del banco Fn138A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F138AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F138AFix::kCases; ++n) std::printf("%s\n", F138AFix::JsonC(F138AFix::MakeC(fn, n)).c_str());
    return 0;
}
