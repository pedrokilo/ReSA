/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleJetPack
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include "TaskSimpleJetPack.h"
#include "TaskSimplePlayerOnFoot.h"

constexpr CVector JETPACK_POS_OFFSET   = { 0.1f, 0.08f, 0.0f };
constexpr CVector JETPACK_ROT_AXIS     = { 0.0f, 1.0f, 0.0f };
constexpr CVector JETBALL_OFFSET       = { 0.f, 0.f, -0.15f };

constexpr float MAGIC = 0.008f; // == 1 / 125

constexpr float THRUST_NOMINAL         = 0.8f;
constexpr float THRUST_FULL            = 0.6f;
constexpr float THRUST_STRAFE          = 0.3f;
constexpr float THRUST_STOP            = 0.5f;
constexpr float THRUST_MAX_ANGLE       = std::bit_cast<float>(0x3FA78D36u); // 0x8D2F48 - Como el original: 75º en radianes (no 1.309f)
constexpr float THRUST_MOVE_DAMPING    = 0.02f;

constexpr float JETPACK_TURN_RATE      = -0.05f;
constexpr float JETPACK_ANGLE_RATE     = 0.9f;

constexpr float LEG_SWING_MAX_ANGLE    = std::bit_cast<float>(0x3F490FDBu); // 0x8D2F58 - Como el original: pi/4 exacto (no 0.7854f)
constexpr float LEG_SWING_DELTA_V_MULT = -0.2f;
constexpr float LEG_SWING_GRAVITY_MULT = 0.01f;
constexpr float LEG_SWING_DAMP_FRAC    = 0.98f;

namespace {
// Como el original: constantes exactas del EXE
const float kJpRadToDeg      = std::bit_cast<float>(0x42652EE0u); // 0x859878 (no es el float más cercano a 180/pi)
const float kJpPi            = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
const float kJpTwoPi         = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
const float kJpMinusPi       = std::bit_cast<float>(0xC0490FDBu); // 0x858CC0
const float kJpInv60         = std::bit_cast<float>(0x3C888889u); // 0x859044 (se multiplica, no se divide entre 60)
const float kJpTimeToRad     = std::bit_cast<float>(0x3ACDE32Eu); // 0x870640
const float kJpJetBallPeriod = std::bit_cast<float>(0x429F27AAu); // 0x8D2F80
const float kJpRandRecip     = std::bit_cast<float>(0x38000100u); // 0x858C7C (1/32767)
} // namespace

void CTaskSimpleJetPack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleJetPack, 0x8705c4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x67B4E0);
    RH_ScopedInstall(Destructor, 0x67E600);

    RH_ScopedInstall(RenderJetPack, 0x67F6A0);

    RH_ScopedInstall(ProcessAnims, 0x67B5C0);
    RH_ScopedInstall(DropJetPack, 0x67B660);
    RH_ScopedInstall(ApplyRollAndPitch, 0x67B720);
    RH_ScopedInstall(DoJetPackEffect, 0x67B7F0);
    RH_ScopedInstall(StopJetPackEffect, 0x67BA10);
    RH_ScopedInstall(ProcessControlInput, 0x67E7B0);
    RH_ScopedInstall(ProcessThrust, 0x67EF20);

    RH_ScopedVMTInstall(Clone, 0x67C690);
    RH_ScopedVMTInstall(GetTaskType, 0x67B5B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x67E6E0);
    RH_ScopedVMTInstall(ProcessPed, 0x6801F0);
}

// 0x67B4E0
CTaskSimpleJetPack::CTaskSimpleJetPack(const CVector* targetPos, float cruiseHeight, int32 hoverTime, CEntity* targetEntity) :
    m_HoverTime{ hoverTime },
    m_CruiseHeight{ cruiseHeight },
    m_TargetEntity{ targetEntity }
{
    CEntity::SafeRegisterRef(m_TargetEntity);

    if (targetPos)
        m_TargetPos = *targetPos;
}

CTaskSimpleJetPack::CTaskSimpleJetPack(const CTaskSimpleJetPack& o) :
    CTaskSimpleJetPack{ &o.m_TargetPos, o.m_CruiseHeight, o.m_HoverTime, o.m_TargetEntity }
{
}

