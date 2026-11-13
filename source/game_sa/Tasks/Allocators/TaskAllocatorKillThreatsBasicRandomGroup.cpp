/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocatorKillThreatsBasicRandomGroup
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

#include "TaskAllocatorKillThreatsBasicRandomGroup.h"
#include "TaskSimpleNone.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexKillPedGroupOnFoot.h"
#include "TaskComplexSeekCoverUntilTargetDead.h"
#include "TaskSimpleLookAbout.h"

#include "InterestingEvents.h"

// 0x69D460
void CTaskAllocatorKillThreatsBasicRandomGroup::AllocateTasks(CPedGroupIntelligence* intel) {
    intel->FlushTasks(intel->GetPedTaskPairs(), nullptr);
    intel->FlushTasks(intel->GetSecondaryPedTaskPairs(), nullptr);

    if (!m_Threat) {
        return;
    }

    auto* const group = &intel->GetPedGroup();
    if (auto* const threatsGroup = m_Threat->GetGroup()) {
        if (threatsGroup == group) {
            NOTSA_LOG_DEBUG("ComputeKillThreatsBasicResponse() - el peatón amenaza ya está en el grupo"); // vanilla
        } else {
            CPed* closest[TOTAL_PED_GROUP_MEMBERS]{};
            ComputeClosestPeds(*group, *threatsGroup, closest);
            for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) { // 0x69D539
                auto* mem = group->GetMembership().GetMember(i);
                if (!mem || mem->IsPlayer()) {
                    continue;
                }
                if (!mem->GetActiveWeapon().IsTypeMelee() || m_Threat->GetActiveWeapon().IsTypeMelee()) {
                    intel->SetEventResponseTask(mem, CTaskComplexKillPedGroupOnFoot{ threatsGroup->GetId(), closest[i] });
                } else {
                    intel->SetEventResponseTask(mem, CTaskComplexSeekCoverUntilTargetDead{ threatsGroup->GetId() });
                }
            }
            g_InterestingEvents.Add(CInterestingEvents::GANG_FIGHT, group->GetMembership().GetLeader());
        }
    } else {
        for (auto* const mem : group->GetMembership().GetMembers()) {
            if (mem->IsPlayer()) {
                continue;
            }
            if (!mem->GetActiveWeapon().IsTypeMelee() || m_Threat->GetActiveWeapon().IsTypeMelee()) { // 0x69D6ED
                intel->SetEventResponseTask( // 0x69D864
                    mem,
                    CTaskComplexSequence{
                        new CTaskComplexKillPedOnFoot{ m_Threat },
                        new CTaskSimpleLookAbout{ (uint32)CGeneral::GetRandomIntInRange(1000, 2000) }, // Como el original: 0x407180, [1000, 2000)
                    }
                );
            } else {
                intel->SetEventResponseTask(mem, CTaskComplexSeekCoverUntilTargetDead{ m_Threat }); // 0x69D71F
            }
        }
        g_InterestingEvents.Add(CInterestingEvents::GANG_ATTACKING_PED, group->GetMembership().GetLeader());
    }
}

namespace {
// 0x5F6920 (lote 141A, banco Fn141A): cuerpo no virtual de ~CTaskAllocatorKillThreatsBasicRandomGroup del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86C80C;
// jmp 0x69C780`). Deja la vtable de la clase del EXE (0x86C80C) y sigue con el cuerpo de ~CTaskAllocatorKillThreatsBasic (0x69C780, con hook, envoltura Destructor()).
__declspec(noinline) void __fastcall KillThreatsBasicRandomGroupDestructorBody(CTaskAllocatorKillThreatsBasicRandomGroup* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C80C;
    static_cast<CTaskAllocatorKillThreatsBasic*>(self)->Destructor();
}
} // namespace

// 0x5F6900 (lote 141A, banco Fn141A): destructor con borrado de CTaskAllocatorKillThreatsBasicRandomGroup del EXE (ranura 0 de su vtable 0x86C80C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x5F6920; test byte [esp+8],1; je ..; push esi; call 0x69D990; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x5F6920), libera `this` con CTaskAllocator::operator delete (0x69D990, con hook; pool de asignadores) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskAllocatorKillThreatsBasicRandomGroup* CTaskAllocatorKillThreatsBasicRandomGroup::ScalarDeletingDestructor(uint8 flags) {
    KillThreatsBasicRandomGroupDestructorBody(this, nullptr);
    if (flags & 1) {
        CTaskAllocator::operator delete(this);
    }
    return this;
}

void CTaskAllocatorKillThreatsBasicRandomGroup::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorKillThreatsBasicRandomGroup, 0x86C80C, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedNamedGlobalInstall(KillThreatsBasicRandomGroupDestructorBody, "Destructor", 0x5F6920); // lote 141A (banco Fn141A)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x5F6900); // lote 141A (banco Fn141A)

    // Como el original son funciones virtuales (huecos 5 y 1 de 0x86C80C): con RH_ScopedInstall el salto iría al
    // thunk vcall, que vuelve a despachar por la vtable del EXE y entra en recursión infinita.
    RH_ScopedVMTInstall(GetType, 0x5F68F0);
    RH_ScopedVMTInstall(AllocateTasks, 0x69D460);
}
