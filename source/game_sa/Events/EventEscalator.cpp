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
 * Validación registrada: 1/1 entradas.
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
        // Como el original: el modelo de la entidad se extiende con signo (movsx) y los MI_ sin signo (movzx)
        const int32 modelId = (int16)ped->m_pContactEntity->m_nModelIndex;
        if (modelId == (int32)(eModelID)ModelIndices::MI_ESCALATORSTEP || modelId == (int32)(eModelID)ModelIndices::MI_ESCALATORSTEP8)
            return true;
    }
    return false;
}