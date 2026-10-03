/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFallToDeath
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexFallToDeath.h"
#include "TaskSimpleInAir.h"
#include "TaskComplexDie.h"
#include "TaskSimpleLand.h"
#include "TaskSimpleDead.h"

void CTaskComplexFallToDeath::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFallToDeath, 0x870520, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x679040);
    RH_ScopedVMTInstall(MakeAbortable, 0x6790C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x679510);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x679120);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x679270);
    RH_ScopedInstall(CalcFall, 0x6796C0);
}

CTaskComplexFallToDeath* CTaskComplexFallToDeath::Constructor(int32 direction, const CVector& posn, bool a4, bool a5) { this->CTaskComplexFallToDeath::CTaskComplexFallToDeath(direction, posn, a4, a5); return this; }

// 0x679040
CTaskComplexFallToDeath::CTaskComplexFallToDeath(int32 direction, const CVector& posn, bool bFallToDeathOverRailing, bool a5) : CTaskComplex() {
    m_Posn                  = posn;
    m_nAnimId               = ANIM_ID_UNDEFINED;
    m_nAnimId1              = ANIM_ID_UNDEFINED;
    m_nFallToDeathDir       = static_cast<eDirection>(direction);
    m_nFlags = m_nFlags & 0xE0 | (8 * ((uint8)bFallToDeathOverRailing | (2 * (uint8)a5))); // todo: flags
}

void CTaskComplexFallToDeath::UpdateAnims(CPed* ped) {
    for (auto& animId : { m_nAnimId, m_nAnimId1 }) {
        if (animId != ANIM_ID_UNDEFINED) {
            if (auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), animId)) {
                assoc->m_BlendDelta = -1000.0f;
            }
        }
    }
}

// 0x6790C0


bool CTaskComplexFallToDeath::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    UpdateAnims(ped);
    return true;
}

// 0x679510


CTask* CTaskComplexFallToDeath::ControlSubTask(CPed* ped) {
    // return plugin::CallMethodAndReturn<CTask*, 0x679510, CTaskComplexFallToDeath*, CPed*>(this, ped);

    if (ped->physicalFlags.bSubmergedInWater) {
        if (!b0x2) {
            b0x2 = true;
            ped->bIsDrowning = true;
            return new CTaskComplexDie(WEAPON_DROWNING, ANIM_GROUP_DEFAULT, ANIM_ID_DROWN, 4.0f, 1.0f, false, false, eDirection::FORWARD, false);
        }
    }

    if (!b0x1 || b0x2 || m_pSubTask->GetTaskType() == TASK_SIMPLE_DEAD) {
        return m_pSubTask;
    }

    const auto& z = ped->GetMoveSpeed().z;
    bool v11 = z < 0.0f ? z > -0.01f : z < 0.01f;
    if (!v11) {
        return m_pSubTask;
    }

    b0x2 = true;
    b0x4 = true;

    UpdateAnims(ped);
    return new CTaskSimpleLand(ANIM_ID_KO_SKID_FRONT);
}

// 0x679120


CTask* CTaskComplexFallToDeath::CreateFirstSubTask(CPed* ped) {
    ped->bIsStanding = false;
    ped->ApplyMoveForce({
        ms_LateralForceMagnitude * m_Posn.x,
        ms_LateralForceMagnitude * m_Posn.y,
        b0x8 ? ms_OverRailingVerticalForce : ms_NoRailingVerticalForce
    });

    m_nAnimId = [&] {
        switch (m_nFallToDeathDir) {
        case eDirection::FORWARD:  return ANIM_ID_KO_SKID_BACK;
        case eDirection::LEFT:     return ANIM_ID_KO_SPIN_L;
        case eDirection::BACKWARD: return ANIM_ID_KO_SKID_FRONT;
        case eDirection::RIGHT:    return ANIM_ID_KO_SPIN_R;
        default:
            NOTSA_UNREACHABLE("");
            return ANIM_ID_UNDEFINED;
        }
    }();

    if (m_nAnimId >= ANIM_ID_WALK && m_nAnimId < ANIM_ID_ROADCROSS) {
        CAnimManager::BlendAnimation(ped->GetRpClump(), nullptr, m_nAnimId, 1000.0f);
    }

    return new CTaskSimpleInAir(false, true, false);
}

// 0x679270

