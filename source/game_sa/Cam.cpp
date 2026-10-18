/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCam
 * Reconstrucción: COMPLETA — 48/48 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 48/48 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Cam.h"
#include "TimeCycle.h"
#include "Camera.h"
#include "Shadows.h"
#include "IdleCam.h"
#include "InterestingEvents.h"
#include "ModelIndices.h"
#include "HandShaker.h"
#include "CullZones.h"
#include "WaterLevel.h"
#include "Tasks/TaskTypes/TaskSimpleGangDriveBy.h"
#include "ControllerConfigManager.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "PlayerPedData.h"
#include "PedClothesDesc.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Ragdoll/IKChainManager.h"
#include "X87Math.h"

auto& gbFirstPersonRunThisFrame = StaticRef<bool>(0xB6EC20);
auto& gLastFrameProcessedDWCineyCam = StaticRef<uint32>(0x8CCB9C);

static inline auto& gbExitCam = StaticRef<std::array<bool, 9>>(0xB6EC5C);

static inline auto& DWCineyCamLastPos = StaticRef<CVector>(0xB6FE8C);
static inline auto& DWCineyCamLastUp = StaticRef<CVector>(0xB6FE98);
static inline auto& DWCineyCamLastRight = StaticRef<CVector>(0xB6FEA4);
static inline auto& DWCineyCamLastFwd = StaticRef<CVector>(0xB6FEB0);

static inline auto& DWCineyCamLastNearClip = StaticRef<float>(0xB6EC08);
static inline auto& DWCineyCamLastFov = StaticRef<float>(0xB6EC0C);

// Constantes float leidas del EXE (algunas no son el float mas cercano al literal decimal).
static inline float CamExeFloat(uintptr_t address) {
    return std::bit_cast<float>(*reinterpret_cast<const uint32*>(address));
}

// Operando de 32 bits (direccion o valor entero) que reciben los ayudantes x87 de X87Math para lo que el ensamblador cargaba en un registro.
template<class T>
static inline unsigned CamAddr(T value) {
    if constexpr (std::is_pointer_v<T>) {
        return static_cast<unsigned>(reinterpret_cast<uintptr_t>(value));
    } else {
        return static_cast<unsigned>(value);
    }
}

// Como el EXE: fsin/fcos del x87 (precision completa; el resultado se redondea a float al guardarlo).
static float CamExeSin(float angle) {
    float result;
    result = TrigSimpleSinCpp(angle);
    return result;
}
static float CamExeCos(float angle) {
    float result;
    result = TrigSimpleCosCpp(angle);
    return result;
}

// 0x59C790 (Multiply3x3 matriz * vector) con el orden de sumas del EXE: ((up * z) + (otro * ...)) + ...
static CVector CamMultiplyMatrixVector(const CMatrix& m, const CVector& v) {
    return CVector{
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y
    };
}

// 0x59C810 (Multiply3x3 vector * matriz) con el orden de sumas del EXE
static CVector CamMultiplyVectorMatrix(const CVector& v, const CMatrix& m) {
    return CVector{
        (m.GetRight().y * v.y + m.GetRight().z * v.z) + v.x * m.GetRight().x,
        (m.GetForward().y * v.y + m.GetForward().x * v.x) + m.GetForward().z * v.z,
        (m.GetUp().y * v.y + m.GetUp().x * v.x) + m.GetUp().z * v.z
    };
}

// 0x509AE0
static void WellBufferMe(float target, float& valueToChange, float& speedSoFar, float topSpeed, float speedStep, bool isAnAngle) {
    // Como el original: constantes pi, 2pi y -pi leidas del EXE (0x858CB8, 0x858CBC, 0x858CC0).
    const float pi    = CamExeFloat(0x858CB8);
    const float twoPi = CamExeFloat(0x858CBC);
    const float negPi = CamExeFloat(0x858CC0);

    float diff = target - valueToChange;
    if (isAnAngle) { // cmp al,1 (0x509AEE)
        for (; diff >= pi; diff -= twoPi) {
            ;
        }
        for (; diff < negPi; diff += twoPi) {
            ;
        }
    }

    const float fullSpeedStep = diff * topSpeed;
    // Como el original (0x509B4D..0x509B6E): el cambio suma si (fullSpeedStep - speedSoFar) > 0 y resta si no
    const float speedDelta = fullSpeedStep - speedSoFar;
    const float change = std::abs(speedDelta) * CTimer::GetTimeStep() * speedStep;
    speedSoFar = speedDelta > 0.0f ? speedSoFar + change : speedSoFar - change;

    // 0x509B70..0x509BA4: si fullSpeedStep < 0 y fullSpeedStep > speedSoFar -> speedSoFar = fullSpeedStep;
    // si no, si fullSpeedStep > 0 y fullSpeedStep < speedSoFar -> speedSoFar = fullSpeedStep.
    if (fullSpeedStep < 0.0f) {
        if (fullSpeedStep > speedSoFar) {
            speedSoFar = fullSpeedStep;
        }
    } else if (fullSpeedStep > 0.0f && fullSpeedStep < speedSoFar) {
        speedSoFar = fullSpeedStep;
    }

    // 0x509BA6..0x509BD2: fld 10.0; fcomp ts; jp => si 10.0 < ts usa 10.0, si no usa ts
    const float step = CamExeFloat(0x85862C) < CTimer::GetTimeStep() ? CamExeFloat(0x85862C) : CTimer::GetTimeStep();
    valueToChange = step * speedSoFar + valueToChange;
}

// 0x509BE0: cdecl, un `float*`; lleva *angle al intervalo [-pi, pi) (mientras >= pi resta 2pi; mientras < -pi suma 2pi) con la x87 a 24 bits del EXE y deja la pila x87,
// las banderas y C0-C3 como el, lo hace CamWrapAngleCpp (X87Math.cpp).
void CCam::WrapAngle(float* angle) {
    CamWrapAngleCpp(angle);
}

// Envoltura del hook de 0x509BE0. El unico llamador del EXE es KeepTrackOfTheSpeed (0x50A002 y 0x50A019) y despues de las dos llamadas lee EDX (`mov eax, dword ptr [edx]`
// en 0x50A052: es el puntero a `up`, que el EXE guarda en EDX antes de WrapAngle y WrapAngle no toca): un hook normal dejaria EDX libre para el compilador y, con el hook de
// KeepTrackOfTheSpeed desactivado, el EXE desreferenciaria basura. Esta envoltura recibe el argumento cdecl y devuelve EDX de entrada intacto sin ensamblador: lo primero que
// ejecuta es CarEnterExitEntryEdxCpp (EDX de entrada en la mitad alta del resultado de 64 bits) y lo ultimo ResidueRegsCpp (EDX = el de entrada, ECX = el puntero, como el EXE, que
// no lo modifica; EAX no lo lee ningun llamador: 0x50A007 y 0x50A01E lo sobrescriben).
#pragma runtime_checks("", off)
static uint32 __declspec(safebuffers) __cdecl WrapAngleKeepEdx(float* angle) {
    const uint32 entryEdx = uint32(CarEnterExitEntryEdxCpp() >> 32);
    CCam::WrapAngle(angle);
    return ResidueRegsCpp(uint32(reinterpret_cast<uintptr_t>(angle)), entryEdx, 0);
}
#pragma runtime_checks("", restore)

void CCam::InjectHooks() {
    RH_ScopedClass(CCam);
    RH_ScopedCategory("Camera");
    RH_ScopedNamedGlobalInstall(WrapAngleKeepEdx, "WrapAngle", 0x509BE0);
    RH_ScopedGlobalInstall(CameraPathInterpolateVector, 0x5B2090);
    RH_ScopedGlobalInstall(CameraPathInterpolateScalar, 0x5B2330);

    RH_ScopedInstall(Constructor, 0x517730);
    RH_ScopedInstall(Init, 0x50E490);
    RH_ScopedInstall(CacheLastSettingsDWCineyCam, 0x50D7A0);
    RH_ScopedInstall(DoCamBump, 0x50CB30);
    RH_ScopedInstall(Finalise_DW_CineyCams, 0x50DD70);
    RH_ScopedInstall(GetCoreDataForDWCineyCamMode, 0x517130);
    RH_ScopedInstall(GetLookFromLampPostPos, 0x5161A0);
    RH_ScopedInstall(GetVectorsReadyForRW, 0x509CE0);
    RH_ScopedInstall(Get_TwoPlayer_AimVector, 0x513E40);
    RH_ScopedInstall(IsTimeToExitThisDWCineyCamMode, 0x517400);
    RH_ScopedInstall(KeepTrackOfTheSpeed, 0x509DF0);
    RH_ScopedInstall(LookBehind, 0x520690);
    RH_ScopedInstall(LookRight, 0x520E40);
    RH_ScopedInstall(RotCamIfInFrontCar, 0x50A4F0);
    RH_ScopedInstall(Using3rdPersonMouseCam, 0x50A850);
    RH_ScopedInstall(GetBoatHeightForFirstPerson, 0x509CA0);
    RH_ScopedInstall(Process, 0x526FC0);
    RH_ScopedInstall(ProcessArrestCamOne, 0x518500);
    RH_ScopedInstall(ProcessPedsDeadBaby, 0x519250);
    RH_ScopedInstall(Process_1rstPersonPedOnPC, 0x50EB70);
    RH_ScopedInstall(Process_1stPerson, 0x517EA0);
    RH_ScopedInstall(Process_AimWeapon, 0x521500);
    RH_ScopedInstall(Process_AttachedCam, 0x512B10);
    RH_ScopedInstall(Process_Cam_TwoPlayer, 0x525E50);
    RH_ScopedInstall(Process_Cam_TwoPlayer_InCarAndShooting, 0x519810);
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars, 0x513510);
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars_TopDown, 0x513BE0);
    RH_ScopedInstall(Process_DW_BirdyCam, 0x51B850);
    RH_ScopedInstall(Process_DW_CamManCam, 0x51B120);
    RH_ScopedInstall(Process_DW_HeliChaseCam, 0x51A740);
    RH_ScopedInstall(Process_DW_PlaneCam1, 0x51C760);
    RH_ScopedInstall(Process_DW_PlaneCam2, 0x51CC30);
    RH_ScopedInstall(Process_DW_PlaneCam3, 0x51D100);
    RH_ScopedInstall(Process_DW_PlaneSpotterCam, 0x51C250);
    RH_ScopedInstall(Process_Editor, 0x50F3F0);
    RH_ScopedInstall(Process_Fixed, 0x51D470);
    RH_ScopedInstall(Process_FlyBy, 0x5B25F0);
    RH_ScopedInstall(Process_FollowCar_SA, 0x5245B0);
    RH_ScopedInstall(Process_FollowPedWithMouse, 0x50F970);
    RH_ScopedInstall(Process_FollowPed_SA, 0x522D40);
    RH_ScopedInstall(Process_M16_1stPerson, 0x5105C0);
    RH_ScopedInstall(Process_Rocket, 0x511B50);
    RH_ScopedInstall(Process_SpecialFixedForSyphon, 0x517500);
    RH_ScopedInstall(Process_WheelCam, 0x512110);

    RH_ScopedGlobalInstall(WellBufferMe, 0x509AE0);
}

// 0x517730
CCam::CCam() {
    Init();
}

// 0x50E490
void CCam::Init() {
    // Como el original (0x50E49E..0x50E4A4): primero reinicia la camara de inactividad (CIdleCam::Reset(true) sobre 0xB6FDA0).
    gIdleCam.Reset(true);
    m_vecFront = CVector(0, 0, -1);
    m_vecUp = CVector(0, 0, 1);
    m_nMode = eCamMode::MODE_FOLLOWPED;
    m_bRotating = false;
    m_nDoCollisionChecksOnFrameNum = 1;
    m_nDoCollisionCheckEveryNumOfFrames = 9;
    m_nFrameNumWereAt = 0;
    m_bCollisionChecksOn = true;
    m_fRealGroundDist = 0.0f;
    m_fBetaSpeed = 0.0f;
    m_fAlphaSpeed = 0.0f;
    m_fCameraHeightMultiplier = 0.75;
    m_fMaxRoleAngle = std::bit_cast<float>(0x3D0EFA35u); // 0x50E547: 2 grados en radianes (no 20)
    m_fDistance = 30.0f;
    m_fDistanceSpeed = 0.0f;
    m_pLastCarEntered = nullptr;
    m_pLastPedLookedAt = nullptr;
    m_bResetStatics = true;
    m_fHorizontalAngle = 0.0f;
    m_fTilt = 0.0f;
    m_fTiltSpeed = 0.0f;
    m_bFixingBeta = false;
    m_fCaMinDistance = 0.0f;
    m_fCaMaxDistance = 0.0f;
    m_bLookingBehind = false;
    m_bLookingLeft = false;
    m_bLookingRight = false;
    m_fPlayerInFrontSyphonAngleOffSet = std::bit_cast<float>(0x3EB2B8C3u); // 0x50E597: 20 grados leidos como inmediato
    m_fSyphonModeTargetZOffSet = 0.5f;
    m_fRadiusForDead = 1.5f;
    m_nDirectionWasLooking = 3; // TODO: enum
    m_bLookBehindCamWasInFront = 0;
    m_fRoll = 0.0f;
    m_fRollSpeed = 0.0f;
    m_fCloseInPedHeightOffset = 0.0f;
    m_fCloseInPedHeightOffsetSpeed = 0.0f;
    m_fCloseInCarHeightOffset = 0.0f;
    m_fCloseInCarHeightOffsetSpeed = 0.0f;
    m_fPedBetweenCameraHeightOffset = 0.0f;
    m_fTargetBeta = 0.0f;
    m_fBufferedTargetBeta = 0.0f;
    m_fBufferedTargetOrientation = 0.0f;
    m_fBufferedTargetOrientationSpeed = 0.0f;
    m_fDimensionOfHighestNearCar = 0.0;
    m_fBeta_Targeting = 0.0f;
    m_fX_Targetting = 0.0f;
    m_fY_Targetting = 0.0f;
    m_pCarWeAreFocussingOn = nullptr;
    m_pCarWeAreFocussingOnI = nullptr;
    m_fCamBumpedHorz = 1.0f;
    m_fCamBumpedVert = 0.0f;
    m_nCamBumpedTime = 0;
    for (int i = 0; i < 4; ++i) {
        m_anTargetHistoryTime[i] = 0;
        m_avecTargetHistoryPos[i] = CVector{};
    }
    m_nCurrentHistoryPoints = 0;
    gPlayerPedVisible = true;
    gbCineyCamMessageDisplayed = 2; // TODO: enum
    gCameraDirection = 3; // TODO: enum
    *reinterpret_cast<uint32*>(&gCameraMode) = 0xFFFFFFFFu; // 0x50E672: 'mov dword ptr [8CC388h],0FFFFFFFFh' (escribe un dword, no la palabra de eCamMode)
    gLastTime2PlayerCameraWasOK = 0;
    gLastTime2PlayerCameraCollided = 0;
    TheCamera.m_bCinemaCamera = false;
}

// 0x50D7A0
void CCam::CacheLastSettingsDWCineyCam() {
    DWCineyCamLastUp       = m_vecUp;
    DWCineyCamLastFwd      = m_vecFront;
    DWCineyCamLastRight    = CrossProduct(m_vecFront, m_vecUp);
    DWCineyCamLastFov      = m_fFOV;
    DWCineyCamLastNearClip = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    DWCineyCamLastPos      = m_vecSource;
}

// 0x50CB30
void CCam::DoCamBump(float horizontal, float vertical) {
    m_fCamBumpedHorz = horizontal;
    m_fCamBumpedVert = vertical;
    m_nCamBumpedTime = CTimer::GetTimeInMS();
}

// 0x50DD70
void CCam::Finalise_DW_CineyCams(const CVector& src, const CVector& dest, float roll, float fov, float nearClip, float shakeDegree) {
    m_vecFront  = (dest - src).Normalized();
    m_vecSource = src;

    // Como el original: el vector "arriba" provisional (sin(roll), 0, cos(roll)) se escribe en m_vecUp (fsin/fcos del x87).
    {
        m_vecUp = CVector{ CamExeSin(roll), 0.0f, CamExeCos(roll) };
        auto rightDir = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecUp       = CrossProduct(rightDir, m_vecFront);
        if (m_vecFront.x == 0.0f && m_vecFront.y == 0.0f) {
            m_vecFront.x = m_vecFront.y = 0.0001f;
        }
        rightDir = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecUp  = CrossProduct(rightDir, m_vecFront);
    }

    m_fFOV = fov;
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.4f); // meant to use nearClip here?
    CacheLastSettingsDWCineyCam();
    gLastFrameProcessedDWCineyCam = CTimer::GetFrameCounter();

    gHandShaker[0].Process(shakeDegree);
    // Como el original (0x50DF0C..0x50DF21): el roll del segundo bloque pasa a ser m_ang.z * shakeDegree (fld 0xB6ECA8; fmul [esp+4Ch]; fstp [roll]).
    roll = gHandShaker[0].m_ang.z * shakeDegree;
    // Como el original (0x50DF25): Multiply3x3 vector * matriz (0x59C810), no la transformacion matriz * vector.
    m_vecFront = CamMultiplyVectorMatrix(m_vecFront, gHandShaker[0].m_resultMat);
    m_vecFront.Normalise();

    {
        m_vecUp = CVector{ CamExeSin(roll), 0.0f, CamExeCos(roll) };
        auto rightDir = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecUp       = CrossProduct(rightDir, m_vecFront);
        if (m_vecFront.x == 0.0f && m_vecFront.y == 0.0f) {
            m_vecFront.x = m_vecFront.y = 0.0001f;
        }
        rightDir = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecUp  = CrossProduct(rightDir, m_vecFront);
    }
}

// 0x517130
void CCam::GetCoreDataForDWCineyCamMode(
    CEntity*& entity,
    CVehicle*& vehicle,
    CVector& dest,
    CVector& src,
    CVector& targetUp,
    CVector& targetRight,
    CVector& targetFwd,
    CVector& targetVel,
    float& targetSpeed,
    CVector& targetAngVel,
    float& targetAngSpeed,
    CColSphere& colSphere
) {
    entity         = m_pCamTargetEntity;
    vehicle        = entity->AsVehicle();
    dest           = entity->GetPosition();
    src            = DWCineyCamLastPos;
    // Como el original (0x517187..0x517255): si la entidad no tiene matriz, los ejes salen del rumbo con fsin/fcos del x87.
    if (const auto* matrix = entity->m_matrix) {
        targetUp    = matrix->GetUp();
        targetRight = matrix->GetRight();
        targetFwd   = matrix->GetForward();
    } else {
        const float heading = entity->m_placement.m_fHeading;
        targetUp    = CVector{ 0.0f, 0.0f, 1.0f };
        targetRight = CVector{ CamExeCos(heading), CamExeSin(heading), 0.0f };
        targetFwd   = CVector{ -CamExeSin(heading), CamExeCos(heading), 0.0f };
    }
    targetVel      = entity->AsPhysical()->GetMoveSpeed();
    targetSpeed    = targetVel.Magnitude();
    targetAngVel   = entity->AsPhysical()->GetTurnSpeed();
    targetAngSpeed = targetAngVel.Magnitude();

    colSphere.Set(
        entity->GetModelInfo()->GetColModel()->GetBoundRadius(),
        entity->GetBoundCentre(),
        eSurfaceType::SURFACE_DEFAULT
    );
}

// 0x5161A0
bool CCam::GetLookFromLampPostPos(CEntity* target, CPed* cop, const CVector& vecTarget, CVector& vecSource) {
    int16 count = 0;
    CEntity* entities[16];
    CWorld::FindObjectsInRange(vecTarget, 30.0f, true, &count, 15, entities, false, false, false, true, true);
    float bestScore = 10000.0f;
    bool found = false;
    for (int16 i = 0; i < count; ++i) {
        auto* entity = entities[i];
        const auto flags = *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(entity) + 0x1C);
        if (!(flags & (4u | 0x40000u))) continue;
        if (!(entity->GetMatrix().GetUp().z > 0.9f) || !IsLampPost(eModelID(entity->m_nModelIndex))) continue;
        const auto delta = entity->GetPosition() - vecTarget;
        const auto distance = std::sqrt(delta.y * delta.y + delta.x * delta.x);
        const auto score = std::abs(StaticRef<float>(0x8CC8D8) - distance);
        if (!(distance > 5.0f) || !(score < bestScore)) continue;
        const auto localTop = entity->GetColModel()->m_boundBox.m_vecMax;
        const auto lampTop = entity->GetMatrix().TransformPoint(localTop);
        auto direction = lampTop - vecTarget;
        direction.Normalise();
        const auto checkEnd = direction + vecTarget;
        if (!CWorld::GetIsLineOfSightClear(lampTop, checkEnd, true, false, false, false, false, true, true)) continue;
        vecSource = lampTop;
        bestScore = score;
        found = true;
    }
    return found;
}

// 0x509CE0
void CCam::GetVectorsReadyForRW() {
    m_vecFront.Normalise();
    if (m_vecFront.x == 0.0f && m_vecFront.y == 0.0f) {
        m_vecFront.x = m_vecFront.y = 0.0001f;
    }
    const auto a = CrossProduct(m_vecFront, { 0.0f, 0.0f, 1.0f }).Normalized();
    m_vecUp = CrossProduct(a, m_vecFront);
}

// 0x513E40
CEntity* CCam::Get_TwoPlayer_AimVector(CVector& out) {
    // Como el original (0x513E47..0x513E61): jugador 1, o el 2 si el 1 va en un vehiculo del que no es conductor.
    CPlayerPed* player = CWorld::Players[0].m_pPed;
    if (player->m_pVehicle && player->m_pVehicle->m_pDriver != player) {
        player = CWorld::Players[1].m_pPed;
    }

    const auto* weaponInfo = CWeaponInfo::GetWeaponInfo(player->GetActiveWeapon().m_Type, player->GetWeaponSkill()); // 0x513E7A..0x513E81
    const auto nearestTargetEntityInScreen = CWeapon::FindNearestTargetEntityWithScreenCoors(
        m_fX_Targetting,
        m_fY_Targetting,
        weaponInfo->m_fWeaponRange + weaponInfo->m_fWeaponRange, // fadd st(0),st (0x513E9F)
        player->GetPosition()
    );

    if (nearestTargetEntityInScreen) {
        const auto& pos = nearestTargetEntityInScreen->GetPosition();
        out = CVector{ pos.x - m_vecSource.x, pos.y - m_vecSource.y, pos.z - m_vecSource.z };
    } else {
        // Como el original (0x513F10..0x513FD9): tan(fov * 0x8631D4) con fptan (x87). La tangente queda en precision
        // extendida para dividirla por la relacion de aspecto y se redondea a float para el otro termino.
        const auto  right    = CrossProduct(m_vecFront, m_vecUp);
        const float fov      = m_fFOV;
        const float fovScale = CamExeFloat(0x8631D4);
        const float aspect   = CDraw::ms_fAspectRatio;
        float tanRounded, tanOverAspect;
        CamTwoPlayerAimTanCpp(fov, fovScale, aspect, &tanOverAspect, &tanRounded);
        const float scaledY = tanOverAspect * m_fY_Targetting;
        const float upX = scaledY * m_vecUp.x, upY = scaledY * m_vecUp.y, upZ = scaledY * m_vecUp.z;
        const float rightX = (right.x * m_fX_Targetting) * tanRounded;
        const float rightY = (right.y * m_fX_Targetting) * tanRounded;
        const float rightZ = (right.z * m_fX_Targetting) * tanRounded;
        out = CVector{ (rightX + m_vecFront.x) - upX, (rightY + m_vecFront.y) - upY, (rightZ + m_vecFront.z) - upZ };
    }
    out.Normalise();
    return nearestTargetEntityInScreen;
}

// 0x517400
bool CCam::IsTimeToExitThisDWCineyCamMode(int32 camId, const CVector& src, const CVector& dst, float t, bool lineOfSightCheck) {
    if (StaticRef<uint8>(0xB6EC5C + camId)) return true;
    const auto delta = dst - src;
    const auto distance = std::sqrt((delta.z * delta.z + delta.x * delta.x) + delta.y * delta.y);
    const auto withinDistance = distance >= StaticRef<float>(0x8CCBCC + (camId - 20) * 4) && distance <= StaticRef<float>(0x8CCBF0 + (camId - 20) * 4);
    bool clear = true;
    if (lineOfSightCheck) {
        CColPoint colPoint;
        CEntity* entity;
        CWorld::pIgnoreEntity = m_pCamTargetEntity;
        clear = !CWorld::ProcessLineOfSight(dst, src, colPoint, entity, true, true, false, false, false, false, false, false);
        CWorld::pIgnoreEntity = nullptr;
    }
    return camId >= 20 && camId <= 28 && (!withinDistance || !clear || CTimer::GetTimeInMS() > StaticRef<uint32>(0x8CCBA4));
}

// 0x509DF0
void CCam::KeepTrackOfTheSpeed(const CVector& source, const CVector& target, const CVector& up, const float& alpha, const float& beta, const float& fov) {
    auto& flags = StaticRef<uint32>(0xB6FF8C);
    auto& oldSource = StaticRef<CVector>(0xB6FF80);
    auto& oldTarget = StaticRef<CVector>(0xB6FF74);
    auto& oldUp = StaticRef<CVector>(0xB6FF68);
    auto& oldBeta = StaticRef<float>(0xB6FF64);
    auto& oldAlpha = StaticRef<float>(0xB6FF60);
    auto& oldFov = StaticRef<float>(0xB6FF5C);
    if (!(flags & 1)) { oldSource = source; flags |= 1; }
    if (!(flags & 2)) { oldTarget = target; flags |= 2; }
    if (!(flags & 4)) { oldUp = up; flags |= 4; }
    const float previousBeta = flags & 8 ? oldBeta : beta;
    flags |= 8;
    if (!(flags & 16)) { oldAlpha = alpha; flags |= 16; }
    const float previousFov = flags & 32 ? oldFov : fov;
    flags |= 32;
    if (StaticRef<bool>(0xB6F052)) {
        oldSource = source;
        oldTarget = target;
        oldUp = up;
    }
    m_vecSourceSpeedOverOneFrame = source - oldSource;
    m_vecTargetSpeedOverOneFrame = target - oldTarget;
    m_vecUpOverOneFrame = up - oldUp;
    m_fFovSpeedOverOneFrame = fov - previousFov;
    m_fBetaSpeedOverOneFrame = beta - previousBeta;
    WrapAngle(&m_fBetaSpeedOverOneFrame);
    m_fAlphaSpeedOverOneFrame = alpha - oldAlpha;
    WrapAngle(&m_fAlphaSpeedOverOneFrame);
    oldSource = source;
    oldTarget = target;
    oldUp = up;
    oldBeta = beta;
    oldAlpha = alpha;
    oldFov = fov;
}

// 0x520690
bool CCam::LookBehind() {
    auto* entity=m_pCamTargetEntity;const auto type=entity->GetType();
    const bool thirdVehicle=(m_nMode==static_cast<eCamMode>(0x12)||m_nMode==static_cast<eCamMode>(0x16)||m_nMode==static_cast<eCamMode>(3))&&type==ENTITY_TYPE_VEHICLE;
    const bool firstVehicle=m_nMode==static_cast<eCamMode>(0x10)&&type==ENTITY_TYPE_VEHICLE;
    if(!thirdVehicle&&!firstVehicle&&type!=ENTITY_TYPE_PED)return false;
    CVector dest=entity->GetPosition();m_vecFront=dest-m_vecSource;
    if(thirdVehicle){
        dest=StaticRef<CVector>(0xB6F018);m_bLookingBehind=true;
        const float distance=m_nMode==static_cast<eCamMode>(0x12)?*reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(this)+0xD0):15.5f;
        const auto front=entity->GetMatrix().GetForward();const float z=front.z+.2f;
        m_vecSource=CVector{dest.x+distance*front.x,dest.y+distance*front.y,z*distance+dest.z};
        CWorld::pIgnoreEntity=entity;StaticRef<int32>(0xB6FC70)=0;
        TheCamera.CameraVehicleModeSpecialCases(static_cast<CVehicle*>(entity));
        TheCamera.CameraColDetAndReact(&m_vecSource,&dest);
        m_vecFront=entity->GetPosition()-m_vecSource;GetVectorsReadyForRW();
        TheCamera.ImproveNearClip(static_cast<CVehicle*>(entity),nullptr,&m_vecSource,&dest);CWorld::pIgnoreEntity=nullptr;
    }
    if(firstVehicle){
        m_bLookingBehind=true;RwCameraSetNearClipPlane(StaticRef<RwCamera*>(0xC1703C),.05f);
        m_vecFront=entity->GetMatrix().GetForward();m_vecFront.Normalise();auto* vehicle=static_cast<CVehicle*>(entity);
        if(vehicle->m_nVehicleType==VEHICLE_TYPE_BOAT)m_vecSource.z-=.5f;
        if(int32(vehicle->GetVehicleAppearance())==2){
            m_vecSource=CVector{m_vecFront.x*2.3f+m_vecSource.x,m_vecFront.y*2.3f+m_vecSource.y,m_vecFront.z*2.3f+m_vecSource.z};m_vecFront=-m_vecFront;GetVectorsReadyForRW();
        }else if(int32(vehicle->GetVehicleAppearance())==3){
            m_vecFront=-entity->GetMatrix().GetUp();m_vecUp=entity->GetMatrix().GetForward();m_vecSource=CVector{m_vecFront.x*.25f+m_vecSource.x,m_vecFront.y*.25f+m_vecSource.y,m_vecFront.z*.25f+m_vecSource.z};
        }else{m_vecSource=CVector{m_vecFront.x*.25f+m_vecSource.x,m_vecFront.y*.25f+m_vecSource.y,m_vecFront.z*.25f+m_vecSource.z};m_vecFront=-m_vecFront;}
    }
    if(type==ENTITY_TYPE_PED){
        const float heading=m_fHorizontalAngle;float x,y;
        CamLookBehindPedSideCpp(heading, &x, &y);
        const auto* raw=reinterpret_cast<const uint8*>(entity);
        m_vecSource=CVector{x,y,0};
        m_vecSource.z=(m_vecSource.z-((*reinterpret_cast<const float*>(raw+0x578)*m_vecSource.x+*reinterpret_cast<const float*>(raw+0x57C)*m_vecSource.y)+*reinterpret_cast<const float*>(raw+0x580)*m_vecSource.z))+.3f;
        m_vecSource.Normalise();float distance=StaticRef<float>(0x8CCE4C)+StaticRef<float>(0xB6F0FC);if(distance<.6f)distance=.6f;
        m_vecSource=CVector{m_vecSource.x*distance+dest.x,m_vecSource.y*distance+dest.y,m_vecSource.z*distance+dest.z};
        const int32 preset=StaticRef<int32>(0xB6F0F0);const float sourceZ=StaticRef<float>(0x8CCE3C+preset*4),targetZ=StaticRef<float>(0x8CCE30+preset*4);auto* ped=static_cast<CPed*>(entity);
        if(ped->GetIntelligence()->GetTaskSwim()){
            const float scale=StaticRef<float>(0x8CCE24+preset*4),offset=StaticRef<float>(0x8CCE18+preset*4);
            m_vecSource=CVector{(dest.x-m_vecSource.x)*scale+dest.x,(dest.y-m_vecSource.y)*scale+dest.y,(dest.z-m_vecSource.z)*scale+dest.z};m_vecSource.z=offset+m_vecSource.z;
        }
        m_vecSource.z=sourceZ+m_vecSource.z;dest.z+=targetZ;
        TheCamera.HandleCameraMotionForDucking(ped,&m_vecSource,&dest,false);StaticRef<int32>(0xB6FC70)=0;
        if(m_pCamTargetEntity){if(auto* task=ped->GetIntelligence()->GetTaskHold(false)){if(auto* held=*reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(task)+8)){StaticRef<CEntity*>(0xB6FC74)=held;++StaticRef<int32>(0xB6FC70);}}}
        StaticRef<uint8>(0x8A5B14)=1;StaticRef<uint8>(0x8A5B15)=1;StaticRef<uint8>(0x8A5B17)=1;
        TheCamera.CameraColDetAndReact(&m_vecSource,&dest);m_vecFront=dest-m_vecSource;GetVectorsReadyForRW();TheCamera.ImproveNearClip(nullptr,ped,&m_vecSource,&dest);
        if(StaticRef<int32>(0xB6F0F0)==1&&StaticRef<RwCamera*>(0xC1703C)->nearPlane>.05f)RwCameraSetNearClipPlane(StaticRef<RwCamera*>(0xC1703C),.05f);
    }
    GetVectorsReadyForRW();return true;
}

// 0x520E40
static void LookSideOrbit(float directionX,float directionY,float sign,float distance,float targetX,float targetY,float& outputX,float& outputY) {
    const uintptr_t atanFunction=std::bit_cast<uintptr_t>(&CGeneral::GetATanOfXY);const float halfPi=std::bit_cast<float>(0x3FC90FDBu);float outX,outY;
    CamLookSideOrbitCpp(directionY, directionX, CamAddr(atanFunction), sign, halfPi, distance, targetX, &outX, targetY, &outY);
    outputX=outX;outputY=outY;
}

