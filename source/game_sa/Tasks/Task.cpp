/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTask
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CTaskComplex
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CTaskSimple
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
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
#include "Pools/PoolRawDelete.h"

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

namespace CTaskBody {
// 0x61A350 (lote 151A, banco Fn151A): copia byte a byte de 0x61A3A0 (`mov [ecx], 0x86D48C; ret`, 7 bytes; solo la alcanza un funclet de desenrollado, `jmp` en 0x83F0E3). EAX, ECX y EDX salen intactos (mismas
// condiciones que CTask::Destructor: optimizada por pragma, sin marco ni comprobaciones). Funcion propia (no la de CTask::Destructor) para que cada direccion tenga su hook y su ruta en el banco.
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
void __fastcall DestructorCopy(CTask* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D48C;
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)
} // namespace CTaskBody

namespace {
// 0x61A660 (lote 169A, banco Fn169A): destructor con borrado de CTask del EXE (ranura 0 de su vtable 0x86D48C; thiscall, un argumento de pila, `ret 4`, 56 bytes). Como el original: deja la vtable de CTask del EXE
// (0x86D48C), y solo si el bit 0 del BYTE de banderas esta puesto libera la ranura de `this` en el pool de tareas (0xB744A8) EN LINEA (indice = desplazamiento aritmetico de 7 bits, estados[indice] |= 0x80,
// cursor = min(cursor, indice) con signo, igual que CTask::operator delete 0x61A5B0 pero sin pasar por el); devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTask* __fastcall CTaskScalarDeletingDestructor(CTask* self, void* /*edx*/, uint8 flags) {
    *reinterpret_cast<uint32*>(self) = 0x86D48C;
    if (flags & 1) {
        PoolFreeSlotRawShift(GetTaskPool(), self, 7);
    }
    return self;
}
} // namespace

class CTaskComplex;

namespace CTaskSimpleBody {
// 0x61A390 (thiscall, sin argumentos de pila): cuerpo no virtual del constructor de CTaskSimple. Pone m_Parent (+4) a 0, deja la vtable de CTaskSimple del EXE (0x86D4A8) y devuelve
// this en EAX; ECX y EDX salen intactos. Los 200 llamadores son constructores de tareas simples del EXE (la vtable de la DLL no vale para ellos).
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
void* __fastcall Constructor(void* self, void* /*edx*/) {
    uint32* const words = static_cast<uint32*>(self);
    words[1] = 0;
    words[0] = 0x86D4A8;
    return self;
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)

static void InjectHooks() {
    RH_ScopedClass(CTaskSimple);
    RH_ScopedCategory("Tasks");

    RH_ScopedGlobalInstall(Constructor, 0x61A390);
}
} // namespace CTaskSimpleBody

namespace CTaskComplexBody {
// 0x61A3D0 (thiscall, sin argumentos de pila): cuerpo no virtual de ~CTaskComplex. Deja la vtable de CTaskComplex del EXE (0x86D4CC), borra la subtarea (m_pSubTask,
// +8) con su destructor virtual con borrado (`push 1; call [vtable]`) si no es nula, pone m_pSubTask a 0 y deja la vtable de CTask del EXE (0x86D48C). El EXE lo envuelve
// en un marco SEH (sólo desenrolla); aquí se omite. Los llamadores (destructores de las tareas complejas, con `call` o `jmp`) no leen EAX, ECX ni EDX tras él.
// __fastcall con un segundo registro sin usar equivale a thiscall sin argumentos de pila (`ret`).
#pragma runtime_checks("", off)
void __fastcall Destructor(CTask* self, void* /*edx*/) {
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

// 0x61A3B0 (thiscall, sin argumentos de pila): cuerpo no virtual del constructor de CTaskComplex. Pone m_Parent (+4) y m_pSubTask (+8) a 0, deja la vtable de CTaskComplex del EXE
// (0x86D4CC, la de la DLL no vale: las subclases del EXE la sobrescriben) y devuelve this en EAX. El EXE usa ECX como registro de trabajo (xor ecx,ecx): ECX no es contrato; EDX se conserva.
// Los 198 llamadores son constructores de tareas complejas del EXE. __fastcall con un segundo registro sin usar equivale a thiscall sin argumentos de pila (`ret`).
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
void* __fastcall Constructor(void* self, void* /*edx*/) {
    uint32* const words = static_cast<uint32*>(self);
    words[1] = 0;
    words[0] = 0x86D4CC;
    words[2] = 0;
    return self;
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)

// 0x61A430 (lote 151A, banco Fn151A; el listado decia CPlayerRelationshipRecorder): CTaskComplex::SetSubTask (ranura 7 de unas cien vtables del EXE; thiscall, un argumento de pila, `ret 4`, 40 bytes). Como el original:
// si la subtarea guardada (+8) es la misma, no hace nada; si no, borra la antigua (si no es nula) con su destructor virtual con borrado (`push 1; call [vtable]`) ANTES de guardar la nueva, y si la nueva no es
// nula le pone el padre (+4) a esta tarea. Se engancha como S (la entrada sirve a todas las vtables; un hook V solo cambiaria la de CTaskComplex). La de la cabecera es `final` y no se puede tomar su direccion.
void __fastcall SetSubTask(CTask* self, void* /*edx*/, CTask* subTask) {
    CTask*& current = *reinterpret_cast<CTask**>(reinterpret_cast<uint8*>(self) + 8);
    if (current == subTask) {
        return;
    }
    if (CTask* const old = current) {
        delete old;
    }
    current = subTask;
    if (subTask) {
        *reinterpret_cast<CTask**>(reinterpret_cast<uint8*>(subTask) + 4) = self;
    }
}

static void InjectHooks() {
    RH_ScopedClass(CTaskComplex);
    RH_ScopedCategory("Tasks");

    RH_ScopedGlobalInstall(Destructor, 0x61A3D0);
    RH_ScopedGlobalInstall(Constructor, 0x61A3B0);
    RH_ScopedGlobalInstall(SetSubTask, 0x61A430); // lote 151A (banco Fn151A)
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

    RH_ScopedInstall(IsGoToTask, 0x61A360);
    RH_ScopedInstall(IsTaskPtr, 0x61A4B0);
    RH_ScopedInstall(Destructor, 0x61A3A0);
    RH_ScopedNamedGlobalInstall(CTaskBody::DestructorCopy, "Destructor-2", 0x61A350); // lote 151A (banco Fn151A): copia byte a byte de 0x61A3A0
    RH_ScopedNamedGlobalInstall(CTaskScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61A660); // lote 169A (banco Fn169A)
    RH_ScopedInstall(operator new, 0x61A5A0);
    RH_ScopedInstall(operator delete, 0x61A5B0);
    RH_ScopedVMTInstall(StopTimer, 0x421180);

    CTaskSimpleBody::InjectHooks();
    CTaskSimple::InjectHooks();
    CTaskComplexBody::InjectHooks();
    CTaskComplex::InjectHooks();
}
