// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace PartnerNext60Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::printf("{\"family\":%u,\"state\":%u,\"task_id\":%u,\"partner_null\":%u,\"found\":%u,\"other_is_ped\":%u,\"other_state\":%u,\"other_first\":%u,\"upd\":%u,\"streamed\":%u,\"face\":%u,\"first\":%u,\"lead\":%u,"
                    "\"sub_type\":%u,\"counter\":%u,\"point\":[%u,%u,%u],\"target\":[%u,%u,%u],\"calc\":[%u,%u,%u],\"def_kind\":%u,\"def_type\":%u,\"wander_type\":%u,\"ped_mat\":%u,\"ped\":[%u,%u],\"part\":[%u,%u],"
                    "\"heading\":%u,\"fwd\":[%u,%u],\"right\":[%u,%u],\"heading_ret\":%u,\"alloc0_null\":%u,\"alloc1_null\":%u,\"time_ms\":%u,\"seq_ret\":%u,\"fill0\":%u,\"fill1\":%u,"
                    "\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"this_off\":%u}\n",
                    c.family, c.state, c.taskId, c.partnerNull, c.found, c.otherIsPed, c.otherState, c.otherFirst, c.upd, c.streamed, c.face, c.first, c.lead,
                    c.subType, c.counter, c.point[0], c.point[1], c.point[2], c.target[0], c.target[1], c.target[2], c.calc[0], c.calc[1], c.calc[2], c.defKind, c.defType, c.wanderType, c.pedMat,
                    c.ped[0], c.ped[1], c.part[0], c.part[1], c.heading, c.fwd[0], c.fwd[1], c.right[0], c.right[1], c.headingRet, c.alloc0Null, c.alloc1Null, c.timeMs, c.seqRet, c.fill0, c.fill1,
                    c.eax, c.edx, c.stack, kThisOff);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"sub_at\":%u,\"task_id_at\":%u,\"partner_at\":%u,\"point_at\":%u,\"target_at\":%u,\"lead_at\":%u,\"state_at\":%u,\"first_at\":%u,\"upd_at\":%u,"
                "\"face_at\":%u,\"streamed_at\":%u,\"counter_at\":%u,\"ped_off\":%u,\"ped_bytes\":%u,\"ped_arena\":%u,\"placement_at\":%u,\"heading_at\":%u,\"matrix_at\":%u,\"intel_at\":%u,\"shift_at\":%u,"
                "\"mat_off\":%u,\"mat_bytes\":%u,\"mat_arena\":%u,\"intel_off\":%u,\"intel_bytes\":%u,\"intel_arena\":%u,\"def_at_intel\":%u,\"task_off\":%u,\"task_bytes\":%u,\"task_arena\":%u,"
                "\"other_off\":%u,\"other_bytes\":%u,\"other_arena\":%u,\"other_partner_at\":%u,\"other_state_at\":%u,\"other_first_at\":%u,\"other_point_at\":%u,"
                "\"alloc_off\":%u,\"alloc_bytes\":%u,\"alloc_arena\":%u,\"seq_off\":%u,\"seq_bytes\":%u,\"seq_arena\":%u,\"time_addr\":%u,\"max_events\":%u,\"vt_slots\":%u,"
                "\"size_stand\":%u,\"size_wander\":%u,\"size_goto\":%u,\"size_turn\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kSubAtThis, kTaskIdAt, kPartnerAt, kPointAt, kTargetAt, kLeadAt, kStateAt, kFirstAt, kUpdAt, kFaceAt, kStreamedAt, kCounterAt,
                kPedOff, kPedBytes, kPedArena, kPlacementAt, kHeadingAt, kMatrixAt, kIntelAt, kShiftAt, kMatOff, kMatBytes, kMatArena, kIntelOff, kIntelBytes, kIntelArena, kDefAtIntel,
                kTaskOff, kTaskBytes, kTaskArena, kOtherOff, kOtherBytes, kOtherArena, kOtherPartnerAt, kOtherStateAt, kOtherFirstAt, kOtherPointAt,
                kAllocOff, kAllocBytes, kAllocArena, kSeqOff, kSeqBytes, kSeqArena, kTimeAddr, kMaxEvents, kVtSlots, kSizeStand, kSizeWander, kSizeGoTo, kSizeTurn);
    return 0;
}
