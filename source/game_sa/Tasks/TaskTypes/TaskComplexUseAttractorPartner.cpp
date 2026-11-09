/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseAttractorPartner
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

#include "Scripted2dEffects.h"
#include "TaskComplexUseAttractorPartner.h"

void CTaskComplexUseAttractorPartner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseAttractorPartner, 0x86e2b0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(ScalarDeletingDestructor, 0x638FD0); // lote 141A (banco Fn141A)

    RH_ScopedInstall(Constructor, 0x6332C0);
    RH_ScopedInstall(Destructor, 0x633300);

    RH_ScopedVMTInstall(Clone, 0x636D60);
    RH_ScopedVMTInstall(GetTaskType, 0x6332F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x633310);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638FF0);
    RH_ScopedVMTInstall(ControlSubTask, 0x633320);
}

// 0x6332C0
CTaskComplexUseAttractorPartner::CTaskComplexUseAttractorPartner(bool isLeader, const CScriptedEffectPair& pair) :
    m_IsLeader{ isLeader },
    m_CurrentFxPair{ &pair }
{ 
}

// 0x636D60
CTaskComplexUseAttractorPartner::CTaskComplexUseAttractorPartner(const CTaskComplexUseAttractorPartner& o) :
    CTaskComplexUseAttractorPartner{ o.m_IsLeader, *o.m_CurrentFxPair }
{
}

// 0x638FF0
CTask* CTaskComplexUseAttractorPartner::CreateFirstSubTask(CPed* ped) {
    return new CTaskComplexUseSequence{ m_CurrentFxPair->Effects[m_IsLeader ? 0 : 1].PartnerTask };
}

// 0x638FD0 (lote 141A, banco Fn141A): destructor con borrado de CTaskComplexUseAttractorPartner del EXE (ranura 0 de su vtable 0x86E2B0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x633300; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x633300; aqui su envoltura Destructor()), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexUseAttractorPartner* CTaskComplexUseAttractorPartner::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
