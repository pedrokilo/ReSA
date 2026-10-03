/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventEditableResponse
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventEditableResponse.h"

#include "PedType.h"
#include "Ragdoll/IKChainManager.h"

void CEventEditableResponse::InjectHooks() {
    RH_ScopedVirtualClass(CEventEditableResponse, 0x85AB80, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AC450);
    RH_ScopedVMTInstall(Clone, 0x420ED0);
    RH_ScopedVMTInstall(HasEditableResponse, 0x420EF0);
    RH_ScopedInstall(WillRespond, 0x4AC490);
    RH_ScopedInstall(InformVehicleOccupants, 0x4AC4A0);
    RH_ScopedInstall(InformRespectedFriends, 0x4B2B00);
    RH_ScopedInstall(InformGroup, 0x4B7DF0);
    RH_ScopedInstall(TriggerLookAt, 0x4AC5A0);
    RH_ScopedOverloadedInstall(ComputeResponseTaskType, "ped", 0x4B56C0, void(CEventEditableResponse::*)(CPed * ped, bool));
    RH_ScopedOverloadedInstall(ComputeResponseTaskType, "group", 0x4B57A0, void(CEventEditableResponse::*)(CPedGroup*));
    RH_ScopedInstall(ComputeResponseTaskOfType, 0x4B5730);
}

// 0x4AC450
CEventEditableResponse::CEventEditableResponse(eTaskType taskType) :
    CEvent() {
    m_bAddToEventGroup     = true;
    m_TaskId               = taskType;
    m_FacialExpressionType = eFacialExpression::NONE;
}

CEventEditableResponse* CEventEditableResponse::Constructor() {
    this->CEventEditableResponse::CEventEditableResponse();
    return this;
}

// 0x420ED0
CEvent* CEventEditableResponse::Clone() const noexcept {
    CEventEditableResponse* clone = CloneEditable();
    clone->m_TaskId               = m_TaskId;
    clone->m_FacialExpressionType = m_FacialExpressionType;
    clone->m_bAddToEventGroup     = m_bAddToEventGroup;
    return clone;
}

// 0x4AC490
bool CEventEditableResponse::WillRespond() const {
    return m_TaskId != TASK_NONE;
}

// 0x4AC4A0
void CEventEditableResponse::InformVehicleOccupants(CPed* ped) {
    CVehicle* vehicle = ped->m_pVehicle;
    if (!ped->bInVehicle || !DoInformVehicleOccupants(ped) || !m_bAddToEventGroup) {
        return;
    }
    const auto InformOccupant = [this, ped](CPed* o) {
        if (!o || o == ped) {
            return;
        }
        const auto clone          = static_cast<CEventEditableResponse*>(Clone());
        clone->m_TaskId           = TASK_NONE;
        clone->m_bAddToEventGroup = false;
        o->GetEventGroup().Add(clone);
        delete clone;
    };
    InformOccupant(vehicle->GetDriver());
    rng::for_each(vehicle->GetPassengers(), InformOccupant);
}

// 0x4B2B00
void CEventEditableResponse::InformRespectedFriends(CPed* ped) {
    if (!m_bAddToEventGroup) {
        return;
    }

    // Como el original: se leen el numero de peatones a informar y el radio antes del bucle
    const int32 numPedsToScan = (int32)ped->GetIntelligence()->m_nDmNumPedsToScan;
    const float fDmRadius     = ped->GetIntelligence()->m_fDmRadius;
    if (!numPedsToScan) {
        return;
    }

    // Como el original: el respeto del peaton se calcula una vez, antes del bucle
    const uint32 respect = ped->GetAcquaintance().GetAcquaintances(ACQUAINTANCE_RESPECT);
    CEntity** entities   = ped->GetIntelligence()->GetPedEntities();
    // Como el original: se recorren los 16 slots del escaner y se para cuando se han informado tantos amigos como
    // m_nDmNumPedsToScan (el contador solo sube cuando la cola acepta el evento)
    int32 numInformed = 0;
    for (int32 i = 0; i < 16; i++) {
        if (numInformed >= numPedsToScan) {
            break;
        }
        CEntity* entity = entities[i];
        if (!entity) {
            continue;
        }

        CPed* entityPed = entity->AsPed();
        if (ped->m_nPedType == PED_TYPE_COP) {
            if (entityPed->m_nPedType != PED_TYPE_COP) {
                continue;
            }

            // Como el original: no comprueba m_pWanted
            CPlayerPedData* playerData = FindPlayerPed(0)->GetPlayerData();
            if (playerData->m_pWanted->GetWantedLevel() != eWantedLevel::WANTED_CLEAN) {
                continue;
            }
        } else {
            // Como el original: si el que informa es el jugador, cuenta el respeto del amigo hacia el jugador
            uint32 flags;
            if (ped->IsPlayer()) {
                flags = entityPed->GetAcquaintance().GetAcquaintances(ACQUAINTANCE_RESPECT) & CPedType::GetPedFlag(ped->m_nPedType);
            } else {
                flags = respect & CPedType::GetPedFlag(entityPed->m_nPedType);
            }
            if (!flags) {
                continue;
            }
        }

        // Como el original: (z*z + x*x) + y*y y comparacion estricta (NaN no informa)
        const CVector& entityPos = entity->GetPosition();
        const CVector& pedPos    = ped->GetPosition();
        const float dx = pedPos.x - entityPos.x;
        const float dy = pedPos.y - entityPos.y;
        const float dz = pedPos.z - entityPos.z;
        const float distSq = dz * dz + dx * dx + dy * dy;
        if (fDmRadius * fDmRadius > distSq) {
            auto clonedEvent                = static_cast<CEventEditableResponse*>(Clone());
            clonedEvent->m_TaskId           = TASK_NONE;
            clonedEvent->m_bAddToEventGroup = false;
            if (CInformFriendsEventQueue::Add(entityPed, clonedEvent)) {
                numInformed++;
            } else {
                delete clonedEvent;
            }
        }
    }
}

