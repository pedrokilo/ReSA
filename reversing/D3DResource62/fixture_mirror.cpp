// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace D3D62Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    std::printf("S\t{\"region_words\":%u,\"tex_base\":%u,\"tex_stride\":%u,\"ib_base\":%u,\"ib_stride\":%u,\"gen_base\":%u,\"free_tex\":%u,\"free_ib\":%u,\"tex_buffers\":%u,\"ib_buffers\":%u,"
                "\"stacks\":%u,\"stack_slots\":%u,\"stack_guard\":%u,\"stack_words\":%u,\"res_count\":%u,\"arg_res\":%u,\"res_words\":%u,\"new_blocks\":%u,\"new_block_words\":%u,\"out_words\":%u,"
                "\"stack_of_tex_16\":%u,\"stack_of_ib_0\":%u,\"stack_of_ib_16\":%u,\"region_base\":%u,\"use_addr\":%u,\"device_addr\":%u}\n",
                kRegionWords, kTexBase, kTexStride, kIbBase, kIbStride, kGenBase, kFreeTexWord, kFreeIbWord, kTexBuffers, kIbBuffers, kStacks, kStackSlots, kStackGuard, kStackWords, kResCount, kArgRes,
                kResWords, kNewBlocks, kNewBlockWords, kOutWords, kStackOfTex(16), kStackOfIb(0), kStackOfIb(16), kRegionBase, kUseAddr, kDeviceAddr);
    return 0;
}
