/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPad
 * Reconstrucción: COMPLETA — 80/80 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 80/80 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Pad.h"
#include "Cam.h"
#include "platform/win/WinInput.h"
#include "ControllerConfigManager.h"
#include "app.h"
#include "X87Math.h"
#include "platform/win/WinPlatform.h"
#include "platform/win/VideoMode.h"

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>
#include <SDLWrapper.hpp>
#endif


// mouse states 

static auto& byte_B73403 = StaticRef<bool>(0xB73403); // TODO: Find out what modifies this, as it has no value by default..
static auto& byte_8CD782 = StaticRef<bool>(0x8CD782); // true by default, left here for documentation purposes, as it's used in multiple CPad functions
static auto& byte_B73401 = StaticRef<char>(0xB73401); // unused, unknown

void CPad::InjectHooks() {
    RH_ScopedClass(CPad);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(GetCameraCycleUp, 0x5404A0);
    RH_ScopedInstall(GetMouseCameraStickHorizontal, 0x540E80);
    RH_ScopedInstall(GetMouseCameraStickVertical, 0x540F80);

    // CPad", 0x541D80, &CPad::Constructor);
    // CPad~", 0x53ED60, &CPad::Destructor);
    RH_ScopedGlobalInstall(GetCurrentKeyPressed, 0x541490);
    RH_ScopedGlobalInstall(DIReleaseMouse, 0x746F70);
    RH_ScopedInstall(Initialise, 0x541D90);
    RH_ScopedInstall(ClearKeyBoardHistory, 0x53F1E0);
    RH_ScopedInstall(ClearMouseHistory, 0x541BD0);
    RH_ScopedInstall(Clear, 0x541A70);
    RH_ScopedInstall(Update, 0x541C40);
    RH_ScopedInstall(UpdateMouse, 0x53F3C0);
    RH_ScopedInstall(ProcessPad, 0x746A10);
    RH_ScopedInstall(ProcessPCSpecificStuff, 0x53FB40);
    RH_ScopedInstall(ReconcileTwoControllersInput, 0x53F530);
    RH_ScopedInstall(SetTouched, 0x53F200);
    RH_ScopedInstall(GetTouchedTimeDelta, 0x53F210);
    RH_ScopedInstall(StartShake, 0x53F920);
    RH_ScopedInstall(StartShake_Distance, 0x53F9A0);
    RH_ScopedInstall(StartShake_Train, 0x53FA70); 
    RH_ScopedInstall(StopShaking, 0x53FB50);
    RH_ScopedInstall(GetCarGunLeftRight, 0x53FC50);
    RH_ScopedInstall(GetCarGunUpDown, 0x53FC10);
    RH_ScopedInstall(GetSteeringLeftRight, 0x53FB80);
    RH_ScopedInstall(GetCarGunFired, 0x53FF90);
    RH_ScopedInstall(CarGunJustDown, 0x53FFE0);
    RH_ScopedInstall(GetSteeringUpDown, 0x53FBD0);
    RH_ScopedOverloadedInstall(GetPedWalkLeftRight, "ped", 0x540DC0, int16 (CPad::*)(CPed*) const);
    RH_ScopedOverloadedInstall(GetPedWalkLeftRight, "",    0x53FC90, int16 (CPad::*)() const);
    RH_ScopedOverloadedInstall(GetPedWalkUpDown, "ped",    0x540E20, int16 (CPad::*)(CPed*) const);
    RH_ScopedOverloadedInstall(GetPedWalkUpDown, "",       0x53FD30, int16 (CPad::*)() const);
    RH_ScopedInstall(GetLookLeft, 0x53FDD0);
    RH_ScopedInstall(GetLookRight, 0x53FE10);
    RH_ScopedInstall(GetLookBehindForCar, 0x53FE70);
    RH_ScopedInstall(GetLookBehindForPed, 0x53FEC0);
    RH_ScopedInstall(GetHorn, 0x53FEE0);
    RH_ScopedInstall(HornJustDown, 0x53FF30);
    RH_ScopedInstall(GetHandBrake, 0x540040);
    RH_ScopedInstall(GetBrake, 0x540080);
    RH_ScopedInstall(GetExitVehicle, 0x5400D0);
    RH_ScopedInstall(ExitVehicleJustDown, 0x540120);
    RH_ScopedInstall(GetMeleeAttack, 0x540340);
    RH_ScopedInstall(MeleeAttackJustDown, 0x540390);
    RH_ScopedInstall(GetAccelerate, 0x5403F0);
    RH_ScopedInstall(GetAccelerateJustDown, 0x540440);
    RH_ScopedInstall(CycleWeaponLeftJustDown, 0x540610);
    RH_ScopedInstall(CycleWeaponRightJustDown, 0x540640);
    RH_ScopedInstall(GetTarget, 0x540670);
    RH_ScopedInstall(GetSprint, 0x5407A0);
    RH_ScopedInstall(SprintJustDown, 0x5407F0);
    RH_ScopedInstall(GetDisplayVitalStats, 0x5408B0);
    RH_ScopedInstall(CollectPickupJustDown, 0x540A70);
    RH_ScopedInstall(GetForceCameraBehindPlayer, 0x540AE0);
    RH_ScopedInstall(SniperZoomIn, 0x540B30);
    RH_ScopedInstall(SniperZoomOut, 0x540B80);
    RH_ScopedInstall(WeaponJustDown, 0x540250);
    RH_ScopedInstall(GetEnterTargeting, 0x5406B0);
    RH_ScopedInstall(GetWeapon, 0x540180);
    RH_ScopedInstall(AimWeaponLeftRight, 0x541040);
    RH_ScopedInstall(AimWeaponUpDown, 0x5410C0);
    RH_ScopedInstall(NextStationJustUp, 0x5405B0);
    RH_ScopedInstall(LastStationJustUp, 0x5405E0);
    RH_ScopedInstall(GetHydraulicJump, 0x53FF70);
    RH_ScopedInstall(GetDuck, 0x540700);
    RH_ScopedInstall(DuckJustDown, 0x540720);
    RH_ScopedInstall(GetJump, 0x540750);
    RH_ScopedInstall(JumpJustDown, 0x540770);
    RH_ScopedInstall(ShiftTargetLeftJustDown, 0x540850);
    RH_ScopedInstall(ShiftTargetRightJustDown, 0x540880);
    RH_ScopedInstall(GetGroupControlForward, 0x541190);
    RH_ScopedInstall(GetGroupControlBack, 0x5411B0);
    RH_ScopedInstall(ConversationYesJustDown, 0x5411D0);
    RH_ScopedInstall(ConversationNoJustDown, 0x541200);
    RH_ScopedInstall(GroupControlForwardJustDown, 0x541230);
    RH_ScopedInstall(GroupControlBackJustDown, 0x541260);
    RH_ScopedInstall(LookAroundLeftRight, 0x540BD0);
    RH_ScopedInstall(LookAroundUpDown, 0x540CC0);
    RH_ScopedInstall(GetAnaloguePadUp, 0x540950);
    RH_ScopedInstall(GetAnaloguePadLeft, 0x5409B0);
    RH_ScopedInstall(GetAnaloguePadRight, 0x5409E0);
    RH_ScopedInstall(GetAnaloguePadDown, 0x540980);
#ifndef NOTSA_USE_SDL3
    RH_ScopedInstall(GetMouseState, 0x746ED0);
#endif
}

