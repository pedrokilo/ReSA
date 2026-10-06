/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCarSlowBeDraggedOutAndStandUp
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

#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexGetUpAndStandStill.h"
#include "TaskSimpleCarSetPedOut.h"
#include "Events/EventDeath.h"
#include "Events/EventGroup.h"

void CTaskComplexCarSlowBeDraggedOutAndStandUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOutAndStandUp, 0x86EF80, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648620);
    RH_ScopedInstall(Destructor, 0x648690);
    RH_ScopedInstall(CreateSubTask, 0x648710);
    RH_ScopedVMTInstall(Clone, 0x64A190);
    RH_ScopedVMTInstall(GetTaskType, 0x648680);
    RH_ScopedVMTInstall(MakeAbortable, 0x6486F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6488F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x648A10);
    RH_ScopedVMTInstall(ControlSubTask, 0x648B80);
}

// 0x648620
CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 a3) {
    // Como el original: vptr, vehiculo y dword10 antes de registrar la referencia
    m_Vehicle = vehicle;
    dword10 = a3;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x648690
CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// Como el original: grupo de animacion del vehiculo = byte 0xDE del handling + 0x58
static int32 GetAnimGroupOf(CVehicle* veh) {
    return (int32)veh->m_pHandlingData->m_nAnimGroup + (int32)ANIM_GROUP_STDCARAMIMS;
}

// 0x648710
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch ((int32)taskType) {
    case TASK_COMPLEX_LEAVE_CAR: { // 0x2C0 (0x648758)
        const auto group = GetAnimGroupOf(m_Vehicle);
        if (group == ANIM_GROUP_COACHCARANIMS) {
            return new CTaskComplexLeaveCar(ped->m_pVehicle, TARGET_DOOR_FRONT_RIGHT, 0, false, true);
        }
        if (group == ANIM_GROUP_TANKCARANIMS) {
            return new CTaskComplexLeaveCar(ped->m_pVehicle, TARGET_DOOR_DRIVER, 0, false, true);
        }
        if (dword10 == 8 && group == ANIM_GROUP_BUSCARANIMS) {
            return new CTaskComplexLeaveCar(ped->m_pVehicle, TARGET_DOOR_FRONT_RIGHT, 0, false, true);
        }
        return nullptr;
    }
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL: // 0xCE (0x648812)
        return new CTaskComplexGetUpAndStandStill();
    case TASK_SIMPLE_CAR_SET_PED_OUT: // 0x330 (0x648845)
        return new CTaskSimpleCarSetPedOut(ped->m_pVehicle, (eTargetDoor)dword10, true);
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT: // 0x337 (0x6488AD)
        return new CTaskComplexCarSlowBeDraggedOut(m_Vehicle, (eTargetDoor)dword10, false);
    default: // incluye TASK_FINISHED (0x516)
        return nullptr;
    }
}

// 0x6486F0
bool CTaskComplexCarSlowBeDraggedOutAndStandUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: solo con prioridad inmediata se delega en la subtarea (sin guarda de nulo)
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
    }
    return false;
}

// 0x6488F0
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateNextSubTask(CPed* ped) {
    switch ((int32)m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_CAR_SET_PED_OUT:
    case TASK_COMPLEX_LEAVE_CAR:
        return nullptr;
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        // Como el original: llama a CPed::IsPlayer (0x5DF8F0) y descarta el resultado
        (void)ped->IsPlayer();
        return nullptr;
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT: {
        // Como el original: salud > 0 con comparacion no ordenada (NaN cuenta como "mayor")
        if (ped->IsAlive() && !(ped->m_fHealth <= 0.0f)) {
            return new CTaskComplexGetUpAndStandStill();
        }
        CEventDeath event{ false };
        ped->GetIntelligence()->m_eventGroup.Add(&event, false);
        return nullptr;
    }
    default:
        return nullptr;
    }
}

// 0x648A10
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask(CPed* ped) {
    if (ped == FindPlayerPed(0)) {
        if (const auto veh = ped->m_pVehicle) {
            veh->m_vehicleAudio.PlayerAboutToExitVehicleAsDriver();
        }
    } else if (ped->m_pVehicle->m_pDriver == ped) { // Como el original: sin guarda de nulo
        ped->SetRadioStation();
    }

    if (const auto veh = ped->m_pVehicle) {
        const auto group = GetAnimGroupOf(veh);
        if (group == ANIM_GROUP_COACHCARANIMS || group == ANIM_GROUP_TANKCARANIMS || (dword10 == 8 && group == ANIM_GROUP_BUSCARANIMS)) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }
    }

    // Como el original: se vuelve a leer el vehiculo del ped sin guarda de nulo
    switch (GetAnimGroupOf(ped->m_pVehicle)) {
    case ANIM_GROUP_STDCARAMIMS:
    case ANIM_GROUP_VANCARANIMS:
    case ANIM_GROUP_RUSTPLANEANIMS:
    case ANIM_GROUP_KARTCARANIMS:
    case ANIM_GROUP_CONVCARANIMS:
    case ANIM_GROUP_STDTALLCARAMIMS:
    case ANIM_GROUP_HOVERCARANIMS:
    case ANIM_GROUP_STDCARUPRIGHT:
        if (dword10 == 10) {
            ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_PUNCH, 0.0f, 1.0f, nullptr, SURFACE_DEFAULT, 0, 0);
        } else if (dword10 == 8) {
            ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_HEAD_BANG, 0.0f, 1.0f, nullptr, SURFACE_DEFAULT, 0, 0);
        }
        break;
    case ANIM_GROUP_LOWCARAMIMS:
        ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_KICK, 0.0f, 1.0f, nullptr, SURFACE_DEFAULT, 0, 0);
        break;
    case ANIM_GROUP_DOZERCARANIMS:
        ped->GetAE().AddAudioEvent(AE_PED_JACKED_DOZER, 0.0f, 1.0f, nullptr, SURFACE_DEFAULT, 0, 0);
        break;
    default:
        break;
    }
    return CreateSubTask(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, ped);
}

// 0x648B80
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
