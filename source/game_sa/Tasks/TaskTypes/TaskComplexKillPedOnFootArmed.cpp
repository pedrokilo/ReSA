/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillPedOnFootArmed
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePause.h"
#include "TaskSimpleThrowControl.h"
#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include <extensions/utility.hpp>

void CTaskComplexKillPedOnFootArmed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFootArmed, 0x86d918, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621190);
    RH_ScopedInstall(Destructor, 0x621250);

    RH_ScopedInstall(LineOfSightClearForAttack, 0x621500);
    RH_ScopedInstall(IsPedInLeaderFiringLine, 0x621300);
    RH_ScopedInstall(CreateSubTask, 0x626FC0);

    RH_ScopedVMTInstall(Clone, 0x6234C0);
    RH_ScopedVMTInstall(GetTaskType, 0x621240);
    RH_ScopedVMTInstall(MakeAbortable, 0x6212B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62C190);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62BF00);
    RH_ScopedVMTInstall(ControlSubTask, 0x62CCE0);
}

// 0x621190
CTaskComplexKillPedOnFootArmed::CTaskComplexKillPedOnFootArmed(
    CPed*  target,
    uint32 duckingConditions,
    uint32 duckTime,
    uint32 duckChancePerc,
    int8   competence
) :
    m_target{ target },
    m_duckingConditions{ duckingConditions },
    m_lengthOfDuck{ duckTime },
    m_duckChancePerc{ duckChancePerc },
    m_competence{ competence }
{
    CEntity::SafeRegisterRef(m_target);
}

// notsa
CTaskComplexKillPedOnFootArmed::CTaskComplexKillPedOnFootArmed(const CTaskComplexKillPedOnFootArmed& o) :
    CTaskComplexKillPedOnFootArmed{
        o.m_target,
        o.m_duckingConditions,
        o.m_lengthOfDuck,
        o.m_duckChancePerc,
        o.m_competence
    }
{
    m_aimImmediate = o.m_aimImmediate;
}

// 0x621250
CTaskComplexKillPedOnFootArmed::~CTaskComplexKillPedOnFootArmed() {
    CEntity::SafeCleanUpRef(m_target);
}

// 0x621500
bool CTaskComplexKillPedOnFootArmed::LineOfSightClearForAttack(CPed* ped) { // ped is the task owner ped
    /*
    * TODO:
    * here's some code to calculate the values to use (based on some variables)
    * nothing special, but it's messy, and it's late, so I won't bother
    */
    const auto reqDistDeltaSq = sq(2.f);
    const auto reqTimeDelta   = 5000;

    //> If the LOS was recently clear, let's consider it still is
    if (CTimer::GetTimeInMS() - m_losClearTime < reqTimeDelta) {
        return true;
    }

    //> Perhaps check if the entities are still kinda around the same position as the last time the LOS was blocked...
    if (CTimer::GetTimeInMS() - m_losBlockedTime < reqTimeDelta) {
        if (reqDistDeltaSq >= (m_target->GetPosition() - m_losBlockedTargetPos).SquaredMagnitude()) {
            if (reqDistDeltaSq >= (ped->GetPosition() - m_losBlockedOurPos).SquaredMagnitude()) {
                return false;
            }
        }
    }

    //> 0x6216DD
    // Temporarily disable the target ped vehicle's collision (To ignore it)
    // Perhaps, `CWorld::pIgnoreEntity` could be used?
    const auto targetVeh = m_target->GetVehicleIfInOne();
    const notsa::ScopeGuard restore{[targetVeh, had = targetVeh && targetVeh->GetUsesCollision()] {
        if (targetVeh) {
            targetVeh->SetUsesCollision(had);
        }
    }};

    if (targetVeh) {
        targetVeh->SetUsesCollision(false);
    }

    const auto GetPedHeadPos = [](CPed* headOf) {
        CVector inout{ 0.1f, 0.f, 0.f };
        headOf->GetTransformedBonePosition(inout, BONE_HEAD);
        return inout;
    };

    if (CWorld::GetIsLineOfSightClear(
        ped->bIsDucking
            ? GetPedHeadPos(ped)
            : ped->GetPosition() + CVector{0.f, 0.f, 0.25f},
        GetPedHeadPos(m_target), // Always use head position
        true,
        true,
        false,
        true,
        false,
        true,
        false
    )) {
        m_losClearTime   = CTimer::GetTimeInMS();
        m_losBlockedTime = 0;
        return true;
    } else {
        m_losBlockedTime      = CTimer::GetTimeInMS();
        m_losClearTime        = 0;
        m_losBlockedOurPos    = ped->GetPosition();
        m_losBlockedTargetPos = m_target->GetPosition();
        return false;
    }
}

