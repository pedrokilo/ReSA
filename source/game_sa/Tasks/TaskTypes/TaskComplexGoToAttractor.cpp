/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToAttractor
 * Reconstrucción: INCOMPLETA — 2/5 funciones reconstruidas.
 * Pendientes de reconstruir: 3.
 * Funciones bloqueadas: 3.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoToAttractor.h"
#include "PedAtmAttractor.h"

void CTaskComplexGoToAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToAttractor, 0x86FF3C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66B640);
    RH_ScopedInstall(Destructor, 0x66B6A0);

    RH_ScopedVMTInstall(Clone, 0x66D130, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66B6C0, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670420, { .reversed = false });
}

// 0x66B640
CTaskComplexGoToAttractor::CTaskComplexGoToAttractor(CPedAttractor* attractor, const CVector& pos, float heading, float attrTime, int32 queueNumber, eMoveState ms) : CTaskComplex() {
    m_Attractor = attractor;
    m_vecAttrPosn = pos;
    m_fAttrHeading = heading;
    m_MoveState = ms;
    m_fAttrTime = attrTime;
    m_nQueueNumber = queueNumber;
}

// 0x66B6B0
bool CTaskComplexGoToAttractor::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x66B6C0
CTask* CTaskComplexGoToAttractor::CreateNextSubTask(CPed* ped) {
    GetPedAttractorManager()->BroadcastArrival(ped, m_Attractor);
    if (ped->bUseAttractorInstantly && m_Attractor->GetHeadOfQueue() != ped) {
        ped->bUseAttractorInstantly = false;
    }
    return nullptr;
}

// 0x670420
CTask* CTaskComplexGoToAttractor::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x670420, CTaskComplexGoToAttractor*, CPed*>(this, ped);
}

// 0x66B710
CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
