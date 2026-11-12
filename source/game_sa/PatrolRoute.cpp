/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPatrolRoute
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "PatrolRoute.h"
#include "Pools/PoolRawDelete.h"

void CPatrolRoute::InjectHooks() {
    RH_ScopedClass(CPatrolRoute);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x41B810);    // lote 146A (banco Fn146A)
    RH_ScopedInstall(operator delete, 0x41B820); // lote 146A (banco Fn146A)
    RH_ScopedInstall(Destructor, 0x6677A0);      // lote 170A (banco Fn170A)
    RH_ScopedInstall(Constructor, 0x66D440);     // lote 175A (banco Fn175A)
    RH_ScopedInstall(Assign, 0x66D4B0);          // lote 175A (banco Fn175A)
}

// 0x6677A0 (lote 170A, banco Fn170A; el listado lo atribuia a CTaskSimpleRunNamedAnim): cuerpo del destructor de CPatrolRoute del EXE (thiscall, sin argumentos de pila, `ret`, 19 bytes: `push 0x46A380; push 8; push 0x28;
// add ecx,4; push ecx; call 0x821E02`, `eh vector destructor iterator` sobre las 8 entradas de 0x28 bytes a partir de this+4 con el destructor de cada una, 0x46A380, que es un solo `ret`). Lo llama el destructor con borrado
// de la ruta (0x66BA70). Como los destructores de las entradas no hacen nada, destruir las ocho no escribe memoria: la funcion no hace nada observable.
void CPatrolRoute::Destructor() {
}

// 0x66D440 (lote 175A, banco Fn175A; el listado lo atribuia a CTaskComplexInvestigateDisturbance): constructor de CPatrolRoute del EXE (thiscall, sin argumentos de pila, `ret`, 112 bytes). Escribe el contador (+0) a cero,
// construye las ocho entradas de 0x28 bytes a partir de this+4 con `eh vector constructor iterator` (0x821D30, con el constructor de entrada 0x46AE30: `mov byte [ecx],0; mov byte [ecx+18h],0`) y despues repite en linea
// los mismos 16 bytes a cero (el primer byte de cada uno de los dos nombres de cada entrada: +4+0x28*i y +0x1C+0x28*i). Devuelve `this`. El efecto neto sobre la memoria es contador y esos 16 bytes a cero; el resto de la
// ruta (el segundo..ultimo byte de cada nombre y los ocho vectores desde +0x144) no se toca. El marco SEH del vector solo desenrolla y se omite.
CPatrolRoute* CPatrolRoute::Constructor() {
    *reinterpret_cast<int32*>(m_data) = 0;
    for (int32 i = 0; i < 8; i++) {
        m_data[4 + 0x28 * i]        = 0;
        m_data[4 + 0x28 * i + 0x18] = 0;
    }
    return this;
}

// 0x66D4B0 (lote 175A, banco Fn175A; el listado lo atribuia a CTaskComplexInvestigateDisturbance): copia de una ruta de patrulla (thiscall, un argumento de pila, `ret 4`, 155 bytes, sin valor de retorno util). Como el
// original: copia el contador (+0) de la ruta origen; si es <= 0 (con signo) no hace mas; si no, por cada indice i (empezando en 0) copia el vector de 12 bytes de +0x144+12*i (tres dwords, cada lectura seguida de su
// escritura) y los dos nombres de la entrada (+4+0x28*i y +0x1C+0x28*i), cada uno como cadena C COMPLETA hasta su NUL incluido (byte a byte, sin limite de 0x18: un nombre sin NUL entra en lo que sigue), y repite mientras
// i < contador RELEIDO de ESTA ruta (+0). El contador no esta acotado por 8: con mas de 8 entradas lee y escribe mas alla de las ocho entradas.
void CPatrolRoute::Assign(const CPatrolRoute* src) {
    const uint8* const from = src->m_data;
    uint8* const       to   = m_data;
    const int32        n    = *reinterpret_cast<const int32*>(from);
    *reinterpret_cast<int32*>(to) = n;
    if (n <= 0) {
        return;
    }
    int32 i = 0;
    do {
        const uint32* const vs = reinterpret_cast<const uint32*>(from + 0x144 + 12 * i);
        uint32* const       vd = reinterpret_cast<uint32*>(to + 0x144 + 12 * i);
        vd[0] = vs[0];
        vd[1] = vs[1];
        vd[2] = vs[2];
        for (const int32 nameOffset : {4, 0x1C}) {
            const uint8* p = from + nameOffset + 0x28 * i;
            uint8*       q = to + nameOffset + 0x28 * i;
            uint8        b;
            do {
                b    = *p++;
                *q++ = b;
            } while (b);
        }
        i++;
    } while (i < *reinterpret_cast<const int32*>(to));
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