bool CCam::LookRight(bool lookRight) {
    auto* entity=m_pCamTargetEntity;const auto type=entity->GetType();const bool thirdVehicle=(m_nMode==static_cast<eCamMode>(0x12)||m_nMode==static_cast<eCamMode>(0x16)||m_nMode==static_cast<eCamMode>(3))&&type==ENTITY_TYPE_VEHICLE;
    const bool firstVehicle=m_nMode==static_cast<eCamMode>(0x10)&&type==ENTITY_TYPE_VEHICLE;
    if(lookRight)m_bLookingRight=true;else m_bLookingLeft=true;
    if(!thirdVehicle){
        if(firstVehicle){
            auto* vehicle=static_cast<CVehicle*>(entity);RwCameraSetNearClipPlane(StaticRef<RwCamera*>(0xC1703C),StaticRef<float>(0x8CCE50));
            if(vehicle->m_nVehicleType==VEHICLE_TYPE_BOAT){
                if(!vehicle->m_pDriver)m_vecSource.z-=.5f;
                else{
                    auto* driver=vehicle->m_pDriver;CVector head{0,0,0};driver->SetPedPositionInCar();driver->UpdateRwMatrix();driver->UpdateRwFrame();driver->UpdateRpHAnim();driver->GetBonePosition(&head,static_cast<eBoneTag>(4),true);
                    const auto& matrix=vehicle->GetMatrix();const float side=StaticRef<float>(lookRight?0x8CC498:0x8CC49C);head+=matrix.GetRight()*side;
                    const float up=StaticRef<float>(0x8CC494);m_vecSource=CVector{matrix.GetUp().x*up+head.x,matrix.GetUp().y*up+head.y,matrix.GetUp().z*up+head.z};
                }
            }
            if(vehicle->m_nVehicleType!=VEHICLE_TYPE_BIKE){const auto right=vehicle->GetMatrix().GetRight();m_vecSource=CVector{m_vecSource.x-right.x*.35f,m_vecSource.y-right.y*.35f,m_vecSource.z-right.z*.35f};}
            m_vecUp=vehicle->GetMatrix().GetUp();m_vecUp.Normalise();m_vecFront=vehicle->GetMatrix().GetForward();m_vecFront.Normalise();
            m_vecFront=lookRight?CrossProduct(m_vecFront,m_vecUp):CrossProduct(m_vecUp,m_vecFront);m_vecFront.Normalise();
            if(int32(vehicle->GetVehicleAppearance())==2)m_vecSource=CVector{m_vecSource.x-m_vecFront.x*1.45f,m_vecSource.y-m_vecFront.y*1.45f,m_vecSource.z-m_vecFront.z*1.45f};
        }else if(type!=ENTITY_TYPE_PED)return false;
        return true;
    }
    CVector target=entity->GetPosition();float distance=9.0f;
    if(m_nMode==static_cast<eCamMode>(0x12))distance=*reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(this)+0xD0);
    else if(m_nMode==static_cast<eCamMode>(0x16)){float height=0;if(GetBoatHeightForFirstPerson(&height)&&!CCullZones::Cam1stPersonForPlayer())m_vecSource.z=target.z+height;}
    auto direction=entity->GetMatrix().GetForward();direction.Normalise();
    LookSideOrbit(direction.x,direction.y,lookRight?1.0f:-1.0f,distance,target.x,target.y,m_vecSource.x,m_vecSource.y);
    auto* col=entity->GetColModel();const float sourceZ=m_vecSource.z;
    CWorld::pIgnoreEntity=entity;StaticRef<int32>(0xB6FC70)=0;CVector collisionTarget=target;
    TheCamera.CameraVehicleModeSpecialCases(static_cast<CVehicle*>(entity));TheCamera.CameraColDetAndReact(&m_vecSource,&collisionTarget);CWorld::pIgnoreEntity=nullptr;
    target=entity->GetPosition();const auto& matrix=entity->GetMatrix();
    float height=(lookRight?col->m_boundBox.m_vecMin.x:col->m_boundBox.m_vecMax.x)*matrix.GetRight().z+target.z;
    height=col->m_boundBox.m_vecMax.z*matrix.GetUp().z+height;
    const float floor=*reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(this)+0x150);
    float maximum=height<floor?floor:height;float adjusted=sourceZ;
    if(maximum+.1f<=sourceZ){if(height<floor)height=floor;adjusted=height+.1f;}
    if(adjusted<=m_vecSource.z)adjusted=m_vecSource.z;m_vecSource.z=adjusted;
    m_vecFront=entity->GetPosition()-m_vecSource;m_vecFront.z+=1.1f;if(m_nMode==static_cast<eCamMode>(0x16))m_vecFront.z+=1.2f;
    GetVectorsReadyForRW();return true;
}

// 0x50A4F0
bool CCam::RotCamIfInFrontCar(const CVector& target, float orientation) {
    auto* entity=m_pCamTargetEntity;
    if(!entity->GetIsTypeVehicle())return false;
    auto* vehicle=static_cast<CVehicle*>(entity);const auto& matrix=entity->GetMatrix();const auto& velocity=vehicle->m_vecMoveSpeed;
    if(velocity.y*velocity.y+velocity.x*velocity.x>std::bit_cast<float>(0x3B6BEDFAu)){
        const float negativeX=-velocity.x,velocityY=velocity.y,halfPi=std::bit_cast<float>(0x3FC90FDBu);
        orientation = Atan2MinusCpp(negativeX, velocityY, halfPi);
    }
    const auto delta=m_vecSource-target;const float distance=std::sqrt(delta.y*delta.y+delta.x*delta.x);
    const auto normalise=[](float v){const float pi=std::bit_cast<float>(0x40490FDBu),twoPi=std::bit_cast<float>(0x40C90FDBu);while(v>pi)v-=twoPi;while(v<-pi)v+=twoPi;return v;};
    const float difference=normalise(orientation-m_fHorizontalAngle);
    const float forwardSpeed=(matrix.GetForward().x*velocity.x+matrix.GetForward().y*velocity.y)+matrix.GetForward().z*velocity.z;
    if(std::abs(difference)>std::bit_cast<float>(0x3EB2B8C0u)&&forwardSpeed>0.1f&&!StaticRef<uint8>(0xB6F080))m_bFixingBeta=true;
    if(!CPad::GetPad(0)->GetLookBehindForCar()&&!CPad::GetPad(0)->GetLookBehindForPed()&&!CPad::GetPad(0)->GetLookLeft()&&!CPad::GetPad(0)->GetLookRight()&&m_nDirectionWasLooking!=3)StaticRef<uint8>(0xB6F042)=1;
    const auto back=StaticRef<uint8>(0xB6F05F),front=StaticRef<uint8>(0xB6F042),rear=StaticRef<uint8>(0xB6F043);
    if(!m_bFixingBeta&&!back&&!front&&!rear)return false;
    const bool active=uintptr_t(this)==0xB6F19Cu+uint32(StaticRef<uint8>(0xB6F081))*sizeof(CCam);
    const bool force=(front==1||rear==1||back==1)&&active;
    if(m_bFixingBeta||force){
        WellBufferMe(orientation,m_fHorizontalAngle,m_fBetaSpeed,0.1f,std::bit_cast<float>(0x3B449BA6u),true);
        if(front==1&&active)m_fHorizontalAngle=orientation;
        if(rear==1&&active)m_fHorizontalAngle=orientation+std::bit_cast<float>(0x40490FDBu);
        if(back&&active)m_fHorizontalAngle=*reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(this)+0x9C);
        const float angle=m_fHorizontalAngle,baseX=target.x,baseY=target.y;float resultX,resultY;
        CamRotInFrontCarOrbitCpp(angle, distance, baseX, &resultX, baseY, &resultY);
        m_vecSource.x=resultX;m_vecSource.y=resultY;
        if(std::abs(normalise(orientation-m_fHorizontalAngle))<std::bit_cast<float>(0x3D0EFA35u))m_bFixingBeta=false;
    }
    StaticRef<uint8>(0xB6F042)=0;StaticRef<uint8>(0xB6F043)=0;return true;
}

// 0x50A850
bool CCam::Using3rdPersonMouseCam() const {
    return CCamera::m_bUseMouse3rdPerson && m_nMode == MODE_FOLLOWPED;
}

// 0x509DC0
bool CCam::GetWeaponFirstPersonOn() {
    return m_pCamTargetEntity && m_pCamTargetEntity->GetIsTypePed() && m_pCamTargetEntity->AsPed()->GetActiveWeapon().m_IsFirstPersonWeaponModeSelected;
}

// inlined -- alpha = vertical angle
void CCam::ClipAlpha() {
    // Como el original en linea (p. ej. 0x511DB5..0x511E27): maximo 0x8630F8 (60 grados), minimo 0x8630F4 (-1.5620698)
    if (m_fVerticalAngle > CamExeFloat(0x8630F8)) {
        m_fVerticalAngle = std::bit_cast<float>(0x3F860A92u);
    } else if (m_fVerticalAngle < CamExeFloat(0x8630F4)) {
        m_fVerticalAngle = std::bit_cast<float>(0xBFC7F1E7u);
    }
}

// 0x509C50 -- beta = horizontal angle
void CCam::ClipBeta() {
    // Como el original (0x509C50..0x509C95): si beta > pi resta 2pi; si no, si beta < -pi suma 2pi (constantes 0x858CB8/BC/C0).
    if (m_fHorizontalAngle > CamExeFloat(0x858CB8)) {
        m_fHorizontalAngle -= CamExeFloat(0x858CBC);
    } else if (m_fHorizontalAngle < CamExeFloat(0x858CC0)) {
        m_fHorizontalAngle += CamExeFloat(0x858CBC);
    }
}

