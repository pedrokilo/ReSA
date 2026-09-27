/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehiclePool
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
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

        RH_ScopedInstall(New, 0x006E2A50);
    }

public:
    using CPool::CPool;
};
