// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`indice-de-funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp
// (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Ultimas lineas: F = FloorRetBits/WidenFloat (muestras), G = Garbage (muestras), S = constantes de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace H65Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonCase(fn, n).c_str());
    for (u32 mode = 0; mode < 8; ++mode)
        for (u32 k = 0; k < 6; ++k) {
            const u32 lo = 0x89ABCDEFu + k, hi = 0x01234567u + mode * 77u;
            std::printf("F\t[%u,%u,%u,%u,%llu]\n", mode, 1234567u + mode * 99u + k, lo, hi, (unsigned long long)FloorRetBits(mode, 1234567u + mode * 99u + k, lo, hi));
        }
    for (u32 x : {0x3F800000u, 0x00000001u, 0x7F800001u, 0x80000000u, 0xFF800000u, 0x007FFFFFu, 0x40490FDBu, 0x7FC12345u, 0xFFFFFFFFu, 0x00800000u, 0x4B000001u})
        std::printf("W\t[%u,%llu]\n", x, (unsigned long long)WidenFloat(x));
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 k = 0; k < 4; ++k) std::printf("G\t[%u,%u,%u,%u]\n", fn, 17u + fn, 123u + k * 17u, Garbage(ImageSeed(fn, 17u + fn), 123u + k * 17u));
    std::printf("S\t{\"arena_words\":%u,\"frames\":%u,\"frame_stride\":%u,\"nodes\":%u,\"found\":%u,\"task_slot\":%u,\"facial_slot\":%u,\"indices\":%u,\"index_count\":%u,\"name_zone\":%u,\"name_bytes\":%u,\"vector\":%u,\"value_slot\":%u,\"max_events\":%u}\n",
                kArenaWords, kFrames, kFrameStride, kNodes, kFound, kTaskSlot, kFacialSlot, kIndices, kIndexCount, kNameZone, kNameBytes, kVector, kValueSlot, kMaxEvents);
    return 0;
}
