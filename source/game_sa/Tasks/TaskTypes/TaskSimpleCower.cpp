/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCower
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleCower.h"

namespace {
// 0x48E900 (con hook propio (Destructor); `jmp 0x61A6F0` = ~CTaskSimpleAnim): cuerpo del destructor de CTaskSimpleCower del EXE. Funcion propia sin expansion en linea para que el banco Fn149A pueda
// sustituirla; la llama el destructor con borrado como el `call 0x48E900` del EXE.
__declspec(noinline) void __fastcall CowerDestructorBody(CTaskSimpleCower* self, void* /*edx*/) { self->CTaskSimpleCower::~CTaskSimpleCower(); }

// 0x48E8E0 (lote 149A, banco Fn149A): destructor con borrado de CTaskSimpleCower del EXE (ranura 0 de su vtable 0x85A0D0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48E900; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleCower* __fastcall CowerScalarDeletingDestructor(CTaskSimpleCower* self, void* /*edx*/, uint8 flags) {
    CowerDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote149ABody_CTaskSimpleCower() { return reinterpret_cast<uintptr_t>(&CowerDestructorBody); } // para el banco Fn149A

void CTaskSimpleCower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCower, 0x85a0d0, 10);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CowerScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48E8E0); // lote 149A (banco Fn149A)

    RH_ScopedInstall(Constructor, 0x48DE70);
    RH_ScopedInstall(Destructor, 0x48E900);

    RH_ScopedVMTInstall(Clone, 0x48DEB0);
    RH_ScopedVMTInstall(IsInterruptable, 0x48DF20);
}
