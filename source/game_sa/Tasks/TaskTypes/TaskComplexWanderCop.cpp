/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderCop
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderCop.h"

#include "TaskSimpleStandStill.h"
#include "TaskComplexPolicePursuit.h"

namespace CTaskComplexWanderBody {
void __fastcall Destructor(CTaskComplexWander* self, void* edx); // TaskComplexWander.cpp: 0x669D20
}

namespace {
// 0x460D80 (lote 184A, sin validar; el listado decia CTaskComplexWanderCop): cuerpo no virtual de ~CTaskComplexWanderCop del EXE (thiscall, sin argumentos de pila, `ret`, 87 bytes con marco SEH que solo desenrolla).
// Deja la vtable de la clase del EXE (0x859C80), borra con el destructor virtual (ranura 0, bandera 1) la tarea guardada de ir a un punto (+0x28) si no es nula y sigue con el cuerpo de ~CTaskComplexWander (0x669D20, con hook).
// Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x460D80` del EXE. El marco SEH se omite.
__declspec(noinline) void __fastcall CTaskComplexWanderCopDestructorBody(CTaskComplexWanderCop* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x859C80;
    delete self->m_pGoToPointAndStandStillTask;
    CTaskComplexWanderBody::Destructor(self, nullptr);
}

// 0x460D60 (lote 184A, sin validar): destructor con borrado de CTaskComplexWanderCop del EXE (ranura 0 de su vtable 0x859C80; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x460D80;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x460D80), libera `this` con CTask::operator delete (0x61A5B0, con hook)
// solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CTaskComplexWanderCop* __fastcall CTaskComplexWanderCopScalarDeletingDestructor(CTaskComplexWanderCop* self, void* /*edx*/, uint8 flags) {
    CTaskComplexWanderCopDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexWanderCop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderCop, 0x859C80, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(CTaskComplexWanderCopDestructorBody, "DestructorBody", 0x460D80); // lote 184A (sin validar)
    RH_ScopedNamedGlobalInstall(CTaskComplexWanderCopScalarDeletingDestructor, "ScalarDeletingDestructor", 0x460D60); // lote 184A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x460CE0);          // lote 184A (sin validar): ranura 1
    RH_ScopedVMTInstall(GetWanderType, 0x460D50);  // lote 184A (sin validar): ranura 11
    RH_ScopedInstall(Constructor, 0x460C80);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x674860);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x674750);
    RH_ScopedVMTInstall(ControlSubTask, 0x674D80);
    RH_ScopedVMTInstall(ScanForStuff, 0x6702B0);
    RH_ScopedInstall(LookForCarAlarms, 0x66B1B0);
    RH_ScopedInstall(LookForStolenCopCars, 0x66B290);
    RH_ScopedInstall(LookForCriminals, 0x66B300);
    RH_ScopedInstall(ShouldPursuePlayer, 0x66B160);
}
CTaskComplexWanderCop* CTaskComplexWanderCop::Constructor(eMoveState moveState, uint8 dir) { this->CTaskComplexWanderCop::CTaskComplexWanderCop(moveState, dir); return this; }

// 0x460C80
CTaskComplexWanderCop::CTaskComplexWanderCop(eMoveState moveState, uint8 dir) : CTaskComplexWander(moveState, dir, true, 0.5f) {
    m_pGoToPointAndStandStillTask = nullptr;
    m_nTimePassedSinceLastLookedForCriminals = 0;
    m_nTimePassedSinceLastLookedForCarAlarmsAndStolenCopCars = 0;
    m_pLastCriminalPedLookedFor = nullptr;
}

// 0x460D80 (cuerpo) y 0x460D60 (con borrado)
CTaskComplexWanderCop::~CTaskComplexWanderCop() {
    delete m_pGoToPointAndStandStillTask;
}

// 0x460D50 (lote 184A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,1`): WANDER_TYPE_COP (1); no lee el objeto (ranura 11 de la vtable 0x859C80; el EXE comparte esta direccion con otra vtable (0x85A454) que tambien devuelve 1).
eWanderType CTaskComplexWanderCop::GetWanderType() {
    return WANDER_TYPE_COP;
}

// 0x460CE0 (lote 184A, sin validar; thiscall, `ret`, 102 bytes con marco SEH que solo desenrolla la reserva; ranura 1 de la vtable 0x859C80): reserva 0x50 bytes con CTask::operator new (0x61A5A0, pool de tareas); sin memoria devuelve
// nulo; si no, llama al constructor (0x460C80, con hook; ECX = memoria) con el estado de movimiento (+0xC) y la direccion (+0x10, BYTE extendido a cero) y devuelve lo que deja en EAX.
CTask* CTaskComplexWanderCop::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskComplexWanderCop));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskComplexWanderCop*>(memory)->Constructor(m_nMoveState, m_nDir);
}

