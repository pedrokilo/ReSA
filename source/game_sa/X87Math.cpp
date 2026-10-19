// Compile separately: /O2 /Oi /arch:IA32 /fp:fast, ordinary /Zi and no RTC/PCH/unity.
// Funciones sueltas y noinline: cada llamador conserva sus opciones de compilación y la
// secuencia x87 no cambia al insertarse en línea (p. ej. /fp:fast reemplaza sqrt(s) > 4 por s > 16).
extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
extern "C" double __cdecl sqrt(double);
extern "C" double __cdecl atan2(double, double);
extern "C" double __cdecl log10(double);
extern "C" double __cdecl pow(double, double);
extern "C" double __cdecl log(double);
extern "C" double __cdecl tan(double);
#pragma intrinsic(sin, cos, sqrt, atan2, log10, pow, log, tan)
#include <float.h>
#include <string.h>
#include <assert.h>
#include <bit>
#include "X87Math.h"

// --- Palabra de control como la del CRT del EXE (_CIasin/_CIacos, 0x821E70/0x822380): (anterior & 0x300) | 0x7F, solo la x87 ---
// _control87 del CRT de la DLL modifica ademas MXCSR (redondeo y mascaras) y, al restaurar con el valor leido de la x87, lo dejaria igualado a ella si antes diferian.
// __control87_2 con puntero SSE nulo toca unicamente la x87, como el EXE. Devuelve la palabra x87 previa (formato _MCW_*).
static __forceinline unsigned ExeFpuControlEnter() {
    unsigned previous = 0, ignored = 0;
    __control87_2(0, 0, &previous, nullptr);
    __control87_2((previous & _MCW_PC) | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM, &ignored, nullptr);
    return previous;
}
static __forceinline void ExeFpuControlLeave(unsigned previous) {
    unsigned ignored = 0;
    __control87_2(previous, _MCW_PC | _MCW_RC | _MCW_EM, &ignored, nullptr);
}

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
    const unsigned previousControl = ExeFpuControlEnter();
    const double acosValue = atan2(sqrt((1.0f + x) * (1.0f - x)), x);
    ExeFpuControlLeave(previousControl);
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
    const unsigned previousControl = ExeFpuControlEnter();
    volatile double result = atan2(sqrt((1.0 + x) * (1.0 - x)), x);
    ExeFpuControlLeave(previousControl);
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

__declspec(noinline) float __cdecl FloatTimesFloatCpp(float a, float b) {
    volatile float result = a * b;
    return result;
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

// _CIasin (0x821E70) en todas sus ramas; el resultado sale como double porque el EXE lo deja en ST0 despues de guardarlo y recargarlo como qword.
//  - |x| < 1 (exponente < 0x3FF): fpatan(x, sqrt((1 + x)(1 - x))) con la palabra de control (anterior & 0x300) | 0x7F, rama de abajo.
//  - x == +-1 exacto: +-pi/2 de 64 bits (0x8E313A), que al guardarse como qword (0x828EA7) es el double 0x3FF921FB54442D18 con el signo de x.
//    No es el fpatan a 24 bits (1.5707964): por 57.29578f da 89.99999 y no 90.
//  - 1 < |x| < 2, |x| >= 2 e infinitos: dominio. ST0 = "indefinido" real (0x8E3130) y errno = EDOM (33); queda el double 0xFFF8000000000000.
//  - NaN: se devuelve la propia x con el bit de NaN silencioso puesto y errno = EDOM (33); (el SNaN no llega: fld/fst ya lo silencian).
// La CRT del EXE (0x83365F/0x82E318/0x82E53A) solo toca ademas las banderas de estado x87 (pegajosas) y el errno del EXE (_errno, 0x82B181).
static void ExeAsinSetDomainErrno() {
    using ExeErrnoFn = int*(__cdecl*)();
    *reinterpret_cast<ExeErrnoFn>(0x0082B181)() = 33; // _errno() del EXE (EDOM)
}

__declspec(noinline) double __cdecl ExeAsinDoubleCpp(double x) {
    const unsigned long long bits = std::bit_cast<unsigned long long>(x);
    const unsigned int exponent = static_cast<unsigned int>(bits >> 32) & 0x7FF00000u;
    if (exponent >= 0x3FF00000u) {
        constexpr unsigned long long kFraction = 0x000FFFFFFFFFFFFFull;
        constexpr unsigned long long kSign = 0x8000000000000000ull;
        unsigned long long result;
        if (exponent == 0x7FF00000u && (bits & kFraction) != 0) {
            result = bits | 0x0008000000000000ull; // NaN silencioso con la misma carga y signo
            ExeAsinSetDomainErrno();
        } else if (exponent == 0x3FF00000u && (bits & kFraction) == 0) {
            result = (bits & kSign) | 0x3FF921FB54442D18ull; // +-pi/2 sin errno
        } else {
            result = 0xFFF8000000000000ull; // real indefinido
            ExeAsinSetDomainErrno();
        }
        return std::bit_cast<double>(result);
    }
    const unsigned previousControl = ExeFpuControlEnter();
    volatile double angle = atan2(x, sqrt((1.0 + x) * (1.0 - x)));
    ExeFpuControlLeave(previousControl);
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

static bool PlayerPedResidueFloatIsTrivial();
static bool CarEnterExitEntryEdxHelperIsTrivial();
static bool PathFindResidueFloatIsTrivial();
static bool ResidueHelpersAreTrivial() {
    auto  member = &ResidueEcx::Get;
    const void* getAddress;
    memcpy(&getAddress, &member, sizeof getAddress);
    return ResidueHelperIsTrivial(reinterpret_cast<const void*>(&ResidueRegsCpp)) && ResidueHelperIsTrivial(getAddress) && EntryEaxHelperIsTrivial() && PlayerPedResidueFloatIsTrivial() && CarEnterExitEntryEdxHelperIsTrivial() && PathFindResidueFloatIsTrivial();
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

// Requiere <cstdint>. Equivale a FLD dword [x]; FISTP dword [res] con la palabra de control vigente (precision y redondeo cualesquiera).
// Por debajo de 2^23 el redondeo a entero lo hace el propio almacenamiento a float de x + M, con M = +-2^23 del signo de x (rejilla de paso 1 en [2^23, 2^24)):
// el fstp dword redondea con el RC vigente (al par, hacia -inf, hacia +inf o hacia 0) y, al llevar M el signo de x, el sentido del redondeo dirigido es
// el correcto tambien para los negativos (con |x| + M y cambio de signo, "hacia -inf" y "hacia +inf" se invertirian). La suma es exacta en 53/64 bits y a 24 bits
// ya es el unico redondeo; solo el almacenamiento aplica el modo.
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
    const float magic = x < 0.0f ? -kMagic : kMagic;
    volatile float t = x + magic; // un unico redondeo a la rejilla entera, con el RC vigente
    t = t - magic;                // exacto
    return static_cast<unsigned int>(static_cast<int>(t));
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


// --- Lote de veinte archivos de nivel 2 (14/10/2026) ---
// --- CPlayerPed (lote w20) ---
// 0x60A82F..0x60A869: fld v; fdiv [0x8D2458]; fsub [0x858624]; fmul [tabla + 0x1C]; fadd [0x858624]. La rama t < 0 -> 0 (0x60A845) es inalcanzable con v > 5.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PlayerPedSprintResultCpp(float speed, float threshold, float multiplier) {
    volatile float result = float(((double(speed) / double(threshold) - 1.0) * double(multiplier)) + 1.0);
    return result;
}
#pragma float_control(pop)

// 0x60D0E9..0x60D0EE: call GetATanOfXY; fsub [compensation] (el valor de ST0 sigue en la pila x87, un solo redondeo a float).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PlayerPedAtanMinusCpp(float (__cdecl* atanFn)(float, float), float x, float y, float compensation) {
    volatile float result = float(double(atanFn(x, y)) - double(compensation));
    return result;
}
#pragma float_control(pop)

// Igual que ResidueRegsCpp pero devolviendo un float en ST0: compilada (/O2) es `fld dword ptr [esp+4]; ret 4` y no toca ECX, EDX ni EAX.
#pragma runtime_checks("", off)
__declspec(noinline) float __fastcall PlayerPedResidueFloatCpp(unsigned int /*ecxValue*/, unsigned int /*edxValue*/, float value) {
    return value;
}
#pragma runtime_checks("", restore)

// Comprobacion al cargar la DLL (anadir a ResidueHelpersAreTrivial): el ayudante tiene que ser `fld dword ptr [esp+4]; ret 4` (o con marco ebp).
static bool PlayerPedResidueFloatIsTrivial() {
    static const unsigned char withoutFrame[] = {0xD9, 0x44, 0x24, 0x04, 0xC2, 0x04, 0x00};                  // fld dword ptr [esp+4]; ret 4
    static const unsigned char withFrame[]    = {0x55, 0x8B, 0xEC, 0xD9, 0x45, 0x08, 0x5D, 0xC2, 0x04, 0x00}; // push ebp; mov ebp,esp; fld dword ptr [ebp+8]; pop ebp; ret 4
    const unsigned char* code = FollowIncrementalThunks(reinterpret_cast<const unsigned char*>(&PlayerPedResidueFloatCpp));
    return memcmp(code, withoutFrame, sizeof withoutFrame) == 0 || memcmp(code, withFrame, sizeof withFrame) == 0;
}

// --- Lote CarEnterExit ---
// CCarEnterExit::GetNearestCarDoor (0x6529A4..0x6529D8): ST1 = fpatan(-dx, dy); ST0 = fpatan(-fwd.x, fwd.y) o el float heading; fsubp st(1) redondea una vez a 24 bits.
// Los dos valores deben quedar en la pila x87 (sin fstp qword entre ellos): revisar el .obj (mismo patrón que TrailerSteerCpp).
__declspec(noinline) float __cdecl CarEnterExitHeadingDiffCpp(float dx, float dy, const float* forward, float heading) {
    const double a = atan2(-double(dx), double(dy));
    const double b = forward ? atan2(-double(forward[0]), double(forward[1])) : double(heading);
    volatile float result = float(a - b);
    return result;
}

// EDX de entrada de la función que lo llama, si es lo primero que ésta ejecuta: no tiene `return`, de modo que MSVC no escribe EAX ni EDX y sale
// (`ret`, con o sin marco de pila); el llamador lo lee como mitad alta del resultado de 64 bits. Comportamiento sin definir en el lenguaje, vigilado
// igual que EntryEaxCpp (comprobación de bytes al cargar la DLL y contrib/audit_register_residue.py).
#pragma warning(push)
#pragma warning(disable : 4716)
__declspec(noinline) unsigned long long __fastcall CarEnterExitEntryEdxCpp() {
}
#pragma warning(pop)

// Colocar DESPUÉS de FollowIncrementalThunks y EntryEaxHelperIsTrivial; se añade a la comprobación existente (ResidueHelpersAreTrivial: `&& CarEnterExitEntryEdxHelperIsTrivial()`).
static bool CarEnterExitEntryEdxHelperIsTrivial() {
    const unsigned char* code = FollowIncrementalThunks(reinterpret_cast<const unsigned char*>(&CarEnterExitEntryEdxCpp));
    static const unsigned char plain[]     = {0xC2, 0x00, 0x00};
    static const unsigned char withFrame[] = {0x55, 0x8B, 0xEC, 0x5D, 0xC2, 0x00, 0x00};
    return code[0] == 0xC3 || memcmp(code, plain, sizeof plain) == 0 || memcmp(code, withFrame, sizeof withFrame) == 0;
}

// fild n; fdivr twoPi; fst angleStep; fcos; fsubr 1.0; fdivr 0.5; fsqrt; fstp radius. Solo angleStep se guarda (fst deja ST0 intacto): fcos, la resta, el cociente y la raiz
// parten del cociente sin redondear a float, a la precision vigente.
__declspec(noinline) float __cdecl PedGroupPlacerCircleCpp(int numPeds, float* radius) {
    const double stepFull = double(std::bit_cast<float>(0x40C90FDBu)) / double(numPeds); // 0x858CBC
    volatile float step = float(stepFull);
    *radius = float(sqrt(0.5 / (1.0 - cos(stepFull)))); // 0x858B8C
    return step;
}

// CWeather::Update (0x72C445/0x72C5E5): 0.5 - cos(a) * 0.5 con el coseno sin redondear (fcos; fmul 0.5; fsubr 0.5; un solo redondeo a float); va tras TrigCompoundCosHalf.
__declspec(noinline) float __cdecl WeatherHalfMinusHalfCosCpp(float angle) {
    return float(0.5 - cos(double(angle)) * 0.5);
}
// CWeather::UpdateInTunnelness (0x72B6A4): fld heading; fsin; fchs; fstp (negacion exacta del seno sin redondear antes; conserva el signo de NaN que da fchs); va tras TrigCompoundNegSinDiv.
__declspec(noinline) float __cdecl WeatherNegSinCpp(float angle) {
    return float(-sin(double(angle)));
}

// --- CQuadBike (14/10/2026) ---
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl QuadBikeDampDeltaStoredCpp(float res, float timeStep, float vel) {
    const float damp = float(ExeCompatiblePowCpp(double(res), double(timeStep))); // fstp dword [damp]
    volatile float result = (vel * damp) - vel;
    return result;
}

__declspec(noinline) float __cdecl QuadBikeDampDeltaFullCpp(float res, float timeStep, float vel) {
    const double damp = ExeCompatiblePowCpp(double(res), double(timeStep)); // ST0 sin redondear
    volatile float result = float((damp * double(vel)) - double(vel));
    return result;
}

__declspec(noinline) float __cdecl QuadBikeLeanAngleCpp(float desLean, float timeStep, float leanAngle, float fullAnimLean, float steerAngle, float steeringLock) {
    const double v        = ExeCompatiblePowCpp(double(desLean), double(timeStep)); // ST0 sin redondear, usado en el producto y en 1 - v
    const float  quotient = (fullAnimLean * steerAngle) / (steeringLock * 0.017453292f); // 0x8595EC; todo float
    volatile float result = float((v * double(leanAngle)) - (double(quotient) * (1.0 - v)));
    return result;
}
#pragma float_control(pop)

// Pickups: (sin(angle) + add) * mult; add y mult se leen tras el seno (volatile) para que /fp:fast no redistribuya el producto.
__declspec(noinline) float __cdecl PickupsSinAddMulCpp(float angle, float add, float mult) {
    const volatile float* addend = &add;
    const volatile float* factor = &mult;
    double value = sin(double(angle));
    value += double(*addend);
    value *= double(*factor);
    volatile float result = float(value);
    return result;
}

// Pickups: orden estricto del EXE; con /fp:fast (1 - d*k)*255 se reasociaria como 255 - d*(k*255).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PickupsAlphaValueCpp(float distance, float factor, float one, float scale) {
    const float scaled = distance * factor;
    const float remainder = one - scaled;
    volatile float result = remainder * scale;
    return result;
}
#pragma float_control(pop)

// Va tras AnimQuaternionDotNegativeCpp en X87Math.cpp (misma sección "Animación").
// Suma de cuadrados en el orden del EXE (0x4D161B..0x4D162F); devuelve double para no redondear al rango de float (x87 conserva el exponente extendido).
#pragma float_control(precise, on, push)
static __declspec(noinline) double RpAnimBlendQuaternionSumSquares(const float* q) {
    return ((q[0] * q[0] + q[1] * q[1]) + q[2] * q[2]) + q[3] * q[3];
}
#pragma float_control(pop)

__declspec(noinline) void __cdecl AnimQuaternionNormaliseCpp(float* q) {
    const double sum = RpAnimBlendQuaternionSumSquares(q);
    // fcom con 0.0f; test ah,44h / jp: solo "igual" (ordenado) va a la rama w = 1; mayor, menor y no ordenado (NaN) normalizan.
    if (sum == 0.0) {
        q[3] = 1.0f;
        return;
    }
    // fsqrt; fdivr 1.0f: el recíproco queda en ST0 (24 bits) y cada producto con el float de memoria se guarda como float.
    const double inv = 1.0f / sqrt(sum);
    q[0] = float(inv * q[0]);
    q[1] = float(inv * q[1]);
    q[2] = float(inv * q[2]);
    q[3] = float(inv * q[3]);
}

// --- Lote de PostEffects.cpp (14/10/2026) ---
// fsin/fcos en funciones aparte SIN float_control(precise) (con precise pasarían a llamadas a la CRT); devuelven ST0 sin redondear.
static __declspec(noinline) double PostFxSinFull(double angle) { return sin(angle); }
static __declspec(noinline) double PostFxCosFull(double angle) { return cos(angle); }

// SetupBackBufferVertex (0x7043D9..0x70440B): ln(base) = fldln2 ; fld base ; fyl2x guardado como double; ln(value) / eso, redondeado a 24 bits; _ftol2 sobre el float.
__declspec(noinline) int __cdecl PostFxLog2TruncViaDoubleCpp(int value, double base) {
    volatile double lnBase = log(base);
    volatile float quotient = float(log(double(value)) / lnBase);
    return static_cast<int>(static_cast<long long>(quotient));
}

// ImmediateModeFilterStuffInitialize (0x703EB5..0x703EDF): ln(base) se queda en ST(1) en precisión extendida (una sola expresión, sin guardar).
__declspec(noinline) int __cdecl PostFxLog2TruncKeepExtendedCpp(int value, double base) {
    volatile float quotient = float(log(double(value)) / log(base));
    return static_cast<int>(static_cast<long long>(quotient));
}

// Ripple y Fog: sin reasociación (el EXE suma/resta paso a paso a 24 bits); el seno entra al producto/suma directamente desde ST0 (la llamada lo devuelve sin guardar).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PostFxRippleWaveCpp(unsigned int timeMs, float speed, float freq, float y, float xoffset) {
    const double arg = UnsignedAsX87(timeMs) * double(speed) + double(freq) * double(y); // ya redondeado a 24 bits: guardarlo como double es exacto
    volatile float result = float(PostFxSinFull(arg) * double(xoffset));
    return result;
}

// El seno se vuelve a calcular en cada uso (misma entrada, mismo resultado) para que nunca se guarde como double entre los dos usos.
__declspec(noinline) void __cdecl PostFxRippleFinalUCpp(unsigned int timeMs, float speed, float freq, float y, float xoffset, int width, float* outU0, float* outU1) {
    const double arg = UnsignedAsX87(timeMs) * double(speed) + double(freq) * double(y);
    *outU0 = float((PostFxSinFull(arg) + double(xoffset)) / double(width));
    *outU1 = float(((double(width) + PostFxSinFull(arg)) - double(xoffset)) / double(width));
}

__declspec(noinline) float __cdecl PostFxFogSinCoordCpp(float angle, float quarter, float radius, float half, float third) {
    volatile float result = float(((PostFxSinFull(double(angle)) * (double(quarter) + double(radius))) + double(half)) - double(third));
    return result;
}
__declspec(noinline) float __cdecl PostFxFogCosCoordCpp(float angle, float quarter, float radius, float half, float third) {
    volatile float result = float(((PostFxCosFull(double(angle)) * (double(quarter) + double(radius))) + double(half)) - double(third));
    return result;
}
#pragma float_control(pop)

// Lote de GangWars.cpp (14/10/2026). Va dentro de X87Math.cpp (usa memcpy de <string.h>); precise evita que /fp:fast reasocie o pliegue las constantes.
#pragma float_control(precise, on, push)
// 0x858C7C = 0x38000100: unidad del generador (1 / 32768 con ajuste), cargada como float.
static double GangWarsUnit() {
    const unsigned int bits = 0x38000100u;
    float unit;
    memcpy(&unit, &bits, sizeof(unit));
    return double(unit);
}

// 0x443DB0: fild rnd; fmul 0x858C7C; fmul 0x858C20 (0.9f); fmul 0x8598B8 (1080000.f); fadd 0x8598B4 (648000.f); jmp _ftol2.
__declspec(noinline) unsigned int __cdecl GangWarsNextAttackDelayCpp(int random) {
    double value = double(random) * GangWarsUnit();
    value = value * double(0.9f);
    value = value * double(1080000.0f);
    value = value + double(648000.0f);
    return static_cast<unsigned int>(static_cast<long long>(value));
}

// 0x444D30: fild; fmul unidad; fmul scale; fadd base; fsub subtract; fstp.
__declspec(noinline) float __cdecl GangWarsRandomOffsetCpp(int random, float base, float scale, float subtract) {
    double value = double(random) * GangWarsUnit();
    value = value * double(scale);
    value = value + double(base);
    value = value - double(subtract);
    volatile float result = float(value);
    return result;
}

// 0x44557C: fild; fmul unidad; (fxch) fmul 3.0f; fadd 2.0f. El resultado queda en ST0 a 24 bits; se devuelve como float.
__declspec(noinline) float __cdecl GangWarsRandomScaledCpp(int random, float scale, float base) {
    double value = double(random) * GangWarsUnit();
    value = value * double(scale);
    value = value + double(base);
    volatile float result = float(value);
    return result;
}

// 0x444C72: fld value; fmul scale; fadd base; fimul multiple; call _ftol2.
__declspec(noinline) int __cdecl GangWarsScaledTruncCpp(float value, float scale, float base, int multiple) {
    double scaled = double(value) * double(scale);
    scaled = scaled + double(base);
    scaled = scaled * double(multiple);
    return static_cast<int>(static_cast<long long>(scaled));
}

// 0x446BA2: fld CTimer::ms_fTimeStep; fmul 0x858B38 (0.02f); fmul +-1000.f; call _ftol2.
__declspec(noinline) unsigned int __cdecl GangWarsTimeDeltaCpp(float step, bool negative) {
    double value = double(step) * double(0.02f);
    value = value * (negative ? -1000.0 : 1000.0);
    return static_cast<unsigned int>(static_cast<long long>(value));
}

// 0x445586: fild index; fmul 0x858CBC (6.2831855f); fidiv total; fst angle.
__declspec(noinline) float __cdecl GangWarsDefenderAngleCpp(int index, int total) {
    double value = double(index) * double(6.2831855f);
    value = value / double(total);
    volatile float result = float(value);
    return result;
}
#pragma float_control(pop)

// --- Lote de Hud.cpp (14/10/2026) ---
// CHud::DrawRadar (0x58A422..0x58A435): fld negRightZ; fld upZ; fpatan; fchs; fsub quarterPi; fstp. (-a) - q con a sin redondear.
__declspec(noinline) float __cdecl HudRadarBaseAngleCpp(float negRightZ, float upZ, float quarterPi) {
    return float(-atan2(double(negRightZ), double(upZ)) - double(quarterPi));
}

// CHud::DrawRadar (0x58A4C4..0x58A4F6): fsin y fcos de la misma copia de ST0, sin fstp intermedio; luego las dos cadenas fmul/fadd/fsubr hasta cada fstp.
__declspec(noinline) void __cdecl HudRadarRingPointCpp(float angle, float zero, float radiusX, float radiusY, float sx40, float centerX, float centerY, float baseY, float* outX, float* outY) {
    const double s = sin(double(angle));
    const double c = cos(double(angle));
    *outX = float(((double(zero) * c + s) * double(radiusX) + double(sx40)) + double(centerX));
    *outY = float((((c - double(zero) * s) * double(radiusY)) + double(centerY)) + double(baseY));
}

// Las dos cadenas siguientes son aritmetica pura (sin fsin/fcos): el orden de las operaciones del EXE no puede reasociarse.
#pragma float_control(precise, on, push)
// CHud::DrawSubtitles (0x58C4F6..0x58C52F): P = c520*w; Q = k0*P; T = (w - P*k1) - Q; V = P*k2 + Q; resultado = (T - V)*k3.
__declspec(noinline) float __cdecl HudSubtitleCentreVitalsCpp(int width, float k520, float k0, float k1, float k2, float k3) {
    const double w = double(width);
    const double p = double(k520) * w;
    const double q = double(k0) * p;
    const double t = (w - p * double(k1)) - q;
    const double v = p * double(k2) + q;
    volatile float result = float((t - v) * double(k3));
    return result;
}

// CHud::DrawSubtitles (0x58C47B..0x58C495): A = c524*h; B = k*A; C = h - B; y = C - (A + A).
__declspec(noinline) float __cdecl HudSubtitleBottomYCpp(int height, float c524, float k866c1c) {
    const double h = double(height);
    const double a = double(c524) * h;
    const double b = double(k866c1c) * a;
    const double c = h - b;
    volatile float result = float(c - (a + a));
    return result;
}
#pragma float_control(pop)

// --- Timer (CTimer) ---
// UnsignedAsX87 (fild con signo + 2^32 si el bit alto esta puesto) ya existe en este archivo.
__declspec(noinline) float __cdecl TimerDivByU32Cpp(float num, unsigned int den) {
    volatile float result = float(double(num) / UnsignedAsX87(den)); // fdivr: num / den
    return result;
}

// fild qword (low63) ; fild qword (bit 63 como INT64_MIN) ; fchs ; faddp  ->  low63 + 2^63 con un solo redondeo; fstp float.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl TimerU64ToFloatCpp(unsigned long long value) {
    const long long low63 = static_cast<long long>(value & 0x7FFFFFFFFFFFFFFFull);
    const long long top = static_cast<long long>(value & 0x8000000000000000ull);
    volatile float result = float(double(low63) - double(top));
    return result;
}

// El cociente queda en ST0 (sin guardar como float); _ftol2 trunca a 64 bits y el EXE usa EAX (NaN/fuera de rango: 0x8000000000000000 -> 0).
__declspec(noinline) unsigned int __cdecl TimerPauseModeCpp(unsigned int pauseMode, float delta, unsigned int divider) {
    const double quotient = double(delta) / UnsignedAsX87(divider);
    return static_cast<unsigned int>(static_cast<long long>(UnsignedAsX87(pauseMode) + quotient));
}

// q = timeElapsed / divider (fdivr) queda en la pila: ftol(nonClipped + q) -> newNonClipped; float(0.05f * q) -> stepNonClipped; ftol(q) -> elapsedMs.
__declspec(noinline) void __cdecl TimerUpdateStepCpp(float timeElapsed, unsigned int divider, unsigned int nonClipped, unsigned int* newNonClipped, float* stepNonClipped, unsigned int* elapsedMs) {
    const double q = double(timeElapsed) / UnsignedAsX87(divider);
    *newNonClipped = static_cast<unsigned int>(static_cast<long long>(UnsignedAsX87(nonClipped) + q));
    *stepNonClipped = float(double(std::bit_cast<float>(0x3D4CCCCDu)) * q); // 0x858C28
    *elapsedMs = static_cast<unsigned int>(static_cast<long long>(q));
}
#pragma float_control(pop)

// --- CTrain::FindPositionOnTrackFromCoors (0x6F6CC0), lote de Train.cpp ---
// Aritmetica float pura en el orden del EXE bajo float_control(precise): sin reasociaciones de /fp:fast (p. ej. sqrt(s)·sqrt(s) -> s ni (low·k)·(1-day) -> low·(k·(1-day))).
// La x87 corre a 24 bits, asi que cada operacion float equivale al fmul/fadd/fsub del EXE; solo la raiz va por double (fsqrt).
#pragma float_control(precise, on, push)
// 0x6F6D85..0x6F6DEB: restas, raiz, productos y cociente quedan en ST a la precision vigente; fst dword copia dx y dy sin consumirlos y solo el cociente final se guarda (fstp dword).
__declspec(noinline) float __cdecl TrainProjectionCpp(float x, float y, float nx, float ny, float px, float py) {
    const double a = double(x) - double(nx);
    const double b = double(y) - double(ny);
    const double length = sqrt(b * b + a * a);
    const double dx = double(nx) - double(x);
    const double dy = double(ny) - double(y);
    const double p = double(px) - double(x);
    const double q = double(py) - double(y);
    return float((p * dx + q * dy) / (length * length));
}

// 0x6F6E95..0x6F6EE3 (cada fild lee un nibble 0..15: exacto con signo o sin el).
__declspec(noinline) float __cdecl TrainNodeLightingCpp(int low, int high, float day) {
    const float k = std::bit_cast<float>(0x3D088889u); // 0x858F10
    const float one = 1.0f;                            // 0x858624
    const float lowPart = (float(low) * k) * (one - day);
    const float highPart = (float(high) * k) * day;
    return lowPart + highPart;
}

// 0x6F6EE9..0x6F6F38 (el valor final se guarda con fstp en m_fContactSurfaceBrightness).
__declspec(noinline) float __cdecl TrainLightingBlendCpp(int low, int high, float day, float previous, float factor) {
    const float k = std::bit_cast<float>(0x3D088889u); // 0x858F10
    const float one = 1.0f;                            // 0x858624
    const float lowPart = (float(low) * k) * (one - day);
    const float highPart = (float(high) * k) * day;
    const float light = lowPart + highPart;
    return (light - previous) * factor + previous;
}

// 0x6F6F95..0x6F6FEC: el EXE guarda un valor intermedio en m_fCurrentRailDistance (0x6F6FB5) que se sobrescribe sin leerse; aqui solo importa el final.
__declspec(noinline) float __cdecl TrainRailDistanceCpp(int before, int after, float factor, float minimum, float maximum, float total) {
    const float k = std::bit_cast<float>(0x3EAAAAABu); // 0x859040
    const float half = std::bit_cast<float>(0x3F000000u); // 0x858B8C
    const float b = float(before) * k;
    const float a = float(after) * k;
    float distance = (b + (a - b) * factor) - (maximum - minimum) * half;
    if (distance < 0.0f) { // fcom + test ah,5 + jp: solo suma con "menor que" estricto; igual y NaN no suman
        distance = distance + total;
    }
    return distance;
}
#pragma float_control(pop)

// --- Lote ProjectileInfo (14/10/2026) ---
// precise: conserva el orden de las sumas y productos del EXE (un redondeo float por operación, como la x87 a 24 bits).
#pragma float_control(precise, on, push)
// CProjectileInfo::Update, 0x7394CF: dot = (sz·dz + sy·dy) + sx·dx; si dot < 0, steering -= direction · dot (el producto z se guarda como float).
__declspec(noinline) void __cdecl ProjInfoRemoveBackwardSteeringCpp(float* steering, const float* direction) {
    const float dot = (steering[2] * direction[2] + steering[1] * direction[1]) + steering[0] * direction[0];
    if (dot < 0.0f) {
        const float productX = direction[0] * dot;
        const float productY = direction[1] * dot;
        const float productZ = direction[2] * dot;
        steering[0] = steering[0] - productX;
        steering[1] = steering[1] - productY;
        steering[2] = steering[2] - productZ;
    }
}

// CProjectileInfo::Update, 0x7395EC: velocity += steering · ((acceleration [· speedMultiplier]) · timestep).
__declspec(noinline) void __cdecl ProjInfoAddSteeringCpp(float* velocity, const float* steering, float acceleration, float timestep, bool fast, float speedMultiplier) {
    float factor = acceleration;
    if (fast) {
        factor = factor * speedMultiplier;
    }
    factor = factor * timestep;
    const float productX = steering[0] * factor;
    const float productY = steering[1] * factor;
    const float productZ = steering[2] * factor;
    velocity[0] = productX + velocity[0];
    velocity[1] = productY + velocity[1];
    velocity[2] = productZ + velocity[2];
}

// (x² + y²) + z² como float (0x73964E); el producto z va el último, a diferencia de CVector::SquaredMagnitude.
static __declspec(noinline) float ProjInfoSquaredXYZCpp(const float* v) {
    volatile float sum = (v[0] * v[0] + v[1] * v[1]) + v[2] * v[2];
    return sum;
}
#pragma float_control(pop)

// CProjectileInfo::Update, 0x73964E: limita la velocidad del misil. La raíz va en ShadowSqrtCpp (noinline) para que /fp:fast no
// convierta sqrt(s) > limit en s > limit²; la comparación es ordenada (con NaN no cambia nada), el cociente queda redondeado a float.
__declspec(noinline) void __cdecl ProjInfoLimitSpeedCpp(float* velocity, float limit) {
    const double magnitude = ShadowSqrtCpp(ProjInfoSquaredXYZCpp(velocity));
    if (magnitude > double(limit)) {
        volatile float scaleStore = float(double(limit) / magnitude);
        const float scale = scaleStore;
        velocity[0] = scale * velocity[0];
        velocity[1] = scale * velocity[1];
        velocity[2] = scale * velocity[2];
    }
}

// CWeapon::FireAreaEffect (0x73E965..0x73E975) y CWeapon::Fire (0x7423D4..0x7423F0): fld x; fchs; fld y; fpatan; fmul [0x859878 = 0x42652EE0]. El producto se redondea una vez a 24 bits.
__declspec(noinline) float __cdecl WeaponHeadingDegreesCpp(float forwardX, float forwardY) {
    volatile float result = float(atan2(-double(forwardX), double(forwardY)) * double(std::bit_cast<float>(0x42652EE0u)));
    return result;
}

// CWeapon::FireAreaEffect (0x73E9C7..0x73E9D4): fld pitch; fptan; fstp st(0); (fchs y fstp los hace el llamador).
__declspec(noinline) float __cdecl WeaponTanCpp(float x) {
    volatile float result = float(tan(double(x)));
    return result;
}

// CWeapon::DoDriveByAutoAiming (0x73DA27..0x73DA48): fmul [0x8595EC]; fptan; fstp st(0); ... fld bestScore; fcomp st(1); test ah,5; jp -> continúa solo si lhs < tan (ordenado).
// El llamador ya hizo el producto aimAngle · 0x8595EC como float (fmul a 24 bits).
__declspec(noinline) bool __cdecl WeaponLessThanTanCpp(float lhs, float radians) {
    return double(lhs) < tan(double(radians));
}

// CWeapon::FireInstantHit, copia 1 (0x740103..0x7401B7) y copia 2 (0x74059B..0x74065B), con sine/cosine ya guardados como float (fstp [esp+0x20]).
// Orden del EXE: producto base·amp (z queda en la pila; y, x se guardan), luego ·seno; x += px·s (guardado), y = y + py·s (guardado), z' = pz·s + z (sin guardar);
// después lo mismo con el coseno: x += qx·c, y = y + qy·c, z = qz·c + z' guardado. Cada operación se redondea a 24 bits (equivale a float); precise mantiene el orden.
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl WeaponSwayAccumulateCpp(float* endpoint, const float* sineBasis, const float* cosineBasis, float amplitude, float sine, float cosine) {
    const float sz = sineBasis[2] * amplitude;
    const float sy = sineBasis[1] * amplitude;
    const float sx = sineBasis[0] * amplitude;
    endpoint[0] = sx * sine + endpoint[0];
    endpoint[1] = endpoint[1] + sy * sine;
    const float zSum = sz * sine + endpoint[2];
    const float cz = cosineBasis[2] * amplitude;
    const float cy = cosineBasis[1] * amplitude;
    const float cx = cosineBasis[0] * amplitude;
    endpoint[0] = cx * cosine + endpoint[0];
    endpoint[1] = endpoint[1] + cy * cosine;
    endpoint[2] = cz * cosine + zSum;
}
#pragma float_control(pop)

// --- CEventKnockOffBike (0x4B0020 CalcForcesAndAnims, 0x4B4AC0 SetPedSafePosition) ---
// El proceso corre la x87 a 24 bits de mantisa con exponente extendido: un "double" calculado aqui equivale a un registro ST, y solo lo que el EXE guarda con fstp se redondea a float.

// 0x4B0020..0x4B004C: fld pedMass; fdiv vehMass; fstp massRatio.
__declspec(noinline) float __cdecl KnockOffRatioCpp(float num, float den) {
    volatile float result = float(double(num) / double(den));
    return result;
}

#pragma float_control(precise, on, push)
// 0x4B00AB..0x4B00BB: fld a; fmul b; fmul c; fmul d; fstp (orden de izquierda a derecha, sin reasociar).
__declspec(noinline) float __cdecl KnockOffMul4Cpp(float a, float b, float c, float d) {
    volatile float result = float(((double(a) * double(b)) * double(c)) * double(d));
    return result;
}

// 0x4B0266: fld v; fmul k; fstp out (tres veces, cada producto se guarda como float).
__declspec(noinline) void __cdecl KnockOffScaleCpp(const float* v, float k, float* out) {
    volatile float x = float(double(v[0]) * double(k));
    volatile float y = float(double(v[1]) * double(k));
    volatile float z = float(double(v[2]) * double(k));
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

// 0x4B0311..0x4B0389 (y copias en 0x4B0422, 0x4B047D, 0x4B0541, 0x4B05E1, 0x4B0623..0x4B065D):
//   ST: upX*upK, upY*upK (sin guardar); [mem] float(upZ*upK); ST: sideX*sideK (sin guardar); [mem] float(sideY*sideK), float(sideZ*sideK)
//   x = float(sideX' + upX'), y = float(float(sideY*sideK) + upY'), z = float(float(sideZ*sideK) + float(upZ*upK)).
__declspec(noinline) void __cdecl KnockOffCombineCpp(const float* up, float upK, const float* side, float sideK, float* out) {
    const double upX = double(up[0]) * double(upK);
    const double upY = double(up[1]) * double(upK);
    volatile float upZStored = float(double(up[2]) * double(upK));
    const double sideX = double(side[0]) * double(sideK);
    volatile float sideYStored = float(double(side[1]) * double(sideK));
    volatile float sideZStored = float(double(side[2]) * double(sideK));
    const float x = float(sideX + upX);
    const float y = float(double(sideYStored) + upY);
    const float z = float(double(sideZStored) + double(upZStored));
    out[0] = x;
    out[1] = y;
    out[2] = z;
}
#pragma float_control(pop)

// 0x4B4B0E..0x4B4B47: fld fwdX; fchs; fld fwdY; fpatan; [fadd pi]; fstp. El fpatan queda sin redondear hasta el fadd/fstp.
__declspec(noinline) float __cdecl KnockOffHeadingCpp(float fwdX, float fwdY, bool addPi) {
    const double heading = atan2(-double(fwdX), double(fwdY));
    volatile float result = addPi ? float(heading + double(kF_40490FDB)) : float(heading);
    return result;
}

// --- Lote de PedGeometryAnalyser.cpp (14/10/2026) ---
// 0x5F16EC..0x5F171B: fld prev.z; fmul plane.z; fld prev.y; fmul plane.y; faddp; fld prev.x; fmul plane.x; faddp; fchs; fstp float.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PedGeomPlaneOffsetCpp(const float* plane, const float* previous) {
    volatile float result = -((previous[2] * plane[2] + previous[1] * plane[1]) + previous[0] * plane[0]);
    return result;
}
#pragma float_control(pop)

// 0x5F13F6..0x5F143D. fpatan; fsub rotation; fadd pi/4 (0x859AB0); fcom 0 y fadd 2pi (0x858CBC) solo si "menor" (iguales, mayores y NaN no); fmul 2/pi (0x858FB8);
// jmp _ftol2. No hay ningun almacenamiento a float en toda la cadena: todo queda en ST0 a la precision vigente (el resultado es la parte baja del entero de 64 bits
// de _ftol2). Por eso la etapa 1 devuelve un double (fpatan menos rotacion, sin redondear a float) y la cadena sigue en double: exacta con PC24 y PC53; con PC64
// los valores pasan por un double entre la etapa 1 y el resto (limite que no se puede cerrar sin un tipo de 80 bits).
// Etapa 1 (sin precise: el compilador emite fpatan): num/den llegan como double ya calculados en la etapa 2 para conservar el -0 de -(dx).
static __declspec(noinline) double __cdecl PedGeomShotAngleDbl(double num, double den, float rotation) {
    return atan2(num, den) - double(rotation);
}
#pragma float_control(precise, on, push)
__declspec(noinline) int __cdecl PedGeomShotSideCpp(const float* origin, const float* target, float rotation) {
    const double quarter = double(std::bit_cast<float>(0x3F490FDBu)); // 0x859AB0
    const double full = double(std::bit_cast<float>(0x40C90FDBu));    // 0x858CBC
    const double scale = double(std::bit_cast<float>(0x3F22F983u));   // 0x858FB8
    const double deltaX = double(target[0]) - double(origin[0]);
    const double num = -deltaX;
    const double den = double(target[1]) - double(origin[1]);
    double value = PedGeomShotAngleDbl(num, den, rotation) + quarter;
    if (value < 0.0) { value += full; }
    value *= scale;
    return int(static_cast<long long>(value));
}

// 0x5F561B..0x5F56B1: acumulador desde 0.0f; por tramo, fld x3 (new - old), dz² + dy² y luego + dx², fsqrt y faddp.
__declspec(noinline) double __cdecl PedGeomRouteLengthCpp(const float* points, int count) {
    double length = 0.0;
    for (int i = 1; i < count; ++i) {
        const float* from = points + 3 * (i - 1);
        const float* to = from + 3;
        const double dx = double(to[0]) - double(from[0]);
        const double dy = double(to[1]) - double(from[1]);
        const double dz = double(to[2]) - double(from[2]);
        length += sqrt((dz * dz + dy * dy) + dx * dx);
    }
    return length;
}
__declspec(noinline) bool __cdecl PedGeomRouteLengthLessCpp(double first, double second) {
    return first < second;
}
#pragma float_control(pop)

// --- Lote de Object.cpp (14/10/2026) ---
// CObject::ProcessControl (0x5A2763..0x5A27BD): ver X87Math.h. El rumbo (fpatan) se compara y se ajusta sin guardarlo como float; solo la diferencia final se redondea.
__declspec(noinline) float __cdecl ObjectDoorAngleDiffCpp(const float* forward, float compactHeading, float doorStart) {
    const float pi    = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
    const float twoPi = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
    double heading    = forward ? atan2(double(-forward[0]), double(forward[1])) : double(compactHeading);
    const float upper = doorStart + pi;
    if (heading > double(upper)) {
        heading = heading - double(twoPi);
    } else {
        const float lower = doorStart - pi;
        if (heading < double(lower)) {
            heading = heading + double(twoPi);
        }
    }
    volatile float result = float(double(doorStart) - heading);
    return result;
}

// CObject::ProcessSamSiteBehaviour (0x5A093C..0x5A0945): call atanFn; fst angle; fsub heading; fstp diff. Revisar en el .obj: ningun fstp/fld entre la llamada y el fsub.
__declspec(noinline) void __cdecl ObjectSamSiteAtanCpp(float (__cdecl* atanFn)(float, float), float x, float y, float heading, float* outAngle, float* outDiff) {
    const double angle = double(atanFn(x, y));
    *outAngle          = float(angle);
    *outDiff           = float(angle - double(heading));
}

// CObject::ProcessTrainCrossingBehaviour (0x5A0CD3..0x5A0D11): _CIacos (0x822380) = fpatan(sqrt((1 + x)(1 - x)), x) con la palabra de control (anterior & 0x300) | 0x7F; el resultado queda en ST0 y el fsubp/faddp
// del paso lo consume sin redondear. x = +-1 da 0 / pi como fldz/fldpi y |x| > 1 o NaN dan NaN (el llamador lo sustituye por sus limites).
__declspec(noinline) float __cdecl ObjectTrainCrossingAngleCpp(float upZ, float step, bool subtract) {
    const unsigned previousControl = ExeFpuControlEnter();
    const double x     = double(upZ);
    const double delta = subtract ? -double(step) : double(step);
    volatile float result = float(atan2(sqrt((1.0 + x) * (1.0 - x)), x) + delta);
    ExeFpuControlLeave(previousControl);
    return result;
}

// CObject::SetMatrixForTrainCrossing (0x59F230..0x59F28B): fcos; cos*axis.x -> float; cos*axis.y -> float; cos*axis.z queda en ST0 (a 24 bits ya es float); fsin; 0*sen -> float; sen -> float;
// forward.x = (0*sen) + cosX; forward.y = (0*sen) + cosY; forward.z = sen + cosZ. El cero es volatile para que /fp:fast no elimine 0*sen (signo de los ceros, NaN).
__declspec(noinline) void __cdecl ObjectTrainCrossingForwardCpp(const float* axis, float angle, float* forward) {
    volatile float zero = 0.0f;
    const double c = cos(double(angle));
    volatile float cosX = float(double(axis[0]) * c);
    volatile float cosY = float(double(axis[1]) * c);
    const double cosZ = double(axis[2]) * c;
    const double s    = sin(double(angle));
    volatile float zeroSin   = float(double(zero) * s);
    volatile float sinStored = float(s);
    forward[0] = float(double(zeroSin) + double(cosX));
    forward[1] = float(double(zeroSin) + double(cosY));
    forward[2] = float(double(sinStored) + cosZ);
}

// --- CAEWeatherAudioEntity (colocar DESPUES de UnsignedAsX87 y de ExeCompatiblePowCpp) ---
__declspec(noinline) float __cdecl WeatherAudioThunderBaseCpp(unsigned int duration, int defaultVolume) {
    const double argument = UnsignedAsX87(duration) * std::bit_cast<float>(0x3D19999Au) + 0.25; // 0x862E40 y 0x862E38
    volatile float result = float(log10(argument) * 20.0f + double(defaultVolume)); // 0x858BA4
    return result;
}

__declspec(noinline) float __cdecl WeatherAudioThunderRiseCpp(int playPos, unsigned int reqPlayPos, float reqVolume) {
    const double ratio = double(playPos) / UnsignedAsX87(reqPlayPos);
    volatile float result = float(log10(ratio) * 20.0f + double(reqVolume));
    return result;
}

// Orden estricto del EXE (d·(1 - p))·k + p; el log10 va en la función siguiente (con precise pasaría a ser una llamada a la CRT).
#pragma float_control(precise, on, push)
static __declspec(noinline) double WeatherAudioThunderFallArgument(float reqVolume, unsigned int diff) {
    const double p = ExeCompatiblePowCpp(10.0, double(reqVolume) * double(std::bit_cast<float>(0x3D4CCCCDu))); // 0x859EA8 y 0x858C28
    return (UnsignedAsX87(diff) * (1.0f - p)) * double(std::bit_cast<float>(0x3BA3D70Au)) + p; // 0x858624 y 0x858B4C
}
#pragma float_control(pop)

__declspec(noinline) float __cdecl WeatherAudioThunderFallCpp(float reqVolume, unsigned int diff) {
    const double argument = WeatherAudioThunderFallArgument(reqVolume, diff);
    volatile float result = float(log10(argument) * 20.0f);
    return result;
}

#pragma float_control(precise, on, push)
static __declspec(noinline) double WeatherAudioCityNoiseArgument(float dist, float speed) {
    return (1.0f - double(dist) * double(std::bit_cast<float>(0x3E2AAAABu))) * double(speed); // 0x85F0A0 y 0x858624
}
#pragma float_control(pop)

__declspec(noinline) float __cdecl WeatherAudioCityNoiseOffsetCpp(float dist, float speed) {
    const double argument = WeatherAudioCityNoiseArgument(dist, speed);
    volatile float result = float(log10(argument) * 20.0f + 30.0f); // 0x858BA4 y 0x858CA4
    return result;
}

__declspec(noinline) float __cdecl WeatherAudioRainTargetCpp(float wet, int defaultVolume) {
    volatile float result = float(log10(double(wet) * 1.25f) * 20.0f + double(defaultVolume)); // 0x8595F0 y 0x858BA4
    return result;
}

// --- Lote de cinco archivos de nivel 2 (15/10/2026) ---
// CBirds::Update (0x71251B..0x712521): faddp st(1) suma el fpatan sin redondear al producto ya redondeado; float(double + double) da el mismo redondeo único.
__declspec(noinline) float __cdecl BirdsAddAtan2Cpp(float add, float x, float y) {
    return float(double(add) + atan2(double(x), double(y)));
}

// CBirds::Update (0x7126C5..0x7126FB): s y c quedan en la pila x87 sin redondear; cada producto se guarda como float (precisión interna de 24 bits) y la suma/resta también.
// El EXE carga x e y antes de escribir x (fstp [px]) y escribe y al final: con px == py gana la escritura de y.
__declspec(noinline) void __cdecl BirdsRotateCpp(float angle, float* px, float* py) {
    const float x = *px;
    const float y = *py;
    const double s = sin(double(angle));
    const double c = cos(double(angle));
    volatile float cx = float(c * double(x));
    volatile float sy = float(s * double(y));
    volatile float cy = float(c * double(y));
    volatile float sx = float(s * double(x));
    const float nx = cx + sy;
    const float ny = cy - sx;
    *px = nx;
    *py = ny;
}

// CMatrix::RotateZ (0x59B390): c = fcos sin redondear (queda en la pila x87); s = fsin guardado como float (fstp dword). Para cada fila (right, forward, up, pos):
// x' = c*x - s*y, y' = c*y + s*x (pos: y' = s*x + c*y); todas las lecturas usan los valores originales de la fila (el EXE lee antes de sobrescribir) y cada resultado se redondea una vez a float.
__declspec(noinline) void __cdecl HoldEntityRotateZCpp(float* m, float angle) {
    const double c = cos(double(angle));
    volatile float storedSin = float(sin(double(angle)));
    const double s = storedSin;
    const double rx = m[0],  ry = m[1];
    const double fx = m[4],  fy = m[5];
    const double ux = m[8],  uy = m[9];
    const double px = m[12], py = m[13];
    m[4]  = float(c * fx - s * fy);
    m[5]  = float(s * fx + c * fy);
    m[8]  = float(c * ux - s * uy);
    m[9]  = float(s * ux + c * uy);
    m[12] = float(c * px - s * py);
    m[13] = float(s * px + c * py);
    m[0]  = float(c * rx - s * ry);
    m[1]  = float(c * ry + s * rx);
}

// CPlaceable::GetHeading / GetRoll (0x441DB0 / 0x420B30). extern "C" y double: CPlaceable las declara como float con otro nombre (/alternatename) para devolver ST0 sin redondear.
// -forward.x por fchs (exacto); sin precise para que fpatan salga en linea.
extern "C" __declspec(noinline) double __cdecl PlaceableHeadingDblCpp(const float* forward) {
    return atan2(-double(forward[0]), double(forward[1]));
}

// atan2(right.z, den) con den = fsqrt(float(x*x) + float(y*y)) y, solo si up.z < 0 (NaN no), den * (-1.0f) con fmul (0x858C1C; no fchs: un NaN conserva su signo).
extern "C" __declspec(noinline) double __cdecl PlaceableRollDblCpp(const float* right, float upZ) {
    const float xx = right[0] * right[0];
    const float yy = right[1] * right[1];
    double den = sqrt(double(xx + yy));
    volatile float minusOne = -1.0f;
    if (upZ < 0.0f)
        den = den * double(minusOne);
    return atan2(double(right[2]), den);
}

// --- Lote de cinco archivos (General, BoneNode y otros; 15/10/2026) ---
// --- CGeneral (General.cpp) ---
static constexpr float kGeneral_38000100 = std::bit_cast<float>(0x38000100u); // 1/32767 (0x858C7C)
static constexpr float kGeneral_38D1B717 = std::bit_cast<float>(0x38D1B717u); // 0.0001 (0x858FC4)
static constexpr float kGeneral_3FC90FDB = std::bit_cast<float>(0x3FC90FDBu); // pi/2 (0x858FE4)
static constexpr float kGeneral_4096CBE4 = std::bit_cast<float>(0x4096CBE4u); // 3pi/2 (0x863AC4)
// pi (0x858CB8) y 2pi (0x858CBC): kF_40490FDB y kF_40C90FDB, definidas arriba.

// Una sola operación x87 tras el fpatan: el resultado sin redondear de fpatan entra directo a fsubr/fadd. Fuera de precise (fpatan/fsqrt en línea).
static __declspec(noinline) double __cdecl GeneralAtanDbl(double ratio) { return atan2(ratio, 1.0); }
static __declspec(noinline) double __cdecl GeneralConstMinusAtanDbl(double k, double ratio) { return k - atan2(ratio, 1.0); }
static __declspec(noinline) double __cdecl GeneralAtanPlusConstDbl(double ratio, double k) { return atan2(ratio, 1.0) + k; }
static __declspec(noinline) double __cdecl GeneralSqrtDbl(double value) { return sqrt(value); }

// 0x41BD90. Orden del EXE: fild; fmul 1/32767; fld max; fsub min; fmulp; fadd min; ret con ST0 SIN guardar. Devuelve double (extern "C"): General.cpp lo
// declara como float con otro nombre (/alternatename) para que ST0 salga tal cual. Con float(...) el resultado pasaba por memoria como float y los
// resultados menores que 2^-126 se redondeaban a subnormal float, mientras que la x87 de PC24 conserva el exponente extendido
// (banco CppX87Precision1, caso 302: ST0 = 0x3F7D8F6FF5.. del EXE frente a 0x3F7D8F6FF0.. de la DLL).
#pragma float_control(precise, on, push)
extern "C" __declspec(noinline) double __cdecl GeneralRandomInRangeDblCpp(int random, float min, float max) {
    return ((double(random) * double(kGeneral_38000100)) * (double(max) - double(min))) + double(min);
}

// 0x53CBE0. Con dx = x2 - x1 y dy = y2 - y1 (dy == 0 incluido -0 pasa a 0.0001f; NaN sigue), a = atan(dx/dy):
//   dx > 0, dy > 0: (pi/2 - a) + pi/2   dx > 0, dy <= 0: pi/2 - (a + pi/2)
//   dx <= 0, dy > 0: -pi/2 - (a + pi/2)  dx <= 0, dy <= 0: (pi/2 - a) - pi/2     (<= incluye NaN)
// El EXE decide por las banderas de fcomp/fcom conservadas a traves de fdiv/fpatan (0x41/0x44 de AH), no por una comparacion nueva.
// Devuelve double (extern "C"): el EXE termina con ST0 sin guardar y con float(...) el resultado se redondearia a 24 bits bajo PC53/PC64 (banco CppX87Precision1).
extern "C" __declspec(noinline) double __cdecl GeneralRadianAngleDblCpp(float x1, float y1, float x2, float y2) {
    const double dx = double(x2) - double(x1);
    double dy = double(y2) - double(y1);
    if (dy == 0.0) {
        dy = double(kGeneral_38D1B717);
    }
    const double ratio = dx / dy;
    const double halfPi = double(kGeneral_3FC90FDB);
    if (dx > 0.0) {
        if (dy > 0.0) {
            return (halfPi - GeneralAtanDbl(ratio)) + halfPi;
        }
        return halfPi - (GeneralAtanDbl(ratio) + halfPi);
    }
    if (dy > 0.0) {
        return -halfPi - (GeneralAtanDbl(ratio) + halfPi);
    }
    return (halfPi - GeneralAtanDbl(ratio)) - halfPi;
}

// 0x53CC70. Todas las ramas terminan en ret con ST0 sin guardar (fsubr/fadd/fpatan): sin float(...) ni almacenamientos (con PC53/PC64 se redondearia a 24 bits).
// |x| y |y| con fchs solo si < 0 (NaN y -0 intactos); "positivo" = estrictamente > 0 (NaN no). Un solo fpatan por llamada, sin redondear antes de la
// ultima suma/resta; (-1/d)*n son dos operaciones separadas (fld -1; fdiv; fmul).
extern "C" __declspec(noinline) double __cdecl GeneralATanOfXYDblCpp(float x, float y) {
    if (x == 0.0f && y == 0.0f) {
        return 0.0f;
    }
    const double dx = double(x);
    const double dy = double(y);
    const double absX = x < 0.0f ? -dx : dx;
    const double absY = y < 0.0f ? -dy : dy;
    const bool xPositive = x > 0.0f;
    const bool yPositive = y > 0.0f;
    const double halfPi = double(kGeneral_3FC90FDB);
    const double threeHalfPi = double(kGeneral_4096CBE4);
    const double pi = double(kF_40490FDB);
    const double twoPi = double(kF_40C90FDB);
    if (absX < absY) {
        if (yPositive) {
            if (xPositive) {
                return GeneralConstMinusAtanDbl(halfPi, dx / dy);
            }
            return GeneralAtanPlusConstDbl((-1.0 / dy) * dx, halfPi);
        }
        if (xPositive) {
            return GeneralAtanPlusConstDbl((-1.0 / dy) * dx, threeHalfPi);
        }
        return GeneralConstMinusAtanDbl(threeHalfPi, dx / dy);
    }
    if (yPositive) {
        if (xPositive) {
            return GeneralAtanDbl(dy / dx); // el fpatan puro sale sin redondear en ST0 (el control de precision solo afecta a +,-,*,/ y sqrt)
        }
        return GeneralConstMinusAtanDbl(pi, (-1.0 / dx) * dy);
    }
    if (xPositive) {
        return GeneralConstMinusAtanDbl(twoPi, (-1.0 / dx) * dy);
    }
    return GeneralAtanPlusConstDbl(dy / dx, pi);
}

// 0x53CE30. d y las raices se mantienen en el rango extendido de la x87 (b·b puede superar FLT_MAX); sqrt en ayudante aparte (fsqrt en línea, 24 bits).
// Los dos fstp float se hacen primero en x2 y luego en x1 (si son el mismo float, gana x1). a, b y c llegan por valor: ninguna salida los altera.
__declspec(noinline) bool __cdecl GeneralSolveQuadraticCpp(float a, float b, float c, float* x1, float* x2) {
    const double db = double(b);
    const double discriminant = db * db - (double(a) * double(c)) * 4.0;
    if (discriminant < 0.0) {
        return false;
    }
    const double root = GeneralSqrtDbl(discriminant);
    const double inverseA = 1.0 / double(a);
    *x2 = float(((root - db) * inverseA) * 0.5);
    *x1 = float(((-db - root) * inverseA) * 0.5);
    return true;
}
#pragma float_control(pop)

// --- Lote de BoneNode.cpp (15/10/2026), prefijo BoneNode (BoneNode_c: 0x6171F0, 0x617080, 0x617650, 0x616CD0) ---
// Constantes del EXE: pi 0x858CB8, 1/180 0x85F0AC, 1/pi 0x86D2B4, 1/45 0x86D2B8 (180.0f en 0x85A994).
static constexpr float kBoneNodePi     = std::bit_cast<float>(0x40490FDBu);
static constexpr float kBoneNodeInv180 = std::bit_cast<float>(0x3BB60B61u);
static constexpr float kBoneNodeInvPi  = std::bit_cast<float>(0x3EA2F983u);
static constexpr float kBoneNodeInv45  = std::bit_cast<float>(0x3CB60B61u);

// fpatan sin redondear en ST0 (se devuelve como double); fuera de la región precise para que sea el fpatan en línea.
static __declspec(noinline) double BoneNodeAtan2Cpp(double y, double x) {
    return atan2(y, x);
}

#pragma float_control(precise, on, push)
// ((ángulo · pi) · (1/180)) · 0.5 sin guardar como float, en el orden del EXE (con /fp:fast se plegarían las constantes); queda en ST0.
static __declspec(noinline) double BoneNodeHalfAngleCpp(float angle) {
    return ((double(angle) * kBoneNodePi) * kBoneNodeInv180) * 0.5f;
}

// Normalización del EXE (0x6172BB..0x617323): ((w² + z²) + y²) + x² y multiplicación por 1/suma (sin raíz); z llega desde registro (ya a 24 bits).
static __declspec(noinline) void BoneNodeNormaliseQuatCpp(float x, float y, float z, float w, float* out) {
    const float sum = ((w * w + z * z) + y * y) + x * x;
    const float inv = 1.0f / sum;
    out[0] = inv * x;
    out[1] = inv * y;
    out[2] = inv * z;
    out[3] = inv * w;
}

// float(atan2(y, x) · 180 · (1/pi)): fpatan sin redondear, un redondeo a 24 bits por fmul y fstp final.
static __declspec(noinline) float BoneNodeAtan2DegCpp(float y, float x) {
    return float((BoneNodeAtan2Cpp(double(y), double(x)) * 180.0f) * kBoneNodeInvPi);
}
#pragma float_control(pop)

// BoneNode_c::EulerToQuat (0x6171F0). Se guardan como float (volatile = fstp/fld del EXE): hz, cos x/y/z, sin x, cc = cz·cx, ss = sz·sx, y la x, y, w del quaternion.
// Quedan en registro: sin(hy) y sin(hz) a precisión completa (nunca como double guardado), cs = cx·sz, sc = sx·cz (a 24 bits: guardarlos como double no pierde nada) y z.
// Revisar el .obj: ningún fstp/fld qword de los fsin entre su cálculo y su uso, y los cuatro productos con sy en el orden descrito.
__declspec(noinline) void __cdecl BoneNodeEulerToQuatCpp(const float* angles, float* out) {
    const double hx = BoneNodeHalfAngleCpp(angles[0]);
    const double hy = BoneNodeHalfAngleCpp(angles[1]);
    volatile float hz = float(BoneNodeHalfAngleCpp(angles[2]));

    volatile float cx = float(cos(hx));
    volatile float cy = float(cos(hy));
    volatile float cz = float(cos(double(hz)));
    volatile float sx = float(sin(hx));
    const double   sy = sin(hy);
    const double   sz = sin(double(hz));

    volatile float  cc = cz * cx;
    volatile double cs = double(cx) * sz;
    volatile double sc = double(sx) * double(cz);
    volatile float  ss = float(sz * double(sx));

    volatile float x = float(double(cy) * sc - cs * sy);
    volatile float y = float(double(cc) * sy + double(ss) * double(cy));
    const double   z = cs * double(cy) - sc * sy;
    volatile float w = float(double(ss) * sy + double(cc) * double(cy));

    BoneNodeNormaliseQuatCpp(x, y, float(z), w, out);
}

#pragma float_control(precise, on, push)
// BoneNode_c::QuatToEuler (0x617080). q = {x, y, z, w}. 2·(a·b) se calcula como p + p; todas las sumas y restas en el orden del EXE.
__declspec(noinline) void __cdecl BoneNodeQuatToEulerCpp(const float* q, float* out) {
    const float yy  = q[1] * q[1];
    const float yy2 = yy + yy;                      // 2y²
    const float a   = 1.0f - yy2;                   // 1 - 2y²
    const float zz  = q[2] * q[2];
    const float c1  = a - (zz + zz);                // (1 - 2y²) - 2z²

    const float zw  = q[2] * q[3];
    const float xy  = q[0] * q[1];
    const float b   = (zw + zw) + (xy + xy);        // 2zw + 2xy

    const float zx  = q[2] * q[0];
    const float wy  = q[3] * q[1];
    const float d   = (zx + zx) - (wy + wy);        // 2zx - 2wy

    const float zy  = q[2] * q[1];
    const float wx  = q[3] * q[0];
    const float c   = (zy + zy) + (wx + wx);        // 2zy + 2wx

    const float xx  = q[0] * q[0];
    const float dd  = (1.0f - (xx + xx)) - yy2;     // (1 - 2x²) - 2y²

    const float s    = -d;
    const float cosP = float(sqrt(double(1.0f - s * s)));  // fsqrt a 24 bits, guardado como float

    out[1] = BoneNodeAtan2DegCpp(s, cosP);

    if (s == 1.0f || s == -1.0f) {
        // Bloqueo cardán: q se vuelve a leer (el EXE lo hace tras escribir out[1]).
        const float gx2 = q[0] * q[0];
        const float gz2 = q[2] * q[2];
        const float g1  = (1.0f - (gx2 + gx2)) - (gz2 + gz2);
        const float gzy = q[2] * q[1];
        const float gwx = q[3] * q[0];
        const float g2  = -((gzy + gzy) - (gwx + gwx));
        out[0] = BoneNodeAtan2DegCpp(g2, g1);
        out[2] = BoneNodeAtan2DegCpp(0.0f, 1.0f);   // fpatan(+0, 1) = +0
    } else {
        const float r   = 1.0f / cosP;
        const float cr  = c * r;
        const float rdd = r * dd;
        out[0]          = BoneNodeAtan2DegCpp(cr, rdd);
        const float br  = b * r;
        const float rc1 = r * c1;
        out[2]          = BoneNodeAtan2DegCpp(br, rc1);
    }
}

// BoneNode_c::Limit (0x617650), tramo entre QuatToEuler y EulerToQuat. Comparaciones ordenadas (false con NaN); los valores que el EXE deja en registro
// (mínimo Z y factor de la cabeza) son double para no pasar por float; el máximo Z estrechado sí se guarda como float ([esp+8]).
__declspec(noinline) void __cdecl BoneNodeLimitClampCpp(float* euler, const float* limitMin, const float* limitMax, bool narrowZ, float poseZ) {
    if (euler[0] < limitMin[0]) {
        euler[0] = limitMin[0];
    }
    if (euler[0] > limitMax[0]) {
        euler[0] = limitMax[0];
    }
    if (euler[1] < limitMin[1]) {
        euler[1] = limitMin[1];
    }
    if (euler[1] > limitMax[1]) {
        euler[1] = limitMax[1];
    }

    double minZ = limitMin[2];
    float  maxZ = limitMax[2];
    if (narrowZ) {
        const float absX = std::bit_cast<float>(std::bit_cast<unsigned int>(euler[0]) & 0x7FFFFFFFu); // fabs
        double factor    = 1.0 - double(absX) * kBoneNodeInv45;
        if (0.0 > factor) {
            factor = 0.0;
        }
        minZ = (minZ - double(poseZ)) * factor + double(poseZ);
        maxZ = float((double(maxZ) - double(poseZ)) * factor + double(poseZ));
    }
    if (euler[2] < minZ) {
        euler[2] = float(minZ);
    }
    if (euler[2] > maxZ) {
        euler[2] = maxZ;
    }
}

// BoneNode_c::CalcWldMat (0x616CD0), matriz local: s = 2/(((x² + y²) + z²) + w²) y productos (s·a)·b con el redondeo a 24 bits del EXE.
__declspec(noinline) void __cdecl BoneNodeLocalMatrixCpp(const float* q, float* right, float* up, float* at) {
    const float qx = q[0];
    const float qy = q[1];
    const float qz = q[2];
    const float qw = q[3];
    const float s  = 2.0f / (((qx * qx + qy * qy) + qz * qz) + qw * qw);

    const float sx = s * qx;
    const float sy = s * qy;
    const float sz = s * qz;
    const float xw = sx * qw;
    const float yw = sy * qw;
    const float zw = sz * qw;
    const float xx = sx * qx;
    const float yy = sy * qy;
    const float zz = sz * qz;
    const float zy = sz * qy;
    const float xz = sx * qz;
    const float yx = sy * qx;

    right[0] = 1.0f - (zz + yy);
    right[1] = zw + yx;
    right[2] = xz - yw;
    up[0]    = yx - zw;
    up[1]    = 1.0f - (zz + xx);
    up[2]    = zy + xw;
    at[0]    = xz + yw;
    at[1]    = zy - xw;
    at[2]    = 1.0f - (yy + xx);
}
#pragma float_control(pop)

// --- Lote de tres archivos (cHandlingDataMgr y otros; 15/10/2026) ---
// Producto de dos float con la fmul del x87 (un solo redondeo); en x87 los subnormales se conservan (el SSE de la DLL los descartaria). Lo usan cHandlingDataMgr::ConvertBikeData* y CPed::EmitFootRainParticles.
__declspec(noinline) float __cdecl X87TimesFloatCpp(float a, float b) {
    return a * b;
}

// --- cHandlingDataMgr (lote w20) ---
__declspec(noinline) float __cdecl HandlingAtofTimes04Cpp(double value) {
    volatile float result = float(value * 0.4); // fmul qword [0x86A950] a 24 bits y fstp dword
    return result;
}

__declspec(noinline) float __cdecl HandlingSinScaledCpp(float angle, float k) {
    return float(sin(double(angle) * double(k))); // fld, fmul dword, fsin, fstp dword (como TrigCompoundScaledSinMulAdd)
}

// cHandlingDataMgr::ConvertDataToGameUnits (0x6F5080): todo el cuerpo aritmetico, con la pila x87 del EXE: accel y la velocidad (ST1/ST0 en el bucle)
// viajan SIN redondear a float (solo el fst dword a memoria las redondea; un float subnormal guardado sigue siendo normal en ST0), y 1/masa tambien
// va sin redondear a la multiplicacion del dano por colision. El codigo C++ normal de la DLL (SSE) descarta los subnormales y redondeaba antes de
// seguir (banco CppX87Precision1, PC24: 127-143 de 1024 casos). Orden estricto del EXE (sin reasociar); NaN: la comparacion falla y el bucle sale.
//   0x6F5085..0x6F50E9: accel = K1*acc; vel = K2*maxVel (se guarda en [esp+8] y en +0x84); freno = K1*freno; 1/masa; flotabilidad = masa*0.8/pct (fidiv);
//   dano = (1/masa)*dano*2000. 0x6F50F0..0x6F5158: bucle de reduccion (0,01 y 1/6 double; 1000 y 0,5 float). 0x6F515A..0x6F5229: ramas por tipo.
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl HandlingConvertGameUnitsCpp(float accelConst, float velocityConst, float mass, int percentSubmerged, float drag, int vehicleId,
                                                              bool maxspLimit, bool driveType4, float* engineAccel, float* maxVelocity, float* brake,
                                                              float* massRecpr, float* buoyancy, float* collision, float* maxFlat, float* maxReverse) {
    const double oneSixth = std::bit_cast<double>(0x3FC5555555555554ull); // 0x872350
    const double accel = double(accelConst) * double(*engineAccel);
    *engineAccel = float(accel);                                          // fst dword [+0x7c]
    const double velocity = double(velocityConst) * double(*maxVelocity);
    volatile float velocityMem = float(velocity);                         // fst dword [esp+8]
    *maxVelocity = velocityMem;                                           // fst dword [+0x84]
    *brake = float(double(accelConst) * double(*brake));
    const double recpr = 1.0 / double(mass);
    *massRecpr = float(recpr);                                            // fst dword [+8]
    *buoyancy = float(double(mass) * double(0.8f) / double(percentSubmerged));
    *collision = float(recpr * double(*collision) * double(2000.0f));

    double v = velocity;
    while (v > 0.0) {                                                     // fcom [0x858B50]; test ah, 0x41
        v = v - 0.01;                                                     // fsub qword [0x85A308]
        const double limit = accel * oneSixth;                            // fmul qword [0x872350]
        double resistance;
        if (drag < 0.01f) {                                               // fcomp [0x858C58]; test ah, 5; jp
            resistance = -((1.0 / (v * v * double(drag) + 1.0) - 1.0) * v);
        } else {                                                          // tambien con NaN
            resistance = double(drag) / double(1000.0f) * double(0.5f) * v * v;
        }
        if (!(limit < resistance)) {                                      // fcomp st(1); test ah, 5; jnp
            break;
        }
    }

    const double minusPointTwo = double(-0.2f);                           // 0x858FDC
    if (vehicleId == 38) {                                                // VT_RCBANDIT: velocidad sin reducir
        *maxFlat = velocityMem;
        *maxReverse = -velocityMem;
    } else if (maxspLimit) {                                              // 0x863278 = 0.8333333f; 0x85C654 = -0.25f; min con -0,2f (NaN se conserva)
        const double flat = double(velocityMem) * double(0.8333333f);
        *maxFlat = float(flat);
        double reverse = flat * double(-0.25f);
        if (minusPointTwo < reverse) {
            reverse = minusPointTwo;
        }
        *maxReverse = float(reverse);
    } else if (vehicleId >= 162 && vehicleId <= 174) {                    // VT_BIKE..VT_FREEWAY (cmp con signo)
        *maxFlat = float(v);
        *maxReverse = -0.05f;                                             // 0xBD4CCCCD
        *maxVelocity = float(v * double(1.2f));                           // 0x858F08
    } else {
        volatile float flatMem = float(v);                                // fst dword [+0x88]; despues fld dword [+0x88] (ya redondeada)
        *maxFlat = flatMem;
        *maxVelocity = float(v * double(1.2f));
        double reverse = double(flatMem) * double(-0.3f);                 // 0x858EE4
        if (minusPointTwo < reverse) {
            reverse = minusPointTwo;
        }
        *maxReverse = float(reverse);
    }
    *engineAccel = float(accel * double(driveType4 ? 0.25f : 0.5f));      // 0x858C84 / 0x858B8C
}
#pragma float_control(pop)

// --- Residuo de los registros x87 muertos (usado por Ftol2RegsCore, CTaskSimpleGangDriveBy y CTaskComplexEnterCar) ---
// Las operaciones del EXE dejan en los registros por debajo de la cima (T-1, T-2, T-3) los ultimos valores que escribieron, y no tocan ningun otro. El compilador no emite
// fiadd/fcom con operando de memoria sin sacar un registro, ni conserva una copia (fld st(1)) para fcompp, asi que lo que deja cada secuencia propia sale distinto. En lugar de
// imitar cada instruccion, la logica real se compila con una pila que nunca llega mas profundo que la del EXE (este nunca escribe en el registro siguiente al ultimo que usa) y estas
// funciones escriben al final los valores del EXE con aritmetica neutra: sumas con +0 y productos por 1,0 exactos (sin PE, DE, UE ni IE: los operandos son float ya redondeados
// a 24 bits o ceros) que no cambian C0..C3 (se comprobo en el hardware: fadd, fmul, fld, fstp, fild, fistp, fpatan y fsqrt no tocan C0, C2 ni C3).
// Los registros llegan por la pila (parametros double): el llamador solo usa T-1 para cargarlos. Las constantes son volatile para que el compilador las cargue como operandos
// de registro y no las pliegue; en ((0 + 0) + (0 + x)) la pila llega a tres registros y el faddp saca `x` de T-3 sin modificarlo (igual en el resto de las sumas).
static const volatile float kPlantOne = 1.0f;
static const volatile float kPlantZeroA = 0.0f;
// Cada sentencia escribe en un registro (T-k) el valor llegado y deja basura en los de encima, que las sentencias siguientes (T-(k-1) .. T-1) sobrescriben. Con precise el
// compilador conserva el arbol: una suma de 2^(k-1) hojas "0 * 1" y "1 * d" a la derecha del todo, que necesita k registros; el ultimo faddp saca `d` de T-k tal cual.
// (Sin precise, /fp:fast reasocia la suma y la pila no llega a k registros.)
#define X87_PLANT_ZERO (kPlantZeroA * kPlantOne)
#pragma float_control(precise, on, push)
static __declspec(noinline) void X87PlantStmt5Cpp(double d) {
    volatile double sink;
    sink = (((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)) + ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)))
         + (((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)) + ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + (kPlantOne * d))));
}
static __declspec(noinline) void X87PlantStmt4Cpp(double d) {
    volatile double sink;
    sink = ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)) + ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + (kPlantOne * d)));
}
static __declspec(noinline) void X87PlantStmt3Cpp(double d) {
    volatile double sink;
    sink = (X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + (kPlantOne * d));
}
static __declspec(noinline) void X87PlantStmt2Cpp(double d) {
    volatile double sink;
    sink = X87_PLANT_ZERO + (kPlantOne * d);
}
static __declspec(noinline) void X87PlantStmt1Cpp(double d) {
    volatile double sink;
    sink = kPlantOne * d;
}
#pragma float_control(pop)
// T-2 = d2 (T-1 queda con una suma: basura que el EXE tambien sobrescribe).
static void X87PlantT2Cpp(double d2) { X87PlantStmt2Cpp(d2); }
// T-3 = d3, T-2 = d2.
static void X87PlantT3T2Cpp(double d3, double d2) { X87PlantStmt3Cpp(d3); X87PlantStmt2Cpp(d2); }
// T-3 = d3, T-2 = d2, T-1 = d1.
static void X87PlantT3T2T1Cpp(double d3, double d2, double d1) { X87PlantStmt3Cpp(d3); X87PlantStmt2Cpp(d2); X87PlantStmt1Cpp(d1); }
// T-4 = d4, T-3 = d3, T-2 = d2.
static void X87PlantT4T3T2Cpp(double d4, double d3, double d2) { X87PlantStmt4Cpp(d4); X87PlantStmt3Cpp(d3); X87PlantStmt2Cpp(d2); }
// T-5 .. T-1.
static void X87PlantT5T4T3T2T1Cpp(double d5, double d4, double d3, double d2, double d1) {
    X87PlantStmt5Cpp(d5); X87PlantStmt4Cpp(d4); X87PlantStmt3Cpp(d3); X87PlantStmt2Cpp(d2); X87PlantStmt1Cpp(d1);
}

// --- Lote Ped (15/10/2026) ---
// --- Lote de Ped.cpp (15/10/2026), prefijo Ped ---
// IMPORTANTE: este bloque debe ir DESPUES de PedGeomShotAngleCpp (static, X87Math.cpp ~l.1640), que reutiliza.

// _ftol2 (0x821B40) sobre el valor de ST0: ver X87Math.h. fld st; fst dword [esp+18h]; fistp qword; fild qword; fsubp (v - R, a la precision vigente); fstp dword.
// R = fistp con el RC vigente (al par, hacia -inf, hacia +inf o hacia 0): se calcula con enteros desde los bits del double v (exacto para un float
// ensanchado y para un ST0 de hasta 53 bits). La fraccion v - R se calcula con la propia x87 (fild + fsub + fstp dword): asi sigue la precision y el redondeo vigentes
// (y el cero exacto sale +0 salvo en "hacia -inf", donde x - x = -0) sin reproducirlos a mano. EAX:EDX salen siempre truncados, como en el EXE.
static long long X87FistpQword(double v, unsigned int roundingControl) {
    const unsigned long long bits = std::bit_cast<unsigned long long>(v);
    const unsigned int       biased = static_cast<unsigned int>(bits >> 52) & 0x7FFu;
    const bool               neg    = (bits >> 63) != 0;
    if (biased >= 0x3FFu + 63u) { // |v| >= 2^63, infinitos y NaN (v = -2^63 da lo mismo): entero indefinido
        return static_cast<long long>(0x8000000000000000ull);
    }
    const unsigned long long mantissa = (bits & 0xFFFFFFFFFFFFFull) | (biased != 0 ? 0x10000000000000ull : 0ull);
    const int                shift    = static_cast<int>(biased != 0 ? biased : 1u) - 1075; // v = mantissa * 2^shift
    unsigned long long magnitude;
    if (shift >= 0) {
        magnitude = mantissa << shift; // ya es entero (|v| < 2^63)
    } else {
        const unsigned int rs = static_cast<unsigned int>(-shift);
        unsigned long long q, rem, half;
        if (rs >= 64u) { // |v| < 2^-11: solo cuenta que no sea cero
            q = 0; rem = mantissa; half = ~0ull;
        } else {
            q = mantissa >> rs; rem = mantissa & ((1ull << rs) - 1ull); half = 1ull << (rs - 1u);
        }
        const bool exact = rem == 0;
        bool up;
        switch (roundingControl) {
        case _RC_DOWN: up = neg && !exact; break;  // hacia -inf: sube la magnitud de los negativos
        case _RC_UP:   up = !neg && !exact; break; // hacia +inf: sube la magnitud de los positivos
        case _RC_CHOP: up = false; break;
        default:       up = rem > half || (rem == half && (q & 1ull) != 0); break; // al par
        }
        magnitude = q + (up ? 1ull : 0ull);
    }
    return neg ? -static_cast<long long>(magnitude) : static_cast<long long>(magnitude);
}

static Ftol2Regs Ftol2RegsCore(double v, unsigned int entryEcx) {
    const bool         neg     = (std::bit_cast<unsigned long long>(v) >> 63) != 0;
    const long long    rounded = X87FistpQword(v, _control87(0, 0) & _MCW_RC);
    const unsigned int low     = unsigned(static_cast<unsigned long long>(rounded));
    const unsigned int high    = unsigned(static_cast<unsigned long long>(rounded) >> 32);
    if (low == 0 && (high & 0x7FFFFFFFu) == 0) { // 0x821B9F: sale con ECX de entrada, EAX = 0, EDX = alta
        // El EXE ya ha ejecutado fild R (R = 0, o -2^63 si v es NaN o no cabe en 64 bits) y saca R y v con dos fstp dword: los registros muertos quedan con R en T-2 y v en T-1,
        // sin tocar ninguna bandera (el camino normal deja ahi R y v - R).
        X87PlantStmt2Cpp(double(rounded));
        X87PlantStmt1Cpp(v);
        return { 0, entryEcx, high };
    }
    // fild R; fsubp (v - R a la precision vigente); fstp dword. El compilador puede emitir fild R; fsubr [v] (un solo registro) y entonces T-2 no recibiria R: los registros muertos
    // del EXE (T-2 = R, T-1 = v - R) se plantan al final, sea cual sea la secuencia que salga.
    volatile double difference = v - double(rounded);
    volatile float fraction = float(difference);
    const unsigned int fracBits = *reinterpret_cast<const volatile unsigned int*>(&fraction); // mov ecx, [esp] del EXE: lectura entera, no por la x87
    X87PlantStmt2Cpp(double(rounded));
    X87PlantStmt1Cpp(difference);
    if (!neg) { // add ecx,7FFFFFFFh / sbb eax,0 / sbb edx,0
        const unsigned int ecx    = fracBits + 0x7FFFFFFFu;
        const unsigned int carry  = ecx < fracBits ? 1u : 0u;
        const unsigned int borrow = low < carry ? 1u : 0u;
        return { low - carry, ecx, high - borrow };
    }
    const unsigned int t     = fracBits ^ 0x80000000u; // xor ecx,80000000h / add ecx,7FFFFFFFh / adc eax,0 / adc edx,0
    const unsigned int ecx   = t + 0x7FFFFFFFu;
    const unsigned int carry = ecx < t ? 1u : 0u;
    const unsigned int eax   = low + carry;
    return { eax, ecx, high + (eax < low ? 1u : 0u) };
}

__declspec(noinline) Ftol2Regs __cdecl Ftol2RegsCpp(float v, unsigned int entryEcx) {
    return Ftol2RegsCore(double(v), entryEcx);
}

// Igual, con un ST0 de hasta 53 bits de mantisa (resultado de una cadena x87 a PC24 o PC53 que no se redondeo a float): exacto en esas dos precisiones.
__declspec(noinline) Ftol2Regs __cdecl Ftol2RegsDblCpp(double v, unsigned int entryEcx) {
    return Ftol2RegsCore(v, entryEcx);
}

// CPed::GetLocalDirection (0x5DEF60..0x5DEFA9 + _ftol2): fpatan(-x, y) sin redondear; fsub de la rotacion (ped+0x558); + pi/4 (0x859AB0); mientras < 0 (fcom ordenado: con NaN no
// entra) + 2pi (0x858CBC); * 2/pi (0x858FB8); _ftol2 con ECX = this. Sin ningun almacenamiento a float entre el fpatan y _ftol2 (ver PedGeomShotAngleDbl).
#pragma float_control(precise, on, push)
__declspec(noinline) Ftol2Regs __cdecl PedLocalDirectionRegsCpp(const float* point, float rotation, unsigned int entryEcx) {
    const double quarter = double(std::bit_cast<float>(0x3F490FDBu)); // 0x859AB0
    const double full    = double(std::bit_cast<float>(0x40C90FDBu)); // 0x858CBC
    const double scale   = double(std::bit_cast<float>(0x3F22F983u)); // 0x858FB8
    double value = PedGeomShotAngleDbl(-double(point[0]), double(point[1]), rotation) + quarter; // -x por fchs: conserva el -0
    while (value < 0.0) {
        value += full;
    }
    value *= scale;
    return Ftol2RegsDblCpp(value, entryEcx);
}
#pragma float_control(pop)

// CPed::SetPedPositionInCar (0x5DF910) y CPed::PositionAttachedPed (0x5DFDF0): fpatan(-x, y) [+ add] como double en ST0 sin redondear (extern "C"; Ped.cpp
// los declara como float con otro nombre via /alternatename, igual que Placeable.cpp). El fadd de add redondea a 24 bits como en el EXE. Sin precise: fpatan en linea.
extern "C" __declspec(noinline) double __cdecl PedAtanNegXYDblCpp(float x, float y) {
    return atan2(-double(x), double(y));
}
extern "C" __declspec(noinline) double __cdecl PedAtanNegXYAddDblCpp(float x, float y, float add) {
    return atan2(-double(x), double(y)) + double(add);
}

// --- Ajuste de exactitud de Ped y RpAnimBlend (15/10/2026) ---
// --- CPed::CalculateNewVelocity (0x5E4C50), prefijo Cnv. Añadir al final de X87Math.cpp (usa ExeCompatiblePowCpp y ExeAsinDoubleCpp, ya definidos arriba) ---
__declspec(noinline) float __cdecl CnvDecayPowCpp(float base, float step) {
    return float(ExeCompatiblePowCpp(double(base), double(step)));
}

__declspec(noinline) float __cdecl CnvAsinFloatCpp(float x) {
    return float(ExeAsinDoubleCpp(double(x)));
}

// Orden estricto del EXE: asin * (1 - blend) [fmulp], blend * previous [fmul], suma [faddp]; sin reasociar.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl CnvBlendedAsinCpp(float x, float blend, float previous) {
    const double angle = ExeAsinDoubleCpp(double(x));
    const double weighted = angle * double(1.0f - blend);
    const double carried = double(blend * previous);
    return float(weighted + carried);
}

__declspec(noinline) float __cdecl CnvZeroPlusSumCpp(float first, float second) {
    volatile float zero = 0.0f; // evita que /fp:fast simplifique 0 + first
    const float accumulated = zero + first;
    return accumulated + second;
}
#pragma float_control(pop)

// --- CPed::KillPedWithCar (0x5F094E): fild random; fmul qword 0.002; fadd qword 1.5; fmul dword lift; fstp dword ---
// Con la palabra de control a 24 bits cada operación redondea a float; en SSE el doble se redondea una sola vez al final y 21% de los casos difiere.
__declspec(noinline) float __cdecl KpcBounceLiftScaleCpp(float lift, int random) {
    volatile float result = float((double(random) * 0.002 + 1.5) * double(lift));
    return result;
}

// --- Lote RpNan (RpAnimBlend, 15/10/2026): aritmética del x87 de 0x4D1680..0x4D32D0 y 0x59C080. Se puede colocar al final de X87Math.cpp. ---
// Con /arch:IA32 son fld/fadd/fsub/fmul de memoria; el proceso corre el x87 a 24 bits. El SSE de la DLL descarta subnormales (DAZ/FTZ) y con dos NaN devuelve el primer operando.
__declspec(noinline) float __cdecl RpNanAddCpp(float a, float b) { return a + b; }
__declspec(noinline) float __cdecl RpNanSubCpp(float a, float b) { return a - b; }

#pragma float_control(precise, on, push)
// Cadenas con intermedios en registros: se calculan como double para que el compilador no los guarde como float (el hardware redondea a 24 bits por la palabra de control).
__declspec(noinline) float __cdecl RpNanMulAddCpp(float a, float b, float c) {
    return float(double(a) * double(b) + double(c));
}

__declspec(noinline) float __cdecl RpNanSkinZCorrectCpp(float z, float bonePosZ) {
    const double scaled = double(z) * 2.5f;            // fmul [0x858FA0]
    const double offset = scaled + 2.0f;               // fadd [0x858CA0]
    const double term   = offset * double(bonePosZ);   // fmul [esi + 0xC]
    return float(term + double(z));                    // fadd [ebp + 0x18]; fstp
}

// 0x59C080. Los valores marcados "float" pasan por fstp dword (desbordan / se desnormalizan); los "reg" siguen en ST.
__declspec(noinline) void __cdecl RpNanQuatToMatrixCpp(const float* q, float* m) {
    const double x2  = double(q[0]) + double(q[0]);               // reg: 2x
    const double y2  = double(q[1]) + double(q[1]);               // reg: 2y
    const float  z2  = float(double(q[2]) + double(q[2]));        // float: [esp] = 2z
    const float  A   = float(x2 * q[0]);                          // x2x
    const float  B   = float(y2 * q[0]);                          // y2x
    const float  C   = float(double(z2) * q[0]);                  // z2x
    const float  D   = float(y2 * q[1]);                          // y2y
    const float  E   = float(double(z2) * q[1]);                  // z2y
    const float  F   = float(double(z2) * q[2]);                  // z2z
    const float  G   = float(x2 * q[3]);                          // x2w
    const double Y2W = y2 * q[3];                                 // reg: 2y·w, sin redondear a float
    const double Z2W = double(z2) * q[3];                         // reg: 2z·w, sin redondear a float
    m[0]  = float(1.0f - (double(F) + double(D)));                // right.x = 1 - (z2z + y2y)
    m[4]  = float(double(B) - Z2W);                               // up.x    = y2x - z2w
    m[8]  = float(Y2W + double(C));                               // at.x    = y2w + z2x
    m[1]  = float(Z2W + double(B));                               // right.y = z2w + y2x
    m[5]  = float(1.0f - (double(F) + double(A)));                // up.y    = 1 - (z2z + x2x)
    m[9]  = float(double(E) - double(G));                         // at.y    = z2y - x2w
    m[2]  = float(double(C) - Y2W);                               // right.z = z2x - y2w
    m[6]  = float(double(G) + double(E));                         // up.z    = x2w + z2y
    m[10] = float(1.0f - (double(D) + double(A)));                // at.z    = 1 - (y2y + x2x)
}
#pragma float_control(pop)

// --- CQuaternion::Get(RwV3d*, float*) (0x59C230) ---
// El EXE: fld w ; fadd st,st ; call _CIacos (0x822380) ; fld st ; fstp [angle] ; fsin ; fdivr [1.0] ; (fld st ; fmul [q.x] ; fstp [axis.x]) ; idem y ;  fmul [q.z] ; fstp [axis.z].
// Se ejecuta en la maquina FlcX87 (la misma que reproduce _CIacos del EXE, incluidos errno = 33 y el indefinido del dominio). Unica salvedad: acos(-1) es fldpi de 64 bits
// (sin banderas de inexacto) y fsin lo recibe con los 64 bits; FlcX87::Fsin lo pasaria a double, asi que ese caso (2w == -1) calcula el seno de pi + (pi_64 - pi_53) con el
// x87 real a 64 bits de precision (la suma no se redondea a double: ST0 llega a fsin) y lo apila ya como valor de registro.
static __declspec(noinline) double ExtendedPiSinCpp() {
    // pi_64 = 0xC90FDAA22168C235 * 2^-62 = kPiHi + kPiLo, exacto: kPiHi es el double mas cercano (0x400921FB54442D18) y kPiLo = pi_64 - kPiHi (11 bits significativos).
    static const volatile double kPiHi = 0x1.921fb54442d18p+1;                  // 0x400921FB54442D18
    static const volatile double kPiLo = 0x1.1a80000000000p-53;   // 3CA1A80000000000 = pi_64 - kPiHi (exacto)
    unsigned previous = 0, ignored = 0;
    __control87_2(0, 0, &previous, nullptr);
    __control87_2(_PC_64, _MCW_PC, &ignored, nullptr);
    const double result = sin(kPiHi + kPiLo);
    __control87_2(previous, _MCW_PC, &ignored, nullptr);
    return result;
}

__declspec(noinline) void __cdecl QuaternionGetAxisAngleCpp(const float* q, float* axis, float* angle) {
    static const float kOne = 1.0f;   // [0x858624]
    FlcX87 fp;
    fp.Fld(q[3]);                           // 59C233 fld [esi + 0xC]
    fp.Arith('+', 0, 0, 0, false);          // 59C236 fadd st(0), st(0)
    FlcCrtAcos(fp);                         // 59C238 call 0x822380
    fp.FldSt(0);                            // 59C23D fld st(0)
    fp.Fstp(*angle);                        // 59C243 fstp [angle]
    const FlcExt top = fp.St(0);
    if (top.kind == FLC_NORMAL && top.neg == 0 && top.sig == 0xC90FDAA22168C235ull && top.exp == 1) {
        const double sine = ExtendedPiSinCpp();   // fsin de pi de 64 bits (fldpi sin redondeo a 53 bits)
        fp.Pop();
        fp.FldExt(FlcFromDoubleExact(sine));
        fp.state |= FLC_PE;
        fp.cc &= ~FLC_C2;
        fp.c1 = false;
        fp.lastClass = 3;
    } else {
        fp.Fsin();                          // 59C249 fsin
    }
    fp.Fdivr(kOne);                         // 59C24B fdivr [0x858624]
    fp.FldSt(0);                            // 59C251
    fp.Fmul(q[0]);                          // 59C253 fmul [esi]
    fp.Fstp(axis[0]);                       // 59C255 fstp [eax]
    fp.FldSt(0);                            // 59C257
    fp.Fmul(q[1]);                          // 59C259 fmul [esi + 4]
    fp.Fstp(axis[1]);                       // 59C25C fstp [eax + 4]
    fp.Fmul(q[2]);                          // 59C25F fmul [esi + 8]
    fp.Fstp(axis[2]);                       // 59C263 fstp [eax + 8]
    fp.Commit();
}


// --- RpAnimBlendClumpUpdateAnimations (0x4D34F0; lote w20) ---
// Una sola expresión y sin reasociar (precise): la x87 de 24 bits redondea cada operación; en doubles de SSE el resultado solo se redondea al final y difiere.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl RpUpdateSyncAccumCpp(float total, float speed, float blend, float acc) {
    return float(double(total) / double(speed) * double(blend) + double(acc));
}
__declspec(noinline) float __cdecl RpUpdateSyncStepCpp(float total, float totalTime, float totalBlend, float timeStep) {
    return float(double(total) / double(totalTime) * double(totalBlend) * double(timeStep));
}
#pragma float_control(pop)

// --- Lote de doce archivos (AEVehicleAudioEntity, TaskSimpleJetPack, PathFind, TaskSimpleSwim, Bmx, PedIK, WaterLevel, MemoryMgr, Physical, Radar, Pad, Matrix; 15/10/2026) ---
// --- Lote Matrix.cpp (w20), prefijo Matrix. Poner al final de X87Math.cpp (tras HoldEntityRotateZCpp, que CMatrix::RotateZ reutiliza). ---
// m = CMatrix como 16 floats: right@0, flags@3, forward@4, up@8, pos@12 (misma disposicion que RwMatrix). Todos los ayudantes son noinline.
// Fuera de precise (sin cadenas de tres terminos): fsin/fcos/fpatan/fsqrt salen en linea. Los valores que el EXE deja en la pila x87 son double
// (sin redondear a float); los que guarda con fst/fstp a la pila del llamado son `volatile float` y se releen igual.
static constexpr float kMatrix_36000000 = std::bit_cast<float>(0x36000000u); // 1,9073486e-6 (0x866D90): umbral de bloqueo cardan de ConvertToEulerAngles

// CMatrix::SetRotateXOnly (0x59AFA0): right = {1,0,0}, forward.x = 0, forward.y = cos, forward.z = sin, up.x = 0, up.y = -sin, up.z = cos; no toca flags, relleno ni posicion.
__declspec(noinline) void __cdecl MatrixSetRotateXOnlyCpp(float* m, float angle) {
    const double c = cos(double(angle));
    const double s = sin(double(angle));
    m[0] = 1.0f; m[1] = 0.0f; m[2] = 0.0f; m[4] = 0.0f; m[8] = 0.0f;
    m[5] = float(c); m[6] = float(s); m[9] = float(-s); m[10] = float(c);
}
// CMatrix::SetRotateYOnly (0x59AFE0): right = {cos, 0, -sin}, forward = {0, 1, 0}, up = {sin, 0, cos}.
__declspec(noinline) void __cdecl MatrixSetRotateYOnlyCpp(float* m, float angle) {
    const double c = cos(double(angle));
    const double s = sin(double(angle));
    m[1] = 0.0f; m[4] = 0.0f; m[5] = 1.0f; m[6] = 0.0f; m[9] = 0.0f;
    m[0] = float(c); m[2] = float(-s); m[8] = float(s); m[10] = float(c);
}
// CMatrix::SetRotateZOnly (0x59B020): right = {cos, sin, 0}, forward = {-sin, cos, 0}, up = {0, 0, 1}.
__declspec(noinline) void __cdecl MatrixSetRotateZOnlyCpp(float* m, float angle) {
    const double c = cos(double(angle));
    const double s = sin(double(angle));
    m[2] = 0.0f; m[6] = 0.0f; m[8] = 0.0f; m[9] = 0.0f; m[10] = 1.0f;
    m[0] = float(c); m[1] = float(s); m[4] = float(-s); m[5] = float(c);
}

// CMatrix::RotateX (0x59B1E0): por fila (y, z): y' = c*y - s*z, z' = s*y + c*z (right, forward, up y pos); fcos sin redondear, fsin guardado como float,
// un redondeo a 24 bits por operacion y un fstp float por resultado. Todas las lecturas van antes de las escrituras. Equivale al PedIKRotateXCpp propuesto por el lote PedIK: dejar solo uno.
__declspec(noinline) void __cdecl MatrixRotateXCpp(float* m, float angle) {
    const double c = cos(double(angle));
    volatile float storedSin = float(sin(double(angle)));
    const double s = storedSin;
    const double ry = m[1],  rz = m[2];
    const double fy = m[5],  fz = m[6];
    const double uy = m[9],  uz = m[10];
    const double py = m[13], pz = m[14];
    m[5]  = float(c * fy - s * fz);
    m[6]  = float(s * fy + c * fz);
    m[9]  = float(c * uy - s * uz);
    m[10] = float(s * uy + c * uz);
    m[13] = float(c * py - s * pz);
    m[14] = float(s * py + c * pz);
    m[1]  = float(c * ry - s * rz);
    m[2]  = float(s * ry + c * rz);
}
// CMatrix::RotateY (0x59B2C0): por fila (x, z): x' = s*z + c*x, z' = c*z - s*x. right.x usa el seno SIN redondear (fst deja el valor en ST0); el resto usa el seno guardado como float.
// Equivale al PedIKRotateYCpp propuesto por el lote PedIK: dejar solo uno.
__declspec(noinline) void __cdecl MatrixRotateYCpp(float* m, float angle) {
    const double c = cos(double(angle));
    const double sFull = sin(double(angle));
    volatile float storedSin = float(sFull);
    const double s = storedSin;
    const double rx = m[0],  rz = m[2];
    const double fx = m[4],  fz = m[6];
    const double ux = m[8],  uz = m[10];
    const double px = m[12], pz = m[14];
    m[4]  = float(s * fz + c * fx);
    m[6]  = float(c * fz - s * fx);
    m[8]  = float(s * uz + c * ux);
    m[10] = float(c * uz - s * ux);
    m[12] = float(s * pz + c * px);
    m[14] = float(c * pz - s * px);
    m[0]  = float(sFull * rz + c * rx);
    m[2]  = float(c * rz - s * rx);
}
// CMatrix::RotateZ (0x59B390): se reutiliza HoldEntityRotateZCpp (misma secuencia del EXE; comprobado instruccion a instruccion).

// Trigonometria y raiz fuera de precise (en linea: fcos, fsin, 1/fsqrt) para las funciones de abajo que SI necesitan precise (sin reasociar productos/sumas).
static __declspec(noinline) double __cdecl MatrixCosDbl(double angle) { return cos(angle); }
static __declspec(noinline) double __cdecl MatrixSinDbl(double angle) { return sin(angle); }
static __declspec(noinline) double __cdecl MatrixInvSqrtDbl(double sum) { return 1.0 / sqrt(sum); } // fsqrt; fdivr [1.0f] (0x858624)

#pragma float_control(precise, on, push)
// CMatrix::SetRotate(x, y, z) (0x59B120). Pila x87 del EXE: cos/sin(x), sin(y), cos(z) y sin(z) sin redondear; se guardan como float cos(y) (fstp), sin(z) (fst) y cos(z)*sin(x) (fstp).
//   a = sin(z)*sin(x) (ST sin redondear). Pone la posicion a cero; no toca flags ni relleno.
__declspec(noinline) void __cdecl MatrixSetRotateCpp(float* m, float x, float y, float z) {
    const double cx = MatrixCosDbl(double(x)), sx = MatrixSinDbl(double(x));
    const double cy = MatrixCosDbl(double(y));
    volatile float storedCy = float(cy);
    const double sy = MatrixSinDbl(double(y));
    const double cz = MatrixCosDbl(double(z));
    const double sz = MatrixSinDbl(double(z));
    volatile float storedSz = float(sz);
    const double a = sz * sx;
    volatile float storedB = float(cz * sx);
    const double CY = storedCy, SZ = storedSz, B = storedB;
    m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f;
    m[0]  = float(cz * CY - a * sy);
    m[1]  = float(B * sy + SZ * CY);
    m[2]  = float(-(sy * cx));
    m[4]  = float(-(SZ * cx));
    m[5]  = float(cz * cx);
    m[6]  = float(sx);
    m[8]  = float(cz * sy + a * CY);
    m[9]  = float(SZ * sy - B * CY);
    m[10] = float(CY * cx);
}

// CMatrix::Reorthogonalise (0x59B6A0). Productos vectoriales y normalizaciones sin comprobar cero. Orden del EXE (los `volatile float` son los fst/fstp a la pila del llamado):
//   T1 = ry*fz - fy*rz (ST), T2 = fx*rz - rx*fz (float), T3 = fy*rx - fx*ry (ST y float); s = (T3*T3f + T2f*T2f) + T1*T1; inv = 1/sqrt(s) (ST y float);
//   up = (T1*inv, invF*T2f, invF*T3f -> float); W1 = V3*fy - V2*fz (ST), W2 = V1*fz - V3f*fx (float), W3 = V2*fx - V1*fy (ST y float);
//   s2 = (W3*W3f + W2f*W2f) + W1*W1; inv2 = 1/sqrt(s2) (float); right = (W1*inv2F, W2f*inv2F, W3f*inv2F -> float);
//   forward = (X3*V2 - X2*V3f, X1*V3f - X3f*V1, X2*V1 - X1*V2). Escribe right, forward y up; no toca flags ni posicion. Las lecturas de m van antes de las escrituras.
__declspec(noinline) void __cdecl MatrixReorthogonaliseCpp(float* m) {
    const double rx = m[0], ry = m[1], rz = m[2];
    const double fx = m[4], fy = m[5], fz = m[6];
    const double t1 = ry * fz - fy * rz;
    volatile float t2f = float(fx * rz - rx * fz);
    const double t3 = fy * rx - fx * ry;
    volatile float t3f = float(t3);
    const double sum1 = (t3 * double(t3f) + double(t2f) * double(t2f)) + t1 * t1;
    const double inv1 = MatrixInvSqrtDbl(sum1);
    volatile float inv1f = float(inv1);
    const double v1 = t1 * inv1;
    const double v2 = double(inv1f) * double(t2f);
    const double v3 = double(inv1f) * double(t3f);
    volatile float v3f = float(v3);
    const double w1 = v3 * fy - v2 * fz;
    volatile float w2f = float(v1 * fz - double(v3f) * fx);
    const double w3 = v2 * fx - v1 * fy;
    volatile float w3f = float(w3);
    const double sum2 = (w3 * double(w3f) + double(w2f) * double(w2f)) + w1 * w1;
    const double inv2 = MatrixInvSqrtDbl(sum2);
    volatile float inv2f = float(inv2);
    const double x1 = w1 * double(inv2f);
    const double x2 = double(w2f) * double(inv2f);
    const double x3 = double(w3f) * double(inv2f);
    volatile float x3f = float(x3);
    const float y1 = float(x3 * v2 - x2 * double(v3f));
    const float y2 = float(x1 * double(v3f) - double(x3f) * v1);
    const float y3 = float(x2 * v1 - x1 * v2);
    m[2]  = x3f;
    m[4]  = y1;
    m[5]  = y2;
    m[10] = v3f;
    m[6]  = y3;
    m[0]  = float(x1);
    m[1]  = float(x2);
    m[8]  = float(v1);
    m[9]  = float(v2);
}
#pragma float_control(pop)

// sqrt(a*a + b*b) con la suma en el orden del EXE (a*a primero) y la raiz sin redondear a float (ST0 para fcom y fpatan). Aparte para que /fp:fast no reescriba la comparacion con el umbral.
static __declspec(noinline) double __cdecl MatrixEulerSqrtSumDbl(double a, double b) { return sqrt(a * a + b * b); }

// CMatrix::ConvertToEulerAngles (0x59A840). m = matriz (se copian right, forward y up a M[9] antes de escribir, de modo que pX/pY/pZ pueden apuntar dentro de m);
// i, j, k = ejes que el EXE saca de las banderas (Idx1/Idx2; ver Matrix.cpp); flags: bit 0 = intercambiar X y Z al final, bit 1 = repeticion (angulos de Euler propios),
// bit 2 = paridad (niega los tres angulos). s = sqrt(...) sin redondear; con s <= 1,9073486e-6 o NaN se toma la rama de bloqueo cardan (Z = 0).
// Las escrituras (y las relecturas de la negacion y el intercambio) son volatile y en el orden del EXE (X, Y, Z), por si los punteros se solapan.
__declspec(noinline) void __cdecl MatrixConvertToEulerCpp(const float* m, float* pX, float* pY, float* pZ, unsigned int flags, int i, int j, int k) {
    const float M[9] = { m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10] };
    volatile float* const px = pX;
    volatile float* const py = pY;
    volatile float* const pz = pZ;
    const double eps = double(kMatrix_36000000);
    if ((flags & 2u) != 0) {
        const double p = M[3 * i + j], q = M[3 * i + k];
        const double s = MatrixEulerSqrtSumDbl(q, p);
        if (s > eps) {
            *px = float(atan2(q, p));
            *py = float(atan2(s, double(M[4 * i])));
            *pz = float(atan2(double(M[3 * k + i]), -double(M[3 * j + i])));
        } else {
            *px = float(atan2(-double(M[3 * k + j]), double(M[4 * k])));
            *py = float(atan2(s, double(M[4 * i])));
            *pz = 0.0f;
        }
    } else {
        const double a1 = M[3 * k + i], d = M[4 * i];
        const double s = MatrixEulerSqrtSumDbl(a1, d);
        if (s > eps) {
            *px = float(atan2(double(M[3 * j + k]), double(M[4 * j])));
            *py = float(atan2(-double(M[3 * j + i]), s));
            *pz = float(atan2(a1, d));
        } else {
            *px = float(atan2(-double(M[3 * k + j]), double(M[4 * k])));
            *py = float(atan2(-double(M[3 * j + i]), s));
            *pz = 0.0f;
        }
    }
    if ((flags & 4u) != 0) {
        *px = -*px;
        *py = -*py;
        *pz = -*pz;
    }
    if ((flags & 1u) != 0) {
        const float z = *pz;
        const float x = *px;
        *px = z;
        *pz = x;
    }
}

// CMatrix::ConvertFromEulerAngles (0x59AA40). Escribe solo los 9 elementos de right/forward/up (m[4*fila+columna]). flags como en ConvertToEulerCpp; i, j, k = ejes del EXE.
// bit 0: (A, B, C) = (z, y, x) en vez de (x, y, z); bit 2: se niegan A, B y C antes del trigonometrico. cos(A), sin(A), cos(B), sin(B) y cos(C) se guardan como float (fstp);
// sin(C) queda SIN redondear en ST0; t10 = cC*cA, t14 = cA*sC, t18 = sA*cC y t54 = sA*sC se guardan como float.
__declspec(noinline) void __cdecl MatrixConvertFromEulerCpp(float* m, float x, float y, float z, unsigned int flags, int i, int j, int k) {
    double angA = double((flags & 1u) != 0 ? z : x);
    double angB = double(y);
    double angC = double((flags & 1u) != 0 ? x : z);
    if ((flags & 4u) != 0) {
        angA = -angA;
        angB = -angB;
        angC = -angC;
    }
    volatile float vcA = float(cos(angA));
    volatile float vcB = float(cos(angB));
    volatile float vcC = float(cos(angC));
    volatile float vsA = float(sin(angA));
    volatile float vsB = float(sin(angB));
    const double sC = sin(angC);
    const double cA = vcA, cB = vcB, cC = vcC, sA = vsA, sB = vsB;
    volatile float v10 = float(cC * cA);
    volatile float v14 = float(cA * sC);
    volatile float v18 = float(sA * cC);
    volatile float v54 = float(sA * sC);
    const double t10 = v10, t14 = v14, t18 = v18, t54 = v54;
    if ((flags & 2u) != 0) {
        m[5 * i]         = float(cB);
        m[4 * i + k]     = float(sB * sA);
        m[4 * i + j]     = float(sB * cA);
        m[4 * k + i]     = float(sC * sB);
        m[5 * k]         = float(t10 - t54 * cB);
        m[4 * k + j]     = float(-(t14 * cB) - t18);
        m[4 * j + i]     = float(-(sB * cC));
        m[4 * j + k]     = float(t18 * cB + t14);
        m[5 * j]         = float(t10 * cB - t54);
    } else {
        m[5 * i]         = float(cC * cB);
        m[4 * i + k]     = float(t18 * sB - t14);
        m[4 * i + j]     = float(t10 * sB + t54);
        m[4 * k + i]     = float(sC * cB);
        m[5 * k]         = float(t54 * sB + t10);
        m[4 * k + j]     = float(t14 * sB - t18);
        m[4 * j + i]     = float(-sB);
        m[4 * j + k]     = float(sA * cB);
        m[5 * j]         = float(cB * cA);
    }
}

// CPedIK::PitchForSlope (0x5FE721): fld angle; fcos; fdivr num; fstp.
__declspec(noinline) float __cdecl PedIKDivCosCpp(float num, float angle) {
    return float(double(num) / cos(double(angle)));
}

// CPedIK::RotateTorsoForArm (0x5FDFB4..0x5FE000). Las dos diferencias (fsub, 24 bits) se guardan como float -exactas- para que /fp:fast no convierta -(a - b) en b - a (cambiaria el signo de un cero).
// Despues: heading = fpatan(-dx, dy) sin redondear; limite superior = rot + pi (24 bits): si heading > limite -> heading - 2pi; si no, inferior = rot - pi: si heading < inferior -> heading + 2pi (con NaN, ninguna rama); resultado = float(heading - rot).
__declspec(noinline) float __cdecl PedIKArmHeadingDiffCpp(const float* direction, const float* pedPos, float rotation) {
    const float pi    = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
    const float twoPi = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
    volatile float dx = float(double(direction[0]) - double(pedPos[0]));
    volatile float dy = float(double(direction[1]) - double(pedPos[1]));
    double heading = atan2(-double(dx), double(dy));
    const float upper = rotation + pi;
    if (heading > double(upper)) {
        heading = heading - double(twoPi);
    } else {
        const float lower = rotation - pi;
        if (heading < double(lower)) {
            heading = heading + double(twoPi);
        }
    }
    volatile float result = float(heading - double(rotation));
    return result;
}

// --- CAEVehicleAudioEntity (lote w20, prefijo VehAudio) ---
// fsin sin redondear a float; fchs solo si sin < 0 (no fabs: -0 y NaN conservan su signo, como PedAudioJetPackSinCpp); sin float_control(precise) para que fsin salga en linea.
// Revisar en el .obj que no aparezca fabs ni llamada a la CRT.
__declspec(noinline) float __cdecl VehAudioBikeLeanCpp(float angle, float lean, bool boost, float boostOffset, float gf) {
    double s = sin(double(angle));
    if (s < 0.0) s = -s;
    double v = s * double(lean);
    if (boost) v = v + double(boostOffset);
    volatile float result = float(v * double(gf));
    return result;
}

__declspec(noinline) float __cdecl VehAudioBicycleFrequencyCpp(float angle, float multiplier, float frequencyBase) {
    double s = sin(double(angle));
    if (s < 0.0) s = -s;
    volatile float result = float((s * double(multiplier) + 1.0) * double(frequencyBase));
    return result;
}

// Sin transcendentales: orden estricto del EXE (sin reasociar) y comparaciones ordenadas (NaN conserva el valor). Cada operacion se redondea a 24 bits (x87 a PC24).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl VehAudioRevsRatioCpp(float absSpeed, float gearMax, float gearDown) {
    const double numerator = double(absSpeed) - double(gearDown);
    const double denominator = double(gearMax) - double(gearDown);
    double ratio = numerator / denominator;
    if (ratio > 1.0) {
        ratio = 1.0;                // 0x4FBC03..0x4FBC14
    } else if (0.0 > ratio) {
        ratio = 0.0;                // 0x4FBBE6..0x4FBBFB (0x859EF8 = 0.0 doble)
    }
    volatile float result = float(ratio);
    return result;
}

__declspec(noinline) float __cdecl VehAudioTrainTiltCpp(float forwardX, float forwardY, float forwardZ, float slope) {
    volatile float zero = 0.0f;     // 0x858B50: fmul por cero (inf/NaN se conservan)
    const double z = (double(forwardY) + double(forwardX)) * double(zero) + double(forwardZ);
    double clamped = z;
    if (!(z > -0.3)) {
        clamped = -0.3;             // 0x862D60 (incluye NaN)
    } else if (!(z < 0.3)) {
        clamped = 0.3;              // 0x862D58
    }
    volatile float tilt = float(clamped * double(slope));
    return tilt;
}

__declspec(noinline) float __cdecl VehAudioBicycleRatioCpp(float speed, float maxVelocity) {
    const double quotient = double(speed) / double(maxVelocity);
    double magnitude = quotient;
    if (quotient < 0.0) magnitude = -quotient;      // fchs solo si el cociente < 0
    const double ratio = (magnitude < 1.0) ? magnitude : 1.0; // !(|q| < 1) o NaN -> 1.0 (0x85A310)
    volatile float stored = float(ratio);           // fst dword
    if (!(ratio > 0.0)) stored = 0.0f;              // fcomp 0 + test ah,41h / je: <= 0 se guarda como +0
    return stored;
}
#pragma float_control(pop)

// --- CTaskSimpleJetPack (lote w20, prefijo JetPack). Añadir al FINAL de X87Math.cpp (usa UnsignedAsX87 y ExeCompatiblePowCpp, ya definidos arriba) ---
// Seno, coseno y raíz van en funciones aparte SIN float_control(precise) (con precise serían llamadas a la CRT) y devuelven double = ST0 sin redondear a float.
// Cada uso consume el valor al instante (no se guarda ningún seno/coseno/pow de 64 bits en memoria): los que el EXE reutiliza se recalculan, el resultado es idéntico.
__declspec(noinline) static double __cdecl JetPackSinExtCpp(float angle) { return sin(double(angle)); }
__declspec(noinline) static double __cdecl JetPackCosExtCpp(float angle) { return cos(double(angle)); }
__declspec(noinline) static double __cdecl JetPackNegSinExtCpp(float angle) { return -sin(double(angle)); } // fsin; fchs (conserva el signo de NaN que da fchs)
__declspec(noinline) static double __cdecl JetPackSqrtExtCpp(double value) { return sqrt(value); }

static constexpr float kJetPackMaxThrustAngle = std::bit_cast<float>(0x3FA78D36u); // 0x8D2F48 (75 grados)
static constexpr float kJetPackAngleRate      = std::bit_cast<float>(0x3F666666u); // 0x8D2F54 (0.9)
static constexpr float kJetPackSwingMax       = std::bit_cast<float>(0x3F490FDBu); // 0x8D2F58 (pi/4)
static constexpr float kJetPackSwingDeltaV    = std::bit_cast<float>(0xBE4CCCCDu); // 0x8D2F5C (-0.2)
static constexpr float kJetPackSwingGravity   = std::bit_cast<float>(0x3C23D70Au); // 0x8D2F60 (0.01)
static constexpr float kJetPackSwingDamp      = std::bit_cast<float>(0x3F7AE148u); // 0x8D2F64 (0.98)
static constexpr float kJetPackSpeedDamping   = std::bit_cast<float>(0x3CA3D70Au); // 0x8D2F4C (0.02)

// RenderJetPack (0x67F7DC..0x67F7EC): fild ms; fadd 2^32 si el bit alto está puesto; fdiv [0x8D2F80]; el cociente (24 bits) alimenta fsin/fcos sin pasar por memoria.
__declspec(noinline) float __cdecl JetPackUnsignedDivCpp(unsigned int value, float divisor) {
    volatile float result = float(UnsignedAsX87(value) / double(divisor));
    return result;
}

// ProcessControlInput (0x67E914..0x67E930), cámara mirando a otro lado: fmul -1.0 (0x858C1C) de x e y, fchs del primero (no es una simple negación: con NaN el signo
// del NaN de x vuelve a invertirse y el de y se conserva) y fpatan; fstp float. Los productos se guardan en volatile para que /fp:fast no los pliegue en una negación.
__declspec(noinline) float __cdecl JetPackLookBackHeadingCpp(float x, float y) {
    volatile float  minusOne = -1.0f;
    volatile double scaledX  = double(x) * double(minusOne);
    volatile double scaledY  = double(y) * double(minusOne);
    const double    negScaledX = -scaledX;
    return float(atan2(negScaledX, scaledY));
}

#pragma float_control(precise, on, push)
// ProcessControlInput (0x67E93E..0x67E962): p = pow(0.9, paso) sin redondear (_CIpow 0x822130); 1 - p (24 bits); ((1 - p) · ud) · 75º + p · ángulo; un redondeo a float al guardar.
__declspec(noinline) float __cdecl JetPackLerpThrustAngleCpp(float scaledUpDown, float angle, float step) {
    const double p = ExeCompatiblePowCpp(double(kJetPackAngleRate), double(step));
    const double blended = ((1.0 - p) * double(scaledUpDown)) * double(kJetPackMaxThrustAngle);
    volatile float result = float(blended + p * double(angle));
    return result;
}

// ProcessThrust (0x67F1C2..0x67F221): c = cos(a), s = -sin(a) sin redondear. Los productos con delante (s·f) y con arriba en Y/Z (c·u) se guardan como float; c·arribaX queda
// en ST0 (24 bits) y se suma a s·delanteX. out[i] = float(s·f[i] + c·u[i]).
__declspec(noinline) void __cdecl JetPackThrustDirCpp(float thrustAngle, const float* up, const float* forward, float* out) {
    volatile float sinForwardX = float(JetPackNegSinExtCpp(thrustAngle) * double(forward[0]));
    volatile float sinForwardY = float(JetPackNegSinExtCpp(thrustAngle) * double(forward[1]));
    volatile float sinForwardZ = float(JetPackNegSinExtCpp(thrustAngle) * double(forward[2]));
    volatile float cosUpY      = float(JetPackCosExtCpp(thrustAngle) * double(up[1]));
    volatile float cosUpZ      = float(JetPackCosExtCpp(thrustAngle) * double(up[2]));
    out[0] = float(double(sinForwardX) + JetPackCosExtCpp(thrustAngle) * double(up[0]));
    out[1] = float(double(sinForwardY) + double(cosUpY));
    out[2] = float(double(sinForwardZ) + double(cosUpZ));
}

// ProcessThrust (0x67F43D..0x67F48A): aceleración = velocidad - velocidad previa del peatón; delta = velocidad - velocidad previa de la tarea.
// X e Y de la aceleración y X de delta se quedan en ST0 (24 bits, sin pasar por float); accZ, deltaY y deltaZ se guardan como float; sums = float(delta + aceleración).
__declspec(noinline) void __cdecl JetPackSwingSumsCpp(const float* pedVelocity, const float* prevPedVelocity, const float* taskVelocity, float* sums) {
    const double   accX   = double(pedVelocity[0]) - double(prevPedVelocity[0]);
    const double   accY   = double(pedVelocity[1]) - double(prevPedVelocity[1]);
    volatile float accZ   = float(double(pedVelocity[2]) - double(prevPedVelocity[2]));
    const double   deltaX = double(pedVelocity[0]) - double(taskVelocity[0]);
    volatile float deltaY = float(double(pedVelocity[1]) - double(taskVelocity[1]));
    volatile float deltaZ = float(double(pedVelocity[2]) - double(taskVelocity[2]));
    sums[0] = float(deltaX + accX);
    sums[1] = float(double(deltaY) + accY);
    sums[2] = float(double(deltaZ) + double(accZ));
}

// ProcessThrust (0x67F4A0..0x67F544 delante con axis = fila +0x10 de la matriz; 0x67F544..0x67F5E4 lateral con axis = fila +0x00): balanceo de una pierna.
// dot = (sz·az + sy·ay) + sx·ax (0 si el peatón estaba/está de pie); v1 = velocidad - dot · -0.2 (ST0); v2 = (v1 - ((sin(ángulo) · paso) · 0.01)) · pow(0.98, paso) (ST0, se guarda float en *speed);
// nuevoÁngulo = v2 · paso + ángulo (ST0): se guarda float en *angle y, si el valor sin redondear > pi/4, *angle = pi/4; si no, si el float guardado < -pi/4, *angle = -pi/4 (NaN se conserva).
__declspec(noinline) void __cdecl JetPackSwingAxisCpp(const float* sums, const float* axis, bool grounded, float step, float* speed, float* angle) {
    const double projected = (double(sums[2]) * double(axis[2]) + double(sums[1]) * double(axis[1])) + double(sums[0]) * double(axis[0]);
    const double dot       = grounded ? 0.0 : projected;
    const double v1        = double(*speed) - dot * double(kJetPackSwingDeltaV);
    const double gravity   = (JetPackSinExtCpp(*angle) * double(step)) * double(kJetPackSwingGravity);
    const double v2        = (v1 - gravity) * ExeCompatiblePowCpp(double(kJetPackSwingDamp), double(step));
    *speed = float(v2);
    const double   swung       = v2 * double(step) + double(*angle);
    volatile float swungStored = float(swung);
    *angle = swungStored;
    if (swung > double(kJetPackSwingMax)) {
        *angle = kJetPackSwingMax;
    } else if (swungStored < -kJetPackSwingMax) {
        *angle = -kJetPackSwingMax;
    }
}

// ProcessThrust (0x67F5EA..0x67F618): base de la amortiguación de la velocidad = 1 - |v| · 0.02 con |v| = float(sqrt((x² + y²) + z²)) (fsqrt a 24 bits, fstp float, fld float).
// La base (ST0, 24 bits y dentro del rango float) se devuelve como float; PowScaleVector3Cpp(velocidad, base, paso) hace pow(base, paso) y los tres productos.
__declspec(noinline) float __cdecl JetPackSpeedDampBaseCpp(const float* velocity) {
    const double   x = velocity[0];
    const double   y = velocity[1];
    const double   z = velocity[2];
    volatile float magnitude = float(JetPackSqrtExtCpp((x * x + y * y) + z * z));
    volatile float base      = float(1.0 - double(magnitude) * double(kJetPackSpeedDamping));
    return base;
}
#pragma float_control(pop)

// --- Lote PathFind (w20). Anadir al final de X87Math.cpp (despues de ResidueRegsCpp, FollowIncrementalThunks y PlayerPedResidueFloatCpp) ---

// Igual que PlayerPedResidueFloatCpp pero ademas deja EAX = eaxValue: llama a ResidueRegsCpp con los mismos ECX/EDX (ya estan en los registros de
// entrada, no se mueven) y despues carga value en ST0. Compilada (/O2) es `push dword ptr [esp+8]; call ResidueRegsCpp; fld dword ptr [esp+4]; ret 8`
// y no toca ECX ni EDX. Se comprueba al cargar la DLL (PathFindResidueFloatIsTrivial, anadir a ResidueHelpersAreTrivial).
// El resultado de ResidueRegsCpp se guarda en una variable volatile: sin ese efecto observable el compilador elimina la llamada (la funcion es pura) y EAX
// no se fija. Guardar EAX en memoria no cambia ECX, EDX ni EAX.
static volatile unsigned int s_pathFindResidueSink;
#pragma runtime_checks("", off)
__declspec(noinline) float __fastcall PathFindResidueFloatCpp(unsigned int ecxValue, unsigned int edxValue, float value, unsigned int eaxValue) {
    s_pathFindResidueSink = ResidueRegsCpp(ecxValue, edxValue, eaxValue);
    return value;
}
#pragma runtime_checks("", restore)

// Colocar la declaracion adelantada `static bool PathFindResidueFloatIsTrivial();` junto a la de PlayerPedResidueFloatIsTrivial y anadir
// `&& PathFindResidueFloatIsTrivial()` a ResidueHelpersAreTrivial. Acepta la forma sin marco (/O2) y la del Debug con marco ebp.
static bool PathFindResidueFloatIsTrivial() {
    static const unsigned char head[]       = {0xFF, 0x74, 0x24, 0x08};       // push dword ptr [esp+8]
    static const unsigned char loadPlain[]  = {0xD9, 0x44, 0x24, 0x04};       // fld dword ptr [esp+4]
    static const unsigned char retPlain[]   = {0xC2, 0x08, 0x00};             // ret 8
    static const unsigned char frameHead[]  = {0x55, 0x8B, 0xEC, 0xFF, 0x75, 0x0C}; // push ebp; mov ebp,esp; push dword ptr [ebp+0Ch]
    static const unsigned char loadFrame[]  = {0xD9, 0x45, 0x08};             // fld dword ptr [ebp+8]
    static const unsigned char retFrame[]   = {0x5D, 0xC2, 0x08, 0x00};       // pop ebp; ret 8
    const unsigned char* code     = FollowIncrementalThunks(reinterpret_cast<const unsigned char*>(&PathFindResidueFloatCpp));
    const unsigned char* expected = FollowIncrementalThunks(reinterpret_cast<const unsigned char*>(&ResidueRegsCpp));
    const bool           framed   = memcmp(code, frameHead, sizeof frameHead) == 0;
    const bool           plain    = memcmp(code, head, sizeof head) == 0;
    if (!framed && !plain) {
        return false;
    }
    const unsigned char* call = code + (framed ? sizeof frameHead : sizeof head);
    if (call[0] != 0xE8) {
        return false;
    }
    int offset;
    memcpy(&offset, call + 1, sizeof offset);
    if (FollowIncrementalThunks(call + 5 + offset) != expected) {
        return false;
    }
    // Tras la llamada solo puede haber la carga del float y el guardado de EAX en la variable volatile (mov [s_pathFindResidueSink], eax = A3 imm32),
    // en cualquiera de los dos ordenes: ninguna de las dos cosas cambia ECX, EDX ni EAX.
    const unsigned char* load = framed ? loadFrame : loadPlain;
    const size_t loadSize     = framed ? sizeof loadFrame : sizeof loadPlain;
    const unsigned char* ret  = framed ? retFrame : retPlain;
    const size_t retSize      = framed ? sizeof retFrame : sizeof retPlain;
    const unsigned char* after = call + 5;
    if (memcmp(after, load, loadSize) == 0 && after[loadSize] == 0xA3) {
        return memcmp(after + loadSize + 5, ret, retSize) == 0;
    }
    if (after[0] == 0xA3 && memcmp(after + 5, load, loadSize) == 0) {
        return memcmp(after + 5 + loadSize, ret, retSize) == 0;
    }
    return false;
}

// CVector::Normalise del EXE (0x59C910..0x59C960) sobre v[0..2]:
//   fld z; fld y; fld x; x*x + y*y (faddp) + z*z (faddp); fcom 0.0; fnstsw ax; test ah,41h; jp -> (suma > 0 o NaN) normaliza, si no v[0] = 1.0f
//   fsqrt; fdivr 1.0f; cada componente: fld st; fmul [v[i]]; fstp [v[i]]  (un redondeo a 24 bits por producto; el reciproco queda en ST0).
// El valor de retorno es el AX de ese fnstsw sin TOP (bits 11..13, que dependen de la profundidad de la pila x87 del llamador y vale 0 aqui):
// C3 = 0x4000 (suma == 0), C0 = 0x0100 (suma < 0, imposible), C3|C2|C0 = 0x4500 (NaN); C1 = 0; mas las banderas sticky IE=1, DE=2, ZE=4, OE=8, UE=0x10, PE=0x20
// que el hardware lleva acumuladas (se leen con _statusfp2 despues de los productos y la suma, como X87CompareStatusCpp). PE sale de x*x, y*y o la suma inexactos
// a 24 bits. Limitacion: fcom (ordenada) tambien marca IE con un NaN silencioso; esta funcion compara con == y no lo marca (el dominio de CPathFind son int16 * 0.125, sin NaN).
#pragma float_control(precise, on, push)
static __declspec(noinline) double PathFindSumSquares3(const float* v) {
    return (v[0] * v[0] + v[1] * v[1]) + v[2] * v[2];
}
#pragma float_control(pop)

__declspec(noinline) unsigned int __cdecl PathFindNormaliseStatusCpp(float* v) {
    const double sum = PathFindSumSquares3(v);
    unsigned int status = 0;
    if (!(sum == sum)) {
        status = 0x4500; // desordenado
    } else if (sum == 0.0) {
        status = 0x4000;
    } else if (sum < 0.0) {
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
    if (sum <= 0.0) { // test ah,41h con C3 (igual) o C0 (menor) y paridad impar: solo x = 1.0f
        v[0] = 1.0f;
        return status;
    }
    const double inv = 1.0f / sqrt(sum); // fsqrt; fdivr [0x858624] (1.0f)
    v[0] = float(inv * v[0]);
    v[1] = float(inv * v[1]);
    v[2] = float(inv * v[2]);
    return status;
}

// --- CTaskSimpleSwim (lote w20, prefijo Swim) ---
// 0x688BE5..0x688BF3: fpatan; fst dword [ped+0x55C]; fsub dword [ped+0x558]. El fpatan completo se guarda como float en *out y se resta sin redondear antes.
// Revisar en el .obj: atan2, fst dword [out], fsub dword [sub], fstp; sin fstp qword entre el atan2 y la resta.
__declspec(noinline) float __cdecl SwimAtan2StoreMinusCpp(float y, float x, float sub, float* out) {
    const double angle = atan2(double(y), double(x));
    *out = float(angle);
    volatile float result = float(angle - double(sub));
    return result;
}

// 0x688C72..0x688C7B: fld x; call _CIasin; fsub [rotX]. ExeAsinDoubleCpp reproduce todas las ramas del CRT del EXE (|x| > 1 y NaN incluidos).
__declspec(noinline) float __cdecl SwimAsinMinusCpp(float x, float sub) {
    volatile float result = float(ExeAsinDoubleCpp(double(x)) - double(sub));
    return result;
}

// 0x688FA2..0x688FA7: call 0x53CBE0 (o su fake/DLL, por el puntero); fsub [TheCamera.m_fOrientation]. El ST0 de la llamada se resta sin pasar por memoria.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl SwimRadianAngleMinusCpp(float (__cdecl* angleFn)(float, float, float, float), float x2, float y2, float sub) {
    volatile float result = float(double(angleFn(0.0f, 0.0f, x2, y2)) - double(sub));
    return result;
}

// 0x68A43A..0x68A4D6. A = sx·right.x [fstp], B = sx·right.y [en pila], C = sx·right.z [fstp]; cos·forward[i] (fcos completo, un redondeo) por sy = P[i]; vx = P0 + A, vy = P1 + B,
// vz = ((sin·sy) + (P2 + C)) + 0,01 (0x8708CC). Lo transcendental sale de TrigSimple*MulCpp; la aritmética queda en esta función con precise para no reasociar.
__declspec(noinline) void __cdecl SwimSprintVelocityCpp(float rotationX, const float* right, const float* forward, float sx, float sy, float* out) {
    const float cosFx = TrigSimpleCosMulCpp(rotationX, forward[0]);
    const float cosFy = TrigSimpleCosMulCpp(rotationX, forward[1]);
    const float cosFz = TrigSimpleCosMulCpp(rotationX, forward[2]);
    const float sinSy = TrigSimpleSinMulCpp(rotationX, sy);
    const float a = sx * right[0];
    const float b = sx * right[1];
    const float c = sx * right[2];
    const float p0 = cosFx * sy;
    const float p1 = cosFy * sy;
    const float p2 = cosFz * sy;
    const float vx = p0 + a;
    const float vy = p1 + b;
    const float p2c = p2 + c;
    const float s = sinSy + p2c;
    const float vz = s + 0.01f; // 0x8708CC
    out[0] = vx;
    out[1] = vy;
    out[2] = vz;
}

// 0x68A4D8..0x68A54D: P = pow(0,9f (0x8708D0), paso) en ST0; Q = 1 - P (fsub, 24 bits); t0 = v.x·Q y t1 = v.y·Q (fstp); R = Q·v.z queda en la pila;
// t2 = P·ms.z (fstp); ms.x = P·ms.x + t0; ms.y = P·ms.y + t1; ms.z = t2 + R. Los tres ms se leen antes de escribir ninguno.
__declspec(noinline) void __cdecl SwimResistanceCpp(float step, float* moveSpeed, const float* wanted) {
    const double keep = ExeCompatiblePowCpp(double(0.9f), double(step));
    const double rest = 1.0 - keep;
    const float msX = moveSpeed[0];
    const float msY = moveSpeed[1];
    const float msZ = moveSpeed[2];
    const float t0 = float(double(wanted[0]) * rest);
    const float t1 = float(double(wanted[1]) * rest);
    const double r = rest * double(wanted[2]);
    const float t2 = float(keep * double(msZ));
    const float outX = float(keep * double(msX) + double(t0));
    const float outY = float(keep * double(msY) + double(t1));
    const float outZ = float(double(t2) + r);
    moveSpeed[0] = outX;
    moveSpeed[1] = outY;
    moveSpeed[2] = outZ;
}
#pragma float_control(pop)

// --- CBmx (prefijo Bmx; colocar al final de X87Math.cpp, tras ExeAsinDoubleCpp y TrigAtan2Cpp) ---
// float(sin(angle * k)) / float(cos(angle * k)): el producto (fmul de 24 bits) pasa a fsin/fcos sin guardarse como float (CBmx::PreRender 0x6C0E0A..0x6C0E34).
__declspec(noinline) float __cdecl BmxSinScaledCpp(float angle, float k) {
    volatile float result = float(sin(double(angle) * double(k)));
    return result;
}
__declspec(noinline) float __cdecl BmxCosScaledCpp(float angle, float k) {
    volatile float result = float(cos(double(angle) * double(k)));
    return result;
}

// CBmx::PreRender 0x6C0A27..0x6C0A31: fld angle; fsin; fmul [colModel+8]; fmul 0.8f. Lecturas volatile para que /fp:fast no reasocie k1 * k2.
__declspec(noinline) float __cdecl BmxSinTimes2Cpp(float angle, float k1, float k2) {
    const volatile float* first  = &k1;
    const volatile float* second = &k2;
    double value = sin(double(angle));
    value *= double(*first);
    value *= double(*second);
    volatile float result = float(value);
    return result;
}

// CBmx::PreRender 0x6C1251..0x6C128C: fcos; fld1; fsub st, st(1); fmul minZ; fmul 0.9f; fadd zOff; fstp. 1 - cos se calcula con el coseno completo (no puede pasar por double).
__declspec(noinline) float __cdecl BmxChassisZCpp(float lean, float boundMinZ, float zOff) {
    float nine = std::bit_cast<float>(0x3F666666u); // 0x858C20
    const volatile float* minZ   = &boundMinZ;
    const volatile float* factor = &nine;
    const volatile float* offset = &zOff;
    double value = 1.0 - cos(double(lean));
    value *= double(*minZ);
    value *= double(*factor);
    value += double(*offset);
    volatile float result = float(value);
    return result;
}

// CBmx::ProcessControl 0x6BFA9C..0x6BFAB8: fld cur; fdiv total; fmul 2PI (0x858CBC); fadd start; fsin; fmul blend; fmul mult; fstp.
__declspec(noinline) float __cdecl BmxLeanTermCpp(float curTime, float totalTime, float start, float blend, float mult) {
    float twoPi = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
    const volatile float* cur    = &curTime;
    const volatile float* total  = &totalTime;
    const volatile float* period = &twoPi;
    const volatile float* from   = &start;
    const volatile float* weight = &blend;
    const volatile float* scale  = &mult;
    double angle = double(*cur) / double(*total);
    angle *= double(*period);
    angle += double(*from);
    double value = sin(angle);
    value *= double(*weight);
    value *= double(*scale);
    volatile float result = float(value);
    return result;
}

// CBmx::ProcessControl 0x6BFB0B..0x6BFB2F: igual y "fadd [LeanAngle]; fstp" sin redondear el termino a float.
__declspec(noinline) float __cdecl BmxLeanTermAddCpp(float curTime, float totalTime, float start, float blend, float mult, float add) {
    float twoPi = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
    const volatile float* cur    = &curTime;
    const volatile float* total  = &totalTime;
    const volatile float* period = &twoPi;
    const volatile float* from   = &start;
    const volatile float* weight = &blend;
    const volatile float* scale  = &mult;
    const volatile float* addend = &add;
    double angle = double(*cur) / double(*total);
    angle *= double(*period);
    angle += double(*from);
    double value = sin(angle);
    value *= double(*weight);
    value *= double(*scale);
    value += double(*addend);
    volatile float result = float(value);
    return result;
}

// CBmx::PreRender 0x6C0FCA..0x6C0FDA: call _CIasin (0x821E70); fmul [0x858C1C] (-1.0f); fstp. La constante va por lectura volatile: -x se compilaria como fchs y cambiaria el signo de un NaN.
__declspec(noinline) float __cdecl BmxNegAsinCpp(float x) {
    static const volatile float kMinusOne = -1.0f; // 0x858C1C
    volatile float result = float(ExeAsinDoubleCpp(double(x)) * double(kMinusOne));
    return result;
}

// --- CWaterLevel (WaterLevel.cpp; lote w20, prefijo WaterLevel). Colocar al final de X87Math.cpp (usa Ftol2RegsDblCpp y TrigCompound*, ya definidos arriba). ---
// La x87 corre a 24 bits: cada operacion double() de una cadena equivale al fmul/fadd/fsub del EXE (redondeo a 24 bits por operacion, exponente extendido entre ellas); solo se guarda como
// float donde el EXE hace fstp/fst dword. Constantes: .rdata del EXE (direcciones en los comentarios).
// sin, cos y sqrt van en funciones aparte SIN float_control(precise) (con precise pasarian a llamadas a la CRT); devuelven ST0 sin redondear.
static __declspec(noinline) double WaterLevelSinFull(double angle) { return sin(angle); }
static __declspec(noinline) double WaterLevelCosFull(double angle) { return cos(angle); }
static __declspec(noinline) double WaterLevelSqrtFull(double value) { return sqrt(value); }

// _ftol2 (0x821B40) sobre ST0 y `and eax, 0FFh`: byte bajo del entero de 64 bits truncado (NaN, infinito y |v| >= 2^63 dan 0x8000000000000000 -> 0).
static __forceinline unsigned int WaterLevelFtolByte(double value) {
    return unsigned(static_cast<long long>(value)) & 0xFFu;
}

#pragma float_control(precise, on, push)
// CWaterLevel::AddWaveToResult 0x6E8269: fld fractionY; fadd fractionX; fcomp 1.0; test ah,5; jp -> mitad superior (la suma ya va redondeada a 24 bits).
__declspec(noinline) bool __cdecl WaterLevelWaveLowerCpp(float fractionX, float fractionY) {
    return double(fractionY) + double(fractionX) < 1.0;
}

// 0x6E82D3..0x6E8546. Mitad inferior: pesos fractionX y fractionY; superior: 1 - fractionX y 1 - fractionY. Suma ((tx + ty) + *level) + base y un solo fstp a *level.
// Normal (0x59C730 con a = V2 = (2,0,dx), b = V1 = (0,2,dy) abajo y a = (-2,0,dx), b = (0,-2,dy) arriba): x = b.z*a.y - a.z*b.y, y = a.z*b.x - b.z*a.x, z = a.x*b.y - b.x*a.y.
__declspec(noinline) void __cdecl WaterLevelWaveInterpolateCpp(float base, float alongX, float alongY, float fractionX, float fractionY, bool lower, float* level, float* normal) {
    const double dy = double(alongY) - double(base);
    const double dx = double(alongX) - double(base);
    const double weightX = lower ? double(fractionX) : 1.0 - double(fractionX);
    const double weightY = lower ? double(fractionY) : 1.0 - double(fractionY);
    *level = float(((dx * weightX + dy * weightY) + double(*level)) + double(base));
    if (normal) {
        const double k = lower ? 2.0 : -2.0;
        normal[0] = float(dy * 0.0 - dx * k);
        normal[1] = float(dx * 0.0 - dy * k);
        normal[2] = 4.0f; // 2*2 - 0*0 (o (-2)*(-2) - 0*0)
    }
}

// 0x6E9E3F..0x6E9EF4: fld/fild, fsub a 24 bits, fld st0; fmul; fld st2; fmul; faddp; fsqrt (el valor queda en ST0 a 24 bits y se compara con floats).
__declspec(noinline) float __cdecl WaterLevelNearestDistanceCpp(float x, float y, float minX, float maxX, float minY, float maxY) {
    const double dx = x < minX ? double(minX) - double(x) : (x > maxX ? double(x) - double(maxX) : 0.0);
    const double dy = y < minY ? double(minY) - double(y) : (y > maxY ? double(y) - double(maxY) : 0.0);
    return float(WaterLevelSqrtFull(dy * dy + dx * dx));
}

// 0x6E9FE6..0x6EA0F9 (cuatro distancias al cuadrado guardadas como float, la del vertice 0 en ST0) y las comparaciones 0x6EA0CA..0x6EA1C8 (fcom + test ah,5; jp: avanza si no es estrictamente menor).
__declspec(noinline) int __cdecl WaterLevelNearestFlowIndexCpp(float x, float y, const float* vertexX, const float* vertexY) {
    float squared[4];
    for (int k = 0; k < 4; ++k) {
        const double dy = double(y) - double(vertexY[k]);
        const double dx = double(x) - double(vertexX[k]);
        squared[k] = float(dy * dy + dx * dx);
    }
    if (squared[0] < squared[1] && squared[0] < squared[2] && squared[0] < squared[3]) {
        return 0;
    }
    if (squared[1] < squared[2] && squared[1] < squared[3]) {
        return 1;
    }
    return squared[2] < squared[3] ? 2 : 3;
}
#pragma float_control(pop)

#pragma float_control(precise, on, push)
// Constantes de las ondas (.rdata): 0x872020, 0x87201C, 0x85A778 (escala a indice de la tabla), 0x872018, 0x872014, 0x872010, 0x87200C, 0x872008.
static constexpr double kWaterWavePhase1Time  = double(std::bit_cast<float>(0x3AA4B5BEu)); // 0x872020
static constexpr double kWaterWavePhase1Space = double(std::bit_cast<float>(0x3DC90FDBu)); // 0x87201C
static constexpr double kWaterWaveToIndex     = double(std::bit_cast<float>(0x4222F983u)); // 0x85A778
static constexpr double kWaterWavePhase2Time  = double(std::bit_cast<float>(0x3AEB4CC7u)); // 0x872018
static constexpr double kWaterWavePhase2Y     = double(std::bit_cast<float>(0x3DF775FAu)); // 0x872014
static constexpr double kWaterWavePhase2X     = double(std::bit_cast<float>(0x3E7775FAu)); // 0x872010
static constexpr double kWaterWavePhase3Time  = double(std::bit_cast<float>(0x3B09421Fu)); // 0x87200C
static constexpr double kWaterWavePhase3Y     = double(std::bit_cast<float>(0x3EA0D97Cu)); // 0x872008

// 0x6E6F4F..0x6E713F. Pila x87: A2 = (multX*multY)*wavyness; x e y quedan exactos (fild) hasta la segunda y tercera onda; cada indice es _ftol2(ST0) & 0xFF; la segunda llamada de cada par suma 64.0 (0x859A44).
__declspec(noinline) void __cdecl WaterLevelWavesFullCpp(int absX, int absY, float multX, float multY, float wavyness, unsigned int rem1, unsigned int rem2, unsigned int rem3,
                                                         float bigAmplitude, float smallAmplitude, const float* sinTable, float* out, float* normal) {
    const double amp = double(multX) * double(multY) * double(wavyness);
    const double x = double(absX);
    const double y = double(absY);

    const double phase1 = (double(int(rem1)) * kWaterWavePhase1Time + (y + x) * kWaterWavePhase1Space) * kWaterWaveToIndex;
    *out = float(((double(sinTable[WaterLevelFtolByte(phase1)]) * 2.0) * amp * double(bigAmplitude)) + double(*out));
    const double cosine1 = double(sinTable[WaterLevelFtolByte(phase1 + 64.0)]) * 2.0 * amp * double(bigAmplitude) * kWaterWavePhase1Space;
    const float negative = float(-cosine1);
    normal[0] = negative;
    normal[1] = negative;
    normal[2] = 1.0f;

    const double phase2 = ((double(int(rem2)) * kWaterWavePhase2Time + y * kWaterWavePhase2Y) + x * kWaterWavePhase2X) * kWaterWaveToIndex;
    *out = float((double(sinTable[WaterLevelFtolByte(phase2)]) * 1.0 * amp * double(smallAmplitude)) + double(*out));
    const double cosine2 = double(sinTable[WaterLevelFtolByte(phase2 + 64.0)]) * 1.0 * amp * double(smallAmplitude) * kWaterWavePhase2X;
    normal[0] = float(cosine2 + double(normal[0]));
    normal[1] = float(cosine2 + double(normal[1]));

    const double phase3 = (double(int(rem3)) * kWaterWavePhase3Time + y * kWaterWavePhase3Y) * kWaterWaveToIndex;
    *out = float((double(sinTable[WaterLevelFtolByte(phase3)]) * 0.5 * amp * double(smallAmplitude)) + double(*out));
    const double cosine3 = double(sinTable[WaterLevelFtolByte(phase3 + 64.0)]) * 0.5 * amp * double(smallAmplitude) * kWaterWavePhase3Y;
    normal[0] = float(cosine3 + double(normal[0]));
}

// 0x6E7146..0x6E7209. c = (nz + ny + nx) * 0.577 (0x872004), 0.0 si c < 0; color = c * colorScale + colorBias. g = nz*0.577 + ny*0.577 + 0.577*nx (se guarda en *glare), h = g*8.0 - 5.0 (0x859000, 0x858C80),
// recortado a [0.0, 0.99 (0x862CD0)] con comparaciones ordenadas (NaN pasa tal cual) y glare = h * sunGlare.
__declspec(noinline) void __cdecl WaterLevelWaveColorGlareCpp(const float* normal, float colorScale, float colorBias, float sunGlare, float* colorMult, float* glare) {
    const double k = double(std::bit_cast<float>(0x3F13B646u)); // 0x872004
    double c = ((double(normal[2]) + double(normal[1])) + double(normal[0])) * k;
    if (0.0 > c) {
        c = 0.0;
    }
    *colorMult = float(c * double(colorScale) + double(colorBias));
    const double g = (double(normal[2]) * k + double(normal[1]) * k) + k * double(normal[0]);
    *glare = float(g);
    double h = g * double(std::bit_cast<float>(0x41000000u)) - double(std::bit_cast<float>(0x40A00000u)); // 8.0, 5.0
    if (0.0 > h) {
        h = 0.0;
    } else if (double(std::bit_cast<float>(0x3F7D70A4u)) < h) { // 0.99
        h = double(std::bit_cast<float>(0x3F7D70A4u));
    }
    *glare = float(h * double(sunGlare));
}

// 0x6E7210..0x6E739F. Como WavesFull pero sin normal: y se guarda como float (fstp dword [esp+8]) y se usa asi en las tres ondas; x sigue exacto; el valor de la segunda onda se suma en la tercera sin pasar por float.
__declspec(noinline) Ftol2Regs __cdecl WaterLevelWavesNativeCpp(int absX, int absY, float multX, float multY, float wavyness, unsigned int rem1, unsigned int rem2, unsigned int rem3,
                                                                float bigAmplitude, float smallAmplitude, const float* sinTable, float* height) {
    const double amp = double(multX) * double(multY) * double(wavyness);
    const double x = double(absX);
    const float ys = float(absY);

    const double phase1 = (double(int(rem1)) * kWaterWavePhase1Time + (double(ys) + x) * kWaterWavePhase1Space) * kWaterWaveToIndex;
    const float first = float(((double(sinTable[WaterLevelFtolByte(phase1)]) * 2.0) * amp * double(bigAmplitude)) + double(*height));
    *height = first;

    const double phase2 = ((double(int(rem2)) * kWaterWavePhase2Time + double(ys) * kWaterWavePhase2Y) + x * kWaterWavePhase2X) * kWaterWaveToIndex;
    const double second = (double(sinTable[WaterLevelFtolByte(phase2)]) * 1.0 * amp * double(smallAmplitude)) + double(first);
    *height = float(second);

    const double phase3 = (double(int(rem3)) * kWaterWavePhase3Time + double(ys) * kWaterWavePhase3Y) * kWaterWaveToIndex;
    const Ftol2Regs regs = Ftol2RegsDblCpp(phase3, 0xBB8u); // ECX de entrada = 0xBB8 (mov ecx,0BB8h antes de div)
    const unsigned int index = regs.eax & 0xFFu;
    *height = float((double(sinTable[index]) * 0.5 * amp * double(smallAmplitude)) + second);
    return { index, regs.ecx, regs.edx };
}

// 0x6E5BB0. Las comparaciones de limites son las del EXE (fcomp + test ah,1: fallan con menor o NaN); u y v salen de fidiv (division por el entero exacto); la mitad se elige con u + v <= 1.0 ordenado.
// outBigWaves nulo: sale con true sin probar la profundidad (0x6E5D5B/0x6E5E7A). Profundidad: (nivel - 6.0 > z y bLimitedDepth) o (nivel + 20.0 < z) devuelven false; el nivel se relee de *outLevel.
__declspec(noinline) bool __cdecl WaterLevelQuadCpp(const WaterLevelVertexCpp* const* vertex, bool limitedDepth, float x, float y, float z, float* outLevel, float* outBigWaves, float* outSmallWaves) {
    const WaterLevelVertexCpp& v0 = *vertex[0];
    const WaterLevelVertexCpp& v1 = *vertex[1];
    const WaterLevelVertexCpp& v2 = *vertex[2];
    const WaterLevelVertexCpp& v3 = *vertex[3];
    const float x0 = float(int(v0.x));
    if (!(x >= x0)) {
        return false;
    }
    if (!(double(int(v1.x)) >= double(x))) {
        return false;
    }
    const float y0 = float(int(v0.y));
    if (!(y >= y0)) {
        return false;
    }
    if (!(double(int(v2.y)) >= double(y))) {
        return false;
    }
    const float u = float((double(x) - double(x0)) / double(int(v1.x) - int(v0.x)));
    const double v = (double(y) - double(y0)) / double(int(v2.y) - int(v0.y));
    if (double(u) + v <= 1.0) {
        *outLevel = float(((double(v2.z) - double(v0.z)) * v + (double(v1.z) - double(v0.z)) * double(u)) + double(v0.z));
        if (!outBigWaves) {
            return true;
        }
        *outBigWaves = float(((double(v2.bigWaves) - double(v0.bigWaves)) * v + (double(v1.bigWaves) - double(v0.bigWaves)) * double(u)) + double(v0.bigWaves));
        *outSmallWaves = float(((double(v2.smallWaves) - double(v0.smallWaves)) * v + (double(v1.smallWaves) - double(v0.smallWaves)) * double(u)) + double(v0.smallWaves));
    } else {
        const double oneMinusU = 1.0 - double(u);
        const double oneMinusV = 1.0 - v;
        *outLevel = float(((double(v2.z) - double(v3.z)) * oneMinusU + (double(v1.z) - double(v3.z)) * oneMinusV) + double(v3.z));
        if (!outBigWaves) {
            return true;
        }
        *outBigWaves = float(((double(v1.bigWaves) - double(v3.bigWaves)) * oneMinusV + (double(v2.bigWaves) - double(v3.bigWaves)) * oneMinusU) + double(v3.bigWaves));
        *outSmallWaves = float(((double(v1.smallWaves) - double(v3.smallWaves)) * oneMinusV + (double(v2.smallWaves) - double(v3.smallWaves)) * oneMinusU) + double(v3.smallWaves));
    }
    const double level = double(*outLevel);
    if (level - 6.0 > double(z) && limitedDepth) { // 0x858B44
        return false;
    }
    return !(level + 20.0 < double(z)); // 0x858BA4
}

// 0x6E5E90. y entre min(y0, y2) y max(y0, y2) (el menor queda en ST exacto, el mayor como float); u y v se guardan como float; si v0.x == v2.x basta v + u <= 1.0, si no hace falta u >= v.
// Con outBigWaves nulo el triangulo si prueba la profundidad (0x6E6168).
__declspec(noinline) bool __cdecl WaterLevelTriangleCpp(const WaterLevelVertexCpp* const* vertex, bool limitedDepth, float x, float y, float z, float* outLevel, float* outBigWaves, float* outSmallWaves) {
    const WaterLevelVertexCpp& v0 = *vertex[0];
    const WaterLevelVertexCpp& v1 = *vertex[1];
    const WaterLevelVertexCpp& v2 = *vertex[2];
    const float x0 = float(int(v0.x));
    if (!(x >= x0)) {
        return false;
    }
    if (!(double(int(v1.x)) >= double(x))) {
        return false;
    }
    const int y0Int = v0.y;
    const int y2Int = v2.y;
    const float y0 = float(y0Int);
    double yMin;
    float yMax;
    if (y0Int < y2Int) {
        yMin = double(y0Int);
        yMax = float(y2Int);
    } else {
        yMin = double(y2Int);
        yMax = float(y0Int);
    }
    if (!(double(y) >= yMin)) {
        return false;
    }
    if (!(y <= yMax)) {
        return false;
    }
    const float u = float((double(x) - double(x0)) / double(int(v1.x) - int(v0.x)));
    const float v = float((double(y) - double(y0)) / double(y2Int - y0Int));
    if (v0.x == v2.x) {
        if (!(double(v) + double(u) <= 1.0)) {
            return false;
        }
        *outLevel = float(((double(v2.z) - double(v0.z)) * double(v) + (double(v1.z) - double(v0.z)) * double(u)) + double(v0.z));
        if (outBigWaves) {
            *outBigWaves = float(((double(v1.bigWaves) - double(v0.bigWaves)) * double(u) + (double(v2.bigWaves) - double(v0.bigWaves)) * double(v)) + double(v0.bigWaves));
            *outSmallWaves = float(((double(v2.smallWaves) - double(v0.smallWaves)) * double(v) + (double(v1.smallWaves) - double(v0.smallWaves)) * double(u)) + double(v0.smallWaves));
        }
    } else {
        if (!(double(u) >= double(v))) {
            return false;
        }
        const double oneMinusU = 1.0 - double(u);
        *outLevel = float(((double(v2.z) - double(v1.z)) * double(v) + (double(v0.z) - double(v1.z)) * oneMinusU) + double(v1.z));
        if (outBigWaves) {
            *outBigWaves = float(((double(v0.bigWaves) - double(v1.bigWaves)) * oneMinusU + (double(v2.bigWaves) - double(v1.bigWaves)) * double(v)) + double(v1.bigWaves));
            *outSmallWaves = float(((double(v0.smallWaves) - double(v1.smallWaves)) * oneMinusU + (double(v2.smallWaves) - double(v1.smallWaves)) * double(v)) + double(v1.smallWaves));
        }
    }
    const double level = double(*outLevel);
    if (level - 6.0 > double(z) && limitedDepth) { // 0x858B44
        return false;
    }
    return !(level + 20.0 < double(z)); // 0x858BA4
}

// CWaterLevel::RenderWater 0x6EF8E2..0x6EF9EB. S1 = (timeStep*flowX)*scale y S2 igual con flowY quedan en ST; 0.08 = 0x859018, 0.04 = 0x858CEC, 1.0 = 0x858624.
// Orden: shifts[0] y [1] += 0.08*S; envolver (> 1.0 -> - 1.0) ; shifts[2] y [3] += S*0.04; envolver (comparacion ordenada: NaN no envuelve).
__declspec(noinline) void __cdecl WaterLevelShiftsAdvanceCpp(float timeStep, float flowX, float flowY, float scale, float* shifts) {
    const double layerScale = double(std::bit_cast<float>(0x3DA3D70Au));
    const double flowScale = double(std::bit_cast<float>(0x3D23D70Au));
    const double s1 = double(timeStep) * double(flowX) * double(scale);
    const double s2 = double(timeStep) * double(flowY) * double(scale);
    shifts[0] = float(layerScale * s1 + double(shifts[0]));
    shifts[1] = float(layerScale * s2 + double(shifts[1]));
    if (shifts[0] > 1.0f) {
        shifts[0] = float(double(shifts[0]) - 1.0);
    }
    if (shifts[1] > 1.0f) {
        shifts[1] = float(double(shifts[1]) - 1.0);
    }
    shifts[2] = float(s1 * flowScale + double(shifts[2]));
    shifts[3] = float(s2 * flowScale + double(shifts[3]));
    if (shifts[2] > 1.0f) {
        shifts[2] = float(double(shifts[2]) - 1.0);
    }
    if (shifts[3] > 1.0f) {
        shifts[3] = float(double(shifts[3]) - 1.0);
    }
}

// 0x6EFA1B..0x6EFA4B: fld angle; fsin; fmul first; fmul second; fadd add; fstp (el seno entra sin redondear).
__declspec(noinline) float __cdecl WaterLevelSinMulMulAddCpp(float angle, float first, float second, float add) {
    return float(WaterLevelSinFull(double(angle)) * double(first) * double(second) + double(add));
}
// Igual con fcos (0x6EFA37..0x6EFA4B, sobre la misma copia de ST0 que el fsin).
__declspec(noinline) float __cdecl WaterLevelCosMulMulAddCpp(float angle, float first, float second, float add) {
    return float(WaterLevelCosFull(double(angle)) * double(first) * double(second) + double(add));
}
// fild random; fmul scale (0x858C7C); fmul multiplier; fstp dword.
__declspec(noinline) float __cdecl WaterLevelNoiseFirstCpp(int random, float scale, float multiplier) {
    return float(double(random) * double(scale) * double(multiplier));
}
// fild random; fmul scale; ... fmul multiplier; (fld cosineSpill; fmul factor); faddp; fstp dword.
__declspec(noinline) float __cdecl WaterLevelNoiseSecondCpp(int random, float scale, float multiplier, float cosineSpill, float factor) {
    return float(double(random) * double(scale) * double(multiplier) + double(cosineSpill) * double(factor));
}
#pragma float_control(pop)

// --- CPhysical (Physical.cpp; w20, prefijo Physical). Añadir al final de X87Math.cpp (no depende de otros ayudantes nuevos). ---

// 0x544A30 (y 0x542560, 0x54DB10, 0x54D920): fld coord; fmul [0x858B38]=0.02f; fadd [0x858B34]=60.0f; fstp qword; call floor (0x8219F0); call _ftol2 (0x821B40); el llamador usa EAX.
// Cada operacion se redondea a 24 bits (x87 del proceso); el double guardado es exacto. floor sin <cmath>: truncado de 64 bits corregido; |valor| >= 2^63, NaN e infinito dan fistp
// "indefinido" (0x8000000000000000 -> EAX = 0), igual que _ftol2 del EXE; por eso se descartan mirando el exponente (sin comparaciones con NaN que /fp:fast pudiera reordenar).
__declspec(noinline) int __cdecl PhysicalSectorCpp(float coordinate) {
    const double sector = double(coordinate) * double(0.02f) + 60.0;
    unsigned long long bits;
    memcpy(&bits, &sector, 8);
    if (((bits >> 52) & 0x7FF) >= 0x43E)
        return 0;
    long long truncated = static_cast<long long>(sector);
    if (double(truncated) > sector)
        --truncated;
    return static_cast<int>(truncated);
}

// CMatrix::Rotate (0x59B460). Etapa A (precise, fsin/fcos por MatrixSinDbl/MatrixCosDbl en linea): cos x, cos y, sin y, cos z, sin z quedan sin redondear en la pila x87; sin x y sin z se guardan como float (fstp/fst dword),
// cos z · sin x tambien; sin z · sin x queda en ST0 sin redondear a float. Cada producto/suma se redondea a 24 bits y los diez resultados se guardan como float (los nueve valores y cos x · cos y,
// que no se guarda, pasan a la etapa B por memoria: son floats o dobles con mantisa de 24 bits, es decir, exactos).
// REVISAR EN EL .obj: ningun fstp qword de cos/sin entre su fsin/fcos y su ultimo producto.
struct PhysicalRotTerms {
    float f1, f4, f5, f6, f7, f8, f9, f10;
    double cc;
};
#pragma float_control(precise, on, push)
// Con /fp:fast el compilador elimina el redondeo a float de sin x, sin z y cos z · sin x y reutiliza el valor sin redondear en los productos siguientes; el EXE los guarda en la pila
// ([esp+58h], [esp+5Ch], [esp+60h]) y los vuelve a leer como float (fmul dword). Por eso van en volatile float (fstp/fld dword) y la función es precise (sin reasociar). fsin/fcos salen de
// MatrixSinDbl/MatrixCosDbl (en línea, resultado en ST0), igual que MatrixSetRotateCpp.
static __declspec(noinline) void __cdecl PhysicalRotTermsCpp(PhysicalRotTerms* t, float x, float y, float z) {
    const double cx = MatrixCosDbl(double(x));
    volatile float f1v = float(MatrixSinDbl(double(x)));     // fstp dword [esp+58h]
    const double cy = MatrixCosDbl(double(y));
    const double sy = MatrixSinDbl(double(y));
    const double cz = MatrixCosDbl(double(z));
    const double sz = MatrixSinDbl(double(z));
    volatile float f2v = float(sz);                          // fst dword [esp+5Ch]
    const double p1 = sz * double(f1v);                      // fmul dword [esp+58h]: queda en ST0 sin guardar
    volatile float f3v = float(cz * double(f1v));            // fstp dword [esp+60h]
    const double f1 = f1v, f2 = f2v, f3 = f3v;
    t->f1  = float(f1);
    t->f4  = float(cz * cy - p1 * sy);                       // [esp]
    t->f5  = float(f3 * sy + f2 * cy);                       // [esp+4]
    t->f6  = float(-(sy * cx));                              // fchs; [esp+8]
    t->f7  = float(-(f2 * cx));                              // fchs; [esp+0Ch]
    t->f8  = float(cz * cx);                                 // [esp+10h]
    t->f9  = float(cz * sy + p1 * cy);                       // [esp+18h]
    t->f10 = float(f2 * sy - f3 * cy);                       // [esp+1Ch]
    t->cc  = cx * cy;                                        // fmulp: no se guarda como float
}
#pragma float_control(pop)
// Etapa B (precise: orden de sumas del EXE, (a·m + b·m) + c·m con cada operacion redondeada a 24 bits y un fstp dword por resultado). Se leen las doce palabras antes de escribir.
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl PhysicalRotateXYZCpp(float* m, float x, float y, float z) {
    PhysicalRotTerms t;
    PhysicalRotTermsCpp(&t, x, y, z);
    const double f1 = t.f1, f4 = t.f4, f5 = t.f5, f6 = t.f6, f7 = t.f7, f8 = t.f8, f9 = t.f9, f10 = t.f10, cc = t.cc;
    const double m0 = m[0], m1 = m[1], m2 = m[2], m4 = m[4], m5 = m[5], m6 = m[6], m8 = m[8], m9 = m[9], m10 = m[10], m12 = m[12], m13 = m[13], m14 = m[14];
    const float o2  = float((cc * m2 + f1 * m1) + f6 * m0);
    const float o4  = float((f4 * m4 + f9 * m6) + f7 * m5);
    const float o5  = float((f5 * m4 + f10 * m6) + f8 * m5);
    const float o6  = float((f6 * m4 + cc * m6) + f1 * m5);
    const float o8  = float((f4 * m8 + f9 * m10) + f7 * m9);
    const float o9  = float((f5 * m8 + f10 * m10) + f8 * m9);
    const float o10 = float((f6 * m8 + cc * m10) + f1 * m9);
    const float o12 = float((f4 * m12 + f9 * m14) + f7 * m13);
    const float o13 = float((f5 * m12 + f10 * m14) + f8 * m13);
    const float o14 = float((f6 * m12 + cc * m14) + f1 * m13);
    const float o0  = float((f9 * m2 + f7 * m1) + f4 * m0);
    const float o1  = float((f10 * m2 + f8 * m1) + f5 * m0);
    m[2] = o2;   m[4] = o4;   m[5] = o5;   m[6] = o6;
    m[8] = o8;   m[9] = o9;   m[10] = o10;
    m[12] = o12; m[13] = o13; m[14] = o14;
    m[0] = o0;   m[1] = o1;
}
#pragma float_control(pop)

// Rumbo de CPlaceable::GetHeading (0x441DB0) dos veces, A - B con un solo fsubp (24 bits), devuelto en ST0 (double exacto: la mantisa ya es de 24 bits). Sin precise para que fpatan salga en linea
// y A y B se queden en la pila x87 (sin fstp qword entre los dos fpatan): REVISAR EN EL .obj.
static __declspec(noinline) double __cdecl PhysicalHeadingDiffDbl(const float* forwardA, float headingA, const float* forwardB, float headingB) {
    const double a = forwardA ? atan2(-double(forwardA[0]), double(forwardA[1])) : double(headingA);
    const double b = forwardB ? atan2(-double(forwardB[0]), double(forwardB[1])) : double(headingB);
    return a - b;
}
// 0x547990..0x547A76: fsubp; fcom [0x858CB8]=pi (no menor ni igual -> fsub [0x858CBC]=2pi y salta); si no, fcom [0x858CC0]=-pi (solo menor -> fadd 2pi); recorte a +-0.5 (0x8CD7E0; NaN pasa);
// fmul [0x8CD7DC]=1e-5f; fmul masa; fadd turnZ; fstp float. Todo sin guardar a float entre operaciones.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PhysicalAttachTurnZCpp(const float* attachedForward, float attachedHeading, const float* selfForward, float selfHeading, float turnZ, float mass) {
    const double pi    = double(std::bit_cast<float>(0x40490FDBu)); // 0x858CB8
    const double twoPi = double(std::bit_cast<float>(0x40C90FDBu)); // 0x858CBC
    const double scale = double(std::bit_cast<float>(0x3727C5ACu)); // 0x8CD7DC
    double angle = PhysicalHeadingDiffDbl(attachedForward, attachedHeading, selfForward, selfHeading);
    if (angle > pi) {
        angle = angle - twoPi;
    } else if (angle < -pi) {                                        // 0x858CC0
        angle = angle + twoPi;
    }
    if (angle > 0.5) {                                               // 0x8CD7E0
        angle = 0.5;
    } else if (angle < -0.5) {
        angle = -0.5;
    }
    return float((angle * scale) * double(mass) + double(turnZ));
}
#pragma float_control(pop)

// CPhysical::ApplySpeed (0x548270): GetHeading (0x441DB0) deja el fpatan en ST0 sin redondear. fld pi; fadd st,st(1) [redondeado a 24 bits]; fcomp door: solo "menor" suma 2pi (con NaN o igual no);
// si no, fld pi; fsubr; fcomp door: "mayor" resta 2pi. Despues fsub door y fstp float (el EXE sigue en ST0, pero |valor| < 0.001 se anula y por encima de eso un float de 24 bits es exacto).
// No reasociar h + pi < door: REVISAR EN EL .obj (fadd + fcomp, sin fstp qword del rumbo).
__declspec(noinline) float __cdecl PhysicalDoorHeadingCpp(const float* forward, float compactHeading, float doorStart) {
    const double pi    = double(std::bit_cast<float>(0x40490FDBu)); // 0x858CB8
    const double twoPi = double(std::bit_cast<float>(0x40C90FDBu)); // 0x858CBC
    const double door  = double(doorStart);
    double heading = forward ? atan2(-double(forward[0]), double(forward[1])) : double(compactHeading);
    const double piPlusHeading = pi + heading;
    if (piPlusHeading < door) {
        heading = heading + twoPi;
    } else {
        const double headingMinusPi = heading - pi;
        if (headingMinusPi > door) {
            heading = heading - twoPi;
        }
    }
    volatile float result = float(heading - door);
    return result;
}

// --- Lote de Radar.cpp (15/10/2026) ---
// CRadar::CalculateCachedSinCos: fld x; fchs; fld y; fpatan; fst orientation; fld st0; fsin; fstp sin; fcos; fstp cos. El fpatan queda sin redondear en ST0 para fsin/fcos.
__declspec(noinline) void __cdecl RadarSinCosFromAtan2Cpp(float x, float y, float* orientation, float* sinOut, float* cosOut) {
    const double angle = atan2(-double(x), double(y));
    *orientation       = float(angle);
    *sinOut            = float(sin(angle));
    *cosOut            = float(cos(angle));
}

// CRadar::CalculateCachedSinCos sin matriz: fld heading; fst orientation; fsin/fcos del mismo valor. El rumbo ya es un float, asi que `angle` es exacto.
__declspec(noinline) void __cdecl RadarSinCosFromAngleCpp(float angle, float* orientation, float* sinOut, float* cosOut) {
    const double a = double(angle);
    *orientation   = float(a);
    *sinOut        = float(sin(a));
    *cosOut        = float(cos(a));
}

// CRadar::DrawRotatingRadarSprite: W/H quedan en la pila x87 como fild (exactos hasta 2^31) y, si el entero es negativo, fadd 2^32 (4294967296.0f, un redondeo a 24 bits).
// theta = (i · halfPi) + (angle - subtract), cada operacion redondeada a 24 bits por la x87 del proceso; vx/vy en el orden fmul/fadd del EXE.
__declspec(noinline) void __cdecl RadarRotatedSpriteVertsCpp(float angle, float x, float y, unsigned int width, unsigned int height, float subtract, float halfPi, float zero, float* verts) {
    const double start = double(angle) - double(subtract);
    double w = double(int(width));
    if (int(width) < 0) {
        w += 4294967296.0f;
    }
    double h = double(int(height));
    if (int(height) < 0) {
        h += 4294967296.0f;
    }
    for (int i = 0; i < 4; i++) {
        const double theta = double(i) * double(halfPi) + start;
        const double s     = sin(theta);
        const double c     = cos(theta);
        verts[2 * i]       = float(((double(zero) * c + s) * w) + double(x));
        verts[2 * i + 1]   = float(((c - double(zero) * s) * h) + double(y));
    }
}

// CRadar::DrawYouAreHereSprite: fld circleAngle; fld st0; fsin; fmul radius; fsubr y; fstp offsetY; fcos; fmul radius; fadd x; fstp offsetX.
__declspec(noinline) void __cdecl RadarYouAreHereOffsetCpp(float circleAngle, float radius, float x, float y, float* outX, float* outY) {
    const double s = sin(double(circleAngle));
    const double c = cos(double(circleAngle));
    *outY          = float(double(y) - s * double(radius));
    *outX          = float(c * double(radius) + double(x));
}

// CRadar::DrawEntityBlip (destello de la pista): fld heading; fmul degToRad; fild steps; fld st1; fcos; fmul st1; fadd x; fstp x; fxch; fsin; fmul st1; fsubr y; fst y.
// El producto por degToRad se redondea a 24 bits (la x87 del proceso) y fcos/fsin leen ese mismo valor; el coseno y el seno no se guardan como float antes de multiplicar por steps.
__declspec(noinline) void __cdecl RadarAirstripRotateCpp(float heading, float degToRad, int steps, float* x, float* y) {
    const double a = double(heading) * double(degToRad);
    const double n = double(steps);
    const float  newX = float(cos(a) * n + double(*x));
    *x                = newX;
    *y                = float(double(*y) - sin(a) * n);
}

// CRadar::DrawRadarMask: fild j; fmul step; fld st0; fcos; fmul cornerX; fstp; fsin; fmul cornerY; fstp (el coseno y el seno NO se guardan como float antes de multiplicar).
__declspec(noinline) void __cdecl RadarArcPointCpp(int j, float step, float cornerX, float cornerY, float* outX, float* outY) {
    const double arc = double(j) * double(step);
    *outX            = float(cos(arc) * double(cornerX));
    *outY            = float(sin(arc) * double(cornerY));
}

// CRadar::DrawRadarGangOverlay: fsin; fadd one; fmul half; fimul alpha; el _ftol2 posterior trunca ST0 (valor en [0, 255]: el redondeo a float no cambia nada).
__declspec(noinline) float __cdecl RadarGangPulseCpp(float angle, int alpha, float one, float half) {
    return float(((sin(double(angle)) + double(one)) * double(half)) * double(alpha));
}

// CRadar::DrawRadarMap: fld forwardZ; fchs; fld upZ; fpatan; fmul degrees; fadd st0, st0; fsubr playerY; fstp bottomY.
__declspec(noinline) float __cdecl RadarPlaneBottomYCpp(float forwardZ, float upZ, float degrees, float playerY) {
    double angle = atan2(-double(forwardZ), double(upZ)) * double(degrees);
    angle        = angle + angle;
    return float(double(playerY) - angle);
}

// CRadar::ClipRadarPoly, rama sin vertices de salida: aritmetica pura (sin fsin/fcos), el orden del EXE no puede reasociarse.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl RadarClipHullInsideCpp(const float* poly) {
    const double slope1 = (double(poly[1]) - double(poly[3])) / (double(poly[0]) - double(poly[2]));   // 0x585333..0x58533E, queda en ST0
    volatile float slope3Stored = float((double(poly[1]) - double(poly[7])) / (double(poly[0]) - double(poly[6]))); // 0x585340..0x58534D: fstp dword [esp+2Ch]
    const double first = (slope1 * double(poly[6]) - double(poly[7])) * (slope1 * double(poly[0]) - double(poly[1])); // 0x585351..0x585360
    if (!(first < 0.0)) {                                                                                // fcomp 0.0; test ah,5; jp -> sin recorte
        return false;
    }
    const double slope3 = double(slope3Stored);
    const double second = (slope3 * double(poly[2]) - double(poly[3])) * (slope3 * double(poly[0]) - double(poly[1])); // 0x58536F..0x585382
    return second < 0.0;
}
#pragma float_control(pop)

// --- CPad (lote w20, prefijo Pad). Anadir al final de X87Math.cpp ---
// Las cadenas con varias operaciones van con precise para que /fp:fast no las reasocie; el seno/coseno va aparte (sin precise) para que siga siendo fsin/fcos en linea.
#pragma float_control(precise, on, push)
// CPad::ProcessPad (0x746B32..0x746B4A): fild del POV con signo; si es negativo fadd 2^32 (float, 0x858C54); * 0.01f (0x858C58); * 0.017453292f (0x8595EC).
// Devuelve el angulo como double para no redondearlo a float (con PC53 el EXE conserva 53 bits en ST0 hasta fsin/fcos; con PC24 ya esta redondeado a 24 bits).
__declspec(noinline) double __cdecl PadPovAngleCpp(unsigned int pov) {
    const int signedPov = static_cast<int>(pov);
    double    angle     = static_cast<double>(signedPov);
    if (signedPov < 0) {
        angle += 4294967296.0;
    }
    angle = angle * static_cast<double>(std::bit_cast<float>(0x3C23D70Au)); // 0.01f (0x858C58)
    angle = angle * static_cast<double>(std::bit_cast<float>(0x3C8EFA35u)); // 0.017453292f (0x8595EC)
    return angle;
}

// CPad::StartShake_Distance (0x53F9E5..0x53FA29): ((dz*dz + dy*dy) + dx*dx), fsqrt, fcomp limite y "test ah,5 / jp": true solo si raiz < limite (ordenada: con NaN da false).
// camera y point apuntan a x, y, z consecutivos.
__declspec(noinline) bool __cdecl PadShakeWithinDistanceCpp(const float* camera, const float* point, float limit) {
    const double dz  = static_cast<double>(camera[2]) - static_cast<double>(point[2]);
    const double dy  = static_cast<double>(camera[1]) - static_cast<double>(point[1]);
    const double dx  = static_cast<double>(camera[0]) - static_cast<double>(point[0]);
    const double sum = (dz * dz + dy * dy) + dx * dx;
    return sqrt(sum) < static_cast<double>(limit);
}

// CPad::StartShake_Train (0x53FAD8..0x53FB08): dy*dy + dx*dx; fst dword [esp+0xC] guarda la copia redondeada a float y fcomp 4900.0f (0x8599CC) compara el valor SIN redondear.
// Devuelve true si suma < 4900 (ordenada). camera apunta a x, y (, z); point a x, y.
__declspec(noinline) bool __cdecl PadShakeTrainWithinCpp(const float* camera, const float* point, float* distanceSquared) {
    const double dy  = static_cast<double>(camera[1]) - static_cast<double>(point[1]);
    const double dx  = static_cast<double>(camera[0]) - static_cast<double>(point[0]);
    const double sum = dy * dy + dx * dx;
    *distanceSquared = static_cast<float>(sum);
    return sum < 4900.0;
}

// CPad::StartShake_Train (0x53FB18..0x53FB36): fld dword distanceSquared; fsqrt; fsubr 70.0f (0x858CE0); fadd 30.0f (0x858CA4); call _ftol2 (0x821B40); mov [edi+0x110], al.
// El truncado lo hace el __ftol2 del compilador (mismo algoritmo que 0x821B40, incluidas las banderas x87); solo se usa el byte bajo.
__declspec(noinline) unsigned char __cdecl PadShakeTrainFrequencyCpp(float distanceSquared) {
    const double remaining = (70.0 - sqrt(static_cast<double>(distanceSquared))) + 30.0;
    return static_cast<unsigned char>(static_cast<long long>(remaining));
}
#pragma float_control(pop)

// CPad::ProcessPad (0x746B50..0x746B62): fld st(0); fsin; fstp [x]; fcos; fmul -1.0f (0x858C1C); fstp [y]. Sin precise: fsin/fcos quedan en linea (con precise serian llamadas a la CRT).
__declspec(noinline) void __cdecl PadPovSinCosCpp(double angle, float* sine, float* negCosine) {
    *sine      = static_cast<float>(sin(angle));
    *negCosine = static_cast<float>(cos(angle) * -1.0);
}

// memmgr: no hace falta ningun ayudante nuevo (MemoryMgr.cpp es entero; solo se reutiliza ResidueRegsCpp ya existente).

// --- CPedIK::PitchForSlope (lote pitfix) ---
// CMatrix::operator* (0x59BE30), llamado por CMatrix::operator*= (0x411A80): cada elemento es una cadena de fmul/faddp del x87 con exponente
// extendido (sin desborde intermedio) y un solo fstp a float. a y b apuntan a un CMatrix (right@0, forward@4, up@8, pos@12, en floats);
// r recibe los 12 floats (los rellenos no se tocan). Los productos y las sumas siguen el orden exacto del EXE.
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl PedIKFixMatMulCpp(const float* a, const float* b, float* r) {
    float out[12];
    out[0]  = (a[4] * b[1] + a[0] * b[0]) + b[2] * a[8];
    out[1]  = (a[9] * b[2] + a[1] * b[0]) + a[5] * b[1];
    out[2]  = (a[6] * b[1] + a[10] * b[2]) + b[0] * a[2];
    out[3]  = (b[6] * a[8] + a[4] * b[5]) + b[4] * a[0];
    out[4]  = (a[9] * b[6] + a[5] * b[5]) + b[4] * a[1];
    out[5]  = (b[4] * a[2] + a[10] * b[6]) + a[6] * b[5];
    out[6]  = (a[4] * b[9] + b[10] * a[8]) + b[8] * a[0];
    out[7]  = (a[9] * b[10] + a[5] * b[9]) + b[8] * a[1];
    out[8]  = (b[8] * a[2] + a[10] * b[10]) + a[6] * b[9];
    out[9]  = ((a[4] * b[13] + b[14] * a[8]) + b[12] * a[0]) + a[12];
    out[10] = ((b[14] * a[9] + b[13] * a[5]) + b[12] * a[1]) + a[13];
    out[11] = ((b[12] * a[2] + a[6] * b[13]) + a[10] * b[14]) + a[14];
    r[0] = out[0]; r[1] = out[1]; r[2] = out[2];
    r[4] = out[3]; r[5] = out[4]; r[6] = out[5];
    r[8] = out[6]; r[9] = out[7]; r[10] = out[8];
    r[12] = out[9]; r[13] = out[10]; r[14] = out[11];
}
#pragma float_control(pop)

// --- CTaskSimpleSwim: sumas/restas con escalado sin redondear el producto (lote swfix, prefijo SwFix) ---
// Insertar tras el #pragma float_control(pop) del bloque Swim (despues de SwimResistanceCpp).
// El EXE hace `fld ts; fmul k; fmul x; fadd [acum]; fstp [acum]`: los productos viven en ST0 con exponente extendido (un producto 1e-46 NO se pierde) y solo
// el fstp final redondea a float. En SSE (DLL) ts*k ya se redondea a 0 y el signo del cero resultante cambia (acum = +0 y producto negativo: EXE -0, SSE +0).
#pragma float_control(precise, on, push)
// acc + (a * b) * c  (0x689350, 0x689372, 0x68946x, 0x688C56, 0x688CD1, 0x688ED0, 0x689252, 0x689046...)
__declspec(noinline) float __cdecl SwFixAccum3Cpp(float acc, float a, float b, float c) {
    volatile float result = float((double(a) * double(b)) * double(c) + double(acc));
    return result;
}
// acc + a * b  (0x688EE0: fld ts; fmul k; fadd [rotX]; fst)
__declspec(noinline) float __cdecl SwFixAccum2Cpp(float acc, float a, float b) {
    volatile float result = float(double(a) * double(b) + double(acc));
    return result;
}
// acc - a * b  (0x689327: fld ts; fmul 0.05; fsubr [moveBlend])
__declspec(noinline) float __cdecl SwFixSubScaledCpp(float acc, float a, float b) {
    volatile float result = float(double(acc) - double(a) * double(b));
    return result;
}
// -(a - b)  (0x6891CC: fld a; fsub b; fchs): a == b da -0, nunca +0 (el compilador con /fp:fast puede reescribirlo como b - a).
__declspec(noinline) float __cdecl SwFixNegDiffCpp(float a, float b) {
    volatile float result = float(-(double(a) - double(b)));
    return result;
}
#pragma float_control(pop)

// --- CTaskSimpleJetPack::ProcessThrust (lote jpfix, prefijo JpFix). Anadir al FINAL de X87Math.cpp, DESPUES del bloque JetPack (usa JetPackSinExtCpp,
// JetPackCosExtCpp, JetPackNegSinExtCpp, JetPackSqrtExtCpp, ExeCompatiblePowCpp y kJetPackSwing*/kJetPackSpeedDamping, ya definidos arriba) ---
// Causa del fallo del caso 469: el EXE hace `fld ped.z; fsub dword [task.z]` (el SNaN queda en MEMORIA y pierde frente al NaN silencioso del registro: 0x7FC00000),
// mientras que el codigo generado de JetPackSwingSumsCpp hacia `fld dword [task.z]` (silencia el SNaN a 0x7FE00000) y `fsubr`: dos NaN silenciosos, gana el de mayor mantisa.
// La semantica de `fld R; fop m32` se verifico en hardware (ctypes, 11 patrones x 4 operaciones x ambos ordenes): memoria SNaN + registro NaN -> el registro; memoria SNaN +
// registro numero -> la memoria silenciada; dos NaN silenciosos -> mayor mantisa y, a igualdad, el de signo positivo; un solo NaN -> ese.
#pragma float_control(precise, on, push)

enum { kJpFixAdd = 0, kJpFixMul = 1, kJpFixSub = 2 /* reg - mem */, kJpFixSubr = 3 /* mem - reg */ };
static constexpr double kJpFixMagic   = double(std::bit_cast<float>(0x3C03126Fu)); // 0x863984 (0.008)
static constexpr double kJpFixNominal = double(std::bit_cast<float>(0x3F4CCCCDu)); // 0x8D2F38 (0.8)
static constexpr double kJpFixFull    = double(std::bit_cast<float>(0x3F19999Au)); // 0x8D2F3C (0.6)
static constexpr double kJpFixStrafe  = double(std::bit_cast<float>(0x3E99999Au)); // 0x8D2F40 (0.3)
static constexpr double kJpFixHeight  = double(std::bit_cast<float>(0x3D4CCCCDu)); // 0x858C28 (0.05)

// `fld reg; fop dword [mem]` con reg = ST0 (ya cargado, nunca senalizante) y mem = float en memoria que puede ser senalizante. Devuelve ST0 (double, sin redondear a float).
__declspec(noinline) static double __cdecl JpFixMemOp(int op, double reg, const float* mem) {
    unsigned int mb;
    memcpy(&mb, mem, sizeof mb);
    unsigned long long rb;
    memcpy(&rb, &reg, sizeof rb);
    const bool memNan = (mb & 0x7FFFFFFFu) > 0x7F800000u;
    const bool regNan = (rb & 0x7FFFFFFFFFFFFFFFull) > 0x7FF0000000000000ull;
    if (memNan || regNan) {
        const unsigned long long mw = (static_cast<unsigned long long>(mb & 0x80000000u) << 32) | 0x7FF0000000000000ull | (static_cast<unsigned long long>(mb & 0x007FFFFFu) << 29);
        unsigned long long result;
        if (!regNan) {
            result = mw | 0x0008000000000000ull;           // solo la memoria es NaN: se silencia
        } else if (!memNan || !(mb & 0x00400000u)) {
            result = rb;                                    // solo el registro es NaN, o la memoria es senalizante: gana el registro
        } else {                                            // dos NaN silenciosos
            const unsigned long long rs = rb & 0x000FFFFFFFFFFFFFull, ms = mw & 0x000FFFFFFFFFFFFFull;
            if (ms > rs) {
                result = mw;
            } else if (ms < rs) {
                result = rb;
            } else {
                result = (rb & 0x7FFFFFFFFFFFFFFFull) | (rb & mw & 0x8000000000000000ull); // empate: gana el positivo
            }
        }
        double d;
        memcpy(&d, &result, sizeof d);
        return d;
    }
    const double m = double(*mem);
    switch (op) {
    case kJpFixAdd:  return reg + m;
    case kJpFixMul:  return reg * m;
    case kJpFixSub:  return reg - m;
    default:         return m - reg;
    }
}

// Rama de parada, fuerza hacia arriba (0x67EF51..0x67EFD2): a = ux*0.008 (ST0, 24 bits); z se guarda como float antes de multiplicar por la masa; x se guarda tras la masa.
__declspec(noinline) void __cdecl JpFixUpForceCpp(const float* up, const float* mass, const float* step, float* out) {
    const double a = double(up[0]) * kJpFixMagic;
    const double b = double(up[1]) * kJpFixMagic;
    const double c = double(up[2]) * kJpFixMagic;
    volatile float cf = float(c);
    volatile float af = float(JpFixMemOp(kJpFixMul, a, mass));
    const double bm = JpFixMemOp(kJpFixMul, b, mass);
    const double cm = JpFixMemOp(kJpFixMul, double(cf), mass);
    out[0] = float(JpFixMemOp(kJpFixMul, double(af), step));
    out[1] = float(JpFixMemOp(kJpFixMul, bm, step));
    out[2] = float(JpFixMemOp(kJpFixMul, cm, step));
}

// Fuerza nominal (0x67F103..0x67F1AD): n = 0.8 (* 0.5 si el empuje es negativo); x: float(a*n) y float(.. * masa); y: (b*n)*masa sin guardar; z: float(c), float(cf*n), cnf*masa.
__declspec(noinline) void __cdecl JpFixNominalForceCpp(const float* up, bool halve, const float* mass, const float* step, float* out) {
    double n = kJpFixNominal;
    if (halve) {
        n = n * 0.5;
    }
    const double a = double(up[0]) * kJpFixMagic;
    const double b = double(up[1]) * kJpFixMagic;
    const double c = double(up[2]) * kJpFixMagic;
    volatile float cf = float(c);
    volatile float af = float(a * n);
    const double bn = b * n;
    volatile float cnf = float(double(cf) * n);
    volatile float afm = float(JpFixMemOp(kJpFixMul, double(af), mass));
    const double bnm = JpFixMemOp(kJpFixMul, bn, mass);
    const double cnfm = JpFixMemOp(kJpFixMul, double(cnf), mass);
    out[0] = float(JpFixMemOp(kJpFixMul, double(afm), step));
    out[1] = float(JpFixMemOp(kJpFixMul, bnm, step));
    out[2] = float(JpFixMemOp(kJpFixMul, cnfm, step));
}

// 0x67F1C2..0x67F23C: c = cos(a) (ST0); c*up[0] queda en el registro, c*up[1] y c*up[2] se guardan como float; s = -sin(a); s*fwd[i] se guardan como float; out = float(s*f + c*u);
// la z de la suma (sin redondear) se compara con 0.75 y, si es mayor, out[2] = 0.75.
__declspec(noinline) void __cdecl JpFixThrustDirCpp(const float* angle, const float* up, const float* forward, float* out) {
    const double c = JetPackCosExtCpp(*angle);
    const double cux = JpFixMemOp(kJpFixMul, c, up + 0);
    volatile float cuy = float(JpFixMemOp(kJpFixMul, c, up + 1));
    volatile float cuz = float(JpFixMemOp(kJpFixMul, c, up + 2));
    const double s = JetPackNegSinExtCpp(*angle);
    volatile float sfx = float(JpFixMemOp(kJpFixMul, s, forward + 0));
    volatile float sfy = float(JpFixMemOp(kJpFixMul, s, forward + 1));
    volatile float sfz = float(JpFixMemOp(kJpFixMul, s, forward + 2));
    out[0] = float(double(sfx) + cux);
    out[1] = float(double(sfy) + double(cuy));
    const double t2 = double(sfz) + double(cuz);
    out[2] = float(t2);
    if (t2 > double(std::bit_cast<float>(0x3F400000u))) { // 0x8703DC (0.75)
        out[2] = std::bit_cast<float>(0x3F400000u);
    }
}

// (suelo + 100) < z: fadd 100 (24 bits) y fcomp contra la memoria de z; NaN -> falso.
__declspec(noinline) bool __cdecl JpFixHeightBelowCpp(float groundHeight, const float* z) {
    const double s = double(groundHeight) + 100.0; // 0x8703D8
    return s < double(*z);
}

// 0x67F2B5..0x67F368: hm = 1, o 1 - (z - (suelo2 + 100)) * 0.05 acotado por abajo a 0 (comparacion estricta: NaN se conserva); p = t0*0.008, q = t1*0.008, r = float(t2*0.008);
// float(p*hm), q*hm, float(float(r)*hm) * 0.6; (float(p*hm)*0.6 guardado) * masa guardado; q*hm*0.6 * masa guardado; r*0.6*masa; cada uno por el paso.
__declspec(noinline) void __cdecl JpFixThrustForceCpp(const float* t, bool scaled, float groundHeight, const float* z, const float* mass, const float* step, float* out) {
    double hm = 1.0;
    if (scaled) {
        const double s = double(groundHeight) + 100.0;
        hm = 1.0 - JpFixMemOp(kJpFixSubr, s, z) * kJpFixHeight;
        if (0.0 > hm) {
            hm = 0.0;
        }
    }
    const double p = double(t[0]) * kJpFixMagic;
    const double q = double(t[1]) * kJpFixMagic;
    const double r = double(t[2]) * kJpFixMagic;
    volatile float rf = float(r);
    volatile float pf = float(p * hm);
    const double qh = q * hm;
    volatile float rhf = float(double(rf) * hm);
    volatile float pff = float(double(pf) * kJpFixFull);
    const double qh6 = qh * kJpFixFull;
    const double r6 = double(rhf) * kJpFixFull;
    const double m = double(*mass);
    volatile float pm = float(double(pff) * m);
    volatile float qm = float(qh6 * m);
    const double r6m = r6 * m;
    out[0] = float(JpFixMemOp(kJpFixMul, double(pm), step));
    out[1] = float(JpFixMemOp(kJpFixMul, double(qm), step));
    out[2] = float(JpFixMemOp(kJpFixMul, r6m, step));
}

// 0x67F386..0x67F433: s*right[0] queda en el registro; s*right[1] y s*right[2] se guardan; * 0.008; z se guarda; * 0.3; x se guarda tras *0.3; * masa (x e y guardados); * paso.
__declspec(noinline) void __cdecl JpFixStrafeForceCpp(const float* strafe, const float* right, const float* mass, const float* step, float* out) {
    const double s = double(*strafe);
    const double srx = JpFixMemOp(kJpFixMul, s, right + 0);
    volatile float sry = float(JpFixMemOp(kJpFixMul, s, right + 1));
    volatile float srz = float(JpFixMemOp(kJpFixMul, s, right + 2));
    const double u0 = srx * kJpFixMagic;
    const double u1 = double(sry) * kJpFixMagic;
    volatile float u2f = float(double(srz) * kJpFixMagic);
    volatile float v0f = float(u0 * kJpFixStrafe);
    const double v1 = u1 * kJpFixStrafe;
    const double v2 = double(u2f) * kJpFixStrafe;
    const double m = double(*mass);
    volatile float w0 = float(double(v0f) * m);
    volatile float w1 = float(v1 * m);
    const double v2m = v2 * m;
    out[0] = float(JpFixMemOp(kJpFixMul, double(w0), step));
    out[1] = float(JpFixMemOp(kJpFixMul, double(w1), step));
    out[2] = float(JpFixMemOp(kJpFixMul, v2m, step));
}

// 0x67EFDA..0x67F0FB (velocidad ya modificada por la primera fuerza): spd = float((x*x + y*y) + z*z); sf = (paso*0.5)*0.008 sin redondear; si sf^2 > spd, sf *= 0.75;
// si sigue sf^2 > spd -> false (velocidad a cero). Si no: n = v * -1 (z guardado), inv = 1/sqrt(spd) (guardado aparte, pero x lo usa sin redondear),
// ax = float(inv*nx), ay = ny*float(inv), az = float(float(nz)*float(inv)); e0 = float(ax*sf), e1 = float(ay*sf), e2 = sf*az; out = (e0, e1, e2) * masa con x e y guardados como float.
__declspec(noinline) bool __cdecl JpFixStopForceCpp(const float* speed, const float* step, const float* mass, float* out) {
    const double x = speed[0], y = speed[1], z = speed[2];
    volatile float spdSq = float((x * x + y * y) + z * z);
    double sf = double(*step) * 0.5 * kJpFixMagic; // 0x8D2F44, 0x863984
    if (sf * sf > double(spdSq)) {
        sf = sf * double(std::bit_cast<float>(0x3F400000u)); // 0x858F34
    }
    if (sf * sf > double(spdSq)) {
        return false;
    }
    const double nx = double(speed[0]) * -1.0;
    const double ny = double(speed[1]) * -1.0;
    volatile float nzf = float(double(speed[2]) * -1.0);
    const double inv = 1.0 / JetPackSqrtExtCpp(double(spdSq));
    volatile float invf = float(inv);
    volatile float axf = float(inv * nx);
    const double ay = ny * double(invf);
    volatile float azf = float(double(nzf) * double(invf));
    volatile float e0f = float(double(axf) * sf);
    volatile float e1f = float(ay * sf);
    const double e2 = sf * double(azf);
    const double m = double(*mass);
    out[0] = float(double(e0f) * m);
    out[1] = float(double(e1f) * m);
    out[2] = float(e2 * m);
    return true;
}

// 0x67F43D..0x67F48A. accX/accY/deltaX quedan en ST0 (sin guardar); accZ/deltaY/deltaZ se guardan como float; la resta con prevPed y task es `fsub m32` (memoria, posible SNaN).
__declspec(noinline) void __cdecl JpFixSwingSumsCpp(const float* ped, const float* prevPed, const float* task, float* sums) {
    const double accX = JpFixMemOp(kJpFixSub, double(ped[0]), prevPed + 0);
    const double accY = JpFixMemOp(kJpFixSub, double(ped[1]), prevPed + 1);
    volatile float accZ = float(JpFixMemOp(kJpFixSub, double(ped[2]), prevPed + 2));
    const double deltaX = JpFixMemOp(kJpFixSub, double(ped[0]), task + 0);
    volatile float deltaY = float(JpFixMemOp(kJpFixSub, double(ped[1]), task + 1));
    volatile float deltaZ = float(JpFixMemOp(kJpFixSub, double(ped[2]), task + 2));
    sums[0] = float(deltaX + accX);
    sums[1] = float(double(deltaY) + accY);
    sums[2] = float(double(deltaZ) + double(accZ));
}

// 0x67F48E..0x67F5E4 (un eje). Todos los `fmul/fsubr/fadd m32` con datos de memoria (eje, velocidad, paso, angulo) usan JpFixMemOp; v1 = *speed - dot * -0.2 (fsubr m32);
// g = ((sin(angulo) * paso) * 0.01); v2 = (v1 - g) * pow(0.98, paso); *speed = float(v2); swung = (v2 * paso) + *angulo (fadd m32); *angulo = float(swung), acotado a +-pi/4.
__declspec(noinline) void __cdecl JpFixSwingAxisCpp(const float* sums, const float* axis, bool grounded, const float* step, float* speed, float* angle) {
    const double pw = ExeCompatiblePowCpp(double(kJetPackSwingDamp), double(*step));
    double dot = JpFixMemOp(kJpFixMul, double(sums[2]), axis + 2);
    dot = dot + JpFixMemOp(kJpFixMul, double(sums[1]), axis + 1);
    dot = dot + JpFixMemOp(kJpFixMul, double(sums[0]), axis + 0);
    if (grounded) {
        dot = 0.0;
    }
    const double v1 = JpFixMemOp(kJpFixSubr, dot * double(kJetPackSwingDeltaV), speed);
    const double g  = JpFixMemOp(kJpFixMul, JetPackSinExtCpp(*angle), step) * double(kJetPackSwingGravity);
    const double v2 = (v1 - g) * pw;
    *speed = float(v2);
    const double swung = JpFixMemOp(kJpFixAdd, JpFixMemOp(kJpFixMul, v2, step), angle);
    volatile float swungStored = float(swung);
    *angle = swungStored;
    if (swung > double(kJetPackSwingMax)) {
        *angle = kJetPackSwingMax;
    } else if (swungStored < -kJetPackSwingMax) {
        *angle = -kJetPackSwingMax;
    }
}

// 0x67F5E4: *twist = float(pow(0.98, paso) * *twist) con `fmul m32`.
__declspec(noinline) void __cdecl JpFixTwistDampCpp(float* twist, const float* step) {
    *twist = float(JpFixMemOp(kJpFixMul, ExeCompatiblePowCpp(double(kJetPackSwingDamp), double(*step)), twist));
}

// 0x67F5EA..0x67F660: mag = float(sqrt((x^2 + y^2) + z^2)); base = 1 - mag*0.02 (sin redondear); p = pow(base, paso); v[i] = float(p * v[i]) (`fmul m32`); los tres se calculan antes de escribir.
__declspec(noinline) void __cdecl JpFixSpeedDampCpp(float* velocity, const float* step) {
    const double x = velocity[0], y = velocity[1], z = velocity[2];
    volatile float magnitude = float(JetPackSqrtExtCpp((x * x + y * y) + z * z));
    const double base = 1.0 - double(magnitude) * double(kJetPackSpeedDamping);
    const double pw = ExeCompatiblePowCpp(base, double(*step));
    const float r0 = float(JpFixMemOp(kJpFixMul, pw, velocity + 0));
    const float r1 = float(JpFixMemOp(kJpFixMul, pw, velocity + 1));
    const float r2 = float(JpFixMemOp(kJpFixMul, pw, velocity + 2));
    velocity[0] = r0;
    velocity[1] = r1;
    velocity[2] = r2;
}
#pragma float_control(pop)

// --- CFormation (Formation.cpp, lote w20, prefijo Formation). Se puede colocar al final de X87Math.cpp. ---
// Reutiliza RpNanAddCpp / RpNanSubCpp / RpNanMulAddCpp / X87TimesFloatCpp / PathFindNormaliseStatusCpp (ya declarados en X87Math.h).
// Convención: los valores que el EXE guarda con fstp/fst dword se escriben como `volatile float` (o como float de salida);
// los que quedan en la pila x87 son double (con el proceso a 24 bits, cada operación double redondea la mantisa a 24 bits
// y conserva el exponente extendido, igual que el x87 del EXE).

// Raíz cuadrada aislada: /fp:fast convierte sqrt(s) < c en s < c*c si ambos están en la misma función.
static __declspec(noinline) double FormationRootCpp(double value) {
    return sqrt(value);
}

// fld dword / fstp dword de un float (quita la señal de un SNaN, como el EXE).
static __declspec(noinline) float FormationFloatThroughCpp(float value) {
    return value;
}

#pragma float_control(precise, on, push)

// CMatrix::TransformPoint del EXE (0x59C890, inlinada en 0x699FF0): out = matriz * {v0, v1, v2}. Cada componente es una cadena
// ((m·z + m·y) + m·x) + traslación sin redondear a float entre operaciones; se escribe x, luego y, luego z.
// `matrix` = float[16] con la misma disposición que CMatrix (right 0, forward 4, up 8, pos 12); `out` no puede solaparse con matrix/vector.
__declspec(noinline) void __cdecl FormationTransformPointCpp(const float* matrix, const float* vector, float* out) {
    const double x = double(vector[0]), y = double(vector[1]), z = double(vector[2]);
    out[0] = float(((double(matrix[8]) * z + double(matrix[4]) * y) + double(matrix[0]) * x) + double(matrix[12]));
    out[1] = float(((double(matrix[9]) * z + double(matrix[1]) * x) + double(matrix[5]) * y) + double(matrix[13]));
    out[2] = float(((double(matrix[10]) * z + double(matrix[2]) * x) + double(matrix[6]) * y) + double(matrix[14]));
}

// 0x69A130..0x69A15E: fld a.x; fsub b.x; fld a.y; fsub b.y; y² + x²; fsqrt; fstp dword.
__declspec(noinline) float __cdecl FormationDistance2DCpp(float firstX, float firstY, float secondX, float secondY) {
    const double dx = double(firstX) - double(secondX);
    const double dy = double(firstY) - double(secondY);
    return float(FormationRootCpp(dy * dy + dx * dx));
}

// CFormation::GenerateGatherDestinations (0x69A6A0..0x69A705): el ángulo queda en ST0 sin redondear para fsin/fcos.
static __declspec(noinline) double FormationGatherAngleCpp(int index, int count, float heading) {
    // 0x858CBC = 2π, 0x858CB8 = π, 0x858FE4 = π/2 (float). OJO: `fdiv st, st(2)` divide π entre `count` (no entre el radio).
    const double twoPi = double(std::bit_cast<float>(0x40C90FDBu));
    const double pi = double(std::bit_cast<float>(0x40490FDBu));
    const double halfPi = double(std::bit_cast<float>(0x3FC90FDBu));
    if (count > 1) {
        return ((double(index) / double(count)) * twoPi - double(heading)) + pi / double(count);
    }
    return double(heading) + halfPi;
}
#pragma float_control(pop)

// Punto del círculo: x = float(float(sin(a) · r) + px), y = float(cos(a) · r + py), z = fld/fstp de pz. sin/cos van fuera de la región `precise`.
__declspec(noinline) void __cdecl FormationGatherPointCpp(int index, int count, float heading, float radius, const float* position, float* out) {
    const double angle = FormationGatherAngleCpp(index, count, heading);
    volatile float sineProduct = float(sin(angle) * double(radius)); // fstp dword [esp+0x18]
    out[1] = float(cos(angle) * double(radius) + double(position[1]));
    out[0] = float(double(sineProduct) + double(position[0]));
    out[2] = FormationFloatThroughCpp(position[2]);
}

#pragma float_control(precise, on, push)

// CFormation::GenerateGatherDestinations_AroundCar (0x69A823..): un punto del lado `positive` (false = primera mitad, true = segunda).
// edge = pos -/+ right·offset (producto sin redondear); luego factor = float(0.5 - index/count);
// x: float(float(fwd.x·len·factor) + edge.x); y: float(fwd.y·len·factor + edge.y) todo en registro; z: float(float(fwd.z·len)·factor + edge.z).
__declspec(noinline) void __cdecl FormationGatherCarPointCpp(const float* position, const float* right, float offset, const float* forward, float length,
                                                            int index, int count, bool positive, float* out) {
    const double rightX = double(right[0]) * double(offset);
    const double rightY = double(right[1]) * double(offset);
    const double rightZ = double(right[2]) * double(offset);
    const float edgeX = float(positive ? rightX + double(position[0]) : double(position[0]) - rightX);
    const float edgeY = float(positive ? rightY + double(position[1]) : double(position[1]) - rightY);
    const float edgeZ = float(positive ? rightZ + double(position[2]) : double(position[2]) - rightZ);
    const double forwardX = double(forward[0]) * double(length);
    const double forwardY = double(forward[1]) * double(length);
    volatile float forwardZ = float(double(forward[2]) * double(length)); // fstp dword [esp+0x4C]
    volatile float factor = float(0.5 - double(index) / double(count));  // fild; fidiv; fsubr 0.5; fstp dword
    volatile float productX = float(forwardX * double(factor));          // fstp dword [esp+0x50]
    out[0] = float(double(productX) + double(edgeX));
    out[1] = float(forwardY * double(factor) + double(edgeY));
    out[2] = float(double(forwardZ) * double(factor) + double(edgeZ));
}

// CFormation::DistributeDestinations: centroide (0x69B2B7..0x69B315 destinos, 0x69B321..0x69B3B7 peatones).
// Las sumas siguen en x87 sin redondear; sumZ (y sumY en destinos) se guardan como float. Destinos: x = sumX·inv (inv sin redondear);
// y/z = float(sum)·inv. Peatones: inv se guarda como float (fst) y la multiplicación de x usa inv sin redondear, y/z usan el float.
__declspec(noinline) void __cdecl FormationCentroidCpp(const float* points, int count, bool destination, float* out) {
    double sumX = 0.0, sumY = 0.0, sumZ = 0.0;
    for (int i = 0; i < count; ++i) {
        sumX += double(points[3 * i]);
        sumY += double(points[3 * i + 1]);
        sumZ += double(points[3 * i + 2]);
    }
    volatile float storedZ = float(sumZ);
    const double inverse = 1.0 / double(count); // fild; fstp float; fdiv (exacto para |count| < 2^24)
    if (destination) {
        volatile float storedY = float(sumY);
        out[0] = float(sumX * inverse);
        out[1] = float(double(storedY) * inverse);
        out[2] = float(double(storedZ) * inverse);
    } else {
        volatile float storedInverse = float(inverse);
        out[0] = float(inverse * sumX);
        out[1] = float(sumY * double(storedInverse));
        out[2] = float(double(storedZ) * double(storedInverse));
    }
}

// Radio medio (0x69B3D0..0x69B407 y 0x69B417..0x69B44C): Σ sqrt((dz² + dy²) + dx²) / count, sin redondear a float (el EXE lo deja en ST0).
__declspec(noinline) double __cdecl FormationMeanRadiusCpp(const float* points, int count, const float* center) {
    double accumulated = 0.0;
    for (int i = 0; i < count; ++i) {
        const double dx = double(points[3 * i]) - double(center[0]);
        const double dy = double(points[3 * i + 1]) - double(center[1]);
        const double dz = double(points[3 * i + 2]) - double(center[2]);
        accumulated += FormationRootCpp((dz * dz + dy * dy) + dx * dx);
    }
    return accumulated / double(count);
}

// 0x69B44C..0x69B484: radio de destinos guardado como float y limitado por abajo a 1.0 (con NaN no cambia); radio de peatones
// (ST0, sin redondear) limitado a 1.0 igual; escala = float(destinos) / peatones.
__declspec(noinline) double __cdecl FormationRadiusScaleCpp(double destinationRadius, double pedRadius) {
    volatile float destination = float(destinationRadius);
    if (destination < 1.0f) destination = 1.0f;
    if (pedRadius < 1.0) pedRadius = 1.0;
    return double(destination) / pedRadius;
}

// 0x69B490..0x69B4F0: reescala un punto de peatón. dz y dx·scale se guardan como float; dy·scale y dz·scale siguen en ST0.
__declspec(noinline) void __cdecl FormationScalePositionCpp(const float* point, const float* pedCenter, const float* destinationCenter, double scale, float* out) {
    const double dx = double(point[0]) - double(pedCenter[0]);
    const double dy = double(point[1]) - double(pedCenter[1]);
    volatile float dz = float(double(point[2]) - double(pedCenter[2])); // fstp dword [esp+0x48]
    volatile float scaledX = float(dx * scale);                         // fstp dword [esp+0x4C]
    const double scaledY = dy * scale;
    const double scaledZ = double(dz) * scale;
    out[0] = float(double(scaledX) + double(destinationCenter[0]));
    out[1] = float(scaledY + double(destinationCenter[1]));
    out[2] = float(scaledZ + double(destinationCenter[2]));
}

// CFormation::FindNearestFreeDestination (0x69B1B0): primer destino libre de menor distancia. La distancia (ST0) se compara sin
// redondear contra el mínimo y, si gana, el mínimo pasa a ser su copia float (fst dword + fld dword). El mínimo inicial es 1e7.
// Al final: *total = float(mínimo + *total) aunque no se haya elegido nada (devuelve -1). `claimed` se lee byte a byte (test al,al).
__declspec(noinline) int __cdecl FormationNearestFreeCpp(const float* points, const unsigned char* claimed, int count, const float* origin, float* total) {
    double minimum = double(std::bit_cast<float>(0x4B189680u)); // 0x86D270 = 10000000.0f
    int selected = -1;
    for (int i = 0; i < count; ++i) {
        if (claimed[i] != 0) continue;
        const double dx = double(points[3 * i]) - double(origin[0]);
        const double dy = double(points[3 * i + 1]) - double(origin[1]);
        const double dz = double(points[3 * i + 2]) - double(origin[2]);
        const double distance = FormationRootCpp((dz * dz + dy * dy) + dx * dx);
        volatile float stored = float(distance); // fst dword [esp+8]
        if (distance < minimum) { // fcomp st(1); test ah,5; jp -> sólo "menor que" ordenado
            minimum = double(stored);
            selected = i;
        }
    }
    *total = float(minimum + double(*total));
    return selected;
}

// CFormation::FindCoverPoints (0x69B8D3..0x69B8FF): sqrt((x² + y²) + z²) < 0.005f sobre la velocidad.
__declspec(noinline) bool __cdecl FormationVehicleStationaryCpp(const float* speed) {
    const double sum = (double(speed[0]) * double(speed[0]) + double(speed[1]) * double(speed[1])) + double(speed[2]) * double(speed[2]);
    return FormationRootCpp(sum) < double(std::bit_cast<float>(0x3BA3D70Au)); // 0x858B4C
}
// CFormation::DistributeDestinations_CoverPoints (0x69B646..0x69B6C8; sólo lo usa formation-extra.patch): candidato peatón -> punto de cobertura.
// pedDistance = sqrt(dy² + dx²) respecto a `target` (ST0, sin redondear); se descarta (false) si 1.0 + pedDistance < targetDistance o hay NaN (fcomp; test ah,1).
// score = 1.0 - (((sqrt(dy² + dx²) peatón-destino + targetDistance) - pedDistance) / pedDistance) sin redondear a float.
__declspec(noinline) bool __cdecl FormationCoverScoreCpp(const float* ped, const float* destination, const float* target, float targetDistance, double* score) {
    const double dx = double(ped[0]) - double(target[0]);
    const double dy = double(ped[1]) - double(target[1]);
    const double pedDistance = FormationRootCpp(dy * dy + dx * dx);
    if (!(1.0 + pedDistance >= double(targetDistance))) return false;
    const double destinationX = double(ped[0]) - double(destination[0]);
    const double destinationY = double(ped[1]) - double(destination[1]);
    const double destinationDistance = FormationRootCpp(destinationY * destinationY + destinationX * destinationX);
    *score = 1.0 - ((destinationDistance + double(targetDistance)) - pedDistance) / pedDistance;
    return true;
}
#pragma float_control(pop)

// fight: no hace falta ningun ayudante nuevo en X87Math.cpp (TaskSimpleFight no usa x87; reutiliza ResidueRegsCpp, EntryEaxCpp y CEntity::RegisterReferenceWithResidue).

// --- CAutomobile (Automobile.cpp; lote w20, prefijo Automobile). Colocar al FINAL de X87Math.cpp (usa UnsignedAsX87 [static, definido arriba], ExeCompatiblePowCpp, ExeAsinDoubleCpp y PlaceableRollDblCpp, ya definidos antes). ---
// 0x6A884D / 0x6A887C (SpawnFlyingComponent): fild tiempo; si es negativo fadd 2^32; fadd addend; _ftol2 (EAX). Cada suma redondea a 24 bits (UnsignedAsX87 ya lo hace).
__declspec(noinline) unsigned int __cdecl AutomobileTimeMsPlusFloatCpp(unsigned int timeMs, float addend) {
    const double sum = UnsignedAsX87(timeMs) + double(addend);
    return static_cast<unsigned int>(static_cast<long long>(sum));
}

// Las cadenas siguientes son de orden estricto (sumas y productos como el EXE); lo transcendental (pow, asin) va en ayudantes aparte, sin precise.
#pragma float_control(precise, on, push)
// ProcessControl 0x6B2DCD..0x6B2DEE: fld base; fld ts; call _CIpow; fld1; fsub st(1); fmul rate; fxch; fmul audio; faddp; fstp. p queda sin redondear en ST0.
__declspec(noinline) float __cdecl AutomobileWheelSpinAudioCpp(float base, float timeStep, float audio, float rate) {
    const double p    = ExeCompatiblePowCpp(double(base), double(timeStep));
    const double rest = (1.0 - p) * double(rate);
    const double kept = p * double(audio);
    volatile float result = float(rest + kept);
    return result;
}

// ProcessControl 0x6B2989..0x6B2A34: _CIasin; fld lock; fmul degToRad; fdivp (ST0 = asin / (lock * degToRad), sin redondear a float); si forceOne o 1.0 < ST0 (fcomp st(1); test ah,5; jp)
// ST0 pasa a ser 1.0 (con NaN se conserva); fmul [esi+494h] y fstp dword.
__declspec(noinline) float __cdecl AutomobileSteerFactorCpp(float x, float steeringLock, float degToRad, float steerAngle, bool forceOne) {
    double factor = ExeAsinDoubleCpp(double(x)) / (double(steeringLock) * double(degToRad));
    if (forceOne || 1.0 < factor)
        factor = 1.0;
    volatile float result = float(factor * double(steerAngle));
    return result;
}

// ProcessControl 0x6B30E0..0x6B317E. fst dword [esp+20h] guarda una copia del primer pow que el EXE no vuelve a leer. up * -1.0f es fmul por 0x858C1C (no fchs: un NaN conserva su signo);
// la componente z se guarda como float (fstp [esp+4Ch]), el producto speed * x y speed * z tambien (fstp [esp+38h] / [esp+40h]) y speed * y queda en la pila.
__declspec(noinline) void __cdecl AutomobileHydraulicForceCpp(float turnSpeedForward, float timeStep, float base, float factor, const float* up, float turnMass, float* force) {
    static const volatile float kMinusOne = -1.0f; // 0x858C1C
    const double tsf     = double(turnSpeedForward);
    const double damped  = ExeCompatiblePowCpp(double(base), double(timeStep));
    const double ratio   = damped / (((tsf * tsf) * double(factor)) + 1.0);
    const double speed   = (ExeCompatiblePowCpp(ratio, double(timeStep)) * tsf) - tsf;
    const double mass    = double(turnMass);
    const double negX    = double(up[0]) * double(kMinusOne);
    const double negY    = double(up[1]) * double(kMinusOne);
    const float  negZ    = float(double(up[2]) * double(kMinusOne));
    const float  speedX  = float(speed * negX);
    const double speedY  = speed * negY;
    const float  speedZ  = float(speed * double(negZ));
    force[0] = float(double(speedX) * mass);
    force[1] = float(speedY * mass);
    force[2] = float(double(speedZ) * mass);
}

// ProcessCarWheelPair 0x6A5CE7..0x6A5D0F: call 0x470000 (fld base; fld ts; _CIpow); fmul temp; fadd 1.0 (0x858624); fstp.
__declspec(noinline) float __cdecl AutomobileTireDecayCpp(float base, float timeStep, float temperatureAboveOne) {
    volatile float result = float(ExeCompatiblePowCpp(double(base), double(timeStep)) * double(temperatureAboveOne) + 1.0);
    return result;
}
#pragma float_control(pop)

// HydraulicControl 0x6A0E9D..0x6A0EBF (sin precise: fpatan, fcos y fsin en linea). El jump ya viene calculado (sqrt(ud^2 + lr^2) * 1.5 / 128 exacto en float).
__declspec(noinline) void __cdecl AutomobileHydraulicStickCpp(float stickLR, float stickUD, float jump, float quarterPiNeg, float* cosOut, float* sinOut) {
    const double angle = atan2(double(stickLR), double(stickUD)) + double(quarterPiNeg);
    const float cosine = float(cos(angle) * double(jump));
    const float sine   = float(sin(angle) * double(jump));
    *cosOut = cosine;
    *sinOut = sine;
}

// ProcessSwingingDoor 0x6A9F12..0x6A9F29: fld angle; fsin; fadd bias; fmul mul (ST0 sin redondear). Sin precise para que fsin salga en linea.
static __declspec(noinline) double AutomobileBonnetSwing(float angle, float bias, float mul) {
    return (sin(double(angle)) + double(bias)) * double(mul);
}

#pragma float_control(precise, on, push)
// ProcessSwingingDoor 0x6A9F29..0x6A9F51: s*fz, s*fy, s*fx; *vy, *vz, suma, *vx, suma; fadd angVel; fstp (sin float intermedios).
__declspec(noinline) float __cdecl AutomobileBonnetAngVelCpp(float angle, float bias, float mul, const float* forward, const float* moveSpeed, float angVel) {
    const double swing = AutomobileBonnetSwing(angle, bias, mul);
    const double sz    = swing * double(forward[2]);
    const double sy    = swing * double(forward[1]);
    const double sx    = swing * double(forward[0]);
    const double py    = sy * double(moveSpeed[1]);
    const double pz    = sz * double(moveSpeed[2]);
    const double pyz   = py + pz;
    const double px    = sx * double(moveSpeed[0]);
    volatile float result = float((pyz + px) + double(angVel));
    return result;
}
#pragma float_control(pop)

// GetCarRoll 0x6A6010: igual que CPlaceable::GetRoll (0x420B30, PlaceableRollDblCpp) y fmul [0x859878] = 57.2957763671875 (exacta como float) a 24 bits; devuelve ST0 sin redondear.
extern "C" __declspec(noinline) double __cdecl AutomobileCarRollDblCpp(const float* right, float upZ) {
    return PlaceableRollDblCpp(right, upZ) * 57.2957763671875;
}

// decision: DecisionMakerTypes.cpp no necesita ayudantes nuevos en X87Math.cpp (aritmetica entera; usa ResidueRegsCpp y CarEnterExitEntryEdxCpp, que ya existen).

// --- CVehicle::SetTransmissionRotation (0x6DBBB0), prefijo Vehicle. Añadir al final de X87Math.cpp (usa atan2 y std::bit_cast, ya disponibles). ---
// Los tres operandos de los fpatan se calculan fuera del modo normal (precise: sin reasociación de /fp:fast); el proceso corre el x87 a 24 bits, así que
// cada fadd/fsub/fmul de double equivale a la operación de memoria del EXE (fadd [R], fmul [0x858B8C], fsub [wz]; fsub [wy]; fsub [R]) con un solo redondeo.
#pragma float_control(precise, on, push)
static __declspec(noinline) double __cdecl VehicleTransmissionNumeratorCpp(float angleL, float angleR, float wheelZ) {
    return (double(angleL) + double(angleR)) * 0.5 - double(wheelZ);
}
static __declspec(noinline) double __cdecl VehicleTransmissionDifferenceCpp(float a, float b) {
    return double(a) - double(b);
}
#pragma float_control(pop)

// El fpatan (llamada al intrínseco, fuera de precise) toma los operandos en ST0/ST1 sin redondear a float; -fpatan + fstp == -float(fpatan) y el PI se suma sobre el float ya guardado (fld [angleX]; fadd [0x858CB8]; fstp).
// |x| se obtiene limpiando el bit de signo (fld quita la señal de un sNaN; fabs limpia el signo también de un NaN); |x| + |x| es exacto (fadd st(0), st(0)).
__declspec(noinline) void __cdecl VehicleTransmissionAnglesCpp(float angleL, float angleR, float matY, const float* wheelPos, bool isFront, float* outAngleX, float* outAngleY) {
    const double numerator   = VehicleTransmissionNumeratorCpp(angleL, angleR, wheelPos[2]);
    const double denominator = VehicleTransmissionDifferenceCpp(matY, wheelPos[1]);
    volatile float angleX    = float(-atan2(numerator, denominator));
    if (isFront) {
        angleX = float(double(angleX) + double(std::bit_cast<float>(0x40490FDBu)));
    }
    const double difference = VehicleTransmissionDifferenceCpp(angleL, angleR);
    const float  absX       = std::bit_cast<float>(std::bit_cast<unsigned int>(wheelPos[0]) & 0x7FFFFFFFu);
    volatile float angleY   = float(atan2(difference, double(absX) + double(absX)));
    *outAngleX = angleX;
    *outAngleY = angleY;
}

// --- Lote Garage (Garage.cpp, w20). Colocar al final de X87Math.cpp (compilado /O2 /Oi /arch:IA32 /fp:fast; el proceso corre el x87 a 24 bits). ---

// Seno y coseno de angle = doorPos * k: el fmul del x87 deja el producto en ST0 con 24 bits de mantisa y exponente extendido (sin redondear a float); fsin/fcos de precision completa y
// un fstp a float cada uno. Sin float_control(precise) para que sin/cos salgan en linea (con precise serian llamadas a la CRT).
static __declspec(noinline) void GarageDoorSinCos(float doorPos, float k, float* sinOut, float* cosOut) {
    const double angle = double(doorPos) * double(k);
    *sinOut = float(sin(angle));
    *cosOut = float(cos(angle));
}

// precise: el compilador no puede reasociar ni distribuir la negacion ni cambiar el orden de las comparaciones (NaN) de las funciones siguientes. Los float que el EXE guarda en pila con fstp
// y vuelve a cargar (dy, dx de la segunda proyeccion, -r) son `volatile float`; lo que queda en ST0 (dx de la primera proyeccion, sumas con r) son double: el hardware los redondea a 24 bits.
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl GarageDoorUpCpp(float doorPos, float k, float fx, float fy, float* up) {
    float sinA, cosA;
    GarageDoorSinCos(doorPos, k, &sinA, &cosA);
    volatile double sinTimesFy = double(sinA) * double(fy); // fld sin; fmul [forward.y]; fchs; fstp
    volatile double sinTimesFx = double(sinA) * double(fx); // fld sin; fmul [forward.x]; fstp
    up[0] = float(-sinTimesFy);
    up[1] = float(sinTimesFx);
    up[2] = cosA;
}

// 0x59C730: cada componente son dos `fld R; fmul dword [M]` y un fsubp (R, M en este orden: b.z/a.y, a.z/b.y, ...). El orden importa con NaN: un NaN senalizante en MEMORIA pierde frente a un NaN
// silencioso del registro, y si esta en el registro (fld lo silencia) gana el de mayor mantisa. El codigo generado de `double(x) * double(y)` conmuta los factores aun con float_control(precise)
// (casos 800 y 981 de Data23), asi que cada producto pasa por JpFixMemOp (registro ya cargado, memoria leida en bits; mas arriba en este archivo). El producto a 24 bits, la resta de dos registros
// (simetrica: mayor mantisa; a igualdad, el positivo) y el fstp float los hace el hardware. Este bloque ya esta dentro del float_control(precise) de GarageDoorUpCpp.
static __forceinline double GarageCrossTerm(double r1, const float* m1, double r2, const float* m2) {
    const double first  = JpFixMemOp(kJpFixMul, r1, m1);
    const double second = JpFixMemOp(kJpFixMul, r2, m2);
    return first - second;
}
__declspec(noinline) void __cdecl GarageDoorRightCpp(const float* a, const float* b, float* out) {
    out[0] = float(GarageCrossTerm(double(b[2]), a + 1, double(a[2]), b + 1)); // fld b.z; fmul a.y; fld a.z; fmul b.y
    out[1] = float(GarageCrossTerm(double(a[2]), b + 0, double(b[2]), a + 0)); // fld a.z; fmul b.x; fld b.z; fmul a.x
    out[2] = float(GarageCrossTerm(double(a[0]), b + 1, double(b[0]), a + 1)); // fld a.x; fmul b.y; fld b.x; fmul a.y
}

__declspec(noinline) bool __cdecl GarageInsidePointCpp(const float* g, const float* p, unsigned int* ecxOut) {
    if (double(p[2]) < double(g[2])) return false; // fld z; fcomp [g.z]
    if (double(p[2]) > double(g[7])) return false; // fld z; fcomp [topZ]
    volatile float dy = float(double(p[1]) - double(g[1]));
    *ecxOut = *reinterpret_cast<const volatile unsigned int*>(&dy); // fstp [esp] pisa el ECX guardado; el pop ecx final lo devuelve
    const double dx = double(p[0]) - double(g[0]);
    volatile float dxStored = float(dx); // fst dword: se guarda pero el primer producto usa ST0
    const double along = dx * double(g[3]) + double(dy) * double(g[4]);
    if (along < 0.0) return false;            // fcom [0.0f]
    if (along > double(g[8])) return false;   // fcomp [ancho]
    const double across = double(dxStored) * double(g[5]) + double(dy) * double(g[6]);
    if (across < 0.0) return false;
    if (across > double(g[9])) return false;  // fcomp [alto]
    return true;
}

__declspec(noinline) bool __cdecl GarageInsideRadiusCpp(const float* g, const float* p, float radius) {
    const double r = radius;
    if (double(g[2]) - r > double(p[2])) return false; // fld [g.z]; fsub r; fcomp z
    if (r + double(g[7]) < double(p[2])) return false; // fld r; fadd [topZ]; fcomp z
    volatile float dy = float(double(p[1]) - double(g[1]));
    const double dx = double(p[0]) - double(g[0]);
    volatile float dxStored = float(dx);
    const double along = dx * double(g[3]) + double(dy) * double(g[4]);
    volatile float negR = float(-r); // fld r; fchs; fstp dword
    if (along < double(negR)) return false;
    if (along > r + double(g[8])) return false; // fld r; fadd [ancho]; fxch; fcompp
    const double across = double(dxStored) * double(g[5]) + double(dy) * double(g[6]);
    if (across < double(negR)) return false;
    if (across > r + double(g[9])) return false;
    return true;
}

__declspec(noinline) bool __cdecl GarageEntityNearCpp(const float* g, float radius, const float* pos) {
    const double r = radius;
    if (double(g[10]) - r > double(pos[0])) return false; // izquierda - r > x
    if (r + double(g[11]) < double(pos[0])) return false; // r + derecha < x
    if (double(g[12]) - r > double(pos[1])) return false; // frente - r > y
    if (r + double(g[13]) < double(pos[1])) return false; // r + fondo < y
    if (double(g[2]) - r > double(pos[2])) return false;  // z del garaje - r > z
    if (r + double(g[7]) < double(pos[2])) return false;  // r + topZ < z
    return true;
}
#pragma float_control(pop)

// =====================================================================================================================================
// Lote Shadows.cpp (prefijo Shadows). Anadir AL FINAL de source/game_sa/X87Math.cpp (usa UnsignedAsX87, definida mas arriba, y ShadowSqrtCpp).
// Regla comun: el EXE corre la x87 a 24 bits de mantisa con exponente extendido y no guarda entre operaciones; por eso los intermedios son
// `double` (un volcado a pila no los redondea a float y el rango de un double cubre el de la x87 a 24 bits) y solo lo que el EXE guarda con
// fstp dword pasa por float (`volatile float` + return, como los demas ayudantes). Las cadenas van con float_control(precise) para que
// /fp:fast no las reasocie; lo trascendental (fsqrt, fpatan) va fuera del bloque precise.
// =====================================================================================================================================

// --- Fuera de precise: raiz y fpatan en linea ---
// fsqrt de un double (24 bits de mantisa) con un solo redondeo a 24 bits; se guarda y recarga como double (FSTP64 + FLD64), igual que ShadowSqrtCpp.
static __declspec(noinline) double ShadowsSqrtDblCpp(double value) {
    volatile double result = sqrt(value);
    return result;
}

// fchs de la x87: invierte el bit de signo tambien en un NaN. /fp:fast reescribe -(a * k) como a * (-k) y entonces el signo de un NaN no se invierte (Gfx
// Wave2, casos 33 y 85: el EXE da 0x7FC00000 y la DLL 0xFFC00000), asi que la inversion se hace sobre los bits y no puede fusionarse con el fmul.
static __declspec(noinline) double ShadowsFchsDbl(double value) {
    unsigned long long bits;
    memcpy(&bits, &value, sizeof bits);
    bits ^= 0x8000000000000000ull;
    memcpy(&value, &bits, sizeof bits);
    return value;
}

// CShadows::StoreRealTimeShadow (0x707E09-0x707E4C): elevacion = -(fpatan(-vy, -vz) * 57.29578f) y azimut = fpatan(-vx, -vy) * 57.29578f (0x859878),
// el fpatan sin redondear, un redondeo a 24 bits por el fmul y fstp dword de cada uno (en la elevacion, fchs entre el fmul y el fstp).
__declspec(noinline) void __cdecl ShadowsSunAnglesCpp(float vx, float vy, float vz, float* azimuth, float* elevation) {
    const double degrees = double(std::bit_cast<float>(0x42652EE0u));
    volatile double elevationProduct = atan2(-double(vy), -double(vz)) * degrees; // fpatan; fmul [0x859878] (24 bits)
    volatile float elevationStored = float(ShadowsFchsDbl(elevationProduct));     // fchs; fstp dword
    volatile float azimuthStored = float(atan2(-double(vx), -double(vy)) * degrees);
    *elevation = elevationStored;
    *azimuth = azimuthStored;
}

#pragma float_control(precise, on, push)
// fabs de la x87 (solo borra el bit de signo, tambien en NaN y -0).
static __forceinline double ShadowsAbsDbl(double value) {
    unsigned long long bits;
    memcpy(&bits, &value, sizeof bits);
    bits &= 0x7FFFFFFFFFFFFFFFull;
    memcpy(&value, &bits, sizeof bits);
    return value;
}
// Sumas de cuadrados en los tres ordenes del EXE; las entradas ya son valores de 24 bits (diferencias hechas en la x87).
static __declspec(noinline) double ShadowsSumSquaresYXDbl(double x, double y) { return y * y + x * x; }
static __declspec(noinline) double ShadowsSumSquaresXYZDbl(double x, double y, double z) { return (x * x + y * y) + z * z; }
static __declspec(noinline) double ShadowsSumSquaresZYXDbl(double x, double y, double z) { return (z * z + y * y) + x * x; }

// CShadows::UpdatePermanentShadows (0x70C9B9-0x70CA46): int((1.0f - transcurrido / cuarto) * valor), con los dos enteros sin signo cargados como fild (+ 2^32 si el bit alto esta
// puesto) y _ftol2 (palabra baja; NaN o fuera de rango -> 0). Antes era un fistp de 32 bits con la palabra de control en truncamiento (0/0 daba 0x80000000, que tras truncar a 16 u 8 bits coincide con 0).
__declspec(noinline) int __cdecl ShadowsPermanentFadeCpp(unsigned int elapsed, unsigned int quarter, int value) {
    const double ratio = UnsignedAsX87(elapsed) / UnsignedAsX87(quarter);
    const double fade = 1.0f - ratio; // fsubr dword [0x858624]
    return static_cast<int>(static_cast<long long>(fade * double(value)));
}

// CShadows::AddPermanentShadow (0x707007-0x70702E): x*x + y*y < limite con cada producto y la suma redondeados a 24 bits (fcomp + test ah,5 + jp: false con NaN).
__declspec(noinline) bool __cdecl ShadowsSumSquaresBelowCpp(float x, float y, float limit) {
    const double sum = double(x) * x + double(y) * y;
    return sum < limit;
}

// CShadows::UpdatePermanentShadows (0x70CB24-0x70CB35 y 0x70CBEE-0x70CBF9): mag = fsqrt(((sz-hz)^2 + (sy-hy)^2) + (sx-hx)^2) con las diferencias source - shadow sin redondear a float;
// si mag < *shortest (estricto, false con NaN) guarda float(mag) en *shortest y devuelve true. source y shadow apuntan a x, y, z.
__declspec(noinline) bool __cdecl ShadowsOilNearerCpp(const float* source, const float* shadow, float* shortest) {
    const double magnitude = ShadowsSqrtDblCpp(ShadowsSumSquaresZYXDbl(double(source[0]) - shadow[0], double(source[1]) - shadow[1], double(source[2]) - shadow[2]));
    if (!(magnitude < *shortest)) {
        return false;
    }
    *shortest = float(magnitude);
    return true;
}

// CShadows::UpdatePermanentShadows (0x70CBBB-0x70CC24): igual que ShadowsOilNearerCpp pero ademas exige (shadow - source) . (first - source) < 0, con
// dot = (sy' * dyf) + (sx' * dxf) y dyf, dxf = first - source, todo a 24 bits sin guardar. first apunta a x, y, z.
__declspec(noinline) bool __cdecl ShadowsOilSecondCpp(const float* source, const float* shadow, const float* first, float* shortest) {
    const double firstDeltaX = double(first[0]) - source[0];
    const double firstDeltaY = double(first[1]) - source[1];
    const double magnitude = ShadowsSqrtDblCpp(ShadowsSumSquaresZYXDbl(double(source[0]) - shadow[0], double(source[1]) - shadow[1], double(source[2]) - shadow[2]));
    if (!(magnitude < *shortest)) {
        return false;
    }
    const double dot = (double(shadow[1]) - source[1]) * firstDeltaY + (double(shadow[0]) - source[0]) * firstDeltaX;
    if (!(dot < 0.0)) {
        return false;
    }
    *shortest = float(magnitude);
    return true;
}

// (a * b) + (c * d) en la x87 (producto escalar de dos terminos) guardado como float (CShadows::CastShadowEntityXY, 0x70874E-0x7088E1). Los argumentos van en el orden de carga del EXE.
__declspec(noinline) float __cdecl ShadowsDot2Cpp(float a, float b, float c, float d) {
    const double first = double(a) * b;
    const double second = double(c) * d;
    volatile float result = float(first + second);
    return result;
}

// ((a * b) + (c * d)) + e sin guardar entre la suma y e (CShadows::CastShadowEntityXY, 0x709840-0x709858: forward * y + right * x + posicion).
__declspec(noinline) float __cdecl ShadowsDot2AddCpp(float a, float b, float c, float d, float e) {
    const double first = double(a) * b;
    const double second = double(c) * d;
    const double sum = first + second;
    volatile float result = float(sum + e);
    return result;
}

// Cruce de borde (CShadows::CastShadowEntityXY, 0x708E80-0x708E9A): (x - ox) * ey - (y - oy) * ex con ex = dx - ox y ey = dy - oy (destino - origen) en registro; se devuelve
// sin redondear a float (el EXE compara ST0 con 0 antes del fst dword y el llamador guarda float(cruce)).
__declspec(noinline) double __cdecl ShadowsEdgeCrossCpp(float x, float y, float originX, float originY, float destinationX, float destinationY) {
    const double edgeX = double(destinationX) - originX;
    const double edgeY = double(destinationY) - originY;
    const double result = (double(x) - originX) * edgeY - (double(y) - originY) * edgeX;
    return result;
}

// Interseccion de recorte (CShadows::CastShadowEntityXY, 0x708EC9-0x708F45 y 0x708F7E-0x708FFB): ratio = |cruce(anterior)| / (|valor| + |cruce(anterior)|) con valor = el float guardado
// del cruce actual; (1 - ratio) * anterior + ratio * actual para la posicion (outPos[0..1]) y para la UV (outUv[0..1]). prevPos/curPos/prevUv/curUv/origin/destination apuntan a x, y;
// outPos/outUv son distintos de las entradas. El orden de los dos sumandos solo cambia el resultado con dos NaN.
__declspec(noinline) void __cdecl ShadowsClipIntersectCpp(const float* prevPos, const float* curPos, const float* prevUv, const float* curUv,
                                                          const float* origin, const float* destination, float value, float* outPos, float* outUv) {
    const double edgeX = double(destination[0]) - origin[0];
    const double edgeY = double(destination[1]) - origin[1];
    const double before = ShadowsAbsDbl((double(prevPos[0]) - origin[0]) * edgeY - (double(prevPos[1]) - origin[1]) * edgeX);
    const double ratio = before / (ShadowsAbsDbl(value) + before);
    const double keep = 1.0f - ratio; // fld [0x858624]; fsub ratio
    outUv[0] = float(keep * prevUv[0] + ratio * curUv[0]);
    outUv[1] = float(keep * prevUv[1] + ratio * curUv[1]);
    outPos[0] = float(ratio * curPos[0] + keep * prevPos[0]);
    outPos[1] = float(ratio * curPos[1] + keep * prevPos[1]);
}

// Distancia al plano (CShadows::CastShadowEntityXY, 0x7096F9-0x709713): (nz * z + ny * y) + nx * x guardado como float. normal y vertex apuntan a x, y, z.
__declspec(noinline) float __cdecl ShadowsPlaneDistanceCpp(const float* normal, const float* vertex) {
    const double zy = double(normal[2]) * vertex[2] + double(normal[1]) * vertex[1];
    volatile float result = float(zy + double(normal[0]) * vertex[0]);
    return result;
}

// Altura del plano (CShadows::CastShadowEntityXY, 0x70971D-0x709750): (-1.0f / nz) * (((ny * y) + (nx * x)) - distancia) guardado como float. normal apunta a x, y, z.
__declspec(noinline) float __cdecl ShadowsPlaneHeightCpp(const float* normal, float x, float y, float distance) {
    const double factor = -1.0f / double(normal[2]); // fld [0x858C1C]; fdiv nz
    const double sum = double(normal[1]) * y + double(normal[0]) * x;
    volatile float result = float((sum - distance) * factor);
    return result;
}

// numerador / (valor * multiplicador) guardado como float (CShadows::CastShadowEntityXYZ, 0x70A142-0x70A176: -0.5 / (vw * 0.9) y 1.0 / (vw * 0.8)).
__declspec(noinline) float __cdecl ShadowsDivByProductCpp(float numerator, float value, float multiplier) {
    const double product = double(value) * multiplier;
    volatile float result = float(double(numerator) / product);
    return result;
}

// valor * multiplicador + base guardado como float (CShadows::CastShadowEntityXYZ, 0x70A208-0x70A247: frontX * -1.1f + centro.x).
__declspec(noinline) float __cdecl ShadowsMulAddCpp(float value, float multiplier, float base) {
    const double product = double(value) * multiplier;
    volatile float result = float(product + base);
    return result;
}

// a * b en la x87 (24 bits de mantisa, exponente extendido) como double (CShadows::StoreStaticShadow 0x70BA47-0x70BA89, CastShadowEntityXYZ 0x70A3B3-0x70A3D1, StoreShadowForVehicle).
__declspec(noinline) double __cdecl ShadowsMulDblCpp(float a, float b) {
    volatile double result = double(a) * b;
    return result;
}

// |a - b| en la x87 como double (CShadows::StoreStaticShadow, 0x70BC53-0x70BCB7: fld a; fsub b; fabs).
__declspec(noinline) double __cdecl ShadowsAbsDiffCpp(float a, float b) {
    volatile double result = ShadowsAbsDbl(double(a) - b);
    return result;
}

// CShadows::CastShadowSectorList / CastRealTimeShadowSectorList (0x70A728-0x70A771 y 0x70A8B1-0x70A8FA): (posZ - zDistance) < (maxZ + altura) y (minZ + altura) < posZ, las cuatro sumas
// a 24 bits sin guardar (fcompp / fcomp + test ah,41h y test ah,5: false con NaN).
__declspec(noinline) bool __cdecl ShadowsHeightBandCpp(float posZ, float zDistance, float maxZ, float minZ, float height) {
    const double lower = double(posZ) - zDistance;
    const double upper = double(maxZ) + height;
    if (!(upper > lower)) {
        return false;
    }
    const double bottom = double(minZ) + height;
    return bottom < double(posZ);
}

// Borde del rectangulo de una sombra (CShadows::RenderStoredShadows 0x70AA76-0x70AAD6, GeneratePolysForStaticShadow 0x70B746-0x70B7A6): (|side| + |front|) y posicion -/+ esa
// extension (maximum: fadd; si no, fsubr), guardado como float.
__declspec(noinline) float __cdecl ShadowsRectEdgeCpp(float position, float front, float side, bool maximum) {
    const double extent = ShadowsAbsDbl(side) + ShadowsAbsDbl(front);
    volatile float result = float(maximum ? extent + position : position - extent);
    return result;
}

// Distancia al cuadrado a la camara (CShadows::StoreStaticShadow 0x70BA27-0x70BA43, StoreShadowForVehicle): (dy * dy) + (dx * dx) guardado como float. position y camera apuntan a x, y.
__declspec(noinline) float __cdecl ShadowsCameraDistanceSquaredCpp(const float* position, const float* camera) {
    const double dy = double(position[1]) - camera[1];
    const double dx = double(position[0]) - camera[0];
    volatile float result = float(dy * dy + dx * dx);
    return result;
}

// fade * valor (fild) truncado con _ftol2, palabra baja (CShadows::StoreStaticShadow 0x70BAB9-0x70BB10, StoreShadowForVehicle 0x70BEFE-0x70BF02). fade es un valor de 24 bits en un double.
__declspec(noinline) int __cdecl ShadowsFadeScaleCpp(double fade, int value) {
    return static_cast<int>(static_cast<long long>(fade * double(value)));
}

// a * b truncado con _ftol2, palabra baja (CShadows::CastShadowEntityXY, 0x709959-0x709979: uv * 200.0f).
__declspec(noinline) int __cdecl ShadowsTruncMulCpp(float a, float b) {
    return static_cast<int>(static_cast<long long>(double(a) * b));
}

// CShadows::StoreShadowForVehicle (0x70C035-0x70C064): desplazamiento = ((largo * 0.5f) - maxY) * shift; x -= desplazamiento * forwardX; y -= desplazamiento * forwardY.
// position apunta a x, y; las dos posiciones se leen antes de escribirse.
__declspec(noinline) void __cdecl ShadowsVehiclePositionCpp(float* position, float length, float maximumY, float shift, float forwardX, float forwardY) {
    const double x = position[0];
    const double y = position[1];
    const double offset = (double(length) * 0.5f - maximumY) * shift;
    const float newX = float(x - offset * forwardX);
    const float newY = float(y - offset * forwardY);
    position[0] = newX;
    position[1] = newY;
}

// CShadows::ShadowRenderTriangleCB (0x709F13-0x709F45): 0 si 1.0f - depth * depth < 0; si no (tambien NaN) el byte bajo de _ftol2(fild(intensity) * (1.0f - depth * depth)).
__declspec(noinline) unsigned int __cdecl ShadowsTriangleAlphaCpp(float depth, unsigned int intensity) {
    const double remaining = 1.0f - double(depth) * depth;
    if (remaining < 0.0) {
        return 0;
    }
    return static_cast<unsigned int>(static_cast<long long>(double(static_cast<int>(intensity)) * remaining)) & 0xFFu;
}

// CShadows::ShadowRenderTriangleCB (0x709D07-0x709D43): ((ny * ay) + (nz * az)) + (nx * ax) > 0 (el producto queda en la x87 sin redondear; false con NaN). normal y at apuntan a x, y, z.
__declspec(noinline) bool __cdecl ShadowsTriangleFacesAwayCpp(const float* normal, const float* at) {
    const double yz = double(normal[1]) * at[1] + double(normal[2]) * at[2];
    const double facing = yz + double(normal[0]) * at[0];
    return facing > 0.0;
}

// CShadows::StoreShadowForVehicle (0x70BE2B-0x70BE5E, rodaje de cinematica): squared / ((mult * mult) * 4.0f) guardado como float.
__declspec(noinline) float __cdecl ShadowsCutsceneScaleCpp(float squared, float lodMultiplier) {
    const double scale = (double(lodMultiplier) * lodMultiplier) * 4.0f;
    volatile float result = float(double(squared) / scale);
    return result;
}

// CShadows::StoreShadowForVehicle (0x70BEC2-0x70BEF8): 1.0f - (fsqrt(squared) - 0.75f * distance) / (0.25f * distance), sin redondear a float (se usa con ShadowsFadeScaleCpp).
// OJO: StoreStaticShadow usa otra cadena ((raiz - 0.75 * d) * (4.0 / d), ShadowDistanceFadeCpp).
__declspec(noinline) double __cdecl ShadowsVehicleFadeCpp(float squared, float distance) {
    const double root = ShadowSqrtCpp(squared);
    const double threshold = double(distance) * 0.75f;
    const double ratio = (root - threshold) / (double(distance) * 0.25f);
    volatile double result = 1.0f - ratio;
    return result;
}

// CShadows::StoreShadowForVehicle (0x70C075-0x70C0C3, moto): factor = |inclinacion| * 5.092958f + 1.0f; si esta abandonada y |rightZ| > 0.6f suma |rightZ| * 4.0f; devuelve
// float(factor * ancho). El factor no pasa por float.
__declspec(noinline) float __cdecl ShadowsBikeBreadthCpp(float lean, float breadth, bool abandoned, float rightZ) {
    double factor = ShadowsAbsDbl(lean) * 5.092958f + 1.0f;
    if (abandoned) {
        const double right = ShadowsAbsDbl(rightZ);
        if (right > 0.6f) {
            factor = factor + right * 4.0f;
        }
    }
    volatile float result = float(factor * breadth);
    return result;
}

// CShadows::StoreShadowForVehicle (0x70C17F-0x70C1D9): magnitud = fsqrt(side.y^2 + side.x^2); si magnitud < 0.5f (false con NaN) escribe *outX = (side.x * (1/mag)) * 0.5f y
// *outY = ((1/mag) * side.y) * 0.5f y devuelve true; si no, no escribe nada. side apunta a x, y.
__declspec(noinline) bool __cdecl ShadowsSideNormalizeCpp(const float* side, float* outX, float* outY) {
    const double magnitude = ShadowsSqrtDblCpp(ShadowsSumSquaresYXDbl(side[0], side[1]));
    if (!(magnitude < 0.5f)) {
        return false;
    }
    const double reciprocal = 1.0f / magnitude; // fdivr dword [0x858624]
    const float x = float((double(side[0]) * reciprocal) * 0.5f);
    const float y = float((reciprocal * side[1]) * 0.5f);
    *outX = x;
    *outY = y;
    return true;
}

// CShadows::StoreShadowForVehicle (0x70C28B-0x70C2C1): fsqrt((x*x + y*y) + z*z) * paso > 0.1f (0x858B1C), con la raiz y el producto a 24 bits sin guardar (false con NaN). speed apunta a x, y, z.
__declspec(noinline) bool __cdecl ShadowsMovementAboveCpp(const float* speed, float timeStep) {
    const double magnitude = ShadowsSqrtDblCpp(ShadowsSumSquaresXYZDbl(speed[0], speed[1], speed[2]));
    return magnitude * timeStep > 0.1f;
}
#pragma float_control(pop)

// ===================================================================================================================
// CCamera (Camera.cpp; lote w20, prefijo Camera). ANADIR AL FINAL de source/game_sa/X87Math.cpp (despues de UnsignedAsX87, que ya esta definida mas arriba).
// Convencion de este bloque: el proceso corre el x87 a 24 bits, asi que cada operacion double deja un valor de 24 bits de mantisa; los valores intermedios que el EXE
// NO guarda en memoria se fijan con "volatile double" (el qword conserva el valor exacto y evita que /fp:fast reasocie), los que el EXE guarda con fst/fstp se
// fijan con "volatile float". Nunca se guarda en un double un resultado de fsin/fcos/fptan/fpatan/fsqrt-sin-redondear (precision completa): ese se consume en la misma expresion.
// ===================================================================================================================
static constexpr float kCamera_3C8EFA35 = std::bit_cast<float>(0x3C8EFA35u); // 0x8595EC (pi / 180)
static constexpr float kCamera_40490FDB = std::bit_cast<float>(0x40490FDBu); // 0x858CB8 (pi)
static constexpr float kCamera_3C0EF921 = std::bit_cast<float>(0x3C0EF921u); // 0x8631D8 (factor del FOV en CalculateFrustumPlanes)
static constexpr float kCamera_3992CCF7 = std::bit_cast<float>(0x3992CCF7u); // 0x863280 (0.00028f)
static constexpr float kCamera_3A83126F = std::bit_cast<float>(0x3A83126Fu); // 0x858CDC (0.001f)
static constexpr float kCamera_3C23D70A = std::bit_cast<float>(0x3C23D70Au); // 0x858C58 (0.01f)

// --- tiempos (CamShake 0x50AA91, CamShakeNoPos 0x50A97E, SetPercentAlongCutScene 0x50C088, AddShakeSimple 0x50D254, LerpFOV 0x50D2A0) ---
// fild del entero con signo, +2^32 si es negativo (fadd a 24 bits) y fmul 0.001f; un solo fstp float. UnsignedMillisToSecondsCpp usa double(unsigned) (sin la
// correccion de 24 bits de +2^32), por eso aqui se usa UnsignedAsX87.
__declspec(noinline) float __cdecl CameraElapsedSecondsCpp(unsigned int elapsed) {
    volatile float seconds = float(UnsignedAsX87(elapsed) * double(kCamera_3A83126F));
    return seconds;
}

// CCamera::CamShake (0x50AA72..0x50AA8D): fmul qword 0.01 (0x85A308), fsubr qword 1.0 (0x85A310), fstp float.
__declspec(noinline) float __cdecl CameraShakePercentCpp(float distance) {
    volatile double scaled = double(distance) * 0.01;
    volatile float percent = float(1.0 - scaled);
    return percent;
}

// CCamera::SetPercentAlongCutScene (0x50C088..0x50C0AA): (percent * 0.01f) sin guardar, * finishTime (fild + 2^32 si es negativo), un solo fstp float.
__declspec(noinline) float __cdecl CameraCutSceneElapsedCpp(float percent, unsigned int finishTime) {
    volatile double scaled = double(percent) * double(kCamera_3C23D70A);
    volatile float elapsed = float(scaled * UnsignedAsX87(finishTime));
    return elapsed;
}

// CCamera::AddShakeSimple (0x50D254) y LerpFOV (0x50D29x): fild reloj (+2^32), fst start (float), fadd plus, fstp end (float). La suma usa el entero SIN pasar por float.
__declspec(noinline) void __cdecl CameraTimePlusCpp(unsigned int nowMs, float plus, float* start, float* end) {
    const double now = UnsignedAsX87(nowMs);
    *start = float(now);
    *end = float(now + double(plus));
}

// --- apuntado en tercera persona (Find3rdPersonQuickAimPitch 0x50AD40, Find3rdPersonCamTargetVector 0x514970) ---
// radianes = (fov * 0.5f) * (pi/180) con cada fmul a 24 bits; fptan queda en ST0 sin redondear y se consume en la misma expresion.
static __forceinline double CameraAimRadians(float fov) {
    volatile double halved = double(fov) * 0.5;
    volatile double radians = halved * double(kCamera_3C8EFA35);
    return radians;
}
// horizontal = float(((crosshairX - 0.5f) + (crosshairX - 0.5f)) * tan) (0x5149A6..0x5149B6: fadd st,st y fmul st(1); fstp).
__declspec(noinline) float __cdecl CameraAimHorizontalCpp(float fov, float crosshairX) {
    const double radians = CameraAimRadians(fov);
    volatile double centred = double(crosshairX) - 0.5;
    volatile double doubled = centred + centred;
    volatile float horizontal = float(tan(radians) * doubled);
    return horizontal;
}
// vertical = ((((0.5f - crosshairY) * 2) * (1.0f / aspect)) * tan) (0x5149BA..0x514A21: fmulp, fmulp; queda en ST0 sin guardar a float en el EXE).
__declspec(noinline) float __cdecl CameraAimVerticalCpp(float fov, float crosshairY, float aspect) {
    const double radians = CameraAimRadians(fov);
    volatile double centred = 0.5 - double(crosshairY);
    volatile double doubled = centred + centred;
    volatile double ratio = 1.0 / double(aspect);
    volatile double scaled = doubled * ratio;
    volatile float vertical = float(tan(radians) * scaled);
    return vertical;
}
// Find3rdPersonQuickAimPitch: -(fpatan((tan * ((0.5f - crosshairY) * 2)) * (1.0f / aspect), 1) + vertical); el resultado vuelve en ST0 (ret).
__declspec(noinline) float __cdecl CameraQuickAimPitchCpp(float fov, float vertical, float crosshairY, float aspect) {
    const double radians = CameraAimRadians(fov);
    volatile double centred = 0.5 - double(crosshairY);
    volatile double doubled = centred + centred;
    volatile double scaled = tan(radians) * doubled;
    volatile double ratio = 1.0 / double(aspect);
    volatile double tilted = scaled * ratio;
    volatile float pitch = float(-(atan2(tilted, 1.0) + double(vertical)));
    return pitch;
}

// --- CCamera::SetNearClipBasedOnPedCollision (0x50CB90..0x50CBC7) ---
// clip = ((sqrt(arg) / denominator) * factor) * (maximum - base) + base; fst float; si el valor SIN redondear es menor que base (ordenado) el resultado es base.
__declspec(noinline) float __cdecl CameraNearClipPedCpp(float arg, float base, float denominator, float factor, float maximum) {
    volatile double root = sqrt(double(arg));
    volatile double quotient = root / double(denominator);
    volatile double scaled = quotient * double(factor);
    volatile double room = double(maximum) - double(base);
    volatile double product = scaled * room;
    volatile double sum = product + double(base);
    volatile float clip = float(sum);
    if (sum < double(base)) {
        clip = base;
    }
    return clip;
}

// --- CCamera::IsSphereVisible (0x420C40..0x420D3C) ---
// point = punto ya transformado a espacio de camara (x, y, z); normals = m_avecFrustumNormals (4 x {x, y, z}); near = [0xC3EFA0], far = [0xC3EF9C].
// Cada comparacion es ordenada como la del EXE (fcomp + test ah): NaN no rechaza; rechaza y+r < near, y-r > far y cada suma de productos > r.
__declspec(noinline) bool __cdecl CameraSphereInFrustumCpp(const float* point, float radius, const float* normals, float nearLimit, float farLimit) {
    const double r = double(radius);
    volatile double nearValue = double(point[1]) + r;
    if (nearValue < double(nearLimit)) {
        return false;
    }
    volatile double farValue = double(point[1]) - r;
    if (farValue > double(farLimit)) {
        return false;
    }
    volatile double left0 = double(point[0]) * double(normals[0]);
    volatile double left1 = double(point[1]) * double(normals[1]);
    volatile double sum0 = left0 + left1;
    if (sum0 > r) {
        return false;
    }
    volatile double right0 = double(point[0]) * double(normals[3]);
    volatile double right1 = double(point[1]) * double(normals[4]);
    volatile double sum1 = right0 + right1;
    if (sum1 > r) {
        return false;
    }
    volatile double top0 = double(point[2]) * double(normals[8]);
    volatile double top1 = double(point[1]) * double(normals[7]);
    volatile double sum2 = top0 + top1;
    if (sum2 > r) {
        return false;
    }
    volatile double bottom0 = double(point[2]) * double(normals[11]);
    volatile double bottom1 = double(point[1]) * double(normals[10]);
    volatile double sum3 = bottom0 + bottom1;
    return !(sum3 > r);
}

// --- CameraLinearComponent (0x50D510, 0x50D430, 0x50D350, ...) ---
// ease: (sin((270 - ratio * 180) * pi/180) + 1) * 0.5, luego * (to - from) + from (fsin sin redondear, fadd 1.0 a 24 bits); sin ease: (to - from) * ratio + from.
__declspec(noinline) float __cdecl CameraLinearBlendCpp(float from, float to, float ratio, bool ease) {
    if (ease) {
        volatile double scaled = double(ratio) * 180.0;
        volatile double angle = 270.0 - scaled;
        volatile double radians = angle * double(kCamera_3C8EFA35);
        volatile double wave = sin(radians) + 1.0;
        volatile double halfWave = wave * 0.5;
        volatile double delta = double(to) - double(from);
        volatile double product = halfWave * delta;
        volatile float value = float(product + double(from));
        return value;
    }
    volatile double delta = double(to) - double(from);
    volatile double product = delta * double(ratio);
    volatile float value = float(product + double(from));
    return value;
}

// --- CCamera::ProcessFOVLerp (0x516500..0x51653B) ---
// ahora = fild reloj (+2^32); no corre si ahora > fin o desordenado (fcom + test ah,41h + jp); ratio = (ahora - inicio) / (fin - inicio) sin guardar nada entre medias.
__declspec(noinline) bool __cdecl CameraFovLerpRatioCpp(unsigned int nowMs, float endTime, float startTime, float* ratio) {
    const double now = UnsignedAsX87(nowMs);
    if (!(now <= double(endTime))) {
        return false;
    }
    volatile double elapsed = now - double(startTime);
    volatile double total = double(endTime) - double(startTime);
    *ratio = float(elapsed / total);
    return true;
}

// --- CCamera::Process, transicion (0x52BA9D..0x52CB10) ---
// fase = fild(elapsed) / fild(duration) (cada fild con +2^32 si es negativo), un solo redondeo de 24 bits.
__declspec(noinline) float __cdecl CameraPhaseCpp(unsigned int elapsed, unsigned int duration) {
    volatile float phase = float(UnsignedAsX87(elapsed) / UnsignedAsX87(duration));
    return phase;
}
// ease = 0.5 - cos(x * pi) * 0.5 con x = 0 / (moving - phase) / moving (initial) o 1 / (phase - moving) / catchup (no initial); moving == 0 (o catchup == 0) sustituye la fase.
// fcos sin redondear, fmul 0.5 y fsubr 0.5 a 24 bits cada uno.
__declspec(noinline) float __cdecl CameraEaseCpp(float phase, float moving, float catchup, bool initial) {
    volatile double argument;
    if (initial) {
        if (moving == 0.0f) {
            argument = 0.0;
        } else {
            volatile double remaining = double(moving) - double(phase);
            argument = remaining / double(moving);
        }
    } else {
        if (catchup == 0.0f) {
            argument = 1.0;
        } else {
            volatile double elapsed = double(phase) - double(moving);
            argument = elapsed / double(catchup);
        }
    }
    volatile double angle = argument * double(kCamera_40490FDB);
    volatile double halfCosine = cos(angle) * 0.5;
    volatile float ease = float(0.5 - halfCosine);
    return ease;
}
// out[i] = ease * speed[i] + base[i]. Productos guardados a float (fstp + fld) segun el modo del EXE; el resto queda en ST0 (extension de exponente):
// modo 0 (objetivo inicial) y: guardado y, z; modo 1 (objetivo, alcance) guardado x; modo 2 y 3 (origen y arriba iniciales) guardado z; modo 4 guardados x, y, z.
static __forceinline double CameraAdvanceProduct(double ease, double speed, bool stored) {
    if (stored) {
        volatile float product = float(ease * speed);
        return double(product);
    }
    volatile double product = ease * speed;
    return product;
}
__declspec(noinline) void __cdecl CameraAdvanceCpp(float* out, const float* base, const float* speed, float ease, int mode) {
    const double coefficient = double(ease);
    const bool storedX = mode == 1 || mode == 4;
    const bool storedY = mode == 0 || mode == 4;
    const bool storedZ = mode != 1;
    out[0] = float(CameraAdvanceProduct(coefficient, double(speed[0]), storedX) + double(base[0]));
    out[1] = float(CameraAdvanceProduct(coefficient, double(speed[1]), storedY) + double(base[1]));
    out[2] = float(CameraAdvanceProduct(coefficient, double(speed[2]), storedZ) + double(base[2]));
}
// Modos 1 y 4 con la velocidad = destination - base calculada como el EXE: modo 4 (0x40FE60: CVector - CVector) guarda cada resta a float; modo 1 (0x52BC51..0x52BC7F) deja
// las restas de x e y en ST0 sin guardar y guarda solo la de z (fstp [esp+18]).
__declspec(noinline) void __cdecl CameraAdvanceTowardCpp(float* out, const float* base, const float* destination, float ease, int mode) {
    const double coefficient = double(ease);
    if (mode == 4) {
        volatile float speedX = float(double(destination[0]) - double(base[0]));
        volatile float speedY = float(double(destination[1]) - double(base[1]));
        volatile float speedZ = float(double(destination[2]) - double(base[2]));
        out[0] = float(CameraAdvanceProduct(coefficient, double(speedX), true) + double(base[0]));
        out[1] = float(CameraAdvanceProduct(coefficient, double(speedY), true) + double(base[1]));
        out[2] = float(CameraAdvanceProduct(coefficient, double(speedZ), true) + double(base[2]));
        return;
    }
    volatile double speedX = double(destination[0]) - double(base[0]);
    volatile double speedY = double(destination[1]) - double(base[1]);
    volatile float speedZ = float(double(destination[2]) - double(base[2]));
    out[0] = float(CameraAdvanceProduct(coefficient, speedX, true) + double(base[0]));
    out[1] = float(CameraAdvanceProduct(coefficient, speedY, false) + double(base[1]));
    out[2] = float(CameraAdvanceProduct(coefficient, double(speedZ), false) + double(base[2]));
}
// fov: difference == false -> operand * coefficient + base; true -> (operand - base) * coefficient + base (fld, fsub, fmul, fadd, fstp).
__declspec(noinline) float __cdecl CameraFovAdvanceCpp(float base, float operand, float coefficient, bool difference) {
    volatile double start = difference ? double(operand) - double(base) : double(operand);
    volatile double scaled = start * double(coefficient);
    volatile float value = float(scaled + double(base));
    return value;
}
// fuerza del temblor = force - elapsed * 0.00028f (fild + 2^32 si es negativo, fmul, fsubr), un fstp/fst float.
__declspec(noinline) float __cdecl CameraShakeForceCpp(float force, unsigned int elapsed) {
    volatile double decayed = UnsignedAsX87(elapsed) * double(kCamera_3992CCF7);
    volatile float value = float(double(force) - decayed);
    return value;
}
// sum + sqrt((dz*dz + dy*dy) + dx*dx) con dx = current - previous (fsub a 24 bits); el resultado queda en ST0 (double sin pasar a float) porque el EXE lo divide despues.
__declspec(noinline) double __cdecl CameraAccumulateDistanceCpp(const float* current, const float* previous, float sum) {
    volatile double dx = double(current[0]) - double(previous[0]);
    volatile double dy = double(current[1]) - double(previous[1]);
    volatile double dz = double(current[2]) - double(previous[2]);
    volatile double zz = dz * dz;
    volatile double yy = dy * dy;
    volatile double zy = zz + yy;
    volatile double xx = dx * dx;
    volatile double all = zy + xx;
    volatile double distance = sqrt(all);
    volatile double total = distance + double(sum);
    return total;
}
// media = sum / fild(frames) (fdivr st(1)), un solo fstp float; sum es el double devuelto por CameraAccumulateDistanceCpp.
__declspec(noinline) float __cdecl CameraAverageDistanceCpp(double sum, int frames) {
    volatile float average = float(sum / double(frames));
    return average;
}

// --- CCamera::CalculateFrustumPlanes (0x514D60..0x514E5C) ---
// ny*py + nz*pz y luego + nx*px, cada producto y suma a 24 bits (el EXE guarda float al final).
__declspec(noinline) float __cdecl CameraDotYZXCpp(const float* normal, const float* position) {
    volatile double yy = double(normal[1]) * double(position[1]);
    volatile double zz = double(normal[2]) * double(position[2]);
    volatile double yz = yy + zz;
    volatile double xx = double(normal[0]) * double(position[0]);
    volatile float dot = float(yz + xx);
    return dot;
}
// terms = {cosine, sine, verticalCosine, verticalSine}: angulo = fov * 0.008726389f (0x8631D8, fmul a 24 bits), fcos y fsin guardados a float,
// ratio = fild(height) / fidiv(width) (24 bits; se guarda a float), verticalCosine = ratio * cosine y verticalSine = -(ratio * sine) (fmul a 24 bits, fchs, guardados).
__declspec(noinline) void __cdecl CameraFrustumTermsCpp(float fov, int height, int width, float* terms) {
    volatile double angle = double(fov) * double(kCamera_3C0EF921);
    volatile float cosine = float(cos(angle));
    volatile float sine = float(sin(angle));
    volatile double ratioWide = double(height) / double(width);
    volatile float ratio = float(ratioWide);
    volatile double verticalCosine = double(ratio) * double(cosine);
    volatile double verticalSine = double(ratio) * double(sine);
    terms[0] = cosine;
    terms[1] = sine;
    terms[2] = float(verticalCosine);
    terms[3] = -float(verticalSine);
}

// --- CCamera::CalculateDerivedValues (0x5151A6..0x5151E5) ---
// r = sqrt(y*y + x*x) (fsqrt a 24 bits, sin guardar); si r == 0 -> nx = 1.0f y ny = y; si no inverse = 1.0 / r y nx = inverse * x, ny = inverse * y (cada uno a 24 bits y fstp float).
__declspec(noinline) void __cdecl CameraFrontNormCpp(float x, float y, float* nx, float* ny) {
    volatile double yy = double(y) * double(y);
    volatile double xx = double(x) * double(x);
    volatile double sum = yy + xx;
    volatile double length = sqrt(sum);
    if (length == 0.0) {
        *nx = 1.0f;
        *ny = y;
        return;
    }
    volatile double inverse = 1.0 / length;
    *nx = float(inverse * double(x));
    *ny = float(inverse * double(y));
}

// --- CGeneral::GetATanOfXY (0x53CC70) y su consumo en ST0 sin redondear ---
// atanFn debe ser &CGeneral::GetATanOfXY (alias /alternatename que devuelve ST0 sin redondear), llamado a traves del puntero para que los bancos lo intercepten en la entrada de la DLL.
// StartTransition (0x515236, 0x515440, 0x5154CF..): float(atan + add) y float(atan - sub), un solo redondeo a 24 bits.
__declspec(noinline) float __cdecl CameraAtanPlusCpp(float (__cdecl* atanFn)(float, float), float x, float y, float add) {
    volatile float result = float(double(atanFn(x, y)) + double(add));
    return result;
}
__declspec(noinline) float __cdecl CameraAtanMinusCpp(float (__cdecl* atanFn)(float, float), float x, float y, float sub) {
    volatile float result = float(double(atanFn(x, y)) - double(sub));
    return result;
}
// CCamera::TryToStartNewCamMode, modos cinematicos (0x51F5DD, 0x51F82C, 0x51FB14, 0x51FD8F): angulo = atanFn(inputX, inputY) + rotation (24 bits, sin guardar);
// outX = float(cos(angle) + inputX); sine = float(sin(angle)) guardado; outY = sine + inputY.
__declspec(noinline) void __cdecl CameraCineyRotatedCpp(float (__cdecl* atanFn)(float, float), float inputX, float inputY, float rotation, float* outX, float* outY) {
    volatile double angle = double(atanFn(inputX, inputY)) + double(rotation);
    volatile float sine = float(sin(angle));
    *outX = float(cos(angle) + double(inputX));
    *outY = sine + inputY;
}
// CCamera (CameraControlChooseJump, 0x52A87A..0x52A892): float(CPlaceable::GetHeading() - sub) con el rumbo en ST0 sin redondear. headingFn = bit_cast de &CPlaceable::GetHeading
// (thiscall sin argumentos; se llama como __fastcall con ECX = self y EDX de relleno). Se llama siempre (los bancos interceptan la entrada de la DLL).
__declspec(noinline) float __cdecl CameraHeadingMinusCpp(float (__fastcall* headingFn)(const void*, void*), const void* self, float sub) {
    volatile float result = float(double(headingFn(self, nullptr)) - double(sub));
    return result;
}
// CCamera::Process (0x52C8C2 y 0x52CA1B): fld value; fmul multiplier; call _ftol2 (0x821B40); solo se usa EAX. _ftol2 devuelve el truncado de 64 bits: fuera del rango de int32 EAX son los
// 32 bits bajos (NO 0x80000000 como cvttss2si). El truncado lo hace el __ftol2 del compilador (/arch:IA32, mismo algoritmo que 0x821B40); el producto se redondea a 24 bits.
__declspec(noinline) int __cdecl CameraTruncateProductCpp(float value, float multiplier) {
    volatile double product = double(value) * double(multiplier);
    return static_cast<int>(static_cast<long long>(product));
}

// --- Lote CStats (Stats.cpp, w20), prefijo Stats. Anadir al FINAL de X87Math.cpp: usa UnsignedAsX87 (definida mas arriba), Ftol2RegsDblCpp y Ftol2Regs (lote Ped);
// <float.h> (_statusfp2, _SW_*) y <bit> ya estan incluidos. Todo es noinline y se compila con /O2 /Oi /arch:IA32 /fp:fast. ---

// Palabra de estado que deja `fnstsw ax` tras comparar st0 con other (fcom/fcomp ordenado: IE con NaN, DE con operandos desnormalizados): condiciones C0 (0x0100),
// C2 (0x0400) y C3 (0x4000) de la comparacion, banderas acumuladas leidas con _statusfp2 (IE 1, DE 2, ZE 4, OE 8, UE 0x10, PE 0x20: son las de TODAS las operaciones x87 ejecutadas
// antes en esta llamada, porque la x87 las conserva) y TOP = 8 - depthAfter (bits 11..13; depthAfter = registros ocupados de la pila x87 tras la instruccion). ES, SF y C1 valen 0.
#pragma float_control(precise, on, push)
static __forceinline unsigned int StatsAxCore(double st0, double other, unsigned int depthAfter) {
    const bool less    = st0 < other;
    const bool greater = st0 > other;
    unsigned int status = 0;
    if (less) {
        status = 0x0100;
    } else if (!greater) {
        status = (st0 == other) ? 0x4000u : 0x4500u; // igual / sin orden
    }
    unsigned int x87 = 0, sse = 0;
    _statusfp2(&x87, &sse);
    if (x87 & _SW_INVALID)    status |= 0x01;
    if (x87 & _SW_DENORMAL)   status |= 0x02;
    if (x87 & _SW_ZERODIVIDE) status |= 0x04;
    if (x87 & _SW_OVERFLOW)   status |= 0x08;
    if (x87 & _SW_UNDERFLOW)  status |= 0x10;
    if (x87 & _SW_INEXACT)    status |= 0x20;
    return status | (((8u - depthAfter) & 7u) << 11);
}

// fcompp / fcomp con los dos operandos ya en la pila (resultados de operaciones x87 anteriores).
__declspec(noinline) unsigned int __cdecl StatsCompareDblDblAxCpp(double st0, double other, unsigned int depthAfter) {
    return StatsAxCore(st0, other, depthAfter);
}
// fcom/fcomp dword ptr [mem]: st0 es un resultado x87 y el segundo operando un float de memoria.
__declspec(noinline) unsigned int __cdecl StatsCompareDblFltAxCpp(double st0, float other, unsigned int depthAfter) {
    return StatsAxCore(st0, static_cast<double>(other), depthAfter);
}
// fld dword ptr [a]; fcom/fcomp dword ptr [b].
__declspec(noinline) unsigned int __cdecl StatsCompareFltFltAxCpp(float st0, float other, unsigned int depthAfter) {
    return StatsAxCore(static_cast<double>(st0), static_cast<double>(other), depthAfter);
}
// fild [contador] (+2^32 si el bit alto esta puesto); fcomp dword ptr [umbral]; fnstsw ax (TOP = 0): UpdateStatsOnRespawn y UpdateStatsAddToHealth.
__declspec(noinline) unsigned int __cdecl StatsCounterVsFloatAxCpp(unsigned int counter, float threshold) {
    return StatsAxCore(UnsignedAsX87(counter), static_cast<double>(threshold), 0);
}
// fld dword ptr [v]; fabs; fcomp 0.0 (0x858B50); fnstsw ax (TOP = 0): CStats::UpdateStatsWhenCycling.
__declspec(noinline) unsigned int __cdecl StatsAbsVsZeroAxCpp(float v) {
    const double magnitude = std::bit_cast<double>(std::bit_cast<unsigned long long>(static_cast<double>(v)) & 0x7FFFFFFFFFFFFFFFull); // fabs: no marca excepciones
    return StatsAxCore(magnitude, 0.0, 0);
}
#pragma float_control(pop)

// Contador sin signo como lo carga el EXE: fild + (test; jge; fadd 0x858C54 = 2^32) en la precision vigente (identico a UnsignedAsX87).
__declspec(noinline) double __cdecl StatsCounterX87Cpp(unsigned int counter) {
    return UnsignedAsX87(counter);
}
// fld dword ptr [limite]; fmul dword ptr [0x858C4C] (1000.0f) sin redondear a float.
__declspec(noinline) double __cdecl StatsScaledLimitCpp(float limit) {
    return static_cast<double>(limit) * 1000.0f;
}
// fld dword ptr [c]; fmul dword ptr [c] (cuadrado de una constante de velocidad).
__declspec(noinline) double __cdecl StatsSquareCpp(float c) {
    return static_cast<double>(c) * static_cast<double>(c);
}

// Suma de cuadrados de v[0..2] (x, y, z consecutivos) en el orden del EXE: (x*x + y*y) + z*z, cada operacion a la precision vigente (UpdateStatsWhenCycling 0x55C8A1..).
#pragma float_control(precise, on, push)
__declspec(noinline) double __cdecl StatsSpeedSumSqCpp(const float* v) {
    const double x = v[0], y = v[1], z = v[2];
    return (x * x + y * y) + z * z;
}
#pragma float_control(pop)
// Modulo de v: fsqrt de la suma anterior (fuera de precise para que sea fsqrt en linea, no una llamada a la CRT): UpdateStatsWhenDriving/Flying.
__declspec(noinline) double __cdecl StatsSpeedCpp(const float* v) {
    return sqrt(StatsSpeedSumSqCpp(v));
}

// fild [entero]; fstp dword ptr [float]: la conversion redondea (PE) si el entero necesita mas de 24 bits (UpdateStatsWhenWeaponHit, habilidad entera).
__declspec(noinline) float __cdecl StatsIntToFloatCpp(int value) {
    volatile float result = static_cast<float>(static_cast<double>(value));
    return result;
}

// _ftol2 (0x821B40) de un float cargado con fld dword ptr: devuelve la palabra baja del entero truncado (EAX del EXE) y deja las banderas PE/IE del fistp.
__declspec(noinline) int __cdecl StatsFtolCpp(float v) {
    volatile long long truncated = static_cast<long long>(static_cast<double>(v));
    return static_cast<int>(truncated);
}

// ftol((paso * 0.02f [0x858B38]) * 1000.0f [0x858C4C]) con los tres registros con que sale _ftol2 (ver Ftol2RegsDblCpp); ecxAtCall = ECX en la llamada.
#pragma float_control(precise, on, push)
__declspec(noinline) Ftol2Regs __cdecl StatsStepMsRegsCpp(float step, unsigned int ecxAtCall) {
    const double v = (static_cast<double>(step) * 0.02f) * 1000.0f;
    volatile long long truncated = static_cast<long long>(v); // PE/IE del fistp
    (void)truncated;
    return Ftol2RegsDblCpp(v, ecxAtCall);
}

// ftol(fild(ticks) [+2^32 si ticks < 0] * k + base): ticks * multiplicador + contador como float (UpdateStatsWhenDriving 0x55CBA8.., Flying 0x55CCFB..).
__declspec(noinline) Ftol2Regs __cdecl StatsAccumulateRegsCpp(int ticks, float k, float base, unsigned int ecxAtCall) {
    const double v = UnsignedAsX87(static_cast<unsigned int>(ticks)) * static_cast<double>(k) + static_cast<double>(base);
    volatile long long truncated = static_cast<long long>(v);
    (void)truncated;
    return Ftol2RegsDblCpp(v, ecxAtCall);
}

// ceil(multiplicador * fild(ticks) [+2^32]) y despues _ftol2 (CStats::UpdateStatsWhenCycling 0x55C961..0x55C972: fmulp, fstp qword, call 0x823820 (ceil del CRT), call _ftol2).
// El producto (24 bits de mantisa) es exacto en double; el ceil se hace con enteros (|v| < 2^52) porque X87Math no usa <cmath>.
static double StatsCeilDouble(double v) {
    if (!(v == v) || v >= 4503599627370496.0 || v <= -4503599627370496.0) {
        return v;
    }
    double whole = static_cast<double>(static_cast<long long>(v));
    if (whole < v) {
        whole += 1.0;
    }
    return whole;
}
__declspec(noinline) Ftol2Regs __cdecl StatsCeilRegsCpp(int ticks, float multiplier, unsigned int ecxAtCall) {
    const double product = static_cast<double>(multiplier) * UnsignedAsX87(static_cast<unsigned int>(ticks));
    const double ceiled  = StatsCeilDouble(product);
    volatile long long truncated = static_cast<long long>(ceiled);
    (void)truncated;
    return Ftol2RegsDblCpp(ceiled, ecxAtCall);
}

// ftol(fild(n) * a * b) con un redondeo a 24 bits por operacion (CStats::FindCriminalRatingString, minutos de vuelo: n * [0x863FD0] * [0x859044]); /fp:fast no puede reasociar (precise).
__declspec(noinline) int __cdecl StatsFlightMinutesCpp(int n, float a, float b) {
    const double v = (static_cast<double>(n) * static_cast<double>(a)) * static_cast<double>(b);
    return static_cast<int>(static_cast<long long>(v));
}

// CStats::FindCriminalRatingNumber (0x559080..0x5591D2): toda la cadena x87 con los enteros cargados con fild y sin redondear (cada operacion a la precision vigente) y los cuatro
// _ftol2 (palabra baja del entero truncado). money5000 = dinero / 5000 como entero con signo (se suma con fiadd). El progreso se omite solo si el total es exactamente 0 (test ah,44h; jnp: con NaN calcula y el resultado es la indefinicion de _ftol2).
__declspec(noinline) int __cdecl StatsCriminalRatingCpp(int kills, int busted, int hospital, int fireLevel, int paramedicLevel, int money5000, int planes, int firesOut,
                                                        int vigilante, int ambulance, bool hasCheated, int cheated, int bullets, int hits, float made, float total) {
    double sum = static_cast<double>(kills) - static_cast<double>(busted) * 3.0f;      // fild kills; fild busted; fmul 3.0; fsubp
    sum -= static_cast<double>(hospital) * 3.0f;                                        // fild; fmul 3.0; fsubp
    sum += (static_cast<double>(fireLevel) + static_cast<double>(paramedicLevel)) * 10.0f;
    sum += static_cast<double>(money5000);                                              // fiadd
    sum += static_cast<double>(planes) * 30.0f;
    sum += static_cast<double>(firesOut);
    sum += static_cast<double>(vigilante);
    sum += static_cast<double>(ambulance);
    int value = static_cast<int>(static_cast<long long>(sum));                          // call 0x821B40
    if (!hasCheated && cheated <= 0) {
        if (value <= 0) {
            value = 0;                                                                  // setle dl; dec edx; and ecx,edx
        }
    } else {
        const double penalised = static_cast<double>(value) - static_cast<double>(cheated) * 10.0f;
        value = static_cast<int>(static_cast<long long>(penalised));
        if (value <= -10000) {
            value = -10000;                                                             // cmp ecx,0xFFFFD8F0; jg
        }
    }
    if (static_cast<double>(bullets) > 100.0f) {                                        // fcom [0x858628]; test ah,41h; jne
        const double accuracy = (static_cast<double>(hits) / static_cast<double>(bullets)) * 500.0f + static_cast<double>(value);
        value = static_cast<int>(static_cast<long long>(accuracy));
    }
    if (total != 0.0f) {                                                                // fcomp 0.0; test ah,44h; jnp: solo el igual (C3) tiene paridad impar y salta; con NaN (C3|C2) calcula
        const double progress = (static_cast<double>(made) / static_cast<double>(total)) * 1000.0f + static_cast<double>(value);
        value = static_cast<int>(static_cast<long long>(progress));
    }
    return value;
}
#pragma float_control(pop)

// CStats::GetFatAndMuscleModifier (0x559AF0..0x559F58): los 16 casos del EXE con la configuracion viva 0x8CDE58..0x8CDEC0 (k[(direccion - 0x8CDE58) / 4]), los helpers Max (0x420800) y Min
// (0x404330) en linea (con NaN devuelven el segundo argumento) y cada operacion a la precision vigente. Fuera de 0..15 (sin signo) devuelve 1.0f. Las ramas que acotan comparan
// con fcom: `test ah,5; jp` deja el valor cuando es mayor o igual o no ordenado, `test ah,41h; jne` cuando es menor o igual o no ordenado.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl StatsFatMuscleModifierCpp(unsigned int mode, float fat, float stamina, float muscle, float maxHealth, int cycling, int lung, int bike, int driving, const float* k) {
    const double thousand = 1000.0f;
    const double one      = 1.0f;
    auto K = [k](unsigned int address) { return static_cast<double>(k[(address - 0x8CDE58u) / 4u]); };
    // Solo se evaluan en los casos que las usan (las operaciones x87 dejan banderas): fat - 200, 1000 - 200, muscle - 50, 1000 - 50.
    auto FatTerm     = [&] { return static_cast<double>(fat) - K(0x8CDEC0); };
    auto FatDenom    = [&] { return thousand - K(0x8CDEC0); };
    auto MuscleTerm  = [&] { return static_cast<double>(muscle) - K(0x8CDEB8); };
    auto MuscleDenom = [&] { return thousand - K(0x8CDEB8); };
    switch (mode) {
    case 0: {
        if (static_cast<double>(fat) > 800.0f) { return 2.0f; }
        if (static_cast<double>(fat) > 400.0f) { return 1.0f; }
        return 0.0f;
    }
    case 1:
    case 2: {
        const float  t = static_cast<float>(FatTerm() / FatDenom());                        // fstp dword [esp]
        const double m = (0.0 > static_cast<double>(t)) ? 0.0 : static_cast<double>(t); // Max(0, t)
        double r = m * K(mode == 1 ? 0x8CDEBCu : 0x8CDEB0u) + one;
        r += (MuscleTerm() * K(mode == 1 ? 0x8CDEB4u : 0x8CDEACu)) / MuscleDenom();
        const double floorValue = (mode == 1) ? static_cast<double>(0.7f) : static_cast<double>(0.8f); // 0x858CB0 / 0x858C98
        if (r < floorValue) { r = floorValue; }
        return static_cast<float>(r);
    }
    case 3: {
        double r = (FatTerm() * K(0x8CDEA8)) / FatDenom() + one;
        r += (MuscleTerm() * K(0x8CDEA4)) / MuscleDenom();
        if (r < static_cast<double>(0.8f)) { r = 0.8f; }
        return static_cast<float>(r);
    }
    case 4: {
        double r = (FatTerm() * K(0x8CDEA0)) / FatDenom() + one;
        r += (MuscleTerm() * K(0x8CDE9C)) / MuscleDenom();
        if (r > 2.0f) { r = 2.0f; }
        return static_cast<float>(r);
    }
    case 5: {
        double r = (FatTerm() * K(0x8CDE98)) / FatDenom() + one;
        r += (MuscleTerm() * K(0x8CDE94)) / MuscleDenom();
        r += (static_cast<double>(stamina) * 0.001f) * K(0x8CDE90);                    // fld [0xB793D8]; fmul [0x858CDC]; fmul k; faddp
        r += (static_cast<double>(cycling) * 0.001f) * K(0x8CDE8C);                    // fild [0xB791B8]; fmul; fmul; faddp
        if (r > 2.0f) {
            r = 2.0f;
        } else if (r < 0.25f) {
            r = 0.25f;                                                                 // 0x858C84
        }
        return static_cast<float>(r);
    }
    case 6: {
        double r = (FatTerm() * K(0x8CDE88)) / FatDenom() + one;
        r += (static_cast<double>(cycling) * 0.001f) * K(0x8CDE84);
        if (r > 2.0f) {
            r = 2.0f;
        } else if (r < 0.5f) {
            r = 0.5f;                                                                  // 0x858B8C
        }
        return static_cast<float>(r);
    }
    case 7:
        return static_cast<float>((static_cast<double>(stamina) * 0.001f) * K(0x8CDE80) + 150.0f); // 0x858A28
    case 8:
        return static_cast<float>(((static_cast<double>(lung) + static_cast<double>(stamina)) * 0.0005f) * K(0x8CDE7C) + thousand); // fild [0xB791A4]; fadd [0xB793D8]; fmul [0x859978]
    case 9:
        return static_cast<float>((static_cast<double>(maxHealth) * 0.001f) * K(0x8CDE78));
    case 10:
        return static_cast<float>(K(0x8CDE78));
    case 11:
    case 12:
    case 13: {
        const float  x = static_cast<float>(static_cast<double>(bike) * 0.001f);       // fild [0xB791B4]; fmul [0x858CDC]; fstp dword [esp]
        const double m = (1.0 < static_cast<double>(x)) ? 1.0 : static_cast<double>(x); // Min(1.0, x)
        const unsigned int address = (mode == 11) ? 0x8CDE74u : (mode == 12) ? 0x8CDE70u : 0x8CDE6Cu;
        return static_cast<float>(m * K(address) + one);
    }
    case 14: {
        double r = (FatTerm() * K(0x8CDE68)) / FatDenom();
        r += (static_cast<double>(driving) * 0.001f) * K(0x8CDE64) + one;             // fild [0xB790A0]; fmul; fmul; fadd [0x858624]; faddp
        if (r > one) { r = one; }
        return static_cast<float>(r);
    }
    case 15: {
        double r = (FatTerm() * K(0x8CDE60)) / FatDenom() + one;
        r += (MuscleTerm() * K(0x8CDE5C)) / MuscleDenom();
        const double floorValue = K(0x8CDE58);
        if (r < floorValue) { r = floorValue; }
        return static_cast<float>(r);
    }
    default:
        return 1.0f;
    }
}
#pragma float_control(pop)

// --- CCam / Cam.cpp (prefijo Cam; lote w20): colocar al final de X87Math.cpp (usa ExeCompatiblePowCpp y ExeAsinDoubleCpp, ya definidos arriba) ---
// Los ayudantes transcriben, instruccion a instruccion, los bloques __asm de Cam.cpp: cada ST(i) es una variable double sN (indice absoluto en la pila x87),
// cada fadd/fsub/fmul/fdiv/fsqrt pasa por una variable volatile double (CAM_R) para que /fp:fast no reasocie ni funda operaciones (el x87 del proceso va a 24 bits:
// el resultado ya sale redondeado), fsin/fcos/fptan/fpatan se emiten en linea y se recalculan en cada uso a partir de una copia aN del argumento ya redondeado (funcion pura: el resultado de 64 bits se consume justo donde se produce, sin derrames de 53 bits entre medias), y cada fst/fstp a memoria es un almacenamiento
// volatile float (CAM_SF) con el redondeo del EXE. Las comparaciones fcom/fnstsw se reproducen con CamX87Status (C0/C2/C3 en el formato de AH) y las ramas con goto.
#define CAM_LF(a) (*(volatile float*)(unsigned)(a))
#define CAM_LD(a) (*(volatile double*)(unsigned)(a))
#define CAM_LU(a) (*(volatile unsigned*)(unsigned)(a))
#define CAM_LW(a) (*(volatile unsigned short*)(unsigned)(a))
#define CAM_LB(a) (*(volatile unsigned char*)(unsigned)(a))
#define CAM_SF(a, v) (*(volatile float*)(unsigned)(a) = (float)(v))
#define CAM_SD(a, v) (*(volatile double*)(unsigned)(a) = (double)(v))
#define CAM_SU(a, v) (*(volatile unsigned*)(unsigned)(a) = (unsigned)(v))
#define CAM_R(dst, expr) { volatile double r_ = (expr); (dst) = r_; }
// Copia de los 32 bits de un float (mov reg,[mem] / mov [mem],reg): sin pasar por el x87.
#define CAM_BITS(var) (*(const volatile unsigned*)&(var))
#define CAM_SETBITS(var, v) (*(volatile unsigned*)&(var) = (unsigned)(v))

// fcom/fcomp + fnstsw ax: bits C3 (0x40), C2 (0x04) y C0 (0x01) del byte alto de AX. Con NaN (desordenado) los tres.
#pragma float_control(precise, on, push)
static __declspec(noinline) unsigned CamX87Status(double a, double b) {
    if (a > b) {
        return 0;
    }
    if (a < b) {
        return 0x01;
    }
    if (a == b) {
        return 0x40;
    }
    return 0x45;
}
#pragma float_control(pop)
// Paridad par del byte bajo (bandera PF tras test/cmp).
static __forceinline unsigned CamParity(unsigned value) {
    value &= 0xFF;
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return ~value & 1;
}
static __forceinline double CamFabs(double value) {
    unsigned long long bits;
    memcpy(&bits, &value, sizeof(bits));
    bits &= 0x7FFFFFFFFFFFFFFFull;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

__declspec(noinline) void __cdecl CamTwoPlayerAimTanCpp(float fov, float fovScale, float aspect, float* p_tanOverAspect, float* p_tanRounded) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0;
    volatile float tanOverAspect = 0.0f;
    volatile float tanRounded = 0.0f;
    s0 = (double)fov;
    CAM_R(s0, s0 * ((double)fovScale));
    a0 = s0;
    s1 = 1.0;
    s1 = (double)aspect;
    CAM_R(s1, ((tan(a0))) / s1);
    tanOverAspect = (float)s1;
    tanRounded = (float)(tan(a0));
    *p_tanOverAspect = tanOverAspect;
    *p_tanRounded = tanRounded;
}

__declspec(noinline) void __cdecl CamLookBehindPedSideCpp(float heading, float* p_x, float* p_y) {
    double s0 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    volatile float x = 0.0f;
    volatile float y = 0.0f;
    s0 = (double)heading;
    a0 = s0;
    s0 = -(cos(a0));
    x = (float)s0;
    s0 = (double)heading;
    a1 = s0;
    s0 = -(sin(a1));
    y = (float)s0;
    *p_x = x;
    *p_y = y;
}

__declspec(noinline) void __cdecl CamLookSideOrbitCpp(float directionY, float directionX, unsigned atanFunction, float sign, float halfPi, float distance, float targetX, float* p_outX, float targetY, float* p_outY) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    volatile float outX = 0.0f;
    volatile float outY = 0.0f;
    s0 = (double)((float(__cdecl*)(float, float))(uintptr_t)atanFunction)(directionX, directionY);
    s1 = (double)sign;
    CAM_R(s1, s1 * ((double)halfPi));
    CAM_R(s0, s0 + (s1));
    s1 = s0;
    a0 = s1;
    CAM_R(s1, (cos(a0)) * ((double)distance));
    CAM_R(s1, s1 + ((double)targetX));
    outX = (float)s1;
    a1 = s0;
    CAM_R(s0, (sin(a1)) * ((double)distance));
    CAM_R(s0, s0 + ((double)targetY));
    outY = (float)s0;
    *p_outX = outX;
    *p_outY = outY;
}

__declspec(noinline) void __cdecl CamRotInFrontCarOrbitCpp(float angle, float distance, float baseX, float* p_resultX, float baseY, float* p_resultY) {
    double s0 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    volatile float resultX = 0.0f;
    volatile float resultY = 0.0f;
    s0 = (double)angle;
    a0 = s0;
    CAM_R(s0, (cos(a0)) * ((double)distance));
    CAM_R(s0, s0 + ((double)baseX));
    resultX = (float)s0;
    s0 = (double)angle;
    a1 = s0;
    CAM_R(s0, (sin(a1)) * ((double)distance));
    CAM_R(s0, s0 + ((double)baseY));
    resultY = (float)s0;
    *p_resultX = resultX;
    *p_resultY = resultY;
}

__declspec(noinline) void __cdecl CamVelocityInverseStepCpp(float one, float denominator, float* p_reciprocal) {
    double s0 = 0.0;
    volatile float reciprocal = 0.0f;
    s0 = (double)one;
    CAM_R(s0, s0 / ((double)denominator));
    reciprocal = (float)s0;
    *p_reciprocal = reciprocal;
}

__declspec(noinline) void __cdecl CamVehicleTargetSpeedCpp(float* p_forwardY, float* p_forwardX, float zero, float one, float velocityX, float velocityY, float positiveScale, float negativeScale, float half, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float forwardY = *p_forwardY;
    volatile float forwardX = *p_forwardX;
    volatile float result = *p_result;
    s0 = (double)forwardY;
    CAM_R(s0, s0 * ((double)forwardY));
    s1 = (double)forwardX;
    CAM_R(s1, s1 * ((double)forwardX));
    CAM_R(s0, s0 + (s1));
    CAM_R(s0, sqrt(s0));
    cc = CamX87Status(s0, (double)zero);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x44u);
    if (!CamParity(((fa & fb) & 0xFFu))) goto cam_camVehicleSkipNormalise;
    CAM_R(s0, ((double)one) / s0);
    s1 = (double)forwardX;
    CAM_R(s1, s1 * (s0));
    forwardX = (float)s1;
    CAM_R(s0, s0 * ((double)forwardY));
    forwardY = (float)s0;
    goto cam_camVehicleProducts;
cam_camVehicleSkipNormalise:;
cam_camVehicleProducts:;
    s0 = (double)forwardX;
    CAM_R(s0, s0 * ((double)velocityX));
    s1 = (double)forwardY;
    CAM_R(s1, s1 * ((double)velocityY));
    s2 = s1;
    CAM_R(s2, s2 + (s0));
    cc = CamX87Status(s2, (double)zero);
    s2 = s1;
    CAM_R(s2, s2 * (s1));
    s3 = s0;
    CAM_R(s3, s3 * (s0));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    CAM_R(s2, s2 + (s3));
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    CAM_R(s2, sqrt(s2));
    s0 = s2;
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_camVehicleNegative;
    CAM_R(s0, s0 * ((double)positiveScale));
    s1 = (double)one;
    cc = CamX87Status(s1, s0);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_camVehicleStore;
    s0 = (double)one;
    goto cam_camVehicleStore;
cam_camVehicleNegative:;
    CAM_R(s0, s0 * ((double)negativeScale));
    s1 = (double)half;
    cc = CamX87Status(s1, s0);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_camVehicleNegate;
    s0 = (double)half;
cam_camVehicleNegate:;
    s0 = -s0;
cam_camVehicleStore:;
    result = (float)s0;
    *p_forwardY = forwardY;
    *p_forwardX = forwardX;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamVehicleSmoothedSpeedCpp(float target, float targetWeight, float previous, float previousWeight, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)target;
    CAM_R(s0, s0 * ((double)targetWeight));
    s1 = (double)previous;
    CAM_R(s1, s1 * ((double)previousWeight));
    CAM_R(s0, s0 + (s1));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamPowerComplementCpp(float base, float step, float one, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    unsigned eax = 0;
    volatile float result = 0.0f;
    s0 = (double)base;
    s1 = (double)step;
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    CAM_R(s0, ((double)one) - s0);
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamDeadWaveCpp(float elapsed, float period, float capped, float factor, float sx, float amplitude, float sy, float* p_scaledY, float* p_y, float tx, float* p_x) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    double a0 = 0.0;
    volatile float scaledY = 0.0f;
    volatile float y = 0.0f;
    volatile float x = 0.0f;
    s0 = (double)elapsed;
    CAM_R(s0, s0 / ((double)period));
    a0 = s0;
    s1 = (double)capped;
    CAM_R(s1, s1 * ((double)factor));
    CAM_R(s0, (sin(a0)) * (s1));
    s1 = (double)sx;
    CAM_R(s1, s1 * ((double)amplitude));
    s2 = (double)sy;
    CAM_R(s2, s2 * ((double)amplitude));
    scaledY = (float)s2;
    CAM_R(s1, s1 * (s0));
    s2 = (double)scaledY;
    CAM_R(s2, s2 * (s0));
    y = (float)s2;
    CAM_R(s1, s1 + ((double)tx));
    x = (float)s1;
    *p_scaledY = scaledY;
    *p_y = y;
    *p_x = x;
}

__declspec(noinline) void __cdecl CamDeadUnsignedElapsedCpp(int elapsedInt, float unsignedCorrection, float* p_elapsed) {
    double s0 = 0.0;
    unsigned fa = 0, fb = 0;
    volatile float elapsed = *p_elapsed;
    s0 = (double)((int)elapsedInt);
    fa = (unsigned)((unsigned)elapsedInt);
    fb = (unsigned)(0x0u);
    if ((int)fa >= (int)fb) goto cam_dead_elapsed_positive;
    CAM_R(s0, s0 + ((double)unsignedCorrection));
cam_dead_elapsed_positive:;
    elapsed = (float)s0;
    *p_elapsed = elapsed;
}

__declspec(noinline) void __cdecl CamDeadCosineCpp(float elapsed, float period, float* p_cosine) {
    double s0 = 0.0;
    double a0 = 0.0;
    volatile float cosine = 0.0f;
    s0 = (double)elapsed;
    CAM_R(s0, s0 / ((double)period));
    a0 = s0;
    cosine = (float)(cos(a0));
    *p_cosine = cosine;
}

__declspec(noinline) void __cdecl CamFirstPersonPedFrontCpp(float pitch, float yaw, float* p_sineY, float* p_sineZ, float* p_cosineX) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    volatile float sineY = 0.0f;
    volatile float sineZ = 0.0f;
    volatile float cosineX = 0.0f;
    s0 = (double)pitch;
    a0 = s0;
    s1 = (double)yaw;
    a1 = s1;
    CAM_R(s1, (cos(a1)) * ((cos(a0))));
    s2 = (double)yaw;
    a2 = s2;
    CAM_R(s2, (sin(a2)) * ((cos(a0))));
    sineY = (float)s2;
    s2 = (double)pitch;
    a3 = s2;
    sineZ = (float)(sin(a3));
    CAM_R(s1, s1 * ((double)CAM_LF(0x858B3C)));
    cosineX = (float)s1;
    *p_sineY = sineY;
    *p_sineZ = sineZ;
    *p_cosineX = cosineX;
}

__declspec(noinline) void __cdecl CamAimOffsetsCpp(float fov, float* p_vResult, float* p_hResult) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0, a4 = 0.0;
    volatile float vResult = 0.0f;
    volatile float hResult = 0.0f;
    s0 = (double)fov;
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B8C)));
    CAM_R(s0, s0 * ((double)CAM_LF(0x8595EC)));
    a0 = s0;
    s1 = 1.0;
    s1 = (double)CAM_LF(0xB6EC14);
    CAM_R(s1, s1 - ((double)CAM_LF(0x858B8C)));
    CAM_R(s1, s1 + (s1));
    CAM_R(s1, s1 * ((tan(a0))));
    s2 = 1.0;
    a1 = s1;
    a2 = s2;
    vResult = (float)(atan2(a1, a2));
    s1 = (double)CAM_LF(0x858B8C);
    CAM_R(s1, s1 - ((double)CAM_LF(0xB6EC10)));
    CAM_R(s1, s1 + (s1));
    s2 = (double)CAM_LF(0x858624);
    CAM_R(s2, s2 / ((double)CAM_LF(0xC3EFA4)));
    CAM_R(s1, s1 * (s2));
    CAM_R(s0, (tan(a0)) * (s1));
    s1 = 1.0;
    a3 = s0;
    a4 = s1;
    hResult = (float)(atan2(a3, a4));
    *p_vResult = vResult;
    *p_hResult = hResult;
}

__declspec(noinline) void __cdecl CamAimTargetAnglesCpp(float tx, float fx, float ty, float fy, float tz, float fz, float* p_dz, float* p_hResult, float* p_fResult, float* p_pResult) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    volatile float dz = 0.0f;
    volatile float hResult = 0.0f;
    volatile float fResult = 0.0f;
    volatile float pResult = 0.0f;
    s0 = (double)tx;
    CAM_R(s0, s0 - ((double)fx));
    s1 = (double)ty;
    CAM_R(s1, s1 - ((double)fy));
    s2 = (double)tz;
    CAM_R(s2, s2 - ((double)fz));
    dz = (float)s2;
    s2 = s0;
    s2 = -s2;
    s3 = s1;
    a0 = s2;
    a1 = s3;
    CAM_R(s2, (atan2(a0, a1)) - ((double)CAM_LF(0x858FE4)));
    hResult = (float)s2;
    s2 = s0;
    CAM_R(s2, s2 * (s0));
    s3 = s1;
    CAM_R(s3, s3 * (s1));
    CAM_R(s2, s2 + (s3));
    fResult = (float)s2;
    s0 = (double)dz;
    s1 = (double)fResult;
    CAM_R(s1, sqrt(s1));
    { double t_ = s1; s1 = s0; s0 = t_; }
    { double t_ = s1; s1 = s0; s0 = t_; }
    a2 = s0;
    a3 = s1;
    pResult = (float)(atan2(a2, a3));
    *p_dz = dz;
    *p_hResult = hResult;
    *p_fResult = fResult;
    *p_pResult = pResult;
}

__declspec(noinline) void __cdecl CamAimProjectionCpp(unsigned cam, unsigned scratch, unsigned rowOffset) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0, a4 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
    esi = (unsigned)(cam);
    edi = (unsigned)(scratch);
    eax = (unsigned)(rowOffset);
    ebp = (unsigned)(eax);
    s0 = (double)CAM_LF((unsigned)(ebp+0x8CC4C0));
    s1 = (double)CAM_LF((unsigned)(esi+0xAC));
    cc = CamX87Status(s1, (double)CAM_LF(0x858B50));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_aim_projection_00522955;
    s1 = (double)CAM_LF((unsigned)(ebp+0x8CC4C8));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esi+0xAC))));
    s2 = (double)CAM_LF(0x858FE4);
    cc = CamX87Status(s2, s1);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_aim_projection_00522951;
    s1 = (double)CAM_LF(0x858FE4);
cam_aim_projection_00522951:;
    goto cam_aim_projection_0052295D;
cam_aim_projection_00522955:;
    s1 = (double)CAM_LF((unsigned)(esi+0xAC));
cam_aim_projection_0052295D:;
    a0 = s1;
    CAM_R(s1, (cos(a0)) * ((double)CAM_LF((unsigned)(ebp+0x8CC4C4))));
    ebp = (unsigned)((unsigned)(esi+0x190));
    edx = (unsigned)(ebp);
    CAM_R(s0, s0 + (s1));
    ebx = (unsigned)((unsigned)(esi+0x19C));
    s1 = (double)CAM_LF((unsigned)(esi+0xAC));
    a1 = s1;
    s2 = (double)CAM_LF((unsigned)(esi+0xBC));
    a2 = s2;
    CAM_R(s2, (cos(a2)) * ((cos(a1))));
    s2 = -s2;
    CAM_SF((unsigned)(edi+0x30), s2);
    eax = (unsigned)(CAM_LU((unsigned)(edi+0x30)));
    s2 = (double)CAM_LF((unsigned)(esi+0xBC));
    CAM_SU((unsigned)(edx), eax);
    a3 = s2;
    CAM_R(s2, (sin(a3)) * ((cos(a1))));
    s2 = -s2;
    CAM_SF((unsigned)(edi+0x34), s2);
    ecx = (unsigned)(CAM_LU((unsigned)(edi+0x34)));
    CAM_SU((unsigned)(edx+0x4), ecx);
    ecx = (unsigned)(ebx);
    s1 = (double)CAM_LF((unsigned)(esi+0xAC));
    a4 = s1;
    CAM_SF((unsigned)(edi+0x38), (sin(a4)));
    eax = (unsigned)(CAM_LU((unsigned)(edi+0x38)));
    s1 = s0;
    CAM_SU((unsigned)(edx+0x8), eax);
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ebp))));
    s2 = s0;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(ebp+0x4))));
    CAM_SF((unsigned)(edi+0x58), s2);
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ebp+0x8))));
    CAM_SF((unsigned)(edi+0x5C), s1);
    s1 = (double)CAM_LF((unsigned)(edi+0x3C));
    CAM_R(s1, s1 - (s0));
    CAM_SF((unsigned)(edi+0x30), s1);
    edx = (unsigned)(CAM_LU((unsigned)(edi+0x30)));
    CAM_SU((unsigned)(ecx), edx);
    s0 = (double)CAM_LF((unsigned)(edi+0x40));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edi+0x58))));
    CAM_SF((unsigned)(edi+0x34), s0);
    s0 = (double)CAM_LF((unsigned)(edi+0x44));
    eax = (unsigned)(CAM_LU((unsigned)(edi+0x34)));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edi+0x5C))));
    CAM_SU((unsigned)(ecx+0x4), eax);
    eax = (unsigned)((unsigned)(edi+0x3C));
    CAM_SF((unsigned)(edi+0x3C), s0);
    edx = (unsigned)(CAM_LU((unsigned)(edi+0x3C)));
    CAM_SU((unsigned)(ecx+0x8), edx);
    ebp = (unsigned)(ebp);
}

__declspec(noinline) void __cdecl CamAimMeleeBlendCpp(unsigned scratch) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    unsigned eax = 0, ebx = 0;
    ebx = (unsigned)(scratch);
    s0 = (double)CAM_LF(0x862F30);
    s1 = (double)CAM_LF(0xB7CB5C);
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    s1 = (double)CAM_LF(0x858624);
    CAM_R(s1, s1 - (s0));
    s2 = (double)CAM_LF((unsigned)(ebx+0x20));
    CAM_R(s2, s2 * (s1));
    s3 = (double)CAM_LF(0xB70100);
    CAM_R(s3, s3 * (s0));
    CAM_R(s2, s2 + (s3));
    CAM_SF(0xB70100, s2);
    s2 = (double)CAM_LF((unsigned)(ebx+0x14));
    CAM_R(s2, s2 * (s1));
    s3 = (double)CAM_LF(0xB700FC);
    CAM_R(s3, s3 * (s0));
    CAM_R(s2, s2 + (s3));
    CAM_SF(0xB700FC, s2);
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ebx+0x28))));
    s2 = (double)CAM_LF(0xB700F4);
    CAM_R(s2, s2 * (s0));
    CAM_R(s1, s1 + (s2));
    CAM_SF(0xB700F4, s1);
}

__declspec(noinline) void __cdecl CamAimMouseAnglesCpp(unsigned cam) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned eax = 0;
    eax = (unsigned)(cam);
    s0 = (double)CAM_LF((unsigned)(eax+0xB4));
    CAM_R(s0, s0 * ((double)CAM_LF(0x85A5C4)));
    s1 = (double)CAM_LF(0xB73428);
    CAM_R(s1, s1 * ((double)CAM_LF(0x858B90)));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 * ((double)CAM_LF(0xB6EC18)));
    s2 = (double)CAM_LF(0xB73424);
    CAM_R(s2, s2 * ((double)CAM_LF(0x8631B0)));
    CAM_R(s2, s2 * (s0));
    CAM_R(s2, s2 * ((double)CAM_LF(0xB6EC1C)));
    CAM_R(s2, s2 + ((double)CAM_LF((unsigned)(eax+0xBC))));
    CAM_SF((unsigned)(eax+0xBC), s2);
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(eax+0xAC))));
    CAM_SF((unsigned)(eax+0xAC), s1);
}

__declspec(noinline) void __cdecl CamAimVehicleResetPitchCpp(float clampedZ, float initialAlpha, float* p_result) {
    double s0 = 0.0;
    unsigned eax = 0;
    volatile float result = 0.0f;
    s0 = (double)clampedZ;
    eax = (unsigned)(0x821E70u);
    s0 = ExeAsinDoubleCpp(s0);
    CAM_R(s0, s0 + ((double)initialAlpha));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamAttachedUpCpp(float angle, float sideX, float* p_sinX, float sideY, float* p_sinY, float sideZ, float* p_sinZ, float upX, float* p_cosX, float upY, float* p_cosY, float upZ, float* p_cosZ) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    volatile float sinX = 0.0f;
    volatile float sinY = 0.0f;
    volatile float sinZ = 0.0f;
    volatile float cosX = 0.0f;
    volatile float cosY = 0.0f;
    volatile float cosZ = 0.0f;
    s0 = (double)angle;
    a0 = s0;
    s1 = (double)sideX;
    CAM_R(s1, s1 * ((sin(a0))));
    sinX = (float)s1;
    s1 = (double)sideY;
    CAM_R(s1, s1 * ((sin(a0))));
    sinY = (float)s1;
    CAM_R(s0, (sin(a0)) * ((double)sideZ));
    sinZ = (float)s0;
    s0 = (double)angle;
    a1 = s0;
    s1 = (double)upX;
    CAM_R(s1, s1 * ((cos(a1))));
    cosX = (float)s1;
    s1 = (double)upY;
    CAM_R(s1, s1 * ((cos(a1))));
    cosY = (float)s1;
    CAM_R(s0, (cos(a1)) * ((double)upZ));
    cosZ = (float)s0;
    *p_sinX = sinX;
    *p_sinY = sinY;
    *p_sinZ = sinZ;
    *p_cosX = cosX;
    *p_cosY = cosY;
    *p_cosZ = cosZ;
}

__declspec(noinline) void __cdecl CamDirectionFromAnglesCpp(float alpha, float beta, float* p_directionX, float* p_directionY, float* p_directionZ) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    volatile float directionX = 0.0f;
    volatile float directionY = 0.0f;
    volatile float directionZ = 0.0f;
    s0 = (double)alpha;
    a0 = s0;
    s1 = (double)beta;
    a1 = s1;
    CAM_R(s1, (cos(a1)) * ((cos(a0))));
    s1 = -s1;
    directionX = (float)s1;
    s1 = (double)beta;
    a2 = s1;
    CAM_R(s0, (cos(a0)) * ((sin(a2))));
    s0 = -s0;
    directionY = (float)s0;
    s0 = (double)alpha;
    a3 = s0;
    directionZ = (float)(sin(a3));
    *p_directionX = directionX;
    *p_directionY = directionY;
    *p_directionZ = directionZ;
}

__declspec(noinline) void __cdecl CamShootingSourceZCpp(float alpha, float radius, float targetZ, float* p_result) {
    double s0 = 0.0;
    double a0 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)alpha;
    a0 = s0;
    CAM_R(s0, (sin(a0)) * ((double)radius));
    CAM_R(s0, ((double)targetZ) - s0);
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamShootingOrbitCpp(unsigned cam, unsigned source, unsigned scratch) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    unsigned eax = 0, ebx = 0, esi = 0, edi = 0;
    esi = (unsigned)(cam);
    edi = (unsigned)(source);
    ebx = (unsigned)(scratch);
    s0 = (double)CAM_LF((unsigned)(edi));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(ebx+0x1C))));
    eax = (unsigned)((unsigned)(esi+0x190));
    s1 = (double)CAM_LF((unsigned)(esi+0x1A0));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(ebx+0x20))));
    s2 = (double)CAM_LF((unsigned)(ebx+0x28));
    a0 = s2;
    s4 = (double)CAM_LF((unsigned)(ebx+0x28));
    a1 = s4;
    CAM_SF((unsigned)(ebx+0x10), (cos(a1)));
    CAM_R(s3, (sin(a0)) * (s1));
    s4 = (double)CAM_LF((unsigned)(ebx+0x10));
    CAM_R(s4, s4 * (s0));
    CAM_R(s3, s3 + (s4));
    CAM_SF((unsigned)(ebx+0x30), s3);
    s3 = (double)CAM_LF((unsigned)(ebx+0x10));
    CAM_SU((unsigned)(ebx+0x10), eax);
    CAM_R(s3, s3 * (s1));
    s2 = sin(a0);
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * (s0));
    CAM_R(s2, s2 - (s3));
    s0 = s2;
    s1 = (double)CAM_LF((unsigned)(ebx+0x30));
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(ebx+0x1C))));
    CAM_SF((unsigned)(edi), s1);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(ebx+0x20))));
    CAM_SF((unsigned)(esi+0x1A0), s0);
}

__declspec(noinline) void __cdecl CamShootingTiltCpp(unsigned cam, unsigned vehicle, unsigned scratch) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0, s5 = 0.0, s6 = 0.0;
    double atanY = 0.0, atanX = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, ebx = 0, esi = 0, edi = 0;
    esi = (unsigned)(cam);
    ebx = (unsigned)(vehicle);
    edi = (unsigned)(scratch);
    s0 = (double)CAM_LF((unsigned)(ebx+0x4C));
    s1 = (double)CAM_LF((unsigned)(ebx+0x48));
    s2 = (double)CAM_LF((unsigned)(ebx+0x44));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    s4 = s1;
    CAM_R(s4, s4 * (s1));
    CAM_R(s3, s3 + (s4));
    s4 = s0;
    CAM_R(s4, s4 * (s0));
    CAM_R(s3, s3 + (s4));
    cc = CamX87Status(s3, (double)CAM_LF(0x858FC4));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_shooting_tilt_end;
    s0 = (double)CAM_LF((unsigned)(ebx+0x48));
    eax = (unsigned)(CAM_LU((unsigned)(ebx+0x594)));
    fa = (unsigned)(eax);
    fb = (unsigned)(0x3u);
    s1 = (double)CAM_LF((unsigned)(ebx+0x44));
    s2 = s1;
    CAM_R(s2, s2 * (s1));
    s3 = s0;
    CAM_R(s3, s3 * (s0));
    CAM_R(s2, s2 + (s3));
    s0 = s2;
    CAM_R(s0, sqrt(s0));
    s1 = (double)CAM_LF((unsigned)(ebx+0x4C));
    { double t_ = s1; s1 = s0; s0 = t_; }
    atanY = s0;
    atanX = s1;
    s1 = (double)CAM_LF(0x862F7C);
    if (fa != fb) goto cam_shooting_tilt_00519F81;
    s1 = (double)CAM_LF(0x862F88);
cam_shooting_tilt_00519F81:;
    s2 = (double)CAM_LF((unsigned)(ebx+0x4C));
    s3 = (double)CAM_LF((unsigned)(ebx+0x48));
    s4 = (double)CAM_LF((unsigned)(ebx+0x44));
    s5 = s4;
    CAM_R(s5, s5 * (s4));
    s6 = s3;
    CAM_R(s6, s6 * (s3));
    CAM_R(s5, s5 + (s6));
    s6 = s2;
    CAM_R(s6, s6 * (s2));
    CAM_R(s5, s5 + (s6));
    s2 = s5;
    CAM_R(s2, sqrt(s2));
    CAM_R(s2, s2 - ((double)CAM_LF(0x858C58)));
    CAM_R(s1, s1 * (s2));
    s2 = (double)CAM_LF(0x858624);
    cc = CamX87Status(s2, s1);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_shooting_tilt_00519FC1;
    s1 = (double)CAM_LF(0x858624);
cam_shooting_tilt_00519FC1:;
    CAM_R(s0, atan2(atanY, atanX) * (s1));
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(edi+0x34))));
    CAM_SF((unsigned)(edi+0x34), s0);
    s0 = (double)CAM_LF(0x862F78);
    s1 = (double)CAM_LF(0xB7CB5C);
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    s1 = (double)CAM_LF(0x858624);
    CAM_R(s1, s1 - (s0));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(edi+0x34))));
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esi+0xAC))));
    CAM_R(s0, s0 + (s1));
    CAM_SF((unsigned)(esi+0xAC), s0);
cam_shooting_tilt_end:;
}

__declspec(noinline) void __cdecl CamShootingFovCpp(unsigned cam, unsigned vehicle) {
    double s0 = 0.0, s1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, ebx = 0, ecx = 0, esi = 0;
    esi = (unsigned)(cam);
    ebx = (unsigned)(vehicle);
    eax = (unsigned)(CAM_LU((unsigned)(ebx+0x594)));
    fa = (unsigned)(eax);
    fb = (unsigned)(eax);
    if ((((fa & fb) & 0xFFFFFFFFu) == 0)) goto cam_shooting_fov_005198DB;
    fa = (unsigned)(eax);
    fb = (unsigned)(0x9u);
    if (fa != fb) goto cam_shooting_fov_0051992F;
cam_shooting_fov_005198DB:;
    ecx = (unsigned)(CAM_LU((unsigned)(ebx+0x14)));
    s0 = (double)CAM_LF((unsigned)(ebx+0x4C));
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(ecx+0x18))));
    s1 = (double)CAM_LF((unsigned)(ebx+0x48));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ecx+0x14))));
    CAM_R(s0, s0 + (s1));
    s1 = (double)CAM_LF((unsigned)(ebx+0x44));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ecx+0x10))));
    CAM_R(s0, s0 + (s1));
    cc = CamX87Status(s0, (double)CAM_LF(0x8CC540));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_shooting_fov_0051992F;
    s0 = (double)CAM_LF((unsigned)(ebx+0x4C));
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(ecx+0x18))));
    s1 = (double)CAM_LF((unsigned)(ebx+0x48));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ecx+0x14))));
    CAM_R(s0, s0 + (s1));
    s1 = (double)CAM_LF((unsigned)(ecx+0x10));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ebx+0x44))));
    CAM_R(s0, s0 + (s1));
    CAM_R(s0, s0 - ((double)CAM_LF(0x8CC540)));
    CAM_R(s0, s0 * ((double)CAM_LF(0xB7CB5C)));
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(esi+0xB4))));
    CAM_SF((unsigned)(esi+0xB4), s0);
cam_shooting_fov_0051992F:;
    s0 = (double)CAM_LF((unsigned)(esi+0xB4));
    cc = CamX87Status(s0, (double)CAM_LF(0x858CE0));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_shooting_fov_0051996F;
    s0 = (double)CAM_LF(0x8CC544);
    s1 = (double)CAM_LF(0xB7CB5C);
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    s1 = (double)CAM_LF((unsigned)(esi+0xB4));
    CAM_R(s1, s1 - ((double)CAM_LF(0x858CE0)));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 + ((double)CAM_LF(0x858CE0)));
    CAM_SF((unsigned)(esi+0xB4), s1);
cam_shooting_fov_0051996F:;
    s0 = (double)CAM_LF((unsigned)(esi+0xB4));
    cc = CamX87Status(s0, (double)CAM_LF(0x858628));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_shooting_fov_0051998E;
    CAM_SU((unsigned)(esi+0xB4), 0x42C80000u);
    goto cam_shooting_fov_end;
cam_shooting_fov_0051998E:;
    s0 = (double)CAM_LF((unsigned)(esi+0xB4));
    cc = CamX87Status(s0, (double)CAM_LF(0x858CE0));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_shooting_fov_end;
    CAM_SU((unsigned)(esi+0xB4), 0x428C0000u);
cam_shooting_fov_end:;
}

__declspec(noinline) void __cdecl CamStoredHeadingMinusHalfPiCpp(float storedHeading, float* p_vehicleHeading) {
    double s0 = 0.0;
    volatile float vehicleHeading = 0.0f;
    s0 = (double)storedHeading;
    CAM_R(s0, s0 - ((double)CAM_LF(0x858FE4)));
    vehicleHeading = (float)s0;
    *p_vehicleHeading = vehicleHeading;
}

__declspec(noinline) void __cdecl CamTwoCarsBlendWeightCpp(float blend, float* p_sine, float* p_weight) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    volatile float sine = 0.0f;
    volatile float weight = 0.0f;
    s0 = (double)blend;
    CAM_R(s0, s0 * ((double)CAM_LF(0x858CB8)));
    s1 = s0;
    a0 = s1;
    sine = (float)(sin(a0));
    a1 = s0;
    CAM_R(s0, (cos(a1)) + ((double)CAM_LF(0x858624)));
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B8C)));
    weight = (float)s0;
    *p_sine = sine;
    *p_weight = weight;
}

__declspec(noinline) void __cdecl CamTwoCarsBlendCpp(unsigned scratch, unsigned cam, unsigned firstPos, unsigned secondPos) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0, s5 = 0.0;
    unsigned eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0;
    esi = (unsigned)(scratch);
    ebx = (unsigned)(cam);
    eax = (unsigned)(firstPos);
    ecx = (unsigned)(secondPos);
    s0 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(ecx))));
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(ecx+0x4))));
    s2 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(ecx+0x8))));
    s3 = s0;
    CAM_R(s0, s0 * (s3));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    CAM_R(s0, s0 + (s3));
    s3 = s1;
    CAM_R(s3, s3 * (s1));
    CAM_R(s0, s0 + (s3));
    { double t_ = s2; s2 = s0; s0 = t_; }
    CAM_R(s2, sqrt(s2));
    s0 = s2;
    CAM_SF((unsigned)(esi+0x1C), s0);
    s0 = (double)CAM_LF((unsigned)(esi+0x48));
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(esi+0x64))));
    CAM_SF((unsigned)(esi+0x3C), s0);
    s1 = (double)CAM_LF((unsigned)(esi+0x48));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esi+0x68))));
    s2 = s1;
    s3 = (double)CAM_LF((unsigned)(esi+0x48));
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(esi+0x6C))));
    CAM_SF((unsigned)(esi+0x94), s3);
    s4 = (double)CAM_LF((unsigned)(esi+0x3C));
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(esi+0x1C))));
    CAM_SF((unsigned)(esi+0x10), s4);
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(esi+0x1C))));
    CAM_SF((unsigned)(esi+0x14), s3);
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esi+0x1C))));
    s3 = (double)CAM_LF((unsigned)(esi+0x10));
    CAM_R(s3, s3 * ((double)CAM_LF(0x858F34)));
    s4 = (double)CAM_LF((unsigned)(esi+0x14));
    CAM_R(s4, s4 * ((double)CAM_LF(0x858F34)));
    CAM_SF((unsigned)(esi+0x40), s4);
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)CAM_LF(0x858F34)));
    CAM_SF((unsigned)(esi+0x44), s3);
    s3 = (double)CAM_LF(0x858624);
    CAM_R(s3, s3 - ((double)CAM_LF((unsigned)(esi+0x2C))));
    CAM_SF((unsigned)(esi+0x1C), s3);
    s4 = (double)CAM_LF((unsigned)(esi+0x30));
    CAM_R(s4, s4 * (s3));
    s5 = s3;
    CAM_R(s5, s5 * ((double)CAM_LF((unsigned)(esi+0x34))));
    CAM_SF((unsigned)(esi+0x5C), s5);
    { double t_ = s4; s4 = s3; s3 = t_; }
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(esi+0x38))));
    CAM_SF((unsigned)(esi+0x60), s4);
    s4 = (double)CAM_LF((unsigned)(esi+0x2C));
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(esi+0x20))));
    s5 = (double)CAM_LF((unsigned)(esi+0x2C));
    CAM_R(s5, s5 * ((double)CAM_LF((unsigned)(esi+0x24))));
    CAM_SF((unsigned)(esi+0x34), s5);
    s5 = (double)CAM_LF((unsigned)(esi+0x2C));
    CAM_R(s5, s5 * ((double)CAM_LF((unsigned)(esi+0x28))));
    CAM_SF((unsigned)(esi+0x38), s5);
    CAM_R(s4, s4 + (s3));
    CAM_SF((unsigned)(esi+0x10), s4);
    s3 = (double)CAM_LF((unsigned)(esi+0x34));
    CAM_R(s3, s3 + ((double)CAM_LF((unsigned)(esi+0x5C))));
    CAM_SF((unsigned)(esi+0x14), s3);
    s3 = (double)CAM_LF((unsigned)(esi+0x38));
    eax = (unsigned)((unsigned)(ebx+0x19C));
    CAM_R(s3, s3 + ((double)CAM_LF((unsigned)(esi+0x60))));
    CAM_SF((unsigned)(esi+0x18), s3);
    s3 = (double)CAM_LF((unsigned)(esi+0x10));
    CAM_R(s3, s3 - (s2));
    CAM_SF((unsigned)(esi+0x20), s3);
    ecx = (unsigned)(CAM_LU((unsigned)(esi+0x20)));
    CAM_SU((unsigned)(eax), ecx);
    s2 = (double)CAM_LF((unsigned)(esi+0x14));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(esi+0x40))));
    CAM_SF((unsigned)(esi+0x24), s2);
    s2 = (double)CAM_LF((unsigned)(esi+0x18));
    edx = (unsigned)(CAM_LU((unsigned)(esi+0x24)));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(esi+0x44))));
    CAM_SU((unsigned)(eax+0x4), edx);
    CAM_SF((unsigned)(esi+0x28), s2);
    { double t_ = s1; s1 = s0; s0 = t_; }
    ecx = (unsigned)(CAM_LU((unsigned)(esi+0x28)));
    CAM_SF((unsigned)(esi+0x10), s1);
    CAM_SU((unsigned)(eax+0x8), ecx);
    s1 = (double)CAM_LF((unsigned)(esi+0x94));
    s2 = (double)CAM_LF((unsigned)(esi+0x1C));
    s3 = (double)CAM_LF((unsigned)(esi+0x88));
    CAM_R(s3, s3 * (s2));
    s4 = (double)CAM_LF((unsigned)(esi+0x8C));
    CAM_R(s4, s4 * (s2));
    CAM_SF((unsigned)(esi+0x5C), s4);
    s4 = (double)CAM_LF((unsigned)(esi+0x90));
    CAM_R(s4, s4 * (s2));
    CAM_SF((unsigned)(esi+0x60), s4);
    s4 = (double)CAM_LF((unsigned)(esi+0x7C));
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(esi+0x2C))));
    s5 = (double)CAM_LF((unsigned)(esi+0x80));
    CAM_R(s5, s5 * ((double)CAM_LF((unsigned)(esi+0x2C))));
    CAM_SF((unsigned)(esi+0x40), s5);
    s5 = (double)CAM_LF((unsigned)(esi+0x84));
    CAM_R(s5, s5 * ((double)CAM_LF((unsigned)(esi+0x2C))));
    CAM_SF((unsigned)(esi+0x44), s5);
    CAM_R(s4, s4 + (s3));
    s2 = s4;
    s3 = (double)CAM_LF((unsigned)(esi+0x40));
    CAM_R(s3, s3 + ((double)CAM_LF((unsigned)(esi+0x5C))));
    s4 = (double)CAM_LF((unsigned)(esi+0x44));
    CAM_R(s4, s4 + ((double)CAM_LF((unsigned)(esi+0x60))));
    CAM_SF((unsigned)(esi+0x60), s4);
    s4 = (double)CAM_LF(0x858624);
    CAM_R(s4, s4 - ((double)CAM_LF((unsigned)(esi+0x48))));
    CAM_SF((unsigned)(esi+0x1C), s4);
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(esi+0x1C))));
    CAM_SF((unsigned)(esi+0x3C), s3);
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esi+0x1C))));
    s3 = (double)CAM_LF((unsigned)(esi+0x60));
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(esi+0x1C))));
    CAM_SF((unsigned)(esi+0x44), s3);
    s3 = (double)CAM_LF((unsigned)(esi+0x3C));
    CAM_R(s3, s3 + ((double)CAM_LF((unsigned)(esi+0x10))));
    CAM_SF((unsigned)(esi+0x10), s3);
    CAM_R(s2, s2 + (s0));
    CAM_SF((unsigned)(esi+0x14), s2);
    s2 = (double)CAM_LF((unsigned)(esi+0x44));
    CAM_R(s2, s2 + (s1));
    eax = (unsigned)(CAM_LU((unsigned)(esi+0x10)));
    CAM_SF((unsigned)(esi+0x18), s2);
    ecx = (unsigned)(CAM_LU((unsigned)(esi+0x14)));
    edi = (unsigned)((unsigned)(ebx+0x190));
    edx = (unsigned)(edi);
    CAM_SU((unsigned)(edx), eax);
    eax = (unsigned)(CAM_LU((unsigned)(esi+0x18)));
    CAM_SU((unsigned)(edx+0x4), ecx);
    CAM_SU((unsigned)(edx+0x8), eax);
}

__declspec(noinline) void __cdecl CamTwoCarsSourcesCpp(float firstZ, float firstHeight, float* p_liftedFirst, float secondZ, float secondHeight, float* p_liftedSecond, float betweenX, float* p_scaledX, float firstX, float* p_fx, float secondX, float* p_sx, float betweenY, float firstY, float* p_fy, float secondY, float* p_sy, float betweenZ, float* p_scaledZ, float* p_fz, float* p_sz) {
    double s0 = 0.0, s1 = 0.0;
    volatile float liftedFirst = 0.0f;
    volatile float liftedSecond = 0.0f;
    volatile float scaledX = 0.0f;
    volatile float fx = 0.0f;
    volatile float sx = 0.0f;
    volatile float fy = 0.0f;
    volatile float sy = 0.0f;
    volatile float scaledZ = 0.0f;
    volatile float fz = 0.0f;
    volatile float sz = 0.0f;
    s0 = (double)firstZ;
    CAM_R(s0, s0 + ((double)firstHeight));
    CAM_R(s0, s0 + ((double)CAM_LF(0x858624)));
    liftedFirst = (float)s0;
    s0 = (double)secondZ;
    CAM_R(s0, s0 + ((double)secondHeight));
    CAM_R(s0, s0 + ((double)CAM_LF(0x858624)));
    liftedSecond = (float)s0;
    s0 = (double)betweenX;
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B44)));
    scaledX = (float)s0;
    s0 = (double)firstX;
    CAM_R(s0, s0 - ((double)scaledX));
    fx = (float)s0;
    s0 = (double)scaledX;
    CAM_R(s0, s0 + ((double)secondX));
    sx = (float)s0;
    s0 = (double)betweenY;
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B44)));
    s1 = (double)firstY;
    CAM_R(s1, s1 - (s0));
    fy = (float)s1;
    CAM_R(s0, s0 + ((double)secondY));
    sy = (float)s0;
    s0 = (double)betweenZ;
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B44)));
    scaledZ = (float)s0;
    CAM_R(s0, ((double)liftedFirst) - s0);
    fz = (float)s0;
    s0 = (double)scaledZ;
    CAM_R(s0, s0 + ((double)liftedSecond));
    sz = (float)s0;
    *p_liftedFirst = liftedFirst;
    *p_liftedSecond = liftedSecond;
    *p_scaledX = scaledX;
    *p_fx = fx;
    *p_sx = sx;
    *p_fy = fy;
    *p_sy = sy;
    *p_scaledZ = scaledZ;
    *p_fz = fz;
    *p_sz = sz;
}

__declspec(noinline) void __cdecl CamEaseCpp(float ratio, float degrees180, float degrees270, float radians, float one, float half, float to, float from, float* p_value) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0;
    volatile float value = 0.0f;
    s0 = (double)ratio;
    CAM_R(s0, s0 * ((double)degrees180));
    CAM_R(s0, ((double)degrees270) - s0);
    CAM_R(s0, s0 * ((double)radians));
    a0 = s0;
    CAM_R(s0, (sin(a0)) + ((double)one));
    CAM_R(s0, s0 * ((double)half));
    s1 = (double)to;
    CAM_R(s1, s1 - ((double)from));
    CAM_R(s0, s0 * (s1));
    CAM_R(s0, s0 + ((double)from));
    value = (float)s0;
    *p_value = value;
}

__declspec(noinline) void __cdecl CamBirdyLampHeightCpp(int randomValue, float randomScale, float height, float one, float pointZ, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)((int)randomValue);
    CAM_R(s0, s0 * ((double)randomScale));
    s1 = (double)height;
    CAM_R(s1, s1 - ((double)one));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 + ((double)one));
    CAM_R(s1, ((double)pointZ) - s1);
    result = (float)s1;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamManFovCpp(unsigned target, unsigned source, float distanceScale, float zero, float one, float degrees180, float degrees270, float radians, float half, float final, float initial, float* p_fov, float progress, float timeScale, float early) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, edx = 0;
    volatile float fov = *p_fov;
    eax = (unsigned)(target);
    edx = (unsigned)(source);
    s0 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edx))));
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edx+0x4))));
    s2 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(edx+0x8))));
    s3 = s0;
    CAM_R(s0, s0 * (s3));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    CAM_R(s0, s0 + (s3));
    s3 = s1;
    CAM_R(s3, s3 * (s1));
    CAM_R(s0, s0 + (s3));
    { double t_ = s2; s2 = s0; s0 = t_; }
    CAM_R(s2, sqrt(s2));
    s0 = s2;
    CAM_R(s0, s0 / ((double)distanceScale));
    cc = CamX87Status(s0, (double)zero);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_manDistanceUpper;
    s0 = (double)zero;
    goto cam_manDistanceEase;
cam_manDistanceUpper:;
    cc = CamX87Status(s0, (double)one);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_manDistanceEase;
    s0 = (double)one;
cam_manDistanceEase:;
    CAM_R(s0, s0 * ((double)degrees180));
    CAM_R(s0, ((double)degrees270) - s0);
    CAM_R(s0, s0 * ((double)radians));
    a0 = s0;
    CAM_R(s0, (sin(a0)) + ((double)one));
    CAM_R(s0, s0 * ((double)half));
    s1 = (double)final;
    CAM_R(s1, s1 - ((double)initial));
    CAM_R(s0, s0 * (s1));
    CAM_R(s0, s0 + ((double)initial));
    fov = (float)s0;
    s0 = (double)progress;
    cc = CamX87Status(s0, (double)timeScale);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_manFovDone;
    s0 = (double)progress;
    CAM_R(s0, s0 / ((double)timeScale));
    cc = CamX87Status(s0, (double)zero);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_manTimeUpper;
    s0 = (double)zero;
    goto cam_manTimeEase;
cam_manTimeUpper:;
    cc = CamX87Status(s0, (double)one);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_manTimeEase;
    s0 = (double)one;
cam_manTimeEase:;
    CAM_R(s0, s0 * ((double)degrees180));
    CAM_R(s0, ((double)degrees270) - s0);
    CAM_R(s0, s0 * ((double)radians));
    a1 = s0;
    CAM_R(s0, (sin(a1)) + ((double)one));
    CAM_R(s0, s0 * ((double)half));
    s1 = (double)fov;
    CAM_R(s1, s1 - ((double)early));
    CAM_R(s0, s0 * (s1));
    CAM_R(s0, s0 + ((double)early));
    fov = (float)s0;
cam_manFovDone:;
    *p_fov = fov;
}

__declspec(noinline) void __cdecl CamHeliInitialFovCpp(float one, float duration, float progress, float degrees180, float degrees270, float radians, float half, float to, float from, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)one;
    CAM_R(s0, s0 / ((double)duration));
    CAM_R(s0, s0 * ((double)progress));
    CAM_R(s0, s0 * ((double)degrees180));
    CAM_R(s0, ((double)degrees270) - s0);
    CAM_R(s0, s0 * ((double)radians));
    a0 = s0;
    CAM_R(s0, (sin(a0)) + ((double)one));
    CAM_R(s0, s0 * ((double)half));
    s1 = (double)to;
    CAM_R(s1, s1 - ((double)from));
    CAM_R(s0, s0 * (s1));
    CAM_R(s0, s0 + ((double)from));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamHeliReturnFovCpp(int start, int now, int end, float zero, float one, float degrees, float phase, float radians, float half, float to, float from, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    double a0 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float result = *p_result;
    s0 = (double)((int)start);
    s1 = (double)((int)now);
    CAM_R(s1, s1 - (s0));
    s2 = (double)((int)end);
    CAM_R(s2, s2 - (s0));
    CAM_R(s1, s1 / (s2));
    s0 = s1;
    cc = CamX87Status(s0, (double)zero);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_heliReturnUpper355;
    s0 = (double)zero;
    goto cam_heliReturnEase355;
cam_heliReturnUpper355:;
    cc = CamX87Status(s0, (double)one);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_heliReturnEase355;
    s0 = (double)one;
cam_heliReturnEase355:;
    CAM_R(s0, s0 * ((double)degrees));
    CAM_R(s0, ((double)phase) - s0);
    CAM_R(s0, s0 * ((double)radians));
    a0 = s0;
    CAM_R(s0, (sin(a0)) + ((double)one));
    CAM_R(s0, s0 * ((double)half));
    s1 = (double)to;
    CAM_R(s1, s1 - ((double)from));
    CAM_R(s0, s0 * (s1));
    CAM_R(s0, s0 + ((double)from));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamHeliDistanceFovCpp(float to, int initial, float one, float duration, float progress, float degrees, float phase, float radians, float half, float from, float zero, unsigned camera, unsigned dest, float* p_distance, float nearDistance, float farDistance, float reduction, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0, s5 = 0.0, s6 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, edx = 0;
    volatile float distance = *p_distance;
    volatile float result = *p_result;
    s0 = (double)to;
    fa = (unsigned)((unsigned)initial);
    fb = (unsigned)(0x0u);
    if (fa == fb) goto cam_heliDistanceMeasure355;
    s0 = (double)one;
    CAM_R(s0, s0 / ((double)duration));
    CAM_R(s0, s0 * ((double)progress));
    CAM_R(s0, s0 * ((double)degrees));
    CAM_R(s0, ((double)phase) - s0);
    CAM_R(s0, s0 * ((double)radians));
    a0 = s0;
    CAM_R(s0, (sin(a0)) + ((double)one));
    CAM_R(s0, s0 * ((double)half));
    s1 = (double)to;
    CAM_R(s1, s1 - ((double)from));
    CAM_R(s0, s0 * (s1));
    CAM_R(s0, s0 + ((double)from));
cam_heliDistanceMeasure355:;
    s1 = (double)zero;
    eax = (unsigned)(camera);
    edx = (unsigned)(dest);
    s2 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(edx))));
    s3 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s3, s3 - ((double)CAM_LF((unsigned)(edx+0x4))));
    s4 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s4, s4 - ((double)CAM_LF((unsigned)(edx+0x8))));
    s5 = s4;
    CAM_R(s5, s5 * (s4));
    s6 = s3;
    CAM_R(s6, s6 * (s3));
    CAM_R(s5, s5 + (s6));
    s6 = s2;
    CAM_R(s6, s6 * (s2));
    CAM_R(s5, s5 + (s6));
    CAM_R(s5, sqrt(s5));
    s2 = s5;
    distance = (float)s2;
    cc = CamX87Status(s2, (double)nearDistance);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_heliDistanceSubtract355;
    s1 = (double)distance;
    CAM_R(s1, s1 - ((double)nearDistance));
    s2 = (double)farDistance;
    CAM_R(s2, s2 - ((double)nearDistance));
    CAM_R(s1, s1 / (s2));
    cc = CamX87Status(s1, (double)zero);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_heliDistanceUpper355;
    s1 = (double)zero;
    goto cam_heliDistanceEase355;
cam_heliDistanceUpper355:;
    cc = CamX87Status(s1, (double)one);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_heliDistanceEase355;
    s1 = (double)one;
cam_heliDistanceEase355:;
    CAM_R(s1, s1 * ((double)degrees));
    CAM_R(s1, ((double)phase) - s1);
    CAM_R(s1, s1 * ((double)radians));
    a1 = s1;
    CAM_R(s1, (sin(a1)) + ((double)one));
    CAM_R(s1, s1 * ((double)half));
    CAM_R(s1, s1 * ((double)reduction));
cam_heliDistanceSubtract355:;
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 - (s0));
    result = (float)s1;
    *p_distance = distance;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamPlaneOrbitWaveCpp(int start, int now, int end, int cycles, float degrees, float radians, float componentX, float* p_resultX, float componentY, float* p_resultY, float componentZ, float* p_resultZ) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    double a0 = 0.0;
    volatile float resultX = 0.0f;
    volatile float resultY = 0.0f;
    volatile float resultZ = 0.0f;
    s0 = (double)((int)start);
    s1 = (double)((int)now);
    CAM_R(s1, s1 - (s0));
    s2 = (double)((int)end);
    CAM_R(s2, s2 - (s0));
    CAM_R(s1, s1 / (s2));
    s2 = (double)((int)cycles);
    CAM_R(s1, s1 * (s2));
    CAM_R(s1, s1 * ((double)degrees));
    CAM_R(s1, s1 * ((double)radians));
    a0 = s1;
    s1 = (double)componentX;
    CAM_R(s1, s1 * ((sin(a0))));
    resultX = (float)s1;
    s1 = (double)componentY;
    CAM_R(s1, s1 * ((sin(a0))));
    resultY = (float)s1;
    CAM_R(s0, (sin(a0)) * ((double)componentZ));
    resultZ = (float)s0;
    *p_resultX = resultX;
    *p_resultY = resultY;
    *p_resultZ = resultZ;
}

__declspec(noinline) void __cdecl CamPlaneSpotterRandomCpp(int randomValue, float randomScale, float range, float halfRange, float origin, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)((int)randomValue);
    CAM_R(s0, s0 * ((double)randomScale));
    s1 = (double)range;
    CAM_R(s1, s1 - ((double)halfRange));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 + ((double)halfRange));
    CAM_R(s1, s1 + ((double)origin));
    result = (float)s1;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFlyByUpCpp(float roll, float radians, float halfPi, float* p_upX, float* p_upZ) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    volatile float upX = 0.0f;
    volatile float upZ = 0.0f;
    s0 = (double)roll;
    CAM_R(s0, s0 * ((double)radians));
    CAM_R(s0, s0 + ((double)halfPi));
    s1 = s0;
    a0 = s1;
    upX = (float)(cos(a0));
    a1 = s0;
    upZ = (float)(sin(a1));
    *p_upX = upX;
    *p_upZ = upZ;
}

__declspec(noinline) void __cdecl CamFollowCarBetaBlendCpp(float betaInput, float betaCorrection, float maximumSpeed, float* p_negativeMaximum, float smoothing, float previous, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float negativeMaximum = *p_negativeMaximum;
    volatile float result = *p_result;
    s0 = (double)betaInput;
    CAM_R(s0, s0 + ((double)betaCorrection));
    cc = CamX87Status(s0, (double)maximumSpeed);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_car_beta_lower355;
    s0 = (double)maximumSpeed;
    goto cam_car_beta_blend355;
cam_car_beta_lower355:;
    s1 = (double)maximumSpeed;
    s1 = -s1;
    negativeMaximum = (float)s1;
    cc = CamX87Status(s0, (double)negativeMaximum);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_car_beta_blend355;
    s0 = (double)negativeMaximum;
cam_car_beta_blend355:;
    s1 = (double)CAM_LF(0x858624);
    CAM_R(s1, s1 - ((double)smoothing));
    CAM_R(s0, s0 * (s1));
    s1 = (double)smoothing;
    CAM_R(s1, s1 * ((double)previous));
    CAM_R(s0, s0 + (s1));
    result = (float)s0;
    s0 = CamFabs(s0);
    cc = CamX87Status(s0, (double)CAM_LF(0x8CCEBC));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_car_beta_done355;
    CAM_SETBITS(result, 0x0u);
cam_car_beta_done355:;
    *p_negativeMaximum = negativeMaximum;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowCarHistoryCpp(float radius, float frontX, float frontY, float frontZ, float* p_scaledZ, float focalX, float* p_outputX, float focalY, float* p_outputY, float focalZ, float* p_outputZ) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    volatile float scaledZ = 0.0f;
    volatile float outputX = 0.0f;
    volatile float outputY = 0.0f;
    volatile float outputZ = 0.0f;
    s0 = (double)radius;
    CAM_R(s0, s0 * ((double)frontX));
    s1 = (double)radius;
    CAM_R(s1, s1 * ((double)frontY));
    s2 = (double)radius;
    CAM_R(s2, s2 * ((double)frontZ));
    scaledZ = (float)s2;
    s2 = (double)focalX;
    CAM_R(s2, s2 - (s0));
    outputX = (float)s2;
    s2 = (double)focalY;
    CAM_R(s2, s2 - (s1));
    outputY = (float)s2;
    s0 = (double)focalZ;
    CAM_R(s0, s0 - ((double)scaledZ));
    outputZ = (float)s0;
    *p_scaledZ = scaledZ;
    *p_outputX = outputX;
    *p_outputY = outputY;
    *p_outputZ = outputZ;
}

__declspec(noinline) void __cdecl CamFollowCarFinalHistoryCpp(unsigned cam, unsigned scratch) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0;
    unsigned eax = 0, ecx = 0, edx = 0, esi = 0, edi = 0;
    esi = (unsigned)(cam);
    edi = (unsigned)(scratch);
    eax = (unsigned)((unsigned)(esi+0x1D8));
    s0 = (double)CAM_LF((unsigned)(esi+0xBC));
    a0 = s0;
    CAM_R(s0, (cos(a0)) * ((double)CAM_LF((unsigned)(edi+0x8C))));
    s0 = -s0;
    CAM_SF((unsigned)(edi+0x6C), s0);
    s1 = (double)CAM_LF((unsigned)(esi+0xBC));
    a1 = s1;
    CAM_R(s1, (sin(a1)) * ((double)CAM_LF((unsigned)(edi+0x8C))));
    s1 = -s1;
    s2 = s1;
    s3 = (double)CAM_LF((unsigned)(edi+0x14));
    a2 = s3;
    CAM_SF((unsigned)(edi+0x8C), (sin(a2)));
    s4 = (double)CAM_LF((unsigned)(edi+0x6C));
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(edi+0x2C))));
    CAM_SF((unsigned)(edi+0x6C), s4);
    s3 = sin(a2);
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(edi+0x2C))));
    CAM_SF((unsigned)(edi+0x70), s3);
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(edi+0x2C))));
    s3 = (double)CAM_LF((unsigned)(edi+0x48));
    CAM_R(s3, s3 - ((double)CAM_LF((unsigned)(edi+0x6C))));
    CAM_SF((unsigned)(edi+0x54), s3);
    ecx = (unsigned)(CAM_LU((unsigned)(edi+0x54)));
    s3 = (double)CAM_LF((unsigned)(edi+0x4C));
    CAM_SU((unsigned)(eax), ecx);
    CAM_R(s3, s3 - ((double)CAM_LF((unsigned)(edi+0x70))));
    CAM_SF((unsigned)(edi+0x58), s3);
    s3 = (double)CAM_LF((unsigned)(edi+0x50));
    edx = (unsigned)(CAM_LU((unsigned)(edi+0x58)));
    CAM_R(s3, s3 - (s2));
    CAM_SU((unsigned)(eax+0x4), edx);
    CAM_SF((unsigned)(edi+0x5C), s3);
    ecx = (unsigned)(CAM_LU((unsigned)(edi+0x5C)));
    CAM_SU((unsigned)(eax+0x8), ecx);
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_SF((unsigned)(edi+0x6C), s1);
    s1 = (double)CAM_LF((unsigned)(edi+0x8C));
    s2 = (double)CAM_LF((unsigned)(edi+0x6C));
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(edi+0x7C))));
    CAM_SF((unsigned)(edi+0x6C), s2);
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(edi+0x7C))));
    CAM_SF((unsigned)(edi+0x70), s1);
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(edi+0x7C))));
    s1 = (double)CAM_LF((unsigned)(edi+0x48));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edi+0x6C))));
    CAM_SF((unsigned)(edi+0x54), s1);
    edx = (unsigned)(CAM_LU((unsigned)(edi+0x54)));
    s1 = (double)CAM_LF((unsigned)(edi+0x4C));
    CAM_SU((unsigned)(esi+0x1E4), edx);
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edi+0x70))));
    CAM_SF((unsigned)(edi+0x58), s1);
    s1 = (double)CAM_LF((unsigned)(edi+0x50));
    eax = (unsigned)(CAM_LU((unsigned)(edi+0x58)));
    CAM_R(s1, s1 - (s0));
    CAM_SU((unsigned)(esi+0x1E8), eax);
    eax = (unsigned)(CAM_LU((unsigned)(edi+0x24)));
    CAM_SF((unsigned)(edi+0x5C), s1);
    ecx = (unsigned)(CAM_LU((unsigned)(edi+0x5C)));
    CAM_SU((unsigned)(esi+0x1EC), ecx);
}

__declspec(noinline) void __cdecl CamFollowCarBetaCorrectionCpp(unsigned cam, unsigned vehicle, unsigned scratch, unsigned front) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0, s5 = 0.0, s6 = 0.0, s7 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, ebx = 0, ecx = 0, esi = 0, edi = 0, ebp = 0;
    esi = (unsigned)(cam);
    edi = (unsigned)(vehicle);
    ecx = (unsigned)(scratch);
    eax = (unsigned)(front);
    ebp = (unsigned)(eax);
    s0 = (double)CAM_LF((unsigned)(ebp));
    s0 = -s0;
    s1 = (double)CAM_LF((unsigned)(esi+0x194));
    a0 = s0;
    a1 = s1;
    CAM_R(s0, (atan2(a0, a1)) - ((double)CAM_LF(0x858FE4)));
    cc = CamX87Status(s0, (double)CAM_LF(0x858CC0));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_follow_car_beta_00524E56;
    CAM_R(s0, s0 + ((double)CAM_LF(0x858CBC)));
cam_follow_car_beta_00524E56:;
    s1 = (double)CAM_LF((unsigned)(edi+0x48));
    s2 = (double)CAM_LF((unsigned)(edi+0x44));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    s4 = s1;
    CAM_R(s4, s4 * (s1));
    CAM_R(s3, s3 + (s4));
    CAM_R(s3, sqrt(s3));
    s1 = s3;
    cc = CamX87Status(s1, (double)CAM_LF(0x858B38));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_follow_car_beta_00524E8B;
    s1 = (double)CAM_LF((unsigned)(edi+0x44));
    s1 = -s1;
    s2 = (double)CAM_LF((unsigned)(edi+0x48));
    a2 = s1;
    a3 = s2;
    CAM_R(s1, (atan2(a2, a3)) - ((double)CAM_LF(0x858FE4)));
    goto cam_follow_car_beta_00524E8D;
cam_follow_car_beta_00524E8B:;
    s1 = s0;
cam_follow_car_beta_00524E8D:;
    s2 = s0;
    CAM_R(s2, s2 + ((double)CAM_LF(0x858CB8)));
    s3 = s1;
    cc = CamX87Status(s3, s2);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_follow_car_beta_00524EA8;
    CAM_R(s1, s1 - ((double)CAM_LF(0x858CBC)));
    goto cam_follow_car_beta_00524EC1;
cam_follow_car_beta_00524EA8:;
    s2 = s0;
    CAM_R(s2, s2 - ((double)CAM_LF(0x858CB8)));
    s3 = s1;
    cc = CamX87Status(s3, s2);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_follow_car_beta_00524EC1;
    CAM_R(s1, s1 + ((double)CAM_LF(0x858CBC)));
cam_follow_car_beta_00524EC1:;
    ebx = (unsigned)(CAM_LU((unsigned)(ecx+0x34)));
    s2 = (double)CAM_LF(0xB7CB5C);
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(ebx+0x8CC628))));
    s3 = (double)CAM_LF(0xB7CB5C);
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(ebx+0x8CC62C))));
    CAM_SF((unsigned)(ecx+0x18), s3);
    s3 = (double)CAM_LF((unsigned)(edi+0x4C));
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(ebp+0x8))));
    s4 = (double)CAM_LF((unsigned)(edi+0x48));
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(ebp+0x4))));
    CAM_R(s3, s3 + (s4));
    s4 = (double)CAM_LF((unsigned)(edi+0x44));
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(ebp))));
    CAM_R(s3, s3 + (s4));
    s4 = s3;
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(ebp))));
    s5 = s3;
    CAM_R(s5, s5 * ((double)CAM_LF((unsigned)(ebp+0x4))));
    CAM_SF((unsigned)(ecx+0x70), s5);
    { double t_ = s4; s4 = s3; s3 = t_; }
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(ebp+0x8))));
    CAM_SF((unsigned)(ecx+0x74), s4);
    CAM_R(s3, ((double)CAM_LF((unsigned)(edi+0x44))) - s3);
    s4 = (double)CAM_LF((unsigned)(edi+0x48));
    CAM_R(s4, s4 - ((double)CAM_LF((unsigned)(ecx+0x70))));
    s5 = (double)CAM_LF((unsigned)(edi+0x4C));
    CAM_R(s5, s5 - ((double)CAM_LF((unsigned)(ecx+0x74))));
    s6 = s3;
    CAM_R(s6, s6 * (s3));
    s7 = s5;
    CAM_R(s7, s7 * (s5));
    CAM_R(s6, s6 + (s7));
    s7 = s4;
    CAM_R(s7, s7 * (s4));
    CAM_R(s6, s6 + (s7));
    CAM_R(s6, sqrt(s6));
    CAM_SF((unsigned)(ecx+0x8C), s6);
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(ecx+0x8C))));
    s3 = (double)CAM_LF(0x858624);
    cc = CamX87Status(s3, s2);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_follow_car_beta_00524F5C;
    s2 = (double)CAM_LF(0x858624);
cam_follow_car_beta_00524F5C:;
    { double t_ = s2; s2 = s1; s1 = t_; }
    CAM_R(s2, s2 - (s0));
    CAM_R(s1, s1 * (s2));
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(ecx+0x18)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_follow_car_beta_00524F75;
    s1 = (double)CAM_LF((unsigned)(ecx+0x18));
    goto cam_follow_car_beta_00524F99;
cam_follow_car_beta_00524F75:;
    s2 = (double)CAM_LF((unsigned)(ecx+0x18));
    s2 = -s2;
    CAM_SF((unsigned)(ecx+0x8C), s2);
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(ecx+0x8C)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_follow_car_beta_00524F99;
    s1 = (double)CAM_LF((unsigned)(ecx+0x8C));
cam_follow_car_beta_00524F99:;
    CAM_R(s0, s0 + (s1));
    s1 = (double)CAM_LF((unsigned)(esi+0xBC));
    CAM_R(s1, s1 + ((double)CAM_LF(0x858CB8)));
    s2 = s0;
    cc = CamX87Status(s2, s1);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_follow_car_beta_00524FBA;
    CAM_R(s0, s0 - ((double)CAM_LF(0x858CBC)));
    goto cam_follow_car_beta_00524FD7;
cam_follow_car_beta_00524FBA:;
    s1 = (double)CAM_LF((unsigned)(esi+0xBC));
    CAM_R(s1, s1 - ((double)CAM_LF(0x858CB8)));
    s2 = s0;
    cc = CamX87Status(s2, s1);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_follow_car_beta_00524FD7;
    CAM_R(s0, s0 + ((double)CAM_LF(0x858CBC)));
cam_follow_car_beta_00524FD7:;
    s1 = (double)CAM_LF(0x858624);
    cc = CamX87Status(s1, (double)CAM_LF(0xB7CB5C));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_follow_car_beta_00524FF2;
    s1 = (double)CAM_LF(0x858624);
    goto cam_follow_car_beta_00524FF8;
cam_follow_car_beta_00524FF2:;
    s1 = (double)CAM_LF(0xB7CB5C);
cam_follow_car_beta_00524FF8:;
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(esi+0xBC))));
    CAM_R(s1, s1 / (s0));
    CAM_SF((unsigned)(ecx+0x44), s1);
    ebp = (unsigned)(ebp);
}

__declspec(noinline) void __cdecl CamFollowCarActualDistanceCpp(unsigned origin, unsigned previous, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    unsigned eax = 0, edx = 0;
    volatile float result = 0.0f;
    eax = (unsigned)(origin);
    edx = (unsigned)(previous);
    s0 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edx))));
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edx+0x4))));
    s2 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(edx+0x8))));
    s3 = s0;
    CAM_R(s0, s0 * (s3));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    CAM_R(s0, s0 + (s3));
    s3 = s1;
    CAM_R(s3, s3 * (s1));
    CAM_R(s0, s0 + (s3));
    { double t_ = s2; s2 = s0; s0 = t_; }
    CAM_R(s2, sqrt(s2));
    s0 = s2;
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowCarAutoAlphaCpp(float base, float step, float wanted, float previous, float* p_result, float maximum) {
    double s0 = 0.0, s1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float result = *p_result;
    s0 = (double)base;
    s1 = (double)step;
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    CAM_R(s0, ((double)CAM_LF(0x858624)) - s0);
    s1 = (double)wanted;
    CAM_R(s1, s1 - ((double)previous));
    CAM_R(s0, s0 * (s1));
    result = (float)s0;
    cc = CamX87Status(s0, (double)maximum);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_car_auto_lower355;
    eax = (unsigned)(CAM_BITS(maximum));
    CAM_SETBITS(result, eax);
    goto cam_car_auto_done355;
cam_car_auto_lower355:;
    s0 = (double)maximum;
    s0 = -s0;
    s1 = (double)result;
    cc = CamX87Status(s1, s0);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_car_auto_keep355;
    result = (float)s0;
    goto cam_car_auto_done355;
cam_car_auto_keep355:;
cam_car_auto_done355:;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowCarAlphaComplementCpp(float smoothing, float* p_complement) {
    double s0 = 0.0;
    volatile float complement = 0.0f;
    s0 = (double)CAM_LF(0x858624);
    CAM_R(s0, s0 - ((double)smoothing));
    complement = (float)s0;
    *p_complement = complement;
}

__declspec(noinline) void __cdecl CamFollowCarAlphaBlendCpp(unsigned cam, unsigned scratch) {
    double s0 = 0.0, s1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, esi = 0, edi = 0;
    esi = (unsigned)(cam);
    edi = (unsigned)(scratch);
    s0 = (double)CAM_LF((unsigned)(edi+0x8C));
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(edi+0x10))));
    s1 = (double)CAM_LF((unsigned)(edi+0x3C));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esi+0xB0))));
    CAM_R(s0, s0 + (s1));
    CAM_SF((unsigned)(edi+0x8C), s0);
    CAM_SF((unsigned)(esi+0xB0), s0);
    s0 = (double)CAM_LF((unsigned)(edi+0x10));
    cc = CamX87Status(s0, (double)CAM_LF(0x858B50));
    s0 = (double)CAM_LF((unsigned)(edi+0x18));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_car_alpha_005259E0;
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B8C)));
cam_car_alpha_005259E0:;
    s1 = (double)CAM_LF((unsigned)(edi+0x8C));
    cc = CamX87Status(s1, s0);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_car_alpha_005259F8;
    CAM_SF((unsigned)(esi+0xB0), s0);
    goto cam_car_alpha_00525A14;
cam_car_alpha_005259F8:;
    s0 = -s0;
    s1 = (double)CAM_LF((unsigned)(edi+0x8C));
    cc = CamX87Status(s1, s0);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_car_alpha_00525A12;
    CAM_SF((unsigned)(esi+0xB0), s0);
    goto cam_car_alpha_00525A14;
cam_car_alpha_00525A12:;
cam_car_alpha_00525A14:;
    s0 = (double)CAM_LF((unsigned)(esi+0xB0));
    s0 = CamFabs(s0);
    cc = CamX87Status(s0, (double)CAM_LF(0x8CCEB4));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_car_alpha_end;
    CAM_SU((unsigned)(esi+0xB0), 0x0u);
cam_car_alpha_end:;
}

__declspec(noinline) void __cdecl CamMouseFinalDistanceCpp(unsigned cam, unsigned source, unsigned scratch) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0;
    edi = (unsigned)(cam);
    ebx = (unsigned)(source);
    esi = (unsigned)(scratch);
    s0 = (double)CAM_LF((unsigned)(esi+0x28));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(ebx))));
    s1 = (double)CAM_LF((unsigned)(esi+0x2C));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(ebx+0x4))));
    s2 = (double)CAM_LF((unsigned)(esi+0x30));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(ebx+0x8))));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    s4 = s1;
    CAM_R(s4, s4 * (s1));
    CAM_R(s3, s3 + (s4));
    s4 = s0;
    CAM_R(s4, s4 * (s0));
    CAM_R(s3, s3 + (s4));
    CAM_R(s3, sqrt(s3));
    s0 = s3;
    CAM_SF((unsigned)(esi+0x10), s0);
    cc = CamX87Status(s0, (double)CAM_LF((unsigned)(edi+0xC4)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_mouse_final_005103C4;
    eax = (unsigned)(CAM_LU((unsigned)(esi+0x10)));
    CAM_SU((unsigned)(edi+0xC4), eax);
    goto cam_mouse_final_end;
cam_mouse_final_005103C4:;
    s0 = (double)CAM_LF(0x862F14);
    s1 = (double)CAM_LF(0xB7CB5C);
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    s1 = (double)CAM_LF(0x858624);
    CAM_R(s1, s1 - (s0));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esi+0x10))));
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(edi+0xC4))));
    CAM_R(s0, s0 + (s1));
    CAM_SF((unsigned)(edi+0xC4), s0);
    s1 = (double)CAM_LF((unsigned)(esi+0x10));
    cc = CamX87Status(s1, (double)CAM_LF(0x858C28));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_mouse_final_0051049C;
    s1 = (double)CAM_LF((unsigned)(ebx));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(esi+0x28))));
    s2 = (double)CAM_LF((unsigned)(ebx+0x4));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(esi+0x2C))));
    s3 = (double)CAM_LF((unsigned)(ebx+0x8));
    CAM_R(s3, s3 - ((double)CAM_LF((unsigned)(esi+0x30))));
    CAM_SF((unsigned)(esi+0x9C), s3);
    s3 = s0;
    CAM_SF((unsigned)(esi+0x14), s3);
    { double t_ = s2; s2 = s1; s1 = t_; }
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esi+0x14))));
    CAM_SF((unsigned)(esi+0xA0), s2);
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esi+0x14))));
    s2 = (double)CAM_LF((unsigned)(esi+0x9C));
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esi+0x14))));
    s3 = (double)CAM_LF(0x858624);
    CAM_R(s3, s3 / ((double)CAM_LF((unsigned)(esi+0x10))));
    s4 = (double)CAM_LF((unsigned)(esi+0xA0));
    CAM_R(s4, s4 * (s3));
    CAM_SF((unsigned)(esi+0x44), s4);
    { double t_ = s3; s3 = s1; s1 = t_; }
    CAM_R(s3, s3 * (s1));
    CAM_SF((unsigned)(esi+0x48), s3);
    CAM_R(s2, s2 * (s1));
    { double t_ = s2; s2 = s1; s1 = t_; }
    s2 = (double)CAM_LF((unsigned)(esi+0x44));
    CAM_R(s2, s2 + ((double)CAM_LF((unsigned)(esi+0x28))));
    CAM_SF((unsigned)(esi+0x34), s2);
    ecx = (unsigned)(CAM_LU((unsigned)(esi+0x34)));
    s2 = (double)CAM_LF((unsigned)(esi+0x48));
    CAM_SU((unsigned)(ebx), ecx);
    CAM_R(s2, s2 + ((double)CAM_LF((unsigned)(esi+0x2C))));
    CAM_SF((unsigned)(esi+0x38), s2);
    edx = (unsigned)(CAM_LU((unsigned)(esi+0x38)));
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(esi+0x30))));
    CAM_SU((unsigned)(ebx+0x4), edx);
    CAM_SF((unsigned)(esi+0x3C), s1);
    eax = (unsigned)(CAM_LU((unsigned)(esi+0x3C)));
    CAM_SU((unsigned)(ebx+0x8), eax);
cam_mouse_final_0051049C:;
    CAM_R(s0, s0 - ((double)CAM_LF(0x8CC38C)));
    ecx = (unsigned)(CAM_LU(0xC1703C));
    s1 = (double)CAM_LF((unsigned)(ecx+0x80));
    s2 = s0;
    cc = CamX87Status(s2, s1);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_mouse_final_005104F4;
    cc = CamX87Status(s0, (double)CAM_LF(0x858B1C));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_mouse_final_005104DA;
    CAM_SF((unsigned)(esi+0x10), s0);
    edx = (unsigned)(CAM_LU((unsigned)(esi+0x10)));
    eax = (unsigned)(0x7EE1D0u);
    ((void(__cdecl*)(void*, unsigned))(uintptr_t)0x7EE1D0)((void*)(uintptr_t)(ecx), (unsigned)(edx));
    goto cam_mouse_final_end;
cam_mouse_final_005104DA:;
    CAM_SU((unsigned)(esi+0x10), 0x3DCCCCCDu);
    edx = (unsigned)(CAM_LU((unsigned)(esi+0x10)));
    eax = (unsigned)(0x7EE1D0u);
    ((void(__cdecl*)(void*, unsigned))(uintptr_t)0x7EE1D0)((void*)(uintptr_t)(ecx), (unsigned)(edx));
    goto cam_mouse_final_end;
cam_mouse_final_005104F4:;
cam_mouse_final_end:;
}

__declspec(noinline) void __cdecl CamMouseChangesStickCpp(float inputY, float fov, float scaleFactor, float stickBeta, float step, float stickScale, float inputX, float* p_betaResult, float stickAlpha, float* p_alphaResult) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    volatile float betaResult = 0.0f;
    volatile float alphaResult = 0.0f;
    s0 = (double)inputY;
    s1 = (double)fov;
    CAM_R(s1, s1 * ((double)scaleFactor));
    s2 = (double)stickBeta;
    CAM_R(s2, s2 * (s1));
    CAM_R(s2, s2 * ((double)step));
    CAM_R(s2, s2 * ((double)stickScale));
    CAM_R(s2, s2 * ((double)inputX));
    betaResult = (float)s2;
    CAM_R(s1, s1 * ((double)stickAlpha));
    CAM_R(s1, s1 * ((double)step));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 * ((double)stickScale));
    alphaResult = (float)s1;
    *p_betaResult = betaResult;
    *p_alphaResult = alphaResult;
}

__declspec(noinline) void __cdecl CamMouseChangesMouseCpp(float inputX, float mouseBeta, float inputY, float mouseAlpha, float fov, float scaleFactor, float betaSensitivity, float* p_betaResult, float alphaSensitivity, float* p_alphaResult) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    volatile float betaResult = 0.0f;
    volatile float alphaResult = 0.0f;
    s0 = (double)inputX;
    CAM_R(s0, s0 * ((double)mouseBeta));
    s1 = (double)inputY;
    CAM_R(s1, s1 * ((double)mouseAlpha));
    s2 = (double)fov;
    CAM_R(s2, s2 * ((double)scaleFactor));
    s3 = (double)betaSensitivity;
    CAM_R(s3, s3 * (s2));
    CAM_R(s3, s3 * (s0));
    betaResult = (float)s3;
    CAM_R(s2, s2 * (s1));
    CAM_R(s2, s2 * ((double)alphaSensitivity));
    alphaResult = (float)s2;
    *p_betaResult = betaResult;
    *p_alphaResult = alphaResult;
}

__declspec(noinline) void __cdecl CamMouseRepeatedRadiusCpp(float fov, float radians, float half, float aspect, float nearPlane, float factor, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)fov;
    CAM_R(s0, s0 * ((double)radians));
    CAM_R(s0, s0 * ((double)half));
    a0 = s0;
    s1 = 1.0;
    CAM_R(s0, (tan(a0)) * ((double)aspect));
    CAM_R(s0, s0 * ((double)nearPlane));
    CAM_R(s0, s0 * ((double)factor));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamMousePerpendicularDistanceCpp(float one, float radiusFactor, float* p_inverse, unsigned collision, unsigned source, unsigned front, float* p_zDifference, float* p_yProjection, float* p_zProjection, float* p_xPerpendicular, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
    unsigned eax = 0, ecx = 0, edx = 0;
    volatile float inverse = 0.0f;
    volatile float zDifference = 0.0f;
    volatile float yProjection = 0.0f;
    volatile float zProjection = 0.0f;
    volatile float xPerpendicular = 0.0f;
    volatile float result = 0.0f;
    s0 = (double)one;
    CAM_R(s0, s0 / ((double)radiusFactor));
    inverse = (float)s0;
    eax = (unsigned)(collision);
    edx = (unsigned)(source);
    ecx = (unsigned)(front);
    s0 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edx))));
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edx+0x4))));
    s2 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(edx+0x8))));
    zDifference = (float)s2;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(ecx+0x8))));
    s3 = s1;
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(ecx+0x4))));
    CAM_R(s2, s2 + (s3));
    s3 = s0;
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(ecx))));
    CAM_R(s2, s2 + (s3));
    s3 = s2;
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(ecx))));
    s4 = s2;
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(ecx+0x4))));
    yProjection = (float)s4;
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)CAM_LF((unsigned)(ecx+0x8))));
    zProjection = (float)s3;
    { double t_ = s2; s2 = s0; s0 = t_; }
    CAM_R(s2, s2 - (s0));
    xPerpendicular = (float)s2;
    s0 = s1;
    CAM_R(s0, s0 - ((double)yProjection));
    s1 = (double)zDifference;
    CAM_R(s1, s1 - ((double)zProjection));
    s2 = s1;
    CAM_R(s2, s2 * (s1));
    s3 = s0;
    CAM_R(s3, s3 * (s0));
    CAM_R(s2, s2 + (s3));
    s3 = (double)xPerpendicular;
    CAM_R(s3, s3 * ((double)xPerpendicular));
    CAM_R(s2, s2 + (s3));
    CAM_R(s2, sqrt(s2));
    s0 = s2;
    CAM_R(s0, s0 * ((double)inverse));
    result = (float)s0;
    *p_inverse = inverse;
    *p_zDifference = zDifference;
    *p_yProjection = yProjection;
    *p_zProjection = zProjection;
    *p_xPerpendicular = xPerpendicular;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamMouseSphereCenterCpp(unsigned front, unsigned source, unsigned result, float nearPlane, float* p_zProduct) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned eax = 0, ecx = 0, edx = 0;
    volatile float zProduct = 0.0f;
    eax = (unsigned)(front);
    edx = (unsigned)(source);
    ecx = (unsigned)(result);
    s0 = (double)nearPlane;
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(eax))));
    s1 = (double)nearPlane;
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(eax+0x4))));
    s2 = (double)nearPlane;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(eax+0x8))));
    zProduct = (float)s2;
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(edx))));
    CAM_SF((unsigned)(ecx), s1);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(edx+0x4))));
    CAM_SF((unsigned)(ecx+0x4), s0);
    s0 = (double)zProduct;
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(edx+0x8))));
    CAM_SF((unsigned)(ecx+0x8), s0);
    *p_zProduct = zProduct;
}

__declspec(noinline) void __cdecl CamMouseInitialSourceCpp(unsigned target, unsigned front, unsigned result, float distance, float* p_zProduct) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned eax = 0, ecx = 0, edx = 0;
    volatile float zProduct = 0.0f;
    eax = (unsigned)(target);
    edx = (unsigned)(front);
    ecx = (unsigned)(result);
    s0 = (double)distance;
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(edx))));
    s1 = (double)distance;
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(edx+0x4))));
    s2 = (double)distance;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(edx+0x8))));
    zProduct = (float)s2;
    s2 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s2, s2 - (s0));
    CAM_SF((unsigned)(ecx), s2);
    s2 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s2, s2 - (s1));
    CAM_SF((unsigned)(ecx+0x4), s2);
    s0 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s0, s0 - ((double)zProduct));
    CAM_SF((unsigned)(ecx+0x8), s0);
    *p_zProduct = zProduct;
}

__declspec(noinline) void __cdecl CamMouseFirstHitDistanceCpp(unsigned target, unsigned hit, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
    unsigned eax = 0, edx = 0;
    volatile float result = 0.0f;
    eax = (unsigned)(target);
    edx = (unsigned)(hit);
    s0 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edx))));
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edx+0x4))));
    s2 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(edx+0x8))));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    s4 = s1;
    CAM_R(s4, s4 * (s1));
    CAM_R(s3, s3 + (s4));
    s4 = s0;
    CAM_R(s4, s4 * (s0));
    CAM_R(s3, s3 + (s4));
    CAM_R(s3, sqrt(s3));
    s0 = s3;
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamMousePullSourceCpp(unsigned target, unsigned source, float* p_zDifference, float factor, float* p_xProduct) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned eax = 0, edx = 0;
    volatile float zDifference = 0.0f;
    volatile float xProduct = 0.0f;
    eax = (unsigned)(target);
    edx = (unsigned)(source);
    s0 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edx))));
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edx+0x4))));
    s2 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(edx+0x8))));
    zDifference = (float)s2;
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)factor));
    xProduct = (float)s1;
    CAM_R(s0, s0 * ((double)factor));
    s1 = (double)zDifference;
    CAM_R(s1, s1 * ((double)factor));
    s2 = (double)xProduct;
    CAM_R(s2, s2 + ((double)CAM_LF((unsigned)(edx))));
    CAM_SF((unsigned)(edx), s2);
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(edx+0x4))));
    CAM_SF((unsigned)(edx+0x4), s1);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(edx+0x8))));
    CAM_SF((unsigned)(edx+0x8), s0);
    *p_zDifference = zDifference;
    *p_xProduct = xProduct;
}

__declspec(noinline) void __cdecl CamMouseRecenterDeltaCpp(float orientation, float beta, float* p_result) {
    double s0 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)orientation;
    CAM_R(s0, s0 + ((double)CAM_LF(0x858CB8)));
    CAM_R(s0, s0 - ((double)beta));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowMouseDistanceCpp(float alpha, float alphaMultiplier, float* p_distance) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float distance = *p_distance;
    s0 = (double)alpha;
    s1 = 0.0;
    cc = CamX87Status(s1, s0);
    if (!((cc & 0x01) != 0)) goto cam_follow_mouse_nonpositive_alpha;
    CAM_R(s0, s0 * ((double)alphaMultiplier));
    cc = CamX87Status(s0, (double)CAM_LF(0x858FE4));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_follow_mouse_positive_bounded;
    s0 = (double)CAM_LF(0x858FE4);
    goto cam_follow_mouse_cos_distance;
cam_follow_mouse_positive_bounded:;
    s0 = (double)alpha;
    CAM_R(s0, s0 * ((double)alphaMultiplier));
    goto cam_follow_mouse_cos_distance;
cam_follow_mouse_nonpositive_alpha:;
cam_follow_mouse_cos_distance:;
    a0 = s0;
    CAM_R(s0, (cos(a0)) * ((double)CAM_LF(0x8CC7C4)));
    CAM_R(s0, s0 + ((double)CAM_LF(0x8CC7C0)));
    distance = (float)s0;
    *p_distance = distance;
}

__declspec(noinline) void __cdecl CamFollowMouseRadiusCpp(float fov, float radians, float half, float factor, float* p_radiusFactor) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0;
    volatile float radiusFactor = 0.0f;
    s0 = (double)fov;
    CAM_R(s0, s0 * ((double)radians));
    CAM_R(s0, s0 * ((double)half));
    a0 = s0;
    s1 = 1.0;
    CAM_R(s0, (tan(a0)) * ((double)CAM_LF(0xC3EFA4)));
    CAM_R(s0, s0 * ((double)factor));
    radiusFactor = (float)s0;
    *p_radiusFactor = radiusFactor;
}

__declspec(noinline) void __cdecl CamRound4ScaleCpp(int five, float value, float* p_scaled) {
    double s0 = 0.0, s1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float scaled = *p_scaled;
    s0 = (double)CAM_LD(0x859EA8);
    s1 = (double)((int)five);
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    CAM_R(s0, s0 * ((double)value));
    s1 = (double)value;
    cc = CamX87Status(s1, (double)CAM_LF(0x858B50));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_camera_round_positive;
    CAM_R(s0, s0 - ((double)CAM_LF(0x858C80)));
    goto cam_camera_round_fraction;
cam_camera_round_positive:;
    CAM_R(s0, s0 + ((double)CAM_LF(0x858C80)));
cam_camera_round_fraction:;
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B1C)));
    scaled = (float)s0;
    *p_scaled = scaled;
}

__declspec(noinline) void __cdecl CamRound4FinishCpp(float integral, int four, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned eax = 0;
    volatile float result = 0.0f;
    s0 = (double)integral;
    s1 = (double)CAM_LD(0x859EA8);
    s2 = (double)((int)four);
    eax = (unsigned)(0x822130u);
    s1 = ExeCompatiblePowCpp(s1, s2);
    CAM_R(s0, s0 / (s1));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowPedAutoAlphaCpp(float base, float step, float wanted, float previous, float* p_result, float maximum) {
    double s0 = 0.0, s1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float result = *p_result;
    s0 = (double)base;
    s1 = (double)step;
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    CAM_R(s0, ((double)CAM_LF(0x858624)) - s0);
    s1 = (double)wanted;
    CAM_R(s1, s1 - ((double)previous));
    CAM_R(s0, s0 * (s1));
    result = (float)s0;
    cc = CamX87Status(s0, (double)maximum);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_ped_auto_lower355;
    eax = (unsigned)(CAM_BITS(maximum));
    CAM_SETBITS(result, eax);
    goto cam_ped_auto_done355;
cam_ped_auto_lower355:;
    s0 = (double)maximum;
    s0 = -s0;
    s1 = (double)result;
    cc = CamX87Status(s1, s0);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_ped_auto_keep355;
    result = (float)s0;
    goto cam_ped_auto_done355;
cam_ped_auto_keep355:;
cam_ped_auto_done355:;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowPedPitchCorrectionCpp(float wantedPitch, float followWeight, float followMaximum, float* p_maximum, float taskWeight, float historyPitch, float* p_negativeMaximum, float verticalOffset, float* p_result) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float maximum = *p_maximum;
    volatile float negativeMaximum = *p_negativeMaximum;
    volatile float result = *p_result;
    s0 = (double)wantedPitch;
    s1 = (double)CAM_LF(0x8CCE84);
    CAM_R(s1, s1 * ((double)followWeight));
    s2 = (double)CAM_LF(0x858624);
    cc = CamX87Status(s2, s1);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_pedPitchWeight355;
    s1 = (double)CAM_LF(0x858624);
cam_pedPitchWeight355:;
    s2 = (double)CAM_LF(0x8CCE80);
    CAM_R(s2, s2 * ((double)followMaximum));
    maximum = (float)s2;
    s2 = (double)taskWeight;
    cc = CamX87Status(s2, (double)CAM_LF(0x858B50));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x44u);
    if (!CamParity(((fa & fb) & 0xFFu))) goto cam_pedPitchProduct355;
    CAM_R(s1, s1 * ((double)taskWeight));
    s2 = (double)maximum;
    CAM_R(s2, s2 * ((double)taskWeight));
    maximum = (float)s2;
cam_pedPitchProduct355:;
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 - ((double)historyPitch));
    CAM_R(s0, s0 * (s1));
    cc = CamX87Status(s0, (double)maximum);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_pedPitchLower355;
    s0 = (double)maximum;
    goto cam_pedPitchSum355;
cam_pedPitchLower355:;
    s1 = (double)maximum;
    s1 = -s1;
    negativeMaximum = (float)s1;
    cc = CamX87Status(s0, (double)negativeMaximum);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_pedPitchSum355;
    s0 = (double)negativeMaximum;
cam_pedPitchSum355:;
    s1 = (double)historyPitch;
    CAM_R(s1, s1 + (s0));
    CAM_R(s1, s1 - ((double)verticalOffset));
    result = (float)s1;
    *p_maximum = maximum;
    *p_negativeMaximum = negativeMaximum;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowPedDistanceCpp(unsigned origin, unsigned previous, float* p_actual, float* p_distance, float rowMinimum, float minimumDistance) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, edx = 0;
    volatile float actual = *p_actual;
    volatile float distance = *p_distance;
    eax = (unsigned)(origin);
    edx = (unsigned)(previous);
    s0 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(edx))));
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(edx+0x4))));
    s2 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s2, s2 - ((double)CAM_LF((unsigned)(edx+0x8))));
    s3 = s0;
    CAM_R(s0, s0 * (s3));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    CAM_R(s0, s0 + (s3));
    s3 = s1;
    CAM_R(s3, s3 * (s1));
    CAM_R(s0, s0 + (s3));
    { double t_ = s2; s2 = s0; s0 = t_; }
    CAM_R(s2, sqrt(s2));
    s0 = s2;
    actual = (float)s0;
    cc = CamX87Status(s0, (double)distance);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_pedDistanceDone355;
    s0 = (double)distance;
    cc = CamX87Status(s0, (double)rowMinimum);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_pedDistanceDone355;
    s0 = (double)minimumDistance;
    cc = CamX87Status(s0, (double)actual);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_pedDistanceActual355;
    eax = (unsigned)(CAM_BITS(minimumDistance));
    CAM_SETBITS(distance, eax);
    goto cam_pedDistanceDone355;
cam_pedDistanceActual355:;
    eax = (unsigned)(CAM_BITS(actual));
    CAM_SETBITS(distance, eax);
cam_pedDistanceDone355:;
    *p_actual = actual;
    *p_distance = distance;
}

__declspec(noinline) void __cdecl CamFollowPedHistoryPitchCpp(float separation, float* p_maximum, float step, float* p_historyPitch) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float maximum = *p_maximum;
    volatile float historyPitch = *p_historyPitch;
    s0 = (double)separation;
    CAM_R(s0, s0 - ((double)CAM_LF(0x8CCE90)));
    CAM_R(s0, s0 * ((double)CAM_LF(0x858F08)));
    s1 = (double)CAM_LF(0x858CB8);
    CAM_R(s1, s1 - ((double)CAM_LF(0x8CCE90)));
    CAM_R(s0, s0 / (s1));
    s1 = (double)CAM_LF(0x858624);
    cc = CamX87Status(s1, s0);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_pedHistoryMaximum355;
    s0 = (double)CAM_LF(0x858624);
cam_pedHistoryMaximum355:;
    s1 = (double)CAM_LF(0x858FE4);
    CAM_R(s1, s1 - ((double)CAM_LF(0x8CCE8C)));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, ((double)CAM_LF(0x858FE4)) - s1);
    maximum = (float)s1;
    s0 = (double)CAM_LF(0x8CCE88);
    s1 = (double)step;
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    s1 = (double)historyPitch;
    cc = CamX87Status(s1, (double)maximum);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_pedHistoryNegative355;
    s1 = (double)CAM_LF(0x858624);
    CAM_R(s1, s1 - (s0));
    CAM_R(s1, s1 * ((double)maximum));
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)historyPitch));
    CAM_R(s0, s0 + (s1));
    historyPitch = (float)s0;
    goto cam_pedHistoryDone355;
cam_pedHistoryNegative355:;
    s1 = (double)maximum;
    s1 = -s1;
    cc = CamX87Status(s1, (double)historyPitch);
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_pedHistoryUnchanged355;
    s1 = (double)historyPitch;
    CAM_R(s1, s1 * (s0));
    s2 = (double)CAM_LF(0x858624);
    CAM_R(s2, s2 - (s0));
    CAM_R(s2, s2 * ((double)maximum));
    CAM_R(s1, s1 - (s2));
    historyPitch = (float)s1;
cam_pedHistoryUnchanged355:;
cam_pedHistoryDone355:;
    *p_maximum = maximum;
    *p_historyPitch = historyPitch;
}

__declspec(noinline) void __cdecl CamFollowPedHistoryBaseCpp(float step, float speed, float wanted, float* p_result) {
    double s0 = 0.0;
    volatile float result = 0.0f;
    s0 = (double)step;
    CAM_R(s0, s0 * ((double)speed));
    CAM_R(s0, s0 + ((double)wanted));
    result = (float)s0;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamFollowPedResetHistoryCpp(unsigned origin, unsigned direction, unsigned firstOut, float firstDistance, float* p_zProduct, unsigned secondOut, float secondDistance) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned eax = 0, ecx = 0, edx = 0;
    volatile float zProduct = 0.0f;
    eax = (unsigned)(origin);
    ecx = (unsigned)(direction);
    edx = (unsigned)(firstOut);
    s0 = (double)firstDistance;
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(ecx))));
    s1 = (double)firstDistance;
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ecx+0x4))));
    s2 = (double)firstDistance;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(ecx+0x8))));
    zProduct = (float)s2;
    s2 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s2, s2 - (s0));
    CAM_SF((unsigned)(edx), s2);
    s2 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s2, s2 - (s1));
    CAM_SF((unsigned)(edx+0x4), s2);
    s0 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s0, s0 - ((double)zProduct));
    CAM_SF((unsigned)(edx+0x8), s0);
    edx = (unsigned)(secondOut);
    s0 = (double)secondDistance;
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(ecx))));
    s1 = (double)secondDistance;
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(ecx+0x4))));
    s2 = (double)secondDistance;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(ecx+0x8))));
    zProduct = (float)s2;
    s2 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s2, s2 - (s0));
    CAM_SF((unsigned)(edx), s2);
    s2 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s2, s2 - (s1));
    CAM_SF((unsigned)(edx+0x4), s2);
    s0 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s0, s0 - ((double)zProduct));
    CAM_SF((unsigned)(edx+0x8), s0);
    *p_zProduct = zProduct;
}

__declspec(noinline) void __cdecl CamFollowPedHistoryCpp(float historyDelta, float verticalOffset, float* p_angle, float* p_cosAlpha, float beta, float* p_xDirection, float* p_zDirection, float historyDistance, float* p_xProduct, float* p_yProduct, unsigned origin, unsigned firstOut, float distance, unsigned secondOut) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    unsigned eax = 0, edx = 0;
    volatile float angle = 0.0f;
    volatile float cosAlpha = 0.0f;
    volatile float xDirection = 0.0f;
    volatile float zDirection = 0.0f;
    volatile float xProduct = 0.0f;
    volatile float yProduct = 0.0f;
    s0 = (double)historyDelta;
    CAM_R(s0, s0 + ((double)verticalOffset));
    angle = (float)s0;
    a0 = s0;
    cosAlpha = (float)(cos(a0));
    s0 = (double)beta;
    a1 = s0;
    CAM_R(s0, (cos(a1)) * ((double)cosAlpha));
    s0 = -s0;
    xDirection = (float)s0;
    s1 = (double)beta;
    a2 = s1;
    CAM_R(s1, (sin(a2)) * ((double)cosAlpha));
    s1 = -s1;
    s2 = s1;
    s3 = (double)angle;
    a3 = s3;
    zDirection = (float)(sin(a3));
    s4 = (double)xDirection;
    CAM_R(s4, s4 * ((double)historyDistance));
    xProduct = (float)s4;
    s3 = sin(a3);
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)historyDistance));
    yProduct = (float)s3;
    CAM_R(s2, s2 * ((double)historyDistance));
    eax = (unsigned)(origin);
    edx = (unsigned)(firstOut);
    s3 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s3, s3 - ((double)xProduct));
    CAM_SF((unsigned)(edx), s3);
    s3 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s3, s3 - ((double)yProduct));
    CAM_SF((unsigned)(edx+0x4), s3);
    s3 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s3, s3 - (s2));
    CAM_SF((unsigned)(edx+0x8), s3);
    { double t_ = s1; s1 = s0; s0 = t_; }
    xDirection = (float)s1;
    s1 = (double)zDirection;
    s2 = (double)xDirection;
    CAM_R(s2, s2 * ((double)distance));
    xProduct = (float)s2;
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)distance));
    yProduct = (float)s1;
    CAM_R(s0, s0 * ((double)distance));
    edx = (unsigned)(secondOut);
    s1 = (double)CAM_LF((unsigned)(eax));
    CAM_R(s1, s1 - ((double)xProduct));
    CAM_SF((unsigned)(edx), s1);
    s1 = (double)CAM_LF((unsigned)(eax+0x4));
    CAM_R(s1, s1 - ((double)yProduct));
    CAM_SF((unsigned)(edx+0x4), s1);
    s1 = (double)CAM_LF((unsigned)(eax+0x8));
    CAM_R(s1, s1 - (s0));
    CAM_SF((unsigned)(edx+0x8), s1);
    *p_angle = angle;
    *p_cosAlpha = cosAlpha;
    *p_xDirection = xDirection;
    *p_zDirection = zDirection;
    *p_xProduct = xProduct;
    *p_yProduct = yProduct;
}

__declspec(noinline) void __cdecl CamFollowPedHistoryHeadingCpp(float frontX, float frontY, float* p_result) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0, a1 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float result = *p_result;
    s0 = (double)frontX;
    s0 = -s0;
    s1 = (double)frontY;
    a0 = s0;
    a1 = s1;
    CAM_R(s0, (atan2(a0, a1)) - ((double)CAM_LF(0x858FE4)));
    result = (float)s0;
    cc = CamX87Status(s0, (double)CAM_LF(0x858CC0));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_ped_history_heading_done355;
    s0 = (double)result;
    CAM_R(s0, s0 + ((double)CAM_LF(0x858CBC)));
    result = (float)s0;
cam_ped_history_heading_done355:;
    *p_result = result;
}

__declspec(noinline) void __cdecl CamM16BlendCpp(float smoothing, float* p_complement, float betaInput, unsigned betaSpeed, unsigned alphaSpeed, float alphaInput, float* p_alphaRounded, unsigned betaAngle, unsigned alphaAngle) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0;
    unsigned eax = 0;
    volatile float complement = 0.0f;
    volatile float alphaRounded = 0.0f;
    s0 = (double)CAM_LF(0x858624);
    CAM_R(s0, s0 - ((double)smoothing));
    complement = (float)s0;
    CAM_R(s0, s0 * ((double)betaInput));
    eax = (unsigned)(betaSpeed);
    s1 = (double)smoothing;
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(eax))));
    CAM_R(s0, s0 + (s1));
    CAM_SF((unsigned)(eax), s0);
    eax = (unsigned)(alphaSpeed);
    s1 = (double)smoothing;
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(eax))));
    s2 = (double)complement;
    CAM_R(s2, s2 * ((double)alphaInput));
    CAM_R(s1, s1 + (s2));
    alphaRounded = (float)s1;
    CAM_SF((unsigned)(eax), s1);
    s1 = (double)alphaRounded;
    { double t_ = s1; s1 = s0; s0 = t_; }
    eax = (unsigned)(betaAngle);
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s1);
    eax = (unsigned)(alphaAngle);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s0);
    *p_complement = complement;
    *p_alphaRounded = alphaRounded;
}

__declspec(noinline) void __cdecl CamM16BumpCpp(int signedElapsed, unsigned horz, unsigned beta, unsigned vert, unsigned alpha, float damping, float step) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    s0 = (double)((int)signedElapsed);
    fa = (unsigned)((unsigned)signedElapsed);
    fb = (unsigned)(0x0u);
    if ((int)fa >= (int)fb) goto cam_m16_elapsed_positive355;
    CAM_R(s0, s0 + ((double)CAM_LF(0x858C54)));
cam_m16_elapsed_positive355:;
    CAM_R(s0, s0 / (double)((int)CAM_LU(0x8CC474)));
    CAM_R(s0, s0 * ((double)CAM_LF(0x858CBC)));
    a0 = s0;
    s1 = (double)CAM_LF(0x8CC480);
    CAM_R(s1, s1 * ((cos(a0))));
    eax = (unsigned)(horz);
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(eax))));
    eax = (unsigned)(beta);
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s1);
    eax = (unsigned)(vert);
    CAM_R(s0, (cos(a0)) * ((double)CAM_LF((unsigned)(eax))));
    CAM_R(s0, s0 * ((double)CAM_LF(0x8CC480)));
    eax = (unsigned)(alpha);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s0);
    s0 = (double)damping;
    s1 = (double)step;
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    eax = (unsigned)(horz);
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s0);
    s0 = (double)damping;
    s1 = (double)step;
    eax = (unsigned)(0x822130u);
    s0 = ExeCompatiblePowCpp(s0, s1);
    eax = (unsigned)(vert);
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s0);
}

__declspec(noinline) void __cdecl CamM16StickInputsCpp(float vertical, int attached, float horizontal, float fov, float step, float* p_betaResult, float* p_alphaResult, float* p_horizontalSign) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0;
    volatile float betaResult = *p_betaResult;
    volatile float alphaResult = *p_alphaResult;
    volatile float horizontalSign = *p_horizontalSign;
    s0 = (double)vertical;
    fa = (unsigned)((unsigned)attached);
    fb = (unsigned)(0x0u);
    if (fa == fb) goto cam_m16_unattached_input355;
    s1 = (double)horizontal;
    CAM_R(s1, s1 * ((double)CAM_LF(0x858B88)));
    s2 = (double)fov;
    CAM_R(s2, s2 * ((double)CAM_LF(0x85A5C4)));
    s3 = s1;
    s3 = CamFabs(s3);
    s4 = s2;
    CAM_R(s4, s4 * ((double)CAM_LF(0x858CEC)));
    CAM_R(s3, s3 * (s4));
    CAM_R(s3, s3 * ((double)step));
    CAM_R(s3, s3 * (s1));
    betaResult = (float)s3;
    s3 = s0;
    CAM_R(s3, s3 * ((double)CAM_LF(0x858B88)));
    s4 = s3;
    s4 = CamFabs(s4);
    { double t_ = s4; s4 = s2; s2 = t_; }
    CAM_R(s4, s4 * ((double)CAM_LF(0x8631C0)));
    CAM_R(s2, s2 * (s4));
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)step));
    CAM_R(s3, s3 * (s2));
    alphaResult = (float)s3;
    goto cam_m16_input_cleanup355;
cam_m16_unattached_input355:;
    s1 = (double)CAM_LF(0x858624);
    s2 = (double)horizontal;
    cc = CamX87Status(s2, (double)CAM_LF(0x858B50));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_m16_positive_h355;
    CAM_SETBITS(horizontalSign, 0xBF800000u);
cam_m16_positive_h355:;
    s2 = s0;
    cc = CamX87Status(s2, (double)CAM_LF(0x858B50));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_m16_positive_v355;
    s1 = (double)CAM_LF(0x858C1C);
cam_m16_positive_v355:;
    s2 = (double)fov;
    CAM_R(s2, s2 * ((double)CAM_LF(0x85A5C4)));
    s3 = (double)horizontal;
    CAM_R(s3, s3 * ((double)horizontal));
    CAM_R(s3, s3 * ((double)CAM_LF(0x858FC4)));
    s4 = s2;
    CAM_R(s4, s4 * ((double)CAM_LF(0x863100)));
    CAM_R(s3, s3 * (s4));
    CAM_R(s3, s3 * ((double)step));
    CAM_R(s3, s3 * ((double)horizontalSign));
    betaResult = (float)s3;
    s3 = s0;
    CAM_R(s3, s3 * (s0));
    CAM_R(s3, s3 * ((double)CAM_LF(0x8630FC)));
    { double t_ = s3; s3 = s2; s2 = t_; }
    CAM_R(s3, s3 * ((double)CAM_LF(0x859B50)));
    CAM_R(s2, s2 * (s3));
    CAM_R(s2, s2 * ((double)step));
    CAM_R(s2, s2 * (s1));
    alphaResult = (float)s2;
cam_m16_input_cleanup355:;
    *p_betaResult = betaResult;
    *p_alphaResult = alphaResult;
    *p_horizontalSign = horizontalSign;
}

__declspec(noinline) void __cdecl CamM16MouseAnglesCpp(float horizontal, float fov, float* p_scale, float vertical, unsigned beta, unsigned alpha) {
    double s0 = 0.0, s1 = 0.0;
    unsigned eax = 0;
    volatile float scale = 0.0f;
    s0 = (double)horizontal;
    s1 = (double)fov;
    CAM_R(s1, s1 * ((double)CAM_LF(0x85A5C4)));
    scale = (float)s1;
    CAM_R(s1, s1 * ((double)CAM_LF(0xB6EC1C)));
    CAM_R(s0, s0 * (s1));
    s1 = (double)scale;
    CAM_R(s1, s1 * ((double)CAM_LF(0xB6EC18)));
    CAM_R(s1, s1 * ((double)vertical));
    { double t_ = s1; s1 = s0; s0 = t_; }
    eax = (unsigned)(beta);
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s1);
    eax = (unsigned)(alpha);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s0);
    *p_scale = scale;
}

__declspec(noinline) void __cdecl CamM16SideRayCpp(float beta, int positive, float alpha, float* p_xDirection, float* p_yDirection, float* p_yProduct, float* p_zProduct, unsigned origin, unsigned outputVector) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    unsigned fa = 0, fb = 0;
    unsigned eax = 0, edx = 0;
    volatile float xDirection = *p_xDirection;
    volatile float yDirection = *p_yDirection;
    volatile float yProduct = *p_yProduct;
    volatile float zProduct = *p_zProduct;
    s0 = (double)beta;
    fa = (unsigned)((unsigned)positive);
    fb = (unsigned)(0x0u);
    if (fa == fb) goto cam_m16_side_negative355;
    CAM_R(s0, s0 + ((double)CAM_LF(0x8631B8)));
    goto cam_m16_side_alpha355;
cam_m16_side_negative355:;
    CAM_R(s0, s0 - ((double)CAM_LF(0x8631B8)));
cam_m16_side_alpha355:;
    s1 = (double)alpha;
    CAM_R(s1, s1 - ((double)CAM_LF(0x8631B4)));
    s2 = s1;
    a0 = s2;
    s3 = s0;
    a1 = s3;
    CAM_R(s3, (cos(a1)) * ((cos(a0))));
    xDirection = (float)s3;
    s2 = cos(a0);
    { double t_ = s2; s2 = s0; s0 = t_; }
    a2 = s2;
    CAM_R(s2, (sin(a2)) * (s0));
    yDirection = (float)s2;
    s0 = s1;
    a3 = s0;
    s1 = (double)xDirection;
    CAM_R(s1, s1 * ((double)CAM_LF(0x858B3C)));
    s2 = (double)yDirection;
    CAM_R(s2, s2 * ((double)CAM_LF(0x858B3C)));
    yProduct = (float)s2;
    s0 = sin(a3);
    { double t_ = s1; s1 = s0; s0 = t_; }
    CAM_R(s1, s1 * ((double)CAM_LF(0x858B3C)));
    zProduct = (float)s1;
    eax = (unsigned)(origin);
    edx = (unsigned)(outputVector);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(edx), s0);
    s0 = (double)yProduct;
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax+0x4))));
    CAM_SF((unsigned)(edx+0x4), s0);
    s0 = (double)zProduct;
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax+0x8))));
    CAM_SF((unsigned)(edx+0x8), s0);
    *p_xDirection = xDirection;
    *p_yDirection = yDirection;
    *p_yProduct = yProduct;
    *p_zProduct = zProduct;
}

__declspec(noinline) void __cdecl CamRocketFrontCpp(float verticalAngle, float horizontalAngle, float* p_frontX, float* p_frontY, float* p_frontZ) {
    double s0 = 0.0, s1 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    volatile float frontX = 0.0f;
    volatile float frontY = 0.0f;
    volatile float frontZ = 0.0f;
    s0 = (double)verticalAngle;
    a0 = s0;
    s1 = (double)horizontalAngle;
    a1 = s1;
    CAM_R(s1, (cos(a1)) * ((cos(a0))));
    s1 = -s1;
    frontX = (float)s1;
    s1 = (double)horizontalAngle;
    a2 = s1;
    CAM_R(s1, (sin(a2)) * ((cos(a0))));
    s1 = -s1;
    frontY = (float)s1;
    s0 = (double)verticalAngle;
    a3 = s0;
    frontZ = (float)(sin(a3));
    *p_frontX = frontX;
    *p_frontY = frontY;
    *p_frontZ = frontZ;
}

__declspec(noinline) void __cdecl CamRocketHeadingCpp(float atanY, float atanX, unsigned atanFn, float halfPi, float* p_heading) {
    double s0 = 0.0;
    volatile float heading = 0.0f;
    s0 = (double)((float(__cdecl*)(float, float))(uintptr_t)atanFn)(atanX, atanY);
    CAM_R(s0, s0 - ((double)halfPi));
    heading = (float)s0;
    *p_heading = heading;
}

__declspec(noinline) void __cdecl CamWheelUpCpp(int phase, float angleScale, float amount, float sx, float* p_lateralX, float sy, float* p_lateralY, float sz, float ux, float uy, float* p_verticalY, float uz, float* p_verticalZ, float* p_outputX, float* p_outputY, float* p_outputZ) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0;
    volatile float lateralX = 0.0f;
    volatile float lateralY = 0.0f;
    volatile float verticalY = 0.0f;
    volatile float verticalZ = 0.0f;
    volatile float outputX = 0.0f;
    volatile float outputY = 0.0f;
    volatile float outputZ = 0.0f;
    s0 = (double)((int)phase);
    CAM_R(s0, s0 * ((double)angleScale));
    a0 = s0;
    CAM_R(s0, (cos(a0)) * ((double)amount));
    s1 = s0;
    a1 = s1;
    s2 = (double)sx;
    CAM_R(s2, s2 * ((sin(a1))));
    lateralX = (float)s2;
    s2 = (double)sy;
    CAM_R(s2, s2 * ((sin(a1))));
    lateralY = (float)s2;
    CAM_R(s1, (sin(a1)) * ((double)sz));
    { double t_ = s1; s1 = s0; s0 = t_; }
    a2 = s1;
    s2 = (double)ux;
    CAM_R(s2, s2 * ((cos(a2))));
    s3 = (double)uy;
    CAM_R(s3, s3 * ((cos(a2))));
    verticalY = (float)s3;
    s3 = (double)uz;
    CAM_R(s3, s3 * ((cos(a2))));
    verticalZ = (float)s3;
    CAM_R(s2, s2 + ((double)lateralX));
    outputX = (float)s2;
    s1 = (double)verticalY;
    CAM_R(s1, s1 + ((double)lateralY));
    outputY = (float)s1;
    s1 = (double)verticalZ;
    CAM_R(s1, s1 + (s0));
    outputZ = (float)s1;
    *p_lateralX = lateralX;
    *p_lateralY = lateralY;
    *p_verticalY = verticalY;
    *p_verticalZ = verticalZ;
    *p_outputX = outputX;
    *p_outputY = outputY;
    *p_outputZ = outputZ;
}

__declspec(noinline) void __cdecl CamPathInterpolateVectorCpp(unsigned out, unsigned knots, unsigned tBits, unsigned index) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned esp = 0, eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0;
    unsigned frame[96] = {0};
    esp = (unsigned)&frame[80];
    CAM_SU(esp + 0x4, out);
    CAM_SU(esp + 0x8, knots);
    CAM_SU(esp + 0xC, tBits);
    CAM_SU(esp + 0x10, index);
    esp = (unsigned)(esp - 0x34u);
    esp = esp - 4;
    CAM_SU(esp, esi);
    esi = (unsigned)(CAM_LU((unsigned)(esp+0x40)));
    s0 = (double)CAM_LF((unsigned)(esi));
    esp = esp - 4;
    CAM_SU(esp, edi);
    { long long t_ = (long long)s0; eax = (unsigned)t_; edx = (unsigned)((unsigned long long)t_ >> 32); }
    edi = (unsigned)(CAM_LU((unsigned)(esp+0x4C)));
    edx = (unsigned)(CAM_LU((unsigned)(edi)));
    s0 = (double)CAM_LF((unsigned)(esi+edx*4));
    ecx = (unsigned)(eax);
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(esi+edx*4-0x28))));
    eax = (unsigned)((unsigned)(ecx+ecx*4));
    CAM_R(s0, s0 * ((double)CAM_LF(0x858C4C)));
    s1 = (double)CAM_LF((unsigned)(esi+eax*8-0x24));
    CAM_R(s1, s1 * ((double)CAM_LF(0x858C4C)));
    CAM_SF((unsigned)(esp+0x44), s1);
    s1 = (double)CAM_LF((unsigned)(esp+0x48));
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(esp+0x44)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B2128;
    edx = (unsigned)(edx - 1);
    eax = (unsigned)(0xCCCCCCCDu);
    { unsigned long long p_ = (unsigned long long)eax * (unsigned)(edx); eax = (unsigned)p_; edx = (unsigned)(p_ >> 32); }
    edx = (unsigned)(edx >> 0x3u);
    fa = (unsigned)(edx);
    fb = (unsigned)(ecx);
    esp = esp - 4;
    CAM_SU(esp, ebx);
    ebx = (ebx & ~(0xFFu << 0)) | (((unsigned)(0x1u) & 0xFFu) << 0);
    if (fa <= fb) goto cam_L005B20F3;
    edx = (unsigned)((unsigned)(ecx+ecx*4-0x5));
    eax = (unsigned)((unsigned)(edx+edx+0x1));
    ebx = (ebx & ~(0xFFu << 0)) | (((unsigned)(((ebx >> 0) & 0xFF) ^ ((ebx >> 0) & 0xFF)) & 0xFFu) << 0);
    CAM_SU((unsigned)(edi), eax);
cam_L005B20F3:;
    cc = CamX87Status(s0, (double)CAM_LF(0x8D0F80));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B2127;
    fa = (unsigned)(((ebx >> 0) & 0xFF));
    fb = (unsigned)(0x1u);
    if ((fa & 0xFFu) != (fb & 0xFFu)) goto cam_L005B2127;
    eax = (unsigned)(CAM_LU((unsigned)(edi)));
    eax = (unsigned)(eax + 0xAu);
    CAM_SU((unsigned)(edi), eax);
    edx = (unsigned)((unsigned)(eax-0x1));
    eax = (unsigned)(0xCCCCCCCDu);
    { unsigned long long p_ = (unsigned long long)eax * (unsigned)(edx); eax = (unsigned)p_; edx = (unsigned)(p_ >> 32); }
    edx = (unsigned)(edx >> 0x3u);
    fa = (unsigned)(edx);
    fb = (unsigned)(ecx);
    if (fa <= fb) goto cam_L005B2127;
    eax = (unsigned)((unsigned)(ecx+ecx*4-0x5));
    ecx = (unsigned)((unsigned)(eax+eax+0x1));
    CAM_SU((unsigned)(edi), ecx);
cam_L005B2127:;
    ebx = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
cam_L005B2128:;
    ecx = (unsigned)(CAM_LU((unsigned)(edi)));
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4-0x28));
    CAM_R(s1, s1 * ((double)CAM_LF(0x858C4C)));
    CAM_R(s1, ((double)CAM_LF((unsigned)(esp+0x48))) - s1);
    CAM_R(s0, (s1) / s0);
    cc = CamX87Status(s0, (double)CAM_LF(0x858624));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_L005B2151;
    s0 = (double)CAM_LF(0x858624);
    goto cam_L005B2166;
cam_L005B2151:;
    cc = CamX87Status(s0, (double)CAM_LF(0x858B50));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B2166;
    s0 = (double)CAM_LF(0x858B50);
cam_L005B2166:;
    s1 = (double)CAM_LF((unsigned)(esp+0x48));
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(esp+0x44)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_L005B217D;
    s0 = (double)CAM_LF(0x858624);
cam_L005B217D:;
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4-0x24));
    edi = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    CAM_SF((unsigned)(esp+0x8), s1);
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4-0x20));
    CAM_SF((unsigned)(esp+0xC), s1);
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4-0x1C));
    CAM_SF((unsigned)(esp+0x10), s1);
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4+0x4));
    CAM_SF((unsigned)(esp+0x14), s1);
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4+0x8));
    CAM_SF((unsigned)(esp+0x18), s1);
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4+0xC));
    CAM_SF((unsigned)(esp+0x1C), s1);
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4-0xC));
    CAM_SF((unsigned)(esp+0x20), s1);
    s2 = (double)CAM_LF((unsigned)(esi+ecx*4-0x8));
    CAM_SF((unsigned)(esp+0x24), s2);
    s2 = (double)CAM_LF((unsigned)(esi+ecx*4-0x4));
    CAM_SF((unsigned)(esp+0x28), s2);
    s2 = (double)CAM_LF((unsigned)(esi+ecx*4+0x10));
    CAM_SF((unsigned)(esp+0x2C), s2);
    s2 = (double)CAM_LF((unsigned)(esi+ecx*4+0x14));
    CAM_SF((unsigned)(esp+0x30), s2);
    s2 = (double)CAM_LF((unsigned)(esi+ecx*4+0x18));
    esi = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    CAM_SF((unsigned)(esp+0x30), s2);
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(esp+0x4)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x44u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B220C;
    s1 = (double)CAM_LF((unsigned)(esp+0x20));
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(esp+0x8)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x44u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B220C;
    s1 = (double)CAM_LF((unsigned)(esp+0x24));
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(esp+0xC)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x44u);
    if (!CamParity(((fa & fb) & 0xFFu))) goto cam_L005B22C4;
cam_L005B220C:;
    s1 = s0;
    eax = (unsigned)(CAM_LU((unsigned)(esp+0x38)));
    CAM_R(s1, s1 * (s0));
    s2 = s1;
    CAM_R(s2, s2 * (s0));
    CAM_SF((unsigned)(esp+0x3C), s2);
    s2 = (double)CAM_LF(0x858624);
    CAM_R(s2, s2 - (s0));
    s3 = s2;
    CAM_R(s3, s3 * (s2));
    CAM_SF((unsigned)(esp), s3);
    CAM_R(s3, s3 * (s2));
    CAM_SF((unsigned)(esp+0x44), s3);
    CAM_R(s2, s2 * (s1));
    CAM_SF((unsigned)(esp+0x40), s2);
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(esp))));
    s1 = (double)CAM_LF((unsigned)(esp+0x40));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esp+0x28))));
    s2 = s0;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esp+0x1C))));
    CAM_R(s1, s1 + (s2));
    CAM_R(s1, s1 * ((double)CAM_LF(0x858B3C)));
    s2 = (double)CAM_LF((unsigned)(esp+0x3C));
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esp+0x10))));
    CAM_R(s1, s1 + (s2));
    s2 = (double)CAM_LF((unsigned)(esp+0x44));
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esp+0x4))));
    CAM_R(s1, s1 + (s2));
    CAM_SF((unsigned)(eax), s1);
    s1 = (double)CAM_LF((unsigned)(esp+0x40));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esp+0x2C))));
    s2 = s0;
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esp+0x20))));
    CAM_R(s1, s1 + (s2));
    CAM_R(s1, s1 * ((double)CAM_LF(0x858B3C)));
    s2 = (double)CAM_LF((unsigned)(esp+0x3C));
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esp+0x14))));
    CAM_R(s1, s1 + (s2));
    s2 = (double)CAM_LF((unsigned)(esp+0x44));
    CAM_R(s2, s2 * ((double)CAM_LF((unsigned)(esp+0x8))));
    CAM_R(s1, s1 + (s2));
    CAM_SF((unsigned)(eax+0x4), s1);
    CAM_R(s0, s0 * ((double)CAM_LF((unsigned)(esp+0x24))));
    s1 = (double)CAM_LF((unsigned)(esp+0x40));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esp+0x30))));
    CAM_R(s0, s0 + (s1));
    CAM_R(s0, s0 * ((double)CAM_LF(0x858B3C)));
    s1 = (double)CAM_LF((unsigned)(esp+0x3C));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esp+0x18))));
    CAM_R(s0, s0 + (s1));
    s1 = (double)CAM_LF((unsigned)(esp+0x44));
    CAM_R(s1, s1 * ((double)CAM_LF((unsigned)(esp+0xC))));
    CAM_R(s0, s0 + (s1));
    CAM_SF((unsigned)(eax+0x8), s0);
    goto cam_L005B22FC;
cam_L005B22C4:;
    s1 = (double)CAM_LF((unsigned)(esp+0x10));
    eax = (unsigned)(CAM_LU((unsigned)(esp+0x38)));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(esp+0x4))));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(esp+0x4))));
    CAM_SF((unsigned)(eax), s1);
    s1 = (double)CAM_LF((unsigned)(esp+0x14));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(esp+0x8))));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(esp+0x8))));
    CAM_SF((unsigned)(eax+0x4), s1);
    s1 = (double)CAM_LF((unsigned)(esp+0x18));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(esp+0xC))));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(esp+0xC))));
    CAM_SF((unsigned)(eax+0x8), s1);
cam_L005B22FC:;
    s0 = (double)CAM_LF(0xB6F8AC);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax))));
    CAM_SF((unsigned)(eax), s0);
    s0 = (double)CAM_LF(0xB6F8B0);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax+0x4))));
    CAM_SF((unsigned)(eax+0x4), s0);
    s0 = (double)CAM_LF(0xB6F8B4);
    CAM_R(s0, s0 + ((double)CAM_LF((unsigned)(eax+0x8))));
    CAM_SF((unsigned)(eax+0x8), s0);
    esp = (unsigned)(esp + 0x34u);
    goto cam_end;
cam_end:;
}

__declspec(noinline) void __cdecl CamPathInterpolateScalarCpp(unsigned out, unsigned knots, unsigned tBits, unsigned index) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0, s5 = 0.0, s6 = 0.0;
    unsigned cc = 0;
    unsigned fa = 0, fb = 0;
    unsigned esp = 0, eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0;
    unsigned frame[96] = {0};
    esp = (unsigned)&frame[80];
    CAM_SU(esp + 0x4, out);
    CAM_SU(esp + 0x8, knots);
    CAM_SU(esp + 0xC, tBits);
    CAM_SU(esp + 0x10, index);
    esp = esp - 4;
    CAM_SU(esp, ebx);
    esp = esp - 4;
    CAM_SU(esp, esi);
    esi = (unsigned)(CAM_LU((unsigned)(esp+0x10)));
    s0 = (double)CAM_LF((unsigned)(esi));
    esp = esp - 4;
    CAM_SU(esp, edi);
    { long long t_ = (long long)s0; eax = (unsigned)t_; edx = (unsigned)((unsigned long long)t_ >> 32); }
    edx = (unsigned)(CAM_LU((unsigned)(esp+0x1C)));
    edi = (unsigned)(CAM_LU((unsigned)(edx)));
    s0 = (double)CAM_LF((unsigned)(esi+edi*4));
    ecx = (unsigned)(eax);
    CAM_R(s0, s0 - ((double)CAM_LF((unsigned)(esi+edi*4-0x10))));
    eax = (unsigned)(eax << 0x4u);
    CAM_R(s0, s0 * ((double)CAM_LF(0x858C4C)));
    s1 = (double)CAM_LF((unsigned)(eax+esi-0xC));
    CAM_R(s1, s1 * ((double)CAM_LF(0x858C4C)));
    CAM_SF((unsigned)(esp+0x14), s1);
    s1 = (double)CAM_LF((unsigned)(esp+0x18));
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(esp+0x14)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B23B4;
    edi = (unsigned)(edi - 1);
    edi = (unsigned)(edi >> 0x2u);
    fa = (unsigned)(edi);
    fb = (unsigned)(ecx);
    ebx = (ebx & ~(0xFFu << 0)) | (((unsigned)(0x1u) & 0xFFu) << 0);
    if (fa <= fb) goto cam_L005B2388;
    eax = (unsigned)((unsigned)(ecx*4+0xFFFFFFFD));
    ebx = (ebx & ~(0xFFu << 0)) | (((unsigned)(((ebx >> 0) & 0xFF) ^ ((ebx >> 0) & 0xFF)) & 0xFFu) << 0);
    CAM_SU((unsigned)(edx), eax);
cam_L005B2388:;
    cc = CamX87Status(s0, (double)CAM_LF(0x8D0F80));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B23B6;
    fa = (unsigned)(((ebx >> 0) & 0xFF));
    fb = (unsigned)(0x1u);
    if ((fa & 0xFFu) != (fb & 0xFFu)) goto cam_L005B23B6;
    eax = (unsigned)(CAM_LU((unsigned)(edx)));
    eax = (unsigned)(eax + 0x4u);
    CAM_SU((unsigned)(edx), eax);
    eax = (unsigned)(eax - 1);
    eax = (unsigned)(eax >> 0x2u);
    fa = (unsigned)(eax);
    fb = (unsigned)(ecx);
    if (fa <= fb) goto cam_L005B23B6;
    ecx = (unsigned)((unsigned)(ecx*4+0xFFFFFFFD));
    CAM_SU((unsigned)(edx), ecx);
    goto cam_L005B23B6;
cam_L005B23B4:;
cam_L005B23B6:;
    ecx = (unsigned)(CAM_LU((unsigned)(edx)));
    s0 = (double)CAM_LF((unsigned)(esi+ecx*4-0x10));
    CAM_R(s0, s0 * ((double)CAM_LF(0x858C4C)));
    CAM_R(s0, ((double)CAM_LF((unsigned)(esp+0x18))) - s0);
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(esi+ecx*4-0x10))));
    CAM_R(s1, s1 * ((double)CAM_LF(0x858C4C)));
    CAM_R(s0, s0 / (s1));
    cc = CamX87Status(s0, (double)CAM_LF(0x858624));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_L005B23EC;
    s0 = (double)CAM_LF(0x858624);
    goto cam_L005B2401;
cam_L005B23EC:;
    cc = CamX87Status(s0, (double)CAM_LF(0x858B50));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x5u);
    if (CamParity(((fa & fb) & 0xFFu))) goto cam_L005B2401;
    s0 = (double)CAM_LF(0x858B50);
cam_L005B2401:;
    s1 = (double)CAM_LF((unsigned)(esp+0x18));
    cc = CamX87Status(s1, (double)CAM_LF((unsigned)(esp+0x14)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x41u);
    if (!(((fa & fb) & 0xFFu) == 0)) goto cam_L005B2418;
    s0 = (double)CAM_LF(0x858624);
cam_L005B2418:;
    s1 = (double)CAM_LF(0x858624);
    CAM_R(s1, s1 - (s0));
    s2 = s1;
    CAM_R(s2, s2 * (s1));
    s3 = s2;
    CAM_R(s3, s3 * (s1));
    s4 = (double)CAM_LF((unsigned)(esi+ecx*4-0x4));
    s5 = (double)CAM_LF((unsigned)(esi+ecx*4+0x8));
    s6 = s4;
    cc = CamX87Status(s6, (double)CAM_LF((unsigned)(esi+ecx*4-0xC)));
    eax = (eax & 0xFFFF0000u) | ((unsigned)(cc << 8) & 0xFFFFu);
    fa = (unsigned)(((eax >> 8) & 0xFF));
    fb = (unsigned)(0x44u);
    if (!CamParity(((fa & fb) & 0xFFu))) goto cam_L005B2477;
    s6 = (double)CAM_LF((unsigned)(esi+ecx*4+0x4));
    edx = (unsigned)(CAM_LU((unsigned)(esp+0x10)));
    CAM_R(s6, s6 * (s0));
    { double t_ = s6; s6 = s5; s5 = t_; }
    edi = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    CAM_R(s6, s6 * (s1));
    CAM_R(s6, s6 * ((double)CAM_LF(0x858B3C)));
    CAM_R(s5, s5 + (s6));
    CAM_R(s5, s5 * (s0));
    { double t_ = s5; s5 = s4; s4 = t_; }
    CAM_R(s5, s5 * (s2));
    CAM_R(s5, s5 * ((double)CAM_LF(0x858B3C)));
    CAM_R(s4, s4 + (s5));
    CAM_R(s4, s4 * (s0));
    { double t_ = s4; s4 = s3; s3 = t_; }
    CAM_R(s4, s4 * ((double)CAM_LF((unsigned)(esi+ecx*4-0xC))));
    esi = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    ebx = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    CAM_R(s3, s3 + (s4));
    CAM_SF((unsigned)(edx), s3);
    goto cam_end;
cam_L005B2477:;
    eax = (unsigned)(CAM_LU((unsigned)(esp+0x10)));
    edi = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    s1 = (double)CAM_LF((unsigned)(esi+ecx*4+0x4));
    CAM_R(s1, s1 - ((double)CAM_LF((unsigned)(esi+ecx*4-0xC))));
    CAM_R(s1, s1 * (s0));
    CAM_R(s1, s1 + ((double)CAM_LF((unsigned)(esi+ecx*4-0xC))));
    esi = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    ebx = (unsigned)(CAM_LU(esp));
    esp = esp + 4;
    CAM_SF((unsigned)(eax), s1);
    goto cam_end;
cam_end:;
}

#undef CAM_LF
#undef CAM_LD
#undef CAM_LU
#undef CAM_LW
#undef CAM_LB
#undef CAM_SF
#undef CAM_SD
#undef CAM_SU
#undef CAM_R
#undef CAM_BITS
#undef CAM_SETBITS

// ===== CBike (Bike.cpp; lote w20, prefijo Bike) =====
// Colocar al FINAL de X87Math.cpp, fuera de cualquier #pragma float_control(precise) abierto y DESPUES de ExeCompatiblePowCpp / ExeAsinDoubleCpp
// (ya definidos arriba). Usa tambien (sin copiarlos) PowScaleVector3Cpp, PowScaleFloatCpp, CnvDecayPowCpp, QuadBikeDampDeltaFullCpp, TrigSimple*Cpp, BmxSin/CosScaledCpp
// y UnsignedPlusFloatCpp. Las constantes del EXE se leen por punteros volatile para que /fp:fast (y el plegado de constantes) no reasocie ni agrupe operaciones.
static double BikeAbsDouble(double value) {
    unsigned long long bits;
    memcpy(&bits, &value, 8);
    bits &= 0x7FFFFFFFFFFFFFFFull;                                  // fabs: limpia el signo tambien de un NaN
    memcpy(&value, &bits, 8);
    return value;
}
// fsqrt en linea (fuera de precise, que lo convertiria en una llamada a la CRT); la comparacion de la raiz la hace el llamador.
static __declspec(noinline) double BikeSqrtFull(double value) { return sqrt(value); }

// --- CBike::CBike (0x6BF59E..0x6BF5BD): fld degrees; fmul 0.017453292f (0x8595EC); fptan; fstp st(0); fstp [this+7A4h]. tan a precision completa.
__declspec(noinline) float __cdecl BikeTanScaledCpp(float degrees, float k) {
    volatile float result = float(tan(double(degrees) * double(k)));
    return result;
}

// --- Transcendentes con el resultado sin redondear en ST0 (fuera de precise) ---
// CBike::CalculateLeanMatrix (0x6B7211..0x6B721D): fld angle; fcos; fsubr 1.0f; fstp. 1 - cos con el coseno completo.
__declspec(noinline) float __cdecl BikeOneMinusCosCpp(float angle) {
    volatile float result = float(1.0 - cos(double(angle)));
    return result;
}
// CBike::PreRender (0x6BDD56..0x6BDD93): fcos; (1.0f - cos) * minZ * 0.9f (0x858C20); sin el sumando de BmxChassisZCpp (el -0 de minZ * 0 se conserva).
__declspec(noinline) float __cdecl BikeChassisHeightCpp(float lean, float minZ) {
    static const volatile float kNine = std::bit_cast<float>(0x3F666666u);   // 0x858C20
    const volatile float* height = &minZ;
    const volatile float* factor = &kNine;
    double value = 1.0 - cos(double(lean));
    value *= double(*height);
    value *= double(*factor);
    volatile float result = float(value);
    return result;
}
// CBike::PreRender (0x6BD2B3..0x6BD320): s = sin(lean) * minZ * 0.8f (0x858C98); out = s * right + point por componente (fmul / fadd de 24 bits, un fstp float por resultado).
__declspec(noinline) void __cdecl BikeContactOffsetCpp(float* out, const float* point, const float* right, float lean, float minZ) {
    static const volatile float kEight = std::bit_cast<float>(0x3F4CCCCDu);  // 0x858C98
    const volatile float* height = &minZ;
    const volatile float* factor = &kEight;
    double s = sin(double(lean));
    s *= double(*height);
    s *= double(*factor);
    const float p0 = point[0], p1 = point[1], p2 = point[2];
    const float r0 = right[0], r1 = right[1], r2 = right[2];
    out[2] = float(s * double(r2) + double(p2));
    out[1] = float(s * double(r1) + double(p1));
    out[0] = float(s * double(r0) + double(p0));
}
// CBike::PreRender (0x6BDB11..0x6BDB37): fld h; fsub o; fdiv len; fst [esp+20h]; call _CIasin (0x821E70); fmul -1.0f (0x858C1C); fstp. El cociente llega a _CIasin en ST0 (sin pasar por float).
__declspec(noinline) float __cdecl BikeForkAngleCpp(float height, float original, float forkLength) {
    static const volatile float kMinusOne = -1.0f;                           // 0x858C1C
    const double quotient = (double(height) - double(original)) / double(forkLength);
    volatile float result = float(ExeAsinDoubleCpp(quotient) * double(kMinusOne));
    return result;
}

// CBike::PlaceOnRoadProperly (0x6BF0F1..0x6BF155): L = rear + front; angle = fpatan((fz - bz) / L, 1.0f); c = fcos(angle); right.x = (fy - by) / L; right.y = -((fx - bx) / L);
// right.z = 0; forward.x = -(c * right.y); forward.y = c * right.x (leyendo los float guardados); forward.z = fsin(angle). m apunta a una CMatrix (right en 0..2, forward en 4..6).
// angle queda en la pila para fcos y fsin (sin redondear); sin precise para que fpatan/fcos/fsin salgan en linea.
__declspec(noinline) void __cdecl BikeRoadOrientationCpp(float* m, float rear, float front, float backX, float backY, float backZ, float frontX, float frontY, float frontZ) {
    const double length = double(rear) + double(front);
    const double angle = atan2((double(frontZ) - double(backZ)) / length, 1.0);
    const double c = cos(angle);
    volatile float rightX = float((double(frontY) - double(backY)) / length);
    volatile float rightY = float(-((double(frontX) - double(backX)) / length));
    m[0] = rightX;
    m[1] = rightY;
    m[2] = 0.0f;
    volatile float forwardX = float(-(c * double(rightY)));
    m[4] = forwardX;
    volatile float forwardY = float(c * double(rightX));
    m[5] = forwardY;
    m[6] = float(sin(angle));
}

// --- Aritmetica con el orden de redondeo del EXE (precise: sin reasociar ni distribuir) ---
#pragma float_control(precise, on, push)
// CBike::PlaceOnRoadProperly (0x6BF167..0x6BF1E0): x = (fx + bx) * 0.5f, y = (fy + by) * 0.5f (sumas de 24 bits sin guardar), z = float(fz + bz) guardado y luego * 0.5f.
__declspec(noinline) void __cdecl BikeRoadMidpointCpp(float frontX, float backX, float frontY, float backY, float frontZ, float backZ, float* out) {
    static const volatile float kHalf = 0.5f;                                // 0x858B8C
    const double half = double(kHalf);
    out[0] = float((double(frontX) + double(backX)) * half);
    out[1] = float((double(frontY) + double(backY)) * half);
    volatile float sumZ = float(double(backZ) + double(frontZ));
    out[2] = float(double(sumZ) * half);
}

// CBike::FixHandsToBars (0x6B8274..0x6B8430): bone += delta * factor. x e y: producto guardado como float (fstp) y luego fadd; z: producto sin guardar. Los tres redondeos a float al escribir.
__declspec(noinline) void __cdecl BikeMoveHandBoneCpp(float* bonePosition, const float* delta, float factor) {
    volatile float deltaX = float(double(delta[0]) * double(factor));
    volatile float deltaY = float(double(delta[1]) * double(factor));
    const double deltaZ = double(delta[2]) * double(factor);
    bonePosition[0] = float(double(deltaX) + double(bonePosition[0]));
    bonePosition[1] = float(double(deltaY) + double(bonePosition[1]));
    bonePosition[2] = float(deltaZ + double(bonePosition[2]));
}

// CBike::ProcessBuoyancy: usa PowScaleVector3Cpp (move y turn). VehicleDamage (0x6B91C2): health = health - damage * factor (fmul; fsubr; fstp).
__declspec(noinline) void __cdecl BikeHealthMinusCpp(float* health, float damage, float factor) {
    *health = float(double(*health) - double(damage) * double(factor));
}

// CBike::SetupSuspensionLines (0x6B8C10..0x6B8C54): z del extremo = ((lower - upper) - wheel * 0.5f) + startZ.
__declspec(noinline) float __cdecl BikeSetupEndCpp(float upper, float lower, float wheel, float startZ) {
    static const volatile float kHalf = 0.5f;                                // 0x858B8C
    const double scaledWheel = double(wheel) * double(kHalf);
    const double span = double(lower) - double(upper);
    volatile float result = float((span - scaledWheel) + double(startZ));
    return result;
}
// CBike::SetupSuspensionLines (0x6B8CFB..0x6B8D34): ((1 - 1 / (force * divisor)) * suspensionLength) + (wheel * 0.5f - startZ); divisor = 4.0f (0x858B90).
__declspec(noinline) float __cdecl BikeSetupHeightCpp(float force, float divisor, float suspensionLength, float wheel, float startZ) {
    static const volatile float kHalf = 0.5f;                                // 0x858B8C
    static const volatile float kOne = 1.0f;                                 // 0x858624
    const double one = double(kOne);
    const double scale = one - one / (double(force) * double(divisor));
    const double lowered = double(wheel) * double(kHalf) - double(startZ);
    volatile float result = float(scale * double(suspensionLength) + lowered);
    return result;
}
// CBike::SetupSuspensionLines (0x6B8D6D..0x6B8E97), por esfera de colision (sphere = x, y, z, radio): solo si (z - radio) < limit (comparacion ordenada).
// Si radio > 0.4f (0x858EE8): radio = max(0.4f, z - limit) (NaN conserva z - limit); luego z = limit + radio.
__declspec(noinline) void __cdecl BikeSetupSphereCpp(float* sphere, float limit) {
    static const volatile float kMinimum = std::bit_cast<float>(0x3ECCCCCDu); // 0x858EE8
    const double minimum = double(kMinimum);
    const double bottom = double(sphere[2]) - double(sphere[3]);
    if (bottom < double(limit)) {
        if (double(sphere[3]) > minimum) {
            const double lowered = double(sphere[2]) - double(limit);
            sphere[3] = float(minimum > lowered ? minimum : lowered);
        }
        sphere[2] = float(double(limit) + double(sphere[3]));
    }
}

// CBike::ProcessRiderAnims (0x6B7932..0x6B795C y 0x6B7C87..0x6B7CAB): p = pow(base, paso) (ST0 sin redondear); (1 - p) * target + p * previous; fstp float.
__declspec(noinline) float __cdecl BikeRiderSmoothCpp(float base, float step, float previous, float target) {
    const double keep = ExeCompatiblePowCpp(double(base), double(step));
    const double rest = 1.0 - keep;
    const double fromTarget = rest * double(target);
    const double fromPrevious = keep * double(previous);
    volatile float result = float(fromTarget + fromPrevious);
    return result;
}
// CBike::ProcessRiderAnims (0x6B7CF1..0x6B7D64): len = sqrt(lateral^2 + forward^2) (sqrt 24 bits, sin guardar); si len > 0.01f (0x87135C): inv = 1 / len guardado como float,
// pesos |forward * inv| y |inv * lateral|; si no: |forward| y |lateral|. forwardOut = peso1 * available, lateralOut = peso2 * available (fabs de x87: tambien limpia el signo de un NaN).
__declspec(noinline) void __cdecl BikeRiderWeightsCpp(float lateral, float forward, float available, float* lateralOut, float* forwardOut) {
    static const volatile float kSmall = std::bit_cast<float>(0x3C23D70Au);   // 0x87135C
    const double length = BikeSqrtFull(double(lateral) * double(lateral) + double(forward) * double(forward));
    double forwardWeight, lateralWeight;
    if (length > double(kSmall)) {
        volatile float inverse = float(1.0 / length);
        forwardWeight = BikeAbsDouble(double(forward) * double(inverse));
        lateralWeight = BikeAbsDouble(double(inverse) * double(lateral));
    } else {
        forwardWeight = BikeAbsDouble(double(forward));
        lateralWeight = BikeAbsDouble(double(lateral));
    }
    *forwardOut = float(forwardWeight * double(available));
    *lateralOut = float(lateralWeight * double(available));
}
// CBike::ProcessRiderAnims (0x6B7EBB..0x6B7EFC y 0x6B7F0C..0x6B7F47): max = 6.0f (0x871338) * speed (fst float); min = -(ST0) (fchs; fstp float); (max - min) * (random * 3.0517578e-5f (0x858C7C)) + min.
__declspec(noinline) float __cdecl BikeRiderJitterCpp(float speed, int randomValue) {
    static const volatile float kSix = 6.0f;                                  // 0x871338
    static const volatile float kScale = std::bit_cast<float>(0x38000100u);   // 0x858C7C
    const double product = double(kSix) * double(speed);
    volatile float maximum = float(product);
    volatile float minimum = float(-product);
    const double fraction = double(randomValue) * double(kScale);
    volatile float result = float((double(maximum) - double(minimum)) * fraction + double(minimum));
    return result;
}
// CBike::ProcessRiderAnims (0x6B7C16..0x6B7C5E): v = 1.0f - (|lean| - 0.3f) / (0.56f - 0.3f); si 0.0f > v: v = 0.0f; result = v * target. 0.56f - 0.3f se resta en x87 (24 bits): constantes por puntero volatile.
__declspec(noinline) float __cdecl BikeRiderForwardLimitCpp(float lean, float target) {
    static const volatile float kLow = std::bit_cast<float>(0x3E99999Au);     // 0x871358 (0.3f)
    static const volatile float kHigh = std::bit_cast<float>(0x3F0F5C29u);    // 0x871354 (0.56f)
    static const volatile float kOne = 1.0f;                                   // 0x858624
    static const volatile float kZero = 0.0f;                                  // 0x858B50
    const double excess = BikeAbsDouble(double(lean)) - double(kLow);
    const double range = double(kHigh) - double(kLow);
    double value = double(kOne) - excess / range;
    if (double(kZero) > value) {
        value = double(kZero);
    }
    volatile float result = float(value * double(target));
    return result;
}

// CBike::ProcessControlInputs (0x6BE497..0x6BE4B7 y 0x6BE5E8..0x6BE64F): ((input * 0.0078125f) - old) * step * factor + old, todo con el redondeo de 24 bits del EXE.
__declspec(noinline) float __cdecl BikeControlBlendCpp(float oldValue, int input, float step, float factor) {
    static const volatile float kScale = std::bit_cast<float>(0x3C000000u);   // 0x858B88
    const double scaled = double(input) * double(kScale);
    volatile float result = float(((scaled - double(oldValue)) * double(step)) * double(factor) + double(oldValue));
    return result;
}
// CBike::ProcessControlInputs (0x6BE504..0x6BE537): input * factor + old (fmul; fadd; fstp).
__declspec(noinline) float __cdecl BikeControlProductAddCpp(float oldValue, float input, float factor) {
    volatile float result = float(double(input) * double(factor) + double(oldValue));
    return result;
}
// CBike::ProcessControlInputs (0x6BE8B4..0x6BE8C6): (degrees * 0.017453292f (0x8595EC)) * steer.
__declspec(noinline) float __cdecl BikeInputSteerCpp(float degrees, float steer) {
    static const volatile float kRadians = std::bit_cast<float>(0x3C8EFA35u);  // 0x8595EC
    const double radians = double(degrees) * double(kRadians);
    volatile float result = float(radians * double(steer));
    return result;
}
// CBike::ProcessControlInputs (0x6BE31B..0x6BE33D): (speed.z * forward.z + speed.y * forward.y) + speed.x * forward.x (fmul / faddp, z primero).
__declspec(noinline) float __cdecl BikeForwardSpeedCpp(const float* speed, const float* forward) {
    const double z = double(speed[2]) * double(forward[2]);
    const double y = double(speed[1]) * double(forward[1]);
    const double x = double(speed[0]) * double(forward[0]);
    volatile float result = float((z + y) + x);
    return result;
}
// CBike::ProcessControlInputs (0x6BE98C..0x6BE9F0): len = sqrt((x*x + y*y) + z*z) (24 bits, sin guardar); si len > 0.28f (0x871254): speed *= limit / len (factor sin guardar; cada producto a float).
__declspec(noinline) void __cdecl BikeSpeedCapCpp(float* speed, float limit) {
    const double x = speed[0], y = speed[1], z = speed[2];
    const double length = BikeSqrtFull((x * x + y * y) + z * z);
    if (length > double(limit)) {
        const double factor = double(limit) / length;
        speed[0] = float(factor * x);
        speed[1] = float(factor * y);
        speed[2] = float(factor * z);
    }
}

// CBike::ProcessControl (0x6B9C04..0x6B9C29), moto parada sobre el caballete: asin(v) (_CIasin, sin redondear) + 0.2617994f (0x858F1C); (1 - keep) * eso; keep * desired - eso.
__declspec(noinline) float __cdecl BikeStoppedLeanCpp(float keep, float desired, float sine) {
    static const volatile float kOffset = std::bit_cast<float>(0x3E860A92u);   // 0x858F1C
    static const volatile float kOne = 1.0f;                                   // 0x858624
    const double carried = double(keep) * double(desired);
    const double shifted = ExeAsinDoubleCpp(double(sine)) + double(kOffset);
    const double rest = double(kOne) - double(keep);
    volatile float result = float(carried - rest * shifted);
    return result;
}
// CBike::ProcessControl (0x6BB991..0x6BB9BA), caballete + conductor: (asin(rz) + offset) + 0.2617994f; carried - (1 - keep) * eso.
__declspec(noinline) float __cdecl BikeSideStandLeanCpp(float keep, float desired, float rightZ, float offset) {
    static const volatile float kOffset = std::bit_cast<float>(0x3E860A92u);   // 0x858F1C
    static const volatile float kOne = 1.0f;                                   // 0x858624
    const double carried = double(keep) * double(desired);
    const double shifted = (ExeAsinDoubleCpp(double(rightZ)) + double(offset)) + double(kOffset);
    const double rest = double(kOne) - double(keep);
    volatile float result = float(carried - rest * shifted);
    return result;
}
// CBike::ProcessControl (0x6BBB7C..0x6BBBA1), inclinacion libre: (asin(y) - offset) * (1 - keep) + keep * desired.
__declspec(noinline) float __cdecl BikeFreeLeanCpp(float keep, float desired, float sine, float offset) {
    static const volatile float kOne = 1.0f;                                   // 0x858624
    const double shifted = ExeAsinDoubleCpp(double(sine)) - double(offset);
    const double rest = double(kOne) - double(keep);
    const double weighted = shifted * rest;
    const double carried = double(keep) * double(desired);
    volatile float result = float(weighted + carried);
    return result;
}
// CBike::ProcessControl (0x6BAB8E..0x6BAC16), manillar: v = asin(y) / (steeringLock * 0.017453292f); si doubleIt: v + v; si 1.0f < v: v = 1.0f; result = v * steer.
__declspec(noinline) float __cdecl BikeSteerBarCpp(float sine, float steeringLock, float steer, bool doubleIt) {
    static const volatile float kRadians = std::bit_cast<float>(0x3C8EFA35u);  // 0x8595EC
    static const volatile float kOne = 1.0f;                                   // 0x858624
    const double scaledLock = double(steeringLock) * double(kRadians);
    double value = ExeAsinDoubleCpp(double(sine)) / scaledLock;
    if (doubleIt) {
        value = value + value;
    }
    if (double(kOne) < value) {
        value = double(kOne);
    }
    volatile float result = float(value * double(steer));
    return result;
}
#pragma float_control(pop)

// --- CPlane (Plane.cpp; lote w20, prefijo Plane). Colocar al FINAL de X87Math.cpp (usa UnsignedAsX87, ExeCompatiblePowCpp y ExeAsinDoubleCpp, ya definidos arriba). ---
// La x87 corre a 24 bits: cada operacion double() de una cadena equivale al fmul/fadd/fsub del EXE (un redondeo por operacion, exponente extendido entre ellas);
// solo se guarda como float (volatile float) donde el EXE hace fstp/fst dword. Las direcciones entre parentesis son las del EXE de GTA SA 1.0 (gta-sa.exe).
// sqrt y sin van en funciones aparte SIN float_control(precise) (con precise pasarian a llamadas a la CRT) y devuelven ST0.
static __declspec(noinline) double PlaneSqrtExtCpp(double value) { return sqrt(value); }
static __declspec(noinline) double PlaneSinExtCpp(double value) { return sin(value); }

// fabs sobre un float cargado con fld (el bit de signo se limpia; para las comparaciones el signo de un NaN no importa).
static __forceinline double PlaneFabsFloat(float value) {
    return double(std::bit_cast<float>(std::bit_cast<unsigned int>(value) & 0x7FFFFFFFu));
}

#pragma float_control(precise, on, push)

// --- CPlane::VehicleDamage (0x6CC4B0) ---
// 0x6CC575..0x6CC5B8: v = mass / scale (fdiv); si minimumOne y 1.0 > v (ordenado) v = 1.0; resultado = v * threshold (un solo fstp). Con NaN no sustituye.
__declspec(noinline) float __cdecl PlaneMassThresholdCpp(float mass, float scale, float threshold, bool minimumOne) {
    double value = double(mass) / double(scale);
    if (minimumOne && 1.0 > value) { // fld 1.0; fcomp st(1); test ah,41h; jne -> solo sustituye si 1.0 > v
        value = 1.0;
    }
    volatile float result = float(value * double(threshold));
    return result;
}

// 0x6CC642..0x6CC6CE: health = health - (damage * factor) (fld damage; fmul factor; fsubr [health]; fstp [health]).
__declspec(noinline) float __cdecl PlaneHealthAfterDamageCpp(float health, float damage, float factor) {
    volatile float result = float(double(health) - double(damage) * double(factor));
    return result;
}

// 0x6CC7E3..0x6CC839 (bucle de componentes 12..24): delta = float(nodePosition - relative) por eje (fsub + fstp), CVector::SquaredMagnitude (0x406DA0) = (x² + y²) + z²,
// closest² = closest · closest; si suma < closest² (comparacion ordenada: fcompp; test ah,5; jp) closest = float(sqrt(suma)) y devuelve true (se elige el componente).
__declspec(noinline) bool __cdecl PlaneSelectCloserCpp(const float* nodePosition, const float* relative, float* closest) {
    volatile float dx = float(double(nodePosition[0]) - double(relative[0]));
    volatile float dy = float(double(nodePosition[1]) - double(relative[1]));
    volatile float dz = float(double(nodePosition[2]) - double(relative[2]));
    const double sum = (double(dx) * double(dx) + double(dy) * double(dy)) + double(dz) * double(dz);
    const double limit = double(*closest) * double(*closest);
    if (sum < limit) {
        *closest = float(PlaneSqrtExtCpp(sum));
        return true;
    }
    return false;
}

// --- CPlane::ProcessControlInputs (0x6CADD0) ---
// 0x6CAF1C..0x6CAF3C (y 0x6CB05B..): ((float(input) · scale) - old) · step · factor + old; fild exacto, un redondeo por operacion, un fstp.
__declspec(noinline) float __cdecl PlaneControlBlendCpp(float oldValue, int input, float scale, float step, float factor) {
    volatile float result = float(((double(input) * double(scale) - double(oldValue)) * double(step)) * double(factor) + double(oldValue));
    return result;
}

// 0x6CADDB..0x6CADFD: velocidad · direccion = (z·z' + y·y') + x·x' (fmul/faddp en ese orden); fstp float.
__declspec(noinline) float __cdecl PlaneForwardSpeedCpp(const float* speed, const float* forward) {
    volatile float result = float((double(speed[2]) * double(forward[2]) + double(speed[1]) * double(forward[1])) + double(speed[0]) * double(forward[0]));
    return result;
}

// 0x6CB1B1..0x6CB25A (y 0x6CB0A5): target = fild(eje) · escala (hecho por el llamador); v = ((target - old) · step) · factor + old; v limitado a [-1, 1]:
// si !(v > -1) -> -1 (NaN incluido); si !(v < 1) -> 1; si no v. Un fstp float.
__declspec(noinline) float __cdecl PlaneInputSkidCpp(float oldValue, float target, float step, float factor) {
    const double value = ((double(target) - double(oldValue)) * double(step)) * double(factor) + double(oldValue);
    float out;
    if (!(value > -1.0)) {
        out = -1.0f;
    } else if (!(value < 1.0)) {
        out = 1.0f;
    } else {
        out = float(value);
    }
    volatile float result = out;
    return result;
}

// 0x6CB260..0x6CB2A1: m = 1.0 - speed (si minimum > m, m = minimum); angulo = -(m · ((steeringLock · radians) · skid)).
__declspec(noinline) float __cdecl PlaneGroundSteerCpp(float speed, float degrees, float skid, float minimum, float radians) {
    double factor = 1.0 - double(speed);
    if (double(minimum) > factor) {
        factor = double(minimum);
    }
    volatile float result = float(-(factor * ((double(degrees) * double(radians)) * double(skid))));
    return result;
}

// 0x6CB55C..0x6CB5BB: fild(eje) · step · rate · scale -> _ftol2 (64 bits); el llamador usa el entero de 16 bits bajo (movsx).
__declspec(noinline) int __cdecl PlaneNozzleChangeCpp(int axis, float step, float rate, float scale) {
    return static_cast<int>(static_cast<long long>(((double(axis) * double(step)) * double(rate)) * double(scale)));
}

// 0x6CB3AD..0x6CB4F0: *gear = float(step · factor + *gear) (fst dword); devuelve true si el valor (ST0) >= limit (fcomp; test ah,1; jne -> no con C0, tampoco con NaN).
__declspec(noinline) bool __cdecl PlaneGearAdvanceCpp(float* gear, float step, float factor, float limit) {
    const double value = double(step) * double(factor) + double(*gear);
    *gear = float(value);
    return value >= double(limit);
}

// 0x6CB68D..0x6CB6B8: sqrt((x² + y²) + z²) < limit (fsqrt; fcomp; test ah,5; jp -> solo "menor que" ordenado).
__declspec(noinline) bool __cdecl PlaneSpeedBelowCpp(const float* speed, float limit) {
    const double x = speed[0], y = speed[1], z = speed[2];
    return PlaneSqrtExtCpp((x * x + y * y) + z * z) < double(limit);
}

// 0x6CB742..0x6CB7A1: l = sqrt((x² + y²) + z²); si l > limit (fcom; test ah,41h; jne) s = limit / l (fdivr) y cada componente = float(s · componente) (los tres productos usan el valor original).
__declspec(noinline) void __cdecl PlaneSpeedCapCpp(float* speed, float limit) {
    const double x = speed[0], y = speed[1], z = speed[2];
    const double length = PlaneSqrtExtCpp((x * x + y * y) + z * z);
    if (length > double(limit)) {
        const double scale = double(limit) / length;
        volatile float scaledX = float(scale * x);
        volatile float scaledY = float(scale * y);
        volatile float scaledZ = float(scale * z);
        speed[0] = scaledX;
        speed[1] = scaledY;
        speed[2] = scaledZ;
    }
}

// 0x6CB48D..0x6CB499: gear = float((drag / divisor) · factor).
__declspec(noinline) float __cdecl PlaneGearResistanceCpp(float drag, float divisor, float factor) {
    volatile float result = float((double(drag) / double(divisor)) * double(factor));
    return result;
}

// --- CPlane::ProcessFlyingCarStuff (0x6CB7C0) ---
// 0x6CB7E0..0x6CB85B: upper = float(spread + 1.0), lower = float(1.0 - spread); r = fild(random) · randomScale; v = ((upper - lower) · r) + lower;
// n = _ftol2(step · timeScale · countScale) (eax; fild de 32 bits, +2^32 si negativo); resultado = low32(_ftol2(v · n)).
__declspec(noinline) unsigned int __cdecl PlaneWaveAdvanceCpp(float spread, float step, int random, float randomScale, float timeScale, float countScale) {
    volatile float upperStore = float(double(spread) + 1.0);
    volatile float lowerStore = float(1.0 - double(spread));
    const double fraction = double(random) * double(randomScale);
    const double value = ((double(upperStore) - double(lowerStore)) * fraction) + double(lowerStore);
    const int count = static_cast<int>(static_cast<long long>((double(step) * double(timeScale)) * double(countScale)));
    double countValue = double(count);
    if (count < 0) {
        countValue += double(4294967296.0f); // 0x858C54
    }
    return static_cast<unsigned int>(static_cast<long long>(value * countValue));
}

// 0x6CB861..0x6CB8C6: s = sqrt((x² + y²) + z²) · 3.0; si s > 1.0 (fcomp; test ah,41h; jne) s = 1.0; fstp float.
__declspec(noinline) float __cdecl PlaneDamageSpeedCpp(const float* speed) {
    const double x = speed[0], y = speed[1], z = speed[2];
    double value = PlaneSqrtExtCpp((x * x + y * y) + z * z) * double(std::bit_cast<float>(0x40400000u)); // 0x858B3C = 3.0f
    if (value > 1.0) {
        value = 1.0;
    }
    volatile float result = float(value);
    return result;
}

// 0x6CBB01..0x6CBB3C (y variantes 0x6CBC0D, 0x6CBD32): fraccion = fild(random) · randomScale; negativo = float(-amplitude) (fchs; fstp);
// ruido = ((amplitude - negativo) · fraccion) + negativo; resultado = float(((((ruido · dañoCuadrado) · step) · speed) + old)).
__declspec(noinline) float __cdecl PlaneDamageNoiseCpp(float oldValue, float amplitude, int random, float squaredDamage, float step, float speed, float randomScale) {
    const double fraction = double(random) * double(randomScale);
    volatile float negative = float(-double(amplitude));
    const double noise = (double(amplitude) - double(negative)) * fraction + double(negative);
    volatile float result = float(((((noise * double(squaredDamage)) * double(step)) * double(speed)) + double(oldValue)));
    return result;
}

// 0x6CBA08..0x6CBAA7 (propulsor): upper/lower se calculan con PlaneFlightBoundCpp. fraccion = fild(random) · randomScale; fase = (fild(onda sin signo) · 2π) / fild(periodo sin signo);
// v = (sin(fase) - 1.0) · (((upper - lower) · fraccion) + lower); resultado = float(((((v · daño) · daño) · step) · amount) + old). fsin sin redondear.
__declspec(noinline) float __cdecl PlanePropellerWaveCpp(float oldValue, float upper, float lower, int random, int damage, unsigned int wave, unsigned int period, float step, float randomScale, float circle, float amount) {
    const double fraction = double(random) * double(randomScale);
    const double damageValue = double(damage);
    const double phase = (UnsignedAsX87(wave) * double(circle)) / UnsignedAsX87(period);
    const double wavy = PlaneSinExtCpp(phase) - 1.0;
    const double amplitude = (double(upper) - double(lower)) * fraction + double(lower);
    const double product = wavy * amplitude;
    volatile float result = float(((((product * damageValue) * damageValue) * double(step)) * double(amount)) + double(oldValue));
    return result;
}

// 0x6CBABA..0x6CBAE2 (timon) y 0x6CBBD7.. (elevadores / alerones): factor = fild(daño) · scale; si storeFactor se guarda como float (fstp + fld); resultado = float((1.0 - factor) · old).
__declspec(noinline) float __cdecl PlaneFlightDecayCpp(float oldValue, int damage, float scale, bool storeFactor) {
    double factor = double(damage) * double(scale);
    if (storeFactor) {
        volatile float stored = float(factor);
        factor = double(stored);
    }
    volatile float result = float((1.0 - factor) * double(oldValue));
    return result;
}

// 0x6CBBA7..0x6CBBCA (y equivalentes): a · b; restar -> float(value - a·b) (fsubr); sumar -> float(a·b + value) (fadd).
__declspec(noinline) float __cdecl PlaneFlightProductAddCpp(float value, float a, float b, bool subtract) {
    const double product = double(a) * double(b);
    volatile float result = float(subtract ? double(value) - product : product + double(value));
    return result;
}

// 0x6CC2CE..0x6CC2F5: float(((target - value) · step) · factor + value).
__declspec(noinline) float __cdecl PlaneFloatBlendCpp(float value, float target, float step, float factor) {
    volatile float result = float(((double(target) - double(value)) * double(step)) * double(factor) + double(value));
    return result;
}

// 0x6CBE6C..0x6CBE9D: distancia² = (dz² + dy²) + dx² con d = float(first - second) en ST0 (sin guardar); true si es menor que threshold (comparacion ordenada).
__declspec(noinline) bool __cdecl PlaneDistanceBelowCpp(const float* first, const float* second, float threshold) {
    const double dx = double(first[0]) - double(second[0]);
    const double dy = double(first[1]) - double(second[1]);
    const double dz = double(first[2]) - double(second[2]);
    return ((dz * dz + dy * dy) + dx * dx) < double(threshold);
}

// 0x6CB92F..0x6CB962: roll = CPlaceable::GetRoll() (0x420B30, deja el fpatan sin redondear en ST0: rollFn se llama como __fastcall con ECX = plane); |roll| contra threshold.
// greater: |roll| > threshold (fcomp; test ah,41h; jne); si no |roll| < threshold (test ah,5; jp). El signo de un NaN y de -0 no cambian la comparacion.
__declspec(noinline) bool __cdecl PlaneRollCompareCpp(float(__fastcall* rollFn)(const void*, void*), const void* plane, float threshold, bool greater) {
    double roll = double(rollFn(plane, nullptr));
    if (roll < 0.0) {
        roll = -roll;
    }
    return greater ? (roll > double(threshold)) : (roll < double(threshold));
}

// 0x6CC25A..0x6CC286: h = height · scale (0x858B38); si 0.0 > h (ordenado) h = 0.0; target = float(h · target).
__declspec(noinline) float __cdecl PlaneCeilingTargetCpp(float height, float target, float scale) {
    double limited = double(height) * double(scale);
    if (0.0 > limited) {
        limited = 0.0;
    }
    volatile float result = float(limited * double(target));
    return result;
}

// 0x6CC2A0..0x6CC2C8: v = accel - (height · adjust) (fmul; fsubr); si -1.0 > v (ordenado) v = -1.0; fstp float.
__declspec(noinline) float __cdecl PlaneCeilingAccelerationCpp(float height, float acceleration, float adjust) {
    double value = double(acceleration) - double(height) * double(adjust);
    if (-1.0 > value) {
        value = -1.0;
    }
    volatile float result = float(value);
    return result;
}

// 0x6CBA08 / 0x6CBA14: subtract -> float(1.0 - spread); si no float(spread + 1.0).
__declspec(noinline) float __cdecl PlaneFlightBoundCpp(float spread, bool subtract) {
    volatile float result = float(subtract ? 1.0 - double(spread) : double(spread) + 1.0);
    return result;
}

// 0x6CBF20..0x6CBF75: out[0] = float((v.x · quarter) · multiplier), out[1] igual, out[2] = float(float(v.z · quarter) · multiplier) (la z pasa por memoria).
__declspec(noinline) void __cdecl PlaneSmokeParticleVelocityCpp(float* out, const float* speed, float quarter, float multiplier) {
    volatile float x = float((double(speed[0]) * double(quarter)) * double(multiplier));
    volatile float y = float((double(speed[1]) * double(quarter)) * double(multiplier));
    volatile float roundedZ = float(double(speed[2]) * double(quarter));
    volatile float z = float(double(roundedZ) * double(multiplier));
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

// 0x6CBF7B..0x6CBF9E (y x2): float(((fild(random) · randomScale) · spread + base) · value).
__declspec(noinline) float __cdecl PlaneVelocityNoiseCpp(float value, int random, float randomScale, float spread, float base) {
    volatile float result = float(((double(random) * double(randomScale)) * double(spread) + double(base)) * double(value));
    return result;
}

// 0x6CC00A..0x6CC045 (y x2): negativo = float(-strength) (fchs; fst; fstp); fraccion = fild(random) · randomScale; resultado = float((((strength - negativo) · fraccion) + negativo) + value).
__declspec(noinline) float __cdecl PlanePositionNoiseCpp(float value, float strength, int random, float randomScale) {
    volatile float negative = float(-double(strength));
    const double fraction = double(random) * double(randomScale);
    const double noise = (double(strength) - double(negative)) * fraction + double(negative);
    volatile float result = float(noise + double(value));
    return result;
}

// 0x6CC1A6..0x6CC1FC: target = standard; accel > 0 -> float((maximum - standard) · accel + standard); accel < 0 -> float((standard - minimum) · accel + standard); si no standard.
__declspec(noinline) float __cdecl PlanePropellerTargetCpp(float acceleration, float standard, float maximum, float minimum) {
    volatile float result = standard;
    if (acceleration > 0.0f) {
        result = float((double(maximum) - double(standard)) * double(acceleration) + double(standard));
    } else if (acceleration < 0.0f) {
        result = float((double(standard) - double(minimum)) * double(acceleration) + double(standard));
    }
    return result;
}

// 0x6CC381..0x6CC3AB: ((x² + y²) + z²) > threshold (double del EXE en 0x859068; fcomp qword; test ah,41h; jne -> comparacion ordenada).
__declspec(noinline) bool __cdecl PlaneSpeedSquaredAboveCpp(const float* speed, double threshold) {
    const double x = speed[0], y = speed[1], z = speed[2];
    return ((x * x + y * y) + z * z) > threshold;
}

// --- CPlane::PreRender (0x6C94A0) ---
// 0x6C9532..0x6C955F: ratio = spring / line (fdiv); hueco = 1.0 - ratio (fsubr); L = (compression - hueco) / (1.0 - hueco); fstp float.
__declspec(noinline) float __cdecl PlaneWheelCompressionCpp(float springLength, float lineLength, float compression) {
    const double ratio = double(springLength) / double(lineLength);
    const double gap = 1.0 - ratio;
    const double numerator = double(compression) - gap;
    const double denominator = 1.0 - gap;
    volatile float result = float(numerator / denominator);
    return result;
}

// 0x6C9564..0x6C95B6: z = posZ + lowerLimit; si L > 0.0 z = z - L · spring (fsubp); si !(z > old) (fcom [edi]; test ah,41h; je) y !snap: z = (z - old) · 0.75 + old; wheelPosition = float(z).
__declspec(noinline) float __cdecl PlaneWheelHeightCpp(float posZ, float lowerLimit, float compression, float springLength, float oldHeight, bool snap) {
    double z = double(posZ) + double(lowerLimit);
    if (compression > 0.0f) {
        z = z - double(compression) * double(springLength);
    }
    if (!(z > double(oldHeight)) && !snap) {
        z = (z - double(oldHeight)) * double(std::bit_cast<float>(0x3F400000u)) + double(oldHeight); // 0x858F34 = 0.75f
    }
    volatile float result = float(z);
    return result;
}

// 0x6C95C4..0x6C9652 (Andromada 592): comp >= 1 (o NaN) -> rotacion · 0.95 (0x858EF0); si no dot = (f.z·n.z + n.x·f.x) + f.y·n.y limitado a [-1, 1] (se guarda como float) y
// rotacion = float(-asin(dot)) con el _CIasin del EXE (ExeAsinDoubleCpp); fchs sobre ST0 sin redondear.
__declspec(noinline) float __cdecl PlaneAndromadaWheelRotationCpp(float rotation, float compression, const float* normal, const float* forward) {
    if (!(compression < 1.0f)) {
        volatile float scaled = float(double(rotation) * double(std::bit_cast<float>(0x3F733333u))); // 0x858EF0 = 0.95f
        return scaled;
    }
    const double dot = (double(forward[2]) * double(normal[2]) + double(normal[0]) * double(forward[0])) + double(forward[1]) * double(normal[1]);
    float argument;
    if (-1.0 > dot) {
        argument = -1.0f;
    } else if (1.0 < dot) {
        argument = 1.0f;
    } else {
        volatile float stored = float(dot);
        argument = stored;
    }
    volatile float result = float(-ExeAsinDoubleCpp(double(argument)));
    return result;
}

// 0x6C96A5..0x6C96E3: v = step · propSpeed + angle (fmul; fadd; fst dword); si v > fullCircle (2π, ordenado) se resta fullCircle (al menos una vez) mientras v > fullCircle; fstp float.
__declspec(noinline) float __cdecl PlanePropAngleCpp(float timeStep, float propSpeed, float angle, float fullCircle) {
    double value = double(timeStep) * double(propSpeed) + double(angle);
    volatile float stored = float(value);
    if (value > double(fullCircle)) {
        value = double(stored);
        do {
            value -= double(fullCircle);
        } while (value > double(fullCircle));
        stored = float(value);
    }
    return stored;
}

// 0x6C987C..0x6C98AE: g = 1.0 - |gear| (fabs; fsubr); *scale = float(g · half + 1.0); *offset = float(g · third).
__declspec(noinline) void __cdecl PlaneGearFactorsCpp(float gear, float half, float third, float* scale, float* offset) {
    const double gap = 1.0 - PlaneFabsFloat(gear);
    volatile float scaleStore = float(gap * double(half) + 1.0);
    volatile float offsetStore = float(gap * double(third));
    *scale = scaleStore;
    *offset = offsetStore;
}

// Angulo de una superficie de control: ((amplitude · radians) · control) y, segun operation: 0 nada, 1 + addend, 2 - addend, 3 cambio de signo (fchs); fstp float.
// Usos: timon 0x6C97AB..0x6C97C9 (0), aleron 0x6C9956..0x6C9971 (+offset), segundo aleron (-offset) y los dos elevadores (cambio de signo / sin cambio) en 0x6C9A2A..0x6C9C94.
__declspec(noinline) float __cdecl PlaneSurfaceAngleCpp(float amplitude, float radians, float control, float addend, int operation) {
    double angle = (double(amplitude) * double(radians)) * double(control);
    if (operation == 1) {
        angle = angle + double(addend);
    } else if (operation == 2) {
        angle = angle - double(addend);
    } else if (operation == 3) {
        angle = -angle;
    }
    volatile float result = float(angle);
    return result;
}

// 0x6C9775..0x6C979B (y copias): vector[i] = float(vector[i] - position[i]) para x, y, z (fld; fsub; fstp).
__declspec(noinline) void __cdecl PlaneSubtractPositionCpp(float* vector, const float* position) {
    volatile float x = float(double(vector[0]) - double(position[0]));
    volatile float y = float(double(vector[1]) - double(position[1]));
    volatile float z = float(double(vector[2]) - double(position[2]));
    vector[0] = x;
    vector[1] = y;
    vector[2] = z;
}

// 0x6C99EA..0x6C9A0E (y 0x6C9AEA): y' = float((((1.0 - finScale) · boxMaxY) · factor) + y).
__declspec(noinline) float __cdecl PlaneFinOffsetCpp(float finScale, float boxMaxY, float y, float factor) {
    volatile float result = float(((1.0 - double(finScale)) * double(boxMaxY)) * double(factor) + double(y));
    return result;
}

// 0x6C9CBB..0x6C9CCC / 0x6C9D16..0x6C9D27: v = direction · propAngle; doubled -> float(v + v) (fadd st,st); si no float(-v) (fchs).
__declspec(noinline) float __cdecl PlanePropRotationCpp(float direction, float propAngle, bool doubled) {
    double value = double(direction) * double(propAngle);
    value = doubled ? value + value : -value;
    volatile float result = float(value);
    return result;
}

// 0x6C9E49..0x6C9EDE: float(|gear| · value) (fabs; fmul; fstp).
__declspec(noinline) float __cdecl PlaneGearRotationCpp(float gear, float value) {
    volatile float result = float(PlaneFabsFloat(gear) * double(value));
    return result;
}

// 0x6C9F40..0x6C9F52 (Hydra): float((fild(misc) · halfPi) / fidiv(limit)).
__declspec(noinline) float __cdecl PlaneNozzleAngleCpp(int misc, float halfPi, int limit) {
    volatile float result = float((double(misc) * double(halfPi)) / double(limit));
    return result;
}

// 0x6C9F83..0x6C9FA5 (Hydra): float((accel + 1.0) · 0.5).
__declspec(noinline) float __cdecl PlaneDustTimeCpp(float acceleration, float one, float half) {
    volatile float result = float((double(acceleration) + double(one)) * double(half));
    return result;
}

// 0x6CA6A9..0x6CA71E: out[i] = float(a[i] + b[i]) (fld a; fadd b; fstp) para i = 0..2 (velocidad + velocidad de friccion / giro + giro de friccion).
__declspec(noinline) void __cdecl PlaneSumVector3Cpp(const float* a, const float* b, float* out) {
    volatile float x = float(double(a[0]) + double(b[0]));
    volatile float y = float(double(a[1]) + double(b[1]));
    volatile float z = float(double(a[2]) + double(b[2]));
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

// 0x6CA768..0x6CA7AF: alfa de la estela = low32(_ftol2(v)) con v = |(((vz·az + vy·ay) + ax·vx) · attackLift) · scale| - offset, limitado por abajo a 0.0 (0.0 > v -> 0.0). El llamador usa el byte bajo.
__declspec(noinline) unsigned int __cdecl PlaneStreakAlphaCpp(const float* speed, const float* up, float attackLift, float scale, float offset) {
    const double dot = (double(speed[2]) * double(up[2]) + double(speed[1]) * double(up[1])) + double(up[0]) * double(speed[0]);
    double value = (dot * double(attackLift)) * double(scale);
    if (value < 0.0) {
        value = -value;
    }
    value = value - double(offset);
    if (0.0 > value) {
        value = 0.0;
    }
    return static_cast<unsigned int>(static_cast<long long>(value));
}

// 0x6CA9AD..0x6CA9D6 (Cropduster) y 0x6CAA39..0x6CAA58 (Stunt): out = {x · 10, y · 10, z · 10 [- 3.0 si subtractOffset]}: cada eje es fld; fmul; fstp (z, ademas, fsub 3.0 antes del fstp).
__declspec(noinline) void __cdecl PlaneSmokeVelocityCpp(const float* speed, float scale, float zOffset, bool subtractOffset, float* out) {
    volatile float x = float(double(speed[0]) * double(scale));
    volatile float y = float(double(speed[1]) * double(scale));
    double z = double(speed[2]) * double(scale);
    if (subtractOffset) {
        z = z - double(zOffset);
    }
    volatile float zStore = float(z);
    out[0] = x;
    out[1] = y;
    out[2] = zStore;
}

// fld; fchs; fstp: -value con el redondeo/aviso del fld (un NaN senalizante se silencia).
__declspec(noinline) float __cdecl PlaneNegateCpp(float value) {
    volatile float result = float(-double(value));
    return result;
}

// Vortex (539), 0x6CA33D..0x6CA633: inclinacion del nodo 23 sobre la matriz temporal (right.z, forward.z, up.x, up.y, up.z). axis.v = ((a.z·v.z + a.y·v.y) + a.x·v.x) · 2.0 limitado a [-0.3, 0.3]
// (v > 0.3 -> 0.3; v < -0.3 -> -0.3; NaN pasa), forward -> up.y, right -> up.x. P = pow(0.9, step) (_CIpow, ST0 sin redondear); 1 - P se guarda como float; cada elemento nuevo = old · P + (1 - P) · R
// con R1 = clamp(((w2 - w3) + (w0 - w1)) · 0.5 / (l0y - l1y), ±0.1), R2 = clamp(((w3 - w1) + (w2 - w0)) · 0.5 / (l3x - l1x), ±0.1), R3 = min(1.0 - ((((w3 + w1) + w2) + w0) · 0.25 - wheelZ) / (wheelSize · 0.5), 1.3).
// Elementos: R1 -> forward.z, R2 -> right.z, R3 -> up.z. `lines` = CColLine[4] de 0x20 bytes (start.x en [0], start.y en [1]).
__declspec(noinline) void __cdecl PlaneHoverTiltCpp(float* rightZ, float* forwardZ, float* upX, float* upY, float* upZ, const float* matrixRight, const float* matrixForward, const float* velocity,
                                                    const float* wheelPositions, const float* lines, float wheelZ, float wheelSize, float timeStep) {
    const double limitUpper = double(std::bit_cast<float>(0x3E99999Au)); // 0x8D3414 = 0.3f
    const double doubleFactor = double(std::bit_cast<float>(0x40000000u)); // 0x8D3418 = 2.0f
    const double forwardDot = ((double(matrixForward[2]) * double(velocity[2]) + double(matrixForward[1]) * double(velocity[1])) + double(matrixForward[0]) * double(velocity[0])) * doubleFactor;
    const double rightDot = ((double(velocity[2]) * double(matrixRight[2]) + double(velocity[1]) * double(matrixRight[1])) + double(matrixRight[0]) * double(velocity[0])) * doubleFactor;
    {
        volatile float forwardClamp = float(forwardDot > limitUpper ? limitUpper : (forwardDot < -limitUpper ? -limitUpper : forwardDot));
        volatile float rightClamp = float(rightDot > limitUpper ? limitUpper : (rightDot < -limitUpper ? -limitUpper : rightDot));
        *upY = forwardClamp;
        *upX = rightClamp;
    }
    const double power = ExeCompatiblePowCpp(double(std::bit_cast<float>(0x3F666666u)), double(timeStep)); // 0x8D3410 = 0.9f
    volatile float oneMinus = float(1.0 - power);
    const double limitSmall = double(std::bit_cast<float>(0x3DCCCCCDu)); // 0x8D340C / 0x8D3408 = 0.1f
    const double half = double(std::bit_cast<float>(0x3F000000u));        // 0x858B8C
    const double quarter = double(std::bit_cast<float>(0x3E800000u));     // 0x858C84
    const double w0 = wheelPositions[0], w1 = wheelPositions[1], w2 = wheelPositions[2], w3 = wheelPositions[3];

    double first = (((w2 - w3) + (w0 - w1)) * half) / (double(lines[1]) - double(lines[9]));
    if (first > limitSmall) {
        first = limitSmall;
    } else if (first < -limitSmall) {
        first = -limitSmall;
    }
    {
        volatile float value = float(double(*forwardZ) * power + double(oneMinus) * first);
        *forwardZ = value;
    }
    double second = (((w3 - w1) + (w2 - w0)) * half) / (double(lines[24]) - double(lines[8]));
    if (second > limitSmall) {
        second = limitSmall;
    } else if (second < -limitSmall) {
        second = -limitSmall;
    }
    {
        volatile float value = float(double(*rightZ) * power + double(oneMinus) * second);
        *rightZ = value;
    }
    double third = 1.0 - ((((w3 + w1) + w2) + w0) * quarter - double(wheelZ)) / (double(wheelSize) * half);
    if (third > double(std::bit_cast<float>(0x3FA66666u))) { // 0x8D3404 = 1.3f
        third = double(std::bit_cast<float>(0x3FA66666u));
    }
    {
        volatile float value = float(double(*upZ) * power + double(oneMinus) * third);
        *upZ = value;
    }
}

#pragma float_control(pop)

// --- Lote CStats (statsfix3) ---
// ECX/EDX de entrada como resultado de 64 bits: lo llama Stats.cpp con el prototipo `uint64(__fastcall*)()` (sin argumentos) justo despues de un colaborador de la DLL
// para conservar los registros con que este sale. Sin pila ni marco: `mov eax,ecx; ret` (EDX no se toca).
__declspec(noinline) unsigned long long __fastcall StatsCaptureEcxEdxCpp(unsigned int ecx, unsigned int edx) {
    // El resultado de 64 bits es EDX:ECX; la mitad alta ya esta en EDX, asi que el compilador solo emite `mov eax,ecx; ret` (devolver `ecx` a secas anadiria `xor edx,edx`).
    return (static_cast<unsigned long long>(edx) << 32) | ecx;
}

#pragma warning(push)
#pragma warning(disable : 4716)
__declspec(noinline) unsigned int __fastcall StatsCaptureEaxCpp() {
}
#pragma warning(pop)

static bool StatsCaptureEaxIsTrivial() {
    const unsigned char* code = FollowIncrementalThunks(reinterpret_cast<const unsigned char*>(&StatsCaptureEaxCpp));
    static const unsigned char plain[]     = {0xC2, 0x00, 0x00};                         // ret 0
    static const unsigned char withFrame[] = {0x55, 0x8B, 0xEC, 0x5D, 0xC2, 0x00, 0x00}; // push ebp; mov ebp,esp; pop ebp; ret 0
    return code[0] == 0xC3 || memcmp(code, plain, sizeof plain) == 0 || memcmp(code, withFrame, sizeof withFrame) == 0;
}

static bool StatsCaptureEcxEdxIsTrivial() {
    static const unsigned char movEax[] = {0x8B, 0xC1, 0xC3};            // mov eax,ecx; ret
    static const unsigned char orEax[]  = {0x33, 0xC0, 0x0B, 0xC1, 0xC3}; // xor eax,eax; or eax,ecx; ret (misma semantica, EDX intacto)
    const unsigned char* p = FollowIncrementalThunks(reinterpret_cast<const unsigned char*>(&StatsCaptureEcxEdxCpp));
    return memcmp(p, movEax, sizeof movEax) == 0 || memcmp(p, orEax, sizeof orEax) == 0;
}
static const bool s_statsCaptureChecked = (assert(StatsCaptureEaxIsTrivial() && StatsCaptureEcxEdxIsTrivial() && "StatsCaptureEcxEdxCpp ya no es `mov eax,ecx; ret`: revisar X87Math.cpp"), true);

// --- CStats::UpdateSexAppealStat (0x55BF20, Stats.cpp, w20), prefijo SexAppeal. Anadir al FINAL de X87Math.cpp (despues de s_statsCaptureChecked) ---
// Todo es noinline y se compila con /O2 /Oi /arch:IA32 /fp:fast. Cada operacion se redondea a 24 bits (PC24 del proceso); lo que el EXE deja sin guardar en memoria
// (distancia, parte del vehiculo, suma final) sigue en doble hasta su comparacion o su fstp, y lo que el EXE guarda como float (apariencia/2, 2*salud-1000, el resultado)
// ESTADO FNSAVE: el banco exige la pila x87 entera tras la funcion (TOP, FCW, FSW con C0-C3 y banderas acumuladas, FTW, FOP, FDP y el contenido crudo de los 8 registros ST, tambien los
// vacios), asi que cada ayudante deja en ella los MISMOS valores muertos que el tramo del EXE (0x55BF23..0x55C093):
//  - Half: fld; fmul [0.5f]; fstp dword.
//  - Near: fld x; fsub; fld y; fsub; fld st; fmul st,st(1); fld st(2); fmul st,st(3); faddp; fsqrt; fcomp [35.0f]: deja dy, la distancia y dx*dx en los registros 2, 3 y 4 de la pila (vacios).
//    s_sexAppealSinkA y las dos ramas con sumideros distintos obligan a MSVC a conservar dx y dy vivos hasta despues de la raiz y a elevar dy*dy antes que dx*dx (si no, eleva los cuadrados
//    en el sitio o en el orden contrario y el residuo cambia).
//  - VehiclePart: fld; fadd st,st; fsub [1000.0f]; fstp dword [local]; fld [1000.0f]; fcomp [local]; ...: el cero y el mil se leen de variables volatile para que el compilador no los suba a
//    registros ni los combine (fld 0.0 con el valor vivo deja un 0.0 en el registro 2, como el EXE) y el local se lee de memoria en cada comparacion (fcomp dword [mem], sin empujar el local).
//  - Finish: fld; fmul [0.5]; fadd [half]; fcom [0.0]; fcom [1000.0]; las sustituciones 0.0/1000.0 se cargan con fild de un entero volatile (mismo valor exacto, sin hoisting) y la comparacion
//    se escribe con el valor como primer operando (`0.0f > v`, `1000.0f < v`) para que salga fcom con operando de memoria y el valor en ST0, como en el EXE (mismas C0/C2/C3 finales).
// Los valores pasan entre ayudantes como double (suma exacta de 24 bits): nunca se recalculan, y el fstp/fld qword intermedio del llamador no cambia ningun registro ni bandera.
// Comprobado con un arnes aislado (fuera del repo) que compara el FNSAVE (salvo FIP) de una transcripcion en ensamblador del EXE con esta cadena en las 30 rutas y mas de 5 millones de entradas con
// NaN, desnormales, infinitos y cero con signo, TOP y residuo iniciales aleatorios: cero diferencias. Cualquier cambio de compilador u opciones exige repetirlo (o ejecutar el banco).

static volatile int s_sexAppealZeroInt = 0, s_sexAppealThousandInt = 1000;
static const volatile float s_sexAppealZeroFloat = 0.0f, s_sexAppealThousandFloat = 1000.0f;
static volatile double s_sexAppealSinkX, s_sexAppealSinkY, s_sexAppealSinkX2, s_sexAppealSinkY2, s_sexAppealSinkA;

// 0x55BF23..0x55BF3B: fld [apariencia]; fmul [0x858B8C] (0.5f); fstp dword [esp+0xC].
__declspec(noinline) void __cdecl SexAppealHalfCpp(float appearance, unsigned int* out) {
    *reinterpret_cast<volatile float*>(out) = static_cast<float>(static_cast<double>(appearance) * 0.5f);
}

// 0x55BF90..0x55BFB9: dx = [esi] - [eax], dy = [esi+4] - [eax+4]; dy*dy + dx*dx; fsqrt; fcomp [0x85F070] (35.0f); test ah,5; jp. Devuelve 1 si la distancia es < 35 (NaN y >= 35 dan 0).
// Residuo del EXE al salir: registros fisicos 2 = dy, 3 = distancia (sqrt) y 4 = dx*dx (el 1 lo pisa despues siempre un fld).
__declspec(noinline) unsigned int __cdecl SexAppealNearCpp(const float* target, const float* player) {
    const double dx = static_cast<double>(target[0]) - static_cast<double>(player[0]);
    const double dy = static_cast<double>(target[1]) - static_cast<double>(player[1]);
    const double a = dy * dy;
    s_sexAppealSinkA = a;
    const double b = dx * dx;
    const double d = sqrt(a + b);
    if (d < 35.0f) {
        s_sexAppealSinkY = dy;
        s_sexAppealSinkX = dx;
        return 1;
    }
    s_sexAppealSinkY2 = dy;
    s_sexAppealSinkX2 = dx;
    return 0;
}

// 0x55BFC5..0x55C04A: fld [ecx+0x4C0]; fadd st,st; fsub 1000.0f; fstp dword [esp] (se guarda como float);
// 1000.0f < local ? 1000 : (local, y 0 si 0 > local; NaN se conserva); despues fmul por el factor de la clase (la tabla de 0x55C0B4 la resuelve Stats.cpp). ST0 sin redondear a float.
__declspec(noinline) double __cdecl SexAppealVehiclePartCpp(float health, unsigned int factorIndex) {
    volatile float local = static_cast<float>((static_cast<double>(health) + static_cast<double>(health)) - 1000.0f);
    double part;
    if (s_sexAppealThousandFloat < local) {
        part = s_sexAppealThousandInt;
    } else {
        part = local;
        if (s_sexAppealZeroFloat > local) {
            part = s_sexAppealZeroInt;
        }
    }
    switch (factorIndex) {
    case 0:  part = part * 0.1f; break; // 0x55C038: fmul [0x858B1C]
    case 2:  part = part * 0.3f; break; // 0x55C030: fmul [0x858C24]
    case 3:  part = part * 0.5f; break; // 0x55C040: fmul [0x858B8C]
    default: break;                     // 0x55C04C: sin factor
    }
    return part;
}

// 0x55C04C..0x55C093: fmul 0.5f; fadd dword [esp+4]; fcom 0.0 (test ah,5; jp: si < 0 -> 0); si no, fcom 1000.0 (test ah,41h; jne: si > 1000 -> 1000); cheat -> 1000; fstp dword [0xB793E4].
// Solo toca el registro 1 de la pila, como el EXE.
__declspec(noinline) void __cdecl SexAppealFinishCpp(double vehiclePart, unsigned int halfBits, unsigned int cheat) {
    const float half = __builtin_bit_cast(float, halfBits); // intrinsico del compilador: std::bit_cast con /Ob0 seria una llamada que devuelve el float en ST0
    double value = (vehiclePart * 0.5f) + static_cast<double>(half);
    if (0.0f > value) {
        value = s_sexAppealZeroInt;
    } else if (1000.0f < value) {
        value = s_sexAppealThousandInt;
    }
    if (cheat != 0) {
        value = s_sexAppealThousandInt;
    }
    *reinterpret_cast<volatile float*>(0xB793E4) = static_cast<float>(value);
}

// --- CPlayerPed::ControlButtonSprint (0x60A610; prefijo PlayerPedSprint) ---
// Tabla del EXE 0x8D2460 (32 bytes por tipo), 1.0f = 0x858624, 0.0f = 0x858B50, tiempo de paso = 0xB7CB5C. Cada ayudante reproduce un tramo x87 de la funcion: operaciones de 24 bits
// (PC24) sobre double y UN solo redondeo a float en cada fstp del EXE; las comparaciones usan el valor del registro (no el float guardado) como el fcomp del EXE.

// 0x60A69E..0x60A757 (sprint recien pulsado): suma = f0 + velocidad; si f10 < suma (fcomp st(1); test ah,5; jp) el resultado es f10, si no la suma (con NaN, la suma);
// fstp dword [datos+0x1C].
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PlayerPedSprintAddClampCpp(float f0, float speed, float f10) {
    const double sum = double(f0) + double(speed);
    volatile float result = float((double(f10) < sum) ? double(f10) : sum);
    return result;
}
#pragma float_control(pop)

// 0x60A6E3..0x60A757 (sprint mantenido, limite = 1.0f) y 0x60A72C..0x60A757 (sin pulsar, limite = 0.0f): r = velocidad - (paso * k) (fmul, fsubr); si limite > r
// (fcomp st(1); test ah,41h; jne) el resultado es el limite, si no r (con NaN, r); fstp dword [datos+0x1C].
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PlayerPedSprintDecayCpp(float timeStep, float k, float speed, float limit) {
    const double r = double(speed) - (double(timeStep) * double(k));
    volatile float result = float((double(limit) > r) ? double(limit) : r);
    return result;
}
#pragma float_control(pop)

// 0x60A776 / 0x60A786: fdiv velocidad / f_C y fst dword [esp+10h] (float(velocidad / f_C), un redondeo a 24 bits en la division y otro a float al guardarlo).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl PlayerPedSprintRatioCpp(float speed, float threshold) {
    volatile float result = float(double(speed) / double(threshold));
    return result;
}
#pragma float_control(pop)

// 0x60A7A9..0x60A7D3: fld ratio; fsub 1.0; (0.0 > r ? 0.0 : r); fmul f_1C; fadd 1.0 y ret con ST0 SIN guardar. Devuelve double (extern "C"): PlayerPed.cpp lo declara como float con
// otro nombre (/alternatename _PlayerPedSprintTailFltCpp=_PlayerPedSprintTailDblCpp) para que ST0 salga tal cual, como el EXE.
#pragma float_control(precise, on, push)
extern "C" __declspec(noinline) double __cdecl PlayerPedSprintTailDblCpp(float ratio, float k) {
    double r = double(ratio) - 1.0;
    if (0.0 > r) {
        r = 0.0;
    }
    return (r * double(k)) + 1.0;
}
#pragma float_control(pop)

// --- CTaskSimpleFight: FightHitCar (0x61D0B0) y FightStrike (0x6240B0) (prefijo Fight; lote gc) ---
// Sin reasociar: cada operacion del EXE redondea a 24 bits (PC24) y los intermedios que el EXE no guarda quedan en ST (rango de exponente extendido).
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl FightHitCarIntensityCpp(float mass, float damage, float scale) {
    volatile float result = (mass * damage) * scale;
    return result;
}

__declspec(noinline) float __cdecl FightStrikeReachCpp(float boundRadius, float hitRadius, bool bigHit) {
    volatile float reach = boundRadius + hitRadius; // fld ; fadd ; fstp dword
    if (bigHit) {
        reach = (hitRadius * 0.5f) + reach;         // fld ; fmul 0.5f ; fadd dword [alcance] ; fstp dword
    }
    return reach;
}

__declspec(noinline) bool __cdecl FightStrikeWithin2DCpp(const float* centre, const float* position, float reach) {
    return ((centre[0] - position[0]) * (centre[0] - position[0]) + (centre[1] - position[1]) * (centre[1] - position[1])) < reach * reach;
}

__declspec(noinline) bool __cdecl FightStrikeWithin3DCpp(const float* centre, const float* position, float reach) {
    return (((centre[0] - position[0]) * (centre[0] - position[0]) + (centre[1] - position[1]) * (centre[1] - position[1])) + (centre[2] - position[2]) * (centre[2] - position[2])) < reach * reach;
}

// direccion = centro - posicion: tres restas con operando en memoria (fld [centro]; fsub [posicion]; fstp dword), en funcion aparte para que el compilador no invierta el orden (fld posicion; fsubr centro).
static __declspec(noinline) void __cdecl FightStrikeDirectionCpp(const float* sphere, const float* position, float* direction) {
    direction[2] = sphere[2] - position[2];
    direction[1] = sphere[1] - position[1];
    direction[0] = sphere[0] - position[0];
}

__declspec(noinline) bool __cdecl FightStrikeSphereHitCpp(const float* sphere, const float* position, float hitRadius, float* direction) {
    FightStrikeDirectionCpp(sphere, position, direction);
    return (((sphere[0] - position[0]) * (sphere[0] - position[0]) + (sphere[1] - position[1]) * (sphere[1] - position[1])) + (sphere[2] - position[2]) * (sphere[2] - position[2]))
         < (sphere[3] + hitRadius) * (sphere[3] + hitRadius);
}

// *objetivo = fld [sumando] ; fadd [objetivo] ; fstp (en funcion aparte para conservar el orden de operandos del EXE: sumando cargado, posicion en memoria).
static __declspec(noinline) void __cdecl FightStrikeAddStoredCpp(const float* addend, float* target) {
    *target = *addend + *target;
}

__declspec(noinline) void __cdecl FightStrikeAdvanceCpp(const float* forward, float radius, float* position) {
    volatile float x15 = forward[0] * 1.5f;                // fstp dword [esp+5Ch] y relectura
    volatile float reach = radius;                         // fld ; fstp dword [esp+14h]: un NaN senalizante queda silenciado
    float z = (forward[2] * 1.5f) * reach;                 // fstp dword [esp+20h] (su direccion escapa: se guarda como float)
    position[0] = (x15 * reach) + position[0];
    position[1] = ((forward[1] * 1.5f) * reach) + position[1]; // la componente y no se guarda antes de sumarla (rango extendido)
    FightStrikeAddStoredCpp(&z, &position[2]);
}
#pragma float_control(pop)

// Comparaciones ordenadas de CTaskSimpleGangDriveBy (AITarget y AimGun) (fld dword [a]; fcomp dword [b]; fnstsw; test ah, ...). Con los dos operandos por puntero y sin precise el compilador
// emite exactamente fld [a]; fcomp [b] (operando de memoria, ST0 = a: mismas C0, C2, C3, IE y DE que el EXE); con parametros por valor invierte el orden. Cada una devuelve el
// resultado del salto del EXE: Gt = "test ah,41h; jne" no salta, Lt = "test ah,5; jp" no salta, Ge = "test ah,1; jne" no salta, Le = "test ah,41h; jp" no salta (con NaN todas, false).
__declspec(noinline) bool __cdecl GangDriveByGtCpp(const volatile float* a, const volatile float* b) { return double(*a) > double(*b); }
__declspec(noinline) bool __cdecl GangDriveByLtCpp(const volatile float* a, const volatile float* b) { return double(*a) < double(*b); }
__declspec(noinline) bool __cdecl GangDriveByGeCpp(const volatile float* a, const volatile float* b) { return double(*a) >= double(*b); }
__declspec(noinline) bool __cdecl GangDriveByLeCpp(const volatile float* a, const volatile float* b) { return double(*a) <= double(*b); }

// --- CTaskSimpleGangDriveBy (lote w21 D; prefijo GangDriveBy: PlayerTarget 0x621960, AITarget 0x627600, AimGun 0x628350). Anadir al FINAL de X87Math.cpp ---
// Usa JetPackSqrtExtCpp (raiz en una funcion aparte que devuelve ST0), ExeAsinDoubleCpp (_CIasin, 0x821E70), RpNanAddCpp/RpNanSubCpp y X87TimesFloatCpp, ya definidos arriba.
// Regla de las etapas: cada fpatan/fsin/fcos (64 bits de mantisa) se calcula en una funcion SIN precise que devuelve double (ST0 sin guardar) y la funcion siguiente lo consume
// al volver; todo lo demas son valores de 24 bits de mantisa (resultados de fadd/fsub/fmul a PC24), que un double guarda sin perdida.

static constexpr float kGangDriveByQuarterPi = std::bit_cast<float>(0x3F490FDBu); // 0x859AB0
static constexpr float kGangDriveByTwoOverPi = std::bit_cast<float>(0x3F22F983u); // 0x858FB8
static constexpr float kGangDriveByRangeBonus = std::bit_cast<float>(0x41A00000u); // 0x858BA4
// Constantes del EXE que el EXE usa como operandos de memoria (fcom/fadd/fsub dword [mem]): como float volatile el compilador las lee igual y no las conserva en un registro de la pila x87.
static const volatile float kGdPi        = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
static const volatile float kGdNegPi     = std::bit_cast<float>(0xC0490FDBu); // 0x858CC0
static const volatile float kGdTwoPi     = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
static const volatile float kGdQuarterPi = std::bit_cast<float>(0x3F490FDBu); // 0x859AB0
static const volatile float kGdTwoOverPi = std::bit_cast<float>(0x3F22F983u); // 0x858FB8
static const volatile float kGdZero      = 0.0f;                              // 0x858B50

// Etapa 1 del rumbo (0x6219E1..0x621A09 y 0x6276DE..0x627703): fpatan(-x, y) menos el rumbo del vehiculo (fpatan(-fwdX, fwdY) de su matriz, o el float del placement si no hay
// matriz); la resta (fsubp / fsub st(1)) redondea a 24 bits. Devuelve ST0 sin guardar.
static __declspec(noinline) double __cdecl GangDriveByHeadingDiffDbl(float x, float y, const float* forward, const float* heading) {
    const double aim     = atan2(-double(x), double(y));
    const double vehicle = forward ? atan2(-double(forward[0]), double(forward[1])) : double(*heading);
    return aim - vehicle;
}

// Rumbo base - rumbo del vehiculo (0x628648 / 0x628672: fsubr [base]); sin guardar.
static __declspec(noinline) double __cdecl GangDriveByBaseMinusHeadingDbl(float base, const float* forward, const float* heading) {
    const double vehicle = forward ? atan2(-double(forward[0]), double(forward[1])) : double(*heading);
    return double(base) - vehicle;
}

static __declspec(noinline) double __cdecl GangDriveBySinDbl(double angle) { return sin(angle); }
static __declspec(noinline) double __cdecl GangDriveByCosDbl(double angle) { return cos(angle); }

#pragma float_control(precise, on, push)
// 0x621A33..0x621A5F, 0x62786C..0x62787B: angulo + pi/4 (0x859AB0); si < 0 (fcom ordenado: con NaN no) + 2pi (0x858CBC); * 2/pi (0x858FB8); _ftol2 y el byte bajo con signo (movsx al).
static __forceinline int GangDriveByQuadrantOf(double angle) {
    double value = angle + kGdQuarterPi;
    // fcom dword [0.0]: ST0 = valor frente a un operando de memoria (C0 = valor < 0). El compilador, si el valor se usa despues, carga la constante y compara al reves
    // (C0 invertido); con una copia volatile del valor, `fld copia; fcomp [0.0]` conserva la orientacion del EXE (la copia no cambia el valor: va a doble sin perdida).
    volatile double copy = value;
    if (double(copy) < 0.0) {
        value = value + kGdTwoPi;
    }
    value = value * kGdTwoOverPi;
    // _ftol2 (0x821B40): las banderas PE/IE del fistp las pone el __ftol2 del compilador; los registros muertos (R y v - R) y el resultado, Ftol2RegsDblCpp.
    volatile long long truncated = static_cast<long long>(value);
    (void)truncated;
    return static_cast<signed char>(Ftol2RegsDblCpp(value, 0).eax);
}

// PlayerTarget (0x621960), cuadrante (0x6219E1..0x621A5F): rumbo de la camara - rumbo del vehiculo, ajuste de +-2pi respecto a +-pi (0x858CB8 / 0x858CC0) sin guardar como float.
// forward = &matriz->forward (x, y, z consecutivos) o nullptr (entonces heading = placement.m_fHeading del vehiculo).
__declspec(noinline) int __cdecl GangDriveByPlayerQuadrantCpp(float aimX, float aimY, const float* forward, const float* heading) {
    double angle = GangDriveByHeadingDiffDbl(aimX, aimY, forward, heading);
    if (angle > kGdPi) {
        angle = angle - kGdTwoPi;
    } else if (angle < kGdNegPi) {
        angle = angle + kGdTwoPi;
    }
    return GangDriveByQuadrantOf(angle);
}

// AITarget (0x627600), cuadrante del angulo ya guardado como float (0x627858..0x62787B).
__declspec(noinline) int __cdecl GangDriveByAiQuadrantCpp(float angle) {
    return GangDriveByQuadrantOf(double(angle));
}

// AITarget (0x627600), 0x627657..0x627741: dx, dy, dz = objetivo - posicion (cada resta a 24 bits y guardada como float), distancia = float(sqrt((dz^2 + dx^2) + dy^2)),
// angulo = float(fpatan(-dx, dy) - rumbo del vehiculo) y ajuste de +-2pi respecto a +-pi con un guardado como float por ajuste.
// target/pos = {x, y, z}; out = {dx, dy, dz, distancia, angulo}.
__declspec(noinline) void __cdecl GangDriveByAiGeometryCpp(const float* target, const float* pos, const float* forward, const float* heading, float* out) {
    volatile float dx = float(double(target[0]) - double(pos[0]));
    volatile float dy = float(double(target[1]) - double(pos[1]));
    volatile float dz = float(double(target[2]) - double(pos[2]));
    const double sum = (double(dz) * double(dz) + double(dx) * double(dx)) + double(dy) * double(dy);
    volatile float distance = float(JetPackSqrtExtCpp(sum));
    const double difference = GangDriveByHeadingDiffDbl(dx, dy, forward, heading);
    volatile float angle = float(difference);
    if (angle > kF_40490FDB) {
        angle = float(double(angle) - double(kF_40C90FDB));
    } else if (angle < -kF_40490FDB) {
        angle = float(double(angle) + double(kF_40C90FDB));
    }
    out[0] = dx;
    out[1] = dy;
    out[2] = dz;
    out[3] = distance;
    out[4] = angle;
    // Registros muertos del EXE tras el rumbo (0x6276E2..0x627707): T-2 = angulo sin ajustar (fsub st(1) y fstp), T-3 = forward[1] cargado por el segundo fpatan (solo con matriz).
    // T-1 lo sobrescribe la primera comparacion siguiente.
    if (forward) {
        X87PlantT3T2Cpp(double(forward[1]), difference);
    } else {
        X87PlantT2Cpp(difference);
    }
}

// AITarget (0x627600), 0x6277E7..0x62780B: punto de mira = delta + posicion del ped; fld dz; fadd z; fld dy; fadd y; fld dx; fadd x; y tres fstp: deja las sumas z, y, x en T-1, T-2 y T-3.
// delta = {dx, dy, dz}; out = {x, y, z}.
__declspec(noinline) void __cdecl GangDriveByAimPointCpp(const float* delta, const float* pos, float* out) {
    const double z = double(delta[2]) + double(pos[2]);
    const double y = double(delta[1]) + double(pos[1]);
    const double x = double(delta[0]) + double(pos[0]);
    out[0] = float(x);
    out[1] = float(y);
    out[2] = float(z);
    X87PlantT3T2T1Cpp(x, y, z);
}

// AimGun (0x628350), 0x6283CB..0x628403 (camara TWOPLAYER_IN_CAR_AND_SHOOTING): float(sqrt((dz^2 + dy^2) + dx^2) + 20.0f) con d = pos - source (sin guardar entre pasos).
// El EXE (fld/fsub x3, fld st(0), fmul, ..., fsqrt, fstp st(3), fstp st(0) x2, fadd, fstp) apila hasta cinco registros y deja muertos T-5 = dx*dx, T-4 = raiz, T-3 = dz, T-2 = dy y
// T-1 = el rango: se escriben al final (la logica real no pasa de tres registros).
__declspec(noinline) float __cdecl GangDriveByAimRangeCpp(const float* pos, const float* source) {
    const double dx = double(pos[0]) - double(source[0]);
    const double dy = double(pos[1]) - double(source[1]);
    const double dz = double(pos[2]) - double(source[2]);
    const double root = JetPackSqrtExtCpp((dz * dz + dy * dy) + dx * dx);
    const double range = root + double(kGangDriveByRangeBonus);
    volatile float result = float(range);
    X87PlantT5T4T3T2T1Cpp(dx * dx, root, dz, dy, range);
    return result;
}

// AimGun (0x628422..0x628460, jugador): x se copia con una lectura entera; y pasa por la x87 (fld/fstp: silencia un NaN senalizante y marca IE/DE como el EXE); z = float(z + 0.7f).
__declspec(noinline) void __cdecl GangDriveByMuzzleCpp(const float* pos, float* out) {
    reinterpret_cast<unsigned int*>(out)[0] = reinterpret_cast<const unsigned int*>(pos)[0]; // mov ecx, [eax]; mov [esp+0x38], ecx: copia entera, no silencia un NaN senalizante
    volatile double y = pos[1];
    out[1] = float(y);
    out[2] = float(double(pos[2]) + double(std::bit_cast<float>(0x3F333333u))); // 0x858CB0
}

// AimGun (0x628350), rama de moto 0x6285F4..0x6286DB. gun = {x, y, z} del hueso de la mano (x e y ya sustituidos por la posicion del ped), target = {x, y, z}.
//   horizontal = float(sqrt(dy^2 + dx^2)) con d = gun - target; elevacion = radianFn(target.z, horizontal, gun.z, 0.0f) (CGeneral::GetRadianAngleBetweenPoints, ST0 sin guardar;
//   radianFn se declara con bits en x1/x2/y2 porque el EXE los empuja sin pasar por la x87);
//   h = base - rumbo del vehiculo; r = float(sin(h) * lean + elevacion); c = float(cos(h)); s = clamp(forward[2], -1, 1) (con NaN se conserva);
//   resultado = float(asin(s) * c + r) con el _CIasin del EXE. forward debe ser distinto de nullptr (el EXE lee forward[2] sin comprobar).
__declspec(noinline) float __cdecl GangDriveByBikeElevationCpp(float (__cdecl* radianFn)(unsigned int, float, unsigned int, unsigned int), const float* target, const float* gun, float baseAngle, const float* forward, const float* heading, float lean) {
    const double dx = double(gun[0]) - double(target[0]);
    const double dy = double(gun[1]) - double(target[1]);
    const double dxSquared = dx * dx;
    const double horizontalRoot = JetPackSqrtExtCpp(dy * dy + dxSquared);
    volatile float horizontal = float(horizontalRoot);
    // x1, x2 e y2 viajan como bits (el EXE los empuja con mov enteros: un NaN senalizante no se silencia); y1 es el float guardado.
    const double elevation = double(radianFn(*reinterpret_cast<const unsigned int*>(&target[2]), horizontal, *reinterpret_cast<const unsigned int*>(&gun[2]), 0u));
    const double h = GangDriveByBaseMinusHeadingDbl(baseAngle, forward, heading);
    const double sum = GangDriveBySinDbl(h) * double(lean) + elevation;
    volatile float result = float(sum);
    const double h2 = GangDriveByBaseMinusHeadingDbl(baseAngle, forward, heading);
    volatile float cosine = float(GangDriveByCosDbl(h2));

    // Recorte de forward[2] a [-1, 1] como el EXE (0x62867F..0x6286CE): fld -1.0; fcomp [fz]; si fz < -1 -> -1; si no fld fz; fld 1.0; fcomp st(1); fstp st(0) (T-2 queda en 1.0)
    // y, si 1.0 < fz -> 1.0; si no se repite la comparacion con -1.0 y, si -1.0 <= fz (o NaN), la pendiente es fz.
    const float minusOne = -1.0f, plusOne = 1.0f; // 0x858C1C / 0x858624
    const float* const fzPointer = &forward[2];
    float slope;
    bool oneRegisterLoaded = false;
    if (GangDriveByGtCpp(&minusOne, fzPointer)) {
        GangDriveByGtCpp(&minusOne, fzPointer); // 0x6286AD repite la comparacion
        slope = minusOne;
    } else {
        oneRegisterLoaded = true;
        if (GangDriveByLtCpp(&plusOne, fzPointer)) {
            slope = plusOne;
        } else {
            GangDriveByGtCpp(&minusOne, fzPointer);
            slope = *fzPointer;
        }
    }
    // Registros muertos del EXE hasta el _CIasin (0x6286CE): T-4 = dx * dx; T-3 = forward[1] (con matriz, segundo fpatan del rumbo) o la raiz horizontal; T-2 = forward[1] del
    // segundo rumbo (con matriz) o la suma sin * lean + elevacion, y 1.0 si el recorte cargo el 1.0. El _CIasin que sigue (rama |pendiente| < 1) sobrescribe T-2 con
    // sqrt((1 + x)(1 - x)) y T-3 con 1 - x por si solo (ExeAsinDoubleCpp deja la misma pila que 0x821EA8..0x821EB4); con |pendiente| = 1 no los toca. Los valores se pasan por
    // variables volatile para que la preparacion de los argumentos no apile registros por debajo del cuarto.
    volatile double plantT4 = dxSquared;
    volatile double plantT3 = forward ? double(forward[1]) : horizontalRoot;
    volatile double plantT2 = forward ? double(forward[1]) : sum;
    if (oneRegisterLoaded) {
        plantT2 = 1.0;
    }
    X87PlantT4T3T2Cpp(plantT4, plantT3, plantT2);
    const double asinValue = ExeAsinDoubleCpp(double(slope));
    volatile float finalResult = float(asinValue * double(cosine) + double(result));
    return finalResult;
}
#pragma float_control(pop)

// --- Grupo E (ge): 0x610DB0, 0x72D6D0 y 0x63A380 (3 de los 21 destinos inciertos de la cohorte de 499) ---
// Todo el aritmetico va aqui (X87Math.cpp se compila con /O2 /Oi /arch:IA32 /fp:fast); precise evita que /fp:fast reasocie o pliegue operaciones, pero con
// precise el compilador llama a __CIatan2 en vez de emitir fpatan en linea, asi que los fpatan van en funciones sueltas SIN precise (como Atan2MinusCpp).
// Las constantes son las del EXE (direccion indicada); constexpr evita cualquier llamada a std::bit_cast dentro de las funciones precise.
extern "C" double __cdecl fabs(double);
#pragma intrinsic(fabs)
namespace GeEnterCar {
constexpr double kUnit15   = double(std::bit_cast<float>(0x38000000u)); // 0x858B14 = 2^-15
constexpr double kHalf     = double(std::bit_cast<float>(0x3F000000u)); // 0x858B8C
constexpr double kPi       = double(std::bit_cast<float>(0x40490FDBu)); // 0x858CB8
constexpr double kTwoPi    = double(std::bit_cast<float>(0x40C90FDBu)); // 0x858CBC
constexpr double kHalfPi   = double(std::bit_cast<float>(0x3FC90FDBu)); // 0x858FE4
constexpr double kNegHalfPi = double(std::bit_cast<float>(0xBFC90FDBu)); // 0x859998
constexpr double kEightDeg = double(std::bit_cast<float>(0x3E0EFA35u)); // 0x86E694
constexpr double kSeven    = double(std::bit_cast<float>(0x3F333333u)); // 0x86E690 = 0.7f
constexpr double kZero     = 0.0;                                       // 0x858B50
}

// 0x610DB0: fild rand (ya enmascarado a 16 bits); fmul dword [0x858B14] (2^-15, exacto); fimul (limite superior - limite inferior, un redondeo a 24 bits);
// call _ftol2 (0x821B40). Devuelve los 32 bits bajos del entero de 64 bits truncado por _ftol2. El llamador suma el limite inferior.
// Los dos enteros llegan como parametros (el compilador solo emite fimul con un entero en memoria) y 2^-15 como float volatile (un operando de memoria, no un registro
// con la constante): la secuencia x87 es fld; fimul; fimul sobre un unico registro, como el EXE. El truncado lo hace Ftol2RegsDblCpp (mismas operaciones x87 que 0x821B40:
// deja R y v - R en los dos registros muertos) y el volatile long long aporta las banderas PE/IE del fistp.
static const volatile float kPopulationUnit15 = std::bit_cast<float>(0x38000000u); // 0x858B14 = 2^-15
#pragma float_control(precise, off, push)
__declspec(noinline) int __cdecl PopulationRandomRangeCpp(int random, int range) {
    const double value = double(random) * kPopulationUnit15 * double(range);
    volatile long long truncated = static_cast<long long>(value); // PE/IE del fistp
    (void)truncated;
    return static_cast<int>(Ftol2RegsDblCpp(value, 0).eax);
}
#pragma float_control(pop)

// 0x72D6D0 (CZoneDef::GetCenter): zona = 8 int16 {esquinaX, esquinaY, vec1X, vec1Y, vec2X, vec2Y, minZ, maxZ}.
// x = (vec2X + vec1X) * 0.5 + esquinaX; y = (vec2Y + vec1Y) * 0.5 + esquinaY; z = (minZ + maxZ) * 0.5 (fild/fiadd/fmul dword [0x858B8C]/fiadd/fstp).
// Todos los pasos son exactos (enteros de 16 bits); el EXE los guarda en el orden x, y, z (z se calcula primero y se guarda el ultimo).
// Los siete enteros llegan como parametros ya extendidos (zSum = minZ + maxZ): el compilador solo emite fiadd/fimul con un entero en memoria y, con 0.5f como float volatile
// (operando de memoria en vez de un registro con la constante), la pila x87 no pasa de tres registros (z, y, x) y no deja residuo de enteros: igual que el EXE.
static const volatile float kCullZoneHalf = std::bit_cast<float>(0x3F000000u); // 0x858B8C
#pragma float_control(precise, off, push)
__declspec(noinline) void __cdecl CullZoneCenterCpp(int zSum, int y5, int y3, int y1, int x4, int x2, int x0, float* out) {
    const double z = double(zSum) * kCullZoneHalf;
    const double y = (double(y5) + double(y3)) * kCullZoneHalf + double(y1);
    const double x = (double(x4) + double(x2)) * kCullZoneHalf + double(x0);
    out[0] = float(x);
    out[1] = float(y);
    out[2] = float(z);
}
#pragma float_control(pop)

// 0x63A460..0x63A478 y 0x63A482..0x63A49A: movsx; fild; fabs; fcomp dword [0x858BA4] (20.0f); test ah, 41h; je -> true si |valor| > 20 (el estado x87 de la comparacion queda
// como en el EXE).
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl EnterCarCamStickBeyondCpp(short value) {
    return fabs(double(int(value))) > 20.0;
}
#pragma float_control(pop)

// 0x63A4D8..0x63A4EA (fpatan sin redondear en ST0, fsub pi/2 y fstp dword): rumbo del coche menos pi/2 a partir de la matriz (up.x, up.y = matrix + 0x10 / + 0x14).
static __declspec(noinline) float EnterCarCamHeadingMatrixCpp(const float* up) {
    volatile float heading = float(atan2(-double(up[0]), double(up[1])) - GeEnterCar::kHalfPi);
    return heading;
}
// 0x63A4E4 (sin matriz): fld dword [coche+0x10]; fsub pi/2; fstp dword.
#pragma float_control(precise, on, push)
static __declspec(noinline) float EnterCarCamHeadingPlacementCpp(float placementHeading) {
    volatile float heading = float(double(placementHeading) - GeEnterCar::kHalfPi);
    return heading;
}
// 0x63A50F..0x63A52D: escalar = (dz * der.z + dy * der.y) + dx * der.x, cada paso redondeado a 24 bits; devuelve ST0 tal cual (24 bits, exponente extendido).
static __declspec(noinline) double EnterCarCamDotCpp(const float* carPos, const float* pedPos, const float* right) {
    const double dx = double(pedPos[0]) - double(carPos[0]);
    const double dy = double(pedPos[1]) - double(carPos[1]);
    const double dz = double(pedPos[2]) - double(carPos[2]);
    return (dz * double(right[2]) + dy * double(right[1])) + dx * double(right[0]);
}
#pragma float_control(pop)
// 0x63A533..0x63A53F: fld distancia; fpatan; fmul 0.7f; fstp dword. El escalar llega con 24 bits (exacto como double).
static __declspec(noinline) float EnterCarCamAngleCpp(double dot, float distance) {
    volatile float angle = float(atan2(dot, double(distance)) * GeEnterCar::kSeven);
    return angle;
}

// 0x63A4CE..0x63A53F (CTaskComplexEnterCar, camara): out = {rumbo - pi/2, escalar, angulo}, los tres guardados como float (fstp/fst dword), como el EXE.
// matrix: matriz del coche o nullptr (entonces el rumbo es placementHeading y, como el EXE, el producto escalar desreferencia matrix + 8 y falla).
// carPos / pedPos: posiciones (matrix + 0x30 o placement + 4) de coche y peaton.
__declspec(noinline) void __cdecl EnterCarCamLateralCpp(const float* matrix, float placementHeading, const float* carPos, const float* pedPos, float distance, float* out) {
    out[0] = matrix ? EnterCarCamHeadingMatrixCpp(matrix + 4) : EnterCarCamHeadingPlacementCpp(placementHeading);
    const double dot = EnterCarCamDotCpp(carPos, pedPos, matrix);
    volatile float dotStored = float(dot);
    out[1] = dotStored;
    out[2] = EnterCarCamAngleCpp(dot, distance);
}

// 0x63A55A..0x63A67A (resto de la camara de EnterCar): camMinY = CColModel::m_boundBox.m_vecMin.y del coche; choice = byte [tarea+0x29] (0, 1 o 2);
// t1 = float [0x86E684 + 4 * choice]; t2 = float [0x86E678 + 4 * choice]; limit = float [0x8D2ECC] (dato modificable del EXE).
// Cada comparacion conserva el significado de su fcom/fcomp + test ah y se compila con la misma forma que el EXE (ST0 frente a un operando de memoria): con NaN el operando de
// memoria no marca DE, y asi las banderas x87 salen como las del EXE. Los registros muertos que deja el EXE son T-3 = objetivo (copia para fcompp, antes de ajustar +-2pi),
// T-2 = escalar (si delta > t1) o -t1 (si no); los escribe X87PlantT3T2Cpp antes de la comparacion final (T-1, el nuevo alfa o la vertical cargada, lo deja la propia secuencia).
// Constantes del EXE como float volatile: el compilador las lee como operandos de memoria (como fadd/fsub/fcom dword [mem]) y no las deja en un registro de la pila x87.
static const volatile float kEnterCarPi        = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
static const volatile float kEnterCarTwoPi     = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
static const volatile float kEnterCarHalfPi    = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4
static const volatile float kEnterCarNegHalfPi = std::bit_cast<float>(0xBFC90FDBu); // 0x859998
static const volatile float kEnterCarZero      = 0.0f;                              // 0x858B50
// Fuera de precise: con precise el compilador carga los dos operandos y usa fcompp; sin el, ST0 frente a operando de memoria (fcomp dword [mem]) como el EXE.
// 0x63A548..0x63A56F: fld [colmodel+4]; fchs; fcomp dword [distancia]; test ah, 41h; jne: true si -min.y > distancia (con NaN, false).
static __declspec(noinline) bool EnterCarCamBeyondCpp(const volatile float* minY, const volatile float* distance) {
    return -double(*minY) > double(*distance);
}
// 0x63A64E..0x63A65B: fld dword [vertical]; fcomp dword [limite]; test ah, 41h; jne: true si vertical > limite (con NaN, false).
static __declspec(noinline) bool EnterCarCamAboveCpp(const volatile float* vertical, const volatile float* limit) {
    return double(*vertical) > double(*limit);
}
#pragma float_control(precise, on, push)
// 0x63A63C..0x63A64C: fmul dword [t2] (delta en ST0); fadd dword [beta]; el llamador guarda el resultado con fstp dword.
static __declspec(noinline) double EnterCarCamBetaSumCpp(double delta, const volatile float* t2, const volatile float* beta) {
    return delta * double(*t2) + double(*beta);
}
// 0x63A64E..0x63A674: fld t2; fmul t1; fsubr dword [alfa]; el llamador guarda el resultado con fstp dword. El producto sale de una funcion (ST0) para que la resta
// use el operando de memoria (fsubr) y no cargue antes el alfa en un segundo registro.
static __declspec(noinline) double EnterCarCamAlphaProductCpp(float t2, float t1) {
    return double(t2) * double(t1);
}
static __declspec(noinline) double EnterCarCamAlphaCpp(const volatile float* alpha, float t2, float t1) {
    const double product = EnterCarCamAlphaProductCpp(t2, t1);
    return double(*alpha) - product;
}
// 0x63A571..0x63A63C: del objetivo (cima de la pila) al delta ya acotado. `x` = +-8 grados o el angulo; devuelve delta; target = objetivo antes de ajustar (copia que el EXE
// compara con fcompp); second = escalar (delta > t1) o -t1. Las comparaciones con la copia usan `1,0 * copia` (registro, no operando de memoria): son fcompp como en el EXE.
static __declspec(noinline) double EnterCarCamDeltaCpp(double x, float heading, const volatile float* dot, unsigned int choice, const volatile float* t1,
                                                       const volatile float* horizontal, double* target, double* second) {
    double objective = (choice == 1u) ? x + (double(heading) + kEnterCarPi) : double(heading) - x;
    volatile double spill = objective;
    *target = spill;
    if (double(*horizontal) + kEnterCarPi < kPlantOne * double(spill)) {
        objective = objective - kEnterCarTwoPi;
    } else if (double(*horizontal) - kEnterCarPi > kPlantOne * double(spill)) {
        objective = objective + kEnterCarTwoPi;
    }
    double delta = objective - double(*horizontal);
    const bool dotPositive = (kPlantOne * double(*dot)) > kEnterCarZero;
    if (dotPositive == (choice == 1u)) {
        if (delta <= kEnterCarNegHalfPi) {
            delta = -delta;
        }
    } else {
        if (delta >= kEnterCarHalfPi) {
            delta = -delta;
        }
    }
    if (delta > double(*t1)) {
        delta = double(*t1);
        *second = double(*dot);
    } else {
        volatile float negT1 = float(-double(*t1));
        *second = double(negT1);
        if (delta < double(negT1)) {
            delta = double(negT1);
        }
    }
    return delta;
}
__declspec(noinline) void __cdecl EnterCarCamStickCpp(float camMinY, float distance, float angle, float heading, float dot, unsigned int choice, float t1, float t2, float limit,
                                                      const volatile float* vertical, const volatile float* horizontal, float* alphaStick, float* betaStick) {
    using namespace GeEnterCar;
    // 0x63A548
    double x = double(angle);
    if (EnterCarCamBeyondCpp(&camMinY, &distance)) {
        x = (double(angle) > kZero) ? -kEightDeg : kEightDeg;
    }
    double target = 0.0, second = 0.0;
    const double delta = EnterCarCamDeltaCpp(x, heading, &dot, choice, &t1, horizontal, &target, &second);
    // 0x63A63C
    volatile float beta = float(EnterCarCamBetaSumCpp(delta, &t2, betaStick));
    *betaStick = beta;
    // Registros muertos del EXE T-3 y T-2 (los de T-1 los deja la propia comparacion o el calculo del alfa; la comparacion final fija C0..C3 y debe ir despues).
    X87PlantT3T2Cpp(target, second);
    // 0x63A64E
    if (EnterCarCamAboveCpp(vertical, &limit)) {
        *alphaStick = float(EnterCarCamAlphaCpp(alphaStick, t2, t1)); // fstp dword [alfa] del registro sin releer: T-1 conserva el valor sin pasar por float
    }
}
#pragma float_control(pop)

// --- Grupo G (BRIEF_21): CQuaternion::Slerp ---
// CQuaternion::Slerp (0x59C300, thiscall con cinco argumentos de pila): out/from/to apuntan a x, y, z, w consecutivos; out puede coincidir con from o con to.
// Rama theta == 0 (fcomp 0.0 + "test ah,0x44 / jp"; con NaN no entra): copia `to` palabra a palabra sin pasar por la x87.
// Rama de calculo: si theta > pi/2 (0x858FE4; con NaN no entra), theta' = float(pi - theta) (0x858CB8; guardado como float), A = sin((1 - t)·theta')·inv y B = -(sin(theta'·t)·inv);
// si no, A = sin((1 - t)·theta)·inv y B = sin(theta·t)·inv. Los senos (fsin) quedan sin redondear en ST0 y cada fsub/fmul se redondea a 24 bits (PC24 del proceso).
// Cada componente se calcula como en el EXE: x = A·from.x + B·to.x, y/z/w = B·to + A·from, con A y B cargados de memoria como operando de fmul (DE solo si el operando
// es subnormal y el otro no es NaN) y las pilas x87 muertas iguales a las del EXE. Se comparte el estado x87 final con el EXE (FSW, FTW, TOP y los ocho registros ST).
// Las lecturas pasan por punteros volatile y A/B se derraman a doubles volatile: sin eso el compilador carga el float y multiplica por el registro (otro DE y otros residuos).
// Verificado con un arnes que ejecuta los bytes reales del EXE reubicados frente a esta funcion: 12 000 000 de casos (NaN con carga distinta, senalizantes, subnormales, +-inf, +-0,
// FLT_MAX, 1e30, solapes out == from y out == to) con salida, FCW, FSW, FTW y los diez bytes de cada registro ST identicos.
static const volatile float kSlerpZero = 0.0f;                          // 0x858B50
static constexpr float kSlerpHalfPi = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4
__declspec(noinline) void __cdecl QuaternionSlerpCpp(float* out, const float* from, const float* to, float theta, float invSin, float t) {
    const volatile float* const vTheta = &theta;
    const volatile float* const vInv   = &invSin;
    const volatile float* const vT     = &t;
    const volatile float* const vFrom  = from;
    const volatile float* const vTo    = to;
    if (*vTheta >= kSlerpZero && *vTheta <= kSlerpZero) {
        const unsigned* source = reinterpret_cast<const unsigned*>(to);
        unsigned* target       = reinterpret_cast<unsigned*>(out);
        target[0] = source[0];
        target[1] = source[1];
        target[2] = source[2];
        target[3] = source[3];
        return;
    }
    double a, b;
    if (*vTheta > kSlerpHalfPi) {
        volatile float flipped = float(double(kF_40490FDB) - double(*vTheta));
        const volatile float* const vAngle = &flipped;
        a = sin((1.0 - double(*vT)) * double(*vAngle)) * double(*vInv);
        b = -(sin(double(*vAngle) * double(*vT)) * double(*vInv));
    } else {
        a = sin((1.0 - double(*vT)) * double(*vTheta)) * double(*vInv);
        b = sin(double(*vTheta) * double(*vT)) * double(*vInv);
    }
    volatile double aSpill = a;
    volatile double bSpill = b;
    {
        const double a0 = aSpill; const double p0 = a0 * vFrom[0];
        const double b0 = bSpill; const double q0 = b0 * vTo[0];
        out[0] = float(p0 + q0);
    }
    {
        const double b1 = bSpill; const double q1 = b1 * vTo[1];
        const double a1 = aSpill; const double p1 = a1 * vFrom[1];
        out[1] = float(q1 + p1);
    }
    {
        const double b2 = bSpill; const double q2 = b2 * vTo[2];
        const double a2 = aSpill; const double p2 = a2 * vFrom[2];
        out[2] = float(q2 + p2);
    }
    volatile double bSink = b;
    volatile double aSink = a;
    {
        const double b3 = bSpill; const double q3 = b3 * vTo[3];
        const double a3 = aSpill; const double p3 = a3 * vFrom[3];
        out[3] = float(q3 + p3);
    }
}

// --- CVehicle::DoHeadLightEffect (0x6E0A50) y CVehicle::DoTailLightEffect (0x6E1780), lote g22b, prefijo VehHeadTail. Anexar al FINAL de X87Math.cpp. ---
// Reutiliza JpFixMemOp / kJpFix* / JetPackNegSinExtCpp / JetPackCosExtCpp / FormationRootCpp (definidos mas arriba en este archivo) y Ftol2RegsDblCpp.
// Convencion: lo que el EXE guarda con fstp dword es un float (o `volatile float`); lo que queda en la pila x87 es double (el proceso corre a 24 bits de mantisa con
// exponente extendido). Cada `fop dword [mem]` con un registro ya cargado pasa por JpFixMemOp (semantica de NaN del hardware: una memoria senalizante pierde frente a un NaN silencioso
// del registro y, si el registro es un numero, gana la memoria silenciada); los fld dword (que quitan la senal) y las operaciones registro-registro son double.

// El seno (fsin; fchs, con el signo de NaN que da fchs) y el coseno salen de JetPackNegSinExtCpp / JetPackCosExtCpp (definidos mas arriba: fuera de precise, devuelven ST0 sin redondear).

// Constantes del EXE (float cargado como double por fmul/fadd dword [mem]).
static constexpr double kVehHeadTailHalf       = double(std::bit_cast<float>(0x3F000000u)); // 0x858B8C (0.5)
static constexpr double kVehHeadTailThreeTenths    = double(std::bit_cast<float>(0x3E99999Au)); // 0x858C24 (0.3)
static constexpr double kVehHeadTailFifth      = double(std::bit_cast<float>(0x3E4CCCCDu)); // 0x858CC4 y 0x8D3688 (0.2)
static constexpr double kVehHeadTailTwoFifths  = double(std::bit_cast<float>(0x3ECCCCCDu)); // 0x8D3684 (0.4)
static constexpr double kVehHeadTailInv150     = double(std::bit_cast<float>(0x3BDA740Eu)); // 0x862D6C (1/150)
static constexpr double kVehHeadTailOne        = double(std::bit_cast<float>(0x3F800000u)); // 0x858624 (1.0)
static constexpr double kVehHeadTail150        = double(std::bit_cast<float>(0x43160000u)); // 0x858A28 (150.0)

#pragma float_control(precise, on, push)

// CMatrix * CVector del EXE (0x59C890): x = ((up.x*z + fwd.x*y) + right.x*x) + pos.x; y = ((up.y*z + right.y*x) + fwd.y*y) + pos.y; z = ((up.z*z + right.z*x) + fwd.z*y) + pos.z,
// sin redondear a float entre operaciones; m = float[16] como CMatrix (right 0, forward 4, up 8, pos 12), v y out = x, y, z (no se solapan).
static __declspec(noinline) void VehHeadTailTransformCpp(const float* m, const float* v, float* out) {
    const double xa = JpFixMemOp(kJpFixMul, double(m[8]), v + 2);
    const double xb = JpFixMemOp(kJpFixMul, double(m[4]), v + 1);
    const double xs = xa + xb;
    const double xc = JpFixMemOp(kJpFixMul, double(m[0]), v + 0);
    const double xt = xs + xc;
    out[0] = float(JpFixMemOp(kJpFixAdd, xt, m + 12));
    const double ya = JpFixMemOp(kJpFixMul, double(m[9]), v + 2);
    const double yb = JpFixMemOp(kJpFixMul, double(m[1]), v + 0);
    const double ys = ya + yb;
    const double yc = JpFixMemOp(kJpFixMul, double(m[5]), v + 1);
    const double yt = ys + yc;
    out[1] = float(JpFixMemOp(kJpFixAdd, yt, m + 13));
    const double za = JpFixMemOp(kJpFixMul, double(m[10]), v + 2);
    const double zb = JpFixMemOp(kJpFixMul, double(m[2]), v + 0);
    const double zs = za + zb;
    const double zc = JpFixMemOp(kJpFixMul, double(m[6]), v + 1);
    const double zt = zs + zc;
    out[2] = float(JpFixMemOp(kJpFixAdd, zt, m + 14));
}

static const volatile double kVehHeadTailZero = 0.0; // 0x858B50
// CVector::NormaliseAndMag del EXE (0x59C970): suma (x*x + y*y) + z*z sin redondear; si es <= 0 (comparacion ordenada: NaN sigue) solo pone x = 1.0f y devuelve 1.0f;
// si no, recip = 1.0 / sqrt(suma) (24 bits), cada componente = float(recip * componente) y devuelve 1.0 / recip (sin guardar a float: el llamador lo hace).
// stale (opcional, 3 valores): z*z, la suma y x antes de normalizar = lo que el EXE deja en los registros muertos 5, 4 y 3 de esta funcion (los lee VehHeadTailHeadGeometryCpp).
static __declspec(noinline) float VehHeadTailNormaliseMagCpp(float* v, double* stale = nullptr) {
    const double x = double(v[0]);
    const double y = double(v[1]);
    const double z = double(v[2]);
    const double zz = z * z;
    volatile double sum = (x * x + y * y) + zz;
    if (stale) {
        stale[0] = zz;
        stale[1] = sum;
        stale[2] = x;
    }
    if (sum <= kVehHeadTailZero) {   // fld suma; fcom [0] (la suma en ST0, como 0x59C98E: los codigos C0-C3 que deja)
        v[0] = 1.0f;
        return 1.0f;
    }
    const double root = FormationRootCpp(sum);
    const double recip = 1.0 / root;
    v[0] = float(JpFixMemOp(kJpFixMul, recip, v + 0));
    v[1] = float(JpFixMemOp(kJpFixMul, recip, v + 1));
    v[2] = float(JpFixMemOp(kJpFixMul, recip, v + 2));
    return float(1.0 / recip);
}

// Parte comun: world = matriz * punto; delta = camara - world (fld camara; fsub [world], z, y, x, cada uno a float); mag = NormaliseAndMag(delta). Devuelve la magnitud y deja delta normalizado en d.
static __declspec(noinline) float VehHeadTailDirectionCpp(const float* matrix, const float* point, const float* camera, float* d, double* stale = nullptr) {
    float world[3];
    VehHeadTailTransformCpp(matrix, point, world);
    d[2] = float(JpFixMemOp(kJpFixSub, double(camera[2]), world + 2));
    d[1] = float(JpFixMemOp(kJpFixSub, double(camera[1]), world + 1));
    d[0] = float(JpFixMemOp(kJpFixSub, double(camera[0]), world + 0));
    return VehHeadTailNormaliseMagCpp(d, stale);
}

// CVehicle::DoTailLightEffect 0x6E1820..0x6E18CC. dummy = posicion del dummy (copiada como bits); mirror = lado izquierdo (x = x - (x + x) con la x original en memoria);
// el producto escalar es el de la direccion a la camara con el trasero de la matriz: ((-fz*dz) + (-fy*dy)) + (-fx*dx).
__declspec(noinline) void __cdecl VehHeadTailTailGeometryCpp(const float* dummy, bool mirror, const float* matrix, const float* camera, float* offset, float* magnitude, float* dot) {
    float v[3];
    memcpy(v, dummy, sizeof v);
    if (mirror) {
        const double twice = double(dummy[0]) + double(dummy[0]);
        v[0] = float(JpFixMemOp(kJpFixSubr, twice, dummy + 0));
    }
    float d[3];
    *magnitude = VehHeadTailDirectionCpp(matrix, v, camera, d);
    const double tz = JpFixMemOp(kJpFixMul, -double(matrix[6]), d + 2);
    const double ty = JpFixMemOp(kJpFixMul, -double(matrix[5]), d + 1);
    const double tx = JpFixMemOp(kJpFixMul, -double(matrix[4]), d + 0);
    *dot = float((tz + ty) + tx);
    offset[0] = v[0];
    offset[1] = v[1];
    offset[2] = v[2];
}

// Rango comun: float(lodMultiplier * 150.0f) (fld [0xB6F118]; fmul [0x858A28]; fstp), calculado como double y guardado una sola vez.
static __declspec(noinline) float VehHeadTailRangeCpp(float lodMultiplier) {
    return float(double(lodMultiplier) * kVehHeadTail150);
}

// CVehicle::DoTailLightEffect 0x6E1916..0x6E19E6. a = dot * 0.5 + 0.2; size = float(((1 - magnitude * (1/150)) * 0.2) * dot); tren (no tranvia): a = min(a * 3, 1.0) y size = float(size * 4);
// mode 1 (freno): red = _ftol2(a * 128); mode 2: red = _ftol2(a * 96); mode 0: red = 0. range = float(lodMultiplier * 150).
__declspec(noinline) void __cdecl VehHeadTailTailCoronaCpp(float magnitude, float dot, float lodMultiplier, bool train, int mode, float* size, float* range, unsigned int* red) {
    double a = double(dot) * kVehHeadTailHalf + kVehHeadTailFifth;            // fld dot; fmul [0.5]; fadd [0.2]
    double s = double(magnitude) * kVehHeadTailInv150;                         // fld magnitude; fmul [1/150]
    s = kVehHeadTailOne - s;                                                   // fsubr [1.0]
    s = s * kVehHeadTailFifth;                                                 // fmul [0.2]
    float sz = float(JpFixMemOp(kJpFixMul, s, &dot));                          // fmul dword [dot]; fstp dword
    if (train) {
        a = a * 3.0;
        if (1.0 < a) {
            a = 1.0;
        }
        sz = float(double(sz) * 4.0);
    }
    unsigned int r = 0;
    if (mode == 1) {
        r = Ftol2RegsDblCpp(a * 128.0, 0).eax;
    } else if (mode == 2) {
        r = Ftol2RegsDblCpp(a * 96.0, 0).eax;
    }
    *red = r;
    *size = sz;
    *range = VehHeadTailRangeCpp(lodMultiplier);
}

#pragma float_control(pop)

// --- Grupo g22c (BRIEF_21): CCam::WrapAngle (0x509BE0) y FxEmitter_c::CreateParticles (0x4A41E0). Anadir al FINAL de X87Math.cpp (usa X87FistpQword, definida mas arriba) ---
// Todo es noinline y se compila con /O2 /Oi /arch:IA32 /fp:fast (PC24 del proceso). Cada bloque deja en la pila x87 los MISMOS valores muertos, las mismas banderas
// (C0-C3 y acumuladas) y el mismo FTW/TOP que el tramo del EXE, porque los bancos (Dependencies22C) comparan el FNSAVE entero salvo FIP (kCompareDeadX87Registers = true).
// Comprobado con un arnes aislado (fuera del repo, reversing/Dependencies22C/offline) que ejecuta los bytes reales del EXE reubicados frente a este codigo: ver README del banco.
// Tecnicas que hay que conservar si se toca algo (el compilador cambia el codigo con cualquiera de ellas):
//  - Las comparaciones van FUERA de `#pragma float_control(precise, on)`: con precise MSVC carga los dos operandos y emite `fcompp` (dos registros), sin precise emite `fld x; fcomp [y]`
//    como el EXE. `*y > *x` carga x y compara con la memoria de y (C0-C3 de "x frente a y"); `*y < *x` es "x mayor que y". Con NaN son falsas (mascaras 5 y 41h).
//  - Los valores que el EXE deja vivos en la pila (rate, visibilidad) se mantienen vivos hasta el final con sumideros volatile (el compilador no puede recolocarlos en el sitio).
//  - La visibilidad 1.0/0.0 se lee de un volatile (el EXE hace `fld [0x858624]`/`fld [0x858B50]`); numerador y denominador van a volatile double para que el orden de carga sea el del EXE
//    (dist - lodStart primero) y la pila no pase de tres registros.

// CCam::WrapAngle (0x509BE0): while (a >= pi) a -= 2pi; while (a < -pi) a += 2pi; ST0 queda sin redondear entre pasos (con PC24 ya son 24 bits). Cada comparacion y cada paso es un
// ayudante propio para que la pila x87 solo use el registro 1 y la comparacion salga `fld [a]; fcomp [pi]` (C0-C3 de "a frente a pi"). pi, 2pi y -pi son 0x858CB8, 0x858CBC y 0x858CC0.
static __declspec(noinline) bool CamWrapAngleAtLeastPi(float v) { return 3.14159274101257f <= v; }
static __declspec(noinline) bool CamWrapAngleBelowNegPi(float v) { return -3.14159274101257f > v; }
static __declspec(noinline) float CamWrapAngleSubTwoPi(float v) { return v - 6.28318548202515f; }
static __declspec(noinline) float CamWrapAngleAddTwoPi(float v) { return v + 6.28318548202515f; }
__declspec(noinline) void __cdecl CamWrapAngleCpp(float* angle) {
    if (CamWrapAngleAtLeastPi(*angle)) {
        float v = *angle;
        do { v = CamWrapAngleSubTwoPi(v); } while (CamWrapAngleAtLeastPi(v));
        *angle = v;
    }
    if (CamWrapAngleBelowNegPi(*angle)) {
        float v = *angle;
        do { v = CamWrapAngleAddTwoPi(v); } while (CamWrapAngleBelowNegPi(v));
        *angle = v;
    }
}

// FxEmitter_c::CreateParticles (0x4A41E0), tramos x87.
static const float s_g22cOne = 1.0f;
static volatile float s_g22cVisibilityOne = 1.0f, s_g22cVisibilityZero = 0.0f;
static volatile double s_g22cSinkVisibility, s_g22cSinkRate, s_g22cFtolRounded, s_g22cFtolFraction, s_g22cFtolSinkA, s_g22cFtolSinkB;
static __declspec(noinline) bool G22cLess(const float* x, const float* y) { return *y > *x; }     // fld [x]; fcomp [y]: x < y ordenado
static __declspec(noinline) bool G22cGreater(const float* x, const float* y) { return *y < *x; }  // fld [x]; fcomp [y]: x > y ordenado

#pragma float_control(precise, on, push)
// 0x4A4236..0x4A425D: lodStart/64 y lodEnd/64 como float (fild; fmul [1/64]; fstp dword).
static __declspec(noinline) void G22cLod(unsigned int lodStartRaw, unsigned int lodEndRaw, float* lod) {
    lod[0] = float(double(int(lodStartRaw)) * 0.015625f);
    lod[1] = float(double(int(lodEndRaw)) * 0.015625f);
}
// 0x4A421C..0x4A42B8: rate = fild(rateMult) * 0.001f (0x858CDC) vive en el registro 1, la visibilidad en el 2 y count * vis * rate + intensidad en el 3 (fstp dword + mov): al salir los
// tres registros muertos valen rate, vis y la suma, como en el EXE. La suma se copia al campo con movs enteros (el EXE hace `mov ecx,[esp+4]; mov [ebx+0x10],ecx`): sin volver a cargarla
// en la x87 (un desnormal activaria DE sin que el EXE lo cargue).
static __declspec(noinline) void G22cSumNear(float count, unsigned int rateMult, float* intensity) {
    const double rate = double(int(rateMult)) * 0.001f;
    const double visibility = s_g22cVisibilityOne;
    volatile float sum = float((double(count) * visibility) * rate + *intensity);
    s_g22cSinkVisibility = visibility;
    s_g22cSinkRate = rate;
    *reinterpret_cast<volatile unsigned int*>(intensity) = *reinterpret_cast<volatile unsigned int*>(&sum);
}
static __declspec(noinline) void G22cSumFar(float count, unsigned int rateMult, float* intensity) {
    const double rate = double(int(rateMult)) * 0.001f;
    const double visibility = s_g22cVisibilityZero;
    volatile float sum = float((double(count) * visibility) * rate + *intensity);
    s_g22cSinkVisibility = visibility;
    s_g22cSinkRate = rate;
    *reinterpret_cast<volatile unsigned int*>(intensity) = *reinterpret_cast<volatile unsigned int*>(&sum);
}
static __declspec(noinline) void G22cSumMid(const float* dist, const float* lod, float count, unsigned int rateMult, float* intensity) {
    const double rate = double(int(rateMult)) * 0.001f;
    volatile double numerator = double(*dist) - lod[0];
    volatile double denominator = double(lod[1]) - lod[0];
    const double visibility = 1.0f - numerator / denominator;
    volatile float sum = float((double(count) * visibility) * rate + *intensity);
    s_g22cSinkVisibility = visibility;
    s_g22cSinkRate = rate;
    *reinterpret_cast<volatile unsigned int*>(intensity) = *reinterpret_cast<volatile unsigned int*>(&sum);
}
#pragma float_control(pop)

// 0x4A421C..0x4A4331 (todo lo que el EXE hace en la x87 entre ProcessEmissionInfo y la creacion de matrices): visibilidad por distancia (dist < lodStart -> 1; dist > lodEnd -> 0; si no
// 1 - (dist - lodStart) / (lodEnd - lodStart)), intensidad += count * vis * rate (se guarda siempre), y los cinco filtros que hacen salir: viento < min, viento > max, lluvia < min,
// lluvia > max e intensidad < 1.0f (todos ordenados: con NaN no salen). Devuelve false si el EXE sale. Las comparaciones de dist con lodStart/lodEnd son `fld dist; fcomp [lod]`.
__declspec(noinline) bool __cdecl FxEmitterAccumulateCpp(float* intensity, const float* dist, unsigned int rateMult, unsigned int lodStartRaw, unsigned int lodEndRaw, float count,
                                                         const float* minWind, const float* maxWind, const float* minRain, const float* maxRain, const float* wind, const float* rain) {
    float lod[2];
    G22cLod(lodStartRaw, lodEndRaw, lod);
    if (G22cLess(dist, &lod[0])) {
        G22cSumNear(count, rateMult, intensity);
    } else if (G22cGreater(dist, &lod[1])) {
        G22cSumFar(count, rateMult, intensity);
    } else {
        G22cSumMid(dist, lod, count, rateMult, intensity);
    }
    if (G22cLess(wind, minWind)) return false;
    if (G22cGreater(wind, maxWind)) return false;
    if (G22cLess(rain, minRain)) return false;
    if (G22cGreater(rain, maxRain)) return false;
    return !G22cLess(intensity, &s_g22cOne);
}

// _ftol2 (0x821B40) del EXE: fld st; fst dword; fistp qword; fild qword; fsubp; fstp dword (o, con R == 0 o R indefinido, dos fstp sin la resta). El _ftol2 del CRT de la DLL (SSE) deja
// las mismas banderas PE/IE pero ningun valor en la pila: tras llamarlo se escriben aqui los dos registros que el EXE deja muertos (1 = v - R o v, 2 = R) con dos cargas y dos
// almacenamientos (sin aritmetica ni comparaciones: no cambia banderas ni C0-C3). La resta v - R solo se hace fuera de la ruta corta, donde es exacta.
static __declspec(noinline) void G22cFtolResidue(const volatile float* value) {
    const double v = static_cast<double>(*value);
    const long long rounded = X87FistpQword(v, _control87(0, 0) & _MCW_RC);
    const bool shortPath = static_cast<unsigned int>(rounded) == 0 && (static_cast<unsigned int>(static_cast<unsigned long long>(rounded) >> 32) & 0x7FFFFFFFu) == 0;
    s_g22cFtolRounded = static_cast<double>(rounded);
    s_g22cFtolFraction = shortPath ? v : v - static_cast<double>(rounded);
    const double fraction = s_g22cFtolFraction;
    const double integer = s_g22cFtolRounded;
    s_g22cFtolSinkB = integer;
    s_g22cFtolSinkA = fraction;
}
// 0x4A43B0 / 0x4A440A: fld [intensidad]; call _ftol2 -> EAX (palabra baja del entero truncado).
__declspec(noinline) int __cdecl FxEmitterTruncateCpp(const float* intensity) {
    const int whole = int(static_cast<long long>(*intensity));
    G22cFtolResidue(intensity);
    return whole;
}
// 0x4A43CB..0x4A43FB: fild [paso]; fdiv [intensidad]; fmul [delta]; fstp dword [esp]. Devuelve ST0 sin redondear a float (el llamador lo guarda y la x87 lo redondea al guardarlo).
__declspec(noinline) float __cdecl FxEmitterTimeSinceCpp(int step, const float* intensity, float deltaTime) {
    return float(double(step) / *intensity * deltaTime);
}
// 0x4A4418..0x4A4431: fld [intensidad]; call _ftol2; fild eax; fsubr [intensidad]; fstp [intensidad] (la parte fraccionaria se queda).
__declspec(noinline) void __cdecl FxEmitterTakeWholeCpp(float* intensity) {
    volatile float* const memory = intensity;
    const int whole = int(static_cast<long long>(*memory));
    G22cFtolResidue(memory);
    *memory = float(*memory - double(whole));
}

// --- Lote g22a (PointLights.cpp, 17/10/2026): GenerateLightsAffectingObject (0x6FFBB0), GetLightMultiplier (0x6FFE70), ProcessVerticalLineUsingCache (0x6FFFF0) y RenderFogEffect (0x7002D0) ---
// Disposicion de CPointLight (0x30 bytes) para los ayudantes: pos {0x00}, direccion {0x0C}, radio 0x18, color RGB {0x1C}, entidad 0x28, tipo 0x2C, tipo de niebla 0x2D, sombras 0x2E.
struct PointLightsRaw {
    float pos[3];
    float dir[3];
    float radius;
    float color[3];
    const void* entity;
    unsigned char type, fogType, shadows, pad;
};
static_assert(sizeof(PointLightsRaw) == 0x30, "CPointLight layout");

// Raiz fuera de las regiones precise (alli el compilador llama a __CIsqrt): FSQRT en linea.
static __declspec(noinline) double PointLightsSqrt(double value) {
    return sqrt(value);
}

// Devuelve el argumento en ST0 sin tocarlo: fuerza que el primer operando de la operacion siguiente ya este en ST0 y el segundo sea un operando de memoria (FADD/FMUL m32), como en
// el EXE. El compilador conmuta las operaciones conmutativas y cargaria antes el operando de memoria: con un NaN en el otro operando el EXE no marca un operando denormal (DE).
static volatile unsigned int PointLightsPassCalls;
static __declspec(noinline) double PointLightsPass(double value) {
    PointLightsPassCalls = PointLightsPassCalls + 1;   // efecto observable: sin el, el compilador fusiona las llamadas iguales y reutiliza el valor en otro registro
    return value;
}

/// --- Registros x87 muertos de CPointLights (mismo metodo que X87PlantStmt*Cpp de CTaskSimpleGangDriveBy): el EXE deja en los registros por debajo de la cima los ULTIMOS valores que
// escribio; la logica real se compila con la pila mas baja posible (variables en memoria) y estas sentencias escriben al final los valores del EXE. El elemento neutro es -0,0 (no +0,0):
// (-0) + x = x tambien para x = +-0 y la suma de dos -0 es -0, de modo que no cambia el signo de un cero.
static const volatile float kPointLightsNegZero = -0.0f;
#define POINTLIGHTS_PLANT_ZERO (kPointLightsNegZero * kPlantOne)
#pragma float_control(precise, on, push)
static __declspec(noinline) void PointLightsPlant6(double d) {
    volatile double sink;
    sink = (((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO)) + ((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO)))
         + (((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO)) + ((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + (kPlantOne * d))));
}
static __declspec(noinline) void PointLightsPlant5(double d) {
    volatile double sink;
    sink = (((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO)) + ((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO)))
         + (((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO)) + ((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + (kPlantOne * d))));
}
static __declspec(noinline) void PointLightsPlant4(double d) {
    volatile double sink;
    sink = ((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO)) + ((POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + (kPlantOne * d)));
}
static __declspec(noinline) void PointLightsPlant3(double d) {
    volatile double sink;
    sink = (POINTLIGHTS_PLANT_ZERO + POINTLIGHTS_PLANT_ZERO) + (POINTLIGHTS_PLANT_ZERO + (kPlantOne * d));
}
static __declspec(noinline) void PointLightsPlant2(double d) {
    volatile double sink;
    sink = POINTLIGHTS_PLANT_ZERO + (kPlantOne * d);
}
static __declspec(noinline) void PointLightsPlant1(double d) {
    volatile double sink;
    sink = kPlantOne * d;
}
#pragma float_control(pop)

// CPointLights::GetLightMultiplier (0x6FFE70). Pila x87 del EXE: F (producto de las luces ANTILIGHT, empieza en 1.0) debajo de S (suma, empieza en 0.0); el resultado F + S
// queda en ST0 sin guardar como float (por eso devuelve double). Para cada luz que no es de tipo 3 ni 4: dx, dy, dz se guardan como float (dy y dz se comparan con el valor de
// ST0 antes de guardarlo la primera vez y con el float guardado la segunda); la distancia (dz*dz + dy*dy) + dx*dx se queda en ST0 sin guardar como float.
// Cada comparacion del EXE es "fld X; fcomp Y" (ST0 = X). MSVC carga el operando izquierdo en ST0 y, si el derecho es un double en memoria, lo usa como operando de FCOMP: por eso
// X es siempre el izquierdo e Y un double volatile (negRadiusD/radiusD), y los codigos de condicion C0/C2/C3 finales coinciden con los del EXE.
// Registros muertos (relativos a la cima de entrada): T-1 = resultado, T-2 = S (el EXE lo deja en su registro tras el ultimo faddp), T-3 = ultimo valor del primer registro temporal
// (n1) y T-4 = ultimo valor del segundo (n2); el EXE no baja de ahi. F, S y los espejos n1/n2 viven en memoria (volatile) y se plantan al final.
#pragma float_control(precise, on, push)
extern "C" __declspec(noinline) double __cdecl PointLightsLightMultiplierDblCpp(const float* point, const void* lights, int count) {
    volatile double F = 1.0;
    volatile double S = 0.0;
    volatile double n1 = 0.0, n2 = 0.0;
    bool wrote1 = false, wrote2 = false;
    if (count > 0) {
        unsigned int counter = 0;
        int index = 0;
        do {
            const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lights) + index;
            const unsigned type = light->type;
            if (type != 3 && type != 4) {
                wrote1 = true;
                volatile float radius = light->radius;
                volatile double radiusD = double(radius);
                volatile double negRadiusD = -double(radius);
                const double dxU = double(light->pos[0]) - double(point[0]);
                volatile float dx = float(dxU);
                n1 = double(dx);
                if (dx > negRadiusD && dx < radiusD) {
                    const double dyWide = double(light->pos[1]) - double(point[1]);
                    volatile float dy = float(dyWide);
                    volatile double dyU = dyWide;
                    n1 = dyWide;
                    if (dyU > negRadiusD) {
                        n1 = double(dy);
                    }
                    if (dyU > negRadiusD && dy < radiusD) {
                        const double dzWide = double(light->pos[2]) - double(point[2]);
                        volatile float dz = float(dzWide);
                        volatile double dzU = dzWide;
                        n1 = dzWide;
                        if (dzU > negRadiusD) {
                            n1 = double(dz);
                        }
                        if (dzU > negRadiusD && dz < radiusD) {
                            const double squared = (double(dz) * double(dz) + double(dy) * double(dy)) + double(dx) * double(dx);
                            wrote2 = true;
                            n2 = double(dx) * double(dx);
                            volatile double distance = PointLightsSqrt(squared);
                            n1 = distance;
                            if (distance < radiusD) {
                                volatile double ratio = double(distance) / double(radius);
                                n1 = double(ratio);
                                if (type == 2) {
                                    F = F * double(ratio);
                                } else {
                                    volatile double attenuation = 1.0 - double(ratio);
                                    n1 = double(attenuation);
                                    volatile double redTerm = (double(attenuation) * double(light->color[0])) * double(0.33333334f);
                                    S = S + double(redTerm);
                                    n2 = double(redTerm);
                                    volatile double greenTerm = (double(attenuation) * double(light->color[1])) * double(0.33333334f);
                                    S = S + double(greenTerm);
                                    n2 = double(greenTerm);
                                    volatile double blueTerm = (double(attenuation) * double(light->color[2])) * double(0.33333334f);
                                    S = S + double(blueTerm);
                                    n1 = double(blueTerm);
                                }
                            }
                        }
                    }
                }
            }
            ++counter;
            index = static_cast<short>(counter);
        } while (index < count);
    }
    if (wrote2) {
        PointLightsPlant4(n2);
    }
    if (wrote1) {
        PointLightsPlant3(n1);
    }
    PointLightsPlant2(S);
    return F + S;
}
#pragma float_control(pop)

// CPointLights::ProcessVerticalLineUsingCache (0x6FFFF0), busqueda en las 32 entradas de aCachedMapReads (12 bytes cada una, x, y, z): "fld entrada; fcomp punto" ordenados de x, y y z
// con salida en la primera diferencia (NaN nunca coincide; +0 y -0 coinciden). MSVC deja el operando derecho en ST0: la entrada va a la derecha y la igualdad se escribe como
// dos comparaciones ordenadas (FCOMP, que da IE con NaN como el EXE; una igualdad "==" saldria como FUCOMP). Devuelve el indice de la primera coincidencia o -1.
#pragma float_control(precise, on, push)
__declspec(noinline) int __cdecl PointLightsCacheFindCpp(const float* cache, const float* point) {
    int index = 0;
    const float* entry = cache;
    do {
        bool equal = true;
        for (int axis = 0; equal && axis < 3; ++axis) {
            const unsigned int bits = *reinterpret_cast<const unsigned int*>(&entry[axis]);
            if ((bits & 0x7FFFFFFFu) > 0x7F800000u) {
                // Entrada NaN: en el EXE "fld entrada; fcomp punto" da IE y desordenado sin que el punto (aunque sea denormal) marque DE. Se copia la entrada a un double y se compara
                // consigo misma (FLD entrada; FCOMP m64): un solo registro x87, como el EXE.
                volatile double self = entry[axis];
                volatile bool unordered = entry[axis] >= self;
                (void)unordered;
                equal = false;
            } else {
                // El punto pasa por un double volatile (FLD punto; FSTP m64: marca DE si es denormal, como el operando del FCOMP del EXE) y la entrada se carga la ultima y se
                // compara con FCOMP m64: el EXE deja en T-1 la entrada (la ultima carga) y nada en T-2.
                volatile double pointWide = point[axis];
                equal = entry[axis] >= pointWide && entry[axis] <= pointWide;
            }
        }
        if (equal) {
            return index;
        }
        entry += 3;
        ++index;
    } while (entry < cache + 96);
    return -1;
}
// Copia de float con fld/fstp (un NaN senalizante sale silenciado): los dos usos de 0x700070..0x7000B8 (cache de resultados y *outZ).
__declspec(noinline) void __cdecl PointLightsFloatCopyCpp(const float* source, float* destination) {
    *destination = *source;
}
// z - 20.0 (0x858BA4) como float: el segundo argumento de CWorld::ProcessVerticalLine (0x700054..0x700060, fld/fsub/fstp).
__declspec(noinline) float __cdecl PointLightsVerticalEndZCpp(float z) {
    return z - 20.0f;
}
#pragma float_control(pop)

// CPointLights::GenerateLightsAffectingObject (0x6FFBB0), cuerpo del bucle para UNA luz (0x6FFBE0..0x6FFE48). Devuelve true si el EXE llamaria a AddAnExtraDirectionalLight
// con args = {dx, dy, dz, rojo, verde, azul} (los tres primeros son copias crudas de los float guardados). *result es el acumulador de la pila ([esp+0x1c] del EXE); totalLighting
// puede ser nulo. dy y dz se comparan primero con el valor de ST0 antes de guardarlo como float y luego con el float guardado; la distancia se guarda como float (FST) y se
// compara su valor sin redondear con el radio. El orden y los operandos de cada operacion siguen el desensamblado; las comparaciones usan el mismo operando en ST0 que el EXE
// (ver el comentario de GetLightMultiplier).
// Registros muertos: el EXE solo usa la pila x87 para los temporales de cada luz (T-1 es el registro del resultado final, que escribe el fld de la salida) y deja en T-2 .. T-4 los
// ultimos valores escritos: dx*dx (tras la distancia), y con totalLighting (1.0 - q) -> s1 (T-2), s2 (T-3) y a*G*K (T-4); despues dy/inverso (T-2) y, si es direccional, nx*dirx (T-2).
// sh2..sh4 son espejos en memoria de esos registros y se plantan antes de salir (PointLightsPlant*), como en CTaskSimpleGangDriveBy.
static volatile double kPointLightsZero = 0.0;
static volatile double kPointLightsHalf = 0.5;
static volatile double kPointLightsOne = 1.0;
static volatile double kPointLightsThird = 0.33333334f;
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsGenerateOneCpp(const void* lightPtr, const float* point, float* totalLighting, const void* entity, float* result, float* args) {
    const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lightPtr);
    const unsigned type = light->type;
    volatile double sh2, sh3, sh4;   // sin inicializar: un 0,0 inicial se quedaria en un registro x87 durante toda la funcion
    bool w2 = false, w3 = false;
    bool answer = false;
    if (type == 3 || type == 4) {
        goto done;
    }
    {
    volatile float radius = light->radius;
    volatile double radiusD = double(radius);
    volatile double negRadiusD = -double(radius);
    volatile float dx = float(double(light->pos[0]) - double(point[0]));
    if (!(dx > negRadiusD && dx < radiusD)) {
        goto done;
    }
    const double dyWide = double(light->pos[1]) - double(point[1]);
    volatile float dy = float(dyWide);
    volatile double dyU = dyWide;
    if (!(dyU > negRadiusD && dy < radiusD)) {
        goto done;
    }
    const double dzWide = double(light->pos[2]) - double(point[2]);
    volatile float dz = float(dzWide);
    volatile double dzU = dzWide;
    if (!(dzU > negRadiusD && dz < radiusD)) {
        goto done;
    }
    const double squared = (double(dz) * double(dz) + double(dy) * double(dy)) + double(dx) * double(dx);
    w2 = true;
    sh2 = double(dx) * double(dx);
    volatile double distanceWide = PointLightsSqrt(squared);
    volatile float distance = float(distanceWide);
    if (!(distanceWide < radiusD)) {
        goto done;
    }
    if (type == 2) {
        volatile float scaled = float((double(distance) / double(radius)) * double(*result));
        *result = scaled;
        goto done;
    }
    volatile double ratio = double(distance) / double(radius);
    if (totalLighting) {
        // Cada termino parte de su propia copia de (1.0 - ratio) en ST0 (el EXE usa FLD ST0 y FMUL m32): con un NaN el EXE no marca el color denormal (DE). Todos los temporales
        // van a memoria (volatile): la pila del EXE no pasa de cuatro registros y la de la DLL no debe pasar de ahi.
        const double redA = PointLightsPass(kPointLightsOne - double(ratio));
        volatile double redTerm = redA * double(light->color[0]);
        const double redScaled = PointLightsPass(double(redTerm) * kPointLightsThird);
        volatile double first = redScaled + double(*totalLighting);
        *totalLighting = float(first);
        const double greenA = PointLightsPass(kPointLightsOne - double(ratio));
        volatile double greenTerm = greenA * double(light->color[1]);
        volatile double greenScaled = double(greenTerm) * kPointLightsThird;
        volatile double second = double(first) + double(greenScaled);
        *totalLighting = float(second);
        const double blueA = PointLightsPass(kPointLightsOne - double(ratio));
        volatile double blueTerm = blueA * double(light->color[2]);
        volatile double third = double(blueTerm) * kPointLightsThird + double(second);
        *totalLighting = float(third);
        w3 = true;
        sh2 = double(second);
        sh3 = double(third);
        sh4 = double(greenScaled);
    }
    volatile float factor;
    if (ratio < kPointLightsHalf) {
        factor = 1.0f;
    } else {
        volatile double half = double(ratio) - 0.5;
        half = double(half) + double(half);
        factor = float(kPointLightsOne - double(half));
    }
    if (distance >= kPointLightsZero && distance <= kPointLightsZero) {
        goto done;
    }
    volatile double inverse = kPointLightsOne / double(distance);
    volatile float* out = args;   // los tres primeros son los float normalizados guardados (el EXE los copia crudos con MOV a los argumentos)
    out[0] = float(double(dx) * double(inverse));
    sh2 = double(dy) * double(inverse);
    out[1] = float(double(sh2));
    out[2] = float(double(inverse) * double(dz));
    if (type == 1 && light->entity != entity) {
        const double aZ = PointLightsPass(double(out[2]));
        volatile double termZ = aZ * double(light->dir[2]);
        const double aY = PointLightsPass(double(out[1]));
        volatile double termY = aY * double(light->dir[1]);
        volatile double dotZY = double(termZ) + double(termY);
        const double aX = PointLightsPass(double(out[0]));
        sh2 = aX * double(light->dir[0]);
        volatile double dot = double(dotZY) + double(sh2);
        volatile double value = -double(dot);
        value = double(value) - kPointLightsHalf;
        value = double(value) + double(value);
        volatile float argument = float(double(value));
        volatile double argumentD = double(argument);
        const float maximum = (kPointLightsZero > argumentD) ? 0.0f : argument;
        factor = float(double(maximum) * double(factor));
    }
    if (!(factor > kPointLightsZero)) {
        goto done;
    }
    out[5] = float(double(factor) * double(light->color[2]));
    out[4] = float(double(factor) * double(light->color[1]));
    out[3] = float(double(factor) * double(light->color[0]));
    answer = true;
    }
done:
    if (w3) {
        PointLightsPlant4(sh4);
        PointLightsPlant3(sh3);
        PointLightsPlant2(sh2);
    } else if (w2) {
        PointLightsPlant2(sh2);
    }
    return answer;
}
#pragma float_control(pop)

// --- CPointLights::RenderFogEffect (0x7002D0): tramos aritmeticos entre llamadas a colaboradores ---
// Constantes de comparacion como double volatile: MSVC carga el operando izquierdo en ST0 y usa el derecho (double en memoria) como operando de FCOM/FCOMP, igual que el EXE
// (fld X; fcomp Y con X en ST0), de modo que C0/C2/C3 al volver son los del EXE. FUCOMP (==) no se usa: la igualdad se escribe con dos comparaciones ordenadas.
static volatile double kPointLightsFogZero = 0.0;
static volatile double kPointLightsFogTwelve = 12.0;
static volatile double kPointLightsFogTwentyFive = 25.0;
static volatile double kPointLightsFogFifteen = 15.0;
static volatile double kPointLightsFogSevenAndHalf = 7.5;
static volatile double kPointLightsFogFive = 5.0;
static volatile double kPointLightsFogTen = 10.0;
static volatile double kPointLightsFogTwenty = 20.0;
static volatile double kPointLightsFogOne = 1.0;
static volatile double kPointLightsFogTwelfth = 0.083333336f;
static volatile double kPointLightsFogFifty = 50.0;
static volatile double kPointLightsFogFifth = 0.2f;

// Espejo de la pila x87 del EXE durante un tramo: Push/Set/Pop/Xch/BinP repiten, en el orden del desensamblado, las instrucciones que ESCRIBEN un registro (fld, operaciones
// aritmeticas, fxch); un registro sacado conserva su ultimo valor, como en el hardware. Flush() planta al final los ultimos valores de T-1 .. T-m (PointLightsPlant*), que es lo que el
// EXE deja en los registros muertos; la logica real se compila con variables en memoria para no escribir en ninguno mas profundo. Los valores se guardan como doubles (exactos: son
// float o resultados de 24 bits de mantisa). v se pone a cero con escrituras enteras para que el constructor no toque la x87.
struct PointLightsShadow {
    volatile double v[8];
    bool w[8];
    int n;
    __forceinline PointLightsShadow() : n(0) {
        for (int i = 0; i < 8; ++i) {
            reinterpret_cast<volatile unsigned long long*>(v)[i] = 0;
            w[i] = false;
        }
    }
    __forceinline void Push(double x) { v[n] = x; w[n] = true; ++n; }
    __forceinline void Set(double x) { v[n - 1] = x; w[n - 1] = true; }
    __forceinline void Pop() { --n; }
    __forceinline void Xch(int i) { const double a = v[n - 1], b = v[n - 1 - i]; v[n - 1] = b; v[n - 1 - i] = a; w[n - 1] = w[n - 1 - i] = true; }
    // operacion con destino st(k) que saca el operando (faddp st(1), fmulp st(2)...): el resultado va a v[n-1-k] y se saca la cima
    __forceinline void BinP(int k, double x) { v[n - 1 - k] = x; w[n - 1 - k] = true; --n; }
    // _ftol2 del EXE (0x821B40) con v en la cima: fld st(0), fistp, fild R, fsubp, fstp dword -> la cima queda con v - R (o con v si R es 0) y el registro siguiente con R
    __forceinline void Ftol2(double x) {
        const long long rounded = X87FistpQword(x, _control87(0, 0) & _MCW_RC);
        const unsigned int low = unsigned(static_cast<unsigned long long>(rounded));
        const unsigned int high = unsigned(static_cast<unsigned long long>(rounded) >> 32);
        if (!(low == 0 && (high & 0x7FFFFFFFu) == 0)) {
            volatile double difference = x - double(rounded);
            v[n - 1] = difference;
        } else {
            v[n - 1] = x;
        }
        w[n - 1] = true;
        v[n] = double(rounded);
        w[n] = true;
        --n;
    }
    __declspec(noinline) void Flush() {
        int m = 0;
        for (int i = 0; i < 8; ++i) {
            if (w[i]) {
                m = i + 1;
            }
        }
        switch (m) {
        case 8: case 7: case 6: PointLightsPlant6(v[5]);
        case 5: PointLightsPlant5(v[4]);
        case 4: PointLightsPlant4(v[3]);
        case 3: PointLightsPlant3(v[2]);
        case 2: PointLightsPlant2(v[1]);
        case 1: PointLightsPlant1(v[0]);
        default: break;
        }
    }
};

// _ftol2 del EXE (0x821B40): ST0 se guarda como float (FST dword: OE/UE/PE), FISTP qword con redondeo al mas cercano (PE/IE) y la correccion posterior deja el entero truncado. El _ftol2
// de la DLL (FISTTP) da el mismo entero y las mismas banderas PE/IE; el almacenamiento a float se anade aqui para las banderas de desbordamiento y subdesbordamiento.
static __forceinline long long PointLightsFogExeFtol2(double value) {
    volatile float asFloat = float(value);
    return static_cast<long long>(value);
}
// Conversion a entero de 16 bits como el EXE: _ftol2 (palabra baja del entero de 64 bits) y MOVSX AX.
static __forceinline int PointLightsFogFtol16(double value) {
    return static_cast<short>(PointLightsFogExeFtol2(value));
}

// 0x700370..0x70039D con fogType == 1: intensidad = float(CWeather::Foggyness) (FLD/FST); false si es exactamente 0 (FCOMP + TEST AH,44h + JNP). El EXE deja Foggyness en T-1 (lo mismo
// que deja esta secuencia fld/fstp/fcomp).
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsFogIntensityCpp(const float* foggyness, float* intensity) {
    *intensity = *foggyness;
    return !(*foggyness >= kPointLightsFogZero && *foggyness <= kPointLightsFogZero);
}
#pragma float_control(pop)

// Tramo 0x7003AB..0x7004E9 (luz direccional): caja de la linea de 12 unidades expandida 5 por lado, alineada a multiplos de 4. out = {gx0, gx1, gy0, gy1} (gx1/gy1 ya con +4).
// false si el EXE no entra en el bucle: gx0 > gx1 (descarta ademas la pila x87) o gy0 > gy1.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsFogDirBoundsCpp(const void* lightPtr, int* out) {
    const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lightPtr);
    PointLightsShadow sh;
    volatile double dirX12 = double(light->dir[0]) * 12.0;
    sh.Push(double(light->dir[0]));
    sh.Set(dirX12);
    volatile double dirY12Wide = double(light->dir[1]) * 12.0;
    sh.Push(double(light->dir[1]));
    sh.Set(dirY12Wide);
    volatile float dirY12 = float(dirY12Wide);
    sh.Pop();
    const double x1Base = PointLightsPass(double(dirX12));
    volatile double x1 = x1Base + double(light->pos[0]);
    sh.Set(x1);
    sh.Push(double(dirY12));
    const double y1Base = PointLightsPass(double(dirY12));
    volatile double y1Wide = y1Base + double(light->pos[1]);
    sh.Set(y1Wide);
    volatile float y1 = float(y1Wide);
    sh.Pop();
    volatile double posXD = double(light->pos[0]);
    volatile float loX, hiX;
    if (x1 < posXD) {
        loX = float(x1);
        *reinterpret_cast<volatile unsigned int*>(&hiX) = *reinterpret_cast<const unsigned int*>(&light->pos[0]);
    } else {
        hiX = float(x1);
        *reinterpret_cast<volatile unsigned int*>(&loX) = *reinterpret_cast<const unsigned int*>(&light->pos[0]);
    }
    sh.Pop();
    sh.Push(double(y1));
    sh.Pop();
    volatile double posYD = double(light->pos[1]);
    volatile double hiY, loY;
    if (y1 < posYD) {
        hiY = double(light->pos[1]);
        loY = double(y1);
        sh.Push(double(y1));
        sh.Push(double(light->pos[1]));
    } else {
        hiY = double(y1);
        loY = double(light->pos[1]);
        sh.Push(double(light->pos[1]));
        sh.Push(double(y1));
    }
    sh.Push(double(loX));
    volatile double loX5Wide = double(loX) - kPointLightsFogFive;
    sh.Set(loX5Wide);
    volatile float loX5 = float(loX5Wide);
    sh.Pop();
    sh.Push(double(hiX));
    volatile double hiX5Wide = double(hiX) + kPointLightsFogFive;
    sh.Set(hiX5Wide);
    volatile float hiX5 = float(hiX5Wide);
    sh.Pop();
    sh.Xch(1);
    volatile double loY5Wide = double(loY) - kPointLightsFogFive;
    sh.Set(loY5Wide);
    volatile float loY5 = float(loY5Wide);
    sh.Pop();
    volatile double hiY5 = double(hiY) + kPointLightsFogFive;
    sh.Set(hiY5);
    sh.Push(double(loX5));
    sh.Ftol2(double(loX5));
    const int xa = PointLightsFogFtol16(double(loX5));
    const int gx0 = xa - xa % 4;
    sh.Push(double(hiX5));
    sh.Ftol2(double(hiX5));
    const int gx1 = PointLightsFogFtol16(double(hiX5)) + 4;
    if (gx0 > gx1) {
        sh.Pop();
        sh.Flush();
        return false;
    }
    sh.Push(double(loY5));
    sh.Ftol2(double(loY5));
    const int ya = PointLightsFogFtol16(double(loY5));
    const int gy0 = ya - ya % 4;
    sh.Ftol2(double(hiY5));
    const int gy1 = PointLightsFogFtol16(hiY5) + 4;
    sh.Flush();
    out[0] = gx0;
    out[1] = gx1;
    out[2] = gy0;
    out[3] = gy1;
    return gy0 <= gy1;
}
#pragma float_control(pop)

// Tramo 0x7004F2..0x7005A1 (celda de la luz direccional, antes de ProcessVerticalLine): solo las celdas con (gy>>2 ^ gx>>2) & 15 impar entran; el punto de la celda (gx, gy, z + 10)
// debe quedar a menos de 12 por delante de la luz (producto con la direccion > 0 y < 12) y a menos de 5 del eje (cuadrado de la distancia perpendicular < 25). origin = {gx, gy, z + 10}
// como float, *endZ = float(origin.z - 20) (segundo argumento de CWorld::ProcessVerticalLine) y *tab = ((gy>>2 ^ gx>>2) & 15) >> 1.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsFogDirCellPreCpp(const void* lightPtr, int gx, int gy, float* origin, float* endZ, int* tab) {
    const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lightPtr);
    const int cell = ((gy >> 2) ^ (gx >> 2)) & 15;
    if (!(cell & 1)) {
        return false;
    }
    PointLightsShadow sh;
    *tab = static_cast<short>(cell) >> 1;
    sh.Push(double(gx));
    origin[0] = float(double(gx));
    volatile double oxWide = double(gx) - double(light->pos[0]);
    sh.Set(oxWide);
    volatile float ox = float(oxWide);
    sh.Pop();
    sh.Push(double(gy));
    origin[1] = float(double(gy));
    volatile double oyWide = double(gy) - double(light->pos[1]);
    sh.Set(oyWide);
    volatile float oy = float(oyWide);
    sh.Pop();
    sh.Push(double(ox));
    const double oxBase = PointLightsPass(double(ox));
    volatile double oxDir = oxBase * double(light->dir[0]);
    sh.Set(oxDir);
    sh.Push(double(oy));
    const double oyBase = PointLightsPass(double(oy));
    volatile double oyDir = oyBase * double(light->dir[1]);
    sh.Set(oyDir);
    volatile double dot = double(oxDir) + double(oyDir);
    sh.BinP(1, dot);
    volatile float dotF = float(dot);
    sh.Pop();
    if (!(dot > kPointLightsFogZero)) {
        sh.Flush();
        return false;
    }
    sh.Push(double(dotF));
    sh.Pop();
    if (!(dotF < kPointLightsFogTwelve)) {
        sh.Flush();
        return false;
    }
    sh.Push(double(oy));
    volatile double oySquared = double(oy) * double(oy);
    sh.Set(oySquared);
    sh.Push(double(ox));
    volatile double oxSquared = double(ox) * double(ox);
    sh.Set(oxSquared);
    volatile double plane = double(oySquared) + double(oxSquared);
    sh.BinP(1, plane);
    sh.Push(double(dotF));
    volatile double dotSquared = double(dotF) * double(dotF);
    sh.Set(dotSquared);
    volatile double perpendicular = double(plane) - double(dotSquared);
    sh.BinP(1, perpendicular);
    sh.Pop();
    if (!(perpendicular < kPointLightsFogTwentyFive)) {
        sh.Flush();
        return false;
    }
    volatile double originZWide = double(light->pos[2]) + kPointLightsFogTen;
    sh.Push(double(light->pos[2]));
    sh.Set(originZWide);
    origin[2] = float(originZWide);
    sh.Pop();
    sh.Push(double(origin[2]));
    volatile double endZWide = double(origin[2]) - kPointLightsFogTwenty;
    sh.Set(endZWide);
    *endZ = float(endZWide);
    sh.Pop();
    sh.Flush();
    return true;
}
#pragma float_control(pop)

// Tramo 0x70060E..0x700754 (luz direccional, tras ProcessVerticalLine con exito): nuevo punto de suelo z + 1.3 (0x859918), mismo filtro con el producto y la distancia perpendicular y
// calculo del factor alfa que se queda en *alpha como float: (0x70070C..0x700754, ver abajo). posn = {gx, gy, z de suelo + 1.3} (argumento de CSprite::CalcScreenCoors). colPointZ apunta al
// z del CColPoint; origin al origen que devolvio PointLightsFogDirCellPreCpp (x e y como float).
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsFogDirCellPostCpp(const void* lightPtr, const float* origin, const float* colPointZ, const float* intensity, float* posn, float* alpha) {
    const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lightPtr);
    PointLightsShadow sh;
    sh.Push(double(*colPointZ));
    volatile double groundZWide = double(*colPointZ) + double(1.2999999523162842);
    sh.Set(groundZWide);
    volatile float groundZ = float(groundZWide);
    sh.Pop();
    sh.Push(double(origin[0]));
    volatile double oxWide = double(origin[0]) - double(light->pos[0]);
    sh.Set(oxWide);
    volatile float ox = float(oxWide);
    sh.Pop();
    sh.Push(double(origin[1]));
    volatile double oyWide = double(origin[1]) - double(light->pos[1]);
    sh.Set(oyWide);
    volatile float oy = float(oyWide);
    sh.Pop();
    sh.Push(double(groundZ));
    const double groundBase = PointLightsPass(double(groundZ));
    volatile double ozWide = groundBase - double(light->pos[2]);
    sh.Set(ozWide);
    volatile float oz = float(ozWide);
    const double ozBase = PointLightsPass(double(ozWide));
    volatile double ozDir = ozBase * double(light->dir[2]);
    sh.Set(ozDir);
    sh.Push(double(ox));
    const double oxBase = PointLightsPass(double(ox));
    volatile double oxDir = oxBase * double(light->dir[0]);
    sh.Set(oxDir);
    volatile double dotZX = double(ozDir) + double(oxDir);
    sh.BinP(1, dotZX);
    sh.Push(double(oy));
    const double oyBase = PointLightsPass(double(oy));
    volatile double oyDir = oyBase * double(light->dir[1]);
    sh.Set(oyDir);
    volatile double dot = double(dotZX) + double(oyDir);
    sh.BinP(1, dot);
    volatile float dotF = float(dot);
    sh.Pop();
    if (!(dot > kPointLightsFogZero)) {
        sh.Flush();
        return false;
    }
    sh.Push(double(dotF));
    sh.Pop();
    if (!(dotF < kPointLightsFogTwelve)) {
        sh.Flush();
        return false;
    }
    sh.Push(double(oz));
    volatile double ozSquared = double(oz) * double(oz);
    sh.Set(ozSquared);
    sh.Push(double(oy));
    volatile double oySquared = double(oy) * double(oy);
    sh.Set(oySquared);
    volatile double zy = double(ozSquared) + double(oySquared);
    sh.BinP(1, zy);
    sh.Push(double(ox));
    volatile double oxSquared = double(ox) * double(ox);
    sh.Set(oxSquared);
    volatile double zyx = double(zy) + double(oxSquared);
    sh.BinP(1, zyx);
    volatile float distSquared = float(zyx);
    sh.Pop();
    sh.Push(double(dotF));
    volatile double dotSquared = double(dotF) * double(dotF);
    sh.Set(dotSquared);
    volatile double perpendicular = double(distSquared) - double(dotSquared);
    sh.Set(perpendicular);
    if (!(perpendicular < kPointLightsFogTwentyFive)) {
        sh.Pop();
        sh.Flush();
        return false;
    }
    volatile double rootPerp = PointLightsSqrt(perpendicular);
    sh.Set(rootPerp);
    // 0x70070C..0x700754: pila [S = raiz perpendicular]
    sh.Push(double(dotF));
    volatile double u = double(dotF) * kPointLightsFogTwelfth;
    sh.Set(u);
    sh.Push(double(distSquared));
    volatile double sd = PointLightsSqrt(double(distSquared));
    sh.Set(sd);
    volatile double ratio = double(dotF) / double(sd);
    sh.Set(ratio);
    const double ratioBase = PointLightsPass(double(ratio));
    volatile double scaled0 = ratioBase * double(*intensity);
    sh.Set(scaled0);
    volatile double scaled = double(scaled0) * kPointLightsFogFifty;
    sh.Set(scaled);
    sh.Push(double(u));
    volatile double uu = double(u) * double(u);
    sh.Set(uu);
    volatile double oneMinusUU = kPointLightsFogOne - double(uu);
    sh.Set(oneMinusUU);
    volatile double window = double(scaled) * double(oneMinusUU);
    sh.BinP(1, window);
    sh.Push(double(rootPerp));
    volatile double rootFifth = double(rootPerp) * kPointLightsFogFifth;
    sh.Set(rootFifth);
    sh.Xch(3);
    volatile double rootFifth2 = double(rootPerp) * kPointLightsFogFifth;
    sh.Set(rootFifth2);
    volatile double edge = double(rootFifth) * double(rootFifth2);
    sh.BinP(3, edge);
    sh.Xch(2);
    volatile double oneMinusEdge = kPointLightsFogOne - double(edge);
    sh.Set(oneMinusEdge);
    volatile double alphaWide = double(window) * double(oneMinusEdge);
    sh.BinP(2, alphaWide);
    sh.Xch(1);
    sh.Pop();
    sh.Pop();
    *alpha = float(alphaWide);
    sh.Flush();
    // copias crudas (MOV) como las del EXE: ningun FLD/FSTP intermedio
    *reinterpret_cast<unsigned int*>(&posn[0]) = *reinterpret_cast<const unsigned int*>(&origin[0]);
    *reinterpret_cast<unsigned int*>(&posn[1]) = *reinterpret_cast<const unsigned int*>(&origin[1]);
    *reinterpret_cast<unsigned int*>(&posn[2]) = *reinterpret_cast<volatile unsigned int*>(&groundZ);
    return true;
}
#pragma float_control(pop)

// Tramo 0x700898..0x7009A8 (luz no direccional, tras ProcessVerticalLineUsingCache con exito): caja de lado 2*size alrededor de la luz (size = 9 con niebla 1, 3 con niebla 2),
// alineada a multiplos de 2. out = {gx0, gx1, gy0, gy1} (gx1/gy1 ya con +2); false si no entra (gx0 > gx1 o gy0 > gy1).
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsFogPointBoundsCpp(const void* lightPtr, const float* size, int* out) {
    const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lightPtr);
    PointLightsShadow sh;
    sh.Push(double(light->pos[0]));
    volatile double xLow = double(light->pos[0]) - double(*size);
    sh.Set(xLow);
    sh.Push(double(*size));
    volatile double xHighWide = double(*size) + double(light->pos[0]);
    sh.Set(xHighWide);
    volatile float xHigh = float(xHighWide);
    sh.Pop();
    sh.Push(double(light->pos[1]));
    volatile double yLowWide = double(light->pos[1]) - double(*size);
    sh.Set(yLowWide);
    volatile float yLow = float(yLowWide);
    sh.Pop();
    sh.Push(double(*size));
    volatile double yHighWide = double(*size) + double(light->pos[1]);
    sh.Set(yHighWide);
    volatile float yHigh = float(yHighWide);
    sh.Pop();
    sh.Ftol2(double(xLow));
    const int xa = PointLightsFogFtol16(double(xLow));
    const int gx0 = xa - xa % 2;
    sh.Push(double(xHigh));
    sh.Ftol2(double(xHigh));
    const int gx1 = PointLightsFogFtol16(double(xHigh)) + 2;
    if (gx0 > gx1) {
        sh.Flush();
        return false;
    }
    sh.Push(double(yLow));
    sh.Ftol2(double(yLow));
    const int ya = PointLightsFogFtol16(double(yLow));
    const int gy0 = ya - ya % 2;
    sh.Push(double(yHigh));
    sh.Ftol2(double(yHigh));
    const int gy1 = PointLightsFogFtol16(double(yHigh)) + 2;
    sh.Flush();
    out[0] = gx0;
    out[1] = gx1;
    out[2] = gy0;
    out[3] = gy1;
    return gy0 <= gy1;
}
#pragma float_control(pop)

// Tramo 0x70094F..0x700A9C (celda de la luz no direccional, antes de CalcScreenCoors): cell = ((gy/2) ^ (gx/2)) & 15 debe ser impar; la distancia de la celda a la luz debe ser menor
// que size y la de la celda a la camara menor de 15; la atenuacion (1 hasta 7.5 de la camara, despues 1 - (d - 7.5)*0.13333334) y 1 - (d/size)^2 dan el alfa de *alpha (por intensidad y 37).
// posn = {gx, gy, z de CPointLights::ProcessVerticalLineUsingCache + 1.6}; *tab = cell >> 1; camera apunta a x, y, z.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl PointLightsFogPointCellPreCpp(const void* lightPtr, int gx, int gy, const float* size, const float* camera, const float* intensity, const float* groundZ, float* posn, float* alpha, int* tab) {
    const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lightPtr);
    const int cell = (((gy - (gy >> 31)) >> 1) ^ ((gx - (gx >> 31)) >> 1)) & 15;
    if (!(cell & 1)) {
        return false;
    }
    PointLightsShadow sh;
    *tab = static_cast<short>(cell) >> 1;
    sh.Push(double(gy));
    sh.Push(double(gy));
    volatile double oy = double(gy) - double(light->pos[1]);
    sh.Set(oy);
    sh.Push(double(gx));
    volatile float gxf = float(double(gx));
    volatile double ox = double(gx) - double(light->pos[0]);
    sh.Set(ox);
    sh.Push(double(ox));
    volatile double oxSquared = double(ox) * double(ox);
    sh.Set(oxSquared);
    sh.Push(double(oy));
    volatile double oySquared = double(oy) * double(oy);
    sh.Set(oySquared);
    volatile double planeSquared = double(oxSquared) + double(oySquared);
    sh.BinP(1, planeSquared);
    volatile double distanceWide = PointLightsSqrt(planeSquared);
    sh.Set(distanceWide);
    sh.BinP(2, distanceWide);   // fstp st(2): copia la cima en T-2 y la saca
    sh.Pop();
    volatile float distance = float(distanceWide);
    volatile double sizeD = double(*size);
    sh.Pop();
    if (!(distanceWide < sizeD)) {
        sh.Flush();
        return false;
    }
    sh.Push(double(gy));
    volatile double cameraY1 = double(gy) - double(camera[1]);
    sh.Set(cameraY1);
    sh.Push(double(gy));
    volatile double cameraY2 = double(gy) - double(camera[1]);
    sh.Set(cameraY2);
    volatile double cameraY = double(cameraY1) * double(cameraY2);
    sh.BinP(1, cameraY);
    sh.Push(double(gxf));
    volatile double cameraX1 = double(gxf) - double(camera[0]);
    sh.Set(cameraX1);
    sh.Push(double(gxf));
    volatile double cameraX2 = double(gxf) - double(camera[0]);
    sh.Set(cameraX2);
    volatile double cameraX = double(cameraX1) * double(cameraX2);
    sh.BinP(1, cameraX);
    volatile double cameraPlane = double(cameraY) + double(cameraX);
    sh.BinP(1, cameraPlane);
    volatile double cameraDistanceWide = PointLightsSqrt(cameraPlane);
    sh.Set(cameraDistanceWide);
    volatile float cameraDistance = float(cameraDistanceWide);
    sh.Pop();
    if (!(cameraDistanceWide < kPointLightsFogFifteen)) {
        sh.Flush();
        return false;
    }
    sh.Push(double(cameraDistance));
    sh.Pop();
    volatile double fade;
    if (cameraDistance < kPointLightsFogSevenAndHalf) {
        fade = 1.0;
        sh.Push(1.0);
    } else {
        sh.Push(double(cameraDistance));
        volatile double fadeA = double(cameraDistance) - kPointLightsFogSevenAndHalf;
        sh.Set(fadeA);
        volatile double fadeB = double(fadeA) * double(0.13333334f);
        sh.Set(fadeB);
        fade = kPointLightsFogOne - double(fadeB);
        sh.Set(fade);
    }
    sh.Push(1.0);
    volatile double inverseSize = kPointLightsFogOne / double(*size);
    sh.Set(inverseSize);
    volatile double ratio = double(inverseSize) * double(distance);
    sh.Set(ratio);
    sh.Push(double(ratio));
    volatile double ratioSquared = double(ratio) * double(ratio);
    sh.Set(ratioSquared);
    volatile double inside = kPointLightsFogOne - double(ratioSquared);
    sh.Set(inside);
    volatile double faded = double(inside) * double(fade);
    sh.Set(faded);
    volatile double withIntensity = double(faded) * double(*intensity);
    sh.Set(withIntensity);
    volatile double alphaWide = double(withIntensity) * 37.0;
    sh.Set(alphaWide);
    sh.Pop();
    sh.Pop();
    sh.Pop();
    *alpha = float(alphaWide);
    *reinterpret_cast<unsigned int*>(&posn[0]) = *reinterpret_cast<volatile unsigned int*>(&gxf);
    posn[1] = float(double(gy));
    sh.Pop();
    sh.Push(double(*groundZ));
    volatile double posnZWide = double(*groundZ) + double(1.600000023841858);
    sh.Set(posnZWide);
    posn[2] = float(posnZWide);
    sh.Pop();
    sh.Flush();
    return true;
}
#pragma float_control(pop)

// Argumentos de CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect (0x70E780) tras CalcScreenCoors con exito (0x70076A..0x700817 y 0x700AAC..0x700B6B): angulo = float(timeValue *
// 0.0007666015625 (direccional, 0x872610) o 0.000191650390625 (resto, 0x872600)), 1/z, intensidad y RGB como low32(_ftol2(alfa [* color])), tamanos h*tab[tab] (* 0.7 en las luces no
// direccionales) y w*tab[tab]. screen = {x, y, z} que devolvio CalcScreenCoors (copias crudas); alpha = el float que dejo PointLightsFogDirCellPostCpp/PointLightsFogPointCellPreCpp.
// Registros muertos: el EXE deja T-1 = w*tab (sin guardar) y T-2 = el entero redondeado de la conversion del rojo (_ftol2).
#pragma float_control(precise, on, push)
__declspec(noinline) void __cdecl PointLightsFogSpriteCpp(const void* lightPtr, const float* alpha, const float* screen, float w, float h, const float* table, int tab, int timeValue, bool pointLight, PointLightsFogSprite* result) {
    const PointLightsRaw* light = reinterpret_cast<const PointLightsRaw*>(lightPtr);
    PointLightsShadow sh;
    result->angle = float(double(timeValue) * (pointLight ? 0.000191650390625 : 0.0007666015625));
    result->recipNearZ = float(1.0 / double(screen[2]));
    // alpha va a una variable volatile double y se multiplica siempre como primer operando (FLD alpha; FMUL color): con alpha NaN el EXE no marca el operando denormal del color
    volatile double alphaWide = double(*alpha);
    result->intensity = static_cast<int>(PointLightsFogExeFtol2(alphaWide));
    sh.Push(alphaWide);
    sh.Ftol2(alphaWide);
    const double alphaBlue = PointLightsPass(alphaWide);
    volatile double blue = alphaBlue * double(light->color[2]);
    result->blue = static_cast<int>(PointLightsFogExeFtol2(blue));
    sh.Push(blue);
    sh.Ftol2(blue);
    const double alphaGreen = PointLightsPass(alphaWide);
    volatile double green = alphaGreen * double(light->color[1]);
    result->green = static_cast<int>(PointLightsFogExeFtol2(green));
    sh.Push(green);
    sh.Ftol2(green);
    const double alphaRed = PointLightsPass(alphaWide);
    volatile double red = alphaRed * double(light->color[0]);
    result->red = static_cast<int>(PointLightsFogExeFtol2(red));
    sh.Push(red);
    sh.Ftol2(red);
    if (pointLight) {
        result->h = float((double(h) * double(table[tab])) * double(0.7f));
    } else {
        result->h = float(double(h) * double(table[tab]));
    }
    volatile double wTab = double(w) * double(table[tab]);
    result->w = float(wTab);
    sh.Push(wTab);
    sh.Pop();
    sh.Flush();
    *reinterpret_cast<unsigned int*>(&result->x) = *reinterpret_cast<const unsigned int*>(&screen[0]);
    *reinterpret_cast<unsigned int*>(&result->y) = *reinterpret_cast<const unsigned int*>(&screen[1]);
    *reinterpret_cast<unsigned int*>(&result->z) = *reinterpret_cast<const unsigned int*>(&screen[2]);
}
#pragma float_control(pop)

// --- Grupo g23a (BRIEF_21): CVehicle::DoVehicleLights (0x6E1A60), prefijo VehLights, y bloques x87 de DoHeadLightEffect (0x6E0A50), prefijo VehHeadTail. Anexar al FINAL de X87Math.cpp. ---
// Reutiliza JpFixMemOp / kJpFix* / JetPackNegSinExtCpp / JetPackCosExtCpp / FormationRootCpp / X87FistpQword / PointLightsPlant1..5 y VehHeadTail{Transform,NormaliseMag,Direction}Cpp (todos
// definidos mas arriba en este archivo), y sustituye a VehHeadTailHeadGeometryCpp / VehHeadTailHeadCoronaCpp de g22b (que se retiran).
// Metodo (el mismo que X87Plant* y PointLights*): cada bloque corresponde a un tramo recto del EXE que empieza y termina con la pila x87 vacia. El bloque calcula todo con valores en
// memoria (double volatile = lo que el EXE deja en un registro, que con el proceso a 24 bits ya esta redondeado; float = lo que guarda con fst/fstp dword), de modo que ninguna instruccion
// use mas registros que el EXE; hace LA MISMA comparacion ordenada (fld izquierdo; fcomp derecho, el izquierdo es el ST0 del EXE) que deja C0/C2/C3 como el EXE y levanta las mismas banderas
// (las operaciones repetidas con los mismos operandos no cambian las banderas acumuladas), planta con PointLightsPlantN los valores que el EXE deja en los registros muertos T-N .. T-2 y
// termina, cuando el EXE termina con un fstp dword que redondea (C1), con ese mismo almacenamiento sobre T-1. Las constantes que el EXE compara con operando de memoria van en
// `const volatile double` (fcomp qword) para que el compilador no cargue un registro mas.
#pragma float_control(precise, on, push)
static const volatile double kVehLightsZero      = 0.0;                                         // 0x858B50
static const volatile double kVehLightsHundred   = double(std::bit_cast<float>(0x42C80000u));   // 0x858628 (100.0)
static const volatile double kVehLightsForty     = double(std::bit_cast<float>(0x42200000u));   // 0x858A10 (40.0)
static const volatile double kVehLightsPopUp     = double(std::bit_cast<float>(0x3F32B8C3u));   // 0x871E28 (0.6981317)
static const volatile double kVehLightsSlowSq    = double(std::bit_cast<float>(0x3E4F5C28u));   // 0x871FDC (0.2025)
static const volatile double kVehLightsLimit     = double(std::bit_cast<float>(0x3F666666u));   // 0x858C20 (0.9)
static const volatile double kVehLightsLimitTrain = double(std::bit_cast<float>(0x3F59999Au));  // 0x862E1C (0.85)
static const volatile double kVehLightsOneV      = double(std::bit_cast<float>(0x3F800000u));   // 0x858624 (1.0)
static constexpr double kVehLightsStep   = double(std::bit_cast<float>(0x3C23D70Au));           // 0x871E2C (0.01)
static constexpr double kVehLightsFour   = double(std::bit_cast<float>(0x40800000u));           // 0x858B90 (4.0)
static constexpr double kVehLights190    = double(std::bit_cast<float>(0x433E0000u));           // 0x858A20
static constexpr double kVehLights255    = double(std::bit_cast<float>(0x437F0000u));           // 0x859AAC
static constexpr double kVehLights210    = double(std::bit_cast<float>(0x43520000u));           // 0x863B34
static constexpr double kVehLights195    = double(std::bit_cast<float>(0x43430000u));           // 0x871FD8

// 0x6E1ABE..0x6E1B96: un vehiculo abandonado con las luces encendidas las apaga si |dx| + |dy| al dueno de la camara supera 100. Por componente: fld cam; fsub dword [pos]; fcomp 0;
// se recalcula (y se invierte con fchs si salio negativo; con NaN, cero o positivo no se invierte). La suma |dy| + |dx| se compara con 100 (fcomp). Registros muertos: T-2 = suma, T-1 = |dx|.
// camera / position apuntan a x, y. Devuelve true si hay que apagar las luces.
__declspec(noinline) bool __cdecl VehLightsAbandonedFarCpp(const float* camera, const float* position) {
    volatile double dx = JpFixMemOp(kJpFixSub, double(camera[0]), position + 0);
    volatile double ax = (dx < kVehLightsZero) ? -dx : dx;
    volatile double dy = JpFixMemOp(kJpFixSub, double(camera[1]), position + 1);
    volatile double ay = (dy < kVehLightsZero) ? -dy : dy;
    volatile double sum = ay + ax;
    const bool far = sum > kVehLightsHundred;
    PointLightsPlant2(sum);
    PointLightsPlant1(ax);
    return far;
}

// 0x6E1C8E..0x6E1CC8 (ZR-350, luces encendidas o alarma): v = ts * 0.01 + prop (fadd dword [prop]); si objetivo < v, v = objetivo; *prop = float(v) (fst); devuelve true si v < objetivo
// (el EXE sale de la funcion). Registros muertos: T-2 = objetivo, T-1 = v.
__declspec(noinline) bool __cdecl VehLightsPropRiseCpp(const float* timeStep, float* prop) {
    volatile double scaled = double(*timeStep) * kVehLightsStep;
    volatile double sum = JpFixMemOp(kJpFixAdd, scaled, prop);
    volatile double kept = (kVehLightsPopUp < sum) ? double(kVehLightsPopUp) : double(sum);
    *prop = float(kept);
    const bool below = kept < kVehLightsPopUp;
    PointLightsPlant2(kVehLightsPopUp);
    PointLightsPlant1(kept);
    return below;
}

// 0x6E1C53..0x6E1C89 (ZR-350, luces apagadas): si prop > 0 (fld; fcomp 0), a = float(prop - ts * 0.01) (fsubr dword [prop]; fstp dword) y *prop = 0x420800(0.0f, a) (devuelve a salvo que 0 > a: un NaN, +-0 y los positivos se conservan, un negativo da +0).
// Registro muerto: T-1 = el ultimo valor cargado (prop si no cambia, el resultado si cambia: lo deja el propio almacenamiento final).
__declspec(noinline) void __cdecl VehLightsPropFallCpp(const float* timeStep, float* prop) {
    volatile double current = double(*prop);
    if (!(current > kVehLightsZero)) {
        return;
    }
    volatile double scaled = double(*timeStep) * kVehLightsStep;
    volatile double difference = JpFixMemOp(kJpFixSubr, scaled, prop);
    volatile float stored = float(difference);
    volatile double loaded = double(stored);
    volatile double result = (kVehLightsZero > loaded) ? 0.0 : double(loaded);   // 0x420800(0.0f, a): fld 0; fcomp a; devuelve a salvo que 0 > a (con NaN o igualdad devuelve a)
    *prop = float(result);
}

// 0x6E2753..0x6E2791: (vx*vx + vy*vy) < 0.2025 (fcomp). Registros muertos: T-4 = vy*vy, T-3 = suma, T-2 = vx, T-1 = vy. speed apunta a x, y.
__declspec(noinline) bool __cdecl VehLightsSlowCpp(const float* speed) {
    volatile double vy = double(speed[1]);
    volatile double vx = double(speed[0]);
    volatile double x2 = vx * vx;
    volatile double y2 = vy * vy;
    volatile double sum = x2 + y2;
    const bool slow = sum < kVehLightsSlowSq;
    PointLightsPlant4(y2);
    PointLightsPlant3(sum);
    PointLightsPlant2(vx);
    PointLightsPlant1(vy);
    return slow;
}

// 0x6E27EE..0x6E283E: point = position - forward * 4 (x e y: fsub st(i) con el producto sin redondear a float; z: fsub dword [z*4 ya guardado como float]). Registros muertos:
// T-3 = y, T-2 = fy * 4, T-1 = z (el fstp dword de z es la ultima instruccion x87).
__declspec(noinline) void __cdecl VehLightsTrailPointCpp(const float* position, const float* forward, float* point) {
    volatile double fx = double(forward[0]) * kVehLightsFour;
    volatile double fy = double(forward[1]) * kVehLightsFour;
    volatile double fz = double(forward[2]) * kVehLightsFour;
    float zq = float(fz);
    volatile double xr = double(position[0]) - fx;
    point[0] = float(xr);
    volatile double yr = double(position[1]) - fy;
    point[1] = float(yr);
    volatile double zr = JpFixMemOp(kJpFixSub, double(position[2]), &zq);
    PointLightsPlant3(yr);
    PointLightsPlant2(fy);
    point[2] = float(zr);
}

// 0x6E2850..0x6E2867: brake > 0 (fld; fcomp 0). Registro muerto: T-1 = brake.
__declspec(noinline) bool __cdecl VehLightsBrakeCpp(const float* brake) {
    return double(*brake) > kVehLightsZero;
}

// 0x6E287A..0x6E28C1: out = -forward componente a componente (fld; fchs; fstp dword). Registro muerto: T-1 = -fz.
__declspec(noinline) void __cdecl VehLightsNegateCpp(const float* forward, float* out) {
    out[0] = float(-double(forward[0]));
    out[1] = float(-double(forward[1]));
    out[2] = float(-double(forward[2]));
}

// --- DoHeadLightEffect (0x6E0A50 y las dos copias en linea de DoVehicleLights con lightId = 1) ---
// 0x6E0A88..0x6E0AB9: el dummy es exactamente (0, 0, 0) (fld; fcomp 0 por componente, ordenado: NaN es distinto de cero; sale en la primera componente distinta). T-1 = ultima componente cargada.
__declspec(noinline) bool __cdecl VehHeadTailDummyZeroCpp(const float* dummy) {
    for (int i = 0; i < 3; ++i) {
        volatile double component = double(dummy[i]);
        if (!(component >= kVehLightsZero && component <= kVehLightsZero)) {
            return false;
        }
    }
    return true;
}

// CVehicle::DoHeadLightEffect 0x6E0AC5..0x6E0BD6. dummy = posicion del dummy en espacio del modelo (x, y, z); forward = frente de la matriz del vehiculo (x, y, z) o nulo si el vehiculo
// no tiene matriz (entonces direccion = (-sin(heading), cos(heading), 0)); mirror = lado izquierdo (x = x - (x + x)); matrix = matriz del argumento (float[16] como CMatrix);
// camera = posicion de la camara. Salidas: offset = punto (relativo al vehiculo) que se pasa a RegisterCorona, magnitude = distancia a la camara (0x59C970), dot = producto escalar
// del vector normalizado a la camara con el frente de la matriz sumado z, y, x (float).
// Registros muertos: el bloque usa cinco registros bajo la cima y los deja con z*z (T-5), la suma de cuadrados (T-4) y la x de delta (T-3) de 0x59C970, el termino x del producto escalar
// (T-2) y el producto escalar sin redondear (T-1); se plantan al final y la ultima instruccion x87 es el fstp dword del producto escalar, como en el EXE.
__declspec(noinline) void __cdecl VehHeadTailHeadGeometryCpp(const float* dummy, const float* forward, float heading, bool mirror, const float* matrix, const float* camera,
                                                             float* offset, float* magnitude, float* dot) {
    double x1, y1;
    volatile float z1;
    if (forward) {
        x1 = double(forward[0]) * kJpFixHeight;
        y1 = double(forward[1]) * kJpFixHeight;
        z1 = float(double(forward[2]) * kJpFixHeight);
    } else {
        x1 = JetPackNegSinExtCpp(heading) * kJpFixHeight;      // fld heading; fsin; fchs; fmul [0x858C28] (el seno de 64 bits llega al producto sin guardarse)
        volatile float cosine = float(JetPackCosExtCpp(heading)); // fld heading; fcos; fstp dword
        y1 = double(cosine) * kJpFixHeight;
        z1 = float(double(0.0f) * kJpFixHeight);
    }
    float v[3];
    v[0] = float(JpFixMemOp(kJpFixAdd, x1, dummy + 0));
    v[1] = float(JpFixMemOp(kJpFixAdd, y1, dummy + 1));
    v[2] = float(JpFixMemOp(kJpFixAdd, double(z1), dummy + 2));
    if (mirror) {
        const double twice = double(dummy[0]) + double(dummy[0]);
        v[0] = float(JpFixMemOp(kJpFixSubr, twice, v + 0));
    }
    memcpy(offset, v, sizeof v);   // copia entera: un fld/fstp mas dejaria otro valor en T-1
    float d[3];
    double stale[3];
    *magnitude = VehHeadTailDirectionCpp(matrix, v, camera, d, stale);
    volatile double tz = JpFixMemOp(kJpFixMul, double(d[2]), matrix + 6);
    volatile double ty = JpFixMemOp(kJpFixMul, double(d[1]), matrix + 5);
    volatile double tx = JpFixMemOp(kJpFixMul, double(d[0]), matrix + 4);
    PointLightsPlant5(stale[0]);
    PointLightsPlant4(stale[1]);
    PointLightsPlant3(stale[2]);
    PointLightsPlant2(tx);
    *dot = float((tz + ty) + tx);
}

// 0x6E0BE0..0x6E0BEF: dot > 0 (fld dword [dot]; fcomp 0; ordenado, NaN no pasa). T-1 = dot.
__declspec(noinline) bool __cdecl VehHeadTailPositiveCpp(const float* dot) {
    return double(*dot) > kVehLightsZero;
}

// 0x6E0C20..0x6E0C58: sqrtDot = float(sqrt(dot)) (fld; fsqrt; fstp dword); limite = 0.9 (0.85 en un tren que no es el tranvia); devuelve sqrtDot > limite (fcomp st(1), ordenado).
// Registros muertos: T-2 = sqrtDot, T-1 = limite en el EXE; aqui no se plantan porque DoHeadLightEffect siempre pasa despues por VehHeadTailHeadCoronaCpp, que reescribe T-3..T-1 (lo comprueba el modo head del ensayo).
__declspec(noinline) bool __cdecl VehHeadTailLimitCpp(const float* dot, bool train, float* sqrtDot) {
    *sqrtDot = float(FormationRootCpp(double(*dot)));
    volatile double limit = train ? kVehLightsLimitTrain : kVehLightsLimit;
    volatile double root = double(*sqrtDot);
    return root > limit;
}

// 0x6E0C5E..0x6E0C6D: magnitude < 40 (fld; fcomp 40; ordenado). T-1 = magnitude.
__declspec(noinline) bool __cdecl VehHeadTailNearCpp(const float* magnitude) {
    return double(*magnitude) < kVehLightsForty;
}

// 0x6E0CC0..0x6E0CC8: rango de la corona = float(lod * 150) (fld; fmul; fstp dword). T-1 = lod * 150.
__declspec(noinline) void __cdecl VehHeadTailRangeStoreCpp(const float* lodMultiplier, float* range) {
    *range = float(double(*lodMultiplier) * kVehHeadTail150);
}

// _ftol2 del EXE (0x821B40) sobre v (hasta 24 bits de mantisa): devuelve la palabra baja (truncada) y los dos valores que el EXE deja en los registros muertos justo debajo del
// argumento: R (fistp con el redondeo vigente, rama de cero incluida) en el mas profundo y v - R (v en la rama de cero) en el otro. Las banderas (fst dword de v, fistp de R) se
// levantan ejecutando esas conversiones (la de entero la hace la CRT: fisttp/fistp con las mismas condiciones de PE e IE para valores de 24 bits).
static __declspec(noinline) unsigned int VehHeadTailFtol2Cpp(double v, double* rounded, double* tail) {
    volatile double source = v;
    volatile float asFloat = float(source);          // fst dword [esp+18h]
    volatile long long truncated = static_cast<long long>(source); // fistp qword [esp+10h]
    (void)asFloat;
    const long long    r    = X87FistpQword(v, _control87(0, 0) & _MCW_RC);
    const unsigned int low  = unsigned(static_cast<unsigned long long>(r));
    const unsigned int high = unsigned(static_cast<unsigned long long>(r) >> 32);
    *rounded = double(r);
    if (low == 0 && (high & 0x7FFFFFFFu) == 0) { // 0x821B9F: dos fstp dword (R y v)
        *tail = v;
    } else {
        volatile double difference = v - double(r);
        *tail = difference;
    }
    return unsigned(static_cast<unsigned long long>(truncated));
}

// 0x6E0D09..0x6E0DAB (y la copia de 0x6E21A7..0x6E22A0): a = sqrtDot * 0.5 + 0.3; size = float(((1 - magnitude / 150) * 0.4) * sqrtDot); tren (no tranvia): a = min(a + a, 1.0) (fcomp
// con 1.0 como ST0) y size = float(size * 4); red = _ftol2(190|210 * a) y blue = _ftol2(a * 255|195) (EAX completo; el llamador pasa el byte bajo); range = float(lod * 150) (ultima
// instruccion x87). Registros muertos: T-3 = R de la primera _ftol2, T-2 = R de la segunda, T-1 = el rango.
__declspec(noinline) void __cdecl VehHeadTailHeadCoronaCpp(const float* magnitude, const float* sqrtDot, const float* lodMultiplier, bool train, bool halogen, bool inlinedCopy,
                                                           float* size, float* range, unsigned int* red, unsigned int* blue) {
    volatile double a = double(*sqrtDot) * kVehHeadTailHalf;
    a = a + kVehHeadTailThreeTenths;
    volatile double s = double(*magnitude) * kVehHeadTailInv150;
    s = kVehHeadTailOne - s;
    if (inlinedCopy) {   // 0x6E21D0: fmul dword [sqrtDot] antes de fmul [0.4]
        s = JpFixMemOp(kJpFixMul, s, sqrtDot);
        s = s * kVehHeadTailTwoFifths;
    } else {             // 0x6E0D32: fmul [0.4] antes de fmul dword [sqrtDot]
        s = s * kVehHeadTailTwoFifths;
        s = JpFixMemOp(kJpFixMul, s, sqrtDot);
    }
    volatile double sized = s;
    *size = float(sized);
    if (train) {
        volatile double twice = a + a;
        volatile double clamped = (kVehLightsOneV < twice) ? double(kVehLightsOneV) : double(twice);
        a = clamped;
        volatile double enlarged = double(*size) * 4.0;
        *size = float(enlarged);
    }
    double rounded1, tail1, rounded2, tail2;
    volatile double first = (halogen ? kVehLights190 : kVehLights210) * a;
    *red = VehHeadTailFtol2Cpp(first, &rounded1, &tail1);
    volatile double second = a * (halogen ? kVehLights255 : kVehLights195);
    *blue = VehHeadTailFtol2Cpp(second, &rounded2, &tail2);
    PointLightsPlant3(rounded1);
    PointLightsPlant2(rounded2);
    *range = float(double(*lodMultiplier) * kVehHeadTail150);
}
#pragma float_control(pop)

// =====================================================================================================================================================
// g23b: CVehicle::FlyingControl (0x6D85F0): maquina x87 de software. Anadir AL FINAL de X87Math.cpp (usa X87PlantStmt1..5Cpp, kPlantOne, X87_PLANT_ZERO e ExeCompatible* ya definidos arriba).
// =====================================================================================================================================================
// g23b: maquina x87 de software. Ver flc_soft.h.

// ------------------------------------------------------------------ enteros de 128 bits (hi:lo)
struct FlcU128 { unsigned long long hi, lo; };

static inline int FlcClz64(unsigned long long v) {
    int n = 0;
    if (v == 0) return 64;
    if (!(v >> 32)) { n += 32; v <<= 32; }
    if (!(v >> 48)) { n += 16; v <<= 16; }
    if (!(v >> 56)) { n += 8; v <<= 8; }
    if (!(v >> 60)) { n += 4; v <<= 4; }
    if (!(v >> 62)) { n += 2; v <<= 2; }
    if (!(v >> 63)) { n += 1; }
    return n;
}
static inline FlcU128 FlcMul64(unsigned long long a, unsigned long long b) {
    const unsigned long long a0 = a & 0xFFFFFFFFull, a1 = a >> 32, b0 = b & 0xFFFFFFFFull, b1 = b >> 32;
    const unsigned long long p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
    const unsigned long long mid = (p00 >> 32) + (p01 & 0xFFFFFFFFull) + (p10 & 0xFFFFFFFFull);
    FlcU128 r;
    r.lo = (p00 & 0xFFFFFFFFull) | (mid << 32);
    r.hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
    return r;
}
static inline FlcU128 FlcShl(FlcU128 v, int n) {   // 0 <= n < 128
    if (n == 0) return v;
    if (n >= 64) { v.hi = v.lo << (n - 64); v.lo = 0; return v; }
    v.hi = (v.hi << n) | (v.lo >> (64 - n));
    v.lo <<= n;
    return v;
}
// desplaza a la derecha con "jam": el bit menos significativo recoge todo lo perdido
static inline FlcU128 FlcShrJam(FlcU128 v, int n) {
    if (n <= 0) return v;
    if (n >= 128) { FlcU128 r{0, (v.hi | v.lo) != 0 ? 1ull : 0ull}; return r; }
    FlcU128 r;
    bool lost;
    if (n >= 64) {
        const int s = n - 64;
        lost = v.lo != 0 || (s > 0 && (v.hi & ((1ull << s) - 1)) != 0);
        r.lo = s == 0 ? v.hi : (v.hi >> s);
        r.hi = 0;
    } else {
        lost = (v.lo & ((1ull << n) - 1)) != 0;
        r.lo = (v.lo >> n) | (v.hi << (64 - n));
        r.hi = v.hi >> n;
    }
    if (lost) r.lo |= 1;
    return r;
}
static inline FlcU128 FlcAdd128(FlcU128 a, FlcU128 b, bool* carry) {
    FlcU128 r;
    r.lo = a.lo + b.lo;
    const unsigned long long c0 = r.lo < a.lo ? 1 : 0;
    const unsigned long long t = a.hi + b.hi;
    const bool c1 = t < a.hi;
    r.hi = t + c0;
    *carry = c1 || r.hi < t;
    return r;
}
static inline FlcU128 FlcSub128(FlcU128 a, FlcU128 b) {   // a >= b
    FlcU128 r;
    r.lo = a.lo - b.lo;
    r.hi = a.hi - b.hi - (a.lo < b.lo ? 1 : 0);
    return r;
}
static inline bool FlcGe128(FlcU128 a, FlcU128 b) { return a.hi > b.hi || (a.hi == b.hi && a.lo >= b.lo); }

// ------------------------------------------------------------------ construccion de valores
static inline FlcExt FlcMake(int kind, int neg, unsigned long long sig, int exp) { FlcExt r; r.sig = sig; r.exp = exp; r.kind = (unsigned char)kind; r.neg = (unsigned char)neg; return r; }
static inline FlcExt FlcIndefinite() { return FlcMake(FLC_NAN, 1, 0xC000000000000000ull, 0); }

FlcExt FlcFromFloat(unsigned int bits, unsigned int* flags) {
    const int neg = (int)(bits >> 31);
    const unsigned int e = (bits >> 23) & 0xFF, m = bits & 0x7FFFFF;
    if (e == 0xFF) {
        if (m == 0) return FlcMake(FLC_INF, neg, 1ull << 63, 0);
        if (flags && !(m & 0x400000)) *flags |= FLC_IE;                       // senalizante: IE y sale silenciado
        return FlcMake(FLC_NAN, neg, (1ull << 63) | (((unsigned long long)m | 0x400000ull) << 40), 0);
    }
    if (e == 0) {
        if (m == 0) return FlcMake(FLC_ZERO, neg, 0, 0);
        if (flags) *flags |= FLC_DE;                                          // desnormal: DE y se normaliza
        const int lz = FlcClz64((unsigned long long)m);
        return FlcMake(FLC_NORMAL, neg, (unsigned long long)m << lz, (63 - lz) - 149);
    }
    return FlcMake(FLC_NORMAL, neg, (1ull << 63) | ((unsigned long long)m << 40), (int)e - 127);
}
FlcExt FlcFromDouble(unsigned long long bits, unsigned int* flags) {
    const int neg = (int)(bits >> 63);
    const unsigned int e = (unsigned int)((bits >> 52) & 0x7FF);
    const unsigned long long m = bits & 0xFFFFFFFFFFFFFull;
    if (e == 0x7FF) {
        if (m == 0) return FlcMake(FLC_INF, neg, 1ull << 63, 0);
        if (flags && !(m & 0x8000000000000ull)) *flags |= FLC_IE;
        return FlcMake(FLC_NAN, neg, (1ull << 63) | ((m | 0x8000000000000ull) << 11), 0);
    }
    if (e == 0) {
        if (m == 0) return FlcMake(FLC_ZERO, neg, 0, 0);
        if (flags) *flags |= FLC_DE;
        const int lz = FlcClz64(m);
        return FlcMake(FLC_NORMAL, neg, m << lz, (63 - lz) - 1074);
    }
    return FlcMake(FLC_NORMAL, neg, (1ull << 63) | (m << 11), (int)e - 1023);
}
FlcExt FlcFromInt(int value) {
    if (value == 0) return FlcMake(FLC_ZERO, 0, 0, 0);
    const int neg = value < 0;
    const unsigned long long mag = neg ? (unsigned long long)(-(long long)value) : (unsigned long long)value;
    const int lz = FlcClz64(mag);
    return FlcMake(FLC_NORMAL, neg, mag << lz, 63 - lz);
}
FlcExt FlcFromDoubleExact(double d) {
    unsigned long long b;
    memcpy(&b, &d, 8);
    return FlcFromDouble(b, nullptr);
}

// ------------------------------------------------------------------ redondeo a 24 bits de (hi:lo) normalizado con peso 2^exp en el bit 127
static FlcExt FlcRound24(int neg, FlcU128 v, int exp, unsigned int* state, bool* c1) {
    // v.hi tiene el bit 63 a 1. Se conservan 24 bits.
    unsigned long long keep = v.hi >> 40;
    const unsigned long long rem = v.hi & ((1ull << 40) - 1);
    const bool guard = (rem >> 39) & 1;
    const bool sticky = (rem & ((1ull << 39) - 1)) != 0 || v.lo != 0;
    const bool inexact = rem != 0 || v.lo != 0;
    bool up = false;
    if (guard && (sticky || (keep & 1))) up = true;
    if (up) {
        ++keep;
        if (keep == (1ull << 24)) { keep = 1ull << 23; ++exp; }
    }
    if (inexact) *state |= FLC_PE;
    *c1 = inexact && up;
    return FlcMake(FLC_NORMAL, neg, keep << 40, exp);
}

// NaN: reglas del x87
static FlcExt FlcPickNaN(const FlcExt& a, const FlcExt& b, unsigned int* state) {
    const bool an = a.kind == FLC_NAN, bn = b.kind == FLC_NAN;
    const bool aSig = an && !(a.sig & (1ull << 62)), bSig = bn && !(b.sig & (1ull << 62));
    if (aSig || bSig) *state |= FLC_IE;
    FlcExt r;
    if (an && bn) {
        if (aSig != bSig) r = aSig ? b : a;                  // QNaN con senalizante: gana la silenciosa
        else if (a.sig != b.sig) r = a.sig > b.sig ? a : b;  // misma clase: la de mayor mantisa
        else r = a.neg ? b : a;                              // misma mantisa y signo distinto: gana la positiva (medido en el hardware)
    } else {
        r = an ? a : b;
    }
    r.sig |= (1ull << 62);
    return r;
}

static FlcExt FlcInfOrIndefinite(int neg, bool invalid, unsigned int* state) {
    if (invalid) { *state |= FLC_IE; return FlcIndefinite(); }
    return FlcMake(FLC_INF, neg, 1ull << 63, 0);
}

// ------------------------------------------------------------------ suma
FlcExt FlcAdd(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcPickNaN(a, b, state);
    if (a.kind == FLC_INF || b.kind == FLC_INF) {
        if (a.kind == FLC_INF && b.kind == FLC_INF && a.neg != b.neg) return FlcInfOrIndefinite(0, true, state);
        return a.kind == FLC_INF ? a : b;
    }
    if (a.kind == FLC_ZERO && b.kind == FLC_ZERO) return FlcMake(FLC_ZERO, (a.neg && b.neg) ? 1 : 0, 0, 0);
    if (a.kind == FLC_ZERO || b.kind == FLC_ZERO) {                       // 0 + x: x redondeado a 24 bits
        const FlcExt& o = a.kind == FLC_ZERO ? b : a;
        return FlcRound24(o.neg, FlcU128{o.sig, 0}, o.exp, state, c1);
    }
    // |x| >= |y|
    FlcExt x = a, y = b;
    if (y.exp > x.exp || (y.exp == x.exp && y.sig > x.sig)) { x = b; y = a; }
    const int d = x.exp - y.exp;
    FlcU128 xv{x.sig, 0}, yv{y.sig, 0};
    yv = FlcShrJam(yv, d);
    FlcU128 r;
    int exp = x.exp;
    if (x.neg == y.neg) {
        bool carry;
        r = FlcAdd128(xv, yv, &carry);
        if (carry) { r = FlcShrJam(r, 1); r.hi |= 1ull << 63; ++exp; }
    } else {
        r = FlcSub128(xv, yv);
        if (r.hi == 0 && r.lo == 0) return FlcMake(FLC_ZERO, 0, 0, 0);      // x - x = +0 (redondeo al par)
        const int lz = r.hi ? FlcClz64(r.hi) : 64 + FlcClz64(r.lo);
        r = FlcShl(r, lz);
        exp -= lz;
    }
    return FlcRound24(x.neg, r, exp, state, c1);
}

// ------------------------------------------------------------------ producto
FlcExt FlcMul(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcPickNaN(a, b, state);
    const int neg = a.neg ^ b.neg;
    if (a.kind == FLC_INF || b.kind == FLC_INF) {
        if (a.kind == FLC_ZERO || b.kind == FLC_ZERO) return FlcInfOrIndefinite(0, true, state);
        return FlcMake(FLC_INF, neg, 1ull << 63, 0);
    }
    if (a.kind == FLC_ZERO || b.kind == FLC_ZERO) return FlcMake(FLC_ZERO, neg, 0, 0);
    FlcU128 p = FlcMul64(a.sig, b.sig);
    int exp = a.exp + b.exp;
    if (p.hi >> 63) ++exp; else p = FlcShl(p, 1);
    return FlcRound24(neg, p, exp, state, c1);
}

// ------------------------------------------------------------------ division
FlcExt FlcDiv(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcPickNaN(a, b, state);
    const int neg = a.neg ^ b.neg;
    if (a.kind == FLC_INF) {
        if (b.kind == FLC_INF) return FlcInfOrIndefinite(0, true, state);
        return FlcMake(FLC_INF, neg, 1ull << 63, 0);
    }
    if (b.kind == FLC_INF) return FlcMake(FLC_ZERO, neg, 0, 0);
    if (b.kind == FLC_ZERO) {
        if (a.kind == FLC_ZERO) return FlcInfOrIndefinite(0, true, state);
        *state |= FLC_ZE;
        return FlcMake(FLC_INF, neg, 1ull << 63, 0);
    }
    if (a.kind == FLC_ZERO) return FlcMake(FLC_ZERO, neg, 0, 0);
    // q = floor(a.sig * 2^64 / b.sig): division larga bit a bit (resto de hasta 65 bits)
    FlcU128 q{0, 0};
    unsigned long long r = 0;
    bool rc = false;
    for (int i = 127; i >= 0; --i) {
        const unsigned long long bit = i >= 64 ? ((a.sig >> (i - 64)) & 1) : 0;
        const bool newc = (r >> 63) & 1;
        r = (r << 1) | bit;
        rc = newc;
        q = FlcShl(q, 1);
        if (rc || r >= b.sig) { r -= b.sig; rc = false; q.lo |= 1; }
    }
    const bool remainder = r != 0;
    const int topbit = q.hi ? 64 + (63 - FlcClz64(q.hi)) : (63 - FlcClz64(q.lo));
    q = FlcShl(q, 127 - topbit);
    if (remainder) q.lo |= 1;
    return FlcRound24(neg, q, topbit - 64 + (a.exp - b.exp), state, c1);
}

// ------------------------------------------------------------------ raiz cuadrada
FlcExt FlcSqrt(const FlcExt& a, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN) return FlcPickNaN(a, a, state);
    if (a.kind == FLC_ZERO) return a;
    if (a.neg) return FlcInfOrIndefinite(0, true, state);
    if (a.kind == FLC_INF) return a;
    const int e = a.exp;
    // radicando N de 128 bits: sig << 63 si el exponente es par, sig << 64 si es impar; la raiz tiene siempre el bit 63 a 1 y el exponente es e >> 1
    FlcU128 n = (e & 1) ? FlcU128{a.sig, 0} : FlcU128{a.sig >> 1, a.sig << 63};
    FlcU128 res{0, 0}, bit{1ull << 62, 0};
    while (bit.hi || bit.lo) {
        bool c;
        const FlcU128 trial = FlcAdd128(res, bit, &c);
        const FlcU128 half{res.hi >> 1, (res.lo >> 1) | (res.hi << 63)};
        if (FlcGe128(n, trial)) { n = FlcSub128(n, trial); res = FlcAdd128(half, bit, &c); }
        else res = half;
        bit = FlcU128{bit.hi >> 2, (bit.lo >> 2) | (bit.hi << 62)};
    }
    const bool inexact = n.hi != 0 || n.lo != 0;
    return FlcRound24(0, FlcU128{res.lo, inexact ? 1ull : 0ull}, e >> 1, state, c1);
}

// ------------------------------------------------------------------ conversiones de salida
unsigned int FlcToFloat(const FlcExt& v, unsigned int* flags, bool* roundedUp) {
    *roundedUp = false;
    const unsigned int s = (unsigned int)v.neg << 31;
    switch (v.kind) {
    case FLC_ZERO: return s;
    case FLC_INF:  return s | 0x7F800000u;
    case FLC_NAN:  return s | 0x7F800000u | (unsigned int)((v.sig >> 40) & 0x7FFFFFu);
    }
    int e = v.exp;
    const unsigned long long sig = v.sig;
    if (e >= -126) {
        unsigned long long keep = sig >> 40;
        const unsigned long long rem = sig & ((1ull << 40) - 1), half = 1ull << 39;
        const bool inexact = rem != 0;
        const bool up = rem > half || (rem == half && (keep & 1));
        if (up) { ++keep; if (keep == (1ull << 24)) { keep = 1ull << 23; ++e; } }
        if (e > 127) { *flags |= FLC_OE | FLC_PE; *roundedUp = true; return s | 0x7F800000u; }
        if (inexact) { *flags |= FLC_PE; *roundedUp = up; }
        return s | ((unsigned int)(e + 127) << 23) | (unsigned int)(keep & 0x7FFFFF);
    }
    // desnormal: unidad = 2^-149; keep = parte entera de sig * 2^(e + 86)
    const int sh = -e - 86;                                            // >= 41
    unsigned long long keep, rem, half;
    if (sh >= 65) { keep = 0; rem = 1; half = 2; }                     // por debajo de 2^-151: redondea a cero (inexacto)
    else if (sh == 64) { keep = 0; rem = sig; half = 1ull << 63; }
    else { keep = sig >> sh; rem = sig & ((1ull << sh) - 1); half = 1ull << (sh - 1); }
    const bool inexact = rem != 0;
    const bool up = rem > half || (rem == half && (keep & 1));
    if (up) ++keep;
    if (inexact) { *flags |= FLC_UE | FLC_PE; *roundedUp = up; }
    return s | (unsigned int)keep;
}

unsigned long long FlcToDoubleFlags(const FlcExt& v, unsigned int* flags, bool* roundedUp) {
    *roundedUp = false;
    const unsigned long long s = (unsigned long long)v.neg << 63;
    switch (v.kind) {
    case FLC_ZERO: return s;
    case FLC_INF:  return s | 0x7FF0000000000000ull;
    case FLC_NAN:  return s | 0x7FF0000000000000ull | ((v.sig >> 11) & 0xFFFFFFFFFFFFFull);
    }
    int e = v.exp;
    unsigned long long keep, rem, half;
    if (e >= -1022) {
        keep = v.sig >> 11; rem = v.sig & 0x7FF; half = 0x400;
    } else {                                                           // desnormal en double: unidad = 2^-1074
        const int sh = -e - 1022 + 11;                                 // bits que se descartan (> 11)
        if (sh >= 65) { keep = 0; rem = 1; half = 2; }
        else if (sh == 64) { keep = 0; rem = v.sig; half = 1ull << 63; }
        else { keep = v.sig >> sh; rem = v.sig & ((1ull << sh) - 1); half = 1ull << (sh - 1); }
    }
    const bool inexact = rem != 0;
    const bool up = rem > half || (rem == half && (keep & 1));
    if (up) {
        ++keep;
        if (e >= -1022 && keep == (1ull << 53)) { keep = 1ull << 52; ++e; }
    }
    if (inexact) { *flags |= FLC_PE | (e < -1022 ? FLC_UE : 0); *roundedUp = up; }
    if (e > 1023) { *flags |= FLC_OE | FLC_PE; *roundedUp = true; return s | 0x7FF0000000000000ull; }
    if (e < -1022) return s | keep;                                    // desnormal (keep ya es la mantisa)
    return s | ((unsigned long long)(e + 1023) << 52) | (keep & 0xFFFFFFFFFFFFFull);
}
unsigned long long FlcToDouble(const FlcExt& v) {
    unsigned int f = 0;
    bool up;
    return FlcToDoubleFlags(v, &f, &up);
}
double FlcToDoubleValue(const FlcExt& v) {
    const unsigned long long b = FlcToDouble(v);
    double d;
    memcpy(&d, &b, 8);
    return d;
}

unsigned int FlcCompare(const FlcExt& a, const FlcExt& b, unsigned int* state, bool unorderedSignals) {
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) {
        const bool aSig = a.kind == FLC_NAN && !(a.sig & (1ull << 62)), bSig = b.kind == FLC_NAN && !(b.sig & (1ull << 62));
        if (unorderedSignals || aSig || bSig) *state |= FLC_IE;
        return FLC_C0 | FLC_C2 | FLC_C3;
    }
    // orden total sobre finitos e infinitos
    auto cmp = [](const FlcExt& x, const FlcExt& y) -> int {
        auto sign = [](const FlcExt& v) { return v.kind == FLC_ZERO ? 0 : (v.neg ? -1 : 1); };
        const int sx = sign(x), sy = sign(y);
        if (sx != sy) return sx < sy ? -1 : 1;
        if (sx == 0) return 0;
        int mag;                                                       // |x| frente a |y|
        if (x.kind == FLC_INF || y.kind == FLC_INF) mag = (x.kind == FLC_INF ? 1 : 0) - (y.kind == FLC_INF ? 1 : 0);
        else if (x.exp != y.exp) mag = x.exp > y.exp ? 1 : -1;
        else mag = x.sig == y.sig ? 0 : (x.sig > y.sig ? 1 : -1);
        return sx > 0 ? mag : -mag;
    };
    const int c = cmp(a, b);
    return c < 0 ? FLC_C0 : (c == 0 ? FLC_C3 : 0);
}
bool FlcSameBits(const FlcExt& a, const FlcExt& b) {
    if (a.kind != b.kind || a.neg != b.neg) return false;
    if (a.kind == FLC_ZERO || a.kind == FLC_INF) return true;
    return a.sig == b.sig && (a.kind == FLC_NAN || a.exp == b.exp);
}

// ================================================================== la maquina
FlcX87::FlcX87() : depth(0), maxDepth(0), state(0), cc(0), ccValid(false), c1(false), lastClass(0), pendingDE(0) {
    memset(reg, 0, sizeof(reg));
    memset(written, 0, sizeof(written));
}
void FlcX87::Push(const FlcExt& v) {
    reg[depth] = v;
    written[depth] = true;
    ++depth;
    if (depth > maxDepth) maxDepth = depth;
    c1 = false;
}
void FlcX87::Write(int i, const FlcExt& v) {
    reg[depth - 1 - i] = v;
    written[depth - 1 - i] = true;
}
static FlcExt FlcMemFloat(unsigned int bits, unsigned int* pendingDE) {
    unsigned int f = 0;
    FlcExt e = FlcFromFloat(bits, &f);
    *pendingDE |= f & FLC_DE;
    if (e.kind == FLC_NAN && !(bits & 0x400000u)) e.sig &= ~(1ull << 62);   // el operando de memoria senalizante conserva su clase hasta resolver el NaN
    return e;
}
static FlcExt FlcMemDouble(unsigned long long bits, unsigned int* pendingDE) {
    unsigned int f = 0;
    FlcExt e = FlcFromDouble(bits, &f);
    *pendingDE |= f & FLC_DE;
    if (e.kind == FLC_NAN && !(bits & 0x8000000000000ull)) e.sig &= ~(1ull << 62);
    return e;
}
FlcExt FlcX87::MemOperand(const float& m) { unsigned int b; memcpy(&b, &m, 4); return FlcMemFloat(b, &pendingDE); }
FlcExt FlcX87::MemOperandD(const double& m) { unsigned long long b; memcpy(&b, &m, 8); return FlcMemDouble(b, &pendingDE); }

void FlcX87::Fld(const float& m) { unsigned int b, f = 0; memcpy(&b, &m, 4); const FlcExt e = FlcFromFloat(b, &f); state |= f; Push(e); lastClass = 3; }
void FlcX87::FldRet(const float& m) { unsigned int b, f = 0; memcpy(&b, &m, 4); Push(FlcFromFloat(b, &f)); lastClass = 3; }
void FlcX87::FldD(const double& m) { unsigned long long b; unsigned int f = 0; memcpy(&b, &m, 8); const FlcExt e = FlcFromDouble(b, &f); state |= f; Push(e); lastClass = 3; }
void FlcX87::FldI(int value) { Push(FlcFromInt(value)); lastClass = 3; }
void FlcX87::FldSt(int i) { const FlcExt e = S(i); Push(e); lastClass = 3; }
void FlcX87::FldZero() { Push(FlcMake(FLC_ZERO, 0, 0, 0)); lastClass = 3; }
void FlcX87::FldExt(const FlcExt& v) { Push(v); lastClass = 3; }
void FlcX87::Fst(float& m) {
    bool up;
    const unsigned int b = FlcToFloat(Top(), &state, &up);
    memcpy(&m, &b, 4);
    c1 = up;
    lastClass = 3;
}
void FlcX87::Fstp(float& m) {
    bool up;
    const unsigned int b = FlcToFloat(Top(), &state, &up);
    memcpy(&m, &b, 4);
    c1 = up;
    --depth;
    lastClass = 2;
}
void FlcX87::FstpSt(int i) {
    if (i != 0) Write(i, Top());
    --depth;
    c1 = false;
    lastClass = 3;
}
void FlcX87::FstSt(int i) { if (i != 0) Write(i, Top()); c1 = false; lastClass = 3; }
void FlcX87::Pop() { --depth; c1 = false; lastClass = 3; }
void FlcX87::Fxch(int i) { const FlcExt t = Top(); Write(0, S(i)); Write(i, t); c1 = false; lastClass = 3; }
void FlcX87::Fchs() { FlcExt v = Top(); v.neg ^= 1; Write(0, v); c1 = false; lastClass = 3; }
void FlcX87::Fabs() { FlcExt v = Top(); v.neg = 0; Write(0, v); c1 = false; lastClass = 3; }

static FlcExt FlcSub(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcAdd(a, b, state, c1);   // el NaN sale sin cambiar de signo
    FlcExt nb = b;
    nb.neg ^= 1;
    return FlcAdd(a, nb, state, c1);
}
void FlcX87::ArithReg(char op, FlcExt a, FlcExt b, int dest, bool pop, bool) {
    bool up = false;
    const unsigned int de = (a.kind == FLC_NAN || b.kind == FLC_NAN) ? 0 : pendingDE;
    pendingDE = 0;
    unsigned int local = 0;
    FlcExt r;
    switch (op) {
    case '+': r = FlcAdd(a, b, &local, &up); break;
    case '-': r = FlcSub(a, b, &local, &up); break;
    case '*': r = FlcMul(a, b, &local, &up); break;
    default:  r = FlcDiv(a, b, &local, &up); break;
    }
    if (!(local & (FLC_IE | FLC_ZE))) local |= de;                         // IE y ZE tienen prioridad sobre DE
    state |= local;
    Write(dest, r);
    c1 = up;
    if (pop) --depth;
    lastClass = 3;
}
void FlcX87::Arith(char op, int dest, int srcA, int srcB, bool pop) { ArithReg(op, S(srcA), S(srcB), dest, pop, false); }
// ST0 = ST0 op m
void FlcX87::Fadd(const float& m)  { ArithReg('+', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fsub(const float& m)  { ArithReg('-', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fsubr(const float& m) { ArithReg('-', MemOperand(m), Top(), 0, false, true); }
void FlcX87::Fmul(const float& m)  { ArithReg('*', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fdiv(const float& m)  { ArithReg('/', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fdivr(const float& m) { ArithReg('/', MemOperand(m), Top(), 0, false, true); }
void FlcX87::FaddD(const double& m)  { ArithReg('+', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FsubD(const double& m)  { ArithReg('-', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FsubrD(const double& m) { ArithReg('-', MemOperandD(m), Top(), 0, false, true); }
void FlcX87::FmulD(const double& m)  { ArithReg('*', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FdivD(const double& m)  { ArithReg('/', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FdivrD(const double& m) { ArithReg('/', MemOperandD(m), Top(), 0, false, true); }
void FlcX87::FimulI(int v) { ArithReg('*', Top(), FlcFromInt(v), 0, false, true); }
void FlcX87::FidivI(int v) { ArithReg('/', Top(), FlcFromInt(v), 0, false, true); }
void FlcX87::FiaddI(int v) { ArithReg('+', Top(), FlcFromInt(v), 0, false, true); }

void FlcX87::FcomMem(const float& m, bool pop) {
    const FlcExt b = MemOperand(m);
    if (!(Top().kind == FLC_NAN || b.kind == FLC_NAN)) state |= pendingDE;
    pendingDE = 0;
    cc = FlcCompare(Top(), b, &state, true);
    ccValid = true;
    c1 = false;
    lastClass = pop ? 1 : 3;
    if (pop) --depth;
}
void FlcX87::FcomSt(int i, bool pop) {
    cc = FlcCompare(Top(), S(i), &state, true);
    ccValid = true;
    c1 = false;
    lastClass = 3;
    if (pop) --depth;
}
void FlcX87::Fcompp() {
    cc = FlcCompare(Top(), S(1), &state, true);
    ccValid = true;
    c1 = false;
    lastClass = 3;
    depth -= 2;
}
void FlcX87::Fsqrt() {
    bool up;
    const FlcExt r = FlcSqrt(Top(), &state, &up);
    Write(0, r);
    c1 = up;
    lastClass = 3;
}
unsigned int FlcX87::Ah() const { return (cc | (c1 ? FLC_C1 : 0)) >> 8; }
float FlcX87::StFloat(int i) {
    bool up;
    unsigned int f = 0;
    const unsigned int b = FlcToFloat(St(i), &f, &up);
    float r;
    memcpy(&r, &b, 4);
    return r;
}

FlcExt FlcFromInt64(long long value) {
    if (value == 0) return FlcMake(FLC_ZERO, 0, 0, 0);
    const int neg = value < 0;
    const unsigned long long mag = neg ? (0ull - (unsigned long long)value) : (unsigned long long)value;
    const int lz = FlcClz64(mag);
    return FlcMake(FLC_NORMAL, neg, mag << lz, 63 - lz);
}
void FlcX87::FldOne() { Push(FlcMake(FLC_NORMAL, 0, 1ull << 63, 0)); lastClass = 3; }
void FlcX87::FldI64(long long v) { Push(FlcFromInt64(v)); lastClass = 3; }
void FlcX87::FstD(double& m) {
    bool up;
    const unsigned long long out = FlcToDoubleFlags(Top(), &state, &up);
    memcpy(&m, &out, 8);
    c1 = up;
    lastClass = 3;
}
void FlcX87::FstpD(double& m) { FstD(m); --depth; }
void FlcX87::Fistp64(long long& m) {
    const FlcExt v = Top();
    unsigned long long out = 0x8000000000000000ull;
    bool inexact = false, up = false, invalid = false;
    if (v.kind == FLC_NAN || v.kind == FLC_INF) invalid = true;
    else if (v.kind == FLC_ZERO) out = 0;
    else {
        if (v.exp >= 63) {
            // |v| >= 2^63: solo -2^63 exacto es valido
            if (v.neg && v.exp == 63 && v.sig == (1ull << 63)) out = 0x8000000000000000ull; else invalid = true;
        } else if (v.exp < -1) {
            // |v| < 0.5: redondea a cero (inexacto)
            out = 0; inexact = true; up = false;
        } else {
            const int sh = 63 - v.exp;                    // bits fraccionarios en sig (>= 1)
            unsigned long long ip, rem, half;
            if (sh >= 64) { ip = 0; rem = v.sig; half = 1ull << 63; }
            else { ip = v.sig >> sh; rem = v.sig & ((1ull << sh) - 1); half = 1ull << (sh - 1); }
            inexact = rem != 0;
            up = rem > half || (rem == half && (ip & 1));
            if (up) ++ip;
            out = v.neg ? (0ull - ip) : ip;
        }
    }
    if (invalid) state |= FLC_IE;
    else if (inexact) state |= FLC_PE;
    m = (long long)out;
    c1 = !invalid && inexact && up;
    --depth;
    lastClass = 3;
}

// ---- fsin / fcos / fpatan: el resultado lo calcula el x87 real (53 bits de mantisa aqui)
static bool FlcOutOfTrigRange(const FlcExt& v) { return v.kind == FLC_NORMAL && v.exp >= 63; }
void FlcX87::Fsin() {
    const FlcExt v = Top();
    FlcExt r = v;
    if (v.kind == FLC_NAN) { r.sig |= (1ull << 62); }
    else if (v.kind == FLC_INF) { state |= FLC_IE; r = FlcMake(FLC_NAN, 1, 0xC000000000000000ull, 0); }
    else if (v.kind == FLC_ZERO) {}
    else if (FlcOutOfTrigRange(v)) { cc |= FLC_C2; }
    else { r = FlcFromDoubleExact(FlcHwSin(FlcToDoubleValue(v))); state |= FLC_PE; cc &= ~FLC_C2; }
    Write(0, r);
    c1 = false;
    lastClass = 3;
}
void FlcX87::Fcos() {
    const FlcExt v = Top();
    FlcExt r = v;
    if (v.kind == FLC_NAN) { r.sig |= (1ull << 62); }
    else if (v.kind == FLC_INF) { state |= FLC_IE; r = FlcMake(FLC_NAN, 1, 0xC000000000000000ull, 0); }
    else if (v.kind == FLC_ZERO) { r = FlcMake(FLC_NORMAL, 0, 1ull << 63, 0); }
    else if (FlcOutOfTrigRange(v)) { cc |= FLC_C2; }
    else { r = FlcFromDoubleExact(FlcHwCos(FlcToDoubleValue(v))); state |= FLC_PE; cc &= ~FLC_C2; }
    Write(0, r);
    c1 = false;
    lastClass = 3;
}
void FlcX87::Fpatan() {
    const FlcExt x = S(0), y = S(1);                         // atan2(ST1, ST0)
    FlcExt r;
    if (x.kind == FLC_NAN || y.kind == FLC_NAN) r = FlcPickNaN(y, x, &state);
    else {
        r = FlcFromDoubleExact(FlcHwAtan2(FlcToDoubleValue(y), FlcToDoubleValue(x)));
        if (r.kind != FLC_ZERO) state |= FLC_PE;
    }
    Write(1, r);
    --depth;
    c1 = false;
    lastClass = 3;
}

// g23b: funciones auxiliares pequenas del EXE como secuencias x87 de la maquina FlcX87 (ver flc_macros.h).

static void FlcSetExeErrno(int value) {
    using ExeErrnoFn = int*(__cdecl*)();
    *reinterpret_cast<ExeErrnoFn>(0x0082B181)() = value;   // _errno() del EXE
}
FlcWorld FlcReadWorld() {
    FlcWorld w;
    w.sse2Pow = *reinterpret_cast<const int*>(0x00C9C400);
    w.matherrFlag = *reinterpret_cast<const int*>(0x00C9ABFC);
    return w;
}

// ======================================================================================================================== CVector
void FlcVecAdd(FlcX87& fp, float* out, const float* a, const float* b) {
    fp.Fld(a[2]); fp.Fadd(b[2]);
    fp.Fld(a[1]); fp.Fadd(b[1]);
    fp.Fld(a[0]); fp.Fadd(b[0]);
    fp.Fstp(out[0]); fp.Fstp(out[1]); fp.Fstp(out[2]);
}
void FlcVecSub(FlcX87& fp, float* out, const float* a, const float* b) {
    fp.Fld(a[2]); fp.Fsub(b[2]);
    fp.Fld(a[1]); fp.Fsub(b[1]);
    fp.Fld(a[0]); fp.Fsub(b[0]);
    fp.Fstp(out[0]); fp.Fstp(out[1]); fp.Fstp(out[2]);
}
void FlcVecMulLeft(FlcX87& fp, float* out, const float& f, const float* v) {
    fp.Fld(f); fp.Fmul(v[2]);
    fp.Fld(f); fp.Fmul(v[1]);
    fp.Fld(f); fp.Fmul(v[0]);
    fp.Fstp(out[0]); fp.Fstp(out[1]); fp.Fstp(out[2]);
}
void FlcVecMulRight(FlcX87& fp, float* out, const float* v, const float& f) { FlcVecMulLeft(fp, out, f, v); }   // 0x40FEC0: misma secuencia (f * v.z, f * v.y, f * v.x)
void FlcVecScale(FlcX87& fp, float* v, const float& f) {
    fp.Fld(f); fp.Fmul(v[0]); fp.Fstp(v[0]);
    fp.Fld(f); fp.Fmul(v[1]); fp.Fstp(v[1]);
    fp.Fld(f); fp.Fmul(v[2]); fp.Fstp(v[2]);
}
void FlcVecAddTo(FlcX87& fp, float* v, const float* a) {
    fp.Fld(a[0]); fp.Fadd(v[0]); fp.Fstp(v[0]);
    fp.Fld(a[1]); fp.Fadd(v[1]); fp.Fstp(v[1]);
    fp.Fld(a[2]); fp.Fadd(v[2]); fp.Fstp(v[2]);
}
static void FlcSumSquaresLeaveSt0(FlcX87& fp, const float* v) {      // 0x4082C0 / 0x406DA0 hasta el segundo faddp: ST0 = (xx + yy) + zz ; ST1 = x ; ST2 = y ; ST3 = z
    fp.Fld(v[2]); fp.Fld(v[1]); fp.Fld(v[0]);
    fp.FldSt(0); fp.Arith('*', 0, 0, 1, false);
    fp.FldSt(2); fp.Arith('*', 0, 0, 3, false);
    fp.Arith('+', 1, 1, 0, true);
    fp.FldSt(3); fp.Arith('*', 0, 0, 4, false);
    fp.Arith('+', 1, 1, 0, true);
}
void FlcVecMagnitude(FlcX87& fp, const float* v) {
    FlcSumSquaresLeaveSt0(fp, v);
    fp.Fsqrt();
    fp.FstpSt(3); fp.Pop(); fp.Pop();
}
void FlcVecMagnitudeSquared(FlcX87& fp, const float* v) {
    FlcSumSquaresLeaveSt0(fp, v);
    fp.FstpSt(3); fp.Pop(); fp.Pop();
}
void FlcVecDot(FlcX87& fp, const float* a, const float* b) {
    fp.Fld(a[2]); fp.Fmul(b[2]);
    fp.Fld(a[1]); fp.Fmul(b[1]);
    fp.Arith('+', 1, 1, 0, true);
    fp.Fld(a[0]); fp.Fmul(b[0]);
    fp.Arith('+', 1, 1, 0, true);
}
static const float kFlcMacZero = 0.0f;
static const float kFlcOneF = 1.0f;
void FlcVecNormalise(FlcX87& fp, float* v) {
    FlcSumSquaresLeaveSt0(fp, v);                                    // ST0 = suma de cuadrados
    fp.FstpSt(3); fp.Pop(); fp.Pop();
    fp.FcomMem(kFlcMacZero, false);                                     // fcom (sin sacar)
    const unsigned ah = fp.Ah();
    const bool parityEven = (((ah & 0x41) == 0) || ((ah & 0x41) == 0x41));
    if (!parityEven) {                                               // suma <= 0
        fp.Pop();
        unsigned int one = 0x3F800000u;
        memcpy(&v[0], &one, 4);
        return;
    }
    fp.Fsqrt();
    fp.Fdivr(kFlcOneF);
    fp.FldSt(0); fp.Fmul(v[0]); fp.Fstp(v[0]);
    fp.FldSt(0); fp.Fmul(v[1]); fp.Fstp(v[1]);
    fp.Fmul(v[2]); fp.Fstp(v[2]);
}
void FlcVecCross(FlcX87& fp, float* out, const float* a, const float* b) {
    float t[3];
    fp.Fld(b[2]); fp.Fmul(a[1]);
    fp.Fld(a[2]); fp.Fmul(b[1]);
    fp.Arith('-', 1, 1, 0, true); fp.Fstp(t[0]);
    fp.Fld(a[2]); fp.Fmul(b[0]);
    fp.Fld(b[2]); fp.Fmul(a[0]);
    fp.Arith('-', 1, 1, 0, true); fp.Fstp(t[1]);
    fp.Fld(a[0]); fp.Fmul(b[1]);
    fp.Fld(b[0]); fp.Fmul(a[1]);
    fp.Arith('-', 1, 1, 0, true); fp.Fstp(t[2]);
    memcpy(out, t, 12);
}
void FlcMatMul3x3(FlcX87& fp, float* out, const float* m, const float* v) {
    float t[3];
    fp.Fld(m[8]);  fp.Fmul(v[2]); fp.Fld(m[4]);  fp.Fmul(v[1]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[0]); fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[0]);
    fp.Fld(m[9]);  fp.Fmul(v[2]); fp.Fld(m[1]);  fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[5]); fp.Fmul(v[1]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[1]);
    fp.Fld(m[10]); fp.Fmul(v[2]); fp.Fld(m[2]);  fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[6]); fp.Fmul(v[1]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[2]);
    memcpy(out, t, 12);
}
void FlcMatInvMul3x3(FlcX87& fp, float* out, const float* m, const float* v) {
    float t[3];
    fp.Fld(m[1]); fp.Fmul(v[1]); fp.Fld(m[2]); fp.Fmul(v[2]); fp.Arith('+', 1, 1, 0, true); fp.Fld(v[0]); fp.Fmul(m[0]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[0]);
    fp.Fld(m[5]); fp.Fmul(v[1]); fp.Fld(m[4]); fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[6]); fp.Fmul(v[2]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[1]);
    fp.Fld(m[9]); fp.Fmul(v[1]); fp.Fld(m[8]); fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[10]); fp.Fmul(v[2]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[2]);
    memcpy(out, t, 12);
}
void FlcFloatMax(FlcX87& fp, const float& a, const float& b) {      // 0x420800: fld a; fcomp b; test ah,0x41; jne -> ST0 = b ; si no ST0 = a
    fp.Fld(a);
    fp.FcomMem(b, true);
    if (fp.Ah() & 0x41) fp.Fld(b); else fp.Fld(a);
}
void FlcFloatMin(FlcX87& fp, const float& a, const float& b) {      // 0x404330: fld a; fcomp b; test ah,5; jp -> ST0 = b ; si no ST0 = a
    fp.Fld(a);
    fp.FcomMem(b, true);
    const unsigned ah = fp.Ah();
    const bool parityEven = (((ah & 5) == 0) || ((ah & 5) == 5));
    if (parityEven) fp.Fld(b); else fp.Fld(a);
}

// ======================================================================================================================== CRT
static unsigned int FlcHighDword(const FlcExt& x) { return (unsigned int)(FlcToDouble(x) >> 32); }

// Cola comun de _CIasin / _CIacos tras calcular el resultado (0x821EB6 / 0x8223C8 / 0x821F1C / 0x82242C): si el CRT comprueba las excepciones (matherrFlag == 0)
// y el x87 lleva PE, 0x828EA7 guarda ST0 como qword y lo recarga (redondeo a 53 bits); con el dominio invalido ademas pone IE y el errno del EXE.
static void FlcMathTail(FlcX87& fp, bool domainError, bool nanInput) {
    const FlcWorld w = FlcReadWorld();
    if (w.matherrFlag != 0) return;
    if (!domainError && !nanInput) {
        if (!((FlcHwSticky() | fp.state) & FLC_PE)) return;
    }
    double d;
    fp.FstD(d);
    if (domainError) { fp.state |= FLC_IE; }
    if (domainError || nanInput) FlcSetExeErrno(33);
    fp.Pop();
    fp.FldD(d);
}
void FlcCrtAsin(FlcX87& fp) {
    const FlcExt x = fp.St(0);
    const unsigned int hi = FlcHighDword(x);
    const unsigned int expo = hi & 0x7FF00000u;
    if (x.kind == FLC_NAN) { FlcMathTail(fp, false, true); return; }
    if (x.kind == FLC_INF || expo > 0x3FF00000u || (expo == 0x3FF00000u && (FlcToDouble(x) & 0xFFFFFFFFFFFFFull) != 0)) {
        fp.Pop();
        fp.FldExt(FlcExt{0xC000000000000000ull, 0, FLC_NAN, 1});            // indefinido real (0x8E3130)
        FlcMathTail(fp, true, false);
        return;
    }
    if (expo == 0x3FF00000u) {                                              // +-1 exacto: +-pi/2 de 64 bits (0x8E313A)
        const int neg = x.neg;
        fp.Pop();
        fp.FldExt(FlcExt{0xC90FDAA22168C235ull, 0, FLC_NORMAL, (unsigned char)neg});
        FlcMathTail(fp, false, false);
        return;
    }
    fp.FldOne(); fp.Arith('+', 0, 0, 1, false);                             // 1 + x
    fp.FldOne(); fp.Arith('-', 0, 0, 2, false);                             // 1 - x
    fp.Arith('*', 1, 1, 0, true);
    fp.Fsqrt();
    fp.Fpatan();                                                            // atan2(x, raiz)
    FlcMathTail(fp, false, false);
}
void FlcCrtAcos(FlcX87& fp) {
    const FlcExt x = fp.St(0);
    const unsigned int hi = FlcHighDword(x);
    const unsigned int expo = hi & 0x7FF00000u;
    if (x.kind == FLC_NAN) { FlcMathTail(fp, false, true); return; }
    if (x.kind == FLC_INF || expo > 0x3FF00000u || (expo == 0x3FF00000u && (FlcToDouble(x) & 0xFFFFFFFFFFFFFull) != 0)) {
        fp.Pop();
        fp.FldExt(FlcExt{0xC000000000000000ull, 0, FLC_NAN, 1});
        FlcMathTail(fp, true, false);
        return;
    }
    if (expo == 0x3FF00000u) {                                              // +-1 exacto: 0 (fldz) o pi (fldpi)
        const int neg = x.neg;
        fp.Pop();
        if (neg) fp.FldExt(FlcExt{0xC90FDAA22168C235ull, 1, FLC_NORMAL, 0}); else fp.FldZero();
        FlcMathTail(fp, false, false);
        return;
    }
    fp.FldOne(); fp.Arith('+', 0, 0, 1, false);
    fp.FldOne(); fp.Arith('-', 0, 0, 2, false);
    fp.Arith('*', 1, 1, 0, true);
    fp.Fsqrt();
    fp.Fxch(1);
    fp.Fpatan();                                                            // atan2(raiz, x)
    FlcMathTail(fp, false, false);
}

// _CIpow con la rama SSE2 del CRT del EXE (0x829B80): fxch; fstp qword; fstp qword; call 0x829B99 (pow en SSE2, argumentos double en la pila, resultado en ST0 por fld qword); la rama SSE2 se toma si
// [0xC9C400] != 0 y las excepciones del x87 y del SSE estan enmascaradas. El nucleo SSE2 (tablas de pow, MXCSR y errno incluidos) es codigo de la CRT del EXE y se llama por su direccion:
// reproducirlo en C++ exigiria copiar sus tablas (0x8884F0..0x88A540) y dejaria las banderas de MXCSR y errno distintas en los casos limite.
void FlcCrtPow(FlcX87& fp) {
    fp.Fxch(1);
    double base, exponent;
    fp.FstpD(base);
    fp.FstpD(exponent);
    using PowCore = double(__cdecl*)(double, double);
    const double result = reinterpret_cast<PowCore>(0x00829B99)(base, exponent);
    fp.FldD(result);
}

int FlcCrtFtol2(FlcX87& fp) {
    float f18;
    long long r;
    fp.FldSt(0);
    fp.Fst(f18);
    fp.Fistp64(r);
    fp.FldI64(r);
    unsigned int edx, eax = (unsigned int)(unsigned long long)r;
    memcpy(&edx, &f18, 4);
    unsigned int hi = (unsigned int)((unsigned long long)r >> 32);
    if (eax == 0 && (hi & 0x7FFFFFFFu) == 0) {                               // 0x821B9F: R == 0 (o R = -2^63 con hi...)
        // el EXE saca R y v con dos fstp dword
        float sink;
        fp.Fstp(sink);
        fp.Fstp(sink);
        return 0;
    }
    fp.Arith('-', 1, 1, 0, true);                                            // fsubp: v - R
    float frac;
    fp.Fstp(frac);
    unsigned int fracBits;
    memcpy(&fracBits, &frac, 4);
    if ((int)edx < 0) {                                                       // v negativo: xor ecx,0x80000000; add ecx,0x7FFFFFFF; adc eax,0; adc edx,0
        const unsigned int t = fracBits ^ 0x80000000u;
        const unsigned int ecx = t + 0x7FFFFFFFu;
        const unsigned int carry = ecx < t ? 1u : 0u;
        return (int)(eax + carry);
    }
    const unsigned int ecx = fracBits + 0x7FFFFFFFu;                          // add ecx,0x7FFFFFFF; sbb eax,0; sbb edx,0
    const unsigned int carry = ecx < fracBits ? 1u : 0u;
    return (int)(eax - carry);
}

// ---- Plantas (niveles 6, 7 y 8 nuevos): Stmt(N) = ZT(N-2) + Stmt(N-1), con ZT(k) = arbol equilibrado de 2^k ceros. Se ejecutan con precision de 64 bits (Commit): `1 * d` es exacto.
#define FZ X87_PLANT_ZERO
#define FZ2 (FZ + FZ)
#define FZ4 (FZ2 + FZ2)
#define FZ8 (FZ4 + FZ4)
#define FZ16 (FZ8 + FZ8)
#define FZ32 (FZ16 + FZ16)
#define FZ64 (FZ32 + FZ32)
#pragma float_control(precise, on, push)
static __declspec(noinline) void FlcPlantStmt6(double d) { volatile double sink; sink = FZ16 + (FZ8 + (FZ4 + (FZ2 + (FZ + (kPlantOne * d))))); }
static __declspec(noinline) void FlcPlantStmt7(double d) { volatile double sink; sink = FZ32 + (FZ16 + (FZ8 + (FZ4 + (FZ2 + (FZ + (kPlantOne * d)))))); }
static __declspec(noinline) void FlcPlantStmt8(double d) { volatile double sink; sink = FZ64 + (FZ32 + (FZ16 + (FZ8 + (FZ4 + (FZ2 + (FZ + (kPlantOne * d))))))); }
#pragma float_control(pop)
static void FlcPlant(int level, double d) {
    switch (level) {
    case 1: X87PlantStmt1Cpp(d); break; case 2: X87PlantStmt2Cpp(d); break; case 3: X87PlantStmt3Cpp(d); break; case 4: X87PlantStmt4Cpp(d); break;
    case 5: X87PlantStmt5Cpp(d); break; case 6: FlcPlantStmt6(d); break; case 7: FlcPlantStmt7(d); break; default: FlcPlantStmt8(d); break;
    }
}

// ---- fsin / fcos / fpatan reales (el intrinseco de /arch:IA32 emite la instruccion x87 y devuelve ST0)
__declspec(noinline) double __cdecl FlcHwSin(double x) { return sin(x); }
__declspec(noinline) double __cdecl FlcHwCos(double x) { return cos(x); }
__declspec(noinline) double __cdecl FlcHwAtan2(double y, double x) { return atan2(y, x); }

// ---- constantes volatile
static const volatile float kFlcZero = 0.0f;
static const volatile float kFlcOne = 1.0f;
static const volatile float kFlcThree = 3.0f;
static const volatile float kFlcInf = __builtin_huge_valf();
static const volatile float kFlcDenormal = 1.0e-45f;
static const volatile double kFlcBigDouble = 1.0e300;
static const volatile double kFlcTinyDouble = 1.0e-300;
static volatile float gFlcSinkF;
static volatile double gFlcSinkD;
static volatile int gFlcSinkI;

// ---- elevan UNA bandera acumulada con un unico registro x87 (T-1); el destino es una variable global volatile que nadie lee
#pragma float_control(precise, on, push)
static __declspec(noinline) void FlcRaiseIE() { gFlcSinkF = kFlcZero * kFlcInf; }
static __declspec(noinline) void FlcRaiseDE() { gFlcSinkF = kFlcDenormal * kFlcOne; }
static __declspec(noinline) void FlcRaiseZE() { gFlcSinkF = kFlcOne / kFlcZero; }
static __declspec(noinline) void FlcRaisePE() { gFlcSinkF = kFlcOne / kFlcThree; }
static __declspec(noinline) void FlcRaiseOE() { gFlcSinkF = float(kFlcBigDouble); }
static __declspec(noinline) void FlcRaiseUE() { gFlcSinkF = float(kFlcTinyDouble); }
#pragma float_control(pop)

// ---- fld dword [a]; fcomp dword [b]  (la forma sin `precise` da esta secuencia, igual que GangDriveByGtCpp)
static __declspec(noinline) void FlcCmpFF(const volatile float* a, const volatile float* b) { gFlcSinkI = (double(*a) > double(*b)) ? 1 : 0; }
// ---- fld qword [a]; fstp dword [sink]: deja a en T-1 y C1 = redondeo de la conversion
static __declspec(noinline) void FlcStoreD(const volatile double* a) { gFlcSinkF = float(*a); }

// ---- Plantado EXACTO de un valor extendido (mantisa de 64 bits) en un registro muerto.
// El plantado de arriba solo escribe valores de 53 bits (un double entra por fld qword). El EXE deja, sin tocar, constantes de 64 bits en los registros muertos: _CIasin(+-1) y
// _CIacos(-1) cargan +-pi/2 y pi con fld tbyte (0x8E313A) y, con PE limpio, no las redondean a 53 bits (0x828EA7). Ese valor se construye en la x87 SIN ensamblador con aritmetica exacta
// a 64 bits (palabra de control PC=64, que pone Commit). sig (64 bits) = 2 * hi + bajo, con hi = sig >> 1 (63 bits) y bajo = sig & 1. hi se parte en dos doubles EXACTOS (la mitad alta,
// con sus ceros bajos, y la mitad baja de 32 bits) y se suman en ST0 sin pasar por memoria (un `fild` de un int64 de 63 bits tambien seria exacto, pero el compilador lo redondea a double al
// convertir en cuanto la expresion es `precise`):
//     valor = (((h1 + h2) + low) * ka) * kb,   h1 + h2 = hi,   low = +-bajo / 2,   ka * kb = 2^(exp - 62)
// Todas las operaciones son exactas (sumas cuyo resultado cabe en 64 bits y productos por potencias de 2): no levantan PE ni ninguna otra bandera, no tocan C0, C2 ni C3 y dejan C1 = 0.
// T-1: la funcion devuelve el double en ST0 (con /fp:fast el compilador no lo redondea a 53 bits) y el llamador lo descarta con `fstp st(0)` (llamada con resultado ignorado), lo que saca el
// registro SIN modificarlo ni almacenar nada (un fstp a memoria lo redondearia y levantaria PE).
// Registros mas profundos (T-2 ..): NO se pueden plantar exactos con este metodo. Se probo el mismo arbol de sumas de X87PlantStmt* con el valor calculado en linea o por llamadas
// (-x y +x para que la suma almacenada sea cero): el compilador (MSVC /O2 /arch:IA32 /fp:fast) evalua primero el subarbol del valor y despues los ceros, que lo sobrescriben, y no hay
// forma de fijar el orden desde C++. Quedan en 53 bits; el EXE solo deja una constante de 64 bits en T-1 (ver README de Dependencies23B).
struct FlcExtPlan { double h1; double h2; double low; double ka; double kb; };
static double FlcPow2(int e) {   // 2^e exacto como double normal (e en [-1022, 1023])
    return std::bit_cast<double>(static_cast<unsigned long long>(e + 1023) << 52);
}
static double FlcSmallInt(unsigned long long m) {   // entero de hasta 53 bits como double, sin instrucciones de coma flotante
    if (m == 0) return 0.0;
    const int lz = std::countl_zero(m);
    const unsigned long long mant = ((m << lz) << 1) >> 12;
    return std::bit_cast<double>((static_cast<unsigned long long>(63 - lz + 1023) << 52) | mant);
}
// false si el valor entra exacto por el plantado de 53 bits (o no es un normal que pueda construirse): el llamador usa entonces FlcPlant.
static bool FlcPlanExt(const FlcExt& x, FlcExtPlan* p) {
    if (x.kind != FLC_NORMAL) return false;
    if (FlcSameBits(FlcFromDouble(FlcToDouble(x), nullptr), x)) return false;   // entra exacto por 53 bits (solo enteros: ninguna instruccion x87)
    const int s1 = x.exp - 62;                           // exponente del bit menos significativo de hi
    if (s1 < -2040 || s1 > 2040) return false;           // fuera de lo que cubren dos potencias de 2 normales
    const int sa = s1 / 2;
    const int sb = s1 - sa;
    const unsigned long long hi = x.sig >> 1;
    const unsigned long long sign = x.neg ? 0x8000000000000000ull : 0ull;
    p->h1  = std::bit_cast<double>(std::bit_cast<unsigned long long>(FlcSmallInt(hi & 0xFFFFFFFF00000000ull)) | sign);
    p->h2  = std::bit_cast<double>(std::bit_cast<unsigned long long>(FlcSmallInt(hi & 0xFFFFFFFFull)) | sign);
    p->low = std::bit_cast<double>((x.sig & 1ull) ? (0x3FE0000000000000ull | sign) : sign);   // +-0,5 o +-0
    p->ka  = FlcPow2(sa);
    p->kb  = FlcPow2(sb);
    return true;
}
#pragma float_control(precise, off, push)
static volatile unsigned gFlcExtTouch;   // lectura volatil: la funcion no es pura y el compilador no puede eliminar la llamada de resultado ignorado
static __declspec(noinline) double __cdecl FlcExtValue(double h1, double h2, double low, double ka, double kb) { (void)gFlcExtTouch; return (((h1 + h2) + low) * ka) * kb; }
#pragma float_control(pop)
static __declspec(noinline) void FlcPlantExt1(const FlcExtPlan& p) { FlcExtValue(p.h1, p.h2, p.low, p.ka, p.kb); }                 // T-1 = x (fstp st(0) del resultado ignorado)
static __declspec(noinline) void FlcStoreExt(const FlcExtPlan& p) { gFlcSinkF = static_cast<float>(FlcExtValue(p.h1, p.h2, p.low, p.ka, p.kb)); }   // fld x; fstp dword: T-1 = x y C1 = redondeo
void FlcX87::Commit() {
    if (maxDepth == 0) return;                                         // el EXE no toco la x87
    // T-1 con mas de 53 bits de mantisa (constante de _CIasin/_CIacos): el plan es de solo enteros y se calcula antes de plantar nada
    FlcExtPlan topPlan;
    const bool topExact = FlcPlanExt(reg[0], &topPlan);
    // 1. banderas acumuladas
    if (state & FLC_IE) FlcRaiseIE();
    if (state & FLC_DE) FlcRaiseDE();
    if (state & FLC_ZE) FlcRaiseZE();
    if (state & FLC_PE) FlcRaisePE();
    if (state & FLC_OE) FlcRaiseOE();
    if (state & FLC_UE) FlcRaiseUE();
    // 2. registros muertos T-maxDepth .. T-2
    double vals[8];
    unsigned long long bits[8];
    for (int i = 0; i < maxDepth; ++i) { bits[i] = FlcToDouble(reg[i]); memcpy(&vals[i], &bits[i], 8); }
    unsigned int cwPrevious = 0, cwIgnored = 0;
    __control87_2(0, 0, &cwPrevious, nullptr);
    __control87_2(_PC_64, _MCW_PC, &cwIgnored, nullptr);                       // precision de 64 bits: `1 * d` es exacto con 53 bits
    for (int k = maxDepth - 1; k >= 1; --k) FlcPlant(k + 1, vals[k]);
    __control87_2(cwPrevious, _MCW_PC, &cwIgnored, nullptr);
    // 3. codigos de condicion, T-1 y C1. Los valores se escriben como enteros: ninguna carga/almacenamiento de coma flotante fuera de la secuencia buscada.
    static unsigned long long vBits;
    static unsigned int faBits, fbBits;
    vBits = bits[0];
    const unsigned int kQNaN = 0x7FC00000u, kOne = 0x3F800000u;
    if (ccValid) {                                  // fld [a]; fcomp [b] con 0 / 1 / NaN fija C0/C2/C3 (y C1 = 0)
        if (cc == (FLC_C0 | FLC_C2 | FLC_C3)) { faBits = kQNaN; fbBits = 0; }
        else if (cc == FLC_C3) { faBits = 0; fbBits = 0; }
        else if (cc == FLC_C0) { faBits = 0; fbBits = kOne; }
        else { faBits = kOne; fbBits = 0; }
        FlcCmpFF(reinterpret_cast<const volatile float*>(&faBits), reinterpret_cast<const volatile float*>(&fbBits));
    }
    if (lastClass == 2) {                                                // fld [v]; fstp dword: T-1 = v y C1 = redondeo del almacenamiento
        if (topExact) {                                                   // v con 64 bits de mantisa: construido exacto (PC = 64)
            __control87_2(_PC_64, _MCW_PC, &cwIgnored, nullptr);
            FlcStoreExt(topPlan);
            __control87_2(cwPrevious, _MCW_PC, &cwIgnored, nullptr);
        } else FlcStoreD(reinterpret_cast<const volatile double*>(&vBits));
    }
    else {
        __control87_2(_PC_64, _MCW_PC, &cwIgnored, nullptr);
        if (topExact) FlcPlantExt1(topPlan);                                 // T-1 = v exacto (64 bits), C1 = 0
        else FlcPlant(1, vals[0]);                                           // T-1 = v, C1 = 0
        __control87_2(cwPrevious, _MCW_PC, &cwIgnored, nullptr);
    }
}

// Banderas acumuladas del x87 real (formato FLC_*), sin modificarlas.
unsigned int FlcHwSticky() {
    unsigned int x87 = 0, sse = 0;
    _statusfp2(&x87, &sse);
    unsigned int r = 0;
    if (x87 & _SW_INVALID) r |= FLC_IE;
    if (x87 & _SW_DENORMAL) r |= FLC_DE;
    if (x87 & _SW_ZERODIVIDE) r |= FLC_ZE;
    if (x87 & _SW_OVERFLOW) r |= FLC_OE;
    if (x87 & _SW_UNDERFLOW) r |= FLC_UE;
    if (x87 & _SW_INEXACT) r |= FLC_PE;
    return r;
}

#undef FZ
#undef FZ2
#undef FZ4
#undef FZ8
#undef FZ16
#undef FZ32
#undef FZ64

// --- Grupo g24a (BRIEF_21): Win::__MainWndProc (0x747EB0), prefijo WndProc. Anadir al FINAL de X87Math.cpp (usa Ftol2RegsCpp, definida mas arriba) ---
// _ftol2 (0x821B40) de ST0 = 1000.0f / (float)fild(frameLimit): el cociente sale del fdivr con la precision vigente (24 bits en el juego) y NO se guarda como float antes de _ftol2, pero a 24 bits
// es exactamente un float. Banderas acumuladas del EXE: PE si el fdivr es inexacto, ZE si frameLimit = 0 (cociente +inf); el fistp de _ftol2 anade PE cuando la parte fraccionaria no es cero aunque
// el cociente sea exacto (p. ej. 62,5 con frameLimit = 16) e IE cuando el cociente no cabe en 64 bits (+inf). Ftol2RegsCpp calcula el entero con enteros (no levanta banderas) y planta los registros
// muertos, de modo que aqui se levantan antes, con operaciones x87 que solo usan T-1 (se sobrescribe despues): una division inexacta (1/3) y una raiz de -1.
#pragma float_control(precise, on, push)
__declspec(noinline) unsigned int __cdecl WndProcTimerPeriodCpp(int frameLimit) {
    volatile float quotient = float(1000.0 / double(frameLimit)); // fild; fdivr [1000.0f]
    const float    v        = quotient;
    if (frameLimit == 0) { // +inf: el fistp de _ftol2 es una operacion invalida
        volatile double minusOne = -1.0;
        volatile double invalid  = sqrt(minusOne);
        (void)invalid;
    } else {
        // Parte fraccionaria no nula (con enteros: una comparacion x87 tocaria C0..C3): el fistp redondea y levanta PE
        const unsigned int bits     = std::bit_cast<unsigned int>(v);
        const int          exponent = int((bits >> 23) & 0xFFu) - 127;
        const unsigned int mantissa = bits & 0x7FFFFFu;
        const bool         integral = exponent >= 23 || (exponent < 0 ? (bits << 1) == 0 : (((mantissa | 0x800000u) & ((1u << (23 - exponent)) - 1u)) == 0));
        if (!integral) {
            volatile float one = 1.0f, three = 3.0f;
            volatile float inexact = one / three;
            (void)inexact;
        }
    }
    return Ftol2RegsCpp(v, 0).eax;
}
#pragma float_control(pop)

// --- Grupo g26a (BRIEF_21): CAEAudioHardware::PlaySound (0x4D86B0). Anadir al FINAL de X87Math.cpp (usa Ftol2RegsDblCpp, ya definido arriba) ---
// 0x4D8782..0x4D879B: fild movsx(posicion) (1..32767); fmul 0.01f (0x858C58); fimul length (int32 con signo); fstp dword. Con la x87 a 24 bits cada operacion redondea una sola vez
// (el producto de 15 + 24 bits cabe, y el del entero de 32 bits se redondea una vez, no dos como con float(length)): `double(...)` en cadena equivale a fmul/fimul del EXE. precise evita que
// /fp:fast reasocie (0.01f * length) primero. El float guardado (volatile) ya es exacto a 24 bits, asi que 0x406CE0 (fld; fstp qword) no cambia nada.
// floor del CRT (SSE2, 0x8219F0..0x821B10): |v| < 1 -> +0 (v >= +0) o -1.0 (v < 0); si no, el entero hacia -inf. Se calcula con enteros desde los bits del float (sin tocar ninguna bandera):
// v nunca es NaN, infinito ni cero negativo (posicion > 0 por un entero) y |v| < 2^40. _ftol2 trunca el resultado (ya entero) y deja EAX/EDX, las banderas y T-2 = R, T-1 = fraccion.
#pragma float_control(precise, on, push)
__declspec(noinline) unsigned int __cdecl AudioHwPercentPositionCpp(int percent, int length) {
    volatile float scaled = float(double(percent) * double(0.01f) * double(length)); // fild; fmul [0x858C58]; fimul length; fstp dword
    const unsigned int bits = *reinterpret_cast<const volatile unsigned int*>(&scaled);
    const bool negative = (bits >> 31) != 0;
    const int exponent = int((bits >> 23) & 0xFFu) - 127;
    long long floored;
    if (exponent < 0) { // |v| < 1
        floored = (negative && (bits << 1) != 0) ? -1ll : 0ll;
    } else {
        const unsigned long long mantissa = (bits & 0x7FFFFFu) | 0x800000u;
        unsigned long long magnitude;
        if (exponent >= 23) {
            magnitude = mantissa << (exponent - 23);
        } else {
            const unsigned int shift = 23u - unsigned(exponent);
            magnitude = mantissa >> shift;
            if (negative && (mantissa & ((1ull << shift) - 1ull)) != 0) {
                ++magnitude; // floor de un negativo no entero: una unidad mas de magnitud
            }
        }
        floored = negative ? -static_cast<long long>(magnitude) : static_cast<long long>(magnitude);
    }
    return Ftol2RegsDblCpp(double(floored), 0u).eax; // ECX de entrada: nadie lo lee tras la llamada
}
#pragma float_control(pop)
