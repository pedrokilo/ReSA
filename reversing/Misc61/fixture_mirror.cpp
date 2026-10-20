// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena y de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace Misc61Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) std::printf("0\t%s\n", JsonC1(MakeC1(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("1\t%s\n", JsonC2(MakeC2(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("2\t%s\n", JsonC4(MakeC4(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("3\t%s\n", JsonC3(MakeC3(n)).c_str());
    std::printf("S\t{\"heap_pages\":%u,\"page_bytes\":%u,\"heap_this_bytes\":%u,\"commonsize_bytes\":%u,\"commonsize_count\":%u,\"free_node_bytes\":%u,"
                "\"entity_bytes\":%u,\"entity_off\":%u,\"entity_arena\":%u,\"matrix_bytes\":%u,\"matrix_off\":%u,\"matrix_arena\":%u,\"entity_placement_at\":%u,\"entity_matrix_at\":%u,\"matrix_pos_at\":%u,"
                "\"node_bytes\":%u,\"node_off\":%u,\"node_arena\":%u,\"node_light_at\":%u,\"node_found_at\":%u,"
                "\"task_bytes\":%u,\"task_off\":%u,\"task_arena\":%u,\"ped_bytes\":%u,\"ped_off\":%u,\"ped_arena\":%u,\"intel_bytes\":%u,\"intel_off\":%u,\"intel_arena\":%u,\"prim_bytes\":%u,\"prim_off\":%u,\"prim_arena\":%u,"
                "\"task_pos_at\":%u,\"task_aim_at\":%u,\"task_speed_at\":%u,\"task_bits_at\":%u,\"task_timer_at\":%u,"
                "\"ped_placement_at\":%u,\"ped_matrix_at\":%u,\"ped_intel_at\":%u,\"ped_shift_at\":%u,\"ped_currot_at\":%u,\"ped_aimrot_at\":%u,\"ped_type_at\":%u,\"intel_primary_at\":%u,\"stand_time\":%u,\"stand_blend\":%u}\n",
                kHeapPages, kPageBytes, kHeapThisBytes, kCommonSizeBytes, kCommonSizeCount, kFreeNodeBytes,
                kEntityBytes, kEntityOff, kEntityArena, kMatrixBytes, kMatrixOff, kMatrixArena, kEntityPlacementAt, kEntityMatrixAt, kMatrixPosAt,
                kNodeBytes, kNodeOff, kNodeArena, kNodeLightAt, kNodeFoundAt,
                kTaskBytes, kTaskOff, kTaskArena, kPedBytes, kPedOff, kPedArena, kIntelBytes, kIntelOff, kIntelArena, kPrimBytes, kPrimOff, kPrimArena,
                kTaskPosAt, kTaskAimAt, kTaskSpeedAt, kTaskBitsAt, kTaskTimerAt,
                kPedPlacementAt, kPedMatrixAt, kPedIntelAt, kPedShiftAt, kPedCurRotAt, kPedAimRotAt, kPedTypeAt, kIntelPrimaryDefaultAt, kStandTime, kStandBlendBits);
    return 0;
}
