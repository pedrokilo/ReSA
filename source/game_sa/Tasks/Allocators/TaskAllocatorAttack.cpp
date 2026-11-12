/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocatorAttack
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskAllocatorAttack.h"
#include "TaskAllocatorKillOnFoot.h"

// 0x69C240
CTaskAllocatorAttack::CTaskAllocatorAttack(CPed* target, int32 groupTargetID, CPed* originator) :
    m_Target{target},
    m_GroupTargetID{groupTargetID},
    m_Originator{originator}
{ }

// 0x69C2D0
CTaskAllocatorAttack::~CTaskAllocatorAttack() {
    // Como el original: primero se limpia la referencia del objetivo y después la del originador (los miembros se
    // destruyen en orden inverso). El puntero no se pone a cero: la limpieza posterior de m_Target no encuentra nada.
    if (CPed* const target = m_Target) {
        target->CleanUpOldReference(reinterpret_cast<CEntity**>(std::addressof(m_Target)));
    }
}

// 0x69C3F0
void CTaskAllocatorAttack::AllocateTasks(CPedGroupIntelligence* intel) {
    m_NextUpdateTimer.Start(3'000);
    CTaskAllocatorKillOnFoot{m_Target, m_GroupTargetID}.AllocateTasks(intel);
}

// 0x69D0C0
CTaskAllocator* CTaskAllocatorAttack::ProcessGroup(CPedGroupIntelligence* intel) {
    if (!m_NextUpdateTimer.IsStarted() || m_NextUpdateTimer.IsOutOfTime()) {
        CTaskAllocatorAttack::AllocateTasks(intel); // Como el original: llamada directa (0x69C3F0), sin pasar por la vtable
    }
    return this;
}

namespace {
// 0x69DA00 (lote 179A, sin validar): destructor con borrado de CTaskAllocatorAttack del EXE (ranura 0 de su vtable 0x870E60; thiscall, un argumento de pila, `ret 4`, 59 bytes: `call 0x69C2D0; test byte [esp+8],1; ..`
// con la liberacion en linea del pool de asignadores si el bit 0 esta puesto; devuelve `this`). Cuerpo de la clase 0x69C2D0 (con hook) y CTaskAllocator::operator delete.
__declspec(noinline) CTaskAllocatorAttack* __fastcall AttackScalarDeletingDestructor(CTaskAllocatorAttack* self, void* /*edx*/, uint8 flags) {
    self->Destructor();
    if (flags & 1) {
        CTaskAllocator::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskAllocatorAttack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorAttack, 0x870e60, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedNamedGlobalInstall(AttackScalarDeletingDestructor, "ScalarDeletingDestructor", 0x69DA00); // lote 179A (sin validar)

    RH_ScopedInstall(Constructor, 0x69C240);
    RH_ScopedInstall(Destructor, 0x69C2D0);

    RH_ScopedVMTInstall(GetType, 0x69C2C0);
    RH_ScopedVMTInstall(AllocateTasks, 0x69C3F0);
    RH_ScopedVMTInstall(ProcessGroup, 0x69D0C0);
}
