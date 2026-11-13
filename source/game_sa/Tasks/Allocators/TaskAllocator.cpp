/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocator
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

#include "TaskAllocator.h"

namespace {
// Cuerpo no virtual de ~CTaskAllocator del EXE (thiscall, sin argumentos de pila, `ret`, 7 bytes): solo deja la vtable de CTaskAllocator del EXE (0x870E30). Lo llaman por salto directo los destructores de limpieza (SEH) de los constructores de los asignadores derivados. (Lote 121A, banco Fn121A.)
void __fastcall TaskAllocatorDestructorBody(CTaskAllocator* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870E30;
}

// 0x69DAC0 (lote 179A, sin validar): destructor con borrado de CTaskAllocator del EXE (ranura 0 de su vtable 0x870E30; thiscall, un argumento de pila, `ret 4`, 56 bytes). Deja SIEMPRE la vtable de la clase (0x870E30) y, con el
// bit 0 del BYTE de banderas, libera `this` con la liberacion en linea del pool de asignadores (0xB744BC; la misma de CTaskAllocator::operator delete 0x69D990). Devuelve `this` (EAX = ECX de entrada).
__declspec(noinline) CTaskAllocator* __fastcall TaskAllocatorScalarDeletingDestructor(CTaskAllocator* self, void* /*edx*/, uint8 flags) {
    *reinterpret_cast<uint32*>(self) = 0x870E30;
    if (flags & 1) {
        CTaskAllocator::operator delete(self);
    }
    return self;
}
} // namespace

// 0x69D980
void* CTaskAllocator::operator new(size_t size) {
    return GetTaskAllocatorPool()->New();
}

// 0x69D990
void CTaskAllocator::operator delete(void* obj) {
    GetTaskAllocatorPool()->Delete(static_cast<CTaskAllocator*>(obj));
}

bool CTaskAllocator::IsFinished(CPedGroupIntelligence* intel) {
    return rng::all_of(
        intel->GetPedTaskPairs(),
        [](const CPedTaskPair& tp) { return !tp.Task; }
    );
}

void CTaskAllocator::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocator, 0x870E30, 6);
    RH_ScopedCategory("Tasks/Allocators/TaskAllocator");

    RH_ScopedNamedGlobalInstall(TaskAllocatorDestructorBody, "Destructor", 0x69BB40);
    RH_ScopedNamedGlobalInstall(TaskAllocatorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x69DAC0); // lote 179A (sin validar)

    RH_ScopedInstall(operator new, 0x69D980);
    RH_ScopedInstall(operator delete, 0x69D990);
    RH_ScopedVMTInstall(ProcessGroup, 0x69BB50);
    RH_ScopedVMTInstall(IsFinished, 0x69C3C0);
    RH_ScopedVMTInstall(Abort, 0x5F68E0);
}
