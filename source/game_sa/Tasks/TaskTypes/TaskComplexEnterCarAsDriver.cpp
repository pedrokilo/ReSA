/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterCarAsDriver
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

#include "TaskComplexEnterCarAsDriver.h"

namespace {
// 0x640330 (lote 122A, banco Fn122A): cuerpo no virtual de ~CTaskComplexEnterCarAsDriver del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x86EAAC ; jmp 0x63DFA0`).
// Deja la vtable de CTaskComplexEnterCarAsDriver del EXE (0x86EAAC) y salta a CTaskComplexEnterCar::Destructor (0x63DFA0). Lo llaman los funclets de limpieza (SEH) y el destructor con borrado.
void __fastcall EnterCarAsDriverDestructorBody(CTaskComplexEnterCarAsDriver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86EAAC;
    self->CTaskComplexEnterCar::Destructor();
}
} // namespace

void CTaskComplexEnterCarAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsDriver, 0x86EAAC, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(EnterCarAsDriverDestructorBody, "Destructor", 0x640330);
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
