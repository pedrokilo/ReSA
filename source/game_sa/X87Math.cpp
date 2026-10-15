// Compile separately: /O2 /Oi /arch:IA32 /fp:fast, ordinary /Zi and no RTC/PCH/unity.
// Funciones sueltas y noinline: cada llamador conserva sus opciones de compilación y la
// secuencia x87 no cambia al insertarse en línea (p. ej. /fp:fast reemplaza sqrt(s) > 4 por s > 16).
extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
extern "C" double __cdecl sqrt(double);
extern "C" double __cdecl atan2(double, double);
extern "C" double __cdecl log10(double);
extern "C" double __cdecl pow(double, double);
#pragma intrinsic(sin, cos, sqrt, atan2, log10, pow)
#include <float.h>
#include <string.h>
#include <assert.h>
#include <bit>
#include "X87Math.h"

// --- Seno y coseno ---
// Acceptance requires native x87 opcodes without any intervening store or CRT call.
__declspec(noinline) float __cdecl TrigSimpleSinCpp(float angle) { return float(sin(double(angle))); }
__declspec(noinline) float __cdecl TrigSimpleCosCpp(float angle) { return float(cos(double(angle))); }
__declspec(noinline) float __cdecl TrigSimpleSinMulCpp(float angle, float k) { return float(sin(double(angle)) * double(k)); }
__declspec(noinline) float __cdecl TrigSimpleCosMulCpp(float angle, float k) { return float(cos(double(angle)) * double(k)); }

// --- Combinaciones ---
__declspec(noinline) float __cdecl TrigCompoundScaledSinMulAdd(float angle, float k, float mult, float add) {
    return float(sin(double(angle) * double(k)) * double(mult) + double(add));
}
__declspec(noinline) float __cdecl TrigCompoundSinMulAdd(float angle, float mult, float add) {
    return float(sin(double(angle)) * double(mult) + double(add));
}
__declspec(noinline) float __cdecl TrigCompoundCosMulAdd(float angle, float mult, float add) {
    return float(cos(double(angle)) * double(mult) + double(add));
}
__declspec(noinline) float __cdecl TrigCompoundNegSinDiv(float angle, float divisor) {
    double value = sin(double(angle));
    value = -value;
    return float(value / double(divisor));
}
__declspec(noinline) float __cdecl TrigCompoundCosHalf(float angle, float half) {
    const volatile float* operand = &half;
    double value = cos(double(angle));
    value *= double(*operand);
    value += double(*operand);
    return float(value);
}
__declspec(noinline) float __cdecl TrigCompoundSubSin(float z, float angle) {
    return float(-(sin(double(angle)) - double(z)));
}

// --- Comparaciones ordenadas ---
// Keep comparisons ordered and let the compiler emit x87 condition codes.
// The emitted comparison uses second against first; verify each object build.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl CloudsEqualOrderedCpp(float first, float second) {
    return first <= second && first >= second;
}
__declspec(noinline) bool __cdecl CloudsLessOrderedCpp(float first, float second) {
    return first < second;
}
#pragma float_control(pop)

// --- Sombras ---
// El valor se guarda como double (FSTP64) y se vuelve a cargar, como el helper original.
extern "C" __declspec(noinline) double __cdecl ShadowSqrtCpp(float value) {
    volatile double result = sqrt(double(value));
    return result;
}
extern "C" __declspec(noinline) double __cdecl ShadowDistanceFadeCpp(float squared, float distance) {
    volatile double result = 1.0f - (sqrt(double(squared)) - double(distance) * 0.75f) * (4.0f / double(distance));
    return result;
}

// --- Animación ---
// Con /fp:fast el compilador reordena las sumas del producto escalar y cambia el redondeo: se calcula en una función
// de orden estricto. Devuelve double y no float: el EXE compara el valor sin redondear, y al devolver float el compilador
// lo guarda como float, con lo que un producto diminuto negativo pasaría a -0 y dejaría de ser "< 0". Un double conserva
// siempre el signo (el producto de dos floats nunca desborda por debajo el rango de un double).
#pragma float_control(precise, on, push)
static __declspec(noinline) double AnimOrderedQuaternionDot(const float* a, const float* b) {
    return ((a[3] * b[3] + a[2] * b[2]) + a[1] * b[1]) + a[0] * b[0];
}
#pragma float_control(pop)

__declspec(noinline) bool __cdecl AnimQuaternionDotNegativeCpp(const float* last, const float* cur) {
    return AnimOrderedQuaternionDot(last, cur) < 0.0;
}

__declspec(noinline) void __cdecl AnimCalcThetaCpp(const float* prev, const float* curr, float* theta, float* invSinTheta) {
    // El EXE guarda el producto como float, lo recorta a 1 comparando el valor sin redondear (con NaN no recorta) y
    // vuelve a cargar el float guardado.
    const double dot = AnimOrderedQuaternionDot(prev, curr);
    volatile float stored = float(dot);
    if (dot > 1.0) {
        stored = 1.0f;
    }
    const float x = stored;
    // acos del CRT del juego (0x822380): palabra de control (anterior & 0x300) | 0x7F, fpatan(sqrt((1 + x)(1 - x)), x).
    const unsigned previousControl = _control87(0, 0);
    _control87((previousControl & _MCW_PC) | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    const double acosValue = atan2(sqrt((1.0f + x) * (1.0f - x)), x);
    _control87(previousControl, _MCW_PC | _MCW_RC | _MCW_EM);
    *theta = float(acosValue);
    // 1/sin con el acos completo (sin redondear); 0 sólo si es exactamente 0 (con NaN sigue el camino de fsin).
    if (acosValue != 0.0) {
        *invSinTheta = float(1.0f / sin(acosValue));
    } else {
        *invSinTheta = 0.0f;
    }
}

// --- Audio ---
__declspec(noinline) float __cdecl AudioLog10Times20Cpp(float x) {
    volatile float result = float(log10(double(x)) * 20.0f); // 0x858BA4
    return result;
}

__declspec(noinline) float __cdecl AudioRadioVolumeDbCpp(float effects, float music) {
    volatile float result = float(log10(double(effects) / double(music)) * 20.0f - 9.0f); // 0x858BA4 y 0x85EED4
    return result;
}

__declspec(noinline) float __cdecl AudioSumToMsCpp(unsigned long long sum) {
    const long long low63 = static_cast<long long>(sum & 0x7FFFFFFFFFFFFFFFull);
    const long long top = static_cast<long long>(sum & 0x8000000000000000ull);
    volatile float ms = float((double(low63) - double(top)) * 0.0001f); // 0x858FC4
    return ms;
}

__declspec(noinline) float __cdecl AudioPercentOfProductCpp(int product) {
    volatile float scaled = float(double(product) * 0.01f); // 0x858C58
    return scaled;
}

__declspec(noinline) float __cdecl AudioLog10Cpp(float p) {
    volatile float result = float(log10(double(p)));
    return result;
}

// Con /fp:fast el compilador reordena las sumas; en una función de orden estricto se conserva el del EXE.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl VectorSquaredMagnitudeAboveCpp(const float* vector, float limit) {
    return (vector[2] * vector[2] + vector[1] * vector[1]) + vector[0] * vector[0] > limit;
}
#pragma float_control(pop)

