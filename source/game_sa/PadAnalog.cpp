// PadAnalog (NOTSA, desviacion propia 15.A): zonas muertas, curva de sensibilidad, inversiones/permutaciones, Southpaw y vibracion del mando XInput segun la rutina de lectura de
// GInputSA 1.11 de Silent (docs/CodigoVivoExe/lectura-mando-ginput.md). Codigo propio; las formulas y constantes salen del analisis estatico del .asi (ver PadAnalog.h).
#include "StdInc.h"

#include "PadAnalog.h"
#include "Pad.h"
#include "Timer.h"
#include "MenuManager.h"
#include "extensions/SilentPatchConfig.h"

#include <cstdlib>
#include <string>

namespace PadAnalog {
namespace {

// ---- opciones -------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::string Up(std::string s) {
    for (auto& c : s) { c = static_cast<char>(std::toupper(static_cast<unsigned char>(c))); }
    return s;
}

// Entero de una opcion: GTASA_PAD<n>_<CLAVE> o `Pad<n>.<Clave>` en gta-reversed.ini. Sin valor devuelve std::nullopt.
std::optional<int> IntOption(const std::string& env, const std::string& ini) {
    const std::string s = OwnDeviationString(env.c_str(), ini.c_str(), "");
    if (s.empty()) {
        return std::nullopt;
    }
    if (s == "true" || s == "on" || s == "yes") { return 1; }
    if (s == "false" || s == "off" || s == "no") { return 0; }
    char* end = nullptr;
    const long v = std::strtol(s.c_str(), &end, 10);
    if (end == s.c_str()) {
        return std::nullopt;
    }
    return static_cast<int>(v);
}

std::optional<int> PadKey(int pad, const char* key) {
    return IntOption("GTASA_PAD" + std::to_string(pad + 1) + "_" + Up(key), "Pad" + std::to_string(pad + 1) + "." + key);
}

struct Globals {
    bool enabled      = true;
    bool ginput       = false;
    bool vibration    = true;
    bool vibrationSet = false;
    bool mapOneToTwo  = false;
    PadParams pad[2];
    bool passThrough[2]{true, true};
    bool testOverride = false;
};

PadParams BuildPad(int pad, bool ginput) {
    PadParams p;
    // Valores por defecto: los de HOY (corte fijo en 0,3 y curva lineal); con PadAnalogGInput=1 los de GInputSA.ini.
    int  leftDz = ginput ? 24 : -1, rightDz = ginput ? 27 : -1;
    int  leftSens = 100, rightSens = 100;
    p.faceButtons    = 50;
    p.analogTriggers = ginput;
    if (const auto v = PadKey(pad, "LeftStickDeadzone"))      { leftDz = *v; }
    if (const auto v = PadKey(pad, "RightStickDeadzone"))     { rightDz = *v; }
    if (const auto v = PadKey(pad, "LeftStickSensitivity"))   { leftSens = *v; }
    if (const auto v = PadKey(pad, "RightStickSensitivity"))  { rightSens = *v; }
    p.left.deadzone     = leftDz < 0 ? -1.0F : DeadzoneFromPercent(leftDz);
    p.right.deadzone    = rightDz < 0 ? -1.0F : DeadzoneFromPercent(rightDz);
    p.left.sensitivity  = SensitivityFromPercent(leftSens);
    p.right.sensitivity = SensitivityFromPercent(rightSens);
    const auto B = [&](const char* k, bool& dst) { if (const auto v = PadKey(pad, k)) { dst = *v != 0; } };
    B("InvertLeftXAxis", p.left.invertX);
    B("InvertLeftYAxis", p.left.invertY);
    B("SwapLeftAxes", p.left.swapAxes);
    B("InvertRightXAxis", p.right.invertX);
    B("InvertRightYAxis", p.right.invertY);
    B("SwapRightAxes", p.right.swapAxes);
    B("Southpaw", p.southpaw);
    B("SwapSticksDuringAiming", p.swapSticksDuringAim);
    B("InvertLook", p.invertLook);
    B("AnalogTriggers", p.analogTriggers);
    B("SensitivityBothAxes", p.curveBothAxes);
    if (const auto v = PadKey(pad, "FaceButtonsSensitivity")) { p.faceButtons = *v; }
    return p;
}

bool IsIdentity(const PadParams& p) {
    const auto stickIdentity = [](const StickParams& s) {
        return s.deadzone < 0.0F && s.sensitivity == 1.0F && !s.invertX && !s.invertY && !s.swapAxes;
    };
    return stickIdentity(p.left) && stickIdentity(p.right) && !p.southpaw && !p.analogTriggers;
}

Globals& G() {
    static Globals g = [] {
        Globals r;
        r.enabled = OwnDeviationOption("GTASA_PAD_ANALOG", "PadAnalog", true);
        r.ginput  = OwnDeviationOption("GTASA_PAD_ANALOG_GINPUT", "PadAnalogGInput", false);
        if (const auto v = IntOption("GTASA_PAD_VIBRATION", "Vibration")) {
            r.vibration    = *v != 0;
            r.vibrationSet = true;
        }
        r.mapOneToTwo = OwnDeviationOption("GTASA_PAD_MAPPADONETOPADTWO", "MapPadOneToPadTwo", false);
        for (int i = 0; i < 2; i++) {
            r.pad[i]         = BuildPad(i, r.ginput);
            r.passThrough[i] = IsIdentity(r.pad[i]);
        }
        const auto& a = r.pad[0];
        SPDLOG_INFO("PadAnalog: activado={} ginput={} vibracion={} map1a2={} | pad1: zona muerta izq={} der={} sens izq={} der={} inv izq=({}{}) swap izq={} inv der=({}{}) swap der={} southpaw={} gatillos analogicos={} pasa-sin-cambios={}",
                    r.enabled, r.ginput, r.vibration, r.mapOneToTwo, a.left.deadzone, a.right.deadzone, a.left.sensitivity, a.right.sensitivity, a.left.invertX ? "X" : "-", a.left.invertY ? "Y" : "-",
                    a.left.swapAxes, a.right.invertX ? "X" : "-", a.right.invertY ? "Y" : "-", a.right.swapAxes, a.southpaw, a.analogTriggers, r.passThrough[0]);
        return r;
    }();
    return g;
}

int PadIndexOf(const CPad* pad) {
    const auto i = pad - &CPad::Pads[0];
    return (i >= 0 && i < MAX_PADS) ? static_cast<int>(i) : 0;
}

// ---- vibracion ------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct XVibration { WORD left, right; };
using SetStateFn = DWORD(WINAPI*)(DWORD, XVibration*);

SetStateFn LoadSetState() {
    static SetStateFn fn = []() -> SetStateFn {
        for (const char* dll : { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll" }) {
            if (const HMODULE m = LoadLibraryA(dll)) {
                if (const auto f = reinterpret_cast<SetStateFn>(GetProcAddress(m, "XInputSetState"))) {
                    return f;
                }
            }
        }
        return nullptr;
    }();
    return fn;
}

int      g_lastLeft  = -1;
int      g_lastRight = -1;
uint32   g_lastUser  = 0;
bool     g_userKnown = false;

void SendMotors(uint32 user, uint16 left, uint16 right) {
    if (g_lastLeft == left && g_lastRight == right && g_userKnown && g_lastUser == user) {
        return; // sin cambios (como GInput: solo llama a XInputSetState si cambia la potencia)
    }
    g_lastLeft  = left;
    g_lastRight = right;
    g_lastUser  = user;
    g_userKnown = true;
    if (const auto fn = LoadSetState()) {
        XVibration v{ left, right };
        fn(user, &v);
    }
}

struct ExitGuard {
    ~ExitGuard() { StopVibration(); }
};
ExitGuard g_exitGuard;

bool g_vibWarned = false;
uint8 g_lt[2]{}, g_rt[2]{};
bool  g_trigValid[2]{};
} // namespace

bool Enabled() { return G().enabled; }
bool GInputDefaults() { return G().ginput; }
bool MapPadOneToPadTwo() { return G().enabled && G().mapOneToTwo; }
const PadParams& Params(int padIndex) { return G().pad[padIndex & 1]; }

bool IsPassThrough(int padIndex) {
    return !G().enabled || G().passThrough[padIndex & 1];
}

bool ProcessSticks(CPad& pad, int padIndex, const StickInput& in) {
    g_lt[padIndex & 1] = in.lt; // para ApplyTriggers (los botones del mando se escriben en PCTempJoyState despues de ProcessPad, no antes)
    g_rt[padIndex & 1] = in.rt;
    g_trigValid[padIndex & 1] = true;
    if (IsPassThrough(padIndex)) {
        return false;
    }
    const auto& cfg = Params(padIndex);
    const auto  o   = ComputeSticks(cfg, in);
    auto&       js  = pad.PCTempJoyState;
    js.LeftStickX   = o.left.x;
    js.LeftStickY   = o.left.y;
    js.RightStickX  = o.right.x;
    js.RightStickY  = o.right.y;
    return true;
}

void ApplyTriggers(CPad& pad, int padIndex) {
    const int i = padIndex & 1;
    if (!G().enabled || !G().pad[i].analogTriggers || !g_trigValid[i]) {
        return;
    }
    // GInput: gatillos analogicos (0x100072A6..). ReconcileTwoControllersInput pone los botones a 255; si L2/R2 vienen solo del mando (ni teclado ni raton) y la asignacion del juego los puso
    // a no-cero, el valor pasa a ser el analogico (siempre >= 1 en ese caso, porque el boton digital del juego exige > 30).
    auto&       ns = pad.NewState;
    const auto& js = pad.PCTempJoyState;
    if (ns.LeftShoulder2 != 0 && js.LeftShoulder2 != 0 && pad.PCTempKeyState.LeftShoulder2 == 0 && pad.PCTempMouseState.LeftShoulder2 == 0) {
        ns.LeftShoulder2 = static_cast<int16>(std::max(1, TriggerValue(g_lt[i])));
    }
    if (ns.RightShoulder2 != 0 && js.RightShoulder2 != 0 && pad.PCTempKeyState.RightShoulder2 == 0 && pad.PCTempMouseState.RightShoulder2 == 0) {
        ns.RightShoulder2 = static_cast<int16>(std::max(1, TriggerValue(g_rt[i])));
    }
}

void OnNoXInput() {
    g_trigValid[0] = g_trigValid[1] = false;
    StopVibration();
}

// ---- vibracion ------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StopVibration() {
    if (g_lastLeft > 0 || g_lastRight > 0) {
        SendMotors(g_lastUser, 0, 0);
    }
}