namespace {
float CamPower(float,float);
static float CamVelocityInverseStep(float step) {
    const float denominator=std::max(1.f,step), one=1.f;
    float reciprocal;
    CamVelocityInverseStepCpp(one, denominator, &reciprocal);
    return reciprocal;
}

static float CamVehicleTargetSpeed(float forwardX,float forwardY,float velocityX,float velocityY) {
    const float zero=0.f,one=1.f,half=.5f,positiveScale=StaticRef<float>(0x86325C),negativeScale=StaticRef<float>(0x863258);
    float result;
    CamVehicleTargetSpeedCpp(&forwardY, &forwardX, zero, one, velocityX, velocityY, positiveScale, negativeScale, half, &result);
    return result;
}
static float CamVehicleSmoothedSpeed(float previous,float target) {
    const float previousWeight=StaticRef<float>(0x863250),targetWeight=StaticRef<float>(0x863254);
    float result;
    CamVehicleSmoothedSpeedCpp(target, targetWeight, previous, previousWeight, &result);
    return result;
}

float CamPowerComplement(float base,float step) {
    const float one=StaticRef<float>(0x858624);
    float result;
    CamPowerComplementCpp(base, step, one, &result);
    return result;
}
}
// 0x526FC0
void CCam::Process() {
    const auto f=[](uintptr_t address){return StaticRef<float>(address);};
    const uintptr_t activeAddress=0xB6F19C+uintptr_t(StaticRef<uint8>(0xB6F081))*sizeof(CCam);
    if (float(StaticRef<int32>(0xB6FE34))<=f(0xB6FDC8)) StaticRef<uint8>(0xC0B184)&=0xFE;
    if (StaticRef<int16>(activeAddress+0xC)!=4) { StaticRef<uint8>(0xC0B184)&=0xFE;StaticRef<int32>(0xB6FE34)=0; }
    if (!m_pCamTargetEntity) { m_pCamTargetEntity=StaticRef<CEntity*>(0xB6F980);m_pCamTargetEntity->RegisterReference(&m_pCamTargetEntity); }
    if (StaticRef<uint8>(0xC8A860)==1) {
        auto* player=FindPlayerPed(-1);
        if (!player || !player->m_pVehicle || player->m_pVehicle->GetModelIndex()!=520) CWeaponEffects::ClearCrossHairImmediately(0);
    }
    if (int32(++m_nFrameNumWereAt)>int32(m_nDoCollisionCheckEveryNumOfFrames)) m_nFrameNumWereAt=1;
    m_bCollisionChecksOn=m_nFrameNumWereAt==m_nDoCollisionChecksOnFrameNum;
    float targetSpeed=0.f,orientation=0.f;
    CVector target;
    if (!m_bCamLookingAtVector) {
        if (m_pCamTargetEntity->GetIsTypeVehicle()) {
            auto* vehicle=static_cast<CVehicle*>(m_pCamTargetEntity);
            target=vehicle->GetPosition();
            if (vehicle->GetMatrix().GetForward().x!=0.f || vehicle->GetMatrix().GetForward().y!=0.f)
                orientation=CGeneral::GetATanOfXY(vehicle->GetMatrix().GetForward().x,vehicle->GetMatrix().GetForward().y);
            CVector forward{vehicle->GetMatrix().GetForward().x,vehicle->GetMatrix().GetForward().y,0.f};forward.Normalise();
            targetSpeed=CamVehicleTargetSpeed(forward.x,forward.y,vehicle->m_vecMoveSpeed.x,vehicle->m_vecMoveSpeed.y);
            m_fSpeedVar=CamVehicleSmoothedSpeed(m_fSpeedVar,targetSpeed);
            if (m_nDirectionWasLooking!=3) {
                if (!CPad::GetPad(0)->GetLookBehindForCar() || CPad::GetPad(0)->GetLookLeft() || CPad::GetPad(0)->GetLookRight()) StaticRef<bool>(0xB6F042)=true;
            }
        } else {
            if (m_pCamTargetEntity==FindPlayerPed(-1)) {
                auto* player=FindPlayerPed(-1);CVector position=player->GetPosition();
                if (FindPlayerPed(-1)->GetIntelligence()->GetTaskClimb()) {
                    FindPlayerPed(-1);
                    auto* climbPlayer=FindPlayerPed(-1);
                    climbPlayer->GetIntelligence()->GetTaskClimb()->GetCameraTargetPos(climbPlayer,position);
                }
                target=position;
                auto& old=StaticRef<CVector>(0x8CCC3C);auto& velocity=StaticRef<CVector>(0xB6EC7C);
                bool clear=(old-position).SquaredMagnitude()>9.f || CTimer::GetTimeStep()<.2f || Using3rdPersonMouseCam() || StaticRef<bool>(0xB6F042) || StaticRef<bool>(0xB6F043);
                if (clear) { velocity.x=velocity.y=0.f; }
                else if (FindPlayerPed(-1)->GetIntelligence()->GetTaskFighting() && int(m_nMode)==53) {
                    const float damping=CamPower(f(0x8CC39C),CTimer::GetTimeStep());target=old*damping+position*(1.f-damping);velocity.x=velocity.y=0.f;
                } else {
                    const float positionWeight=CamPowerComplement(f(0x8CC394),CTimer::GetTimeStep());
                    const float velocityWeight=CamPowerComplement(f(0x8CC398),CTimer::GetTimeStep());
                    target=(old+velocity*CTimer::GetTimeStep())*(1.f-positionWeight)+position*positionWeight;
                    const CVector displacement=target-old;
                    const CVector weightedDisplacement=displacement*velocityWeight;
                    velocity=velocity*(1.f-velocityWeight)+weightedDisplacement*CamVelocityInverseStep(CTimer::GetTimeStep());
                    target.z=position.z;
                }
                velocity.z=0.f;old=target;
            } else target=m_pCamTargetEntity->GetPosition();
            const auto& forward=m_pCamTargetEntity->GetMatrix().GetForward();
            if (forward.x!=0.f || forward.y!=0.f) orientation=CGeneral::GetATanOfXY(forward.x,forward.y);
            m_fSpeedVar=0.f;
        }
    } else target=m_vecCamFixedModeVector;
    m_nDirectionWasLooking=StaticRef<int32>(0x8CC384);StaticRef<int32>(0x8CC384)=3;
    if (uintptr_t(this)==activeAddress) {
        const int mode=int(m_nMode);
        if ((mode==18 || mode==16 || mode==22 || mode==3) && m_pCamTargetEntity->GetIsTypeVehicle()) {
            auto* vehicle=static_cast<CVehicle*>(m_pCamTargetEntity);
            const bool specialVehicle=vehicle && (vehicle->GetVehicleAppearance()==VEHICLE_APPEARANCE_HELI || vehicle->GetVehicleAppearance()==VEHICLE_APPEARANCE_PLANE);
            if (CPad::GetPad(0)->GetLookBehindForCar()) {
                StaticRef<bool>(0xB6F080)=StaticRef<bool>(0xB6F077)=StaticRef<bool>(0xB6F062)=false;
                if (m_nDirectionWasLooking!=0) StaticRef<bool>(0xB6F052)=true;
                StaticRef<int32>(0x8CC384)=0;
            } else {
                int direction=3;
                if (CPad::GetPad(0)->GetLookLeft() && !specialVehicle) direction=1;
                else if (CPad::GetPad(0)->GetLookRight() && !specialVehicle) direction=2;
                StaticRef<int32>(0x8CC384)=direction;
                if (direction!=3) StaticRef<bool>(0xB6F062)=StaticRef<bool>(0xB6F077)=StaticRef<bool>(0xB6F080)=false;
                if (m_nDirectionWasLooking!=uint32(direction)) StaticRef<bool>(0xB6F052)=true;
            }
        } else if (mode==4 && m_pCamTargetEntity->GetIsTypePed()) {
            if (CPad::GetPad(0)->GetLookBehindForPed()) {
                if (m_nDirectionWasLooking!=0 && !StaticRef<bool>(0xB6F080)) StaticRef<bool>(0xB6F052)=true;
                StaticRef<int32>(0x8CC384)=0;
            } else if (m_nDirectionWasLooking!=3) StaticRef<float>(0x8CCB84)=1.f;
        } else if (mode==53 && m_nDirectionWasLooking!=3) StaticRef<float>(0x8CCB84)=1.f;
    }
    if (StaticRef<bool>(0xB6F052)) { StaticRef<float>(0x8CCB84)=1.f;StaticRef<bool>(0xB6F999)=true; }
    const int mode=int(m_nMode);
    if (mode!=3 && mode!=18 && mode!=22 && mode!=16 && mode!=49) StaticRef<bool>(0x8D5109)=false;
    gbFirstPersonRunThisFrame=false;
    switch (mode) {
    case 3:case 18:case 22:Process_FollowCar_SA(target,orientation,m_fSpeedVar,targetSpeed,false);break;
    case 4:if (!StaticRef<bool>(0xB6EC2E) || StaticRef<bool>(0x8CCF00)) Process_FollowPed_SA(target,orientation,m_fSpeedVar,targetSpeed,false);else Process_FollowPedWithMouse(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 7:case 34:case 45:case 46:Process_M16_1stPerson(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 8:Process_Rocket(target,orientation,m_fSpeedVar,targetSpeed,false);break;
    case 14:Process_WheelCam(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 15:Process_Fixed(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 16:Process_1stPerson(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 17:Process_FlyBy(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 29:ProcessPedsDeadBaby();StaticRef<bool>(0xB6F04E)=StaticRef<bool>(0xB6F050)=false;break;
    case 32:ProcessArrestCamOne();break;
    case 33:break;
    case 35:Process_SpecialFixedForSyphon(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 39:case 40:case 41:case 42:case 43:case 52:Process_1rstPersonPedOnPC(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 44:Process_Editor(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 47:Process_AttachedCam();break;
    case 48:Process_Cam_TwoPlayer();break;
    case 49:Process_Cam_TwoPlayer_InCarAndShooting();break;
    case 50:Process_Cam_TwoPlayer_Separate_Cars();break;
    case 51:Process_Rocket(target,orientation,m_fSpeedVar,targetSpeed,true);break;
    case 53:case 55:case 65:Process_AimWeapon(target,orientation,m_fSpeedVar,targetSpeed);break;
    case 54:Process_Cam_TwoPlayer_Separate_Cars_TopDown();break;
    case 56:Process_DW_HeliChaseCam(false);break;
    case 57:Process_DW_CamManCam(false);break;
    case 58:Process_DW_BirdyCam(false);break;
    case 59:Process_DW_PlaneSpotterCam(false);break;
    case 60:case 61:StaticRef<bool>(0xB6F059)=false;break;
    case 62:Process_DW_PlaneCam1(false);break;
    case 63:Process_DW_PlaneCam2(false);break;
    case 64:Process_DW_PlaneCam3(false);break;
    default:m_vecSource=CVector{0,0,0};m_vecFront=CVector{0,1,0};m_vecUp=CVector{0,0,1};break;
    }
    const int finalMode=int(m_nMode);
    if (finalMode<56 || finalMode>64) StaticRef<int32>(0x8CC488)=-1;
    StaticRef<int32>(0x8CC388)=finalMode;
    const CVector difference=m_vecSource-m_vecTargetCoorsForFudgeInter;
    m_fTrueBeta=CGeneral::GetATanOfXY(difference.x,difference.y);
    m_fTrueAlpha=CGeneral::GetATanOfXY(std::sqrt(difference.x*difference.x+difference.y*difference.y),difference.z);
    if (!StaticRef<bool>(0xB6F080)) KeepTrackOfTheSpeed(m_vecSource,m_vecTargetCoorsForFudgeInter,m_vecUp,m_fTrueAlpha,m_fTrueBeta,m_fFOV);
    m_vecSourceBeforeLookBehind=m_vecSource;m_bLookingRight=m_bLookingLeft=m_bLookingBehind=false;
    if (uintptr_t(this)==activeAddress) {
        const int direction=StaticRef<int32>(0x8CC384);
        if (direction==0) LookBehind();else if (direction==1) LookRight(false);else if (direction==2) LookRight(true);
        m_nDirectionWasLooking=StaticRef<int32>(0x8CC384);
    }
    if (StaticRef<bool>(0xB6FCDD)) { m_fFOV=f(0xB6FCE0);StaticRef<bool>(0xB6FCDD)=false; }
    if (StaticRef<bool>(0xB6FD14)) { m_vecSource=StaticRef<CVector>(0xB6FD08);StaticRef<bool>(0xB6FD14)=false; }
    if (StaticRef<bool>(0xB6FCB4)) { m_vecFront=StaticRef<CVector>(0xB6FCA8)-m_vecSource;m_vecFront.Normalise();GetVectorsReadyForRW();StaticRef<bool>(0xB6FCB4)=false; }
}

// 0x518500
bool CCam::ProcessArrestCamOne() {
    m_fFOV=45.0f;auto* target=StaticRef<CEntity*>(0xB6F980);auto& mode=StaticRef<int32>(0xB6EC58);auto& tracked=StaticRef<CEntity*>(0xB6EC5C);
    CPed* cop=nullptr;CVector dest,source;
    const auto playerCop=[&](){CPed* result=nullptr;if(FindPlayerPed(-1)){auto* player=FindPlayerPed(-1);if(*reinterpret_cast<CEntity**>(*reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(player)+0x480)+8)){player=FindPlayerPed(-1);result=*reinterpret_cast<CPed**>(*reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(player)+0x480)+8);}}return result;};
    const auto geometry=[&](const CVector& position){m_vecSource=position;CVector copy=position;plugin::CallMethod<0x514030>(&TheCamera,&copy,&dest,&m_vecSource,m_fFOV);};
    const auto axes=[&](){m_vecFront=dest-m_vecSource;m_vecFront.Normalise();m_vecUp=CVector{0,0,1};CVector side=CrossProduct(m_vecFront,m_vecUp);side.Normalise();m_vecUp=CrossProduct(side,m_vecFront);};
    const auto copSource=[&](CPed* from,bool alternate){return alternate?plugin::CallMethodAndReturn<bool,0x516010>(this,target,from,&dest,&source):plugin::CallMethodAndReturn<bool,0x515D80>(this,target,from,&dest,&source);};
    const auto cheese=[&](CPed* from,bool initialise){return plugin::CallMethodAndReturn<bool,0x512EF0>(this,from,initialise);};
    if(m_bResetStatics){
        mode=0;CPed* actor=nullptr;std::array<int32,6> sequence;sequence.fill(-1);
        if(target->GetType()==ENTITY_TYPE_PED){actor=static_cast<CPed*>(target);actor->GetBonePosition(&dest,static_cast<eBoneTag>(3),true);cop=playerCop();
            if(cop&&float(std::rand())*std::bit_cast<float>(0x38000100u)>.5f)sequence={1,2,3,2,8,-1};else sequence={1,3,2,8,-1,-1};
        }else if(target->GetType()==ENTITY_TYPE_VEHICLE){auto* driver=static_cast<CVehicle*>(target)->m_pDriver;if(driver&&driver->IsPlayer()){actor=driver;actor->GetBonePosition(&dest,static_cast<eBoneTag>(3),true);}else dest=target->GetPosition();
            cop=playerCop();if(cop&&float(std::rand())*std::bit_cast<float>(0x38000100u)>.65f)sequence={2,8,3,2,-1,-1};else sequence={8,3,2,-1,-1,-1};
        }else return false;
        if(!StaticRef<uint8>(0xBAADC0)){const auto text=plugin::CallMethodAndReturn<const GxtChar*,0x6A0050>(&TheText,"BUSTED");plugin::Call<0x69F2B0>(text,uint32(5000),int32(2));}
        bool eligible=false;
        for(int32 index=0;;++index){
            if(mode||index>=6||sequence[index]<1){geometry(source);axes();if(mode)m_bResetStatics=false;return true;}
            tracked=nullptr;CPed* nextCop=cop;
            switch(sequence[index]){
            case 1:
                StaticRef<float>(0xB6EC60)=float(CTimer::GetTimeInMS());
                if(cheese(cop,true)){*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(target)+0x1C)&=~0x80u;mode=1;m_bResetStatics=false;return true;}
                break;
            case 2:case 3:
                if(cop){eligible=copSource(cop,sequence[index]==3);tracked=cop;nextCop=nullptr;}
                else if(actor){
                    auto** nearby=reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(actor->GetIntelligence())+0x130);
                    for(int32 i=0;i<16;++i)if(auto* candidate=nearby[i]){
                        auto* task=candidate->GetTaskManager().FindActiveTaskByType(static_cast<eTaskType>(0x44C));
                        if(task&&*reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(task)+8)==FindPlayerPed(-1)){eligible=copSource(candidate,false);if(eligible){tracked=candidate;break;}}
                    }
                }
                break;
            case 8:eligible=GetLookFromLampPostPos(target,cop,dest,source);break;
            }
            if(eligible){if(tracked)tracked->RegisterReference(&tracked);mode=sequence[index];if(mode==3){const float choice=(float(std::rand())*std::bit_cast<float>(0x38000100u))*5.0f;mode=choice<1?3:choice<2?4:choice<3?5:choice<4?6:7;}}
            cop=nextCop;
        }
    }
    if(mode==1){*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(target)+0x1C)&=~0x80u;cop=playerCop();return cheese(cop,false);}
    if(target->GetType()==ENTITY_TYPE_PED)static_cast<CPed*>(target)->GetBonePosition(&dest,static_cast<eBoneTag>(3),true);
    else if(target->GetType()==ENTITY_TYPE_VEHICLE){auto* driver=static_cast<CVehicle*>(target)->m_pDriver;if(driver&&driver->IsPlayer())driver->GetBonePosition(&dest,static_cast<eBoneTag>(3),true);else dest=target->GetPosition();}
    else return false;
    bool move=false;
    if(mode==2){if(tracked){const bool found=copSource(static_cast<CPed*>(tracked),false);const float limit=CTimer::GetTimeStep()*StaticRef<float>(0x8CC7EC)+m_vecSource.z;if(limit<source.z)source.z=limit;move=found;}}
    else if(mode>=4&&mode<=7){
        source=m_vecSource;m_vecFront=dest-source;m_vecFront.Normalise();m_vecUp=CVector{0,0,1};CVector side=CrossProduct(m_vecFront,m_vecUp);side.Normalise();if(mode==6||mode==7)side=-side;
        const CVector test{side.x*.5f+source.x,side.y*.5f+source.y,side.z*.5f+source.z};
        if(!CWorld::TestSphereAgainstWorld(test,.4f,target,true,true,false,true,false,true)){
            const float speed=StaticRef<float>(0x8CC7E4),step=CTimer::GetTimeStep();source=CVector{(side.x*speed)*step+source.x,(side.y*speed)*step+source.y,(side.z*speed)*step+source.z};
            if(mode==5||mode==7)source.z=step*StaticRef<float>(0x8CC7E8)+source.z;
            else {bool found=false;const float ground=CWorld::FindGroundZFor3DCoord(source,&found,nullptr);if(found)source.z=ground+StaticRef<float>(0x8CC7F8);}
            move=true;
        }
    }else if(mode==8){
        source=m_vecSource;m_vecFront=dest-source;m_vecFront.z=0;m_vecFront.Normalise();m_vecUp=CVector{0,0,1};CVector side=CrossProduct(m_vecFront,m_vecUp);side.Normalise();
        const float offset=StaticRef<float>(0x8CC7FC);m_vecFront=CVector{(dest.x-source.x)+side.x*offset,(dest.y-source.y)+side.y*offset,(dest.z-source.z)+side.z*offset};m_vecFront.z=0;m_vecFront.Normalise();
        const CVector test{m_vecFront.x*.5f+source.x,m_vecFront.y*.5f+source.y,m_vecFront.z*.5f+source.z};
        if(!CWorld::TestSphereAgainstWorld(test,.4f,target,true,true,false,true,false,true)){const float speed=StaticRef<float>(0x8CC800),step=CTimer::GetTimeStep();source=CVector{(speed*m_vecFront.x)*step+source.x,(speed*m_vecFront.y)*step+source.y,(speed*m_vecFront.z)*step+source.z};move=true;}
    }
    if(move){geometry(source);axes();}else {CVector copy=m_vecSource;plugin::CallMethod<0x514030>(&TheCamera,&copy,&dest,&m_vecSource,m_fFOV);}
    return true;
}

static CVector DeadCamWave(float elapsed, const CVector& side, const CVector& target) {
    const float capped = elapsed > 1000.0f ? 1000.0f : elapsed;
    const float period = StaticRef<float>(0x8CC810), amplitude = StaticRef<float>(0x8CC80C), factor = .001f;
    const float sx=side.x, sy=side.y, tx=target.x; float x, y, scaledY;
    CamDeadWaveCpp(elapsed, period, capped, factor, sx, amplitude, sy, &scaledY, &y, tx, &x);
    return {x, y + target.y, 0};
}

// 0x519250
void CCam::ProcessPedsDeadBaby() {
    auto* target=StaticRef<CEntity*>(0xB6F980);
    CVector dest;
    if(target->GetType()==ENTITY_TYPE_PED)static_cast<CPed*>(target)->GetBonePosition(&dest,static_cast<eBoneTag>(3),true);
    else if(target->GetType()==ENTITY_TYPE_VEHICLE){dest=target->GetPosition();dest.z+=target->GetColModel()->m_boundBox.m_vecMax.z;}
    else return;
    CVector source=m_vecSource, side;
    if(m_bResetStatics){
        StaticRef<float>(0xB70054)=float(CTimer::GetTimeInMS());
        StaticRef<uint32>(0xB6F094)=CTimer::GetTimeInMS();StaticRef<uint32>(0xB70050)=0;
        source=CVector{dest.x,dest.y,dest.z+StaticRef<float>(0x8CC804)};
        float water=0;if(CWaterLevel::GetWaterLevel(source.x,source.y,source.z,water,0,nullptr)&&source.z<water+1.5f)source.z=water+1.5f;
        side=CrossProduct(target->GetMatrix().GetForward(),CVector{0,0,1});side.z=0;side.Normalise();
        m_vecFront=dest-source;m_vecFront.Normalise();m_vecUp=CrossProduct(side,m_vecFront);m_vecUp.Normalise();m_bResetStatics=false;
    }else{
        if(!CWorld::TestSphereAgainstWorld({source.x,source.y,source.z+.2f},.3f,target,true,true,false,true,false,true))source.z+=CTimer::GetTimeStep()*StaticRef<float>(0x8CC808);
        side=CrossProduct(target->GetMatrix().GetForward(),CVector{0,0,1});side.z=0;side.Normalise();
        const int32 elapsedInt=int32(CTimer::GetTimeInMS()-StaticRef<uint32>(0xB6F094));float elapsed;
        const float unsignedCorrection=4294967296.0f;
        CamDeadUnsignedElapsedCpp(elapsedInt, unsignedCorrection, &elapsed);
        const auto wave=DeadCamWave(elapsed,side,dest);
        CVector movement{wave.x-source.x,wave.y-source.y,0};movement.Normalise();
        const CVector test{movement.x*.2f+source.x,movement.y*.2f+source.y,movement.z*.2f+source.z};
        if(!CWorld::TestSphereAgainstWorld(test,.3f,target,true,true,false,true,false,true)){source.x=wave.x;source.y=wave.y;}
        const float period=StaticRef<float>(0x8CC810),amplitude=StaticRef<float>(0x8CC814);
        const float scale=(elapsed>2000.0f?2000.0f:elapsed)*.0005f;float cosine;
        CamDeadCosineCpp(elapsed, period, &cosine);
        m_vecFront=CVector{((side.x*amplitude)*scale)*cosine,((side.y*amplitude)*scale)*cosine,((side.z*amplitude)*scale)*cosine-1.0f};
        m_vecFront.Normalise();m_vecUp=CrossProduct(side,m_vecFront);m_vecUp.Normalise();
    }
    m_vecSource=source;
    plugin::CallMethod<0x514030>(&TheCamera,&source,&dest,&m_vecSource,m_fFOV);
    StaticRef<uint8>(0xB6F055)=0;
}

// 0x50EB70
void CCam::Process_1rstPersonPedOnPC(const CVector&, float orientation, float, float) {
    const auto f = [](uintptr_t a) { return *reinterpret_cast<const float*>(a); };
    StaticRef<uint32>(0xB6FFE0) |= 1;
    if (m_nMode != MODE_SNIPER_RUNABOUT) m_fFOV = 70.f;
    StaticRef<bool>(0xB6F066) = false;
    if (!m_pCamTargetEntity->GetRwObject()) return;
    if (m_pCamTargetEntity->GetIsTypePed()) {
        auto* ped = m_pCamTargetEntity->AsPed();
        auto* hierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
        const auto index = RpHAnimIDGetIndex(hierarchy, ConvertPedNode2BoneTag(2));
        auto& matrix = RpHAnimHierarchyGetMatrixArray(hierarchy)[index];
        CVector point = StaticRef<CVector>(0x8CCC54);
        RwV3dTransformPoints(&point, &point, 1, &matrix);
        const RwV3d zero{0.f,0.f,0.f}; RwMatrixScale(&matrix, &zero, rwCOMBINEPRECONCAT);
        auto& initial = StaticRef<CVector>(0xB6FFC4);
        if (m_bResetStatics) {
            m_fHorizontalAngle = orientation; m_fVerticalAngle = 0.f; m_fInitialPlayerOrientation = orientation;
            m_fHorizontalAngle = ped->m_fCurrentRotation + f(0x858FE4); m_fVerticalAngle = 0.f;
            StaticRef<bool>(0xB6FFDC) = false; StaticRef<float>(0xB6FFD8) = 0.f; StaticRef<float>(0xB6FFD4) = 0.f;
            m_bCollisionChecksOn = true; m_fInitialPlayerOrientation = ped->m_fCurrentRotation + f(0x858FE4);
            m_vecBufferedPlayerBodyOffset = point; StaticRef<float>(0xB6FFD0) = 0.f; initial = point;
        }
        m_vecBufferedPlayerBodyOffset.y = point.y;
        if (!TheCamera.m_bHeadBob) {
            CVector direction{ped->GetMatrix().GetForward().x,ped->GetMatrix().GetForward().y,0.f}; direction.Normalise();
            const float deltaX = point.x - initial.x, deltaY = point.y - initial.y;
            const float magnitude = std::sqrt(deltaY * deltaY + deltaX * deltaX);
            m_vecSource.x = (direction.x * magnitude) * f(0x863108) + ped->GetPosition().x;
            m_vecSource.y = (direction.y * magnitude) * f(0x863108) + ped->GetPosition().y;
            m_vecSource.z = ((magnitude * 0.f) * f(0x863108) + ped->GetPosition().z) + f(0x863104);
        } else {
            const float ratio = StaticRef<float>(0xB6FC48);
            m_vecBufferedPlayerBodyOffset.x = ratio * m_vecBufferedPlayerBodyOffset.x + (1.f - ratio) * point.x;
            m_vecBufferedPlayerBodyOffset.z = ratio * m_vecBufferedPlayerBodyOffset.z + (1.f - ratio) * point.z;
            m_vecSource = ped->GetMatrix().TransformPoint(m_vecBufferedPlayerBodyOffset);
        }
        CVector bone; ped->GetBonePosition(&bone, static_cast<eBoneTag>(3), true);
        CPad::GetPad(0);
        const float mouseX = StaticRef<float>(0xB73424), mouseY = StaticRef<float>(0xB73428);
        float betaDelta, alphaDelta;
        if (mouseX == 0.f && mouseY == 0.f) {
            const float horizontal = float(-int(CPad::GetPad(0)->GetMouseCameraStickHorizontal()));
            orientation = float(int(CPad::GetPad(0)->GetMouseCameraStickVertical()));
            const float horizontalSign = horizontal < 0.f ? -1.f : 1.f, verticalSign = orientation < 0.f ? -1.f : 1.f;
            const float scale = m_fFOV * f(0x85A5C4);
            betaDelta = (((horizontal * horizontal) * f(0x858FC4)) * (scale * f(0x863100))) * horizontalSign * CTimer::GetTimeStep();
            alphaDelta = (((orientation * orientation) * f(0x8630FC)) * (scale * f(0x859B50))) * verticalSign * CTimer::GetTimeStep();
        } else {
            const float vertical = mouseY * f(0x858B90), scale = m_fFOV * f(0x85A5C4);
            betaDelta = (scale * StaticRef<float>(0xB6EC1C)) * (mouseX * f(0x859024));
            alphaDelta = (scale * StaticRef<float>(0xB6EC18)) * vertical;
            orientation = vertical;
        }
        m_fHorizontalAngle = betaDelta + m_fHorizontalAngle; m_fVerticalAngle = alphaDelta + m_fVerticalAngle;
        ClipBeta();
        if (m_fVerticalAngle > f(0x8630F8)) m_fVerticalAngle = std::bit_cast<float>(0x3F860A92u);
        else if (m_fVerticalAngle < f(0x8630F4)) m_fVerticalAngle = std::bit_cast<float>(0xBFC7F1E7u);
        if (ped->IsPlayer() && ped->m_pAttachedTo) {
            float base = orientation;
            switch (ped->m_fTurretAngleA) {
            case 0: base = ped->m_pAttachedTo->GetHeading() + f(0x858FE4); break;
            case 1: base = ped->m_pAttachedTo->GetHeading() + f(0x858CB8); break;
            case 2: base = ped->m_pAttachedTo->GetHeading() - f(0x858FE4); break;
            case 3: base = ped->m_pAttachedTo->GetHeading(); break;
            }
            float angle = m_fHorizontalAngle - base;
            if (angle > f(0x858CB8)) angle -= f(0x858CBC);
            else if (angle < -f(0x858CB8)) angle += f(0x858CBC);
            if (angle > ped->m_fTurretAngleB) angle = ped->m_fTurretAngleB;
            else if (angle < -ped->m_fTurretAngleB) angle = -ped->m_fTurretAngleB;
            m_fHorizontalAngle = angle + base;
        }
        float pitch = m_fVerticalAngle;
        float yaw = m_fHorizontalAngle;
        float cosineX, sineY, sineZ;
        CamFirstPersonPedFrontCpp(pitch, yaw, &sineY, &sineZ, &cosineX);
        m_vecFront.x = (cosineX + m_vecSource.x) - m_vecSource.x;
        m_vecFront.y = (sineY * f(0x858B3C) + m_vecSource.y) - m_vecSource.y;
        m_vecFront.z = (sineZ * f(0x858B3C) + m_vecSource.z) - m_vecSource.z;
        m_vecFront.Normalise();
        m_vecSource.x = m_vecFront.x * f(0x858EE8) + m_vecSource.x;
        m_vecSource.y = m_vecFront.y * f(0x858EE8) + m_vecSource.y;
        m_vecSource.z = m_vecFront.z * f(0x858EE8) + m_vecSource.z;
        StaticRef<float>(0xB6F174) = m_fVerticalAngle; GetVectorsReadyForRW();
        float heading;
        float headingInputX = -m_vecFront.x;
        float headingInputY = m_vecFront.y;
        heading = TrigAtan2Cpp(headingInputX, headingInputY);
        auto* player = StaticRef<CPed*>(0xB6F980);
        player->m_fCurrentRotation = heading; player->m_fAimingRotation = heading; player->SetHeading(heading);
        if (player->GetRwObject()) player->UpdateRwMatrix();
        if (m_nMode == MODE_SNIPER_RUNABOUT) {
            const bool zoomOut = CPad::GetPad(0)->SniperZoomOut();
            bool zoomIn = false;
            if (!zoomOut) zoomIn = CPad::GetPad(0)->SniperZoomIn();
            if (zoomOut || zoomIn) {
                if (CPad::GetPad(0)->SniperZoomOut()) m_fFOV = (CTimer::GetTimeStep() * 255.f + 10000.f) * m_fFOV * f(0x858FC4);
                else if (CPad::GetPad(0)->SniperZoomIn()) m_fFOV /= (CTimer::GetTimeStep() * 255.f + 10000.f) * f(0x858FC4);
            }
            StaticRef<uint32>(0xB6F0B4) = 180; StaticRef<uint32>(0xB6F0AC) = 180; StaticRef<uint32>(0xB6F0B0) = 255; StaticRef<uint32>(0xB6F0B8) = 1; StaticRef<uint32>(0xB6F0C8) = 120;
            if (m_fFOV > 70.f) m_fFOV = 70.f; if (m_fFOV < 15.f) m_fFOV = 15.f;
        }
    }
    m_bResetStatics = false; RwCameraSetNearClipPlane(Scene.m_pRwCamera, .05f);
}

// 0x517EA0
void CCam::Process_1stPerson(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static auto& s_LastWheelieTime = StaticRef<float>(0x8CCD14);
    // Making sure player doesn't see below ground when flipped.
    // Name is made up cuz I found it funny to name it like that.
    static auto& s_GroundFaultProtection = StaticRef<float>(0xB7004C);

    gbFirstPersonRunThisFrame = true;

    m_fFOV = 70.0f;
    if (!m_pCamTargetEntity->GetRwObject()) {
        return;
    }

    if (m_bResetStatics) {
        // Como el original (0x517ED2..0x517F29): primero orientation y luego, para un ped, rumbo + pi/2 (0x858FE4)
        m_fHorizontalAngle          = orientation;
        m_fVerticalAngle            = 0.0f;
        m_fInitialPlayerOrientation = orientation;
        if (m_pCamTargetEntity->GetIsTypePed()) {
            const float heading = m_pCamTargetEntity->AsPed()->m_fCurrentRotation + CamExeFloat(0x858FE4);
            m_fVerticalAngle            = 0.0f;
            m_fHorizontalAngle          = heading;
            m_fInitialPlayerOrientation = heading;
        }

        s_GroundFaultProtection                 = 0.0f;
        TheCamera.m_fAvoidTheGeometryProbsTimer = 0.0f;
    }

    if (m_pCamTargetEntity->GetIsTypePed()) {
        m_bResetStatics = false;
        return;
    }

    auto* const targetVeh = static_cast<CVehicle*>(m_pCamTargetEntity);
    // El EXE lee el byte 0x614 del vehiculo (bit 7 = bWheelieForCamera de las motos) sea cual sea el tipo (0x517FA7, 0x51802C).
    auto& wheelieFlags = *(reinterpret_cast<uint8*>(targetVeh) + 0x614);

    // 0x517F49..0x517F5E: fild del tiempo (con signo) y fadd 2^32 si es negativo
    // El x87 (24 bits) redondea una sola vez la suma exacta fild + 2^32: se hace en double para no redondear dos veces.
    const int32 timeSigned = static_cast<int32>(CTimer::GetTimeInMS());
    const float wheelieTime = timeSigned < 0
        ? static_cast<float>(static_cast<double>(timeSigned) + static_cast<double>(CamExeFloat(0x858C54)))
        : static_cast<float>(timeSigned);
    if (s_LastWheelieTime > wheelieTime) {
        s_LastWheelieTime = 0.0f;
    }

    // Como el original (0x517F7D..0x517FE1): con diferencia < 3000 (0x8CCD10) siempre entra; si no, solo una moto con
    // bWheelieForCamera o con el temporizador de TheCamera > 0 (y entonces refresca el tiempo).
    const float wheelieDiff = wheelieTime - s_LastWheelieTime;
    bool enterWheelieBlock;
    if (wheelieDiff < CamExeFloat(0x8CCD10)) {
        enterWheelieBlock = true;
    } else if (targetVeh->m_nVehicleType != VEHICLE_TYPE_BIKE) {
        enterWheelieBlock = false;
    } else if (!(wheelieFlags & 0x80) && !(TheCamera.m_fAvoidTheGeometryProbsTimer > 0.0f)) {
        enterWheelieBlock = false;
    } else {
        if (wheelieDiff >= CamExeFloat(0x8CCD10)) {
            s_LastWheelieTime = wheelieTime;
        }
        enterWheelieBlock = true;
    }

    if (enterWheelieBlock) {
        if (!CPad::GetPad(0)->NewState.LeftShoulder2 && !CPad::GetPad(0)->NewState.RightShoulder2) {
            if (Process_WheelCam(target, orientation, speedVar, speedVarWanted)) {
                if (wheelieFlags & 0x80) {
                    TheCamera.m_fAvoidTheGeometryProbsTimer = 50.0f;
                    return;
                }
                TheCamera.m_fAvoidTheGeometryProbsTimer = TheCamera.m_fAvoidTheGeometryProbsTimer - CTimer::GetTimeStep();
                wheelieFlags |= 0x80;
                return;
            }
            TheCamera.m_fAvoidTheGeometryProbsTimer = 0.0f;
            wheelieFlags &= 0x7F;
            s_LastWheelieTime = 0.0f;
        }
    }

    // Como el original (0x51809F..0x5180FF): si el vehiculo no tiene matriz se la reserva y se rellena desde la transformacion simple
    if (!targetVeh->m_matrix) {
        targetVeh->AllocateMatrix();
        targetVeh->m_placement.UpdateMatrix(targetVeh->m_matrix);
    }
    const CMatrix* entityWorldMat = targetVeh->m_matrix;
    if (targetVeh->m_nVehicleType == VEHICLE_TYPE_BIKE) {
        auto* bike = targetVeh->AsBike();
        bike->CalculateLeanMatrix();
        entityWorldMat = &bike->m_mLeanMatrix;
    }

    // 0x5180EB..0x518145: el EXE mira el tipo del modelo (+0x3C); barca => dummy 0 (luz delantera), si no el 4 (asiento delantero)
    auto* const modelInfo = CModelInfo::GetModelInfo(int32(int16(targetVeh->m_nModelIndex))); // movsx eax,word ptr [ecx+22h] (0x5180EB)
    const bool  boatModel = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(modelInfo) + 0x3C) == 5;
    const CVector& dummy  = modelInfo->AsVehicleModelInfoPtr()->GetVehicleStruct()->m_avDummyPos[boatModel ? DUMMY_LIGHT_FRONT_MAIN : DUMMY_SEAT_FRONT];   // [eax+5Ch] (0x5180FA)
    const CVector  localPosition{ 0.0f, dummy.y + CamExeFloat(0x859018), dummy.z + CamExeFloat(0x8631EC) }; // x se pone a 0 (0x518139)

    m_fFOV = 60.0f;
    // 0x59C790 con su orden de sumas, luego se suma la posicion del vehiculo
    m_vecSource = CamMultiplyMatrixVector(*entityWorldMat, localPosition);
    {
        const CVector& vehiclePosition = targetVeh->GetPosition();
        m_vecSource.x = m_vecSource.x + vehiclePosition.x;
        m_vecSource.y = vehiclePosition.y + m_vecSource.y;
        m_vecSource.z = vehiclePosition.z + m_vecSource.z;
    }

    if (targetVeh->m_nVehicleType == VEHICLE_TYPE_BOAT) {
        m_vecSource.z = m_vecSource.z + CamExeFloat(0x858B8C);
    } else if (targetVeh->m_nVehicleType == VEHICLE_TYPE_BIKE && targetVeh->m_pDriver) {
        CVector neckPos{};
        targetVeh->m_pDriver->GetBonePosition(&neckPos, BONE_NECK, true); // 0x5E4280 (no GetTransformedBonePosition)
        const CVector& moveSpeed = targetVeh->GetMoveSpeed();
        const float timeStep = CTimer::GetTimeStep();
        neckPos.x = neckPos.x + timeStep * moveSpeed.x; // 0x40FEC0 (v * f) y 0x411A00 (+=)
        neckPos.y = neckPos.y + timeStep * moveSpeed.y;
        neckPos.z = neckPos.z + timeStep * moveSpeed.z;

        m_vecSource.z = neckPos.z + CamExeFloat(0x8CC7B4); // 0.15

        auto right = CrossProduct(m_vecFront, m_vecUp);
        right.x = right.x * CamExeFloat(0x8CCD0C); // 0x40FEF0: right *= 1.0
        right.y = right.y * CamExeFloat(0x8CCD0C);
        right.z = right.z * CamExeFloat(0x8CCD0C);

        // 0x40FE30 (a + b) y 0x40FE60 (a - b) sobre la fuente y el vector derecho
        const CVector rayStart{ m_vecSource.x + right.x, m_vecSource.y + right.y, m_vecSource.z + right.z };
        const CVector rayEnd{ m_vecSource.x - right.x, m_vecSource.y - right.y, m_vecSource.z - right.z };
        if (!CWorld::GetIsLineOfSightClear(rayStart, rayEnd, true, false, false, false)) {
            m_vecSource = targetVeh->GetPosition();
            m_vecSource.z = (neckPos.z + CamExeFloat(0x8CC7B4)) + CamExeFloat(0x8631EC);
        }
    }

    // todo: refactor
    if (targetVeh->IsUpsideDown()) {
        if (!(s_GroundFaultProtection < CamExeFloat(0x858B8C))) { // jp: >= 0.5 o NaN
            s_GroundFaultProtection = std::bit_cast<float>(0x3F000000u);
        } else {
            s_GroundFaultProtection = s_GroundFaultProtection + CamExeFloat(0x858B10);
        }
    } else if (!(s_GroundFaultProtection < CamExeFloat(0x858B50))) { // jp: >= 0 o NaN
        s_GroundFaultProtection = 0.0f;
    } else {
        s_GroundFaultProtection = s_GroundFaultProtection - CamExeFloat(0x858B10);
    }
    m_vecSource.z = s_GroundFaultProtection + m_vecSource.z;

    // 0x518372..0x518404: front y up se copian de la matriz (ejes 0x10 y 0x20) y se normalizan
    m_vecFront = entityWorldMat->GetForward();
    m_vecFront.Normalise();
    m_vecUp = entityWorldMat->GetUp();
    m_vecUp.Normalise();
    const auto a = CrossProduct(m_vecFront, m_vecUp).Normalized();
    m_vecUp = CrossProduct(a, m_vecFront);
    m_vecUp.Normalise();

    if (float wl{}; CWaterLevel::GetWaterLevel(m_vecSource, wl, true) && m_vecSource.z < wl - CamExeFloat(0x858C24)) {
        ApplyUnderwaterMotionBlur();
    }
    m_bResetStatics = false;
}

// 0x521500
namespace { float CamPower(float,float); float M16Asin(float); CVector M16Direction(float,float); void CameraRoundVector4(CVector&); float CameraHeadingMinusHalfPI(CPlaceable*); }
static float CamAimHeading355(float frontX, float frontY, float headingOffset) {
    float result;
    result = Atan2MinusCpp(-frontX, frontY, headingOffset);
    return result;
}
// Native 52198E..5219ED: tangent stays resident across both offsets.
static void CamAimOffsets355(float fov, float& horizontal, float& vertical) {
    float hResult,vResult;
    CamAimOffsetsCpp(fov, &vResult, &hResult);
    horizontal=hResult;vertical=vResult;
}
// 521FD4..522032: X/Y differences stay resident; Z, flat and angles spill.
static void CamAimTargetAngles355(const CVector& target, const CVector& focal, float& horizontal, float& flat, float& pitch) {
    const float tx=target.x,ty=target.y,tz=target.z,fx=focal.x,fy=focal.y,fz=focal.z;
    float dz,hResult,fResult,pResult;
    CamAimTargetAnglesCpp(tx, fx, ty, fy, tz, fz, &dz, &hResult, &fResult, &pResult);
    horizontal=hResult;flat=fResult;pitch=pResult;
}
// 522915..522A13: distance remains extended through projection;
// front and source products spill only at the EXE's prescribed stores.
static void CamAimProjection355(CCam* cam, const CVector& focal, uint32 rowOffset) {
    float scratch[0x60/4]{};
    scratch[0x3C/4]=focal.x;scratch[0x40/4]=focal.y;scratch[0x44/4]=focal.z;
    CamAimProjectionCpp(CamAddr(cam), CamAddr(scratch), CamAddr(rowOffset));
}

// 521915..52196E keeps pow and complement resident across all three blends.
static void CamAimMeleeBlend355(float desiredHorizontal,float desiredVertical,float desiredBlend) {
    float scratch[0x2C/4]{};
    scratch[0x20/4]=desiredHorizontal;scratch[0x14/4]=desiredVertical;scratch[0x28/4]=desiredBlend;
    CamAimMeleeBlendCpp(CamAddr(scratch));
}
static void CamAimMouseAngles355(CCam* cam) {
    CamAimMouseAnglesCpp(CamAddr(cam));
}

static float CamAimVehicleResetPitch355(float clampedZ,float initialAlpha) {
    float result;
    CamAimVehicleResetPitchCpp(clampedZ, initialAlpha, &result);
    return result;
}

void CCam::Process_AimWeapon(const CVector& targetPosition, float, float, float) {
    auto& initialized=StaticRef<uint32>(0xB70110);
    auto& target=StaticRef<CVector>(0xB70104);
    if (!(initialized&1)) { initialized|=1; target=CVector{0,0,0}; }
    if (!m_pCamTargetEntity->GetIsTypePed()) return;
    auto* ped=static_cast<CPed*>(m_pCamTargetEntity);
    if (!ped->IsPlayer()) return;
    auto* intelligence=ped->GetIntelligence();
    auto* gun=intelligence->GetTaskUseGun();
    auto* info=gun ? intelligence->GetTaskUseGun()->m_WeaponInfo : CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type,eWeaponSkill::STD);
    const auto f=[](uintptr_t address){return StaticRef<float>(address);};
    const uint32 flags=*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped)+0x46C);
    const bool inVehicle=(flags&0x100)!=0;
    auto* vehicle=ped->m_pVehicle;
    const bool driver=inVehicle && vehicle && vehicle->m_pDriver==ped;
    int state=0;
    if (!inVehicle) {
        if (intelligence->GetTaskJetPack()) state=1;
        else if (ped->GetActiveWeapon().IsTypeMelee()) state=3;
    } else state=!vehicle || (*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(vehicle)+0x590)!=9 && *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(vehicle)+0x594)!=2) ? 2 : 1;
    const uintptr_t row=0x8CC4C0+state*28;
    const int weapon=int(ped->GetActiveWeapon().m_Type);
    const float step=CTimer::GetTimeStep();
    const float desiredFov=weapon==30 || weapon==31 ? f(0x8CC4B4) : weapon==33 ? f(0x8CC4B8) : 70.f;
    if (!StaticRef<bool>(0xB6F080)) {
        if (!m_bResetStatics || weapon==33) {
            const float raised=step+m_fFOV;
            if (desiredFov<=raised) { const float reduced=m_fFOV-step; m_fFOV=reduced<=desiredFov?desiredFov:reduced; }
            else m_fFOV=raised;
        } else m_fFOV=desiredFov;
    }
    float horizontalOffset,verticalOffset;
    auto& meleeBlend=StaticRef<float>(0xB700F4);
    if (info->m_nWeaponFire==WEAPON_FIRE_MELEE) {
        if (!(initialized&2)) { initialized|=2; StaticRef<float>(0xB70100)=3.f; }
        if (!(initialized&4)) { initialized|=4; StaticRef<float>(0xB700FC)=20.f; }
        float desiredHorizontal=3.f,desiredVertical=20.f,desiredBlend=0.f;
        if (intelligence->GetTaskFighting() && int(ped->m_nMoveState)<4 && ped->m_pTargetedObject) {
            auto& cooldown=StaticRef<float>(0xB700F8);
            if (cooldown<=step) {
                if (cooldown < -step) cooldown+=step;
                else {
                    const CVector start=ped->GetPosition()+CVector{0,0,.75f};
                    CVector delta=CrossProduct(ped->m_pTargetedObject->GetPosition()-ped->GetPosition(),CVector{0,0,1});
                    const float length=std::max(.7f,delta.Magnitude());
                    delta*=2.f/length;
                    cooldown=CWorld::GetIsLineOfSightClear(start,start+delta,true,true,false,true,false,true,true)?100.f:-100.f;
                }
            } else cooldown-=step;
            if (cooldown>=0.f) { desiredHorizontal=0.f;desiredVertical=70.f;desiredBlend=1.f; }
        }
        if (!m_bResetStatics) {
            if (!StaticRef<bool>(0xB6F080)) {
                CamAimMeleeBlend355(desiredHorizontal,desiredVertical,desiredBlend);
            }
        } else { StaticRef<float>(0xB70100)=desiredHorizontal; StaticRef<float>(0xB700FC)=desiredVertical; meleeBlend=0.f; }
        horizontalOffset=f(0xB70100)*.017453292f;verticalOffset=f(0xB700FC)*.017453292f;
    } else {
        meleeBlend=0.f;
        CamAimOffsets355(m_fFOV,horizontalOffset,verticalOffset);
    }
    if (m_bResetStatics) {
        TheCamera.ResetDuckingSystem(ped);m_bRotating=false;m_bCollisionChecksOn=true;m_fAlphaSpeed=m_fBetaSpeed=0.f;
        StaticRef<bool>(0xB6EC44)=true;StaticRef<int32>(0xB6EC48)=60000;StaticRef<float>(0x8CC530)=-1001.f;StaticRef<uint32>(0xB6EC4C)=0;
        if (!StaticRef<bool>(0xB6EC2E) || ped->m_pTargetedObject) {
            m_fVerticalAngle=f(row+12);
            if (!inVehicle || !vehicle) {
                if (!ped->m_pTargetedObject) {
                    m_fHorizontalAngle=(ped->m_fCurrentRotation-StaticRef<float>(0x858FE4))+verticalOffset;
                    if (flags&1) {
                        const auto& forward=ped->GetMatrix().GetForward();
                        m_fVerticalAngle-=M16Asin(std::clamp(DotProduct(ped->field_578,forward),-1.f,1.f));
                        if (weapon==42) m_fVerticalAngle+=f(0x8D610C);
                    }
                }
            } else { m_fHorizontalAngle=(ped->m_fCurrentRotation-StaticRef<float>(0x858FE4))-verticalOffset; m_fVerticalAngle=CamAimVehicleResetPitch355(std::clamp(vehicle->GetMatrix().GetForward().z,-1.f,1.f),m_fVerticalAngle); }
        }
    }
    auto& forcedStep=StaticRef<float>(0xA44498);
    if (forcedStep>0.f) {
        float delta=m_fHorizontalAngle-f(0xA4449C);if (delta<0.f) delta+=StaticRef<float>(0x858CBC);
        if (delta<forcedStep || StaticRef<float>(0x858CBC)-delta<forcedStep) { m_fHorizontalAngle=f(0xA4449C);forcedStep=0.f; }
        else m_fHorizontalAngle+=delta<=StaticRef<float>(0x858CBC)-delta?-forcedStep:forcedStep;
    }
    CVector focal=targetPosition;
    ped->UpdateRpHAnim();
    focal.z=ped->GetPosition().z+.5f+f(row+16);
    if (m_fFOV<70.f) focal.z+=std::min((70.f-m_fFOV)/(70.f-f(0x8CC4B4)),1.f)*.1f;
    const float heightDifference=focal.z-targetPosition.z;
    float lateral=.2f;
    if (!info->flags.bAimWithArm && ped->GetPlayerData()->m_pPedClothesDesc->HasVisibleNewHairCut(1)) lateral=.3f;
    else if (m_fFOV<70.f) lateral=std::min((70.f-m_fFOV)/(70.f-f(0x8CC4B8)),1.f)*.1f+.2f;
    if (!StaticRef<bool>(0x8CCE64)) focal+=ped->GetMatrix().GetRight()*lateral;
    else {
        const CVector right=CrossProduct(m_vecFront,m_vecUp);
        const float weight=(1.f-std::asin(m_vecFront.z)*.63661975f)*lateral;
        focal+=right*weight;
    }
    if (auto* entity=ped->m_pTargetedObject) {
        CVector aim;
        if (entity->GetIsTypePed() && info->m_nWeaponFire!=WEAPON_FIRE_MELEE) static_cast<CPed*>(entity)->GetBonePosition(&aim,static_cast<eBoneTag>(3),true);
        else aim=entity->GetPosition();
        if (info->m_nWeaponFire==WEAPON_FIRE_MELEE) aim.z+=heightDifference*f(0x8CCE60);
        if (!m_bResetStatics && intelligence->GetTaskFighting()) { const float weight=CamPower(f(0x8CC39C),step);target=aim*(1.f-weight)+target*weight; }
        else target=aim;
    }
    auto* pad=CPad::GetPad(0);
    if (!ped->m_pTargetedObject) {
        if (!StaticRef<bool>(0xB6EC2E) || pad->DisablePlayerControls || (f(0xB73424)==0.f && f(0xB73428)==0.f)) {
            const float horizontal=-float(pad->AimWeaponLeftRight(ped)),vertical=float(pad->AimWeaponUpDown(ped));
            const float scale=m_fFOV*.0125f;
            const float betaInput=scale*f(0x859B50)*std::abs(horizontal)*horizontal*step*f(0x8CC4A0)*f(0x8CC4A0);
            const float alphaInput=scale*f(0x8631AC)*std::abs(vertical)*vertical*step*f(0x8CC4A0)*f(0x8CC4A0);
            const float weight=CamPower(std::abs(horizontal)<=2.f && std::abs(vertical)<=2.f?f(0x8CCE58):f(0x8CCE5C),step);
            float betaChange=(1.f-weight)*betaInput+weight*m_fBetaSpeed;
            float alphaChange=weight*m_fAlphaSpeed+(1.f-weight)*alphaInput;
            m_fBetaSpeed=betaChange;m_fAlphaSpeed=alphaChange;
            auto& following=StaticRef<bool>(0xB6EC44);
            auto& idle=StaticRef<int32>(0xB6EC48);
            auto& followHeading=StaticRef<float>(0x8CC530);
            if (driver) {
                if (horizontal==0.f && vertical==0.f) { if (!pad->GetWeapon(ped)) idle+=int32((step*f(0x858B38))*f(0x858C4C)); }
                else idle=0;
                if (idle<=StaticRef<int32>(0x8CC534)) {
                    if (idle<=StaticRef<int32>(0x8CC538)) { following=true;if (pad->GetWeapon(ped)) idle=0; }
                    else {
                        const float desired=ped->m_fCurrentRotation-StaticRef<float>(0x858FE4);
                        float delta=(desired-verticalOffset)-m_fHorizontalAngle;
                        if (delta>StaticRef<float>(0x858CBC)) delta-=StaticRef<float>(0x858CBC);else if (delta < -StaticRef<float>(0x858CBC)) delta+=StaticRef<float>(0x858CBC);
                        if (delta<.5235988f) { followHeading=desired+verticalOffset;idle=StaticRef<int32>(0x8CC534)+1;following=false; }
                        else following=true;
                    }
                } else { following=false;followHeading=(ped->m_fCurrentRotation-StaticRef<float>(0x858FE4))+verticalOffset; }
            } else if (inVehicle && vehicle && pad->GetEnterTargeting()) {
                auto& last=StaticRef<uint32>(0xB6EC4C);
                if (float(uint32(CTimer::GetTimeInMS()-last))>=f(0x8CCE54)) last=CTimer::GetTimeInMS();
                else { betaChange=StaticRef<float>(0x858CB8);alphaChange=0.f; }
            }
            if (following) { followHeading=-1001.f;m_fHorizontalAngle+=betaChange;m_fVerticalAngle+=alphaChange; }
            else {
                if (followHeading < -1000.f) followHeading=ped->m_fCurrentRotation-StaticRef<float>(0x858FE4);
                if (!info->flags.bAimWithArm && info->m_nWeaponFire!=WEAPON_FIRE_MELEE) {
                    const float heading=followHeading+StaticRef<float>(0x858FE4);
                    ped->m_fAimingRotation=ped->m_fCurrentRotation=heading;ped->SetHeading(heading);ped->UpdateRwMatrix();
                }
                float desired=followHeading-verticalOffset, maximum=step*f(0x8CC4A8),deadzone=0.f;
                if (driver) { maximum*=f(0x8CC4AC);deadzone=f(0x8CC4B0); }
                float delta=desired-m_fHorizontalAngle;
                if (delta>StaticRef<float>(0x858CB8)) desired-=StaticRef<float>(0x858CBC);else if (delta < -StaticRef<float>(0x858CB8)) desired+=StaticRef<float>(0x858CBC);
                delta=desired-m_fHorizontalAngle;
                if (deadzone>0.f) delta=delta>deadzone?delta-deadzone:delta < -deadzone?delta+deadzone:0.f;
                if (std::abs(delta)<maximum) { following=true;m_fHorizontalAngle+=delta; }
                else m_fHorizontalAngle+=delta<0.f?-maximum:maximum;
                if (driver) {
                    desired=M16Asin(std::clamp(vehicle->GetMatrix().GetForward().z,-1.f,1.f))+f(row+12);
                    delta=desired-m_fVerticalAngle;
                    if (delta>StaticRef<float>(0x858CB8)) desired-=StaticRef<float>(0x858CBC);else if (delta < -StaticRef<float>(0x858CB8)) desired+=StaticRef<float>(0x858CBC);
                    delta=desired-m_fVerticalAngle;
                    delta=delta>deadzone?delta-deadzone:delta < -deadzone?delta+deadzone:0.f;
                    m_fVerticalAngle+=std::abs(delta)<maximum?delta:delta<0.f?-maximum:maximum;
                } else m_fVerticalAngle+=alphaChange;
            }
        } else {
            m_fBetaSpeed=m_fAlphaSpeed=0.f;
            CamAimMouseAngles355(this);
        }
    } else {
        const CVector delta=target-focal;
        float horizontalAngle,flat,pitch;
        CamAimTargetAngles355(target,focal,horizontalAngle,flat,pitch);
        if (info->m_nWeaponFire==WEAPON_FIRE_MELEE) pitch*=std::cos(verticalOffset);
        else {
            const float distance=std::min(f(row),(focal-m_vecSource).Magnitude());
            const float targetDistance=std::sqrt(delta.z*delta.z+flat);
            const float multiplier=(targetDistance+distance)/targetDistance;
            horizontalOffset*=multiplier;verticalOffset*=multiplier;
        }
        float desiredBeta=horizontalAngle+verticalOffset,desiredAlpha=pitch-horizontalOffset;
        if (desiredAlpha < -StaticRef<float>(0x858CB8)) desiredAlpha+=StaticRef<float>(0x858CBC);else if (desiredAlpha>StaticRef<float>(0x858CB8)) desiredAlpha-=StaticRef<float>(0x858CBC);
        const float maximum=m_bResetStatics?1000.f:step*f(0x8CC4A4);
        float difference=desiredAlpha-m_fVerticalAngle;
        if (maximum<=std::abs(difference)) desiredAlpha=m_fVerticalAngle+(difference<0.f?-maximum:maximum);
        m_fVerticalAngle=desiredAlpha;
        difference=desiredBeta-m_fHorizontalAngle;
        if (difference>StaticRef<float>(0x858CB8)) desiredBeta-=StaticRef<float>(0x858CBC);else if (difference < -StaticRef<float>(0x858CB8)) desiredBeta+=StaticRef<float>(0x858CBC);
        difference=desiredBeta-m_fHorizontalAngle;
        if (maximum<=std::abs(difference)) desiredBeta=m_fHorizontalAngle+(difference<0.f?-maximum:maximum);
        m_fHorizontalAngle=desiredBeta;m_fAlphaSpeed=m_fBetaSpeed=0.f;
    }
    ClipBeta();
    m_fVerticalAngle=std::clamp(m_fVerticalAngle,-f(row+24),f(row+20));
    CamAimProjection355(this,focal,uint32(state)*28u);
    TheCamera.HandleCameraMotionForDuckingDuringAim(ped,&m_vecSource,&focal,false);
    m_vecTargetCoorsForFudgeInter=focal;
    CCamera::SetColVarsAimWeapon(state);
    if (StaticRef<int32>(0x8CC384)==3) {
        TheCamera.CameraGenericModeSpecialCases(ped);TheCamera.CameraPedAimModeSpecialCases(ped);
        TheCamera.CameraColDetAndReact(&m_vecSource,&focal);TheCamera.ImproveNearClip(nullptr,ped,&m_vecSource,&focal);
    }
    StaticRef<bool>(0xB6F042)=false;StaticRef<bool>(0xB6F043)=false;
    if (meleeBlend>0.f && ped->m_pTargetedObject) {
        const float weight=meleeBlend*.5f;
        m_vecFront=(focal*(1.f-weight)+target*weight)-m_vecSource;m_vecFront.Normalise();
    }
    GetVectorsReadyForRW();
    if ((!info->flags.bAimWithArm || (flags&0x4000000)) && info->m_nWeaponFire!=WEAPON_FIRE_MELEE && !inVehicle) {
        float heading=-1000.f;
        if (weapon==41) heading=CamAimHeading355(m_vecFront.x,m_vecFront.y,verticalOffset);
        else if (auto* aimed=ped->m_pTargetedObject) { const CVector delta=aimed->GetPosition()-ped->GetPosition();heading=CamAimHeading355(delta.x,delta.y,0.f); }
        else if (StaticRef<bool>(0xB6EC44)) heading=CamAimHeading355(m_vecFront.x,m_vecFront.y,verticalOffset);
        if (heading>-100.f) {ped->m_fCurrentRotation=ped->m_fAimingRotation=heading-.05f;ped->SetHeading(heading);ped->UpdateRwMatrix();}
        StaticRef<CPed*>(0xB6F980)->GetPlayerData()->m_fLookPitch=TheCamera.Find3rdPersonQuickAimPitch();
    }
    m_bResetStatics=false;
}