// 0x0
CTask* CTaskComplexFallToDeath::CreateNextSubTask(CPed* ped) {
    // return plugin::CallMethodAndReturn<CTask*, 0x679270, CTaskComplexFallToDeath*, CPed*>(this, ped);

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_IN_AIR: {
        // Como el original: |z| (fchs si z < 0); caida rapida (|z| >= 0.1 o NaN) => b0x4 y animacion KO_SKID_FRONT
        const float z = ped->GetMoveSpeed().z;
        const float absZ = z < 0.0f ? -z : z;
        b0x4 = !(absZ < 0.1f);
        return new CTaskSimpleLand(b0x4 ? ANIM_ID_KO_SKID_FRONT : ANIM_ID_UNDEFINED);
    }
    case TASK_SIMPLE_LAND: {
        if (!b0x1) {
            // Como el original: si aun se mueve en vertical (|z| >= 0.01 o NaN) vuelve al aire
            const float z = ped->GetMoveSpeed().z;
            const float absZ = z < 0.0f ? -z : z;
            if (!(absZ < 0.01f) && (!ped->m_pContactEntity || ped->m_pContactEntity->GetIsTypeBuilding())) {
                b0x1 = true;
                return new CTaskSimpleInAir(false, true, false);
            }
        }

        if (!b0x4) {
            if (m_nAnimId != ANIM_ID_UNDEFINED) {
                if (auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), m_nAnimId)) {
                    assoc->m_BlendDelta = -1000.0f;
                }
            }
            m_nAnimId1 = ANIM_ID_KO_SKID_FRONT;
            CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_KO_SKID_FRONT, 8.0f);
        }

        ped->GetIntelligence()->ClearTasks(false, true);
        return new CTaskSimpleDead(CTimer::GetTimeInMS(), false);
    }
    default:
        return nullptr;
    }
}

// 0x6796C0
bool CTaskComplexFallToDeath::CalcFall(CPed* ped, int32& outFallDir, bool& outFallToDeathOverRailing) {
    if (ped->bInVehicle || ped->physicalFlags.bSubmergedInWater) {
        return false;
    }

    // Como el original: rand() & 3 dos veces por intercambio, 8 intercambios
    eDirection rndFallDirs[4]{eDirection::FORWARD, eDirection::LEFT, eDirection::BACKWARD, eDirection::RIGHT};
    for (auto i = 8; i-- > 0;) {
        const auto a = CGeneral::GetRandomNumber() & 3;
        const auto b = CGeneral::GetRandomNumber() & 3;
        std::swap(rndFallDirs[a], rndFallDirs[b]);
    }

    const auto IsLOSClear = [](const CVector& origin, const CVector& target) {
        return CWorld::GetIsLineOfSightClear(origin, target, true, true, false, true, false, false, false);
    };

    // Como el original: los vectores salen directamente de la matriz y cada suma sigue el orden del EXE
    const CMatrix& mat = *ped->m_matrix;
    for (auto i = 0; i < 2; i++) {
        outFallDir = (int32)rndFallDirs[i];
        CVector dir{};
        switch (rndFallDirs[i]) {
        case eDirection::FORWARD:  dir = mat.GetForward(); break;
        case eDirection::LEFT:     dir = mat.GetRight() * -1.0f; break;
        case eDirection::BACKWARD: dir = mat.GetForward() * -1.0f; break;
        case eDirection::RIGHT:    dir = mat.GetRight(); break;
        default:                   NOTSA_UNREACHABLE();
        }
        const CVector offset = dir * ms_FallCheckOffset;
        const CVector offsetFar = dir * 0.7f;

        // 1. Hay vacio bajo el punto de caida
        CVector pedPos = ped->GetPosition();
        const CVector fallPos{ pedPos.x + offset.x, pedPos.y + offset.y, (ms_FallCheckHeight + pedPos.z) + offset.z };
        if (!IsLOSClear(fallPos, CVector{ fallPos.x, fallPos.y, fallPos.z - ms_FallCheckDepth })) {
            continue;
        }

        // 2. El punto mas alejado se ve desde el peaton
        pedPos = ped->GetPosition();
        const CVector farPos{ fallPos.x + offsetFar.x, fallPos.y + offsetFar.y, fallPos.z + offsetFar.z };
        if (!IsLOSClear(farPos, CVector{ pedPos.x, pedPos.y, ms_FallCheckHeight + pedPos.z })) {
            continue;
        }

        // 3. Barandilla: algo entre la cintura del peaton y el punto de caida
        pedPos = ped->GetPosition();
        const float lowZ = pedPos.z - ms_RailingCheckDrop;
        outFallToDeathOverRailing = !IsLOSClear(
            CVector{ pedPos.x, pedPos.y, lowZ },
            CVector{ (pedPos.x + offset.x) + offsetFar.x, (pedPos.y + offset.y) + offsetFar.y, (lowZ + offset.z) + offsetFar.z }
        );
        return true;
    }
    return false;
}
