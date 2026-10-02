/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocatorPool
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma once

#include "Pool.h"
#include "Allocators/TaskAllocator.h"

/**
 * Pool especializado para allocators de tareas, reservando espacio para sus variantes derivadas.
 *
 * Reutiliza el almacenamiento administrado por CPool para evitar asignaciones
 * dispersas y conservar el modelo de memoria esperado por el juego original.
 */
class CTaskAllocatorPool : public CPool<CTaskAllocator, char[32]> { // char[32] == CTaskAllocatorPlayerCommandAttack
public:
    /**
     * Registra las operaciones con dirección propia de CTaskAllocatorPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CTaskAllocatorPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0069D8E0);
    }

public:
    using CPool::CPool;

    // 0x69D8E0
    // Como el original: busca a partir de m_nFirstFree+1 (con una sola vuelta; si el pool está lleno deja
    // m_nFirstFree = 0 y devuelve nullptr), marca el slot como ocupado, incrementa su ID de 7 bits y no
    // toca el almacenamiento del objeto (sin el relleno 0xCD de CPool::New).
    CTaskAllocator* New() {
        return NewLegacy();
    }
};
