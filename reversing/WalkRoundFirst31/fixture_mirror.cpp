// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input` (salvo route_init y this_init, que Core
// toma de los rellenos aleatorios de las arenas: fixture_mirror.py los reconstruye con el mismo generador), usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Capacidad del pool de tareas del juego: 500 (Pools.cpp: ConstructPool<CTaskPool>(500, 0x80)).
// La ultima linea imprime las constantes de arena para contrastarlas con el oraculo.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace WalkRoundFirst31Fix;
static void Vec(const char* name, const u32* v) { std::printf("\"%s\":[%u,%u,%u],", name, v[0], v[1], v[2]); }
int main(int argc, char** argv) {
    const u32 cap = argc > 1 ? u32(std::atoi(argv[1])) : 500u;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const Pool p = MakePool(c, cap, n);
        // arenas de referencia como en Core.hpp: this (Rand(n,50)) y route (Rand(n,55)) rellenas con FillWords; this_init = palabras desde +0x24, route_init = 25 palabras desde kRouteOff
        std::array<u8, kThisArena> thisRef{};
        std::array<u8, kRouteArena> routeRef{};
        FillWords(thisRef.data(), kThisArena, Rand(n, 50));
        FillWords(routeRef.data(), kRouteArena, Rand(n, 55));
        std::printf("{\"family\":%u,\"obj_null\":%u,\"move_state\":%u,\"mat_present\":%u,\"route_count\":%u,\"route_entries\":[", c.family, c.objNull, c.moveState, c.matPresent, c.routeCount);
        for (u32 j = 0; j < kRouteWords - 1; ++j) std::printf("%s%u", j ? "," : "", c.routeEntries[j]);
        std::printf("],\"route_dist\":%u,\"route_ret\":%u,\"time\":%u,", c.routeDist, c.routeRet, c.time);
        Vec("place", c.place); Vec("mat_pos", c.mpos); Vec("mat_fwd", c.mfwd); Vec("mat_right", c.mright); Vec("ped_pos", c.pedPos); Vec("ped_fwd", c.pedFwd); Vec("ped_place", c.pedPlace);
        std::printf("\"route_init\":[");
        for (u32 j = 0; j < kRouteWords; ++j) { u32 w; std::memcpy(&w, routeRef.data() + kRouteOff + j * 4, 4); std::printf("%s%u", j ? "," : "", w); }
        std::printf("],\"this_init\":[");
        for (u32 j = 0; j < 12; ++j) { u32 w; std::memcpy(&w, thisRef.data() + kThisOff + kF24At + j * 4, 4); std::printf("%s%u", j ? "," : "", w); }
        std::printf("],\"pool_mode\":%u,\"pool_k\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"pool\":{\"cap\":%u,\"last\":%ld,\"free_total\":%u,\"win\":[", c.poolMode, c.poolK, c.eax, c.edx, c.stack, cap, p.last, p.freeTotal);
        for (size_t k = 0; k < p.win.size(); ++k) std::printf("%s[%u,%u]", k ? "," : "", p.win[k].idx, p.win[k].state);
        std::printf("]}}\n");
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"obj_bytes\":%u,\"obj_off\":%u,\"obj_arena\":%u,\"mat_bytes\":%u,\"mat_off\":%u,\"mat_arena\":%u,"
                "\"ped_bytes\":%u,\"ped_off\":%u,\"ped_arena\":%u,\"pmat_bytes\":%u,\"pmat_off\":%u,\"pmat_arena\":%u,\"route_bytes\":%u,\"route_off\":%u,\"route_arena\":%u,\"slot\":%u,\"window\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kObjBytes, kObjOff, kObjArena, kMatBytes, kMatOff, kMatArena, kPedBytes, kPedOff, kPedArena, kPMatBytes, kPMatOff, kPMatArena,
                kRouteBytes, kRouteOff, kRouteArena, kSlot, kWindowMax);
    return 0;
}
