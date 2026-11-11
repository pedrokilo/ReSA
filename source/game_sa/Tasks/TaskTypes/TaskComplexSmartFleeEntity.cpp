/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSmartFleeEntity
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

#include "TaskComplexSmartFleeEntity.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexSmartFleePoint.h"
#include "Ragdoll/IKChainManager.h"
#include "InterestingEvents.h"
#include "EventSeenPanickedPed.h"

namespace {
// 0x65C4D0 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskComplexSmartFleeEntity del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x65C4D0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexSmartFleeEntityDestructorBody(CTaskComplexSmartFleeEntity* self, void* /*edx*/) { self->CTaskComplexSmartFleeEntity::~CTaskComplexSmartFleeEntity(); }

// 0x65D280 (lote 157A, banco Fn157A): destructor con borrado de CTaskComplexSmartFleeEntity del EXE (ranura 0 de su vtable 0x86F784; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x65C4D0; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexSmartFleeEntity* __fastcall CTaskComplexSmartFleeEntityScalarDeletingDestructor(CTaskComplexSmartFleeEntity* self, void* /*edx*/, uint8 flags) {
    CTaskComplexSmartFleeEntityDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskComplexSmartFleeEntity() { return reinterpret_cast<uintptr_t>(&CTaskComplexSmartFleeEntityDestructorBody); } // para el banco Fn157A

void CTaskComplexSmartFleeEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSmartFleeEntity, 0x86f784, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexSmartFleeEntityScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65D280); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x65C430);
    RH_ScopedInstall(Destructor, 0x65C4D0);
    RH_ScopedInstall(CreateSubTask, 0x65C530);

    RH_ScopedVMTInstall(Clone, 0x65CF40);
    RH_ScopedVMTInstall(GetTaskType, 0x65C4C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65C680);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65C6F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x65C780);
}

// 0x65C430
CTaskComplexSmartFleeEntity::CTaskComplexSmartFleeEntity(
    CEntity* fleeFrom,
    bool scream,
    float safeDistance,
    int32 fleeTime,
    int32 posCheckPeriod,
    float posChangeTolerance,

    // VVV NOTSA ARGS VVV //
    eMoveState moveState
)  :

    m_fleeFrom{fleeFrom},
    m_pedScream{scream},
    m_safeDistance{safeDistance},
    m_time{fleeTime},
    m_posCheckPeriod{posCheckPeriod},
    m_posChangeTolerance{posChangeTolerance},
    m_moveState{moveState}
{
    CEntity::SafeRegisterRef(m_fleeFrom);
}

// 0x65CF40: Como el original, Clone copia también el estado de movimiento (+0x30)
CTaskComplexSmartFleeEntity::CTaskComplexSmartFleeEntity(const CTaskComplexSmartFleeEntity& o) :
    CTaskComplexSmartFleeEntity{ o.m_fleeFrom, o.m_pedScream, o.m_safeDistance, o.m_time, o.m_posCheckPeriod, o.m_posChangeTolerance, o.m_moveState }
{
}

// 0x65C4D0
CTaskComplexSmartFleeEntity::~CTaskComplexSmartFleeEntity() {
    CEntity::SafeCleanUpRef(m_fleeFrom);
}

// 0x65C680
CTask* CTaskComplexSmartFleeEntity::CreateNextSubTask(CPed* ped) {
    // Como el original: GetTaskType se llama dos veces (la primera se descarta); STAND_STILL con entidad vuelve a huir
    (void)m_pSubTask->GetTaskType();
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(m_fleeFrom ? TASK_COMPLEX_SMART_FLEE_POINT : TASK_FINISHED);
    case TASK_COMPLEX_SMART_FLEE_POINT:
        return CreateSubTask(TASK_FINISHED);
    default:
        return nullptr;
    }
}

// 0x65C6F0
CTask* CTaskComplexSmartFleeEntity::CreateFirstSubTask(CPed* ped) {
    if (!m_fleeFrom) {
        return nullptr;
    }

    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_13, ped);

    if (!ped->bInVehicle && !g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.LookAt(
            "TaskSmartFleeEntity",
            ped,
            m_fleeFrom,
            3000,
            m_fleeFrom->GetIsTypePed() // Como el original: tipo de entidad (byte 0x36 & 7) == peatón, no el estado
                ? BONE_HEAD
                : BONE_UNKNOWN,
            nullptr,
            true,
            0.25f,
            500,
            3,
            false
        );
    }

    return CreateSubTask(TASK_COMPLEX_SMART_FLEE_POINT);
}