// 0x674860
CTask* CTaskComplexWanderCop::CreateNextSubTask(CPed* ped) {
    if (ped->m_nPedType != PED_TYPE_COP)
        return CTaskComplexWander::CreateNextSubTask(ped);

    if (ped->AsCop()->m_bDontPursuit)
        return nullptr;

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_POLICE_PURSUIT) {
        m_nSubTaskCreatedTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_nSubTaskCreatedTimer.m_nInterval = 3000;
        m_nSubTaskCreatedTimer.m_bStarted = true;
        if (m_pGoToPointAndStandStillTask) {
            return m_pGoToPointAndStandStillTask->Clone();
        } else {
            return CTaskComplexWander::CreateFirstSubTask(ped);
        }
    }

    if (m_pGoToPointAndStandStillTask) {
        // Como el original: primero el tipo de la subtarea y despues el de la tarea guardada
        const auto subTaskType = m_pSubTask->GetTaskType();
        if (subTaskType == m_pGoToPointAndStandStillTask->GetTaskType()) {
            return nullptr;
        }
    }

    if (m_pGoToPointAndStandStillTask) {
        return m_pGoToPointAndStandStillTask->Clone();
    } else {
        return CTaskComplexWander::CreateNextSubTask(ped);
    }
}

// 0x674750
CTask* CTaskComplexWanderCop::CreateFirstSubTask(CPed* ped) {
    if (ped->m_nPedType != PED_TYPE_COP)
        return CTaskComplexWander::CreateFirstSubTask(ped);

    if (ped->AsCop()->m_bDontPursuit) {
        return new CTaskSimpleStandStill(100'000, true, false, 8.0f);
    }

    if (!ShouldPursuePlayer(ped)) {
        if (m_pGoToPointAndStandStillTask) {
            return m_pGoToPointAndStandStillTask->Clone();
        }
        return CTaskComplexWander::CreateFirstSubTask(ped);
    }

    return new CTaskComplexPolicePursuit; // Como el original: sin () (no se pone a cero el relleno 0xD..0xF)
}

// 0x674D80
CTask* CTaskComplexWanderCop::ControlSubTask(CPed* ped) {
    if (ped->m_nPedType != PED_TYPE_COP)
        return CTaskComplexWander::ControlSubTask(ped);

    if (!ShouldPursuePlayer(ped)) {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_POLICE_PURSUIT || m_pGoToPointAndStandStillTask) {
            return m_pSubTask;
        }
        return CTaskComplexWander::ControlSubTask(ped);
    }

    if (m_nSubTaskCreatedTimer.m_bStarted && !m_nSubTaskCreatedTimer.IsOutOfTime() || !m_pSubTask->MakeAbortable(ped)) {
        return m_pSubTask;
    }

    return new CTaskComplexPolicePursuit; // Como el original: sin () (no se pone a cero el relleno 0xD..0xF)
}

// 0x6702B0
void CTaskComplexWanderCop::ScanForStuff(CPed* ped) {
    if (!m_nScanForStuffTimer.m_bStarted) {
        m_nScanForStuffTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_nScanForStuffTimer.m_nInterval = 50;
        m_nScanForStuffTimer.m_bStarted = true;
    }

    // Como el original: se mira el tipo de la subtarea, no el de esta tarea
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_POLICE_PURSUIT || !m_nScanForStuffTimer.m_bStarted)
        return;

    if (m_nScanForStuffTimer.m_bStopped) {
        m_nScanForStuffTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_nScanForStuffTimer.m_bStopped = false;
    }

    if (CTimer::GetTimeInMS() >= (m_nScanForStuffTimer.m_nStartTime + m_nScanForStuffTimer.m_nInterval)) {
        m_nScanForStuffTimer.m_nInterval = 50;
        m_nScanForStuffTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_nScanForStuffTimer.m_bStarted = true;

        if (CTimer::GetTimeInMS() >= m_nTimePassedSinceLastLookedForCarAlarmsAndStolenCopCars) {
            LookForCarAlarms(ped);
            LookForStolenCopCars(ped);
        }

        if (CTimer::GetTimeInMS() >= m_nTimePassedSinceLastLookedForCriminals) {
            LookForCriminals(ped);
        }
    }
}

