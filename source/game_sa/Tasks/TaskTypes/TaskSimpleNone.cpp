/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleNone
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

#include "TaskSimpleNone.h"

void CTaskSimpleNone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleNone, 0x859CFC, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4635F0);

    // Destructor 0x463560: el EXE comparte esta función con CTaskSimpleLeaveGroup (plegado
    // de funciones idénticas) y ya se instala allí; instalarla dos veces dispara el assert
    // de dirección duplicada en ReversibleHooks.

    RH_ScopedVMTInstall(Clone, 0x463570);
    RH_ScopedVMTInstall(GetTaskType, 0x4635C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x4635D0);
    RH_ScopedVMTInstall(ProcessPed, 0x4635E0);
}

namespace {
// 0x463560 (lote 128A, banco Fn128A): cuerpo del destructor de CTaskSimpleNone del EXE, sin hook propio (`mov dword [ecx],0x859CFC; jmp CTask::Destructor (0x61A3A0)`, thiscall sin argumentos). Funcion propia sin expansion
// en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x463560` del EXE.
__declspec(noinline) void __fastcall SimpleNoneDestructorBody(CTaskSimpleNone* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x859CFC;
    self->CTask::Destructor();
}
} // namespace

uintptr_t Lote128ASimpleNoneDestructorBody() { return reinterpret_cast<uintptr_t>(&SimpleNoneDestructorBody); } // para el banco Fn128A

// 0x4635F0 (lote 128A, banco Fn128A) : destructor con borrado de CTaskSimpleNone del EXE (ranura 0 de su vtable 0x859CFC; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x463560; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x463560), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleNone* CTaskSimpleNone::ScalarDeletingDestructor(uint8 flags) {
    SimpleNoneDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
