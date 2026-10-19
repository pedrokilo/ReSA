// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Capacidad del pool de tareas del juego: 500 (Pools.cpp: ConstructPool<CTaskPool>(500, 0x80)).
// La ultima linea imprime las constantes de arena para contrastarlas con el oraculo.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace WalkRoundControl31Fix;
static void Vec(const char* name, const u32* v) { std::printf("\"%s\":[%u,%u,%u],", name, v[0], v[1], v[2]); }
int main(int argc, char** argv) {
    const u32 cap = argc > 1 ? u32(std::atoi(argv[1])) : 500u;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const Pool p = MakePool(c, cap, n);
        std::printf("{\"family\":%u,\"obj_null\":%u,\"sub_type\":%u,\"abort_res\":%u,\"f2c\":%u,\"f2d\":%u,\"f24\":%u,\"f28\":%u,\"time\":%u,\"counter\":%d,\"mat_present\":%u,", c.family, c.objNull, c.subType, c.abortRes, c.f2c, c.f2d, c.f24, c.f28, c.time, c.counter, c.matPresent);
        Vec("start_pos", c.startPos); Vec("start_fwd", c.startFwd); Vec("start_right", c.startRight); Vec("pos", c.pos); Vec("fwd", c.fwd); Vec("right", c.right); Vec("place", c.place);
        std::printf("\"pool_mode\":%u,\"pool_k\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"pool\":{\"cap\":%u,\"last\":%ld,\"free_total\":%u,\"win\":[", c.poolMode, c.poolK, c.eax, c.edx, c.stack, cap, p.last, p.freeTotal);
        for (size_t k = 0; k < p.win.size(); ++k) std::printf("%s[%u,%u]", k ? "," : "", p.win[k].idx, p.win[k].state);
        std::printf("]}}\n");
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"sub_bytes\":%u,\"sub_off\":%u,\"sub_arena\":%u,\"obj_bytes\":%u,\"obj_off\":%u,\"obj_arena\":%u,"
                "\"mat_bytes\":%u,\"mat_off\":%u,\"mat_arena\":%u,\"ped_bytes\":%u,\"ped_off\":%u,\"ped_arena\":%u,\"int_bytes\":%u,\"int_off\":%u,\"int_arena\":%u,\"slot\":%u,\"window\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kSubBytes, kSubOff, kSubArena, kObjBytes, kObjOff, kObjArena, kMatBytes, kMatOff, kMatArena, kPedBytes, kPedOff, kPedArena, kIntBytes, kIntOff, kIntArena, kSlot, kWindowMax);
    return 0;
}
