#pragma once

#include <cmath>
#include <cstdint>

class CPad;

/*
 * PadAnalog (NOTSA, desviacion propia 15.A; docs/DesviacionesConocidas.md, docs/CodigoVivoExe/lectura-mando-ginput.md): lectura del mando XInput con zonas muertas, curva de sensibilidad,
 * inversiones/permutaciones de ejes, Southpaw y vibracion, segun la rutina de lectura de GInputSA 1.11 de Silent (0x10006EA0/0x10006ED7 y 0x100151D0 del .asi; autorizacion pedida por el usuario del
 * proyecto; ver THIRD_PARTY_NOTICES.md). Codigo C++ propio: las formulas se recuperaron leyendo el .asi (analisis estatico) y se comprobaron contra una evaluacion instruccion a instruccion de esa rutina
 * (contrib/pad_analog). NO se porta la capa de importacion de XInput de GInput (ordinales 2/3/100), ni Steam/Guide, ni SIXAXIS/SCP.
 *
 * Opciones (variable de entorno > linea de gta-reversed.ini > valor por defecto):
 *   GTASA_PAD_ANALOG=0|1          / PadAnalog=          modulo entero (por defecto 1; 0 = comportamiento anterior: ni curvas, ni zonas muertas propias, ni vibracion)
 *   GTASA_PAD_ANALOG_GINPUT=0|1   / PadAnalogGInput=    por defecto 0: los valores por defecto de cada clave son los de HOY (zona muerta fija de 0,3 sin reescalar, curva lineal); con 1 pasan a ser los
 *                                                       de GInputSA.ini (LeftStickDeadzone=24, RightStickDeadzone=27, sensibilidad 100, FaceButtonsSensitivity=50, gatillos analogicos)
 *   GTASA_PAD1_<CLAVE> / Pad1.<Clave>=   y   GTASA_PAD2_<CLAVE> / Pad2.<Clave>=   con las claves de GInputSA.ini:
 *       LeftStickDeadzone, RightStickDeadzone (0-75), LeftStickSensitivity, RightStickSensitivity (0-200), InvertLeftXAxis, InvertLeftYAxis, SwapLeftAxes, InvertRightXAxis, InvertRightYAxis,
 *       SwapRightAxes, Southpaw, SwapSticksDuringAiming, InvertLook, FaceButtonsSensitivity (1-100)
 *       y propias: AnalogTriggers=0|1 (gatillos L2/R2 analogicos), SensitivityBothAxes=0|1 (la curva de GInput solo toca el eje X de cada stick; con 1 tambien el Y)
 *   GTASA_PAD_VIBRATION=0|1 / Vibration=  (como [GInput] Vibration; por defecto 1; la preferencia de vibracion del menu del juego sigue mandando)
 *   GTASA_PAD_MAPPADONETOPADTWO=0|1 / MapPadOneToPadTwo=   (como [GInput]; por defecto 0)
 */
