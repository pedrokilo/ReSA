/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexBeInGroup
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexBeInGroup.h"

namespace {
// 0x632EA0 (lote 147A, banco Fn147A): cuerpo del destructor de CTaskComplexBeInGroup del EXE (con hook propio (Destructor); vtable 0x86E22C). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x632EA0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexBeInGroupDestructorBody(CTaskComplexBeInGroup* self, void* /*edx*/) { self->CTaskComplexBeInGroup::~CTaskComplexBeInGroup(); }

// 0x638A80 (lote 147A, banco Fn147A): destructor con borrado de CTaskComplexBeInGroup del EXE (ranura 0 de su vtable 0x86E22C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x632EA0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexBeInGroup* __fastcall CTaskComplexBeInGroupScalarDeletingDestructor(CTaskComplexBeInGroup* self, void* /*edx*/, uint8 flags) {
    CTaskComplexBeInGroupDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote147ABody_CTaskComplexBeInGroup() { return reinterpret_cast<uintptr_t>(&CTaskComplexBeInGroupDestructorBody); } // para el banco Fn147A

void CTaskComplexBeInGroup::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeInGroup, 0x86E22C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexBeInGroupScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638A80); // lote 147A (banco Fn147A)

    RH_ScopedInstall(Constructor, 0x632E50);
    RH_ScopedInstall(Destructor, 0x632EA0);
    RH_ScopedInstall(MonitorMainGroupTask, 0x633010);
    RH_ScopedInstall(MonitorSecondaryGroupTask, 0x6330B0);
    RH_ScopedVMTInstall(Clone, 0x636BE0);
    RH_ScopedVMTInstall(GetTaskType, 0x632E90);
    RH_ScopedVMTInstall(MakeAbortable, 0x632EB0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632F40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632FB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x638AA0);
}

// 0x632E50
CTaskComplexBeInGroup::CTaskComplexBeInGroup(int32 groupId, bool isLeader) : CTaskComplex() {
    m_nGroupId           = groupId;
    m_Ped                = nullptr;
    m_MainTask           = nullptr;
    m_SecondaryTask      = nullptr;
    m_bIsLeader          = isLeader;
    m_nMainTaskId        = TASK_NONE;
    m_nSecondaryTaskSlot = -1;
}

// 0x633010
CTask* CTaskComplexBeInGroup::MonitorMainGroupTask(CPed* ped) {
    const auto subTask = m_pSubTask; // Como el original: se lee al principio y es lo que se devuelve al final
    if (const auto groupMainTask = CPedGroups::GetGroup(m_nGroupId).GetIntelligence().GetTaskMain(ped)) {
        if (groupMainTask != m_MainTask || groupMainTask->GetTaskType() != m_nMainTaskId) {
            if (m_pSubTask->MakeAbortable(ped)) {
                m_MainTask = groupMainTask;
                m_nMainTaskId = groupMainTask->GetTaskType();
                return groupMainTask->Clone();
            }
        }
    } else if (m_pSubTask->MakeAbortable(ped)) {
        m_MainTask = nullptr;
        m_nMainTaskId = TASK_NONE;
        return nullptr;
    }
    return subTask;
}

// 0x6330B0
void CTaskComplexBeInGroup::MonitorSecondaryGroupTask(CPed* ped) {
    // Check if ped has finished the group task (if any)
    auto& groupIntel = CPedGroups::GetGroup(m_nGroupId).GetIntelligence();
    const auto grpSecTask = groupIntel.GetTaskSecondary(ped);
    const auto pedGrpSecTaskSlot = groupIntel.GetTaskSecondarySlot(ped);
    const auto pedGrpSecTask = ped->GetTaskManager().GetTaskSecondary(pedGrpSecTaskSlot); // Como el original: también con el hueco -1
    if (m_SecondaryTask == grpSecTask) {
        if (m_SecondaryTask) { // Check if theres any task at all (Not both nullptr)
            // Check if ped has finished the task
            if (!pedGrpSecTask) {
                groupIntel.ReportFinishedTask(ped, m_SecondaryTask);
                m_SecondaryTask = nullptr;
                m_nSecondaryTaskSlot = -1;
            }
        }
    } else { // Group has a new task, apply it to the ped

        // Abort peds task in the previous stored slot
        if (const auto task = ped->GetTaskManager().GetTaskSecondary((eSecondaryTask)m_nSecondaryTaskSlot)) {
            task->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }

        // Make sure ped has the task the new task (and abort current)
        if (!pedGrpSecTask || pedGrpSecTask->MakeAbortable(ped)) {
            m_SecondaryTask = grpSecTask;
            m_nSecondaryTaskSlot = pedGrpSecTaskSlot;
            if (grpSecTask) { 
                ped->GetTaskManager().SetTaskSecondary(grpSecTask->Clone(), pedGrpSecTaskSlot);
            }
        }
    }
}

// 0x632EB0
bool CTaskComplexBeInGroup::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{ 
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        auto& groupIntel = GetGroup().GetIntelligence();
        for (auto task : { &m_MainTask, &m_SecondaryTask }) {
            // Como el original: solo se informa si es una tarea valida, pero se anula siempre que no sea nula
            if (*task) {
                if (CTask::IsTaskPtr(*task)) {
                    groupIntel.ReportFinishedTask(ped, *task);
                }
                *task = nullptr;
            }
        }
    }

    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x632F40
CTask* CTaskComplexBeInGroup::CreateNextSubTask(CPed* ped) {
    auto& intel = GetGroup().GetIntelligence();
    intel.ReportFinishedTask(ped, m_pSubTask);      // Report this task as finished, and proceed
    if (const auto main = intel.GetTaskMain(ped)) { // Return group's main task (if any)
        m_MainTask = main;
        m_nMainTaskId = main->GetTaskType();
        return main->Clone();
    } else {
        m_MainTask = nullptr;
        m_nMainTaskId = TASK_NONE;
        return nullptr;
    }
}

// 0x632FB0
CTask* CTaskComplexBeInGroup::CreateFirstSubTask(CPed* ped) {
    m_Ped = ped;
    m_Ped->RegisterReference(reinterpret_cast<CEntity**>(&m_Ped));

    // Below code basically the same as `CreateNextSubTask`
    // TODO: Maybe move this into a separate function? (To reduce copy paste)
    auto& intel = GetGroup().GetIntelligence();
    if (const auto main = intel.GetTaskMain(ped)) { // Return group's main task (if any)
        m_MainTask = main;
        m_nMainTaskId = main->GetTaskType();
        return main->Clone();
    } else {
        m_MainTask = nullptr;
        m_nMainTaskId = TASK_NONE;
        return nullptr;
    }
}

// 0x638AA0
CTask* CTaskComplexBeInGroup::ControlSubTask(CPed* ped) {
    MonitorSecondaryGroupTask(ped);
    return MonitorMainGroupTask(ped);
}
