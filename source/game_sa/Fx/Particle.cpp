#include "StdInc.h"

#include "Particle.h"

/**
 * Registra el constructor de Particle_c (0x4A9110, lote 144A, banco Fn144A). La struct es solo cabecera salvo este archivo; el constructor del EXE solo anula los enlaces de ListItem_c.
 */
void Particle_c::InjectHooks() {
    RH_ScopedClass(Particle_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A9110);
}
