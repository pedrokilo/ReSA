/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarJumpOut
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
#include "X87Math.h"
#include "PedPlacement.h"
#include "TaskSimpleCarJumpOut.h"
#include "TaskSimpleCarSetPedOut.h"

void CTaskSimpleCarJumpOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarJumpOut, 0x86ee70, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x64BE40);
    RH_ScopedInstall(Destructor, 0x647490);

    RH_ScopedGlobalInstall(FinishAnimCarJumpOutCB, 0x647700);

    RH_ScopedInstall(ComputeAnimID_ToHook, 0x647690);
    RH_ScopedInstall(StartAnim, 0x64BF00);

    RH_ScopedVMTInstall(Clone, 0x64D220);
    RH_ScopedVMTInstall(GetTaskType, 0x647500);
    RH_ScopedVMTInstall(MakeAbortable, 0x647510);
    RH_ScopedVMTInstall(ProcessPed, 0x64DD60);
    RH_ScopedVMTInstall(SetPedPosition, 0x647590);
}

// 0x64BE40
CTaskSimpleCarJumpOut::CTaskSimpleCarJumpOut(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask) :
    m_veh{veh},
    m_door{door},
    m_lineUpTask{lineUpTask},
    m_initialVehSpeed{ 0.f }
{
    if (m_veh) {
        m_veh->RegisterReference(reinterpret_cast<CEntity**>(&m_veh)); // Como el original: registra la referencia (la DLL la limpiaba)
        m_initialVehSpeed = m_veh->GetMoveSpeed().Magnitude();
    }
}

CTaskSimpleCarJumpOut::CTaskSimpleCarJumpOut(const CTaskSimpleCarJumpOut& o) :
    CTaskSimpleCarJumpOut{o.m_veh, o.m_door, o.m_lineUpTask}
{
}

// 0x647490
CTaskSimpleCarJumpOut::~CTaskSimpleCarJumpOut() {
    CEntity::SafeCleanUpRef(m_veh);

    if (m_anim) {
        m_anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x647700
void CTaskSimpleCarJumpOut::FinishAnimCarJumpOutCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarJumpOut*>(data);
    self->m_finishedAnim = true;
    self->m_anim = nullptr;
}

// 0x64BF00
void CTaskSimpleCarJumpOut::StartAnim(const CPed* ped) {
    const auto [groupId, animId] = ComputeAnimID();
    m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), groupId, animId, 8.f);
    m_anim->SetFinishCallback(FinishAnimCarJumpOutCB, this);
}

// Signature changed, `ComputeAnimID_Wrapper` is hooked instead.
// 0X647690
auto CTaskSimpleCarJumpOut::ComputeAnimID() -> std::pair<AssocGroupId, AnimationId> {
    AssocGroupId animGroup;
    AnimationId  animId;
    ComputeAnimID_ToHook(animGroup, animId);
    return { animGroup, animId };
}

// 0x647690
void CTaskSimpleCarJumpOut::ComputeAnimID_ToHook(AssocGroupId& animGroup, AnimationId& animId) {
    // Como el original: con otra puerta no se escribe la animación y se usa el valor que ya tenga
    switch (m_door) {
    case TARGET_DOOR_FRONT_RIGHT:
    case TARGET_DOOR_REAR_RIGHT:
        animId = ANIM_ID_CAR_ROLLOUT_RHS;
        break;
    case TARGET_DOOR_DRIVER:
    case TARGET_DOOR_REAR_LEFT:
        animId = ANIM_ID_CAR_ROLLOUT_LHS;
        break;
    }
    animGroup = m_veh->GetAnimGroup().GetGroup(animId);
}

// 0x647510
bool CTaskSimpleCarJumpOut::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
        }
        return true;
    }
    case ABORT_PRIORITY_URGENT: {
        // Como el original: el tipo del evento se pide antes de mirar la animación (dos veces si no es EVENT_IN_AIR)
        if (event && (event->GetEventType() == EVENT_IN_AIR || event->GetEventType() == EVENT_IN_WATER) && m_anim && !ped->bInVehicle) {
            m_anim->m_BlendDelta = -2.f;
            m_finishedAnim = true;
            return true;
        }
        break;
    }
    }
    return false;
}

