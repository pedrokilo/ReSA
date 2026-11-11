#include "StdInc.h"
#include "Furniture_c.h"

void Furniture_c::InjectHooks() {
    RH_ScopedClass(Furniture_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(Constructor, 0x590EC0); // lote 165A (banco Fn165A): `push esi; mov esi,ecx; call 0x4A8DB0 (enlaces anterior y siguiente a cero); mov eax,esi; pop esi; ret`; no toca el resto
}