// 0x67E600
CTaskSimpleJetPack::~CTaskSimpleJetPack() {
    if (m_JetPackClump) {
        CVisibilityPlugins::GetClumpModelInfo(m_JetPackClump)->RemoveRef();
        if (const auto a = GetFirstAtomic(m_JetPackClump)) {
            if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(a))) {
                RpClumpForAllAtomics(m_JetPackClump, AtomicRemoveAnimFromSkinCB, nullptr);
            }
        }
        RpClumpDestroy(m_JetPackClump);
        m_JetPackClump = nullptr; // Como el original: el EXE pone el puntero a nulo
    }
    StopJetPackEffect();
    CEntity::SafeCleanUpRef(m_TargetEntity);
}

// 0x67E6E0
bool CTaskSimpleJetPack::MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (event) {
        if (event->GetEventType() == EVENT_DAMAGE) {
            if (ped->m_fHealth > 0.f || (!ped->bIsStanding && !ped->physicalFlags.bSubmergedInWater)) {
                return false;
            }
        }

        if (ped->IsPlayer() && event->GetEventType() == EVENT_ON_FIRE) {
            return true;
        }

        if (event->GetEventPriority() < 63) {
            return false;
        }
    }

    if (m_bSwitchedWeapons && ped->IsPlayer()) {
        ped->ReplaceWeaponWhenExitingVehicle();
        m_bSwitchedWeapons = false;
    }

    m_bIsFinished = true;

    StopJetPackEffect();
    ped->GetAE().TurnOffJetPack();

    return true;
}

// 0x6801F0
bool CTaskSimpleJetPack::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        if (m_bSwitchedWeapons && ped->IsPlayer()) {
            ped->ReplaceWeaponWhenExitingVehicle();
        }
        StopJetPackEffect();
        return true;
    }

    if (!m_bSwitchedWeapons) {
        if (ped->IsPlayer()) {
            ped->RemoveWeaponWhenEnteringVehicle(true);
        }
        m_bSwitchedWeapons = true; // Como el original: también si no es jugador
    }

    ped->bCheckColAboveHead = true;

    if (ProcessAnims(ped)) {
        if (ped->m_fHealth < 1.f) { // Como el original: con NaN se controla como jugador
            m_ThrustStop   = false;
            m_ThrustFwd    = 0;
            m_ThrustAngle  = 0.f;
            // Como el original: rand() & 1 suma 1 y si no resta 1 (antes la precedencia lo dejaba en +-1)
            m_ThrustStrafe = (CGeneral::GetRandomNumber() & 1) ? m_ThrustStrafe + 1.f : m_ThrustStrafe - 1.f;
            if (m_ThrustStrafe > 3.f) {
                m_ThrustStrafe = 3.f;
            } else if (m_ThrustStrafe < -3.f) {
                m_ThrustStrafe = -3.f;
            }

            // Como el original: t = ms · 0.0015707964 (un redondeo); ((fsin(t) · paso) · -0.05) · rand() · (1/32767)
            const float t  = UnsignedTimesFloatCpp(CTimer::m_snTimeInMilliseconds, kJpTimeToRad);
            const float rf = (float)CGeneral::GetRandomNumber() * kJpRandRecip;
            ped->m_fAimingRotation = TrigSimpleSinMulCpp(t, CTimer::GetTimeStep()) * JETPACK_TURN_RATE * rf + ped->m_fAimingRotation;

            if (ped->GetPlayerData()) {
                ped->GetPlayerData()->m_fMoveBlendRatio = 0.f;
            }
        } else if (ped->IsPlayer()) {
            ProcessControlInput(ped->AsPlayer());
        }
        ProcessThrust(ped);
        if (m_ThrustFwd > 0 || std::abs(m_ThrustStrafe) > 0.1f) {
            ped->bIsStanding = false;
        }
    }

    return false;
}