// 0x541D80
CPad::CPad() {
    Clear(true, true);
}

// 0x541D90
void CPad::Initialise() {
    for (auto& pad : Pads) {
        pad.Clear(true, true);
        pad.Mode = 0;
    }
    padNumber = 0;
    byte_B73401 = 0;
}

// 0x53F1E0
void CPad::ClearKeyBoardHistory() {
    NewKeyState.Clear();
    OldKeyState.Clear();
    TempKeyState.Clear();
}

// 0x541BD0
void CPad::ClearMouseHistory() {
    TempMouseControllerState.Clear();
    NewMouseControllerState.Clear();
    OldMouseControllerState.Clear();
}

// 0x541A70
void CPad::Clear(bool clearDisabledControls, bool resetPhase) {
    NewState.Clear();
    OldState.Clear();

    PCTempKeyState.Clear();
    PCTempJoyState.Clear();
    PCTempMouseState.Clear();

    ClearKeyBoardHistory();
    ClearMouseHistory();

    if (resetPhase) {
        Phase = 0;
    }
    ShakeFreq = 0;
    ShakeDur = 0;
    std::ranges::fill(SteeringLeftRightBuffer, 0);
    DrunkDrivingBufferUsed = 0;
    if (clearDisabledControls) {
        DisablePlayerControls = 0;
        bDisablePlayerEnterCar = false;
        bDisablePlayerDuck = false;
        bDisablePlayerFireWeapon = false;
        bDisablePlayerFireWeaponWithL1 = false;
        bDisablePlayerCycleWeapon = false;
        bDisablePlayerJump = false;
        bDisablePlayerDisplayVitalStats = false;
    }
    JustOutOfFrontEnd = 0;
    bApplyBrakes = false;
    std::ranges::fill(bHornHistory, 0);
    iCurrHornHistory = 0;
    AverageWeapon = 0;
    AverageEntries = 0;
    LastTimeTouched = 0;
    NoShakeBeforeThis = 0;
    NoShakeFreq = 0;
}

// 0x541C40
void CPad::Update(int32 pad) {
    OldState = NewState;

    // Writes directly into NewState
    ReconcileTwoControllersInput(NewState, PCTempKeyState, PCTempJoyState);
    ReconcileTwoControllersInput(NewState, PCTempMouseState, NewState);

    PCTempJoyState.Clear();
    PCTempKeyState.Clear();
    PCTempMouseState.Clear();

    if (NewState.CheckForInput()) {
        SetTouched();
    }

    iCurrHornHistory = (iCurrHornHistory + 1) % 5;
    bHornHistory[iCurrHornHistory] = GetHorn();

    // Shift right by 1
    std::copy_backward(
        std::begin(SteeringLeftRightBuffer),
        std::end(SteeringLeftRightBuffer) - 1,
        std::end(SteeringLeftRightBuffer)
    );

    if (JustOutOfFrontEnd)
        JustOutOfFrontEnd--;
}

// 0x541DD0
void CPad::UpdatePads() {
    ZoneScoped;

    // Como el original: sin el bloqueo de ratón/teclado del menú ImGui (ya no existe).
    GetPad(0)->UpdateMouse();
    ProcessPad(PAD1);
    ControlsManager.ClearSimButtonPressCheckers();
    ControlsManager.AffectPadFromKeyBoard();
    ControlsManager.AffectPadFromMouse();
    GetPad(PAD1)->Update(PAD1);
    GetPad(PAD2)->Update(PAD2);

    OldKeyState = std::exchange(NewKeyState, TempKeyState);
}

// 0x746ED0
HRESULT CPad::GetMouseState(DIMOUSESTATE2 *dm) {
    if (!PSGLOBAL(diMouse)) {
        return 1;
    }

    // Zero out the mouse state
    dm->lX = dm->lY = dm->lZ = 0;
    std::fill(std::begin(dm->rgbButtons), std::end(dm->rgbButtons), 0);

    // Try to get the device state
    HRESULT hr = PSGLOBAL(diMouse)->GetDeviceState(sizeof(DIMOUSESTATE2), dm);
    if (SUCCEEDED(hr)) {
        return 0; // Success
    }

    // If failed, try to reacquire the device
    while (PSGLOBAL(diMouse)->Acquire() == DIERR_INPUTLOST) {
        // Keep trying until acquisition succeeds or fails with a different error
    }

    // Zero out the mouse state again
    dm->lX = dm->lY = dm->lZ = 0;
    std::fill(std::begin(dm->rgbButtons), std::end(dm->rgbButtons), 0);

    // Try one more time to get the state
    return PSGLOBAL(diMouse)->GetDeviceState(sizeof(DIMOUSESTATE2), dm);
}

// 0x53F3C0
void CPad::UpdateMouse() {
    if (!IsForegroundApp()) {
        return;
    }
    if (!PSGLOBAL(diMouse)) {
        WinInput::diMouseInit(!FrontEndMenuManager.m_bMenuActive && IsVideoModeExclusive());
    }
    DIMOUSESTATE2 state;
    if (!PSGLOBAL(diMouse) || FAILED(GetMouseState(&state))) {
        return;
    }
    const auto invertX = !FrontEndMenuManager.m_bMenuActive && FrontEndMenuManager.bInvertMouseX ? -1 : 1;
    const auto invertY = !FrontEndMenuManager.m_bMenuActive && FrontEndMenuManager.bInvertMouseY ? -1 : 1;
    TempMouseControllerState.m_AmountMoved = {float(invertX * state.lX), float(invertY * state.lY)};
    TempMouseControllerState.m_fWheelMoved = float(state.lZ);
    TempMouseControllerState.isMouseWheelMovedUp = state.lZ > 0;
    TempMouseControllerState.isMouseWheelMovedDown = state.lZ < 0;
    TempMouseControllerState.isMouseLeftButtonPressed = state.rgbButtons[0] & 0x80;
    TempMouseControllerState.isMouseRightButtonPressed = state.rgbButtons[1] & 0x80;
    TempMouseControllerState.isMouseMiddleButtonPressed = state.rgbButtons[2] & 0x80;
    TempMouseControllerState.isMouseFirstXPressed = state.rgbButtons[3] & 0x80;
    TempMouseControllerState.isMouseSecondXPressed = state.rgbButtons[4] & 0x80;
    OldMouseControllerState = std::exchange(NewMouseControllerState, TempMouseControllerState);
    if (NewMouseControllerState.CheckForInput()) {
        SetTouched();
    }
}

