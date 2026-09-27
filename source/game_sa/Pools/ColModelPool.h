/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColModelPool
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
#include "ColModel.h"

/**
 * Pool especializado para modelos de colisión reutilizados por entidades y model infos.
 *
 * Hereda de CPool el almacenamiento fijo, la reutilización de slots y el sistema
 * de referencias/índices del juego. La clase concreta sólo fija el tipo máximo
 * almacenado y registra las entradas que tienen dirección propia en el ejecutable.
 */
class CColModelPool : public CPool<CColModel> {
public:
    /**
     * Registra los hooks propios de CColModelPool; las operaciones genéricas del pool
     * continúan implementadas por CPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CColModelPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0040FB80);
    }

public:
    using CPool::CPool;
};
