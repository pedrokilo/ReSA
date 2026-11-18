// Vuelca las filas `input` del banco Fn226A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F226AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F226AFix::kCases; ++n) std::printf("%s\n", F226AFix::JsonC(F226AFix::MakeC(fn, n)).c_str());
    return 0;
}