namespace {
// fild value; fmul 0.0005f (0x859978); fstp: el entero entra exacto en ST0 y el producto se redondea una sola vez (IntTimesFloatCpp).
float NormalizePadAxis(LONG value) {
    return IntTimesFloatCpp(static_cast<int>(value), 0.0005F);
}
}

// 0x746A10
void CPad::ProcessPad(ePadID padID) {
    LPDIRECTINPUTDEVICE8* pDiDevice = nullptr;
    DIJOYSTATE2 joyState;
    auto& leftStickPos = StaticRef<RwV2d>(0xC8CFB8);
    auto& rightStickPos = StaticRef<RwV2d>(0xC8CFB0);

    if (padID == PAD1) {
        pDiDevice = &PSGLOBAL(diDevice1);
    } else if (padID == PAD2) {
        pDiDevice = &PSGLOBAL(diDevice2);
    } else {
        return;
    }
    
    if (!pDiDevice || !*pDiDevice) {
        return;
    }

    // Poll the device to read the current state
    HRESULT hr = (*pDiDevice)->Poll();
    
    if (FAILED(hr)) {
        // DInput is telling us that the input stream has been
        // interrupted. We just re-acquire and try again.
        hr = (*pDiDevice)->Acquire();
        while (hr == DIERR_INPUTLOST) 
            hr = (*pDiDevice)->Acquire();
        
        // Return if acquisition failed due to other reasons
        if (FAILED(hr))
            return;
        
        hr = (*pDiDevice)->Poll();
        if (FAILED(hr))
            return;
    }

    // Get the device state
    WIN_FCHECK((*pDiDevice)->GetDeviceState(sizeof(joyState), &joyState));

    // Update NewJoyState with the current joyState and get the previous value
    DIJOYSTATE2 previousNewJoyState = std::exchange(ControlsManager.m_NewJoyState, joyState);

    if (ControlsManager.m_bJoyJustInitialised) {
        ControlsManager.m_OldJoyState = ControlsManager.m_NewJoyState;
        ControlsManager.m_bJoyJustInitialised = false;
    } else {
        ControlsManager.m_OldJoyState = previousNewJoyState;
    }

    RsPadEventHandler(RsEvent::rsPADBUTTONUP, &padID);

    if (*pDiDevice) {
        // Calculate left stick position
        leftStickPos.x = NormalizePadAxis(joyState.lX);
        leftStickPos.y = NormalizePadAxis(joyState.lY);
        
        // Handle POV
        if (LOWORD(joyState.rgdwPOV[0]) != 0xFFFF) {
            // Como el original (0x746B32..0x746B62): fild con signo y, si es negativo, fadd de 2^32 (float) con la precision x87 del juego; despues
            // * 0.01f * 0.017453292f. El angulo queda en ST0 sin redondear a float (fld st(0); fsin; fstp; fcos; fmul -1.0f; fstp): PadPovAngleCpp lo
            // devuelve como double y PadPovSinCosCpp calcula seno y -coseno con la misma precision.
            const double angle = PadPovAngleCpp(joyState.rgdwPOV[0]);
            float horizontal, vertical;
            PadPovSinCosCpp(angle, &horizontal, &vertical);
            leftStickPos = {horizontal, vertical};
        }
        
        // Calculate right stick position
        if (AllValidWinJoys.JoyStickNum[padID].bZRotPresent && AllValidWinJoys.JoyStickNum[padID].bZAxisPresent) {
            rightStickPos.x = NormalizePadAxis(joyState.lZ);
            rightStickPos.y = NormalizePadAxis(joyState.lRz);
        }
    }

    // Como el original (0x746B00: je 0x746BA6): con el dispositivo nulo solo se salta el bloque de sticks (POV y Z/RZ); eventos y ejes siguen siempre.
    padID = padNumber ? PAD2 : PAD1;
    RsPadEventHandler(RsEvent::rsPADBUTTONUP, &padID);
    RsPadEventHandler(RsEvent::rsPADBUTTONDOWN, &padID);
    CPad& pPad = *CPad::GetPad(padNumber ? PAD2 : PAD1);

    const auto UpdateJoyStickPosition = [&](float& pos, int16& outSwapped, int16& outNormal, bool isInverted, bool isSwapped) {
        if (std::fabs(pos) <= 0.3F) {
            return;
        }
        if (isInverted) {
            pos *= -1.0f;
        }
        (isSwapped ? outSwapped : outNormal) = static_cast<int16>(pos * 128.0f);
    };

    // Left stick
    UpdateJoyStickPosition(leftStickPos.x, pPad.PCTempJoyState.LeftStickY, pPad.PCTempJoyState.LeftStickX, FrontEndMenuManager.m_bInvertPadX1, FrontEndMenuManager.m_bSwapPadAxis1);
    UpdateJoyStickPosition(leftStickPos.y, pPad.PCTempJoyState.LeftStickX, pPad.PCTempJoyState.LeftStickY, FrontEndMenuManager.m_bInvertPadY1, FrontEndMenuManager.m_bSwapPadAxis1);

    // Right stick
    UpdateJoyStickPosition(rightStickPos.x, pPad.PCTempJoyState.RightStickY, pPad.PCTempJoyState.RightStickX, FrontEndMenuManager.m_bInvertPadX2, FrontEndMenuManager.m_bSwapPadAxis2);
    UpdateJoyStickPosition(rightStickPos.y, pPad.PCTempJoyState.RightStickX, pPad.PCTempJoyState.RightStickY, FrontEndMenuManager.m_bInvertPadY2, FrontEndMenuManager.m_bSwapPadAxis2);
}

// 0x53FB40
void CPad::ProcessPCSpecificStuff() {
    // NOP
}

