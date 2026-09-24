/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexAttractorPartnerWait
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexAttractorPartnerWait.h"
#include "Scripted2dEffects.h"

void CTaskComplexAttractorPartnerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexAttractorPartnerWait, 0x86e284, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x636CF0);
    RH_ScopedVMTInstall(GetTaskType, 0x633280);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6332A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638F20);
    RH_ScopedVMTInstall(ControlSubTask, 0x6332B0);
}

// 0x633250
CTaskComplexAttractorPartnerWait::CTaskComplexAttractorPartnerWait(bool isLeader, const CScriptedEffectPair& pair) :
    m_IsLeader{ isLeader },
    m_CurrentFxPair{ &pair }
{
}

// 0x636CF0
CTaskComplexAttractorPartnerWait::CTaskComplexAttractorPartnerWait(const CTaskComplexAttractorPartnerWait& o) :
    CTaskComplexAttractorPartnerWait{ o.m_IsLeader, *o.m_CurrentFxPair }
{
}

// 0x638F20
CTask* CTaskComplexAttractorPartnerWait::CreateFirstSubTask(CPed* ped) {
    return new CTaskComplexUseSequence{ m_CurrentFxPair->Effects[m_IsLeader ? 0 : 1].WaitingTask };
}
