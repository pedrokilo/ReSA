/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedAttractor
 * Reconstrucción: COMPLETA — 20/20 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 20/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedAttractor.h"

#include "Tasks/TaskTypes/TaskComplexGoToAttractor.h"
#include "Tasks/TaskTypes/TaskComplexWaitAtAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUsePairedAttractor.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"

void CPedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CPedAttractor, 0x86C538, 6);
    RH_ScopedCategory("Attractors");

    RH_ScopedInstall(Constructor, 0x5EDFB0);
    RH_ScopedInstall(Destructor, 0x5EC410);

    RH_ScopedInstall(SetTaskForPed, 0x5EECA0);
    RH_ScopedInstall(RegisterPed, 0x5EEE30);
    RH_ScopedInstall(DeRegisterPed, 0x5EC5B0);
    RH_ScopedInstall(IsRegisteredWithPed, 0x5EB4C0);
    // Como el original: las virtuales se enganchan por la vtable (huecos 2-5 de 0x86C538); con RH_ScopedInstall
    // el símbolo de la DLL es un thunk de llamada virtual que vuelve a la entrada enganchada -> bucle infinito
    RH_ScopedVMTInstall(IsAtHeadOfQueue, 0x5EB530);
    RH_ScopedInstall(GetTaskForPed, 0x5EC500);
    RH_ScopedInstall(GetQueueSlot, 0x5EB550);
    //RH_ScopedInstall(GetNoOfRegisteredPeds, 0xdeadbeef, { .reversed = false }); // Address incorrect
    RH_ScopedInstall(GetHeadOfQueue, 0x5EB590);
    RH_ScopedInstall(GetTailOfQueue, 0x5EB5B0);
    RH_ScopedInstall(HasEmptySlot, 0x5EAF10);
    //RH_ScopedInstall(ComputeFreeSlot, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeDeltaPos, 0x5E9600);
    RH_ScopedInstall(ComputeDeltaHeading, 0x5E9640);
    RH_ScopedInstall(ComputeAttractTime, 0x5E95E0);
    RH_ScopedVMTOverloadedInstall(ComputeAttractPos, "v", 0x5EA110, void(CPedAttractor::*)(int32, CVector&));
    RH_ScopedVMTOverloadedInstall(ComputeAttractHeading, "v", 0x5EA1C0, void(CPedAttractor::*)(int32, float&));
    RH_ScopedVMTInstall(BroadcastDeparture, 0x5EF160);
    RH_ScopedInstall(BroadcastArrival, 0x5EEF80);
    RH_ScopedInstall(AbortPedTasks, 0x5EAF60);
}

// 0x5EAFD0
void* CPedAttractor::operator new(unsigned size) {
    return GetPedAttractorPool()->New();
}

// 0x5EAFE0
void CPedAttractor::operator delete(void* object) {
    GetPedAttractorPool()->Delete(static_cast<CPedAttractor*>(object));
}

// 0x5EDFB0
CPedAttractor::CPedAttractor(
    C2dEffectPedAttractor* fx,
    CEntity*               entity,
    eMoveState             moveState,
    size_t                 maxNoOfPeds,
    float                  spacing,
    float                  achieveQueueTime,
    float                  achieveQueueShuffleTime,
    float                  arriveRange,
    float                  headingRange,
    float                  deltaPos,
    float                  deltaHeading
) :
    m_Fx{fx},
    m_Entity{entity},
    m_MoveState{moveState},
    m_MaxNumPeds{maxNoOfPeds},
    m_Spacing{spacing},
    m_AchieveQueueTime{achieveQueueTime},
    m_AchieveQueueShuffleTime{achieveQueueShuffleTime},
    m_ArriveRange{arriveRange},
    m_HeadingRange{headingRange},
    m_DeltaPos{deltaPos},
    m_DeltaHeading{deltaHeading}
{
    const CMatrix mat = entity
        ? entity->GetMatrix()
        : CMatrix::Unity();
    m_Pos      = CPedAttractorManager::ComputeEffectPos(fx, mat);
    m_QueueDir = CPedAttractorManager::ComputeEffectQueueDir(fx, mat);
    m_UseDir   = CPedAttractorManager::ComputeEffectUseDir(fx, mat);

    strcpy(m_ScriptName, fx->m_szScriptName); // Como el original: strcpy (0x821BB5); strcpy_s rellenaba con 0xFE en Debug
}

void CPedAttractor::Shutdown() {
    ms_tasks.clear();
}

