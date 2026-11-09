/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderMedic
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

#include "TaskComplexWanderMedic.h"

namespace CTaskComplexWanderBody {
void __fastcall Destructor(CTaskComplexWander* self, void* edx); // TaskComplexWander.cpp: 0x669D20
}

namespace {
// 0x658850 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CTaskComplexWanderMedic del EXE (`mov [ecx],0x86F48C; jmp 0x669D20`). Deja la vtable de
// CTaskComplexWanderMedic del EXE (0x86F48C) y sigue con el cuerpo de ~CTaskComplexWander (0x669D20, con hook). Sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall WanderMedicDestructorBody(CTaskComplexWanderMedic* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F48C;
    CTaskComplexWanderBody::Destructor(self, nullptr);
}
} // namespace

void CTaskComplexWanderMedic::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderMedic, 0x86F48C, 15);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(WanderMedicDestructorBody, "Destructor", 0x658850);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x658830);

    RH_ScopedInstall(Constructor, 0x658770);

    RH_ScopedVMTInstall(Clone, 0x6587A0);
    RH_ScopedVMTInstall(GetWanderType, 0x658810);
    RH_ScopedVMTInstall(ScanForStuff, 0x658820);
}

// 0x658770
CTaskComplexWanderMedic::CTaskComplexWanderMedic(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) : CTaskComplexWander(MoveState, Dir, bWanderSensibly) {
    // NOP
}

// 0x658830 (lote 142A, banco Fn142A) : destructor con borrado de CTaskComplexWanderMedic del EXE (ranura 0 de su vtable 0x86F48C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x658850; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x658850), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexWanderMedic* CTaskComplexWanderMedic::ScalarDeletingDestructor(uint8 flags) {
    WanderMedicDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
