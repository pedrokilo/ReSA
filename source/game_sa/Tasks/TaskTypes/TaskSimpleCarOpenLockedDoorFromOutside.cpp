/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarOpenLockedDoorFromOutside
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
#include "TaskSimpleCarOpenLockedDoorFromOutside.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarOpenLockedDoorFromOutside::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarOpenLockedDoorFromOutside, 0x86ecc0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646070);
    RH_ScopedInstall(Destructor, 0x6460F0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x64ADE0);

    RH_ScopedInstall(FinishAnimCarOpenLockedDoorFromOutsideCB, 0x6461A0);
    RH_ScopedInstall(StartAnim, 0x64AE00);

    RH_ScopedVMTInstall(Clone, 0x649A10);
    RH_ScopedVMTInstall(GetTaskType, 0x6460E0);
    RH_ScopedVMTInstall(MakeAbortable, 0x646160);
    RH_ScopedVMTInstall(ProcessPed, 0x64D8E0);
    RH_ScopedVMTInstall(SetPedPosition, 0x646180);

}

// 0x646070
CTaskSimpleCarOpenLockedDoorFromOutside::CTaskSimpleCarOpenLockedDoorFromOutside(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpUtil) :
    m_lineUpUtil{lineUpUtil},
    m_veh{veh},
    m_door{door}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleCarOpenLockedDoorFromOutside::CTaskSimpleCarOpenLockedDoorFromOutside(const CTaskSimpleCarOpenLockedDoorFromOutside& o) :
    CTaskSimpleCarOpenLockedDoorFromOutside{ o.m_veh, o.m_door, o.m_lineUpUtil }
{
}

// 0x6460F0
CTaskSimpleCarOpenLockedDoorFromOutside::~CTaskSimpleCarOpenLockedDoorFromOutside() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

// 0x6461A0
void CTaskSimpleCarOpenLockedDoorFromOutside::FinishAnimCarOpenLockedDoorFromOutsideCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarOpenLockedDoorFromOutside*>(data);
    self->m_anim = nullptr;
    self->m_hasAnimFinished = true;
}

// 0x64AE00
void CTaskSimpleCarOpenLockedDoorFromOutside::StartAnim(CPed* ped) {
    const auto animationId = [this] {
        switch ((eTargetDoor)m_door) {
        case TARGET_DOOR_DRIVER:
        case TARGET_DOOR_REAR_LEFT: // Como el original: las dos puertas izquierdas usan la animación LHS
            return ANIM_ID_CAR_DOORLOCKED_LHS;
        default:
            return ANIM_ID_CAR_DOORLOCKED_RHS;
        }
    }();
    m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_veh->GetAnimGroup().GetGroup(animationId), animationId, 4.f);
    m_anim->SetFinishCallback(FinishAnimCarOpenLockedDoorFromOutsideCB, this);
}

// 0x646160
bool CTaskSimpleCarOpenLockedDoorFromOutside::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }

    if (m_anim) {
        m_anim->m_BlendDelta = -1000.f;
    }

    return true;
}

// 0x64D8E0
bool CTaskSimpleCarOpenLockedDoorFromOutside::ProcessPed(CPed* ped) {
    if (m_hasAnimFinished || !m_veh) { // Como el original: sin vehículo la tarea termina
        return true;
    }

    if (!m_anim) {
        StartAnim(ped);
    }

    return false;
}

// 0x646180
bool CTaskSimpleCarOpenLockedDoorFromOutside::SetPedPosition(CPed* ped) {
    m_lineUpUtil->ProcessPed(ped, m_veh, m_anim);
    return true;
}

// 0x64ADE0 (lote 128A, banco Fn128A) : destructor con borrado de CTaskSimpleCarOpenLockedDoorFromOutside del EXE (ranura 0 de su vtable 0x86ECC0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6460F0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x6460F0), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleCarOpenLockedDoorFromOutside* CTaskSimpleCarOpenLockedDoorFromOutside::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
