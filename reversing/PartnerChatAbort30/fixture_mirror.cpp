// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py.
#include <cstdio>
#include "Fixtures.hpp"
using namespace PartnerChatAbort30Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::printf("{\"family\":%u,\"res\":%u,\"res_high\":%u,\"conv\":%u,\"loaded\":%u,\"ped\":%u,\"prio\":%u,\"event\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"this_off\":%u,\"slot\":%u}\n",
                    c.family, c.res, c.resHigh, c.conv, c.loaded, c.ped, c.prio, c.event, c.eax, c.edx, c.stack, kThisOff, kMakeAbortableSlot);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"sub_bytes\":%u,\"sub_off\":%u,\"sub_arena\":%u,\"slots\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kSubBytes, kSubOff, kSubArena, kSlots);
    return 0;
}