// 0x621300
bool CTaskComplexKillPedOnFootArmed::IsPedInLeaderFiringLine(CPed* ped) {
    const auto pedGrp = ped->GetGroup();
    if (!pedGrp) {
        return false;
    }

    const auto grpLeaderPlyr = pedGrp->GetMembership().GetLeader();
    if (!grpLeaderPlyr || !grpLeaderPlyr->IsPlayer()) {
        return false;
    }

    if (!grpLeaderPlyr->m_pTargetedObject || grpLeaderPlyr->GetActiveWeapon().IsTypeMelee()) {
        return false;
    }

    const auto &leaderPos2D      = grpLeaderPlyr->GetPosition2D();
    const auto &leaderPos        = grpLeaderPlyr->GetPosition();
    const auto leaderToPed       = ped->GetPosition() - leaderPos, // 0x6213BD
               leaderToTargetDir = (grpLeaderPlyr->m_pTargetedObject->GetPosition() - leaderPos).Normalized(); // 0x621394

    /* clang-format off
     * --[projPointOnLeaderToTargetRay2D]-->[leaderPos]---[leaderToTargetDir]-->[Target]
     *                                          /
     *                                         /
     *                                        /
     *                                       /
     *                                 [leaderToPed]
     *                                     /
     *                                    /
     *                                  \|/
     *                                [Ped]
     * clang-format on */

    //> 0x6213AE
    const auto projPointOnLeaderToTargetRay2D = leaderPos + CVector2D{ leaderToPed }.ProjectOnToNormal(leaderToTargetDir);
    if ((projPointOnLeaderToTargetRay2D - ped->GetPosition2D()).SquaredMagnitude() >= sq(2.f)) {
        return false;
    }

    //> 0x0621482
    if (leaderToTargetDir.Dot(leaderToPed) <= 0.f) { // Ped is "behind" leader (Like on the ASCII art above)
        return false;
    }

    //> 0x6214CD
    if (leaderToPed.SquaredMagnitude() >= sq(10.f)) {
        return false;
    }

    return true;
}