// --- Lote de nueve archivos ---
__declspec(noinline) double __cdecl ExeCompatiblePowCpp(double base, double exponent) {
    unsigned long long b, e;
    memcpy(&b, &base, 8);
    memcpy(&e, &exponent, 8);
    const unsigned long long magnitudeMask = 0x7FFFFFFFFFFFFFFFull, infinity = 0x7FF0000000000000ull, quietBit = 0x0008000000000000ull;
    const bool exponentNaN = (e & magnitudeMask) > infinity, exponentInfinite = (e & magnitudeMask) == infinity;
    // ±1 elevado a NaN: el EXE devuelve ese NaN silenciado; a ±infinito: el NaN indefinido (0xFFF8000000000000)
    if ((b & magnitudeMask) == 0x3FF0000000000000ull && (exponentNaN || exponentInfinite)) {
        const unsigned long long r = exponentNaN ? (e | quietBit) : 0xFFF8000000000000ull;
        double result;
        memcpy(&result, &r, 8);
        return result;
    }
    // NaN elevado a ±0: el EXE devuelve la base NaN silenciada
    if ((b & magnitudeMask) > infinity && (e & magnitudeMask) == 0) {
        const unsigned long long r = b | quietBit;
        double result;
        memcpy(&result, &r, 8);
        return result;
    }
    return pow(base, exponent);
}

__declspec(noinline) void __cdecl PowScaleVector3Cpp(float* vector, float damp, float step) {
    const double factor = ExeCompatiblePowCpp(double(damp), double(step));
    vector[0] = float(double(vector[0]) * factor);
    vector[1] = float(double(vector[1]) * factor);
    vector[2] = float(double(vector[2]) * factor);
}

__declspec(noinline) float __cdecl UnsignedMillisToSecondsCpp(unsigned int millis) {
    volatile float seconds = float(double(millis) * 0.001f); // 0x858CDC
    return seconds;
}

__declspec(noinline) float __cdecl Atan2MinusCpp(float num, float den, float sub) {
    volatile float result = float(atan2(double(num), double(den)) - double(sub));
    return result;
}

__declspec(noinline) float __cdecl GammaScaleCpp(double value) {
    volatile float scaled = float(value * 65535.0);
    return scaled;
}

__declspec(noinline) double __cdecl ExeAcosDoubleCpp(double x) {
    const unsigned previousControl = _control87(0, 0);
    _control87((previousControl & _MCW_PC) | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    volatile double result = atan2(sqrt((1.0 + x) * (1.0 - x)), x);
    _control87(previousControl, _MCW_PC | _MCW_RC | _MCW_EM);
    return result;
}

__declspec(noinline) float __cdecl ExeAcosCpp(float x) {
    volatile float result = float(ExeAcosDoubleCpp(double(x)));
    return result;
}

// --- Lote de diez archivos ---
__declspec(noinline) double __cdecl InterestingSectorCpp(float coordinate, float radius, bool useEnd) {
    const double edge = useEnd ? double(coordinate) + double(radius) : double(coordinate) - double(radius);
    volatile double result = edge * 0.02f + 60.0f;
    return result;
}

__declspec(noinline) float __cdecl TrigIntSinMulCpp(int i, float k) {
    return float(sin(double(i) * double(k)));
}

__declspec(noinline) float __cdecl IntTimesFloatCpp(int n, float f) {
    volatile float result = float(double(n) * double(f));
    return result;
}

// El orden estricto es el del EXE: con /fp:fast prevHP - (prevHP - factor) se simplificaría a factor.
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl FallRemainingHealthCpp(float prevHP, float factor, float minimumHP, float* remaining, float* damage) {
    double left = double(prevHP) - double(factor);
    if (double(minimumHP) > left) {
        left = double(minimumHP);
    }
    *remaining = float(left);
    *damage = float(double(prevHP) - left);
}
#pragma float_control(pop)

__declspec(noinline) void __cdecl SpriteBilinearOffsetCpp(float* vertices, float width, float height) {
    const double du = 1.0f / (double(width) + double(width));
    const double dv = 1.0f / (double(height) + double(height));
    // offsets de los bytes 0x14, 0x18, 0x30, 0x34, 0x4C, 0x50, 0x68 y 0x6C
    vertices[5] = float(double(vertices[5]) + du);
    vertices[6] = float(double(vertices[6]) + dv);
    vertices[12] = float(double(vertices[12]) + du);
    vertices[13] = float(double(vertices[13]) + dv);
    vertices[19] = float(double(vertices[19]) + du);
    vertices[20] = float(double(vertices[20]) + dv);
    vertices[26] = float(double(vertices[26]) + du);
    vertices[27] = float(double(vertices[27]) + dv);
}

__declspec(noinline) void __cdecl ScriptAngledCornersCpp(float angle, float startX, float startY, float endX, float endY, float width, float* corners) {
    const double twoPi = 0x1.921FB6p+2f; // 0x40C90FDB
    double a = double(angle) + double(0x1.921FB6p+0f); // + pi/2 (0x3FC90FDB)
    while (a < 0.0) {
        a += twoPi;
    }
    while (a > twoPi) {
        a -= twoPi;
    }
    const double sinWidth = sin(a) * double(width);
    corners[2] = float(double(endX) + sinWidth);
    const double minusCosWidth = -(cos(a) * double(width));
    volatile float storedMinusCos = float(minusCosWidth);
    corners[3] = float(minusCosWidth + double(endY));
    corners[0] = float(sinWidth + double(startX));
    corners[1] = float(double(storedMinusCos) + double(startY));
}

// --- Lote de trece archivos de tareas ---
__declspec(noinline) float __cdecl TrigAtan2Cpp(float y, float x) {
    return float(atan2(double(y), double(x)));
}

#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl DistanceSquaredZXYCpp(const float* a, const float* b) {
    const double dx = double(a[0]) - double(b[0]);
    const double dy = double(a[1]) - double(b[1]);
    const double dz = double(a[2]) - double(b[2]);
    volatile float result = float((dz * dz + dx * dx) + dy * dy);
    return result;
}
#pragma float_control(pop)

__declspec(noinline) void __cdecl PowDecaySpeedCpp(float* speed, float* shift, float step) {
    const double scaled = ExeCompatiblePowCpp(0.9599999785423279, double(step)) * double(*speed);
    *speed = float(scaled);
    *shift = float(scaled * double(step) + double(*shift));
}

__declspec(noinline) float __cdecl IntRatioCpp(int num, int den) {
    volatile float result = float(double(num) / double(den));
    return result;
}

// --- Lote de dieciséis archivos, fase A ---
static constexpr float kF_3727C5AC = std::bit_cast<float>(0x3727C5ACu);
static constexpr float kF_3C8EFA2E = std::bit_cast<float>(0x3C8EFA2Eu);
static constexpr float kF_3E22F983 = std::bit_cast<float>(0x3E22F983u);
static constexpr float kF_40490FDB = std::bit_cast<float>(0x40490FDBu);
static constexpr float kF_40C90FDB = std::bit_cast<float>(0x40C90FDBu);
static constexpr float kF_42652EE0 = std::bit_cast<float>(0x42652EE0u);
static constexpr float kF_42652EED = std::bit_cast<float>(0x42652EEDu);

