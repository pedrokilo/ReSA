// Vuelca las filas `input` del banco Fn97A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F97AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F97AFix::kCases; ++n) std::printf("%s\n", F97AFix::JsonC(F97AFix::MakeC(fn, n)).c_str());
    return 0;
}