// 0x53F530
CControllerState& CPad::ReconcileTwoControllersInput(CControllerState& out, const CControllerState& controllerA, const CControllerState& controllerB) {
#define RECONCILE_BUTTON(button)                                                                                                                                                   \
    {                                                                                                                                                                              \
        if (controllerA.button || controllerB.button)                                                                                                                              \
            reconciledState.button = 255;                                                                                                                                          \
    }

#define RECONCILE_AXIS_POSITIVE(axis)                                                                                                                                              \
    {                                                                                                                                                                              \
        if (controllerA.axis >= 0 && controllerB.axis >= 0)                                                                                                                        \
            reconciledState.axis = std::max(controllerA.axis, controllerB.axis);                                                                                                   \
    }

#define RECONCILE_AXIS_NEGATIVE(axis)                                                                                                                                              \
    {                                                                                                                                                                              \
        if (controllerA.axis <= 0 && controllerB.axis <= 0)                                                                                                                        \
            reconciledState.axis = std::min(controllerA.axis, controllerB.axis);                                                                                                   \
    }

#define RECONCILE_AXIS(axis)                                                                                                                                                       \
    {                                                                                                                                                                              \
        RECONCILE_AXIS_POSITIVE(axis);                                                                                                                                             \
        RECONCILE_AXIS_NEGATIVE(axis);                                                                                                                                             \
    }

#define FIX_AXIS_DIR(axis)                                                                                                                                                         \
    {                                                                                                                                                                              \
        if (controllerA.axis > 0 && controllerB.axis < 0 || controllerA.axis < 0 && controllerB.axis > 0)                                                                          \
            reconciledState.axis = 0;                                                                                                                                              \
    }

#define FIX_RECON_DIR(pos, neg, axis)                                                                                                                                              \
    {                                                                                                                                                                              \
        if ((reconciledState.pos || reconciledState.axis < 0) && (reconciledState.neg || reconciledState.axis > 0)) {                                                              \
            reconciledState.pos = 0;                                                                                                                                               \
            reconciledState.neg = 0;                                                                                                                                               \
            reconciledState.axis = 0;                                                                                                                                              \
        }                                                                                                                                                                          \
    }

    static CControllerState reconciledState;
    reconciledState.Clear();

    RECONCILE_BUTTON(LeftShoulder1)
    RECONCILE_BUTTON(LeftShoulder2)
    RECONCILE_BUTTON(RightShoulder1)
    RECONCILE_BUTTON(RightShoulder2)
    RECONCILE_BUTTON(Start)
    RECONCILE_BUTTON(Select)
    RECONCILE_BUTTON(ButtonSquare)
    RECONCILE_BUTTON(ButtonTriangle)
    RECONCILE_BUTTON(ButtonCross)
    RECONCILE_BUTTON(ButtonCircle)
    RECONCILE_BUTTON(ShockButtonL)
    RECONCILE_BUTTON(ShockButtonR)
    RECONCILE_BUTTON(m_bChatIndicated)
    RECONCILE_BUTTON(m_bPedWalk)
    RECONCILE_BUTTON(m_bRadioTrackSkip)
    RECONCILE_BUTTON(m_bVehicleMouseLook)

    RECONCILE_AXIS(LeftStickX);
    RECONCILE_AXIS(LeftStickY);
    FIX_AXIS_DIR(LeftStickX);
    FIX_AXIS_DIR(LeftStickY);
    RECONCILE_AXIS(RightStickX);
    RECONCILE_AXIS(RightStickY);
    FIX_AXIS_DIR(RightStickX);
    FIX_AXIS_DIR(RightStickY);

    RECONCILE_BUTTON(DPadUp)
    RECONCILE_BUTTON(DPadDown)
    RECONCILE_BUTTON(DPadLeft)
    RECONCILE_BUTTON(DPadRight)

    FIX_RECON_DIR(DPadUp, DPadDown, LeftStickY);
    FIX_RECON_DIR(DPadLeft, DPadRight, LeftStickX);

    out = reconciledState;
    return out;

#undef RECONCILE_BUTTON
#undef RECONCILE_AXIS_POSITIVE
#undef RECONCILE_AXIS_NEGATIVE
#undef RECONCILE_AXIS
#undef FIX_AXIS_DIR
#undef FIX_RECON_DIR
}

// 0x53F200
void CPad::SetTouched() {
    LastTimeTouched = CTimer::GetTimeInMS();
}

// 0x53F210
uint32 CPad::GetTouchedTimeDelta() const {
    return CTimer::GetTimeInMS() - LastTimeTouched;
}

// 0x53F920
void CPad::StartShake(int16 time, uint8 freq, uint32 shakeDelayMs) {
    if (!FrontEndMenuManager.m_PrefsUseVibration || CCutsceneMgr::ms_running)
        return;

    if (freq) {
        if (CTimer::GetTimeInMS() >= NoShakeBeforeThis || freq > NoShakeFreq) {
            if (time > ShakeDur) {
                ShakeDur = time;
                ShakeFreq = freq;
            }
            NoShakeBeforeThis = shakeDelayMs + CTimer::GetTimeInMS();
            NoShakeFreq = freq;
        }
    } else {
        ShakeDur = 0;
        ShakeFreq = 0;
    }
}

// originally 3x float instead of CVector
// 0x53F9A0
void CPad::StartShake_Distance(int16 time, uint8 freq, CVector pos) {
    if (!FrontEndMenuManager.m_PrefsUseVibration || CCutsceneMgr::ms_running)
        return;

    const auto& camera = TheCamera.GetPosition();
    // Orden z^2 + y^2 + x^2, raiz y comparacion ordenada con 70.0f (0x858CE0) como el EXE: ver PadShakeWithinDistanceCpp (X87Math.cpp).
    const bool withinDistance = PadShakeWithinDistanceCpp(&camera.x, &pos.x, 70.0f);
    if (withinDistance) {
        if (freq) {
            if (time > ShakeDur) {
                ShakeDur = time;
                ShakeFreq = freq;
            }
        } else {
            ShakeDur = 0;
            ShakeFreq = 0;
        }
    }
}

// 0x53FA70
void CPad::StartShake_Train(CVector2D point) {
    if (!FrontEndMenuManager.m_PrefsUseVibration || CCutsceneMgr::ms_running) {
        return;
    }

    if (FindPlayerVehicle(-1) && FindPlayerVehicle(-1)->IsTrain()) {
        return;
    }

    const auto& camera = TheCamera.GetPosition();
    // y^2 + x^2 (0x53FAD8..0x53FAF9): se guarda como float (fst) pero se compara sin redondear con 4900.0f (0x8599CC).
    float distanceSquared;
    const bool withinDistance = PadShakeTrainWithinCpp(&camera.x, &point.x, &distanceSquared);
    if (withinDistance && ShakeDur < 100) {
        ShakeDur = 100;
        // 70.0f - sqrt(distanceSquared) + 30.0f y _ftol2 (0x821B40): solo cuenta AL (0x53FB36).
        ShakeFreq = static_cast<char>(PadShakeTrainFrequencyCpp(distanceSquared));
    }
}

// 0x541D70
void CPad::StopPadsShaking() {
    for (auto i = 0u; i < std::size(Pads); i++) {
        GetPad(i)->StopShaking(i);
    }
}