__declspec(noinline) double __cdecl ExeAsinDoubleCpp(double x) {
    const unsigned previousControl = _control87(0, 0);
    _control87((previousControl & _MCW_PC) | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    volatile double angle = atan2(x, sqrt((1.0 + x) * (1.0 - x)));
    _control87(previousControl, _MCW_PC | _MCW_RC | _MCW_EM);
    return angle;
}

__declspec(noinline) float __cdecl ExeAsinDegreesCpp(float x) {
    return float(ExeAsinDoubleCpp(double(x)) * kF_42652EE0); // 57.29578f (0x859878), después de restaurar la palabra de control
}

__declspec(noinline) float __cdecl UnsignedPlusFloatCpp(int value, float delta) {
    return float(double(static_cast<unsigned int>(value)) + double(delta));
}

__declspec(noinline) float __cdecl FloatPlusIntCpp(float f, int i) {
    volatile float result = float(double(f) + double(i));
    return result;
}

__declspec(noinline) float __cdecl IntTimesTwoFloatsCpp(int n, float a, float b) {
    volatile float result = float(double(n) * double(a) * double(b));
    return result;
}

// sqrt(dist² + 1) con fsqrt en línea (fuera del modo precise, que lo convertiría en una llamada a la CRT)
static __declspec(noinline) double __cdecl FxFrustumHypotenuse(float dist) {
    volatile double result = sqrt(double(dist) * double(dist) + 1.0);
    return result;
}

// Orden estricto del EXE; las funciones trigonométricas van en llamadas aparte (en modo precise se convertirían en llamadas a la CRT).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl CurvesCorrectedDistCpp(float current, float total, float speedVariation, float* outT) {
    if (total < kF_3727C5AC) { // 1e-5 (0x858C14)
        *outT = 0.5f;
        return 0.0f;
    }
    const double ratio = double(current) / double(total);
    *outT = float(0.5 - TrigSimpleCosCpp(float(kF_40490FDB * ratio)) * 0.5); // pi (0x858CB8)
    const double sine = TrigSimpleSinCpp(float(ratio * kF_40C90FDB));          // 2pi (0x858CBC)
    const double scaledSine = sine * (double(total) * kF_3E22F983);            // 1/(2pi) (0x8594F0)
    const double blend = ((1.0 - (double(speedVariation) + double(speedVariation))) + 1.0) * 0.5;
    return float(scaledSine * double(speedVariation) + blend * double(current));
}

__declspec(noinline) float __cdecl FxFrustumRadiusCpp(float dist, float farClip) {
    const float angle = float(TrigAtan2Cpp(dist, 1.0f) * kF_42652EED);           // 0x42652EED
    const double hypotenuse = FxFrustumHypotenuse(dist);
    const double scaled = hypotenuse * double(farClip);
    const double sinSupplement = TrigSimpleSinCpp(float((180.0 - (double(angle) + double(angle))) * kF_3C8EFA2E)); // 0x3C8EFA2E
    const double sinAngle = TrigSimpleSinCpp(float(double(angle) * kF_3C8EFA2E));
    return float((scaled / sinSupplement) * sinAngle);
}
#pragma float_control(pop)

// fld [a]; fcomp [b]; fnstsw ax (CTaskComplexSeekEntity<XYOffset>::SetMaxEntityDist2D, 0x4BC470): palabra de estado x87 tras comparar.
// Se compara en x87 (el archivo se compila con /arch:IA32) para que el cargador marque IE y DE como lo hace el EXE; las condiciones C0, C2 y C3
// salen del resultado de la comparación y los indicadores de excepción acumulados se leen del x87 con _statusfp2. TOP = 0 y C1 = 0.
#pragma float_control(precise, on, push)
__declspec(noinline) unsigned int __cdecl X87CompareStatusCpp(const float* a, const float* b) {
    unsigned int status = 0;
    if (!(*a == *a) || !(*b == *b)) {
        status = 0x4500; // desordenado
    } else if (*a == *b) {
        status = 0x4000;
    } else if (*a < *b) {
        status = 0x0100;
    }
    unsigned int x87 = 0, sse = 0;
    _statusfp2(&x87, &sse);
    if (x87 & _SW_INVALID)    status |= 0x01;
    if (x87 & _SW_DENORMAL)   status |= 0x02;
    if (x87 & _SW_ZERODIVIDE) status |= 0x04;
    if (x87 & _SW_OVERFLOW)   status |= 0x08;
    if (x87 & _SW_UNDERFLOW)  status |= 0x10;
    if (x87 & _SW_INEXACT)    status |= 0x20;
    return status;
}
#pragma float_control(pop)

// --- Residuo de registros ---
__declspec(noinline) unsigned int __fastcall ResidueRegsCpp(unsigned int /*ecxValue*/, unsigned int /*edxValue*/, unsigned int eaxValue) {
    return eaxValue;
}

__declspec(noinline) unsigned int __thiscall ResidueEcx::Get(unsigned int eaxValue) {
    return eaxValue;
}

// EAX de entrada de la función que lo llama, si es lo primero que ésta ejecuta: no tiene `return`, de modo que MSVC no escribe EAX y sale
// (`ret`, con o sin marco de pila). Es comportamiento sin definir en el lenguaje: lo vigilan la comprobación de bytes de abajo, al cargar la
// DLL, y contrib/audit_register_residue.py en los objetos que lo usan.
#pragma warning(push)
#pragma warning(disable : 4716)
__declspec(noinline) unsigned int __fastcall EntryEaxCpp() {
}
#pragma warning(pop)

// Comprobación al cargar la DLL (se activa con assert): los dos ayudantes tienen que ser exactamente "devolver el parámetro de la pila y salir",
// sin tocar ECX ni EDX. Si un cambio de opciones o de compilador los altera, las funciones que reproducen el residuo del EXE dejarían
// registros equivocados sin avisar. Se siguen los saltos de enlace incremental (E9) que pone el modo Debug.
static const unsigned char* FollowIncrementalThunks(const unsigned char* code) {
    for (int hops = 0; hops < 4 && code[0] == 0xE9; ++hops) {
        int offset;
        memcpy(&offset, code + 1, sizeof offset);
        code += 5 + offset;
    }
    return code;
}

static bool ResidueHelperIsTrivial(const void* function) {
    static const unsigned char withFrame[]    = {0x55, 0x8B, 0xEC, 0x8B, 0x45, 0x08, 0x5D, 0xC2, 0x04, 0x00}; // push ebp; mov ebp,esp; mov eax,[ebp+8]; pop ebp; ret 4
    static const unsigned char withoutFrame[] = {0x8B, 0x44, 0x24, 0x04, 0xC2, 0x04, 0x00};                  // mov eax,[esp+4]; ret 4
    const unsigned char* code = FollowIncrementalThunks(static_cast<const unsigned char*>(function));
    return memcmp(code, withFrame, sizeof withFrame) == 0 || memcmp(code, withoutFrame, sizeof withoutFrame) == 0;
}

static bool EntryEaxHelperIsTrivial() {
    const unsigned char* code = FollowIncrementalThunks(reinterpret_cast<const unsigned char*>(&EntryEaxCpp));
    static const unsigned char plain[]     = {0xC2, 0x00, 0x00};             // ret 0 (así termina MSVC una __fastcall sin parámetros de pila)
    static const unsigned char withFrame[] = {0x55, 0x8B, 0xEC, 0x5D, 0xC2, 0x00, 0x00}; // push ebp; mov ebp,esp; pop ebp; ret 0
    return code[0] == 0xC3 || memcmp(code, plain, sizeof plain) == 0 || memcmp(code, withFrame, sizeof withFrame) == 0;
}

static bool ResidueHelpersAreTrivial() {
    auto  member = &ResidueEcx::Get;
    const void* getAddress;
    memcpy(&getAddress, &member, sizeof getAddress);
    return ResidueHelperIsTrivial(reinterpret_cast<const void*>(&ResidueRegsCpp)) && ResidueHelperIsTrivial(getAddress) && EntryEaxHelperIsTrivial();
}

static const bool s_residueHelpersChecked = (assert(ResidueHelpersAreTrivial() && "Los ayudantes de residuo de registros ya no son triviales: revisar X87Math.cpp y las opciones de compilacion"), true);

// Orden estricto del EXE (a·b)·c·k; el acos va en una llamada aparte.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl IKChainRotationAngleCpp(float boneSpeed, float dot, float chainSpeed) {
    return float(((double(boneSpeed) * ExeAcosDoubleCpp(double(dot))) * double(chainSpeed)) * kF_42652EE0);
}
#pragma float_control(pop)

