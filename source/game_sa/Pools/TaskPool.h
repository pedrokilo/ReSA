/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskPool
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma once

#include "Pool.h"
#include "TaskSimpleSlideToCoord.h"

/**
 * Pool especializado para tareas de IA, reservando tamaño suficiente para sus variantes derivadas.
 *
 * Hereda la administración de slots, handles y reutilización de memoria de CPool;
 * esta clase fija el tipo almacenado y conecta sus operaciones con los hooks del juego.
 * Los objetos obtenidos continúan perteneciendo al pool hasta que se liberan mediante
 * la interfaz correspondiente de CPool.
 */
class CTaskPool : public CPool<CTask, CTaskSimpleSlideToCoord> {
public:
    /**
     * Registra las operaciones específicas de CTaskPool en sus direcciones originales.
     * La lógica común de administración de memoria permanece centralizada en CPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CTaskPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x61A500);
    }

public:
    using CPool::CPool;
};
