/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPad
 * Reconstrucción: COMPLETA — 75/75 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 75/75 entradas.
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
float NormalizePadAxis(LONG value) {
    const float scale = 0.0005F;
    float result;
    __asm {
        fild value
        fmul scale
        fstp result
    }
    return result;
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
            // Como el original (0x746B32): fild con signo y, si es negativo, fadd de 2^32 (float) con la precision x87 del juego.
            const LONG povSigned = static_cast<LONG>(joyState.rgdwPOV[0]);
            const float twoPow32 = 4294967296.0F;
            const float hundredth = 0.01F, degreesToRadians = 0.01745329238474369F;
            float horizontal, vertical;
            __asm {
                fild povSigned
                cmp povSigned, 0
                jge povNonNegative
                fadd twoPow32
            povNonNegative:
                fmul hundredth
                fmul degreesToRadians
                fld st(0)
                fsin
                fstp horizontal
                fcos
                fchs
                fstp vertical
            }
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

// x87 ST(0) input, EDX:EAX truncated integer output; native 0x821B40 arithmetic.
// Called only by the shake arithmetic block; no delegate to the original EXE.
static __declspec(naked) int64 __cdecl PadShakeFloatToInteger() {
    __asm {
        push ebp
        mov ebp, esp
        sub esp, 20h
        and esp, 0FFFFFFF0h
        fld st(0)
        fst dword ptr [esp + 18h]
        fistp qword ptr [esp + 10h]
        fild qword ptr [esp + 10h]
        mov edx, dword ptr [esp + 18h]
        mov eax, dword ptr [esp + 10h]
        test eax, eax
        je shake_ftol_zero
        shake_ftol_adjust:
        fsubp st(1), st(0)
        test edx, edx
        jns shake_ftol_positive
        fstp dword ptr [esp]
        mov ecx, dword ptr [esp]
        xor ecx, 80000000h
        add ecx, 7FFFFFFFh
        adc eax, 0
        mov edx, dword ptr [esp + 14h]
        adc edx, 0
        jmp shake_ftol_done
        shake_ftol_positive:
        fstp dword ptr [esp]
        mov ecx, dword ptr [esp]
        add ecx, 7FFFFFFFh
        sbb eax, 0
        mov edx, dword ptr [esp + 14h]
        sbb edx, 0
        jmp shake_ftol_done
        shake_ftol_zero:
        mov edx, dword ptr [esp + 14h]
        test edx, 7FFFFFFFh
        jne shake_ftol_adjust
        fstp dword ptr [esp + 18h]
        fstp dword ptr [esp + 18h]
        shake_ftol_done:
        leave
        ret
    }
}

// originally 3x float instead of CVector
// 0x53F9A0
void CPad::StartShake_Distance(int16 time, uint8 freq, CVector pos) {
    if (!FrontEndMenuManager.m_PrefsUseVibration || CCutsceneMgr::ms_running)
        return;

    const auto& camera = TheCamera.GetPosition();
    const float cameraX = camera.x, cameraY = camera.y, cameraZ = camera.z;
    const float pointX = pos.x, pointY = pos.y, pointZ = pos.z, limit = 70.0f;
    bool withinDistance;
    // Preserve native z^2 + y^2 + x^2 order and x87 exception effects.
    __asm {
        fld cameraZ
        fsub pointZ
        fld cameraZ
        fsub pointZ
        fmulp st(1), st(0)
        fld cameraY
        fsub pointY
        fld cameraY
        fsub pointY
        fmulp st(1), st(0)
        faddp st(1), st(0)
        fld cameraX
        fsub pointX
        fld cameraX
        fsub pointX
        fmulp st(1), st(0)
        faddp st(1), st(0)
        fsqrt
        fcomp limit
        fnstsw ax
        test ah, 5
        setnp withinDistance
    }
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
    const float cameraX = camera.x, cameraY = camera.y;
    const float pointX = point.x, pointY = point.y, squaredLimit = 4900.0f;
    const float seventy = 70.0f, thirty = 30.0f;
    float distanceSquared;
    bool withinDistance;
    // Preserve native 2D y^2 + x^2, its float spill and x87 comparison.
    __asm {
        fld cameraY
        fsub pointY
        fld cameraY
        fsub pointY
        fmulp st(1), st(0)
        fld cameraX
        fsub pointX
        fld cameraX
        fsub pointX
        fmulp st(1), st(0)
        faddp st(1), st(0)
        fst distanceSquared
        fcomp squaredLimit
        fnstsw ax
        test ah, 5
        setnp withinDistance
    }
    if (withinDistance && ShakeDur < 100) {
        ShakeDur = 100;
        uint8 frequency;
        __asm {
            fld distanceSquared
            fsqrt
            fsubr seventy
            fadd thirty
            call PadShakeFloatToInteger
            mov frequency, al
        }
        ShakeFreq = frequency;
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
// Native EAX returns the output address; last rising key in EXE priority wins.
__declspec(naked) int GetCurrentKeyPressed(RsKeyCodes&) {
    __asm {
    mov eax,dword ptr [esp+4] // EXE 0x00541490
    push esi // EXE 0x00541494
    mov dword ptr [eax],420h // EXE 0x00541495
    mov edx,2 // EXE 0x0054149B
    xor ecx,ecx // EXE 0x005414A0
    push edi // EXE 0x005414A2
    L005414A3: cmp word ptr [ecx+00B731A8h],0
    je L005414BC // EXE 0x005414AB
    cmp word ptr [ecx+00B72F38h],0 // EXE 0x005414AD
    jne L005414BC // EXE 0x005414B5
    lea esi,[edx-2] // EXE 0x005414B7
    mov dword ptr [eax],esi // EXE 0x005414BA
    L005414BC: cmp word ptr [ecx+00B731AAh],0
    je L005414D5 // EXE 0x005414C4
    cmp word ptr [ecx+00B72F3Ah],0 // EXE 0x005414C6
    jne L005414D5 // EXE 0x005414CE
    lea esi,[edx-1] // EXE 0x005414D0
    mov dword ptr [eax],esi // EXE 0x005414D3
    L005414D5: cmp word ptr [ecx+00B731ACh],0
    je L005414EB // EXE 0x005414DD
    cmp word ptr [ecx+00B72F3Ch],0 // EXE 0x005414DF
    jne L005414EB // EXE 0x005414E7
    mov dword ptr [eax],edx // EXE 0x005414E9
    L005414EB: cmp word ptr [ecx+00B731AEh],0
    je L00541504 // EXE 0x005414F3
    cmp word ptr [ecx+00B72F3Eh],0 // EXE 0x005414F5
    jne L00541504 // EXE 0x005414FD
    lea esi,[edx+1] // EXE 0x005414FF
    mov dword ptr [eax],esi // EXE 0x00541502
    L00541504: cmp word ptr [ecx+00B731B0h],0
    je L0054151D // EXE 0x0054150C
    cmp word ptr [ecx+00B72F40h],0 // EXE 0x0054150E
    jne L0054151D // EXE 0x00541516
    lea esi,[edx+2] // EXE 0x00541518
    mov dword ptr [eax],esi // EXE 0x0054151B
    L0054151D: add edx,5
    lea esi,[edx-2] // EXE 0x00541520
    add ecx,0Ah // EXE 0x00541523
    cmp esi,0FFh // EXE 0x00541526
    jl L005414A3 // EXE 0x0054152C
    mov edx,3EAh // EXE 0x00541532
    xor ecx,ecx // EXE 0x00541537
    mov esi,2 // EXE 0x00541539
    mov edi,edi // EXE 0x0054153E
    L00541540: cmp word ptr [ecx+00B73190h],0
    je L00541559 // EXE 0x00541548
    cmp word ptr [ecx+00B72F20h],0 // EXE 0x0054154A
    jne L00541559 // EXE 0x00541552
    lea edi,[edx-1] // EXE 0x00541554
    mov dword ptr [eax],edi // EXE 0x00541557
    L00541559: cmp word ptr [ecx+00B73192h],0
    je L0054156F // EXE 0x00541561
    cmp word ptr [ecx+00B72F22h],0 // EXE 0x00541563
    jne L0054156F // EXE 0x0054156B
    mov dword ptr [eax],edx // EXE 0x0054156D
    L0054156F: cmp word ptr [ecx+00B73194h],0
    je L00541588 // EXE 0x00541577
    cmp word ptr [ecx+00B72F24h],0 // EXE 0x00541579
    jne L00541588 // EXE 0x00541581
    lea edi,[edx+1] // EXE 0x00541583
    mov dword ptr [eax],edi // EXE 0x00541586
    L00541588: cmp word ptr [ecx+00B73196h],0
    je L005415A1 // EXE 0x00541590
    cmp word ptr [ecx+00B72F26h],0 // EXE 0x00541592
    jne L005415A1 // EXE 0x0054159A
    lea edi,[edx+2] // EXE 0x0054159C
    mov dword ptr [eax],edi // EXE 0x0054159F
    L005415A1: cmp word ptr [ecx+00B73198h],0
    je L005415BA // EXE 0x005415A9
    cmp word ptr [ecx+00B72F28h],0 // EXE 0x005415AB
    jne L005415BA // EXE 0x005415B3
    lea edi,[edx+3] // EXE 0x005415B5
    mov dword ptr [eax],edi // EXE 0x005415B8
    L005415BA: cmp word ptr [ecx+00B7319Ah],0
    je L005415D3 // EXE 0x005415C2
    cmp word ptr [ecx+00B72F2Ah],0 // EXE 0x005415C4
    jne L005415D3 // EXE 0x005415CC
    lea edi,[edx+4] // EXE 0x005415CE
    mov dword ptr [eax],edi // EXE 0x005415D1
    L005415D3: add edx,6
    add ecx,0Ch // EXE 0x005415D6
    dec esi // EXE 0x005415D9
    jne L00541540 // EXE 0x005415DA
    cmp word ptr ds:[00B733A8h],0 // EXE 0x005415E0
    pop edi // EXE 0x005415E8
    pop esi // EXE 0x005415E9
    je L005415FC // EXE 0x005415EA
    cmp word ptr ds:[00B73138h],0 // EXE 0x005415EC
    jne L005415FC // EXE 0x005415F4
    mov dword ptr [eax],3E8h // EXE 0x005415F6
    L005415FC: cmp word ptr ds:[00B733AAh],0
    je L00541616 // EXE 0x00541604
    cmp word ptr ds:[00B7313Ah],0 // EXE 0x00541606
    jne L00541616 // EXE 0x0054160E
    mov dword ptr [eax],3F5h // EXE 0x00541610
    L00541616: cmp word ptr ds:[00B733ACh],0
    je L00541630 // EXE 0x0054161E
    cmp word ptr ds:[00B7313Ch],0 // EXE 0x00541620
    jne L00541630 // EXE 0x00541628
    mov dword ptr [eax],3F6h // EXE 0x0054162A
    L00541630: cmp word ptr ds:[00B733AEh],0
    je L0054164A // EXE 0x00541638
    cmp word ptr ds:[00B7313Eh],0 // EXE 0x0054163A
    jne L0054164A // EXE 0x00541642
    mov dword ptr [eax],3F7h // EXE 0x00541644
    L0054164A: cmp word ptr ds:[00B733B0h],0
    je L00541664 // EXE 0x00541652
    cmp word ptr ds:[00B73140h],0 // EXE 0x00541654
    jne L00541664 // EXE 0x0054165C
    mov dword ptr [eax],3F8h // EXE 0x0054165E
    L00541664: cmp word ptr ds:[00B733B2h],0
    je L0054167E // EXE 0x0054166C
    cmp word ptr ds:[00B73142h],0 // EXE 0x0054166E
    jne L0054167E // EXE 0x00541676
    mov dword ptr [eax],3F9h // EXE 0x00541678
    L0054167E: cmp word ptr ds:[00B733B4h],0
    je L00541698 // EXE 0x00541686
    cmp word ptr ds:[00B73144h],0 // EXE 0x00541688
    jne L00541698 // EXE 0x00541690
    mov dword ptr [eax],3FAh // EXE 0x00541692
    L00541698: cmp word ptr ds:[00B733B6h],0
    je L005416B2 // EXE 0x005416A0
    cmp word ptr ds:[00B73146h],0 // EXE 0x005416A2
    jne L005416B2 // EXE 0x005416AA
    mov dword ptr [eax],3FBh // EXE 0x005416AC
    L005416B2: cmp word ptr ds:[00B733B8h],0
    je L005416CC // EXE 0x005416BA
    cmp word ptr ds:[00B73148h],0 // EXE 0x005416BC
    jne L005416CC // EXE 0x005416C4
    mov dword ptr [eax],3FCh // EXE 0x005416C6
    L005416CC: cmp word ptr ds:[00B733BAh],0
    je L005416E6 // EXE 0x005416D4
    cmp word ptr ds:[00B7314Ah],0 // EXE 0x005416D6
    jne L005416E6 // EXE 0x005416DE
    mov dword ptr [eax],3FDh // EXE 0x005416E0
    L005416E6: cmp word ptr ds:[00B733BCh],0
    je L00541700 // EXE 0x005416EE
    cmp word ptr ds:[00B7314Ch],0 // EXE 0x005416F0
    jne L00541700 // EXE 0x005416F8
    mov dword ptr [eax],3FEh // EXE 0x005416FA
    L00541700: cmp word ptr ds:[00B733BEh],0
    je L0054171A // EXE 0x00541708
    cmp word ptr ds:[00B7314Eh],0 // EXE 0x0054170A
    jne L0054171A // EXE 0x00541712
    mov dword ptr [eax],410h // EXE 0x00541714
    L0054171A: cmp word ptr ds:[00B733C0h],0
    je L00541734 // EXE 0x00541722
    cmp word ptr ds:[00B73150h],0 // EXE 0x00541724
    jne L00541734 // EXE 0x0054172C
    mov dword ptr [eax],411h // EXE 0x0054172E
    L00541734: cmp word ptr ds:[00B733C2h],0
    je L0054174E // EXE 0x0054173C
    cmp word ptr ds:[00B73152h],0 // EXE 0x0054173E
    jne L0054174E // EXE 0x00541746
    mov dword ptr [eax],409h // EXE 0x00541748
    L0054174E: cmp word ptr ds:[00B733C4h],0
    je L00541768 // EXE 0x00541756
    cmp word ptr ds:[00B73154h],0 // EXE 0x00541758
    jne L00541768 // EXE 0x00541760
    mov dword ptr [eax],3FFh // EXE 0x00541762
    L00541768: cmp word ptr ds:[00B733C6h],0
    je L00541782 // EXE 0x00541770
    cmp word ptr ds:[00B73156h],0 // EXE 0x00541772
    jne L00541782 // EXE 0x0054177A
    mov dword ptr [eax],400h // EXE 0x0054177C
    L00541782: cmp word ptr ds:[00B733C8h],0
    je L0054179C // EXE 0x0054178A
    cmp word ptr ds:[00B73158h],0 // EXE 0x0054178C
    jne L0054179C // EXE 0x00541794
    mov dword ptr [eax],402h // EXE 0x00541796
    L0054179C: cmp word ptr ds:[00B733CAh],0
    je L005417B6 // EXE 0x005417A4
    cmp word ptr ds:[00B7315Ah],0 // EXE 0x005417A6
    jne L005417B6 // EXE 0x005417AE
    mov dword ptr [eax],401h // EXE 0x005417B0
    L005417B6: cmp word ptr ds:[00B733CCh],0
    je L005417D0 // EXE 0x005417BE
    cmp word ptr ds:[00B7315Ch],0 // EXE 0x005417C0
    jne L005417D0 // EXE 0x005417C8
    mov dword ptr [eax],40Fh // EXE 0x005417CA
    L005417D0: cmp word ptr ds:[00B733CEh],0
    je L005417EA // EXE 0x005417D8
    cmp word ptr ds:[00B7315Eh],0 // EXE 0x005417DA
    jne L005417EA // EXE 0x005417E2
    mov dword ptr [eax],403h // EXE 0x005417E4
    L005417EA: cmp word ptr ds:[00B733D0h],0
    je L00541804 // EXE 0x005417F2
    cmp word ptr ds:[00B73160h],0 // EXE 0x005417F4
    jne L00541804 // EXE 0x005417FC
    mov dword ptr [eax],404h // EXE 0x005417FE
    L00541804: cmp word ptr ds:[00B733D2h],0
    je L0054181E // EXE 0x0054180C
    cmp word ptr ds:[00B73162h],0 // EXE 0x0054180E
    jne L0054181E // EXE 0x00541816
    mov dword ptr [eax],405h // EXE 0x00541818
    L0054181E: cmp word ptr ds:[00B733D4h],0
    je L00541838 // EXE 0x00541826
    cmp word ptr ds:[00B73164h],0 // EXE 0x00541828
    jne L00541838 // EXE 0x00541830
    mov dword ptr [eax],406h // EXE 0x00541832
    L00541838: cmp word ptr ds:[00B733D6h],0
    je L00541852 // EXE 0x00541840
    cmp word ptr ds:[00B73166h],0 // EXE 0x00541842
    jne L00541852 // EXE 0x0054184A
    mov dword ptr [eax],407h // EXE 0x0054184C
    L00541852: cmp word ptr ds:[00B733D8h],0
    je L0054186C // EXE 0x0054185A
    cmp word ptr ds:[00B73168h],0 // EXE 0x0054185C
    jne L0054186C // EXE 0x00541864
    mov dword ptr [eax],408h // EXE 0x00541866
    L0054186C: cmp word ptr ds:[00B733DAh],0
    je L00541886 // EXE 0x00541874
    cmp word ptr ds:[00B7316Ah],0 // EXE 0x00541876
    jne L00541886 // EXE 0x0054187E
    mov dword ptr [eax],40Ah // EXE 0x00541880
    L00541886: cmp word ptr ds:[00B733DCh],0
    je L005418A0 // EXE 0x0054188E
    cmp word ptr ds:[00B7316Ch],0 // EXE 0x00541890
    jne L005418A0 // EXE 0x00541898
    mov dword ptr [eax],40Bh // EXE 0x0054189A
    L005418A0: cmp word ptr ds:[00B733DEh],0
    je L005418BA // EXE 0x005418A8
    cmp word ptr ds:[00B7316Eh],0 // EXE 0x005418AA
    jne L005418BA // EXE 0x005418B2
    mov dword ptr [eax],40Ch // EXE 0x005418B4
    L005418BA: cmp word ptr ds:[00B733E0h],0
    je L005418D4 // EXE 0x005418C2
    cmp word ptr ds:[00B73170h],0 // EXE 0x005418C4
    jne L005418D4 // EXE 0x005418CC
    mov dword ptr [eax],40Dh // EXE 0x005418CE
    L005418D4: cmp word ptr ds:[00B733E2h],0
    je L005418EE // EXE 0x005418DC
    cmp word ptr ds:[00B73172h],0 // EXE 0x005418DE
    jne L005418EE // EXE 0x005418E6
    mov dword ptr [eax],40Eh // EXE 0x005418E8
    L005418EE: cmp word ptr ds:[00B733E4h],0
    je L00541908 // EXE 0x005418F6
    cmp word ptr ds:[00B73174h],0 // EXE 0x005418F8
    jne L00541908 // EXE 0x00541900
    mov dword ptr [eax],412h // EXE 0x00541902
    L00541908: cmp word ptr ds:[00B733E6h],0
    je L00541922 // EXE 0x00541910
    cmp word ptr ds:[00B73176h],0 // EXE 0x00541912
    jne L00541922 // EXE 0x0054191A
    mov dword ptr [eax],413h // EXE 0x0054191C
    L00541922: cmp word ptr ds:[00B733E8h],0
    je L0054193C // EXE 0x0054192A
    cmp word ptr ds:[00B73178h],0 // EXE 0x0054192C
    jne L0054193C // EXE 0x00541934
    mov dword ptr [eax],414h // EXE 0x00541936
    L0054193C: cmp word ptr ds:[00B733EAh],0
    je L00541956 // EXE 0x00541944
    cmp word ptr ds:[00B7317Ah],0 // EXE 0x00541946
    jne L00541956 // EXE 0x0054194E
    mov dword ptr [eax],415h // EXE 0x00541950
    L00541956: cmp word ptr ds:[00B733ECh],0
    je L00541970 // EXE 0x0054195E
    cmp word ptr ds:[00B7317Ch],0 // EXE 0x00541960
    jne L00541970 // EXE 0x00541968
    mov dword ptr [eax],416h // EXE 0x0054196A
    L00541970: cmp word ptr ds:[00B733F0h],0
    je L0054198A // EXE 0x00541978
    cmp word ptr ds:[00B73180h],0 // EXE 0x0054197A
    jne L0054198A // EXE 0x00541982
    mov dword ptr [eax],418h // EXE 0x00541984
    L0054198A: cmp word ptr ds:[00B733EEh],0
    je L005419A4 // EXE 0x00541992
    cmp word ptr ds:[00B7317Eh],0 // EXE 0x00541994
    jne L005419A4 // EXE 0x0054199C
    mov dword ptr [eax],417h // EXE 0x0054199E
    L005419A4: cmp word ptr ds:[00B733F2h],0
    je L005419BE // EXE 0x005419AC
    cmp word ptr ds:[00B73182h],0 // EXE 0x005419AE
    jne L005419BE // EXE 0x005419B6
    mov dword ptr [eax],419h // EXE 0x005419B8
    L005419BE: cmp word ptr ds:[00B733F4h],0
    je L005419D8 // EXE 0x005419C6
    cmp word ptr ds:[00B73184h],0 // EXE 0x005419C8
    jne L005419D8 // EXE 0x005419D0
    mov dword ptr [eax],41Ah // EXE 0x005419D2
    L005419D8: cmp word ptr ds:[00B733F6h],0
    je L005419F2 // EXE 0x005419E0
    cmp word ptr ds:[00B73186h],0 // EXE 0x005419E2
    jne L005419F2 // EXE 0x005419EA
    mov dword ptr [eax],41Bh // EXE 0x005419EC
    L005419F2: cmp word ptr ds:[00B733F8h],0
    je L00541A0C // EXE 0x005419FA
    cmp word ptr ds:[00B73188h],0 // EXE 0x005419FC
    jne L00541A0C // EXE 0x00541A04
    mov dword ptr [eax],41Ch // EXE 0x00541A06
    L00541A0C: cmp word ptr ds:[00B733FAh],0
    je L00541A26 // EXE 0x00541A14
    cmp word ptr ds:[00B7318Ah],0 // EXE 0x00541A16
    jne L00541A26 // EXE 0x00541A1E
    mov dword ptr [eax],41Dh // EXE 0x00541A20
    L00541A26: cmp word ptr ds:[00B733FCh],0
    je L00541A40 // EXE 0x00541A2E
    cmp word ptr ds:[00B7318Ch],0 // EXE 0x00541A30
    jne L00541A40 // EXE 0x00541A38
    mov dword ptr [eax],41Eh // EXE 0x00541A3A
    L00541A40: cmp word ptr ds:[00B733FEh],0
    je L00541A5A // EXE 0x00541A48
    cmp word ptr ds:[00B7318Eh],0 // EXE 0x00541A4A
    jne L00541A5A // EXE 0x00541A52
    mov dword ptr [eax],41Fh // EXE 0x00541A54
    L00541A5A: ret
    }
}

IDirectInputDevice8* DIReleaseMouse() { // todo: wininput
    return plugin::CallAndReturn<IDirectInputDevice8*, 0x746F70>();
}

void InitialiseMouse(bool exclusive) {
    WinInput::diMouseInit(exclusive);
}

static uintptr_t MouseCameraModeBody=std::bit_cast<uintptr_t>(&CCam::Using3rdPersonMouseCam);
static uintptr_t MouseCameraFtolLibrary=0x821B40;

// 0x5404A0: native table 5404DC = B3,C1,B3,B3; all candidate targets local.
__declspec(naked) bool CPad::GetCameraCycleUp() const {
    __asm {
        movzx eax,word ptr [ecx+10Ah]
        cmp eax,3
        ja invalidMode
        cmp eax,1
        je alternate
        cmp word ptr [ecx+1Ah],0
        je released
        cmp word ptr [ecx+4Ah],0
        jmp previous
    alternate:
        cmp word ptr [ecx+10h],0
        je released
        cmp word ptr [ecx+40h],0
    previous:
        jne released
        mov eax,1
        ret
    released:
        xor eax,eax
        ret
    invalidMode:
        xor al,al
        ret
    }
}

// 0x540e80: exact native x87/integer branches, actual CRT tail call.
__declspec(naked) int16 CPad::GetMouseCameraStickHorizontal() const {
    __asm {
    push ecx
    movsx eax,word ptr ds:[00B7345Ch]
    mov dword ptr [esp],eax
    fild dword ptr [esp]
    fst dword ptr [esp]
    fcomp dword ptr ds:[00858B50h]
    fld dword ptr [esp]
    fnstsw ax
    test ah,5
    jp L00540EA7
    fchs
    L00540EA7: fcomp dword ptr ds:[00858628h]
    fnstsw ax
    test ah,41h
    jne L00540F07
    cmp word ptr [ecx+0000010Eh],0
    jne L00540ECA
    cmp word ptr [ecx+26h],0
    setne al
    test al,al
    jne L00540F07
    L00540ECA: fld dword ptr [esp]
    fcomp dword ptr ds:[00858B50h]
    fld dword ptr [esp]
    fnstsw ax
    test ah,41h
    jne L00540EF3
    fsub dword ptr ds:[00858628h]
    fadd dword ptr ds:[00858B40h]
    add esp,4
    jmp dword ptr [MouseCameraFtolLibrary]
    L00540EF3: fadd dword ptr ds:[00858628h]
    fsub dword ptr ds:[00858B40h]
    add esp,4
    jmp dword ptr [MouseCameraFtolLibrary]
    L00540F07: mov ecx,0B6F19Ch
    call dword ptr [MouseCameraModeBody]
    test al,al
    je L00540F76
    fld dword ptr [esp]
    fcomp dword ptr ds:[00858B50h]
    fld dword ptr [esp]
    fnstsw ax
    test ah,5
    jp L00540F2C
    fchs
    L00540F2C: fcomp dword ptr ds:[00858B40h]
    fnstsw ax
    test ah,41h
    jne L00540F76
    fld dword ptr [esp]
    fcomp dword ptr ds:[00858B50h]
    fld dword ptr [esp]
    fnstsw ax
    test ah,41h
    jne L00540F62
    fsub dword ptr ds:[00858B40h]
    fmul dword ptr ds:[00858B8Ch]
    add esp,4
    jmp dword ptr [MouseCameraFtolLibrary]
    L00540F62: fadd dword ptr ds:[00858B40h]
    fmul dword ptr ds:[00858B8Ch]
    add esp,4
    jmp dword ptr [MouseCameraFtolLibrary]
    L00540F76: xor ax,ax
    pop ecx
    ret
    }
}

// 0x540f80: exact native x87/integer branches, actual CRT tail call.
__declspec(naked) int16 CPad::GetMouseCameraStickVertical() const {
    __asm {
    push ecx
    mov al,byte ptr ds:[00B73402h]
    test al,al
    push esi
    push edi
    mov di,word ptr ds:[00B7345Eh]
    mov word ptr [esp+8],di
    je L00540FA5
    mov eax,dword ptr [esp+8]
    neg eax
    mov dword ptr [esp+8],eax
    mov di,ax
    L00540FA5: test di,di
    movsx esi,di
    mov eax,esi
    jge L00540FB1
    neg eax
    L00540FB1: cmp eax,64h
    jle L00540FDF
    cmp word ptr [ecx+0000010Eh],0
    jne L00540FCC
    cmp word ptr [ecx+26h],0
    setne al
    test al,al
    jne L00540FDF
    L00540FCC: test di,di
    jle L00540FD8
    pop edi
    lea eax,[esi-32h]
    pop esi
    pop ecx
    ret
    L00540FD8: pop edi
    lea eax,[esi+32h]
    pop esi
    pop ecx
    ret
    L00540FDF: mov ecx,0B6F19Ch
    call dword ptr [MouseCameraModeBody]
    test al,al
    je L00541035
    test di,di
    mov eax,esi
    jge L00540FF6
    neg eax
    L00540FF6: cmp eax,32h
    jle L00541035
    test di,di
    pop edi
    jle L0054101B
    add esi,0FFFFFFCEh
    mov dword ptr [esp+4],esi
    fild dword ptr [esp+4]
    pop esi
    fmul dword ptr ds:[00858B8Ch]
    add esp,4
    jmp dword ptr [MouseCameraFtolLibrary]
    L0054101B: add esi,32h
    mov dword ptr [esp+4],esi
    fild dword ptr [esp+4]
    pop esi
    fmul dword ptr ds:[00858B8Ch]
    add esp,4
    jmp dword ptr [MouseCameraFtolLibrary]
    L00541035: pop edi
    xor ax,ax
    pop esi
    pop ecx
    ret
    }
}
