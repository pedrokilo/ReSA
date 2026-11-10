/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHandsUp
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
#include "TaskSimpleHandsUp.h"

namespace {
// 0x48EA60 (lote 151A, banco Fn151A): cuerpo del destructor de CTaskSimpleHandsUp del EXE (con hook propio, Destructor; vtable 0x85A29C). Funcion propia sin expansion en linea para que el banco pueda sustituirla;
// la llama el destructor con borrado como el `call 0x48EA60` del EXE.
__declspec(noinline) void __fastcall HandsUpDestructorBody(CTaskSimpleHandsUp* self, void* /*edx*/) { self->CTaskSimpleHandsUp::~CTaskSimpleHandsUp(); }

// 0x48EA40 (lote 151A, banco Fn151A; el listado decia CTaskSimpleHandsUp y lo es): destructor con borrado de CTaskSimpleHandsUp del EXE (ranura 0 de su vtable 0x85A29C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48EA60; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskSimpleHandsUp* __fastcall HandsUpScalarDeletingDestructor(CTaskSimpleHandsUp* self, void* /*edx*/, uint8 flags) {
    HandsUpDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote151ABody_CTaskSimpleHandsUp() { return reinterpret_cast<uintptr_t>(&HandsUpDestructorBody); } // para el banco Fn151A

void CTaskSimpleHandsUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHandsUp, 0x85a29c, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E970);
    RH_ScopedInstall(Destructor, 0x48EA60);
    RH_ScopedNamedGlobalInstall(HandsUpScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48EA40); // lote 151A (banco Fn151A)

    RH_ScopedVMTInstall(Clone, 0x48E9C0);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E9B0);
}
