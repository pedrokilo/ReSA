// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Capacidad del pool de tareas del juego: 500 (Pools.cpp: ConstructPool<CTaskPool>(500, 0x80)).
// La ultima linea imprime las constantes de arena para contrastarlas con el oraculo.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace WalkRoundNext31Fix;
static void Vec(const char* name, const u32* v) { std::printf("\"%s\":[%u,%u,%u],", name, v[0], v[1], v[2]); }
int main(int argc, char** argv) {
    const u32 cap = argc > 1 ? u32(std::atoi(argv[1])) : 500u;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const Pool p = MakePool(c, cap, n);
        std::printf("{\"family\":%u,\"obj_null\":%u,\"sub_type\":%u,\"route_ret\":%u,\"route_init\":[%u", c.family, c.objNull, c.subType, c.routeRet, c.routeCount);
        for (u32 j = 0; j < kRouteWords - 1; ++j) std::printf(",%u", c.routeInit[j]);
        std::printf("],");
        Vec("target", c.target); Vec("p1", c.p1); Vec("p2", c.p2);
        std::printf("\"side1\":%d,\"side2\":%d,\"move_state\":%u,\"pool_mode\":%u,\"pool_k\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"pool\":{\"cap\":%u,\"last\":%ld,\"free_total\":%u,\"win\":[",
                    c.side1, c.side2, c.moveState, c.poolMode, c.poolK, c.eax, c.edx, c.stack, cap, p.last, p.freeTotal);
        for (size_t k = 0; k < p.win.size(); ++k) std::printf("%s[%u,%u]", k ? "," : "", p.win[k].idx, p.win[k].state);
        std::printf("]}}\n");
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"sub_bytes\":%u,\"sub_off\":%u,\"sub_arena\":%u,\"obj_bytes\":%u,\"obj_off\":%u,\"obj_arena\":%u,"
                "\"ped_bytes\":%u,\"ped_off\":%u,\"ped_arena\":%u,\"route_bytes\":%u,\"route_off\":%u,\"route_arena\":%u,\"slot\":%u,\"window\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kSubBytes, kSubOff, kSubArena, kObjBytes, kObjOff, kObjArena, kPedBytes, kPedOff, kPedArena, kRouteBytes, kRouteOff, kRouteArena, kSlot, kWindowMax);
    return 0;
}