// 0x53FB50
void CPad::StopShaking(int16 arg0) {
    // NOP
}

// 0x53FC50
int16 CPad::GetCarGunLeftRight() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
        return GetRightStickX();
    case 3:
        return (NewState.DPadRight - NewState.DPadLeft) / 2;
    }
    return 0;
}

// 0x53FC10
int16 CPad::GetCarGunUpDown() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
        return GetRightStickY();
    case 3:
        return (NewState.DPadUp - NewState.DPadDown) / 2;
    }
    return 0;
}

// 0x53FB80
int16 CPad::GetSteeringLeftRight() {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
    case 3: {
        SteeringLeftRightBuffer[0] = GetLeftStickX();
        return SteeringLeftRightBuffer[DrunkDrivingBufferUsed];
    }
    }
    return 0;
}

// 0x53FF90
int16 CPad::GetCarGunFired() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 2: {
        if (BUTTON_IS_DOWN(ButtonCircle))
            return 1;

        if (!bDisablePlayerFireWeaponWithL1 && BUTTON_IS_DOWN(LeftShoulder1))
            return 2;

        break;
    }
    case 3:
        return NewState.RightShoulder1 != 0;
    }
    return false;
}

// 0x53FFE0
int16 CPad::CarGunJustDown() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 2: {
        if (IsCirclePressed())
            return 1;

        if (!bDisablePlayerFireWeaponWithL1 && IsLeftShoulder1Pressed())
            return 2;

        break;
    }
    case 3:
        return IsRightShoulder1Pressed();
    }
    return false;
}

// 0x53FBD0
int16 CPad::GetSteeringUpDown() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
    case 3:
        return GetLeftStickY();
    }
    return 0;
}

// 0x540DC0
int16 CPad::GetPedWalkLeftRight(CPed* ped) const {
    if (DisablePlayerControls)
        return 0;

    if (byte_B73403 || CCamera::m_bUseMouse3rdPerson) {
        if (ped && ped->m_pAttachedTo)
            return 0;

        if (byte_8CD782)
            return GetLeftStickX();

        return GetRightStickX();
    }

    return 0;
}

// 0x53FC90
int16 CPad::GetPedWalkLeftRight() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
    case 3:
        return GetLeftStickX();
    }
    return 0;
}

// 0x540E20
int16 CPad::GetPedWalkUpDown(CPed* ped) const {
    if (DisablePlayerControls)
        return 0;

    if (byte_B73403 || CCamera::m_bUseMouse3rdPerson) {
        if (ped && ped->m_pAttachedTo)
            return 0;

        if (byte_8CD782)
            return GetLeftStickY();

        return GetRightStickY();
    }

    return 0;
}

// 0x53FD30
int16 CPad::GetPedWalkUpDown() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
    case 3:
        return GetLeftStickY();
    }
    return 0;
}

// 0x53FDD0
bool CPad::GetLookLeft() const {
    if (DisablePlayerControls)
        return false;

    if (IsLeftShoulder2Pressed())
        return false;

    if (IsRightShoulder2JustUp())
        return false;

    return NewState.LeftShoulder2 && !NewState.RightShoulder2;
}

// 0x53FE10
bool CPad::GetLookRight() const {
    if (DisablePlayerControls)
        return false;

    if (IsRightShoulder2Pressed())
        return false;

    if (IsLeftShoulder2JustUp())
        return false;

    return NewState.RightShoulder2 && !NewState.LeftShoulder2;
}

bool CPad::GetLookBehindForCar() const noexcept {
    if (DisablePlayerControls)
        return false;

    return (NewState.LeftShoulder2 || OldState.LeftShoulder2) && (NewState.RightShoulder2 || OldState.RightShoulder2);
}

// 0x53FEE0
bool CPad::GetHorn() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 3:
        return NewState.ShockButtonL;
    case 1:
        return NewState.LeftShoulder1;
    case 2:
        return NewState.RightShoulder1;
    }
    return false;
}

// 0x53FF30
bool CPad::HornJustDown() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 3:
        return IsLeftShockPressed();
    case 1:
        return IsLeftShoulder1Pressed();
    case 2:
        return IsRightShoulder1Pressed();
    }
    return false;
}

// 0x540040
int16 CPad::GetHandBrake() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
        return NewState.RightShoulder1;
    case 2:
        return NewState.ButtonTriangle;
    case 3:
        return NewState.LeftShoulder1;
    }
    return 0;
}

// 0x540080
int16 CPad::GetBrake() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
        return NewState.ButtonSquare;
    case 3:
        int32 axis = 2 * GetRightStickY();
        return axis < 0 ? 0 : axis;
    }
    return 0;
}

// 0x5400D0
bool CPad::GetExitVehicle() const {
    if (DisablePlayerControls || bDisablePlayerEnterCar)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 3:
        return NewState.ButtonTriangle;
    case 2:
        return NewState.LeftShoulder1;
    }
    return false;
}

// 0x540120
bool CPad::ExitVehicleJustDown() const {
    if (DisablePlayerControls || bDisablePlayerEnterCar)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 3:
        return IsTrianglePressed();
    case 2:
        return IsLeftShoulder1Pressed();
    }
    return false;
}

// 0x540340
uint8 CPad::GetMeleeAttack(bool bCheckButtonCircleStateOnly) const {
    if (DisablePlayerControls)
        return 0;

    if (NewState.ButtonCircle)
        return 1;

    if (bCheckButtonCircleStateOnly) {
        if (NewState.ButtonCross)
            return 2;

        if (NewState.ButtonSquare)
            return 3;

        if (NewState.ButtonTriangle)
            return 4;
    }

    return 0;
}

// 0x540390
uint8 CPad::MeleeAttackJustDown(bool bCheckButtonCircleStateOnly) const {
    if (DisablePlayerControls)
        return 0;

    if (IsCirclePressed())
        return 1;

    if (!bCheckButtonCircleStateOnly)
        return 0;

    if (IsCrossPressed())
        return 2;

    if (BUTTON_IS_DOWN(ButtonSquare))
        return 3;

    if (IsTrianglePressed())
        return 4;

    return 0;
}

// 0x5403F0
int16 CPad::GetAccelerate() const {
    if (DisablePlayerControls)
        return 0;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
        return NewState.ButtonCross;
    case 3: {
        int32 axis = -2 * GetRightStickY();
        return axis < 0 ? 0 : axis;
    }
    }
    return 0;
}

// 0x540440
bool CPad::GetAccelerateJustDown() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
        return IsCrossPressed();
    case 3:
        return IsRightStickYPressed();
    }
    return false;
}

