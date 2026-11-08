/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSay
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
#include "TaskSimpleSay.h"

void CTaskSimpleSay::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSay, 0x85a1dc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E360);
    RH_ScopedInstall(Destructor, 0x48E4E0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x48E4C0);

    RH_ScopedVMTInstall(Clone, 0x48E390);
    RH_ScopedVMTInstall(GetTaskType, 0x48E420);
    RH_ScopedVMTInstall(MakeAbortable, 0x48E430);
    RH_ScopedVMTInstall(ProcessPed, 0x48E440);
}

// 0x48E360
CTaskSimpleSay::CTaskSimpleSay(eGlobalSpeechContext gctx, int32 sayDuration) :
    m_Duration{sayDuration},
    m_GlobalSpeechContext{gctx}
{
}

// 0x48E440
bool CTaskSimpleSay::ProcessPed(CPed* ped) {
    m_Timer.StartIfNotAlready(m_Duration);
    ped->Say(m_GlobalSpeechContext);
    return m_Timer.IsOutOfTime();
}

// 0x48E4C0 (lote 128A, banco Fn128A) : destructor con borrado de CTaskSimpleSay del EXE (ranura 0 de su vtable 0x85A1DC; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48E4E0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x48E4E0), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleSay* CTaskSimpleSay::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
