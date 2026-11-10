/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveCarAndDie
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
#include "TaskComplexLeaveCarAndDie.h"

namespace {
// 0x62F270 (lote 152A, banco Fn152A; thiscall, sin argumentos de pila, 11 bytes `mov [ecx],86DD00h; jmp 63B970`; el listado lo atribuia a CTaskComplexLeaveCarAndDie): cuerpo del destructor de la clase del EXE. Como el
// original: deja la vtable de la clase del EXE (0x86DD00) y sigue con el cuerpo de ~CTaskComplexLeaveCar (0x63B970, con hook propio; llamada no virtual en una funcion propia sin expansion en linea para que el banco
// pueda sustituirla; el EXE salta en lugar de llamar y la funcion sale con lo que devuelva ese cuerpo, que ningun llamador lee).
__declspec(noinline) void __fastcall LeaveCarAndDieBaseDestructorCall(CTaskComplexLeaveCarAndDie* self, void* /*edx*/) { self->CTaskComplexLeaveCar::~CTaskComplexLeaveCar(); }
__declspec(noinline) void __fastcall LeaveCarAndDieDestructorBody(CTaskComplexLeaveCarAndDie* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DD00;
    LeaveCarAndDieBaseDestructorCall(self, nullptr);
}
} // namespace

uintptr_t Lote152ABody_LeaveCarBaseDestructor() { return reinterpret_cast<uintptr_t>(&LeaveCarAndDieBaseDestructorCall); } // para el banco Fn152A

void CTaskComplexLeaveCarAndDie::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndDie, 0x86dd00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F1A0);
    RH_ScopedNamedGlobalInstall(LeaveCarAndDieDestructorBody, "DestructorBody", 0x62F270); // lote 152A (banco Fn152A)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x62F250); // lote 152A (banco Fn152A)
    RH_ScopedVMTInstall(Clone, 0x62F1D0);
    RH_ScopedVMTInstall(GetTaskType, 0x62F240);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642280);
}

// 0x62F1A0
// Como el original: constructor de CTaskComplexLeaveCar (sin salir sensato, forzando la salida) con la vtable de esta clase
CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime) :
    CTaskComplexLeaveCar{targetVehicle, nTargetDoor, nDelayTime, false, true}
{
    m_bDie = true;
}

// NOTSA
CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie(const CTaskComplexLeaveCarAndDie& o) :
    CTaskComplexLeaveCar{o.m_pTargetVehicle, o.m_nTargetDoor, o.m_nDelayTime, false, true}
{
    m_bDie = true;
}

// 0x642280
CTask* CTaskComplexLeaveCarAndDie::CreateFirstSubTask(CPed* ped) {
    ped->m_nPedState = PEDSTATE_DIE; // Don't use `SetPedState` because it has side-effects
    return CTaskComplexLeaveCar::CreateFirstSubTask(ped);
}

// 0x62F250 (lote 152A, banco Fn152A): destructor con borrado de CTaskComplexLeaveCarAndDie del EXE (ranura 0 de su vtable 0x86DD00; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x62F270; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x62F270),
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexLeaveCarAndDie* CTaskComplexLeaveCarAndDie::ScalarDeletingDestructor(uint8 flags) {
    LeaveCarAndDieDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
