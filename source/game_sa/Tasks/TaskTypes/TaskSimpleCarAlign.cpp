/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarAlign
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleCarAlign.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarAlign::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarAlign, 0x86EC78, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x645B40);
    RH_ScopedInstall(Destructor, 0x645BE0);

    RH_ScopedGlobalInstall(FinishAnimCarAlignCB, 0x645E40);

    RH_ScopedInstall(FixHeading, 0x645CB0);
    RH_ScopedInstall(StartAnim, 0x64AC00);

    RH_ScopedVMTInstall(Clone, 0x649930);
    RH_ScopedVMTInstall(GetTaskType, 0x645BD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x645C50);
    RH_ScopedVMTInstall(ProcessPed, 0x64D6D0);
    RH_ScopedVMTInstall(SetPedPosition, 0x645C70);
}

// 0x645B40
CTaskSimpleCarAlign::CTaskSimpleCarAlign(CVehicle* veh, CVector const& doorPos, uint32 targetDoor, CTaskUtilityLineUpPedWithCar* taskLineUp) :
    m_veh{veh},
    m_doorPos{doorPos},
    m_door{targetDoor},
    m_lineUpTask{taskLineUp}
{
    CEntity::SafeRegisterRef(m_veh);
}

// NOTSA
CTaskSimpleCarAlign::CTaskSimpleCarAlign(const CTaskSimpleCarAlign& o) :
    CTaskSimpleCarAlign{o.m_veh, o.m_doorPos, o.m_door, o.m_lineUpTask}
{
}

// 0x645BE0
CTaskSimpleCarAlign::~CTaskSimpleCarAlign() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x645E40
void CTaskSimpleCarAlign::FinishAnimCarAlignCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarAlign*>(data);
    self->m_anim = nullptr;
    self->m_animHasFinished = true;
}

// 0x645CB0
void CTaskSimpleCarAlign::FixHeading(CPed* ped) {
    if (!m_veh || !m_veh->IsAutomobile()) {
        return;
    }

    // Como el original: copia del vector derecho de la matriz del vehiculo, por -1 en las puertas izquierdas
    CVector carLeftOrRight = m_veh->GetRight();
    switch (m_door) {
    case CAR_DOOR_LF:
    case CAR_DOOR_LR:
        carLeftOrRight.x *= -1.f;
        carLeftOrRight.y *= -1.f;
        carLeftOrRight.z *= -1.f;
        break;
    }

    // Como el original: producto (z*z' + y*y') + x*x'; solo sigue si 0 < dot < 0,1 (NaN termina)
    const auto Dot = [&](const CVector& f) {
        return (carLeftOrRight.z * f.z + carLeftOrRight.y * f.y) + carLeftOrRight.x * f.x;
    };
    const float dot = Dot(ped->GetForwardVector());
    if (!(dot > 0.f) || !(dot < 0.1f)) {
        return;
    }

    // Como el original: x e y se escalan por el producto y se restan dos veces del forward (z sin escalar no se usa)
    const float dot2 = Dot(ped->GetForwardVector());
    const float xd   = carLeftOrRight.x * dot2;
    const float yd   = carLeftOrRight.y * dot2;
    const auto  fwd  = ped->GetForwardVector();
    const float x    = (fwd.x - xd) - xd;
    const float y    = (fwd.y - yd) - yd;

    ped->m_fAimingRotation = ped->m_fCurrentRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(x, y, 0.f, 0.f));
}

// 0x64AC00
void CTaskSimpleCarAlign::StartAnim(CPed* ped) {
    const auto animId = [this, ped] {
        const auto doorOpenPos = CCarEnterExit::GetPositionToOpenCarDoor(m_veh, m_door);
        float z = doorOpenPos.z - ped->GetPosition().z;
        if (z < 0.f) { // Como el original: negativo se limita a 0 (NaN se conserva)
            z = 0.f;
        }
        // Como el original: alto solo si z > 4,4 (NaN usa la animacion baja)
        switch (m_door) {
        case CAR_DOOR_LF:
        case CAR_DOOR_LR:
            return (z > 4.4f) ? ANIM_ID_CAR_ALIGNHI_LHS : ANIM_ID_CAR_ALIGN_LHS;
        default:
            return (z > 4.4f) ? ANIM_ID_CAR_ALIGNHI_RHS : ANIM_ID_CAR_ALIGN_RHS;
        }
    }();
    m_anim = CAnimManager::BlendAnimation(
        ped->GetRpClump(),
        m_veh->GetAnimGroup().GetGroup(animId),
        animId,
        4.f
    );
    m_anim->SetFinishCallback(FinishAnimCarAlignCB, this);
}

// 0x645C50
bool CTaskSimpleCarAlign::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
        }
        return true;
    }
    default:
        return false;
    }
}

// 0x64D6D0
bool CTaskSimpleCarAlign::ProcessPed(CPed* ped) {
    if (m_animHasFinished || !m_veh) {
        return true;
    }
    if (!m_anim) {
        FixHeading(ped);
        StartAnim(ped);
    }
    return false;
}

// 0x645C70
bool CTaskSimpleCarAlign::SetPedPosition(CPed* ped) {
    std::ignore = CCarEnterExit::GetPositionToOpenCarDoor(m_veh, m_door); // Como el original: se calcula y no se usa
    m_lineUpTask->ProcessPed(ped, m_veh, m_anim);
    return true;
}