// 0x4B7DF0
void CEventEditableResponse::InformGroup(CPed* ped) {
    CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);
    if (!pedGroup) {
        return;
    }

    auto clonedEvent                = static_cast<CEventEditableResponse*>(Clone());
    clonedEvent->m_TaskId           = TASK_NONE;
    clonedEvent->m_bAddToEventGroup = false;
    if (!CInformGroupEventQueue::Add(ped, pedGroup, clonedEvent)) {
        delete clonedEvent;
    }
}

// 0x4AC5A0
void CEventEditableResponse::TriggerLookAt(CPed* ped) const {
    CEntity* sourceEntity = GetSourceEntity();
    if (sourceEntity) {
        if (sourceEntity->GetIsTypePed()) {
            g_ikChainMan.LookAt("CEventEditableResponse", ped, sourceEntity->AsPed(), 2'000, BONE_HEAD, nullptr, true, 0.25f, 500, 3, false);
            return;
        }
        g_ikChainMan.LookAt("CEventEditableResponse", ped, sourceEntity, 2'000, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
    }
}

// 0x4B56C0
void CEventEditableResponse::ComputeResponseTaskType(CPed* ped, bool bDecisionMakerTypeInGroup) {
    if (m_TaskId != TASK_NONE) {
        return;
    }
    // Como el original: GetEventType, ComputeEventSourceType y GetInstance en este orden
    const auto eventType       = GetEventType();
    const auto eventSourceType = CEventSource::ComputeEventSourceType(*this, *ped);
    const bool bInVehicle      = ped->bInVehicle;
    CDecisionMakerTypes::GetInstance()->MakeDecision(
        ped,
        eventType,
        eventSourceType,
        bInVehicle,
        TASK_SIMPLE_INFORM_RESPECTED_FRIENDS,
        TASK_SIMPLE_INFORM_GROUP,
        TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD,
        TASK_INVALID,
        bDecisionMakerTypeInGroup,
        static_cast<int16&>(m_TaskId),
        static_cast<int16&>(m_FacialExpressionType)
    );
}

// 0x4B57A0
void CEventEditableResponse::ComputeResponseTaskType(CPedGroup* pedGroup) {
    if (m_TaskId != TASK_NONE) {
        return;
    }
    // Como el original: GetEventType se consulta una sola vez, antes de buscar al miembro
    const auto eventType = GetEventType();
    CPed* groupLeader = pedGroup->GetMembership().GetLeader();
    CPed* member      = groupLeader;
    if (groupLeader && groupLeader->IsPlayer()) {
        member = nullptr;
    }
    if (!member) {
        for (size_t memberId = 0; memberId < TOTAL_PED_GROUP_FOLLOWERS; memberId++) {
            if (member = pedGroup->GetMembership().GetMember(memberId)) {
                break;
            }
        }
    }
    if (member) {
        const auto eventSourceType = CEventSource::ComputeEventSourceType(*this, *member);
        const bool bInVehicle      = member->bInVehicle;
        m_TaskId = CDecisionMakerTypes::GetInstance()->MakeDecision(
            pedGroup,
            eventType,
            eventSourceType,
            bInVehicle,
            TASK_SIMPLE_INFORM_GROUP,
            TASK_SIMPLE_INFORM_RESPECTED_FRIENDS,
            TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD,
            TASK_INVALID
        );
    } else {
        m_TaskId = TASK_NONE;
    }
    if (m_TaskId == TASK_NONE && pedGroup->GetMembership().GetLeader()) {
        if (pedGroup->GetMembership().GetLeader()->IsPlayer()) {
            groupLeader                = pedGroup->GetMembership().GetLeader();
            const auto eventSourceType = CEventSource::ComputeEventSourceType(*this, *groupLeader);
            const bool bInVehicle      = groupLeader->bInVehicle;
            m_TaskId = CDecisionMakerTypes::GetInstance()->MakeDecision(
                pedGroup,
                eventType,
                eventSourceType,
                bInVehicle,
                TASK_SIMPLE_INFORM_GROUP,
                TASK_SIMPLE_INFORM_RESPECTED_FRIENDS,
                TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD,
                TASK_INVALID
            );
        }
    }
}

// 0x4B5730
bool CEventEditableResponse::ComputeResponseTaskOfType(CPed* ped, eTaskType taskId) {
    int16 outTaskId       = TASK_INVALID;
    int16 facialTaskType  = TASK_INVALID;
    // Como el original: GetEventType y una sola llamada a ComputeEventSourceType antes de GetInstance
    const auto eventType       = GetEventType();
    const auto eventSourceType = CEventSource::ComputeEventSourceType(*this, *ped);
    const bool bInVehicle      = ped->bInVehicle;
    CDecisionMakerTypes::GetInstance()->MakeDecision(
        ped,
        eventType,
        eventSourceType,
        bInVehicle,
        TASK_INVALID,
        TASK_INVALID,
        TASK_INVALID,
        taskId,
        false,
        outTaskId,
        facialTaskType
    );
    return taskId == outTaskId;
}