// 0x626FC0
CTask* CTaskComplexKillPedOnFootArmed::CreateSubTask(eTaskType taskType, CPed* ped) {
    const auto& weapon = *CWeaponInfo::GetWeaponInfo(ped);
    switch (taskType) {
    case TASK_SIMPLE_PAUSE: {
        CTaskSimpleStandStill stand{0, false, false, 8.f};
        stand.ProcessPed(ped);
        return new CTaskSimplePause{100};
    }
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{20000, false, false, 8.f};
    case TASK_SIMPLE_DUCK:
        return new CTaskSimpleDuck{DUCK_STANDALONE, uint16(m_lengthOfDuck), -1};
    case TASK_SIMPLE_GUN_CTRL: {
        CTask* result;
        if (weapon.flags.bThrow) {
            result = new CTaskSimpleThrowControl{m_target, nullptr};
        } else {
            const auto command = LineOfSightClearForAttack(ped) ? eGunCommand::FIREBURST : eGunCommand::NONE;
            auto* gun = new CTaskSimpleGunControl{m_target, CVector{}, CVector{}, command, 5, -1};
            gun->m_aimImmidiately = m_aimImmediate;
            m_aimImmediate = false;
            const auto now = CTimer::GetTimeInMS();
            m_shootTimer = now + uint32(CGeneral::GetRandomIntInRange(4000, 8000));
            result = gun;
            if (ped->GetGroup()) {
                ped->Say(CTX_GLOBAL_SURROUNDED);
            }
        }
        m_lastAttackTime = CTimer::GetTimeInMS();
        m_losBlockedTime = 0;
        return result;
    }
    case TASK_COMPLEX_SEEK_ENTITY: {
        const float elapsed = float(CTimer::GetTimeInMS() - uint32(m_needToMoveInCloserTime));
        float radius = 6.f;
        CTask* result;
        if (!m_needToMoveInCloserTime || elapsed < 3000.f) {
            result = new CTaskComplexSeekEntityStandard{m_target, 50000, 1000, radius, 2.f, 2.f, true, true};
        } else if (elapsed > 8000.f) {
            CVector direction{m_target->GetPosition().x - ped->GetPosition().x, m_target->GetPosition().y - ped->GetPosition().y, 0.f};
            direction.Normalise();
            const float sidewaysX = -direction.y * 1.5f;
            const float sidewaysY = direction.x * 1.5f;
            const auto& targetPos = m_target->GetPosition();
            const CVector left{sidewaysX + targetPos.x, sidewaysY + targetPos.y, targetPos.z};
            const CVector right{targetPos.x - sidewaysX, targetPos.y - sidewaysY, targetPos.z};
            if (CWorld::GetIsLineOfSightClear(targetPos, left, true, true, false, false, true, false, false)) {
                result = new CTaskComplexGoToPointAndStandStill{PEDMOVE_RUN, left, 0.5f, 2.f, false, false};
            } else if (CWorld::GetIsLineOfSightClear(targetPos, right, true, true, false, false, true, false, false)) {
                result = new CTaskComplexGoToPointAndStandStill{PEDMOVE_RUN, right, 0.5f, 2.f, false, false};
            } else {
                result = new CTaskComplexSeekEntityStandard{m_target, 50000, 1000, 1.f, 2.f, 2.f, true, true};
            }
        } else {
            radius = 6.f - (elapsed - 3000.f) * 0.001f;
            result = new CTaskComplexSeekEntityStandard{m_target, 50000, 1000, radius, 2.f, 2.f, true, true};
        }
        if (m_target && m_target->IsPlayer()) {
            if (ped->m_nPedType == PED_TYPE_COP) {
                const auto context = FindPlayerWanted()->m_NumCopsInPursuit <= 1 ? CTX_GLOBAL_SOLO : CTX_GLOBAL_CHASE_FOOT;
                if (ped->Say(context) >= 0) {
                    m_target->Say(CTX_GLOBAL_CHASED, 3500);
                }
            } else if (ped->GetGroup()) {
                ped->Say(CTX_GLOBAL_CHASE_FOOT);
            }
        }
        return result;
    }
    default:
        return nullptr;
    }
}

