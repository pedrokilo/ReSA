// Vuelca las filas `input` del banco Fn607B (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F607BFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F607BFix::kCases; ++n) std::printf("%s\n", F607BFix::JsonC(F607BFix::MakeC(fn, n)).c_str());
    return 0;
}