// 0x512B10
void CCam::Process_AttachedCam() {
    m_fFOV=70.0f;const float angle=StaticRef<float>(0xB6F978)*std::bit_cast<float>(0x3C8EFA35u);
    auto* attached=StaticRef<CEntity*>(0xB6F984);const auto& matrix=attached->GetMatrix();
    m_vecSource=matrix.TransformVector(StaticRef<CVector>(0xB6F960));m_vecSource+=attached->GetPosition();
    if(!StaticRef<uint8>(0xB6F054))m_vecFront=StaticRef<CEntity*>(0xB6F980)->GetPosition()-m_vecSource;
    else {m_vecFront=matrix.TransformVector(StaticRef<CVector>(0xB6F96C));m_vecFront+=attached->GetPosition();m_vecFront-=m_vecSource;}
    m_vecFront.Normalise();auto side=CrossProduct(m_vecFront,CVector{0,0,1});side.Normalise();auto vertical=CrossProduct(side,m_vecFront);vertical.Normalise();
    float water=0;
    if(CWaterLevel::GetWaterLevel(m_vecSource.x,m_vecSource.y,m_vecSource.z,water,1,nullptr)&&m_vecSource.z<water-0.3f){
        const auto velocity=StaticRef<CVector>(0xB7C508);const float length=std::sqrt((velocity.z*velocity.z+velocity.y*velocity.y)+velocity.x*velocity.x);const float maximum=StaticRef<float>(0x8CC7A8);const float scale=length>maximum?maximum/length:1.0f;
        StaticRef<int32>(0xB6F0B4)=int32(velocity.x*scale);StaticRef<int32>(0xB6F0B0)=int32(velocity.y*scale);StaticRef<int32>(0xB6F0AC)=int32(velocity.z*scale);StaticRef<int32>(0xB6F0B8)=2;StaticRef<uint32>(0xB6F0C8)=StaticRef<uint32>(0x8CC7A4);
    }
    const float sideX=side.x,sideY=side.y,sideZ=side.z,upX=vertical.x,upY=vertical.y,upZ=vertical.z;float sinX,sinY,sinZ,cosX,cosY,cosZ;
    CamAttachedUpCpp(angle, sideX, &sinX, sideY, &sinY, sideZ, &sinZ, upX, &cosX, upY, &cosY, upZ, &cosZ);
    m_vecUp=CVector{cosX+sinX,cosY+sinY,cosZ+sinZ};StaticRef<uint32>(0xB7CD68)=0;
}

namespace {
// Uncatalogued helpers used by both cooperative camera modes (0x513220/0x5132D0).
float CamPower(float base, float step) {
    float result;
    result = CnvDecayPowCpp(base, step);
    return result;
}
// Uncatalogued stick helpers used by the mouse follow camera (0x540E80/0x540F80).
int16 MouseCameraStick(const CPad* pad, bool vertical) {
    return vertical?pad->GetMouseCameraStickVertical():pad->GetMouseCameraStickHorizontal();
}

bool TwoPlayerCameraVisible(const CVector& source) {
    CColPoint point;
    CEntity* entity{};
    StaticRef<uint8>(0x8CCB80) = 5;
    if (CWorld::ProcessLineOfSight(source, StaticRef<CPed*>(0xB7CD98)->GetPosition(), point, entity, true, false, false, false, false, true, true, false)) return false;
    return !CWorld::ProcessLineOfSight(source, StaticRef<CPed*>(0xB7CF28)->GetPosition(), point, entity, true, false, false, false, false, true, true, false);
}
void TwoPlayerCameraGeometry(CCam* cam, float beta, CVector& source, CVector& front, CVector& target) {
    const float alpha = cam->m_fVerticalAngle;
    float directionX;
    float directionY;
    float directionZ;
    CamDirectionFromAnglesCpp(alpha, beta, &directionX, &directionY, &directionZ);
    front = CVector{directionX,directionY,directionZ};
    CVector flat{front.x,front.y,0.f}; flat.Normalise();
    const auto first = StaticRef<CPed*>(0xB7CD98)->GetPosition();
    const auto second = StaticRef<CPed*>(0xB7CF28)->GetPosition();
    const auto difference = first - second;
    const float distance = std::sqrt(difference.z * difference.z + difference.y * difference.y + difference.x * difference.x) * StaticRef<float>(0x8631C8) + StaticRef<float>(0x858F48);
    CVector separation{difference.x,difference.y,0.f}; separation.Normalise();
    const float weight = StaticRef<float>(0x858B8C) - (separation.y * flat.y + separation.x * flat.x + flat.z * separation.z) * StaticRef<float>(0x858C84);
    const float complement = 1.f - weight;
    target.x = weight * first.x + complement * second.x;
    target.y = weight * first.y + complement * second.y;
    target.z = weight * first.z + complement * second.z;
    source.x = target.x - distance * front.x;
    source.y = target.y - distance * front.y;
    source.z = target.z - distance * front.z;
    source.z = distance * StaticRef<float>(0x858B1C) + source.z;
}
bool TwoPlayerChangeCamera(const CPad* pad) {
    const auto* bytes = reinterpret_cast<const uint8*>(pad);
    const auto mode = *reinterpret_cast<const uint16*>(bytes + 0x10A);
    if (mode > 3) return false;
    const size_t offset = mode == 1 ? 0x10 : 0x1A;
    return *reinterpret_cast<const int16*>(bytes + offset) != 0 && *reinterpret_cast<const int16*>(bytes + offset + 0x30) == 0;
}
}

// 0x525E50
void CCam::Process_Cam_TwoPlayer() {
    auto& mode = StaticRef<uint32>(0x8A5E50);
    if (TwoPlayerChangeCamera(CPad::GetPad(0))) mode = ((mode != 0) - 1) & 2;
    else if (TwoPlayerChangeCamera(CPad::GetPad(1))) mode = (mode == 1) + 1;
    auto* first = StaticRef<CPed*>(0xB7CD98);
    auto* second = StaticRef<CPed*>(0xB7CF28);
    if (mode < 2) {
        auto* selected = mode == 0 ? first : second;
        if (selected->bInVehicle && selected->m_pVehicle) {
            m_pCamTargetEntity = selected->m_pVehicle;
            Process_FollowCar_SA(selected->m_pVehicle->GetPosition(),0.f,0.f,0.f,false);
            m_pCamTargetEntity = first;
        } else {
            if (mode == 1) m_pCamTargetEntity = second;
            Process_FollowPed_SA(selected->GetPosition(),0.f,0.f,0.f,false);
            if (mode == 1) m_pCamTargetEntity = first;
        }
        m_bResetStatics = false;
        return;
    }
    m_fVerticalAngle = std::bit_cast<float>(0xBF060A92u); m_fAlphaSpeed = 0.f;
    CVector candidateSource, candidateFront;
    float wantedBeta = m_fHorizontalAngle;
    uint32 attempt = 0;
    for (; attempt < 21; ++attempt) {
        const float offset = float(int32(attempt + 1) / 2) * .15f;
        wantedBeta = attempt % 2 == 0 ? m_fHorizontalAngle - offset : offset + m_fHorizontalAngle;
        TwoPlayerCameraGeometry(this,wantedBeta,candidateSource,candidateFront,m_vecTargetCoorsForFudgeInter);
        if (TwoPlayerCameraVisible(candidateSource)) { StaticRef<uint32>(0xB6EC24) = CTimer::GetTimeInMS(); break; }
    }
    if (attempt == 21) wantedBeta = m_fHorizontalAngle;
    if (attempt > 0) StaticRef<uint32>(0xB6EC28) = CTimer::GetTimeInMS();
    if (m_bResetStatics) m_fHorizontalAngle = wantedBeta;
    const auto wrap = [](float target,float current) { if (target > current + StaticRef<float>(0x858CB8)) return target - StaticRef<float>(0x858CBC); if (target < current - StaticRef<float>(0x858CB8)) return target + StaticRef<float>(0x858CBC); return target; };
    float desired = std::clamp((wrap(wantedBeta,m_fHorizontalAngle) - m_fHorizontalAngle) * .2f,-.1f,.1f) + m_fHorizontalAngle;
    float additional = 0.f;
    const auto now = CTimer::GetTimeInMS();
    if (attempt == 0 && now >= StaticRef<uint32>(0xB6EC28) + 1000) {
        const auto velocity = first->m_vecMoveSpeed + second->m_vecMoveSpeed;
        const float lengthSquared = velocity.y * velocity.y + velocity.x * velocity.x + velocity.z * velocity.z;
        if (lengthSquared > .01f) {
            float heading;
            const float headingX = -velocity.x;
            const float headingY = velocity.y;
            heading = Atan2MinusCpp(headingX, headingY, CamExeFloat(0x858FE4));
            heading = wrap(heading,desired);
            const float rate = StaticRef<float>(0x8CC5E8) * CTimer::GetTimeStep();
            const float limit = StaticRef<float>(0x8CC5EC) * CTimer::GetTimeStep();
            additional = std::clamp((heading - desired) * std::min(std::sqrt(lengthSquared) * rate,1.f),-limit,limit);
            if (additional > .01f) wantedBeta += .15f; else if (additional < .01f) wantedBeta -= .15f;
        } else wantedBeta -= .15f;
        if (std::abs(additional) > .01f) {
            TwoPlayerCameraGeometry(this,wantedBeta,candidateSource,candidateFront,m_vecTargetCoorsForFudgeInter);
            if (!TwoPlayerCameraVisible(candidateSource)) {
                if ((additional > 0.f && m_fBetaSpeed > 0.f) || (additional < 0.f && m_fBetaSpeed < 0.f)) m_fBetaSpeed = 0.f;
                StaticRef<uint32>(0xB6EC28) = now; additional = 0.f;
            }
        }
    }
    desired = wrap(additional + desired,m_fHorizontalAngle);
    float velocity = (desired - m_fHorizontalAngle) / std::max(CTimer::GetTimeStep(),1.f);
    float smoothing;
    smoothing = CnvDecayPowCpp(StaticRef<float>(0x8CC5E0), StaticRef<float>(0xB7CB5C));
    if (attempt == 0 && StaticRef<uint32>(0xB6EC28) + 1000 <= now) {
        const auto left = CPad::GetPad(0)->AimWeaponLeftRight(first);
        const auto right = CPad::GetPad(1)->AimWeaponLeftRight(second);
        const float input = std::clamp(float(-int32(left) - int32(right)),-128.f,128.f);
        additional = m_fFOV * StaticRef<float>(0x85A5C4) * StaticRef<float>(0x859B50) * std::abs(input) * StaticRef<float>(0x8CC4A0) * StaticRef<float>(0x8CC4A0) * input;
        if (additional > .01f) wantedBeta += .15f; else if (additional < .01f) wantedBeta -= .15f;
        if (std::abs(additional) > .01f) {
            TwoPlayerCameraGeometry(this,wantedBeta,candidateSource,candidateFront,m_vecTargetCoorsForFudgeInter);
            if (!TwoPlayerCameraVisible(candidateSource)) {
                if ((additional > 0.f && m_fBetaSpeed > 0.f) || (additional < 0.f && m_fBetaSpeed < 0.f)) m_fBetaSpeed = 0.f;
                StaticRef<uint32>(0xB6EC28) = now; additional = 0.f;
            }
        }
        velocity += additional;
    }
    const float limit = StaticRef<float>(0x8CC5E4);
    velocity = std::clamp(velocity,-limit,limit);
    m_fBetaSpeed = smoothing * m_fBetaSpeed + (1.f - smoothing) * velocity;
    m_fHorizontalAngle = m_fBetaSpeed * CTimer::GetTimeStep() + m_fHorizontalAngle;
    TwoPlayerCameraGeometry(this,m_fHorizontalAngle,m_vecSource,m_vecFront,m_vecTargetCoorsForFudgeInter);
    if (attempt == 21 && now - StaticRef<uint32>(0xB6EC24) > 500) {
        CColPoint point; CEntity* entity{}; StaticRef<uint8>(0x8CCB80) = 5;
        if (CWorld::ProcessLineOfSight(m_vecTargetCoorsForFudgeInter,m_vecSource,point,entity,true,false,false,false,false,true,true,false)) m_vecSource = point.m_vecPoint;
        if (StaticRef<uint32>(0x96A8B4) < now && StaticRef<uint32>(0x96A8B8) < 6) {
            CHud::SetHelpMessage(TheText.Get("WRN2_2P"),false,false,false);
            StaticRef<uint32>(0x96A8B4) = now + 60000; ++StaticRef<uint32>(0x96A8B8);
        }
    }
    m_vecUp = CVector{0.f,0.f,1.f}; m_vecUp.Normalise();
    auto right = CrossProduct(m_vecUp,m_vecFront); right.Normalise();
    m_vecUp = CrossProduct(m_vecFront,right); m_vecUp.Normalise();
    m_fFOV = 70.f; m_bResetStatics = false;
}

// 0x519810
static float CamShootingSourceZ355(float alpha, float radius, float targetZ) {
    float result;
    CamShootingSourceZCpp(alpha, radius, targetZ, &result);
    return result;
}
// Native 51A0C5..51A11D: sine and coordinate differences stay resident,
// cosine and rotated X use the prescribed float stores.
static void CamShootingOrbit355(CCam* cam, const CVector& target, float excess) {
    float scratch[0x34/4]{};
    scratch[0x1C/4]=target.x; scratch[0x20/4]=target.y; scratch[0x24/4]=target.z;
    scratch[0x28/4]=excess;
    auto* source=&cam->m_vecSource;
    CamShootingOrbitCpp(CamAddr(cam), CamAddr(source), CamAddr(scratch));
}
// Exact 519F1B..519FF2: magnitude/atan/mix/pow retain the native x87 stores.
static void CamShootingTilt355(CCam* cam, CVehicle* vehicle, float wantedAlpha) {
    float scratch[0x38/4]{};
    scratch[0x34/4]=wantedAlpha;
    CamShootingTiltCpp(CamAddr(cam), CamAddr(vehicle), CamAddr(scratch));
}
// Exact 5198D2..5199A1: dot and pow resident; all branches/stores preserved.
static void CamShootingFov355(CCam* cam, CVehicle* vehicle) {
    CamShootingFovCpp(CamAddr(cam), CamAddr(vehicle));
}
void CCam::Process_Cam_TwoPlayer_InCarAndShooting() {
    if (!m_pCamTargetEntity->GetIsTypeVehicle()) return;
    auto* first = StaticRef<CPed*>(0xB7CD98);
    auto* second = StaticRef<CPed*>(0xB7CF28);
    auto* vehicle = first->m_pVehicle;
    CVector target = vehicle->GetPosition();
    float vehicleHeading;
    auto* headingMatrix = *reinterpret_cast<CMatrix**>(reinterpret_cast<uint8*>(vehicle) + 0x14);
    const float headingInputX = headingMatrix ? -headingMatrix->GetForward().x : 0.f;
    const float headingInputY = headingMatrix ? headingMatrix->GetForward().y : 0.f;
    const float storedHeading = *reinterpret_cast<float*>(reinterpret_cast<uint8*>(vehicle) + 0x10);
    if (headingMatrix) {
        vehicleHeading = Atan2MinusCpp(headingInputX, headingInputY, CamExeFloat(0x858FE4));
    } else {
        CamStoredHeadingMinusHalfPiCpp(storedHeading, &vehicleHeading);
    }
    auto* pad = CPad::GetPad(vehicle->m_pDriver == first ? 1 : 0);
    auto* shooter = vehicle->m_pDriver == first ? second : first;
    const auto subclass = *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(vehicle) + 0x594);
    CamShootingFov355(this,vehicle);
    const float horizontal = float(pad->AimWeaponLeftRight(shooter));
    const float vertical = float(-int32(pad->AimWeaponUpDown(shooter)));
    const float absHorizontal = std::abs(horizontal), absVertical = std::abs(vertical);
    const float scale = m_fFOV * StaticRef<float>(0x85A5C4);
    m_fX_Targetting = scale * StaticRef<float>(0x859B50) * absHorizontal * CTimer::GetTimeStep() * .004f * .004f * horizontal + m_fX_Targetting;
    m_fY_Targetting = scale * StaticRef<float>(0x8631AC) * absVertical * CTimer::GetTimeStep() * .004f * .004f * vertical + m_fY_Targetting;
    auto& weapon = shooter->GetActiveWeapon();
    auto* info = CWeaponInfo::GetWeaponInfo(weapon.m_Type,shooter->GetWeaponSkill());
    float targetX, targetY;
    auto* aimTarget = CWeapon::FindNearestTargetEntityWithScreenCoors(m_fX_Targetting,m_fY_Targetting,info->m_fWeaponRange + info->m_fWeaponRange,shooter->GetPosition(),&targetX,&targetY);
    if (aimTarget && absHorizontal < 120.f && absVertical < 120.f) {
        const float limit = CTimer::GetTimeStep() * .01f;
        m_fX_Targetting = std::clamp((targetX - m_fX_Targetting) * (1.f - CamPower(StaticRef<float>(0x862F90),CTimer::GetTimeStep())),-limit,limit) + m_fX_Targetting;
        m_fY_Targetting = std::clamp((targetY - m_fY_Targetting) * (1.f - CamPower(StaticRef<float>(0x862F90),CTimer::GetTimeStep())),-limit,limit) + m_fY_Targetting;
    }
    float excessHorizontal = 0.f;
    if (m_fX_Targetting > .9f) { excessHorizontal = m_fX_Targetting - .9f; m_fX_Targetting = .9f; }
    if (m_fX_Targetting < -.9f) { excessHorizontal = m_fX_Targetting + .9f; m_fX_Targetting = -.9f; }
    if (m_fY_Targetting > .9f) { m_fVerticalAngle -= (m_fY_Targetting - .9f) * CTimer::GetTimeStep() * .5f; m_fY_Targetting = .9f; }
    if (m_fY_Targetting < -.9f) { m_fVerticalAngle -= (m_fY_Targetting + .9f) * CTimer::GetTimeStep() * .5f; m_fY_Targetting = -.9f; }
    if (absHorizontal < 1.f && absVertical < 1.f && !aimTarget) {
        const float x = m_fX_Targetting, y = m_fY_Targetting - -.4f;
        const float length = std::sqrt(x * x + y * y), step = CTimer::GetTimeStep() * .002f;
        if (step <= length) {
            const float reciprocal = 1.f / length;
            m_fX_Targetting -= step * reciprocal * x; m_fY_Targetting -= reciprocal * step * y;
        } else { m_fX_Targetting = 0.f; m_fY_Targetting = -.4f; }
    }
    const float height = vehicle->GetColModel()->m_boundBox.m_vecMax.z;
    float maxDistance = std::abs(vehicle->GetColModel()->m_boundBox.m_vecMin.y) * 2.f + .5f;
    float wantedAlpha = std::bit_cast<float>(0xBEB2B8C3u);
    if (uint32(vehicle->GetVehicleAppearance()) == 3 && (*reinterpret_cast<const uint8*>(reinterpret_cast<const uint8*>(vehicle) + 0x36) & 0xF8) != 0x40) {
        target.x = StaticRef<float>(0x8CC53C) * vehicle->GetMatrix().GetUp().x * height + target.x;
        target.y = StaticRef<float>(0x8CC53C) * vehicle->GetMatrix().GetUp().y * height + target.y;
        target.z = StaticRef<float>(0x8CC53C) * vehicle->GetMatrix().GetUp().z * height + target.z;
    } else {
        const float extra = StaticRef<float>(0x8CC600) * height - StaticRef<float>(0x8CC608);
        if (extra > 0.f) { target.z += extra; maxDistance += extra; wantedAlpha = (StaticRef<float>(0x8CCD1C) / maxDistance) * extra + wantedAlpha; }
    }
    const float minDistance = maxDistance * .9f; maxDistance += 2.f;
    m_fCaMinDistance = minDistance; m_fCaMaxDistance = maxDistance;
    const float deltaX = m_vecSource.x - target.x, deltaY = m_vecSource.y - target.y;
    float distance = std::sqrt(deltaY * deltaY + deltaX * deltaX);
    m_fDistanceBeforeChanges = distance;
    if (distance < .002f) distance = .002f;
    const float headingX = -(target.x - m_vecSource.x);
    const float headingY = target.y - m_vecSource.y;
    float heading;
    heading = Atan2MinusCpp(headingX, headingY, CamExeFloat(0x858FE4));
    m_fHorizontalAngle = heading;
    if (maxDistance < distance || distance < minDistance) {
        const float radius = maxDistance < distance ? maxDistance : minDistance;
        m_vecSource.x = deltaX * (radius / distance) + target.x; m_vecSource.y = (radius / distance) * deltaY + target.y;
    }
    CamShootingTilt355(this,vehicle,wantedAlpha);
    m_fVerticalAngle = std::clamp(m_fVerticalAngle,-1.0471976f,subclass == 3 ? .17453294f : .5235988f);
    m_vecSource.z = CamShootingSourceZ355(m_fVerticalAngle,maxDistance,target.z);
    RotCamIfInFrontCar(target,vehicleHeading);
    m_vecTargetCoorsForFudgeInter = target;
    const CVector oldSource = m_vecSource;
    // Existing CCamera dependency has no DLL definition; retain its canonical EXE ABI.
    plugin::CallMethod<0x514030>(&TheCamera,&oldSource,&m_vecTargetCoorsForFudgeInter,&m_vecSource,m_fFOV);
    CamShootingOrbit355(this,target,excessHorizontal);
    target -= m_vecSource; m_bResetStatics = false; m_vecFront = target;
    GetVectorsReadyForRW();
    if (!vehicle->CanPedLeanOut(shooter)) weapon.Update(shooter);
    if (pad->GetCarGunFired() && !vehicle->CanPedLeanOut(shooter) && !weapon.IsTypeMelee() && weapon.GetState() == WEAPONSTATE_READY) {
        CVector shotTarget;
        if (!aimTarget) {
            const auto right = CrossProduct(m_vecFront,m_vecUp);
            const float tangent = std::tan(m_fFOV * StaticRef<float>(0x8631D4));
            shotTarget = m_vecFront + right * (m_fX_Targetting * tangent) + m_vecUp * ((tangent / StaticRef<float>(0xC3EFA4)) * m_fY_Targetting);
            shotTarget *= info->m_fWeaponRange * 3.f; shotTarget += m_vecSource;
        } else shotTarget = aimTarget->GetPosition();
        const auto direction = shotTarget - m_vecSource;
        float angle = std::atan2(-direction.x,direction.y) - vehicle->GetHeading();
        if (angle > StaticRef<float>(0x858CB8)) angle -= StaticRef<float>(0x858CBC); else if (angle < -StaticRef<float>(0x858CB8)) angle += StaticRef<float>(0x858CBC);
        angle += StaticRef<float>(0x859AB0); if (angle < 0.f) angle += StaticRef<float>(0x858CBC);
        CTaskSimpleGangDriveBy task{nullptr,nullptr,100.f,100,eDrivebyStyle::AI_ALL_DIRN,shooter != vehicle->m_apPassengers[1]};
        task.m_pWeaponInfo = CWeaponInfo::GetWeaponInfo(weapon.m_Type,shooter->GetWeaponSkill());
        task.m_nFakeShootDirn = int8(int32(angle * StaticRef<float>(0x858FB8)));
        // Native51A3FD invokes the drive-by collaborator, not ProcessPed (62D3B0).
        plugin::CallMethod<0x627CC0>(&task,shooter);
        CamShakeNoPos(&TheCamera,StaticRef<float>(0x8CCD18));
    }
}

// 0x513510
static void CamTwoCarsBlend355(CCam* cam,const CVector* firstPos,const CVector* secondPos,const CVector* firstSource,const CVector* secondSource,const CVector* towardSecond,const CVector* towardFirst,const CVector* side,float blend) {
    float sine,weight;
    CamTwoCarsBlendWeightCpp(blend, &sine, &weight);
    float scratch[0x98/4]{};
    const auto put=[&](size_t offset,const CVector& value){std::memcpy(reinterpret_cast<uint8*>(scratch)+offset,&value,sizeof(value));};
    put(0x20,*firstSource);put(0x30,*secondSource);put(0x7C,*towardSecond);put(0x88,*towardFirst);put(0x64,*side);
    scratch[0x48/4]=sine;scratch[0x2C/4]=weight;
    CamTwoCarsBlendCpp(CamAddr(scratch), CamAddr(cam), CamAddr(firstPos), CamAddr(secondPos));
}
// 5135B0..513673: lifted Z values and scaled X/Z spill at native locations.
static void CamTwoCarsSources355(const CVector& first, const CVector& second, const CVector& between, float firstHeight, float secondHeight, CVector& firstSource, CVector& secondSource) {
    const float firstX=first.x,firstY=first.y,firstZ=first.z;
    const float secondX=second.x,secondY=second.y,secondZ=second.z;
    const float betweenX=between.x,betweenY=between.y,betweenZ=between.z;
    float liftedFirst,liftedSecond,scaledX,scaledZ,fx,fy,fz,sx,sy,sz;
    CamTwoCarsSourcesCpp(firstZ, firstHeight, &liftedFirst, secondZ, secondHeight, &liftedSecond, betweenX, &scaledX, firstX, &fx, secondX, &sx, betweenY, firstY, &fy, secondY, &sy, betweenZ, &scaledZ, &fz, &sz);
    firstSource=CVector{fx,fy,fz};secondSource=CVector{sx,sy,sz};
}
void CCam::Process_Cam_TwoPlayer_Separate_Cars() {
    m_fFOV = 80.f;
    auto* first = StaticRef<CPed*>(0xB7CD98)->m_pVehicle;
    auto* second = StaticRef<CPed*>(0xB7CF28)->m_pVehicle;
    CVector between = second->GetPosition() - first->GetPosition();
    between.Normalise();
    const auto firstPos = first->GetPosition(), secondPos = second->GetPosition();
    const auto firstHeight = CModelInfo::GetModelInfo(first->GetModelIndex())->GetColModel()->m_boundBox.m_vecMax.z;
    const auto secondHeight = CModelInfo::GetModelInfo(second->GetModelIndex())->GetColModel()->m_boundBox.m_vecMax.z;
    CVector firstSource,secondSource;
    CamTwoCarsSources355(firstPos,secondPos,between,firstHeight,secondHeight,firstSource,secondSource);
    CVector towardSecond = secondPos - firstSource; towardSecond.Normalise();
    CVector towardFirst = firstPos - secondSource; towardFirst.Normalise();
    CVector side{firstSource.y - secondSource.y, secondSource.x - firstSource.x, 0.f};
    side.Normalise(); side.z = -.1f; side.Normalise();
    auto& selected = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0x110);
    auto& blend = *reinterpret_cast<float*>(reinterpret_cast<uint8*>(this) + 0x114);
    CamTwoCarsBlend355(this,&firstPos,&secondPos,&firstSource,&secondSource,&towardSecond,&towardFirst,&side,blend);
    m_vecFront.Normalise();
    m_vecTargetCoorsForFudgeInter = selected == 0 ? secondPos : firstPos;
    m_vecUp = CVector{0.f,0.f,1.f}; m_vecUp.Normalise();
    auto right = CrossProduct(m_vecUp,m_vecFront); right.Normalise();
    m_vecUp = CrossProduct(m_vecFront,right); m_vecUp.Normalise();
    auto* selectedVehicle = StaticRef<CPed*>(0xB7CD98 + selected * 400)->m_pVehicle;
    auto* otherVehicle = StaticRef<CPed*>(0xB7CD98 + ((selected - 1u) & 1u) * 400)->m_pVehicle;
    CVector flat{m_vecFront.x,m_vecFront.y,0.f}; flat.Normalise();
    const auto& selectedSpeed = selectedVehicle->m_vecMoveSpeed;
    const float speed = flat.y * selectedSpeed.y + selectedSpeed.z * flat.z + selectedSpeed.x * flat.x;
    const auto& otherSpeed = otherVehicle->m_vecMoveSpeed;
    if (speed < -.13f && speed < -flat.x * otherSpeed.x + otherSpeed.y * -flat.y + otherSpeed.z * -flat.z) selected = (selected - 1u) & 1u;
    const float step = CTimer::GetTimeStep() * StaticRef<float>(0x8CCCB4);
    if (selected == 0) {
        const float next = blend - step;
        blend = next <= 0.f ? 0.f : next;
    } else {
        const float next = step + blend;
        blend = next >= 1.f ? 1.f : next;
    }
}

// 0x513BE0
void CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown() {
    m_fFOV = 80.0f;

    // Como el original (0x513BEE..0x513C1B): lee ped->m_pVehicle (+0x58C) en bruto, sin comprobar bInVehicle.
    CPlayerPed* ped1 = CWorld::Players[0].m_pPed;
    CPlayerPed* ped2 = CWorld::Players[1].m_pPed;
    CVehicle* vehicle1 = ped1->m_pVehicle;
    CVehicle* vehicle2 = ped2->m_pVehicle;
    CEntity*  entity1  = vehicle1 ? static_cast<CEntity*>(vehicle1) : static_cast<CEntity*>(ped1);
    CEntity*  entity2  = vehicle2 ? static_cast<CEntity*>(vehicle2) : static_cast<CEntity*>(ped2);
    const CVector& pos1 = entity1->GetPosition();
    const CVector& pos2 = entity2->GetPosition();

    // 0x513C35..0x513C60: centro = (suma) * 0.5; la suma de x es pos2.x + pos1.x
    const float half = CamExeFloat(0x858B8C);
    const float sumX = pos2.x + pos1.x;
    const float sumY = pos1.y + pos2.y;
    const float sumZ = pos1.z + pos2.z;
    const float centerX = sumX * half, centerY = sumY * half, centerZ = sumZ * half;

    // 0x513C79..0x513CA7: distancia con la suma de cuadrados en el orden z, y, x
    const float dx = pos1.x - pos2.x, dy = pos1.y - pos2.y, dz = pos1.z - pos2.z;
    const float distance = std::sqrt((dz * dz + dy * dy) + dx * dx);

    // 0x513CA7..0x513CD3: ambos en vehiculo 1.0; solo uno 0x858F34 (0.75); ninguno 0x8631D0 (0.45)
    float wanted;
    if (vehicle1) {
        wanted = vehicle2 ? CamExeFloat(0x858624) : CamExeFloat(0x858F34);
    } else {
        wanted = vehicle2 ? CamExeFloat(0x858F34) : CamExeFloat(0x8631D0);
    }

    // 0x513CD3..0x513D32: la multiplicacion se acerca al deseado a razon de ts * 0.005 (0x858B4C)
    const float difference = wanted - m_fCameraHeightMultiplier;
    const float maxStep = CTimer::GetTimeStep() * CamExeFloat(0x858B4C);
    const float magnitude = difference < 0.0f ? -difference : difference; // fcom 0 + test ah,5 + fchs
    if (magnitude < maxStep) {
        m_fCameraHeightMultiplier = wanted;
    } else if (difference >= 0.0f || difference != difference) { // jp: >= 0 o NaN
        m_fCameraHeightMultiplier = maxStep + m_fCameraHeightMultiplier;
    } else {
        m_fCameraHeightMultiplier = m_fCameraHeightMultiplier - maxStep;
    }

    // 0x513D38..0x513D5B: max(distancia + 10.0, 30.0) * multiplicador (ya actualizado)
    float height = distance + CamExeFloat(0x85862C);
    if (CamExeFloat(0x858CA4) > height) { // fld 30.0; fcomp st(1); test ah,41h; jne (un NaN se queda como esta)
        height = CamExeFloat(0x858CA4);
    }
    height *= m_fCameraHeightMultiplier;

    // 0x513D61..0x513E2F: angulo de inclinacion 0x8CCCB8 (0.4) con fsin/fcos del x87
    const float angle = CamExeFloat(0x8CCCB8);
    const float sinA = CamExeSin(angle);
    const float negCosA = -CamExeCos(angle);
    const float zero = CamExeFloat(0x858B50);
    const float sourceX = centerX - zero * height;
    m_vecSource = CVector{ sourceX, centerY - sinA * height, centerZ - negCosA * height };
    m_vecFront  = CVector{ 0.0f, sinA, negCosA };
    m_vecUp     = CVector{ 0.0f, -negCosA, sinA };
    m_vecTargetCoorsForFudgeInter = m_vecSource;
}

