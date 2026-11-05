// Vuelca las filas `input` del banco Fn97B (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F97BFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F97BFix::kCases; ++n) std::printf("%s\n", F97BFix::JsonC(F97BFix::MakeC(fn, n)).c_str());
    return 0;
}
