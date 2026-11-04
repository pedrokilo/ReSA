// Vuelca las filas `input` del banco Fn89A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned n = 0; n < F89AFix::kCases; ++n) std::printf("%s\n", F89AFix::JsonC(F89AFix::MakeC(0, n)).c_str());
    return 0;
}
