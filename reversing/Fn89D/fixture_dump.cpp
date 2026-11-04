// Vuelca las filas `input` del banco Fn89D (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned n = 0; n < F89DFix::kCases; ++n) std::printf("%s\n", F89DFix::JsonC(F89DFix::MakeC(0, n)).c_str());
    return 0;
}