// 0x540610
bool CPad::CycleWeaponLeftJustDown() const {
    if (DisablePlayerControls || bDisablePlayerCycleWeapon)
        return false;

    return IsLeftShoulder2Pressed();
}

// 0x540640
bool CPad::CycleWeaponRightJustDown() const {
    if (DisablePlayerControls || bDisablePlayerCycleWeapon)
        return false;

    return IsRightShoulder2Pressed();
}

// 0x540670
bool CPad::GetTarget() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
        return NewState.RightShoulder1;
    case 3:
        return NewState.LeftShoulder1;
    }
    return false;
}

// 0x5407A0
bool CPad::GetSprint() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 3:
        return NewState.ButtonCross;
    case 2:
        return NewState.ButtonCircle;
    }
    return false;
}

// 0x5407F0
bool CPad::SprintJustDown() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 3:
        return IsCrossPressed();
    case 2:
        return IsCirclePressed();
    }
    return false;
}

// 0x5408B0
int16 CPad::GetDisplayVitalStats(CPed* ped) const {
    if (DisablePlayerControls || bDisablePlayerDisplayVitalStats)
        return false;

    bool isUsingGun = ped && ped->GetIntelligence()->IsUsingGun();
    return Mode <= 3u && !isUsingGun && NewState.LeftShoulder1 != 0;
}

// 0x540A70
bool CPad::CollectPickupJustDown() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
        return IsLeftShoulder1Pressed();
    case 2:
        return IsTrianglePressed();
    case 3:
        return IsCirclePressed();
    }
    return false;
}

// 0x540AE0
bool CPad::GetForceCameraBehindPlayer() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
        return BUTTON_IS_DOWN(LeftShoulder1);
    case 2:
        return BUTTON_IS_DOWN(ButtonTriangle);
    case 3:
        return BUTTON_IS_DOWN(ButtonCircle);
    }
    return false;
}

// 0x540B30
bool CPad::SniperZoomIn() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 3:
        return BUTTON_IS_DOWN(LeftShoulder2) || BUTTON_IS_DOWN(ButtonSquare);
    case 2:
        return BUTTON_IS_DOWN(ButtonTriangle);
    }
    return false;
}

// 0x540B80
bool CPad::SniperZoomOut() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 3:
        return BUTTON_IS_DOWN(RightShoulder2) || BUTTON_IS_DOWN(ButtonCross);
    case 2:
        return BUTTON_IS_DOWN(ButtonSquare);
    }
    return false;
}

// todo: Describe our modification
// 0x540250
bool CPad::WeaponJustDown(CPed* ped) const {
    if (DisablePlayerControls || bDisablePlayerFireWeapon)
        return false;

    switch (Mode) {
    case 0:
    case 1: {
        if (IsCirclePressed())
            return true;

        if (bDisablePlayerFireWeaponWithL1)
            return false;

        if (ped && (ped->m_pAttachedTo || ped->GetIntelligence()->IsUsingGun())) {
            return IsLeftShoulder1Pressed();
        } else {
            return false;
        }
    }
    case 2:
        return IsCrossPressed();
    case 3:
        return IsRightShoulder1Pressed();
    default:
        return false;
    }
}

// 0x5406B0
bool CPad::GetEnterTargeting() const {
    if (DisablePlayerControls)
        return false;

    switch (Mode) {
    case 0:
    case 1:
    case 2:
        return IsRightShoulder1Pressed();
    case 3:
        return IsLeftShoulder1Pressed();
    }
    return false;
}

// 0x540180
int32 CPad::GetWeapon(CPed* ped) const {
    if (DisablePlayerControls || bDisablePlayerFireWeapon)
        return 0;

    switch (Mode) {
    case 0:
    case 1: {
        if (!bDisablePlayerFireWeaponWithL1) {
            if (ped && (ped->m_pAttachedTo || ped->GetIntelligence()->IsUsingGun())) {
                return NewState.ButtonCircle + NewState.LeftShoulder1;
            }
        }
        return NewState.ButtonCircle;
    }
    case 2:
        return NewState.ButtonCross;
    case 3:
        return NewState.RightShoulder1;
    }
    return 0;
}

// 0x541040
int16 CPad::AimWeaponLeftRight(CPed* ped) const {
    if (DisablePlayerControls)
        return 0;

    if (!CCamera::m_bUseMouse3rdPerson && ped) {
        if (ped->m_pAttachedTo || ped->IsInVehicleAsPassenger()) {
            if (std::fabs(GetRightStickX()) < std::fabs(GetLeftStickX())) {
                return GetLeftStickX();
            }
        }
    }
    return GetRightStickX();
}

// 0x5410C0
int16 CPad::AimWeaponUpDown(CPed* ped) const {
    if (DisablePlayerControls)
        return 0;

    if (!CCamera::m_bUseMouse3rdPerson && ped) {
        if (ped->m_pAttachedTo || ped->IsInVehicleAsPassenger()) {
            if (std::fabs(GetRightStickY()) < std::fabs(GetLeftStickY())) {
                return bInvertLook4Pad ? -GetLeftStickY() : GetLeftStickY();
            }
        }
    }
    return bInvertLook4Pad ? -GetRightStickY() : GetRightStickY();
}

// 0x540BD0
int16 CPad::LookAroundLeftRight(CPed* ped) noexcept {
    if (DisablePlayerControls) {
        return 0;
    }

    auto s1 = NewState.LeftStickX, s2 = NewState.RightStickX;
    if (byte_8CD782) {
        std::swap(s1, s2);
    }

    if (!CCamera::m_bUseMouse3rdPerson && (!byte_B73403 || ped && ped->m_pAttachedTo) && std::abs(s1) < std::abs(s2)) {
        s1 = s2;
    }

    if (std::abs(s1) <= 35) {
        return 0;
    }

    constexpr auto UNK = 1.3763441f;
    return static_cast<int16>(static_cast<int32>((static_cast<double>(s1) + (s1 < 0 ? 35.0 : -35.0)) * static_cast<double>(UNK)));
}

// 0x540CC0
int16 CPad::LookAroundUpDown(CPed* ped) noexcept {
    if (DisablePlayerControls) {
        return 0;
    }

    auto [s1, s2] = [&] {
        if (byte_8CD782) {
            return std::make_pair(NewState.RightStickY, NewState.LeftStickY);
        } else {
            return std::make_pair(NewState.LeftStickY, NewState.RightStickY);
        }
    }();

    if (!CCamera::m_bUseMouse3rdPerson && (!byte_B73403 || ped && ped->m_pAttachedTo) && std::abs(s1) < std::abs(s2)) {
        s1 = s2;
    }

    if (bInvertLook4Pad) {
        s1 = -s1;
    }

    if (std::abs(s1) <= 35) {
        return 0;
    }

    constexpr auto UNK = 1.3763441f;
    return static_cast<int16>(static_cast<int32>((static_cast<double>(s1) + (s1 < 0 ? 35.0 : -35.0)) * static_cast<double>(UNK)));
}

