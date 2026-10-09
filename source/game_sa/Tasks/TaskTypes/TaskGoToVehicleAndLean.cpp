/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskGoToVehicleAndLean
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 5.
 * Validación registrada: 5/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   MakeAbortable | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskGoToVehicleAndLean.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskLeanOnVehicle.h"
#include "TaskComplexGangLeader.h"

void CTaskGoToVehicleAndLean::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGoToVehicleAndLean, 0x86FAC8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660E60);
    RH_ScopedInstall(Destructor, 0x660EE0);
    RH_ScopedVMTInstall(Clone, 0x6621B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x664500);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x664590);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x664D40);
    RH_ScopedVMTInstall(ControlSubTask, 0x664E60);
}

// 0x660E60
CTaskGoToVehicleAndLean::CTaskGoToVehicleAndLean(CVehicle* vehicle, int32 leanAnimDurationInMs) {
    m_Vehicle = vehicle;
    m_LeanAnimDurationInMs = leanAnimDurationInMs;
    m_LeanOnVehicle = false;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x660EE0
CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean() {
    if (m_LeanOnVehicle) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_LeanOnVehicle = false;
    }
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x664500
bool CTaskGoToVehicleAndLean::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    const bool aborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (priority != ABORT_PRIORITY_IMMEDIATE && !aborted) return false;
    DoTidyUp(ped);
    return true;
}

// 0x664590
CTask* CTaskGoToVehicleAndLean::CreateNextSubTask(CPed* ped) {
    if (!m_Vehicle) { if (m_pSubTask) m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); return nullptr; }
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x387)) {
        CVector dir = m_Vehicle->GetMatrix().GetRight();
        if (!field_15) dir *= -1.f;
        const auto angle = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f));
        return new CTaskSimpleAchieveHeading(angle, .5f, .2f);
    }
    if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0x386) && m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCB)) { DoTidyUp(ped); return nullptr; }
    return m_LeanOnVehicle ? static_cast<CTask*>(new CTaskLeanOnVehicle(m_Vehicle, m_LeanAnimDurationInMs, 0)) : new CTaskSimpleStandStill(500, false, false, 8.f);
}

// 0x664D40
CTask* CTaskGoToVehicleAndLean::CreateFirstSubTask(CPed* ped) {
    if (!m_Vehicle || m_Vehicle->vehicleFlags.bHasGangLeaningOn) { DoTidyUp(ped); return nullptr; }
    m_Vehicle->vehicleFlags.bHasGangLeaningOn = true;
    m_TargetPos = CalcTargetPos(ped);
    const CVector delta = m_TargetPos - ped->GetPosition();
    if ((delta.z*delta.z + delta.y*delta.y) + delta.x*delta.x < 1.f) return nullptr;
    return new CTaskComplexGoToPointAndStandStill(static_cast<eMoveState>(4), m_TargetPos, .05f, 2.f, false, true);
}

// 0x664E60
CTask* CTaskGoToVehicleAndLean::ControlSubTask(CPed* ped) {
    if (!m_Vehicle) { if (m_pSubTask) m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr); return nullptr; }
    if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) { DoTidyUp(ped); return nullptr; }
    const auto speed = m_Vehicle->GetSpeed(ped->GetPosition() - m_Vehicle->GetPosition());
    if ((m_Vehicle->m_pDriver || std::sqrt((speed.z*speed.z + speed.y*speed.y) + speed.x*speed.x) > .01f) && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { DoTidyUp(ped); return nullptr; }
    const auto delta = CalcTargetPos(ped) - m_TargetPos;
    if ((delta.x*delta.x + delta.y*delta.y) + delta.z*delta.z > std::bit_cast<float>(uint32{0x3C23D70B}) && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { DoTidyUp(ped); return nullptr; }
    if (!m_LeanOnVehicle) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto index = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[index].IsLoaded != 1) CStreaming::RequestModel(IFPToModelId(index), STREAMING_KEEP_IN_MEMORY);
            else { CAnimManager::AddAnimBlockRef(index); m_LeanOnVehicle = true; }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_LeanOnVehicle = false;
    }
    return m_pSubTask;
}

CVector CTaskGoToVehicleAndLean::CalcTargetPos(CPed* ped) {
    const auto& box = CModelInfo::GetModelInfo(m_Vehicle->m_nModelIndex)->GetColModel()->m_boundBox;
    const auto delta = ped->GetPosition() - m_Vehicle->GetPosition();
    const auto right = m_Vehicle->GetRight();
    const float dot = (delta.x*right.x + delta.y*right.y) + delta.z*right.z;
    field_15 = dot > 0.f;
    const CVector local{field_15 ? box.m_vecMax.x + .5f : box.m_vecMin.x - .5f, 0.f, 0.f};
    return m_Vehicle->GetMatrix().TransformPoint(local);
}

// 0x660F60
void CTaskGoToVehicleAndLean::DoTidyUp(CPed* ped) {
    if (m_Vehicle)
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;

    if (m_pSubTask)
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
}
