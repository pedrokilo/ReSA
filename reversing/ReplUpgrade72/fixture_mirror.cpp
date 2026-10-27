// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Ultima linea: constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace R72Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    std::printf("S\t{\"veh\":%u,\"vmi\":%u,\"u0\":%u,\"hnd\":%u,\"f0\":%u,\"f1\":%u,\"fa\":%u,\"at0\":%u,\"at1\":%u,\"at2\":%u,\"clump\":%u,\"g1\":%u,\"size\":%u,\"max_events\":%u,\"ev_args\":%u,\"v_words\":%u,\"families\":%u,\"pool\":%u,\"num_infos\":%u}\n",
                oVeh, oVMi, oU0, oHnd, oF0, oF1, oFA, oAT0, oAT1, oAT2, oClump, oG1, kArenaSize, kMaxEvents, kEvArgs, kVWords, kFamilies, kPoolSize, kNumModelInfos);
    return 0;
}