// 0x5EECA0
void CPedAttractor::SetTaskForPed(CPed* ped, CTask* task) {
    const auto numPairs = (int32)m_PedTaskPairs.size();
    for (int32 i = 0; i < numPairs; ++i) {
        auto& pair = m_PedTaskPairs.begin()[i];
        if (pair.Ped != ped) {
            continue;
        }
        if (!pair.UsedTask && pair.Task) { // 0x5EED79
            if (const auto erased = rng::find(ms_tasks, pair.Task); erased != ms_tasks.end()) { ms_tasks.erase(erased); } // Como el original: no borra si no está // 0x5EEDA0, 0x5EEDAD
            delete pair.Task; // 0x5EEDD1
        }
        pair.Task     = task;
        pair.UsedTask = false;
        ms_tasks.emplace_back(task); // Como el original: la tarea nueva se apunta en ms_tasks (0x5EEDF2)
        return;
    }
    // Como el original (0x5EED19): par nuevo {ped, tarea, -1, 0, false} y la tarea también va a ms_tasks
    m_PedTaskPairs.emplace_back(CPedTaskPair{ .Ped = ped, .Task = task, .Slot = TASK_SECONDARY_INVALID, .MatchID = 0, .UsedTask = false });
    ms_tasks.emplace_back(task);
}

// 0x5EEE30
bool CPedAttractor::RegisterPed(CPed* ped) {
    if (const auto it = rng::find(m_AttractPeds, ped); it != m_AttractPeds.end()) { // Como el original: si ya estaba, lo quita y devuelve false
        m_AttractPeds.erase(it);
        return false;
    }
    if ((int32)(m_ArrivedPeds.size() + m_AttractPeds.size()) >= (int32)m_MaxNumPeds) { // Como el original: con signo (jge)
        return false;
    }
    m_AttractPeds.emplace_back(ped); // Como el original: entra en m_AttractPeds (0x5EEED1), no en la cola
    const auto slot = (int32)m_ArrivedPeds.size();
    CVector pos;
    ComputeAttractPos(slot, pos);
    float heading;
    ComputeAttractHeading(slot, heading);
    const float time = m_AchieveQueueTime;
    SetTaskForPed(ped, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, m_MoveState });
    return true;
}

// 0x5EC5B0
bool CPedAttractor::DeRegisterPed(CPed* ped) {
    if (const auto erased = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); erased != m_PedTaskPairs.end()) { m_PedTaskPairs.erase(erased); } // Como el original: no borra si no está
    if (const auto it = rng::find(m_AttractPeds, ped); it != m_AttractPeds.end()) { // Como el original: busca y borra en m_AttractPeds (0x5EC600)
        m_AttractPeds.erase(it);
        return true;
    }
    return BroadcastDeparture(ped);
}

// 0x5EB4C0
bool CPedAttractor::IsRegisteredWithPed(const CPed* ped) const {
    return rng::contains(m_AttractPeds, ped)
        || rng::contains(m_ArrivedPeds, ped);
}

// 0x5EB530
bool CPedAttractor::IsAtHeadOfQueue(CPed* ped) {
    return *m_ArrivedPeds.begin() == ped; // Como el original: lee el primer hueco aunque la cola esté vacía (o sin reservar)
}

// 0x5EC500
CTask* CPedAttractor::GetTaskForPed(CPed* ped) {
    const auto it = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped);
    if (it == m_PedTaskPairs.end()) {
        return nullptr;
    }
    if (!it->UsedTask) {
        // Como el original (0x5EC55D): solo se quita si está en la lista; si no, no se borra nada
        if (const auto t = rng::find(ms_tasks, it->Task); t != ms_tasks.end()) {
            ms_tasks.erase(t);
        }
    }
    it->UsedTask = true;
    return it->Task;
}

// 0x5EB550
int32 CPedAttractor::GetQueueSlot(const CPed* ped) {
    const auto numArrived = (int32)m_ArrivedPeds.size();
    for (int32 i = 0; i < numArrived; ++i) {
        if (m_ArrivedPeds.begin()[i] == ped) {
            return i;
        }
    }
    return -1; // Como el original: -1 si no está en la cola
}

// 0x5EB590
CPed* CPedAttractor::GetHeadOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.front();
}

// 0x5EB5B0
CPed* CPedAttractor::GetTailOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.back();
}

int32 CPedAttractor::ComputeFreeSlot() {
    assert(0);
    return 0;
}

// 0x5E9600
float CPedAttractor::ComputeDeltaPos() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaPos, m_DeltaPos);
}

// 0x5E9640
float CPedAttractor::ComputeDeltaHeading() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaHeading, m_DeltaHeading);
}

// inlined
// 0x5E95E0
void CPedAttractor::ComputeAttractTime(int32 unused, bool time1_or_time2, float& outTime) const {
    if (time1_or_time2)
        outTime = m_AchieveQueueShuffleTime;
    else
        outTime = m_AchieveQueueTime;
}

