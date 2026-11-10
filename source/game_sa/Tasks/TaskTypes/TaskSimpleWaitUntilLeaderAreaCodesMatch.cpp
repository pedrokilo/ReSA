/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitUntilLeaderAreaCodesMatch
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleWaitUntilLeaderAreaCodesMatch.h"

namespace {
// 0x5F67E0 (lote 142A, banco Fn142A): cuerpo del destructor de CTaskSimpleWaitUntilLeaderAreaCodesMatch del EXE (`mov [ecx],0x86C7E8; jmp 0x6355E0` = ~CTaskSimpleWaitUntilAreaCodesMatch; sin hook propio: el
// destructor de la clase es el generado por el compilador). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x5F67E0` del EXE.
// Lote 147A (banco Fn147A): tambien se engancha como S en 0x5F67E0. Como el EXE (`mov [ecx],0x86C7E8; jmp 0x6355E0`, 11 bytes): deja la vtable de la clase del EXE (0x86C7E8) y sigue con el cuerpo de
// ~CTaskSimpleWaitUntilAreaCodesMatch (0x6355E0, con hook propio; llamada no virtual en una funcion propia sin expansion en linea para que el banco pueda sustituirla).
__declspec(noinline) void __fastcall LeaderAreaCodesMatchBaseDestructorCall(CTaskSimpleWaitUntilLeaderAreaCodesMatch* self, void* /*edx*/) { self->CTaskSimpleWaitUntilAreaCodesMatch::~CTaskSimpleWaitUntilAreaCodesMatch(); }
__declspec(noinline) void __fastcall LeaderAreaCodesMatchDestructorBody(CTaskSimpleWaitUntilLeaderAreaCodesMatch* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C7E8;
    LeaderAreaCodesMatchBaseDestructorCall(self, nullptr);
}
} // namespace

uintptr_t Lote147ALeaderAreaCodesMatchBaseDestructorCall() { return reinterpret_cast<uintptr_t>(&LeaderAreaCodesMatchBaseDestructorCall); } // para el banco Fn147A
uintptr_t Lote142ALeaderAreaCodesMatchDestructorBody() { return reinterpret_cast<uintptr_t>(&LeaderAreaCodesMatchDestructorBody); } // para el banco Fn142A

void CTaskSimpleWaitUntilLeaderAreaCodesMatch::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilLeaderAreaCodesMatch, 0x86c7e8, 9);
    RH_ScopedCategory("Tasks/TaskTypes"); 
    RH_ScopedNamedGlobalInstall(LeaderAreaCodesMatchDestructorBody, "Destructor", 0x5F67E0); // lote 147A (banco Fn147A)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x5F6880);

    RH_ScopedVMTInstall(Clone, 0x5F67F0);
    RH_ScopedVMTInstall(GetTaskType, 0x5F6870);
    RH_ScopedVMTInstall(MakeAbortable, 0x6356A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6356C0);
}

// 0x5F6880 (lote 142A, banco Fn142A) : destructor con borrado de CTaskSimpleWaitUntilLeaderAreaCodesMatch del EXE (ranura 0 de su vtable 0x86C7E8; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi;
// mov esi,ecx; call 0x5F67E0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5F67E0), libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleWaitUntilLeaderAreaCodesMatch* CTaskSimpleWaitUntilLeaderAreaCodesMatch::ScalarDeletingDestructor(uint8 flags) {
    LeaderAreaCodesMatchDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x6356A0
bool CTaskSimpleWaitUntilLeaderAreaCodesMatch::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    } else {
        ped->SetUsesCollision(true);
        return true;
    }
}

// 0x6356C0
bool CTaskSimpleWaitUntilLeaderAreaCodesMatch::ProcessPed(CPed* ped) {
    if (CTaskSimpleWaitUntilAreaCodesMatch::ProcessPed(ped)) {
        ped->SetUsesCollision(true);
        return true;
    }
    if (!m_b) {
        if (ped->GetTaskManager().Find<Type>()) {
            m_b = true;
            ped->SetUsesCollision(false);
        }
    }
    return false;
}
