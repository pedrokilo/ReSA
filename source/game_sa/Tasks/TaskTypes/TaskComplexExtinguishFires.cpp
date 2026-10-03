/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexExtinguishFires
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexExtinguishFires.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleGunControl.h"
#include "FireManager.h"

void CTaskComplexExtinguishFires::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexExtinguishFires, 0x870dd0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6975C0);
    RH_ScopedInstall(Destructor, 0x6975F0);

    RH_ScopedInstall(GetNearestFire, 0x697600);

    RH_ScopedVMTInstall(Clone, 0x6977B0);
    RH_ScopedVMTInstall(GetTaskType, 0x6975E0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x698110);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x698290);
    RH_ScopedVMTInstall(ControlSubTask, 0x698330);
}

CTaskComplexExtinguishFires::CTaskComplexExtinguishFires(const CTaskComplexExtinguishFires& o) :
    CTaskComplexExtinguishFires{}
{
}

// 0x697600
CFire* CTaskComplexExtinguishFires::GetNearestFire(CPed* ped) {
    const auto fire = gFireManager.FindNearestFire(ped->GetPosition(), false, false);
    if (!fire) {
        return nullptr;
    }
    // Como el original: (z*z + x*x) + y*y y el fuego se acepta también si la distancia es NaN (fcompp + test ah,5)
    const auto& pedPos = ped->GetPosition();
    const float dx = pedPos.x - fire->GetPosition().x;
    const float dy = pedPos.y - fire->GetPosition().y;
    const float dz = pedPos.z - fire->GetPosition().z;
    const float distSq = dz * dz + dx * dx + dy * dy;
    if (20.f * 20.f < distSq) {
        return nullptr;
    }
    return fire;
}

// 0x698110
CTask* CTaskComplexExtinguishFires::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
    case TASK_SIMPLE_GUN_CTRL: {
        // Como el original: se apaga m_fireToExt (el mismo puntero que el fuego más cercano)
        if (GetNearestFire(ped) == m_fireToExt) {
            m_fireToExt->Extinguish();
            m_fireToExt = nullptr;
        }
        return CreateFirstSubTask(ped);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        const auto nearestFireNow = GetNearestFire(ped);
        if (m_fireToExt != nearestFireNow) {
            return CreateFirstSubTask(ped);
        }

        // Como el original: el índice del arma activa se extiende con signo (movsx)
        if (ped->m_aWeapons.data()[(int8)ped->m_nActiveWeaponSlot].m_Type == WEAPON_EXTINGUISHER) {
            return new CTaskSimpleGunControl{ nullptr, nearestFireNow->GetPosition(), {}, eGunCommand::PISTOLWHIP, 1, -1 };
        }
        return new CTaskSimpleStandStill{ 2000, false, false, 8.f };
    }
    default: // Como el original: una subtarea desconocida devuelve nullptr
        return nullptr;
    }
}

// 0x698290
CTask* CTaskComplexExtinguishFires::CreateFirstSubTask(CPed* ped) {
    if (m_fireToExt = GetNearestFire(ped)) {
        return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_RUN, m_fireToExt->GetPosition(), 1.5f, 2.f, false, false };
    }
    return nullptr;
}

// 0x698330
CTask* CTaskComplexExtinguishFires::ControlSubTask(CPed* ped) {
    // Como el original: el tipo de la subtarea se consulta otra vez antes de cada comparación
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GUN_CTRL) {
        if (GetNearestFire(ped) != m_fireToExt && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return CreateFirstSubTask(ped);
        }
    } else if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        const auto nearestFireNow = GetNearestFire(ped);
        if (!nearestFireNow) {
            return nullptr;
        }
        if (m_fireToExt != nearestFireNow) {
            m_fireToExt = nearestFireNow; // Como el original: se guarda el nuevo fuego
            static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask)->GoToPoint(nearestFireNow->GetPosition(), 1.5f, 2.f, false);
        }
    }
    return m_pSubTask;
}