// 0x66B1B0
void CTaskComplexWanderCop::LookForCarAlarms(CPed* ped) {
    // Como el original: FindPlayerVehicle se llama en cada comprobacion
    if (!FindPlayerVehicle() || !FindPlayerVehicle()->IsAutomobile())
        return;

    CVehicle* vehicle = FindPlayerVehicle();
    if (!vehicle->m_nAlarmState || vehicle->m_nAlarmState == 0xFFFF || vehicle->GetStatus() == STATUS_WRECKED)
        return;

    // Como el original: (vehiculo - ped), suma (z*z + x*x) + y*y comparada con 20^2 (NaN no pasa)
    const auto& pedPos = ped->GetPosition();
    const auto& vehPos = FindPlayerVehicle()->GetPosition();
    const float dx = vehPos.x - pedPos.x, dy = vehPos.y - pedPos.y, dz = vehPos.z - pedPos.z;
    if (dz * dz + dx * dx + dy * dy < sq(20.0f)) {
        FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1);
    }
}

// 0x66B290
void CTaskComplexWanderCop::LookForStolenCopCars(CPed* ped) {
    // Como el original: FindPlayerPed se llama en cada comprobacion
    CPlayerPed* player = FindPlayerPed();

    CWanted* wanted = nullptr;
    if (player->GetPlayerData()) {
        wanted = player->GetPlayerWanted();
    }

    // NOTSA: el original lee el nivel de busqueda sin comprobar `wanted` (falla sin datos de jugador)
    if (wanted && wanted->GetWantedLevel() == eWantedLevel::WANTED_CLEAN && FindPlayerPed()->m_pVehicle) {
        if (FindPlayerPed()->m_pVehicle->vehicleFlags.bIsLawEnforcer) {
            FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1);
        }
    }
}

// notsa, code from `LookForCriminals`
CPed* CTaskComplexWanderCop::FindFirstCriminalInSight(CPed* ped) const {
    const auto& pedPos = ped->GetPosition();
    for (auto& candidate : ped->GetIntelligence()->m_pedScanner.GetEntities<CPed>()) {
        if (!candidate.IsGangster()) {
            if (candidate.m_nPedType != PED_TYPE_CRIMINAL || &candidate == m_pLastCriminalPedLookedFor) {
                continue;
            }
        }

        // Como el original: GetActiveTask se llama dos veces
        if (!candidate.GetTaskManager().GetActiveTask()) {
            continue;
        }
        if (candidate.GetTaskManager().GetActiveTask()->GetTaskType() != GetTaskType()) {
            continue;
        }

        // Como el original: (candidato - ped), suma (z*z + y*y) + x*x; solo pasa si es menor que 10^2 (NaN no pasa)
        const auto& candidatePos = candidate.GetPosition();
        const float dx = candidatePos.x - pedPos.x, dy = candidatePos.y - pedPos.y, dz = candidatePos.z - pedPos.z;
        if (!(dz * dz + dy * dy + dx * dx < sq(10.0f))) {
            continue;
        }

        // Como el original: producto con el eje Y de la matriz en orden (z + y) + x; solo pasa si es mayor que 0 (NaN no pasa)
        const auto& fwd = ped->GetForward(); // Como el original: sin comprobar la matriz
        if (!(dz * fwd.z + dy * fwd.y + dx * fwd.x > 0.f)) {
            continue;
        }

        if (!CWorld::GetIsLineOfSightClear(pedPos, candidatePos, true, false, false, true, false, false, false)) {
            continue;
        }

        return &candidate;
    }
    return nullptr;
}

// 0x66B300
void CTaskComplexWanderCop::LookForCriminals(CPed* ped) {
    auto* const criminal = FindFirstCriminalInSight(ped);
    if (!criminal) {
        return;
    }

    CEventPedToChase eventPedToChase(criminal);
    ped->GetEventGroup().Add(&eventPedToChase, false);

    CEventPedToFlee eventPedToFlee(ped);
    criminal->GetEventGroup().Add(&eventPedToFlee, false);

    // 30 seconds wait for next check
    m_nTimePassedSinceLastLookedForCriminals = CTimer::GetTimeInMS() + 30'000;
    m_pLastCriminalPedLookedFor              = criminal;
}

// 0x66B160
bool CTaskComplexWanderCop::ShouldPursuePlayer(CPed* ped) {
    // Como el original: comparacion con signo (nivel <= 0)
    if (static_cast<int32>(FindPlayerWanted()->GetWantedLevel()) <= 0)
        return false;

    if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_COMPLEX_POLICE_PURSUIT)
        return false;

    // Como el original: FindPlayerWanted se vuelve a llamar
    return FindPlayerWanted()->CanCopJoinPursuit(ped->AsCop());
}
