/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderStandard
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderStandard.h"
#include "EventSexyVehicle.h"
#include "EventChatPartner.h"
#include "EventAcquaintancePedDislike.h"

void CTaskComplexWanderStandard::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderStandard, 0x85A200, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E4F0);
    RH_ScopedInstall(WillChat, 0x66AED0);
    RH_ScopedInstall(SetNextMinScanTime, 0x66AF60);
    RH_ScopedInstall(LookForSexyCars, 0x66AFD0);
    RH_ScopedInstall(LookForChatPartners, 0x66FDA0);
    RH_ScopedInstall(LookForGangMembers, 0x670100);

    // El original (0x48E530) pasa también m_bWanderSensibly; el Clone inline usa el valor por defecto.
    RH_ScopedVMTInstall(Clone, 0x48E530);
    RH_ScopedVMTInstall(GetWanderType, 0x48E5D0);
    RH_ScopedVMTInstall(ScanForStuff, 0x672600);
}

// 0x48E4F0
CTaskComplexWanderStandard::CTaskComplexWanderStandard(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWander(MoveState, Dir, bWanderSensibly),
    m_nMinNextScanTime{ 0 }
{
}

// 0x672600
void CTaskComplexWanderStandard::ScanForStuff(CPed* ped) {
    if (!m_TaskTimer.m_bStarted) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval = 50;
        m_TaskTimer.m_bStarted = true;
    }

    if (CTimer::GetTimeInMS() < m_nMinNextScanTime)
        return;

    if (m_TaskTimer.m_bStarted) { // V547 Expression 'm_TaskTimer.m_bStarted' is always true.
        if (m_TaskTimer.m_bStopped) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStopped = false;
        }

        if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + m_TaskTimer.m_nInterval) {
            m_TaskTimer.m_nInterval = 50;
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStarted = true;
            if (!LookForGangMembers(ped) && !LookForChatPartners(ped)) {
                CTaskComplexWanderStandard::LookForSexyCars(ped);
            }
        }
    }
}

// 0x66AED0
bool __stdcall CTaskComplexWanderStandard::WillChat(const CPed& first, const CPed& second) {
    if (first.m_nPedType == static_cast<ePedType>(20) || second.m_nPedType == static_cast<ePedType>(20) || first.m_nPedType == PED_TYPE_COP || second.m_nPedType == PED_TYPE_COP) {
        return false;
    }
    if (first.IsPlayer() || second.IsPlayer() || IsPedTypeGang(first.m_nPedType) || IsPedTypeGang(second.m_nPedType)) {
        return false;
    }
    return CPedIntelligence::AreFriends(first, second);
}

// 0x66AF60
void CTaskComplexWanderStandard::SetNextMinScanTime(CPed* ped) {
    auto* task = ped->GetTaskManager().GetActiveTask();
    if (task && task->GetTaskType() == static_cast<CTask*>(this)->GetTaskType()) {
        auto* wander = static_cast<CTaskComplexWander*>(task);
        if (wander->GetWanderType() == static_cast<CTaskComplexWander*>(this)->GetWanderType()) {
            static_cast<CTaskComplexWanderStandard*>(ped->GetTaskManager().GetActiveTask())->m_nMinNextScanTime = CTimer::GetTimeInMS() + 100000;
        }
    }
}

// 0x66AFD0
bool CTaskComplexWanderStandard::LookForSexyCars(CPed* ped) {
    const auto& pos = ped->GetPosition();
    for (auto* entity : ped->GetIntelligence()->GetVehicleScanner().m_apEntities) {
        auto* vehicle = static_cast<CVehicle*>(entity);
        if (!vehicle || vehicle == ped->m_pVehicle || vehicle->m_pHandlingData->m_nMonetaryValue <= 40000 || !(vehicle->m_fHealth > 500.0f)) {
            continue;
        }
        const auto& target = vehicle->GetPosition();
        const float x = target.x - pos.x, y = target.y - pos.y, z = target.z - pos.z;
        if (!((z*z + y*y) + x*x < 25.0f)) {
            continue;
        }
        const auto& forward = ped->GetForward();
        if (!((z*forward.z + y*forward.y) + x*forward.x > 0.0f)) {
            continue;
        }
        if (CWorld::GetIsLineOfSightClear(pos, target, true, false, false, true, false, false, false)) {
            CEventSexyVehicle event(vehicle);
            ped->GetIntelligence()->GetEventGroup().Add(&event, false);
            SetNextMinScanTime(ped);
            return true;
        }
    }
    return false;
}

