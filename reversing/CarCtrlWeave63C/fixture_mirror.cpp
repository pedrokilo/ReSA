// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp
// (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace CwCFix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    std::printf("S\t{\"ent_words\":%u,\"links\":%u,\"link_bytes\":%u,\"link_words\":%u,\"guard\":%u,\"speed\":%u,\"seed\":%u,\"b36\":%u,\"fieldc\":%u,\"scale\":%u,\"cur\":%u,\"next\":%u,\"lane_curr\":%u,"
                "\"lane_next\":%u,\"lane_cur\":%u,\"lane_nx\":%u,\"tempact\":%u,\"temptime\":%u,\"move\":%u,\"sub\":%u,\"timer\":%u,\"navi\":%u}\n",
                kEntWords, kLinks, kLinkBytes, kLinkWords, kGuardWords, kOffSpeed, kOffSeed, kOffB36, kOffFieldC, kOffScale, kOffCur, kOffNext, kOffLaneCurr, kOffLaneNext, kOffLaneCur, kOffLaneNx, kOffTempAct,
                kOffTempTime, kOffMove, kOffSub, kTimerAddr, kNaviAddr);
    return 0;
}
