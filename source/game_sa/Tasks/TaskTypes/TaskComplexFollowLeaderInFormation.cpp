/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFollowLeaderInFormation
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 6.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   Constructor | 0x006949A0 | S
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   GetTaskType | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "SeekEntity/TaskComplexSeekEntity.h"

void CTaskComplexFollowLeaderInFormation::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowLeaderInFormation, 0x870c3c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6949A0);
    RH_ScopedInstall(Destructor, 0x694A40);

    RH_ScopedVMTInstall(Clone, 0x695740);
    RH_ScopedVMTInstall(GetTaskType, 0x694A30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x696820);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6968E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x696940);
}

// 0x6949A0
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* ped, const CVector& posn, float a5) : CTaskComplex() {
    m_Group = pedGroup;
    m_Leader = ped;
    m_Pos = posn;
    m_Int = 4;
    m_Dist = a5;
    CEntity::SafeRegisterRef(m_Leader);
}

// For 0x695740
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(const CTaskComplexFollowLeaderInFormation& o) :
    CTaskComplexFollowLeaderInFormation{
        o.m_Group,
        o.m_Leader,
        o.m_Pos,
        o.m_Dist
    }
{
}

// 0x694A40
CTaskComplexFollowLeaderInFormation::~CTaskComplexFollowLeaderInFormation() {
    CEntity::SafeCleanUpRef(m_Leader);
}

// 0x696820
CTask* CTaskComplexFollowLeaderInFormation::CreateNextSubTask(CPed* ped) {
    const auto move = m_Leader->m_nMoveState;
    switch (m_pSubTask->GetTaskType()) {
    case static_cast<eTaskType>(0xCA): return CreateSubTask(static_cast<eTaskType>(0xCA), ped);
    case static_cast<eTaskType>(700): return CreateFirstSubTask(ped);
    case static_cast<eTaskType>(0x2C0): case static_cast<eTaskType>(0x2C5): case static_cast<eTaskType>(0xCB): return CreateSubTask(static_cast<eTaskType>(0x38B), ped);
    case static_cast<eTaskType>(0x38B): return CreateSubTask(static_cast<eTaskType>(move == 0 || move == 1 ? 0xCB : 0x38B), ped);
    default: return nullptr;
    }
}

// 0x6968E0
CTask* CTaskComplexFollowLeaderInFormation::CreateFirstSubTask(CPed* ped) {
    if (!ped->bInVehicle || !ped->m_pVehicle) return CreateSubTask(static_cast<eTaskType>(0x38B), ped);
    return CreateSubTask(static_cast<eTaskType>(m_Leader->bInVehicle && m_Leader->m_pVehicle == ped->m_pVehicle ? 0x2C5 : 0x2C0), ped);
}

// 0x696940
CTask* CTaskComplexFollowLeaderInFormation::ControlSubTask(CPed* ped) {
    auto* result = m_pSubTask;
    if (!m_Leader) return nullptr;
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x2C5)) return result;
    if (!m_Leader->bInVehicle || !m_Leader->m_pVehicle) {
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(700)) {
            if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
            return CreateFirstSubTask(ped);
        }
        if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCA) && m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCB)) return result;
        const auto d = ped->GetPosition() - m_Leader->GetPosition();
        if (m_Dist*m_Dist <= (d.x*d.x + d.y*d.y) + d.z*d.z) return result;
    } else {
        const auto vehicleRadius = [&]() { return CModelInfo::GetModelInfo(m_Leader->m_pVehicle->m_nModelIndex)->GetColModel()->m_boundSphere.m_fRadius; };
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x38B)) {
            const float radius = vehicleRadius();
            auto* bytes = reinterpret_cast<uint8*>(m_pSubTask);
            auto& maxDist = *reinterpret_cast<float*>(bytes + 0x18);
            if (radius >= maxDist) {
                maxDist = radius;
                *reinterpret_cast<uint32*>(bytes + 0x28) = CTimer::GetTimeInMS();
                *reinterpret_cast<int32*>(bytes + 0x2C) = 0;
                bytes[0x30] = 1;
            }
            if (m_Dist <= 0.f) return result;
            if ((ped->GetPosition() - m_Leader->GetPosition()).SquaredMagnitude() <= m_Dist*m_Dist) return result;
            if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
            CTaskSimpleStandStill stand(0, false, false, 8.f);
            stand.ProcessPed(ped);
            return new CTaskSimplePause(2000);
        }
        if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCB) || CPedGroups::GetPedsGroup(ped)) return result;
        auto* vehicle = m_Leader->m_pVehicle;
        const float radius = vehicleRadius(), radius2 = vehicleRadius();
        const auto distance = (ped->GetPosition() - m_Leader->GetPosition()).SquaredMagnitude();
        if (distance < radius*radius2) {
            if (vehicle->m_nMaxPassengers <= vehicle->m_nNumPassengers || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
            return new CTaskComplexEnterCarAsPassenger(vehicle, 0, false);
        }
        if (m_Dist*m_Dist <= distance || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
    }
    return CreateSubTask(static_cast<eTaskType>(0x38B), ped);
}

CTask* CTaskComplexFollowLeaderInFormation::CreateSubTask(eTaskType type, CPed* ped) {
    switch (type) {
    case static_cast<eTaskType>(0xCB): return new CTaskSimpleStandStill(500, false, false, 8.f);
    case static_cast<eTaskType>(0xCA): return new CTaskSimplePause(2000);
    case static_cast<eTaskType>(700): return new CTaskComplexEnterCarAsPassenger(m_Leader->m_pVehicle, 0, false);
    case static_cast<eTaskType>(0x2C0): return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    case static_cast<eTaskType>(0x2C5): return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
    case static_cast<eTaskType>(0x38B): {
        auto* task = new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>(m_Leader, 50000, 1000, 1.f, 2.f, 2.f, false, true);
        task->GetSeekPosCalculator().SetOffset(m_Pos);
        task->SetMoveState(static_cast<eMoveState>(7));
        return task;
    }
    case static_cast<eTaskType>(0x386): {
        auto& members = m_Group->GetMembership();
        auto* other = members.GetMember(CGeneral::GetRandomIntInRange(0, members.CountMembers()));
        if (other == ped) other = members.GetLeader();
        if (other && CGeneral::GetRandomIntInRange(0, 50) == 20) {
            auto dir = other->GetPosition() - ped->GetPosition();
            dir.Normalise();
            const float angle = CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f);
            auto* storage = CTask::operator new(sizeof(CTaskSimpleAchieveHeading));
            return storage ? ::new (storage) CTaskSimpleAchieveHeading(CGeneral::LimitRadianAngle(angle), .5f, .2f) : nullptr;
        }
        return new CTaskSimpleAchieveHeading(ped->m_fCurrentRotation, .5f, .2f);
    }
    default: return nullptr;
    }
}
