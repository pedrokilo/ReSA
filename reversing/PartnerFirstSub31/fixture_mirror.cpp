// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py.
#include <cstdio>
#include "Fixtures.hpp"
using namespace PartnerFirstSub31Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::printf("{\"family\":%u,\"lead\":%u,\"task_id\":%u,\"conv\":%u,\"f75\":%u,\"loaded\":%u,\"req\":%u,\"req_high\":%u,\"alloc_null\":%s,\"fill\":%u,\"ped\":%u,\"partner\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"this_off\":%u,\"obj_off\":%u}\n",
                    c.family, c.lead, c.taskId, c.conv, c.f75, c.loaded, c.req, c.reqHigh, c.allocNull ? "true" : "false", c.fill, c.ped, c.partner, c.eax, c.edx, c.stack, kThisOff, kObjOff);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"obj_bytes\":%u,\"obj_off\":%u,\"obj_arena\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kObjBytes, kObjOff, kObjArena);
    return 0;
}
