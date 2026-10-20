// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso (funcion x caso) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace FollowFootsteps62Fix;
static void Arr(const u32* v, unsigned n) { std::printf("["); for (unsigned i = 0; i < n; ++i) std::printf(i ? ",%u" : "%u", v[i]); std::printf("]"); }
int main() {
    for (u32 fn = 0; fn < kFns; ++fn) {
        for (u32 n = 0; n < kCasesPer; ++n) {
            const C c = MakeC(fn, n);
            std::printf("{\"fn\":%u,\"family\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"tgt_null\":%u,\"route_null\":%u,\"move_state\":%u,\"flags_byte\":%u,\"create_timer_delta\":%u,\"los_timer_delta\":%u,\"now\":%u,"
                        "\"tgt_mat\":%u,\"ped_mat\":%u,\"tgt_in_veh\":%u,\"tgt_noise\":%u,\"counter\":%u,",
                        c.fn, c.family, c.eax, c.edx, c.stack, c.tgtNull, c.routeNull, c.moveState & 0xFFFFu, c.flagsByte & 0xFFu, c.createTimerDelta, c.losTimerDelta, c.now, c.tgtMat, c.pedMat, c.tgtInVeh, c.tgtNoise, c.counter);
            std::printf("\"tgt\":"); Arr(c.tgt, 3); std::printf(",\"tgt_alt\":"); Arr(c.tgtAlt, 3); std::printf(",\"ped\":"); Arr(c.ped, 3); std::printf(",\"ped_alt\":"); Arr(c.pedAlt, 3);
            std::printf(",\"count\":%u,\"ent\":", c.count); Arr(c.ent, 24);
            std::printf(",\"sub_type\":%u,\"alive_ret\":%u,\"los_ret\":%u,\"simplest_null\":%u,\"is_goto_ret\":%u,\"abort_ret\":%u,\"create_kind\":%u,\"create_val\":%u,\"new_null\":%u,"
                        "\"type\":%u,\"prio\":%u,\"ev_ptr\":%u,\"ped_arg\":%u,\"sub_move0\":%u}\n",
                        c.subType, c.aliveRet, c.losRet, c.simplestNull, c.isGoToRet, c.abortRet, c.createKind, c.createVal, c.newNull, c.type, c.prio, c.evPtr, c.pedArg, 0x5A5A0000u + Rand(n, 60) % 0x10000u);
        }
    }
    std::printf("{\"sizes\":{\"this_off\":%u,\"this_bytes\":%u,\"this_arena\":%u,\"sub_at\":%u,\"target_at\":%u,\"create_timer_at\":%u,\"los_timer_at\":%u,\"route_at\":%u,\"move_at\":%u,\"flags_at\":%u,"
                "\"sub_off\":%u,\"sub_bytes\":%u,\"sub_arena\":%u,\"sub_move_at\":%u,\"ped_off\":%u,\"ped_bytes\":%u,\"ped_arena\":%u,\"place_at\":%u,\"mat_at\":%u,\"ped_flags_at\":%u,\"intel_at\":%u,"
                "\"mat_off\":%u,\"mat_bytes\":%u,\"mat_arena\":%u,\"mat_pos_at\":%u,\"intel_off\":%u,\"intel_bytes\":%u,\"intel_arena\":%u,\"counter_at\":%u,\"mgr_at\":%u,"
                "\"route_off\":%u,\"route_bytes\":%u,\"route_arena\":%u,\"go_off\":%u,\"go_bytes\":%u,\"go_arena\":%u,\"alloc_off\":%u,\"alloc_bytes\":%u,\"alloc_arena\":%u,\"max_events\":%u,\"vt_slots\":%u,\"timer\":%u,\"cases\":%u,\"fns\":%u}}\n",
                kThisOff, kThisBytes, kThisArena, kSubAtThis, kTargetAt, kCreateTimerAt, kLosTimerAt, kRouteAt, kMoveAt, kFlagsAt, kSubOff, kSubBytes, kSubArena, kSubMoveAt, kPedOff, kPedBytes, kPedArena, kPlaceAt, kMatAt, kPedFlagsAt, kIntelAt,
                kMatOff, kMatBytes, kMatArena, kMatPosAt, kIntelOff, kIntelBytes, kIntelArena, kCounterAt, kMgrAt, kRouteOff, kRouteBytes, kRouteArena, kGoOff, kGoBytes, kGoArena, kAllocOff, kAllocBytes, kAllocArena, kMaxEvents, kVtSlots, kTimerGlobal, kCasesPer, kFns);
    return 0;
}
