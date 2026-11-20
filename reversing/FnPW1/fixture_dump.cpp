// Vuelca las filas `input` del banco FnPW1 (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned n = 0; n < PW1Fix::kCases; ++n) std::printf("%s\n", PW1Fix::JsonC(PW1Fix::MakeC(n)).c_str());
    return 0;
}
