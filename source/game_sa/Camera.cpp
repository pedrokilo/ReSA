/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCamera
 * Reconstrucción: COMPLETA — 110/110 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 110/110 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Camera.h"
#include "PlayerPedData.h"
#include "PedIntelligence.h"
#include "Object.h"
#include "Garages.h"
#include "AudioEngine.h"
#include "GameLogic.h"
#include "DummyObject.h"
#include "TaskSimpleArrestPed.h"
#include "TaskSimpleSwim.h"
#include "CollisionData.h"
#include "PedModelInfo.h"

#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleDuck.h"
#include "Hud.h"

auto& TheCamera = StaticRef<CCamera>(0xB6F028);
auto& gbModelViewer = StaticRef<bool>(0xBA6728);
auto& gbCineyCamMessageDisplayed = StaticRef<int8>(0x8CC381); // 2
auto& gCameraDirection = StaticRef<int32>(0x8CC384);         // 3
auto& gCameraMode = StaticRef<eCamMode>(0x8CC388);        // -1
auto& gLastTime2PlayerCameraWasOK = StaticRef<uint32>(0xB6EC24);    // 0
auto& gLastTime2PlayerCameraCollided = StaticRef<uint32>(0xB6EC28); // 0
auto& gPlayerPedVisible = StaticRef<bool>(0x8CC380); // true
auto& gCurCamColVars = StaticRef<uint8>(0x8CCB80);
auto& gCurDistForCam = StaticRef<float>(0x8CCB84);
auto& gpCamColVars = StaticRef<float*>(0xB6FE88);
auto& gCamColVars = StaticRef<float[28][6]>(0x8CC8E0);

namespace {
// Como el original: constantes float del EXE que NO son el float mas cercano al literal decimal (leidas de .rdata).
constexpr float kExePi         = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
constexpr float kExeTwoPi      = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
constexpr float kExeHalfPi     = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4
constexpr float kExeMilli      = std::bit_cast<float>(0x3A83126Fu); // 0x858CDC (0.001f)
constexpr float kExeTimeStep   = std::bit_cast<float>(0x3CA3D70Au); // 0x858B38 (0.02f): el EXE usa paso * 0.02f, no paso / 50
constexpr float kExePercent    = std::bit_cast<float>(0x3C23D70Au); // 0x858C58 (0.01f)

// Como el original: el jugador activo se lee en linea (movzx byte [0B7CD74h]; imul 190h; [+0B7CD98h]) sin llamar a FindPlayerPed.
inline CPlayerPed* ActivePlayerPed() {
    return CWorld::Players[CWorld::PlayerInFocus].m_pPed;
}

// Como el original (0x50A97E..0x50A98E, 0x50AA91..0x50AA9D): fild del entero con signo, +2^32 si es negativo, fmul 0.001f.
// El producto se redondea una sola vez a 24 bits (la conversion a float previa daria otro resultado).
float ElapsedSeconds(int32 elapsedMs) {
    const float fourGig = 4294967296.0f, milli = kExeMilli;
    float result;
    __asm {
        fild elapsedMs
        cmp elapsedMs, 0
        jge positive
        fadd fourGig
    positive:
        fmul milli
        fstp result
    }
    return result;
}

// Como el original (0x50C088..0x50C0AA): (porcentaje * 0.01f) * tiempo final (fild, +2^32 si es negativo), un solo redondeo
float CutSceneElapsed(float percent, uint32 finishTime) {
    const int32 finish = int32(finishTime);
    const float fourGig = 4294967296.0f, hundredth = kExePercent;
    float elapsed;
    __asm {
        fld percent
        fmul hundredth
        fild finish
        cmp finish, 0
        jge positive
        fadd fourGig
    positive:
        fmulp st(1), st(0)
        fstp elapsed
    }
    return elapsed;
}

// Como el original (0x50AC43.., 0x50D254..): reloj en milisegundos como entero con signo (+2^32 si es negativo) y
// suma de un float SIN pasar antes por float. Guarda el inicio (fst) y el fin (fadd + fstp).
void StoreTimeAndTimePlus(float& start, float& end, float plus) {
    const auto nowMs = int32(CTimer::GetTimeInMS());
    const float fourGig = 4294967296.0f;
    float startResult, endResult;
    __asm {
        fild nowMs
        cmp nowMs, 0
        jge positive
        fadd fourGig
    positive:
        fst startResult
        fadd plus
        fstp endResult
    }
    start = startResult;
    end   = endResult;
}
}

CCam& CCamera::GetActiveCamera() {
    return TheCamera.m_aCams[TheCamera.m_nActiveCam];
}

void CCamera::InjectHooks() {
    RH_ScopedClass(CCamera);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(IsSphereVisible, "matrix", 0x420C40, bool(CCamera::*)(const CVector&,float,RwMatrix*));
    RH_ScopedInstall(GetArrPosForVehicleType, 0x50AF00);
    RH_ScopedInstall(GetPositionAlongSpline, 0x50AF80);
    RH_ScopedInstall(GetRoughDistanceToGround, 0x516B00);
    RH_ScopedInstall(InitialiseCameraForDebugMode, 0x50AF90);
    RH_ScopedInstall(ProcessObbeCinemaCameraPed, 0x50B880);
    RH_ScopedInstall(ProcessWideScreenOn, 0x50B890);
    RH_ScopedInstall(RenderMotionBlur, 0x50B8F0);
    RH_ScopedInstall(SetCameraDirectlyBehindForFollowPed_CamOnAString, 0x50BD40);
    RH_ScopedInstall(SetCameraDirectlyInFrontForFollowPed_CamOnAString, 0x50BD70);
    RH_ScopedInstall(SetCamPositionForFixedMode, 0x50BEC0);
    RH_ScopedInstall(SetFadeColour, 0x50BF00);
    RH_ScopedInstall(SetMotionBlur, 0x50BF40);
    RH_ScopedInstall(SetMotionBlurAlpha, 0x50BF80);
    RH_ScopedInstall(SetNearClipScript, 0x50BF90);
    RH_ScopedInstall(SetNewPlayerWeaponMode, 0x50BFB0);
    RH_ScopedInstall(SetRwCamera, 0x50C100);
    RH_ScopedInstall(SetWideScreenOn, 0x50C140);
    RH_ScopedInstall(SetWideScreenOff, 0x50C150);
    RH_ScopedInstall(StartCooperativeCamMode, 0x50C260);
    RH_ScopedInstall(StopCooperativeCamMode, 0x50C270);
    RH_ScopedInstall(AllowShootingWith2PlayersInCar, 0x50C280);
    RH_ScopedInstall(StoreValuesDuringInterPol, 0x50C290);
    RH_ScopedInstall(ProcessScriptedCommands, 0x516AE0);
    RH_ScopedInstall(FinishCutscene, 0x514950);
    RH_ScopedInstall(LerpFOV, 0x50D280);
    RH_ScopedInstall(UpdateAimingCoors, 0x50CB10);
    RH_ScopedInstall(SetColVarsAimWeapon, 0x50CBF0);
    RH_ScopedInstall(ClearPlayerWeaponMode, 0x50AB10);
    RH_ScopedInstall(DontProcessObbeCinemaCamera, 0x50AB40);
    RH_ScopedInstall(Enable1rstPersonCamCntrlsScript, 0x50AC00);
    RH_ScopedInstall(FindCamFOV, 0x50AD20);
    RH_ScopedInstall(GetFading, 0x50ADE0);
    RH_ScopedInstall(GetFadingDirection, 0x50ADF0);
    RH_ScopedInstall(Get_Just_Switched_Status, 0x50AE10);
    RH_ScopedInstall(GetGameCamPosition, 0x50AE50);

    RH_ScopedInstall(Constructor, 0x51A450);
    RH_ScopedInstall(InitCameraVehicleTweaks, 0x50A3B0);
    RH_ScopedInstall(ApplyVehicleCameraTweaks, 0x50A480);
    RH_ScopedInstall(CamShake, 0x50A9F0);
    RH_ScopedInstall(GetScreenRect, 0x50AB50);
    RH_ScopedInstall(Enable1rstPersonWeaponsCamera, 0x50AC10);
    RH_ScopedInstall(Fade, 0x50AC20);
    RH_ScopedInstall(Find3rdPersonQuickAimPitch, 0x50AD40);
    RH_ScopedInstall(GetCutSceneFinishTime, 0x50AD90);
    RH_ScopedInstall(GetScreenFadeStatus, 0x50AE20);
    RH_ScopedInstall(GetLookingLRBFirstPerson, 0x50AE60);
    RH_ScopedInstall(GetLookDirection, 0x50AE90);
    RH_ScopedInstall(GetLookingForwardFirstPerson, 0x50AED0);
    RH_ScopedInstall(CopyCameraMatrixToRWCam, 0x50AFA0);
    RH_ScopedInstall(CalculateMirroredMatrix, 0x50B380);
    RH_ScopedInstall(DealWithMirrorBeforeConstructRenderList, 0x50B510);
    RH_ScopedInstall(ProcessFade, 0x50B5D0);
    RH_ScopedInstall(ProcessMusicFade, 0x50B6D0);
    RH_ScopedInstall(Restore, 0x50B930);
    RH_ScopedInstall(RestoreWithJumpCut, 0x50BAB0);
    RH_ScopedInstall(SetCamCutSceneOffSet, 0x50BD20);
    RH_ScopedInstall(SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString, 0x50BDA0);
    RH_ScopedInstall(SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString, 0x50BE30);
    RH_ScopedInstall(Using1stPersonWeaponMode, 0x50BFF0);
    RH_ScopedInstall(SetParametersForScriptInterpolation, 0x50C030);
    RH_ScopedInstall(SetPercentAlongCutScene, 0x50C070);
    RH_ScopedInstall(SetZoomValueFollowPedScript, 0x50C160);
    RH_ScopedInstall(SetZoomValueCamStringScript, 0x50C1B0);
    RH_ScopedInstall(UpdateTargetEntity, 0x50C360);
    RH_ScopedInstall(TakeControl, 0x50C7C0);
    RH_ScopedInstall(TakeControlNoEntity, 0x50C8B0);
    RH_ScopedInstall(TakeControlAttachToEntity, 0x50C910);
    RH_ScopedInstall(TakeControlWithSpline, 0x50CAE0);
    RH_ScopedInstall(SetCamCollisionVarDataSet, 0x50CB60);
    RH_ScopedInstall(SetNearClipBasedOnPedCollision, 0x50CB90);
    RH_ScopedInstall(SetColVarsPed, 0x50CC50);
    RH_ScopedInstall(SetColVarsVehicle, 0x50CCA0);
    RH_ScopedInstall(StartTransitionWhenNotFinishedInter, 0x515BC0);
    RH_ScopedInstall(StartTransition, 0x515200);
    RH_ScopedInstall(CameraGenericModeSpecialCases, 0x50CD30);
    RH_ScopedInstall(CameraPedModeSpecialCases, 0x50CD80);
    RH_ScopedInstall(CameraPedAimModeSpecialCases, 0x50CDA0);
    RH_ScopedInstall(CameraVehicleModeSpecialCases, 0x50CDE0);
    RH_ScopedInstall(IsExtraEntityToIgnore, 0x50CE80);
    RH_ScopedInstall(ConsiderPedAsDucking, 0x50CEB0);
    RH_ScopedInstall(ResetDuckingSystem, 0x50CEF0);
    RH_ScopedInstall(HandleCameraMotionForDucking, 0x50CFA0);
    RH_ScopedInstall(HandleCameraMotionForDuckingDuringAim, 0x50D090);
    RH_ScopedInstall(VectorMoveLinear, 0x50D160);
    RH_ScopedInstall(VectorTrackLinear, 0x50D1D0);
    RH_ScopedInstall(AddShakeSimple, 0x50D240);
    RH_ScopedInstall(InitialiseScriptableComponents, 0x50D2D0);
    RH_ScopedInstall(DrawBordersForWideScreen, 0x514860);
    RH_ScopedInstall(Find3rdPersonCamTargetVector, 0x514970);
    RH_ScopedInstall(CalculateGroundHeight, 0x514B80);
    RH_ScopedInstall(CalculateFrustumPlanes, 0x514D60);
    RH_ScopedInstall(CalculateDerivedValues, 0x5150E0);
    RH_ScopedInstall(ImproveNearClip, 0x516B20);
    RH_ScopedInstall(SetCameraUpForMirror, 0x51A560);
    RH_ScopedInstall(RestoreCameraAfterMirror, 0x51A5A0);
    RH_ScopedInstall(ConeCastCollisionResolve, 0x51A5D0);
    RH_ScopedInstall(TryToStartNewCamMode, 0x51E560);
    RH_ScopedInstall(CameraColDetAndReact, 0x520190);
    RH_ScopedInstall(CamControl, 0x527FA0);
    RH_ScopedInstall(Process, 0x52B730);
    RH_ScopedInstall(DeleteCutSceneCamDataMemory, 0x5B24A0);
    RH_ScopedInstall(LoadPathSplines, 0x5B24D0);
    RH_ScopedInstall(Init, 0x5BC520);

    RH_ScopedOverloadedInstall(ProcessVectorTrackLinear, "0", 0x50D350, void(CCamera::*)(float));
    RH_ScopedOverloadedInstall(ProcessVectorTrackLinear, "1", 0x516440, void(CCamera::*)());
    RH_ScopedOverloadedInstall(ProcessVectorMoveLinear, "0", 0x50D430, void(CCamera::*)(float));
    RH_ScopedOverloadedInstall(ProcessVectorMoveLinear, "1", 0x5164A0, void(CCamera::*)());
    RH_ScopedOverloadedInstall(ProcessFOVLerp, "0", 0x50D510, void(CCamera::*)(float));
    RH_ScopedOverloadedInstall(ProcessFOVLerp, "1", 0x516500, void(CCamera::*)());
    //RH_ScopedOverloadedInstall(ProcessJiggle, "0", 0x516560, { .reversed = false });

    RH_ScopedGlobalInstall(CamShakeNoPos, 0x50A970);
}

CCamera* CCamera::Constructor() {
    // Como el original (0x51A450): solo escribe lo que escribe el EXE. El constructor de C++ (inicializadores de miembro) tocaria
    // bytes que el EXE no toca (matrices a identidad, CamTweak a cero, valores por defecto...), asi que se transcribe a mano.
    // CPlaceable::CPlaceable (0x54F1E0): posicion, rumbo y puntero de matriz a cero
    m_placement.m_vPosn = CVector{ 0.0f, 0.0f, 0.0f };
    m_placement.m_fHeading = 0.0f;
    m_matrix = nullptr;
    *reinterpret_cast<uint32*>(this) = 0x8630E8; // vtable de CCamera del EXE

    for (auto& cam : m_aCams) {
        cam.Init(); // 0x517730 (CCam::CCam) = CCam::Init
    }
    for (auto& path : m_aPathArray) {
        path.m_pArrPathData = nullptr; // 0x5097B0
    }

    // Las matrices solo reciben a cero m_pAttachMatrix (+0x40) y el dword de +0x44 (CMatrix inline del EXE)
    for (auto* matrix : { &m_mCameraMatrix, &m_mCameraMatrixOld, &m_mViewMatrix, &m_mMatInverse, &m_mMatMirrorInverse, &m_mMatMirror }) {
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(matrix) + 0x40) = 0;
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(matrix) + 0x44) = 0;
    }

    for (auto& tweak : m_aCamTweak) { // 0x50E470
        tweak.ModelID = -1;
        tweak.Dist    = 1.0f;
        tweak.Alt     = 1.0f;
        tweak.Angle   = 0.0f;
    }

    m_nShakeType = 1;
    m_bMusicFadedOut = false;
    m_matrix = reinterpret_cast<CMatrixLink*>(&m_mCameraMatrix);
    m_fDuckCamMotionFactor = 0.0f;
    m_fDuckAimCamMotionFactor = 0.0f;

    InitialiseScriptableComponents();
    return this;
}

// 0x51A450
CCamera::CCamera() : CPlaceable() {
    m_nShakeType = 1;
    m_bMusicFadedOut = false;
    m_matrix = reinterpret_cast<CMatrixLink*>(&m_mCameraMatrix);
    m_fDuckCamMotionFactor = 0.0f;
    m_fDuckAimCamMotionFactor = 0.0f;

    InitialiseScriptableComponents();
}

// 0x50A870
CCamera::~CCamera() {
    m_matrix = nullptr;
}

// 0x5BC520
void CCamera::Init() {
    // Como el original (0x5BC520..0x5BC903): primero se ponen a cero 0x358 dwords desde +0x18 (todo salvo CPlaceable) y luego
    // se escriben los valores iniciales. El codigo anterior era una reconstruccion distinta (no era este metodo del EXE).
    std::memset(reinterpret_cast<uint8*>(this) + 0x18, 0, 0x358u * 4u);
    m_pRwCamera = nullptr;
    TheCamera.InitialiseScriptableComponents(); // 0x5BC539: ecx = 0xB6F028 (TheCamera), no `this`
    m_bPlayerWasOnBike = false;
    m_b1rstPersonRunCloseToAWall = false;
    m_fPositionAlongSpline = 0.0f;
    m_bCameraJustRestored = false;

    for (auto& camera : m_aCams) {
        camera.Init();
    }

    m_aCams[0].m_fTargetCloseInDist = StaticRef<float>(0x86A46C) - StaticRef<float>(0x86A468);
    m_aCams[0].m_nMode = MODE_FOLLOWPED;
    m_aCams[1].m_nMode = MODE_FOLLOWPED;
    m_aCams[0].m_fMinRealGroundDist = StaticRef<float>(0x86A468);
    m_bEnable1rstPersonCamCntrlsScript = false;
    m_bAllow1rstPersonWeaponsCamera = false;
    m_bCooperativeCamMode = false;
    m_bAllowShootingWith2PlayersInCar = true;
    m_bVehicleSuspenHigh = false;
    m_aCams[0].m_fTargetZoomGroundOne = std::bit_cast<float>(0xBF0CCCCDu);       // -0.55f
    m_aCams[0].m_fTargetZoomGroundTwo = 1.5f;
    m_aCams[0].m_fTargetZoomGroundThree = std::bit_cast<float>(0x40666666u);     // 3.6f
    m_aCams[0].m_fTargetZoomOneZExtra = std::bit_cast<float>(0x3D75C28Fu);       // NO es 0.06f (0x3D75C290)
    m_aCams[0].m_fTargetZoomTwoZExtra = std::bit_cast<float>(0xBDCCCCCDu);       // -0.1f
    m_aCams[0].m_fTargetZoomTwoInteriorZExtra = 0.0f;
    m_aCams[0].m_fTargetZoomThreeZExtra = std::bit_cast<float>(0xBD8F5C29u);     // -0.07f
    m_aCams[0].m_fTargetZoomZCloseIn = std::bit_cast<float>(0x3F668113u);        // 0.90040702f
    m_bMoveCamToAvoidGeom = false;
    ClearPlayerWeaponMode();
    m_bInATunnelAndABigVehicle = false;
    for (auto& camera : m_aCams) {
        camera.m_pCamTargetEntity = nullptr;
    }
    m_aCams[0].m_fCamBufferedHeight = 0.0f;
    m_aCams[0].m_fCamBufferedHeightSpeed = 0.0f;
    m_aCams[1].m_fCamBufferedHeight = 0.0f;
    m_aCams[1].m_fCamBufferedHeightSpeed = 0.0f;
    for (auto& camera : m_aCams) {
        camera.m_bCamLookingAtVector = false;
        camera.m_fPlayerVelocity = 0.0f;
    }
    m_bHeadBob = false;
    m_fFractionInterToStopMoving = 0.25f;
    m_fFractionInterToStopCatchUp = 0.75f;
    m_fGaitSwayBuffer = std::bit_cast<float>(0x3F59999Au); // 0.85f
    m_bScriptParametersSetForInterp = false;
    m_nCamShakeStart = 0;
    m_fCamShakeForce = 0.0f;
    m_nModeObbeCamIsInForCar = 30;
    m_bIgnoreFadingStuffForMusic = false;
    m_bWaitForInterpolToFinish = false;
    m_pToGarageWeAreIn = nullptr;
    m_pToGarageWeAreInForHackAvoidFirstPerson = nullptr;
    m_bPlayerIsInGarage = false;
    m_bJustCameOutOfGarage = false;
    m_fNearClipScript = std::bit_cast<float>(0x3F666666u); // 0.9f
    m_bUseNearClipScript = false;
    m_bDoingSpecialInterp = false;
    m_bAboveGroundTrainNodesLoaded = false;
    m_bBelowGroundTrainNodesLoaded = false;
    m_nModeForTwoPlayersSeparateCars = MODE_TWOPLAYER_SEPARATE_CARS;
    m_nModeForTwoPlayersSameCarShootingAllowed = MODE_TWOPLAYER_IN_CAR_AND_SHOOTING;
    m_nModeForTwoPlayersSameCarShootingNotAllowed = MODE_BEHINDCAR;
    m_nModeForTwoPlayersNotBothInCar = MODE_TWOPLAYER;
    m_bWideScreenOn = false;
    m_fFOV_Wide_Screen = 0.0f;
    m_bRestoreByJumpCut = false;
    m_nCarZoom = 2;
    m_nPedZoom = 2;
    m_fCarZoomSmoothed = 0.0f;
    m_fCarZoomBase = 0.0f;
    m_fCarZoomTotal = 0.0f;
    m_fPedZoomSmoothed = 0.0f;
    m_fPedZoomTotal = 0.0f;
    m_fPedZoomBase = 0.0f;
    m_pTargetEntity = nullptr;
    // Con vehiculo FindPlayerVehicle se llama dos veces; el jugador activo se lee en linea
    if (!FindPlayerVehicle(-1, false)) {
        m_pTargetEntity = ActivePlayerPed();
    } else {
        m_pTargetEntity = FindPlayerVehicle(-1, false);
    }
    if (m_pTargetEntity) {
        m_pTargetEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pTargetEntity));
    }
    m_bInitialNodeFound = false;
    m_fScreenReductionPercentage = 0.0f;
    m_fScreenReductionSpeed = 0.0f;
    m_bWideScreenOn = false;
    m_bWantsToSwitchWidescreenOff = false;
    m_bWorldViewerBeingUsed = false;
    m_fPlayerExhaustion = 1.0f;
    m_fPedOrientForBehindOrInFront = 0.0f;
    // 0xBA67A5 = CMenuManager::m_bStartGameLoading
    if (!StaticRef<uint8>(0xBA67A5)) {
        m_bFading = false;
        CDraw::FadeValue = 0;
        m_fFadeAlpha = 0.0f;
        m_bMusicFading = false;
        m_fTimeToFadeMusic = 0.0f;
        m_fEffectsFaderScalingFactor = 0.0f;
        m_fMouseAccelVertical = std::bit_cast<float>(0x3AC49BA6u);
    }
    if (StaticRef<uint8>(0xBA67A5) == 1) {
        m_fTimeToFadeMusic = 0.0f;
    }
    m_bStartingSpline = false;
    m_nTypeOfSwitch = eSwitchType::INTERPOLATION;
    m_bUseScriptZoomValuePed = false;
    m_bUseScriptZoomValueCar = false;
    m_fPedZoomValueScript = 0.0f;
    m_fCarZoomValueScript = 0.0f;
    m_bUseSpecialFovTrain = false;
    m_fFovForTrain = std::bit_cast<float>(0x428C0000u); // 70.0f
    m_nModeToGoTo = MODE_FOLLOWPED;
    m_bJust_Switched = false;
    m_bUseTransitionBeta = false;
    m_mCameraMatrix.SetScale(1.0f);
    m_bTargetJustBeenOnTrain = false;
    m_bInitialNoNodeStaticsSet = false;
    m_nLongestTimeInMill = 5000;
    m_nTimeLastChange = 0;
    m_bIdleOn = false;
    m_nTimeWeLeftIdle_StillNoInput = 0;
    m_nTimeWeEnteredIdle = 0;
    m_fLODDistMultiplier = 1.0f;
    m_bCamDirectlyBehind = false;
    m_bCamDirectlyInFront = false;
    m_nMotionBlur = 0;
    m_bGarageFixedCamPositionSet = false;
    SetMotionBlur(255, 255, 255, 0, eMotionBlurType::NONE);
    m_bCullZoneChecksOn = false;
    m_bFailedCullZoneTestPreviously = false;
    m_nCheckCullZoneThisNumFrames = 6;
    m_nZoneCullFrameNumWereAt = 0;
    m_fCameraAverageSpeed = 0.0f;
    m_fCameraSpeedSoFar = 0.0f;
    m_vecPreviousCameraPosition = CVector{ 0.0f, 0.0f, 0.0f };
    m_nWorkOutSpeedThisNumFrames = 4;
    m_nNumFramesSoFar = 0;
    m_bJustInitialized = true;
    m_bTransitionState = false;
    m_nTimeTransitionStart = 0;
    m_bLookingAtPlayer = true;
    m_bLookingAtVector = false;
    m_f3rdPersonCHairMultX = std::bit_cast<float>(0x3F07AE14u); // 0.53f
    m_f3rdPersonCHairMultY = std::bit_cast<float>(0x3ECCCCCDu); // 0.4f
    m_fAvoidTheGeometryProbsTimer = 0.0f;
    m_nAvoidTheGeometryProbsDirn = 0;
    gPlayerPedVisible = true;
    m_bResetOldMatrix = true;
}

// 0x50A3B0
void CCamera::InitCameraVehicleTweaks() {
    m_fCurrentTweakDistance   = 1.0f;
    m_fCurrentTweakAltitude   = 1.0f;
    m_fCurrentTweakAngle      = 0.0f;
    m_nCurrentTweakModelIndex = -1;

    if (!m_bCameraVehicleTweaksInitialized) {
        for (auto& camTweak : m_aCamTweak) {
            camTweak.ModelID = -1;
            camTweak.Dist   = 1.0f;
            camTweak.Alt   = 1.0f;
            camTweak.Angle      = 0.0f;
        }

        m_aCamTweak[0].ModelID = MODEL_RCGOBLIN;
        m_aCamTweak[0].Dist = 1.0f;
        m_aCamTweak[0].Alt = 1.0f;
        m_aCamTweak[0].Angle    = 0.178997f; // todo: magic number

        m_bCameraVehicleTweaksInitialized = true;
    }
}

// 0x50D2D0
void CCamera::InitialiseScriptableComponents() {
    m_fTrackLinearStartTime    = -1.0f;
    m_fTrackLinearEndTime      = -1.0f;
    m_fStartShakeTime          = -1.0f;
    m_fEndShakeTime            = -1.0f;
    m_fEndZoomTime             = -1.0f;
    m_fStartZoomTime           = -1.0f;
    m_fZoomInFactor            = +0.0f;
    m_fZoomOutFactor           = +0.0f;
    m_bTrackLinearWithEase     = true;
    m_nZoomMode                = 1;
    m_bMoveLinearWithEase      = true;
    m_fMoveLinearStartTime     = -1.0f;
    m_fMoveLinearEndTime       = -1.0f;
    m_bBlockZoom               = false;
    m_bCameraPersistPosition   = false;
    m_bCameraPersistTrack      = false;
    m_bVecTrackLinearProcessed = false;
    m_bVecMoveLinearProcessed  = false;
    m_bFOVLerpProcessed        = false;
}

