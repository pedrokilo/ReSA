// Vuelca las filas `input` del banco FnS7A1A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < FS7Fix::kFunctions; ++fn)
        for (unsigned n = 0; n < FS7Fix::kCases; ++n) std::printf("%s\n", FS7Fix::JsonC(FS7Fix::MakeC(fn, n)).c_str());
    return 0;
}
