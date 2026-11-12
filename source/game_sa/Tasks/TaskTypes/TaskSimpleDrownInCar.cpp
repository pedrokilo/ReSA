/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDrownInCar
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleDrownInCar.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x62FF20 (lote 177A, sin validar): cuerpo del constructor de CTaskSimpleDrownInCar del EXE (thiscall, sin argumentos, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x61A390; mov [esi],86DE54h; mov eax,esi; pop esi; ret`). Llama
// al constructor de CTaskSimple (0x61A390: +4 = 0, vtable 0x86D4A8), deja la vtable de la clase del EXE (0x86DE54) y devuelve this. Sin mas miembros. Funcion propia (la clase usa el constructor generado por el compilador).
__declspec(noinline) CTaskSimpleDrownInCar* __fastcall DrownInCarConstructorBody(CTaskSimpleDrownInCar* self, void* /*edx*/) {
    CTaskSimpleBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x86DE54;
    return self;
}

// 0x62FF60 (lote 177A, sin validar): cuerpo del destructor de CTaskSimpleDrownInCar del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86DE54h; jmp 0x61A3A0`). Deja la vtable de la clase del EXE (0x86DE54)
// y sigue con el cuerpo de ~CTask (0x61A3A0, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x62FF60` del EXE.
__declspec(noinline) void __fastcall DrownInCarDestructorBody(CTaskSimpleDrownInCar* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DE54;
    self->CTask::Destructor();
}

// 0x6378F0 (lote 177A, sin validar): destructor con borrado de CTaskSimpleDrownInCar del EXE (ranura 0 de su vtable 0x86DE54; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62FF60;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x62FF60), libera `this` con CTask::operator delete (0x61A5B0, con hook)
// solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CTaskSimpleDrownInCar* __fastcall DrownInCarScalarDeletingDestructor(CTaskSimpleDrownInCar* self, void* /*edx*/, uint8 flags) {
    DrownInCarDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleDrownInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDrownInCar, 0x86DE54, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(DrownInCarConstructorBody, "Constructor", 0x62FF20);                       // lote 177A (sin validar)
    RH_ScopedNamedGlobalInstall(DrownInCarDestructorBody, "DestructorBody", 0x62FF60);                     // lote 177A (sin validar)
    RH_ScopedNamedGlobalInstall(DrownInCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6378F0); // lote 177A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x62FF40);                                                            // lote 177A (sin validar): retorno de constante
    RH_ScopedVMTInstall(MakeAbortable, 0x62FF50);                                                          // lote 177A (sin validar): siempre falso
    RH_ScopedVMTInstall(ProcessPed, 0x62FF70);
}

// 0x62FF40 (lote 177A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,0D8h`): TASK_SIMPLE_DROWN_IN_CAR (216); no lee el objeto (ranura 4 de la vtable 0x86DE54).
eTaskType CTaskSimpleDrownInCar::GetTaskType() const {
    return Type;
}

// 0x62FF50 (lote 177A, sin validar; thiscall, tres argumentos de pila, `ret 0Ch`, 5 bytes: `xor al,al`): siempre falso; no lee ni el objeto ni los argumentos (ranura 6).
bool CTaskSimpleDrownInCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return false;
}

// 0x62FF70
bool CTaskSimpleDrownInCar::ProcessPed(CPed* ped) {
    ped->SetPedState(PEDSTATE_DIE);
    ped->bIsInTheAir = false;

    if (ped == FindPlayerPed()) {
        CStats::IncrementStat(STAT_TIMES_DROWNED, 1.0f);
    }
    // Como el original: segunda llamada a FindPlayerPed antes de forzar la parada del vehículo (0x62FFC8)
    if (ped == FindPlayerPed()) {
        ped->m_pVehicle->SetStatus(STATUS_FORCED_STOP);
    }

    CEventDeath event(true);
    ped->GetEventGroup().Add(&event, false);
    return true;
}
