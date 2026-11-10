/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitForBus
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
#include "TaskSimpleWaitForBus.h"

namespace {
// 0x632A90 (con hook propio (Destructor); `mov [ecx],0x86E1B8; jmp 0x61A3A0`): cuerpo del destructor de CTaskSimpleWaitForBus del EXE. Funcion propia sin expansion en linea para que el banco Fn149A pueda
// sustituirla; la llama el destructor con borrado como el `call 0x632A90` del EXE.
__declspec(noinline) void __fastcall WaitForBusDestructorBody(CTaskSimpleWaitForBus* self, void* /*edx*/) { self->CTaskSimpleWaitForBus::~CTaskSimpleWaitForBus(); }

// 0x6389B0 (lote 149A, banco Fn149A): destructor con borrado de CTaskSimpleWaitForBus del EXE (ranura 0 de su vtable 0x86E1B8; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x632A90; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleWaitForBus* __fastcall WaitForBusScalarDeletingDestructor(CTaskSimpleWaitForBus* self, void* /*edx*/, uint8 flags) {
    WaitForBusDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote149ABody_CTaskSimpleWaitForBus() { return reinterpret_cast<uintptr_t>(&WaitForBusDestructorBody); } // para el banco Fn149A

void CTaskSimpleWaitForBus::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitForBus, 0x86e1b8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(WaitForBusScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6389B0); // lote 149A (banco Fn149A)

    RH_ScopedInstall(Constructor, 0x632A50);
    RH_ScopedInstall(Destructor, 0x632A90);

    RH_ScopedVMTInstall(Clone, 0x636AD0);
    RH_ScopedVMTInstall(GetTaskType, 0x632A70);
    RH_ScopedVMTInstall(MakeAbortable, 0x632A80);
    RH_ScopedVMTInstall(ProcessPed, 0x632AA0);
}

CTaskSimpleWaitForBus::CTaskSimpleWaitForBus(const CTaskSimpleWaitForBus& o) :
    CTaskSimpleWaitForBus{}
{
}

// 0x632AA0
bool CTaskSimpleWaitForBus::ProcessPed(CPed* ped) {
    // Como el original: m_bus solo se escribe al encontrar un autobús; si no hay ninguno devuelve false
    for (auto* const entity : ped->GetIntelligence()->GetVehicleScanner().m_apEntities) {
        const auto veh = static_cast<CVehicle*>(entity);
        if (!veh || !veh->vehicleFlags.bIsBus) {
            continue;
        }
        // Como el original: primero la velocidad (comparación estricta) y después las plazas libres
        if (!(sq(0.005f) > veh->m_vecMoveSpeed.SquaredMagnitude())) {
            continue;
        }
        if (veh->HasSpaceForAPassenger()) {
            m_bus = veh;
            return true;
        }
    }
    return false;
}
