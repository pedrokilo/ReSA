/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillPedFromBoat
 * Reconstrucción: INCOMPLETA — 2/7 funciones reconstruidas.
 * Pendientes de reconstruir: 5.
 * Funciones bloqueadas: 5.
 * Validación registrada: 0/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   GetTaskType | 0x00000000 | V
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

void CTaskComplexKillPedFromBoat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedFromBoat, 0x86da98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6227C0);
    RH_ScopedInstall(Destructor, 0x622830);

    RH_ScopedVMTInstall(Clone, 0x6238A0, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x622820, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x622890, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x622900, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x622980, { .reversed = false });

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
    return plugin::CallMethodAndReturn<CTask*, 0x6238A0, const CTaskComplexKillPedFromBoat*>(this);
}

// 0x622890
CTask* CTaskComplexKillPedFromBoat::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x622890, CTaskComplexKillPedFromBoat*, CPed*>(this, ped);
}

// 0x622900
CTask* CTaskComplexKillPedFromBoat::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x622900, CTaskComplexKillPedFromBoat*, CPed*>(this, ped);
}

// 0x622980
CTask* CTaskComplexKillPedFromBoat::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x622980, CTaskComplexKillPedFromBoat*, CPed*>(this, ped);
}
