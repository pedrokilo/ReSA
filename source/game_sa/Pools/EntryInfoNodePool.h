/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntryInfoNodePool
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
#include "EntryInfoNode.h"

/**
 * Pool especializado para CEntryInfoNode que enlazan entidades con las estructuras espaciales.
 *
 * Reutiliza el almacenamiento administrado por CPool para evitar asignaciones
 * dispersas y conservar el modelo de memoria esperado por el juego original.
 */
class CEntryInfoNodePool : public CPool<CEntryInfoNode> {
public:
    /**
     * Registra las operaciones con dirección propia de CEntryInfoNodePool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CEntryInfoNodePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x00536D10);
    }

public:
    using CPool::CPool;

    // Conserva la interfaz esperada por CEntryInfoNode delegando en el allocator
    // legacy del pool; el objeto sigue siendo propiedad del propio pool.
    CEntryInfoNode* New() {
        return NewLegacy();
    }

    // Devuelve el slot al pool sin liberar memoria mediante el heap general.
    void Delete(CEntryInfoNode* node) { DeleteLegacy(node); }
};