// 0x67F6A0
void CTaskSimpleJetPack::RenderJetPack(CPed* ped) {
    if (m_JetPackClump && !m_bIsFinished) {
        RwMatrix* const boneMats = RpHAnimHierarchyGetMatrixArray(GetAnimHierarchyFromSkinClump(ped->GetRpClump()));
        const auto jp = RpClumpGetFrame(m_JetPackClump);

        // Update JetPack matrix
        {
            const auto jpMat = RwFrameGetMatrix(jp);
            std::memcpy(jpMat, &boneMats[PED_NODE_LEFT_ARM], sizeof(RwMatrix)); // Como el original: rep movs de la matriz entera
            RwMatrixTranslate(jpMat, &JETPACK_POS_OFFSET, rwCOMBINEPRECONCAT);
            RwMatrixRotate(jpMat, &JETPACK_ROT_AXIS, 90.f, rwCOMBINEPRECONCAT);
        }

        // Como el original: comparaciones estrictas con pi exacto del EXE
        auto currHeading = ped->m_fAimingRotation;
        if (currHeading > m_PrevHeading + kJpPi) {
            currHeading -= kJpTwoPi;
        } else if (currHeading < m_PrevHeading - kJpPi) {
            currHeading += kJpTwoPi;
        }
        auto headingDelta = currHeading - m_PrevHeading;
        if (headingDelta > 0.2f) {
            headingDelta = 0.2f;
        } else if (headingDelta < -0.2f) {
            headingDelta = -0.2f;
        }
        // Como el original: (delta · 180/pi) · 10, que se suma en un lado y se resta en el otro
        const float headingDeg = headingDelta * kJpRadToDeg * 10.f;

        const auto ProcessJetBall = [&](const char* jbFrameName, bool bLeftSide) {
            const auto jb = CClumpModelInfo::GetFrameFromName(m_JetPackClump, jbFrameName);
            if (!jb) {
                return;
            }
            float x, y;
            if (m_ThrustStop) {
                const float t = JetPackUnsignedDivCpp(CTimer::m_snTimeInMilliseconds, kJpJetBallPeriod);
                x = TrigSimpleSinMulCpp(t, bLeftSide ? 45.f : -45.f);
                y = TrigSimpleCosMulCpp(t, 45.f);
            } else {
                const float a = bLeftSide ? m_ThrustAngle * kJpRadToDeg + headingDeg : m_ThrustAngle * kJpRadToDeg - headingDeg;
                x = 90.f < a ? 90.f : (-90.f > a ? -90.f : a);
                const float rotY = m_ThrustStrafe * 45.f;
                y = bLeftSide ? (0.f > rotY ? 0.f : rotY) : (0.f < rotY ? 0.f : rotY);
            }
            const auto  jbMat = RwFrameGetMatrix(jb);
            const RwV3d jbPos = *RwMatrixGetPos(jbMat);
            RwMatrixRotate(jbMat, &CPedIK::XaxisIK, x, rwCOMBINEREPLACE);
            RwMatrixRotate(jbMat, &CPedIK::YaxisIK, y, rwCOMBINEPRECONCAT);
            RwV3dAssign(RwMatrixGetPos(jbMat), &jbPos); // Changing the rotation also changes the position I assume, so restore it
        };

        ProcessJetBall("jetball1", true);
        ProcessJetBall("jetball2", false);

        RwFrameUpdateObjects(jp);
        RpClumpRender(m_JetPackClump);
        DoJetPackEffect(ped);
    } else {
        StopJetPackEffect();
    }
    m_PrevHeading = ped->m_fCurrentRotation;
}