// --- Lote de seis archivos de audio (12/10/2026) ---
// Entero sin signo como lo carga el EXE: fild (con signo) y, si el bit alto está puesto, fadd 2^32 en la precisión vigente.
static __forceinline double UnsignedAsX87(unsigned int value) {
    double result = double(static_cast<int>(value));
    if (static_cast<int>(value) < 0) {
        result += 4294967296.0f;
    }
    return result;
}


__declspec(noinline) float __cdecl UnsignedTimesFloatCpp(unsigned int value, float factor) {
    volatile float result = float(UnsignedAsX87(value) * double(factor));
    return result;
}

__declspec(noinline) long __cdecl SecondsToMsCpp(double seconds) {
    volatile double ms = seconds * 1000.0; // 0x85A318
    return static_cast<long>(static_cast<long long>(ms));
}

__declspec(noinline) long __cdecl BytesToMsCpp(unsigned int bytes, unsigned int bytesPerSecond) {
    const double bytesValue = UnsignedAsX87(bytes);
    const double rateValue  = UnsignedAsX87(bytesPerSecond);
    const double ratio      = bytesValue / rateValue;
    volatile double ms = ratio * 1000.0f;
    return static_cast<long>(static_cast<long long>(ms));
}

// Sin reasociación: el EXE multiplica (posición · 0.001f) y luego por la tasa, cada paso a 24 bits.
#pragma float_control(precise, on, push)
__declspec(noinline) int __cdecl WaveCursorBytesCpp(unsigned int position, unsigned int bytesPerSecond) {
    double bytes = UnsignedAsX87(position) * 0.001f; // 0x858CDC
    bytes        = bytes * UnsignedAsX87(bytesPerSecond);
    bytes        = bytes + 44.0f;
    volatile double stored = bytes;
    return static_cast<int>(static_cast<long long>(stored));
}
#pragma float_control(pop)

// start + 20·log10(1 + (10^(diff·0.05) - 1) · transcurrido / duración) (CAESmoothFadeThread::Service)
__declspec(noinline) float __cdecl SmoothFadeVolumeCpp(float diff, float start, unsigned int elapsed, int fadeTime) {
    const float  exponent = float(double(diff) * 0.05f); // 0x3D4CCCCD
    double       step     = ExeCompatiblePowCpp(10.0, double(exponent));
    step                  = step - 1.0f;
    const double progress = UnsignedAsX87(elapsed) / double(fadeTime);
    step                  = step * progress;
    step                  = step + 1.0f;
    const double decibels = log10(step) * 20.0f;
    volatile float result = float(decibels + double(start));
    return result;
}

__declspec(noinline) float __cdecl OneMinusUnsignedRatioCpp(unsigned int numerator, unsigned int denominator) {
    const double numeratorValue   = UnsignedAsX87(numerator);
    const double denominatorValue = UnsignedAsX87(denominator);
    const double ratio            = numeratorValue / denominatorValue;
    volatile float result = float(1.0f - ratio);
    return result;
}

// --- Lote de cinco archivos (14/10/2026): Quaternion, Boat, Trailer, Event ---
// Cuaternión de eje y ángulo (CQuaternion::Set, 0x59C600): {sin(a/2)·eje, cos(a/2)}; el seno queda sin redondear en ST0 para las tres multiplicaciones (de stash@{0}, verificado).
__declspec(noinline) void __cdecl QuaternionFromAxisAngleCpp(float* out, const float* axis, float angle) {
    const double half = double(angle) * 0.5f;
    const double s    = sin(half);
    out[0]            = float(s * double(axis[0]));
    out[1]            = float(s * double(axis[1]));
    out[2]            = float(s * double(axis[2]));
    out[3]            = float(cos(half));
}

// float(from - atan2(num, den)) con el atan2 (fpatan) sin redondear y un solo redondeo a float (CBoat::ProcessControl, rumbo bloqueado, 0x6F2060).
__declspec(noinline) float __cdecl Atan2FromCpp(float num, float den, float from) {
    return float(double(from) - atan2(double(num), double(den)));
}

// *value = float(pow(base, step) * *value) con el factor de _CIpow (0x822130) en ST0 sin redondear (CBoat::ProcessControlInputs, 0x6F0C14).
__declspec(noinline) void __cdecl PowScaleFloatCpp(float* value, float base, float step) {
    *value = float(ExeCompatiblePowCpp(double(base), double(step)) * double(*value));
}

// CTrailer::ProcessAI (0x6CF5FC): rumbo del tractor (A) corregido en ±2π contra el propio (B); devuelve A' - B con A y B sin redondear.
// Un rumbo es atan2(-fwd.x, fwd.y) con fpatan (forward != nullptr apunta a x, y consecutivos) o el rumbo compacto de m_placement si no hay matriz.
// A y B deben quedar en la pila x87 (sin fstp qword entre los dos fpatan) y la comparación no debe reasociarse: revisar el .obj.
__declspec(noinline) float __cdecl TrailerSteerCpp(const float* tractorForward, float tractorHeading, const float* selfForward, float selfHeading) {
    double a = tractorForward ? atan2(-double(tractorForward[0]), double(tractorForward[1])) : double(tractorHeading);
    const double b = selfForward ? atan2(-double(selfForward[0]), double(selfForward[1])) : double(selfHeading);
    const double piPlusB = double(kF_40490FDB) + b;   // 0x858CB8
    if (a > piPlusB) {
        a = a - double(kF_40C90FDB);                  // 0x858CBC
    } else {
        const double bMinusPi = b - double(kF_40490FDB);
        if (a < bMinusPi) {
            a = a + double(kF_40C90FDB);
        }
    }
    return float(a - b);
}

// 10 · log10(x) con fldlg2/fyl2x a precisión completa, producto por 10.0f (0x85862C) redondeado una vez y guardado como float (CEvent::GetSoundLevel, 0x4B2850).
__declspec(noinline) float __cdecl Log10Times10Cpp(float x) {
    volatile float result = float(log10(double(x)) * 10.0f); // 0x85862C
    return result;
}

// 10 · log10((power2 + power1) / power1): suma y cociente redondeados por la x87, fyl2x completo, producto por 10.0f (0x85862C) y un solo redondeo a float (CEvent::CalcSoundLevelIncrement, 0x4AC050).
__declspec(noinline) float __cdecl SoundLevelIncrementCpp(double power2, float power1) {
    const double quotient = (power2 + double(power1)) / double(power1);
    volatile float result = float(log10(quotient) * 10.0f); // 0x85862C
    return result;
}


// --- Lote de cinco archivos (14/10/2026, 2): Fire, PlayerInfo, PointLights, Replay ---
// --- CFire (Fire.cpp) --- (colocar DESPUES de UnsignedAsX87, que es static y está definido más arriba)
// CFireManager::StartFire sin objetivo (0x539F00..0x539FD0): ((rnd · factorA) · factorB + offset) · tiempo + ahora. Cada operación se redondea a 24 bits
// (palabra de control del proceso); los enteros sin signo entran como fild + 2^32. El _ftol2 posterior lo hace el llamador sobre el float devuelto
// (el valor de ST0 ya está redondeado a 24 bits, así que guardarlo como float no cambia nada). precise evita que /fp:fast reasocie los productos.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl FireBurnTimeCpp(int rnd, float factorA, float factorB, float offset, unsigned int burnTime, unsigned int nowMs) {
    double chance = double(rnd) * double(factorA);
    chance = chance * double(factorB);
    chance = chance + double(offset);
    double total = chance * UnsignedAsX87(burnTime);
    total = total + UnsignedAsX87(nowMs);
    volatile float result = float(total);
    return result;
}
#pragma float_control(pop)

// CFire::ProcessFire (0x53AD79..0x53AD8F): fld strength; fisub whole (entero exacto, un solo redondeo a 24 bits). whole = 32 bits bajos del _ftol2.
__declspec(noinline) float __cdecl IntFractionCpp(float value, int whole) {
    volatile float result = float(double(value) - double(whole));
    return result;
}

