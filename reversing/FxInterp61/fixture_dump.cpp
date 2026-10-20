// Herramienta SIN el juego: vuelca los `input` (4 funciones x 4096 casos, mismo formato que Core.hpp::InputOf) como JSON lineas, para host_check.py y fixture_mirror.py.
//   cl /nologo /EHsc /std:c++20 /O2 fixture_dump.cpp /Fe:fixture_dump.exe   (en un directorio temporal) ; fixture_dump.exe > casos.jsonl
#include "Fixtures.hpp"
#include <cstdio>
using namespace FxInterp61Fix;
int main() {
    static u8 arena[kArena];
    for (unsigned func = 0; func < kFunctions; ++func) {
        for (unsigned n = 0; n < kCases; ++n) {
            const F f = MakeF(func, n);
            BuildArena(f, arena, 0);
            std::printf("{\"func\":%u,\"family\":%u,\"delta\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"arena\":\"", f.func, f.family, f.delta, f.eax, f.edx, f.stack);
            for (u32 i = 0; i < kArena; ++i) std::printf("%02x", arena[i]);
            std::printf("\"}\n");
        }
    }
}
