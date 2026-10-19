// Volcado de las 4096 fixtures de Fixtures.hpp como filas JSON (las mismas claves que `input` de Core.hpp) y de las constantes de la arena.
// Solo sirve para fixture_selfcheck.py (autoprueba cruzada oraculo <-> Fixtures.hpp sin juego): cl /EHsc /O2 /std:c++20 fixture_dump.cpp
#include <cstdio>
#include "Fixtures.hpp"
using namespace CameraSoundDist30Fix;

static void Array3(const char* key, const u32* v) { std::printf("\"%s\":[%u,%u,%u]", key, v[0], v[1], v[2]); }

int main() {
    std::printf("{\"layout\":{\"guard\":%u,\"obj_bytes\":%u,\"entity_bytes\":%u,\"matrix_bytes\":%u,\"arena\":%u,\"written_bytes\":%u,\"cases\":%u}}\n",
        kGuard, kObjBytes, kEntityBytes, kMatrixBytes, kArena, kWrittenBytes, kCases);
    for (unsigned n = 0; n < kCases; ++n) {
        const F f = MakeF(n);
        std::printf("{\"case\":%u,\"family\":%u,\"mode\":%u,\"active\":%u,\"etype\":%u,", n, f.family, f.mode, f.active, f.etype);
        Array3("fwd", f.fwd); std::printf(",\"has_matrix\":%u,", f.hasMatrix);
        Array3("mat_pos", f.matPos); std::printf(","); Array3("place_pos", f.placePos);
        std::printf(",\"frame\":%u,\"old158\":%u,\"old15c\":%u,\"old160\":%u,\"hit\":%u,\"hit_z\":%u,\"ret_high\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n",
            f.frame, f.old158, f.old15C, f.old160, f.hit, f.hitZ, f.retHigh, f.eax, f.edx, f.stack);
    }
    return 0;
}
