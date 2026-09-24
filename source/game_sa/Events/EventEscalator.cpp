/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventEscalator
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventEscalator.h"


void CEventEscalator::InjectHooks() {
    RH_ScopedVirtualClass(CEventEscalator, 0x86CD00, 16);
    RH_ScopedCategory("Events");

    RH_ScopedVMTInstall(AffectsPed, 0x4B2580);
}

// 0x4B2580
bool CEventEscalator::AffectsPed(CPed* ped) {
    if (ped->IsAlive() && !ped->IsPlayer() && ped->m_pContactEntity) {
        auto modelId = ped->m_pContactEntity->m_nModelIndex;
        if (modelId == ModelIndices::MI_ESCALATORSTEP || modelId == ModelIndices::MI_ESCALATORSTEP8)
            return true;
    }
    return false;
}