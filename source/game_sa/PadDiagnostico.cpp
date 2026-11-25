#include "StdInc.h"
#include <cstdio>
#include <cstdarg>

#include "PadDiagnostico.h"
#include "Pad.h"
#include "ControllerConfigManager.h"
#include "CutsceneMgr.h"
#include "Camera.h"
#include "MenuManager.h"
#include "platform/win/WinInput.h"
#include "app.h"

namespace PadDiag {
namespace {
FILE*       s_file{};
bool        s_enabled{};
bool        s_init{};
RawXInput   s_raw{};
DIJOYSTATE2 s_joy{};
bool        s_haveJoy{};
uint32      s_frame{};
std::string s_lastLine;
ULONGLONG   s_lastWrite{};

void Open() {
    if (s_init) {
        return;
    }
    s_init = true;
    char value[8]{};
    if (!GetEnvironmentVariableA("GTASA_PAD_DEBUG", value, sizeof(value)) || value[0] != '1') {
        return;
    }
    CreateDirectoryA("logs", nullptr);
    s_file = fopen("logs/pad.log", "w");
    if (!s_file) {
        return;
    }
    s_enabled = true;
    SYSTEMTIME st;
    GetLocalTime(&st);
    fprintf(s_file, "# pad.log %04d-%02d-%02d %02d:%02d:%02d (GTASA_PAD_DEBUG=1)\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    for (const char* dll : { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll" }) {
        if (const HMODULE m = GetModuleHandleA(dll)) {
            char path[MAX_PATH]{};
            GetModuleFileNameA(m, path, MAX_PATH);
            fprintf(s_file, "# DLL XInput cargada: %s\n", path);
        }
    }
    fflush(s_file);
}

void Out(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(s_file, fmt, ap);
    va_end(ap);
}

std::string JoyButtons(const DIJOYSTATE2& j) {
    std::string r;
    for (int i = 0; i < 16; i++) {
        if (j.rgbButtons[i] & 0x80) {
            r += std::to_string(i + 1) + ",";
        }
    }
    return r.empty() ? "-" : r;
}

std::string Dpad(uint16 b) {
    std::string r;
    if (b & 1) r += "U";
    if (b & 2) r += "D";
    if (b & 4) r += "L";
    if (b & 8) r += "R";
    return r.empty() ? "-" : r;
}

std::string PadButtons(const CControllerState& s) {
    std::string r;
    const auto add = [&](const char* n, int16 v) { if (v) { r += n; r += ","; } };
    add("L1", s.LeftShoulder1);
    add("L2", s.LeftShoulder2);
    add("R1", s.RightShoulder1);
    add("R2", s.RightShoulder2);
    add("up", s.DPadUp);
    add("down", s.DPadDown);
    add("left", s.DPadLeft);
    add("right", s.DPadRight);
    add("Start", s.Start);
    add("Select", s.Select);
    add("Square", s.ButtonSquare);
    add("Triangle", s.ButtonTriangle);
    add("Cross", s.ButtonCross);
    add("Circle", s.ButtonCircle);
    add("ShockL", s.ShockButtonL);
    add("ShockR", s.ShockButtonR);
    return r.empty() ? "-" : r;
}

int CountJoyActions() {
    int n = 0;
    for (const auto& a : ControlsManager.m_Actions) {
        if (a.Keys[+eControllerType::JOY_STICK].m_uiActionInitiator != 0) {
            n++;
        }
    }
    return n;
}
} // namespace

bool Enabled() {
    Open();
    return s_enabled;
}

void NoteRead(const RawXInput& raw, const DIJOYSTATE2* joyState) {
    if (!Enabled()) {
        return;
    }
    s_raw = raw;
    s_haveJoy = joyState != nullptr;
    if (joyState) {
        s_joy = *joyState;
    }
}

void NoteEvent(const char* text) {
    if (!Enabled()) {
        return;
    }
    Out("[%8llu ms][f%u] EVENTO: %s\n", GetTickCount64(), s_frame, text);
    fflush(s_file);
}

void EndFrame() {
    if (!Enabled()) {
        return;
    }
    s_frame++;
    const auto& pad  = *CPad::GetPad(0);
    const auto& s    = pad.NewState;
    const auto& joy0 = AllValidWinJoys.JoyStickNum[0];
    const auto  ped  = FindPlayerPed();
    const auto  veh  = FindPlayerVehicle(-1, false);

    char line[1400];
    // Sin el numero de paquete en la parte comparada: un stick con ruido lo cambia sin que cambie nada visible; el paquete se anota igualmente al escribir.
    snprintf(line, sizeof(line),
        "xi[idx%u rc=%u uso=%d btn=0x%04X dpad=%s LT=%u RT=%u L=(%d,%d) R=(%d,%d)] "
        "di[%s lX=%d lY=%d lZ=%d lRz=%d pov=%u btns=%s] "
        "pad0[btns=%s LS=(%d,%d) RS=(%d,%d) Mode=%d DisCtl=%u] "
        "juego[menu=%d pant=%d cine=%d metodo=%d mouse3p=%d cam=%d ped=%d veh=%d] "
        "cfg[adj=%d Z=%d RZ=%d vid=%04X pid=%04X ddev1=%d ddev2=%d mapJoy=%d padNum=%d]",
        s_raw.index, s_raw.rc, s_raw.usingXInput, s_raw.buttons, Dpad(s_raw.buttons).c_str(), s_raw.lt, s_raw.rt, s_raw.lx, s_raw.ly, s_raw.rx, s_raw.ry,
        s_haveJoy ? JoyButtons(s_joy).c_str() : "(sin lectura)", s_joy.lX, s_joy.lY, s_joy.lZ, s_joy.lRz, s_joy.rgdwPOV[0], s_haveJoy ? JoyButtons(s_joy).c_str() : "-",
        PadButtons(s).c_str(), s.LeftStickX, s.LeftStickY, s.RightStickX, s.RightStickY, pad.Mode, (unsigned)pad.DisablePlayerControls,
        FrontEndMenuManager.m_bMenuActive, (int)FrontEndMenuManager.m_nCurrentScreen, (int)CCutsceneMgr::IsRunning(), (int)FrontEndMenuManager.m_ControlMethod,
        (int)CCamera::m_bUseMouse3rdPerson, (int)TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode, ped ? (int)ped->m_nPedState : -1, veh ? 1 : 0,
        joy0.bJoyAttachedToPort, joy0.bZAxisPresent, joy0.bZRotPresent, (unsigned)joy0.wVendorID, (unsigned)joy0.wProductID,
        PSGLOBAL(diDevice1) != nullptr, PSGLOBAL(diDevice2) != nullptr, CountJoyActions(), (int)CPad::padNumber);

    const auto now = GetTickCount64();
    if (s_lastLine != line || now - s_lastWrite >= 500) {
        // se limita a 20 escrituras por segundo cuando hay cambios continuos
        if (s_lastLine == line || now - s_lastWrite >= 50) {
            Out("[%8llu ms][f%u][pkt%u] %s\n", now, s_frame, s_raw.packet, line);
            fflush(s_file);
            s_lastLine = line;
            s_lastWrite = now;
        }
    }
}
} // namespace PadDiag