// 0x50AF90
void CCamera::InitialiseCameraForDebugMode() {
    // Como el original (0x50AF90): el EXE de venta es un simple `ret`; el codigo de depuracion (#ifndef FINAL) no existe en el.
}

// 0x50A480
void CCamera::ApplyVehicleCameraTweaks(CVehicle* vehicle) {
    // Como el original (0x50A48B / 0x50A4A8): el indice de modelo se lee con movsx (int16 con signo); GetModelIndex() devuelve uint32 sin signo
    const int32 modelIndex = int32(int16(vehicle->m_nModelIndex));
    if (modelIndex == m_nCurrentTweakModelIndex) {
        return;
    }

    InitCameraVehicleTweaks();
    for (auto& camTweak : m_aCamTweak) {
        // Como el original (0x50A4A6 `test edx,edx; jl`): las entradas con ModelID negativo se saltan
        if (camTweak.ModelID >= 0 && camTweak.ModelID == modelIndex) {
            // Como el original (0x50A4BF..0x50A4DD): Dist, ModelID, Alt, Angle
            m_fCurrentTweakDistance   = camTweak.Dist;
            m_nCurrentTweakModelIndex = camTweak.ModelID;
            m_fCurrentTweakAltitude   = camTweak.Alt;
            m_fCurrentTweakAngle      = camTweak.Angle;
            return;
        }
    }
}

// 0x50A9F0
void CCamera::CamShake(float strength, CVector from) {
    // Como el original (0x50A9F0): la camara activa sale de `this` (ecx), no de TheCamera
    const auto& source = m_aCams[m_nActiveCam].m_vecSource;
    const float dx = source.x - from.x, dy = source.y - from.y, dz = source.z - from.z;

    // Distancia del EXE: raiz del plano XY redondeada a float y luego raiz de (dz^2 + plano^2)
    const float planar = std::sqrt(dx * dx + dy * dy);
    float dist = std::sqrt(dz * dz + planar * planar);
    if (dist > 100.0f) {          // fcom 100; test ah,41h; jne
        dist = 100.0f;
    } else if (dist < 0.0f) {     // fcom 0; test ah,5; jp
        dist = 0.0f;
    }

    // 0x50AA72..0x50AA8D: producto por 0.01 (double del EXE) y resta a 1.0 (double), redondeo de 24 bits en cada paso
    const double kHundredth = StaticRef<double>(0x85A308), kOne = StaticRef<double>(0x85A310);
    float percentShakeForce;
    __asm {
        fld dist
        fmul kHundredth
        fsubr kOne
        fstp percentShakeForce
    }

    float shakeForce = (m_fCamShakeForce - ElapsedSeconds(int32(CTimer::GetTimeInMS() - m_nCamShakeStart))) * percentShakeForce;
    // 0x50AAAD..0x50AAD9: primero el tope superior (2.0) y luego el inferior (0.0), con la orientacion de las comparaciones del EXE
    if (!(shakeForce < 2.0f)) {
        shakeForce = 2.0f;
    }
    if (!(shakeForce > 0.0f)) {
        shakeForce = 0.0f;
    }

    const float toShakeForce = (percentShakeForce * strength) * std::bit_cast<float>(0x3EB33333u); // 0x858F9C
    if (toShakeForce > shakeForce) {
        m_fCamShakeForce = toShakeForce;
        m_nCamShakeStart = CTimer::GetTimeInMS();
    }
}

// 0x50A970
void CamShakeNoPos(CCamera* camera, float strength) {
    // Como el original (0x50A970): el tiempo transcurrido va por fild * 0.001f, y el tope superior (2.0) se aplica antes que el inferior (0.0)
    float oldShake = camera->m_fCamShakeForce - ElapsedSeconds(int32(CTimer::GetTimeInMS() - camera->m_nCamShakeStart));
    if (!(oldShake < 2.0f)) {
        oldShake = 2.0f;
    }
    if (!(oldShake > 0.0f)) {
        oldShake = 0.0f;
    }

    if (strength > oldShake) {
        camera->m_fCamShakeForce = strength;
        camera->m_nCamShakeStart = CTimer::GetTimeInMS();
    }
}

// 0x50AB10
void CCamera::ClearPlayerWeaponMode() {
    m_PlayerWeaponMode.m_nMode = 0;
    m_PlayerWeaponMode.m_nMaxZoom = 1;
    m_PlayerWeaponMode.m_nMinZoom = -1;
    m_PlayerWeaponMode.m_fDuration = 0.0f;
}

// 0x50AB40
void CCamera::DontProcessObbeCinemaCamera() {
    bDidWeProcessAnyCinemaCam = false;
}

// 0x50AC00
void CCamera::Enable1rstPersonCamCntrlsScript() {
    m_bEnable1rstPersonCamCntrlsScript = true;
}

// 0x50AC10
void CCamera::Enable1rstPersonWeaponsCamera() {
    m_bAllow1rstPersonWeaponsCamera = true;
}

// 0x50AC20
void CCamera::Fade(float duration, eFadeFlag direction) {
    m_fFadeDuration = duration;
    m_bFading = true;
    m_nFadeInOutFlag = direction;
    m_nFadeStartTime = CTimer::GetTimeInMS();

    if (m_bIgnoreFadingStuffForMusic && direction != eFadeFlag::FADE_OUT) {
        return;
    }
    m_bMusicFading           = true;
    m_nMusicFadingDirection  = direction;

    // Como el original (0x50AC62..0x50ACB4): min(max(duracion * 0.3f, 0.3f), duracion) con la orientacion de las comparaciones del EXE
    // (fcom 0.3: `>` conserva el producto; fcomp duracion: `<` conserva el valor, si no vale duracion)
    const float scaled = duration * 0.3f;
    float musicFade = scaled > 0.3f ? scaled : 0.3f;
    if (!(musicFade < duration)) {
        musicFade = duration;
    }
    m_fTimeToFadeMusic = musicFade;
    if (direction == eFadeFlag::FADE_IN) {
        m_fTimeToWaitToFadeMusic = duration - musicFade;
        float faded = musicFade - 0.1f;
        if (!(faded > 0.0f)) {  // fcom 0; test ah,41h; je
            faded = 0.0f;
        }
        m_fTimeToFadeMusic      = faded;
        m_nFadeTimeStartedMusic = CTimer::GetTimeInMS();
        return;
    }
    m_fTimeToWaitToFadeMusic = 0.0f;
    m_nFadeTimeStartedMusic  = CTimer::GetTimeInMS();
}

// 0x50AD20
float CCamera::FindCamFOV() const {
    return m_aCams[m_nActiveCam].m_fFOV;
}

/*!
* @addr 0x50AD40
* @return Rotation in radians at which the gun should point at, relative to the camera's vertical angle
*/
float CCamera::Find3rdPersonQuickAimPitch() const {
    // Como el original (0x50AD40..0x50AD8D): x87 con fptan/fpatan a precision completa; tan(fov * 0.5 * pi/180) * ((0.5 - Y) * 2),
    // luego * (1 / aspecto) y atan de eso, mas el angulo vertical, con el signo cambiado.
    const auto& cam = m_aCams[m_nActiveCam];
    const float fov = cam.m_fFOV, vertical = cam.m_fVerticalAngle;
    const float half = StaticRef<float>(0x858B8C), toRadians = StaticRef<float>(0x8595EC), one = StaticRef<float>(0x858624);
    const float crosshairY = m_f3rdPersonCHairMultY, aspect = CDraw::ms_fAspectRatio;
    float result;
    __asm {
        fld fov
        fmul half
        fmul toRadians
        fptan
        fstp st(0)
        fld half
        fsub crosshairY
        fadd st(0), st(0)
        fmulp st(1), st(0)
        fld one
        fdiv aspect
        fmulp st(1), st(0)
        fld1
        fpatan
        fadd vertical
        fchs
        fstp result
    }
    return result;
}

// 0x50AD90
uint32 CCamera::GetCutSceneFinishTime() {
    // Como el original (0x50AD90): `other` es un puntero a la otra camara (el codigo anterior COPIABA la otra camara sobre la activa)
    const auto& active = m_aCams[m_nActiveCam];
    if (active.m_nMode == eCamMode::MODE_FLYBY) {
        return active.m_nFinishTime;
    }

    const auto& other = m_aCams[(m_nActiveCam + 1) % 2];
    if (other.m_nMode == eCamMode::MODE_FLYBY) {
        return other.m_nFinishTime;
    }

    return 0;
}

// 0x50ADE0
bool CCamera::GetFading() const {
    return m_bFading;
}

// TODO: eFadingDirection
// 0x50ADF0
int32 CCamera::GetFadingDirection() const {
    if (m_bFading)
        return m_nFadeInOutFlag == eFadeFlag::FADE_OUT;
    else
        return 2;
}

// 0x50AE10
bool CCamera::Get_Just_Switched_Status() const {
    return m_bJust_Switched;
}

// 0x50AE20
eNameState CCamera::GetScreenFadeStatus() const {
    if (m_fFadeAlpha == 0.0f) {
        return NAME_DONT_SHOW;
    }
    if (m_fFadeAlpha == 255.0f) {
        return NAME_FADE_IN;
    }

    return NAME_SHOW;
}

// 0x50AE50
CVector* CCamera::GetGameCamPosition() {
    return &m_vecGameCamPos;
}

// 0x50AE60
bool CCamera::GetLookingLRBFirstPerson() const {
    return m_aCams[m_nActiveCam].m_nMode == eCamMode::MODE_1STPERSON
        && m_aCams[m_nActiveCam].m_nDirectionWasLooking != LOOKING_DIRECTION_FORWARD;
}

// 0x50AED0
bool CCamera::GetLookingForwardFirstPerson() const {
    return m_aCams[m_nActiveCam].m_nMode == eCamMode::MODE_1STPERSON
        && m_aCams[m_nActiveCam].m_nDirectionWasLooking == LOOKING_DIRECTION_FORWARD;
}

// 0x50AE90
int32 CCamera::GetLookDirection() const {
    const auto& cam = m_aCams[m_nActiveCam];
    if (cam.m_nMode != eCamMode::MODE_CAM_ON_A_STRING &&
        cam.m_nMode != eCamMode::MODE_1STPERSON &&
        cam.m_nMode != eCamMode::MODE_BEHINDBOAT &&
        cam.m_nMode != eCamMode::MODE_FOLLOWPED ||
        (cam.m_nDirectionWasLooking == LOOKING_DIRECTION_FORWARD)
    ) {
        return LOOKING_DIRECTION_FORWARD;
    }

    return cam.m_nDirectionWasLooking; // todo: unsigned/signed
}

// 0x50AF00
bool CCamera::GetArrPosForVehicleType(eVehicleType type, int32& arrPos) {
    switch (type) {
    case VEHICLE_TYPE_MTRUCK:
        arrPos = 0;
        return true;
    case VEHICLE_TYPE_QUAD:
        arrPos = 1;
        return true;
    case VEHICLE_TYPE_HELI:
        arrPos = 2;
        return true;
    case VEHICLE_TYPE_PLANE:
        arrPos = 4;
        return true;
    case VEHICLE_TYPE_BOAT:
        arrPos = 3;
        return true;
    default:
        return false;
    }
}

// 0x50AF80
float CCamera::GetPositionAlongSpline() const {
    return m_fPositionAlongSpline;
}

// 0x516B00
float CCamera::GetRoughDistanceToGround() {
    return m_aCams[m_nActiveCam].m_vecSource.z - CalculateGroundHeight(eGroundHeightType::ENTITY_BB_BOTTOM);
}

// 0x50AFA0
void CCamera::CopyCameraMatrixToRWCam(bool bUpdateMatrix) {
    auto* frame=RwCameraGetFrame(m_pRwCamera);
    auto* matrix=RwFrameGetMatrix(frame);
    if(!bUpdateMatrix)m_mCameraMatrixOld.UpdateMatrix(matrix);
    matrix->pos=m_mCameraMatrix.GetPosition();
    matrix->at=m_mCameraMatrix.GetForward();
    matrix->up=m_mCameraMatrix.GetUp();
    matrix->right=m_mCameraMatrix.GetRight();
    auto& flags=StaticRef<uint32>(0xB6FFC0);
    for(uint32 bit=0;bit<4;++bit){
        if(!(flags&(1u<<bit))){
            flags|=1u<<bit;
            StaticRef<CVector>(0xB6FFB4-bit*12)=CVector{-99999.0f,-99999.0f,-99999.0f};
        }
    }
    const float positionThreshold=StaticRef<float>(0x8CCC7C),directionThreshold=StaticRef<float>(0x8CCC78);
    const auto stabilize=[](RwV3d& value,CVector& previous,float threshold){
        const float x=previous.x-value.x,y=previous.y-value.y,z=previous.z-value.z;
        if(x*x+y*y+z*z<threshold*threshold)value=previous;
    };
    auto& oldPosition=StaticRef<CVector>(0xB6FFB4);
    auto& oldAt=StaticRef<CVector>(0xB6FFA8);
    auto& oldUp=StaticRef<CVector>(0xB6FF9C);
    auto& oldRight=StaticRef<CVector>(0xB6FF90);
    stabilize(matrix->pos,oldPosition,positionThreshold);
    stabilize(matrix->at,oldAt,directionThreshold);
    stabilize(matrix->up,oldUp,directionThreshold);
    stabilize(matrix->right,oldRight,directionThreshold);
    oldPosition=CVector{matrix->pos};oldAt=CVector{matrix->at};oldUp=CVector{matrix->up};oldRight=CVector{matrix->right};
    RwMatrixUpdate(matrix);
    RwFrameUpdateObjects(frame);
    RwFrameOrthoNormalize(frame);
    if(m_bResetOldMatrix&&!bUpdateMatrix){m_mCameraMatrixOld.UpdateMatrix(matrix);m_bResetOldMatrix=false;}
}

// 0x50B380
void CCamera::CalculateMirroredMatrix(CVector posn, float mirrorV, CMatrix *camMatrix, CMatrix* mirrorMatrix) {
    // Como el original (0x50B380..0x50B505): el producto escalar se suma en el orden z, y, x y el factor 2 se aplica al resultado
    // (n * (2 * (n.p - V))), no a la normal.
    const CVector position = camMatrix->GetPosition();
    const float positionFactor = (((posn.z * position.z) + (posn.y * position.y)) + (posn.x * position.x) - mirrorV);
    const float positionTwice = positionFactor + positionFactor;
    mirrorMatrix->GetPosition() = CVector{
        position.x - posn.x * positionTwice,
        position.y - posn.y * positionTwice,
        position.z - posn.z * positionTwice
    };

    const auto reflectDirection = [&posn](const CVector& v) {
        const float dot = ((v.z * posn.z) + (v.y * posn.y)) + (v.x * posn.x);
        const float twice = dot + dot;
        return CVector{ v.x - posn.x * twice, v.y - posn.y * twice, v.z - posn.z * twice };
    };

    const CVector fwd = reflectDirection(camMatrix->GetForward());
    mirrorMatrix->GetForward() = fwd;

    const CVector up = reflectDirection(camMatrix->GetUp());
    mirrorMatrix->GetUp() = up;

    mirrorMatrix->GetRight() = CVector{
        up.y * fwd.z - up.z * fwd.y,
        up.z * fwd.x - up.x * fwd.z,
        up.x * fwd.y - up.y * fwd.x
    };
}

// 0x50B510
void CCamera::DealWithMirrorBeforeConstructRenderList(bool bActiveMirror, CVector mirrorNormal, float mirrorV, CMatrix* matMirror) {
    m_bMirrorActive = bActiveMirror;

    if (!bActiveMirror)
        return;

    if (matMirror)
        m_mMatMirror = *matMirror;
    else
        CalculateMirroredMatrix(mirrorNormal, mirrorV, &m_mCameraMatrix, &m_mMatMirror);

    m_mMatMirrorInverse = Invert(m_mMatMirror);
}

/// III/VC leftover
// 0x50B8F0
void CCamera::RenderMotionBlur() const {
    ZoneScoped;

    if (m_nBlurType != eMotionBlurType::NONE) {
        // CMBlur::MotionBlurRender(); // todo: Add CMBlur::MotionBlurRender is NOP, 0x71D700
    }
}

// 0x50B930
void CCamera::Restore() {
    // Como el original (0x50B930..0x50BAA5): el jugador activo se lee en linea y FindPlayerVehicle se llama dos veces con vehiculo
    m_bLookingAtPlayer = true;
    m_bLookingAtVector = false;
    m_nTypeOfSwitch = eSwitchType::INTERPOLATION;
    m_bUseNearClipScript = false;
    m_nModeObbeCamIsInForCar = 30;
    m_fPositionAlongSpline = 0.0f;
    m_bStartingSpline = false;
    m_bScriptParametersSetForInterp = false;
    m_nWhoIsInControlOfTheCamera = 0;

    if (!FindPlayerVehicle(-1, false)) {
        m_nModeToGoTo = MODE_FOLLOWPED;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = ActivePlayerPed();
    } else {
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = FindPlayerVehicle(-1, false);
    }
    CEntity::SafeRegisterRef(m_pTargetEntity);

    switch (ActivePlayerPed()->m_nPedState) {
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_CARJACK:
    case PEDSTATE_OPEN_DOOR:
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        break;
    }

    if (ActivePlayerPed()->m_nPedState == PEDSTATE_EXIT_CAR) {
        m_nModeToGoTo = MODE_FOLLOWPED;

        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = ActivePlayerPed();
        CEntity::SafeRegisterRef(m_pTargetEntity);
    }

    CEntity::ClearReference(m_pAttachedEntity);

    m_bEnable1rstPersonCamCntrlsScript = false;
    m_bAllow1rstPersonWeaponsCamera = false;
    m_bUseScriptZoomValuePed = false;
    m_bUseScriptZoomValueCar = false;
    m_fAvoidTheGeometryProbsTimer = 0.0f;
    m_bStartInterScript = true;
    m_bCameraJustRestored = true;
}

// 0x50BAB0
void CCamera::RestoreWithJumpCut() {
    // Como el original (0x50BAB0..0x50BD1C): ademas de lo de Restore marca m_bCameraJustRestored y apaga los dos permisos de primera persona
    m_bRestoreByJumpCut = true;
    m_bLookingAtPlayer = true;
    m_bLookingAtVector = false;
    m_nTypeOfSwitch = eSwitchType::JUMPCUT;
    m_nWhoIsInControlOfTheCamera = 0;
    m_fPositionAlongSpline = 0.0f;
    m_bStartingSpline = false;
    m_bUseNearClipScript = false;
    m_nModeObbeCamIsInForCar = 30;
    m_bScriptParametersSetForInterp = false;
    m_bCameraJustRestored = true;
    m_bEnable1rstPersonCamCntrlsScript = false;
    m_bAllow1rstPersonWeaponsCamera = false;

    if (!FindPlayerVehicle(-1, false)) {
        m_nModeToGoTo = MODE_FOLLOWPED;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = ActivePlayerPed();
    } else {
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = FindPlayerVehicle(-1, false);
    }
    CEntity::SafeRegisterRef(m_pTargetEntity);

    switch (ActivePlayerPed()->m_nPedState) {
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_CARJACK:
    case PEDSTATE_OPEN_DOOR:
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        break;
    }

    if (ActivePlayerPed()->m_nPedState == PEDSTATE_EXIT_CAR) {
        m_nModeToGoTo = MODE_FOLLOWPED;

        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = ActivePlayerPed();
        CEntity::SafeRegisterRef(m_pTargetEntity);
    }

    // Los dos jugadores se leen directamente de la tabla (0xB7CD98 y 0xB7CF28), no con FindPlayerPed
    CPlayerPed* const player0 = CWorld::Players[0].m_pPed;
    CPlayerPed* const player1 = CWorld::Players[1].m_pPed;
    if (!m_bCooperativeCamMode || !player0 || !player1) {
        m_bUseScriptZoomValuePed = false;
        m_bUseScriptZoomValueCar = false;
        return;
    }

    CEntity::SafeCleanUpRef(m_pTargetEntity);

    if (!player0->IsInVehicle() || !player1->IsInVehicle()) {
        m_nModeToGoTo = m_nModeForTwoPlayersNotBothInCar;
        m_pTargetEntity = player0;
        CEntity::SafeRegisterRef(m_pTargetEntity);

        m_bUseScriptZoomValuePed = false;
        m_bUseScriptZoomValueCar = false;
        return;
    }

    if (player0->m_pVehicle == player1->m_pVehicle) {
        if (m_bAllowShootingWith2PlayersInCar) {
            m_nModeToGoTo = m_nModeForTwoPlayersSameCarShootingAllowed;
        } else {
            m_nModeToGoTo = m_nModeForTwoPlayersSameCarShootingNotAllowed;
        }
    } else {
        m_nModeToGoTo = m_nModeForTwoPlayersSeparateCars;
    }

    m_pTargetEntity = player0->m_pVehicle;
    CEntity::SafeRegisterRef(m_pTargetEntity);

    m_bUseScriptZoomValuePed = false;
    m_bUseScriptZoomValueCar = false;
}

// 0x50BD20
void CCamera::SetCamCutSceneOffSet(const CVector& offset) {
    m_vecCutSceneOffset = offset;
}

// 0x50BD40
void CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString() {
    m_bCamDirectlyBehind = true;
    CPed* player = FindPlayerPed();
    if (player) {
        m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(player->GetForward().x, player->GetForward().y);
    }
}

// 0x50BD70
void CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString() {
    m_bCamDirectlyInFront = true;
    CPed* player = FindPlayerPed();
    if (player != nullptr) {
        m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(player->GetForward().x, player->GetForward().y);
    }
}

// unused
// 0x50BDA0
void CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString(CPed* targetPed) {
    // Como el original (0x50BDA0): el rumbo sale de GetATanOfXY del vector delantero (no de GetHeading)
    if (!targetPed) {
        return;
    }

    m_bCamDirectlyBehind = true;
    m_bLookingAtPlayer = false;

    TheCamera.m_pTargetEntity = targetPed;
    CEntity::ChangeEntityReference(GetActiveCamera().m_pCamTargetEntity, targetPed);
    m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(targetPed->GetForward().x, targetPed->GetForward().y);
}

// 0x50BE30
void CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString(CPed* targetPed) {
    if (!targetPed) {
        return;
    }

    m_bLookingAtPlayer = false;
    TheCamera.m_pTargetEntity = targetPed; // Como el original (0x50BE53): 0xB6F980 es TheCamera.m_pTargetEntity, no `this`

    CCam& camera = GetActiveCamera();
    CEntity::SafeCleanUpRef(camera.m_pCamTargetEntity);

    camera.m_pCamTargetEntity = targetPed;
    camera.m_pCamTargetEntity->RegisterReference(camera.m_pCamTargetEntity);

    m_bCamDirectlyInFront = true;
    m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(targetPed->GetForward().x, targetPed->GetForward().y);
}

// 0x50BEC0
void CCamera::SetCamPositionForFixedMode(const CVector& fixedModeSource, const CVector& fixedModeUpOffset) {
    m_vecFixedModeSource = fixedModeSource;
    m_vecFixedModeUpOffSet = fixedModeUpOffset;
    m_bGarageFixedCamPositionSet = false;
}

// 0x50BF00
void CCamera::SetFadeColour(uint8 red, uint8 green, uint8 blue) {
    m_bFadeTargetIsSplashScreen = false;
    if (red == 2 && green == 2 && blue == 2) {
        m_bFadeTargetIsSplashScreen = true;
    }

    CDraw::FadeRed   = red;
    CDraw::FadeGreen = green;
    CDraw::FadeBlue  = blue;
}

// 0x50BF40
void CCamera::SetMotionBlur(uint32 red, uint32 green, uint32 blue, int32 value, eMotionBlurType blurType) {
    // Como el original (0x50BF40): los argumentos se copian como dwords completos (los miembros son uint32, no bytes)
    m_nBlurRed    = red;
    m_nBlurGreen  = green;
    m_nBlurBlue   = blue;
    m_nBlurType   = blurType;
    m_nMotionBlur = value;
}

// 0x50BF80
void CCamera::SetMotionBlurAlpha(int32 alpha) {
    m_nMotionBlurAddAlpha = alpha;
}

// 0x50BF90
void CCamera::SetNearClipScript(float nearClip) {
    m_fNearClipScript = nearClip;
    m_bUseNearClipScript = true;
}

// 0x50BFB0
void CCamera::SetNewPlayerWeaponMode(eCamMode mode, int16 minZoom, int16 maxZoom) {
    m_PlayerWeaponMode.m_nMode     = mode;
    m_PlayerWeaponMode.m_nMinZoom  = minZoom;
    m_PlayerWeaponMode.m_nMaxZoom  = maxZoom;
    m_PlayerWeaponMode.m_fDuration = 0.0f;
}

// 0x50BFF0
bool CCamera::Using1stPersonWeaponMode() const {
    switch (m_PlayerWeaponMode.m_nMode) {
    case MODE_SNIPER:
    case MODE_M16_1STPERSON:
    case MODE_ROCKETLAUNCHER:
    case MODE_ROCKETLAUNCHER_HS:
    case MODE_HELICANNON_1STPERSON:
    case MODE_CAMERA:
    case MODE_AIMWEAPON_ATTACHED:
        return true;
    default:
        return false;
    }
}

// 0x50C030
void CCamera::SetParametersForScriptInterpolation(float interpolationToStopMoving, float interpolationToCatchUp, uint32 timeForInterpolation) {
    m_nScriptTimeForInterpolation = timeForInterpolation;
    m_bScriptParametersSetForInterp = true;
    // Como el original (0x50C038, 0x50C052): * 0.01f, no `/ 100.0f`
    m_fScriptPercentageInterToStopMoving = interpolationToStopMoving * kExePercent;
    m_fScriptPercentageInterToCatchUp = interpolationToCatchUp * kExePercent;
}

// 0x50C070
void CCamera::SetPercentAlongCutScene(float percent) {
    // Como el original (0x50C070..0x50C0F4): (porcentaje * 0.01f) * tiempo final (fild, +2^32 si es negativo), un solo redondeo;
    // la otra camara es un puntero (el codigo anterior COPIABA la otra camara sobre la activa)
    auto& active = m_aCams[m_nActiveCam];
    if (active.m_nMode == eCamMode::MODE_FLYBY) {
        active.m_fTimeElapsedFloat = CutSceneElapsed(percent, active.m_nFinishTime);
        return;
    }

    auto& other = m_aCams[(m_nActiveCam + 1) % 2];
    if (other.m_nMode == eCamMode::MODE_FLYBY) {
        other.m_fTimeElapsedFloat = CutSceneElapsed(percent, other.m_nFinishTime);
        return;
    }
}

// 0x50C100
void CCamera::SetRwCamera(RwCamera* camera) {
    m_pRwCamera = camera;
    m_mViewMatrix.Attach(&camera->viewMatrix, false);
}