// CPlayerInfo::EvaluateCarPosition (0x56DAD0): float(from - atanFn(x, y)). atanFn es CGeneral::GetATanOfXY (0x53CC70), que devuelve el fpatan
// sin redondear en ST0; la resta (fsubr) lo consume directamente y solo se redondea el resultado. Revisar en el .obj: call, fsub/fsubr de "from",
// fstp, sin ningun fstp/fld entre la llamada y la resta.
__declspec(noinline) float __cdecl PlayerInfoAtanFromCpp(float (__cdecl* atanFn)(float, float), float from, float x, float y) {
    volatile float result = float(double(from) - double(atanFn(x, y)));
    return result;
}

// --- CPointLights::AddLight (0x7000E0) ---
// Fase 1 (0x7000F5..0x700170): offsetX = x - camX y limit = radio + 15 (0x858B48) se guardan como float; se sale salvo que
// -limit < offsetX < limit y lo mismo para offsetY. Las comparaciones son estrictas y dan false con NaN (FCOMP + TEST AH,5 / 41h).
// offsetY se compara sin redondear (FST + FCOMP) y se vuelve a cargar de memoria para el límite inferior. camera apunta a x, y, z.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsInRangeXYCpp(const float* camera, float pointX, float pointY, float radius, float* outOffsetX, float* outOffsetY, float* outLimit) {
    volatile float offsetX = pointX - camera[0];
    volatile float limit = radius + 15.0f;
    if (!(offsetX < limit)) {
        return false;
    }
    volatile float lowerLimit = -limit;
    if (!(offsetX > lowerLimit)) {
        return false;
    }
    const float rawOffsetY = pointY - camera[1];
    volatile float offsetY = rawOffsetY;
    if (!(rawOffsetY < limit)) {
        return false;
    }
    if (!(offsetY > lowerLimit)) {
        return false;
    }
    *outOffsetX = offsetX;
    *outOffsetY = offsetY;
    *outLimit = limit;
    return true;
}
#pragma float_control(pop)

// (dz·dz + oy·oy) + ox·ox sin redondear a float, en el orden del EXE (0x700197..0x7001B9); devuelve double para no redondear.
#pragma float_control(precise, on, push)
static __declspec(noinline) double PointLightsSquaredDistance(float dz, float offsetY, float offsetX) {
    return (dz * dz + offsetY * offsetY) + offsetX * offsetX;
}
// Con distance < limit: false si no; true y, si limit·0.75 (0x858F34) <= distance, atenúa RGB con 1 - ((distance / limit - 0.75) · 4.0 (0x858B90))
// (0x70027C..0x7002B9). El coeficiente sigue en ST0 sin guardarlo: red, green y blue se multiplican por él y se guardan como float.
static __declspec(noinline) bool PointLightsClassifyFade(float distance, float limit, float* red, float* green, float* blue) {
    if (!(distance < limit)) {
        return false;
    }
    if (limit * 0.75f > distance) {
        return true;
    }
    const float factor = 1.0f - (distance / limit - 0.75f) * 4.0f;
    *red = *red * factor;
    *green = *green * factor;
    *blue = factor * *blue;
    return true;
}
#pragma float_control(pop)

// Fase 2: la raíz va fuera de las regiones precise para que salga FSQRT en línea; se guarda como float (FST) y se pasa por valor.
__declspec(noinline) bool __cdecl PointLightsDistanceFadeCpp(float pointZ, const float* camera, float offsetX, float offsetY, float limit, float* red, float* green, float* blue) {
    const double squared = PointLightsSquaredDistance(pointZ - camera[2], offsetY, offsetX);
    volatile float distance = float(sqrt(squared));
    return PointLightsClassifyFade(distance, limit, red, green, blue);
}

// --- Lote de Replay.cpp (14/10/2026) ---
// CReplay::ProcessLookAroundCam (0x45D760..0x45DA7A): cos(view) queda en ST1 sin redondear para los dos primeros productos; cada producto
// (sin·cos, ·dist) se redondea con la precisión de 24 bits del proceso y se guarda como float. Orden de multiplicación del EXE:
// (sin(dir)·cos(view))·dist, (cos(dir)·cos(view))·dist, sin(view)·dist. Revisar el .obj: sin fsincos, sin fstp qword de cos(view)
// y sin que /fp:fast reasocie (cos(view)·dist) compartido.
__declspec(noinline) void __cdecl ReplayLookAroundOffsetsCpp(float view, float dirAngle, float dist, float* offsets) {
    const double cosView = cos(double(view));
    double       x       = sin(double(dirAngle)) * cosView;
    x                    = x * double(dist);
    double y             = cos(double(dirAngle)) * cosView;
    y                    = y * double(dist);
    offsets[0]           = float(x);
    offsets[1]           = float(y);
    offsets[2]           = float(sin(double(view)) * double(dist));
}


// --- Lote de cinco archivos (14/10/2026, 3): Ropes, ShotInfo, SimpleTransform, Sprite ---
// CRopes::FindCoorsAlongRope (0x555E40..0x555F4C). dist·31 queda sin redondear en ST0, t = dist31 - segIdx (fisub),
// t·pb.x sin redondear a float, t·pb.y y t·pb.z guardados como float, (1 - t) sin redondear, u·pa.{x,y,z} guardados como float; resultado = float(uxv + t·pb.x), float(uyv + tyv), float(uzv + tzv).
// Las variables intermedias de memoria son volatile para forzar el fstp/fld del EXE; el resto es double (ST0 a 24 bits de mantisa y exponente extendido).
#pragma float_control(precise, on, push)
__declspec(noinline) int __cdecl RopesInterpolateCpp(float dist, const float* segments, float* out) {
    const double dist31 = double(dist) * 31.0;
    const int segIdx = (int)dist31; // _ftol2: trunca (NaN -> 0x80000000, como el EXE)
    // aritmética de punteros de 32 bits con vuelta, como el lea del EXE
    const float* pa = reinterpret_cast<const float*>(reinterpret_cast<uintptr_t>(segments) + static_cast<unsigned>(segIdx) * 12u);
    const float* pb = reinterpret_cast<const float*>(reinterpret_cast<uintptr_t>(pa) + 12u);
    const double t  = dist31 - double(segIdx);
    const double tx = t * double(pb[0]);                 // queda en la pila, sin redondear
    volatile float tyv = float(t * double(pb[1]));
    volatile float tzv = float(t * double(pb[2]));
    const double u = 1.0 - t;                            // fld 1.0 ; fsub st(2)
    volatile float uxv = float(u * double(pa[0]));
    volatile float uyv = float(u * double(pa[1]));
    volatile float uzv = float(u * double(pa[2]));
    const float rx = float(double(uxv) + tx);
    const float ry = float(double(uyv) + double(tyv));
    const float rz = float(double(uzv) + double(tzv));
    out[0] = rx;
    out[1] = ry;
    out[2] = rz;
    return segIdx;
}
#pragma float_control(pop)

// CShotInfo::AddShot (0x739CE8..0x739DD3): m_DestroyTime = float(_ftol(vida + tiempo)). El tiempo sin signo se carga con fild (entero con signo, exacto)
// y, si es negativo, se le suma 2^32 (redondeo a 24 bits); luego fadd con la vida (un redondeo) y _ftol (truncado a 64 bits, se usa EAX) y fild/fstp a float.
__declspec(noinline) float __cdecl ShotInfoDestroyTimeCpp(float lifespan, unsigned int nowMs) {
    volatile float sum = float(UnsignedAsX87(nowMs) + double(lifespan));
    const int destroy  = static_cast<int>(static_cast<long long>(sum));
    volatile float result = float(destroy);
    return result;
}

