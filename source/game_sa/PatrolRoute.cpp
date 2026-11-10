#include "StdInc.h"
#include "PatrolRoute.h"
#include "Pools/PoolRawDelete.h"

void CPatrolRoute::InjectHooks() {
    RH_ScopedClass(CPatrolRoute);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x41B810);    // lote 146A (banco Fn146A)
    RH_ScopedInstall(operator delete, 0x41B820); // lote 146A (banco Fn146A)
}

// 0x41B810 (lote 146A, banco Fn146A): `mov ecx,[0xB744B4]; jmp 0x41B660` (cdecl con un argumento de pila que no se lee; `ret`). Toma una ranura del pool de rutas de patrulla con CPatrolRoutePool::New
// (0x41B660, con hook); devuelve nulo si esta lleno.
void* CPatrolRoute::operator new(size_t size) {
    return GetPatrolRoutePool()->New();
}

// 0x41B820 (lote 146A, banco Fn146A): cdecl con un argumento de pila, `ret`, 51 bytes. Libera la ranura de `ptr` en el pool de rutas de patrulla (0xB744B4) en linea, como el EXE: indice =
// (ptr - almacenamiento) / 0x1A4 (division con signo truncada hacia cero), estados[indice] |= 0x80 y, si indice < cursor (con signo), cursor = indice.
void CPatrolRoute::operator delete(void* ptr) {
    PoolFreeSlotRaw(GetPatrolRoutePool(), ptr, sizeof(CPatrolRoute));
}
