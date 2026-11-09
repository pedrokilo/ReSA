/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFinishBrain
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
#include "TaskSimpleFinishBrain.h"

namespace {
// 0x4636E0 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CTaskSimpleFinishBrain del EXE (`mov [ecx],0x859CFC; jmp 0x61A3A0`): como la clase no tiene
// destructor propio, el del EXE deja la vtable de su base CTaskSimpleNone (0x859CFC) y sigue con el cuerpo de ~CTask (0x61A3A0, con hook). Sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall FinishBrainDestructorBody(CTaskSimpleFinishBrain* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x859CFC;
    self->CTask::Destructor();
}
} // namespace

void CTaskSimpleFinishBrain::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFinishBrain, 0x859d20, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(FinishBrainDestructorBody, "Destructor", 0x4636E0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4636C0);

    RH_ScopedVMTInstall(Clone, 0x463660);
    RH_ScopedVMTInstall(GetTaskType, 0x4636B0);
}

// 0x463660
CTaskSimpleFinishBrain::CTaskSimpleFinishBrain(const CTaskSimpleFinishBrain&) :
    CTaskSimpleFinishBrain{/* nothing to copy */}
{
}

// 0x4636C0 (lote 142A, banco Fn142A) : destructor con borrado de CTaskSimpleFinishBrain del EXE (ranura 0 de su vtable 0x859D20; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x4636E0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x4636E0), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleFinishBrain* CTaskSimpleFinishBrain::ScalarDeletingDestructor(uint8 flags) {
    FinishBrainDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
