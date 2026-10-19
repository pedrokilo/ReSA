// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Las direcciones de las arenas (punteros de asociaciones y bloques) son ficticias pero coherentes
// con las del banco real: al oraculo solo le importa la aritmetica ((bloque - 0xB5D4A0) >> 5) y los bytes que cambian al poner a cero un puntero.
#include <cstdio>
#include "Fixtures.hpp"
using namespace FightProcessPed31Fix;
static void Arr(const char* name, const u32* v, int n) {
    std::printf("\"%s\":[", name);
    for (int i = 0; i < n; ++i) std::printf("%s%u", i ? "," : "", v[i]);
    std::printf("],");
}
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const u32 animPtr = c.hasAnim ? 0x00A1B2C0u + n * 0x100u : 0u, idlePtr = c.hasIdle ? 0x00B3C4D0u + n * 0x100u : 0u, newIdlePtr = 0x00C5D6E0u + n * 0x100u;
        u32 blocks[3], gb[4];
        for (u32 k = 0; k < 3; ++k) blocks[k] = 0x00B5E000u + 0x20u * k + (n % 7) * 0x400u;
        for (u32 k = 0; k < 4; ++k) gb[k] = c.groupBlock[k] ? blocks[c.groupBlock[k] - 1] : 0u;
        std::printf("{\"family\":%u,\"finished\":%u,\"in_control\":%u,\"anims_ref\":%u,\"req_group\":%u,\"idle_period\":%u,\"idle_counter\":%u,\"combo\":%u,\"move\":%u,\"next\":%u,\"last\":%u,"
                    "\"has_target\":%u,\"has_anim\":%u,\"has_idle\":%u,\"has_hier\":%u,\"anim_w\":%u,\"anim_d\":%u,\"anim_time\":%u,\"anim_step\":%u,\"anim_flags\":%u,\"anim_id\":%u,\"hier_total\":%u,"
                    "\"idle_w\":%u,\"idle_d\":%u,\"is_player\":%u,\"move_state\":%u,\"wslot\":%u,\"wtype\":%u,\"style\":%u,\"group\":%u,\"clump\":%u,\"has_matrix\":%u,\"swim0\":%u,",
                    c.family, c.finished, c.inControl, c.animsRef, c.reqGroup, c.idlePeriod, c.idleCounter, c.combo, c.move, c.next, c.last, c.hasTarget, c.hasAnim, c.hasIdle, c.hasHier, c.animW,
                    c.animD, c.animTime, c.animStep, c.animFlags, c.animId, c.hierTotal, c.idleW, c.idleD, c.isPlayer, c.moveState, c.slot, c.wtype, c.style, c.group, c.clump, c.hasMatrix, c.swim0);
        Arr("mat", c.mat.data(), 12);
        Arr("ped_pos", c.pedPos.data(), 3);
        Arr("speed", c.speed.data(), 3);
        std::printf("\"tgt_has_matrix\":%u,", c.tgtHasMatrix);
        Arr("tgt_pos", c.tgtPos.data(), 3);
        Arr("tgt_mat_pos", c.tgtMatPos.data(), 3);
        std::printf("\"cam_flag\":%u,\"get_target\":%u,", c.camFlag, c.getTarget);
        Arr("cam_front", c.camFront.data(), 3);
        std::printf("\"timestep\":%u,\"base_combo\":%u,\"r_avail\":%u,\"r_attack\":%u,", c.timestep, c.baseCombo, c.rAvail, c.rAttack);
        Arr("group_block_addr", gb, 4);
        Arr("block_addr", blocks, 3);
        Arr("block_loaded", c.blockLoaded.data(), 3);
        std::printf("\"name_block_addr\":%u,\"table_seed\":%u,\"table_writes\":[", blocks[c.nameBlock - 1], c.tableSeed);
        for (size_t i = 0; i < c.writes.size(); ++i) std::printf("%s[%u,%u]", i ? "," : "", c.writes[i].addr, c.writes[i].value);
        std::printf("],\"anim_ptr\":%u,\"idle_ptr\":%u,\"newidle_ptr\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"this_off\":%u,\"vslot\":%u}\n", animPtr, idlePtr, newIdlePtr, c.eax, c.edx, c.stack,
                    kThisOff, kProcessPedSlot);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"anim_bytes\":%u,\"anim_off\":%u,\"anim_arena\":%u,\"hier_bytes\":%u,\"hier_off\":%u,\"hier_arena\":%u,"
                "\"ped_bytes\":%u,\"ped_off\":%u,\"ped_arena\":%u,\"mat_bytes\":%u,\"mat_off\":%u,\"mat_arena\":%u,\"tgt_bytes\":%u,\"tgt_off\":%u,\"tgt_arena\":%u,"
                "\"blocks_bytes\":%u,\"blocks_off\":%u,\"blocks_arena\":%u,\"groups_bytes\":%u,\"groups_off\":%u,\"groups_arena\":%u,\"winfo_bytes\":%u,\"winfo_off\":%u,\"winfo_arena\":%u,"
                "\"pad_bytes\":%u,\"pad_off\":%u,\"pad_arena\":%u,\"region\":%u,\"slots\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kAnimBytes, kAnimOff, kAnimArena, kHierBytes, kHierOff, kHierArena, kPedBytes, kPedOff, kPedArena, kMatBytes, kMatOff, kMatArena,
                kTgtBytes, kTgtOff, kTgtArena, kBlocksBytes, kBlocksOff, kBlocksArena, kGroupsBytes, kGroupsOff, kGroupsArena, kWinfoBytes, kWinfoOff, kWinfoArena, kPadBytes, kPadOff, kPadArena, kRegion, kSlots);
    return 0;
}
