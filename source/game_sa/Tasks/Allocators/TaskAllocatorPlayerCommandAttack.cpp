/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocatorPlayerCommandAttack
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexLeaveCar.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexLeaveCar.h"

#include "TaskAllocatorPlayerCommandAttack.h"

// 0x69C480
CTaskAllocatorPlayerCommandAttack::CTaskAllocatorPlayerCommandAttack(CPed* target, int32 groupTargetID, ePedType pedTypeToAttack) :
    CTaskAllocatorAttack{target, groupTargetID, FindPlayerPed()},
    m_PedTypeToAttack{pedTypeToAttack}
{ }

// 0x69C4D0
void CTaskAllocatorPlayerCommandAttack::AllocateTasks(CPedGroupIntelligence* intel) {
    // Como el original: 0x69C4FA compara con signo (`jl`) el tipo de peaton contra 0; solo un tipo negativo
    // ataca al objetivo concreto (no se comprueba PED_TYPE_PLAYER1/PLAYER2).
    if ((int32)m_PedTypeToAttack < 0) {
        if (m_Target) {
            CTaskAllocatorAttack::AllocateTasks(intel);
        }
        return;
    }

    m_NextUpdateTimer.Start(3'000);

    auto* const group = &intel->GetPedGroup();

    CPedList attackers{};
    attackers.BuildListFromGroup_NoLeader(group->GetMembership()); // 0x69C52C
    // Como el original: GetLeader se llama dos veces (0x69C537 y 0x69C546).
    if (group->GetMembership().GetLeader()) { // 0x69C537
        if (group->GetMembership().GetLeader()->IsPlayer()) {
            attackers.RemovePedsThatDontListenToPlayer();
        }
    }
    attackers.RemovePedsAttackingPedType(m_PedTypeToAttack); // 0x69C567

    CFormation::m_DestinationPeds.BuildListOfPedsOfPedType(m_PedTypeToAttack);
    CFormation::DistributeDestinations_PedsToAttack(attackers);

    for (auto* const p : attackers.GetPeds()) { // 0x69C6C8
        CTaskComplexSequence tseq{};

        // 0x69C5C9
        if (p->IsInVehicle()) {
            tseq.AddTask(new CTaskComplexLeaveCar{ p->m_pVehicle, 0, 0, true, false });
        }

        // 0x69C60F
        CPed* target;
        CFormation::ReturnTargetPedForPed(p, &target);
        tseq.AddTask(new CTaskComplexKillPedOnFoot{ target, -1, 0, 0, 0, 2 });

        // 0x69C67F
        tseq.AddTask(new CTaskSimpleStandStill{ 999'999, true });

        // 0x69C6A8
        // Como el original: llama directamente a SetTask (0x5F7540) con los pares principales, slot -1 y sin forzar.
        intel->SetTask(p, tseq, intel->GetPedTaskPairs(), TASK_SECONDARY_INVALID, false);
    }
}

// 0x69D110
CTaskAllocator* CTaskAllocatorPlayerCommandAttack::ProcessGroup(CPedGroupIntelligence* intel) {
    // Como el original: CTaskAllocatorAttack::ProcessGroup esta en linea y llama a CTaskAllocatorAttack::AllocateTasks
    // (0x69C3F0) de forma directa, no a la version virtual de esta clase.
    if (!m_NextUpdateTimer.IsStarted() || m_NextUpdateTimer.IsOutOfTime()) {
        CTaskAllocatorAttack::AllocateTasks(intel);
    }
    return IsFinished(intel)
        ? nullptr
        : this;
}

namespace {
// 0x69C480 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskAllocatorPlayerCommandAttack del EXE (thiscall, un puntero a peaton y dos dwords en la pila, `ret 0Ch`, devuelve this). Primero CWorld::FindPlayerPed(-1) (0x56E210), despues el constructor de CTaskAllocatorAttack (0x69C240) con (objetivo, id de grupo, jugador), vtable 0x870E78 y +0x1C = tercer argumento.
CTaskAllocatorPlayerCommandAttack* __fastcall PlayerCommandAttackConstructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, CPed* target, int32 groupTargetId, uint32 value) {
    CPed* const player = FindPlayerPed(-1);
    reinterpret_cast<CTaskAllocatorAttack*>(self)->Constructor(target, groupTargetId, player);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x870E78;
    *reinterpret_cast<uint32*>(bytes + 0x1C) = value;
    return self;
}

// 0x69DB40 (lote 179A, sin validar): cuerpo no virtual de ~CTaskAllocatorPlayerCommandAttack del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],870E78h ; jmp 0x69C2D0`). Deja la vtable de la clase del
// EXE (0x870E78) y salta al cuerpo de ~CTaskAllocatorAttack (0x69C2D0, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x69DB40` del EXE.
__declspec(noinline) void __fastcall PlayerCommandAttackDestructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870E78;
    static_cast<CTaskAllocatorAttack*>(self)->Destructor();
}

// 0x69DB00 (lote 179A, sin validar): destructor con borrado de CTaskAllocatorPlayerCommandAttack del EXE (ranura 0 de su vtable 0x870E78; thiscall, un argumento de pila, `ret 4`, 59 bytes: `call 0x69DB40; test byte [esp+8],1; ..`
// con la liberacion en linea del pool de asignadores (la de CTaskAllocator::operator delete 0x69D990) si el bit 0 esta puesto; devuelve `this`).
__declspec(noinline) CTaskAllocatorPlayerCommandAttack* __fastcall PlayerCommandAttackScalarDeletingDestructor(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, uint8 flags) {
    PlayerCommandAttackDestructorBody(self, nullptr);
    if (flags & 1) {
        CTaskAllocator::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskAllocatorPlayerCommandAttack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorPlayerCommandAttack, 0x870e78, 6);
    RH_ScopedCategory("Tasks/Allocators");
    RH_ScopedNamedGlobalInstall(PlayerCommandAttackDestructorBody, "DestructorBody", 0x69DB40);                     // lote 179A (sin validar)
    RH_ScopedNamedGlobalInstall(PlayerCommandAttackScalarDeletingDestructor, "ScalarDeletingDestructor", 0x69DB00); // lote 179A (sin validar)
    RH_ScopedNamedGlobalInstall(PlayerCommandAttackConstructorBody, "Constructor", 0x69C480);

    RH_ScopedVMTInstall(AllocateTasks, 0x69C4D0);
    RH_ScopedVMTInstall(GetType, 0x69C4C0);
    RH_ScopedVMTInstall(ProcessGroup, 0x69D110);
}
