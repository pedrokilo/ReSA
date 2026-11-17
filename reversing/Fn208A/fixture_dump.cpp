// Vuelca las filas `input` del banco Fn208A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F208AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F208AFix::kCases; ++n) std::printf("%s\n", F208AFix::JsonC(F208AFix::MakeC(fn, n)).c_str());
    return 0;
}
