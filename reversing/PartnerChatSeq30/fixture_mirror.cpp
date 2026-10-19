// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`
// (mas los azares de los nodos de referencia previos), usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py.
// Capacidad del pool de tareas del juego: 500 (Pools.cpp: ConstructPool<CTaskPool>(500, 0x80)). La ultima linea imprime las constantes de arena para contrastarlas con el oraculo.
#include <cstdio>
#include "Fixtures.hpp"
using namespace PartnerChatSeq30Fix;
int main(int argc, char** argv) {
    const u32 cap = argc > 1 ? u32(std::atoi(argv[1])) : 500u;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const Pool p = MakePool(c, cap, n);
        const int ns = int(i32(c.nByte << 24) >> 24);
        std::printf("{\"family\":%u,\"lead\":%u,\"has_partner\":%u,\"etype\":%u,\"temp\":%u,\"proc\":%u,\"ipl\":%u,\"refs_len\":%u,\"dup_kind\":%u,\"pool_mode\":%u,\"pool_k\":%u,"
                    "\"conv\":%u,\"other_conv\":%u,\"conv_eff\":%u,\"n\":%d,\"pt\":[%u,%u,%u],\"conv_writes\":[",
                    c.family, c.lead, c.hasPartner, c.etype, c.temp, c.proc, c.ipl, c.refsLen, c.dupKind, c.poolMode, c.poolK, c.conv, c.otherConv, c.convEff, ns, c.pt[0], c.pt[1], c.pt[2]);
        if (c.convEff) {
            const auto w = ConvWrites(c);
            for (size_t k = 0; k < w.size(); ++k) std::printf("%s[%d,%u]", k ? "," : "", w[k].idx, w[k].value);
        }
        std::printf("],\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"dup_idx\":%u,\"pp_rand\":[%u,%u,%u],\"idx_ok\":%d,\"pool\":{\"cap\":%u,\"last\":%ld,\"free_total\":%u,\"win\":[",
                    c.eax, c.edx, c.stack, c.refsLen ? Rand(n, 40) % c.refsLen : 0u, Rand(n, 41) & 0xFu, Rand(n, 42) & 0xFu, Rand(n, 43) & 0xFu, 1, cap, p.last, p.freeTotal);
        for (size_t k = 0; k < p.win.size(); ++k) std::printf("%s[%u,%u]", k ? "," : "", p.win[k].idx, p.win[k].state);
        std::printf("]}}\n");
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"ent_bytes\":%u,\"ent_off\":%u,\"ent_arena\":%u,\"int_bytes\":%u,\"int_off\":%u,\"int_arena\":%u,"
                "\"other_bytes\":%u,\"other_off\":%u,\"other_arena\":%u,\"node_size\":%u,\"node_off\":%u,\"node_arena\":%u,\"slot\":%u,\"window\":%u,\"conv_save_at\":%u,\"conv_save_bytes\":%u,\"conv_base\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kEntBytes, kEntOff, kEntArena, kIntBytes, kIntOff, kIntArena, kOtherBytes, kOtherOff, kOtherArena, kNodeSize, kNodeOff, kNodeArena,
                kSlot, kWindowMax, kConvSaveAt, kConvSaveBytes, kConvBase);
    return 0;
}
