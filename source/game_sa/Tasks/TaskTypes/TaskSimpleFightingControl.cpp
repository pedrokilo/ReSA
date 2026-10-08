/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFightingControl
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleFightingControl.h"
#include "PedStats.h"
#include "TaskSimpleFight.h"

#include "EventObjectCollision.h"
#include "EventBuildingCollision.h"

constexpr auto FIGHT_CTRL_MAX_ATTACK_ANGLE_RAD = DegreesToRadians(15);
constexpr auto FIGHT_CTRL_FIGHT_IDLE_TIME = 60000.f;

static auto& FightControlAttackInterval = StaticRef<uint32>(0x8D2E4C);
static auto& FightControlIdleTime = StaticRef<float>(0x8D2E50);
static auto& FightControlBlockChance = StaticRef<float>(0x8D2E54);
static auto& FightControlBlockTimeMin = StaticRef<int32>(0x8D2E58);
static auto& FightControlBlockTimeMax = StaticRef<int32>(0x8D2E5C);

static uint32 FightControlElapsed(float step) {
    const float seconds = .02f, milliseconds = 1000.f;
    int64 elapsed;
    uint16 control, truncate;
    __asm {
        fld step
        fmul seconds
        fmul milliseconds
        fnstcw control
        mov ax, control
        or ax, 0C00h
        mov truncate, ax
        fldcw truncate
        fistp elapsed
        fldcw control
    }
    return static_cast<uint32>(elapsed);
}

static bool FightControlShouldBlock(float random, float chance, float rate) {
    const float scale = .025f;
    bool result;
    __asm {
        fld random
        fld chance
        fmul rate
        fmul scale
        fcompp
        fnstsw ax
        test ah, 41h
        setz result
    }
    return result;
}

void CTaskSimpleFightingControl::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFightingControl, 0x86d700, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61DC10);
    RH_ScopedInstall(Destructor, 0x61DCA0);

    RH_ScopedInstall(CalcMoveCommand, 0x624B50);

    RH_ScopedVMTInstall(Clone, 0x622EB0);
    RH_ScopedVMTInstall(GetTaskType, 0x61DC90);
    RH_ScopedVMTInstall(MakeAbortable, 0x61DD00);
    RH_ScopedVMTInstall(ProcessPed, 0x62A0A0);
}

// 0x61DC10
CTaskSimpleFightingControl::CTaskSimpleFightingControl(CEntity* entityToFight, float angleRad, float attackRange) :
    m_target{entityToFight},
    m_angleRad{angleRad},
    m_maxAttackRange{ attackRange }
{
    CEntity::SafeRegisterRef(m_target);
}

// NOTSA
CTaskSimpleFightingControl::CTaskSimpleFightingControl(const CTaskSimpleFightingControl& o) :
    CTaskSimpleFightingControl{o.m_target, o.m_angleRad, o.m_maxAttackRange }
{
}

// 0x61DCA0
CTaskSimpleFightingControl::~CTaskSimpleFightingControl() {
    CEntity::SafeCleanUpRef(m_target);
}

// 0x624B50
int16 CTaskSimpleFightingControl::CalcMoveCommand(CPed* ped) {
    if (!m_target) {
        return -1;
    }

    const auto& targetPos = m_target->GetPosition();
    const auto& pedPos    = ped->GetPosition();

    const auto segmentPedToTarget = targetPos - pedPos;

    // Check if target is outside of the attacking angle
    if (std::abs(CGeneral::LimitRadianAngle(segmentPedToTarget.Heading()) - ped->m_fCurrentRotation) >= FIGHT_CTRL_MAX_ATTACK_ANGLE_RAD) {
        m_nextAttackTime += (uint32)CTimer::GetTimeStepInMS();
        return 0;
    }

    switch (m_target->GetType()) {
    case ENTITY_TYPE_PED: {
        const auto range = segmentPedToTarget.Magnitude() - m_maxAttackRange;
        if (range > 0.1f) {
            return 3;
        }

        const auto rnd = rand();

        if (range > -0.1f) { // Inverted
            return (rnd % 16 == 0) ? 7 : -1;
        }

        if (range >= 0.8f) {
            if (rand() % 64 == 0) {
                return 8;
            }
            if (rand() % 64 == 0) {
                return 10;
            }
        } else if (rnd % 16 == 0) {
            return 9;
        }
     
        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        CVector planes[4]{};
        float planesDot[4]{};
        CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(targetPos.z, *m_target, &planes, planesDot);

        const auto hitSide = CPedGeometryAnalyser::ComputeEntityHitSide(pedPos, *m_target);
        if ((DotProduct(planes[hitSide], pedPos) + planesDot[hitSide]) >= 0.7f) {
            return 3;
        }

        break;
    }
    default: {
        if (segmentPedToTarget.Magnitude() - m_maxAttackRange > 0.3f) {
            return 3;
        }
        break;
    }
    }

    return -1;
}

