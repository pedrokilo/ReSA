/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehiclePool
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma once

#include "Pool.h"
#include "Heli.h"

/**
 * Pool especializado para vehículos y sus variantes derivadas, usando CHeli como tipo de tamaño máximo.
 *
 * CPool conserva la propiedad del almacenamiento y permite reutilizar slots con
 * direcciones estables, algo importante para referencias y handles del motor.
 */
class CVehiclePool : public CPool<CVehicle, CHeli> {
public:
    /**
     * Registra las operaciones propias de CVehiclePool; la gestión común permanece en CPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CVehiclePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(GetAt, 0x0041CC10); // CPool<CVehicle, CHeli>::GetAt: 0 si el bit 7 del estado de la ranura esta puesto, si no almacenamiento + indice * 0xA18
        RH_ScopedInstall(New, 0x006E2A50);
        // Lote 185A (sin validar; el listado decia CObjectPool): CPool<CVehicle, CHeli>::GetAtRef (thiscall, un argumento de pila, `ret 4`, 41 bytes; 297 llamadores). Mismo cuerpo que CPedPool::GetAtRef (0x404910):
        // ranura = ref >> 8 (con signo); si el BYTE de estado de esa ranura es igual al byte bajo de la referencia (incluye el bit 0x80 de libre) devuelve almacenamiento + ranura * 0xA18, si no nulo.
        // El EXE no comprueba el limite de la ranura; la plantilla si (referencias fuera de rango dan nulo).
        RH_ScopedInstall(GetAtRef, 0x004048E0);
    }

public:
    using CPool::CPool;

    // 0x6E2A50
    // Como el original: busca a partir de m_nFirstFree+1 (con una sola vuelta; si el pool está lleno deja
    // m_nFirstFree = 0 y devuelve nullptr), marca el slot como ocupado, incrementa su ID de 7 bits y no
    // toca el almacenamiento del objeto (sin el relleno 0xCD de CPool::New).
    CVehicle* New() {
        return NewLegacy();
    }
};
