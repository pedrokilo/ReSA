// Vuelca las filas `input` del banco Fn89C (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned n = 0; n < F89CFix::kCases; ++n) std::printf("%s\n", F89CFix::JsonC(F89CFix::MakeC(0, n)).c_str());
    return 0;
}