// 0x51B850
static float BirdyCameraEase(float ratio, float from, float to) {
    const float degrees270 = 270.f, degrees180 = 180.f, radians = std::bit_cast<float>(0x3C8EFA35u), one = 1.f, half = .5f;
    float value;
    CamEaseCpp(ratio, degrees180, degrees270, radians, one, half, to, from, &value);
    return value;
}

static float BirdyLampHeight355(float pointZ,float height,int randomValue) {
    const float randomScale=StaticRef<float>(0x858C7C),one=1.f;
    float result;
    CamBirdyLampHeightCpp(randomValue, randomScale, height, one, pointZ, &result);
    return result;
}

bool CCam::Process_DW_BirdyCam(bool) {
    StaticRef<uint8>(0xB6F059) = 0;
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->GetIsTypeVehicle()) return false;
    CEntity* target; CVehicle* vehicle;
    CVector dest, source, up, right, forward, velocity, angularVelocity;
    float speed, angularSpeed; CColSphere sphere;
    GetCoreDataForDWCineyCamMode(target, vehicle, dest, source, up, right, forward, velocity, speed, angularVelocity, angularSpeed, sphere);
    const uint32 now = CTimer::GetTimeInMS();
    auto& flags = StaticRef<uint32>(0xB70098);
    auto& first = StaticRef<CVector>(0xB70080);
    auto& second = StaticRef<CVector>(0xB7008C);
    auto& counter = StaticRef<int32>(0xB7007C);
    const auto constant = [](uintptr_t address) { return StaticRef<float>(address); };
    if (!(flags & 1)) { flags |= 1; } // EXE 0x404350 is a no-op default constructor.
    const int32 maximum = StaticRef<int32>(0x8CCD74);
    if (!(flags & 2)) { flags |= 2; counter = maximum; }
    auto& failed = StaticRef<uint8>(0xB6EC72);
    auto& start = StaticRef<uint32>(0x8CCBA0); auto& end = StaticRef<uint32>(0x8CCBA4);
    const auto eligible = [](CEntity* object) {
        const uint32 objectFlags = *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(object) + 0x1C);
        return (objectFlags & (4u | 0x40000u)) && object->GetMatrix().GetUp().z > .9f && IsLampPost(static_cast<eModelID>(object->m_nModelIndex));
    };
    const auto lampPoint = [](CEntity* object) {
        CVector point = object->GetMatrix().TransformPoint(object->GetColModel()->m_boundBox.m_vecMax);
        const float height = object->GetColModel()->m_boundBox.m_vecMax.z - object->GetColModel()->m_boundBox.m_vecMin.z * .5f;
        point.z=BirdyLampHeight355(point.z,height,std::rand());
        return point;
    };
    if (StaticRef<int32>(0x8CC488) != 0x3A || StaticRef<uint32>(0x8CCB9C) < CTimer::GetFrameCounter() - 1u) {
        StaticRef<int32>(0x8CC488) = 0x3A; end = StaticRef<uint32>(0x8CCBB0) + now; start = now; failed = 0; counter = maximum;
        gHandShaker[0].Reset();
        std::array<CEntity*,128> ahead{}, behind{}, firstLamps{}, secondLamps{}; int16 aheadCount{}, behindCount{}, firstCount{}, secondCount{};
        const CVector aheadCenter = (forward * constant(0x8CCD70)) * constant(0x8CCD6C) + dest;
        CWorld::FindObjectsInRange(aheadCenter, constant(0x8CCD70), true, &aheadCount, 127, ahead.data(), false, false, false, true, true);
        const CVector behindCenter = dest - (forward * constant(0x8CCD68)) * constant(0x8CCD64);
        CWorld::FindObjectsInRange(behindCenter, constant(0x8CCD68), true, &behindCount, 127, behind.data(), false, false, false, true, true);
        for (int16 i = 0; i < aheadCount; ++i) if (eligible(ahead[i])) firstLamps[firstCount++] = ahead[i];
        for (int16 i = 0; i < behindCount; ++i) if (eligible(behind[i])) secondLamps[secondCount++] = behind[i];
        if (firstCount < 1) { failed = 1; return false; }
        bool found = false;
        for (int16 i = 0; i < firstCount && !found; ++i) {
            if (!eligible(firstLamps[i])) continue;
            const CVector firstPoint = lampPoint(firstLamps[i]);
            CVector toward = firstPoint - dest; toward.Normalise(); const CVector nearTarget = toward + dest;
            if (constant(0x8CCD60) > 0.f) {
                for (int16 j = i; j < secondCount && !found; ++j) {
                    if (!eligible(secondLamps[j])) continue;
                    const CVector secondPoint = lampPoint(secondLamps[j]);
                    if (CWorld::GetIsLineOfSightClear(firstPoint, secondPoint, true, false, false, false, false, true, true) &&
                        CWorld::GetIsLineOfSightClear(secondPoint, nearTarget, true, false, false, false, false, true, true)) {
                        source = first = firstPoint; second = secondPoint; found = true;
                    }
                }
            }
        }
        if (!found) { failed = 1; return false; }
    }
    const float duration = float(int32(end - start));
    const float progress = float(int32(now - start)) / float(int32(duration));
    CVector direction = second - first; direction.Normalise();
    const CVector trim = direction * constant(0x8CCD5C);
    const CVector from = second - trim;
    CVector to{first.x + (trim.x + trim.x), first.y + (trim.y + trim.y), first.z + (trim.z + trim.z)};
    if (!failed) {
        source.x = BirdyCameraEase(progress, from.x, to.x);
        source.y = BirdyCameraEase(progress, from.y, to.y);
        source.z = BirdyCameraEase(progress, from.z, to.z);
    }
    if (IsTimeToExitThisDWCineyCamMode(0x16, source, dest, progress, false)) { failed = 1; return false; }
    CWorld::pIgnoreEntity = target;
    CColPoint point; CEntity* hit = nullptr;
    const bool blocked = CWorld::ProcessLineOfSight(dest, source, point, hit, true, true, false, false, false, false, false, false);
    CWorld::pIgnoreEntity = nullptr;
    if (blocked) {
        const int32 previous = counter--;
        if (previous == 0) { failed = 1; return false; }
    } else {
        const int32 previous = counter++;
        if (maximum < previous) counter = maximum;
    }
    Finalise_DW_CineyCams(source, dest, 0.f, 70.f, .3f, 0.f);
    return true;
}

static float PlaneSpotterEase(float ratio,float from,float to);

static float CamManFov(const CVector& target, const CVector& source, float progress) {
    const float distanceScale=StaticRef<float>(0x8CCD44), initial=StaticRef<float>(0x8CCD3C), final=StaticRef<float>(0x8CCD40);
    const float timeScale=StaticRef<float>(0x8CCD38), early=StaticRef<float>(0x8CCD34);
    const float zero=0.f, one=1.f, half=.5f, degrees180=180.f, degrees270=270.f, radians=std::bit_cast<float>(0x3C8EFA35u);
    float fov;
    CamManFovCpp(CamAddr(&target), CamAddr(&source), distanceScale, zero, one, degrees180, degrees270, radians, half, final, initial, &fov, progress, timeScale, early);
    return fov;
}

// 0x51B120
bool CCam::Process_DW_CamManCam(bool) {
    StaticRef<uint8>(0xB6F059)=0;
    if(!m_pCamTargetEntity||!m_pCamTargetEntity->GetIsTypeVehicle())return false;
    CEntity* entity;CVehicle* vehicle;CVector dest,src,up,right,forward,velocity,angular;float speed,angularSpeed;CColSphere sphere;
    GetCoreDataForDWCineyCamMode(entity,vehicle,dest,src,up,right,forward,velocity,speed,angular,angularSpeed,sphere);
    const auto now=CTimer::GetTimeInMS();auto& flags=StaticRef<uint32>(0xB70078);flags|=1;auto& counter=StaticRef<int32>(0xB70074);const int32 maximum=StaticRef<int32>(0x8CCD58);
    if(!(flags&2)){flags|=2;counter=maximum;}
    auto& mode=StaticRef<int32>(0x8CC488);auto& start=StaticRef<uint32>(0x8CCBA0);auto& end=StaticRef<uint32>(0x8CCBA4);auto& failed=StaticRef<uint8>(0xB6EC71);
    if(mode!=0x39||gLastFrameProcessedDWCineyCam<CTimer::GetFrameCounter()-1u){
        mode=0x39;end=StaticRef<uint32>(0x8CCBAC)+now;counter=maximum;failed=0;start=now;gHandShaker[0].Reset();
        const float range=StaticRef<float>(0x8CCD54);const CVector center{forward.x*range+dest.x,forward.y*range+dest.y,forward.z*range+dest.z};
        CEntity* objects[16];int16 count=0;CWorld::FindObjectsInRange(center,range,true,&count,15,objects,false,false,false,true,true);
        if(count<1){failed=1;return false;}
        float best=10000.0f;CEntity* chosen=nullptr;
        for(int16 i=0;i<count;++i){auto* object=objects[i];const uint32 objectFlags=*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(object)+0x1C);
            if(!(objectFlags&(4u|0x40000u))||!(object->GetMatrix().GetUp().z>.9f)||!IsLampPost(eModelID(int16(object->GetModelIndex()))))continue;
            const auto relative=object->GetPosition()-dest;const float distance=std::sqrt(relative.y*relative.y+relative.x*relative.x);
            if(!(distance<best&&distance>StaticRef<float>(0x8CCD50)))continue;
            CVector point=object->GetMatrix().TransformPoint(object->GetColModel()->m_boundBox.m_vecMax);
            point.z-=object->GetColModel()->m_boundBox.m_vecMax.z;
            point.z=object->GetColModel()->m_boundBox.m_vecMin.z*.5f+point.z;
            CVector direction=point-dest;direction.Normalise();direction+=dest;
            if(StaticRef<float>(0x8CCD4C)>0&&CWorld::GetIsLineOfSightClear(point,direction,true,false,false,false,false,true,true)){
                chosen=object;best=distance;src=point;StaticRef<CVector>(0xB70068)=point;
            }
        }
        if(!chosen){failed=1;return false;}
    }
    const int32 elapsed=int32(now-start),duration=int32(end-start);float progress;
    progress = IntRatioCpp(elapsed, duration);
    if(!failed){src=StaticRef<CVector>(0xB70068);CVector direction=dest-src;direction.Normalise();const float offset=StaticRef<float>(0x8CCD48);src=CVector{direction.x*offset+src.x,direction.y*offset+src.y,direction.z*offset+src.z};}
    const float fov=CamManFov(dest,src,progress);
    if(IsTimeToExitThisDWCineyCamMode(21,src,dest,progress,false)){failed=1;return false;}
    CColPoint collision;CEntity* hit;CWorld::pIgnoreEntity=entity;
    const bool blocked=CWorld::ProcessLineOfSight(dest,src,collision,hit,true,true,false,false,false,false,false,false);CWorld::pIgnoreEntity=nullptr;
    const int32 previous=counter;
    if(!blocked){counter=int32(uint32(previous)+1u);if(previous>maximum)counter=maximum;}
    else {counter=int32(uint32(previous)-1u);if(previous==0){failed=1;return false;}}
    float shake=std::sqrt((velocity.y*velocity.y+velocity.z*velocity.z)+velocity.x*velocity.x)*StaticRef<float>(0x8CCD30);
    const float minimum=StaticRef<float>(0x8CCD2C);shake=shake<=minimum?minimum:shake;if(shake<0)shake=0;else if(shake>1)shake=1;
    Finalise_DW_CineyCams(src,dest,0,fov,10.0f-(fov*std::bit_cast<float>(0x3C6A0EA1u))*9.7f,shake);
    return true;
}

static float HeliChaseInitialFov(float progress, float duration, float from, float to) {
    const float one = 1.f, half = .5f, degrees180 = 180.f, degrees270 = 270.f;
    const float radians = std::bit_cast<float>(0x3C8EFA35u);
    float result;
    CamHeliInitialFovCpp(one, duration, progress, degrees180, degrees270, radians, half, to, from, &result);
    return result;
}

static float HeliChaseReturnFov355(int32 now, int32 start, int32 end, float from, float to) {
    const float zero = StaticRef<float>(0x858B50), one = StaticRef<float>(0x858624);
    const float degrees = StaticRef<float>(0x85A994), phase = StaticRef<float>(0x859070);
    const float radians = StaticRef<float>(0x8595EC), half = StaticRef<float>(0x858B8C);
    float result;
    CamHeliReturnFovCpp(start, now, end, zero, one, degrees, phase, radians, half, to, from, &result);
    return result;
}

static float HeliChaseDistanceFov355(const CVector& camera, const CVector& dest, float progress, float duration, float from, float to, bool initial, float nearDistance, float farDistance, float reduction) {
    const float zero=StaticRef<float>(0x858B50),one=StaticRef<float>(0x858624),half=StaticRef<float>(0x858B8C);
    const float degrees=StaticRef<float>(0x85A994),phase=StaticRef<float>(0x859070),radians=StaticRef<float>(0x8595EC);
    float distance,result;
    CamHeliDistanceFovCpp(to, initial, one, duration, progress, degrees, phase, radians, half, from, zero, CamAddr(&camera), CamAddr(&dest), &distance, nearDistance, farDistance, reduction, &result);
    return result;
}

// 0x51A740
bool CCam::Process_DW_HeliChaseCam(bool) {
    StaticRef<uint8>(0xB6F059) = 0;
    // The shipped selector multiplies by the mutable zero at 0x858B50.
    const int32 selection = static_cast<int32>(static_cast<int64>(float(std::rand() & 0xFFFF) * StaticRef<float>(0x858B14) * StaticRef<float>(0x858B50)));
    const uintptr_t state = 0xB6FEC0 - uintptr_t(selection * 0x9C);
    const auto number = [state](size_t offset) -> float& { return StaticRef<float>(state + offset); };
    const auto integer = [state](size_t offset) -> int32& { return StaticRef<int32>(state + offset); };
    const auto flag = [state](size_t offset) -> uint8& { return StaticRef<uint8>(state + offset); };
    const auto vector = [state](size_t offset) -> CVector& { return StaticRef<CVector>(state + offset); };
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->GetIsTypeVehicle()) return false;
    CEntity* target; CVehicle* vehicle;
    CVector dest, source, up, right, forward, velocity, angularVelocity;
    float speed, angularSpeed; CColSphere sphere;
    GetCoreDataForDWCineyCamMode(target, vehicle, dest, source, up, right, forward, velocity, speed, angularVelocity, angularSpeed, sphere);
    const uint32 now = CTimer::GetTimeInMS();
    auto& failed = StaticRef<uint8>(0xB6EC70);
    const auto resetState = [&] {
        integer(0x44) = integer(0x48); number(0x7C) = number(0x2C) - number(0x30);
        flag(0x40) = flag(0x5C) = flag(0x80) = flag(0x81) = flag(0x88) = 0;
        integer(0x64) = integer(0x60);
    };
    const auto clearLine = [&](const CVector& from, const CVector& to) {
        CWorld::pIgnoreEntity = target;
        CColPoint point; CEntity* hit = nullptr;
        const bool blocked = CWorld::ProcessLineOfSight(from, to, point, hit, true, true, false, false, false, false, false, false);
        CWorld::pIgnoreEntity = nullptr;
        return !blocked;
    };
    if (StaticRef<int32>(0x8CC488) != 0x38 || StaticRef<uint32>(0x8CCB9C) < CTimer::GetFrameCounter() - 1u) {
        StaticRef<int32>(0x8CC488) = 0x38; StaticRef<uint32>(0x8CCBA4) = StaticRef<uint32>(0x8CCBA8) + now;
        StaticRef<uint32>(0x8CCBA0) = now; failed = 0;
        reinterpret_cast<void(__thiscall*)(void*)>(0x50E180)(reinterpret_cast<void*>(state));
        resetState();
        reinterpret_cast<void(__cdecl*)()>(0x50E090)();
        gHandShaker[0].Reset();
        bool found = false;
        for (int32 attempt = 0; attempt < integer(0x58); ++attempt) {
            vector(0x0C) = dest - forward * number(0x1C);
            vector(0) = forward * number(0x18) + dest;
            vector(0x0C).z += number(0x20); vector(0).z += number(0x20);
            const float firstSign = std::rand() < 0x3FFF ? -1.f : 1.f;
            const float secondSign = std::rand() < 0x3FFF ? -1.f : 1.f;
            const CVector firstOffset{right.x * number(0x24) * firstSign, right.y * number(0x24) * firstSign, (number(0x24) * 0.f) * firstSign};
            vector(0x0C) += firstOffset;
            const CVector secondOffset{right.x * number(0x24) * secondSign, right.y * number(0x24) * secondSign, secondSign * (number(0x24) * 0.f)};
            vector(0) += secondOffset;
            if (!CWorld::TestSphereAgainstWorld(vector(0x0C), number(0x78), nullptr, true, true, false, false, false, false) && clearLine(dest, vector(0x0C))) {
                flag(0x81) = std::rand() < 0x3FFF; flag(0x88) = std::rand() < 0x3FFF; found = true; break;
            }
        }
        if (!found) { resetState(); failed = 1; return false; }
    }
    if (failed) return false;
    const float progress = float(int32(now - StaticRef<uint32>(0x8CCBA0))) / float(int32(StaticRef<uint32>(0x8CCBA4) - StaticRef<uint32>(0x8CCBA0)));
    CVector camera = (vector(0) - vector(0x0C)) * progress + vector(0x0C);
    const float prediction = number(0x34);
    dest.x = forward.x * speed * prediction + forward.x + dest.x;
    dest.y = forward.y * speed * prediction + forward.y + dest.y;
    dest.z = (forward.z * speed) * prediction + forward.z + dest.z;
    CVector horizontal = dest - camera; horizontal.z = 0.f;
    const float horizontalDistance = std::sqrt(horizontal.x * horizontal.x + horizontal.y * horizontal.y);
    const float inverseDistance = 1.f / horizontalDistance;
    horizontal.x *= inverseDistance; horizontal.y *= inverseDistance;
    if (horizontalDistance < number(0x74)) {
        camera.x = dest.x - horizontal.x * number(0x74);
        camera.y = dest.y - horizontal.y * number(0x74);
    }
    float fov = HeliChaseDistanceFov355(camera, dest, progress, number(0x28), number(0x2C), number(0x30), progress < number(0x28) && !flag(0x81), number(0x68), number(0x6C), number(0x70));
    const float roll = progress * number(0x38);
    if (flag(0x5C) || CWorld::TestSphereAgainstWorld(camera, StaticRef<float>(0x8CCD28), nullptr, true, true, false, false, false, false)) {
        StaticRef<uint32>(0xB70064) |= 1;
        if (!flag(0x5C)) { StaticRef<CVector>(0xB70058) = camera; flag(0x5C) = 1; StaticRef<int32>(0x8CCD24) = 100; }
        auto& countdown = StaticRef<int32>(0x8CCD24);
        if (countdown < 0) { --countdown; failed = 1; return false; }
        const auto previous = StaticRef<CVector>(0xB70058);
        camera = (camera - previous) * StaticRef<float>(0x8CCD20) + previous;
        --countdown;
    }
    const auto startReturn = [&] {
        number(0x84) = fov; flag(0x88) = 1;
        integer(0x8C) = int32(now); integer(0x90) = integer(0x98) + int32(now);
    };
    if (!flag(0x40)) {
        if (!clearLine(dest, camera)) {
            flag(0x80) = 1;
            if (!flag(0x88) && integer(0x64) < integer(0x60) / 4) startReturn();
            const int32 previous = integer(0x64)--;
            if (previous == 0) { vector(0x4C) = dest; flag(0x40) = 1; }
        } else {
            integer(0x64) = std::min(integer(0x64) + 1, integer(0x60));
        }
    } else {
        dest = vector(0x4C);
        const int32 previous = integer(0x44)--;
        if (previous == 0) { failed = 1; return false; }
    }
    if (!flag(0x88) && number(0x94) <= progress) startReturn();
    if (flag(0x88)) {
        fov = HeliChaseReturnFov355(int32(now), integer(0x8C), integer(0x90), number(0x84), number(0x2C));
    }
    if (IsTimeToExitThisDWCineyCamMode(0x14, camera, dest, progress, false)) { failed = 1; return false; }
    Finalise_DW_CineyCams(camera, dest, roll, fov, number(0x3C), 1.f);
    return true;
}

namespace {
CVector PlaneOrbitWave(const CVector& up, float amplitude, int32 cycles, int32 now, int32 start, int32 end) {
    const float radians=std::bit_cast<float>(0x3C8EFA35u),degrees=360.0f;
    float componentX=up.x*amplitude,componentY=up.y*amplitude,componentZ=up.z*amplitude;
    float resultX,resultY,resultZ;
    CamPlaneOrbitWaveCpp(start, now, end, cycles, degrees, radians, componentX, &resultX, componentY, &resultY, componentZ, &resultZ);
    return CVector{resultX,resultY,resultZ};
}
bool ProcessPlaneOrbit(CCam* cam, bool second) {
    StaticRef<uint8>(0xB6F059)=0;
    if(!cam->m_pCamTargetEntity||cam->m_pCamTargetEntity->GetType()!=ENTITY_TYPE_VEHICLE)return false;
    CEntity* entity;CVehicle* vehicle;CVector dest,src,up,right,forward,velocity,angular;float speed,angularSpeed;CColSphere sphere;
    cam->GetCoreDataForDWCineyCamMode(entity,vehicle,dest,src,up,right,forward,velocity,speed,angular,angularSpeed,sphere);
    const auto now=CTimer::GetTimeInMS();auto& failed=StaticRef<uint8>(second?0xB6EC77:0xB6EC76);
    if(dest.z<StaticRef<float>(second?0x8CCDDC:0x8CCDBC)){failed=1;return false;}
    auto& mode=StaticRef<int32>(0x8CC488);auto& start=StaticRef<uint32>(0x8CCBA0);auto& end=StaticRef<uint32>(0x8CCBA4);
    CColPoint point;CEntity* hit;
    if(mode!=(second?0x3F:0x3E)||gLastFrameProcessedDWCineyCam<CTimer::GetFrameCounter()-1u){
        mode=second?0x3F:0x3E;end=StaticRef<uint32>(second?0x8CCBC4:0x8CCBC0)+now;failed=0;start=now;
        CWorld::pIgnoreEntity=entity;const bool blocked=CWorld::ProcessLineOfSight(dest,src,point,hit,true,true,false,false,false,false,false,false);CWorld::pIgnoreEntity=nullptr;
        if(blocked){failed=1;return false;}
        StaticRef<float>(second?0xB700D0:0xB700C0)=1;
        if(std::rand()<0x3FFF)StaticRef<float>(second?0xB700D0:0xB700C0)=-1;
        if(second){StaticRef<float>(0xB700CC)=1;if(std::rand()<0x3FFF)StaticRef<float>(0xB700CC)=-1;}
        if(std::rand()<0x3FFF)StaticRef<float>(second?0x8CCDD8:0x8CCDB4)=-1;
    }else if(!second&&TheCamera.GetRoughDistanceToGround()<StaticRef<float>(0x8CCDB8)){failed=1;return false;}
    const int32 elapsed=int32(now-start),duration=int32(end-start);float progress;
    progress = IntRatioCpp(elapsed, duration);
    forward.Normalise();right.Normalise();
    if(second){forward*=((1.0f-2.0f*progress)*StaticRef<float>(0xB700CC));right*=((1.0f-progress)*StaticRef<float>(0x8CCDD8));}
    else right*=StaticRef<float>(0x8CCDB4);
    src=dest+forward*StaticRef<float>(second?0x8CCDD4:0x8CCDB0);
    src+=right*StaticRef<float>(second?0x8CCDD0:0x8CCDAC);
    src+=up*StaticRef<float>(second?0x8CCDCC:0x8CCDA8);
    if(!second)src+=up*StaticRef<float>(0x8CCDA4)*(progress-0.5f)*StaticRef<float>(0xB700C0);
    src+=PlaneOrbitWave(up,StaticRef<float>(second?0x8CCDC4:0x8CCD9C),StaticRef<int32>(second?0x8CCDC8:0x8CCDA0),int32(now),int32(start),int32(end));
    CWorld::pIgnoreEntity=entity;const bool blocked=CWorld::ProcessLineOfSight(dest,src,point,hit,true,true,false,false,false,false,false,false);CWorld::pIgnoreEntity=nullptr;
    auto& flags=StaticRef<uint32>(second?0xB700C8:0xB700BC);auto& counter=StaticRef<int32>(second?0xB700C4:0xB700B8);int32 previous=counter;const auto maximum=StaticRef<int32>(second?0x8CCDC0:0x8CCD98);
    if(!(flags&1)){flags|=1;previous=maximum;}
    if(!blocked){counter=int32(uint32(previous)+1u);if(previous>maximum)counter=maximum;}
    else {counter=int32(uint32(previous)-1u);if(previous==0){failed=1;return false;}}
    if(cam->IsTimeToExitThisDWCineyCamMode(second?27:26,src,dest,progress,false)){failed=1;return false;}
    cam->Finalise_DW_CineyCams(src,dest,0,70,5,1);return true;
}
}
// 0x51C760
bool CCam::Process_DW_PlaneCam1(bool) { return ProcessPlaneOrbit(this,false); }
// 0x51CC30
bool CCam::Process_DW_PlaneCam2(bool) { return ProcessPlaneOrbit(this,true); }

// 0x51D100
bool CCam::Process_DW_PlaneCam3(bool) {
    StaticRef<uint8>(0xB6F059)=0;
    if(!m_pCamTargetEntity||m_pCamTargetEntity->GetType()!=ENTITY_TYPE_VEHICLE)return false;
    CEntity* entity;CVehicle* vehicle;
    CVector dest,src,up,right,forward,velocity,angular;
    float speed,angularSpeed;
    CColSphere sphere;
    GetCoreDataForDWCineyCamMode(entity,vehicle,dest,src,up,right,forward,velocity,speed,angular,angularSpeed,sphere);
    const auto now=CTimer::GetTimeInMS();
    auto& failed=StaticRef<uint8>(0xB6EC78);
    if(dest.z<StaticRef<float>(0x8CCDEC)){failed=1;return false;}
    auto& mode=StaticRef<int32>(0x8CC488);
    auto& start=StaticRef<uint32>(0x8CCBA0);
    auto& end=StaticRef<uint32>(0x8CCBA4);
    CColPoint point;CEntity* hit;
    if(mode!=0x40||gLastFrameProcessedDWCineyCam<CTimer::GetFrameCounter()-1u){
        mode=0x40;end=StaticRef<uint32>(0x8CCBC8)+now;failed=0;start=now;
        CWorld::pIgnoreEntity=entity;
        const bool blocked=CWorld::ProcessLineOfSight(dest,src,point,hit,true,true,false,false,false,false,false,false);
        CWorld::pIgnoreEntity=nullptr;
        if(blocked){failed=1;return false;}
    }
    const int32 elapsed=int32(now-start),duration=int32(end-start);
    float progress;
    progress = IntRatioCpp(elapsed, duration);
    CWorld::pIgnoreEntity=entity;
    const auto& box=CModelInfo::GetModelInfo(int16(entity->GetModelIndex()))->GetColModel()->m_boundBox;
    auto& height=StaticRef<float>(0x8CCDE8);
    height=(box.m_vecMax.y-box.m_vecMin.y)*0.5f;height+=height;
    forward=CVector{forward.x*height+dest.x,dest.y+forward.y*height,forward.z*height+dest.z};
    const float offset=StaticRef<float>(0x8CCDE4);
    const float zOffset=up.z*offset;
    src=CVector{up.x*offset+forward.x,forward.y+up.y*offset,forward.z+zOffset};
    const bool blocked=CWorld::ProcessLineOfSight(dest,src,point,hit,true,true,false,false,false,false,false,false);
    CWorld::pIgnoreEntity=nullptr;
    auto& flags=StaticRef<uint32>(0xB700D8);
    auto& counter=StaticRef<int32>(0xB700D4);
    int32 previous=counter;
    const int32 maximum=StaticRef<int32>(0x8CCDE0);
    if(!(flags&1)){flags|=1;previous=maximum;}
    if(!blocked){counter=int32(uint32(previous)+1u);if(previous>maximum)counter=maximum;}
    else {counter=int32(uint32(previous)-1u);if(previous==0){failed=1;return false;}}
    if(IsTimeToExitThisDWCineyCamMode(28,src,dest,progress,false)){failed=1;return false;}
    Finalise_DW_CineyCams(src,dest,0,70,5,1);
    return true;
}

static float PlaneSpotterEase(float ratio,float from,float to) {if(ratio<0)ratio=0;else if(ratio>1)ratio=1;const float degrees270=270,degrees180=180,radians=std::bit_cast<float>(0x3C8EFA35u),one=1,half=.5f;float value;
        CamEaseCpp(ratio, degrees180, degrees270, radians, one, half, to, from, &value);
        return value;
}

static float PlaneSpotterRandomCoordinate(int randomValue, float range, float halfRange, float origin) {
    const float randomScale=StaticRef<float>(0x858C7C);
    float result;
    CamPlaneSpotterRandomCpp(randomValue, randomScale, range, halfRange, origin, &result);
    return result;
}

// 0x51C250
bool CCam::Process_DW_PlaneSpotterCam(bool) {
    StaticRef<uint8>(0xB6F059)=0;if(!m_pCamTargetEntity||!m_pCamTargetEntity->GetIsTypeVehicle())return false;
    CEntity* entity;CVehicle* vehicle;CVector dest,src,up,right,forward,velocity,angular;float speed,angularSpeed;CColSphere sphere;
    GetCoreDataForDWCineyCamMode(entity,vehicle,dest,src,up,right,forward,velocity,speed,angular,angularSpeed,sphere);
    const auto now=CTimer::GetTimeInMS();auto& flags=StaticRef<uint32>(0xB700B4);flags|=1;
    auto& mode=StaticRef<int32>(0x8CC488);auto& start=StaticRef<uint32>(0x8CCBA0);auto& end=StaticRef<uint32>(0x8CCBA4);auto& failed=StaticRef<uint8>(0xB6EC73);auto& position=StaticRef<CVector>(0xB700A8);
    CColPoint point;CEntity* hit;
    if(mode!=0x3B||gLastFrameProcessedDWCineyCam<CTimer::GetFrameCounter()-1u){
        mode=0x3B;end=StaticRef<uint32>(0x8CCBB4)+now;failed=0;start=now;
        bool blocked=false;const int32 tries=StaticRef<int32>(0x8CCD94);
        for(int32 i=0;i<tries;++i){
            const float range=StaticRef<float>(0x8CCD8C),half=range*.5f;
            src.z=dest.z-StaticRef<float>(0x8CCD90);
            src.x=PlaneSpotterRandomCoordinate(std::rand(),range,half,dest.x);
            src.y=PlaneSpotterRandomCoordinate(std::rand(),range,half,dest.y);
            CWorld::pIgnoreEntity=entity;blocked=CWorld::ProcessLineOfSight(dest,src,point,hit,true,true,false,false,false,false,false,false);CWorld::pIgnoreEntity=nullptr;
            if(blocked){position=CVector{point.m_vecPoint.x,point.m_vecPoint.y,point.m_vecPoint.z+2.0f};break;}
        }
        if(tries>0&&!blocked){failed=1;return false;}
        StaticRef<uint8>(0xB700A4)=uint8(std::rand()<0x3FFF);
    }
    const int32 elapsed=int32(now-start),duration=int32(end-start);float progress;
    progress = IntRatioCpp(elapsed, duration);
    src=position;float fov=70;
    const auto delta=position-dest;const float planar=std::sqrt(delta.y*delta.y+delta.x*delta.x);
    if(!(planar>=5.0f)){failed=1;return false;}
    if(StaticRef<uint8>(0xB700A4)){
        const float distance=std::sqrt((delta.z*delta.z+delta.y*delta.y)+delta.x*delta.x);
        fov=PlaneSpotterEase(distance/StaticRef<float>(0x8CCD88),StaticRef<float>(0x8CCD80),StaticRef<float>(0x8CCD84));
        if(progress<StaticRef<float>(0x8CCD7C)){if(!(flags&2)){flags|=2;StaticRef<float>(0xB700A0)=StaticRef<float>(0x8CCD80);}fov=PlaneSpotterEase(progress/StaticRef<float>(0x8CCD7C),StaticRef<float>(0xB700A0),fov);}
    }
    CWorld::pIgnoreEntity=entity;const bool blocked=CWorld::ProcessLineOfSight(dest,src,point,hit,true,true,false,false,false,false,false,false);CWorld::pIgnoreEntity=nullptr;
    auto& counter=StaticRef<int32>(0xB7009C);int32 previous=counter;const auto maximum=StaticRef<int32>(0x8CCD78);if(!(flags&4)){flags|=4;previous=maximum;}
    if(!blocked){counter=int32(uint32(previous)+1u);if(previous>maximum)counter=maximum;}else {counter=int32(uint32(previous)-1u);if(previous==0){failed=1;return false;}}
    if(IsTimeToExitThisDWCineyCamMode(23,src,dest,progress,false)){failed=1;return false;}
    const float clip=10.0f-(fov*std::bit_cast<float>(0x3C6A0EA1u))*9.7f;
    Finalise_DW_CineyCams(src,dest,0,fov,clip,1);return true;
}

