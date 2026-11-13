/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleEvasiveStep
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

#include "TaskSimpleEvasiveStep.h"

void CTaskSimpleEvasiveStep::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleEvasiveStep, 0x86F1F4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x653160);
    RH_ScopedInstall(Destructor, 0x6531D0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x655E80); // lote 180A (sin validar)
    RH_ScopedInstall(StartAnim, 0x655EA0);
    RH_ScopedInstall(FinishAnimEvasiveStepCB, 0x653290);

    RH_ScopedVMTInstall(Clone, 0x655810);
    RH_ScopedVMTInstall(GetTaskType, 0x6531C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x653240);
    RH_ScopedVMTInstall(ProcessPed, 0x657A60);
}

// 0x653160
CTaskSimpleEvasiveStep::CTaskSimpleEvasiveStep(CEntity* entity) : CTaskSimple() {
    m_Entity = entity;
    m_bFinished = false;
    m_Assoc = nullptr;
    CEntity::SafeRegisterRef(m_Entity);
}

// 0x6531D0
CTaskSimpleEvasiveStep::~CTaskSimpleEvasiveStep() {
    if (m_Assoc) {
        m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    CEntity::SafeCleanUpRef(m_Entity);
}

// 0x655E80 (lote 180A, sin validar; el listado decia CTaskComplexWalkRoundFire): destructor con borrado de CTaskSimpleEvasiveStep del EXE (ranura 0 de su vtable 0x86F1F4; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6531D0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6531D0, con hook), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleEvasiveStep* CTaskSimpleEvasiveStep::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x653240
bool CTaskSimpleEvasiveStep::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        if (m_Assoc) {
            m_Assoc->m_BlendDelta = -4.0f;
            m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Assoc = nullptr;
        }
        return true;
    default:
        return false;
    }
}

// 0x657A60
bool CTaskSimpleEvasiveStep::ProcessPed(CPed* ped) {
    if (m_bFinished)
        return true;

    if (!m_Entity)
        return true;

    if (!m_Assoc) {
        if (m_Entity->GetIsTypeVehicle()) { // Como el original: comprueba el tipo de entidad (byte 0x36 & 7 == 2), no el estado
            ped->Say(CTX_GLOBAL_DODGE);
        }
        StartAnim(ped);
    }
    return false;
}

// 0x655EA0
void CTaskSimpleEvasiveStep::StartAnim(CPed* ped) {
    m_Assoc = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_EV_STEP, 8.0f);
    m_Assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, false);
    m_Assoc->SetFinishCallback(FinishAnimEvasiveStepCB, this);
}

// 0x653290
void CTaskSimpleEvasiveStep::FinishAnimEvasiveStepCB(CAnimBlendAssociation* assoc, void* data) {
    auto task = static_cast<CTaskSimpleEvasiveStep*>(data);
    task->m_Assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
    if (task->m_Assoc->m_BlendDelta >= 0.0f) {
        task->m_Assoc->m_BlendDelta = -4.0f;
    }
    task->m_bFinished = true;
    task->m_Assoc = nullptr;
}
