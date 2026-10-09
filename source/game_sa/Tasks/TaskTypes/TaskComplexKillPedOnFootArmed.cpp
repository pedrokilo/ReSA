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
 * Validación registrada: 11/11 entradas.
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
    // Como el original: 0x62151B-0x621527 llama CWeaponInfo::GetWeaponInfo(tipo, habilidad) y descarta el resultado (sin efectos)

    //> 0x62152C Umbrales dinamicos (el EXE no usa constantes fijas)
    uint32 reqTimeDelta   = 10000; // ebp = 0x2710
    float  reqDistDeltaSq = 9.f;   // [esp+2Ch] = 0x41100000
    if (   (int32)ped->GetAreaCode() != 0 // byte [ped+2Fh] (esta en un interior)
        || (m_target && m_target->IsPlayer() && ((int32)CGame::currArea != 0 /* 0x621558 [0xB72914] */ || ped->IsCreatedByMission() /* 0x621561 [ped+484h] == 2 */))
    ) {
        reqTimeDelta = 5000; // 0x621569 mov ebp,1388h
        if (StaticRef<int32>(0xB7298C) < 2) { // CLoadMonitor::m_eProcLevel (HIGH = 2), 0x62156F cmp/jge
            reqDistDeltaSq = 4.f;             // 0x40800000
        }
    }

    // Como el original: (dz*dz + dy*dy) + dx*dx con diferencia (a - b) (0x6215BB..0x6215DA)
    const auto DistSqZYX = [](const CVector& a, const CVector& b) {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        const float dz = a.z - b.z;
        return (dz * dz + dy * dy) + dx * dx;
    };

    const auto now = CTimer::GetTimeInMS();

    //> If the LOS was recently clear, let's consider it still is
    if (now - m_losClearTime < reqTimeDelta) { // 0x62158A cmp ecx,ebp / jae
        return true;
    }

    //> Perhaps check if the entities are still kinda around the same position as the last time the LOS was blocked...
    if (now - m_losBlockedTime <= reqTimeDelta) { // 0x62159D cmp eax,ebp / ja => solo se vuelve a comprobar si paso MAS que el umbral
        const bool targetMoved = m_target && DistSqZYX(m_target->GetPosition(), m_losBlockedTargetPos) > reqDistDeltaSq; // 0x6215E6 test ah,41h / je
        if (!targetMoved && !(DistSqZYX(ped->GetPosition(), m_losBlockedOurPos) > reqDistDeltaSq)) { // 0x62162A
            return false;
        }
    }

    //> 0x62163A
    CVector startPos{ 0.1f, 0.f, 0.f };
    if (ped->bIsDucking) { // test [ped+46Ch],4000000h: agachado => posicion + 0.25 en z
        const auto& pedPos = ped->GetPosition();
        startPos = CVector{ pedPos.x, pedPos.y, pedPos.z + 0.25f }; // fadd [858C84h]
    } else { // de pie => hueso de la cabeza (5E01C0, hueso 5)
        ped->GetTransformedBonePosition(startPos, BONE_HEAD);
    }
    CVector endPos{ 0.1f, 0.f, 0.f };
    m_target->GetTransformedBonePosition(endPos, BONE_HEAD); // Always use head position

    // Como el original: 0x6216DD primero se calculan las posiciones y despues se desactiva la colision del vehiculo del objetivo
    CVehicle* targetVeh{};
    bool      hadCollision{};
    if (m_target->bInVehicle && m_target->m_pVehicle) {
        targetVeh    = m_target->m_pVehicle;
        hadCollision = targetVeh->m_bUsesCollision;
        targetVeh->m_bUsesCollision = false;
    }

    const bool losClear = CWorld::GetIsLineOfSightClear(
        startPos,
        endPos,
        true,
        true,
        false,
        true,
        false,
        true,
        false
    );

    if (losClear) {
        m_losClearTime   = CTimer::GetTimeInMS();
        m_losBlockedTime = 0;
    } else {
        m_losClearTime        = 0;
        m_losBlockedTime      = CTimer::GetTimeInMS();
        m_losBlockedTargetPos = m_target->GetPosition();
        m_losBlockedOurPos    = ped->GetPosition();
    }

    if (targetVeh) { // 0x62179B restaura el bit 0 de [vehiculo+1Ch]
        targetVeh->m_bUsesCollision = hadCollision;
    }
    return losClear;
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

    const auto& leaderPos = grpLeaderPlyr->GetPosition();
    const auto  targetPos = grpLeaderPlyr->m_pTargetedObject->GetPosition();
    const auto& pedPos    = ped->GetPosition();

    //> 0x621394: direccion lider->objetivo con z = 0 y normalizada (40FE00 resta, mov [esp+18h],0 y 59C910 Normalise)
    CVector leaderToTargetDir = targetPos - leaderPos;
    leaderToTargetDir.z       = 0.f;
    leaderToTargetDir.Normalise();

    //> 0x6213BD
    const CVector leaderToPed = pedPos - leaderPos;

    /* clang-format off
     * --[projPointOnLeaderToTargetRay2D]-->[leaderPos]---[leaderToTargetDir]-->[Target]
     *                                          /
     *                                         /
     *                                [leaderToPed]
     *                                     /
     *                                   [Ped]
     * clang-format on */

    //> 0x6213C6: producto escalar (y + z) + x en el orden del EXE; luego dir * dot (40FEF0) y + posicion del lider (411A00)
    const float proj = (leaderToTargetDir.y * leaderToPed.y + leaderToTargetDir.z * leaderToPed.z) + leaderToTargetDir.x * leaderToPed.x;
    CVector projPointOnLeaderToTargetRay2D = leaderToTargetDir;
    projPointOnLeaderToTargetRay2D.x *= proj;
    projPointOnLeaderToTargetRay2D.y *= proj;
    projPointOnLeaderToTargetRay2D.z *= proj;
    projPointOnLeaderToTargetRay2D.x = leaderPos.x + projPointOnLeaderToTargetRay2D.x;
    projPointOnLeaderToTargetRay2D.y = leaderPos.y + projPointOnLeaderToTargetRay2D.y;
    projPointOnLeaderToTargetRay2D.z = leaderPos.z + projPointOnLeaderToTargetRay2D.z;

    //> 0x62141B: (x, y) de la proyeccion menos la posicion del peaton con z = 0; 406DA0 y fcomp 4.0 con jp => estrictamente menor
    const CVector projToPed{ projPointOnLeaderToTargetRay2D.x - pedPos.x, projPointOnLeaderToTargetRay2D.y - pedPos.y, 0.f };
    if (!(projToPed.SquaredMagnitude() < 4.f)) {
        return false;
    }

    //> 0x621482: producto escalar SIN normalizar (peaton - lider) . (objetivo - lider), orden (z + y) + x; fcomp 0.0 con test 41h
    const CVector leaderToTarget = targetPos - leaderPos;
    const float   dotToTarget    = (leaderToPed.z * leaderToTarget.z + leaderToPed.y * leaderToTarget.y) + leaderToPed.x * leaderToTarget.x;
    if (!(dotToTarget > 0.f)) { // Ped is "behind" leader (Like on the ASCII art above)
        return false;
    }

    //> 0x6214CD: 406DA0 y fcomp 100.0 con jp => estrictamente menor
    if (!((pedPos - leaderPos).SquaredMagnitude() < 100.f)) {
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
    m_newTarget = false; // Como el original: 0x62BF22 mov byte ptr [edi+34h],0 (antes de comprobar el objetivo)

    // Como el original: 0x62BF26-0x62BF47 CWeaponInfo::GetWeaponInfo(tipo, habilidad) (el resultado se usa mas abajo)
    if (!m_target) {
        return nullptr;
    }

    const auto &ourPos    = ped->GetPosition(),
               &targetPos = m_target->GetPosition();

    // Como el original: 0x62BF78 d = objetivo - peaton, distSq = (dx*dx + dy*dy) + dz*dz, dist = fsqrt(distSq) (24 bits)
    const float dx = targetPos.x - ourPos.x;
    const float dy = targetPos.y - ourPos.y;
    const float dz = targetPos.z - ourPos.z;
    const float targetToOurPedDistSq = (dx * dx + dy * dy) + dz * dz;
    const float targetToOurPedDist   = std::sqrt(targetToOurPedDistSq);
    const bool  targetArmed          = !m_target->GetActiveWeapon().IsTypeMelee(); // 0x62BFB9 (73B1C0)

    if (   !ped->bStayInSamePlace
        && (plugin::CallAndReturn<int32, 0x821B1E>() & 1) // Como el original: 0x62BFD7 rand() & 1 (no CGeneral::RandomBool)
        && m_competence > 0
        && (targetToOurPedDist > 30.f || (targetToOurPedDist > 6.f && targetArmed)) // 0x62BFEF fcomp 30.0 / 6.0 con la distancia (no al cuadrado)
    ) {
        ped->ReleaseCoverPoint();
        if (ped->m_pCoverPoint = CCover::FindAndReserveCoverPoint(ped, targetPos, false)) {
            CVector coverPos{};
            // Como el original: 0x62C06A siempre llama a FindCoordinatesCoverPoint (ignora el resultado)
            (void)CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, targetPos, coverPos); // el EXE deja coverPos sin inicializar si falla
            coverPos.z += 1.f; // 0x62C06F fadd [858624h] (1.0)
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
        }
    }

    return CreateSubTask([&, this] {
        const auto& ourActiveWep = ped->GetActiveWeapon().GetWeaponInfo(ped);

        // Como el original: 0x62C104 distSq <= (rango * rango) * 0.25 (fmul [858C84h]); antes era (rango / 4)^2
        if (targetToOurPedDistSq <= sq(ourActiveWep.m_fTargetRange) * 0.25f && LineOfSightClearForAttack(ped)) {
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
    const auto ogSubTask = m_pSubTask; // 0x62CCFE
    // Como el original: 0x62CD0E CWeaponInfo::GetWeaponInfo(tipo, habilidad) se obtiene al principio
    const auto& ourwi = ped->GetActiveWeapon().GetWeaponInfo(ped);

    if (m_newTarget) {
        return CreateFirstSubTask(ped);
    }

    if (!m_target || m_target->m_fHealth <= 0.f) {
        return nullptr;
    }

    //> 0x62CD55
    if (   (m_duckingConditions & 4) == 0
        || CTimer::GetTimeInMS() <= m_lastDuckTime + m_lengthOfDuck + 2000
        || ped->bIsDucking
    ) {
        // Como el original: 0x62CDFB GetRandomNumberInRange(0, 100) (407180) < probabilidad, sin signo (jae)
        if (   m_duckingConditions & 1
            && m_bShotFiredByPlayer
            && (uint32)CGeneral::GetRandomIntInRange(0, 100) < m_duckChancePerc
        ) {
            if (const auto quack = ped->GetIntelligence()->GetTaskDuck()) {
                quack->SetDuckTimer((uint16)m_lengthOfDuck);
            } else {
                ped->GetIntelligence()->SetTaskDuckSecondary((uint16)m_lengthOfDuck); // Como el original: sin tocar m_lastDuckTime en 0x62CE3C
            }
        }
    } else {
        // Como el original: 0x62CDB3 primero la distancia^2 (< 400.0, estricta) y SOLO entonces el aleatorio (407180)
        if ((m_target->GetPosition() - ped->GetPosition()).SquaredMagnitude() < 400.f) {
            if ((uint32)CGeneral::GetRandomIntInRange(0, 100) < m_duckChancePerc) {
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

    switch (m_pSubTask->GetTaskType()) { //> 0x62CE74
    case TASK_COMPLEX_SEEK_ENTITY: { //> 0x62D12D
        bool closeAndFacing = false;
        if (!ped->bStayInSamePlace) {
            const float dist = (m_target->GetPosition() - ped->GetPosition()).Magnitude(); // 4082C0
            if (ourwi.m_fTargetRange * 0.5f > dist) { // 0x62D180 fcompp + test 41h / jne
                if (   DotProduct(m_target->GetMoveSpeed(), ped->GetForward()) < 0.f // 0x62D191 (40FDB0) + test ah,5 / jnp
                    || CTimer::GetTimeInMS() - m_lastAttackTime > 2000                // 0x62D1AF cmp ecx,7D0h / jbe
                ) {
                    closeAndFacing = true;
                }
            }
        }
        // Como el original: 0x62D1BC lider -> (cerca/quieto y LOS) -> (sumergido y LOS), con hasta dos llamadas a LOS
        if (   IsPedInLeaderFiringLine(ped)
            || ((closeAndFacing || ped->bStayInSamePlace) && LineOfSightClearForAttack(ped))
            || (m_target->physicalFlags.bSubmergedInWater && LineOfSightClearForAttack(ped))
        ) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        if (ped->GetGroup()) { // 0x62D21A
            ped->Say(CTX_GLOBAL_COVER_ME);
        }
        break;
    }
    case TASK_SIMPLE_GUN_CTRL: { //> 0x62CE8D
        const float dist  = (m_target->GetPosition() - ped->GetPosition()).Magnitude(); // 4082C0 (raiz, no al cuadrado)
        const float range = ourwi.m_fTargetRange;
        const bool  stay  = ped->bStayInSamePlace;
        // Como el original: 0x62CED7 dist > rango; 0x62CEE8 rango*0.5 < dist && !stay && ahora > ultimoAtaque + 2500 (9C4h);
        // 0x62CF0C dist > 4.0 && !stay && ahora > shootTimer; si no, !LOS
        if (   dist > range
            || (range * 0.5f < dist && !stay && CTimer::GetTimeInMS() > m_lastAttackTime + 2500)
            || (dist > 4.f && !stay && CTimer::GetTimeInMS() > m_shootTimer)
            || !LineOfSightClearForAttack(ped)
        ) {
            const auto gctrl = static_cast<CTaskSimpleGunControl*>(m_pSubTask);
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

        //> 0x62CF7F
        const auto actionDir = [&, this]() -> CVector2D {
            if (m_target->physicalFlags.bSubmergedInWater) {
                return { 0.f, 0.f };
            }
            if (CTimer::GetTimeInMS() < m_lastStrafeTime && !ped->bStayInSamePlace) { // 0x62CFB5 / 0x62CFBA
                switch (m_strafeDir) {
                case eStrafeDir::LEFT:    return { -1.f,  0.f };
                case eStrafeDir::RIGHT:   return {  1.f,  0.f };
                case eStrafeDir::FORWARD: return {  0.f, -1.f };
                case eStrafeDir::BACK:    return {  0.f,  1.f };
                default:                  return {  0.f,  0.f }; // Como el original: 0x62CFC9 ja => sin movimiento
                }
            }
            if (dist < 4.f && !ped->bStayInSamePlace) { // 0x62D019 fcomp 4.0 + test ah,5 / jp; test stay / jne
                return { 0.f, 1.f }; // Forwards
            }
            if (m_bStrafeBack) { // 0x62D03A
                switch (m_strafeDir) { // Strafe back to where we came from the last time
                case eStrafeDir::LEFT:    m_strafeDir = eStrafeDir::RIGHT;   break;
                case eStrafeDir::RIGHT:   m_strafeDir = eStrafeDir::LEFT;    break;
                case eStrafeDir::FORWARD: m_strafeDir = eStrafeDir::BACK;    break;
                case eStrafeDir::BACK:    m_strafeDir = eStrafeDir::FORWARD; break;
                default:                  break; // Como el original: 0x62D048 ja => sin cambio
                }
                m_bStrafeBack    = false;
                m_lastStrafeTime = CTimer::GetTimeInMS() + 2500;
            }
            return { 0.f, 0.f };
        }();

        //> 0x62D07A
        if (quack) {
            if (m_strafeDir == eStrafeDir::LEFT || m_strafeDir == eStrafeDir::RIGHT) { // 0x62D086
                if (CTimer::GetTimeInMS() > m_lastRollTime + 3000) { // 0x62D0B7 add eax,0BB8h / jbe
                    quack->ControlDuckMove(actionDir);
                    m_lastRollTime = CTimer::GetTimeInMS();
                } else {
                    quack->ControlDuckMove(CVector2D{ 0.f, 0.f });
                }
            } else {
                quack->ControlDuckMove(actionDir);
            }
        } else {
            ugun->ControlGunMove(actionDir);
        }

        break;
    }
    }

    return ogSubTask; // Como el original: 0x62D253 devuelve la subtarea del principio
}
