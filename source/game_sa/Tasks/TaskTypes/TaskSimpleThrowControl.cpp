/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleThrowControl
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleThrowProjectile.h"

void CTaskSimpleThrowControl::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleThrowControl, 0x86D7B4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61F8B0);
    RH_ScopedInstall(Destructor, 0x61F950);

    RH_ScopedVMTInstall(Clone, 0x6230B0);
    RH_ScopedVMTInstall(GetTaskType, 0x61F940);
    RH_ScopedVMTInstall(MakeAbortable, 0x61F9B0);
    RH_ScopedVMTInstall(ProcessPed, 0x61F9F0);
}

// 0x61F8B0
CTaskSimpleThrowControl::CTaskSimpleThrowControl(CEntity* targetEntity, CVector const* pos) :
    m_entity{targetEntity},
    m_pos{pos ? *pos : CVector{}}
{
    assert(m_entity);

    CEntity::SafeRegisterRef(m_entity);
}

// NOTSA
CTaskSimpleThrowControl::CTaskSimpleThrowControl(const CTaskSimpleThrowControl& o) :
    CTaskSimpleThrowControl{o.m_entity, &o.m_pos}
{
}

// 0x61F950
CTaskSimpleThrowControl::~CTaskSimpleThrowControl() {
    CEntity::SafeCleanUpRef(m_entity);
}

// 0x61F9B0
bool CTaskSimpleThrowControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!ped->GetIntelligence()->GetTaskThrow()) {
        return true;
    }
    return ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, priority, event);
}

// 0x61F9F0
bool CTaskSimpleThrowControl::ProcessPed(CPed* ped) {
    if (byte8) {
        return true;
    }
    ped->SetMoveState(PEDMOVE_STILL);
    auto& manager = ped->GetTaskManager();
    auto* secondary = manager.GetTaskSecondary(TASK_SECONDARY_ATTACK);
    if (!secondary) {
        if (m_isAttacking) {
            return true;
        }
        manager.SetTaskSecondary(new CTaskSimpleThrowProjectile{m_entity, m_pos}, TASK_SECONDARY_ATTACK);
        m_isAttacking = true;
    } else if (secondary->GetTaskType() != TASK_SIMPLE_THROW_PROJECTILE) {
        secondary->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        return false;
    }
    auto* throwing = ped->GetIntelligence()->GetTaskThrow();
    const auto* targetPos = m_pos != CVector{} ? &m_pos : nullptr;
    if (m_entity || targetPos) {
        const auto delta = (m_entity ? m_entity->GetPosition() : m_pos) - ped->GetPosition();
        float heading;
        // Preserve the original x87 atan2 and its signed-zero behavior.
        const float x = delta.x, y = delta.y;
        __asm {
            fld x
            fchs
            fld y
            fpatan
            fstp heading
        }
        ped->m_fAimingRotation = heading;
    }
    if (!m_isAttacking) {
        return false;
    }
    if (throwing->m_bIsFinished) {
        return true;
    }
    if (targetPos) {
        throwing->m_TargetPos = *targetPos;
    }
    return false;
}
