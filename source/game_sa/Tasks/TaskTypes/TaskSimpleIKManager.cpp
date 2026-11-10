/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleIKManager
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
#include "TaskSimpleIKManager.h"

namespace {
// 0x633830 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskSimpleIKManager del EXE (con hook propio (Destructor); vtable 0x86E358). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x633830` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleIKManagerDestructorBody(CTaskSimpleIKManager* self, void* /*edx*/) { self->CTaskSimpleIKManager::~CTaskSimpleIKManager(); }

// 0x639330 (lote 145A, banco Fn145A): destructor con borrado de CTaskSimpleIKManager del EXE (ranura 0 de su vtable 0x86E358; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x633830; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleIKManager* __fastcall CTaskSimpleIKManagerScalarDeletingDestructor(CTaskSimpleIKManager* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleIKManagerDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskSimpleIKManager() { return reinterpret_cast<uintptr_t>(&CTaskSimpleIKManagerDestructorBody); } // para el banco Fn145A

void CTaskSimpleIKManager::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKManager, 0x86E358, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleIKManagerScalarDeletingDestructor, "ScalarDeletingDestructor", 0x639330); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x6337F0);
    RH_ScopedInstall(Destructor, 0x633830);

    RH_ScopedInstall(AddIKChainTask, 0x633940);
    RH_ScopedInstall(GetTaskAtSlot, 0x6339B0);
    RH_ScopedVMTInstall(Clone, 0x639350);
    RH_ScopedVMTInstall(GetTaskType, 0x633820);
    RH_ScopedVMTInstall(MakeAbortable, 0x6338A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6338E0);
}

// For 0x639350
CTaskSimpleIKManager::CTaskSimpleIKManager(const CTaskSimpleIKManager& o) :
    CTaskSimpleIKManager()
{
    // Como el original: solo se clonan los huecos ocupados (AddIKChainTask en linea con el mismo hueco)
    for (size_t slot = 0; slot < m_IKChainTasks.size(); slot++) {
        if (const auto t = o.m_IKChainTasks[slot]) {
            m_IKChainTasks[slot] = static_cast<CTaskSimpleIKChain*>(t->Clone());
        }
    }
}

// 0x633830
CTaskSimpleIKManager::~CTaskSimpleIKManager() {
    for (const auto task : m_IKChainTasks) {
        delete task;
    }
}

// 0x633940
void CTaskSimpleIKManager::AddIKChainTask(CTaskSimpleIKChain* task, eIKChainSlot slot) {
    if ((int32)slot < 0) { // Find free slot, and store `task` there. (Como el original: cualquier hueco negativo)
        const auto it = rng::find(m_IKChainTasks, nullptr);
        if (it != m_IKChainTasks.end()) {
            *it = task;
        }
    } else { // Store `task` in the given `slot` (Como el original: sobrescribe sin comprobar)
        m_IKChainTasks.data()[(size_t)slot] = task;
    }
}

// 0x6338A0
bool CTaskSimpleIKManager::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == eAbortPriority::ABORT_PRIORITY_IMMEDIATE) {
        for (auto& task : m_IKChainTasks) {
            delete std::exchange(task, nullptr);
        }
        return true;
    } else {
        m_IsAborting = true;
        return false;
    }
}

// 0x6338E0
bool CTaskSimpleIKManager::ProcessPed(CPed* ped) {
    if (m_IsAborting) {
        return true;
    }

    bool hasAllProcessedPed{true};
    for (auto& task : m_IKChainTasks) {
        if (task) {
            if (task->ProcessPed(ped)) {
                delete std::exchange(task, nullptr);
            } else {
                hasAllProcessedPed = false;
            }
        }
    }
    return hasAllProcessedPed;
}

