// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Ultima linea: constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace V66Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    std::printf("S\t{\"veh\":%u,\"vmi\":%u,\"vst\":%u,\"u0\":%u,\"hnd\":%u,\"ped\":%u,\"mat\":%u,\"clump\":%u,\"g1\":%u,\"size\":%u,\"max_events\":%u,\"ev_args\":%u,\"v_words\":%u,\"families\":%u}\n",
                oVeh, oVMi, oVSt, oU0, oHnd, oPed, oMat, oClump, oG1, kArenaSize, kMaxEvents, kEvArgs, kVWords, kFamilies);
    return 0;
}