// 0x50C140
void CCamera::SetWideScreenOn() {
    m_bWideScreenOn = true;
    m_bWantsToSwitchWidescreenOff = false;
}

// 0x50C150
void CCamera::SetWideScreenOff() {
    m_bWantsToSwitchWidescreenOff = m_bWideScreenOn;
}

// 0x50C160
void CCamera::SetZoomValueFollowPedScript(int16 zoomMode) {
    // Como el original (0x50C160): solo 0, 1 y 2 modifican el valor; cualquier otro modo deja el valor anterior
    switch (zoomMode) {
    case 0:
        m_fPedZoomValueScript = 0.25f;
        break;
    case 1:
        m_fPedZoomValueScript = 1.50f;
        break;
    case 2:
        m_fPedZoomValueScript = std::bit_cast<float>(0x4039999Au); // 2.90f
        break;
    }
    m_bUseScriptZoomValuePed = true;
}

// zoomMode : 0- ZOOM_ONE , 1- ZOOM_TWO , 2- ZOOM_THREE
// 0x50C1B0
void CCamera::SetZoomValueCamStringScript(int16 zoomMode) {
    // Como el original (0x50C1B0): el tipo se comprueba con (byte [entidad+0x36] & 7) == 2 (vehiculo), no con el estado
    auto entity = m_aCams[0].m_pCamTargetEntity;

    if (entity->GetIsTypeVehicle()) {
        int32 arrPos{};
        // El resultado se ignora (el EXE no comprueba el retorno; arrPos queda en 0 si el tipo no esta en la tabla)
        GetArrPosForVehicleType(static_cast<eVehicleType>(entity->AsVehicle()->GetVehicleAppearance()), arrPos);

        // Tablas de 5 floats del EXE: 0x8CC3E0, 0x8CC3F4 y 0x8CC408 (modos 0, 1 y 2); otro modo no cambia el valor
        constexpr uintptr_t tables[] = { 0x8CC3E0, 0x8CC3F4, 0x8CC408 };
        if (zoomMode == 0 || zoomMode == 1 || zoomMode == 2) {
            m_fCarZoomValueScript = StaticRef<float>(tables[zoomMode] + 4u * uint32(arrPos));
        }
        m_bUseScriptZoomValueCar = true;
    } else {
        SetZoomValueFollowPedScript(zoomMode);
    }
}

// 0x50C260
void CCamera::StartCooperativeCamMode() {
    m_bCooperativeCamMode = true;
    CGameLogic::n2PlayerPedInFocus = eFocusedPlayer::NONE;
}

// 0x50C270
void CCamera::StopCooperativeCamMode() {
    m_bCooperativeCamMode = false;
    CGameLogic::n2PlayerPedInFocus = eFocusedPlayer::NONE;
}

// 0x50C280
void CCamera::AllowShootingWith2PlayersInCar(bool bAllow) {
    m_bAllowShootingWith2PlayersInCar = bAllow;
}

// 0x50C290
void CCamera::StoreValuesDuringInterPol(CVector* sourceDuringInter, CVector* targetDuringInter, CVector* upDuringInter, float* FOVDuringInter) {
    m_vecSourceDuringInter = *sourceDuringInter;
    m_vecTargetDuringInter = *targetDuringInter;
    m_vecUpDuringInter     = *upDuringInter;
    m_fFOVDuringInter      = *FOVDuringInter;

    auto dist = *sourceDuringInter - m_vecTargetDuringInter;
    m_fBetaDuringInterPol = CGeneral::GetATanOfXY(dist.x, dist.y);

    float distOnGround = dist.Magnitude2D();
    m_fAlphaDuringInterPol = CGeneral::GetATanOfXY(distOnGround, dist.z);
}

// 0x50C360
void CCamera::UpdateTargetEntity() {
    // Como el original (0x50C360..0x50C7B9): transcripcion de las llamadas a FindPlayerPed/FindPlayerVehicle y de las lecturas en linea
    // de la tabla de jugadores; la entidad objetivo se cambia siempre con CleanUpOldReference + asignacion + RegisterReference.
    // La entidad nueva se obtiene DESPUES de limpiar la referencia anterior (en el EXE: CleanUpOldReference, FindPlayerPed/Vehicle, asignar, RegisterReference)
    const auto setTarget = [this](auto&& getEntity) {
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = getEntity();
        m_pTargetEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pTargetEntity));
    };
    const auto playerPed = [] { return static_cast<CEntity*>(FindPlayerPed(-1)); };
    const auto playerVehicle = [] { return static_cast<CEntity*>(FindPlayerVehicle(-1, false)); };

    bool playerDoingSomething = false;
    m_bPlayerWasOnBike = false;
    if (m_pTargetEntity && m_pTargetEntity->GetIsTypeVehicle()) {
        const auto& speed = m_pTargetEntity->AsVehicle()->m_vecMoveSpeed;
        if (((speed.x * speed.x + speed.y * speed.y) + speed.z * speed.z) > StaticRef<float>(0x8CCC80)) {
            m_bPlayerWasOnBike = true;
        }
    }

    // Como el original: bl vale 0 (0x50C372) y solo se pone a 1 dentro de `who == 2` (0x50C3CC)
    bool something = false;
    if (m_nWhoIsInControlOfTheCamera == 2) {
        something = true;
        if (m_nModeObbeCamIsInForCar == 8 || m_nModeObbeCamIsInForCar == 7) {
            if (FindPlayerPed(-1)->m_nPedState != PEDSTATE_ARRESTED) {
                something = false;
            }
            if (!FindPlayerVehicle(-1, false)) {
                setTarget(playerPed);
            }
        }
    }

    if (!((m_bLookingAtPlayer || something) && !m_bTransitionState)) {
        if (m_pTargetEntity && !m_bTargetJustBeenOnTrain) {
            return;
        }
    }

    bool onFootPath;
    if (!FindPlayerVehicle(-1, false)) {
        onFootPath = true;
    } else if (CGameLogic::IsCoopGameGoingOn()) {
        onFootPath = false;
    } else if (!FindPlayerPed(-1)->GetTaskManager().GetSimplestActiveTask()) {
        onFootPath = false;
    } else {
        onFootPath = FindPlayerPed(-1)->GetTaskManager().GetSimplestActiveTask()->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY;
    }

    if (onFootPath) {
        setTarget(playerPed);
        if (FindPlayerPed(-1)->m_nPedState == PEDSTATE_ENTER_CAR
            || FindPlayerPed(-1)->m_nPedState == PEDSTATE_CARJACK
            || FindPlayerPed(-1)->m_nPedState == PEDSTATE_OPEN_DOOR) {
            playerDoingSomething = true;
        } else {
            auto& camTarget = m_aCams[m_nActiveCam].m_pCamTargetEntity;
            if (m_pTargetEntity != camTarget) {
                CEntity::SafeCleanUpRef(camTarget);
                camTarget = m_pTargetEntity;
                camTarget->RegisterReference(reinterpret_cast<CEntity**>(&camTarget));
            }
        }
    } else {
        setTarget(playerVehicle);
    }

    // Inverso de CanPedOpenLocks (bl del EXE): true si el jugador no tiene vehiculo o no puede abrirlo
    bool cannotOpenLocks = true;
    if (const auto player = ActivePlayerPed(); player && player->m_pVehicle) {
        const auto asArgument = FindPlayerPed(-1);
        if (FindPlayerPed(-1)->m_pVehicle->CanPedOpenLocks(asArgument)) {
            cannotOpenLocks = false;
        }
    }

    if (ActivePlayerPed()->m_nPedState == PEDSTATE_ENTER_CAR && !cannotOpenLocks && !playerDoingSomething) {
        if (m_nCarZoom != 0) {
            CEntity::SafeCleanUpRef(m_pTargetEntity);
            m_pTargetEntity = ActivePlayerPed()->m_pVehicle;
            if (!ActivePlayerPed()->m_pVehicle) {
                m_pTargetEntity = FindPlayerPed(-1);
            }
            m_pTargetEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pTargetEntity));
        }
    }

    const auto pedState = ActivePlayerPed()->m_nPedState;
    if ((pedState == PEDSTATE_CARJACK || pedState == PEDSTATE_OPEN_DOOR) && !cannotOpenLocks) {
        if (!playerDoingSomething && m_nCarZoom != 0) {
            CEntity::SafeCleanUpRef(m_pTargetEntity);
            m_pTargetEntity = ActivePlayerPed()->m_pVehicle;
            m_pTargetEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pTargetEntity));
        }
        if (!ActivePlayerPed()->m_pVehicle) {
            setTarget(playerPed);
        }
    }

    if (ActivePlayerPed()->m_nPedState == PEDSTATE_EXIT_CAR) {
        setTarget(playerPed);
    }
    if (ActivePlayerPed()->m_nPedState == PEDSTATE_DRAGGED_FROM_CAR) {
        setTarget(playerPed);
    }

    if (m_pTargetEntity->GetIsTypeVehicle() && m_nCarZoom == 0) {
        if (FindPlayerPed(-1)->m_nPedState == PEDSTATE_ARRESTED) {
            setTarget(playerPed);
        }
    }
}

// 0x50C7C0
void CCamera::TakeControl(CEntity* target, eCamMode modeToGoTo, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    // Como el original (0x50C7C0..0x50C8AC)
    if (!m_bCinemaCamera) {
        if (whoIsInControlOfTheCamera == 2 && m_nWhoIsInControlOfTheCamera == 1) {
            return;
        }
    }
    m_nWhoIsInControlOfTheCamera = whoIsInControlOfTheCamera;

    CEntity* newTargetEntity = target;
    auto newGoToMode = modeToGoTo;
    if (!target) {
        newTargetEntity = FindPlayerVehicle(-1, false) ? static_cast<CEntity*>(FindPlayerVehicle(-1, false)) : static_cast<CEntity*>(ActivePlayerPed());
    } else if (modeToGoTo == MODE_NONE) {
        // 0x50C840: el tipo se lee de la entidad objetivo ACTUAL (m_pTargetEntity), no de la nueva
        switch (m_pTargetEntity->GetType()) {
        case ENTITY_TYPE_PED:
            newGoToMode = MODE_FOLLOWPED;
            break;
        case ENTITY_TYPE_VEHICLE:
            newGoToMode = MODE_CAM_ON_A_STRING;
            break;
        }
    }

    m_bLookingAtVector = false;
    CEntity::SafeCleanUpRef(m_pTargetEntity);
    m_pTargetEntity = newTargetEntity;
    m_pTargetEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pTargetEntity));

    m_nModeToGoTo = newGoToMode;
    m_nTypeOfSwitch = switchType;
    m_bLookingAtPlayer = false;
    m_bStartInterScript = true;
    (void)FindPlayerPed(-1); // 0x50C8A0: el EXE la llama y descarta el resultado
}

// 0x50C8B0
void CCamera::TakeControlNoEntity(const CVector& fixedModeVector, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    if (whoIsInControlOfTheCamera == 2 && m_nWhoIsInControlOfTheCamera == 1)
        return;

    m_nWhoIsInControlOfTheCamera = whoIsInControlOfTheCamera;
    m_bLookingAtVector           = true;
    m_nModeToGoTo                = MODE_FIXED;
    m_bLookingAtPlayer           = false;
    m_vecFixedModeVector         = fixedModeVector;
    m_nTypeOfSwitch              = switchType;
    m_bStartInterScript          = true;
}

// 0x50C910
void CCamera::TakeControlAttachToEntity(CEntity* target, CEntity* attached, CVector* attachedCamOffset, CVector* attachedCamLookAt, float tilt, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    // Como el original (0x50C910..0x50CAD7). Antes esta funcion llamaba al propio EXE (plugin::CallMethod).
    if (whoIsInControlOfTheCamera == 2 && m_nWhoIsInControlOfTheCamera == 1) {
        return;
    }
    m_nWhoIsInControlOfTheCamera = whoIsInControlOfTheCamera;

    if (!attached) {
        attached = FindPlayerVehicle(-1, false) ? static_cast<CEntity*>(FindPlayerVehicle(-1, false)) : static_cast<CEntity*>(ActivePlayerPed());
    }

    if (target) {
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = target;
        m_pTargetEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pTargetEntity));
        m_bLookingAtVector = false;
    } else {
        m_bLookingAtVector = true;
        // 0x509760: true si el punto de mira y el desplazamiento difieren en alguna componente
        const bool differs = !(attachedCamLookAt->x == attachedCamOffset->x
                            && attachedCamLookAt->y == attachedCamOffset->y
                            && attachedCamLookAt->z == attachedCamOffset->z);
        if (differs) {
            m_vecAttachedCamLookAt = *attachedCamLookAt;
        } else {
            m_vecAttachedCamLookAt = CVector{ 0.0f, 0.0f, 0.0f };
        }
    }

    if (attachedCamOffset->x == 0.0f && attachedCamOffset->y == 0.0f && attachedCamOffset->z == 0.0f) {
        m_vecAttachedCamOffset = CVector{ 0.0f, 0.0f, 2.0f };
    } else {
        m_vecAttachedCamOffset = *attachedCamOffset;
    }

    m_fAttachedCamAngle = tilt;
    CEntity::SafeCleanUpRef(m_pAttachedEntity);
    m_pAttachedEntity = attached;
    m_pAttachedEntity->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedEntity));

    m_nModeToGoTo = MODE_ATTACHCAM;
    m_nTypeOfSwitch = switchType;
    m_bLookingAtPlayer = false;
    m_bStartInterScript = true;
    (void)FindPlayerPed(-1); // 0x50CAC8: el EXE la llama y descarta el resultado
}

// 0x50CAE0
void CCamera::TakeControlWithSpline(eSwitchType switchType) {
    m_bLookingAtPlayer = false;
    m_bLookingAtVector = false;
    m_bCutsceneFinished = false;
    m_nModeToGoTo = MODE_FLYBY;
    m_nTypeOfSwitch = switchType;
    m_bStartInterScript = true;
}

// 0x50CB10
void CCamera::UpdateAimingCoors(const CVector& aimingTargetCoors) {
    m_vecAimingTargetCoors = aimingTargetCoors;
}

// 0x515BD0
void CCamera::UpdateSoundDistances() {
    plugin::CallMethod<0x515BD0, CCamera*>(this);
}

// unused
// 0x50CB90
void CCamera::SetNearClipBasedOnPedCollision(float arg2) {
    const float base=gpCamColVars[4],denominator=StaticRef<float>(0xB6EC68),factor=StaticRef<float>(0x8CCC84),maximum=StaticRef<float>(0x858C24);
    float clip;
    __asm {
        fld base
        fld arg2
        fsqrt
        fdiv denominator
        fmul factor
        fld maximum
        fsub st(0),st(2)
        fmulp st(1),st(0)
        fadd st(0),st(1)
        fst clip
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp keepClip
        fstp clip
        jmp clipDone
    keepClip:
        fstp st(0)
    clipDone:
    }
    RwCameraSetNearClipPlane(Scene.m_pRwCamera,clip);
}

// TODO: eAimingType
// 0x50CBF0
void CCamera::SetColVarsAimWeapon(int32 aimingType) {
    switch (aimingType) {
    case 0:
        CCamera::SetCamCollisionVarDataSet(0);
        break;
    case 1:
        CCamera::SetCamCollisionVarDataSet(1);
        break;
    case 2:
        CCamera::SetCamCollisionVarDataSet(2);
        break;
    case 3:
        CCamera::SetCamCollisionVarDataSet(3);
        break;
    default:
        return;
    }
}

// 0x50CC50
void CCamera::SetColVarsPed(ePedType pedType, int32 nCamPedZoom) {
    // Como el original (0x50CC50): otros tipos de ped no tocan nada (el codigo anterior reiniciaba el conjunto 0)
    int32 camColVars;
    switch (pedType) {
    case PED_TYPE_PLAYER1:
        camColVars = nCamPedZoom + 3;
        break;
    case PED_TYPE_PLAYER2:
        camColVars = nCamPedZoom + 6;
        break;
    default:
        return;
    }

    if (camColVars != gCurCamColVars) {
        gCurCamColVars = uint8(camColVars);
        gCurDistForCam = 1.0f;
        gpCamColVars = gCamColVars[camColVars];
    }
}

// 0x50CD30
void CCamera::CameraGenericModeSpecialCases(CPed* targetPed) {
    // Como el original (0x50CD30): se guarda la entidad SOSTENIDA (task+8), no el ped
    m_nExtraEntitiesCount = 0;

    if (!targetPed) {
        return;
    }

    auto* taskHold = static_cast<CTaskSimpleHoldEntity*>(targetPed->GetIntelligence()->GetTaskHold(false));
    if (!taskHold || !taskHold->m_pEntityToHold) {
        return;
    }

    m_pExtraEntity[m_nExtraEntitiesCount++] = taskHold->m_pEntityToHold;
}

// 0x50CD80
void CCamera::CameraPedModeSpecialCases(CPed* /*ped*/) {
    CCollision::bCamCollideWithVehicles = true;
    CCollision::bCamCollideWithObjects  = true;
    CCollision::bCamCollideWithPeds     = true;
}

// 0x50CDA0
void CCamera::CameraPedAimModeSpecialCases(CPed* ped) {
    CameraPedModeSpecialCases(ped);

    if (ped->IsInVehicle()) {
        m_pExtraEntity[m_nExtraEntitiesCount++] = ped->m_pVehicle;
    }
}

// 0x50CDE0
void CCamera::CameraVehicleModeSpecialCases(CVehicle* vehicle) {
    // Como el original (0x50CDE0..0x50CE78): la velocidad se compara al cuadrado y el valor lento es 0x3C23D70B
    CCollision::bCamCollideWithObjects = false;

    const auto& speed = vehicle->m_vecMoveSpeed;
    const float speedSquared = (speed.x * speed.x + speed.y * speed.y) + speed.z * speed.z;
    const float limit = StaticRef<float>(0x8CCC88);
    if (limit * limit < speedSquared) {
        CCollision::relVelCamCollisionVehiclesSqr = 1.0f;
        CCollision::bCamCollideWithVehicles = true;
        CCollision::bCamCollideWithPeds     = false;
        CCollision::bCamCollideWithObjects  = false;
    } else {
        CCollision::relVelCamCollisionVehiclesSqr = std::bit_cast<float>(0x3C23D70Bu);
        CCollision::bCamCollideWithVehicles = true;
        CCollision::bCamCollideWithPeds     = true;
        CCollision::bCamCollideWithObjects  = true;
    }

    if (vehicle->m_pVehicleBeingTowed) {
        m_pExtraEntity[m_nExtraEntitiesCount++] = vehicle->m_pVehicleBeingTowed;
    }
}

// 0x50CE80
bool CCamera::IsExtraEntityToIgnore(CEntity* entity) {
    // Como el original (0x50CE80): recorre solo las `m_nExtraEntitiesCount` primeras entradas (con signo)
    for (int32 i = 0; i < m_nExtraEntitiesCount; ++i) {
        if ((&m_pExtraEntity[0])[i] == entity) {
            return true;
        }
    }
    return false;
}

// 0x420C40
static uintptr_t CameraTransformPointsLibrary=0x7EDD90;
// Native camera frustum comparisons; actual RW library transforms one copied point.
__declspec(naked) bool CCamera::IsSphereVisible(const CVector&,float,RwMatrix*) {
    __asm {
    sub esp,0Ch
    mov eax,dword ptr [esp+10h]
    fld dword ptr [eax+4]
    mov edx,dword ptr [esp+18h]
    push esi
    push edx
    mov esi,ecx
    mov ecx,dword ptr [eax]
    fstp dword ptr [esp+0Ch]
    fld dword ptr [eax+8]
    push 1
    lea eax,[esp+0Ch]
    mov dword ptr [esp+0Ch],ecx
    push eax
    lea ecx,[esp+10h]
    fstp dword ptr [esp+18h]
    push ecx
    call dword ptr [CameraTransformPointsLibrary]
    fld dword ptr [esp+18h]
    fadd dword ptr [esp+28h]
    add esp,10h
    fcomp dword ptr ds:[00C3EFA0h]
    fnstsw ax
    test ah,5
    jnp L00420D2D
    fld dword ptr [esp+8]
    fsub dword ptr [esp+18h]
    fcomp dword ptr ds:[00C3EF9Ch]
    fnstsw ax
    test ah,41h
    je L00420D2D
    fld dword ptr [esp+4]
    fmul dword ptr [esi+00000B24h]
    fld dword ptr [esp+8]
    fmul dword ptr [esi+00000B28h]
    faddp st(1),st
    fcomp dword ptr [esp+18h]
    fnstsw ax
    test ah,41h
    je L00420D2D
    fld dword ptr [esp+4]
    fmul dword ptr [esi+00000B30h]
    fld dword ptr [esp+8]
    fmul dword ptr [esi+00000B34h]
    faddp st(1),st
    fcomp dword ptr [esp+18h]
    fnstsw ax
    test ah,41h
    je L00420D2D
    fld dword ptr [esp+0Ch]
    fmul dword ptr [esi+00000B44h]
    fld dword ptr [esp+8]
    fmul dword ptr [esi+00000B40h]
    faddp st(1),st
    fcomp dword ptr [esp+18h]
    fnstsw ax
    test ah,41h
    je L00420D2D
    fld dword ptr [esp+0Ch]
    fmul dword ptr [esi+00000B50h]
    fld dword ptr [esp+8]
    fmul dword ptr [esi+00000B4Ch]
    faddp st(1),st
    fcomp dword ptr [esp+18h]
    fnstsw ax
    test ah,41h
    jne L00420D36
    L00420D2D: xor al,al
    pop esi
    add esp,0Ch
    ret 0Ch
    L00420D36: mov al,1
    pop esi
    add esp,0Ch
    ret 0Ch
    }
}

// 0x420D40 - NOTE: Function has no hook
bool CCamera::IsSphereVisible(const CVector& origin, float radius) {
    return IsSphereVisible(origin, radius, (RwMatrix*)&m_mMatInverse)
        || (m_bMirrorActive && IsSphereVisible(origin, radius, (RwMatrix*)&m_mMatMirrorInverse));
}

// 0x50CEB0
bool CCamera::ConsiderPedAsDucking(CPed* ped) {
    auto task = ped->GetIntelligence()->GetTaskDuck(true);
    return task && ped->bIsDucking && !task->m_bIsAborting;
}

// 0x50CEF0
void CCamera::ResetDuckingSystem(CPed* ped) {
    // Como el original (0x50CEF0..0x50CF98): el modulo de la velocidad se compara AL CUADRADO con 0x8630EC (1.0000001e-6f)
    m_fDuckCamMotionFactor    = 0.0f;
    m_fDuckAimCamMotionFactor = 0.0f;
    if (!ped)
        return;

    auto* task = ped->GetIntelligence()->GetTaskDuck(true);
    if (!task)
        return;

    if (!ped->bIsDucking || task->m_bIsAborting)
        return;

    const auto& speed = ped->m_vecMoveSpeed;
    const float speedSquared = (speed.x * speed.x + speed.y * speed.y) + speed.z * speed.z;
    m_fDuckCamMotionFactor    = speedSquared > std::bit_cast<float>(0x358637BEu)
        ? StaticRef<float>(0x8CCB98) - 0.5f
        : StaticRef<float>(0x8CCB94) - 1.0f;
    m_fDuckAimCamMotionFactor = std::bit_cast<float>(0xBEB33333u); // -0.35f
}

// arg5 always used as false
// 0x50CFA0
void CCamera::HandleCameraMotionForDucking(CPed* ped, CVector* source, CVector* targPosn, bool arg5) {
    ped->GetIntelligence()->GetTaskDuck(true);
    auto* task = ped->GetIntelligence()->GetTaskDuck(true);
    float target = 0.0f;
    if (task && ped->bIsDucking && !task->m_bIsAborting) {
        const auto& speed = ped->m_vecMoveSpeed;
        const auto square = (speed.x * speed.x + speed.y * speed.y) + speed.z * speed.z;
        target = !(square > std::bit_cast<float>(0x358637BEu))
            ? StaticRef<float>(0x8CCB94) - 1.0f
            : StaticRef<float>(0x8CCB98) - 0.5f;
    }
    if (!arg5) {
        const auto change = target - m_fDuckCamMotionFactor;
        const auto amount = CTimer::GetTimeStep() * 0.1f;
        m_fDuckCamMotionFactor = change * amount + m_fDuckCamMotionFactor;
    }
    if (source) source->z = m_fDuckCamMotionFactor + source->z;
    if (targPosn) targPosn->z = m_fDuckCamMotionFactor + targPosn->z;
}

// arg5 always used as false
// 0x50D090
void CCamera::HandleCameraMotionForDuckingDuringAim(CPed* ped, CVector* source, CVector* targPosn, bool arg5) {
    ped->GetIntelligence()->GetTaskDuck(true);
    auto* task = ped->GetIntelligence()->GetTaskDuck(true);
    const auto target = task && ped->bIsDucking && !task->m_bIsAborting ? -0.35f : 0.0f;
    if (!arg5) {
        const auto change = target - m_fDuckAimCamMotionFactor;
        const auto amount = CTimer::GetTimeStep() * 0.13f;
        m_fDuckAimCamMotionFactor = change * amount + m_fDuckAimCamMotionFactor;
    }
    if (source) source->z = m_fDuckAimCamMotionFactor + source->z;
    if (targPosn) targPosn->z = m_fDuckAimCamMotionFactor + targPosn->z;
}

namespace {
// Como el original: conservar fsin y las operaciones x87 en precisiÃ³n del juego.
float CameraLinearComponent(float from, float to, float ratio, bool ease) {
    float value;
    if (ease) {
        const float degrees = 180.0f, startAngle = 270.0f;
        const float radians = std::bit_cast<float>(0x3C8EFA35u);
        const float one = 1.0f, half = 0.5f;
        __asm {
            fld ratio
            fmul degrees
            fsubr startAngle
            fmul radians
            fsin
            fadd one
            fmul half
            fld to
            fsub from
            fmulp st(1), st
            fadd from
            fstp value
        }
    } else {
        value = (to - from) * ratio + from;
    }
    return value;
}
double CameraLinearTime() {
    const auto time = CTimer::GetTimeInMS();
    const auto signedTime = int32(time);
    // Como el original: fild conserva el entero hasta la primera aritmÃ©tica.
    return signedTime < 0 ? double(float(double(signedTime) + 4294967296.0)) : double(signedTime);
}
}

// 0x50D160
void CCamera::VectorMoveLinear(CVector* to, CVector* from, float duration, bool bMoveLinearWithEase) {
    const auto now = CameraLinearTime();
    m_fMoveLinearStartTime = float(now);
    m_fMoveLinearEndTime = float(now + duration);
    memcpy(&m_vecMoveLinearPosnStart, from, sizeof(CVector));
    memcpy(&m_vecMoveLinearPosnEnd, to, sizeof(CVector));
    m_bMoveLinearWithEase = bMoveLinearWithEase;
}

