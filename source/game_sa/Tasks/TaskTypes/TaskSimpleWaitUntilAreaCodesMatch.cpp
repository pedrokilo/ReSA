/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitUntilAreaCodesMatch
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleWaitUntilAreaCodesMatch.h"
#include "EntryExitManager.h"

namespace {
// 0x6355E0 (lote 147A, banco Fn147A): cuerpo del destructor de CTaskSimpleWaitUntilAreaCodesMatch del EXE (con hook propio (Destructor); vtable 0x86E544). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x6355E0` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleWaitUntilAreaCodesMatchDestructorBody(CTaskSimpleWaitUntilAreaCodesMatch* self, void* /*edx*/) { self->CTaskSimpleWaitUntilAreaCodesMatch::~CTaskSimpleWaitUntilAreaCodesMatch(); }

// 0x6397A0 (lote 147A, banco Fn147A): destructor con borrado de CTaskSimpleWaitUntilAreaCodesMatch del EXE (ranura 0 de su vtable 0x86E544; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6355E0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleWaitUntilAreaCodesMatch* __fastcall CTaskSimpleWaitUntilAreaCodesMatchScalarDeletingDestructor(CTaskSimpleWaitUntilAreaCodesMatch* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleWaitUntilAreaCodesMatchDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote147ABody_CTaskSimpleWaitUntilAreaCodesMatch() { return reinterpret_cast<uintptr_t>(&CTaskSimpleWaitUntilAreaCodesMatchDestructorBody); } // para el banco Fn147A

void CTaskSimpleWaitUntilAreaCodesMatch::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilAreaCodesMatch, 0x86e544, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleWaitUntilAreaCodesMatchScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6397A0); // lote 147A (banco Fn147A)

    RH_ScopedInstall(Constructor, 0x635540);
    RH_ScopedInstall(Destructor, 0x6355E0);

    RH_ScopedVMTInstall(Clone, 0x637190);
    RH_ScopedVMTInstall(GetTaskType, 0x6355C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6355D0);
    RH_ScopedVMTInstall(ProcessPed, 0x635640);
}

// 0x635540
CTaskSimpleWaitUntilAreaCodesMatch::CTaskSimpleWaitUntilAreaCodesMatch(CPed* pedToWaitFor) :
    CTaskSimpleStandStill{1000, true, false, 8.f},
    m_pedToWaitFor{pedToWaitFor}
{
    CEntity::SafeRegisterRef(m_pedToWaitFor);
}

// 0x6355E0
CTaskSimpleWaitUntilAreaCodesMatch::~CTaskSimpleWaitUntilAreaCodesMatch() {
    CEntity::SafeCleanUpRef(m_pedToWaitFor);
}

// 0x635640
bool CTaskSimpleWaitUntilAreaCodesMatch::ProcessPed(CPed* ped) {
    if (!m_pedToWaitFor) {
        return true;
    }

    if (!CEntryExitManager::ms_exitEnterState) {
        if (m_pedToWaitFor->GetAreaCode() == ped->GetAreaCode()) {
            return true;
        }
    }

    return !ped->bInVehicle && CTaskSimpleStandStill::ProcessPed(ped);
}
