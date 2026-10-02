/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseScriptedAttractor
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
#include "TaskComplexUseScriptedAttractor.h"
#include "Scripted2dEffects.h"
#include "PedAttractor.h"

void CTaskComplexUseScriptedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseScriptedAttractor, 0x86e2dc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x633330);
    RH_ScopedInstall(Destructor, 0x633360);

    RH_ScopedVMTInstall(Clone, 0x636DD0);
    RH_ScopedVMTInstall(GetTaskType, 0x633350);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x633370);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6390C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x633380);
}

// 0x633330
CTaskComplexUseScriptedAttractor::CTaskComplexUseScriptedAttractor(const CPedAttractor& attractor) :
    m_attractor{ attractor }
{
}

CTaskComplexUseScriptedAttractor::CTaskComplexUseScriptedAttractor(const CTaskComplexUseScriptedAttractor& o) :
    CTaskComplexUseScriptedAttractor{o.m_attractor}
{
}

// 0x6390C0
CTask* CTaskComplexUseScriptedAttractor::CreateFirstSubTask(CPed* ped) {
    // Como el original: solo coincide un puntero exactamente igual al inicio de un efecto
    for (auto i = 0u; i < std::size(CScripted2dEffects::ms_effects); i++) {
        if (reinterpret_cast<const void*>(m_attractor.m_Fx) == &CScripted2dEffects::ms_effects[i]) {
            return new CTaskComplexUseSequence{ CScripted2dEffects::ms_effectSequenceTaskIDs[i] };
        }
    }
    return nullptr;
}