// 0x67EF20
void CTaskSimpleJetPack::ProcessThrust(CPed* ped) {
    // Como el original: toda la aritmetica float es x87 (ayudantes JpFix* de X87Math.cpp: 24 bits, exponente extendido, NaN y operandos de memoria
    // senalizantes como fmul/fsub/fadd m32). Se pasan punteros a la memoria original (masa, paso, matriz, velocidades) para no silenciar un NaN senalizante.
    CVector& speed = ped->m_vecMoveSpeed;
    const CVector prevPedVel = speed;
    const float* const stepPtr = &CTimer::ms_fTimeStep;

    if (m_ThrustStop) {
        float force[3];
        JpFixUpForceCpp(&ped->m_matrix->GetUp().x, &ped->m_fMass, stepPtr, force);
        ped->ApplyMoveForce(CVector{ force[0], force[1], force[2] });

        // Como el original: la velocidad se lee despues de aplicar la fuerza; (-v / |v|) * fuerza * masa sin paso, o velocidad cero
        if (JpFixStopForceCpp(&speed.x, stepPtr, &ped->m_fMass, force)) {
            ped->ApplyMoveForce(CVector{ force[0], force[1], force[2] });
        } else {
            speed = CVector{ 0.f, 0.f, 0.f };
        }
    } else {
        float force[3];
        JpFixNominalForceCpp(&ped->m_matrix->GetUp().x, m_ThrustFwd < 0, &ped->m_fMass, stepPtr, force);
        ped->ApplyMoveForce(CVector{ force[0], force[1], force[2] });

        if (m_ThrustFwd > 0) { // 0x67F1B7
            const CMatrix& mat = *ped->m_matrix;
            float t[3];
            JpFixThrustDirCpp(&m_ThrustAngle, &mat.GetUp().x, &mat.GetForward().x, t); // incluye el limite 0.75 a la z de la suma

            // Como el original: 1 - (z - (suelo + 100)) * 0.05 solo por encima, con dos llamadas a CalculateGroundHeight (la segunda solo por encima)
            const float* const z = &ped->GetPosition().z;
            const bool above = JpFixHeightBelowCpp(TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_TOP), z);
            const float ground2 = above ? TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_TOP) : 0.f;
            JpFixThrustForceCpp(t, above, ground2, z, &ped->m_fMass, stepPtr, force);
            ped->ApplyMoveForce(CVector{ force[0], force[1], force[2] });
        }

        if (m_ThrustStrafe != 0.f) { // 0x67F36F
            JpFixStrafeForceCpp(&m_ThrustStrafe, &ped->m_matrix->GetRight().x, &ped->m_fMass, stepPtr, force);
            ped->ApplyMoveForce(CVector{ force[0], force[1], force[2] });
        }
    }

    // Como el original (0x67F43D..0x67F660): balanceo de piernas y amortiguacion de la velocidad (x87 en X87Math)
    {
        float sums[3];
        JpFixSwingSumsCpp(&speed.x, &prevPedVel.x, &m_PrevVelocity.x, sums);
        const bool grounded = ped->bIsStanding || ped->bWasStanding;
        JpFixSwingAxisCpp(sums, &ped->m_matrix->GetForward().x, grounded, stepPtr, &m_LegSwingFwdSpeed, &m_LegSwingFwd);
        JpFixSwingAxisCpp(sums, &ped->m_matrix->GetRight().x, grounded, stepPtr, &m_LegSwingSideSpeed, &m_LegSwingSide);
        JpFixTwistDampCpp(&m_LegTwist, stepPtr);
        JpFixSpeedDampCpp(&speed.x, stepPtr);
        m_PrevVelocity = speed;
    }

    if (!m_bIsFinished) {
        ped->GetAE().TurnOnJetPack();
        ped->GetAE().UpdateJetPack((float)m_ThrustFwd, m_ThrustAngle);
    }
}

// 0x67B5C0
bool CTaskSimpleJetPack::ProcessAnims(CPed* ped) {
    if (CStreaming::IsModelLoaded(MODEL_JETPACK)) {
        if (!m_JetPackClump) {
            const auto mi = CModelInfo::GetModelInfo(MODEL_JETPACK);
            m_JetPackClump = reinterpret_cast<RpClump*>(mi->CreateInstance());
            mi->AddRef(); // Como el original: sin comprobar el clump creado
        }
    } else {
        CStreaming::RequestModel(MODEL_JETPACK, STREAMING_KEEP_IN_MEMORY);
    }

    if (!m_JetPackClump) {
        return false;
    }

    if (ped->bIsStanding) {
        if (ped->IsPlayer()) {
            ped->AsPlayer()->SetRealMoveAnim();
        }
    } else {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 4.f);
        ped->SetMoveState(PEDMOVE_STILL);
    }

    return true;
}

