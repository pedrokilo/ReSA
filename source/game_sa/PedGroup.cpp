/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroup
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedGroup.h"
#include <TaskSimpleCarSetPedOut.h>
#include <TaskComplexFollowLeaderInFormation.h>
#include <TaskComplexEnterCar.h>
#include <TaskSimpleGoToPoint.h>
#include "PedList.h"
#include "Formation.h"

// 0x5FC150
// Como el original: solo escribe los campos que el EXE escribe (los miembros no se construyen por defecto).
CPedGroup::CPedGroup() {
    new (&m_groupMembership) CPedGroupMembership(*this); // Escribe el grupo, los 8 huecos a cero y la separacion (60.0, 0x86C6BC)
    m_groupIntelligence.Constructor();                   // 0x5F7250 (no escribe los tres primeros punteros)
    m_groupIntelligence.m_pPedGroup = this;
    m_bIsMissionGroup = false;
    m_pPed = nullptr;
    m_bMembersEnterLeadersVehicle = true;
}

// 0x5FC190
// Como el original: primero la inteligencia (0x5F7350) y luego quita todos los miembros.
CPedGroup::~CPedGroup() {
    m_groupIntelligence.Flush();
    m_groupMembership.Flush();
}

//! @addr 0x5FB010
//! @returns Distance of the furthers member from the leader
float CPedGroup::FindDistanceToFurthestMember() {
    float maxDist = 0.0f;
    CPed* const leader = m_groupMembership.GetMember(CPedGroupMembership::LEADER_MEM_ID);
    if (!leader) {
        return maxDist;
    }
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        CPed* const mem = m_groupMembership.GetMember(i);
        if (!mem) {
            continue;
        }
        // Como el original: raiz de ((dz^2 + dx^2) + dy^2) con la posicion del miembro menos la del lider
        const CVector& leaderPos = leader->GetPosition();
        const CVector& memPos = mem->GetPosition();
        const float dx = memPos.x - leaderPos.x;
        const float dy = memPos.y - leaderPos.y;
        const float dz = memPos.z - leaderPos.z;
        const float dist = std::sqrt((dz * dz + dx * dx) + dy * dy);
        if (!(maxDist > dist)) {
            maxDist = dist;
        }
    }
    return maxDist;
}

// 0x5FB0A0
float CPedGroup::FindDistanceToNearestMember(CPed** ppOutNearestMember) {
    float minDist = 1.0e10f; // 0x86C84C
    CPed* const leader = m_groupMembership.GetMember(CPedGroupMembership::LEADER_MEM_ID);
    if (!leader) {
        return minDist;
    }
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        CPed* const mem = m_groupMembership.GetMember(i);
        if (!mem) {
            continue;
        }
        const CVector& leaderPos = leader->GetPosition();
        const CVector& memPos = mem->GetPosition();
        const float dx = memPos.x - leaderPos.x;
        const float dy = memPos.y - leaderPos.y;
        const float dz = memPos.z - leaderPos.z;
        const float dist = std::sqrt((dz * dz + dx * dx) + dy * dy);
        if (!(minDist < dist)) {
            minDist = dist;
        }
        // Como el original: el puntero de salida se escribe con cada seguidor (queda el ultimo, no el mas cercano)
        if (ppOutNearestMember) {
            *ppOutNearestMember = mem;
        }
    }
    return minDist;
}

// 0x5FACD0
CPed* CPedGroup::GetClosestGroupPed(CPed* ped, float* pOutDistSq) {
    float best = 1.0e8f; // 0x86C848
    CPed* closest = nullptr;
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        CPed* const mem = m_groupMembership.GetMember(i);
        if (!mem || mem == ped) {
            continue;
        }
        // Como el original: ((dx^2 + dz^2) + dy^2) con la posicion de ped menos la del miembro
        const CVector& memPos = mem->GetPosition();
        const CVector& pedPos = ped->GetPosition();
        const float dx = pedPos.x - memPos.x;
        const float dy = pedPos.y - memPos.y;
        const float dz = pedPos.z - memPos.z;
        const float distSq = (dx * dx + dz * dz) + dy * dy;
        if (distSq < best) {
            best = distSq;
            closest = mem;
        }
    }
    // Como el original: la distancia se escribe siempre que haya puntero (1e8 si no hay nadie)
    if (pOutDistSq) {
        *pOutDistSq = best;
    }
    return closest;
}

// 0x5FB790
void CPedGroup::Flush() {
    m_groupMembership.Flush();
    m_groupIntelligence.Flush();
    m_bIsMissionGroup = false;
}

// 0x5F7DB0
bool CPedGroup::IsAnyoneUsingCar(const CVehicle* vehicle) {
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        CPed* const mem = m_groupMembership.GetMember(i);
        if (!mem) {
            continue;
        }
        if (mem->m_pVehicle && mem->bInVehicle && mem->m_pVehicle == vehicle) {
            return true;
        }
        // Como el original: se comprueban las dos tareas de entrar al coche (conductor y pasajero) por separado
        if (auto* const task = mem->GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER)) {
            if (static_cast<CTaskComplexEnterCar*>(task)->GetTargetCar() == vehicle) {
                return true;
            }
        }
        if (auto* const task = mem->GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER)) {
            if (static_cast<CTaskComplexEnterCar*>(task)->GetTargetCar() == vehicle) {
                return true;
            }
        }
    }
    return false;
}