// 0x50F3F0 - debug
void CCam::Process_Editor(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static auto& s_LookAtAngle     = StaticRef<float>(0xB6FFE4);
    static auto& s_DoRenderShadows = StaticRef<bool>(0xB7295A);

    if (m_bResetStatics) {
        m_vecSource = CVector{ 796.0f, -937.0f, 40.0f }; // 0x44470000, 0xC46A4000, 0x42200000
        if (m_pCamTargetEntity) {
            CEntity::SafeCleanUpRef(m_pCamTargetEntity); // 0x571A00
        }
        m_pCamTargetEntity = nullptr;
    }
    m_bResetStatics = false; // 0x50F453 (fuera del if)
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.3f);
    m_fFOV = 70.0f;

    // Como el original (0x50F468..0x50F4CF): inclinacion += (stickY * 0x8595EC) * 0.02; giro += (stickX * 0x8631A8) * 0x8631A4
    const int16 stickX = CPad::GetPad(1)->GetLeftStickX();
    const int16 stickY = CPad::GetPad(1)->GetLeftStickY();
    m_fVerticalAngle   = ((float)stickY * CamExeFloat(0x8595EC)) * CamExeFloat(0x858B38) + m_fVerticalAngle;
    m_fHorizontalAngle = ((float)stickX * CamExeFloat(0x8631A8)) * CamExeFloat(0x8631A4) + m_fHorizontalAngle;

    // 0x50F4D5..0x50F514: posicion de la entidad objetivo o, sin ella, la propia fuente
    const CVector lookAt = m_pCamTargetEntity ? m_pCamTargetEntity->GetPosition() : m_vecSource;

    // 0x50F518..0x50F5E6: limites +-1.5620698 (0x8631A0, 0x8630F4); la logica del pad solo corre si no se pasa del limite inferior
    if (m_fVerticalAngle > CamExeFloat(0x8631A0)) {
        m_fVerticalAngle = std::bit_cast<float>(0x3FC7F1E7u);
    }
    if (m_fVerticalAngle < CamExeFloat(0x8630F4)) {
        m_fVerticalAngle = std::bit_cast<float>(0xBFC7F1E7u);
    } else {
        if (CPad::GetPad(1)->IsSquareDown()) {
            s_LookAtAngle = s_LookAtAngle + CamExeFloat(0x858B1C);
        } else if (CPad::GetPad(1)->IsCrossDown()) {
            s_LookAtAngle = s_LookAtAngle - CamExeFloat(0x858B1C);
        } else {
            s_LookAtAngle = 0.0f;
        }
    }
    // 0x50F57F..0x50F5E6: limite +-70.0 (0x858CE0, 0x86319C)
    if (s_LookAtAngle > CamExeFloat(0x858CE0)) {
        s_LookAtAngle = std::bit_cast<float>(0x428C0000u);
    } else if (s_LookAtAngle < CamExeFloat(0x86319C)) {
        s_LookAtAngle = std::bit_cast<float>(0xC28C0000u);
    }

    // 0x50F5E6..0x50F660: front = (lookAt - source) normalizado; source += lookAtAngle * front
    m_vecFront = CVector{ lookAt.x - m_vecSource.x, lookAt.y - m_vecSource.y, lookAt.z - m_vecSource.z };
    m_vecFront.Normalise();
    m_vecSource.x = s_LookAtAngle * m_vecFront.x + m_vecSource.x;
    m_vecSource.y = s_LookAtAngle * m_vecFront.y + m_vecSource.y;
    m_vecSource.z = s_LookAtAngle * m_vecFront.z + m_vecSource.z;
    if (m_vecSource.z < CamExeFloat(0x863198)) { // -450.0: fcomp + test ah,5 + jp
        m_vecSource.z = std::bit_cast<float>(0xC3E10000u);
    }

    // 0x50F680..0x50F720: boton R2 pulsado => lleva al vehiculo del jugador (Teleport virtual) o escribe la posicion en el jugador
    if (CPad::GetPad(1)->IsRightShoulder2Pressed()) {
        if (auto* vehicle = FindPlayerVehicle(-1, false)) {
            vehicle->Teleport(m_vecSource, false);
        } else {
            auto* player = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
            CVector& position = player->m_matrix ? player->m_matrix->GetPosition() : player->m_placement.m_vPosn;
            position.x = m_vecSource.x;
            position.y = m_vecSource.y;
            position.z = m_vecSource.z;
        }
    }

    // 0x50F720..0x50F847: envuelve x e y de la fuente cuando (v * 0.02 + 60.0) sale de [5.0, 115.0]
    const auto wrapSource = [](float& value) {
        const auto scaled = [&] { return value * CamExeFloat(0x858B38) + CamExeFloat(0x858B34); };
        if (scaled() > CamExeFloat(0x863194)) {
            do {
                value = value - CamExeFloat(0x858624);
            } while (scaled() > CamExeFloat(0x863194));
        }
        if (scaled() < CamExeFloat(0x858C80)) {
            do {
                value = value + CamExeFloat(0x858624);
            } while (scaled() < CamExeFloat(0x858C80));
        }
    };
    wrapSource(m_vecSource.x);
    wrapSource(m_vecSource.y);

    GetVectorsReadyForRW();

    // Como el original (0x50F858..0x50F8B2): sombra salvo que se pulse el stick izquierdo (pulsacion nueva)
    if (!CPad::GetPad(1)->IsLeftShockPressed() && s_DoRenderShadows) {
        CShadows::StoreShadowToBeRendered(
            eShadowType::SHADOW_ADDITIVE,
            gpShadowExplosionTex,
            m_vecSource,
            {12.0f, 0.0f},
            {0.0f, -12.0f},
            128,
            128,
            128,
            128,
            1000.0f,
            false,
            1.0f,
            nullptr,
            false
        );
    }

    // Como el original (0x50F8B2..0x50F954): con m_Wants_To_Draw_Hud el EXE solo formatea con sprintf tres textos
    // de depuracion en un buffer local (nunca se muestran); el NOTSA_LOG_DEBUG de la DLL se elimina.
}

// Como el original (0x51D610..0x51D6D9): la mezcla submarina que Process_Fixed lleva en linea escribe directamente los
// dwords de TheCamera (0xB6F0B4 rojo, 0xB6F0B0 verde, 0xB6F0AC azul, 0xB6F0C8 valor, 0xB6F0B8 tipo), sin llamar a SetMotionBlur.
static void CamApplyUnderwaterBlurInlined() {
    const float waterRed   = CamExeFloat(0xB7C508);
    const float waterGreen = CamExeFloat(0xB7C50C);
    const float waterBlue  = CamExeFloat(0xB7C510);
    const float limit      = CamExeFloat(0x8CC7A8);
    const float magnitude  = std::sqrt((waterBlue * waterBlue + waterGreen * waterGreen) + waterRed * waterRed);
    int32 red, green, blue;
    if (magnitude > limit) { // fcom + test ah,41h + jne
        const float scale = limit / magnitude;
        red   = (int32)(int64)(waterRed * scale);
        green = (int32)(int64)(waterGreen * scale);
        blue  = (int32)(int64)(waterBlue * scale);
    } else {
        red   = (int32)(int64)waterRed;
        green = (int32)(int64)waterGreen;
        blue  = (int32)(int64)waterBlue;
    }
    StaticRef<int32>(0xB6F0B4) = red;
    StaticRef<int32>(0xB6F0B0) = green;
    StaticRef<int32>(0xB6F0AC) = blue;
    StaticRef<int32>(0xB6F0C8) = *reinterpret_cast<const int32*>(0x8CC7A4);
    StaticRef<int32>(0xB6F0B8) = 2;
}

// 0x51D470
void CCam::Process_Fixed(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    if (m_nDirectionWasLooking != 3) {
        m_nDirectionWasLooking = 3; // todo: enum
    }

    m_vecSource = m_vecCamFixedModeSource;
    m_vecFront  = CVector{ target.x - m_vecSource.x, target.y - m_vecSource.y, target.z - m_vecSource.z };
    m_vecFront.Normalise();
    m_vecTargetCoorsForFudgeInter = target;
    GetVectorsReadyForRW();

    // Como el original (0x51D507..0x51D5A8): el "arriba" provisional se construye en m_vecUp
    m_vecUp = CVector{ 0.0f, 0.0f, 1.0f };
    m_vecUp.x = m_vecUp.x + m_vecCamFixedModeUpOffSet.x;
    m_vecUp.y = m_vecCamFixedModeUpOffSet.y + m_vecUp.y;
    m_vecUp.z = m_vecCamFixedModeUpOffSet.z + m_vecUp.z;
    m_vecUp.Normalise();
    const auto a = CrossProduct(m_vecFront, m_vecUp).Normalized();
    m_vecUp = CrossProduct(a, m_vecFront);

    m_fFOV = 70.0f;
    // Como el original (0x51D5B5..0x51D5C9): si el byte 0xB6F05E vale 1, el FOV es el float de 0xB6F158
    if (StaticRef<uint8>(0xB6F05E) == 1) {
        m_fFOV = StaticRef<float>(0xB6F158);
    }

    if (float wl{}; CWaterLevel::GetWaterLevel(m_vecSource, wl, true) && m_vecSource.z < wl) {
        CamApplyUnderwaterBlurInlined();
    }

    if (gAllowScriptedFixedCameraCollision) {
        const auto savedIgnoreEntity = CWorld::pIgnoreEntity;

        CWorld::pIgnoreEntity = FindPlayerVehicle();
        CVector out{};
        float   outDist{1.0f};
        if (TheCamera.ConeCastCollisionResolve(m_vecSource, target, out, 2.0f, 0.1f, outDist)) {
            m_vecSource = out; // 0x51D73E..0x51D74F: se copian x, y y z
        }

        CWorld::pIgnoreEntity = savedIgnoreEntity;
    }
}

// 0x5B25F0
void CCam::Process_FlyBy(const CVector&, float, float, float) {
    if(StaticRef<uint8>(0xB6F045))return;
    m_vecUp=CVector{0,0,1};
    auto* fovPath=StaticRef<float*>(0xB6F988);auto* rollPath=StaticRef<float*>(0xB6F98C);auto* sourcePath=StaticRef<float*>(0xB6F990);auto* targetPath=StaticRef<float*>(0xB6F994);
    auto& fovIndex=StaticRef<int32>(0xBC4084);auto& rollIndex=StaticRef<int32>(0xBC4088);auto& sourceIndex=StaticRef<int32>(0xBC4080);auto& targetIndex=StaticRef<int32>(0xBC407C);
    if(!StaticRef<uint8>(0xB6F05B)){
        m_fTimeElapsedFloat=0;const int32 count=int32(sourcePath[0]);m_nFinishTime=uint32(int32(sourcePath[count*10-9]*1000.0f));
        fovIndex=rollIndex=5;sourceIndex=targetIndex=11;StaticRef<uint8>(0xB6F05B)=1;StaticRef<float>(0xBC4074)=fovPath[2];StaticRef<float>(0xBC4078)=fovPath[2];
    }else m_fTimeElapsedFloat+=(CTimer::GetTimeStepNonClipped()*0.02f)*1000.0f;
    const uint32 integerTime=uint32(int32(m_fTimeElapsedFloat));
    const float time=float(integerTime),finish=float(m_nFinishTime);
    const bool done=finish<=time;
    if(done){sourceIndex=(int32(sourcePath[0])-1)*10+1;targetIndex=(int32(targetPath[0])-1)*10+1;rollIndex=int32(rollPath[0])*4-3;fovIndex=int32(fovPath[0])*4-3;}
    else StaticRef<float>(0xB6F168)=time/finish;
    const auto advance=[&](const float* path,int32& index,int32 step){if(!done)while((path[index]-path[1])*1000.0f<=time)index+=step;};
    advance(sourcePath,sourceIndex,10);CameraPathInterpolateVector(&m_vecSource,sourcePath,time,&sourceIndex);
    advance(targetPath,targetIndex,10);CameraPathInterpolateVector(&m_vecFront,targetPath,time,&targetIndex);
    float roll=0;advance(rollPath,rollIndex,4);CameraPathInterpolateScalar(&roll,rollPath,time,&rollIndex);
    const float radians=std::bit_cast<float>(0x3C8EFA35u),halfPi=std::bit_cast<float>(0x3FC90FDBu);float upX,upZ;
    CamFlyByUpCpp(roll, radians, halfPi, &upX, &upZ);
    m_vecUp.x=upX;m_vecUp.z=upZ;
    advance(fovPath,fovIndex,4);CameraPathInterpolateScalar(&StaticRef<float>(0xBC4074),fovPath,time,&fovIndex);
    if(done){StaticRef<float>(0xB6F168)=1;sourceIndex=targetIndex=rollIndex=fovIndex=0;}
    m_vecTargetCoorsForFudgeInter=m_vecFront;m_vecFront-=m_vecSource;m_vecFront.Normalise();
    const auto side=CrossProduct(m_vecUp,m_vecFront);m_vecUp=CrossProduct(m_vecFront,side);m_vecUp.Normalise();m_fFOV=StaticRef<float>(0xBC4074);
}

// 0x5245B0
static float CamFollowCarBetaBlend355(float betaInput,float betaCorrection,float maximumSpeed,float smoothing,float previous) {
    float result,negativeMaximum;
    CamFollowCarBetaBlendCpp(betaInput, betaCorrection, maximumSpeed, &negativeMaximum, smoothing, previous, &result);
    return result;
}

// Native reset history 524CCF..524D87: X/Y products stay resident;
// Z product spills before subtracting focal Z.
static CVector CamFollowCarHistory355(const CVector& focal,const CVector& front,float radius) {
    const float focalX=focal.x,focalY=focal.y,focalZ=focal.z;
    const float frontX=front.x,frontY=front.y,frontZ=front.z;
    float outputX,outputY,outputZ,scaledZ;
    CamFollowCarHistoryCpp(radius, frontX, frontY, frontZ, &scaledZ, focalX, &outputX, focalY, &outputY, focalZ, &outputZ);
    return CVector{outputX,outputY,outputZ};
}
// Native525C21..525D48: alpha cosine spills, beta sine remains resident.
static void CamFollowCarFinalHistory355(CCam* cam,const CVector& focal,float alpha,float historyDistance,float distance) {
    float scratch[0x90/4]{};
    scratch[0x14/4]=alpha;scratch[0x2C/4]=historyDistance;scratch[0x7C/4]=distance;
    scratch[0x48/4]=focal.x;scratch[0x4C/4]=focal.y;scratch[0x50/4]=focal.z;
    float cosineAlpha;
    cosineAlpha = TrigSimpleCosCpp(alpha);
    scratch[0x8C/4]=cosineAlpha;
    CamFollowCarFinalHistoryCpp(CamAddr(cam), CamAddr(scratch));
}
// Exact524E30..525006: history/velocity headings, dot/projection, limit
// and beta correction keep the native resident order and float spills.
static float CamFollowCarBetaCorrection355(CCam* cam,CVehicle* vehicle,uint32 rowOffset) {
    float scratch[0x90/4]{};
    *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(scratch)+0x34)=rowOffset;
    auto* front=&cam->m_vecFront;
    CamFollowCarBetaCorrectionCpp(CamAddr(cam), CamAddr(vehicle), CamAddr(scratch), CamAddr(front));
    return scratch[0x44/4];
}

static float CamFollowCarActualDistance355(const CVector& focal, const CVector& history) {
    const CVector* origin=&focal;const CVector* previous=&history;float result;
    CamFollowCarActualDistanceCpp(CamAddr(origin), CamAddr(previous), &result);
    return result;
}

static float CamFollowCarAutoAlpha355(float base,float step,float wanted,float previous,float maximum) {
    float result;
    CamFollowCarAutoAlphaCpp(base, step, wanted, previous, &result, maximum);
    return result;
}
// Native5259A1..525A29: product/sum precision, limits and cutoff.
static void CamFollowCarAlphaBlend355(CCam* cam,float smoothing,float alphaInput,float maximumSpeed) {
    float scratch[0x90/4]{};
    scratch[0x3C/4]=smoothing;scratch[0x10/4]=alphaInput;scratch[0x18/4]=maximumSpeed;
    float complement;
    CamFollowCarAlphaComplementCpp(smoothing, &complement);
    scratch[0x8C/4]=complement;
    CamFollowCarAlphaBlendCpp(CamAddr(cam), CamAddr(scratch));
}
void CCam::Process_FollowCar_SA(const CVector& targetPosition,float,float,float,bool preserveAngles) {
    if (!m_pCamTargetEntity->GetIsTypeVehicle()) return;
    auto* vehicle=static_cast<CVehicle*>(m_pCamTargetEntity);
    auto* pad=CPad::GetPad(vehicle->m_pDriver && vehicle->m_pDriver->m_nPedType==PED_TYPE_PLAYER2?1:0);
    const auto f=[](uintptr_t address){return StaticRef<float>(address);};
    const auto raw=[](const void* object,size_t offset){return static_cast<const uint8*>(object)+offset;};
    const float step=CTimer::GetTimeStep();
    TheCamera.ApplyVehicleCameraTweaks(vehicle);
    const int model=vehicle->GetModelIndex();
    const int type=*reinterpret_cast<const int32*>(raw(vehicle,0x590)),subclass=*reinterpret_cast<const int32*>(raw(vehicle,0x594));
    uint32 state=0;
    if (model==441 || model==464 || model==564 || model==594) state=5;
    else if (model==465 || model==501) state=6;
    else if (type==9 || subclass==2) state=1;
    else if (subclass==3) state=2;
    else if (subclass==4) {
        if (model==520 && int32(*reinterpret_cast<const uint16*>(raw(vehicle,0x86C)))>=int32(StaticRef<int16>(0x8D33C8))) state=2;
        else state=uint32((model==539)-1)&3;
    } else if (subclass==5) state=4;
    const uintptr_t row=0x8CC600+state*60;
    float distance=f(0xB6F0E8)+f(row+4);
    int arrayPosition=0;
    TheCamera.GetArrPosForVehicleType(static_cast<eVehicleType>(vehicle->GetVehicleAppearance()),arrayPosition);
    const bool fixedStatus=(*reinterpret_cast<const uint8*>(raw(vehicle,0x36))&0xF8)==0x40;
    float verticalOffset=0.f;
    const int zoom=StaticRef<int32>(0xB6F0DC);
    if (fixedStatus) verticalOffset=f(0x8CC430+arrayPosition*4);
    else if (zoom==1) verticalOffset=f(0x8CC41C+arrayPosition*4);
    else if (zoom==2) verticalOffset=f(0x8CC430+arrayPosition*4);
    else if (zoom==3) verticalOffset=f(0x8CC444+arrayPosition*4);
    float height=vehicle->GetColModel()->m_boundBox.m_vecMax.z;
    const auto* widthModel=vehicle->GetColModel();
    float width=std::abs(widthModel->m_boundBox.m_vecMin.y)+std::abs(widthModel->m_boundBox.m_vecMin.y);
    auto& towingBlend=StaticRef<float>(0xB7011C);
    CVector focal=targetPosition;
    auto* trailer=vehicle->m_pVehicleBeingTowed;
    if (!trailer) {
        if (subclass==9 || subclass==2) {
            if (!vehicle->m_apPassengers[0]) { if (towingBlend>0.f) towingBlend=std::max(towingBlend-step*f(0x8CCEE8),0.f); }
            else if (towingBlend<1.f) towingBlend=std::min(step*f(0x8CCEE8)+towingBlend,1.f);
            height+=f(0x8CCEE0)*towingBlend;
        } else towingBlend=0.f;
    } else {
        if (towingBlend<1.f) towingBlend=std::min(step*f(0x8CCEE8)+towingBlend,1.f);
        const auto minimum=vehicle->GetColModel()->m_boundBox.m_vecMin;
        const auto maximum=vehicle->GetColModel()->m_boundBox.m_vecMax;
        width+=(maximum-minimum).Magnitude()*f(0x8CCEE4)*towingBlend;
        const float maximumZ=std::max(vehicle->GetColModel()->m_boundBox.m_vecMax.z,height);
        height=(maximumZ-height)*towingBlend+height;
        const float weight=towingBlend*.5f;
        focal=focal*(1.f-weight)+trailer->GetPosition()*weight;
    }
    distance+=f(0xB6FD70)*width;
    const float minimumDistance=f(0xB6FD70)*width*f(row+12);
    if (vehicle->GetVehicleAppearance()==VEHICLE_APPEARANCE_HELI && !fixedStatus) focal+=vehicle->GetMatrix().GetUp()*f(0x8CC53C)*height;
    else {
        const float excess=height*f(row)-f(row+8);
        if (excess>0.f) {focal.z+=excess;distance+=excess;verticalOffset+=(f(0x8CCEDC)/distance)*excess;}
    }
    focal.z*=f(0xB6FD74);verticalOffset+=f(0xB6FD78);
    float historyDistance=f(row+16);
    if (zoom==1 && (state==0 || state==1)) historyDistance*=.65f;
    historyDistance=std::max(historyDistance,distance);
    m_fCaMaxDistance=distance;m_fCaMinDistance=3.5f;
    const bool reset=m_bResetStatics;
    const auto& matrix=vehicle->GetMatrix();
    if (!reset) {
        const float speed=vehicle->m_vecMoveSpeed.x*matrix.GetForward().x+matrix.GetForward().y*vehicle->m_vecMoveSpeed.y+matrix.GetForward().z*vehicle->m_vecMoveSpeed.z;
        if ((subclass==0 || subclass==9) && speed>f(0x8CC540)) m_fFOV+=(speed-f(0x8CC540))*step;
        if (m_fFOV>70.f) m_fFOV=(m_fFOV-70.f)*CamPower(f(0x8CC544),step)+70.f;
        m_fFOV=std::clamp(m_fFOV,70.f,100.f);
    } else m_fFOV=70.f;
    if (reset || StaticRef<bool>(0xB6F042) || StaticRef<bool>(0xB6F043)) {
        m_bResetStatics=false;m_bRotating=false;m_bCollisionChecksOn=true;StaticRef<bool>(0xB6F999)=true;
        if (!StaticRef<bool>(0xB6F050) && !preserveAngles) {
            m_fVerticalAngle=0.f;m_fHorizontalAngle=CameraHeadingMinusHalfPI(vehicle);
            if (StaticRef<bool>(0xB6F043)) m_fHorizontalAngle+=StaticRef<float>(0x858CB8);
        }
        m_fBetaSpeed=m_fAlphaSpeed=0.f;m_fDistance=1000.f;
        m_vecFront=M16Direction(m_fHorizontalAngle,m_fVerticalAngle);
        m_avecTargetHistoryPos[0]=CamFollowCarHistory355(focal,m_vecFront,historyDistance);m_anTargetHistoryTime[0]=CTimer::GetTimeInMS();
        m_avecTargetHistoryPos[1]=CamFollowCarHistory355(focal,m_vecFront,distance);m_nCurrentHistoryPoints=0;
        if (!StaticRef<bool>(0xB6F050) && !preserveAngles) m_fVerticalAngle=-verticalOffset;
    }
    m_vecFront=focal-m_avecTargetHistoryPos[0];m_vecFront.Normalise();
    const float actualDistance=CamFollowCarActualDistance355(focal,m_avecTargetHistoryPos[1]);
    const auto& velocity=vehicle->m_vecMoveSpeed;
    const float betaCorrection=CamFollowCarBetaCorrection355(this,vehicle,state*60u);
    float historyPitch=M16Asin(std::clamp(m_vecFront.z,-1.f,1.f));
    if (actualDistance<distance && minimumDistance<distance) distance=std::max(minimumDistance,actualDistance);
    float upper=f(row+52);
    const bool heightCorrection=velocity.SquaredMagnitude()<.040000003f && (type!=9 || *reinterpret_cast<const uint8*>(raw(vehicle,0x804))>3) && subclass!=3 && (subclass!=4 || *reinterpret_cast<const uint8*>(raw(vehicle,0x960))!=0);
    if (heightCorrection) {
        CVector right=CrossProduct(matrix.GetForward(),CVector{0,0,1});right.Normalise();
        CVector upward=CrossProduct(right,matrix.GetForward());upward.Normalise();
        if (upward.x*m_vecFront.x+upward.z*m_vecFront.z+upward.y*m_vecFront.y>0.f) {
            const auto& box=vehicle->GetColModel()->m_boundBox;
            const float elevation=(focal.z-vehicle->GetPosition().z)+vehicle->GetHeightAboveRoad();
            const float heading=vehicle->GetHeading()-StaticRef<float>(0x858FE4);
            const float folded=M16Asin(std::abs(std::sin(m_fHorizontalAngle-heading)));
            float sideDistance;
            if (folded<=std::atan2(box.m_vecMax.x,-box.m_vecMin.y)) sideDistance=(f(0x8CCED4)-box.m_vecMin.y)/std::cos(folded);
            else sideDistance=(f(0x8CCED8)+box.m_vecMax.x)/std::cos(std::max(0.f,StaticRef<float>(0x858FE4)-folded));
            const float pitch=std::atan2(matrix.GetForward().z,std::sqrt(matrix.GetForward().y*matrix.GetForward().y+matrix.GetForward().x*matrix.GetForward().x));
            upper=std::atan2(elevation,sideDistance*f(0x8CCED0))+pitch*std::cos(m_fHorizontalAngle-(vehicle->GetHeading()-StaticRef<float>(0x858FE4)));
            if (type==0 && *reinterpret_cast<const uint8*>(raw(vehicle,0x960))>1 && std::abs(DotProduct(vehicle->m_vecTurnSpeed,matrix.GetForward()))<.05f) {
                const float roll=std::atan2(matrix.GetRight().z,std::sqrt(matrix.GetRight().y*matrix.GetRight().y+matrix.GetRight().x*matrix.GetRight().x));
                upper+=roll*std::cos((m_fHorizontalAngle-(vehicle->GetHeading()-StaticRef<float>(0x858FE4)))+StaticRef<float>(0x858FE4));
            }
        }
    }
    float idealAlpha=std::clamp(historyPitch-verticalOffset,-f(row+56),upper);
    const float maximumAlpha=step*f(row+24);
    const float automaticAlpha=CamFollowCarAutoAlpha355(f(row+20),step,idealAlpha,m_fVerticalAngle,maximumAlpha);
    const float horizontal=-float(pad->AimWeaponLeftRight(nullptr));
    float vertical=float(pad->AimWeaponUpDown(nullptr));
    if (StaticRef<bool>(0xB6EC2E)) vertical=0.f;
    const float inputScale=m_fFOV*.0125f;
    float betaInput=inputScale*.071428575f*std::abs(horizontal)*horizontal*f(0x8CC4A0)*f(0x8CC4A0);
    float alphaInput=inputScale*.042857144f*std::abs(vertical)*vertical*f(0x8CC4A0)*f(0x8CC4A0);
    bool restricted=true;
    if (model==443 || model==486 || model==406 || model==524 || model==592 || model==520 || model==525 || model==530 || model==531) alphaInput=0.f;
    else if (model==564 || (type==0 && (*reinterpret_cast<const uint32*>(raw(vehicle,0x38C))&0x20000))) alphaInput=betaInput=0.f;
    else restricted=false;
    if (StaticRef<int32>(0x8CC384)!=3) alphaInput=betaInput=0.f;
    if (state==0) {
        const float steering=float(pad->GetSteeringUpDown());
        auto* driver=vehicle->m_pDriver;
        if (std::abs(steering)>120.f && driver && driver->GetIntelligence()->m_TaskMgr.GetActiveTask()) {
            if (int(driver->GetIntelligence()->m_TaskMgr.GetActiveTask()->GetTaskType())!=704) {
                const float activeSteering=float(pad->GetSteeringUpDown());
                alphaInput+=inputScale*.042857144f*std::abs(activeSteering)*activeSteering*f(0x8CC4A0)*f(0x8CC4A0)*.5f;
            }
        }
    }
    if (alphaInput>0.f) alphaInput*=.5f;
    bool mouseActive=false;
    auto& mouseTimer=StaticRef<float>(0xB70118);
    if (StaticRef<bool>(0xB6EC2E) && !pad->DisablePlayerControls) {
        const float mouseY=f(0xB73428)+f(0xB73428),mouseX=f(0xB73424)*-2.f;
        const bool allow=StaticRef<bool>((subclass==3 || subclass==4)?0xC1CC03:0xC1CC02);
        if ((mouseX==0.f && mouseY==0.f) || (*reinterpret_cast<const uint16*>(raw(pad,0x2C))==0 && allow)) {
            if (mouseTimer>0.f) {
                idealAlpha=m_fVerticalAngle;m_fBetaSpeed=m_fAlphaSpeed=0.f;alphaInput=betaInput=0.f;
                mouseTimer=std::max(0.f,mouseTimer-step);mouseActive=true;
            }
        } else {
            alphaInput=mouseY*inputScale*f(0xB6EC1C);betaInput=mouseX*inputScale*f(0xB6EC1C);
            m_fBetaSpeed=m_fAlphaSpeed=0.f;idealAlpha=m_fVerticalAngle;mouseTimer=f(0x8CCECC)*50.f;mouseActive=true;
        }
    }
    if (auto* passenger=vehicle->m_apPassengers[0]; passenger && passenger->GetIntelligence()->m_TaskMgr.GetActiveTask()) {
        auto& manager=passenger->GetIntelligence()->m_TaskMgr;
        if (int(manager.GetActiveTask()->GetTaskType())==1309 && (*raw(manager.GetActiveTask(),0x2D)&1))
            alphaInput=upper-f(0x8CCEC8)<=m_fVerticalAngle?0.f:step*f(0x8CCEC4);
    }
    if (restricted) {
        auto& locked=StaticRef<bool>(0xB70114);
        bool correct=!locked;
        if (StaticRef<int32>(0x8CC388)!=18) { locked=false;correct=true; }
        if (correct && std::abs(verticalOffset+m_fVerticalAngle)>.05f) alphaInput=(-verticalOffset-m_fVerticalAngle)*f(0x8CCEC0);
        else locked=true;
    }
    alphaInput*=f(row+48);betaInput*=f(row+48);
    const float smoothing=CamPower(f(row+32),step);
    float maximumSpeed=f(row+36);
    m_fBetaSpeed=CamFollowCarBetaBlend355(betaInput,betaCorrection,maximumSpeed,smoothing,m_fBetaSpeed);
    // The helper applies the EXE cutoff before the extended x87 result is discarded.
    if (!mouseActive) betaInput=step*m_fBetaSpeed;
    m_fHorizontalAngle+=betaInput;
    if (StaticRef<bool>(0xB6F050)) m_fHorizontalAngle=CGeneral::GetATanOfXY(m_vecFront.x,m_vecFront.y)+StaticRef<float>(0x858CB8);
    ClipBeta();
    if (state<2 && idealAlpha<m_fVerticalAngle && distance<=actualDistance) {
        const uint8 contacts=type==0?*raw(vehicle,0x960):(type==9?*raw(vehicle,0x804):0);
        if (contacts>1) alphaInput+=(idealAlpha-m_fVerticalAngle)*f(0x8CCEB8);
    }
    CamFollowCarAlphaBlend355(this,smoothing,alphaInput,maximumSpeed);
    idealAlpha+=mouseActive?alphaInput:step*m_fAlphaSpeed;
    m_fVerticalAngle+=mouseActive?alphaInput:automaticAlpha;
    if (m_fVerticalAngle>upper) { m_fVerticalAngle=upper;m_fAlphaSpeed=0.f; }
    else if (m_fVerticalAngle < -f(row+56)) { m_fVerticalAngle=-f(row+56);m_fAlphaSpeed=0.f; }
    if (std::abs(f(0x8CCEB0)-m_fVerticalAngle)<f(0x8CCEAC)) m_fVerticalAngle=f(0x8CCEB0);
    StaticRef<float>(0x8CCEB0)=m_fVerticalAngle;
    if (std::abs(f(0x8CCEA8)-m_fHorizontalAngle)<f(0x8CCEA4)) m_fHorizontalAngle=f(0x8CCEA8);
    StaticRef<float>(0x8CCEA8)=m_fHorizontalAngle;
    m_vecFront=M16Direction(m_fHorizontalAngle,m_fVerticalAngle);
    CameraRoundVector4(m_vecSource);GetVectorsReadyForRW();
    StaticRef<bool>(0xB6F042)=StaticRef<bool>(0xB6F043)=false;
    m_vecSource=focal-m_vecFront*distance;m_vecTargetCoorsForFudgeInter=focal;
    m_avecTargetHistoryPos[2]=m_avecTargetHistoryPos[0];
    CamFollowCarFinalHistory355(this,focal,idealAlpha+verticalOffset,historyDistance,distance);
    CCamera::SetColVarsVehicle(static_cast<eVehicleType>(state),zoom);
    if (StaticRef<int32>(0x8CC384)==3) {
        StaticRef<bool>(0xB6FC70)=false;CWorld::pIgnoreEntity=vehicle;
        TheCamera.CameraVehicleModeSpecialCases(vehicle);
        if (*raw(vehicle,0x429)&4) StaticRef<bool>(0x9655E5)=true;
        TheCamera.CameraColDetAndReact(&m_vecSource,&focal);TheCamera.ImproveNearClip(vehicle,nullptr,&m_vecSource,&focal);
        CWorld::pIgnoreEntity=nullptr;CameraRoundVector4(m_vecSource);
    }
    StaticRef<bool>(0xB6F042)=StaticRef<bool>(0xB6F043)=false;
    CameraRoundVector4(m_vecSource);GetVectorsReadyForRW();
    StaticRef<CVector>(0xB6F018)=focal;
}

