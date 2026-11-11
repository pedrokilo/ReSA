/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterCarAsDriver
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

#include "TaskComplexEnterCarAsDriver.h"

namespace {
// 0x640330 (lote 122A, banco Fn122A): cuerpo no virtual de ~CTaskComplexEnterCarAsDriver del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x86EAAC ; jmp 0x63DFA0`).
// Deja la vtable de CTaskComplexEnterCarAsDriver del EXE (0x86EAAC) y salta a CTaskComplexEnterCar::Destructor (0x63DFA0). Lo llaman los funclets de limpieza (SEH) y el destructor con borrado.
__declspec(noinline) void __fastcall EnterCarAsDriverDestructorBody(CTaskComplexEnterCarAsDriver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86EAAC;
    self->CTaskComplexEnterCar::Destructor();
}

// 0x643C60 (lote 165A, banco Fn165A): destructor con borrado de CTaskComplexEnterCarAsDriver del EXE (ranura 0 de su vtable 0x86EAAC; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x640330; test byte
// [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x640330, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo
// si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexEnterCarAsDriver* __fastcall EnterCarAsDriverScalarDeletingDestructor(CTaskComplexEnterCarAsDriver* self, void* /*edx*/, uint8 flags) {
    EnterCarAsDriverDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexEnterCarAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsDriver, 0x86EAAC, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(EnterCarAsDriverDestructorBody, "Destructor", 0x640330);
    RH_ScopedNamedGlobalInstall(EnterCarAsDriverScalarDeletingDestructor, "ScalarDeletingDestructor", 0x643C60); // lote 165A (banco Fn165A)
    RH_ScopedInstall(Constructor, 0x6402F0);

    RH_ScopedVMTInstall(Clone, 0x643780);
    RH_ScopedVMTInstall(GetTaskType, 0x640320);
}

// 0x6402F0
CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle) :
    CTaskComplexEnterCar(targetVehicle, true, false, false, false)
{
}

CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle, eMoveState moveState) : // NOTSA
    CTaskComplexEnterCarAsDriver{ targetVehicle }
{
    m_MoveState = moveState;
}

// For 0x643780
CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver(const CTaskComplexEnterCarAsDriver& o) :
    CTaskComplexEnterCarAsDriver{ o.m_Car }
{
    m_MoveState = o.m_MoveState;
}
