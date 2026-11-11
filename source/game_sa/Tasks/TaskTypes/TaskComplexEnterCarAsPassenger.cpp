/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterCarAsPassenger
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

#include "TaskComplexEnterCarAsPassenger.h"

namespace {
// 0x640380 (lote 122A, banco Fn122A): cuerpo no virtual de ~CTaskComplexEnterCarAsPassenger del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x86EADC ; jmp 0x63DFA0`).
// Deja la vtable de CTaskComplexEnterCarAsPassenger del EXE (0x86EADC) y salta a CTaskComplexEnterCar::Destructor (0x63DFA0). Lo llaman los funclets de limpieza (SEH) y el destructor con borrado.
__declspec(noinline) void __fastcall EnterCarAsPassengerDestructorBody(CTaskComplexEnterCarAsPassenger* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86EADC;
    self->CTaskComplexEnterCar::Destructor();
}

// 0x643C80 (lote 165A, banco Fn165A): destructor con borrado de CTaskComplexEnterCarAsPassenger del EXE (ranura 0 de su vtable 0x86EADC; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x640380; test byte
// [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x640380, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo
// si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexEnterCarAsPassenger* __fastcall EnterCarAsPassengerScalarDeletingDestructor(CTaskComplexEnterCarAsPassenger* self, void* /*edx*/, uint8 flags) {
    EnterCarAsPassengerDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexEnterCarAsPassenger::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsPassenger, 0x86EADC, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(EnterCarAsPassengerDestructorBody, "Destructor", 0x640380);
    RH_ScopedNamedGlobalInstall(EnterCarAsPassengerScalarDeletingDestructor, "ScalarDeletingDestructor", 0x643C80); // lote 165A (banco Fn165A)
    RH_ScopedInstall(Constructor, 0x640340);

    RH_ScopedVMTInstall(Clone, 0x6437F0);
    RH_ScopedVMTInstall(GetTaskType, 0x640370);
}

// 0x640340
CTaskComplexEnterCarAsPassenger::CTaskComplexEnterCarAsPassenger(CVehicle* targetVehicle, int32 nTargetSeat, bool bCarryOnAfterFallingOff)
    : CTaskComplexEnterCar(targetVehicle, false, false, false, bCarryOnAfterFallingOff)
{
    m_TargetSeat = nTargetSeat;
}

// For 0x6437F0
CTaskComplexEnterCarAsPassenger::CTaskComplexEnterCarAsPassenger(const CTaskComplexEnterCarAsPassenger& o) :
    CTaskComplexEnterCarAsPassenger{
        o.m_Car,
        o.m_TargetSeat,
        o.m_bCarryOnAfterFallingOff
    }
{
    m_MoveState = o.m_MoveState;
}
