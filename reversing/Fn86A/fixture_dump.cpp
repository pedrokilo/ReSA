// Vuelca las filas `input` del banco Fn86A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned n = 0; n < F86AFix::kCases; ++n) std::printf("%s\n", F86AFix::JsonC(F86AFix::MakeC(0, n)).c_str());
    return 0;
}
