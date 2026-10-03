/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSeekCoverUntilTargetDead
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
#include "TaskComplexSeekCoverUntilTargetDead.h"
#include "SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexSeekCoverUntilTargetDead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekCoverUntilTargetDead, 0x86dac4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor_Ped, 0x622BC0);
    RH_ScopedInstall(Constructor_Grp, 0x622B80);
    RH_ScopedInstall(Destructor, 0x622C20);

    RH_ScopedInstall(GetCoverPed, 0x622D50);
    RH_ScopedInstall(GetTargetPed, 0x622CC0);

    RH_ScopedVMTInstall(Clone, 0x623900);
    RH_ScopedVMTInstall(GetTaskType, 0x622BB0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x622C90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62E6C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x62E8B0);
}

// 0x622BC0
CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(CPed* ped) :
    m_targetPed{ped}
{
    CEntity::SafeRegisterRef(m_targetPed);
}

// 0x622B80
CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(PedGroupId grpId) :
    m_groupId{ grpId }
{
}

CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(const CTaskComplexSeekCoverUntilTargetDead& o) :
    m_groupId{ o.m_groupId },
    m_targetPed{ o.m_groupId == -1 ? o.m_targetPed : nullptr }
{
    CEntity::SafeRegisterRef(m_targetPed);
}

// 0x622C20
CTaskComplexSeekCoverUntilTargetDead::~CTaskComplexSeekCoverUntilTargetDead() {
    CEntity::SafeCleanUpRef(m_targetPed);
    CEntity::SafeCleanUpRef(m_coverPed);
}

// 0x622D50
CPed* CTaskComplexSeekCoverUntilTargetDead::GetCoverPed(CPed* ped) {
    // Como el original: GetPedsGroup y recorrido de los 8 miembros (lider incluido)
    const auto grp = CPedGroups::GetPedsGroup(ped);
    if (!grp) {
        return nullptr;
    }
    float closestDistSq = std::numeric_limits<float>::max();
    CPed* closest{};
    for (auto i = 0; i < 8; i++) {
        const auto mem = grp->GetMembership().GetMember(i);
        if (!mem || mem == ped) {
            continue;
        }
        if (mem->GetActiveWeapon().IsTypeMelee() || !mem->IsAlive()) {
            continue;
        }
        // Como el original: (peaton - miembro), suma (z*z + x*x) + y*y y comparacion estricta (NaN no cuenta)
        const auto& memPos = mem->GetPosition();
        const auto& pedPos = ped->GetPosition();
        const float dx = pedPos.x - memPos.x;
        const float dy = pedPos.y - memPos.y;
        const float dz = pedPos.z - memPos.z;
        const float distSq = (dz * dz + dx * dx) + dy * dy;
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest       = mem;
        }
    }
    return closest;
}

// 0x622CC0
CPed* CTaskComplexSeekCoverUntilTargetDead::GetTargetPed() const {
    if (m_groupId < 0) { // Como el original: comparacion con signo (jl), no solo -1
        return m_targetPed && m_targetPed->IsAlive()
            ? m_targetPed
            : nullptr;
    }

    // Otherwise get a member from the group that is alive

    auto& group = CPedGroups::ms_groups.data()[m_groupId]; // Como el original: sin comprobar que el grupo este activo

    if (const auto leader = group.GetMembership().GetLeader()) {
        if (leader->IsAlive()) {
            return leader;
        }
    }

    // Como el original: solo los 7 seguidores (GetMember 0..6)
    for (auto i = 0; i < 7; i++) {
        if (const auto mem = group.GetMembership().GetMember(i)) {
            if (mem->IsAlive()) {
                return mem;
            }
        }
    }

    return nullptr;
}

// 0x622C90
CTask* CTaskComplexSeekCoverUntilTargetDead::CreateNextSubTask(CPed* ped) {
    // Como el original: tras STAND_STILL (la ultima tarea) termina; con cualquier otra subtarea vuelve a empezar
    return m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL
        ? nullptr
        : CreateFirstSubTask(ped);
}

// 0x62E6C0
CTask* CTaskComplexSeekCoverUntilTargetDead::CreateFirstSubTask(CPed* ped) {
    // Como el original: limpia objetivo y cubierto, calcula ambos y solo registra las referencias
    // (objetivo y luego cubierto) si los dos existen
    CEntity::SafeCleanUpRef(m_targetPed);
    CEntity::SafeCleanUpRef(m_coverPed);
    m_coverPed  = GetCoverPed(ped);
    m_targetPed = GetTargetPed();

    if (!m_coverPed || !m_targetPed) {
        return nullptr;
    }

    m_targetPed->RegisterReference(reinterpret_cast<CEntity**>(&m_targetPed));
    m_coverPed->RegisterReference(reinterpret_cast<CEntity**>(&m_coverPed));

    return CreateSequence(ped);
}

// 0x62E8B0
CTask* CTaskComplexSeekCoverUntilTargetDead::ControlSubTask(CPed* ped) {
    // No target, or they're dead?
    if (!m_targetPed || !m_targetPed->IsAlive()) {
        // Como el original: limpia la referencia sin anular el puntero antes de buscar
        CEntity::SafeCleanUpRef(m_targetPed);
        m_targetPed = GetTargetPed(); // Try finding a new one
        CEntity::SafeRegisterRef(m_targetPed);

        // If no new one is found, try aborting current subtask, and finish up
        if (!m_targetPed && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return new CTaskSimpleStandStill{ -1 };
        }
    }

    // No ped to cover, or they're dead?
    if (!m_coverPed || !m_coverPed->IsAlive()) { // Inverted
        CEntity::SafeCleanUpRef(m_coverPed);
#ifdef FIX_BUGS
        m_coverPed = GetCoverPed(ped);
#else
        m_coverPed = GetTargetPed(); // Como el original (fallo del EXE)
#endif
        CEntity::SafeRegisterRef(m_coverPed);
        // Try aborting current subtask and returning new
        if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { // Moved out as common code
            if (m_coverPed) { // Found a new ped to cover, do it all over again
                return CreateSequence(ped);
            }
            return new CTaskSimpleStandStill{ -1 }; // No new ped to cover, so finish it up
        }
    }

    return m_pSubTask;
}

CTaskComplexSequence* CTaskComplexSeekCoverUntilTargetDead::CreateSequence(CPed* ped) {
    // Como el original: la secuencia se crea primero y las tareas se crean y se anaden en orden
    // (buscar, girarse, quedarse quieto); radio 1,5 y angulo pi exacto (0x40490FDB)
    const auto seq = new CTaskComplexSequence;
    seq->AddTask(new CTaskComplexSeekEntityRadiusAngleOffset{
        m_coverPed,
        50000,
        1000,
        1.0f,
        2.0f,
        2.0f,
        true,
        true,
        { 1.5f, std::bit_cast<float>(0x40490FDBu) }
    });
    seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord{ m_targetPed });
    seq->AddTask(new CTaskSimpleStandStill{ 2000 });
    return seq;
}