// 0x50D1D0
void CCamera::VectorTrackLinear(CVector* to, CVector* from, float duration, bool bEase) {
    const auto now = CameraLinearTime();
    m_fTrackLinearStartTime = float(now);
    m_fTrackLinearEndTime = float(now + duration);
    memcpy(&m_vecTrackLinearEndPoint, from, sizeof(CVector));
    memcpy(&m_vecTrackLinearStartPoint, to, sizeof(CVector));
    m_bTrackLinearWithEase = bEase;
}

// 0x516400
void CCamera::AddShake(float duration, float a2, float a3, float a4, float a5) {
    return AddShakeSimple(duration, 1, 1.0f);
}

// 0x50D240
void CCamera::AddShakeSimple(float durationMs, int32 type, float intensity) {
    // Como el original (0x50D240): el fin se suma al reloj SIN pasar antes por float
    m_fShakeIntensity = intensity;
    m_nShakeType = type;
    StoreTimeAndTimePlus(m_fStartShakeTime, m_fEndShakeTime, durationMs);
}

// 0x50D280
void CCamera::LerpFOV(float zoomInFactor, float zoomOutFactor, float timeLimit, bool bEase) {
    // Como el original (0x50D280): el fin se suma al reloj SIN pasar antes por float
    StoreTimeAndTimePlus(m_fStartZoomTime, m_fEndZoomTime, timeLimit);

    m_nZoomMode = bEase; // TODO: Rename
    m_fZoomInFactor = zoomInFactor;
    m_fZoomOutFactor = zoomOutFactor;
}

// 0x50B5D0
void CCamera::ProcessFade() {
    ZoneScoped;

    // Como el original (0x50B5D0..0x50B6C7)
    if (!m_bFading) {
        return;
    }

    const auto step = [this] { return ((CTimer::GetTimeStep() * kExeTimeStep) / m_fFadeDuration) * 255.0f; };
    const auto flag = uint16(m_nFadeInOutFlag);
    if (flag == 1) { // FADE_OUT
        m_fFadeAlpha = m_fFadeDuration == 0.0f ? 0.0f : m_fFadeAlpha - step();
        if (m_fFadeAlpha <= 0.0f) {
            m_bFading    = false;
            m_fFadeAlpha = 0.0f;
        }
    } else if (flag == 0) { // FADE_IN
        if (m_fFadeAlpha >= 255.0f) {
            m_bFading = false;
        }
        m_fFadeAlpha = m_fFadeDuration == 0.0f ? 255.0f : step() + m_fFadeAlpha;
        if (m_fFadeAlpha >= 255.0f) {
            m_fFadeAlpha = 255.0f;
        }
    }
    CDraw::FadeValue = uint8(int64(m_fFadeAlpha)); // _ftol2 (truncacion de 64 bits) y se guarda `al`
}

// 0x50B6D0
void CCamera::ProcessMusicFade() {
    // Como el original (0x50B6D0..0x50B82C): paso * 0.02f (no GetTimeStepInSeconds) y comparaciones con la orientacion del EXE
    if (!m_bMusicFading) {
        return;
    }

    const float step = CTimer::GetTimeStep() * kExeTimeStep;
    if (m_fTimeToWaitToFadeMusic > 0.0f) {
        m_fTimeToWaitToFadeMusic = m_fTimeToWaitToFadeMusic - step;
    } else {
        if (uint16(m_nMusicFadingDirection) == 1) { // FADE_OUT
            m_fEffectsFaderScalingFactor = !(m_fTimeToFadeMusic <= 0.0f)
                ? step / m_fTimeToFadeMusic + m_fEffectsFaderScalingFactor
                : 1.0f;
            if (m_fEffectsFaderScalingFactor >= 1.0f) {
                m_bMusicFadedOut = false;
                m_bMusicFading = false;
                m_fEffectsFaderScalingFactor = 1.0f;
            }
        } else if (uint16(m_nFadeInOutFlag) == 0) { // 0x50B781: `cmp word [esi+0C30h],dx` compara la bandera del fundido de PANTALLA, no la direccion de la musica
            if (m_fEffectsFaderScalingFactor <= 0.0f) {
                m_bMusicFadedOut = true;
                m_bMusicFading = false;
                m_fEffectsFaderScalingFactor = 0.0f;
            }
            m_fEffectsFaderScalingFactor = !(m_fTimeToFadeMusic <= 0.0f)
                ? m_fEffectsFaderScalingFactor - step / m_fTimeToFadeMusic
                : 0.0f;
            if (m_fEffectsFaderScalingFactor <= 0.0f) {
                m_fEffectsFaderScalingFactor = 0.0f;
            }
        }
    }

    if (!AudioEngine.IsLoadingTuneActive()) {
        AudioEngine.SetMusicFaderScalingFactor(m_fEffectsFaderScalingFactor);
        AudioEngine.SetEffectsFaderScalingFactor(m_fEffectsFaderScalingFactor);
    }
}

// unused, empty
// 0x50B880
void CCamera::ProcessObbeCinemaCameraPed() {
    // NOP
}

//
void CCamera::ProcessObbeCinemaCameraPlane() {
    assert(0);
}

//
void CCamera::ProcessObbeCinemaCameraTrain() {
    assert(0);
}

// 0x50B890
void CCamera::ProcessWideScreenOn() {
    if (m_bWantsToSwitchWidescreenOff) {
        m_bWantsToSwitchWidescreenOff = false;
        m_bWideScreenOn = false;
        m_fWideScreenReductionAmount = 0.0f;
        m_fScreenReductionPercentage = 0.0f;
        m_fFOV_Wide_Screen = 0.0f;
    } else {
        m_fWideScreenReductionAmount = 1.0f;
        m_fScreenReductionPercentage = 30.0f;
        m_fFOV_Wide_Screen = m_aCams[m_nActiveCam].m_fFOV * 0.3f;
    }
}

// 0x516440
void CCamera::ProcessVectorTrackLinear() {
    const auto now = CameraLinearTime();
    if (now <= m_fTrackLinearEndTime) {
        ProcessVectorTrackLinear(float(now - m_fTrackLinearStartTime) / (m_fTrackLinearEndTime - m_fTrackLinearStartTime));
    } else if (m_bCameraPersistTrack) {
        m_bVecTrackLinearProcessed = true;
    }
}

// 0x50D350
void CCamera::ProcessVectorTrackLinear(float ratio) {
    m_bVecTrackLinearProcessed = true;
    m_vecTrackLinear.x = CameraLinearComponent(m_vecTrackLinearEndPoint.x, m_vecTrackLinearStartPoint.x, ratio, m_bTrackLinearWithEase);
    m_vecTrackLinear.y = CameraLinearComponent(m_vecTrackLinearEndPoint.y, m_vecTrackLinearStartPoint.y, ratio, m_bTrackLinearWithEase);
    m_vecTrackLinear.z = CameraLinearComponent(m_vecTrackLinearEndPoint.z, m_vecTrackLinearStartPoint.z, ratio, m_bTrackLinearWithEase);
}

//
void CCamera::ProcessObbeCinemaCameraBoat() {
    assert(0);
}

//
void CCamera::ProcessObbeCinemaCameraCar() {
    assert(0);
}

//
void CCamera::ProcessObbeCinemaCameraHeli() {
    assert(0);
}

// 0x50D430
void CCamera::ProcessVectorMoveLinear(float ratio) {
    m_bVecMoveLinearProcessed = true;
    m_vecMoveLinear.x = CameraLinearComponent(m_vecMoveLinearPosnStart.x, m_vecMoveLinearPosnEnd.x, ratio, m_bMoveLinearWithEase);
    m_vecMoveLinear.y = CameraLinearComponent(m_vecMoveLinearPosnStart.y, m_vecMoveLinearPosnEnd.y, ratio, m_bMoveLinearWithEase);
    m_vecMoveLinear.z = CameraLinearComponent(m_vecMoveLinearPosnStart.z, m_vecMoveLinearPosnEnd.z, ratio, m_bMoveLinearWithEase);
}

// 0x516500
void CCamera::ProcessFOVLerp() {
    // Como el original (0x516500..0x516556): el reloj se compara y se resta SIN pasar por float (fild, +2^32 si es negativo)
    const float endTime = m_fEndZoomTime, startTime = m_fStartZoomTime, fourGig = 4294967296.0f;
    const int32 nowMs = int32(CTimer::GetTimeInMS());
    float ratio;
    int32 running;
    __asm {
        mov running, 0
        fild nowMs
        cmp nowMs, 0
        jge positive
        fadd fourGig
    positive:
        fcom endTime
        fnstsw ax
        test ah, 41h
        jp notRunning
        fsub startTime
        fld endTime
        fsub startTime
        fdivp st(1), st(0)
        fstp ratio
        mov running, 1
        jmp done
    notRunning:
        fstp st(0)
    done:
    }
    if (running) {
        ProcessFOVLerp(ratio);
    } else if (m_bBlockZoom) { /* Finished */
        m_bFOVLerpProcessed = true;
    }
}

// 0x50D510
void CCamera::ProcessFOVLerp(float ratio) {
    m_bFOVLerpProcessed = true;
    m_fFOVNew = CameraLinearComponent(m_fZoomInFactor, m_fZoomOutFactor, ratio, m_nZoomMode != 0);
}

// 0x5164A0
void CCamera::ProcessVectorMoveLinear() {
    const auto now = CameraLinearTime();
    if (now <= m_fMoveLinearEndTime) {
        ProcessVectorMoveLinear(float(now - m_fMoveLinearStartTime) / (m_fMoveLinearEndTime - m_fMoveLinearStartTime));
    } else if (m_bCameraPersistPosition) {
        m_bVecMoveLinearProcessed = true;
    }
}

// unused
// 0x51A6F0
void CCamera::ProcessShake() {
    plugin::CallMethod<0x51A6F0, CCamera*>(this);
}

// shakeIntensity not used
// 0x516560
CVector* CCamera::ProcessShake(float intensity) {
    return plugin::CallMethodAndReturn<CVector*, 0x516560, CCamera*, float>(this, intensity);
}

// inlined - 0x52B845
// 0x516AE0
void CCamera::ProcessScriptedCommands() {
    ProcessVectorMoveLinear();
    ProcessVectorTrackLinear();
    ProcessFOVLerp();
}

namespace {
double CameraProcessPhase(uint32 elapsed,uint32 duration) {
    const uint64 numerator=elapsed,denominator=duration;double result;
    __asm {
        fild numerator
        fild denominator
        fdivp st(1),st(0)
        fstp result
    }
    return result;
}
double CameraProcessFractionEase(double phase,float moving,float catchup,bool initial) {
    const float pi=StaticRef<float>(0x858CB8),half=.5f;double result;
    if(initial){
        if(moving==0.f)phase=0.;
        else {
            __asm {
                fld phase
                fsubr moving
                fdiv moving
                fmul pi
                fcos
                fmul half
                fsubr half
                fstp result
            }
            return result;
        }
    }else{
        if(catchup==0.f)phase=1.;
        else {
            __asm {
                fld phase
                fsub moving
                fdiv catchup
                fmul pi
                fcos
                fmul half
                fsubr half
                fstp result
            }
            return result;
        }
    }
    __asm {
        fld phase
        fmul pi
        fcos
        fmul half
        fsubr half
        fstp result
    }
    return result;
}
void CameraProcessAdvance(CVector& result,const CVector& base,const CVector& speed,double coefficient,int mode) {
    const float stored=float(coefficient);const double spilledCoefficient=mode==0?coefficient:double(stored);float product;
    if(mode==0||mode==2){
        __asm { fld coefficient }
    }else{
        __asm { fld stored }
    }
    __asm {
        mov eax,speed
        mov edx,base
        mov ecx,result
        fmul dword ptr [eax]
    }
    if(mode==1||mode==4){
        __asm { fstp product
            fld product
        }
    }
    __asm {
        mov eax,speed
        mov edx,base
        mov ecx,result
        fadd dword ptr [edx]
        fstp dword ptr [ecx]
    }
    if(mode==0||mode==4){
        __asm {
            mov eax,speed
            mov edx,base
            mov ecx,result
            fld spilledCoefficient
            fmul dword ptr [eax+4]
            fstp product
            fld product
            fadd dword ptr [edx+4]
            fstp dword ptr [ecx+4]
            fld spilledCoefficient
            fmul dword ptr [eax+8]
            fstp product
            fld product
            fadd dword ptr [edx+8]
            fstp dword ptr [ecx+8]
        }
    }else{
        __asm {
            mov eax,speed
            mov edx,base
            mov ecx,result
            fld stored
            fmul dword ptr [eax+4]
            fadd dword ptr [edx+4]
            fstp dword ptr [ecx+4]
            fld stored
            fmul dword ptr [eax+8]
        }
        if(mode!=1){
            __asm { fstp product
                fld product
            }
        }
        __asm {
            mov edx,base
            mov ecx,result
            fadd dword ptr [edx+8]
            fstp dword ptr [ecx+8]
        }
    }
}
float CameraProcessFovAdvance(float base,float operand,float coefficient,bool difference) {
    float result;
    if(difference){
        __asm { fld operand
            fsub base
        }
    }else{
        __asm { fld operand }
    }
    __asm {
        fmul coefficient
        fadd base
        fstp result
    }
    return result;
}
float CameraProcessShakeForce(float force, uint32 elapsed) {
    const double duration = double(elapsed);
    const float decay = .00028f;
    float value;
    __asm {
        fld duration
        fmul decay
        fsubr force
        fstp value
    }
    return value;
}
double CameraProcessAccumulateDistance(const CVector& current,const CVector& previous,float sum) {
    double value;
    __asm {
        mov eax,current
        mov edx,previous
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fld st(0)
        fmul st(0),st(1)
        fld st(2)
        fmul st(0),st(3)
        faddp st(1),st(0)
        fld st(3)
        fmul st(0),st(4)
        faddp st(1),st(0)
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fadd sum
        fstp value
    }
    return value;
}
float CameraProcessAverageDistance(double sum,int32 frames) {
    float value;
    __asm {
        fld sum
        fild frames
        fdivp st(1),st(0)
        fstp value
    }
    return value;
}

int32 CameraProcessTruncateProduct(float value, float multiplier) {
    int32 result;
    __asm {
        fld value
        fmul multiplier
        mov eax, 0x821B40
        call eax
        mov result, eax
    }
    return result;
}
void CameraProcessOrthonormalise(CVector& front, CVector& up, eCamMode mode) {
    up.Normalise();
    CVector side;
    if (mode == MODE_TOPDOWN || int(mode) == 37) {
        front.Normalise();
        side = CVector{-1.f, 0.f, 0.f};
        up = CrossProduct(front, side);
    } else {
        front.Normalise(); up.Normalise();
        side = CrossProduct(front, up); side.Normalise();
        up = CrossProduct(side, front);
    }
    up.Normalise();
}
void CameraProcessMinimumDistance(CVector& source, const CVector& target) {
    const float dx = source.x - target.x, dy = source.y - target.y;
    const float minimum = StaticRef<float>(0x8CCF20);
    if (std::sqrt(dx * dx + dy * dy) < minimum) {
        const float angle = CGeneral::GetATanOfXY(dx, dy);
        source.x = std::cos(angle) * minimum + target.x;
        source.y = std::sin(angle) * minimum + target.y;
    }
}
}

// 0x52B730
void CCamera::Process() {
    ResetMadeInvisibleObjects();
    m_bJust_Switched = false;
    m_vecRealPreviousCameraPosition = GetPosition();
    if (m_bLookingAtPlayer || m_bTargetJustBeenOnTrain || m_nWhoIsInControlOfTheCamera == 2) UpdateTargetEntity();
    if (!m_pTargetEntity) {
        m_pTargetEntity = FindPlayerPed(); m_pTargetEntity->RegisterReference(&m_pTargetEntity);
    }
    auto initialiseTarget = [&](CCam& camera) {
        if (!camera.m_pCamTargetEntity) {
            camera.m_pCamTargetEntity = m_pTargetEntity;
            camera.m_pCamTargetEntity->RegisterReference(&camera.m_pCamTargetEntity);
        }
    };
    initialiseTarget(m_aCams[m_nActiveCam]); initialiseTarget(m_aCams[(m_nActiveCam + 1) & 1]);
    CamControl();
    ProcessVectorMoveLinear(); ProcessVectorTrackLinear(); ProcessFOVLerp();
    if (m_bFading) ProcessFade();
    if (m_bMusicFading) ProcessMusicFade();
    if (m_bWideScreenOn) ProcessWideScreenOn();
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, .3f);
    auto frontHeading = [&] {
        const auto& front = GetActiveCam().m_vecFront;
        return front.x == 0.f && front.y == 0.f ? 0.f : CGeneral::GetATanOfXY(front.x, front.y);
    };
    const float beforeHeading = frontHeading();
    GetActiveCam().Process();
    const float afterHeading = frontHeading();
    if (m_bTransitionState && m_nTransitionDuration + m_nTimeTransitionStart < CTimer::GetTimeInMS()) {
        m_bTransitionState = false; m_bDoingSpecialInterp = false; m_bWaitForInterpolToFinish = false;
    }
    if (m_bUseNearClipScript) RwCameraSetNearClipPlane(Scene.m_pRwCamera, m_fNearClipScript);
    float headingChange = afterHeading - beforeHeading;
    while (headingChange >= StaticRef<float>(0x858CB8)) headingChange -= StaticRef<float>(0x858CBC);
    while (headingChange < -StaticRef<float>(0x858CB8)) headingChange += StaticRef<float>(0x858CBC);
    if (std::abs(headingChange) > .3f) m_bJust_Switched = true;
    const bool forceVehicle = GetActiveCam().m_nDirectionWasLooking != 3 && m_pTargetEntity->GetIsTypeVehicle();
    const float now = float(CTimer::GetTimeInMS());
    if (now <= m_fEndShakeTime) ProcessShake((now - m_fStartShakeTime) / (m_fEndShakeTime - m_fStartShakeTime));
    CVector source, front, up;
    float fov = 0.f;
    if (!m_bTransitionState || forceVehicle) {
        auto& active = GetActiveCam(); source = active.m_vecSource; up = active.m_vecUp;
        if (!m_bMoveCamToAvoidGeom) front = active.m_vecFront;
        else {
            source += m_vecClearGeometryVec;
            front = active.m_vecTargetCoorsForFudgeInter - source; front.Normalise();
            CVector side = CrossProduct(front, up); side.Normalise();
            up = CrossProduct(side, front); up.Normalise();
        }
        StaticRef<bool>(0xB70143) = false;
        fov = active.m_fFOV;
    } else {
        const uint32 elapsed = std::min(CTimer::GetTimeInMS() - m_nTimeTransitionStart, m_nTransitionDuration);
        const float phase=float(CameraProcessPhase(elapsed,m_nTransitionDuration));
        double targetPhase=CameraProcessPhase(elapsed,m_nTransitionDurationTargetCoors);
        if (targetPhase < 0.) targetPhase = 0.; else if (targetPhase > 1.) targetPhase = 1.;
        CVector targetPosition = m_vecTargetWhenInterPol;
        if (targetPhase <= double(m_fFractionInterToStopMovingTarget)) {
            const double ease=CameraProcessFractionEase(targetPhase,m_fFractionInterToStopMovingTarget,m_fFractionInterToStopCatchUpTarget,true);
            CameraProcessAdvance(targetPosition, m_vecStartingTargetForInterPol, m_vecTargetSpeedAtStartInter, ease, 0);
            m_vecTargetWhenInterPol = targetPosition;
        } else if (double(m_fFractionInterToStopMovingTarget) < targetPhase) {
            const double ease=CameraProcessFractionEase(targetPhase,m_fFractionInterToStopMovingTarget,m_fFractionInterToStopCatchUpTarget,false);
            if (m_fFractionInterToStopMovingTarget == 0.f) m_vecTargetWhenInterPol = m_vecStartingTargetForInterPol;
            CameraProcessAdvance(targetPosition, m_vecTargetWhenInterPol, GetActiveCam().m_vecTargetCoorsForFudgeInter - m_vecTargetWhenInterPol, ease, 1);
        }
        if (phase <= m_fFractionInterToStopMoving) {
            const double ease=CameraProcessFractionEase(double(phase),m_fFractionInterToStopMoving,m_fFractionInterToStopCatchUp,true);
            CameraProcessAdvance(m_vecSourceWhenInterPol, m_vecStartingSourceForInterPol, m_vecSourceSpeedAtStartInter, ease, 2);
            if (m_bLookingAtPlayer) CameraProcessMinimumDistance(m_vecSourceWhenInterPol, targetPosition);
            CameraProcessAdvance(m_vecUpWhenInterPol, m_vecStartingUpForInterPol, m_vecUpSpeedAtStartInter, ease, 3);
            m_fFOVWhenInterPol = CameraProcessFovAdvance(m_fStartingFOVForInterPol,m_fFOVSpeedAtStartInter,float(ease),false);
            source = m_vecSourceWhenInterPol;
            front = targetPosition - source;
            StoreValuesDuringInterPol(&source, &m_vecTargetWhenInterPol, &m_vecUpWhenInterPol, &m_fFOVWhenInterPol);
            front.Normalise();
            up = m_bLookingAtPlayer ? CVector{0.f,0.f,1.f} : m_vecUpWhenInterPol;
            CameraProcessOrthonormalise(front, up, GetActiveCam().m_nMode);
            fov = m_fFOVWhenInterPol;
        } else if (m_fFractionInterToStopMoving < phase && phase <= 1.f) {
            const double ease=CameraProcessFractionEase(double(phase),m_fFractionInterToStopMoving,m_fFractionInterToStopCatchUp,false);
            CameraProcessAdvance(source, m_vecSourceWhenInterPol, GetActiveCam().m_vecSource - m_vecSourceWhenInterPol, ease, 4);
            if (m_bLookingAtPlayer) CameraProcessMinimumDistance(source, targetPosition);
            fov = CameraProcessFovAdvance(m_fFOVWhenInterPol,GetActiveCam().m_fFOV,float(ease),true);
            CameraProcessAdvance(up, m_vecUpWhenInterPol, GetActiveCam().m_vecUp - m_vecUpWhenInterPol, ease, 4);
            front = targetPosition - source;
            StoreValuesDuringInterPol(&source, &targetPosition, &up, &fov);
            front.Normalise();
            if (m_bLookingAtPlayer) up = CVector{0.f,0.f,1.f};
            CameraProcessOrthonormalise(front, up, GetActiveCam().m_nMode);
            fov = m_fFOVWhenInterPol;
        }
        const CVector reverse = source - targetPosition;
        const float alpha = CGeneral::GetATanOfXY(std::sqrt(reverse.x * reverse.x + reverse.y * reverse.y), reverse.z);
        const float beta = CGeneral::GetATanOfXY(reverse.x, reverse.y);
        GetActiveCam().KeepTrackOfTheSpeed(source, targetPosition, up, alpha, beta, fov);
    }
    if (m_bTransitionState && !m_bLookingAtVector && m_bLookingAtPlayer && !CCullZones::CamStairsForPlayer() && !m_bPlayerIsInGarage) {
        CColPoint collision;
        CEntity* entity = nullptr;
        if (CWorld::ProcessLineOfSight(m_pTargetEntity->GetPosition(), source, collision, entity, true, false, false, true, false, true, true, false)) {
            source = collision.m_vecPoint; RwCameraSetNearClipPlane(Scene.m_pRwCamera, .05f);
        }
    }
    const float drunk = StaticRef<float>(0xC73C58);
    if (drunk > 0.f) {
        const float angle = StaticRef<float>(0xB6EC30) * StaticRef<float>(0x8595EC);
        const float cosine = std::cos(angle), sine = std::sin(angle);
        source.x += cosine * drunk * -.02f; source.z += sine * drunk * -.02f;
        up.Normalise(); up.x += cosine * drunk * .05f; up.y += sine * drunk * .05f;
        up.Normalise(); front.Normalise(); front.x += cosine * drunk * -.1f; front.y += sine * drunk * -.1f;
        front.Normalise(); CVector side = CrossProduct(front, up); side.Normalise(); up = CrossProduct(side, front); up.Normalise();
        StaticRef<float>(0xB6EC30) += 5.f;
    }
    m_mCameraMatrix.GetRight() = CrossProduct(up, front);
    m_mCameraMatrix.GetUp() = up; m_mCameraMatrix.GetForward() = front; m_mCameraMatrix.GetPosition() = source;
    float shake = CameraProcessShakeForce(m_fCamShakeForce, CTimer::GetTimeInMS() - m_nCamShakeStart);
    if (shake >= 2.f) shake = 2.f; else if (shake <= 0.f) shake = 0.f;
    const uint32 random = uint32(std::rand()); const float amount = shake * .1f;
    m_mCameraMatrix.GetPosition().x += float(int32((random & 15) - 7)) * amount;
    m_mCameraMatrix.GetPosition().y += float(int32(((random >> 4) & 15) - 7)) * amount;
    m_mCameraMatrix.GetPosition().z += float(int32(((random >> 8) & 15) - 7)) * amount;
    if (amount > 0.f && int(m_nBlurType) != 1) m_nMotionBlurAddAlpha = uint32(std::min(150, 25 - CameraProcessTruncateProduct(shake, StaticRef<float>(0x86327C))));
    if (int(GetActiveCam().m_nMode) == 16 && FindPlayerVehicle(-1, false) && FindPlayerVehicle(-1, false)->GetMatrix().GetUp().z < .2f) {
        m_nBlurBlue = 255; m_nBlurGreen = 255; m_nBlurRed = 255; m_nBlurType = static_cast<eMotionBlurType>(1); m_nMotionBlur = 240;
        StaticRef<bool>(0xB70142) = true;
    } else if (StaticRef<bool>(0xB70142)) StaticRef<bool>(0xB70142) = false;
    CDraw::SetFOV(fov); CalculateDerivedValues(false, true); CopyCameraMatrixToRWCam(false);
    m_vecGameCamPos = m_mCameraMatrix.GetPosition(); UpdateSoundDistances();
    m_fLODDistMultiplier = !StaticRef<bool>(0xB5F851) || StaticRef<bool>(0xB5F853) ? 70.f / CDraw::ms_fFOV : 1.f;
    m_fGenerationDistMultiplier = m_fLODDistMultiplier;
    m_fLODDistMultiplier *= CRenderer::ms_lodDistScale;
    const int32 farClip = CameraProcessTruncateProduct(StaticRef<float>(uintptr_t(Scene.m_pRwCamera) + 0x84), 100.f);
    RwCameraSetFarClipPlane(Scene.m_pRwCamera, float(farClip) * .01f);
    StaticRef<float>(0xC3EFA0) = StaticRef<float>(uintptr_t(m_pRwCamera) + 0x80);
    StaticRef<float>(0xC3EF9C) = StaticRef<float>(uintptr_t(m_pRwCamera) + 0x84);
    if (m_bJustInitialized || m_bJust_Switched) { m_vecPreviousCameraPosition = m_mCameraMatrix.GetPosition(); m_bJustInitialized = false; }
    ++m_nNumFramesSoFar;
    const double accumulatedDistance = CameraProcessAccumulateDistance(m_mCameraMatrix.GetPosition(), m_vecPreviousCameraPosition, m_fCameraSpeedSoFar);
    m_fCameraSpeedSoFar = float(accumulatedDistance);
    if (m_nNumFramesSoFar == m_nWorkOutSpeedThisNumFrames) {
        m_fCameraSpeedSoFar = 0.f; m_nNumFramesSoFar = 0;
        m_fCameraAverageSpeed = CameraProcessAverageDistance(accumulatedDistance, int32(m_nWorkOutSpeedThisNumFrames));
    }
    const float orientation = m_fOrientation + StaticRef<float>(0x858CB8);
    m_vecPreviousCameraPosition = m_mCameraMatrix.GetPosition();
    if (GetActiveCam().m_nDirectionWasLooking != 3 && int(GetActiveCam().m_nMode) != 37) {
        GetActiveCam().m_vecSource = GetActiveCam().m_vecSourceBeforeLookBehind; m_fOrientation = orientation;
    }
    if (m_bTransitionState) {
        auto& other = m_aCams[(m_nActiveCam + 1) & 1];
        if (other.m_pCamTargetEntity && m_pTargetEntity && m_pTargetEntity->GetIsTypePed() && !other.m_pCamTargetEntity->GetIsTypeVehicle() && int(GetActiveCam().m_nMode) != 37 && other.m_nDirectionWasLooking != 3) {
            other.m_vecSource = m_aCams[m_nActiveCam & 1].m_vecSourceBeforeLookBehind; m_fOrientation = orientation;
        }
    }
    m_bCameraJustRestored = false; m_bMoveCamToAvoidGeom = false;
    CVector underwaterPoint = GetPosition();
    if (m_matrix) underwaterPoint += m_matrix->GetForward() * .4f;
    else { underwaterPoint.x -= std::sin(m_placement.m_fHeading) * .4f; underwaterPoint.y += std::cos(m_placement.m_fHeading) * .4f; }
    float waterLevel;
    if (CWaterLevel::GetWaterLevel(underwaterPoint.x, underwaterPoint.y, underwaterPoint.z, waterLevel, true, nullptr) && underwaterPoint.z - .6f <= waterLevel) {
        CWeather::WaterDepth = std::max(0.f, waterLevel - underwaterPoint.z);
        CWeather::UnderWaterness = underwaterPoint.z + .6f < waterLevel ? 1.f : 1.f - (underwaterPoint.z - (waterLevel - .6f)) * .8333333f;
    } else CWeather::UnderWaterness = 0.f;
}

