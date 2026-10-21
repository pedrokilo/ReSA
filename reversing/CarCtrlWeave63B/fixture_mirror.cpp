// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp
// (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace CwBFix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    std::printf("S\t{\"ents\":%u,\"nodes\":%u,\"ent_words\":%u,\"mat_words\":%u,\"cm_words\":%u,\"mi_words\":%u,\"intel_words\":%u,\"guard\":%u,\"pos\":%u,\"matrix\":%u,\"model\":%u,\"flags\":%u,\"scan\":%u,\"b36\":%u,\"speed\":%u,"
                "\"style\":%u,\"tempact\":%u,\"temptime\":%u,\"ctrl\":%u,\"b42b\":%u,\"driver\":%u,\"intel\":%u,\"d514\":%u,\"d584\":%u,\"dfc\":%u,\"vtype\":%u,\"timer\":%u,\"scan_addr\":%u,\"focus\":%u,\"players\":%u,\"remote\":%u}\n",
                kEnts, kMaxNodes, kEntWords, kMatWords, kCmWords, kMiWords, kIntelWords, kGuardWords, kOffPos, kOffMatrix, kOffModel, kOffFlags, kOffScan, kOffB36, kOffSpeed, kOffStyle, kOffTempAct, kOffTempTime,
                kOffCtrl, kOff42B, kOffDriver, kOffIntel, kOff514, kOff584, kOffFC, kOffVType, kTimerAddr, kScanAddr, kFocusAddr, kPlayersAddr, kRemoteOff);
    return 0;
}
