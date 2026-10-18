/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventScriptCommand
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

#include "EventScriptCommand.h"
#include "X87Math.h"

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

namespace {
// Marco de excepciones del original (EXCEPTION_REGISTRATION_RECORD + datos propios): su manejador (0x83A018) solo desenrolla, ejecutando
// la accion del estado 0 (0x83A010 -> 0x4AC010), que deja la vtable de CEvent. Aqui el manejador hace lo mismo y no atiende la excepcion.
struct DestructorFrame {
    DestructorFrame*   next;
    void*              handler;
    volatile uintptr_t* vtable;
};

EXCEPTION_DISPOSITION __cdecl DestructorUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) {
    if (record->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)) {
        *static_cast<DestructorFrame*>(frame)->vtable = 0x85AB38;
    }
    return ExceptionContinueSearch;
}
} // namespace

// 0x4B0A50: destructor sin liberar. Como el original: la vtable pasa a la del propio evento (0x85B378) mientras se destruye la tarea
// (destructor virtual con el indicador de liberar) y, al terminar, a la de la clase base CEvent (0x85AB38). Igual que el original se
// registra un marco en la cadena de excepciones (fs:[0]) mientras dura la destruccion de la tarea, de modo que si lanza una excepcion
// el desenrollado deje tambien la vtable de CEvent, y se retira al terminar (sin __try/__finally del compilador: su marco y su
// epilogo cambiarian ECX/EDX tras la ultima llamada). Al salir el original deja ECX = cabeza anterior de la cadena, EAX y EDX como los
// dejo el destructor de la tarea (o la cabeza y el EDX de entrada si no hay tarea; aqui el EDX se recibe como parametro). Sin
// comprobaciones de ejecucion para no cambiar ECX/EDX tras la ultima llamada.
#pragma warning(push)
#pragma warning(disable: 4733) // el marco se registra a mano en fs:[0], como el original (manejador 0x83A018)
#pragma runtime_checks("", off)
uint32 __fastcall CEventScriptCommand::Destructor(CEventScriptCommand* self, uint32 entryEdx) {
    const uint32 previousHead = __readfsdword(0);
    uint32       eax          = previousHead;
    uint32       edx          = entryEdx;
    auto&        vtable       = *reinterpret_cast<volatile uintptr_t*>(self);
    DestructorFrame frame{ reinterpret_cast<DestructorFrame*>(static_cast<uintptr_t>(previousHead)), reinterpret_cast<void*>(&DestructorUnwind), &vtable };
    vtable = 0x85B378;
    __writefsdword(0, static_cast<uint32>(reinterpret_cast<uintptr_t>(&frame)));
    if (CTask* const task = self->m_task) {
        // El destructor de la tarea devuelve this en EAX y deja su propio residuo en EDX: se declara con retorno de 64 bits para leer ambos.
        using DeletingDestructor = uint64(__thiscall*)(CTask*, int);
        const uint64 residue = reinterpret_cast<DeletingDestructor*>(*reinterpret_cast<void***>(task))[0](task, 1);
        eax = static_cast<uint32>(residue);
        edx = static_cast<uint32>(residue >> 32);
    }
    __writefsdword(0, previousHead);
    vtable = 0x85AB38;
    return ResidueRegsCpp(previousHead, edx, eax);
}
#pragma runtime_checks("", restore)
#pragma warning(pop)
