/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseSequence
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

#include "TaskComplexUseSequence.h"

namespace {
// 0x6396C0 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexUseSequence del EXE (sin hook propio; vtable 0x86E518). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x6396C0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexUseSequenceDestructorBody(CTaskComplexUseSequence* self, void* /*edx*/) { self->CTaskComplexUseSequence::~CTaskComplexUseSequence(); }

// 0x639F90 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexUseSequence del EXE (ranura 0 de su vtable 0x86E518; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6396C0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexUseSequence* __fastcall CTaskComplexUseSequenceScalarDeletingDestructor(CTaskComplexUseSequence* self, void* /*edx*/, uint8 flags) {
    CTaskComplexUseSequenceDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexUseSequence() { return reinterpret_cast<uintptr_t>(&CTaskComplexUseSequenceDestructorBody); } // para el banco Fn145A

void CTaskComplexUseSequence::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseSequence, 0x86e518, 11);
    RH_ScopedCategory("Tasks");
    RH_ScopedNamedGlobalInstall(CTaskComplexUseSequenceScalarDeletingDestructor, "ScalarDeletingDestructor", 0x639F90); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x635450);
    RH_ScopedVMTInstall(Clone, 0x637100);
    RH_ScopedVMTInstall(MakeAbortable, 0x639730);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6354A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6354D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x635530);
}

// 0x635450
CTaskComplexUseSequence::CTaskComplexUseSequence(int32 sequenceIndex) :
    m_nSequenceIndex{sequenceIndex}
{
    CTaskSequences::ms_taskSequence[m_nSequenceIndex].m_RefCnt++;
}

// Used by `Clone` @ 0x637100
CTaskComplexUseSequence::CTaskComplexUseSequence(const CTaskComplexUseSequence& o) :
    CTaskComplexUseSequence{ o.m_nSequenceIndex }
{
    m_nCurrentTaskIndex = o.m_nCurrentTaskIndex;
    m_nEndTaskIndex     = o.m_nEndTaskIndex;
}

CTaskComplexUseSequence::~CTaskComplexUseSequence() {
    if (m_nSequenceIndex != -1) {
        auto complexSequence = &CTaskSequences::ms_taskSequence[m_nSequenceIndex];
        bool bFinalReference = complexSequence->m_RefCnt == 1;
        complexSequence->m_RefCnt--;
        if (bFinalReference && complexSequence->m_bFlushTasks) {
            complexSequence->m_bFlushTasks = false;
            complexSequence->Flush();
        }
    }
}

CTaskComplexUseSequence* CTaskComplexUseSequence::Constructor(int32 sequenceIndex) {
    this->CTaskComplexUseSequence::CTaskComplexUseSequence(sequenceIndex);
    return this;
}

bool CTaskComplexUseSequence::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!m_pSubTask) {
        return true;
    }

    bool bMakeAbortable = m_pSubTask->MakeAbortable(ped, priority, event);
    if (bMakeAbortable && event && event->GetEventType() == EVENT_DAMAGE) {
        auto* eventDamage = (CEventDamage*)event;
        if (eventDamage->m_damageResponse.m_bHealthZero && eventDamage->m_bAddToEventGroup) {
            CTaskComplexSequence* sequence = &CTaskSequences::ms_taskSequence[m_nSequenceIndex];
            bool bFinalReference = sequence->m_RefCnt == 1;
            sequence->m_RefCnt--;
            if (bFinalReference && sequence->m_bFlushTasks) {
                sequence->m_bFlushTasks = false;
                sequence->Flush();
            }
            m_nSequenceIndex = -1;
        }
    }
    return bMakeAbortable;
}

CTask* CTaskComplexUseSequence::CreateNextSubTask(CPed* ped) {
    if (m_nSequenceIndex == -1) {
        return nullptr;
    } else {
        auto taskComplexSequence = &CTaskSequences::ms_taskSequence[m_nSequenceIndex];
        return taskComplexSequence->CreateNextSubTask(ped, m_nCurrentTaskIndex, m_nSequenceRepeatedCount);
    }
}


CTask* CTaskComplexUseSequence::CreateFirstSubTask(CPed* ped) {
    if (m_nSequenceIndex == -1) {
        return nullptr;
    }

    CTask* firstSubTask = nullptr;
    CTask* currentSequenceTask = CTaskSequences::ms_taskSequence[m_nSequenceIndex].m_Tasks[m_nCurrentTaskIndex];
    if (currentSequenceTask) {
        firstSubTask = currentSequenceTask->Clone();
    }

    if (m_nEndTaskIndex != -1) {
        if (firstSubTask && firstSubTask->GetTaskType() == TASK_COMPLEX_USE_SEQUENCE) {
            auto sequence = reinterpret_cast<CTaskComplexUseSequence*>(firstSubTask);
            sequence->m_nCurrentTaskIndex = m_nEndTaskIndex;
            sequence->m_nEndTaskIndex = -1;
        }
    }
    return firstSubTask;
}

// 0x0
CTask* CTaskComplexUseSequence::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