// 0x6212B0
bool CTaskComplexKillPedOnFootArmed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT: {
        if (const auto aimedAtEvent = notsa::dyn_cast_if_present<const CEventGunAimedAt>(event)) {
            if (aimedAtEvent->m_AimedBy == m_target) {
                return false;
            }
        }
        break;
    }
    case ABORT_PRIORITY_IMMEDIATE:
        break;
    case ABORT_PRIORITY_LEISURE:
    default:
        return false;
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x62C190
CTask* CTaskComplexKillPedOnFootArmed::CreateNextSubTask(CPed* ped) {
    if (!m_target) {
        return nullptr;
    }
    const bool targetArmed = !m_target->GetActiveWeapon().IsTypeMelee();
    const auto* weapon = CWeaponInfo::GetWeaponInfo(ped);
    const float range = weapon->m_fTargetRange;
    const auto delta = m_target->GetPosition() - ped->GetPosition();
    const float distance = std::sqrt((delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z);
    auto reset = [&](CTask* result) { m_needToMoveInCloserTime = 0; return result; };
    auto attackOrPause = [&] { return CreateSubTask(LineOfSightClearForAttack(ped) ? TASK_SIMPLE_GUN_CTRL : TASK_SIMPLE_PAUSE, ped); };
    const auto type = m_pSubTask->GetTaskType();
    if (type == TASK_SIMPLE_DUCK) {
        auto* result = attackOrPause();
        ped->bIsDucking = false;
        return reset(result);
    }
    if (type == TASK_SIMPLE_PAUSE || type == TASK_SIMPLE_STAND_STILL) {
        if (!ped->bStayInSamePlace && !ped->bKindaStayInSamePlace) {
            ped->Say(eGlobalSpeechContext(144));
            auto* result = CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
            if (!m_needToMoveInCloserTime) m_needToMoveInCloserTime = CTimer::GetTimeInMS();
            return result;
        }
        const auto* currentWeapon = CWeaponInfo::GetWeaponInfo(ped);
        const auto difference = m_target->GetPosition() - ped->GetPosition();
        const float squared = (difference.x * difference.x + difference.y * difference.y) + difference.z * difference.z;
        return reset(CreateSubTask(squared < currentWeapon->m_fTargetRange * currentWeapon->m_fTargetRange && LineOfSightClearForAttack(ped) ? TASK_SIMPLE_GUN_CTRL : TASK_SIMPLE_PAUSE, ped));
    }
    if (type == eTaskType(900)) return reset(attackOrPause());
    if (type == eTaskType(903) || type == TASK_COMPLEX_SEEK_ENTITY) {
        if (LineOfSightClearForAttack(ped)) return reset(CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped));
        auto* result = CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        if (!m_needToMoveInCloserTime) m_needToMoveInCloserTime = CTimer::GetTimeInMS();
        return result;
    }
    if (type != eTaskType(901) && type != TASK_SIMPLE_GUN_CTRL && type != TASK_SIMPLE_THROW_CTRL) {
        return reset(nullptr);
    }
    // The EXE consumes the full 32-bit CRT result and tests its low bits.
    auto random = [] { return plugin::CallAndReturn<int32, 0x821B1E>(); };
    auto seek = [&](float radius) -> CTask* {
        return new CTaskComplexSeekEntityStandard{m_target, 50000, 1000, radius,
            StaticRef<float>(0x859E30), StaticRef<float>(0x859E34), true, true};
    };
    auto finish = [&](CTask* result) -> CTask* {
        if (!result) {
            result = CreateSubTask(TASK_SIMPLE_PAUSE, ped);
            m_lastAttackTime = CTimer::GetTimeInMS();
        }
        if ((random() & 1) && m_competence > 0 && !ped->bNotAllowedToDuck &&
            (!ped->m_pCoverPoint || (ped->m_pCoverPoint->GetUsage() != CCoverPoint::eUsage::WALLTOLEFT && ped->m_pCoverPoint->GetUsage() != CCoverPoint::eUsage::WALLTORIGHT))) {
            ped->GetIntelligence()->SetTaskDuckSecondary(6000);
        }
        return reset(result);
    };
    if (ped->bStayInSamePlace) return finish(nullptr);
    if (distance < 3.f) return finish(CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped));
    const float nearDistance = std::min(range * .8f, 23.f);
    if (distance > nearDistance) {
        if (ped->bKindaStayInSamePlace) return finish(nullptr);
        return finish(seek(std::min(range * .6f, 20.f)));
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GUN_CTRL &&
        static_cast<CTaskSimpleGunControl*>(m_pSubTask)->m_isLOSBlocked &&
        !ped->bStayInSamePlace && !ped->bKindaStayInSamePlace) {
        ped->Say(eGlobalSpeechContext(144));
        if (auto* result = seek(2.f)) return finish(result);
    } else if (targetArmed && ped->m_pCoverPoint &&
        CCover::DoesCoverPointStillProvideCover(ped->m_pCoverPoint, m_target->GetPosition())) {
        CVector coordinates;
        if (CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, m_target->GetPosition(), coordinates)) {
            const auto difference = ped->GetPosition() - coordinates;
            if (std::sqrt(difference.x * difference.x + difference.y * difference.y) < .75f) {
                if (ped->m_pCoverPoint->GetUsage() == CCoverPoint::eUsage::LOWCOVER) {
                    if (distance < 12.f && !ped->bNotAllowedToDuck && !(random() & 3) &&
                        !ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK)) {
                        ped->Say(eGlobalSpeechContext(85));
                        if (auto* result = new CTaskSimpleDuck{DUCK_STANDALONE, 2500, -1}) return finish(result);
                        ped->ReleaseCoverPoint();
                    } else {
                        ped->ReleaseCoverPoint();
                    }
                } else if (!(random() & 1) && !weapon->flags.bThrow) {
                    auto* result = new CTaskSimpleGunControl{m_target, CVector{}, CVector{}, eGunCommand::FIREBURST, 5, -1};
                    result->m_aimImmidiately = m_aimImmediate;
                    m_aimImmediate = false;
                    m_shootTimer = CTimer::GetTimeInMS() + 5000;
                    m_lastAttackTime = CTimer::GetTimeInMS();
                    m_lastStrafeTime = CTimer::GetTimeInMS() + 2500;
                    m_strafeDir = eStrafeDir::RIGHT;
                    m_bStrafeBack = true;
                    if (ped->m_pCoverPoint->GetUsage() == CCoverPoint::eUsage::WALLTOLEFT) m_strafeDir = eStrafeDir::LEFT;
                    ped->GetIntelligence()->ClearTaskDuckSecondary();
                    if (result) return finish(result);
                    ped->ReleaseCoverPoint();
                } else {
                    ped->ReleaseCoverPoint();
                }
            } else {
                ped->ReleaseCoverPoint();
            }
        }
    }
    CTask* result{};
    if (m_competence > 0 && distance > 6.f && targetArmed && (random() & 1)) {
        ped->ReleaseCoverPoint();
        ped->m_pCoverPoint = CCover::FindAndReserveCoverPoint(ped, m_target->GetPosition(), m_competence == 2);
        if (ped->m_pCoverPoint) {
            CVector coordinates; // Original leaves this uninitialized if its helper fails.
            CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, m_target->GetPosition(), coordinates);
            coordinates.z += 1.f;
            const auto difference = coordinates - m_target->GetPosition();
            if (range * .75f > std::sqrt(difference.x * difference.x + difference.y * difference.y) &&
                CWorld::GetIsLineOfSightClear(coordinates, ped->GetPosition(), true, true, false, false, false, false, false)) {
                if (CPedGroups::GetPedsGroup(ped)) ped->Say(eGlobalSpeechContext(65));
                ped->GetIntelligence()->SetTaskDuckSecondary(6000);
                if (auto* gotoPoint = new CTaskSimpleGoToPoint{PEDMOVE_RUN, coordinates, .5f, true, false}) return finish(gotoPoint);
            } else {
                ped->ReleaseCoverPoint();
            }
        }
    }
    if (distance > 10.f) {
        const auto choice = random() & 3;
        if (choice == 0 && !ped->bKindaStayInSamePlace) {
            const float radius = distance - 4.f;
            ped->Say(eGlobalSpeechContext(144));
            result = seek(radius);
        } else if (choice == 1 && !ped->bKindaStayInSamePlace) {
            m_strafeDir = eStrafeDir::FORWARD;
            m_lastStrafeTime = CTimer::GetTimeInMS() + 2000;
            m_bStrafeBack = false;
        }
    }
    if (distance > 5.f && !(random() & 3)) {
        m_strafeDir = (random() & 1) ? eStrafeDir::RIGHT : eStrafeDir::LEFT;
        const auto position = ped->GetPosition();
        const auto side = ped->GetRight() * 2.5f;
        const auto endpoint = m_strafeDir == eStrafeDir::RIGHT ? position + side : position - side;
        if (!CWorld::GetIsLineOfSightClear(position, endpoint, true, true, false, true, false, false, false)) m_strafeDir = m_strafeDir == eStrafeDir::RIGHT ? eStrafeDir::LEFT : eStrafeDir::RIGHT;
        m_lastStrafeTime = CTimer::GetTimeInMS() + 2000;
        m_bStrafeBack = false;
    }
    if (!result) {
        if (LineOfSightClearForAttack(ped)) {
            result = CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        } else if (!(random() & 3)) {
            m_strafeDir = random() < 0x3fff ? eStrafeDir::RIGHT : eStrafeDir::LEFT;
            m_lastStrafeTime = CTimer::GetTimeInMS() + 2000;
            m_bStrafeBack = false;
        }
    }
    return finish(result);
}