// CSimpleTransform::Invert (0x54EF90): base = {x, y, z, rumbo}, out = {x, y}. cos/sin quedan sin redondear en la x87 (no se guardan como float);
// out[0] se escribe antes de releer base[0] para y, como el EXE (la salida puede ser la entrada).
__declspec(noinline) void __cdecl SimpleTransformInvertCpp(const float* base, float* out) {
    const double c = cos(double(base[3]));
    const double s = sin(double(base[3]));
    out[0] = float(-(c * double(base[0])) - s * double(base[1]));
    out[1] = float(s * double(base[0]) - c * double(base[1]));
}

// Coeficientes de las cuatro esquinas de un sprite girado: {-cos - sin, sin - cos, cos + sin, cos - sin}.
__declspec(noinline) void __cdecl SpriteQuadCoeffsCpp(float rotation, float* coeffs) {
    const double s = sin(double(rotation));
    const double c = cos(double(rotation));
    coeffs[0] = float(-c - s);
    coeffs[1] = float(s - c);
    coeffs[2] = float(c + s);
    coeffs[3] = float(c - s);
}

// {w·cos, sin·h, cos·h, sin·w}
__declspec(noinline) void __cdecl SpriteDimensionTermsCpp(float rotation, float width, float height, float* terms) {
    const double s = sin(double(rotation));
    const double c = cos(double(rotation));
    terms[0] = float(c * double(width));
    terms[1] = float(s * double(height));
    terms[2] = float(c * double(height));
    terms[3] = float(s * double(width));
}


// --- Lote de cuatro archivos (14/10/2026, 4): common, TaskComplexDestroyCarArmed, TimeCycle ---
// TransformPoint (0x54ECE0): transform = {x, y, z, rumbo}; cos/sin quedan sin redondear en la x87 (fcos/fsin, sin fstp intermedio).
// x se escribe antes de releer in[0]/in[1] para y, y z se lee al final: la salida puede ser la entrada (los accesos no se pueden reordenar por el posible alias).
__declspec(noinline) void __cdecl CommonTransformPointCpp(float* out, const float* transform, const float* in) {
    const double c = cos(double(transform[3]));
    const double s = sin(double(transform[3]));
    out[0] = float((c * double(in[0]) - s * double(in[1])) + double(transform[0]));
    out[1] = float((c * double(in[1]) + s * double(in[0])) + double(transform[1]));
    out[2] = float(double(transform[2]) + double(in[2]));
}

// CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges (0x628C80..0x628D2D): fld z/y/x; x·x; + y·y; + z·z; fsqrt; fstp.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl DestroyCarDistanceCpp(const float* v) {
    return float(sqrt(double((v[0] * v[0] + v[1] * v[1]) + v[2] * v[2])));
}
#pragma float_control(pop)

// CTimeCycle::Initialise (0x5BC026): dirección de la luz {cos(b)·cos(a), sin(b)·cos(a), sin(a)}; cos(a) queda en ST1 sin redondear y cada producto se redondea una vez (24 bits) al guardarlo.
__declspec(noinline) void __cdecl TimeCycleDirnToSunCpp(double angleA, double angleB, float* out) {
    const double cosA = cos(angleA);
    out[0] = float(cos(angleB) * cosA);
    out[1] = float(sin(angleB) * cosA);
    out[2] = float(sin(angleA));
}

// CTimeCycle::CalcColoursForPoint (0x560A80): x = sunX + sin(a), z = sunZ - cos(a) con seno y coseno sin redondear y un solo redondeo a float cada uno.
__declspec(noinline) void __cdecl TimeCycleVectorToSunCpp(float angle, float sunX, float sunZ, float* outX, float* outZ) {
    *outX = float(sin(double(angle)) + double(sunX));
    *outZ = float(double(sunZ) - cos(double(angle)));
}


// Common::TransformVectors (0x54EE30), bucle: x = cos·in.x - sin·in.y y luego y = sin·in.x + cos·in.y, con x escrito antes de releer in.x (la salida puede ser la entrada) y z copiada como dword.
// Con la x87 (y no con SSE) los NaN se propagan como en el EXE (cargas utiles y signos). El contador decrece hasta cero como dec/jne.
__declspec(noinline) void __cdecl CommonTransformVectorsCpp(float* out, int numVectors, float cosHeading, float sinHeading, const float* in) {
    for (unsigned int remaining = static_cast<unsigned int>(numVectors); remaining != 0; --remaining) {
        out[0] = float(double(cosHeading) * double(in[0]) - double(sinHeading) * double(in[1]));
        out[1] = float(double(sinHeading) * double(in[0]) + double(cosHeading) * double(in[1]));
        unsigned int z;
        memcpy(&z, &in[2], sizeof(z));
        memcpy(&out[2], &z, sizeof(z));
        out += 3;
        in += 3;
    }
}

// --- Lote de diecinueve archivos (14/10/2026, 5) ---
// CAEPedAudioEntity::UpdateJetPack (0x4E1068..0x4E10A2): fsin sin redondear; fst al float de sinStored; fchs solo si sin < 0; fmul k; fadd add; un redondeo a float.
__declspec(noinline) float __cdecl PedAudioJetPackSinCpp(float angle, float k, float add, float* sinStored) {
    double s = sin(double(angle));
    *sinStored = float(s);
    if (s < 0.0) s = -s;
    volatile float result = float(s * double(k) + double(add));
    return result;
}

// --- CAEMFDecoder (0x4E7940, 0x4E78E0) ---
// GetStreamPlayTimeMs: fild · fdiv por el divisor · _ftol2; con la x87 a 24 bits la división ya sale redondeada una sola vez.
__declspec(noinline) long __cdecl MfDecoderPlayTimeMsCpp(int playTime, float divider) {
    volatile float ms = float(double(playTime) / double(divider));
    return static_cast<long>(static_cast<long long>(ms));
}

// SetCursor: entero sin signo como fild (+ 2^32 si el bit alto está puesto) · multiplicador · _ftol2; igual que UnsignedTimesFloatCpp.
__declspec(noinline) long __cdecl MfDecoderCursorToPlayTimeCpp(unsigned int pos, float multiplier) {
    return static_cast<long>(static_cast<long long>(UnsignedTimesFloatCpp(pos, multiplier)));
}

// --- CarAI (0x41CD00): forward sin matriz sin redondear ---
// -sin y cos en funciones aparte SIN float_control(precise) (con precise sin/cos pasarían a llamadas a la CRT en vez de fsin/fcos).
// Devuelven double (ST0 sin redondear a float), como los deja el EXE en el registro del x87.
__declspec(noinline) static double __cdecl CarAINegSinExtCpp(float heading) { return -sin(double(heading)); }
__declspec(noinline) static double __cdecl CarAICosExtCpp(float heading) { return cos(double(heading)); }

// EXE: fld h; fsin; fchs; fld h; fcos; fldz; fld dz; fmul st1; fxch st2; fmul dy; faddp st2; fxch st2; fmul dx; faddp st1; fstp r.
// Orden fijo: (dz*0 + cos*dy) + (-sin)*dx; dz*0 se conserva (NaN/inf en dz propagan como en el EXE, con precise no se elimina).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl CarAIHeadOnDotNoMatrixCpp(float heading, float dx, float dy, float dz) {
    const double negSin = CarAINegSinExtCpp(heading);
    const double cosine = CarAICosExtCpp(heading);
    return float((double(dz) * 0.0 + cosine * double(dy)) + negSin * double(dx));
}
#pragma float_control(pop)

