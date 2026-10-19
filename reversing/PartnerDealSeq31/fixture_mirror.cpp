// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`
// (mas los azares de los nodos de referencia previos), usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py.
// Capacidad del pool de tareas del juego: 500 (Pools.cpp: ConstructPool<CTaskPool>(500, 0x80)). La ultima linea imprime las constantes de arena para contrastarlas con el oraculo.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace PartnerDealSeq31Fix;
int main(int argc, char** argv) {
    const u32 cap = argc > 1 ? u32(std::atoi(argv[1])) : 500u;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const Pool p = MakePool(c, cap, n);
        std::printf("{\"family\":%u,\"lead\":%u,\"has_partner\":%u,\"etype\":%u,\"temp\":%u,\"proc\":%u,\"ipl\":%u,\"refs_len\":%u,\"dup_kind\":%u,\"pool_mode\":%u,\"pool_k\":%u,"
                    "\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"dup_idx\":%u,\"pp_rand\":[%u,%u,%u],\"pool\":{\"cap\":%u,\"last\":%ld,\"free_total\":%u,\"win\":[",
                    c.family, c.lead, c.hasPartner, c.etype, c.temp, c.proc, c.ipl, c.refsLen, c.dupKind, c.poolMode, c.poolK,
                    c.eax, c.edx, c.stack, c.refsLen ? Rand(n, 40) % c.refsLen : 0u, Rand(n, 41) & 0xFu, Rand(n, 42) & 0xFu, Rand(n, 43) & 0xFu, cap, p.last, p.freeTotal);
        for (size_t k = 0; k < p.win.size(); ++k) std::printf("%s[%u,%u]", k ? "," : "", p.win[k].idx, p.win[k].state);
        std::printf("]}}\n");
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"ent_bytes\":%u,\"ent_off\":%u,\"ent_arena\":%u,"
                "\"node_size\":%u,\"node_off\":%u,\"node_arena\":%u,\"slot\":%u,\"window\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kEntBytes, kEntOff, kEntArena, kNodeSize, kNodeOff, kNodeArena, kSlot, kWindowMax);
    return 0;
}