// 0x50F970
// Exact resident x87 tail of 0x510378..0x5104F4; original stack slots mirrored.
static void CamMouseFinalDistance355(CCam* cam, const CVector& target) {
    float scratch[0xB4 / 4]{};
    scratch[0x28 / 4] = target.x;
    scratch[0x2C / 4] = target.y;
    scratch[0x30 / 4] = target.z;
    auto* source = &cam->m_vecSource;
    CamMouseFinalDistanceCpp(CamAddr(cam), CamAddr(source), CamAddr(scratch));
}
static void CamMouseChanges355(float fov,float inputX,float inputY,bool sticks,float& beta,float& alpha) {
    const float scaleFactor=StaticRef<float>(0x85A5C4), mouseBeta=-2.5f,mouseAlpha=4.f;
    const float betaSensitivity=StaticRef<float>(0xB6EC1C),alphaSensitivity=StaticRef<float>(0xB6EC18);
    const float stickBeta=StaticRef<float>(0x859B50),stickAlpha=StaticRef<float>(0x8631AC),step=CTimer::GetTimeStep(),stickScale=StaticRef<float>(0x8CC7CC);
    float betaResult,alphaResult;
    if(sticks){
        CamMouseChangesStickCpp(inputY, fov, scaleFactor, stickBeta, step, stickScale, inputX, &betaResult, stickAlpha, &alphaResult);
    }else{
        CamMouseChangesMouseCpp(inputX, mouseBeta, inputY, mouseAlpha, fov, scaleFactor, betaSensitivity, &betaResult, alphaSensitivity, &alphaResult);
    }
    beta=betaResult;alpha=alphaResult;
}
static float CamMouseRepeatedRadius355(float fov,float nearPlane) {
    const float radians=StaticRef<float>(0x8595EC),half=.5f,factor=StaticRef<float>(0x862F10),aspect=StaticRef<float>(0xC3EFA4);
    float result;
    CamMouseRepeatedRadiusCpp(fov, radians, half, aspect, nearPlane, factor, &result);
    return result;
}

static float CamMousePerpendicularDistance355(const CVector& source,const CVector& front,float radiusFactor) {
    const CVector& collision=StaticRef<CVector>(0xB9B250);
    const float one=1.f;
    float inverse,result,zDifference,yProjection,zProjection,xPerpendicular;
    CamMousePerpendicularDistanceCpp(one, radiusFactor, &inverse, CamAddr(&collision), CamAddr(&source), CamAddr(&front), &zDifference, &yProjection, &zProjection, &xPerpendicular, &result);
    return result;
}
static void CamMouseSphereCenter355(CVector& result,const CVector& front,const CVector& source,float nearPlane) {
    float zProduct;
    CamMouseSphereCenterCpp(CamAddr(&front), CamAddr(&source), CamAddr(&result), nearPlane, &zProduct);
}

static void CamMouseInitialSource355(CVector& result,const CVector& target,const CVector& front,float distance) {
    float zProduct;
    CamMouseInitialSourceCpp(CamAddr(&target), CamAddr(&front), CamAddr(&result), distance, &zProduct);
}
static float CamMouseFirstHitDistance355(const CVector& target,const CVector& hit) {
    float result;
    CamMouseFirstHitDistanceCpp(CamAddr(&target), CamAddr(&hit), &result);
    return result;
}
static void CamMousePullSource355(CVector& source,const CVector& target) {
    const float factor=.3f;
    float xProduct,zDifference;
    CamMousePullSourceCpp(CamAddr(&target), CamAddr(&source), &zDifference, factor, &xProduct);
}

static float CamMouseRecenterDelta355(const CVector& direction,float orientation,float beta) {
    float result;
    const float x=direction.x,y=direction.y;
    if(direction.z>=StaticRef<float>(0x858CAC)) {
        result = Atan2MinusCpp(y, x, beta);
    } else {
        CamMouseRecenterDeltaCpp(orientation, beta, &result);
    }
    return result;
}

void CCam::Process_FollowPedWithMouse(const CVector& position, float orientation, float, float) {
    m_fFOV = 70.f;
    if (!m_pCamTargetEntity->GetIsTypePed()) return;
    if (m_bResetStatics) { m_bRotating = false; m_bCollisionChecksOn = true; CPad::ClearMouseHistory(); m_bResetStatics = false; }
    auto* playerVehicle = FindPlayerVehicle(-1,false);
    const bool specialVehicle = playerVehicle && *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(FindPlayerVehicle(-1,false)) + 0x590) == 6;
    CVector target{position.x,position.y,StaticRef<float>(0x8CC7D0) + position.z};
    const auto* pad = CPad::GetPad(0);
    const auto* padBytes = reinterpret_cast<const uint8*>(pad);
    const bool recenter = (*reinterpret_cast<const uint8*>(padBytes + 0x10E) & 0x20) != 0;
    float betaChange;
    float alphaChange;
    if (!recenter) {
        const float mouseX = StaticRef<float>(0xB73424), mouseY = StaticRef<float>(0xB73428);
        if ((mouseX == 0.f && mouseY == 0.f) || *reinterpret_cast<const int16*>(padBytes + 0x10E) != 0) {
            const auto x = MouseCameraStick(pad,false), y = MouseCameraStick(pad,true);
            CamMouseChanges355(m_fFOV,float(-int32(x)),float(y),true,betaChange,alphaChange);
        } else {
            CamMouseChanges355(m_fFOV,mouseX,mouseY,false,betaChange,alphaChange);
        }
    } else {
        auto direction = m_vecSource - target; direction.Normalise();
        betaChange = CamMouseRecenterDelta355(direction,orientation,m_fHorizontalAngle); alphaChange = 0.f;
    }
    const auto transition = StaticRef<uint8>(0xC3EFAB);
    const bool controlsCamera = StaticRef<uint8>(0xB6F079) != 0 && StaticRef<uint16>(0xB6FC58) == 1;
    if ((controlsCamera && StaticRef<uint16>(0x8CC7D4) < transition) || transition > 200 || recenter) alphaChange = std::clamp(StaticRef<float>(0x8CC7D8) - m_fVerticalAngle,-.05f,.05f);
    m_fHorizontalAngle = betaChange + m_fHorizontalAngle; m_fVerticalAngle = alphaChange + m_fVerticalAngle; ClipBeta();
    m_fVerticalAngle = std::clamp(m_fVerticalAngle,std::bit_cast<float>(0xBFC7F1E7u),std::bit_cast<float>(0x3F490FDBu));
    float alpha = m_fVerticalAngle;
    const float alphaMultiplier = StaticRef<float>(0x8CC7C8);
    float distance;
    CamFollowMouseDistanceCpp(alpha, alphaMultiplier, &distance);
    if (StaticRef<uint8>(0xB6F05F) == 1) m_fHorizontalAngle = *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(this) + 0x9C);
    if (StaticRef<uint8>(0xB6F042) == 1) m_fHorizontalAngle = StaticRef<float>(0xB6F148) + StaticRef<float>(0x858CB8);
    if (StaticRef<uint8>(0xB6F043) == 1) m_fHorizontalAngle = StaticRef<float>(0xB6F148);
    if (specialVehicle) m_fHorizontalAngle = orientation;
    const float beta = m_fHorizontalAngle;
    float frontX;
    float frontY;
    float frontZ;
    CamDirectionFromAnglesCpp(alpha, beta, &frontX, &frontY, &frontZ);
    m_vecFront = CVector{frontX,frontY,frontZ};
    CamMouseInitialSource355(m_vecSource,target,m_vecFront,distance);
    m_vecTargetCoorsForFudgeInter = target;
    CWorld::pIgnoreEntity = m_pCamTargetEntity;
    CColPoint point; CEntity* hit{};
    if (CWorld::ProcessLineOfSight(target,m_vecSource,point,hit,true,true,true,true,false,false,true,false)) {
        float hitDistance=CamMouseFirstHitDistance355(target,point.m_vecPoint);
        if (!hit->GetIsTypePed() || distance - hitDistance <= .4f) {
            m_vecSource = point.m_vecPoint;
            if (hitDistance < .6f) RwCameraSetNearClipPlane(Scene.m_pRwCamera,std::max(hitDistance - .3f,.05f));
        } else {
            const auto firstHit = point.m_vecPoint;
            if (!CWorld::ProcessLineOfSight(firstHit,m_vecSource,point,hit,true,true,true,true,false,false,true,false)) RwCameraSetNearClipPlane(Scene.m_pRwCamera,std::min((distance - hitDistance) - .35f,.9f));
            else {
                hitDistance = (target - point.m_vecPoint).Magnitude(); m_vecSource = point.m_vecPoint;
                if (hitDistance < .6f) RwCameraSetNearClipPlane(Scene.m_pRwCamera,std::max(hitDistance - .3f,.05f));
            }
        }
    }
    CWorld::pIgnoreEntity = nullptr;
    const float fov = m_fFOV;
    float radiusFactor;
    const float radians = .017453292f, half = .5f, factor = 1.1f;
    CamFollowMouseRadiusCpp(fov, radians, half, factor, &radiusFactor);
    float nearPlane = Scene.m_pRwCamera->nearPlane;
    CVector sphereCenter;
    CamMouseSphereCenter355(sphereCenter,m_vecFront,m_vecSource,nearPlane);
    auto* sphereHit = CWorld::TestSphereAgainstWorld(sphereCenter,radiusFactor*nearPlane,nullptr,true,true,false,true,false,false);
    for (int32 iteration = 0; sphereHit && iteration < 6; ++iteration) {
        const float separation=CamMousePerpendicularDistance355(m_vecSource,m_vecFront,radiusFactor);
        const float newNear = std::max(std::min(separation,nearPlane),.1f);
        if (newNear < nearPlane) RwCameraSetNearClipPlane(Scene.m_pRwCamera,newNear);
        if (newNear == .1f) {
            CamMousePullSource355(m_vecSource,target);
        }
        nearPlane = Scene.m_pRwCamera->nearPlane;
        CamMouseSphereCenter355(sphereCenter,m_vecFront,m_vecSource,nearPlane);
        sphereHit = CWorld::TestSphereAgainstWorld(sphereCenter,CamMouseRepeatedRadius355(m_fFOV,nearPlane),nullptr,true,true,false,true,false,false);
    }
    CamMouseFinalDistance355(this,target);
    StaticRef<uint8>(0xB6F042) = 0; StaticRef<uint8>(0xB6F043) = 0; GetVectorsReadyForRW();
    if (controlsCamera && transition > 128) {
        float heading;
        const float headingX = -m_vecFront.x;
        const float headingY = m_vecFront.y;
        heading = TrigAtan2Cpp(headingX, headingY);
        auto* player = StaticRef<CPed*>(0xB6F980); player->m_fCurrentRotation = heading; player->m_fAimingRotation = heading; player->SetHeading(heading);
        if (player->GetRwObject()) player->UpdateRwMatrix();
    }
}

// 0x522D40
namespace {
float CameraRound4(float value) {
    float scaled; float integral;
    float result;
    int five=5;
    int four=4;
    CamRound4ScaleCpp(five, value, &scaled);
    std::modf(scaled,&integral);
    CamRound4FinishCpp(integral, four, &result);
    return result;
}
float CameraHeadingMinusHalfPI(CPlaceable* ped) {
    if (!*reinterpret_cast<CMatrix**>(reinterpret_cast<uint8*>(ped)+0x14)) return *reinterpret_cast<float*>(reinterpret_cast<uint8*>(ped)+0x10)-StaticRef<float>(0x858FE4);
    float headingInputX=-ped->GetMatrix().GetForward().x;
    float headingInputY=ped->GetMatrix().GetForward().y;
    float result;
    result = Atan2MinusCpp(headingInputX, headingInputY, CamExeFloat(0x858FE4));
    return result;
}
void CameraRoundVector4(CVector& v) { v.x=CameraRound4(v.x);v.y=CameraRound4(v.y);v.z=CameraRound4(v.z); }
}
static float CamFollowPedAutoAlpha355(float base,float step,float wanted,float previous,float maximum) {
    float result;
    CamFollowPedAutoAlphaCpp(base, step, wanted, previous, &result, maximum);
    return result;
}

static float CamFollowPedPitchCorrection355(float wantedPitch, float historyPitch, float followWeight, float followMaximum, float taskWeight, float verticalOffset) {
    float maximum,negativeMaximum,result;
    CamFollowPedPitchCorrectionCpp(wantedPitch, followWeight, followMaximum, &maximum, taskWeight, historyPitch, &negativeMaximum, verticalOffset, &result);
    return result;
}

static float CamFollowPedDistance355(const CVector& focal,const CVector& history,float distance,float rowMinimum,float minimumDistance) {
    const CVector* origin=&focal;const CVector* previous=&history;float actual;
    CamFollowPedDistanceCpp(CamAddr(origin), CamAddr(previous), &actual, &distance, rowMinimum, minimumDistance);
    return distance;
}

static float CamFollowPedLimitHistoryPitch355(float separation,float historyPitch,float step) {
    float maximum;
    CamFollowPedHistoryPitchCpp(separation, &maximum, step, &historyPitch);
    return historyPitch;
}

static float CamFollowPedHistoryBase355(float step,float speed,float wanted) {
    float result;
    CamFollowPedHistoryBaseCpp(step, speed, wanted, &result);
    return result;
}

static void CamFollowPedResetHistory355(const CVector& focal, const CVector& front, float firstDistance, float secondDistance, CVector& first, CVector& second) {
    const CVector* origin=&focal;const CVector* direction=&front;CVector* firstOut=&first;CVector* secondOut=&second;
    float zProduct;
    CamFollowPedResetHistoryCpp(CamAddr(origin), CamAddr(direction), CamAddr(firstOut), firstDistance, &zProduct, CamAddr(secondOut), secondDistance);
}

static void CamFollowPedHistory355(float historyDelta,float verticalOffset,float beta,const CVector& focal,float historyDistance,float distance,CVector& first,CVector& second) {
    float angle,cosAlpha,xDirection,zDirection,xProduct,yProduct;
    const CVector* origin=&focal;CVector* firstOut=&first;CVector* secondOut=&second;
    CamFollowPedHistoryCpp(historyDelta, verticalOffset, &angle, &cosAlpha, beta, &xDirection, &zDirection, historyDistance, &xProduct, &yProduct, CamAddr(origin), CamAddr(firstOut), distance, CamAddr(secondOut));
}

// Native5234EC..523531: fpatan-minusHalfPI then extended comparison;
// wrap reloads the prescribed angle float store.
static float CamFollowPedHistoryHeading355(const CVector& front) {
    const float frontX=front.x,frontY=front.y;
    float result;
    CamFollowPedHistoryHeadingCpp(frontX, frontY, &result);
    return result;
}
void CCam::Process_FollowPed_SA(const CVector& targetPosition, float, float, float, bool resetHistory) {
    if (!m_pCamTargetEntity->GetIsTypePed()) return;
    auto* ped=static_cast<CPed*>(m_pCamTargetEntity);
    if (!ped->IsPlayer()) return;
    auto* pad=CPad::GetPad(ped->m_nPedType==PED_TYPE_PLAYER2?1:0);
    auto* intelligence=ped->GetIntelligence();
    const auto f=[](uintptr_t address){return StaticRef<float>(address);};
    const uint32 flags=*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped)+0x46C);
    const float step=CTimer::GetTimeStep();
    const uint32 interior=StaticRef<int32>(0xB72914)!=0;
    const uintptr_t row=0x8CC548+interior*60;
    float extraDistance=f(row+4);
    if (!(flags&1) && StaticRef<int32>(0xB6F0F0)==3 && intelligence->GetUsingParachute()) extraDistance+=extraDistance;
    float minimumDistance=f(row+12), maximumDistance=f(row+16);
    float distance=f(0xB6F0FC)+extraDistance;
    const float upper=f(row+52),lower=f(row+56);
    if (f(0xB6EC50)<distance) minimumDistance=distance;
    float verticalOffset=f(row+8);
    switch (StaticRef<int32>(0xB6F0F0)) {
    case 1:verticalOffset+=m_fTargetZoomOneZExtra;break;
    case 2:verticalOffset+=interior?m_fTargetZoomTwoInteriorZExtra:m_fTargetZoomTwoZExtra;break;
    case 3:verticalOffset+=m_fTargetZoomThreeZExtra;break;
    }
    float verticalTaskWeight=0.f,horizontalTaskWeight=0.f;
    StaticRef<float>(0xB6EC50)=distance;
    if (auto* swim=intelligence->GetTaskSwim()) {
        horizontalTaskWeight=1.f;
        if (int(intelligence->GetTaskSwim()->m_nSwimState)!=4) verticalTaskWeight=.5f;
    } else if (intelligence->GetTaskJetPack()) {
        horizontalTaskWeight=.5f;if (!(flags&1)) verticalTaskWeight=3.f;
    }
    if (!StaticRef<bool>(0xB6F080)) {
        if (!m_bResetStatics) {
            const float raised=step+m_fFOV;
            if (70.f<=raised) { const float reduced=m_fFOV-step;m_fFOV=reduced<=70.f?70.f:reduced; }
            else m_fFOV=raised;
        } else m_fFOV=70.f;
    }
    CVector focal=targetPosition;focal.z+=f(row);
    const float historyDistance=std::max(distance,maximumDistance);
    const bool reset=m_bResetStatics || StaticRef<bool>(0xB6F042) || StaticRef<bool>(0xB6F043) || resetHistory;
    if (resetHistory) {
        StaticRef<CVector>(0x8CCC3C)=ped->GetPosition();StaticRef<CVector>(0xB6EC7C)=CVector{0,0,0};
        focal=ped->GetPosition();focal.z+=f(row);
    } else if (!reset) {
        auto* standing=ped->m_standingOnEntity;
        const auto boat=[](CEntity* e){return e && e->GetIsTypeVehicle() && *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(e)+0x594)==6;};
        if (standing && (boat(standing) || boat(*reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(standing)+0xFC)))) {
            const auto& velocity=*reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(standing)+0x44);
            const float speed=velocity.Magnitude();
            const float factor=std::max(0.f,speed-f(0x8CCEA0))/std::max(f(0x8CCEA0),speed);
            const CVector offset=velocity*factor*step;
            m_avecTargetHistoryPos[0]-=offset;m_avecTargetHistoryPos[1]-=offset;
        }
    }
    if (reset) {
        TheCamera.ResetDuckingSystem(ped);m_bRotating=false;StaticRef<bool>(0xB6EC54)=false;m_bCollisionChecksOn=true;
        const bool preserve=StaticRef<bool>(0xB6F050) || resetHistory;
        if (!preserve) {
            m_fHorizontalAngle=CameraHeadingMinusHalfPI(ped);
            if (StaticRef<bool>(0xB6F043)) m_fHorizontalAngle+=StaticRef<float>(0x858CB8);
        }
        m_fBetaSpeed=m_fAlphaSpeed=0.f;m_fDistance=1000.f;
        if (distance==f(0xB6F0F4)) { StaticRef<float>(0xB6F0FC)=f(0xB6F0F8);distance=f(0xB6F0F8); }
        if (!preserve) {
            m_fVerticalAngle=0.f;
            if (flags&1) m_fVerticalAngle=-M16Asin(std::clamp(DotProduct(ped->field_578,ped->GetMatrix().GetForward()),-1.f,1.f));
        }
        m_vecFront=M16Direction(m_fHorizontalAngle,m_fVerticalAngle);
        CamFollowPedResetHistory355(focal,m_vecFront,historyDistance,distance,m_avecTargetHistoryPos[0],m_avecTargetHistoryPos[1]);
        m_anTargetHistoryTime[0]=CTimer::GetTimeInMS();m_nCurrentHistoryPoints=0;
        if (!preserve) m_fVerticalAngle=-verticalOffset;
        auto* swim=intelligence->GetTaskSwim();
        if (!swim || int(intelligence->GetTaskSwim()->m_nSwimState)==4) { if (intelligence->GetTaskJetPack()) m_fVerticalAngle-=.34906587f; }
        else m_fVerticalAngle-=.2617994f;
        CPad::GetPad(0)->ClearMouseHistory();
    }
    m_vecFront=focal-m_avecTargetHistoryPos[0];m_vecFront.Normalise();
    distance=CamFollowPedDistance355(focal,m_avecTargetHistoryPos[1],distance,f(row+12),minimumDistance);
    float historyAngle=CamFollowPedHistoryHeading355(m_vecFront);
    float pedAngle=CameraHeadingMinusHalfPI(ped);
    float difference=pedAngle-historyAngle;
    if (difference>StaticRef<float>(0x858CB8)) pedAngle-=StaticRef<float>(0x858CBC);else if (difference < -StaticRef<float>(0x858CB8)) pedAngle+=StaticRef<float>(0x858CBC);
    auto& forceBehind=StaticRef<bool>(0xB6EC54);
    if (pad->GetForceCameraBehindPlayer()) forceBehind=true;
    else if (forceBehind && (ped->m_vecMoveSpeed.SquaredMagnitude()>.001f || std::abs(pedAngle-historyAngle)<.01f || pad->AimWeaponLeftRight(ped)!=0 || pad->AimWeaponUpDown(ped)!=0)) forceBehind=false;
    difference=pedAngle-historyAngle;
    float followWeight=0.f, followMaximum=0.f, angleChange=0.f;
    if (std::abs(difference)<f(0x8CCE9C) || forceBehind || horizontalTaskWeight!=0.f) {
        followWeight=step*f(row+40);followMaximum=step*f(row+44);
        if (!forceBehind && horizontalTaskWeight==0.f) {
            const float speed=ped->m_standingOnEntity ? (ped->m_vecMoveSpeed-*reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(ped->m_standingOnEntity)+0x44)).Magnitude() : ped->m_vecMoveSpeed.Magnitude();
            followWeight=std::min(speed*followWeight,1.f);
        } else {
            followWeight=std::min(followWeight*f(0x8CCE98),1.f);followMaximum*=f(0x8CCE94);
            if (horizontalTaskWeight!=0.f) {followWeight*=horizontalTaskWeight;followMaximum*=horizontalTaskWeight;}
        }
        angleChange=std::clamp(difference*followWeight,-followMaximum,followMaximum);
    }
    historyAngle+=angleChange;
    if (historyAngle>m_fHorizontalAngle+StaticRef<float>(0x858CB8)) historyAngle-=StaticRef<float>(0x858CBC);else if (historyAngle<m_fHorizontalAngle-StaticRef<float>(0x858CB8)) historyAngle+=StaticRef<float>(0x858CBC);
    const float betaCorrection=(historyAngle-m_fHorizontalAngle)/std::max(step,1.f);
    float historyPitch=M16Asin(std::clamp(m_vecFront.z,-1.f,1.f));
    const float separation=std::abs(pedAngle-historyAngle);
    if (separation>f(0x8CCE90) && ped->m_vecMoveSpeed.SquaredMagnitude()>.002f) {
        historyPitch=CamFollowPedLimitHistoryPitch355(separation,historyPitch,step);
    }
    float pitchCorrection=0.f;
    if (verticalTaskWeight!=0.f || (forceBehind && (flags&1))) {
        float wantedPitch=0.f;
        if (intelligence->GetTaskJetPack()) wantedPitch=-.34906587f;
        else if (intelligence->GetTaskSwim()) wantedPitch=-.2617994f;
        else if (flags&1) wantedPitch=-M16Asin(std::clamp(DotProduct(ped->field_578,ped->GetMatrix().GetForward()),-1.f,1.f));
        pitchCorrection=CamFollowPedPitchCorrection355(wantedPitch,historyPitch,followWeight,followMaximum,verticalTaskWeight,verticalOffset);
    } else {
        pitchCorrection=historyPitch-verticalOffset;
    }
    const float wantedAlpha=std::clamp(pitchCorrection,-lower,upper);
    const float maximumAlphaChange=step*f(row+24);
    const float alphaAuto=CamFollowPedAutoAlpha355(f(row+20),step,wantedAlpha,m_fVerticalAngle,maximumAlphaChange);
    float betaStick=float(-int32(pad->AimWeaponLeftRight(ped))),alphaStick=float(pad->AimWeaponUpDown(ped));
    if (auto* gun=intelligence->GetTaskUseGun()) {
        gun=intelligence->GetTaskUseGun();
        if ((int(gun->m_LastCmd)==2 || int(gun->m_LastCmd)==3) && intelligence->GetTaskUseGun()->m_WeaponInfo && !intelligence->GetTaskUseGun()->m_WeaponInfo->flags.bAimWithArm) {
            const float walk=pad->GetPedWalkLeftRight();
            if (std::abs(betaStick)<std::abs(walk)) betaStick=float(-int32(pad->GetPedWalkLeftRight()));
        }
    }
    if ((alphaStick!=0.f || betaStick!=0.f) && pad->GetPedWalkLeftRight()==0 && pad->GetPedWalkUpDown()==0) {
        auto* player=FindPlayerPed(0);const CVector forward=player->GetMatrix().GetForward();
        const CVector cameraForward=TheCamera.GetForwardVector();
        if (DotProduct(cameraForward,forward)>.3f) { CVector point=player->GetPosition()+cameraForward*5.f;g_ikChainMan.LookAt("FollowPedSA",player,nullptr,1500,static_cast<eBoneTag>(-1),&point,false,.25f,500,3,false); }
    }
    const float scale=m_fFOV*.0125f;
    betaStick=scale*f(0x859B50)*std::abs(betaStick)*f(0x8CC4A0)*f(0x8CC4A0)*betaStick;
    alphaStick=scale*f(0x8631AC)*std::abs(alphaStick)*f(0x8CC4A0)*f(0x8CC4A0)*alphaStick;
    if (auto* climb=intelligence->GetTaskClimb()) {
        intelligence->GetTaskClimb()->GetCameraStickModifier(ped,m_fVerticalAngle,m_fHorizontalAngle,alphaStick,betaStick);
    } else {
        auto* active=intelligence->GetTaskManager().GetActiveTask();
        if (int(active->GetTaskType())==701) static_cast<CTaskComplexEnterCar*>(intelligence->GetTaskManager().GetActiveTask())->GetCameraStickModifier(ped,distance,m_fVerticalAngle,m_fHorizontalAngle,alphaStick,betaStick);
    }
    const float speedWeight=CamPower(f(row+32),step),maximumSpeed=f(row+36);
    const float wantedBetaSpeed=std::clamp(betaStick+betaCorrection,-maximumSpeed,maximumSpeed);
    m_fBetaSpeed=(1.f-speedWeight)*wantedBetaSpeed+speedWeight*m_fBetaSpeed;
    if (std::abs(m_fBetaSpeed)<f(0x8CCE7C)) m_fBetaSpeed=0.f;
    const bool mouse=StaticRef<bool>(0xB6EC2E) && !pad->DisablePlayerControls;
    float betaDelta;
    if (!mouse) betaDelta=step*m_fBetaSpeed;
    else { betaStick=m_fFOV*.0125f*f(0xB73424)*-2.5f*f(0xB6EC1C);m_fBetaSpeed=0.f;betaDelta=betaStick; }
    m_fHorizontalAngle+=betaDelta;ClipBeta();
    m_fAlphaSpeed=alphaStick*speedWeight+(1.f-speedWeight)*m_fAlphaSpeed;
    m_fAlphaSpeed=std::clamp(m_fAlphaSpeed,-maximumSpeed,maximumSpeed);
    if (std::abs(m_fAlphaSpeed)<f(0x8CCE78)) m_fAlphaSpeed=0.f;
    const float historyDelta=CamFollowPedHistoryBase355(step,m_fAlphaSpeed,wantedAlpha);
    // EXE keeps the inertia term for history at stack+0x18, but applies
    // automatic vertical correction alone from stack+0x3C at 0x524147.
    float alphaDelta=alphaAuto;
    if (mouse) {
        alphaStick=m_fFOV*.0125f*f(0xB73428)*2.5f*f(0xB6EC1C);
        const uint8 transition=StaticRef<uint8>(0xC3EFAB);
        if ((StaticRef<bool>(0xB6F079) && TheCamera.GetFadingDirection()==1 && transition>45) || transition>200) alphaStick=std::clamp(-verticalOffset-m_fVerticalAngle,-.05f,.05f);
        m_fAlphaSpeed=0.f;alphaDelta=alphaStick;
    }
    m_fVerticalAngle+=alphaDelta;
    if (m_fVerticalAngle>upper) {m_fVerticalAngle=upper;m_fAlphaSpeed=0.f;}
    else if (m_fVerticalAngle < -lower) {m_fVerticalAngle=-lower;m_fAlphaSpeed=0.f;}
    if (std::abs(f(0x8CCE74)-m_fVerticalAngle)<f(0x8CCE70)) m_fVerticalAngle=f(0x8CCE74);
    StaticRef<float>(0x8CCE74)=m_fVerticalAngle;
    if (std::abs(f(0x8CCE6C)-m_fHorizontalAngle)<f(0x8CCE68)) m_fHorizontalAngle=f(0x8CCE6C);
    StaticRef<float>(0x8CCE6C)=m_fHorizontalAngle;
    m_vecFront=M16Direction(m_fHorizontalAngle,m_fVerticalAngle);GetVectorsReadyForRW();
    StaticRef<bool>(0xB6F042)=false;StaticRef<bool>(0xB6F043)=false;
    m_vecSource=focal-m_vecFront*distance;CameraRoundVector4(m_vecSource);
    CamFollowPedHistory355(historyDelta,verticalOffset,m_fHorizontalAngle,focal,historyDistance,distance,m_avecTargetHistoryPos[0],m_avecTargetHistoryPos[1]);
    if (pad->GetForceCameraBehindPlayer() && pad->AimWeaponLeftRight(nullptr)!=0) {
        float difference=m_fHorizontalAngle-(ped->m_fCurrentRotation-StaticRef<float>(0x858FE4));
        if (difference>StaticRef<float>(0x858CB8)) difference-=StaticRef<float>(0x858CBC);else if (difference < -StaticRef<float>(0x858CB8)) difference+=StaticRef<float>(0x858CBC);
        if (std::abs(difference)<step*.1f) ped->m_fAimingRotation=m_fHorizontalAngle+StaticRef<float>(0x858FE4);
    }
    TheCamera.HandleCameraMotionForDucking(ped,&m_vecSource,&focal,false);
    m_vecTargetCoorsForFudgeInter=focal;CameraRoundVector4(m_vecSource);
    CCamera::SetColVarsPed(static_cast<ePedType>(interior),StaticRef<int32>(0xB6F0F0));
    if (StaticRef<int32>(0x8CC384)==3) {
        TheCamera.CameraGenericModeSpecialCases(ped);StaticRef<bool>(0x8A5B14)=true;StaticRef<bool>(0x8A5B15)=true;StaticRef<bool>(0x8A5B17)=true;
        TheCamera.CameraColDetAndReact(&m_vecSource,&focal);TheCamera.ImproveNearClip(nullptr,ped,&m_vecSource,&focal);CameraRoundVector4(m_vecSource);
    }
    StaticRef<bool>(0xB6F042)=false;StaticRef<bool>(0xB6F043)=false;CameraRoundVector4(m_vecSource);GetVectorsReadyForRW();
    if (!interior && StaticRef<int32>(0xB6F0D8)!=1 && (flags&1) && !plugin::CallMethodAndReturn<bool,0x441390>(ped) && !StaticRef<bool>(0xB6FCDD) && !StaticRef<bool>(0xB6FD14) && !StaticRef<bool>(0xB6FCB4) && ped->m_vecMoveSpeed.SquaredMagnitude()<.0001f) gIdleCam.Process();
    else StaticRef<uint32>(0xB6FE34)=0;
    m_bResetStatics=false;
}


namespace {
float M16Asin(float input) {
    float result;
    result = CnvAsinFloatCpp(input);
    return result;
}
CVector M16Direction(float horizontal, float vertical) {
    float x, y, z;
    CamDirectionFromAnglesCpp(vertical, horizontal, &x, &y, &z);
    return {x,y,z};
}
}
// 0x5105C0
static void CamM16Blend355(float smoothing,float betaInput,float alphaInput,float* betaSpeed,float* alphaSpeed,float* betaAngle,float* alphaAngle) {
    float complement,alphaRounded;
    CamM16BlendCpp(smoothing, &complement, betaInput, CamAddr(betaSpeed), CamAddr(alphaSpeed), alphaInput, &alphaRounded, CamAddr(betaAngle), CamAddr(alphaAngle));
}

static void CamM16Bump355(float* beta,float* alpha,float* horz,float* vert,uint32 elapsed) {
    const float damping=StaticRef<float>(0x8CC47C),step=CTimer::GetTimeStep();
    const int32 signedElapsed=static_cast<int32>(elapsed);
    CamM16BumpCpp(signedElapsed, CamAddr(horz), CamAddr(beta), CamAddr(vert), CamAddr(alpha), damping, step);
}

static void CamM16StickInputs355(bool attached,float horizontal,float vertical,float fov,float step,float* beta,float* alpha) {
    float betaResult,alphaResult,horizontalSign=1.f;
    CamM16StickInputsCpp(vertical, attached, horizontal, fov, step, &betaResult, &alphaResult, &horizontalSign);
    *beta=betaResult;*alpha=alphaResult;
}

static void CamM16MouseAngles355(float horizontal,float vertical,float fov,float* beta,float* alpha) {
    float scale;
    CamM16MouseAnglesCpp(horizontal, fov, &scale, vertical, CamAddr(beta), CamAddr(alpha));
}

static CVector CamM16SideRay355(float beta,float alpha,bool positive,const CVector& source) {
    float xDirection,yDirection,yProduct,zProduct;
    CVector result;const CVector* origin=&source;CVector* outputVector=&result;
    CamM16SideRayCpp(beta, positive, alpha, &xDirection, &yDirection, &yProduct, &zProduct, CamAddr(origin), CamAddr(outputVector));
    return result;
}