// fld x; fchs; fld y; fpatan; fmul [0x85A778 = 0x4222F983]; jmp _ftol2 (0x821B40). fpatan no depende del control de precisión: sale sin redondear
// y solo el fmul (24 bits) redondea el producto; el truncamiento a 64 bits es el de _ftol2 (NaN/infinito -> 0x8000000000000000 -> 0 en los 32 bits bajos).
static __declspec(noinline) float __cdecl CoverAtan2ScaledCpp(float num, float den) {
    const float scale = std::bit_cast<float>(0x4222F983u);
    return float(atan2(double(num), double(den)) * double(scale));
}
__declspec(noinline) int __cdecl CoverDirFromVectorCpp(float x, float y) {
    volatile float scaled = CoverAtan2ScaledCpp(-x, y);
    const float value = scaled;
    return int(static_cast<long long>(value));
}

// fsub x3; fild dirIndex; fmul [0x859BBC = 0x3CC90FDB]; fsin; fchs; fcos -> float; ((dy * cos) + (-sin * dx)) + (dz * 0). Con precise el orden
// y el dz * 0 (NaN/infinito) se conservan; el seno/coseno van a ayudantes aparte (sin precise), el producto -sin * dx usa TrigSimpleSinMulCpp con k = -dx.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl CoverDirDotCpp(float dx, float dy, float dz, int dirIndex) {
    const float step = std::bit_cast<float>(0x3CC90FDBu);
    const float angle = IntTimesFloatCpp(dirIndex, step);
    const float cosValue = TrigSimpleCosCpp(angle);
    const float p1 = dy * cosValue;
    const float p2 = TrigSimpleSinMulCpp(angle, -dx);
    const float q = p1 + p2;
    const float zeroTerm = dz * 0.0f;
    volatile float result = q + zeroTerm;
    return result;
}
#pragma float_control(pop)

__declspec(noinline) float __cdecl EntityHeadingDegreesPlusCpp(float num, float den, float k, float base) {
    volatile float result = float(atan2(double(num), double(den)) * double(k) + double(base));
    return result;
}

// Lote ProcSurfaceInfo (precise para conservar el orden y las operaciones x87 del EXE; sin transcendentales).
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl ProcSurfaceScaleRowCpp(float* row, float xy, float z) {
    row[0] = xy * row[0];
    row[1] = xy * row[1];
    row[2] = z * row[2];
}
__declspec(noinline) void __cdecl ProcSurfaceOffsetZCpp(float* z, float minZ, float zScale) {
    *z = *z - (minZ * zScale - minZ);
}
#pragma float_control(pop)

// Requiere <cstdint>. Equivale a FLD dword [x]; FISTP dword [res] con la palabra de control por defecto del proceso (redondeo al par).
#pragma float_control(precise, on, push)
unsigned int __cdecl RwPlCoreFastRealToUInt32Cpp(float x) {
    constexpr float kMagic = 8388608.0f; // 2^23: por encima todo float es ya entero
    if (!(x < 2147483648.0f) || x < -2147483648.0f) { // fuera de rango, +inf, -inf o NaN
        return 0x80000000u;                            // entero indefinido de FISTP
    }
    const float ax = x < 0.0f ? -x : x;
    if (ax >= kMagic) {
        return static_cast<unsigned int>(static_cast<int>(x)); // ya entero: el truncado es exacto
    }
    volatile float t = ax + kMagic; // suma exacta en x87 y un unico redondeo a float = redondeo al par de la parte fraccionaria
    t = t - kMagic;
    const int r = static_cast<int>(t);
    return static_cast<unsigned int>(x < 0.0f ? -r : r);
}
#pragma float_control(pop)

// Va en X87Math.cpp (/O2 /Oi /arch:IA32 /fp:fast). El seno se guarda como float (volatile) antes de calcular el coseno, que se usa de inmediato sin guardarlo.
__declspec(noinline) void __cdecl TheScriptsAngledRectTermsCpp(float angle, float vAMx, float vAMy, float* terms) {
    volatile float sinStored = float(sin(double(angle)));
    const double s = double(sinStored);
    const double c = cos(double(angle));
    terms[0] = float(c * double(vAMy));
    terms[1] = float(s * double(vAMx));
    terms[2] = float(c * double(vAMx));
    terms[3] = float(s * double(vAMy));
}

// --- CAEAudioUtility (14/10/2026): tiempo, conversiones bytes/ms y tabla logarítmica ---
// Con /fp:fast se reasociaría el orden de las operaciones: se conserva el del EXE.
#pragma float_control(precise, on, push)
__declspec(noinline) unsigned long long __cdecl AudioUtilTimeMsCpp(long long counter, long long frequency, unsigned long long start) {
    const long long startLow = static_cast<long long>(start & 0x7FFFFFFFFFFFFFFFull); // qword (alta & 0x7FFFFFFF : baja)
    const long long startTop = static_cast<long long>(start & 0x8000000000000000ull); // qword (alta & 0x80000000 : 0), fchs después
    double elapsed           = double(counter) / double(frequency);                  // fild, fild [0xB610F0], fdivp
    elapsed                  = elapsed * 1000.0f;                                    // 0x858C4C
    const double startValue  = double(startLow) + -double(startTop);
    volatile double ms       = elapsed - startValue;
    return static_cast<unsigned long long>(static_cast<long long>(ms));              // _ftol2
}

__declspec(noinline) double __cdecl AudioUtilBytesToMsQuotientCpp(unsigned int bytes, unsigned int product) {
    const double bytesValue = UnsignedAsX87(bytes);
    const double scaled     = UnsignedAsX87(product) * 0.002f; // 0x858F44
    volatile double quotient = bytesValue / scaled;
    return quotient;
}

__declspec(noinline) double __cdecl AudioUtilMsToBytesProductCpp(unsigned int product, unsigned int ms) {
    const double scaled = UnsignedAsX87(product) * 0.002f; // 0x858F44
    volatile double bytes = scaled * UnsignedAsX87(ms);
    return bytes;
}
#pragma float_control(pop)

__declspec(noinline) float __cdecl AudioUtilLogInverseCpp(double denominator) {
    volatile float result = float(10.0f / denominator); // fdivr [0x85862C]; fst dword
    return result;
}

// --- CAEAudioHardware::RescaleChannelVolumes (0x4D8990) ---
// 0x4D8B58: fld 10.0 (0x859EA8); fld vol; fmul 0.05f (0x858C28); _CIpow (0x822130); fst [gain]; fadd/fstp [sum] o fstp st(0).
__declspec(noinline) void __cdecl AudioHwPowGainCpp(float vol, float* gain, float* sum) {
    const float  exponent = float(double(vol) * 0.05f); // 0x3D4CCCCD
    const double power    = ExeCompatiblePowCpp(10.0, double(exponent));
    *gain                 = float(power);
    if (sum) {
        *sum = float(power + double(*sum));
    }
}

// 0x4D8BC4: fild diff; fmul [gain]; fidiv divisor; recorte a [0, gain] (NaN conservado); fadd [sum]; fstp [sum].
// float_control(precise): sin reasociar (diff · gain) / divisor ni cambiar las comparaciones.
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl AudioHwFadeNearEndCpp(int diff, int divisor, const float* gain, float* sum) {
    const double g = double(*gain);
    double       r = double(diff) * g;
    r              = r / double(divisor);
    if (r < 0.0) {
        r = 0.0;
    } else if (r > g) {
        r = g;
    }
    *sum = float(r + double(*sum));
}
#pragma float_control(pop)

// --- Lote de IdleCam.cpp (14/10/2026): CIdleCam::ProcessFOVZoom y CIdleCam::ProcessSlerp ---
// El seno va en una función aparte (sin float_control precise) para que siga siendo el fsin nativo y no una llamada a la CRT.
__declspec(noinline) static double __cdecl IdleCamSinCpp(double angle) { return sin(angle); }

static constexpr float kIdleCamPi       = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
static constexpr float kIdleCamTwoPi    = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
static constexpr float kIdleCamDegToRad = std::bit_cast<float>(0x3C8EFA35u); // 0x8595EC