// 0x61DD00
bool CTaskSimpleFightingControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_LEISURE:
        return false;
    case ABORT_PRIORITY_IMMEDIATE:
        return true;
    case ABORT_PRIORITY_URGENT: {
        if (event) {
            if (event->GetEventPriority() < 22) {
                return false;
            }

            switch (event->GetEventType()) {
            case EVENT_PED_COLLISION_WITH_PED:
            case EVENT_PED_COLLISION_WITH_PLAYER:
            case EVENT_PLAYER_COLLISION_WITH_PED: {
                return false;
            }
            case EVENT_VEHICLE_COLLISION:
            case EVENT_OBJECT_COLLISION:
            case EVENT_BUILDING_COLLISION: {
                if (!m_target) {
                    return true;
                }

                // This is not how they did it, but I'd like to avoid using blackmagic.
                // Once we reverse all stuff related to these events we should make a common base class
                // like `CEventEntityCollision` or something similar.
                const auto impactNormal = [event] {
                    switch (event->GetEventType()) {
                    case EVENT_VEHICLE_COLLISION:
                        return static_cast<const CEventVehicleCollision*>(event)->m_impactNormal;
                    case EVENT_OBJECT_COLLISION:
                        return static_cast<const CEventObjectCollision*>(event)->m_impactNormal;
                    case EVENT_BUILDING_COLLISION:
                        return static_cast<const CEventBuildingCollision*>(event)->m_impactNormal;
                    default:
                        NOTSA_UNREACHABLE();
                    }
                }();

                // Check if vector are opposing each other with 45deg tolerance
                if (-DotProduct(Normalized(m_target->GetPosition() - ped->GetPosition()), impactNormal) < SQRT_2 / 2) {
                    return false; // They aren't
                }
            }
            }
        }

        if (ped->GetIntelligence()->GetTaskFighting()) {
            return ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, priority, event);
        }

        return true;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x62A0A0
bool CTaskSimpleFightingControl::ProcessPed(CPed* ped) {
    if (!m_target || m_bool) {
        return true;
    }

    const float shootingRate = !ped->IsCreatedByMission() && ped->m_nPedType != PED_TYPE_COP && ped->m_nWeaponShootingRate == 40
        ? static_cast<float>(ped->m_pStats->m_flags) // Original reads the word at CPedStat + 0x30.
        : static_cast<float>(ped->m_nWeaponShootingRate);
    ped->GiveWeaponAtStartOfFight();

    int32 command = 0;
    if (!ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
        ped->GetTaskManager().SetTaskSecondary(new CTaskSimpleFight{m_target, 0, static_cast<uint32>(FightControlIdleTime)}, TASK_SECONDARY_ATTACK);
        m_nextAttackTime = 0;
    } else {
        if (ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->GetTaskType() != TASK_SIMPLE_FIGHT) {
            ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
            return false;
        }
        if (m_nextAttackTime <= CTimer::GetTimeInMS()) {
            m_nextAttackTime = 0;
            command = ped->m_nFightingStyle == 4 || ped->GetActiveWeapon().m_Type != WEAPON_UNARMED ? 11 : 12;
        } else {
            const bool startBlock = m_target->GetIsTypePed() && !m_someTime
                && FightControlShouldBlock(CGeneral::GetRandomNumberInRange(0.f, 100.f), FightControlBlockChance, shootingRate);
            if (startBlock) {
                m_someTime = CGeneral::GetRandomIntInRange(FightControlBlockTimeMin, FightControlBlockTimeMax);
                command = 2;
            } else if (m_someTime) {
                const auto elapsed = FightControlElapsed(CTimer::GetTimeStep());
                m_someTime = m_someTime > elapsed ? m_someTime - elapsed : 0;
                command = 2;
            }
        }
    }

    auto* fight = static_cast<CTaskSimpleFight*>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK));
    m_maxAttackRange = CTaskSimpleFight::m_aComboData[std::max(int32(fight->m_nComboSet) - 4, 0)].m_fRanges;
    if (!m_nextAttackTime) {
        // The original keeps the random ratio and denominator on the x87 stack
        // until its truncating conversion to milliseconds.
        const int32 random = CGeneral::GetRandomNumber();
        const float reciprocal = 1.f / 32767.f, quarter = .25f, rateScale = .025f, aggressionScale = .7f, base = .3f;
        const float aggression = m_unk3;
        const uint32 interval = FightControlAttackInterval;
        const double unsignedCorrection = 4294967296.0;
        int32 delay;
        __asm {
            fild random
            fmul reciprocal
            fadd quarter
            fld shootingRate
            fmul aggression
            fmul rateScale
            fmul aggressionScale
            fadd base
            fdivp st(1), st(0)
            fild interval
            cmp interval, 0
            jge positive_interval
            fadd unsignedCorrection
        positive_interval:
            fmulp st(1), st(0)
            sub esp, 12
            fnstcw word ptr [esp]
            mov ax, word ptr [esp]
            or ax, 0C00h
            mov word ptr [esp+2], ax
            fldcw word ptr [esp+2]
            fistp qword ptr [esp+4]
            fldcw word ptr [esp]
            mov eax, [esp+4]
            add esp, 12
            mov delay, eax
        }
        m_nextAttackTime = static_cast<uint32>(delay) + CTimer::GetTimeInMS();
    }
    if (fight->m_nComboSet <= 1) {
        const auto move = CalcMoveCommand(ped);
        if (move > -1) {
            command = move;
        }
    }
    fight->ControlFight(m_target, static_cast<uint8>(command));
    return false;
}