// 0x5EA110
void CPedAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) {
    if (m_Fx) {
        outPos = m_Pos - m_QueueDir * (float)pedId * m_Spacing; // Como el original: (dir · id) · separación

        if (pedId) {
            outPos.x += ComputeDeltaPos();
            outPos.y += ComputeDeltaPos();
        }
    }
}

// bQueue - bad name?
// 0x5EA1C0
void CPedAttractor::ComputeAttractHeading(int32 bQueue, float& heading) {
    if (m_Fx) {
        const CVector dir = bQueue ? m_QueueDir : m_UseDir; // Como el original: copia la dirección sin modificar m_UseDir
        heading = CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.0f, 0.0f);
        if (bQueue)
            heading += CPedAttractor::ComputeDeltaHeading();
    }
}

// 0x5EF160
bool CPedAttractor::BroadcastDeparture(CPed* ped) {
    // Como el original: el peatón que se va se busca en la cola (m_ArrivedPeds)
    const auto numArrived = (int32)m_ArrivedPeds.size();
    int32 idx = 0;
    for (; idx < numArrived; ++idx) {
        if (m_ArrivedPeds.begin()[idx] == ped) {
            break;
        }
    }
    if (idx >= numArrived) {
        return false;
    }
    // Los que van detrás avanzan un hueco: esperan 2000 · i ms y van al hueco i - 1 (0x5EF1C6)
    for (int32 i = idx + 1, slot = idx; i < numArrived; ++i, ++slot) {
        CPed* const queuedPed = m_ArrivedPeds.begin()[i];
        CVector pos;
        ComputeAttractPos(slot, pos);
        float heading;
        ComputeAttractHeading(slot, heading);
        const float time = m_AchieveQueueShuffleTime;
        auto* const seq = new CTaskComplexSequence;
        seq->AddTask(new CTaskSimpleStandStill{ 2000 * i, false, false, 8.0f });
        seq->AddTask(new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK });
        SetTaskForPed(queuedPed, seq);
    }
    if (const auto erased = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); erased != m_PedTaskPairs.end()) { m_PedTaskPairs.erase(erased); } // Como el original: no borra si no está // 0x5EF2F6
    m_ArrivedPeds.erase(m_ArrivedPeds.begin() + idx); // 0x5EF334
    const auto numAttract = (int32)m_AttractPeds.size(); // 0x5EF371
    for (int32 i = 0; i < numAttract; ++i) {
        CPed* const attractedPed = m_AttractPeds.begin()[i];
        const auto slot = (int32)m_ArrivedPeds.size();
        CVector pos;
        ComputeAttractPos(slot, pos);
        float heading;
        ComputeAttractHeading(slot, heading);
        const float time = m_AchieveQueueTime;
        SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK });
    }
    return true;
}

// 0x5EEF80
bool CPedAttractor::BroadcastArrival(CPed* ped) {
    if (!rng::contains(m_ArrivedPeds, ped)) {
        m_ArrivedPeds.emplace_back(ped); // 0x5EEFCB
        if (const auto erased = rng::find(m_AttractPeds, ped); erased != m_AttractPeds.end()) { m_AttractPeds.erase(erased); } // Como el original: no borra si no está // 0x5EF002
        for (auto it = m_AttractPeds.begin(); it != m_AttractPeds.end();) { // 0x5EF013: relee el final en cada vuelta
            CPed* const attractedPed = *it++;
            const auto slot = (int32)m_ArrivedPeds.size();
            CVector pos;
            ComputeAttractPos(slot, pos);
            float heading;
            ComputeAttractHeading(slot, heading);
            const float time = m_AchieveQueueTime;
            SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK }); // Como el original: PEDMOVE_WALK (push 4) // 0x5EF07B
        }
    }

    if (!IsAtHeadOfQueue(ped)) { // 0x5EF129
        SetTaskForPed(ped, new CTaskComplexWaitAtAttractor{ this, GetQueueSlot(ped) });
    } else if (GetType() == PED_ATTRACTOR_SCRIPTED) { // 0x5EF0D6 - inverted (!)
        SetTaskForPed(ped, new CTaskComplexUsePairedAttractor{ this });
    } else { // 0x5EF0FA
        SetTaskForPed(ped, new CTaskComplexUseAttractor{ this });
    }

    return true;
}

// 0x5EAF60
void CPedAttractor::AbortPedTasks() {
    const auto count = static_cast<int32>(m_PedTaskPairs.size());
    for (int32 i = 0; i < count; ++i) {
        auto& p = m_PedTaskPairs._First[i];
        p.Ped = nullptr;
        // Como el original: el destructor todavía ve el puntero Task.
        delete p.Task;
        p.Task = nullptr;
        p.Slot = TASK_SECONDARY_INVALID;
    }
    // TODO/NOTE: Why not clear the array?
}
