/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGeneral
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
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

#include <random>

#include "X87Math.h"

// GeneralATanOfXYDblCpp (X87Math.cpp) devuelve un double: el fpatan puro de GetATanOfXY queda sin redondear en ST0. Aqui se declara como float con otro
// nombre (alias del enlazador) para que el metodo devuelva ST0 tal cual, sin pasar por memoria como float.
extern "C" float __cdecl GeneralATanOfXYFltCpp(float x, float y);
#pragma comment(linker, "/alternatename:_GeneralATanOfXYFltCpp=_GeneralATanOfXYDblCpp")

static std::random_device randomDevice;
static std::mt19937 randomEngine(randomDevice());

// 0x407180 - 2-arg wrapper to allow hooking the original function while the template has an extra `inclusive` arg
static int32 GetRandomNumberInRange_int32(int32 min, int32 max) {
    return CGeneral::GetRandomIntInRange(min, max); // La plantilla entera puede salir desfasada en uno
}

// 0x41BD90
// Como el original: lerp con la x87 (entero del RNG por 1/32767, por (max - min), mas min; un redondeo a 24 bits por operacion y el resultado
// queda en ST(0) sin guardar). El aritmetico esta en X87Math.cpp; el RNG se llama una sola vez, como el EXE, y acepta extremos invertidos.
template<>
float CGeneral::GetRandomNumberInRange<float>(float min, float max) {
    return GeneralRandomInRangeCpp(static_cast<int>(GetRandomNumber()), min, max);
}

void CGeneral::InjectHooks() {
    RH_ScopedNamespace(CGeneral);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LimitAngle, 0x53CB00);
    RH_ScopedInstall(LimitRadianAngle, 0x53CB50);
    RH_ScopedOverloadedInstall(GetRadianAngleBetweenPoints, "", 0x53CBE0, float(*)(float, float, float, float));
    RH_ScopedInstall(GetATanOfXY, 0x53CC70);
    RH_ScopedInstall(GetNodeHeadingFromVector, 0x53CDC0);
    RH_ScopedInstall(SolveQuadratic, 0x53CE30);
    RH_ScopedInstall(GetAngleBetweenPoints, 0x53CEA0);
    RH_ScopedGlobalInstall(GetRandomNumberInRange_int32, 0x407180);
    RH_ScopedOverloadedInstall(GetRandomNumberInRange<float>, "", 0x41BD90, float (*)(float, float));
}

// 0x53CB00
float CGeneral::LimitAngle(float angle) {
    while (angle >= 180.0f) {
        angle -= 360.0f;
    }

    while (angle < -180.0f) {
        angle += 360.0f;
    }

    return angle;
}

namespace {
// Como el original: constantes exactas del EXE (no las de common.h)
const float kGenPi           = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
const float kGenMinusPi      = std::bit_cast<float>(0xC0490FDBu); // 0x858CC0
const float kGenTwoPi        = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
const float kGenPiOver8      = std::bit_cast<float>(0x3EC90FDBu); // 0x859F50
const float kGenInvTwoPi     = std::bit_cast<float>(0x3E22F983u); // 0x8594F0
const float kGenRadToDeg     = std::bit_cast<float>(0x42652EE0u); // 0x859878 (no es el float más cercano a 180/pi)
} // namespace

// 0x53CB50
// Como el original: pi y 2pi exactos del EXE; NaN pasa sin tocar.
float CGeneral::LimitRadianAngle(float angle) {
    float result = std::clamp(angle, -25.0f, 25.0f);

    while (result >= kGenPi) {
        result -= kGenTwoPi;
    }

    while (result < kGenMinusPi) {
        result += kGenTwoPi;
    }

    return result;
}

// 0x53CBE0
// Como el original: transcripcion x87 en X87Math.cpp (cociente a la precision vigente, fpatan completo, pi/2 +- pi/2 en ese orden
// y signos de x e y evaluados como en el EXE).
float CGeneral::GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    return GeneralRadianAngleCpp(x1, y1, x2, y2);
}

// 0x53CC70
// Como el original: transcripcion x87 en X87Math.cpp; la rama x > 0, |x| >= |y| devuelve el fpatan sin redondear en ST(0).
float CGeneral::GetATanOfXY(float x, float y) {
    return GeneralATanOfXYFltCpp(x, y);
}

// 0x53CDC0
// Como el original: (2pi - a) + pi/8, por (1/2pi) y por 8 con las constantes del EXE, floor en double y truncado.
uint32 CGeneral::GetNodeHeadingFromVector(float x, float y) {
    float angle = GetRadianAngleBetweenPoints(x, y, 0.0f, 0.0f);
    if (angle < 0.0f) {
        angle += kGenTwoPi;
    }

    angle = kGenTwoPi - angle + kGenPiOver8;

    if (angle >= kGenTwoPi) {
        angle -= kGenTwoPi;
    }

    return (uint32)(int64)std::floor((double)(angle * kGenInvTwoPi * 8.0f));
}

/*!
* Solves: ax^2 + bx + c = 0
* @returns If there is a solution for the equation
* @addr 0x53CE30
*/
// Como el original: transcripcion x87 en X87Math.cpp (b*b - a*c*4 sin desbordar a float en registro, 1/a y * 0.5;
// x2 = (sqrt(d) - b) * (1/a) * 0.5 se escribe antes que x1 = (-b - sqrt(d)) * (1/a) * 0.5; un discriminante NaN resuelve).
bool CGeneral::SolveQuadratic(float a, float b, float c, float& x1, float& x2) {
    return GeneralSolveQuadraticCpp(a, b, c, &x1, &x2);
}

// 0x53CEA0
// Como el original: por 57.2957763671875 (constante del EXE).
float CGeneral::GetAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    return GetRadianAngleBetweenPoints(x1, y1, x2, y2) * kGenRadToDeg;
}

uint16 CGeneral::GetRandomNumber() { // Why does this return `uint16`? Should be `int32` (same as retval of `rand()`)
    static_assert(RAND_MAX == 0x7FFF, "PC-generated random numbers should not exceed 32767");
#ifdef BETTER_RNG
    static_assert(false, "PC-generated random numbers should not exceed 32767");
#else
    return rand();
#endif
}

float CGeneral::GetRadianAngleBetweenPoints(CVector2D a, CVector2D b) {
    return GetRadianAngleBetweenPoints(a.x, a.y, b.x, b.y);
}

bool CGeneral::RandomBool(float chanceOfTrue) {
    assert(chanceOfTrue <= 100.f);
    return CGeneral::GetRandomNumberInRange(0.f, 100.f) <= chanceOfTrue;
}

/*!
* @return true/false with 50/50 change
* @addr notsa
*/
bool CGeneral::DoCoinFlip() {
	return CGeneral::GetRandomNumber() >= RAND_MAX / 2;
}
