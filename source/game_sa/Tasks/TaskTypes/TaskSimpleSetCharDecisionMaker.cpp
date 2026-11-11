/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetCharDecisionMaker
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
#include "TaskSimpleSetCharDecisionMaker.h"

void CTaskSimpleSetCharDecisionMaker::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetCharDecisionMaker, 0x859da0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x46A470);
    RH_ScopedInstall(Destructor, 0x46AF10);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x46AEF0); // lote 166A (banco Fn166A)

    RH_ScopedVMTInstall(Clone, 0x46A490);
    RH_ScopedVMTInstall(GetTaskType, 0x46A4F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x46A500);
    RH_ScopedVMTInstall(ProcessPed, 0x635430);
}

// 0x46AEF0 (lote 166A, banco Fn166A): destructor con borrado de CTaskSimpleSetCharDecisionMaker del EXE (ranura 0 de su vtable 0x859DA0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x46AF10; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x46AF10), libera `this` con
// CTask::operator delete (0x61A5B0) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleSetCharDecisionMaker* CTaskSimpleSetCharDecisionMaker::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x46A470
CTaskSimpleSetCharDecisionMaker::CTaskSimpleSetCharDecisionMaker(uint32 decisionMaker) :
    m_decisionMaker{ decisionMaker }
{
}

CTaskSimpleSetCharDecisionMaker::CTaskSimpleSetCharDecisionMaker(const CTaskSimpleSetCharDecisionMaker& o) :
    CTaskSimpleSetCharDecisionMaker{o.m_decisionMaker}
{
}

// 0x635430
bool CTaskSimpleSetCharDecisionMaker::ProcessPed(CPed* ped) {
    ped->GetIntelligence()->SetPedDecisionMakerType(m_decisionMaker);
    return true;
}
