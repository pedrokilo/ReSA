/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPtrNodeDoubleLinkPool
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
#include "PtrNodeDoubleLink.h"

/**
 * Pool especializado para nodos genéricos de listas doblemente enlazadas.
 *
 * Reutiliza el almacenamiento administrado por CPool para evitar asignaciones
 * dispersas y conservar el modelo de memoria esperado por el juego original.
 */
class CPtrNodeDoubleLinkPool : public CPool<CPtrNodeDoubleLink<void*>> {
public:
    /**
     * Registra las operaciones con dirección propia de CPtrNodeDoubleLinkPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CPtrNodeDoubleLinkPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x05522E0);
    }

public:
    using CPool::CPool;

    // 0x5522E0
    // Como el original: busca a partir de m_nFirstFree+1 (con una sola vuelta; si el pool está lleno deja
    // m_nFirstFree = 0 y devuelve nullptr), marca el slot como ocupado, incrementa su ID de 7 bits y no
    // toca el almacenamiento del objeto (sin el relleno 0xCD de CPool::New).
    CPtrNodeDoubleLink<void*>* New() {
        return NewLegacy();
    }
};