int LastSentLeft() { return g_lastLeft; }
int LastSentRight() { return g_lastRight; }

void UpdateVibration(CPad& pad, uint32 xinputUser) {
    const auto& g = G();
    if (!g.enabled || !g.vibration) {
        StopVibration();
        return;
    }
    if (g.vibrationSet && !g_vibWarned) {
        g_vibWarned = true;
        FrontEndMenuManager.m_PrefsUseVibration = g.vibration; // Vibration= del ini como en GInput; despues el menu del juego sigue mandando
    }
    if (FrontEndMenuManager.m_bMenuActive || !FrontEndMenuManager.m_PrefsUseVibration) {
        StopVibration();
        return;
    }
    if (pad.ShakeDur == 0) {
        StopVibration();
        return;
    }
    // GInput (0x100074C1..0x10007540): con el mando activo y tocado hace menos de 60 s: motor izquierdo = ShakeFreq * 0x101, derecho 0; la duracion baja timeStep * 20 ms por fotograma.
    if (static_cast<int32>(CTimer::GetTimeInMS() - pad.LastTimeTouched) <= 60000) { // comparacion con signo como el .asi (jg)
        SendMotors(xinputUser, MotorFromShakeFreq(static_cast<uint8>(pad.ShakeFreq)), 0);
        pad.ShakeDur = ShakeDurAfterFrame(pad.ShakeDur, CTimer::GetTimeStep());
        if (pad.ShakeDur > 0) {
            return;
        }
    }
    pad.ShakeDur = 0;
    SendMotors(xinputUser, 0, 0);
}

// ---- pad actual / getters ------------------------------------------------------------------------------------------------------------------------------------------------------
PadNumberScope::PadNumberScope(int padID) : m_saved(CPad::padNumber), m_active(padID == PAD1 && MapPadOneToPadTwo()) {
    if (m_active) {
        CPad::padNumber = 1; // MapPadOneToPadTwo: el primer mando XInput maneja al jugador 2 (GInput 0x10015580/0x100151D0)
    }
}
PadNumberScope::~PadNumberScope() {
    if (m_active) {
        auto& n = CPad::padNumber;
        n = static_cast<std::remove_cvref_t<decltype(n)>>(m_saved);
    }
}

bool LookInvertToggle(const CPad* pad) {
    return G().enabled && G().pad[PadIndexOf(pad)].invertLook;
}
bool AimSwapToggle(const CPad* pad) {
    return G().enabled && G().pad[PadIndexOf(pad)].swapSticksDuringAim;
}

} // namespace PadAnalog
