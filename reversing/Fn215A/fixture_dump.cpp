// Vuelca las filas `input` del banco Fn215A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F215AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F215AFix::kCases; ++n) std::printf("%s\n", F215AFix::JsonC(F215AFix::MakeC(fn, n)).c_str());
    return 0;
}
