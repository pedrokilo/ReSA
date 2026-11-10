// Vuelca las filas `input` del banco Fn607A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F607AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F607AFix::kCases; ++n) std::printf("%s\n", F607AFix::JsonC(F607AFix::MakeC(fn, n)).c_str());
    return 0;
}