// 0x5F7CC0
void CPedGroup::PlayerGaveCommand_Attack(CPed* playerPed, CPed* target) {
    // Como el original: el evento de grupo es un local que se destruye al salir (despues de hablar)
    CEventGroupEvent groupEvent{ playerPed, new CEventPlayerCommandToGroup{PLAYER_GROUP_COMMAND_ATTACK, target} };
    if (!m_groupIntelligence.AddEvent(&groupEvent)) {
        return;
    }
    if (target && target->m_nPedType != PED_TYPE_GANG2) {
        // Como el original: habla el jugador (playerPed), el tipo del objetivo solo elige la frase
        playerPed->Say(target->IsGangster() ? CTX_GLOBAL_ORDER_ATTACK_MANY : CTX_GLOBAL_ORDER_ATTACK_SINGLE);
    }
}

// 0x5FAB60
void CPedGroup::PlayerGaveCommand_Gather(CPed* ped) {
    CPedList pedList;
    pedList.Empty();
    pedList.BuildListFromGroup_NotInCar_NoLeader(&m_groupMembership);

    if (ped->GetIntelligence()->IsInACarOrEnteringOne() && ped->m_pVehicle) {
        CFormation::GenerateGatherDestinations_AroundCar(pedList, ped->m_pVehicle);
    } else {
        CFormation::GenerateGatherDestinations(pedList, ped);
    }
    CFormation::DistributeDestinations(pedList);

    for (auto i = 0; i < (int32)pedList.m_count; i++) {
        CPed* const mem = pedList.m_peds.data()[i];
        CVector destination;
        if (CFormation::ReturnDestinationForPed(mem, &destination)) {
            // Como el original: correr (7) hasta el destino con radio 2 y los dos indicadores (true, false)
            CTaskSimpleGoToPoint task{ PEDMOVE_SPRINT, destination, 2.0f, true, false };
            m_groupIntelligence.SetTask(mem, task, m_groupIntelligence.GetPedTaskPairs(), TASK_SECONDARY_INVALID, false);
        }
    }
}

// 0x5FC7E0
void CPedGroup::Process() {
    m_groupMembership.Process();
    m_groupIntelligence.Process();
}

// 0x5FB7D0
void CPedGroup::RemoveAllFollowers() {
    // Como el original: recorre los 8 huecos y quita por indice todos menos el del lider
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        if (m_groupMembership.GetMember(i) && i != CPedGroupMembership::LEADER_MEM_ID) {
            m_groupMembership.RemoveMember(i);
        }
    }
}

// 0x5F7AD0
void CPedGroup::Teleport(const CVector& pos) {
    CPed* const leader = m_groupMembership.GetMember(CPedGroupMembership::LEADER_MEM_ID);

    if (leader && leader->IsAlive()) {
        leader->Teleport(pos, false);
    }

    // Como el original: sin evento actual se sigue; con evento se mira el tipo del evento interno
    if (const auto* const groupEvent = m_groupIntelligence.GetCurrentEvent()) {
        if (groupEvent->m_event->GetEventType() == EVENT_LEADER_ENTRY_EXIT) {
            return;
        }
    }

    // Set *followers* out of the vehicle
    if (leader && !leader->bInVehicle) {
        for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* const f = m_groupMembership.GetMember(i);
            if (!f || !f->bInVehicle || f->IsCreatedByMission() || !f->IsAlive()) {
                continue;
            }
            CTaskSimpleCarSetPedOut{
                f->m_pVehicle,
                (eTargetDoor)CCarEnterExit::ComputeTargetDoorToExit(f->m_pVehicle, f),
                false
            }.ProcessPed(f);
        }
    }

    // Teleport *followers*: el desplazamiento se elige por hueco (0..6), no por orden entre los presentes
    const auto& offsets = CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets;
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        CPed* const f = m_groupMembership.GetMember(i);
        if (!f || !f->IsAlive()) {
            continue;
        }
        const CVector2D& off = offsets.data()[i];
        f->Teleport(CVector{ pos.x + off.x, pos.y + off.y, pos.z }, false);
        f->PositionAnyPedOutOfCollision();
        f->GetTaskManager().AbortFirstPrimaryTaskIn({ TASK_PRIMARY_PHYSICAL_RESPONSE, TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }, f);
    }
}

int32 CPedGroup::GetId() const {
    return CPedGroups::GetGroupId(this);
}

bool CPedGroup::IsActive() const {
    return CPedGroups::ms_activeGroups[GetId()];
}

void CPedGroup::InjectHooks() {
    RH_ScopedClass(CPedGroup);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5FC150);
    RH_ScopedInstall(Destructor, 0x5FC190);

    RH_ScopedInstall(Teleport, 0x5F7AD0);
    RH_ScopedInstall(PlayerGaveCommand_Gather, 0x5FAB60);
    RH_ScopedInstall(PlayerGaveCommand_Attack, 0x5F7CC0);
    RH_ScopedInstall(IsAnyoneUsingCar, 0x5F7DB0);
    RH_ScopedInstall(GetClosestGroupPed, 0x5FACD0);
    RH_ScopedInstall(FindDistanceToFurthestMember, 0x5FB010);
    RH_ScopedInstall(FindDistanceToNearestMember, 0x5FB0A0);
    RH_ScopedInstall(Flush, 0x5FB790);
    RH_ScopedInstall(Process, 0x5FC7E0);
    RH_ScopedInstall(RemoveAllFollowers, 0x5FB7D0);
}
