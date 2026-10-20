// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace PartnerControl60Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::printf("{\"family\":%u,\"state\":%u,\"partner_null\":%u,\"completed\":%u,\"found\":%u,\"other_is_ped\":%u,\"abort1\":%u,\"abort2\":%u,\"sub_type\":%u,\"counter\":%u,\"looking\":%u,\"task_id\":%u,"
                    "\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"this_off\":%u}\n",
                    c.family, c.state, c.partnerNull, c.completed, c.found, c.otherIsPed, c.abort1, c.abort2, c.subType, c.counter, c.looking, c.taskId, c.eax, c.edx, c.stack, kThisOff);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"sub_at\":%u,\"task_id_at\":%u,\"partner_at\":%u,\"state_at\":%u,\"completed_at\":%u,\"counter_at\":%u,"
                "\"sub_off\":%u,\"sub_bytes\":%u,\"sub_arena\":%u,\"ped_off\":%u,\"ped_bytes\":%u,\"ped_arena\":%u,\"part_off\":%u,\"part_bytes\":%u,\"part_arena\":%u,\"intel_at_partner\":%u,"
                "\"intel_off\":%u,\"intel_bytes\":%u,\"intel_arena\":%u,\"other_off\":%u,\"other_bytes\":%u,\"other_arena\":%u,\"other_partner_at\":%u,\"ik\":%u,\"goto_type\":%u,\"max_events\":%u,\"vt_slots\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kSubAtThis, kTaskIdAt, kPartnerAt, kStateAt, kCompletedAt, kCounterAt, kSubOff, kSubBytes, kSubArena, kPedOff, kPedBytes, kPedArena,
                kPartOff, kPartBytes, kPartArena, kIntelAtPartner, kIntelOff, kIntelBytes, kIntelArena, kOtherOff, kOtherBytes, kOtherArena, kOtherPartnerAt, kIkGlobal, kGoToType, kMaxEvents, kVtSlots);
    return 0;
}