// 0x514860
void CCamera::DrawBordersForWideScreen() {
    // Como el original (0x514860..0x51494F): el CRect del EXE es (izquierda, arriba, derecha, abajo); el constructor de la DLL recibe
    // (izquierda, ABAJO, derecha, ARRIBA), asi que se rellenan los campos a mano para que +4 y +0xC coincidan con los del EXE.
    CRect rect;
    GetScreenRect(&rect);
    if (m_nBlurType == eMotionBlurType::NONE || m_nBlurType == eMotionBlurType::LIGHT_SCENE) {
        m_nMotionBlurAddAlpha = 80;
    }
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

    CRect upper;
    upper.left   = -5.0f;
    upper.top    = -5.0f;
    upper.right  = float(RsGlobal.maximumWidth + 5);
    upper.bottom = rect.top;
    CSprite2d::DrawRect(upper, CRGBA{ 0, 0, 0, 255 });

    CRect lower;
    lower.left   = -5.0f;
    lower.top    = rect.bottom;
    lower.right  = float(RsGlobal.maximumWidth + 5);
    lower.bottom = float(RsGlobal.maximumHeight + 5);
    CSprite2d::DrawRect(lower, CRGBA{ 0, 0, 0, 255 });
}

// 0x4748A0
bool CCamera::VectorMoveRunning() const {
    return CTimer::m_snTimeInMilliseconds <= m_fMoveLinearEndTime;
}

// 0x474891
bool CCamera::VectorTrackRunning() const {
    return CTimer::m_snTimeInMilliseconds <= m_fTrackLinearEndTime;
}

// 0x514950
void CCamera::FinishCutscene() {
    SetPercentAlongCutScene(100.0f);
    m_fPositionAlongSpline = 1.0f;
    m_bCutsceneFinished = true;
}

// 0x514970
void CCamera::Find3rdPersonCamTargetVector(float range, CVector gunMuzzle, CVector& outSource, CVector& outTarget) {
    // Como el original (0x514970..0x514B75): el vector de apuntado se construye directamente en outTarget (front + up * v + right * h),
    // con tan() de fptan a precision completa y el producto (1 / aspecto) del EXE.
    auto& cam = m_aCams[m_nActiveCam];
    const float fov = cam.m_fFOV, crosshairX = m_f3rdPersonCHairMultX, crosshairY = m_f3rdPersonCHairMultY, aspect = CDraw::ms_fAspectRatio;
    const float half = StaticRef<float>(0x858B8C), toRadians = StaticRef<float>(0x8595EC), one = StaticRef<float>(0x858624);
    float horizontal, vertical;
    __asm {
        fld fov
        fmul half
        fmul toRadians
        fptan
        fstp st(0)
        fld crosshairX
        fsub half
        fadd st(0), st(0)
        fmul st(0), st(1)
        fstp horizontal
        fld half
        fsub crosshairY
        fadd st(0), st(0)
        fld one
        fdiv aspect
        fmulp st(1), st(0)
        fmulp st(1), st(0)
        fstp vertical
    }

    std::memcpy(&outSource, &cam.m_vecSource, sizeof(CVector));
    std::memcpy(&outTarget, &cam.m_vecFront, sizeof(CVector));

    if (cam.m_nMode == eCamMode::MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
        cam.Get_TwoPlayer_AimVector(outTarget);
    } else {
        outTarget.x = vertical * cam.m_vecUp.x + outTarget.x;
        outTarget.y = vertical * cam.m_vecUp.y + outTarget.y;
        outTarget.z = vertical * cam.m_vecUp.z + outTarget.z;

        CVector right;
        CrossProduct(&right, &cam.m_vecFront, &cam.m_vecUp);
        outTarget.x = horizontal * right.x + outTarget.x;
        outTarget.y = horizontal * right.y + outTarget.y;
        outTarget.z = horizontal * right.z + outTarget.z;
        outTarget.Normalise();
    }

    // Proyeccion de la boca del arma sobre la direccion (suma en el orden x, z, y del EXE)
    const float dx = gunMuzzle.x - outSource.x, dy = gunMuzzle.y - outSource.y, dz = gunMuzzle.z - outSource.z;
    const float projected = (dx * outTarget.x + dz * outTarget.z) + dy * outTarget.y;
    outSource.x = projected * outTarget.x + outSource.x;
    outSource.y = projected * outTarget.y + outSource.y;
    outSource.z = projected * outTarget.z + outSource.z;

    const float reachX = range * outTarget.x, reachY = range * outTarget.y, reachZ = range * outTarget.z;
    outTarget.x = outSource.x + reachX;
    outTarget.y = outSource.y + reachY;
    outTarget.z = outSource.z + reachZ;
}

// 0x514B80
float CCamera::CalculateGroundHeight(eGroundHeightType type) {
    static auto& lastCalcCamPos    = StaticRef<CVector>(0xB70034);
    static auto& exactGroundHeight = StaticRef<float>(0xB70030);
    static auto& bbTopZ            = StaticRef<float>(0xB7002C);
    static auto& bbBottomZ         = StaticRef<float>(0xB70028);

    // Como el original (0x514B80): bandera de inicializacion de las estaticas (bit 0 de 0xB70040)
    auto& initFlags = StaticRef<uint32>(0xB70040);
    if (!(initFlags & 1)) {
        initFlags |= 1;
        lastCalcCamPos = CVector{ 0.0f, 0.0f, 0.0f };
    }

    const auto& camPos = GetPosition();

    // Possibly update the positions (If the camera has moved enough)
    const auto CheckDelta = [](float d) { return std::abs(d) > 20.f; };
    if (CheckDelta(lastCalcCamPos.x - camPos.x) || CheckDelta(lastCalcCamPos.y - camPos.y) || CheckDelta(lastCalcCamPos.z - camPos.z)) { // Check if there's enough of a delta
        CColPoint cp;
        CEntity* hitEntity;
        if (CWorld::ProcessVerticalLine({ camPos.x, camPos.y, 1000.f }, -1000.f, cp, hitEntity, true, false, false, false, true)) {
            const auto& hitEntPos = hitEntity->GetPosition();
            // 0x514C7C: el modelo de colision sale directamente de la tabla de CModelInfo (movsx del indice del modelo)
            const auto& hitBB = hitEntity->GetModelInfo()->GetColModel()->GetBoundingBox();

            exactGroundHeight = cp.m_vecPoint.z;

            bbTopZ = hitBB.m_vecMax.z + hitEntPos.z;

            const auto bbsz = hitBB.GetSize();
            if (bbsz.x > 120.f || bbsz.y > 120.f) {
                bbBottomZ = exactGroundHeight;
            } else {
                bbBottomZ = hitEntPos.z + hitBB.m_vecMin.z;
            }
            if (bbBottomZ < 0.0f) {
                bbBottomZ = 0.0f;
            }
        }
        lastCalcCamPos = camPos;
    }

    switch (type) {
    case eGroundHeightType::ENTITY_BB_TOP:       return bbTopZ;
    case eGroundHeightType::EXACT_GROUND_HEIGHT: return exactGroundHeight;
    default:                                     return bbBottomZ; // 0x514D4A: cualquier otro valor devuelve el limite inferior
    }
}

// 0x514D60
static float CameraFrustumDot355(const CVector& normal,const CVector& position) {
    float result;
    const auto* n=&normal;const auto* p=&position;
    __asm {
        mov eax,n
        mov edx,p
        fld dword ptr [eax+4]
        fmul dword ptr [edx+4]
        fld dword ptr [eax+8]
        fmul dword ptr [edx+8]
        faddp st(1),st
        fld dword ptr [eax]
        fmul dword ptr [edx]
        faddp st(1),st
        fstp result
    }
    return result;
}

void CCamera::CalculateFrustumPlanes(bool bForMirror) {
    const float fov = CDraw::GetFOV(), factor = StaticRef<float>(0x8631D8);
    float cosine, sine;
    __asm {
        fld fov
        fmul factor
        fld st(0)
        fcos
        fstp cosine
        fsin
        fstp sine
    }
    const int32 height = StaticRef<int32>(0xC17048), width = StaticRef<int32>(0xC17044);
    float ratio, verticalCosine, verticalSine;
    __asm {
        fild height
        fidiv width
        fst ratio
        fmul cosine
        fstp verticalCosine
        fld ratio
        fmul sine
        fchs
        fstp verticalSine
    }
    m_avecFrustumNormals[0] = CVector{cosine,-sine,0.0f};
    m_avecFrustumNormals[1] = CVector{-cosine,-sine,0.0f};
    m_avecFrustumNormals[2] = CVector{0.0f,verticalSine,-verticalCosine};
    m_avecFrustumNormals[3] = CVector{0.0f,verticalSine,verticalCosine};
    auto& normals = bForMirror ? m_avecFrustumWorldNormals_Mirror : m_avecFrustumWorldNormals;
    auto& offsets = bForMirror ? m_fFrustumPlaneOffsets_Mirror : m_fFrustumPlaneOffsets;
    TransformVectors(reinterpret_cast<RwV3d*>(normals.data()),4,m_mCameraMatrix,reinterpret_cast<const RwV3d*>(m_avecFrustumNormals.data()));
    for (size_t i = 0; i < normals.size(); ++i) {
        const auto& normal = normals[i];
        const auto& position = GetPosition();
        offsets[i] = CameraFrustumDot355(normal,position);
    }
}

// 0x5150E0
void CCamera::CalculateDerivedValues(bool bForMirror, bool bOriented) {
    m_mMatInverse = Invert(m_mCameraMatrix);
    CalculateFrustumPlanes(bForMirror);
    auto& forward = m_mCameraMatrix.GetForward();
    if (forward.x == 0.0f && forward.y == 0.0f) {
        forward.x = std::bit_cast<float>(0x38D1B717u);
    } else if (bOriented) {
        const float x = forward.x, y = forward.y;
        float orientation;
        __asm {
            fld x
            fld y
            fpatan
            fstp orientation
        }
        m_fOrientation = orientation;
    }
    m_fCamFrontXNorm = forward.x;
    m_fCamFrontYNorm = forward.y;
    const float x = forward.x, y = forward.y, one = 1.0f;
    float nx, ny;
    __asm {
        fld x
        fld y
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fsqrt
        ftst
        fnstsw ax
        test ah,44h
        jp derived_normalize
        fstp st(0)
        fld one
        fstp nx
        fld y
        fstp ny
        jmp derived_finished
    derived_normalize:
        fdivr one
        fld st(0)
        fmul st,st(3)
        fstp nx
        fmul st,st(1)
        fstp ny
    derived_finished:
        fstp st(0)
        fstp st(0)
    }
    m_fCamFrontXNorm = nx;
    m_fCamFrontYNorm = ny;
}

// 0x516B20
void CCamera::ImproveNearClip(CVehicle* vehicle, CPed* ped, CVector* source, CVector* targPosn) {
    const auto delta=*source-*targPosn;const float distance=std::sqrt((delta.z*delta.z+delta.y*delta.y)+delta.x*delta.x);
    auto* camera=Scene.m_pRwCamera;
    if(distance>StaticRef<float>(0x8CCD08)){const float nearClipValue=StaticRef<float>(0x8CCD04)*gCurDistForCam;if(camera->nearPlane<nearClipValue)RwCameraSetNearClipPlane(camera,nearClipValue);}
    const uint8 active=*reinterpret_cast<const uint8*>(reinterpret_cast<const uint8*>(this)+0x59);
    auto* activeCam=reinterpret_cast<CCam*>(reinterpret_cast<uint8*>(this)+0x174+uint32(active)*sizeof(CCam));
    if(vehicle){
        const auto type=vehicle->m_nVehicleSubType;
        if(type==VEHICLE_TYPE_HELI||type==VEHICLE_TYPE_PLANE){
            if(gCurDistForCam>StaticRef<float>(0x8CCD00)){
                const float ground=CalculateGroundHeight(static_cast<eGroundHeightType>(0));
                if(activeCam->m_vecSource.z-ground>StaticRef<float>(0x8CCCFC)){
                    const float limit=StaticRef<float>(0x8CCCF8)*gCurDistForCam,value=distance*StaticRef<float>(0x8CCCF4);const float nearClipValue=value<limit?value:limit;
                    if(camera->nearPlane<nearClipValue)RwCameraSetNearClipPlane(camera,nearClipValue);
                }
            }else if(type==VEHICLE_TYPE_HELI)RwCameraSetNearClipPlane(camera,StaticRef<float>(0x8CCCF0));
        }
    }else if(ped){
        if(!ped->bIsStanding){
            const bool parachute=ped->GetIntelligence()->GetUsingParachute();auto* swim=ped->GetIntelligence()->GetTaskSwim();
            if(swim){float water=0;const bool found=CWaterLevel::GetWaterLevel(source->x,source->y,source->z,water,0,nullptr);
                if(found&&std::abs(water-source->z)<StaticRef<float>(0x8CCCEC))RwCameraSetNearClipPlane(camera,StaticRef<float>(0x8CCCE8));
                else if(*reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(swim)+0xA)==4&&StaticRef<int32>(0xB6F0F0)==1)RwCameraSetNearClipPlane(camera,StaticRef<float>(0x8CCCE4));
            }else if(parachute||ped->GetIntelligence()->GetTaskJetPack()){
                if(GetRoughDistanceToGround()>StaticRef<float>(0x8CCCFC)){
                    const float value=delta.Magnitude()*StaticRef<float>(0x8CCCDC),limit=StaticRef<float>(0x8CCCE0)*gCurDistForCam;
                    const float nearClipValue=value<limit?value:limit;if(nearClipValue>camera->nearPlane)RwCameraSetNearClipPlane(camera,nearClipValue);
                }
            }
        }else{
            auto* model=static_cast<CPedModelInfo*>(CModelInfo::GetModelInfo(int16(ped->GetModelIndex())));
            const float fov=StaticRef<float>(0xB6F250+uint32(StaticRef<uint8>(0xB6F081))*sizeof(CCam));
            const float angle=(90.0f-fov*.5f)*std::bit_cast<float>(0x3C8EFA35u),radius=StaticRef<float>(0xB6EC6C);float limit;
            __asm { fld angle
                fsin
                fmul radius
                fstp limit
            }
            model->AnimatePedColModelSkinnedWorld(ped->GetRpClump());
            const auto& cam=StaticRef<CCam>(0xB6F19C+uint32(active)*sizeof(CCam));
            const float plane=(cam.m_vecFront.z*cam.m_vecSource.z+cam.m_vecFront.y*cam.m_vecSource.y)+cam.m_vecSource.x*cam.m_vecFront.x;
            const auto* spheres=model->m_pHitColModel->m_pColData->m_pSpheres;float minimum=999999.0f;
            for(int32 i=0;i<12;++i){const auto& sphere=spheres[i];const auto& center=sphere.m_vecCenter;
                float value=(center.x*cam.m_vecFront.x+center.z*cam.m_vecFront.z)+center.y*cam.m_vecFront.y;value=(value-plane)-sphere.m_fRadius;
                if(sphere.m_Surface.m_nPiece==9)value-=StaticRef<float>(0x8CCCD8)*sphere.m_fRadius;
                if(value<minimum)minimum=value;
            }
            float nearClipValue;
            if(minimum>limit)nearClipValue=limit;else nearClipValue=minimum<StaticRef<float>(0x8CCCD4)?StaticRef<float>(0x8CCCD4):minimum;
            if(nearClipValue>StaticRef<float>(0x8CCCD0))nearClipValue=StaticRef<float>(0x8CCCD0);
            nearClipValue=float(int32(nearClipValue*100.0f))*.01f;RwCameraSetNearClipPlane(camera,nearClipValue);
        }
    }
    float nearest=999999.0f;CCollision::CheckPeds(*source,activeCam->m_vecFront,nearest);
}

static auto& preMirrorMat = StaticRef<CMatrix>(0xB6FE40);

// 0x51A560
void CCamera::SetCameraUpForMirror() {
    preMirrorMat = m_mCameraMatrix;
    m_mCameraMatrix = m_mMatMirror;
    CopyCameraMatrixToRWCam(true);
    CalculateDerivedValues(true, false);
}

// 0x51A5A0
void CCamera::RestoreCameraAfterMirror() {
    SetMatrix(preMirrorMat);
    CopyCameraMatrixToRWCam(true);
    CalculateDerivedValues(false, false);
}

// 0x51A5D0
bool CCamera::ConeCastCollisionResolve(const CVector& pos, const CVector& lookAt, CVector& outDest, float radius, float minDist, float& outDist) {
    // Como el original (0x51A5D0..0x51A6E0): igualdad componente a componente, esferas con material 0, pieza 0 e iluminacion 0xFF,
    // y destino = (pos - lookAt) * dist + lookAt (no la lerp generica `to * t + from * (1 - t)`).
    if (pos.x == lookAt.x && pos.y == lookAt.y && pos.z == lookAt.z) {
        return false;
    }

    const CColSphere lookAtSphere{ CSphere{ lookAt, radius }, eSurfaceType(0), 0, tColLighting(0xFF) };
    const CColSphere posSphere{ CSphere{ pos, radius }, eSurfaceType(0), 0, tColLighting(0xFF) };
    if (CCollision::CameraConeCastVsWorldCollision(lookAtSphere, posSphere, outDist, minDist)) {
        const float dx = pos.x - lookAt.x, dy = pos.y - lookAt.y, dz = pos.z - lookAt.z;
        const float dist = outDist;
        outDest.x = dx * dist + lookAt.x;
        outDest.y = dy * dist + lookAt.y;
        outDest.z = dz * dist + lookAt.z;
        return true;
    } else {
        std::memcpy(&outDest, &pos, sizeof(CVector));
        outDist = 1.f;
        return false;
    }
}

// 0x51E560
static CVector CineyModeRotatedDirection(CVector velocity, float rotation) {
    const float inputX = velocity.x, inputY = velocity.y; float outputX, outputY, sine;
    const uintptr_t angleFunction = 0x53CC70;
    __asm {
        push inputY
        push inputX
        call angleFunction
        add esp, 8
        fadd rotation
        fld st(0)
        fcos
        fxch st(1)
        fsin
        fstp sine
        fld inputX
        fadd st, st(1)
        fstp outputX
        fstp st(0)
        fld sine
        fadd inputY
        fstp outputY
    }
    CVector direction{outputX, outputY, 0.f}; direction.Normalise(); return direction;
}

bool CCamera::TryToStartNewCamMode(int32 camSequence) {
    const auto boatBlocked = [this](CVehicle* vehicle) { return vehicle && *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(vehicle) + 0x590) == 5 && m_pTargetEntity->m_nModelIndex != 460; };
    const auto finish = [this](int32 mode) {
        TakeControl(FindPlayerEntity(-1), static_cast<eCamMode>(mode), static_cast<eSwitchType>(2), 2);
        return !reinterpret_cast<bool(__cdecl*)()>(0x50B830)();
    };
    const auto fixed = [this](const CVector& position) { SetCamPositionForFixedMode(position, CVector{0.f, 0.f, 0.f}); };
    const auto clear = [](const CVector& position) { return CWorld::GetIsLineOfSightClear(FindPlayerCoors(-1), position, true, false, false, false, false, false, false); };
    const auto ground = [](CVector& position, float raise) {
        bool found;
        float height = CWorld::FindGroundZFor3DCoord(CVector{position.x, position.y, position.z + 5.f}, &found, nullptr);
        if (!found) height = CWorld::FindRoofZFor3DCoord(position.x, position.y, position.z - 5.f, &found);
        if (found) position.z = height + raise;
    };
    const auto waterFloor = [](CVector& position, float level) {
        float water;
        if (CWaterLevel::GetWaterLevelNoWaves(position, &water, nullptr, nullptr)) {
            float clearance = StaticRef<float>(0x8CC8C0);
            if (auto* vehicle = FindPlayerVehicle(-1, false)) {
                if (*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(FindPlayerVehicle(-1, false)) + 0x590) == 5) clearance = StaticRef<float>(0x8CC8C4);
            }
            if (position.z < clearance + water) position.z = clearance + water;
        }
        (void)level;
    };
    if (camSequence == 0) {
        auto* vehicle = FindPlayerVehicle(-1, false);
        if (!vehicle || boatBlocked(vehicle) || vehicle->m_nModelIndex == 432) return false;
        CVector position = FindPlayerVehicle(-1, false)->GetMatrix().TransformVector(CVector{-1.4f, -2.3f, .3f});
        position += FindPlayerVehicle(-1, false)->GetPosition(); FindPlayerVehicle(-1, false);
        if (!CWorld::GetIsLineOfSightClear(vehicle->GetPosition(), position, true, false, false, false, false, false, false)) return false;
        TakeControl(vehicle, static_cast<eCamMode>(14), static_cast<eSwitchType>(2), 2); return true;
    }
    if (camSequence == 6 || camSequence == 29) {
        TakeControl(FindPlayerEntity(-1), static_cast<eCamMode>(camSequence == 6 ? 16 : 18), static_cast<eSwitchType>(2), 2); return true;
    }
    if (camSequence == 7 || camSequence == 8) {
        if (int32(FindPlayerPed(-1)->GetWantedLevel()) <= 0 || !FindPlayerVehicle(-1, false)) return false;
        auto* pool = reinterpret_cast<void*>(StaticRef<uintptr_t>(0xB74494));
        int32 remaining = *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(pool) + 8);
        auto* vehicle = FindPlayerVehicle(-1, false);
        if (vehicle && boatBlocked(FindPlayerVehicle(-1, false))) return false;
        while (remaining != 0) {
            --remaining;
            auto* candidate = reinterpret_cast<CVehicle*(__thiscall*)(void*,int32)>(0x41CC10)(pool, remaining);
            if (!candidate || *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(candidate) + 0x590) != 0 || candidate == FindPlayerVehicle(-1, false) || !(reinterpret_cast<uint8*>(candidate)[0x428] & 1)) continue;
            if (camSequence == 7 && (reinterpret_cast<uint8*>(candidate)[0x36] & 0xF8) != 0x18) continue;
            const float deltaX = candidate->GetPosition().x - FindPlayerCoors(-1).x;
            const float deltaY = candidate->GetPosition().y - FindPlayerCoors(-1).y;
            if ((FindPlayerCoors(-1) - candidate->GetPosition()).Magnitude() >= 30.f) continue;
            const float ahead = deltaX * FindPlayerVehicle(-1, false)->GetMatrix().GetForward().x + deltaY * FindPlayerVehicle(-1, false)->GetMatrix().GetForward().y;
            if (!(ahead < 0.f)) continue;
            const float alignment = FindPlayerVehicle(-1, false)->GetMatrix().GetForward().x * candidate->GetMatrix().GetForward().x + FindPlayerVehicle(-1, false)->GetMatrix().GetForward().y * candidate->GetMatrix().GetForward().y;
            if (!(alignment > .8f)) continue;
            if (camSequence == 7) {
                TakeControl(candidate, static_cast<eCamMode>(18), static_cast<eSwitchType>(2), 2);
                if (!reinterpret_cast<bool(__cdecl*)()>(0x50B830)()) return true;
            } else {
                CVector position = candidate->GetMatrix().TransformVector(CVector{-1.4f, -2.3f, .3f}); position += candidate->GetPosition();
                if (!CWorld::GetIsLineOfSightClear(candidate->GetPosition(), position, true, false, false, false, false, false, false)) return false;
                TakeControl(candidate, static_cast<eCamMode>(14), static_cast<eSwitchType>(2), 2);
                if (!reinterpret_cast<bool(__cdecl*)()>(0x50B830)()) return true;
            }
        }
        return false;
    }
    if (camSequence >= 20 && camSequence <= 28) {
        auto& camera = GetActiveCam(); bool started; int32 mode;
        switch (camSequence) {
        case 20: started = camera.Process_DW_HeliChaseCam(true); mode = 56; break;
        case 21: started = camera.Process_DW_CamManCam(true); mode = 57; break;
        case 22: started = camera.Process_DW_BirdyCam(true); mode = 58; break;
        case 23: started = camera.Process_DW_PlaneSpotterCam(true); mode = 59; break;
        case 24: case 25: StaticRef<uint8>(0xB6F059) = 0; return false;
        case 26: started = camera.Process_DW_PlaneCam1(true); mode = 62; break;
        case 27: started = camera.Process_DW_PlaneCam2(true); mode = 63; break;
        default: started = camera.Process_DW_PlaneCam3(true); mode = 64; break;
        }
        return started && finish(mode);
    }
    if (camSequence != 1 && camSequence != 2 && camSequence != 3 && camSequence != 5 && (camSequence < 15 || camSequence > 19)) return false;
    if (camSequence >= 15 && camSequence <= 17 && !FindPlayerVehicle(-1, false)) return false;
    if (camSequence == 2) {
        auto* vehicle = FindPlayerVehicle(-1, false);
        if (vehicle && boatBlocked(FindPlayerVehicle(-1, false))) return false;
    }
    CVector position = FindPlayerCoors(-1); CVector direction;
    if (camSequence == 18 || camSequence == 19) {
        auto* vehicle = FindPlayerVehicle(-1, false);
        const bool boat = vehicle && *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(FindPlayerVehicle(-1, false)) + 0x590) == 5;
        position.z += camSequence == 18 ? (boat ? 23.f : -23.f) : (boat ? 4.f : -1.f);
        direction = FindPlayerSpeed(-1);
    } else { direction = FindPlayerSpeed(-1); direction.z = 0.f; direction.Normalise(); }
    if (camSequence == 1 || camSequence == 2 || camSequence == 3 || camSequence == 5) {
        const float distance = camSequence == 1 ? 20.f : camSequence == 2 ? 16.f : 30.f;
        const float side = camSequence == 1 ? 3.f : camSequence == 2 ? 2.5f : camSequence == 3 ? 8.f : -6.f;
        position.x = direction.y * side + (direction.x * distance + position.x);
        position.y = direction.y * distance + position.y + direction.x * -side;
        position.z += direction.z * distance;
        if (camSequence == 3) position.z += 16.f;
        if (camSequence == 1) {
            auto* vehicle = FindPlayerVehicle(-1, false);
            if (vehicle && boatBlocked(FindPlayerVehicle(-1, false))) return false;
        }
        if (camSequence != 3) ground(position, camSequence == 1 ? 1.5f : camSequence == 2 ? .5f : 3.5f);
    } else if (camSequence == 15) {
        position += direction * 34.f; position.z = FindPlayerCoors(-1).z + .5f;
        if (*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(FindPlayerVehicle(-1, false)) + 0x590) == 5) position.z += 1.f;
    } else {
        const uintptr_t angleAddress = camSequence == 16 ? 0x8630F8 : camSequence == 17 ? 0x863228 : camSequence == 18 ? 0x863224 : 0x863220;
        direction = CineyModeRotatedDirection(direction, StaticRef<float>(angleAddress));
        position += direction * (camSequence == 16 ? 30.f : camSequence == 17 ? 25.f : camSequence == 18 ? 15.f : 12.5f);
        bool found = false; float height;
        if (camSequence == 16 || camSequence == 17) {
            position.z = FindPlayerCoors(-1).z - (camSequence == 16 ? 5.5f : 1.f);
            height = CWorld::FindRoofZFor3DCoord(position.x, position.y, position.z, &found);
            if (found == true) position.z = height + .5f; else waterFloor(position, height);
        } else {
            height = CWorld::FindGroundZFor3DCoord(position, &found, nullptr);
            // The executable tests the returned height against 1.0 here.
            if (height == 1.f) { if (position.z < height) position.z = height + .5f; }
            else waterFloor(position, height);
        }
    }
    if (!clear(position)) return false;
    if (camSequence == 3) {
        fixed(position);
        if (!finish(15)) return false;
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, .15f); return true;
    }
    if (camSequence == 5) { fixed(position); return finish(15); }
    CVector delta = FindPlayerCoors(-1) - position;
    if (camSequence == 15 || camSequence == 17) delta = FindPlayerCoors(-1) - position;
    if (camSequence == 1 || camSequence == 2) delta.z = 0.f;
    const float maximumDistance = camSequence == 1 ? 40.f : camSequence == 2 ? 29.f : camSequence == 15 ? 44.f : camSequence == 18 ? 57.f : camSequence == 19 ? 36.f : 50.f;
    const bool movingCheck = camSequence == 1 || camSequence == 2 || camSequence == 15 || camSequence == 17;
    if (delta.Magnitude() > maximumDistance) {
        if (!movingCheck) return false;
        const auto speed = FindPlayerSpeed(-1);
        if (delta.x * speed.x + speed.z * 0.f + delta.y * speed.y > 0.f) return false;
    }
    const float minimumDistance = camSequence == 1 ? 4.5f : camSequence == 2 || camSequence == 17 || camSequence == 19 ? 2.f : camSequence == 18 ? 1.f : 3.f;
    if (delta.Magnitude() < minimumDistance) return false;
    fixed(position);
    if (camSequence == 2) { TakeControl(FindPlayerEntity(-1), static_cast<eCamMode>(15), static_cast<eSwitchType>(2), 2); RwCameraSetNearClipPlane(Scene.m_pRwCamera, .15f); return !reinterpret_cast<bool(__cdecl*)()>(0x50B830)(); }
    return finish(15);
}