void CCam::Process_M16_1stPerson(const CVector&, float, float, float) {
    if (!m_pCamTargetEntity->GetIsTypePed()) return;
    auto* ped = static_cast<CPed*>(m_pCamTargetEntity);
    auto* pad = CPad::GetPad(0);
    const bool attached = ped->IsPlayer() && ped->m_pAttachedTo;
    const auto constant = [](uintptr_t address) { return StaticRef<float>(address); };
    const float step = CTimer::GetTimeStep();
    float upper = std::bit_cast<float>(0x3F860A92u), lower = std::bit_cast<float>(0x3FBF0243u);
    float upOffset = .1f, backOffset = .3f;
    auto& zoom = StaticRef<float>(0xB6FFE8);
    auto& obstruction = StaticRef<bool>(0xB6FFF4);
    if (m_bResetStatics) {
        if (!StaticRef<bool>(0xB6EC2E) || ped->m_pTargetedObject || attached) {
            m_fHorizontalAngle = attached ? constant(0xA4449C) : ped->m_fCurrentRotation - StaticRef<float>(0x858FE4);
            m_fVerticalAngle = 0.f;
        }
        zoom = m_fFOV; m_bResetStatics = false; obstruction = false;
        StaticRef<uint32>(0xB6FFF0) = 0; StaticRef<uint32>(0xB6FFEC) = 0;
        m_fInitialPlayerOrientation = ped->m_fCurrentRotation - StaticRef<float>(0x858FE4);
        m_bCollisionChecksOn = true;
        m_fFOVSpeed = m_fAlphaSpeed = m_fBetaSpeed = 0.f;
    }
    const int mode = int(m_nMode);
    if (mode == 7 || mode == 46) {
        const int zoomIn = int(ControlsManager.GetMouseButtonAssociatedWithAction(static_cast<eControllerAction>(8)));
        const int zoomOut = int(ControlsManager.GetMouseButtonAssociatedWithAction(static_cast<eControllerAction>(9)));
        const float wheel = constant(0xB73420);
        bool changed = false;
        if ((wheel > 0.f && zoomOut == 4) || (wheel < 0.f && zoomOut == 5)) {
            zoom = (std::abs(wheel)*7.f + 10000.f)*zoom*.0001f; changed = true;
        } else if ((wheel > 0.f && zoomIn == 4) || (wheel < 0.f && zoomIn == 5)) {
            zoom /= (std::abs(wheel)*7.f + 10000.f)*.0001f; changed = true;
        }
        if (pad->SniperZoomOut() && !changed) {
            m_fFOV = (step*255.f + 10000.f)*m_fFOV*.0001f;
            zoom = m_fFOV; m_fFOVSpeed = 0.f;
        } else if (pad->SniperZoomIn() && !changed) {
            m_fFOV /= (step*255.f + 10000.f)*.0001f;
            zoom = m_fFOV; m_fFOVSpeed = 0.f;
        } else if (std::abs(zoom - m_fFOV) > .5f) {
            WellBufferMe(zoom,m_fFOV,m_fFOVSpeed,.5f,.25f,false);
        } else m_fFOVSpeed = 0.f;
        if (m_fFOV > 70.f) m_fFOV = 70.f;
        if (zoom > 70.f) zoom = 70.f;
        else {
            const float minimum = mode == 46 ? 3.f : 15.f;
            if (m_fFOV < minimum) m_fFOV = minimum;
            if (zoom < minimum) zoom = minimum;
        }
        StaticRef<uint32>(0xB6F0B4)=180; StaticRef<uint32>(0xB6F0B0)=255;
        StaticRef<uint32>(0xB6F0AC)=180; StaticRef<uint32>(0xB6F0B8)=1; StaticRef<uint32>(0xB6F0C8)=120;
    } else m_fFOV = 70.f;
    if (attached && constant(0xA44498) > 0.f) {
        float difference = m_fHorizontalAngle - constant(0xA4449C);
        if (difference < 0.f) difference += StaticRef<float>(0x858CBC);
        if (difference < constant(0xA44498) || StaticRef<float>(0x858CBC)-difference < constant(0xA44498)) {
            m_fHorizontalAngle=constant(0xA4449C); StaticRef<float>(0xA44498)=0.f;
        } else m_fHorizontalAngle += difference <= StaticRef<float>(0x858CBC)-difference ? -constant(0xA44498) : constant(0xA44498);
    }
    bool anglesUpdated=false;
    float horizontal = constant(0xB73424), vertical = constant(0xB73428), betaDelta=0.f, alphaDelta=0.f;
    if (horizontal == 0.f && vertical == 0.f) {
        horizontal = -float(pad->LookAroundLeftRight(ped)); vertical = float(pad->LookAroundUpDown(ped));
        CamM16StickInputs355(attached,horizontal,vertical,m_fFOV,step,&betaDelta,&alphaDelta);
        const float smoothing=CamPower(std::abs(horizontal)<2.f && std::abs(vertical)<2.f ? constant(0x8CCC94) : constant(0x8CCC98),step);
        CamM16Blend355(smoothing,betaDelta,alphaDelta,&m_fBetaSpeed,&m_fAlphaSpeed,&m_fHorizontalAngle,&m_fVerticalAngle);
        anglesUpdated=true;
    } else {
        horizontal *= -3.f; vertical *= 3.f;
        if (*reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(pad)+0x10E) || *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(pad)+0x117) || step<=0.f) horizontal=vertical=0.f;
        CamM16MouseAngles355(horizontal,vertical,m_fFOV,&m_fHorizontalAngle,&m_fVerticalAngle);
        anglesUpdated=true;
        m_fBetaSpeed=m_fAlphaSpeed=0.f;
    }
    if (!anglesUpdated) {m_fHorizontalAngle+=betaDelta; m_fVerticalAngle+=alphaDelta;}
    ClipBeta();
    if (int32(m_nCamBumpedTime)>0) {
        CamM16Bump355(&m_fHorizontalAngle,&m_fVerticalAngle,&m_fCamBumpedHorz,&m_fCamBumpedVert,uint32(CTimer::GetTimeInMS()-m_nCamBumpedTime));
        if (m_nCamBumpedTime+StaticRef<uint32>(0x8CC478)<CTimer::GetTimeInMS()) m_nCamBumpedTime=0;
    }
    const bool ducking=(*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped)+0x46C)&0x4000000)!=0;
    if (ducking) backOffset=.8f;
    if (attached) {
        auto* entity=ped->m_pAttachedTo;
        float slopeInput=entity->GetMatrix().GetForward().z, heading;
        const float attachedForwardX=-entity->GetMatrix().GetForward().x;
        const float attachedForwardY=entity->GetMatrix().GetForward().y;
        heading = Atan2MinusCpp(attachedForwardX, attachedForwardY, CamExeFloat(0x858FE4));
        switch (ped->m_fTurretAngleA) {
        case 1: slopeInput=-entity->GetMatrix().GetRight().z; heading=entity->GetHeading(); break;
        case 2: slopeInput=-entity->GetMatrix().GetForward().z; heading=entity->GetHeading()+StaticRef<float>(0x858FE4); break;
        case 3: slopeInput=entity->GetMatrix().GetRight().z; heading=entity->GetHeading()-StaticRef<float>(0x858CB8); break;
        }
        const float slope=M16Asin(std::clamp(slopeInput,-1.f,1.f));
        ped->PositionAttachedPed();
        if (ped->GetRwObject()) ped->UpdateRwMatrix();
        ped->UpdateRwFrame(); ped->UpdateRpHAnim();
        if (entity->GetIsTypeVehicle() && *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity)+0x590)==9) {
            upOffset=backOffset=0.f; upper=lower=ped->m_nTurretPosnMode;
        }
        ped->GetBonePosition(&m_vecSource,static_cast<eBoneTag>(5),true);
        m_vecSource += ped->GetMatrix().GetUp()*upOffset;
        m_vecSource -= ped->GetMatrix().GetForward()*backOffset;
        float delta=heading-m_fHorizontalAngle;
        if (delta>StaticRef<float>(0x858CB8)) heading-=StaticRef<float>(0x858CBC); else if (delta < -StaticRef<float>(0x858CB8)) heading+=StaticRef<float>(0x858CBC);
        delta=heading-m_fHorizontalAngle;
        const float limit=ped->m_fTurretAngleB*.75f, quarter=ped->m_fTurretAngleB*.25f;
        if (delta>limit || delta < -limit) {
            delta += delta>limit ? -limit : limit;
            const float correction=std::max(step*.05f*std::abs(delta),std::abs(delta)-quarter);
            if (entity->GetIsTypeVehicle() && *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity)+0x594)==3 && *reinterpret_cast<float*>(reinterpret_cast<uint8*>(entity)+0x954)>0.f) {
                static_cast<CAutomobile*>(entity)->SetHeliOrientation(*reinterpret_cast<float*>(reinterpret_cast<uint8*>(entity)+0x954)-step*delta*.1f);
            }
            if (delta!=0.f) m_fHorizontalAngle += delta<0.f ? -correction : correction;
        }
        if (m_fVerticalAngle>slope+upper*.75f) {
            const float excess=std::abs(slope-m_fVerticalAngle+upper*.75f);
            m_fVerticalAngle-=std::max(step*.05f*excess,excess-upper*.25f);
        }
        if (m_fVerticalAngle<slope-lower*.75f) {
            const float excess=std::abs(slope-m_fVerticalAngle-lower*.75f);
            m_fVerticalAngle+=std::max(step*.05f*excess,excess-lower*.25f);
        }
    } else {
        m_fVerticalAngle=std::clamp(m_fVerticalAngle,-lower,upper);
        if (ped->GetRwObject()) ped->UpdateRwMatrix();
        ped->UpdateRwFrame(); ped->UpdateRpHAnim();
        ped->GetBonePosition(&m_vecSource,static_cast<eBoneTag>(5),true);
        m_vecSource.z+=.1f;
        const auto& forward=ped->GetMatrix().GetForward();
        m_vecSource.x-=(ducking?constant(0x8CC7BC):backOffset)*forward.x;
        m_vecSource.y-=(ducking?constant(0x8CC7BC):backOffset)*forward.y;
        if (ducking) { m_vecSource.x-=constant(0x8CC7B8)*ped->GetMatrix().GetRight().x; m_vecSource.y-=constant(0x8CC7B8)*ped->GetMatrix().GetRight().y; }
    }
    m_fVerticalAngle=std::clamp(m_fVerticalAngle,-constant(0x8CCC90),constant(0x8CCC90));
    m_vecFront=M16Direction(m_fHorizontalAngle,m_fVerticalAngle);
    CVector ray=m_vecFront*3.f+m_vecSource;
    m_vecSource+=m_vecFront*.4f;
    if (m_bCollisionChecksOn) {
        bool clear=CWorld::GetIsLineOfSightClear(ray,m_vecSource,true,true,false,true,false,true,true);
        if (clear) {
            ray=CamM16SideRay355(m_fHorizontalAngle,m_fVerticalAngle,true,m_vecSource);
            clear=CWorld::GetIsLineOfSightClear(ray,m_vecSource,true,true,false,true,false,true,true);
        }
        if (clear) {
            ray=CamM16SideRay355(m_fHorizontalAngle,m_fVerticalAngle,false,m_vecSource);
            clear=CWorld::GetIsLineOfSightClear(ray,m_vecSource,true,true,false,true,false,true,true);
        }
        obstruction=!clear;
        if (!clear) RwCameraSetNearClipPlane(Scene.m_pRwCamera,.3f);
    }
    if (!obstruction && mode==46) RwCameraSetNearClipPlane(Scene.m_pRwCamera,((15.f-std::min(m_fFOV,15.f))*.15f+1.f)*.3f);
    m_vecSource-=m_vecFront*.4f; GetVectorsReadyForRW();
    float headingInputX=-m_vecFront.x, headingInputY=m_vecFront.y, heading;
    heading = TrigAtan2Cpp(headingInputX, headingInputY);
    auto* player=StaticRef<CPed*>(0xB6F980);
    player->m_fCurrentRotation=player->m_fAimingRotation=heading;
}

// 0x511B50
void CCam::Process_Rocket(const CVector& target, float orientation, float speedVar, float speedVarWanted, bool isHeatSeeking) {
    static auto& dword_B6FFF8 = StaticRef<uint32>(0xB6FFF8);
    static auto& dword_B6FFFC = StaticRef<uint32>(0xB6FFFC);
    static auto& byte_B70000  = StaticRef<bool>(0xB70000);

    // Como el original (0x511B58..0x511B66): si el objetivo no es un ped se salta directamente al final (que fija el plano cercano)
    if (m_pCamTargetEntity->GetIsTypePed()) {
        auto* targetPed = m_pCamTargetEntity->AsPed();
        m_fFOV = 70.0f;
        if (m_bResetStatics) {
            if (!CCamera::m_bUseMouse3rdPerson || targetPed->m_pTargetedObject) {
                m_fVerticalAngle = 0.0f;
                m_fHorizontalAngle = targetPed->m_fCurrentRotation - CamExeFloat(0x858FE4);
            }
            m_fInitialPlayerOrientation = m_fHorizontalAngle;
            m_bResetStatics             = false;
            byte_B70000                 = false;
            dword_B6FFFC                = 0;
            dword_B6FFF8                = 0;
            m_bCollisionChecksOn        = true;
        }
        // Como el original: UpdateRwMatrix en linea, UpdateRwFrame (0x532B00) y UpdateRpHAnim (0x532B20)
        m_pCamTargetEntity->UpdateRwMatrix();
        m_pCamTargetEntity->UpdateRwFrame();
        m_pCamTargetEntity->UpdateRpHAnim();
        CVector headPosition;
        targetPed->GetBonePosition(&headPosition, BONE_HEAD, true); // 0x5E4280 (no GetTransformedBonePosition)
        m_vecSource = headPosition;
        m_vecSource.z = m_vecSource.z + CamExeFloat(0x858B1C); // fadd 0.1

        const float mouseX = CPad::NewMouseControllerState.m_AmountMoved.x;
        const float mouseY = CPad::NewMouseControllerState.m_AmountMoved.y;
        float horizontalDelta, verticalDelta;
        if (mouseX != 0.0f || mouseY != 0.0f) {
            // 0x511DD4..0x511E12
            const float fovFactor = m_fFOV * CamExeFloat(0x85A5C4);
            horizontalDelta = (mouseX * CamExeFloat(0x859024)) * (fovFactor * CCamera::m_fMouseAccelHorzntl);
            verticalDelta   = (fovFactor * CCamera::m_fMouseAccelVertical) * (mouseY * CamExeFloat(0x858B90));
        } else {
            // 0x511CAE..0x511D90
            const float horizontalInput = (float)(-(int32)CPad::GetPad(0)->LookAroundLeftRight(targetPed));
            const float verticalInput   = (float)(int32)CPad::GetPad(0)->LookAroundUpDown(targetPed);
            const float horizontalSign  = horizontalInput < 0.0f ? CamExeFloat(0x858C1C) : CamExeFloat(0x858624);
            const float verticalSign    = verticalInput < 0.0f ? -1.0f : 1.0f;
            const float fovFactor       = m_fFOV * CamExeFloat(0x85A5C4);
            horizontalDelta = horizontalSign * ((((horizontalInput * horizontalInput) * CamExeFloat(0x858FC4)) * (fovFactor * CamExeFloat(0x863100))) * CTimer::GetTimeStep());
            verticalDelta   = (((verticalInput * verticalInput) * CamExeFloat(0x8630FC)) * (fovFactor * CamExeFloat(0x859B50))) * CTimer::GetTimeStep() * verticalSign;
        }
        m_fHorizontalAngle = horizontalDelta + m_fHorizontalAngle;
        m_fVerticalAngle   = verticalDelta + m_fVerticalAngle;
        ClipBeta();
        // 0x511DB5..0x511E27: limites inferior/superior fijos (60 grados arriba: 0x8630F8, -1.5620698 abajo: 0x8630F4)
        if (m_fVerticalAngle > CamExeFloat(0x8630F8)) {
            m_fVerticalAngle = std::bit_cast<float>(0x3F860A92u);
        } else if (m_fVerticalAngle < CamExeFloat(0x8630F4)) {
            m_fVerticalAngle = std::bit_cast<float>(0xBFC7F1E7u);
        }

        // 0x511E31..0x511E80: cos(vertical) se mantiene en precision extendida en el x87 para los dos productos
        const float verticalAngle   = m_fVerticalAngle;
        const float horizontalAngle = m_fHorizontalAngle;
        float frontX, frontY, frontZ;
        CamRocketFrontCpp(verticalAngle, horizontalAngle, &frontX, &frontY, &frontZ);
        m_vecFront = CVector{ frontX, frontY, frontZ };
        GetVectorsReadyForRW();

        // 0x511E8A..0x511EBC: rumbo = atan2 del front menos pi/2 en el ped objetivo de TheCamera
        // GetATanOfXY devuelve el resultado en ST0 sin redondear a float y el EXE resta pi/2 (0x511E99 fsub) antes de guardarlo:
        // se llama en ensamblador para no redondear dos veces.
        float heading;
        {
            const float atanX = m_vecFront.x, atanY = m_vecFront.y, halfPi = CamExeFloat(0x858FE4);
            const auto atanFn = &CGeneral::GetATanOfXY;
            CamRocketHeadingCpp(atanY, atanX, CamAddr(atanFn), halfPi, &heading);
        }
        auto* cameraTargetPed = StaticRef<CPed*>(0xB6F980);
        cameraTargetPed->m_fCurrentRotation = heading;
        cameraTargetPed->m_fAimingRotation  = heading;

        if (isHeatSeeking) {
            auto* player = FindPlayerPed(-1);
            auto* const playerData = reinterpret_cast<uint8*>(player->GetPlayerData());
            // Offsets del EXE (0x511ED1..): +0x94 tiempo de pulsacion, +0x98 ultimo objetivo, +0xA0 tiempo LOS (31 bits) | LOS (bit 31)
            uint32& pressedTime = *reinterpret_cast<uint32*>(playerData + 0x94);
            CEntity*& lastTarget = *reinterpret_cast<CEntity**>(playerData + 0x98);
            uint32& losWord = *reinterpret_cast<uint32*>(playerData + 0xA0);

            if (pressedTime == 0) {
                pressedTime = CTimer::GetTimeInMS();
            }

            CEntity* hsTarget = CWeapon::PickTargetForHeatSeekingMissile(
                m_vecSource,
                m_vecFront,
                1.2f,
                player,
                false,
                lastTarget
            );

            const uint32 now = CTimer::GetTimeInMS();
            bool lineOfSight;
            // 0x511F3C..0x512028: la resta usa (campo de 31 bits == 0 ? 1 : 0) y no el tiempo; el campo se guarda como (ahora > 1)
            if (hsTarget && (now - ((losWord & 0x7FFFFFFFu) == 0 ? 1u : 0u)) > 1000u) {
                losWord = (losWord & 0x80000000u) | (1u < now ? 1u : 0u);

                const bool targetUsesCollision = hsTarget->GetUsesCollision();
                const bool playerUsesCollision = player->GetUsesCollision();
                player->SetUsesCollision(false);
                hsTarget->SetUsesCollision(false);

                lineOfSight = CWorld::GetIsLineOfSightClear(
                    player->GetPosition(),
                    hsTarget->GetPosition(),
                    true,
                    true,
                    false,
                    true,
                    false,
                    true
                );
                player->SetUsesCollision(playerUsesCollision);
                hsTarget->SetUsesCollision(targetUsesCollision);
                losWord = (losWord & 0x7FFFFFFFu) | ((uint32)(uint8)lineOfSight << 31);
            } else {
                lineOfSight = ((losWord >> 31) & 1u) != 0;
            }

            // 0x51202A..0x51203E: sin linea de vision el objetivo se descarta antes de comparar con el ultimo
            if (!lineOfSight) {
                hsTarget = nullptr;
            }
            if (!hsTarget || hsTarget != lastTarget) {
                pressedTime = CTimer::GetTimeInMS();
            }

            if (hsTarget) {
                CWeaponEffects::MarkTarget(
                    CrossHairId(0),
                    hsTarget->GetPosition(),
                    255,
                    255,
                    255,
                    100,
                    1.3f,
                    true
                );
            }

            auto& crosshair = gCrossHair[CrossHairId(0)];
            const uint32 time = CTimer::GetTimeInMS() - pressedTime;

            crosshair.m_nTimeWhenToDeactivate = 0;
            crosshair.m_color.Set(
                255,
                time <= 1'500 ? 255 : 0,
                time <= 1'500 ? 255 : 0
            );
            crosshair.m_fRotation = time <= 1'500 ? 0.0f : 1.0f;
            lastTarget = hsTarget;
        }
    }

    RwCameraSetNearClipPlane(Scene.m_pRwCamera, CamExeFloat(0x8CCC9C)); // 0.15f
}

// 0x517500
void CCam::Process_SpecialFixedForSyphon(const CVector& target, float, float, float) {
    m_vecSource=m_vecCamFixedModeSource;
    m_vecTargetCoorsForFudgeInter=target;
    m_vecTargetCoorsForFudgeInter.z+=m_fSyphonModeTargetZOffSet;
    m_vecFront=target-m_vecSource;
    const CVector fixedSource=m_vecCamFixedModeSource;
    // Existing external collaborator, absent implementation in Camera.cpp.
    plugin::CallMethod<0x514030>(&TheCamera,&fixedSource,&m_vecTargetCoorsForFudgeInter,&m_vecSource,m_fFOV);
    m_vecFront.z+=m_fSyphonModeTargetZOffSet;
    GetVectorsReadyForRW();
    m_vecUp+=m_vecCamFixedModeUpOffSet;
    m_vecUp.Normalise();
    auto side=CrossProduct(m_vecUp,m_vecFront);
    side.Normalise();
    m_vecFront=CrossProduct(side,m_vecUp);
    m_vecFront.Normalise();
    m_fFOV=70.0f;
    if(!m_pCamTargetEntity||!m_pCamTargetEntity->GetIsTypePed())return;
    auto* ped=static_cast<CPed*>(m_pCamTargetEntity);
    if(!ped->m_pTargetedObject)return;
    const auto weaponType=*(reinterpret_cast<const eWeaponType*>(reinterpret_cast<const uint8*>(ped)+0x5A0+int32(int8(ped->m_nActiveWeaponSlot))*0x1C));
    const auto* info=CWeaponInfo::GetWeaponInfo(weaponType,ped->GetWeaponSkill());
    if(!info||(info->flags.bAimWithArm&&!ped->bIsDucking)||info->m_nWeaponFire==WEAPON_FIRE_MELEE)return;
    const auto delta=ped->m_pTargetedObject->GetPosition()-ped->GetPosition();
    const float negativeX=-delta.x,deltaY=delta.y;float angle;
    angle = TrigAtan2Cpp(negativeX, deltaY);
    ped->m_fCurrentRotation=angle;ped->m_fAimingRotation=angle;ped->SetHeading(angle);ped->UpdateRwMatrix();
}

// 0x512110
// Keep the wheel camera's procedural roll in x87 through sine/cosine;
// the original spills only the lateral x/y and vertical y/z products.
static CVector WheelCameraUp(const CVector& side, const CVector& vertical, uint32 time) {
    const int32 phase = static_cast<int32>(time & 0x1FFFF);
    const float angleScale = StaticRef<float>(0x8631C4), amount = .4f;
    const float sx = side.x, sy = side.y, sz = side.z;
    const float ux = vertical.x, uy = vertical.y, uz = vertical.z;
    float lateralX, lateralY, verticalY, verticalZ, outputX, outputY, outputZ;
    CamWheelUpCpp(phase, angleScale, amount, sx, &lateralX, sy, &lateralY, sz, ux, uy, &verticalY, uz, &verticalZ, &outputX, &outputY, &outputZ);
    return CVector{outputX, outputY, outputZ};
}

bool CCam::Process_WheelCam(const CVector&, float, float, float) {
    auto* target = m_pCamTargetEntity;
    const auto constant = [](uintptr_t address) { return StaticRef<float>(address); };
    const auto rawSubtype = [](CEntity* entity) { return *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(entity) + 0x590); };
    m_fFOV = 70.f;
    CVector localOffset, side, vertical;
    if (target->GetIsTypePed()) {
        localOffset = CVector{-.3f, -.5f, .1f};
        const auto& matrix = target->GetMatrix();
        m_vecSource = matrix.TransformVector(localOffset);
        m_vecSource += target->GetPosition();
        m_vecFront = CVector{1.f, 0.f, 0.f};
    } else {
        localOffset = CVector{target->GetColModel()->m_boundBox.m_vecMin.x - constant(0x8CC7DC), -2.3f, .3f};
        m_vecSource = target->GetMatrix().TransformPoint(localOffset);
        m_vecFront = target->GetMatrix().GetForward();
    }
    bool basisReady = false;
    if (target->GetIsTypeVehicle() &&
        (target->AsVehicle()->GetVehicleAppearance() == static_cast<eVehicleAppearance>(3) ||
         target->AsVehicle()->GetVehicleAppearance() == static_cast<eVehicleAppearance>(5))) {
        side = target->GetMatrix().GetRight();
        vertical = target->GetMatrix().GetUp();
        localOffset.x = -1.55f;
        m_vecSource = target->GetMatrix().TransformPoint(localOffset);
        basisReady = true;
    } else if (target->GetIsTypeVehicle() && rawSubtype(target) == 5) {
        side = CrossProduct(m_vecFront, CVector{0.f, 0.f, 1.f});
        side.Normalise();
        vertical = CrossProduct(side, m_vecFront);
        vertical.Normalise();
        auto* driver = target->AsVehicle()->m_pDriver;
        if (!driver) {
            m_vecSource.z += constant(0x8CCC68) + constant(0x8CCC68);
        } else {
            CVector head{0.f, 0.f, 0.f};
            driver->GetBonePosition(&head, static_cast<eBoneTag>(5), true);
            const auto velocity = target->AsVehicle()->m_vecMoveSpeed;
            head.x += CTimer::GetTimeStep() * velocity.x;
            head.y += CTimer::GetTimeStep() * velocity.y;
            head.z += CTimer::GetTimeStep() * velocity.z;
            head.x = side.x * constant(0x8CCC60) + head.x;
            head.y = side.y * constant(0x8CCC60) + head.y;
            head.z = side.z * constant(0x8CCC60) + head.z;
            auto forward = target->GetMatrix().GetForward();
            head.x = constant(0x8CCC64) * forward.x + head.x;
            head.y = constant(0x8CCC64) * forward.y + head.y;
            head.z = constant(0x8CCC64) * forward.z + head.z + constant(0x8CCC68);
            if (target->m_nModelIndex == 430) {
                head.x = side.x * constant(0x8CCC6C) + head.x;
                head.y = side.y * constant(0x8CCC6C) + head.y;
                head.z = side.z * constant(0x8CCC6C) + head.z;
                forward = target->GetMatrix().GetForward();
                const CVector scaled = forward * constant(0x8CCCA4);
                const CVector displaced = constant(0x8CCC70) * scaled;
                head.x += displaced.x;
                head.y += displaced.y;
                head.z = head.z + displaced.z + constant(0x8CCCA0) * constant(0x8CCC74);
            }
            m_vecSource = head;
        }
        basisReady = true;
    } else if (target->GetIsTypeVehicle() && rawSubtype(target) == 9) {
        side = target->GetMatrix().GetRight();
        m_vecFront = CrossProduct(m_vecUp, side);
        m_vecFront.Normalise();
        vertical = CVector{0.f, 0.f, 1.f};
        localOffset.x = (constant(0x8CC7DC) - constant(0x8CC7E0)) + localOffset.x;
        m_vecSource = target->GetPosition();
        const auto& right = target->GetMatrix().GetRight();
        m_vecSource.x += localOffset.x * right.x;
        m_vecSource.y += localOffset.x * right.y;
        m_vecSource.z += localOffset.x * right.z;
        m_vecSource.x += localOffset.y * m_vecFront.x;
        m_vecSource.y += localOffset.y * m_vecFront.y;
        m_vecSource.z += localOffset.y * m_vecFront.z;
        m_vecSource.x += localOffset.z * m_vecUp.x;
        m_vecSource.y += localOffset.z * m_vecUp.y;
        m_vecSource.z += localOffset.z * m_vecUp.z;
        basisReady = true;
    } else if (target->GetIsTypeVehicle() && rawSubtype(target) == 6) {
        const auto velocity = target->AsVehicle()->m_vecMoveSpeed;
        if (velocity.x * m_vecFront.x + m_vecFront.y * velocity.y + m_vecFront.z * velocity.z < 0.f)
            m_vecFront = -m_vecFront;
    }
    if (!basisReady) {
        side = CrossProduct(m_vecFront, CVector{0.f, 0.f, 1.f});
        side.Normalise();
        vertical = CrossProduct(side, m_vecFront);
        vertical.Normalise();
    }
    float water = 0.f;
    if (CWaterLevel::GetWaterLevel(m_vecSource.x, m_vecSource.y, m_vecSource.z, water, true, nullptr) && m_vecSource.z < water - .3f) {
        const auto red = constant(0xB7C508), green = constant(0xB7C50C), blue = constant(0xB7C510);
        const float magnitude = std::sqrt(blue * blue + green * green + red * red);
        const float factor = magnitude > constant(0x8CC7A8) ? constant(0x8CC7A8) / magnitude : 1.f;
        StaticRef<int32>(0xB6F0B4) = static_cast<int32>(static_cast<int64>(red * factor));
        StaticRef<int32>(0xB6F0B0) = static_cast<int32>(static_cast<int64>(green * factor));
        StaticRef<int32>(0xB6F0AC) = static_cast<int32>(static_cast<int64>(blue * factor));
        StaticRef<int32>(0xB6F0B8) = 2;
        StaticRef<uint32>(0xB6F0C8) = StaticRef<uint32>(0x8CC7A4);
    }
    m_vecUp = WheelCameraUp(side, vertical, CTimer::GetTimeInMS());
    m_vecUp.Normalise();
    m_vecFront.Normalise();
    CWorld::pIgnoreEntity = target;
    CColPoint point;
    CEntity* hit = nullptr;
    const bool blocked = CWorld::ProcessLineOfSight(m_vecSource, target->GetPosition(), point, hit, true, false, false, true, false, false, true, false);
    CWorld::pIgnoreEntity = nullptr;
    return !blocked;
}

// based on 0x51847C - 0x5184EC
void CCam::ApplyUnderwaterMotionBlur() {
    static constexpr uint32 UNDERWATER_CAM_BLUR      = 20;    // 0x8CC7A4
    static constexpr float  UNDERWATER_CAM_MAG_LIMIT = 10.0f; // 0x8CC7A8

    // Como el original (0x51845C..0x51847C): la suma de cuadrados va en el orden z, y, x ((z*z + y*y) + x*x).
    const auto colorMag = std::sqrt(
        (CTimeCycle::GetWaterBlue() * CTimeCycle::GetWaterBlue() + CTimeCycle::GetWaterGreen() * CTimeCycle::GetWaterGreen()) +
        CTimeCycle::GetWaterRed() * CTimeCycle::GetWaterRed()
    );

    const auto factor = (colorMag <= UNDERWATER_CAM_MAG_LIMIT) ? 1.0f : UNDERWATER_CAM_MAG_LIMIT / colorMag;

    TheCamera.SetMotionBlur(
        static_cast<uint32>(factor * CTimeCycle::GetWaterRed()),
        static_cast<uint32>(factor * CTimeCycle::GetWaterGreen()),
        static_cast<uint32>(factor * CTimeCycle::GetWaterBlue()),
        UNDERWATER_CAM_BLUR,
        eMotionBlurType::LIGHT_SCENE
    );
}

// 0x4D58A0
int32 ConvertPedNode2BoneTag(int32 simpleId) {
    const auto map = notsa::make_mapping<int32, int32>({
        { 1,  3 },
        { 2,  5 },
        { 3,  32},
        { 4,  22},
        { 5,  34},
        { 6,  24},
        { 7,  41},
        { 8,  51},
        { 9,  43},
        { 10, 53},
        { 11, 52},
        { 12, 42},
        { 13, 33},
        { 14, 23},
        { 15, 31},
        { 16, 21},
        { 17, 4 },
        { 18, 8 },
    });
    return notsa::find_value_or(map, simpleId, -1);
}

// 0x509A30
bool IsLampPost(eModelID modelId) {
    using namespace ModelIndices;

    return notsa::contains<eModelID>(
        {
            MI_SINGLESTREETLIGHTS1,
            MI_SINGLESTREETLIGHTS2,
            MI_SINGLESTREETLIGHTS3,
            MI_BOLLARDLIGHT,
            MI_MLAMPPOST,
            MI_STREETLAMP1,
            MI_STREETLAMP2,
            MI_TELPOLE02,
            MI_TRAFFICLIGHTS_MIAMI,
            MI_TRAFFICLIGHTS_TWOVERTICAL,
            MI_TRAFFICLIGHTS_3,
            MI_TRAFFICLIGHTS_4,
            MI_TRAFFICLIGHTS_GAY,
            MI_TRAFFICLIGHTS_5,
        },
        modelId
    );
}

// 0x5B2090: spline de la ruta de camara. La aritmetica x87 del EXE (incluida la conversion _ftol2 del indice y la pila de
// 0x34 bytes) vive en CamPathInterpolateVectorCpp (X87Math.cpp); los argumentos cdecl llegan como palabras de 32 bits.
void CameraPathInterpolateVector(CVector* out, const float* knots, float t, int32* index) {
    CamPathInterpolateVectorCpp(CamAddr(out), CamAddr(knots), std::bit_cast<uint32>(t), CamAddr(index));
}

// 0x5B2330: igual para un escalar (cuatro floats por nodo).
void CameraPathInterpolateScalar(float* out, const float* knots, float t, int32* index) {
    CamPathInterpolateScalarCpp(CamAddr(out), CamAddr(knots), std::bit_cast<uint32>(t), CamAddr(index));
}

// 0x509CA0: altura de la camara en primera persona sobre una barca. Como el original: datos del modelo del objetivo de la camara
// (word en +0x22 del objetivo, tabla de modelos 0xA9B0C8, id de manejo = word en +0x4A), cHandlingDataMgr::GetBoatPointer y copia
// de los 32 bits de +0x20 (mov, no fld/fstp: conserva el patron de bits). El EXE solo devuelve AL (ret 4, thiscall).
bool CCam::GetBoatHeightForFirstPerson(float* height) {
    auto* const target = m_pCamTargetEntity;
    if (!target) {
        return false;
    }
    auto* const modelInfo = CModelInfo::GetModelInfo(int32(int16(target->m_nModelIndex)));
    const auto handlingId = static_cast<uint8>(*reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(modelInfo) + 0x4A));
    auto* const boat = gHandlingDataMgr.GetBoatPointer(handlingId);
    if (!boat) {
        return false;
    }
    *reinterpret_cast<uint32*>(height) = *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(boat) + 0x20);
    return true;
}