// 0x67E7B0
void CTaskSimpleJetPack::ProcessControlInput(CPlayerPed* player) {
    const auto pad = player->GetPadFromPlayer();
    // Como el original: GetWeaponSkill() sin tipo (0x5E6580) del arma activa
    const auto weaponType = player->m_aWeapons.data()[(int8)player->m_nActiveWeaponSlot].m_Type;
    const auto wi = CWeaponInfo::GetWeaponInfo(weaponType, player->GetWeaponSkill());

    if (wi->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT && wi->flags.bAimWithArm) { // 0x67E7E5
        if (const auto task = player->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT)) {
            if (task->GetTaskType() == TASK_SIMPLE_PLAYER_ON_FOOT) {
                static_cast<CTaskSimplePlayerOnFoot*>(task)->ProcessPlayerWeapon(player);
            }
        }
    }

    // Como el original: estado de movimiento < PEDMOVE_WALK con signo
    if (player->bIsStanding && (int32)player->m_nMoveState < (int32)PEDMOVE_WALK && pad->GetExitVehicle()) { // 0x67E829
        DropJetPack(player);
        return;
    }

    const auto step = CTimer::GetTimeStep();
    const auto walkUpDown    = (float)pad->GetPedWalkUpDown();
    const auto walkLeftRight = (float)pad->GetPedWalkLeftRight();
    const auto padMoveMag    = std::sqrt(walkUpDown * walkUpDown + walkLeftRight * walkLeftRight) * kJpInv60;
    float      walkUpDownScaled = 0.f;

    const auto ClampStrafeUp = [this](float s, float limit) { m_ThrustStrafe = limit < s ? limit : s; };
    const auto ClampStrafeDown = [this](float s, float limit) { m_ThrustStrafe = limit > s ? limit : s; };
    const auto DecayStrafe = [&, this] { // 0x67ED69
        if (m_ThrustStrafe > 0.f) {
            ClampStrafeDown(m_ThrustStrafe - step * 0.05f, 0.f);
        } else if (m_ThrustStrafe < 0.f) {
            ClampStrafeUp(step * 0.05f + m_ThrustStrafe, 0.f);
        }
    };

    if (CCamera::m_bUseMouse3rdPerson && !player->bIsStanding) { // 0x67E8BA
        const float walkLeftRightScaled = walkLeftRight * 0.0078125f;
        walkUpDownScaled = walkUpDown * 0.0078125f;
        // Como el original: fpatan del frente de la cámara; mirando a otro lado se multiplican x e y por -1 y se niega x (no se resta pi)
        const CVector front = TheCamera.GetActiveCam().m_vecFront;
        player->m_fAimingRotation = TheCamera.GetLookDirection() == LOOKING_DIRECTION_FORWARD
            ? TrigAtan2Cpp(-front.x, front.y)
            : JetPackLookBackHeadingCpp(front.x, front.y);
        m_ThrustAngle = JetPackLerpThrustAngleCpp(walkUpDownScaled, m_ThrustAngle, step);

        if (walkLeftRightScaled < 0.f) {
            ClampStrafeDown(m_ThrustStrafe - step * 0.1f, -1.f);
        } else if (walkLeftRightScaled > 0.f) {
            ClampStrafeUp(step * 0.1f + m_ThrustStrafe, 1.f);
        } else if (m_ThrustStrafe > 0.f) {
            const float s = m_ThrustStrafe - step * 0.05f;
            m_ThrustStrafe = 0.f > s ? 0.f : s; // 0x420800
        } else if (m_ThrustStrafe < 0.f) {
            const float s = step * 0.05f + m_ThrustStrafe;
            m_ThrustStrafe = 0.f < s ? 0.f : s; // 0x404330
        }
    } else if (!player->m_pTargetedObject && !player->bIsStanding && !pad->GetTarget()) { // 0x67EA51
        const float walkLeftRightScaled = walkLeftRight * 0.0078125f;
        walkUpDownScaled = walkUpDown * 0.0078125f;
        if (walkLeftRightScaled != 0.f) { // 0x67EAA1
            // Como el original: ((lr / 128) · paso) · -0.05 + rumbo, con un solo ajuste de +-2pi
            const float a = walkLeftRightScaled * step * JETPACK_TURN_RATE + player->m_fAimingRotation;
            if (a > kJpPi) {
                player->m_fAimingRotation = a - kJpTwoPi;
            } else if (a < kJpMinusPi) {
                player->m_fAimingRotation = a + kJpTwoPi;
            } else {
                player->m_fAimingRotation = a;
            }
        }
        m_ThrustAngle = JetPackLerpThrustAngleCpp(walkUpDownScaled, m_ThrustAngle, step); // Como el original: siempre en esta rama
    } else { // 0x67EB22
        if (padMoveMag > 0.f) {
            // Como el original: ángulo entre (0, 0) y (-lr, ud) menos la orientación de la cámara (0xB6F178), limitado
            const float angle = CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -walkLeftRight, walkUpDown) - StaticRef<float>(0xB6F178);
            player->m_fAimingRotation = CGeneral::LimitRadianAngle(angle);
            m_ThrustAngle = THRUST_MAX_ANGLE * padMoveMag * -0.3515625f;
        } else {
            player->GetPlayerData()->m_fMoveBlendRatio = 0.f;
        }
    }

    if (player->m_pTargetedObject && !player->bIsStanding) {
        // Como el original: límite |dz| · 0.2 + 3, divisor max(dist, 0.1) por recíproco, (1 - dist / límite) · 3, masa y paso
        const CVector& targetPos = player->m_pTargetedObject->GetPosition();
        const CVector& playerPos = player->GetPosition();
        const float dx = playerPos.x - targetPos.x;
        const float dy = playerPos.y - targetPos.y;
        const float dz = playerPos.z - targetPos.z;
        const float limit = std::abs(dz) * 0.2f + 3.f;
        const float dist2D = std::sqrt(dx * dx + dy * dy);
        if (dist2D < limit) {
            const float inv = 1.f / (0.1f > dist2D ? 0.1f : dist2D);
            const float k = (1.f - dist2D / limit) * 3.f;
            const float x = dx * inv, y = dy * inv, z = inv * 0.f;
            const float mass = player->m_fMass;
            player->ApplyMoveForce(CVector{
                x * k * MAGIC * THRUST_FULL * mass * step,
                y * k * MAGIC * THRUST_FULL * mass * step,
                z * k * MAGIC * THRUST_FULL * mass * step
            });
        }
    }

    if (CCamera::m_bUseMouse3rdPerson && !player->bIsStanding) { // 0x67ED2D
        m_ThrustStop = false;
        if (pad->GetBrake()) {
            m_ThrustFwd = -1;
        } else {
            m_ThrustFwd = (char)((pad->GetAccelerate() || walkUpDownScaled != 0.f) ? 1 : 0);
        }
        DecayStrafe();
    } else if (pad->GetLookBehindForCar()) { // 0x67EDA4
        m_ThrustStop   = true;
        m_ThrustFwd    = 0;
        m_ThrustStrafe = 0.f;
    } else {
        m_ThrustStop = false;
        // Como el original: sin ratón solo acelera con el botón (no con el stick)
        m_ThrustFwd  = (char)(pad->GetBrake() ? -1 : (pad->GetAccelerate() ? 1 : 0));
        if (pad->GetLookLeft() && !pad->GetTarget()) {
            ClampStrafeDown(m_ThrustStrafe - step * 0.1f, -1.f);
        } else if (pad->GetLookRight() && !pad->GetTarget()) {
            ClampStrafeUp(step * 0.1f + m_ThrustStrafe, 1.f);
        } else {
            DecayStrafe();
        }
    }

    // 0x67EEA9
    auto& br = player->GetPlayerData()->m_fMoveBlendRatio;
    if (padMoveMag > 0.f) {
        const auto delta = step * 0.07f;
        br = padMoveMag - br > delta
            ? delta + br
            : padMoveMag;
    } else {
        br = 0.f;
    }
}

