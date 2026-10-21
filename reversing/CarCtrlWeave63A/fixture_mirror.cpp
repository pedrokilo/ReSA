// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp
// (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace CwAFix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    for (u32 k = 0; k < 16; ++k) std::printf("R\t[%u,%u,%u]\n", 0x1234567u + k, k, Rand(0x1234567u + k, k * 4 + 2));
    std::printf("S\t{\"objs\":%u,\"ent_words\":%u,\"mat_words\":%u,\"cm_words\":%u,\"mi_words\":%u,\"guard\":%u,\"pos\":%u,\"heading\":%u,\"matrix\":%u,\"model\":%u,\"b36\":%u,\"speed\":%u,\"timesw\":%u,"
                "\"style\":%u,\"mission\":%u,\"tempact\":%u,\"temptime\":%u,\"ctrl\":%u,\"target\":%u,\"obstr\":%u,\"b42b\":%u,\"d46c\":%u,\"d58c\":%u,\"timer\":%u}\n",
                kObjs, kEntWords, kMatWords, kCmWords, kMiWords, kGuardWords, kOffPos, kOffHeading, kOffMatrix, kOffModel, kOffB36, kOffSpeed, kOffTimeSw, kOffStyle, kOffMission, kOffTempAct, kOffTempTime,
                kOffCtrl, kOffTarget, kOffObstr, kOff42B, kOff46C, kOff58C, kTimerAddr);
    return 0;
}
