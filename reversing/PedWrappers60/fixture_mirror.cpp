// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input` (la serializacion es la misma funcion de
// Fixtures.hpp), prefijada con la funcion (K = SpecialEntityCalcCollisionSteps, S = SpecialEntityPreCollisionStuff, P = ProcessControl). Lo compila y lo ejecuta fixture_mirror.py.
#include <cstdio>
#include "Fixtures.hpp"
using namespace PedWrappers60Fix;
int main() {
    for (u32 n = 0; n < kCasesCS; ++n) std::printf("K %s\n", JsonK(MakeK(n)).c_str());
    for (u32 n = 0; n < kCasesPS; ++n) std::printf("S %s\n", JsonS(MakeS(n)).c_str());
    for (u32 n = 0; n < kCasesPC; ++n) std::printf("P %s\n", JsonP(MakeP(n)).c_str());
    std::printf("Z {\"sizes\":{\"ped\":[%u,%u,%u],\"pdata\":[%u,%u,%u],\"pped\":[%u,%u,%u],\"ent\":[%u,%u,%u],\"veh\":[%u,%u,%u],\"mat\":[%u,%u,%u],\"minfo\":[%u,%u,%u],\"tok\":[%u,%u,%u],"
                "\"matarr\":[%u,%u,%u],\"bools\":[%u,%u,%u],\"vtbl\":[%u,%u,%u]}}\n",
                kPedBytes, kPedOff, kPedArena, kPdataBytes, kPdataOff, kPdataArena, kPpedBytes, kPpedOff, kPpedArena, kEntBytes, kEntOff, kEntArena, kVehBytes, kVehOff, kVehArena,
                kMatBytes, kMatOff, kMatArena, kMinfoBytes, kMinfoOff, kMinfoArena, kTokBytes, kTokOff, kTokArena, kMatArrBytes, kMatArrOff, kMatArrArena, kBoolsBytes, kBoolsOff, kBoolsArena,
                kVtblBytes, kVtblOff, kVtblArena);
    return 0;
}