// 0x67B720
void CTaskSimpleJetPack::ApplyRollAndPitch(CPed* ped) {
    if (ped->bIsStanding || ped->bWasStanding) {
        return;
    }
    for (const auto nodeId : { PED_NODE_LEFT_LEG, PED_NODE_RIGHT_LEG }) { // Rotate legs according to current swing values
        const auto q = &ped->m_apBones[nodeId]->KeyFrame->q;
        RtQuatRotate(q, &CPedIK::ZaxisIK, m_LegSwingFwd * kJpRadToDeg, rwCOMBINEPOSTCONCAT); // Como el original: 180/pi del EXE
        RtQuatRotate(q, &CPedIK::YaxisIK, m_LegSwingSide * kJpRadToDeg, rwCOMBINEPOSTCONCAT);
    }
    ped->bUpdateMatricesRequired = true;
}

// 0x67B660
void CTaskSimpleJetPack::DropJetPack(CPed* ped) {
    if (!m_bIsFinished) {
        CVector pickupPos;
        CPickups::CreatePickupCoorsCloseToCoors(ped->GetPosition(), pickupPos);
        CPickups::GenerateNewOne(pickupPos, MODEL_JETPACK, PICKUP_ONCE_TIMEOUT_SLOW, 0, 0, true, nullptr);
    }
    MakeAbortable(ped);
    m_ThrustStop   = 0;
    m_ThrustFwd    = 0;
    m_ThrustStrafe = 0.f;
}

