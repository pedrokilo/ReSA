/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDieInCar
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

#include "TaskSimpleDieInCar.h"
#include "TaskSimpleDie.h"

namespace {
// 0x62FC70 (lote 159A, banco Fn159A): cuerpo del destructor de CTaskSimpleDieInCar del EXE (con hook (Destructor); vtable 0x86DDE0). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el
// `call 0x62FC70` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleDieInCarDestructorBody(CTaskSimpleDieInCar* self, void* /*edx*/) { self->CTaskSimpleDieInCar::~CTaskSimpleDieInCar(); }

// 0x6375B0 (lote 159A, banco Fn159A): destructor con borrado de CTaskSimpleDieInCar del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62FC70; test byte [esp+8],1;
// je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0
// del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleDieInCar* __fastcall CTaskSimpleDieInCarScalarDeletingDestructor(CTaskSimpleDieInCar* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleDieInCarDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote159ABody_CTaskSimpleDieInCar() { return reinterpret_cast<uintptr_t>(&CTaskSimpleDieInCarDestructorBody); } // para el banco Fn159A

void CTaskSimpleDieInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDieInCar, 0x86DDE0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62FC20);
    RH_ScopedInstall(Destructor, 0x62FC70);
    RH_ScopedNamedGlobalInstall(CTaskSimpleDieInCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6375B0); // lote 159A (banco Fn159A)

    RH_ScopedVMTInstall(Clone, 0x635EF0);
    RH_ScopedVMTInstall(GetTaskType, 0x62FC60);
    RH_ScopedVMTInstall(ProcessPed, 0x6398F0);
}

// 0x62FC20
CTaskSimpleDieInCar::CTaskSimpleDieInCar(AssocGroupId groupId, AnimationId animId) : CTaskSimpleDie(groupId, animId, 4.0f, 0.0f) {
    // NOP
}

// 0x6398F0
bool CTaskSimpleDieInCar::ProcessPed(CPed* ped) {
    return CTaskSimpleDie::ProcessPed(ped);
}
