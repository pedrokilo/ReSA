// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena para contrastarlas con el oraculo.
#include <cstdio>
#include "Fixtures.hpp"
using namespace PartnerDealFirst31Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::printf("{\"family\":%u,\"ped_kind\":%u,\"ped\":%u,\"ret_kind\":%u,\"ret\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n",
                    c.family, c.pedKind, c.ped, c.retKind, c.ret, c.eax, c.edx, c.stack);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u}}\n", kThisBytes, kThisOff, kThisArena);
    return 0;
}
