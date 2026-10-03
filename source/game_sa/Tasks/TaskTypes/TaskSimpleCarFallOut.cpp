/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarFallOut
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
#include "TaskSimpleCarFallOut.h"

// 0x648D60
CTaskSimpleCarFallOut::CTaskSimpleCarFallOut(CVehicle* target, eTargetDoor door, CTaskUtilityLineUpPedWithCar* lineUpUtility) :
    m_Vehicle{ target },
    m_Door{ door },
    m_LineUpUtility{ lineUpUtility }
{
    CEntity::SafeRegisterRef(m_Vehicle); // Como el original: registra la referencia al vehículo
}

// 0x64A280 (Clone)
CTaskSimpleCarFallOut::CTaskSimpleCarFallOut(const CTaskSimpleCarFallOut& o) :
    CTaskSimpleCarFallOut{ o.m_Vehicle, o.m_Door, o.m_LineUpUtility }
{
}

// 0x648DE0
CTaskSimpleCarFallOut::~CTaskSimpleCarFallOut() {
    CEntity::SafeCleanUpRef(m_Vehicle); // Como el original
    if (notsa::IsFixBugs()) {
        if (const auto a = std::exchange(m_Anim, nullptr)) {
            a->SetDefaultFinishCallback();
        }
    }
}

// 0x648EE0
void CTaskSimpleCarFallOut::FinishAnimFallOutCB(CAnimBlendAssociation* anim, void* data) {
    auto* const self = static_cast<CTaskSimpleCarFallOut*>(data);

    self->m_HasFinished = true;
    self->m_Anim        = nullptr;

    if (self->m_Vehicle && self->m_Vehicle->IsAutomobile()) {
        // Como el original: abre la puerta (cerrada -> abierta, dañada -> abierta y dañada), pidiendo el estado dos veces
        auto& dm = self->m_Vehicle->AsAutomobile()->GetDamageManager();
        const auto comp = (tComponent)(self->m_Door);
        // Como el original: Get == OK; si no, otro Get == DAMAGED; y un último Get para el +1 (2 o 3 lecturas)
        if (dm.GetDoorStatus_Component(comp) == DAMSTATE_OK || dm.GetDoorStatus_Component(comp) == DAMSTATE_DAMAGED) {
            dm.SetDoorStatus_Component(comp, (eDoorStatus)(dm.GetDoorStatus_Component(comp) + 1));
        }
    }
}

// 0x648E90
auto CTaskSimpleCarFallOut::ComputeAnimID() {
    const auto animId = notsa::contains({ TARGET_DOOR_DRIVER, TARGET_DOOR_REAR_LEFT }, m_Door)
        ? ANIM_ID_CAR_FALLOUT_LHS
        : ANIM_ID_CAR_FALLOUT_RHS;
    return std::make_pair(m_Vehicle->GetAnimGroup().GetGroup(animId), animId);
}

// 0x64CCE0
void CTaskSimpleCarFallOut::StartAnim(const CPed* ped) {
    CCarEnterExit::RemoveCarSitAnim(ped);

    const auto [animGrp, animId] = ComputeAnimID();
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), animGrp, animId, 1000.0f);
    m_Anim->SetFinishCallback(FinishAnimFallOutCB, this);
}

// notsa
void CTaskSimpleCarFallOut::DoOpenCarDoor(CPed* ped, float time) {
    const auto [animGrp, animId] = ComputeAnimID();
    m_Vehicle->ProcessOpenDoor(ped, m_Door, animGrp, animId, time);
}

// 0x64CC60
bool CTaskSimpleCarFallOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    if (m_Anim) {
        m_Anim->SetBlendDelta(-1000.f);
        m_Anim->SetDefaultFinishCallback();
        m_Anim = nullptr;
    }
    if (m_Vehicle) {
        DoOpenCarDoor(ped, 1.f);
    }
    return true;
}

// 0x64E2C0
bool CTaskSimpleCarFallOut::ProcessPed(CPed* ped) {
    if (!m_Vehicle || m_HasFinished) {
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
        return false;
    }
    if (!ped->bInVehicle) {
        return true;
    }
    DoOpenCarDoor(ped, m_Anim->GetCurrentTime()); // Como el original: la puerta sigue el tiempo de la animación
    return false;
}

// 0x648E40
bool CTaskSimpleCarFallOut::SetPedPosition(CPed* ped) {
    if (m_Vehicle) {
        if (m_HasFinished || (m_Anim && m_Anim->GetBlendAmount() != 0.f)) {
            m_LineUpUtility->ProcessPed(ped, m_Vehicle, m_Anim);
        } else {
            ped->SetPedPositionInCar();
        }
    }
    return true;
}

void CTaskSimpleCarFallOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarFallOut, 0x86efd0, 9);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x648D60);
    RH_ScopedInstall(Destructor, 0x648DE0);

    RH_ScopedGlobalInstall(FinishAnimFallOutCB, 0x648EE0);

    //RH_ScopedInstall(ComputeAnimID, 0x648E90); // cant hook, changed signature
    RH_ScopedInstall(StartAnim, 0x64CCE0);

    RH_ScopedVMTInstall(Clone, 0x64A280);
    RH_ScopedVMTInstall(GetTaskType, 0x648DD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x64CC60);
    RH_ScopedVMTInstall(ProcessPed, 0x64E2C0);
    RH_ScopedVMTInstall(SetPedPosition, 0x648E40);
}