// 0x520190
bool CCamera::CameraColDetAndReact(CVector* source, CVector* target) {
    const CVector delta=*source-*target;
    const float length=std::sqrt((delta.z*delta.z+delta.y*delta.y)+delta.x*delta.x);
    float radius=length*gpCamColVars[0]*StaticRef<float>(0x8CCB90);
    auto* ignored=CWorld::pIgnoreEntity;
    if(gCurCamColVars>=10&&ignored){
        float limit;
        if(ignored->GetType()==ENTITY_TYPE_VEHICLE&&reinterpret_cast<CVehicle*>(ignored)->m_nVehicleSubType==VEHICLE_TYPE_AUTOMOBILE){
            const int32 model=int16(ignored->GetModelIndex());
            auto& cachedModel=StaticRef<int32>(0xB700F0);
            auto& lowest=StaticRef<float>(0xB700EC);
            if(model!=cachedModel){
                auto* data=ignored->GetColModel()->m_pColData;
                lowest=100.0f;
                if(data){for(int32 i=0;i<int16(data->m_nNumSpheres);++i){const auto& sphere=data->m_pSpheres[i];const float bottom=sphere.m_vecCenter.z-sphere.m_fRadius;if(bottom<lowest)lowest=bottom;}}
                cachedModel=model;
            }
            const auto& matrix=ignored->GetMatrix();
            const auto relative=*target-ignored->GetPosition();
            const auto& up=matrix.GetUp();
            limit=(relative.z*up.z+relative.y*up.y)+relative.x*up.x-lowest;
            if(limit<0.2f)limit=0.2f;
        }else{
            const auto& box=CModelInfo::GetModelInfo(int16(ignored->GetModelIndex()))->GetColModel()->m_boundBox;
            const float x=(box.m_vecMax.x-box.m_vecMin.x)*0.5f,y=(box.m_vecMax.y-box.m_vecMin.y)*0.5f;
            limit=(box.m_vecMax.z-box.m_vecMin.z)*0.5f;
            if(!(x<y)){if(y<limit)limit=y;}else if(x<limit)limit=x;
        }
        if(gpCamColVars[1]<limit){if(gpCamColVars[1]<radius)radius=gpCamColVars[1];}
        else if(limit<radius)radius=limit;
    }
    if(gpCamColVars[1]<radius)radius=gpCamColVars[1];
    if(radius<StaticRef<float>(0x8CCE18))radius=StaticRef<float>(0x8CCE18);
    float minDistance=gpCamColVars[2];
    if(gCurCamColVars<10)minDistance=StaticRef<float>(gCurCamColVars<4?0x8CCE10:0x8CCE14)/length;
    const bool bike=gCurCamColVars>=10&&ignored&&ignored->GetType()==ENTITY_TYPE_VEHICLE&&reinterpret_cast<CVehicle*>(ignored)->m_nVehicleType==VEHICLE_TYPE_BIKE;
    if(bike)minDistance=StaticRef<float>(0x8CCE0C);
    StaticRef<float>(0xB6EC6C)=radius;
    CVector resolved;
    float fraction;
    const bool collided=TheCamera.ConeCastCollisionResolve(*source,*target,resolved,radius,minDistance,fraction);
    if(collided&&fraction<=gpCamColVars[3])RwCameraSetNearClipPlane(Scene.m_pRwCamera,gpCamColVars[4]);
    if(!(fraction<gCurDistForCam)){
        auto& flags=StaticRef<uint32>(0xB700E8);
        auto& previous=StaticRef<CVector>(0xB700DC);
        if(!(flags&1)){flags|=1;previous=CVector{0,0,0};}
        const auto moved=*source-previous;
        const float threshold=StaticRef<float>(0x8CCE08);
        if(threshold*threshold<(moved.z*moved.z+moved.y*moved.y)+moved.x*moved.x){
            float step=(CTimer::GetTimeStep()*gpCamColVars[5])*(fraction-gCurDistForCam);
            if(StaticRef<float>(0x8CCE04)<step)step=StaticRef<float>(0x8CCE04);
            gCurDistForCam+=step;
        }
        previous=*source;
    }else gCurDistForCam=fraction;
    if(gCurDistForCam>1.0f)gCurDistForCam=1.0f;
    *source=CVector{delta.x*gCurDistForCam+target->x,delta.y*gCurDistForCam+target->y,delta.z*gCurDistForCam+target->z};
    if(bike&&gCurDistForCam<StaticRef<float>(0x8CCE00))RwCameraSetNearClipPlane(Scene.m_pRwCamera,StaticRef<float>(0x8CCDFC));
    return collided;
}

