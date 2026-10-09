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

    // Como el original (0x624B89): resta objetivo - peaton y magnitud sumando z*z + x*x y luego + y*y.
    const float deltaX = targetPos.x - pedPos.x;
    const float deltaY = targetPos.y - pedPos.y;
    const float deltaZ = targetPos.z - pedPos.z;
    const float magnitude = std::sqrt((deltaZ * deltaZ + deltaX * deltaX) + deltaY * deltaY);

    // Como el original (0x624BAF): rumbo atan2(-x, y) con fpatan (precision completa), se resta la rotacion actual,
    // se envuelve UNA sola vez (> pi: -2pi; < -pi: +2pi) y se compara |angulo| con 0x8D2E60 (estricto).
    const float rotation = ped->m_fCurrentRotation;
    const float piValue = std::bit_cast<float>(0x40490FDBu), minusPiValue = std::bit_cast<float>(0xC0490FDBu), twoPiValue = std::bit_cast<float>(0x40C90FDBu);
    const float attackAngle = std::bit_cast<float>(0x3E860A92u);
    bool outsideAngle;
    __asm {
        fld deltaX
        fchs
        fld deltaY
        fpatan
        fsub rotation
        fcom piValue
        fnstsw ax
        test ah, 41h
        jne check_lower
        fsub twoPiValue
        jmp wrapped
    check_lower:
        fcom minusPiValue
        fnstsw ax
        test ah, 5
        jp wrapped
        fadd twoPiValue
    wrapped:
        fabs
        fcomp attackAngle
        fnstsw ax
        test ah, 41h
        setz outsideAngle
    }
    if (outsideAngle) {
        // Como el original (0x624BF4): paso * 0.02 * 1000 truncado por _ftol2 y sumado a la hora del proximo ataque.
        m_nextAttackTime += FightControlElapsed(CTimer::GetTimeStep());
        return 0;
    }

    switch (m_target->GetType()) {
    case ENTITY_TYPE_PED: {
        // Como el original (0x624C2B): rand() del EXE se consume SIEMPRE antes de comparar la distancia.
        const int32 random = plugin::CallAndReturn<int32, 0x821B1E>();
        const bool lucky = (random & 0xF) == 0;
        const float range = magnitude - m_maxAttackRange;
        if (range > 0.1f) {
            return 3;
        }

        if (range > -0.1f) {
            return lucky ? 7 : -1;
        }

        // Como el original (0x624C7E): aqui se compara la magnitud (no el rango) con 0.8.
        if (!(magnitude < 0.8f)) {
            if ((plugin::CallAndReturn<int32, 0x821B1E>() & 0x3F) == 0) {
                return 8;
            }
            if ((plugin::CallAndReturn<int32, 0x821B1E>() & 0x3F) == 0) {
                return 10;
            }
        } else if (lucky) {
            return 9;
        }

        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        CVector planes[4];
        float planesDot[4];
        // Como el original (0x624D09): llama a la version con cache 0x5F3660.
        CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(targetPos.z, *m_target, &planes, planesDot);

        const auto hitSide = CPedGeometryAnalyser::ComputeEntityHitSide(pedPos, *m_target);
        // Como el original (0x624D43): (z*z + y*y) + x*x, luego + plano, comparacion estricta con 0.7.
        const auto& plane = planes[hitSide];
        const float distance = ((plane.z * pedPos.z + plane.y * pedPos.y) + plane.x * pedPos.x) + planesDot[hitSide];
        if (distance > 0.7f) {
            return 3;
        }

        break;
    }
    default: {
        if (magnitude - m_maxAttackRange > 0.3f) {
            return 3;
        }
        break;
    }
    }

    return -1;
}

// 0x61DD00
bool CTaskSimpleFightingControl::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_LEISURE) {
        return false;
    }
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        return true;
    }

    // Como el original: cualquier otro valor (URGENT sin evento o prioridades fuera de rango) cae al final.
    if (priority == ABORT_PRIORITY_URGENT && event) {
        if (event->GetEventPriority() < 22) {
            return false;
        }

        // Como el original (0x61DD53): GetEventType se vuelve a llamar por cada comparacion.
        if (event->GetEventType() == EVENT_PED_COLLISION_WITH_PED
            || event->GetEventType() == EVENT_PED_COLLISION_WITH_PLAYER
            || event->GetEventType() == EVENT_PLAYER_COLLISION_WITH_PED) {
            return false;
        }

        if (event->GetEventType() == EVENT_VEHICLE_COLLISION
            || event->GetEventType() == EVENT_OBJECT_COLLISION
            || event->GetEventType() == EVENT_BUILDING_COLLISION) {
            if (!m_target) {
                return true;
            }

            // Como el original: la normal de impacto de los tres eventos esta en +0x18.
            const auto& impactNormal = *reinterpret_cast<const CVector*>(reinterpret_cast<const uint8*>(event) + 0x18);
            CVector direction = m_target->GetPosition() - ped->GetPosition();
            direction.Normalise();

            // Como el original (0x61DDE2): y*ny + z*nz, luego + x*nx; se niega y se compara con 0.707 (0x863C1C).
            const float dot = (direction.y * impactNormal.y + direction.z * impactNormal.z) + direction.x * impactNormal.x;
            if (-dot < std::bit_cast<float>(0x3F34FDF4u)) { // NaN no entra, igual que el jp del original
                return false;
            }
        }
    }

    if (ped->GetIntelligence()->GetTaskFighting()) {
        return ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, priority, event);
    }

    return true;
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
