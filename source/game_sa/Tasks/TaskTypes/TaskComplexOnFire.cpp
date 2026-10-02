/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexOnFire
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexOnFire.h"
#include "TaskComplexDie.h"
#include "TaskComplexSmartFleeEntity.h"

void CTaskComplexOnFire::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOnFire, 0x86e308, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x633390);
    RH_ScopedInstall(Destructor, 0x6333C0);

    RH_ScopedInstall(ComputeFireDamage, 0x6333D0);
    RH_ScopedInstall(CreateSubTask, 0x633470);

    RH_ScopedVMTInstall(Clone, 0x636E30);
    RH_ScopedVMTInstall(GetTaskType, 0x6333B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6391B0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6391F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x639200);
}

// 0x6333D0
void CTaskComplexOnFire::ComputeFireDamage(CPed* ped, CPedDamageResponse& outResp) {
    if (ped->physicalFlags.bFireProof) {
        return;
    }

    CPedDamageResponseCalculator calc{
        ped->m_pFire ? ped->m_pFire->GetEntityStartedFire() : nullptr,
        CTimer::GetTimeStep() * ped->m_fireDmgMult * 500.f / 1000.f, // TODO: Magic numberz
        WEAPON_FLAMETHROWER,
        PED_PIECE_TORSO,
        false
    };
    calc.ComputeDamageResponse(ped, outResp, false);
}

// 0x633470
CTask* CTaskComplexOnFire::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_DIE:
        return new CTaskComplexDie{
            WEAPON_UNARMED,
            ANIM_GROUP_DEFAULT,
            ANIM_ID_KO_SHOT_FRONT_0
        };
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        return new CTaskComplexSmartFleeEntity{
            FindPlayerPed(),
            0,
            1000.0,
            100000,
            1000,
            1.f
        };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6391B0
CTask* CTaskComplexOnFire::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_DIE:
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        return CreateSubTask(TASK_FINISHED);
    default:
        return nullptr;
    }
}

// 0x6391F0
CTask* CTaskComplexOnFire::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_COMPLEX_SMART_FLEE_ENTITY);
}

// 0x639200
CTask* CTaskComplexOnFire::ControlSubTask(CPed* ped) {
    if (ped->physicalFlags.bSubmergedInWater) {
        if (ped->m_pFire) {
            ped->m_pFire->Extinguish();
        }
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SMART_FLEE_ENTITY: {
        if (!ped->m_pFire) {
            MakeAbortable(ped);
            return nullptr;
        }

        if (CLocalisation::PedsOnFire()) {
            ped->Say(CTX_GLOBAL_PAIN_ON_FIRE, 0, 0.1f);
        }

        CPedDamageResponse resp{};
        ComputeFireDamage(ped, resp);
        if (resp.m_bHealthZero && MakeAbortable(ped)) {
            CEventHandler::RegisterKill(ped, ped->m_pFire->GetEntityStartedFire(), WEAPON_MOLOTOV, false);
            return CreateSubTask(TASK_COMPLEX_DIE);
        }
        break;
    }
    }

    return m_pSubTask;
}
