/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleTriggerLookAt
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleTriggerLookAt.h"

namespace {
// 0x634500 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskSimpleTriggerLookAt del EXE (SIN hook; marco SEH: vtable 0x86E3CC, CleanUpOldReference de la entidad +8 y ~CTask). Se llama por su direccion del EXE; el banco lo sustituye por un falso
// identico en la direccion del EXE (la DLL llama a esa direccion).
__declspec(noinline) void __fastcall CTaskSimpleTriggerLookAtDestructorBody(CTaskSimpleTriggerLookAt* self, void* /*edx*/) { plugin::CallMethod<0x634500, CTaskSimpleTriggerLookAt*>(self); }

// 0x6394D0 (lote 157A, banco Fn157A): destructor con borrado de CTaskSimpleTriggerLookAt del EXE (ranura 0 de su vtable 0x86E3CC; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x634500; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleTriggerLookAt* __fastcall CTaskSimpleTriggerLookAtScalarDeletingDestructor(CTaskSimpleTriggerLookAt* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleTriggerLookAtDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskSimpleTriggerLookAt() { return reinterpret_cast<uintptr_t>(&CTaskSimpleTriggerLookAtDestructorBody); } // para el banco Fn157A

void CTaskSimpleTriggerLookAt::InjectHooks() {
    RH_ScopedClass(CTaskSimpleTriggerLookAt); // sin RH_ScopedVirtualClass: la clase no define su constructor, asi que la DLL no emite su vtable; vtable del EXE 0x86E3CC (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleTriggerLookAtScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6394D0); // lote 157A (banco Fn157A)

    // TODO: no hay funciones reconstruidas todavía (el constructor está declarado pero no definido).
}
