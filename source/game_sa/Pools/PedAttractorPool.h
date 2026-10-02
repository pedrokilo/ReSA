/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedAttractorPool
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
#include "PedAttractor.h"

/**
 * Pool especializado para atractores que coordinan interacciones de peatones.
 *
 * Hereda la administración de slots, handles y reutilización de memoria de CPool;
 * esta clase fija el tipo almacenado y conecta sus operaciones con los hooks del juego.
 * Los objetos obtenidos continúan perteneciendo al pool hasta que se liberan mediante
 * la interfaz correspondiente de CPool.
 */
// Como el original: el pool del EXE (0x5EA9F0 y CPools::Initialise) usa slots de 0xC4 bytes,
// mayores que sizeof(CPedAttractor) (0x8C); con 0x8C la DLL indexaba mal el pool real.
struct CPedAttractorPoolSlot { uint8 data[0xC4]; };

class CPedAttractorPool : public CPool<CPedAttractor, CPedAttractorPoolSlot> {
public:
    /**
     * Registra las operaciones específicas de CPedAttractorPool en sus direcciones originales.
     * La lógica común de administración de memoria permanece centralizada en CPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CPedAttractorPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x5EA9F0);
    }

public:
    using CPool::CPool;

    // 0x5EA9F0
    // Como el original: busca a partir de m_nFirstFree+1 (con una sola vuelta; si el pool está lleno deja
    // m_nFirstFree = 0 y devuelve nullptr), marca el slot como ocupado, incrementa su ID de 7 bits y no
    // toca el almacenamiento del objeto (sin el relleno 0xCD de CPool::New).
    CPedAttractor* New() {
        return NewLegacy();
    }
};
