// Volcado de las 4096 fixtures de Fixtures.hpp como filas JSON (las mismas claves que `input` de Core.hpp) y de las constantes de la arena.
// Solo sirve para fixture_selfcheck.py (autoprueba cruzada oraculo <-> Fixtures.hpp sin juego): cl /EHsc /O2 /std:c++20 fixture_dump.cpp
#include <cstdio>
#include "Fixtures.hpp"
using namespace CameraShake30Fix;

static void Array(const char* key, const u32* v, unsigned count) {
    std::printf("\"%s\":[", key);
    for (unsigned i = 0; i < count; ++i) std::printf(i ? ",%u" : "%u", v[i]);
    std::printf("]");
}

int main() {
    std::printf("{\"layout\":{\"guard\":%u,\"obj_bytes\":%u,\"arena\":%u,\"table_bytes\":%u,\"entry_bytes\":%u,\"entries\":%u,\"cases\":%u}}\n",
        kGuard, kObjBytes, kArena, kTableBytes, kEntryBytes, kEntries, kCases);
    for (unsigned n = 0; n < kCases; ++n) {
        const F f = MakeF(n);
        u8 table[kTableBytes];
        FillWords(table, kTableBytes, f.tableSeed);
        std::printf("{\"case\":%u,\"family\":%u,\"type\":%u,\"intensity\":%u,\"active\":%u,", n, f.family, f.type, f.intensity, f.active);
        Array("front", f.front, 3); std::printf(","); Array("up", f.up, 3);
        std::printf(",\"ang_z\":%u,", f.angZ); Array("mat", f.mat, 9);
        std::printf(",\"flag\":%u,\"table_in\":\"", f.flag);
        for (unsigned i = 0; i < kTableBytes; ++i) std::printf("%02x", table[i]);
        std::printf("\",\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n", f.eax, f.edx, f.stack);
    }
    return 0;
}
