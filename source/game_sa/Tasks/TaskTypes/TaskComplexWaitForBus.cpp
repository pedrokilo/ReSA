/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWaitForBus
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexWaitForBus.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskComplexEnterCarAsPassenger.h"

namespace {
// 0x632930 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexWaitForBus del EXE (con hook propio (Destructor); vtable 0x86E18C). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x632930` del EXE.
__declspec(noinline) void __fastcall CTaskComplexWaitForBusDestructorBody(CTaskComplexWaitForBus* self, void* /*edx*/) { self->CTaskComplexWaitForBus::~CTaskComplexWaitForBus(); }

// 0x638920 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexWaitForBus del EXE (ranura 0 de su vtable 0x86E18C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x632930; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexWaitForBus* __fastcall CTaskComplexWaitForBusScalarDeletingDestructor(CTaskComplexWaitForBus* self, void* /*edx*/, uint8 flags) {
    CTaskComplexWaitForBusDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexWaitForBus() { return reinterpret_cast<uintptr_t>(&CTaskComplexWaitForBusDestructorBody); } // para el banco Fn145A

void CTaskComplexWaitForBus::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForBus, 0x86e18c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexWaitForBusScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638920); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x632900);

    RH_ScopedInstall(Destructor, 0x632930);

    RH_ScopedVMTInstall(Clone, 0x636A60);
    RH_ScopedVMTInstall(GetTaskType, 0x632920);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632940);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638940);
    RH_ScopedVMTInstall(ControlSubTask, 0x632A40);
}

CTaskComplexWaitForBus::CTaskComplexWaitForBus(const CTaskComplexWaitForBus& o) :
    CTaskComplexWaitForBus{}
{
}

// 0x632940
CTask* CTaskComplexWaitForBus::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_WAIT_FOR_BUS: {
        m_bus = notsa::cast<CTaskSimpleWaitForBus>(m_pSubTask)->m_bus;
        return new CTaskComplexEnterCarAsPassenger{ m_bus, 0, false };
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: {
        ped->m_bRemoveFromWorld = true;
        ped->bDontDragMeOutCar = true;


        auto* player = FindPlayerPed();
        if ((player && m_bus->IsPassenger(player)) || m_bus->IsDriver(player)) {
            player->GetPlayerData()->m_nBusFaresCollected += 5;
        }

        return nullptr; // Got on the bus, so task is finito
    }
    default:
        return nullptr;
    }
}

// 0x638940
CTask* CTaskComplexWaitForBus::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleWaitForBus{};
}

CTask* CTaskComplexWaitForBus::Clone() const { return new CTaskComplexWaitForBus{ *this }; }
eTaskType CTaskComplexWaitForBus::GetTaskType() const { return Type; }
CTask* CTaskComplexWaitForBus::ControlSubTask(CPed* ped) { return m_pSubTask; }
