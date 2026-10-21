/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTask
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CTaskComplex
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
// Sin PCH (CMakeLists.txt) StdInc.h redefine NOMINMAX (winincl.h) y el aviso C4005 es error: mismo recurso que Clothes.cpp, Stats.cpp y DecisionMakerTypes.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "Task.h"

// Keep the base destruction step explicit: the executable installs CTask's
// vtable after a complex task has released its owned subtask.
CTask::~CTask() {
}

// 0x61A3A0: cuerpo no virtual de ~CTask. Los destructores del EXE (259 llamadores, 148 de ellos con `jmp` final) lo llaman con `this` en ECX y esperan que
// EAX, ECX y EDX salgan intactos, asi que el codigo debe ser exactamente `mov dword ptr [ecx], 0x86D48C; ret`: se optimiza por pragma (CMakeLists.txt: /Zi y
// BasicRuntimeChecks=Default para este archivo), sin marco de pila ni comprobaciones de ejecucion. La vtable es la del EXE (no la de la DLL), como en el original.
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
void CTask::Destructor() {
    *reinterpret_cast<uint32*>(this) = 0x86D48C;
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)

class CTaskComplex;

namespace CTaskComplexBody {
// 0x61A3D0 (thiscall, sin argumentos de pila): cuerpo no virtual de ~CTaskComplex. Deja la vtable de CTaskComplex del EXE (0x86D4CC), borra la subtarea (m_pSubTask,
// +8) con su destructor virtual con borrado (`push 1; call [vtable]`) si no es nula, pone m_pSubTask a 0 y deja la vtable de CTask del EXE (0x86D48C). El EXE lo envuelve
// en un marco SEH (sólo desenrolla); aquí se omite. Los llamadores (destructores de las tareas complejas, con `call` o `jmp`) no leen EAX, ECX ni EDX tras él.
// __fastcall con un segundo registro sin usar equivale a thiscall sin argumentos de pila (`ret`).
#pragma runtime_checks("", off)
static void __fastcall Destructor(CTask* self, void* /*edx*/) {
    uint32& vtable = *reinterpret_cast<uint32*>(self);
    CTask*& subTask = *reinterpret_cast<CTask**>(reinterpret_cast<uint8*>(self) + 8);
    vtable = 0x86D4CC;
    if (CTask* const owned = subTask) {
        delete owned;
    }
    subTask = nullptr;
    vtable = 0x86D48C;
}
#pragma runtime_checks("", restore)

static void InjectHooks() {
    RH_ScopedClass(CTaskComplex);
    RH_ScopedCategory("Tasks");

    RH_ScopedGlobalInstall(Destructor, 0x61A3D0);
}
} // namespace CTaskComplexBody

// 0x61A5A0
void* CTask::operator new(size_t size) {
    return GetTaskPool()->New();
}

// 0x61A5B0
void CTask::operator delete(void* object) {
    // Como el original: marca el slot libre y baja m_LastFreeSlot (comparación con signo), sin rellenar con 0xDD
    GetTaskPool()->Delete(static_cast<CTask*>(object));
}

// 0x421180
void CTask::StopTimer(const CEvent* event) {
    // NOP
}

// 0x61A360
bool CTask::IsGoToTask(CTask* task) {
    // Como el original: el tipo se pide una vez por comparación (dos llamadas virtuales)
    return task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT
        || task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE;
}

// 0x61A4B0
bool CTask::IsTaskPtr(CTask* task) {
    // Como el original: recorre los slots ocupados de mayor a menor y compara con la dirección exacta del slot
    // (un puntero dentro de un slot pero no a su inicio no cuenta)
    auto* const pool = GetTaskPool();
    for (auto i = (int32)pool->GetSize(); i-- > 0;) {
        if (!pool->IsFreeSlotAtIndex(i) && pool->GetAt(i) == task) {
            return true;
        }
    }
    return false;
}

void CTask::InjectHooks() {
    RH_ScopedCategory("Tasks");
    RH_ScopedVirtualClass(CTask, 0x86D48C, 7);

    RH_ScopedInstall(IsTaskPtr, 0x61A4B0);
    RH_ScopedInstall(Destructor, 0x61A3A0);
    RH_ScopedInstall(operator new, 0x61A5A0);
    RH_ScopedInstall(operator delete, 0x61A5B0);
    RH_ScopedVMTInstall(StopTimer, 0x421180);

    CTaskComplexBody::InjectHooks();
}
