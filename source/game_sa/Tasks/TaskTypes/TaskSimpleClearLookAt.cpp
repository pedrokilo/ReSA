/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleClearLookAt
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
#include "TaskSimpleClearLookAt.h"
#include "Ragdoll/IKChainManager.h"

namespace {
// 0x6346A0 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskSimpleClearLookAt del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x6346A0` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleClearLookAtDestructorBody(CTaskSimpleClearLookAt* self, void* /*edx*/) { self->CTaskSimpleClearLookAt::~CTaskSimpleClearLookAt(); }

// 0x6394F0 (lote 157A, banco Fn157A): destructor con borrado de CTaskSimpleClearLookAt del EXE (ranura 0 de su vtable 0x86E404; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6346A0; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleClearLookAt* __fastcall CTaskSimpleClearLookAtScalarDeletingDestructor(CTaskSimpleClearLookAt* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleClearLookAtDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskSimpleClearLookAt() { return reinterpret_cast<uintptr_t>(&CTaskSimpleClearLookAtDestructorBody); } // para el banco Fn157A

void CTaskSimpleClearLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleClearLookAt, 0x86E404, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleClearLookAtScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6394F0); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x634670);
    RH_ScopedInstall(Destructor, 0x6346A0);

    RH_ScopedVMTInstall(Clone, 0x636F20);
    RH_ScopedVMTInstall(GetTaskType, 0x634690);
    RH_ScopedVMTInstall(MakeAbortable, 0x6346B0);
    RH_ScopedVMTInstall(ProcessPed, 0x6346C0);
}

// 0x6346C0
bool CTaskSimpleClearLookAt::ProcessPed(CPed* ped) {
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 500u);
    }
    return true;
}
