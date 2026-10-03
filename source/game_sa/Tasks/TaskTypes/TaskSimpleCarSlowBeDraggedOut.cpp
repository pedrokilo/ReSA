/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSlowBeDraggedOut
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
#include "TaskSimpleCarSlowBeDraggedOut.h"

// 0x648230
CTaskSimpleCarSlowBeDraggedOut::CTaskSimpleCarSlowBeDraggedOut(CVehicle* target, eTargetDoor door, CTaskUtilityLineUpPedWithCar* lineUpUtility, bool isDraggedBySelf) :
    m_Vehicle{ target },
    m_Door{ door },
    m_LineUpUtility{ lineUpUtility },
    m_IsDraggedBySelf{ isDraggedBySelf }
{
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x64A040 (Clone)
CTaskSimpleCarSlowBeDraggedOut::CTaskSimpleCarSlowBeDraggedOut(const CTaskSimpleCarSlowBeDraggedOut& o) :
    CTaskSimpleCarSlowBeDraggedOut{ o.m_Vehicle, o.m_Door, o.m_LineUpUtility, false } // Como el original: el clon no se arrastra solo
{
}

// 0x6482C0
CTaskSimpleCarSlowBeDraggedOut::~CTaskSimpleCarSlowBeDraggedOut() {
    CEntity::SafeCleanUpRef(m_Vehicle); // Como el original: primero la referencia y luego la animación (sin anular m_Anim)
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x6483A0
void CTaskSimpleCarSlowBeDraggedOut::FinishAnimCarSlowBeDraggedOutCB(CAnimBlendAssociation* pAnim, void* data) {
    auto* const self = notsa::cast<CTaskSimpleCarSlowBeDraggedOut>(static_cast<CTask*>(data));

    self->m_HasAnimFinished = true;
    self->m_Anim            = nullptr;
}

// 0x648330
auto CTaskSimpleCarSlowBeDraggedOut::ComputeAnimID() {
    const auto animId = [&] {
        switch ( m_Door ) {
        case TARGET_DOOR_FRONT_RIGHT:
        case TARGET_DOOR_REAR_RIGHT: return ANIM_ID_CAR_JACKEDRHS;
        case TARGET_DOOR_DRIVER:
        case TARGET_DOOR_REAR_LEFT:  return ANIM_ID_CAR_JACKEDLHS;
        default:                     NOTSA_UNREACHABLE();
        }
    }();
    return std::make_tuple(m_Vehicle->GetAnimGroup().GetGroup(animId), animId);
}

// notsa
void CTaskSimpleCarSlowBeDraggedOut::DoOpenCarDoor(CPed* ped, float time) {
    const auto [animGrp, animId] = ComputeAnimID();
    m_Vehicle->ProcessOpenDoor(ped, m_Door, animGrp, animId, time);
}

// 0x64C190
void CTaskSimpleCarSlowBeDraggedOut::StartAnim(CPed* ped) {
    CCarEnterExit::RemoveCarSitAnim(ped);

    const auto [animGroup, animId] = ComputeAnimID();
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), animGroup, animId, 1000.f);
    m_Anim->SetFinishCallback(FinishAnimCarSlowBeDraggedOutCB, this);
    m_FrontAnim = m_Anim->HasFlag(ANIMATION_IS_FRONT);
}

// 0x64C080
bool CTaskSimpleCarSlowBeDraggedOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    if (m_Anim) {
        m_Anim->SetBlendDelta(-1000.f);
    }
    if (m_Vehicle) {
        DoOpenCarDoor(ped, 1.f);
    }
    return true;
}

// 0x64E180
bool CTaskSimpleCarSlowBeDraggedOut::ProcessPed(CPed* ped) {
    if (!m_Vehicle) {
        return true;
    }
    if (m_HasAnimFinished) {
        if (!RpAnimBlendClumpGetAssociation(ped->GetRpClump(), { ANIM_ID_FLOOR_HIT_F, ANIM_ID_FLOOR_HIT })) {
            CAnimManager::BlendAnimation(
                ped->GetRpClump(),
                ANIM_GROUP_DEFAULT,
                m_FrontAnim
                    ? ANIM_ID_FLOOR_HIT_F
                    : ANIM_ID_FLOOR_HIT,
                1000.f
            );
            return false;
        }
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
        return false;
    }
    if (!ped->bInVehicle) {
        return true;
    }
    if (m_Anim->GetCurrentTime() > 1.7f) {
        m_LineUpUtility->m_nDoorOpenPosType = 2;
    }
    if (m_IsDraggedBySelf) {
        DoOpenCarDoor(ped, m_Anim->GetCurrentTime()); // Como el original: la puerta sigue el tiempo de la animación
    }
    return false;
}

// 0x64C0E0
bool CTaskSimpleCarSlowBeDraggedOut::SetPedPosition(CPed* ped) {
    if (m_Vehicle) {
        if (!m_HasAnimFinished && (!m_Anim || m_Anim->GetBlendAmount() == 0.f)) {
            ped->SetPedPositionInCar();
        } else if (m_Anim) {
            m_LineUpUtility->ProcessPed(ped, m_Vehicle, m_Anim);
        } else {
            // Como el original: la asociación se guarda en m_Anim durante la llamada y luego se anula
            const auto [animGrp, animId] = ComputeAnimID();
            m_Anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), animId);
            if (m_Anim) {
                m_LineUpUtility->ProcessPed(ped, m_Vehicle, m_Anim);
            }
            m_Anim = nullptr;
        }
    }
    return true;
}

void CTaskSimpleCarSlowBeDraggedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSlowBeDraggedOut, 0x86ef0c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648230);
    RH_ScopedInstall(Destructor, 0x6482C0);

    RH_ScopedGlobalInstall(FinishAnimCarSlowBeDraggedOutCB, 0x6483A0);

    //RH_ScopedInstall(ComputeAnimID, 0x648330); // cant hook, I've changed the signature
    RH_ScopedInstall(StartAnim, 0x64C190);

    RH_ScopedVMTInstall(Clone, 0x64A040);
    RH_ScopedVMTInstall(GetTaskType, 0x6482B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x64C080);
    RH_ScopedVMTInstall(ProcessPed, 0x64E180);
    RH_ScopedVMTInstall(SetPedPosition, 0x64C0E0);
}