#pragma float_control(precise, on, push)
// CIdleCam::ProcessFOVZoom (0x517E2E..0x517E63): el cociente, el ángulo y el seno quedan en ST0 (24 bits de mantisa del proceso); fld to; fsub from; fmulp; fadd from; un solo fstp a float.
__declspec(noinline) float __cdecl IdleCamFovLerpCpp(float now, float started, float duration, float from, float to) {
    const double ratio  = (double(now) - double(started)) / double(duration);
    const double angle  = (270.0 - ratio * 180.0) * double(kIdleCamDegToRad);
    const double factor = (IdleCamSinCpp(angle) + 1.0) * 0.5;
    return float(factor * (double(to) - double(from)) + double(from));
}

// CIdleCam::ProcessSlerp (0x517A8E..0x517BD5). Las tres primeras llamadas a atanFn se guardan como float (+ pi las de d1/d2 en x/y); la cuarta (lookAtDistAtan)
// queda en ST0 y se ajusta en +-2PI según (look - slerp) > pi / < -pi (comparaciones ordenadas: con NaN no cambia). t se guarda como float y se limita a 1 solo si
// el valor sin redondear es > 1. Salidas: outX = (lookDist - slerpDist) · lerpT + slerpDist, outZ = (lookAtAtan - slerpAtan) · lerpT + slerpAtan.
__declspec(noinline) float __cdecl IdleCamSlerpCpp(float (__cdecl* atanFn)(float, float), const float* d1, const float* d2, float mag1, float mag2, float now, float timeLast, float duration, float* outX, float* outZ) {
    const double pi    = double(kIdleCamPi);
    const double twoPi = double(kIdleCamTwoPi);

    const float slerpAtan  = float(double(atanFn(d1[0], d1[1])) + pi);
    const float slerpDist  = atanFn(mag1, d1[2]);
    const float lookAtAtan = float(double(atanFn(d2[0], d2[1])) + pi);
    double lookDist        = double(atanFn(mag2, d2[2]));

    const double diffDist = lookDist - double(slerpDist);
    if (diffDist > pi) {
        lookDist -= twoPi;
    } else if (diffDist < -pi) {
        lookDist += twoPi;
    }

    double lookAng        = double(lookAtAtan);
    const double diffAtan = lookAng - double(slerpAtan);
    if (diffAtan > pi) {
        lookAng -= twoPi;
    } else if (diffAtan < -pi) {
        lookAng += twoPi;
    }

    const double rawT = (double(now) - double(timeLast)) / double(duration);
    float t = float(rawT);
    if (rawT > 1.0) {
        t = 1.0f;
    }

    const double factor = (IdleCamSinCpp((270.0 - double(t) * 180.0) * double(kIdleCamDegToRad)) + 1.0) * 0.5;
    *outX = float((lookDist - double(slerpDist)) * factor + double(slerpDist));
    *outZ = float((lookAng - double(slerpAtan)) * factor + double(slerpAtan));
    return t;
}
#pragma float_control(pop)

// --- CTaskSimpleFightingControl (14/10/2026) ---
// Sin reasociación: cada operación del EXE redondea a 24 bits y /fp:fast fusionaría 0.02f · 1000.f.
#pragma float_control(precise, on, push)
__declspec(noinline) unsigned int __cdecl FightCtlElapsedCpp(float step) {
    // _ftol2 (0x821B40) trunca a 64 bits; el EXE usa EAX (32 bits bajos). NaN/fuera de rango: 0x8000000000000000 -> 0.
    const float milliseconds = (step * 0.02f) * 1000.f; // 0x858B38, 0x858C4C
    return static_cast<unsigned int>(static_cast<long long>(milliseconds));
}

__declspec(noinline) bool __cdecl FightCtlShouldBlockCpp(float random, float chance, float rate) {
    // fcompp: ST0 = (chance · rate) · 0.025f (0x859038) frente a ST1 = random; test ah,41h == 0  <=>  ST0 > ST1.
    return (chance * rate) * 0.025f > random;
}

__declspec(noinline) unsigned int __cdecl FightCtlAttackDelayCpp(int random, float shootingRate, float aggression, unsigned int interval) {
    // random llega de rand() & 0x7FFF: float(random) es exacto (el EXE lo carga con fild).
    const float numerator   = float(random) * (1.f / 32767.f) + 0.25f; // 0x858C7C = 0x38000100, 0x858C84
    const float denominator = (((shootingRate * aggression) * 0.025f) * 0.7f) + 0.3f; // 0x859038, 0x858CB0, 0x858C24
    const float quotient    = numerator / denominator; // fdivp st(1)
    const int signedInterval = static_cast<int>(interval);
    float product;
    if (signedInterval >= 0) {
        // fild exacto · cociente con un solo redondeo (exacto para intervalo < 2^24; por encima, el producto cabe en 53 bits solo si el cociente tiene pocos bits).
        product = float(double(quotient) * double(signedInterval));
    } else {
        // fild del valor con signo y fadd 2^32 (0x858C54) con un solo redondeo a 24 bits; luego fmulp.
        const float unsignedValue = float(double(signedInterval) + 4294967296.0);
        product = quotient * unsignedValue;
    }
    return static_cast<unsigned int>(static_cast<long long>(product));
}
#pragma float_control(pop)

__declspec(noinline) float __cdecl OnFootSinPlusCpp(float pitch, float z) {
    return float(sin(double(pitch)) + double(z));
}

// Cada operación se redondea a float en una variable volatile (el fmul/fadd del EXE a 24 bits); seno y coseno entran sin redondear al primer fmul
// (rama no cooperativa) o ya redondeados a float (rama cooperativa). La multiplicación por 0.0f del eje z se conserva (inf/NaN) con un cero volatile.
__declspec(noinline) void __cdecl OnFootFighterProjectCpp(const float* right, const float* forward, float angle, float magnitude, bool roundTrig, float* out) {
    volatile float zero = 0.0f;
    double negSin = -sin(double(angle));
    double cosine = cos(double(angle));
    if (roundTrig) {
        volatile float roundedSin = float(negSin);
        volatile float roundedCos = float(cosine);
        negSin = roundedSin;
        cosine = roundedCos;
    }
    volatile float xCos = float(cosine * double(right[1]));
    volatile float xSin = float(negSin * double(right[0]));
    volatile float xSum = xCos + xSin;
    volatile float xZ = right[2] * zero;
    volatile float xFull = xSum + xZ;
    volatile float projectedX = xFull * magnitude;
    volatile float yCos = float(cosine * double(forward[1]));
    volatile float ySin = float(negSin * double(forward[0]));
    volatile float ySum = yCos + ySin;
    volatile float yZ = forward[2] * zero;
    volatile float yFull = ySum + yZ;
    volatile float projectedY = yFull * magnitude;
    out[0] = projectedX;
    out[1] = -projectedY;
}

// CWeaponEffects::Render (0x742CF0), 0x7437A7: fld a; fmul qword k; fadd b; fstp (misma secuencia x87, /arch:IA32 y PC de 24 bits).
__declspec(noinline) float __cdecl WeaponFxMulDoubleAddCpp(float a, double k, float b) {
    volatile float result = float(double(a) * k + double(b));
    return result;
}

// CWeaponEffects::Render (0x742CF0), 0x743630..0x7436B9: fpatan; fld st0; fsin; fstp; fcos; fstp; fld1; fsub (cos sin redondear).
__declspec(noinline) void __cdecl WeaponFxAngleSinCosCpp(float y, float x, float* outSin, float* outCos, float* outOneMinusCos) {
    const double angle = atan2(double(y), double(x));
    *outSin = float(sin(angle));
    const double c = cos(angle);
    *outCos = float(c);
    *outOneMinusCos = float(1.0 - c);
}