namespace PadAnalog {

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Parte pura (sin dependencias del juego; la usa el banco contrib/pad_analog/bench_core.cpp). Constantes leidas del .asi (direcciones del .rdata de GInputSA.asi).
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline constexpr double kXInputScale       = 3.051850947599719e-05; // 0x10034800: 1/32767
inline constexpr double kStickOutScale     = 127.0;                  // 0x10034818
inline constexpr double kLegacyOutScale    = 128.0;                  // el codigo actual del proyecto (CPad::ProcessPad) escala por 128
inline constexpr double kLegacyDeadzone    = 0.3;                    // idem: |pos| <= 0.3f se ignora, sin reescalar
inline constexpr double kPctToFraction     = 0.009999999776482582;   // 0x100347EC: 0.01f
inline constexpr float  kMaxDeadzone       = 0.75F;                  // 0x10034808: tope de la zona muerta del INI
inline constexpr float  kMaxSensitivity    = 2.0F;                   // 0x10034814: tope de la sensibilidad del INI
inline constexpr double kTriggerDeadzone   = 30.0;                   // 0x10034828
inline constexpr double kTriggerScale      = 1.1333333253860474;     // 0x1003480C (255/225 como float)
inline constexpr double kFaceBase          = 25500.0;                // 0x1003483C
inline constexpr double kScpTriggerDeadzone = 0.11764705926179886;   // 0x100347F4 (30/255 como float)
inline constexpr double kScpTriggerScale   = 289.0;                  // 0x10034838

struct StickParams {
    float deadzone    = -1.0F;  // < 0: zona muerta actual del proyecto (corte fijo en 0,3, sin reescalar); >= 0: la de GInput (0..0,75)
    float sensitivity = 1.0F;   // 0..2: 0 = cubica, 1 = lineal, 2 = raiz cubica
    bool  invertX     = false;
    bool  invertY     = false;  // false = el convenio del juego (arriba = negativo)
    bool  swapAxes    = false;
};

struct StickOut {
    int16_t x = 0;
    int16_t y = 0;
};

// Valor del INI (entero) -> fraccion con el redondeo del .asi (entero * 0.01f en x87, guardado como float) y tope.
inline float DeadzoneFromPercent(int pct) {
    const float v = static_cast<float>(static_cast<double>(pct) * kPctToFraction);
    return v > kMaxDeadzone ? kMaxDeadzone : (v < 0.0F ? 0.0F : v);
}
inline float SensitivityFromPercent(int pct) {
    const float v = static_cast<float>(static_cast<double>(pct) * kPctToFraction);
    return v > kMaxSensitivity ? kMaxSensitivity : (v < 0.0F ? 0.0F : v);
}

// Zona muerta de GInput: (|v|-dz)/(1-dz) con el signo de v; dentro de la zona muerta (dz > |v|) da 0.
inline double ApplyDeadzone(double v, double dz) {
    if (dz > std::fabs(v)) {
        return 0.0;
    }
    return (v > 0.0 ? (v - dz) : (v + dz)) / (1.0 - dz);
}

// Curva de sensibilidad de GInput (0x10006FB6..0x10007020). s <= 1: s*x + (1-s)*x^3 (0 = cubica pura, 1 = lineal exacta). s > 1: (s-1)*cbrt(x) + (2-s)*x (2 = raiz cubica pura).
inline double ApplyCurve(double x, double s) {
    if (s > 1.0) {
        return (s - 1.0) * std::cbrt(x) + (2.0 - s) * x;
    }
    return s * x + (1.0 - s) * (x * x * x);
}

inline double Clamp1(double v) { return v < -1.0 ? -1.0 : (v > 1.0 ? 1.0 : v); }

// Un stick: ux/uy son el valor normalizado con "arriba = positivo" (para el XInput directo: sThumb * kXInputScale). Devuelve los int16 que van a CControllerState (abajo = positivo).
// `curveBothAxes`: false = como GInput (la curva solo se aplica al eje X; en el mando izquierdo y en el derecho).
inline StickOut ConvertStick(double ux, double uy, const StickParams& p, bool curveBothAxes) {
    double x, y, scale;
    if (p.deadzone >= 0.0F) {
        x = ApplyDeadzone(ux, p.deadzone);
        y = ApplyDeadzone(uy, p.deadzone);
        scale = kStickOutScale;
    } else {
        x = std::fabs(ux) <= static_cast<double>(static_cast<float>(kLegacyDeadzone)) ? 0.0 : ux;
        y = std::fabs(uy) <= static_cast<double>(static_cast<float>(kLegacyDeadzone)) ? 0.0 : uy;
        scale = kLegacyOutScale;
    }
    x = ApplyCurve(x, p.sensitivity);
    if (curveBothAxes) {
        y = ApplyCurve(y, p.sensitivity);
    }
    if (p.invertX) {
        x = -x;
    }
    if (!p.invertY) {
        y = -y;
    }
    x = Clamp1(x);
    y = Clamp1(y);
    StickOut o;
    if (p.swapAxes) {
        o.x = static_cast<int16_t>(static_cast<int>(y * scale));
        o.y = static_cast<int16_t>(static_cast<int>(x * scale));
    } else {
        o.x = static_cast<int16_t>(static_cast<int>(x * scale));
        o.y = static_cast<int16_t>(static_cast<int>(y * scale));
    }
    return o;
}

// Gatillo XInput (0..255) -> 0..254 como GInput (0x100072A6..0x100072FD): hasta 30 no hay nada; por encima, (t-30)*1,1333 truncado.
inline int TriggerValue(int t) {
    if (!(static_cast<double>(t) > kTriggerDeadzone)) {
        return 0;
    }
    return static_cast<int>((static_cast<double>(t) - kTriggerDeadzone) * kTriggerScale);
}

// FaceButtonsSensitivity (1-100; 0 = "cualquier pulsacion cuenta entera") -> factor (0x10009F5C..0x10009FB7): 25500/valor, nunca menor que 255. Solo lo usa la ruta SCP de GInput (botones analogicos).
inline float FaceButtonsFactor(int value) {
    if (value <= 0) {
        return INFINITY;
    }
    const float f = static_cast<float>(kFaceBase / static_cast<double>(value));
    return f < 255.0F ? 255.0F : f;
}
// Boton analogico 0..1 -> 0..255 (0x10007835..): b*factor, tope 255. NaN (0 * infinito) da 0.
inline int FaceButtonValue(float b, float factor) {
    const double v = static_cast<double>(b) * static_cast<double>(factor);
    if (v > 255.0) {
        return 255;
    }
    return std::isnan(v) ? 0 : static_cast<int>(v);
}
// Gatillo analogico de SCP 0..1 -> 0..254 (0x100077E4..0x10007826).
inline int ScpTriggerValue(float t) {
    if (!(static_cast<double>(t) > kScpTriggerDeadzone)) {
        return 0;
    }
    return static_cast<int>((static_cast<double>(t) - kScpTriggerDeadzone) * kScpTriggerScale);
}

// Vibracion: CPad::ShakeFreq (0..255, byte) -> motor izquierdo (16 bits) como GInput (0x100074EC..): freq * 0x101; el derecho siempre 0.
inline uint16_t MotorFromShakeFreq(uint8_t freq) { return static_cast<uint16_t>(static_cast<uint16_t>(freq) * 0x101u); }
// Duracion restante (ms) tras un fotograma: ShakeDur += trunc(timeStep * -20.0f) (0x10007500..0x10007518); <= 0 detiene la vibracion.
inline int16_t ShakeDurAfterFrame(int16_t dur, float timeStep) {
    return static_cast<int16_t>(dur + static_cast<int>(static_cast<double>(timeStep) * -20.0));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Parte con el juego (PadAnalog.cpp)
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct PadParams {
    StickParams left;
    StickParams right;
    bool        southpaw             = false;
    bool        swapSticksDuringAim  = false;
    bool        invertLook           = false;
    int         faceButtons          = 50;
    bool        analogTriggers       = false;
    bool        curveBothAxes        = false;
    bool        explicitDeadzone() const { return left.deadzone >= 0.0F || right.deadzone >= 0.0F; }
};

struct StickInput {
    int16_t rawLX = 0, rawLY = 0, rawRX = 0, rawRY = 0; // XInput (arriba = positivo)
    uint8_t lt = 0, rt = 0;
    // El valor que el juego ya normaliza (CPad::NormalizePadAxis sobre el DIJOYSTATE2 fabricado; Y con el signo del juego): para la zona muerta actual del proyecto
    float legacyLX = 0, legacyLY = 0, legacyRX = 0, legacyRY = 0;
    bool menuInvertX1 = false, menuInvertY1 = false, menuSwap1 = false; // preferencias del menu del juego (stick izquierdo)
    bool menuInvertX2 = false, menuInvertY2 = false, menuSwap2 = false; // stick derecho
};

// Resultado completo de un fotograma de sticks (lo que ProcessSticks escribe en CPad::PCTempJoyState).
struct PadSticksOut {
    StickOut left;
    StickOut right;
};
// Parte pura de ProcessSticks y equivalente actual del proyecto (el lambda UpdateJoyStickPosition de CPad::ProcessPad); el banco comprueba que la tuberia con la configuracion por defecto lo reproduce.
inline PadSticksOut LegacySticks(const StickInput& in) {
    PadSticksOut o;
    const auto axis = [](float pos, bool inverted, bool swapped, int16_t& outSwapped, int16_t& outNormal) {
        if (std::fabs(pos) <= 0.3F) {
            return;
        }
        if (inverted) {
            pos *= -1.0F;
        }
        (swapped ? outSwapped : outNormal) = static_cast<int16_t>(pos * 128.0F);
    };
    axis(in.legacyLX, in.menuInvertX1, in.menuSwap1, o.left.y, o.left.x);
    axis(in.legacyLY, in.menuInvertY1, in.menuSwap1, o.left.x, o.left.y);
    axis(in.legacyRX, in.menuInvertX2, in.menuSwap2, o.right.y, o.right.x);
    axis(in.legacyRY, in.menuInvertY2, in.menuSwap2, o.right.x, o.right.y);
    return o;
}

inline PadSticksOut ComputeSticks(const PadParams& cfg, const StickInput& in) {
    StickParams l = cfg.left;
    StickParams r = cfg.right;
    l.invertX ^= in.menuInvertX1;
    l.invertY ^= in.menuInvertY1;
    l.swapAxes ^= in.menuSwap1;
    r.invertX ^= in.menuInvertX2;
    r.invertY ^= in.menuInvertY2;
    r.swapAxes ^= in.menuSwap2;
    const auto pick = [](const StickParams& p, int16_t rawX, int16_t rawY, float legX, float legY, double& ux, double& uy) {
        if (p.deadzone >= 0.0F) { // GInput: el valor crudo de XInput / 32767
            ux = static_cast<double>(rawX) * kXInputScale;
            uy = static_cast<double>(rawY) * kXInputScale;
        } else {                  // zona muerta actual: el valor ya normalizado por el juego (Y con el signo del juego: abajo = positivo)
            ux = legX;
            uy = -static_cast<double>(legY);
        }
    };
    double lx, ly, rx, ry;
    pick(l, in.rawLX, in.rawLY, in.legacyLX, in.legacyLY, lx, ly);
    pick(r, in.rawRX, in.rawRY, in.legacyRX, in.legacyRY, rx, ry);
    PadSticksOut o;
    o.left  = ConvertStick(lx, ly, l, cfg.curveBothAxes);
    o.right = ConvertStick(rx, ry, r, cfg.curveBothAxes);
    if (cfg.southpaw) {
        { const StickOut t = o.left; o.left = o.right; o.right = t; }
    }
    return o;
}


[[nodiscard]] bool Enabled();                 // modulo encendido (GTASA_PAD_ANALOG)
[[nodiscard]] bool GInputDefaults();           // PadAnalogGInput
[[nodiscard]] const PadParams& Params(int padIndex);
[[nodiscard]] bool MapPadOneToPadTwo();

// true si no hay nada que cambiar para este pad (configuracion por defecto equivalente a la actual): CPad::ProcessPad sigue con su propio codigo.
[[nodiscard]] bool IsPassThrough(int padIndex);
// Escribe los sticks en pad.PCTempJoyState; true = ya escrito (ProcessPad se salta su bloque de sticks). Guarda los gatillos crudos para ApplyTriggers.
bool ProcessSticks(CPad& pad, int padIndex, const StickInput& in);
// Gatillos analogicos (opcion AnalogTriggers): en CPad::Update, justo despues de ReconcileTwoControllersInput (que pone los botones a 255), sustituye el 255 digital de L2/R2 por el valor analogico.
void ApplyTriggers(CPad& pad, int padIndex);
// Sin mando XInput en este fotograma: para la vibracion y olvida los gatillos.
void OnNoXInput();

// Vibracion (por fotograma, solo con el mando XInput `xinputUser` conectado). Sin mando o con el modulo apagado deja la vibracion parada.
void UpdateVibration(CPad& pad, uint32_t xinputUser);
void StopVibration();                   // CPad::StopShaking / StopPadsShaking / mando perdido
// Ultima potencia enviada al mando (banco de pruebas): izquierdo/derecho; -1 = nunca enviada.
[[nodiscard]] int LastSentLeft();
[[nodiscard]] int LastSentRight();

// MapPadOneToPadTwo: durante ProcessPad el pad actual (CPad::padNumber) pasa a ser el 2 si la opcion esta activa.
class PadNumberScope {
public:
    explicit PadNumberScope(int padID);
    ~PadNumberScope();
    PadNumberScope(const PadNumberScope&) = delete;
    PadNumberScope& operator=(const PadNumberScope&) = delete;
private:
    int  m_saved;
    bool m_active;
};

// Getters de CPad: InvertLook y SwapSticksDuringAiming se aplican alternando el valor que ya tiene el juego (bInvertLook4Pad / byte_8CD782). Sin configuracion devuelven false (nada cambia).
[[nodiscard]] bool LookInvertToggle(const CPad* pad);
[[nodiscard]] bool AimSwapToggle(const CPad* pad);

} // namespace PadAnalog