namespace {
uint16 CameraControlMode(const CCam& cam) {
    return static_cast<uint16>(cam.m_nMode);
}
bool CameraControlModeIn(uint16 mode, std::initializer_list<uint16> modes) {
    for (const auto candidate : modes) {
        if (mode == candidate) {
            return true;
        }
    }
    return false;
}
void CameraControlSetTarget(CCam& cam, CEntity* entity) {
    if (cam.m_pCamTargetEntity) {
        cam.m_pCamTargetEntity->CleanUpOldReference(&cam.m_pCamTargetEntity);
    }
    cam.m_pCamTargetEntity = entity;
    entity->RegisterReference(&cam.m_pCamTargetEntity);
}
void CameraControlCopyFixedState(CCamera& camera, bool lookingAtVector) {
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    cam.m_vecCamFixedModeVector = camera.m_vecFixedModeVector;
    CameraControlSetTarget(cam, camera.m_pTargetEntity);
    cam.m_vecCamFixedModeSource = camera.m_vecFixedModeSource;
    cam.m_vecCamFixedModeUpOffSet = camera.m_vecFixedModeUpOffSet;
    cam.m_bCamLookingAtVector = lookingAtVector;
    cam.m_vecLastAboveWaterCamPosition = camera.m_aCams[(camera.m_nActiveCam + 1) & 1].m_vecLastAboveWaterCamPosition;
    camera.m_fCarZoomSmoothed = camera.m_fCarZoomBase;
    camera.m_fPedZoomSmoothed = camera.m_fPedZoomBase;
}
// 0x52B4FB..0x52B725: references, visibility, script release and zoom sound.
void CameraControlFinish(CCamera& camera, uint16 initialMode, bool cinematicAllowed) {
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    camera.m_bStartInterScript = false;
    if (!cam.m_pCamTargetEntity) {
        cam.m_pCamTargetEntity = camera.m_pTargetEntity;
        cam.m_pCamTargetEntity->RegisterReference(&cam.m_pCamTargetEntity);
    }
    const auto mode = CameraControlMode(cam);
    if (mode == 17 || (camera.m_pTargetEntity->GetIsTypePed()
        && CameraControlModeIn(mode, {16, 7, 34, 46, 45, 8, 51}))) {
        if (FindPlayerPed()->m_bIsVisible) {
            FindPlayerPed()->m_bIsVisible = false;
            if (const auto held = FindPlayerPed()->GetIntelligence()->GetTaskHold(false);
                held && held->m_pEntityToHold) {
                held->m_pEntityToHold->m_bIsVisible = false;
            }
        }
    } else {
        FindPlayerPed()->m_bIsVisible = true;
    }
    if (CameraControlMode(camera.m_aCams[camera.m_nActiveCam]) == 15) {
        FindPlayerPed()->m_bIsVisible = gPlayerPedVisible;
    }
    bool restored = false;
    if (!cinematicAllowed && camera.m_nWhoIsInControlOfTheCamera == 2) {
        camera.RestoreWithJumpCut();
        restored = true;
        camera.m_bCamDirectlyBehind = true;
        if (const auto ped = FindPlayerPed()) {
            camera.m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(
                ped->GetMatrix().GetForward().x, ped->GetMatrix().GetForward().y
            );
        }
    }
    const auto finalMode = CameraControlMode(camera.m_aCams[camera.m_nActiveCam]);
    if (initialMode != finalMode || restored || finalMode == 4 || finalMode == 18) {
        if (CPad::GetPad(0)->sub_540530()
            && StaticRef<uint8>(0xA43088) != 1
            && (camera.m_bLookingAtPlayer || camera.m_nWhoIsInControlOfTheCamera == 2)
            && !camera.m_bWideScreenOn
            && (camera.m_nWhoIsInControlOfTheCamera != 2
                || (StaticRef<uint8>(0xB6EC34) == 1
                    && *reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x10E) == 0))) {
            AudioEngine.ReportFrontendAudioEvent(static_cast<eAudioEvents>(32), 0.0f, 1.0f);
        }
    }
}
// Shared immediate-switch operation at 0x52A34A, 0x52B398 and 0x52B3F9.
void CameraControlCommitJump(CCamera& camera, uint16 mode, bool lookingAtVector) {
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    cam.m_nMode = static_cast<eCamMode>(mode);
    camera.m_bJust_Switched = true;
    CameraControlCopyFixedState(camera, lookingAtVector);
    camera.m_bTransitionState = false;
    camera.m_bDoingSpecialInterp = false;
    camera.m_bStartInterScript = false;
    cam.m_bResetStatics = true;
}
// 0x527FA0..0x5280E4: reset transient flags and update periodic cull state.
uint16 CameraControlBegin(CCamera& camera) {
    const auto initialMode = CameraControlMode(camera.m_aCams[camera.m_nActiveCam]);
    camera.m_bObbeCinematicPedCamOn = false;
    camera.m_bObbeCinematicCarCamOn = false;
    camera.m_bUseTransitionBeta = false;
    camera.m_bUseSpecialFovTrain = false;
    camera.m_bJustCameOutOfGarage = false;
    camera.m_bTargetJustCameOffTrain = false;
    camera.m_bInATunnelAndABigVehicle = false;
    camera.m_bJustJumpedOutOf1stPersonBecauseOfTarget = false;
    StaticRef<uint8>(0xB6EC34) = 0;
    if (!camera.m_aCams[camera.m_nActiveCam].m_pCamTargetEntity && !camera.m_pTargetEntity) {
        const auto player = StaticRef<uint8>(0xB7CD74);
        camera.m_pTargetEntity = *reinterpret_cast<CPed**>(0xB7CD98 + player * 400);
        camera.m_pTargetEntity->RegisterReference(&camera.m_pTargetEntity);
    }
    // Signed comparison in EXE even though the declared members are uint32.
    const auto nextCullFrame = static_cast<int32>(camera.m_nZoneCullFrameNumWereAt) + 1;
    camera.m_nZoneCullFrameNumWereAt = nextCullFrame;
    if (static_cast<int32>(camera.m_nCheckCullZoneThisNumFrames) < nextCullFrame) {
        camera.m_nZoneCullFrameNumWereAt = 1;
    }
    camera.m_bCullZoneChecksOn = camera.m_nZoneCullFrameNumWereAt == camera.m_nCheckCullZoneThisNumFrames;
    if (camera.m_bCullZoneChecksOn) {
        camera.m_bFailedCullZoneTestPreviously = CCullZones::CamCloseInForPlayer() != 0;
    }
    if (camera.m_bLookingAtPlayer) {
        auto& padFlags = *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x10E);
        padFlags &= 0xFE;
        FindPlayerPed()->m_bIsVisible = true;
    }
    return initialMode;
}
// 0x52A002..0x52A2C7: cooperative mode and arrest/death overrides.
// Returns the previous arrest latch, which participates in jump selection below.
bool CameraControlApplyPlayerOverrides(CCamera& camera) {
    auto& requested = StaticRef<uint16>(0xB70140);
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    auto* const player0 = StaticRef<CPed*>(0xB7CD98);
    auto* const player1 = StaticRef<CPed*>(0xB7CF28);
    if (camera.m_bCooperativeCamMode && player0 && player1) {
        if (!player0->bInVehicle || !player1->bInVehicle || !player0->m_pVehicle || !player1->m_pVehicle) {
            requested = static_cast<uint16>(camera.m_nModeForTwoPlayersNotBothInCar);
        } else {
            camera.m_pTargetEntity = player0->m_pVehicle;
            if (player0->m_pVehicle == player1->m_pVehicle) {
                requested = static_cast<uint16>(camera.m_bAllowShootingWith2PlayersInCar
                    ? camera.m_nModeForTwoPlayersSameCarShootingAllowed
                    : camera.m_nModeForTwoPlayersSameCarShootingNotAllowed);
            } else {
                requested = static_cast<uint16>(camera.m_nModeForTwoPlayersSeparateCars);
            }
        }
    }
    const auto activePlayer = StaticRef<uint8>(0xB7CD74);
    const auto player = *reinterpret_cast<CPed**>(0xB7CD98 + activePlayer * 400);
    auto& arrestLatch = StaticRef<uint8>(0xB7013C);
    const bool wasArrested = player->m_nPedState != PEDSTATE_ARRESTED && arrestLatch != 0;
    if (player->m_nPedState == PEDSTATE_ARRESTED) {
        arrestLatch = 1;
    } else if (arrestLatch != 0) {
        arrestLatch = 0;
    }
    auto& previousPedState = StaticRef<int32>(0xB70138);
    const bool enteredArrested = previousPedState != PEDSTATE_ARRESTED
        && player->m_nPedState == PEDSTATE_ARRESTED
        && (camera.m_nCarZoom != 0 || !camera.m_pTargetEntity->GetIsTypeVehicle());
    previousPedState = player->m_nPedState;
    if (enteredArrested) {
        requested = 32;
        StaticRef<uint32>(0xB70134) = 32;
        cam.m_bResetStatics = true;
    } else if (player->m_nPedState == PEDSTATE_ARRESTED) {
        requested = StaticRef<uint16>(0xB70134);
    }
    if (player->m_nPedState == PEDSTATE_DEAD) {
        camera.m_bObbeCinematicCarCamOn = false;
        if (CameraControlMode(cam) != 29) {
            if (CameraControlMode(cam) == 32) {
                requested = 32;
                return wasArrested;
            }
            if (camera.m_pTargetEntity->GetIsTypePed()) {
                auto* const target = static_cast<CPed*>(camera.m_pTargetEntity);
                for (auto* const nearby : target->GetIntelligence()->m_pedScanner.m_apEntities) {
                    if (!nearby) {
                        continue;
                    }
                    const auto arrest = static_cast<CTaskSimpleArrestPed*>(
                        static_cast<CPed*>(nearby)->GetTaskManager().FindActiveTaskByType(TASK_SIMPLE_ARREST_PED)
                    );
                    if (arrest && arrest->m_Ped == FindPlayerPed()) {
                        const auto displacement = nearby->GetPosition() - target->GetPosition();
                        if (displacement.Magnitude() < 4.0f) {
                            cam.m_bResetStatics = true;
                            requested = 32;
                            return wasArrested;
                        }
                    }
                }
            }
            cam.m_bResetStatics = true;
        }
        requested = 29;
    }
    return wasArrested;
}
// 0x52A2C7..0x52A447: script-requested immediate restoration.
void CameraControlApplyRestore(CCamera& camera) {
    if (!camera.m_bRestoreByJumpCut) {
        return;
    }
    const auto requested = StaticRef<uint16>(0xB70140);
    if (!CameraControlModeIn(requested, {4, 34, 7, 8, 51, 46, 11, 28, 35, 18, 3})
        && StaticRef<uint8>(0xB6EC2E) == 0) {
        camera.SetCameraDirectlyBehindForFollowPed_CamOnAString();
    }
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    StaticRef<uint16>(0xB70140) = static_cast<uint16>(camera.m_nModeToGoTo);
    cam.m_nMode = camera.m_nModeToGoTo;
    camera.m_bJust_Switched = true;
    cam.m_bResetStatics = true;
    CameraControlCopyFixedState(camera, false);
    camera.m_bRestoreByJumpCut = false;
    cam.m_bResetStatics = true;
    camera.m_bTransitionState = false;
    camera.m_bDoingSpecialInterp = false;
}
// 0x52A447..0x52A714: cinematic admission and class-specific collaborators.
// Those five EXE collaborators have no reconstructed body in this catalogue;
// their exact thiscall entry points are deliberately external to CamControl.
bool CameraControlApplyCinematic(CCamera& camera, bool& jump) {
    auto& requested = StaticRef<uint16>(0xB70140);
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    if (gbModelViewer) {
        requested = 9;
    }
    bool allowed = true;
    if (camera.m_pTargetEntity) {
        if (camera.m_pTargetEntity->GetIsTypeVehicle()) {
            if (camera.m_nCarZoom == 5) {
                camera.m_bObbeCinematicCarCamOn = true;
            }
        } else if (camera.m_nPedZoom == 5) {
            camera.m_bObbeCinematicPedCamOn = true;
        }
    }
    if (FindPlayerVehicle(-1, false) && FindPlayerVehicle(-1, false)->m_nVehicleType == VEHICLE_TYPE_TRAIN) {
        camera.m_bObbeCinematicCarCamOn = true;
    }
    if (camera.m_pTargetEntity && camera.m_pTargetEntity->GetIsTypeVehicle()) {
        if (const auto player = FindPlayerPed(); player
            && (player->m_nPedState == PEDSTATE_ARRESTED || player->m_nPedState == PEDSTATE_DEAD)) {
            camera.m_bObbeCinematicPedCamOn = false;
            allowed = false;
            requested = player->m_nPedState == PEDSTATE_ARRESTED ? 32 : 29;
        }
    }
    if (camera.m_bTargetJustBeenOnTrain
        || CameraControlModeIn(requested, {29, 23, 28, 11, 7, 35, 8, 51, 32, 33, 34, 36, 39, 40, 52, 42, 43, 41, 45, 46})
        || camera.m_nWhoIsInControlOfTheCamera == 1
        || camera.m_bJustCameOutOfGarage || camera.m_bPlayerIsInGarage
        || CameraControlMode(cam) == 29) {
        allowed = false;
    }
    if (camera.m_bCinemaCamera) {
        camera.m_bObbeCinematicCarCamOn = true;
        allowed = true;
    }
    if (camera.m_bObbeCinematicPedCamOn && allowed) {
        return true;
    }
    if (!camera.m_bObbeCinematicCarCamOn || !allowed) {
        if (camera.m_bPlayerIsInGarage && camera.m_bObbeCinematicCarCamOn) {
            jump = true;
        }
        StaticRef<uint8>(0xB6EC2D) = 0;
        return false;
    }
    StaticRef<uint8>(0x8D5109) = 0;
    if (camera.m_pTargetEntity->GetIsTypeVehicle()) {
        const auto vehicle = static_cast<CVehicle*>(camera.m_pTargetEntity);
        if (vehicle->m_nVehicleSubType == VEHICLE_TYPE_PLANE) {
            plugin::CallMethod<0x526C80, CCamera*>(&camera);
        } else if (static_cast<int32>(vehicle->GetVehicleAppearance()) == 3) {
            plugin::CallMethod<0x526AE0, CCamera*>(&camera);
        } else if (vehicle->m_nVehicleSubType == VEHICLE_TYPE_BOAT) {
            plugin::CallMethod<0x526E20, CCamera*>(&camera);
        } else if (vehicle->m_nVehicleSubType == VEHICLE_TYPE_TRAIN) {
            plugin::CallMethod<0x526950, CCamera*>(&camera);
        } else {
            plugin::CallMethod<0x5267C0, CCamera*>(&camera);
        }
    }
    return true;
}
// The EXE steps toward the requested zoom on both sides, including equality.
// Retain the comparison order rather than using a clamp with different NaN rules.
void CameraControlStepZoom(float& value, float target) {
    const auto step = StaticRef<float>(0xB7CB5C) * StaticRef<float>(0x85A6F0);
    if (target <= value) {
        const auto next = value - step;
        value = next < target ? target : next;
    } else {
        const auto next = step + value;
        value = target < next ? target : next;
    }
}
// 0x528AC6..0x528CEB: per-appearance car zoom and cull height buffering.
void CameraControlVehicleZoom(CCamera& camera) {
    auto& requested = StaticRef<uint16>(0xB70140);
    const auto vehicle = static_cast<CVehicle*>(camera.m_pTargetEntity);
    int32 zoomIndex = 0;
    camera.GetArrPosForVehicleType(static_cast<eVehicleType>(vehicle->GetVehicleAppearance()), zoomIndex);
    const auto zoom = static_cast<int32>(camera.m_nCarZoom);
    if (zoom == 0 && !camera.m_bPlayerIsInGarage) {
        camera.m_fCarZoomBase = 0.0f;
        requested = 16;
    } else if (zoom == 1) {
        camera.m_fCarZoomBase = StaticRef<float[5]>(0x8CC3E0)[zoomIndex];
    } else if (zoom == 2) {
        camera.m_fCarZoomBase = StaticRef<float[5]>(0x8CC3F4)[zoomIndex];
    } else if (zoom == 3) {
        camera.m_fCarZoomBase = StaticRef<float[5]>(0x8CC408)[zoomIndex];
    }
    if (zoom == 4 && !camera.m_bPlayerIsInGarage) {
        camera.m_fCarZoomBase = 1.0f;
    }
    if (camera.m_fCarZoomTotal == 0.0f) {
        camera.m_fCarZoomTotal = camera.m_fCarZoomBase;
    }
    float heightOffset = 0.0f;
    if (camera.m_bUseScriptZoomValueCar) {
        CameraControlStepZoom(camera.m_fCarZoomSmoothed, camera.m_fCarZoomValueScript);
    } else if (!camera.m_bFailedCullZoneTestPreviously) {
        CameraControlStepZoom(camera.m_fCarZoomSmoothed, camera.m_fCarZoomBase);
        if (zoom == 3 && camera.m_fCarZoomBase == 0.0f) {
            camera.m_fCarZoomSmoothed = camera.m_fCarZoomBase;
        }
    } else {
        heightOffset = 0.65f;
        CameraControlStepZoom(camera.m_fCarZoomSmoothed, -0.65f);
    }
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    plugin::Call<0x509AE0, float, float*, float*, float, float, bool>(
        heightOffset, &cam.m_fCloseInCarHeightOffset, &cam.m_fCloseInCarHeightOffsetSpeed,
        0.1f, 0.25f, false
    );
}
// 0x529003..0x52915F: ped zoom and cull height buffering.
void CameraControlPedZoom(CCamera& camera) {
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    const auto zoom = static_cast<int32>(camera.m_nPedZoom);
    if (zoom == 1) {
        camera.m_fPedZoomBase = cam.m_fTargetZoomGroundOne;
    } else if (zoom == 3) {
        camera.m_fPedZoomBase = cam.m_fTargetZoomGroundThree;
    } else {
        camera.m_fPedZoomBase = cam.m_fTargetZoomGroundTwo;
    }
    float heightOffset = 0.0f;
    if (camera.m_bUseScriptZoomValuePed) {
        CameraControlStepZoom(camera.m_fPedZoomSmoothed, camera.m_fPedZoomValueScript);
    } else if (!camera.m_bFailedCullZoneTestPreviously) {
        CameraControlStepZoom(camera.m_fPedZoomSmoothed, camera.m_fPedZoomBase);
        if (zoom == 3 && camera.m_fPedZoomBase == 0.0f) {
            camera.m_fPedZoomSmoothed = camera.m_fPedZoomBase;
        }
    } else {
        heightOffset = 0.7f;
        CameraControlStepZoom(camera.m_fPedZoomSmoothed, StaticRef<float>(0x8CCF14));
    }
    plugin::Call<0x509AE0, float, float*, float*, float, float, bool>(
        heightOffset, &cam.m_fCloseInPedHeightOffset, &cam.m_fCloseInPedHeightOffsetSpeed,
        0.1f, 0.025f, false
    );
}
// 0x52B252..0x52B4FB: script control and temporarily permitted first person.
void CameraControlScriptTransition(CCamera& camera) {
    auto& requested = StaticRef<uint16>(0xB70140);
    bool changeNow = false;
    bool useWeaponMode = false;
    if (camera.m_bEnable1rstPersonCamCntrlsScript || camera.m_bAllow1rstPersonWeaponsCamera) {
        if (requested == 16) {
            changeNow = CameraControlMode(camera.m_aCams[camera.m_nActiveCam]) != 16;
        } else {
            if (CameraControlModeIn(camera.m_PlayerWeaponMode.m_nMode, {7, 16, 8, 51})
                && CPad::GetPad(0)->GetTarget() && camera.m_bAllow1rstPersonWeaponsCamera) {
                changeNow = true;
                useWeaponMode = true;
            } else if (CameraControlMode(camera.m_aCams[camera.m_nActiveCam])
                != static_cast<uint16>(camera.m_nModeToGoTo)) {
                camera.m_bStartInterScript = true;
                camera.m_nTypeOfSwitch = eSwitchType::JUMPCUT;
                auto& padFlags = *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x10E);
                padFlags &= 0xFE;
            }
        }
    }
    if (camera.m_bStartInterScript && camera.m_nTypeOfSwitch == eSwitchType::INTERPOLATION) {
        requested = static_cast<uint16>(camera.m_nModeToGoTo);
        if (camera.m_bTransitionState) {
            camera.m_bDoingSpecialInterp = true;
        }
        camera.StartTransition(camera.m_nModeToGoTo);
        return;
    }
    if ((!camera.m_bStartInterScript || camera.m_nTypeOfSwitch != eSwitchType::JUMPCUT) && !changeNow) {
        return;
    }
    camera.m_bTransitionState = false;
    camera.m_bDoingSpecialInterp = false;
    const auto mode = camera.m_bEnable1rstPersonCamCntrlsScript && requested == 16
        ? uint16{16} : useWeaponMode ? camera.m_PlayerWeaponMode.m_nMode
        : static_cast<uint16>(camera.m_nModeToGoTo);
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    cam.m_nMode = static_cast<eCamMode>(mode);
    camera.m_bJust_Switched = true;
    cam.m_bResetStatics = true;
    CameraControlCopyFixedState(camera, camera.m_bLookingAtVector);
    camera.m_bJust_Switched = true;
}
// 0x52ADD6..0x52B24D: choose interpolation, continuation, or an immediate switch.
void CameraControlPlayerTransition(CCamera& camera, bool jump) {
    auto& requested = StaticRef<uint16>(0xB70140);
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    const auto oldMode = CameraControlMode(cam);
    if (requested != oldMode && !cam.m_pCamTargetEntity) {
        jump = true;
    }
    if (camera.m_bPlayerIsInGarage) {
        if (camera.m_pToGarageWeAreIn) {
            const auto garageType = *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(camera.m_pToGarageWeAreIn) + 0x4C);
            if ((garageType == 2 || garageType == 3 || garageType == 4)
                && camera.m_pTargetEntity->GetIsTypeVehicle()
                && camera.m_pTargetEntity->m_nModelIndex == 423 && requested != oldMode) {
                jump = true;
            }
        }
        if (cam.m_pCamTargetEntity) {
            const auto& target = cam.m_pCamTargetEntity->GetPosition();
            const auto& source = cam.m_vecSource;
            const auto& fixed = camera.m_vecFixedModeSource;
            // Keep native y,z,x accumulation order.
            const auto planeSide = (target.y - source.y) * (target.y - fixed.y)
                + (target.z - source.z) * (target.z - fixed.z)
                + (target.x - source.x) * (target.x - fixed.x);
            if (planeSide < 0.0f) {
                jump = true;
            }
        }
    }
    if (requested != oldMode && !jump && !camera.m_bTransitionState) {
        if (!camera.m_bWaitForInterpolToFinish) {
            camera.StartTransition(static_cast<eCamMode>(requested));
        }
        return;
    }
    if (requested == oldMode || !jump) {
        if (!camera.m_bTransitionState || requested == oldMode) {
            if (requested == 15 && camera.m_pTargetEntity != cam.m_pCamTargetEntity
                && camera.m_bPlayerIsInGarage) {
                if (camera.m_bTransitionState) {
                    camera.m_bDoingSpecialInterp = true;
                }
                camera.StartTransition(static_cast<eCamMode>(15));
            }
        } else {
            if (!camera.m_bWaitForInterpolToFinish && camera.m_bLookingAtPlayer) {
                // These three player lookups are separate in the original.
                const auto x = FindPlayerPed()->GetPosition().x - camera.m_mCameraMatrix.GetPosition().x;
                const auto y = FindPlayerPed()->GetPosition().y - camera.m_mCameraMatrix.GetPosition().y;
                const auto z = FindPlayerPed()->GetPosition().z - camera.m_mCameraMatrix.GetPosition().z;
                if (camera.m_pTargetEntity && camera.m_pTargetEntity->GetIsTypePed()
                    && CVector{x, y, z}.Magnitude() > 17.5f && (requested == 11 || requested == 28)) {
                    camera.m_bWaitForInterpolToFinish = true;
                }
            }
            if (!camera.m_bWaitForInterpolToFinish) {
                camera.m_bDoingSpecialInterp = true;
                camera.StartTransition(static_cast<eCamMode>(requested));
            }
        }
        return;
    }
    if ((!camera.m_bPlayerIsInGarage || camera.m_bJustCameOutOfGarage)
        && !CameraControlModeIn(requested, {4, 34, 7, 8, 51, 46, 11, 16, 28, 35})
        && StaticRef<uint8>(0xB6EC2E) == 0) {
        camera.SetCameraDirectlyBehindForFollowPed_CamOnAString();
    }
    CameraControlCommitJump(camera, requested, camera.m_bLookingAtVector);
}
void CameraControlPolarSource(float angle, float radius, const CVector& position, CVector& source) {
    const float positionX = position.x;
    const float positionY = position.y;
    float resultX;
    float resultY;
    __asm {
        fld angle
        fcos
        fmul radius
        fadd positionX
        fstp resultX
        fld angle
        fsin
        fmul radius
        fadd positionY
        fstp resultY
    }
    source = CVector{resultX, resultY, position.z + 1.15f};
}
// 0x529BA3..0x52A002: queued player/weapon modes and fixed dead-target aiming.
void CameraControlWeaponMode(CCamera& camera, bool stairs) {
    auto& requested = StaticRef<uint16>(0xB70140);
    const auto weaponMode = camera.m_PlayerWeaponMode.m_nMode;
    if (!CPad::GetPad(0)->GetTarget() && weaponMode != 0
        && !CameraControlModeIn(weaponMode, {45, 55, 65})
        && (weaponMode != 46 || !FindPlayerPed()->m_pAttachedTo)) {
        camera.ClearPlayerWeaponMode();
    }
    if (camera.m_PlayerMode.m_nMode != 0) {
        requested = camera.m_PlayerMode.m_nMode;
    }
    const auto preferredMode = camera.m_PlayerWeaponMode.m_nMode;
    const auto defaultMode = requested;
    if (preferredMode == 0 || stairs) {
        return;
    }
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    if (!CameraControlModeIn(preferredMode, {7, 8, 51, 34, 45, 46}) && !cam.GetWeaponFirstPersonOn()) {
        if (defaultMode == 37) {
            return;
        }
        const auto position = camera.m_pTargetEntity->GetPosition();
        const auto aimDisplacement = camera.m_vecAimingTargetCoors - position;
        const auto sourceDisplacement = cam.m_vecSource - position;
        if (!FindPlayerPed()->m_pTargetedObject && !FindPlayerPed()->GetPlayerData()->m_bFreeAiming) {
            return;
        }
        float deadDistance = 0.0f;
        bool targetDead = false;
        if (FindPlayerPed()->m_pTargetedObject
            && FindPlayerPed()->m_pTargetedObject->GetIsTypePed()
            && (static_cast<CPed*>(FindPlayerPed()->m_pTargetedObject)->m_nPedState == PEDSTATE_DEAD
                || static_cast<CPed*>(FindPlayerPed()->m_pTargetedObject)->m_nPedState == PEDSTATE_DIE)) {
            targetDead = true;
            deadDistance = StaticRef<float>(0x8CCF08);
        }
        const auto distance = std::sqrt(aimDisplacement.x * aimDisplacement.x + aimDisplacement.y * aimDisplacement.y);
        const auto azimuth = CGeneral::GetATanOfXY(sourceDisplacement.x, sourceDisplacement.y);
        // Deliberately evaluated, although the original discards the second angle.
        CGeneral::GetATanOfXY(aimDisplacement.x, aimDisplacement.y);
        requested = camera.m_PlayerWeaponMode.m_nMode;
        float fixedDistance = 0.0f;
        if (requested == 53 && targetDead && FindPlayerPed()->m_pTargetedObject
            && (!camera.m_bTransitionState || CameraControlMode(camera.m_aCams[camera.m_nActiveCam]) == 35)) {
            auto threshold = deadDistance;
            if (CameraControlMode(camera.m_aCams[camera.m_nActiveCam]) == 35
                && FindPlayerPed()->m_pTargetedObject->GetIsTypePed()) {
                threshold = StaticRef<float>(0x8CCF04);
            }
            if (distance < threshold) {
                fixedDistance = 5.6f;
                requested = 35;
            }
        }
        if (requested != 35) {
            StaticRef<uint8>(0xB7013D) = 0;
            return;
        }
        if (StaticRef<uint8>(0xB7013D) == 0) {
            CVector source;
            CameraControlPolarSource(azimuth, fixedDistance, camera.m_pTargetEntity->GetPosition(), source);
            CColPoint collision;
            CEntity* hitEntity = nullptr;
            if (CWorld::ProcessLineOfSight(camera.m_pTargetEntity->GetPosition(), source, collision, hitEntity,
                true, false, false, true, false, true, true, false)) {
                camera.SetCamPositionForFixedMode(collision.m_vecPoint, CVector{0.0f, 0.0f, 0.0f});
            } else {
                camera.SetCamPositionForFixedMode(source, CVector{0.0f, 0.0f, 0.0f});
            }
            StaticRef<uint8>(0xB7013D) = 1;
        }
        return;
    }
    requested = preferredMode;
    const auto playerIndex = StaticRef<uint8>(0xB7CD74);
    const auto player = *reinterpret_cast<CPed**>(0xB7CD98 + playerIndex * 400);
    if (player->m_nPedState == PEDSTATE_SEEK_CAR && defaultMode != 37
        && !camera.m_aCams[camera.m_nActiveCam].GetWeaponFirstPersonOn()) {
        requested = 4;
    }
}
bool CameraControlCycleUp(CPad* pad) {
    return pad->GetCameraCycleUp();
}
void CameraControlVehicleCycle(CCamera& camera) {
    auto& requested = StaticRef<uint16>(0xB70140);
    const bool cycle = CameraControlCycleUp(CPad::GetPad(0)) || CPad::GetPad(0)->sub_5404F0();
    if (cycle && StaticRef<uint8>(0xA43088) != 1 && !camera.m_bWideScreenOn) {
        if (!camera.m_bFailedCullZoneTestPreviously
            && (camera.m_bLookingAtPlayer || camera.m_nWhoIsInControlOfTheCamera == 2)
            && !CGameLogic::IsCoopGameGoingOn()) {
            auto zoom = static_cast<int32>(camera.m_nCarZoom)
                + (CameraControlCycleUp(CPad::GetPad(0)) ? -1 : 1);
            if (zoom >= 6) {
                zoom = 0;
            } else if (zoom < 0) {
                zoom = 5;
            }
            camera.m_nCarZoom = zoom;
            if (zoom == 4) {
                camera.m_nCarZoom = CameraControlCycleUp(CPad::GetPad(0)) ? 3 : 5;
            } else if (zoom == 0 && camera.m_bDisableFirstPersonInCar) {
                camera.m_nCarZoom = CameraControlCycleUp(CPad::GetPad(0)) ? 5 : 1;
            }
        }
    }
    if (camera.m_bFailedCullZoneTestPreviously && camera.m_nCarZoom != 4 && camera.m_nCarZoom != 0) {
        requested = 18;
    }
}
// 0x528CEB..0x529003: ped cycling and first-person maintenance.
void CameraControlPedCycle(CCamera& camera) {
    const bool cycle = CameraControlCycleUp(CPad::GetPad(0)) || CPad::GetPad(0)->sub_5404F0();
    if (cycle && StaticRef<uint8>(0xA43088) != 1 && !camera.m_bWideScreenOn
        && !camera.m_bFailedCullZoneTestPreviously && !camera.m_bFirstPersonBeingUsed
        && (camera.m_bLookingAtPlayer || camera.m_nWhoIsInControlOfTheCamera == 2)
        && !CGameLogic::IsCoopGameGoingOn()) {
        auto zoom = static_cast<int32>(camera.m_nPedZoom)
            + (CameraControlCycleUp(CPad::GetPad(0)) ? -1 : 1);
        if (zoom >= 4) {
            zoom = 1;
        } else if (zoom < 1) {
            zoom = 3;
        }
        camera.m_nPedZoom = zoom;
    }
    auto& requested = StaticRef<uint16>(0xB70140);
    requested = 4;
    bool maintainFirstPerson = false;
    if ((camera.m_bLookingAtPlayer || camera.m_bEnable1rstPersonCamCntrlsScript)
        && camera.m_pTargetEntity->GetIsTypePed()
        && (!camera.m_bWideScreenOn || camera.m_bEnable1rstPersonCamCntrlsScript)
        && !camera.m_aCams[camera.m_nActiveCam].Using3rdPersonMouseCam()) {
        if (!FindPlayerPed()->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            if (CPad::GetPad(0)->GetMouseCameraStickHorizontal() == 0) {
                CPad::GetPad(0)->GetMouseCameraStickVertical();
            }
        }
        if (camera.m_bFirstPersonBeingUsed) {
            if (CPad::GetPad(0)->GetPedWalkLeftRight() == 0
                && CPad::GetPad(0)->GetPedWalkUpDown() == 0
                && *reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x1C) == 0
                && *reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x1E) == 0
                && *reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x20) == 0
                && *reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x22) == 0
                && *reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x1A) == 0
                && static_cast<float>(CTimer::GetTimeInMS() - camera.m_nFirstPersonCamLastInputTime) <= 2850.0f) {
                if (!CPad::GetPad(0)->GetEnterTargeting()) {
                    maintainFirstPerson = true;
                } else {
                    camera.m_bJustJumpedOutOf1stPersonBecauseOfTarget = true;
                }
            }
            camera.m_bFirstPersonBeingUsed = maintainFirstPerson;
        }
    } else {
        camera.m_bFirstPersonBeingUsed = false;
    }
    if (!FindPlayerPed()->IsPedInControl() || FindPlayerPed()->GetPlayerData()->m_fMoveBlendRatio > 0.0f) {
        camera.m_bFirstPersonBeingUsed = false;
    }
    if (camera.m_bFirstPersonBeingUsed) {
        requested = 16;
        auto& padFlags = *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x10E);
        padFlags |= 1;
    }
}
float CameraControlWrapHeading(float heading, float reference) {
    if (heading > reference + StaticRef<float>(0x858CB8)) {
        heading -= StaticRef<float>(0x858CBC);
    } else if (heading < reference - StaticRef<float>(0x858CB8)) {
        heading += StaticRef<float>(0x858CBC);
    }
    return heading;
}
float CameraControlTargetHeading(CPed* ped) {
    if (!ped->m_pTargetedObject) {
        return ped->GetHeading();
    }
    const auto displacement = ped->m_pTargetedObject->GetPosition() - ped->GetPosition();
    const float componentX = displacement.x;
    const float componentY = displacement.y;
    float heading;
    __asm {
        fld componentX
        fchs
        fld componentY
        fpatan
        fstp heading
    }
    return heading;
}
// 0x52A714..0x52ADD6: native mode-pair jump rules and transition beta.
void CameraControlChooseJump(CCamera& camera, bool stairs, bool wasArrested, bool& jump) {
    auto& requested = StaticRef<uint16>(0xB70140);
    auto& cam = camera.m_aCams[camera.m_nActiveCam];
    const auto oldMode = CameraControlMode(cam);
    if (CameraControlModeIn(requested, {1, 16, 37})
        || (CameraControlModeIn(requested, {18, 22}) && CameraControlModeIn(oldMode, {1, 16, 37}))
        || (requested == 15 && oldMode == 1)) {
        jump = true;
    }
    if (CameraControlModeIn(requested, {53, 55, 65}) && camera.m_pTargetEntity
        && camera.m_pTargetEntity->GetIsTypePed()) {
        const auto ped = static_cast<CPed*>(camera.m_pTargetEntity);
        const bool jetPack = ped->GetIntelligence()->GetTaskJetPack() != nullptr;
        if (requested == 53 && oldMode == 4 && !jetPack) {
            const auto heading = CameraControlWrapHeading(
                CameraControlTargetHeading(ped) - StaticRef<float>(0x858FE4), cam.m_fHorizontalAngle
            );
            if (StaticRef<float>(0x8CC46C) * StaticRef<float>(0x8595EC)
                < std::fabs(heading - cam.m_fHorizontalAngle)) {
                jump = true;
            } else {
                const auto displacement = ped->GetPosition() - camera.m_mCameraMatrix.GetPosition();
                if ((StaticRef<float>(0xB6F0FC) + 2.0f) * 1.5f < displacement.Magnitude()) {
                    jump = true;
                }
            }
            if (StaticRef<uint8>(0xB6EC2E) != 0) {
                jump = false;
            }
        } else {
            jump = true;
        }
    }
    if ((requested == 48 && oldMode != 49) || (requested == 49 && oldMode != 48)
        || (oldMode == 48 && requested != 49) || (oldMode == 49 && requested != 48)) {
        jump = true;
    }
    if ((requested == 1 && (oldMode == 37 || oldMode == 29))
        || (requested == 37 && (oldMode == 1 || oldMode == 29))) {
        jump = oldMode == 29;
        return;
    }
    if (CameraControlModeIn(requested, {16, 7, 34, 8, 51, 39, 40, 52, 42, 43, 41, 45, 32, 33, 46})
        && camera.m_pTargetEntity->GetIsTypePed()) {
        jump = true;
        return;
    }
    if (requested == 15) {
        if (!camera.m_bPlayerIsInGarage) {
            if (oldMode == 29) {
                jump = true;
            }
        } else if (CameraControlModeIn(oldMode, {7, 45, 8, 51, 34, 37, 16, 39, 40, 52, 42, 43, 41, 46}) || stairs) {
            if (camera.m_pTargetEntity && camera.m_pTargetEntity->GetIsTypeVehicle()) {
                jump = true;
            }
        }
        return;
    }
    if (requested != 4) {
        if (CameraControlModeIn(requested, {38, 32, 33, 29}) || oldMode == 29) {
            jump = true;
        }
        return;
    }
    bool aimingJump = false;
    if (oldMode == 53) {
        const auto ped = static_cast<CPed*>(camera.m_pTargetEntity);
        if (ped->CanWeRunAndFireWithWeapon()
            && (*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x46C) & 0x4000000) == 0) {
            const auto heading = CameraControlWrapHeading(
                ped->GetHeading() - StaticRef<float>(0x858FE4), cam.m_fHorizontalAngle
            );
            aimingJump = StaticRef<float>(0x8CC46C) * StaticRef<float>(0x8595EC)
                < std::fabs(heading - cam.m_fHorizontalAngle) || !ped->bIsStanding;
            if (StaticRef<uint8>(0xB6EC2E) != 0) {
                aimingJump = false;
                camera.m_bJustCameOutOfGarage = true;
            }
        }
    }
    const bool leavingSpecialMode = CameraControlModeIn(oldMode,
        {16, 7, 34, 8, 51, 29, 32, 33, 30, 39, 40, 52, 42, 43, 41, 45, 1, 37, 46});
    if ((leavingSpecialMode || aimingJump || wasArrested) && !camera.m_bJustCameOutOfGarage) {
        if (CameraControlModeIn(oldMode, {7, 8, 51, 34, 16, 39, 40, 52, 42, 43, 41, 45, 46})) {
            const auto angle = CGeneral::GetATanOfXY(cam.m_vecFront.x, cam.m_vecFront.y) - StaticRef<float>(0x858FE4);
            const auto ped = static_cast<CPed*>(camera.m_pTargetEntity);
            ped->m_fCurrentRotation = angle;
            ped->m_fAimingRotation = angle;
        }
        camera.m_bUseTransitionBeta = true;
        jump = true;
        if (oldMode == 37) {
            auto displacement = camera.m_aCams[camera.m_nActiveCam].m_vecSource - FindPlayerPed()->GetPosition();
            displacement.z = 0.0f;
            displacement.Normalise();
            cam.m_fTransitionBeta = CGeneral::GetATanOfXY(0.001f, 1.0f);
        } else {
            cam.m_fTransitionBeta = CGeneral::GetATanOfXY(cam.m_vecFront.x, cam.m_vecFront.y) + StaticRef<float>(0x858CB8);
        }
    }
}
void CameraControlGarageDoors(CGarage* garage, CObject*& first, CObject*& second) {
    plugin::CallMethod<0x449FF0, CGarage*, CObject**, CObject**>(garage, &first, &second);
}
float CameraControlZoneExtent(const CAttributeZone* zone) {
    const auto xExtent = std::abs(static_cast<int32>(zone->zoneDef.m_vec1X))
        + std::abs(static_cast<int32>(zone->zoneDef.m_vec2X));
    const auto yExtent = std::abs(static_cast<int32>(zone->zoneDef.m_vec1Y))
        + std::abs(static_cast<int32>(zone->zoneDef.m_vec2Y));
    return static_cast<float>(xExtent > yExtent ? xExtent : yExtent);
}
CVector CameraControlZoneCameraPosition(CAttributeZone* zone) {
    CVector result;
    const auto position = plugin::CallMethodAndReturn<CVector*, 0x72D6D0, CAttributeZone*, CVector*>(zone, &result);
    return *position;
}
// Chooses the clear side of a staircase zone; the vehicle path uses the
// target itself as the displacement origin, as the original ASM does.
void CameraControlZoneSide(CCamera& camera, CAttributeZone* zone, bool vehicle,
    CVector& source, CVector& center) {
    center = vehicle ? camera.m_pTargetEntity->GetPosition() : CameraControlZoneCameraPosition(zone);
    const auto target = camera.m_pTargetEntity->GetPosition();
    auto direction = target - center;
    direction.z = 0.0f;
    direction.Normalise();
    const auto extent = CameraControlZoneExtent(zone);
    const auto range = extent + extent;
    auto candidate = camera.m_pTargetEntity->GetPosition() + direction * range;
    if (!CWorld::GetIsLineOfSightClear(camera.m_pTargetEntity->GetPosition(), candidate,
        true, false, false, false, false, false, true)) {
        candidate = camera.m_pTargetEntity->GetPosition() - direction * range;
        if (!CWorld::GetIsLineOfSightClear(camera.m_pTargetEntity->GetPosition(), candidate,
            true, false, false, false, false, false, true)) {
            return;
        }
    }
    source = candidate;
}
void CameraControlGarageDoorSource(CCamera& camera, CObject* first, CObject* second,
    const CVector& center, const CVector& target, bool vehicle) {
    CVector firstPosition{0.0f, 0.0f, 0.0f};
    CVector secondPosition{0.0f, 0.0f, 0.0f};
    CVector direction;
    if (first) {
        firstPosition = first->m_pDummyObject->GetPosition();
        direction = first->m_pDummyObject->GetMatrix().GetRight();
    }
    if (second) {
        secondPosition = second->m_pDummyObject->GetPosition();
        direction = second->m_pDummyObject->GetMatrix().GetRight();
    }
    CVector base;
    if (!first && !second) {
        base = target;
        direction = target - center;
        direction.z = 0.0f;
        direction.Normalise();
    } else if (!first) {
        base = secondPosition;
    } else if (!second) {
        base = firstPosition;
    } else {
        base = CVector{
            (secondPosition.x - firstPosition.x) * 0.5f + firstPosition.x,
            (secondPosition.y - firstPosition.y) * 0.5f + firstPosition.y,
            (secondPosition.z - firstPosition.z) * 0.5f + firstPosition.z
        };
    }
    auto source = base + direction * StaticRef<float>(vehicle ? 0x8CCF1C : 0x8CCF10);
    source.z += StaticRef<float>(vehicle ? 0x8CCF18 : 0x8CCF0C);
    camera.SetCamPositionForFixedMode(source, CVector{0.0f, 0.0f, 0.0f});
    camera.m_bGarageFixedCamPositionSet = true;
}
void CameraControlLeaveGarage(CCamera& camera) {
    if (camera.m_bPlayerIsInGarage) {
        camera.m_bJustCameOutOfGarage = true;
        camera.m_bPlayerIsInGarage = false;
    }
}
// 0x52829E..0x528AC6: vehicle garage and stairs selection.
void CameraControlVehicleGarage(CCamera& camera, bool& stairs) {
    auto& requested = StaticRef<uint16>(0xB70140);
    const auto vehicle = static_cast<CVehicle*>(camera.m_pTargetEntity);
    auto vehicleType = vehicle->m_nVehicleType;
    if (vehicleType == VEHICLE_TYPE_BOAT && vehicle->m_nModelIndex != 460) {
        requested = 22;
        return;
    }
    if (vehicleType == VEHICLE_TYPE_BOAT) {
        vehicleType = VEHICLE_TYPE_AUTOMOBILE;
    }
    if (vehicleType != VEHICLE_TYPE_AUTOMOBILE && vehicleType != VEHICLE_TYPE_BIKE) {
        return;
    }
    CAttributeZone* zone = nullptr;
    if (vehicleType == VEHICLE_TYPE_BIKE && CCullZones::CamStairsForPlayer()) {
        zone = CCullZones::FindZoneWithStairsAttributeForPlayer();
        if (zone) {
            stairs = true;
        }
    }
    if (CGarages::IsPointInAGarageCameraZone(camera.m_pTargetEntity->GetPosition()) || stairs) {
        if (((!camera.m_bGarageFixedCamPositionSet && camera.m_bLookingAtPlayer)
                || camera.m_nWhoIsInControlOfTheCamera == 2)
            && (camera.m_pToGarageWeAreIn || zone)) {
            const auto target = camera.m_pTargetEntity->GetPosition();
            CVector initialSource;
            CVector center;
            CObject* first = nullptr;
            CObject* second = nullptr;
            if (!camera.m_pToGarageWeAreIn) {
                initialSource = camera.m_aCams[camera.m_nActiveCam].m_vecSource;
                center = target;
                const auto deltaX = target.x - initialSource.x;
                const auto deltaY = target.y - initialSource.y;
                if (std::sqrt(deltaX * deltaX + deltaY * deltaY) > 15.0f) {
                    CameraControlZoneSide(camera, zone, true, initialSource, center);
                }
            } else {
                CameraControlGarageDoors(camera.m_pToGarageWeAreIn, first, second);
                initialSource = first ? first->GetPosition() : target;
                center = CVector{
                    (camera.m_pToGarageWeAreIn->m_fRightCoord + camera.m_pToGarageWeAreIn->m_fLeftCoord) * 0.5f,
                    (camera.m_pToGarageWeAreIn->m_fBackCoord + camera.m_pToGarageWeAreIn->m_fFrontCoord) * 0.5f,
                    0.0f
                };
            }
            initialSource.z = 0.0f;
            if (!camera.m_pToGarageWeAreIn && !zone) {
                center = CVector{target.x, target.y, 0.0f};
            }
            auto direction = initialSource - center;
            const auto currentPosition = camera.m_pTargetEntity->GetPosition();
            bool foundGround;
            CWorld::FindGroundZFor3DCoord(currentPosition, &foundGround, nullptr);
            direction.z = 0.0f;
            direction.Normalise();
            CameraControlGarageDoorSource(camera, first, second, center, currentPosition, true);
        }
        if ((CGarages::CameraShouldBeOutside() || stairs) && camera.m_bGarageFixedCamPositionSet
            && (camera.m_bLookingAtPlayer || camera.m_nWhoIsInControlOfTheCamera == 2)) {
            if (camera.m_pToGarageWeAreIn || zone) {
                requested = 15;
                camera.m_bPlayerIsInGarage = true;
            }
            return;
        }
        CameraControlLeaveGarage(camera);
    } else {
        CameraControlLeaveGarage(camera);
        camera.m_bGarageFixedCamPositionSet = false;
    }
    requested = 18;
}
// 0x52915F..0x529B9E: pedestrian garage/stairs camera geometry.
void CameraControlPedGarage(CCamera& camera, bool& stairs) {
    auto& requested = StaticRef<uint16>(0xB70140);
    CAttributeZone* zone = nullptr;
    if (CCullZones::CamStairsForPlayer()) {
        zone = CCullZones::FindZoneWithStairsAttributeForPlayer();
        if (zone) {
            stairs = true;
        }
    }
    if (!(CGarages::IsPointInAGarageCameraZone(camera.m_pTargetEntity->GetPosition()) || stairs)) {
        CameraControlLeaveGarage(camera);
        camera.m_bGarageFixedCamPositionSet = false;
        return;
    }
    if (!camera.m_bGarageFixedCamPositionSet && camera.m_bLookingAtPlayer) {
        if (camera.m_pToGarageWeAreIn || stairs) {
            CObject* first = nullptr;
            CObject* second = nullptr;
            auto source = camera.m_aCams[camera.m_nActiveCam].m_vecSource;
            CVector center;
            const auto target = camera.m_pTargetEntity->GetPosition();
            if (!camera.m_pToGarageWeAreIn) {
                CameraControlZoneSide(camera, zone, false, source, center);
                source.z = 0.0f;
            } else {
                CameraControlGarageDoors(camera.m_pToGarageWeAreIn, first, second);
                if (first) {
                    source = first->GetPosition();
                } else if (!second) {
                    source = target;
                }
                source.z = 0.0f;
                center = CVector{
                    (camera.m_pToGarageWeAreIn->m_fRightCoord + camera.m_pToGarageWeAreIn->m_fLeftCoord) * 0.5f,
                    (camera.m_pToGarageWeAreIn->m_fBackCoord + camera.m_pToGarageWeAreIn->m_fFrontCoord) * 0.5f,
                    0.0f
                };
            }
            const auto currentPosition = camera.m_pTargetEntity->GetPosition();
            bool foundGround;
            auto ground = CWorld::FindGroundZFor3DCoord(currentPosition, &foundGround, nullptr);
            if (!foundGround) {
                ground = currentPosition.z - 0.2f;
            }
            auto direction = source - center;
            direction.z = 0.0f;
            direction.Normalise();
            if (!camera.m_pToGarageWeAreIn && stairs) {
                source = center + direction * (zone ? CameraControlZoneExtent(zone) * 0.7f + 3.75f : 3.75f);
            } else if (!first && second) {
                source = direction * 13.0f + source;
            } else {
                source += direction * 13.0f;
            }
            if (camera.m_nPedZoom != 4 || stairs) {
                source.z = ground + 3.1f;
            } else {
                source = center;
                source.z = center.z + FindPlayerPed()->GetPosition().z + 2.1f;
                if (camera.m_pToGarageWeAreIn && camera.m_pToGarageWeAreIn->m_fRightCoord < source.z) {
                    source.z = camera.m_pToGarageWeAreIn->m_fRightCoord;
                }
            }
            camera.SetCamPositionForFixedMode(source, CVector{0.0f, 0.0f, 0.0f});
            camera.m_bGarageFixedCamPositionSet = true;
            if (camera.m_pToGarageWeAreIn) {
                CameraControlGarageDoors(camera.m_pToGarageWeAreIn, first, second);
                CameraControlGarageDoorSource(camera, first, second, center, currentPosition, false);
            }
        }
    }
    if ((!CGarages::CameraShouldBeOutside() && !stairs) || !camera.m_bLookingAtPlayer
        || !camera.m_bGarageFixedCamPositionSet) {
        CameraControlLeaveGarage(camera);
        requested = 4;
    } else if (camera.m_pToGarageWeAreIn || stairs) {
        requested = 15;
        camera.m_bPlayerIsInGarage = true;
    }
}
} // namespace

