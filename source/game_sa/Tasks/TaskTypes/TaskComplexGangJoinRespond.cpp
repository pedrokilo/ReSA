/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGangJoinRespond
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
 *   MakeAbortable | 0x00000000 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   MakeAbortable | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGangJoinRespond.h"
// #include "TaskComplexTurnToFaceEntityOrCoord.h"

void CTaskComplexGangJoinRespond::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangJoinRespond, 0x86FB5C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6616F0);
    RH_ScopedInstall(Destructor, 0x661720);

    RH_ScopedVMTInstall(Clone, 0x662290, { .reversed = false });
    RH_ScopedVMTInstall(MakeAbortable, 0x661790, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6617A0, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6618D0, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x661950, { .reversed = false });
}

// 0x6616F0
CTaskComplexGangJoinRespond::CTaskComplexGangJoinRespond(bool response) :
    m_response{response}
{
}

// 0x661720
CTaskComplexGangJoinRespond::~CTaskComplexGangJoinRespond() {
    if (m_animsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_animsReferenced = false;
    }
}

// 0x6617A0
CTask* CTaskComplexGangJoinRespond::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6617A0, CTaskComplexGangJoinRespond*, CPed*>(this, ped);
}

// 0x6618D0
CTask* CTaskComplexGangJoinRespond::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6618D0, CTaskComplexGangJoinRespond*, CPed*>(this, ped);
    // b10 = 0;
    // return new CTaskComplexTurnToFaceEntityOrCoord(FindPlayerPed(0), 0.5f, 0.2f);
}

// 0x661950
CTask* CTaskComplexGangJoinRespond::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x661950, CTaskComplexGangJoinRespond*, CPed*>(this, ped);
}
