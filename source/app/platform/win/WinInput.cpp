/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Input
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"


#include "WinInput.h"
#include "ControllerConfigManager.h"
#include "WinPlatform.h"
#include "VideoMode.h"
#include <intrin.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace WinInput {
// 0x746990
HRESULT CreateInput() {
    if (PSGLOBAL(diInterface)) {
        return S_OK; // Already created
    }

    if (SUCCEEDED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&PSGLOBAL(diInterface), nullptr))) {
        return S_OK;
    }

    return S_FALSE; 
}

// 0x7487CF
bool Initialise() {
    ControlsManager.MakeControllerActionsBlank();
    ControlsManager.InitDefaultControlConfiguration();

    if (FAILED(CreateInput())) {
        return false;
    }

    diMouseInit(false);
    diPadInit(); // Keep native device enumeration for both startup paths.

    return true;
}

HRESULT ReleaseInput() {
    if (const auto ifc = PSGLOBAL(diInterface)) {
        return ifc->Release();
    }
    return S_OK; // Nothing to release
}

HRESULT Shutdown() {
    return ReleaseInput();
}

// 0x746D80
HRESULT diPadSetRanges(LPDIRECTINPUTDEVICE8 dev, DWORD padNum) {
    if (dev == NULL) {
        return S_OK; // Weird but okay
    }

    enum {
        NO_PROPERTY,    // Device doesn't have this property
        PROP_READ_ONLY, // Deivce does have this property, but it's read-only
        SUCCESS         // Device has property and we've set it successfully
    };

    const auto SetPropery = [&](DWORD prop) {
        // Set ranges
        DIDEVICEOBJECTINSTANCEA objinfo{
            sizeof(DIDEVICEOBJECTINSTANCEA)
        };
        DIPROPRANGE range{
            .diph = DIPROPHEADER{
                .dwSize       = sizeof(DIPROPRANGE),
                .dwHeaderSize = sizeof(DIPROPHEADER),
                .dwObj        = prop,
                .dwHow        = DIPH_BYOFFSET,
            },
            .lMin = -2000,
            .lMax = 2000,
        };
        if (FAILED(dev->GetObjectInfo(&objinfo, prop, DIPH_BYOFFSET))) {
            return NO_PROPERTY; 
        }
        if (FAILED(dev->SetProperty(DIPROP_RANGE, &range.diph))) {
            return PROP_READ_ONLY; 
        }
        return SUCCESS; 
    };

    if (SetPropery(DIJOFS_X) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    if (SetPropery(DIJOFS_Y) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    const auto SetProperyAndSetFlag = [&](DWORD prop, bool& flag) {
        const auto res = SetPropery(DIJOFS_Z);
        if (res != NO_PROPERTY) {
            flag = true;
        }
        return res;
    };

    if (SetProperyAndSetFlag(DIJOFS_Z, AllValidWinJoys.JoyStickNum[padNum].bZAxisPresent) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    if (SetProperyAndSetFlag(DIJOFS_RZ, AllValidWinJoys.JoyStickNum[padNum].bZRotPresent) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    return S_OK;
}

// 0x7469A0
void diMouseInit(bool exclusive) {
    WIN_FCHECK(PSGLOBAL(diInterface)->CreateDevice(GUID_SysMouse, &PSGLOBAL(diMouse), 0));
    WIN_FCHECK(PSGLOBAL(diMouse)->SetDataFormat(&c_dfDIMouse2));
    WIN_FCHECK(PSGLOBAL(diMouse)->SetCooperativeLevel(PSGLOBAL(window), DISCL_FOREGROUND | (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE)));
    WIN_FCHECK(PSGLOBAL(diMouse)->Acquire());
}

// 0x7485C0 - _InputInitialiseJoys: borra los dos registros de pad, enumera con 0x7477C0 y, para cada pad presente, lee VID/PID.
//
// Reconstruccion exacta del EXE (sin ensamblador). Detalles que el EXE tiene y que se conservan:
//  - Solo se borran wDeviceID y los tres indicadores de cada pad: wVendorID/wProductID (y el relleno del byte +7) NO se tocan.
//  - La enumeracion es el 0x7477C0 del EXE (EnumDevices con 0x747020 y diPadSetRanges 0x746D80); su resultado se ignora.
//  - GetCapabilities se llama con dwSize = 0x2C y su resultado se ignora; ControlsManager.InitDefaultControlConfigJoyPad recibe
//    la constante 0x20, no dwButtons.
//  - HRESULT de GetProperty ignorado: si falla (o no escribe dwData) el EXE guarda los restos de pila que habia en el DIPROPDWORD,
//    y el pad 2 reutiliza el mismo DIPROPDWORD que el pad 1 (cabecera y dwData); si no hay pad 1, su cabecera tambien es resto de
//    pila. Esos cinco dwords estan en [R-0x40, R-0x2C) siendo R la direccion de retorno del EXE: se copian al entrar, antes de que
//    el marco de la DLL pueda pisarlos (diPadInit va sin comprobaciones de ejecucion y con un marco de pocos bytes).
//  - psGlobal se vuelve a leer exactamente donde lo hace el EXE (tras GetCapabilities y tras InitDefaultControlConfigJoyPad); el
//    puntero del pad 1 se vuelve a leer antes de cada llamada.
//  - Ningun llamador lee EAX/ECX/EDX al volver (0x748813 los sobrescribe), por eso no se reproduce el residuo de registros.
static_assert(sizeof(DIDEVCAPS) == 0x2C && sizeof(DIPROPDWORD) == 0x14 && sizeof(DIPROPHEADER) == 0x10);
static inline uint32 s_diPadEntryResidue[5]; // dwSize, dwHeaderSize, dwObj, dwHow, dwData

__declspec(noinline) static void diPadInitBody() {
    AllValidWinJoys.ClearJoyInfo(0);
    AllValidWinJoys.ClearJoyInfo(1);

    plugin::Call<0x7477C0>();

    DIPROPDWORD prop;
    prop.diph.dwSize       = s_diPadEntryResidue[0];
    prop.diph.dwHeaderSize = s_diPadEntryResidue[1];
    prop.diph.dwObj        = s_diPadEntryResidue[2];
    prop.diph.dwHow        = s_diPadEntryResidue[3];
    prop.dwData            = s_diPadEntryResidue[4];

    psGlobalType* ps = RsGlobal.ps;
    if (ps->diDevice1) {
        AllValidWinJoys.JoyStickNum[0].bJoyAttachedToPort = true;

        DIDEVCAPS caps;
        caps.dwSize = sizeof(DIDEVCAPS);
        ps->diDevice1->GetCapabilities(&caps);

        ps = RsGlobal.ps;
        prop.diph.dwSize       = sizeof(DIPROPDWORD);
        prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        prop.diph.dwObj        = 0;
        prop.diph.dwHow        = DIPH_DEVICE;
        ps->diDevice1->GetProperty(DIPROP_VIDPID, &prop.diph);

        AllValidWinJoys.JoyStickNum[0].wVendorID  = LOWORD(prop.dwData);
        AllValidWinJoys.JoyStickNum[0].wProductID = HIWORD(prop.dwData);

        ControlsManager.InitDefaultControlConfigJoyPad(0x20);

        ps = RsGlobal.ps;
    }

    if (ps->diDevice2) {
        ps->diDevice2->GetProperty(DIPROP_VIDPID, &prop.diph);

        AllValidWinJoys.JoyStickNum[1].wVendorID  = LOWORD(prop.dwData);
        AllValidWinJoys.JoyStickNum[1].wProductID = HIWORD(prop.dwData);
        AllValidWinJoys.JoyStickNum[1].bJoyAttachedToPort = true;
    }
}

#pragma runtime_checks("", off)
void diPadInit() {
    // Debe ser lo primero: lee el residuo de pila del llamador del EXE (ver arriba) antes de que nada reutilice esa zona.
    const auto* r = reinterpret_cast<const uint32*>(static_cast<const uint8*>(_AddressOfReturnAddress()) - 0x40);
    s_diPadEntryResidue[0] = r[0];
    s_diPadEntryResidue[1] = r[1];
    s_diPadEntryResidue[2] = r[2];
    s_diPadEntryResidue[3] = r[3];
    s_diPadEntryResidue[4] = r[4];
    diPadInitBody();
}
#pragma runtime_checks("", restore)

// 0x747020
BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCEA inst, LPVOID) {
    static int32 snJoyCount = 0;

    LPDIRECTINPUTDEVICE8* joy = [] {
        switch (snJoyCount) {
        case 0:
            return &PSGLOBAL(diDevice1);
        case 1:
            return &PSGLOBAL(diDevice2);
        default:
            NOTSA_UNREACHABLE();
        }
    }();

    if (SUCCEEDED(PSGLOBAL(diInterface)->CreateDevice(inst->guidInstance, joy, nullptr))) {
        return TRUE;
    }

    if (SUCCEEDED((*joy)->SetDataFormat(&c_dfDIJoystick2))) {
        (*joy)->Release();
        return TRUE;
    }

    ++snJoyCount;

    if (FAILED((*joy)->SetCooperativeLevel(PSGLOBAL(window), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
        (*joy)->Release();
        return TRUE;
    }

    return snJoyCount != 2; // todo: CJoySticks, JOYPAD_COUNT
}

// 0x53F2D0
CMouseControllerState GetMouseSetUp() {
    CMouseControllerState state;

    if (!PSGLOBAL(diMouse)) {
        WinInput::diMouseInit(!FrontEndMenuManager.m_bMenuActive && IsVideoModeExclusive());
    }

    if (!PSGLOBAL(diMouse)) {
        return state; // No mouse device
    }
    
    DIDEVCAPS devCaps;
    devCaps.dwSize = sizeof(DIDEVCAPS);

    // Get device capabilities
    if (SUCCEEDED(PSGLOBAL(diMouse)->GetCapabilities(&devCaps))) {
        switch (devCaps.dwButtons) {
        case 8:
        case 7:
        case 6:
        case 5:
        case 4:
        case 3:
            state.isMouseMiddleButtonPressed = true; // Assumes button 3+ implies MMB exists
            [[fallthrough]];
        case 2:
            state.isMouseRightButtonPressed = true; // Assumes button 2+ implies RMB exists
            [[fallthrough]];
        case 1:
            state.isMouseLeftButtonPressed = true; // Assumes button 1+ implies LMB exists
            [[fallthrough]];
        default:
            break;
        }

        if (devCaps.dwAxes >= 3) {
            state.isMouseWheelMovedDown = true;
            state.isMouseWheelMovedUp = true;
        }

        if (devCaps.dwButtons >= 4) {
            state.isMouseFirstXPressed = true;
        }
        if (devCaps.dwButtons >= 5) {
            state.isMouseSecondXPressed = true;
        }
    }
    return state;
}

// diPadInit se registra aparte de InjectHooks: con NOTSA_USE_SDL3 el resto de WinInput no se engancha, pero el banco
// Dependencies24B necesita el hook registrado (el EXE de SDL no llama a 0x7485C0, asi que enganchar es inocuo).
void InjectPadHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Input");

    RH_ScopedGlobalInstall(diPadInit, 0x7485C0);
}

void InjectHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Input");

    RH_ScopedGlobalInstall(Initialise, 0x7487CF); 
    RH_ScopedGlobalInstall(diMouseInit, 0x7469A0, {.locked = true}); // Can't be hooked because it fails with ACCESS DENIED and crashes
    RH_ScopedGlobalInstall(EnumDevicesCallback, 0x747020);
    RH_ScopedGlobalInstall(diPadSetRanges, 0x746D80);
    RH_ScopedGlobalInstall(GetMouseSetUp, 0x53F2D0);
}

bool IsKeyPressed(unsigned int keyCode) {
    return (GetKeyState(keyCode) & 0x8000) != 0;
}

bool IsKeyDown(unsigned int keyCode) {
    return GetAsyncKeyState(keyCode) >> 0x8;
}
} // namespace WinInput