// 0x65C780
CTask* CTaskComplexSmartFleeEntity::ControlSubTask(CPed* ped) {
    if (!m_fleeFrom) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        return m_pSubTask;
    }
    if (m_pSubTask->GetTaskType() != TASK_COMPLEX_SMART_FLEE_POINT) {
        return m_pSubTask;
    }
    const auto fleePointTask = static_cast<CTaskComplexSmartFleePoint*>(m_pSubTask);
    fleePointTask->m_moveState = m_moveState;

    // Como el original: el temporizador solo se reinicia cuando la entidad se ha alejado más que la tolerancia
    if (!m_posCheckTimer.m_bStarted) {
        return m_pSubTask;
    }
    if (m_posCheckTimer.m_bStopped) {
        m_posCheckTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_posCheckTimer.m_bStopped   = false;
    }
    const uint32 now = CTimer::GetTimeInMS();
    if (now < m_posCheckTimer.m_nStartTime + (uint32)m_posCheckTimer.m_nInterval) {
        return m_pSubTask;
    }

    // Como el original: (dz² + dy²) + dx² frente a tolerancia² con comparación estricta
    const auto& currPos = m_fleeFrom->GetPosition();
    const float dx = m_pos.x - currPos.x, dy = m_pos.y - currPos.y, dz = m_pos.z - currPos.z;
    const float distSq = dz * dz + dy * dy + dx * dx;
    if (!(m_posChangeTolerance * m_posChangeTolerance < distSq)) {
        return m_pSubTask;
    }
    m_posCheckTimer.m_nStartTime = now;
    m_posCheckTimer.m_nInterval  = m_posCheckPeriod;
    m_posCheckTimer.m_bStarted   = true;
    m_pos = m_fleeFrom->GetPosition();
    fleePointTask->SetFleePosition(m_pos, m_safeDistance, m_pedScream);

    if ((int32)m_moveState > 4) { // Como el original: jle con 4 (PEDMOVE_WALK), con signo
        CEventSeenPanickedPed event{ ped };
        GetEventGlobalGroup()->Add(&event, false);
    }

    return m_pSubTask;
}

// 0x65C530 (thiscall, `ret 4`, marco SEH que solo desenrolla)
// Como el original: 0xCB (parado): CTask::operator new(0x20) primero y, si no es nulo, rand() del CRT (CGeneral::GetRandomNumber) & 0xFFFF * (1/32768) * 50 truncado como tiempo del
// constructor (false, false, 8.0f); 0x38E (huir de un punto): arranca el temporizador (sin mirar el byte de parada), copia la posicion de la entidad (de su matriz o de su placement) a m_pos,
// reserva 0x44 bytes y, si no es nulo, construye (m_pos, m_pedScream, m_safeDistance, m_time); 0x516 y cualquier otro tipo devuelven nulo.
CTask* CTaskComplexSmartFleeEntity::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL: {
        static_assert(sizeof(CTaskSimpleStandStill) == 0x20);
        void* const mem = CTask::operator new(sizeof(CTaskSimpleStandStill));
        if (!mem) {
            return nullptr;
        }
        const int32 time = (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * 3.0517578125e-05f * 50.f);
        return ::new (mem) CTaskSimpleStandStill{ time, false, false, 8.f };
    }
    case TASK_COMPLEX_SMART_FLEE_POINT: {
        m_posCheckTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_posCheckTimer.m_nInterval  = m_posCheckPeriod;
        m_posCheckTimer.m_bStarted   = true;
        m_pos = m_fleeFrom->GetPosition();
        static_assert(sizeof(CTaskComplexSmartFleePoint) == 0x44);
        void* const mem = CTask::operator new(sizeof(CTaskComplexSmartFleePoint));
        if (!mem) {
            return nullptr;
        }
        return ::new (mem) CTaskComplexSmartFleePoint{ m_pos, m_pedScream, m_safeDistance, m_time };
    }
    default:
        return nullptr;
    }
}
