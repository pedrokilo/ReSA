// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Las ultimas lineas: muestras de RetVal (R) y constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace D3D63Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    for (u32 mode = 0; mode < 8; ++mode)
        for (u32 k = 0; k < 20; ++k) std::printf("R\t[%u,%u,%u,%u]\n", mode, 0x1234567u + mode * 77u, k, RetVal(mode, 0x1234567u + mode * 77u, k));
    std::printf("S\t{\"region_words\":%u,\"tex_base\":%u,\"tex_stride\":%u,\"ib_base\":%u,\"ib_stride\":%u,\"ib_buffers\":%u,\"gen\":%u,\"gen_word\":%u,\"region_base\":%u,\"max_events\":%u}\n",
                kRegionWords, kTexBufferBase, kTexStride, kIbBufferBase, kIbStride, kIbBuffers, kGenericIb, kGenWord, kRegionBase, kMaxEvents);
    return 0;
}
