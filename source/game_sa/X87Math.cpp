// Compile separately: /O2 /Oi /arch:IA32 /fp:fast, ordinary /Zi and no RTC/PCH/unity.
// Funciones sueltas y noinline: cada llamador conserva sus opciones de compilación y la
// secuencia x87 no cambia al insertarse en línea (p. ej. /fp:fast reemplaza sqrt(s) > 4 por s > 16).
extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
extern "C" double __cdecl sqrt(double);
extern "C" double __cdecl atan2(double, double);
extern "C" double __cdecl log10(double);
#pragma intrinsic(sin, cos, sqrt, atan2, log10)
#include <float.h>

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