// 0x541290
int32 CPad::sub_541290() {
    if (DisablePlayerControls || bDisablePlayerFireWeapon) {
        AverageWeapon = 0;
        AverageEntries = 1;
        return 0;
    }

    switch (Mode) {
    case 0:
    case 1:
        AverageWeapon = NewState.ButtonCircle;
        AverageEntries = 1;
        return NewState.ButtonCircle;
    case 2:
        AverageWeapon = NewState.ButtonCross;
        AverageEntries = 1;
        return NewState.ButtonCross;
    case 3:
        AverageWeapon = NewState.RightShoulder1;
        AverageEntries = 1;
        return NewState.RightShoulder1;
    default: {
        AverageWeapon = 0;
        AverageEntries = 1;
        return 0;
    }
    }
}

// 0x540A40
bool CPad::sub_540A40() {
    static auto& oldfStickX = StaticRef<int16>(0xB73704); // 0
    auto leftStickX = GetPad()->GetLeftStickX();

    if (!leftStickX && oldfStickX > leftStickX) {
        oldfStickX = leftStickX;
        return true;
    } else {
        oldfStickX = leftStickX;
        return false;
    }
}

// 0x540A10
bool CPad::sub_540A10() {
    static auto& oldfStickX = StaticRef<int16>(0xB73700); // 0
    auto leftStickX = GetPad()->GetLeftStickX();

    if (!leftStickX && oldfStickX < leftStickX) {
        oldfStickX = leftStickX;
        return true;
    } else {
        oldfStickX = leftStickX;
        return false;
    }
}

// 0x540950
bool CPad::GetAnaloguePadUp() {
    static auto& oldfStickY = StaticRef<int16>(0xB736F0); // 0
    auto leftStickY = GetPad()->GetLeftStickY();

    if (leftStickY < -15 && oldfStickY >= -5) {
        oldfStickY = leftStickY;
        return true;
    } else {
        oldfStickY = leftStickY;
        return false;
    }
}

// 0x5409B0
bool CPad::GetAnaloguePadLeft() {
    static auto& oldfStickX = StaticRef<int16>(0xB736F8); // 0
    auto leftStickX = GetPad()->GetLeftStickX();

    if (leftStickX < -15 && oldfStickX >= -5) {
        oldfStickX = leftStickX;
        return true;
    } else {
        oldfStickX = leftStickX;
        return false;
    }
}

// 0x5409E0
bool CPad::GetAnaloguePadRight() {
    static auto& oldfStickX = StaticRef<int16>(0xB736FC); // 0
    auto leftStickX = GetPad()->GetLeftStickX();

    if (leftStickX > 15 && oldfStickX <= 5) {
        oldfStickX = leftStickX;
        return true;
    } else {
        oldfStickX = leftStickX;
        return false;
    }
}

// 0x540980
bool CPad::GetAnaloguePadDown() {
    static auto& oldfStickY = StaticRef<int16>(0xB736F4); // 0
    auto leftStickY = GetPad()->GetLeftStickY();

    if (leftStickY > 15 && oldfStickY <= 5) {
        oldfStickY = leftStickY;
        return true;
    } else {
        oldfStickY = leftStickY;
        return false;
    }
}

// 0x540530
bool CPad::sub_540530() const noexcept {
    switch (Mode) {
    case 0:
    case 2:
    case 3: {
        if (!NewState.Select)
            return sub_5404F0();

        if (OldState.Select == 0)
            return true;

        return sub_5404F0();
    }
    case 1: {
        if (NewState.DPadUp && OldState.DPadUp == 0)
            return true;

        return sub_5404F0();
    }
    default:
        return false;
    }
}

/*!
 * CTRL + M or F7
 */
bool CPad::DebugMenuJustPressed() {
    return (IsCtrlPressed() && IsStandardKeyJustPressed('M')) || IsF7JustPressed();
}