// 0x67B7F0
void CTaskSimpleJetPack::DoJetPackEffect(CPed* ped) {
    // Como el original: primero se crean los dos efectos (el tiempo clave vuelve a 0 al crear cada uno),
    // después se colocan y al final se acerca el tiempo clave a su objetivo de 0.1 en 0.1 sin pasarse.
    if (const auto clump = ped->GetRpClump()) {
        if (const auto mat = RwFrameGetMatrix(RpClumpGetFrame(clump))) {
            for (auto& fx : m_ThrusterFX) {
                if (fx) {
                    continue;
                }
                const CVector zero{ 0.f, 0.f, 0.f };
                fx = g_fxMan.CreateFxSystem("jetpack", zero, mat, false);
                if (fx) {
                    fx->Play();
                    fx->SetLocalParticles(true);
                    fx->CopyParentMatrix();
                    m_FxKeyTime = 0.f;
                }
            }
        }
    }
    const char* const jbFrameNames[2]{ "jetball1", "jetball2" };
    for (size_t i = 0; i < 2; ++i) {
        if (const auto fx = m_ThrusterFX[i]) {
            fx->SetMatrix(RwFrameGetLTM(CClumpModelInfo::GetFrameFromName(m_JetPackClump, jbFrameNames[i])));
            fx->SetOffsetPos(JETBALL_OFFSET);
        }
    }

    // When pressing the brake button (space) you're going down faster.
    // But the flame is blue, even though the audio for the blue flame isn't playing.
    // So, going by the logic "blue == thrust is generated", "orange == no thrust generated"
    // I conclude it this is a bug.
    const bool  thrusting = m_ThrustFwd != 0 || m_ThrustStrafe != 0.f; // Como el original: también al frenar (-1)
    const float target    = thrusting ? 1.f : 0.f;
    if (target > m_FxKeyTime) {
        m_FxKeyTime += 0.1f;
        if (m_FxKeyTime > 1.f) {
            m_FxKeyTime = 1.f;
        }
    } else if (target < m_FxKeyTime) {
        m_FxKeyTime -= 0.1f;
        if (m_FxKeyTime < 0.f) {
            m_FxKeyTime = 0.f;
        }
    }
    for (const auto fx : m_ThrusterFX) {
        if (fx) {
            fx->SetConstTime(true, m_FxKeyTime);
        }
    }
}

// 0x67BA10
void CTaskSimpleJetPack::StopJetPackEffect() {
    for (auto& fx : m_ThrusterFX) {
        if (fx) {
            fx->Kill();
            fx = nullptr;
        }
    }
}