// 0x527FA0: selector de modo, geometría de garajes y política de transición.
void CCamera::CamControl() {
    const auto initialMode = CameraControlBegin(*this);
    bool jump = false;
    bool stairs = false;
    auto& requested = StaticRef<uint16>(0xB70140);
    if (StaticRef<uint8>(0xB7CB49) == 0 && StaticRef<uint8>(0xB7CB48) == 0 && !m_bIdleOn) {
        if (m_bTargetJustBeenOnTrain
            && (!m_pTargetEntity->GetIsTypeVehicle()
                || static_cast<CVehicle*>(m_pTargetEntity)->m_nVehicleType != VEHICLE_TYPE_TRAIN)) {
            Restore();
            m_bTargetJustCameOffTrain = true;
            m_bTargetJustBeenOnTrain = false;
            m_bWantsToSwitchWidescreenOff = m_bWideScreenOn;
        }
        if (m_pTargetEntity->GetIsTypeVehicle()) {
            if (StaticRef<int32>(0x8CC824) > 0) {
                requested = static_cast<uint16>(StaticRef<int32>(0x8CC824));
                StaticRef<int32>(0x8CC824) = -1;
            }
            if (static_cast<CVehicle*>(m_pTargetEntity)->m_nVehicleType == VEHICLE_TYPE_TRAIN) {
                requested = 3;
            } else {
                CameraControlVehicleCycle(*this);
                CameraControlVehicleGarage(*this, stairs);
                CameraControlVehicleZoom(*this);
            }
        } else if (m_pTargetEntity->GetIsTypePed()) {
            CameraControlPedCycle(*this);
            CameraControlPedZoom(*this);
            CameraControlPedGarage(*this, stairs);
            CameraControlWeaponMode(*this, stairs);
        }
    }
    const auto wasArrested = CameraControlApplyPlayerOverrides(*this);
    CameraControlApplyRestore(*this);
    const auto cinematic = CameraControlApplyCinematic(*this, jump);
    if (m_bLookingAtPlayer) {
        CameraControlChooseJump(*this, stairs, wasArrested, jump);
        CameraControlPlayerTransition(*this, jump);
    } else {
        CameraControlScriptTransition(*this);
    }
    CameraControlFinish(*this, initialMode, cinematic);
}


// 0x5B24A0
void CCamera::DeleteCutSceneCamDataMemory() {
    // Como el original (0x5B24A0): operator delete (0x8214BD) solo si el puntero no es nulo
    for (auto& splines : m_aPathArray) {
        if (splines.m_pArrPathData) {
            ::operator delete(splines.m_pArrPathData);
            splines.m_pArrPathData = nullptr;
        }
    }
}

// 0x5B24D0
void CCamera::LoadPathSplines(FILE* file) {
    DeleteCutSceneCamDataMemory();
    int32 block = -1, remaining = 0;
    bool expectCount = true;
    float* data = nullptr;
    while (auto* line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || *line == '\0') continue;
        if (remaining == 0) {
            if (expectCount) {
                if (++block > 3) return;
                std::sscanf(line, "%d", &remaining);
                const uint32 bytes = uint32(remaining) * (block < 2 ? 16u : 40u) + 4u;
                data = static_cast<float*>(::operator new(bytes));
                m_aPathArray.data()[block].m_pArrPathData = data;
                *data++ = float(remaining);
                expectCount = false;
            } else if (*line == ';') {
                expectCount = true;
            }
        } else {
            --remaining;
            for (auto* token = std::strtok(line, ", \t"); token; token = std::strtok(nullptr, ", \t")) {
                *data++ = float(std::atof(token));
            }
        }
    }
}

// 0x50AB50
void CCamera::GetScreenRect(CRect* rect) const {
    // Como el original (0x50AB50..0x50ABEE): enteros con fild, % * 0.01f y 1/448 del EXE (no `/ 100.f` ni SCREEN_SCALE_Y)
    rect->left  = 0.0f;
    rect->right = float(RsGlobal.maximumWidth);

    if (m_bWideScreenOn) {
        const float height     = float(RsGlobal.maximumHeight);
        const float halfHeight = float(RsGlobal.maximumHeight / 2);
        const float scaleY     = std::bit_cast<float>(0x3B124925u); // 0x859524
        const float reduction  = (halfHeight * m_fScreenReductionPercentage) * kExePercent;
        rect->top    = reduction - (height * scaleY) * 22.0f;
        rect->bottom = (height - reduction) - (height * scaleY) * 14.0f;
    } else {
        rect->top    = 0.0f;
        rect->bottom = float(RsGlobal.maximumHeight);
    }
}

// 0x50CB60
void CCamera::SetCamCollisionVarDataSet(int32 index) {
    if (index == gCurCamColVars) {
        return;
    }

    gCurCamColVars = index;
    gCurDistForCam = 1.0f;
    gpCamColVars   = gCamColVars[index];
}

// 0x50CCA0
void CCamera::SetColVarsVehicle(eVehicleType vehicleType, int32 camVehicleZoom) {
    switch (vehicleType) {
        case VEHICLE_TYPE_AUTOMOBILE:
        case VEHICLE_TYPE_PLANE:
            SetCamCollisionVarDataSet(camVehicleZoom + 9);
            return;
        case VEHICLE_TYPE_MTRUCK:
            SetCamCollisionVarDataSet(camVehicleZoom + 12);
            return;
        case VEHICLE_TYPE_QUAD:
            SetCamCollisionVarDataSet(camVehicleZoom + 15);
            return;
        case VEHICLE_TYPE_HELI:
            SetCamCollisionVarDataSet(camVehicleZoom + 18);
            return;
        case VEHICLE_TYPE_BOAT:
            SetCamCollisionVarDataSet(camVehicleZoom + 21);
            return;
        case VEHICLE_TYPE_TRAIN:
            SetCamCollisionVarDataSet(camVehicleZoom + 24);
            return;
    }
}

// 0x515BC0
void CCamera::StartTransitionWhenNotFinishedInter(eCamMode newCamMode) {
    m_bDoingSpecialInterp = true;
    StartTransition(newCamMode);
}

// 0x515200
/**
 * @brief Initiates a camera transition to a new camera mode.
 * 
 * This function handles the transition between different camera modes, setting up all necessary parameters
 * for a smooth camera movement. It manages aspects such as:
 * - Camera rotation and positioning
 * - Transition timing and interpolation fractions
 * - Special handling for weapon modes
 * - Entity references and target updates
 * 
 * The transition process includes:
 * 1. Setting up default transition values
 * 2. Handling player rotation for weapon modes
 * 3. Setting up the new camera parameters
 * 4. Managing specific camera mode transitions
 * 5. Initializing transition state and interpolation values
 * 6. Storing starting speeds and final transition parameters
 * 
 * @param newCamMode The camera mode to transition to (type eCamMode)
 * 
 * @note This function is central to the game's camera system and affects how the camera behaves
 * when switching between different views (e.g., from following a ped to aiming a weapon).
 * 
 * @see eCamMode
 * @see CCam
 */
void CCamera::StartTransition(eCamMode newCamMode) {
    CCam& activeCam             = m_aCams[m_nActiveCam];
    const auto activeCamMode    = activeCam.m_nMode;

    // Unused flag, not used in the game.
    // In GTA III/VC it was used for the Colt Python.
    m_bItsOkToLookJustAtThePlayer = false;

    // Default values
    m_bUseTransitionBeta          = false;
    m_fFractionInterToStopMoving  = 0.25f;
    m_fFractionInterToStopCatchUp = 0.75f;

    // Como el original (0x515236..0x5152CD): el angulo del ped sale de GetATanOfXY (ST0 sin redondear) menos pi/2 del EXE (0x858FE4)
    if (notsa::contains({ MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_SNIPER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_M16_1STPERSON_RUNABOUT, MODE_FIGHT_CAM_RUNABOUT, MODE_HELICANNON_1STPERSON, MODE_CAMERA, MODE_1STPERSON_RUNABOUT }, activeCamMode)
        && m_pTargetEntity && m_pTargetEntity->GetIsTypePed()) {
        const float frontX = activeCam.m_vecFront.x, frontY = activeCam.m_vecFront.y, halfPi = kExeHalfPi;
        const auto atanOfXY = &CGeneral::GetATanOfXY;
        float angle;
        __asm {
            push frontY
            push frontX
            call atanOfXY
            add esp, 8
            fsub halfPi
            fstp angle
        }
        m_pTargetEntity->AsPed()->m_fCurrentRotation = angle;
        m_pTargetEntity->AsPed()->m_fAimingRotation  = angle;
    }

    // Setup new camera
    activeCam.m_vecCamFixedModeVector = m_vecFixedModeVector;
    CEntity::ChangeEntityReference(activeCam.m_pCamTargetEntity, m_pTargetEntity);

    activeCam.m_vecCamFixedModeSource   = m_vecFixedModeSource;
    activeCam.m_vecCamFixedModeUpOffSet = m_vecFixedModeUpOffSet;
    activeCam.m_bCamLookingAtVector     = m_bLookingAtVector;

    // Como el original (0x5153B7..): solo cambian los casos listados; cualquier otro modo no toca nada
    switch (newCamMode) {
    case MODE_BEHINDCAR:
    case MODE_BEHINDBOAT:
        activeCam.m_fBetaSpeed = 0.0f;
        break;
    case MODE_FOLLOWPED: {
        if (m_bJustCameOutOfGarage) {
            // 0x515440..0x51548C: GetATanOfXY (ST0 sin redondear) + pi del EXE (0x858CB8)
            const float frontX = activeCam.m_vecFront.x, frontY = activeCam.m_vecFront.y, pi = kExePi;
            const auto atanOfXY = &CGeneral::GetATanOfXY;
            float angle;
            __asm {
                push frontY
                push frontX
                call atanOfXY
                add esp, 8
                fadd pi
                fstp angle
            }
            activeCam.m_fHorizontalAngle = angle;
            activeCam.m_fTransitionBeta  = 0.0f;
        }

        if (m_bTargetJustCameOffTrain) {
            m_bCamDirectlyInFront = true;
        }

        if (activeCam.m_nMode == MODE_CAM_ON_A_STRING) {
            m_bUseTransitionBeta = true;
            // 0x5154B8..0x5155C8: el EXE normaliza un 0.0 (no el angulo): |0| <= pi/2 siempre se cumple y se suma SIEMPRE 235 grados
            // (0x8631DC = 4.1015239f); la rama de 55 grados (0x8631E0) no se alcanza nunca.
            const float frontX = activeCam.m_vecFront.x, frontY = activeCam.m_vecFront.y, deg235 = std::bit_cast<float>(0x40833FAFu);
            const auto atanOfXY = &CGeneral::GetATanOfXY;
            float angle;
            __asm {
                push frontY
                push frontX
                call atanOfXY
                add esp, 8
                fadd deg235
                fstp angle
            }
            activeCam.m_fTransitionBeta = angle;
        }
        break;
    }
    case MODE_SNIPER:
    case MODE_ROCKETLAUNCHER:
    case MODE_M16_1STPERSON:
    case MODE_SNIPER_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT:
    case MODE_1STPERSON_RUNABOUT:
    case MODE_M16_1STPERSON_RUNABOUT:
    case MODE_FIGHT_CAM_RUNABOUT:
    case MODE_HELICANNON_1STPERSON:
    case MODE_CAMERA:
    case MODE_ROCKETLAUNCHER_HS:
    case MODE_ROCKETLAUNCHER_RUNABOUT_HS: {
        // 0x5153CB..0x51543B: fpatan(x, y) directo del vector delantero de la matriz (no GetATanOfXY); con vehiculo FindPlayerVehicle se llama 3 veces
        float x, y;
        if (FindPlayerVehicle(-1, false)) {
            const auto yMatrix = FindPlayerVehicle(-1, false)->GetMatrix().GetForward().y;
            x = FindPlayerVehicle(-1, false)->GetMatrix().GetForward().x;
            y = yMatrix;
        } else {
            x = ActivePlayerPed()->GetMatrix().GetForward().x;
            y = ActivePlayerPed()->GetMatrix().GetForward().y;
        }
        float angle;
        __asm {
            fld x
            fld y
            fpatan
            fstp angle
        }
        activeCam.m_fHorizontalAngle = angle;
        activeCam.m_fVerticalAngle   = 0.0f;
        break;
    }
    case MODE_CAM_ON_A_STRING: {
        if (m_bLookingAtPlayer && !m_bJustCameOutOfGarage) {
            // 0x515582..0x5155E5: el EXE guarda GetATanOfXY tal cual (FIXED o no), sin sumar grados
            m_bUseTransitionBeta = true;
            activeCam.m_fTransitionBeta = CGeneral::GetATanOfXY(activeCam.m_vecFront.x, activeCam.m_vecFront.y);
        }
        break;
    }
    case MODE_PED_DEAD_BABY:
        activeCam.m_fVerticalAngle = std::bit_cast<float>(0x3E860A92u); // 0x5155EE (15 grados)
        break;
    }

    // Backup horizontal angle before Init.
    const float horizAngle = activeCam.m_fHorizontalAngle;

    int targetCoorsDuration = 600; // Like android version instead bool.
    m_nTransitionDuration   = 1'350;

    // Switch active camera
    if (activeCamMode == MODE_FOLLOWPED && newCamMode == MODE_CAM_ON_A_STRING
        || activeCamMode == MODE_CAM_ON_A_STRING && newCamMode == MODE_FOLLOWPED) {
        activeCam.m_nMode = newCamMode;
    } else {
        activeCam.Init();
        activeCam.m_nMode            = newCamMode;
        activeCam.m_fHorizontalAngle = horizAngle;
    }

    [&]() -> const void {
        if (newCamMode == MODE_CAM_ON_A_STRING && notsa::contains({ MODE_SYPHON_CRIM_IN_FRONT, MODE_FOLLOWPED, MODE_SYPHON, MODE_SPECIAL_FIXED_FOR_SYPHON, MODE_AIMWEAPON }, activeCamMode)) {
            m_fFractionInterToStopMoving  = 0.1f;
            m_fFractionInterToStopCatchUp = 0.9f;
            m_nTransitionDuration         = 750;
            return;
        }

        switch (activeCamMode) {
        case MODE_SYPHON_CRIM_IN_FRONT:
            if (newCamMode == MODE_SYPHON) {
                m_nTransitionDuration = 1'800;
                return;
            }
            break;
        case MODE_SPECIAL_FIXED_FOR_SYPHON:
            m_fFractionInterToStopMoving  = 0.2f;  // dword_8CCCCC
            m_fFractionInterToStopCatchUp = 0.8f;  // *&dword_8CCCC8
            m_nTransitionDuration         = 1'000; // dword_8CCCC4
            return;
        case MODE_FIXED:
            m_fFractionInterToStopMoving  = 0.05f;
            m_fFractionInterToStopCatchUp = 0.95f;
            return;
        }

        if (m_bPlayerWasOnBike && newCamMode == MODE_FOLLOWPED) {
            if (activeCamMode == MODE_CAM_ON_A_STRING) {
                m_nTransitionDuration         = 800;
                m_fFractionInterToStopMoving  = 0.02f;
                m_fFractionInterToStopCatchUp = 0.98f;
                return;
            }
        } else {
            switch (newCamMode) {
            case MODE_CAM_ON_A_STRING:
            case MODE_BEHINDBOAT:
                if (notsa::contains({ MODE_SNIPER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_1STPERSON_RUNABOUT, MODE_M16_1STPERSON_RUNABOUT, MODE_FIGHT_CAM_RUNABOUT, MODE_CAMERA }, activeCamMode)) {
                    m_fFractionInterToStopMoving  = 0.0f;
                    m_fFractionInterToStopCatchUp = 1.0f;
                    m_nTransitionDuration         = 1;
                    return;
                }
                break;
            case MODE_AIMWEAPON:
                m_fFractionInterToStopMoving  = StaticRef<float>(0xB70044); // 0x5157B4: dword_B70044 (estatico del EXE)
                m_fFractionInterToStopCatchUp = 1.0f; // *&dword_8CCCC0
                m_nTransitionDuration         = 400;  // dword_8CCCBC
                targetCoorsDuration           = 350;
                return;
            }

            if (!notsa::contains({ MODE_FOLLOWPED, MODE_SYPHON_CRIM_IN_FRONT, MODE_SYPHON, MODE_SPECIAL_FIXED_FOR_SYPHON }, newCamMode)) {
                m_nTransitionDuration = 1'350;
                return;
            }
        }
        if (!notsa::contains({ MODE_SYPHON_CRIM_IN_FRONT, MODE_FOLLOWPED, MODE_SYPHON, MODE_AIMWEAPON }, activeCamMode)) {
            m_nTransitionDuration = 1'350;
            return;
        }
        m_fFractionInterToStopMoving  = 0.1f;
        m_fFractionInterToStopCatchUp = 0.9f;
        m_nTransitionDuration         = 350;
        targetCoorsDuration           = 350;
    }();

    // Initialize transition state
    m_bTransitionState       = true;
    m_nTimeTransitionStart   = CTimer::GetTimeInMS();
    m_bTransitionJUSTStarted = true;

    // Store starting interpolation values
    if (m_bDoingSpecialInterp) {
        m_vecStartingSourceForInterPol = m_vecSourceDuringInter;
        m_vecStartingTargetForInterPol = m_vecTargetDuringInter;
        m_vecStartingUpForInterPol     = m_vecUpDuringInter;
        m_fStartingAlphaForInterPol    = m_fAlphaDuringInterPol;
        m_fStartingBetaForInterPol     = m_fBetaDuringInterPol;
    } else {
        m_vecStartingSourceForInterPol = activeCam.m_vecSource;
        m_vecStartingTargetForInterPol = activeCam.m_vecTargetCoorsForFudgeInter;
        m_vecStartingUpForInterPol     = activeCam.m_vecUp;
        m_fStartingAlphaForInterPol    = activeCam.m_fTrueAlpha;
        m_fStartingBetaForInterPol     = activeCam.m_fTrueBeta;
    }

    // Update active camera parameters
    activeCam.m_bCamLookingAtVector     = m_bLookingAtVector;
    activeCam.m_vecCamFixedModeVector   = m_vecFixedModeVector;
    activeCam.m_vecCamFixedModeSource   = m_vecFixedModeSource;
    activeCam.m_vecCamFixedModeUpOffSet = m_vecFixedModeUpOffSet;
    activeCam.m_nMode                   = newCamMode;
    CEntity::ChangeEntityReference(activeCam.m_pCamTargetEntity, m_pTargetEntity);

    // Store starting speeds
    m_fStartingFOVForInterPol    = activeCam.m_fFOV;
    m_vecSourceSpeedAtStartInter = activeCam.m_vecSourceSpeedOverOneFrame;
    m_vecTargetSpeedAtStartInter = activeCam.m_vecTargetSpeedOverOneFrame;
    m_vecUpSpeedAtStartInter     = activeCam.m_vecUpOverOneFrame;
    m_fAlphaSpeedAtStartInter    = activeCam.m_fAlphaSpeedOverOneFrame;
    m_fBetaSpeedAtStartInter     = activeCam.m_fBetaSpeedOverOneFrame;
    m_fFOVSpeedAtStartInter      = activeCam.m_fFovSpeedOverOneFrame;

    // Setup final transition parameters
    if (m_bLookingAtPlayer) {
        m_fFractionInterToStopMovingTarget  = 0.0f;
        m_fFractionInterToStopCatchUpTarget = 1.0f;
        m_nTransitionDurationTargetCoors    = targetCoorsDuration;
    } else {
        if (m_bScriptParametersSetForInterp) {
            m_fFractionInterToStopMoving  = m_fScriptPercentageInterToStopMoving;
            m_fFractionInterToStopCatchUp = m_fScriptPercentageInterToCatchUp;
            m_nTransitionDuration         = m_nScriptTimeForInterpolation;
        }
        m_nTransitionDurationTargetCoors    = m_nTransitionDuration;
        m_fFractionInterToStopMovingTarget  = m_fFractionInterToStopMoving;
        m_fFractionInterToStopCatchUpTarget = m_fFractionInterToStopCatchUp;
    }
}

auto CCamera::GetFrustumPoints() -> std::array<CVector, 5> {
    CVector pts[5]{};

    // First, the corners
    const auto farPlane  = RwCameraGetFarClipPlane(m_pRwCamera);
    const auto farVWSize = CVector2D{ *RwCameraGetViewWindow(m_pRwCamera) } * farPlane;
    const auto corners   = CRect{ -farVWSize, farVWSize }.GetCorners3D(farPlane);

    // Copy it into pts
    rng::copy(corners, pts);

    // Last is the center point
    pts[4] = CVector{ 0.f, 0.f, 0.f };

    // Transform them to world space
    RwV3dTransformPoints(pts, pts, 5, GetRwMatrix());

    // top left, top right, bottom right, bottom left, center
    return std::to_array(pts);
}
