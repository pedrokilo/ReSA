// Vuelca las filas `input` del banco Fn95C (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F95CFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F95CFix::kCases; ++n) std::printf("%s\n", F95CFix::JsonC(F95CFix::MakeC(fn, n)).c_str());
    return 0;
}