// 0x66FDA0
bool CTaskComplexWanderStandard::LookForChatPartners(CPed* ped) {
    if (!g_surfaceInfos.IsPavement(ped->m_nContactSurface) || int32(m_nMoveState) > 4) {
        return false;
    }
    auto* player = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
    if (player->bInVehicle && player->m_pVehicle) {
        const auto& speed = player->m_pVehicle->m_vecMoveSpeed;
        if ((speed.x*speed.x + speed.y*speed.y) + speed.z*speed.z > std::bit_cast<float>(0x3D23D70Bu)) {
            return false;
        }
    }
    if (CStreaming::IsVeryBusy() || CGameLogic::LaRiotsActiveHere()) {
        return false;
    }
    const auto& pos = ped->GetPosition();
    auto* intel = ped->GetIntelligence();
    for (auto* entity : intel->GetPedScanner().m_apEntities) {
        auto* other = static_cast<CPed*>(entity);
        if (!other || !g_surfaceInfos.IsPavement(other->m_nContactSurface)) {
            continue;
        }
        auto* otherIntel = other->GetIntelligence();
        if (!otherIntel->GetTaskManager().GetActiveTask() || otherIntel->GetTaskManager().GetActiveTask()->GetTaskType() != static_cast<CTask*>(this)->GetTaskType()) {
            continue;
        }
        if (intel->FindTaskByType(static_cast<eTaskType>(0x4B4)) || otherIntel->FindTaskByType(static_cast<eTaskType>(0x4B4)) || intel->GetEventGroup().GetEventOfType(static_cast<eEventType>(0x13)) || otherIntel->GetEventGroup().GetEventOfType(static_cast<eEventType>(0x13)) || intel->FindTaskByType(static_cast<eTaskType>(0x4BF)) || otherIntel->FindTaskByType(static_cast<eTaskType>(0x4BF)) || !WillChat(*ped, *other)) {
            continue;
        }
        const auto& target = other->GetPosition();
        const CVector delta{target.x-pos.x, target.y-pos.y, target.z-pos.z};
        if (!(delta.SquaredMagnitude() < 100.0f)) {
            continue;
        }
        const auto& ourForward = ped->GetForward();
        const auto& theirForward = other->GetForward();
        const auto dot = [&](const CVector& f){return (delta.x*f.x + delta.z*f.z) + delta.y*f.y;};
        if (!(dot(ourForward) > 0.0f) || !(dot(theirForward) < 0.0f) || !CWorld::GetIsLineOfSightClear(pos, target, true, false, false, true, false, false, false)) {
            continue;
        }
        CEventChatPartner ours(true, other);
        intel->GetEventGroup().Add(&ours, false);
        CEventChatPartner theirs(false, ped);
        otherIntel->GetEventGroup().Add(&theirs, false);
        SetNextMinScanTime(ped);
        SetNextMinScanTime(other);
        return true;
    }
    return false;
}

// 0x670100
bool CTaskComplexWanderStandard::LookForGangMembers(CPed* ped) {
    if (CPedGroups::GetPedsGroup(ped)) {
        return false;
    }
    CVector nodePos;
    ComputeTargetPos(ped, nodePos, m_NextNode);
    const auto& pos = ped->GetPosition();
    const CVector ourDelta{pos.x-nodePos.x, pos.y-nodePos.y, pos.z-nodePos.z};
    for (auto* entity : ped->GetIntelligence()->GetPedScanner().m_apEntities) {
        auto* other = static_cast<CPed*>(entity);
        if (!other) {
            continue;
        }
        const auto& target = other->GetPosition();
        const CVector delta{target.x-nodePos.x, target.y-nodePos.y, target.z-nodePos.z};
        if (!((ourDelta.x*delta.x + ourDelta.y*delta.y) + ourDelta.z*delta.z > 0.0f) || !((delta.x*delta.x + delta.y*delta.y) + delta.z*delta.z < 25.0f)) {
            continue;
        }
        if (auto* group = CPedGroups::GetPedsGroup(other); group && group->GetMembership().CountMembersExcludingLeader() > 0) {
            CEventAcquaintancePedDislike event(other, static_cast<eTaskType>(0x3AC));
            ped->GetIntelligence()->GetEventGroup().Add(&event, false);
            SetNextMinScanTime(ped);
            return true;
        }
    }
    return false;
}
