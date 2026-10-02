/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseAttractor
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseAttractor.h"
#include "ScriptsForBrains.h"
#include "PedAttractor.h"
#include "TaskSimpleUseAtm.h"
#include "TaskComplexSitDownThenIdleThenStandUp.h"
#include "TaskComplexWaitForBus.h"
#include "TaskComplexWaitForDryWeather.h"
#include "TaskComplexUseScriptedBrain.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskComplexSunbathe.h"
#include "InterestingEvents.h"

void CTaskComplexUseAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseAttractor, 0x86e134, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6326F0);
    RH_ScopedInstall(Destructor, 0x632720);

    RH_ScopedVMTInstall(Clone, 0x6369B0);
    RH_ScopedVMTInstall(GetTaskType, 0x632710);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632730);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6385D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6327C0);
}

// 0x6326F0
CTaskComplexUseAttractor::CTaskComplexUseAttractor(CPedAttractor* attractor) :
    m_attractor{attractor}
{
    assert(attractor);
}

CTaskComplexUseAttractor::CTaskComplexUseAttractor(const CTaskComplexUseAttractor& o) :
    CTaskComplexUseAttractor{ o.m_attractor }
{
}

// 0x632730
CTask* CTaskComplexUseAttractor::CreateNextSubTask(CPed* ped) {
    switch (m_attractor->GetType()) {
    case PED_ATTRACTOR_PIZZA: {
        // El original compara el dinero como sin signo (un valor negativo también resta 10).
        ped->m_nMoneyCount = (uint16)ped->m_nMoneyCount > 10 ? ped->m_nMoneyCount - 10 : 0;
        break;
    }
    case PED_ATTRACTOR_ATM: {
        ped->m_nMoneyCount += 20 + 20 * CGeneral::GetRandomIntInRange(0, 50);
        break;
    }
    }
    GetPedAttractorManager()->BroadcastDeparture(ped, m_attractor);
    return nullptr;
}

// 0x6385D0
CTask* CTaskComplexUseAttractor::CreateFirstSubTask(CPed* ped) {
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_3, ped);

    const auto task = [&, this]() -> CTask* {
        switch (m_attractor->GetType()) {
        case PED_ATTRACTOR_ATM:
            return new CTaskSimpleUseAtm{};
        case PED_ATTRACTOR_SEAT:
        case PED_ATTRACTOR_STEP: {
            const auto duration = CGeneral::GetRandomIntInRange(0, 100) > 40 ? gDefaultTaskTime : CGeneral::GetRandomIntInRange(15'000, 60'000);
            // El original vuelve a consultar el tipo después de reservar la tarea.
            return new CTaskComplexSitDownThenIdleThenStandUp{ duration, m_attractor->GetType() == PED_ATTRACTOR_STEP, !!ped->bUseAttractorInstantly };
        }
        case PED_ATTRACTOR_STOP:
            return new CTaskComplexWaitForBus{};
        case PED_ATTRACTOR_PIZZA: {
            const auto duration = CGeneral::GetRandomIntInRange(6000, 12'000);
            return new CTaskSimpleStandStill{ duration };
        }
        case PED_ATTRACTOR_SHELTER:
            return new CTaskComplexWaitForDryWeather{};
        case PED_ATTRACTOR_TRIGGER_SCRIPT: {
            if (ped->bHasAScriptBrain) {
                return nullptr;
            }
            if (!CTheScripts::ScriptsForBrains.HasAttractorScriptBrainWithThisNameLoaded(m_attractor->m_ScriptName)) {
                return nullptr;
            }
            CEntity* const entity = m_attractor->m_Entity;
            if (!entity) {
                return nullptr;
            }
            // Un objeto sin atractores habilitados no arranca el script, pero la tarea se crea igual.
            if (!entity->GetIsTypeObject() || entity->AsObject()->objectFlags.bEnableDisabledAttractors) {
                CTheScripts::ScriptsForBrains.StartAttractorScriptBrainWithThisName(m_attractor->m_ScriptName, ped, ped->bUseAttractorInstantly);
            }
            CScriptedBrainTaskStore::Clear(ped);
            return new CTaskComplexUseScriptedBrain{ m_attractor->m_ScriptName };
        }
        case PED_ATTRACTOR_LOOK_AT: {
            const auto duration = CGeneral::GetRandomIntInRange(3000, 6000);
            return new CTaskSimpleStandStill{ duration };
        }
        case PED_ATTRACTOR_SCRIPTED:
            return new CTaskComplexUseScriptedAttractor{ *m_attractor };
        case PED_ATTRACTOR_PARK:
            return new CTaskComplexSunbathe{ nullptr, true };
        default:
            return nullptr;
        }
    }();

    if (!task) {
        GetPedAttractorManager()->BroadcastDeparture(ped, m_attractor);
    }
    ped->bUseAttractorInstantly = false;
    return task;
}

// 0x6327C0
CTask* CTaskComplexUseAttractor::ControlSubTask(CPed* ped) {
    if (m_attractor) {
        if (CEntity* const entity = m_attractor->m_Entity) {
            if (entity->GetIsTypeObject() && entity->AsObject()->objectFlags.bIsBroken) {
                m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
            }
        }
    }
    return m_pSubTask;
}
