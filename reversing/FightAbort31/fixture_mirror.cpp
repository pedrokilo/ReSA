// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. Las direcciones de las arenas (anim_ptr / idle_ptr) son ficticias: al oraculo solo le importan
// los bytes que cambian al poner a cero el puntero, y esos bytes no afectan a la cobertura.
#include <cstdio>
#include "Fixtures.hpp"
using namespace FightAbort31Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        const u32 animPtr = c.hasAnim ? 0x00A1B2C0u + n * 0x100u : 0u, idlePtr = c.hasIdle ? 0x00B3C4D0u + n * 0x100u : 0u;
        std::printf("{\"family\":%u,\"prio\":%u,\"has_event\":%u,\"ev_type\":%u,\"has_anim\":%u,\"has_idle\":%u,\"ped_kind\":%u,\"anim_delta\":%u,\"idle_weight\":%u,\"idle_delta\":%u,"
                    "\"pd_x\":%u,\"pd_y\":%u,\"finished0\":%u,\"next0\":%u,\"ret\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"anim_ptr\":%u,\"idle_ptr\":%u,\"ped_clump\":%u,\"ped_group\":%u,"
                    "\"this_off\":%u,\"slot\":%u}\n",
                    c.family, c.prio, c.hasEvent, c.evType, c.hasAnim, c.hasIdle, c.pedKind, c.animDelta, c.idleWeight, c.idleDelta, c.pdX, c.pdY, c.finished0, c.next0, c.ret, c.eax, c.edx,
                    c.stack, animPtr, idlePtr, Rand(n, 60), Rand(n, 61), kThisOff, kGetEventPrioritySlot);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"anim_bytes\":%u,\"anim_off\":%u,\"anim_arena\":%u,\"ped_bytes\":%u,\"ped_off\":%u,\"ped_arena\":%u,"
                "\"pd_bytes\":%u,\"pd_off\":%u,\"pd_arena\":%u,\"ev_bytes\":%u,\"ev_off\":%u,\"ev_arena\":%u,\"slots\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kAnimBytes, kAnimOff, kAnimArena, kPedBytes, kPedOff, kPedArena, kPdBytes, kPdOff, kPdArena, kEvBytes, kEvOff, kEvArena, kSlots);
    return 0;
}
