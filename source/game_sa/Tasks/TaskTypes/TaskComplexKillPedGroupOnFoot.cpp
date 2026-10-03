/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillPedGroupOnFoot
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
#include "TaskComplexKillPedGroupOnFoot.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleLookAbout.h"
#include "TaskSimpleNone.h"

void CTaskComplexKillPedGroupOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedGroupOnFoot, 0x86da48, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x622240);
    RH_ScopedInstall(Destructor, 0x6222B0);

    RH_ScopedInstall(CreateSubTask, 0x622320);
    RH_ScopedInstall(ComputeNearestLivingGroupPed, 0x622450);

    RH_ScopedVMTInstall(Clone, 0x6237C0);
    RH_ScopedVMTInstall(GetTaskType, 0x6222A0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x629570);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x629630);
    RH_ScopedVMTInstall(ControlSubTask, 0x622310);
}

// 0x622240
CTaskComplexKillPedGroupOnFoot::CTaskComplexKillPedGroupOnFoot(int32 groupId, CPed* ped) :
    m_grpId{groupId},
    m_currPedToKill{ped}
{
    CEntity::SafeRegisterRef(m_currPedToKill);
}

// NOTSA (For 0x6237C0)
CTaskComplexKillPedGroupOnFoot::CTaskComplexKillPedGroupOnFoot(const CTaskComplexKillPedGroupOnFoot& o) :
    CTaskComplexKillPedGroupOnFoot{o.m_grpId, o.m_currPedToKill}
{
}

// 0x6222B0
CTaskComplexKillPedGroupOnFoot::~CTaskComplexKillPedGroupOnFoot() {
    CEntity::SafeCleanUpRef(m_currPedToKill);
}

// 0x622320
CTask* CTaskComplexKillPedGroupOnFoot::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType)
    {
    case TASK_FINISHED:
        return nullptr;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: {
        return new CTaskComplexKillPedOnFoot{
            m_currPedToKill,
            -1,
            0,
            0,
            0,
            true
        };
    }
    case TASK_NONE:
        return new CTaskSimpleNone{};
    case TASK_SIMPLE_LOOK_ABOUT:
        // Como el original: CGeneral::GetRandomNumberInRange(int, int) (0x407180), [3000, 4000)
        return new CTaskSimpleLookAbout{ (uint32)CGeneral::GetRandomIntInRange(3000, 4000) };
    default:
        return nullptr; // Como el original
    }
}

// 0x622450
CPed* CTaskComplexKillPedGroupOnFoot::ComputeNearestLivingGroupPed(CPed& ped, bool flag) {
    auto& membership = CPedGroups::ms_groups.data()[m_grpId].GetMembership(); // Como el original: sin comprobar si el grupo está activo

    // Como el original: se ignora el objetivo del líder del grupo del peatón (si no es él mismo); si el líder no apunta
    // a un peatón, se ignora el miembro más cercano al líder
    CPed* pedToIgnore = nullptr;
    if (!flag) {
        if (const auto grp = CPedGroups::GetPedsGroup(&ped)) {
            if (const auto leader = grp->GetMembership().GetLeader(); leader && leader != &ped) {
                const auto target = leader->m_pTargetedObject;
                pedToIgnore = target && target->GetIsTypePed()
                    ? target->AsPed()
                    : ComputeNearestLivingGroupPed(*leader, true);
            }
        }
    }

    CPed* closest       = nullptr;
    float closestDistSq = FLT_MAX;
    for (auto i = 0; i < 8; i++) {
        const auto mem = membership.GetMember(i);
        if (!mem || mem == pedToIgnore || !mem->IsAlive()) {
            continue;
        }
        if (!(mem->m_fHealth > 0.f)) { // Como el original: NaN tampoco cuenta como vivo
            continue;
        }
        if (mem == &ped || CPedGroups::AreInSameGroup(mem, &ped)) {
            continue;
        }

        // Como el original: distancia (z·z + y·y) + x·x, estrictamente menor (NaN no cuenta)
        const auto& memPos = mem->GetPosition();
        const auto& pedPos = ped.GetPosition();
        const float dx = memPos.x - pedPos.x, dy = memPos.y - pedPos.y, dz = memPos.z - pedPos.z;
        const float distSq = dz * dz + dy * dy + dx * dx;
        if (!(distSq < closestDistSq)) {
            continue;
        }

        if (mem->IsPlayer()) {
            if (const auto wanted = mem->AsPlayer()->GetWanted()) { // NOTSA: el EXE no comprueba los datos de jugador nulos
                if (wanted->m_NumCopsInPursuit && ped.m_nPedType != PED_TYPE_COP && ped.IsCreatedBy(PED_GAME)) {
                    continue;
                }
            }
        }

        closestDistSq = distSq;
        closest       = mem;
    }

    return closest ? closest : pedToIgnore; // Como el original: sin miembro válido devuelve el peatón ignorado
}

// 0x629570
CTask* CTaskComplexKillPedGroupOnFoot::CreateNextSubTask(CPed* ped) {
    CEntity::ClearReference(m_currPedToKill); // Como el original: se suelta el objetivo antes de mirar la subtarea

    switch (m_pSubTask->GetTaskType()) {
    case TASK_NONE:
    case TASK_SIMPLE_LOOK_ABOUT:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // Kill next or finish
        m_currPedToKill = ComputeNearestLivingGroupPed(*ped);
        if (!m_currPedToKill) {
            return CreateSubTask(TASK_SIMPLE_LOOK_ABOUT, ped);
        }
        m_currPedToKill->RegisterReference(reinterpret_cast<CEntity**>(&m_currPedToKill));
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    }
    default:
        return nullptr; // Como el original
    }
}

// 0x629630
CTask* CTaskComplexKillPedGroupOnFoot::CreateFirstSubTask(CPed* ped) {
    if (m_currPedToKill) {
        return CreateSubTask(CPedGroups::AreInSameGroup(ped, m_currPedToKill) ? TASK_NONE : TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    }
    // Como el original: sin objetivo cercano crea TASK_NONE (no LOOK_ABOUT)
    m_currPedToKill = ComputeNearestLivingGroupPed(*ped);
    if (!m_currPedToKill) {
        return CreateSubTask(TASK_NONE, ped);
    }
    m_currPedToKill->RegisterReference(reinterpret_cast<CEntity**>(&m_currPedToKill));
    return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
}
