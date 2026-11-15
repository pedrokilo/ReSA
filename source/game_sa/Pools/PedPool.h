/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedPool
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma once

#include "Pool.h"
#include "CopPed.h"

/**
 * Pool especializado para peatones, incluyendo las variantes derivadas que requieren mayor tamaño.
 *
 * Hereda de CPool el almacenamiento fijo, la reutilización de slots y el sistema
 * de referencias/índices del juego. La clase concreta sólo fija el tipo máximo
 * almacenado y registra las entradas que tienen dirección propia en el ejecutable.
 */
class CPedPool : public CPool<CPed, CCopPed> {
public:
    /**
     * Registra los hooks propios de CPedPool; las operaciones genéricas del pool
     * continúan implementadas por CPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CPedPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(GetRef, 0x4442D0);
        RH_ScopedInstall(New, 0x005E45E0);
        RH_ScopedInstall(GetAtRef, 0x404910);
        // Lote 189A (sin validar; el listado decia CStreaming): CPool<CPed, CCopPed>::GetAt (thiscall, un argumento de pila, `ret 4`, 31 bytes; 6 llamadores; el elemento de 0x7C4 bytes y el ECX = 0xB74490 comprobado en un llamador, 0x46DF01, son los del pool de peatones).
        // Nulo si el bit 7 del BYTE de estado de la ranura esta puesto (libre); si no, almacenamiento + indice * 0x7C4. El EXE no comprueba el limite del indice; la plantilla si (assert).
        RH_ScopedInstall(GetAt, 0x4082A0);
        // Lote 199A (sin validar; el listado decia CPed): CPool<CPed, CCopPed>::CreateAtRef (thiscall, un argumento de pila, `ret 4`, 74 bytes; lo llama CPed::operator new(size, ref) 0x5E4730, que ya calcula el almacenamiento).
        // Mismo cuerpo que CPool<CObject>::CreateAtRef (0x59F610) y CPool<CVehicle, CHeli>::CreateAtRef (0x6D5DD0): ranura = ref >> 8 (con signo); borra el bit 0x80 del estado y deja los 7 bits bajos iguales a los de la
        // referencia; cursor (+0xC) = 0 y avanza mientras el estado de la ranura tenga el bit 0x80 apagado. No comprueba que la ranura estuviera libre ni que este en rango (la plantilla si, con assert). EAX queda con basura.
        RH_ScopedInstall(CreateAtRef, 0x5E0540);
    }

public:
    using CPool::CPool;

    // 0x4442D0: el EXE deja ECX = tabla de banderas y EDX = indice << 8; GetRef devuelve EDX:EAX con ResidueRegsCpp
    uint32 GetRef(const CPed*);

    // Como el original: busca desde el cursor + 1 y no rellena el almacenamiento (ver CPool::NewLegacy).
    CPed* New() {
        return NewLegacy();
    }
};