// 0x64DD60
bool CTaskSimpleCarJumpOut::ProcessPed(CPed* ped) {
    if (m_finishedAnim) {
        return true;
    }

    if (!m_veh) {
        return true;
    }

    // Start calculating animation shift for ped in certain vehicles
    if ([this]{
        if (!m_anim) {
            return true;
        }

        if (m_veh->IsBike() || m_veh->IsSubQuad()) {
            return m_anim->m_CurrentTime < std::bit_cast<float>(0x3F3BBBBCu); // Como el original: 0x86F0E8 (0,7333333)
        } else if (m_veh->IsAutomobile()) {
            return m_anim->m_CurrentTime < 0.45f;
        }

        return false;    
    }()) {
        // Como el original: pow(0.96, paso) con el _CIpow del EXE (0x822130) y el producto en ST0
        PowDecaySpeedCpp(&m_initialVehSpeed, &ped->m_vecAnimMovingShiftLocal.y, CTimer::GetTimeStep()); // X87Math.cpp
    }

    // Not in vehicle anymore?
    if (!ped->bInVehicle) { // Inverted
        if (m_veh && m_veh->IsAutomobile()) { // 0x64DE0E
            ped->m_pedIK.bSlopePitch = true; // Como el original: bit 0x8 de las banderas del IK
            ped->bFallenDown = true;
        }
        return !m_anim;
    }

    // Anim not playing yet?
    if (!m_anim) { // Inverted - 0x64DE56
        StartAnim(ped); 
        return false;
    }

    // Start opening the door - 0x64DE74
    const auto GetDoorOpenTimeThreshold = [this] {
        if (m_veh->IsBike() || m_veh->IsSubQuad()) {
            return 0.35f;
        } else if (m_veh->IsSubPlane()) { // Como el original: subtipo avión
            return 0.15f;
        }
        return 0.07f;
    };
    if (!(GetDoorOpenTimeThreshold() < m_anim->m_CurrentTime)) { // Como el original: NaN también abre la puerta
        if (m_veh->IsAutomobile()) {
            const auto [grpId, animId] = ComputeAnimID(); // 0x64E00D
            m_veh->ProcessOpenDoor(ped, m_door, grpId, animId, m_anim->m_CurrentTime);
        }
        return false;
    }

    // 0x64DEC0 - Como el original: la tarea local vive hasta el final (su destructor va después de abrir la puerta)
    CTaskSimpleCarSetPedOut getOutTask{ m_veh, (eTargetDoor)m_door, true };
    getOutTask.m_nDoorFlagsToClear = m_doorFlagsToClear;
    getOutTask.m_nNumGettingInToClear = m_numGettingInToClear;
    getOutTask.ProcessPed(ped);

    CCarEnterExit::RemoveCarSitAnim(ped);

    // Change rotation of ped
    {
        // Como el original: GetHeading después de mirar la orientación y PI exacto del EXE (0x858CB8)
        ped->m_fAimingRotation = ped->m_fCurrentRotation = m_veh->IsTotallyUpsideDown()
            ? CGeneral::LimitRadianAngle(m_veh->GetHeading() + std::bit_cast<float>(0x40490FDBu))
            : m_veh->GetHeading();
        ped->SetHeading(ped->m_fCurrentRotation);
    }
    ped->GetMoveSpeed() = m_veh->GetMoveSpeed();
    ped->m_pEntityIgnoredCollision = m_veh;
    ped->bKnockedOffBike = true;
    if (m_veh->IsAutomobile()) {
        // Como el original: abre la puerta por componente (cerrada -> abierta, dañada -> abierta y dañada), pidiendo el estado 2 o 3 veces
        auto& dm = m_veh->AsAutomobile()->m_damageManager;
        const auto comp = (tComponent)m_door;
        if (dm.GetDoorStatus_Component(comp) == DAMSTATE_OK || dm.GetDoorStatus_Component(comp) == DAMSTATE_DAMAGED) {
            dm.SetDoorStatus_Component(comp, (eDoorStatus)(dm.GetDoorStatus_Component(comp) + 1));
        }
    }

    return false;
}

// 0x647590
bool CTaskSimpleCarJumpOut::SetPedPosition(CPed* ped) {
    if (!ped->bInVehicle) {
        return false;
    }

    ped->SetPedPositionInCar();

    // Do some Z position interpolation for ped for specific anim types
    if (m_anim) {
        // Como el original: FindZCoorForPed se llama con cualquier animación; el id solo decide si se ajusta la altura
        const auto& pedPos = ped->GetPosition();
        auto pedPosAdj{ pedPos };

        CWorld::pIgnoreEntity = m_veh;
        pedPosAdj = CPedPlacement::FindZCoorForPed(pedPosAdj).first; // Como el original: corrige la copia en el sitio
        CWorld::pIgnoreEntity = nullptr;

        const auto timeDivier = (m_veh->IsBike() || m_veh->IsSubQuad()) ? 0.27f : 0.07f;
        switch (m_anim->m_AnimId) {
        case ANIM_ID_CAR_ROLLOUT_RHS:
        case ANIM_ID_CAR_ROLLOUT_LHS: {
            if (pedPosAdj.z > pedPos.z) {
                ped->GetMatrix().GetPosition().z += m_anim->m_CurrentTime / timeDivier * (pedPosAdj.z - pedPos.z);
            }
            break;
        }
        }
    }

    return true;
}
