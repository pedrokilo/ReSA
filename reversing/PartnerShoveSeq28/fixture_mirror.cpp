// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Capacidad del pool de tareas del juego: 500 (Pools.cpp: ConstructPool<CTaskPool>(500, 0x80)).
#include <cstdio>
#include "Fixtures.hpp"
using namespace PartnerShoveSeq28Fix;
int main(int argc, char** argv) {
    const u32 cap = argc > 1 ? u32(std::atoi(argv[1])) : 500u;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const Pool p = MakePool(c, cap, n);
        std::printf("{\"family\":%u,\"lead\":%u,\"has_partner\":%u,\"etype\":%u,\"temp\":%u,\"proc\":%u,\"ipl\":%u,\"refs_len\":%u,\"dup\":%u,\"pool_mode\":%u,\"pool_k\":%u,"
                    "\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"dup_node\":%u,\"pool\":{\"cap\":%u,\"last\":%ld,\"free_total\":%u,\"win\":[",
                    c.family, c.lead, c.hasPartner, c.etype, c.temp, c.proc, c.ipl, c.refsLen, c.dup, c.poolMode, c.poolK, c.eax, c.edx, c.stack,
                    c.refsLen ? Rand(n, 40) % c.refsLen : 0u, cap, p.last, p.freeTotal);
        for (size_t k = 0; k < p.win.size(); ++k) std::printf("%s[%u,%u]", k ? "," : "", p.win[k].idx, p.win[k].state);
        std::printf("]}}\n");
    }
    return 0;
}