// 0x62BF00
CTask* CTaskComplexKillPedOnFootArmed::CreateFirstSubTask(CPed* ped) {
    if (!m_target) {
        return nullptr;
    }

    const auto &ourPos    = ped->GetPosition(),
               &targetPos = m_target->GetPosition();

    const auto targetToOurPedDistSq = (ourPos - targetPos).SquaredMagnitude();

    if (   !ped->bStayInSamePlace
        && CGeneral::RandomBool(50)
        && m_competence > 0
        && (targetToOurPedDistSq >= sq(30.f) || targetToOurPedDistSq >= sq(6.f) && !m_target->GetActiveWeapon().IsTypeMelee()) 
    ) {
        ped->ReleaseCoverPoint();
        if (ped->m_pCoverPoint = CCover::FindAndReserveCoverPoint(ped, targetPos, false)) {
            CVector coverPos{};
            if (!notsa::IsFixBugs() || CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, targetPos, coverPos)) {
                if (CWorld::GetIsLineOfSightClear(coverPos, ourPos, true, true, false, false, false, false, false)) {
                    ped->GetIntelligence()->SetTaskDuckSecondary(6000);
                    if (const auto task = new CTaskSimpleGoToPoint{
                        PEDMOVE_RUN,
                        coverPos,
                        0.5f,
                        true
                    }) { // I can't believe my eyes.... error handling?
                        return task;
                    }
                } else {
                    ped->ReleaseCoverPoint();
                }
            } else {
                NOTSA_LOG_WARN("Can't find cover point's coordinates!"); // Originally the game has left `coverPos` uninitialized in this case
            }
        }
    }

    return CreateSubTask([&, this] {
        const auto& ourActiveWep = ped->GetActiveWeapon().GetWeaponInfo(ped);

        if (targetToOurPedDistSq <= sq(ourActiveWep.m_fTargetRange / 4.f) && LineOfSightClearForAttack(ped)) {
            return TASK_SIMPLE_GUN_CTRL;
        }

        if (!ped->bStayInSamePlace && !ped->bKindaStayInSamePlace) {
            return TASK_COMPLEX_SEEK_ENTITY;
        }

        if (LineOfSightClearForAttack(ped) || ourActiveWep.flags.bThrow) {
            return TASK_SIMPLE_PAUSE;
        }

        return TASK_SIMPLE_GUN_CTRL;
    }(), ped);
}

