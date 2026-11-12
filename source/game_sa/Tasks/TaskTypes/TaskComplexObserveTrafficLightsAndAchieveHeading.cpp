/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexObserveTrafficLightsAndAchieveHeading
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

#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"

#include "TaskComplexObserveTrafficLights.h"
#include "TaskSimpleAchieveHeading.h"

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
void __fastcall Destructor(CTask* self, void* edx);  // Task.cpp: 0x61A3D0
}

static_assert(int32(TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING) == 0xE1); // 0x631940: mov eax,0E1h

namespace {
// 0x631910 (lote 141A, banco Fn141A): cuerpo del constructor de CTaskComplexObserveTrafficLightsAndAchieveHeading del EXE (thiscall, dos argumentos de pila, `ret 8`, 34 bytes). Base
// CTaskComplex (0x61A3B0, con hook); +0xC = primer argumento (duracion), vtable 0x86DFC4 y +0x10 = segundo argumento copiado como dword (sin pasar por el x87); devuelve this.
CTaskComplexObserveTrafficLightsAndAchieveHeading* __fastcall ObserveTrafficLightsAndAchieveHeadingConstructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint32 durationInMs, uint32 headingBits) {
    CTaskComplexBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes + 0xC) = durationInMs;
    *reinterpret_cast<uint32*>(bytes) = 0x86DFC4;
    *reinterpret_cast<uint32*>(bytes + 0x10) = headingBits;
    return self;
}
} // namespace

namespace {
// 0x631980 (lote 163A, banco Fn163A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86DFC4h; jmp 0x61A3D0`): cuerpo del destructor de CTaskComplexObserveTrafficLightsAndAchieveHeading del EXE.
// Deja la vtable de la clase del EXE (0x86DFC4) y sigue con el cuerpo de ~CTaskComplex (0x61A3D0, con hook). Antes la DLL usaba el destructor generado de la clase (que deja la vtable de la DLL).
// Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall CTaskComplexObserveTrafficLightsAndAchieveHeadingDestructorBody(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DFC4;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x637F80 (lote 157A, banco Fn157A): destructor con borrado de CTaskComplexObserveTrafficLightsAndAchieveHeading del EXE (ranura 0 de su vtable 0x86DFC4; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x631980; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexObserveTrafficLightsAndAchieveHeading* __fastcall CTaskComplexObserveTrafficLightsAndAchieveHeadingScalarDeletingDestructor(CTaskComplexObserveTrafficLightsAndAchieveHeading* self, void* /*edx*/, uint8 flags) {
    CTaskComplexObserveTrafficLightsAndAchieveHeadingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskComplexObserveTrafficLightsAndAchieveHeading() { return reinterpret_cast<uintptr_t>(&CTaskComplexObserveTrafficLightsAndAchieveHeadingDestructorBody); } // para el banco Fn157A

void CTaskComplexObserveTrafficLightsAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexObserveTrafficLightsAndAchieveHeading, 0x86DFC4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexObserveTrafficLightsAndAchieveHeadingDestructorBody, "DestructorBody", 0x631980); // lote 163A (banco Fn163A)
    RH_ScopedNamedGlobalInstall(CTaskComplexObserveTrafficLightsAndAchieveHeadingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x637F80); // lote 157A (banco Fn157A)

    RH_ScopedNamedGlobalInstall(ObserveTrafficLightsAndAchieveHeadingConstructorBody, "Constructor", 0x631910); // lote 141A (banco Fn141A)
    RH_ScopedInstall(CreateSubTask, 0x631990); // lote 170A (banco Fn170A)

    RH_ScopedVMTInstall(GetTaskType, 0x631940); // lote 141A (banco Fn141A): mov eax,0E1h
    RH_ScopedVMTInstall(Clone, 0x636490);
    RH_ScopedVMTInstall(ControlSubTask, 0x631AD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x631AC0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631A70);
    RH_ScopedVMTInstall(MakeAbortable, 0x631950);
}

// 0x631910
CTaskComplexObserveTrafficLightsAndAchieveHeading::CTaskComplexObserveTrafficLightsAndAchieveHeading(int32 durationInMs, float targetHeading) : CTaskComplex() {
    m_nDurationInMs = durationInMs;
    m_fTargetHeading = targetHeading;
}

// 0x636490
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::Clone() const {
    return new CTaskComplexObserveTrafficLightsAndAchieveHeading(m_nDurationInMs, m_fTargetHeading);
}

// 0x631AD0
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x631AC0
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
}

// 0x631950
bool CTaskComplexObserveTrafficLightsAndAchieveHeading::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) && m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x631A70
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS);
    default:
        return nullptr;
    }
}

// 0x631990 (lote 170A, banco Fn170A; thiscall, un argumento de pila, `ret 4`, 218 bytes con marco SEH que solo desenrolla): como el original, con el tipo 0xE0 reserva 0x18 bytes con CTask::operator new (0x61A5A0, con
// hook) y, si no es nulo, construye en linea una CTaskComplexObserveTrafficLights (base CTaskComplex 0x61A3B0, con hook; vtable del EXE 0x86DF98; +0xC = +0x10 = 0; bytes +0x14 y +0x15 = 0; sin escribir el relleno +0x16..+0x17) y
// devuelve el objeto; con el tipo 0x386 reserva 0x18 bytes y, si no es nulo, llama al constructor de CTaskSimpleAchieveHeading (0x667E20, con hook) con (rumbo = +0x10 de esta tarea, 0.5f, 0.2f) y devuelve SU EAX (no el
// puntero reservado); cualquier otro tipo (0x516 incluido) o sin memoria devuelve nulo. Los objetos nuevos llevan la vtable del EXE, no la de la DLL.
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask(eTaskType taskType) const {
    if (taskType == TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS) {
        void* const memory = CTask::operator new(sizeof(CTaskComplexObserveTrafficLights));
        if (!memory) {
            return nullptr;
        }
        CTaskComplexBody::Constructor(memory, nullptr);
        uint8* const bytes = static_cast<uint8*>(memory);
        *reinterpret_cast<uint32*>(bytes) = 0x86DF98;
        *reinterpret_cast<uint32*>(bytes + 0xC) = 0;
        *reinterpret_cast<uint32*>(bytes + 0x10) = 0;
        bytes[0x14] = 0;
        bytes[0x15] = 0;
        return static_cast<CTask*>(memory);
    }
    if (taskType == TASK_SIMPLE_ACHIEVE_HEADING) {
        auto* const memory = static_cast<CTaskSimpleAchieveHeading*>(CTask::operator new(sizeof(CTaskSimpleAchieveHeading)));
        if (!memory) {
            return nullptr;
        }
        return memory->Constructor(m_fTargetHeading, 0.5f, 0.2f);
    }
    return nullptr;
}
