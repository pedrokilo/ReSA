// Vuelca las filas `input` del banco Fn142A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F142AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F142AFix::kCases; ++n) std::printf("%s\n", F142AFix::JsonC(F142AFix::MakeC(fn, n)).c_str());
    return 0;
}
