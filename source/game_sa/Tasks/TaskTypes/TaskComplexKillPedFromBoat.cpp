/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillPedFromBoat
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 5.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   GetTaskType | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexKillPedFromBoat.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexKillPedOnFoot.h"

void CTaskComplexKillPedFromBoat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedFromBoat, 0x86da98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6227C0);
    RH_ScopedInstall(Destructor, 0x622830);

    RH_ScopedVMTInstall(Clone, 0x6238A0);
    RH_ScopedVMTInstall(GetTaskType, 0x622820);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x622890);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x622900);
    RH_ScopedVMTInstall(ControlSubTask, 0x622980);

}

// 0x6227C0
CTaskComplexKillPedFromBoat::CTaskComplexKillPedFromBoat(CPed* ped) {
    m_Ped = ped;
    CEntity::SafeRegisterRef(m_Ped);
}

// 0x622830
CTaskComplexKillPedFromBoat::~CTaskComplexKillPedFromBoat() {
    CEntity::SafeCleanUpRef(m_Ped);
}

// 0x6238A0
CTask* CTaskComplexKillPedFromBoat::Clone() const {
    return new CTaskComplexKillPedFromBoat(m_Ped);
}

// 0x622890
CTask* CTaskComplexKillPedFromBoat::CreateNextSubTask(CPed* ped) {
    return new CTaskSimpleStandStill(0, true, false, 8.0f);
}

// 0x622900
CTask* CTaskComplexKillPedFromBoat::CreateFirstSubTask(CPed* ped) {
    ped->bStayInSamePlace = true;
    return new CTaskSimpleStandStill(0, true, false, 8.0f);
}

// 0x622980
CTask* CTaskComplexKillPedFromBoat::ControlSubTask(CPed* ped) {
    auto* result = m_pSubTask;
    const auto type = result->GetTaskType();
    if (type == static_cast<eTaskType>(0x3E8)) {
        if (ped->m_nPedType == PED_TYPE_COP && m_Ped && m_Ped->IsPlayer()) {
            if (m_Ped->bInVehicle && m_Ped->m_pVehicle) {
                if (reinterpret_cast<const uint8*>(m_Ped->m_pVehicle)[0x1B8] == 3) {
                    AudioEngine.SayPedless(static_cast<eAudioEvents>(0x34), static_cast<eGlobalSpeechContext>(0xAD), ped, 0, 1.0f, false, false, false);
                }
            } else if (m_Ped->physicalFlags.bSubmergedInWater) {
                AudioEngine.SayPedless(static_cast<eAudioEvents>(0x34), static_cast<eGlobalSpeechContext>(0xAF), ped, 0, 1.0f, false, false, false);
            }
        }
        if (ped->m_nPedType == PED_TYPE_COP && m_Ped && m_Ped->IsPlayer() && FindPlayerWanted(-1)->m_WantedLevel == static_cast<eWantedLevel>(0) && result->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            result = new CTaskSimpleStandStill(0, true, false, 8.0f);
        }
    } else if (type == static_cast<eTaskType>(0xCB)) {
        if (ped->m_nPedType == PED_TYPE_COP && m_Ped && m_Ped->IsPlayer() && FindPlayerWanted(-1)->m_WantedLevel != static_cast<eWantedLevel>(0) && result->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            result = new CTaskComplexKillPedOnFoot(m_Ped, -1, 0, 0, 0, 1);
        }
    }
    return result;
}
