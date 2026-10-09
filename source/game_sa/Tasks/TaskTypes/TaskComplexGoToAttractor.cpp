/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToAttractor
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoToAttractor.h"
#include "PedAtmAttractor.h"
#include "PedPlacement.h"
#include "Tasks/TaskComplexSequence.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexGoToAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToAttractor, 0x86FF3C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66B640);
    RH_ScopedInstall(Destructor, 0x66B6A0);

    RH_ScopedVMTInstall(Clone, 0x66D130);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66B6C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670420);
}

// 0x66B640
CTaskComplexGoToAttractor::CTaskComplexGoToAttractor(CPedAttractor* attractor, const CVector& pos, float heading, float attrTime, int32 queueNumber, eMoveState ms) {
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
    // Como el original: el tipo 4 fuerza carrera; la llegada instantanea corrige Z en el destino almacenado.
    const auto moveState = m_Attractor->GetType() == PED_ATTRACTOR_SHELTER ? PEDMOVE_RUN : m_MoveState;
    if (ped->bUseAttractorInstantly) {
        m_vecAttrPosn = CPedPlacement::FindZCoorForPed(m_vecAttrPosn).first;
        ped->GetPosition() = m_vecAttrPosn;
        ped->m_fAimingRotation = m_fAttrHeading;
        ped->m_fCurrentRotation = m_fAttrHeading;
        return new CTaskSimpleStandStill{0, false, false, 8.f};
    }
    auto* sequence = new CTaskComplexSequence;
    sequence->AddTask(new CTaskComplexGoToPointAndStandStill{moveState, m_vecAttrPosn, 0.5f, 2.f, false, false});
    sequence->AddTask(new CTaskSimpleSlideToCoord{m_vecAttrPosn, m_fAttrHeading, 0.5f});
    return sequence;
}

// 0x66B710
CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
