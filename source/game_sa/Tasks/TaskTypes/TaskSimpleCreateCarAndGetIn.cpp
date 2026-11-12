/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCreateCarAndGetIn
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

#include "TaskSimpleCreateCarAndGetIn.h"
#include "PedPlacement.h"
#include "TaskSimpleStandStill.h"
#include "CarCtrl.h"
#include "TaskSimpleCarSetPedInAsDriver.h"

namespace {
// 0x64CEA0 (con hook propio (Destructor)): cuerpo del destructor de CTaskSimpleCreateCarAndGetIn del EXE. Funcion propia sin expansion en linea para que el banco Fn171A pueda sustituirla; la llama el
// destructor con borrado como el `call 0x64CEA0` del EXE.
__declspec(noinline) void __fastcall CreateCarAndGetInDestructorBody(CTaskSimpleCreateCarAndGetIn* self, void* /*edx*/) { self->CTaskSimpleCreateCarAndGetIn::~CTaskSimpleCreateCarAndGetIn(); }

// 0x64E480 (lote 171A, banco Fn171A): destructor con borrado de CTaskSimpleCreateCarAndGetIn del EXE (ranura 0 de su vtable 0x86F070; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x64CEA0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleCreateCarAndGetIn* __fastcall CreateCarAndGetInScalarDeletingDestructor(CTaskSimpleCreateCarAndGetIn* self, void* /*edx*/, uint8 flags) {
    CreateCarAndGetInDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote171ABody_CTaskSimpleCreateCarAndGetIn() { return reinterpret_cast<uintptr_t>(&CreateCarAndGetInDestructorBody); } // para el banco Fn171A

void CTaskSimpleCreateCarAndGetIn::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCreateCarAndGetIn, 0x86F070, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(CreateCarAndGetInScalarDeletingDestructor, "ScalarDeletingDestructor", 0x64E480); // lote 171A (banco Fn171A)

    RH_ScopedInstall(Constructor, 0x6493E0);
    RH_ScopedInstall(Destructor, 0x64CEA0);
    RH_ScopedVMTInstall(Clone, 0x64A410);
    RH_ScopedVMTInstall(GetTaskType, 0x649430);
    RH_ScopedVMTInstall(MakeAbortable, 0x649440);
    RH_ScopedVMTInstall(ProcessPed, 0x64CF40);
}

// 0x6493E0
CTaskSimpleCreateCarAndGetIn::CTaskSimpleCreateCarAndGetIn(const CVector& pos, int32 model) : CTaskSimple() {
    m_Pos            = pos;
    m_CreatedVeh     = nullptr;
    m_nModel         = model;
    m_nTimeMs        = 0;
    m_nWaitTime      = 0;
    m_bWaitTimeSet   = false;
    m_bResetWaitTime = false;
}

// 0x64CEA0
CTaskSimpleCreateCarAndGetIn::~CTaskSimpleCreateCarAndGetIn() {
    if (m_CreatedVeh) {
        CleanupCreatedVehicle(); // Como el original: el destructor no pone m_CreatedVeh a nulo
    }
}

// 0x649440
bool CTaskSimpleCreateCarAndGetIn::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return true;
}

// 0x64CF40
bool CTaskSimpleCreateCarAndGetIn::ProcessPed(CPed* ped) {
    CVector nodePos{};
    if (!ThePaths.FindNodeCoorsForScript(nodePos, ThePaths.FindNodeClosestToCoors(m_Pos, PATH_TYPE_VEH, 999999.88f, 1, 1, 0, 0, 0))) {
        if (!m_bWaitTimeSet) {
            m_nTimeMs = CTimer::GetTimeInMS();
            m_nWaitTime = 2000;
            m_bWaitTimeSet = true;
        }
        if (m_bResetWaitTime) {
            m_nTimeMs = CTimer::GetTimeInMS();
            m_bResetWaitTime = false;
        }
        return CTimer::GetTimeInMS() >= m_nTimeMs + m_nWaitTime;
    }

    m_VehCreationPos = nodePos;
    m_bWaitTimeSet = false;

    // If vehicle is not created yet lets check if it can be created at all
    if (!m_CreatedVeh) {
        // Make sure position isn't visible by camera
        if (TheCamera.IsSphereVisible({ m_VehCreationPos, 3.f })) {
            return true;
        }

        // Also make sure peds position isn't visible by the camera
        if (TheCamera.IsSphereVisible({ ped->GetPosition(), ped->GetColModel()->GetBoundRadius() })) {
            return true;
        }

        // And make sure the ped isn't stuck or something
        if (!CPedPlacement::IsPositionClearForPed(ped->GetPosition(), 3.f, -1, 0, true, true, true)) {
            return true;
        }
    }

    // Weird..
    {
        CTaskSimpleStandStill task{ 0, 0, 0, 8.f };
        task.ProcessPed(ped);
    }

    // Make sure model is loaded
    if (!CStreaming::IsModelLoaded(m_nModel)) {
        CStreaming::RequestModel(m_nModel, STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
        return false;
    }

    // Create vehicle and set ped in as driver
    if (!m_CreatedVeh) {
        m_CreatedVeh = CCarCtrl::CreateCarForScript(m_nModel, m_VehCreationPos, true);

        // Como el original: la tarea se construye sin utilidad y luego se marca m_bWarpingInToCar = true
        CTaskSimpleCarSetPedInAsDriver task{ m_CreatedVeh, true, nullptr };
        task.ProcessPed(ped);
    }

    // Como el original: mientras el vehiculo siga estatico (esperando colision) se sigue esperando
    if (m_CreatedVeh->GetIsStatic()) {
        return false;
    }

    // El vehiculo ya no es estatico: se libera y la tarea termina
    CleanupCreatedVehicle();
    m_CreatedVeh = nullptr;
    return true;
}

// NOTSA
void CTaskSimpleCreateCarAndGetIn::CleanupCreatedVehicle() {
    CTheScripts::CleanUpThisVehicle(m_CreatedVeh);
    CTheScripts::MissionCleanUp.RemoveEntityFromList(GetVehiclePool()->GetRef(m_CreatedVeh), MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
}