// 0x62CCE0
CTask* CTaskComplexKillPedOnFootArmed::ControlSubTask(CPed* ped) {
    if (m_newTarget) {
        return CreateFirstSubTask(ped);
    }

    if (!m_target || m_target->m_fHealth <= 0.f) {
        return nullptr;
    }
    const auto ogSubTask = m_pSubTask;

    const auto &ourPos    = ped->GetPosition(),
               &targetPos = m_target->GetPosition();

    const auto targetToOurPedDistSq = (ourPos - targetPos).SquaredMagnitude();

    //> 0x62CD55
    if (   (m_duckingConditions & 4) == 0
        || CTimer::GetTimeInMS() <= m_lastDuckTime + m_lengthOfDuck + 2000
        || ped->bIsDucking
    ) {
        if (   m_duckingConditions & 1
            && m_bShotFiredByPlayer
            && CGeneral::RandomBool((float)m_duckChancePerc)
        ) {
            if (const auto quack = ped->GetIntelligence()->GetTaskDuck()) {
                quack->SetDuckTimer((uint16)m_lengthOfDuck);
            } else {
                ped->GetIntelligence()->SetTaskDuckSecondary((uint16)m_lengthOfDuck);
#ifdef FIX_BUGS
                m_lastDuckTime = CTimer::GetTimeInMS(); // Not actually sure if this is necessary tbh
#endif
            }
        }
    } else {
        if (CGeneral::RandomBool((float)m_duckChancePerc)) {
            if (targetToOurPedDistSq <= sq(20.f)) {
                ped->GetIntelligence()->SetTaskDuckSecondary((uint16)m_lengthOfDuck);
            }
        }
        m_lastDuckTime = CTimer::GetTimeInMS();
    }

    //> 0x62CE56
    const auto quack = ped->GetIntelligence()->GetTaskDuck();
    if (quack) {
        quack->m_bIsInControl = true;
    }

    //> 0x62CE6E
    if (m_pSubTask != ogSubTask) { // ?????
        return ogSubTask;
    }

    const auto& ourwi = ped->GetActiveWeapon().GetWeaponInfo(ped);
    switch (m_pSubTask->GetTaskType()) { //> 0x62CE7E
    case TASK_COMPLEX_SEEK_ENTITY: {
        if ([&, this]{
            if (IsPedInLeaderFiringLine(ped)) {
                return true;
            }
            if (m_target->physicalFlags.bSubmergedInWater && LineOfSightClearForAttack(ped)) {
                return true;
            }
            if (ped->bStayInSamePlace) {
                return LineOfSightClearForAttack(ped);
            } else if (sq(ourwi.m_fTargetRange / 2.f) >= targetToOurPedDistSq) {
                if (CTimer::GetTimeInMS() - m_lastAttackTime >= 2000 || m_target->GetMoveSpeed().Dot(ped->GetForward()) < 0.f) {
                    return LineOfSightClearForAttack(ped);
                }
            }
            return false;
        }()) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        } else {
            if (ped->GetGroup()) {
                ped->Say(CTX_GLOBAL_COVER_ME);
            }
        }
        break;
    }
    case TASK_SIMPLE_GUN_CTRL: { //> 0x62CE87
        if (   targetToOurPedDistSq >= sq(ourwi.m_fTargetRange)
            || targetToOurPedDistSq >= sq(ourwi.m_fTargetRange / 2.f) && !ped->bStayInSamePlace && CTimer::GetTimeInMS() - m_lastAttackTime >= 2000
            || targetToOurPedDistSq >= sq(4.f) && !ped->bStayInSamePlace && CTimer::GetTimeInMS() >= m_shootTimer
            || !LineOfSightClearForAttack(ped)
        ) {
            const auto gctrl = notsa::cast<CTaskSimpleGunControl>(m_pSubTask);
            if (gctrl->m_firingTask != eGunCommand::END_LEISURE) {
                gctrl->m_nextAtkTimeMs = 0;
                gctrl->m_firingTask    = eGunCommand::END_LEISURE;
            }
        }

        const auto ugun = ped->GetIntelligence()->GetTaskUseGun();
        if (!ugun) {
            break;
        }

        if (ped->GetGroup()) {
            ped->Say(CTX_GLOBAL_SURROUNDED);
        }

        //> 0x62CF88
        const auto actionDir = [&, this]() -> CVector2D {
            if (m_target->physicalFlags.bSubmergedInWater) {
                return { 0.f, 0.f };
            }
            if (CTimer::GetTimeInMS() >= m_lastStrafeTime || !ped->bStayInSamePlace) {
                if (targetToOurPedDistSq <= sq(4.f) && ped->bStayInSamePlace) {
                    return { 0.f, 1.f }; // Forwards
                }
                if (m_bStrafeBack) {
                    m_bStrafeBack    = false;
                    m_lastStrafeTime = CTimer::GetTimeInMS() + 2500;
                    m_strafeDir      = [&, this] { // Strafe back to where we came from the last time
                        switch (m_strafeDir)
                        {
                        case eStrafeDir::LEFT:    return eStrafeDir::RIGHT;
                        case eStrafeDir::RIGHT:   return eStrafeDir::LEFT;
                        case eStrafeDir::FORWARD: return eStrafeDir::BACK;
                        case eStrafeDir::BACK:    return eStrafeDir::FORWARD;
                        default:                  NOTSA_UNREACHABLE();
                        }
                    }();
                }
                return { 0.f, 0.f };
            } else {
                switch (m_strafeDir)
                {
                case eStrafeDir::LEFT:    return { -1.f,  0.f };
                case eStrafeDir::RIGHT:   return {  1.f,  0.f };
                case eStrafeDir::FORWARD: return {  0.f, -1.f };
                case eStrafeDir::BACK:    return {  0.f,  1.f };
                default:                  NOTSA_UNREACHABLE();
                }
            }
        }();

        //> 0x62D080
        if (quack) {
            quack->ControlDuckMove([&, this] {
                if (notsa::contains(std::to_array({ eStrafeDir::LEFT, eStrafeDir::RIGHT }), m_strafeDir)) {
                    if (m_lastRollTime + 3000 >= CTimer::GetTimeInMS()) {
                        return CVector2D{ 0.f, 0.f };
                    } else {
                        m_lastRollTime = CTimer::GetTimeInMS();
                    }
                }
                return actionDir;
            }());
        } else {
            ugun->ControlGunMove(actionDir);
        }

        break;
    }
    }

    return m_pSubTask;
}
