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
static bool ResidueHelpersAreTrivial() {
    auto  member = &ResidueEcx::Get;
    const void* getAddress;
    memcpy(&getAddress, &member, sizeof getAddress);
    return ResidueHelperIsTrivial(reinterpret_cast<const void*>(&ResidueRegsCpp)) && ResidueHelperIsTrivial(getAddress) && EntryEaxHelperIsTrivial() && PlayerPedResidueFloatIsTrivial() && CarEnterExitEntryEdxHelperIsTrivial();
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
        return { 0, entryEcx, high };
    }
    volatile float fraction = float(v - double(rounded)); // fild + fsubp + fstp dword con el modo vigente
    const unsigned int fracBits = std::bit_cast<unsigned int>(static_cast<float>(fraction));
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
