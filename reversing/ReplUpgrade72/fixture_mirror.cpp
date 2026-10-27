// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Ultima linea: constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace R72Fix;
int main() {
    // Mapa de nulos SINTETICO (solo para medir cobertura sin el juego): nulas 612..999, 1194..1999 y 18631..19999 (el juego real puede diferir; los indices reales se eligen en `begin`)
    std::vector<u8> isNull(kNumModelInfos, 0);
    for (u32 i = 612; i < 1000; ++i) isNull[i] = 1;
    for (u32 i = 1194; i < 2000; ++i) isNull[i] = 1;
    for (u32 i = 18631; i < kNumModelInfos; ++i) isNull[i] = 1;
    std::array<u32, kSel> sel{};
    if (!SelectNullSlots(isNull, sel)) return 2;
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) { Spec sp = MakeSpec(fn, n); Realize(sp, sel); std::printf("%u\t%s\n", fn, JsonSpec(sp).c_str()); }
    std::printf("S\t{\"veh\":%u,\"vmi\":%u,\"u0\":%u,\"hnd\":%u,\"f0\":%u,\"f1\":%u,\"fa\":%u,\"at0\":%u,\"at1\":%u,\"at2\":%u,\"clump\":%u,\"g1\":%u,\"size\":%u,\"max_events\":%u,\"ev_args\":%u,\"v_words\":%u,\"families\":%u,\"pool\":%u,\"num_infos\":%u}\n",
                oVeh, oVMi, oU0, oHnd, oF0, oF1, oFA, oAT0, oAT1, oAT2, oClump, oG1, kArenaSize, kMaxEvents, kEvArgs, kVWords, kFamilies, kSel, kNumModelInfos);
    return 0;
}