// 0x541490
// EAX sale con la direccion de salida (el EXE la deja asi; la firma declara int), ECX = 0x18 y EDX = 0x3F6 por los contadores de los bucles (0x5415D3..0x5415D9);
// ESI y EDI se restauran. El ultimo flanco ascendente en el orden del EXE gana: teclas estandar 0..254 (la 255 no se mira), F1..F12 y las especiales de la tabla
// (shift antes que rshift, como 0x5419A4). Sin comprobaciones de ejecucion: RTC llamaria a __RTC_CheckEsp y cambiaria ECX/EDX tras la ultima llamada.
#pragma runtime_checks("", off)
int GetCurrentKeyPressed(RsKeyCodes& keys) {
    struct SpecialKey {
        int16 CKeyboardState::* member;
        RsKeyCodes              code;
    };
    // Orden de evaluacion del EXE (0x5415E0..0x541A5A).
    static constexpr SpecialKey specialKeys[] = {
        {&CKeyboardState::esc,       rsESC      }, // 0xB733A8
        {&CKeyboardState::insert,    rsINS      }, // 0xB733AA
        {&CKeyboardState::del,       rsDEL      }, // 0xB733AC
        {&CKeyboardState::home,      rsHOME     }, // 0xB733AE
        {&CKeyboardState::end,       rsEND      }, // 0xB733B0
        {&CKeyboardState::pgup,      rsPGUP     }, // 0xB733B2
        {&CKeyboardState::pgdn,      rsPGDN     }, // 0xB733B4
        {&CKeyboardState::up,        rsUP       }, // 0xB733B6
        {&CKeyboardState::down,      rsDOWN     }, // 0xB733B8
        {&CKeyboardState::left,      rsLEFT     }, // 0xB733BA
        {&CKeyboardState::right,     rsRIGHT    }, // 0xB733BC
        {&CKeyboardState::scroll,    rsSCROLL   }, // 0xB733BE
        {&CKeyboardState::pause,     rsPAUSE    }, // 0xB733C0
        {&CKeyboardState::numlock,   rsNUMLOCK  }, // 0xB733C2
        {&CKeyboardState::div,       rsDIVIDE   }, // 0xB733C4
        {&CKeyboardState::mul,       rsTIMES    }, // 0xB733C6
        {&CKeyboardState::sub,       rsMINUS    }, // 0xB733C8
        {&CKeyboardState::add,       rsPLUS     }, // 0xB733CA
        {&CKeyboardState::enter,     rsPADENTER }, // 0xB733CC
        {&CKeyboardState::decimal,   rsPADDEL   }, // 0xB733CE
        {&CKeyboardState::num1,      rsPADEND   }, // 0xB733D0
        {&CKeyboardState::num2,      rsPADDOWN  }, // 0xB733D2
        {&CKeyboardState::num3,      rsPADPGDN  }, // 0xB733D4
        {&CKeyboardState::num4,      rsPADLEFT  }, // 0xB733D6
        {&CKeyboardState::num5,      rsPAD5     }, // 0xB733D8
        {&CKeyboardState::num6,      rsPADRIGHT }, // 0xB733DA
        {&CKeyboardState::num7,      rsPADHOME  }, // 0xB733DC
        {&CKeyboardState::num8,      rsPADUP    }, // 0xB733DE
        {&CKeyboardState::num9,      rsPADPGUP  }, // 0xB733E0
        {&CKeyboardState::num0,      rsPADINS   }, // 0xB733E2
        {&CKeyboardState::back,      rsBACKSP   }, // 0xB733E4
        {&CKeyboardState::tab,       rsTAB      }, // 0xB733E6
        {&CKeyboardState::capslock,  rsCAPSLK   }, // 0xB733E8
        {&CKeyboardState::extenter,  rsENTER    }, // 0xB733EA
        {&CKeyboardState::lshift,    rsLSHIFT   }, // 0xB733EC
        {&CKeyboardState::shift,     rsSHIFT    }, // 0xB733F0
        {&CKeyboardState::rshift,    rsRSHIFT   }, // 0xB733EE
        {&CKeyboardState::lctrl,     rsLCTRL    }, // 0xB733F2
        {&CKeyboardState::rctrl,     rsRCTRL    }, // 0xB733F4
        {&CKeyboardState::lalt,      rsLALT     }, // 0xB733F6
        {&CKeyboardState::ralt,      rsRALT     }, // 0xB733F8
        {&CKeyboardState::lwin,      rsLWIN     }, // 0xB733FA
        {&CKeyboardState::rwin,      rsRWIN     }, // 0xB733FC
        {&CKeyboardState::apps,      rsAPPS     }, // 0xB733FE
    };
    const auto& newState = CPad::NewKeyState;
    const auto& oldState = CPad::OldKeyState;
    const auto  rising   = [](int16 now, int16 before) { return now != 0 && before == 0; };

    keys = rsNULL;
    for (int32 i = 0; i < 255; i++) {
        if (rising(newState.standardKeys[i], oldState.standardKeys[i])) {
            keys = static_cast<RsKeyCodes>(i);
        }
    }
    for (int32 i = 0; i < 12; i++) {
        if (rising(newState.FKeys[i], oldState.FKeys[i])) {
            keys = static_cast<RsKeyCodes>(rsF1 + i);
        }
    }
    for (const auto& special : specialKeys) {
        if (rising(newState.*special.member, oldState.*special.member)) {
            keys = special.code;
        }
    }
    return static_cast<int>(ResidueRegsCpp(0x18, 0x3F6, reinterpret_cast<uintptr_t>(&keys)));
}
#pragma runtime_checks("", restore)

// 0x746F70
// Como el original: sin dispositivo no hace nada; si lo hay, Unacquire (vtbl+0x20), vuelve a leer PSGLOBAL(diMouse) y llama Release (vtbl+8), vuelve a leer y lo deja a nulo.
// El EXE deja en EAX el retorno de Release (o 0), pero ningun llamador (0x576CCE, 0x576EBC, 0x576F8C) lo lee.
void DIReleaseMouse() { // todo: wininput
    if (!PSGLOBAL(diMouse)) {
        return;
    }
    PSGLOBAL(diMouse)->Unacquire();
    PSGLOBAL(diMouse)->Release();
    PSGLOBAL(diMouse) = nullptr;
}

void InitialiseMouse(bool exclusive) {
    WinInput::diMouseInit(exclusive);
}

// 0x5404A0: tabla de saltos 5404DC = B3, C1, B3, B3 (modos 0, 2 y 3: Select; modo 1: D-pad arriba); Mode se lee sin signo (ja) y fuera de 0..3 devuelve solo AL = 0.
bool CPad::GetCameraCycleUp() const {
    switch (static_cast<uint16>(Mode)) {
    case 0:
    case 2:
    case 3:
        return NewState.Select != 0 && OldState.Select == 0;
    case 1:
        return NewState.DPadUp != 0 && OldState.DPadUp == 0;
    default:
        return false;
    }
}

// 0x540E80: el eje es el del pad 0 (0xB7345C = Pads[0].NewState.RightStickX) y el EXE lo pasa por x87 (fild, fcomp, fsub/fadd, fmul 0.5f, _ftol2). Todos los valores
// (int16, +-100, +-50, mitades) son exactos en float de 24 bits, de modo que los resultados enteros coinciden y no se levanta ninguna excepcion x87;
// _ftol2 trunca hacia cero igual que la division entera de C++.
int16 CPad::GetMouseCameraStickHorizontal() const {
    const int32 stick = Pads[0].NewState.RightStickX;
    const int32 magnitude = stick < 0 ? -stick : stick;
    // 0x540E9E..0x540ECA: por encima de 100 solo se evita este camino con los controles activos y ShockButtonR pulsado.
    if (magnitude > 100 && !(DisablePlayerControls == 0 && NewState.ShockButtonR != 0)) {
        return static_cast<int16>(stick > 0 ? stick - 50 : stick + 50);
    }
    // 0x540F07: la comprobacion de la camara va antes que la zona muerta de 50.
    if (!StaticRef<CCam>(0xB6F19C).Using3rdPersonMouseCam() || magnitude <= 50) {
        return 0;
    }
    return static_cast<int16>(stick > 0 ? (stick - 50) / 2 : (stick + 50) / 2);
}

// 0x540F80: igual con Pads[0].NewState.RightStickY (0xB7345E) y la inversion de bInvertLook4Pad (0xB73402): neg sobre 32 bits y se conserva el int16 (di), de modo que -32768 queda en -32768.
int16 CPad::GetMouseCameraStickVertical() const {
    int16 stick = Pads[0].NewState.RightStickY;
    if (bInvertLook4Pad) {
        stick = static_cast<int16>(-static_cast<int32>(stick));
    }
    const int32 value = stick;
    const int32 magnitude = value < 0 ? -value : value;
    if (magnitude > 100 && !(DisablePlayerControls == 0 && NewState.ShockButtonR != 0)) {
        return static_cast<int16>(value > 0 ? value - 50 : value + 50);
    }
    if (!StaticRef<CCam>(0xB6F19C).Using3rdPersonMouseCam() || magnitude <= 50) {
        return 0;
    }
    return static_cast<int16>(value > 0 ? (value - 50) / 2 : (value + 50) / 2);
}
