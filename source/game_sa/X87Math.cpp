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
#include <bit>

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

__declspec(noinline) float __cdecl ExeAsinDegreesCpp(float x) {
    const unsigned previousControl = _control87(0, 0);
    _control87((previousControl & _MCW_PC) | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    volatile double angle = atan2(double(x), sqrt((1.0 + x) * (1.0 - x)));
    _control87(previousControl, _MCW_PC | _MCW_RC | _MCW_EM);
    return float(angle * kF_42652EE0); // 57.29578f (0x859878)
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
