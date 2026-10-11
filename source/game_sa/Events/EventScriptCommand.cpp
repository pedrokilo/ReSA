/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventScriptCommand
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

#include "EventScriptCommand.h"

#include "TaskSimpleRunAnim.h"

void CEventScriptCommand::InjectHooks()
{
    RH_ScopedVirtualClass(CEventScriptCommand, 0x85B378, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Destructor, 0x4B0A50);
    RH_ScopedInstall(Constructor, 0x4B0A00);
    // Como el original se llaman por la vtable: con RH_ScopedInstall el destino era el thunk virtual de la DLL, que
    // vuelve a saltar por la vtable del EXE (entrada enganchada) y se queda en un bucle infinito
    RH_ScopedVMTInstall(GetEventType, 0x4B0A30);
    RH_ScopedVMTInstall(GetEventPriority, 0x4B0B20);
    RH_ScopedVMTInstall(Clone, 0x4B6490);
    RH_ScopedVMTInstall(AffectsPed, 0x4B0AF0);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B0BA0);
    RH_ScopedVMTInstall(IsValid, 0x4B0AB0);
    RH_ScopedVMTInstall(CloneScriptTask, 0x4B0AA0);
}

// 0x4B0A00
CEventScriptCommand::CEventScriptCommand(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds)
{
    m_primaryTaskIndex = primaryTaskIndex;
    m_task = task;
    m_affectsDeadPeds = affectsDeadPeds;
}

CEventScriptCommand::~CEventScriptCommand()
{
    delete m_task;
}

CEventScriptCommand* CEventScriptCommand::Constructor(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds)
{
    this->CEventScriptCommand::CEventScriptCommand(primaryTaskIndex, task, affectsDeadPeds);
    return this;
}

// 0x4B0B20
int32 CEventScriptCommand::GetEventPriority() const
{
    if (m_affectsDeadPeds)
        return 75;
    if (!m_task)
        return 53;
    // Como el original: GetTaskType se vuelve a llamar por la vtable en cada comparación.
    if (m_task->GetTaskType() == TASK_SIMPLE_NAMED_ANIM && (static_cast<CTaskSimpleRunAnim*>(m_task)->m_nFlags & ANIMATION_IS_LOOPED))
        return 71;
    if (m_task->GetTaskType() == TASK_SIMPLE_DIE || m_task->GetTaskType() == TASK_SIMPLE_SWIM || m_task->GetTaskType() == TASK_COMPLEX_USE_MOBILE_PHONE)
        return 71;
    return m_task->GetTaskType() == TASK_COMPLEX_DIE ? 71 : 53;
}

// 0x4B0AF0
bool CEventScriptCommand::AffectsPed(CPed* ped)
{
    return ped->IsAlive() || m_affectsDeadPeds;
}

// 0x4B0BA0
bool CEventScriptCommand::TakesPriorityOver(const CEvent& refEvent)
{
    // Como el original: el tipo del evento de referencia se pide una vez por comparación y su prioridad antes que la propia.
    if (m_affectsDeadPeds && (refEvent.GetEventType() == EVENT_DEATH || m_affectsDeadPeds && refEvent.GetEventType() == EVENT_DAMAGE))
        return true;
    const int32 refPriority = refEvent.GetEventPriority();
    return GetEventPriority() >= refPriority;
}

// 0x4B0AB0
bool CEventScriptCommand::IsValid(CPed* ped)
{
    if (ped)
        return ped->IsAlive();
    if (m_bValid || m_nTimeActive <= GetLifeTime())
        return true;
    return false;
}

// 0x4B0AA0
CTask* CEventScriptCommand::CloneScriptTask() const
{
    if (m_task)
        return m_task->Clone();
    return nullptr;
}

#pragma warning(push)
#pragma warning(disable: 4733) // Handler SEH original 83A018.
__declspec(naked) void CEventScriptCommand::Destructor() {
    __asm {
        push        0FFFFFFFFh
        push        83A018h
        mov         eax,dword ptr fs:[00000000h]
        push        eax
        mov         dword ptr fs:[00000000h],esp
        push        ecx
        push        esi
        mov         esi,ecx
        mov         dword ptr [esp+4],esi
        mov         dword ptr [esi],85B378h
        mov         ecx,dword ptr [esi+10h]
        test        ecx,ecx
        mov         dword ptr [esp+10h],0
        je eventdtor7_4B0A88
        mov         eax,dword ptr [ecx]
        push        1
        call        dword ptr [eax]
eventdtor7_4B0A88:
        mov         ecx,dword ptr [esp+8]
        mov         dword ptr [esi],85AB38h
        pop         esi
        mov         dword ptr fs:[00000000h],ecx
        add         esp,10h
        ret
    }
}
#pragma warning(pop)
