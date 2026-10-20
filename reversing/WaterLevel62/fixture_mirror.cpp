// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena y de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace Water62Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) std::printf("0\t%s\n", JsonA0(MakeA0(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("1\t%s\n", JsonB1(MakeB1(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("2\t%s\n", JsonC2(MakeC2(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("3\t%s\n", JsonC3(MakeC3(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("4\t%s\n", JsonE4(MakeE4(n)).c_str());
    std::printf("S\t{\"vertex_bytes\":%u,\"vertex_count\":%u,\"quad_bytes\":%u,\"quad_count\":%u,\"tri_bytes\":%u,\"tri_count\":%u,\"block_words\":%u,\"combo_words\":%u,"
                "\"depth_obj\":%u,\"depth_obj_bytes\":%u,\"depth_arena\":%u,\"line_obj\":%u,\"line_obj_bytes\":%u,\"line_arena\":%u,\"null_out\":%u}\n",
                kVertexBytes, kVertexCount, kQuadBytes, kQuadCount, kTriBytes, kTriCount, kBlockWords, kComboWords,
                kDepthObj, kDepthObjBytes, kDepthArena, kLineObj, kLineObjBytes, kLineArena, kNullOut);
    return 0;
}
