/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderCriminal
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderCriminal.h"
#include "CarEnterExit.h"
#include "Events/EventVehicleToSteal.h"
#include "PedIntelligence.h"

namespace {
// 0x48E720 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexWanderCriminal del EXE (con hook propio (Destructor); vtable 0x85A23C). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x48E720` del EXE.
__declspec(noinline) void __fastcall CTaskComplexWanderCriminalDestructorBody(CTaskComplexWanderCriminal* self, void* /*edx*/) { self->CTaskComplexWanderCriminal::~CTaskComplexWanderCriminal(); }

// 0x48E700 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexWanderCriminal del EXE (ranura 0 de su vtable 0x85A23C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48E720; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexWanderCriminal* __fastcall CTaskComplexWanderCriminalScalarDeletingDestructor(CTaskComplexWanderCriminal* self, void* /*edx*/, uint8 flags) {
    CTaskComplexWanderCriminalDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexWanderCriminal() { return reinterpret_cast<uintptr_t>(&CTaskComplexWanderCriminalDestructorBody); } // para el banco Fn145A

void CTaskComplexWanderCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderCriminal, 0x85A23C, 15);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexWanderCriminalScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48E700); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x48E610);
    RH_ScopedInstall(Destructor, 0x48E720);
    RH_ScopedInstall(LookForCarsToSteal, 0x66B4F0);

    RH_ScopedVMTInstall(Clone, 0x48E650);
    RH_ScopedVMTInstall(GetWanderType, 0x48E6F0);
    RH_ScopedVMTInstall(ScanForStuff, 0x670350);
}

// 0x48E610
CTaskComplexWanderCriminal::CTaskComplexWanderCriminal(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) : CTaskComplexWander(MoveState, Dir, bWanderSensibly) {
    // Como el original: el constructor pone a cero el temporizador (0x28..0x31) y m_nMinNextScanTime (0x34)
    m_nMinNextScanTime = 0;
}

// 0x670350
void CTaskComplexWanderCriminal::ScanForStuff(CPed* ped) {
    if (!m_TaskTimer.m_bStarted) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval  = 50;
        m_TaskTimer.m_bStarted   = true;
        // Como el original: el reloj se lee antes de llamar a rand(); rand() & 0xFFFF * 2^-15 * -30000 (x87 a 24 bits) -> _ftol2
        const auto now = CTimer::GetTimeInMS();
        const auto offset = static_cast<float>(CGeneral::GetRandomNumber()) * 3.0517578125e-05f * -30000.0f;
        m_nMinNextScanTime = now - static_cast<uint32>(static_cast<int32>(offset));
    }

    if (m_TaskTimer.m_bStopped) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_bStopped   = false;
    }

    if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + static_cast<uint32>(m_TaskTimer.m_nInterval)) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval  = 50;
        m_TaskTimer.m_bStarted   = true;
        if (CTimer::GetTimeInMS() >= m_nMinNextScanTime) {
            LookForCarsToSteal(ped);
        }
    }
}

// 0x66B4F0
void CTaskComplexWanderCriminal::LookForCarsToSteal(CPed* ped) {
    // Como el original: recorre los 16 vehiculos del escaner y se queda con el robable mas cercano (distancia al cuadrado
    // sumada como (dz*dz + dy*dy) + dx*dx, solo gana si es estrictamente menor)
    CVehicle* closest{};
    float     closestDistSq = std::bit_cast<float>(0x7F7FFFFFu);
    for (size_t i = 0; i < 16; i++) {
        auto* const veh = static_cast<CVehicle*>(ped->GetIntelligence()->GetVehicleEntities()[i]);
        if (!veh) {
            continue;
        }
        if (!CCarEnterExit::IsVehicleStealable(veh, ped)) {
            continue;
        }
        const auto& vehPos = veh->GetPosition();
        const auto& pedPos = ped->GetPosition();
        const float dx = pedPos.x - vehPos.x;
        const float dy = pedPos.y - vehPos.y;
        const float dz = pedPos.z - vehPos.z;
        const float distSq = dz * dz + dy * dy + dx * dx;
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest       = veh;
        }
    }
    if (!closest) {
        return;
    }

    m_nMinNextScanTime = CTimer::GetTimeInMS() + 30'000;
    // Como el original: solo con rand() * (1/32767) < 0.2
    if (static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL < 0.2f) {
        CEventVehicleToSteal event{ closest };
        ped->GetIntelligence()->GetEventGroup().Add(&event, false);
    }
}
