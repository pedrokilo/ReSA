// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena.
#include <cstdio>
#include "Fixtures.hpp"
using namespace Small64Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) std::printf("0\t%s\n", JsonA0(MakeA0(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("1\t%s\n", JsonB1(MakeB1(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("2\t%s\n", JsonC2(MakeC2(n)).c_str());
    std::printf("S\t{\"num_groups\":%u,\"num_ped_groups\":%u,\"trans_base\":%u,\"trans_guard\":%u,\"trans_main\":%u,\"trans_bytes\":%u,\"ped_base\":%u,\"ped_guard\":%u,\"ped_main\":%u,\"ped_bytes\":%u,"
                "\"task_obj\":%u,\"task_obj_bytes\":%u,\"task_arena\":%u,\"addr_verts\":%u,\"addr_indices\":%u}\n",
                kNumGroups, kNumPedGroups, kTransBase, kTransGuard, kTransMain, kTransBytes, kPedBase, kPedGuard, kPedMain, kPedBytes,
                kTaskObj, kTaskObjBytes, kTaskArena, kAddrVerts, kAddrIndices);
    return 0;
}
