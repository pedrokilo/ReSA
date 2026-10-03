/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSequence
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexSequence.h"

void CTaskComplexSequence::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSequence, 0x86e200, 11);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Constructor, 0x632BD0);

    RH_ScopedVMTInstall(Clone, 0x5F6710);
    RH_ScopedVMTInstall(MakeAbortable, 0x632C00);
    RH_ScopedVMTOverloadedInstall(CreateNextSubTask, "V", 0x638A40, CTask*(CTaskComplexSequence::*)(CPed*));
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638A60);
    RH_ScopedVMTInstall(ControlSubTask, 0x632D00);

    RH_ScopedOverloadedInstall(CreateNextSubTask, "Internal", 0x632C70, CTask*(CTaskComplexSequence::*)(CPed*, int32&, int32&) const);
    RH_ScopedOverloadedInstall(AddTask, "0", 0x632D10, bool(CTaskComplexSequence::*)(CTask*));
    RH_ScopedOverloadedInstall(AddTask, "1", 0x632D50, void(CTaskComplexSequence::*)(int32, CTask*));

    RH_ScopedInstall(Flush, 0x632C10);
    RH_ScopedInstall(Contains, 0x41BF10);
    RH_ScopedInstall(SetCanBeEmptied, 0x463610);
    RH_ScopedInstall(DeRegister, 0x636BC0);
}

// For 0x5F6710
CTaskComplexSequence::CTaskComplexSequence(const CTaskComplexSequence& o) :
    m_RepeatMode{o.m_RepeatMode},
    m_CurrTaskIdx{o.m_CurrTaskIdx}
{
    rng::transform(o.m_Tasks, m_Tasks.begin(), [] (CTask* t) {
        return t ? t->Clone() : nullptr;
    });
}

// 0x6389F0
CTaskComplexSequence::~CTaskComplexSequence() {
    Flush();
}

// 0x632C10
void CTaskComplexSequence::Flush() {
    // Como el original: borra la tarea (si hay) y despues pone el hueco a nulo
    for (auto& task : m_Tasks) {
        if (task) {
            delete task;
        }
        task = nullptr;
    }
    m_CurrTaskIdx = 0;
    m_RepeatMode  = eRepeatMode::DONT_REPEAT;
    m_RepeatedCnt = 0;
}

CTaskComplexSequence* CTaskComplexSequence::Constructor() {
    this->CTaskComplexSequence::CTaskComplexSequence();
    return this;
}

// 0x638A40
CTask* CTaskComplexSequence::CreateNextSubTask(CPed* ped) {
    return CreateNextSubTask(ped, m_CurrTaskIdx, m_RepeatedCnt);
}

// 0x638A60
CTask* CTaskComplexSequence::CreateFirstSubTask(CPed* ped) {
    const auto curr = m_Tasks[m_CurrTaskIdx];;
    return curr
        ? curr->Clone()
        : nullptr;
}

// 0x632D00
CTask* CTaskComplexSequence::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x632C00
bool CTaskComplexSequence::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: delega en la subtarea (override propio para no compartir CTaskComplex::MakeAbortable con otros hooks)
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x632D10
bool CTaskComplexSequence::AddTask(CTask* task) {
    // Como el original: sin comprobar la tarea; devuelve si se pudo anadir
    for (auto& t : m_Tasks) {
        if (!t) {
            t = task;
            return true;
        }
    }

    if (task) {
        delete task;
    }
    NOTSA_LOG_DEBUG("Failed to add task to sequence");
    return false;
}

// 0x632D50
void CTaskComplexSequence::AddTask(int32 sequenceIdx, CTask* task) {
    // Como el original: sin comprobar la tarea; indice con signo (jge)
    if (sequenceIdx >= (int32)std::size(m_Tasks)) {
        if (task) {
            delete task;
        }
        NOTSA_LOG_DEBUG("Failed to add task to sequence");
    } else {
        if (const auto old = m_Tasks.data()[sequenceIdx]) {
            delete old;
        }
        m_Tasks.data()[sequenceIdx] = task;
    }
}

// 0x632C70
CTask* CTaskComplexSequence::CreateNextSubTask(CPed* ped, int32& taskIndex, int32& repeatCount) const {
    UNUSED(ped);

    const auto idx = ++taskIndex;
    const auto mode = (uint32)m_RepeatMode;
    if (mode == 0) { // DONT_REPEAT
        if (idx == 8 || !m_Tasks.data()[idx]) {
            return nullptr;
        }
        return m_Tasks.data()[idx]->Clone();
    }
    // Como el original: al terminar la secuencia vuelve al principio y cuenta la repeticion
    if (idx == 8 || !m_Tasks.data()[idx]) {
        taskIndex = 0;
        repeatCount++;
    }
    // Como el original: un modo distinto de 0 y 1 es el numero de repeticiones (se para al alcanzarlo)
    if (mode != 1 && (uint32)repeatCount == mode) {
        return nullptr;
    }
    return m_Tasks.data()[taskIndex]->Clone();
}

// 0x41BF10
bool CTaskComplexSequence::Contains(eTaskType taskType) const {
    // Como el original: se saltan los huecos vacios
    for (const auto& task : m_Tasks) {
        if (task && task->GetTaskType() == taskType) {
            return true;
        }
    }
    return false;
}

// 0x463610
void CTaskComplexSequence::SetCanBeEmptied(bool flush) {
    if (m_RefCnt || !flush) {
        m_bFlushTasks = flush;
    } else {
        m_bFlushTasks = false;
        Flush();
    }
}

// 0x636BC0
void CTaskComplexSequence::DeRegister() {
    if (!--m_RefCnt && m_bFlushTasks) {
        m_bFlushTasks = false;
        Flush();
    }
}

void CTaskComplexSequence::SetRepeatMode(bool repeat) {
    m_RepeatMode = repeat
        ? eRepeatMode::FOREVER
        : eRepeatMode::DONT_REPEAT;
}
