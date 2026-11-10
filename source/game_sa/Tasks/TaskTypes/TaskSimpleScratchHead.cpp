/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleScratchHead
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleScratchHead.h"

namespace {
// 0x61A6F0 (cuerpo de ~CTaskSimpleAnim, sin hook): llamada no virtual al destructor de la base, en una funcion propia sin expansion en linea para que el banco Fn147A pueda sustituirla.
__declspec(noinline) void __fastcall ScratchHeadBaseDestructorCall(CTaskSimpleScratchHead* self, void* /*edx*/) { self->CTaskSimpleAnim::~CTaskSimpleAnim(); }

// 0x48E930 (lote 147A, banco Fn147A; thiscall, sin argumentos de pila, 5 bytes: `jmp 0x61A6F0`): cuerpo no virtual de ~CTaskSimpleScratchHead del EXE. La clase no tiene destructor propio: el del EXE es
// un salto directo al de CTaskSimpleAnim (0x61A6F0); no escribe ninguna vtable. Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall ScratchHeadDestructorBody(CTaskSimpleScratchHead* self, void* /*edx*/) { ScratchHeadBaseDestructorCall(self, nullptr); }

// 0x48E910 (lote 147A, banco Fn147A): destructor con borrado de CTaskSimpleScratchHead del EXE (ranura 0 de su vtable 0x85A100; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48E930; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x48E930),
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleScratchHead* __fastcall ScratchHeadScalarDeletingDestructor(CTaskSimpleScratchHead* self, void* /*edx*/, uint8 flags) {
    ScratchHeadDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote147AScratchHeadBaseDestructorCall() { return reinterpret_cast<uintptr_t>(&ScratchHeadBaseDestructorCall); } // para el banco Fn147A

void CTaskSimpleScratchHead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleScratchHead, 0x85A100, 10);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(ScratchHeadDestructorBody, "Destructor", 0x48E930);                         // lote 147A (banco Fn147A)
    RH_ScopedNamedGlobalInstall(ScratchHeadScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48E910); // lote 147A (banco Fn147A)

    RH_ScopedInstall(Constructor, 0x48DF30);
    RH_ScopedVMTInstall(Clone, 0x48DF60);
    RH_ScopedVMTInstall(IsInterruptable, 0x48DFD0);
}
