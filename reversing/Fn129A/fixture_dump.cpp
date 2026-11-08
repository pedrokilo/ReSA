// Vuelca las filas `input` del banco Fn129A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F129AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F129AFix::kCases; ++n) std::printf("%s\n", F129AFix::JsonC(F129AFix::MakeC(fn, n)).c_str());
    return 0;
}
