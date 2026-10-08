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

static std::random_device randomDevice;
static std::mt19937 randomEngine(randomDevice());

// 0x407180 - 2-arg wrapper to allow hooking the original function while the template has an extra `inclusive` arg
static int32 GetRandomNumberInRange_int32(int32 min, int32 max) {
    return CGeneral::GetRandomIntInRange(min, max); // La plantilla entera puede salir desfasada en uno
}

// 0x41BD90
// x87 is intentional: GTA changes its precision control, and intermediate
// values have a wider exponent range than float. SSE/double expressions can
// change rounding or overflow. Keep ST(0) intact for native callers as well.
template<>
__declspec(naked) float CGeneral::GetRandomNumberInRange<float>(float min, float max) {
    __asm {
        push ecx
        call GetRandomNumber
        movzx eax, ax
        mov [esp], eax
        fild dword ptr [esp]
        fmul dword ptr [RAND_MAX_FLOAT_RECIPROCAL]
        fld dword ptr [esp + 12]
        fsub dword ptr [esp + 8]
        fmulp st(1), st(0)
        fadd dword ptr [esp + 8]
        pop ecx
        ret
    }
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
const float kGenZero         = 0.0f;                              // 0x858B50
const float kGenHalf         = 0.5f;                              // 0x858B8C
const float kGenOne          = 1.0f;                              // 0x858624
const float kGenFour         = 4.0f;                              // 0x858B90
const float kGenMinusOne     = -1.0f;                             // 0x858C1C
const float kGenTinyY        = std::bit_cast<float>(0x38D1B717u); // 0x858FC4 (0.0001)
const float kGenHalfPi       = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4
const float kGenMinusHalfPi  = std::bit_cast<float>(0xBFC90FDBu); // 0x859998
const float kGenPi           = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
const float kGenMinusPi      = std::bit_cast<float>(0xC0490FDBu); // 0x858CC0
const float kGenTwoPi        = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
const float kGenThreeHalfPi  = std::bit_cast<float>(0x4096CBE4u); // 0x863AC4
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
// Como el original: transcripción x87 (cociente a la precisión vigente, fpatan completo, pi/2 +- pi/2 en ese orden
// y banderas de la comparación de y conservadas a través de fpatan).
__declspec(naked) float CGeneral::GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    __asm {
        fld   dword ptr [esp+0Ch]
        fsub  dword ptr [esp+4]
        fld   dword ptr [esp+10h]
        fsub  dword ptr [esp+8]
        fcom  dword ptr [kGenZero]
        fnstsw ax
        test  ah, 44h
        jp    L_yNotZero
        fstp  st(0)
        fld   dword ptr [kGenTinyY]
    L_yNotZero:
        fld   st(1)
        fcomp dword ptr [kGenZero]
        fnstsw ax
        fcom  dword ptr [kGenZero]
        fxch  st(1)
        test  ah, 41h
        fdiv  st, st(1)
        fld1
        fpatan
        fstp  st(1)
        fnstsw ax
        jne   L_xNotPositive
        test  ah, 41h
        jne   L_xPositiveYNotPositive
        fsubr dword ptr [kGenHalfPi]
        fadd  dword ptr [kGenHalfPi]
        ret
    L_xPositiveYNotPositive:
        fadd  dword ptr [kGenHalfPi]
        fsubr dword ptr [kGenHalfPi]
        ret
    L_xNotPositive:
        test  ah, 41h
        jne   L_xNotPositiveYNotPositive
        fadd  dword ptr [kGenHalfPi]
        fsubr dword ptr [kGenMinusHalfPi]
        ret
    L_xNotPositiveYNotPositive:
        fsubr dword ptr [kGenHalfPi]
        fsub  dword ptr [kGenHalfPi]
        ret
    }
}

// 0x53CC70
// Como el original: transcripción x87; la rama x > 0, |x| >= |y| devuelve fpatan sin redondear en ST(0).
__declspec(naked) float CGeneral::GetATanOfXY(float x, float y) {
    __asm {
        fld   dword ptr [esp+4]
        fcomp dword ptr [kGenZero]
        fnstsw ax
        test  ah, 44h
        jp    L_notBothZero
        fld   dword ptr [esp+8]
        fcomp dword ptr [kGenZero]
        fnstsw ax
        test  ah, 44h
        jp    L_notBothZero
        fld   dword ptr [kGenZero]
        ret
    L_notBothZero:
        fld   dword ptr [esp+4]
        fcomp dword ptr [kGenZero]
        fld   dword ptr [esp+4]
        fnstsw ax
        test  ah, 5
        jp    L_absX
        fchs
    L_absX:
        fld   dword ptr [esp+8]
        fcomp dword ptr [kGenZero]
        fld   dword ptr [esp+8]
        fnstsw ax
        test  ah, 5
        jp    L_absY
        fchs
    L_absY:
        fxch  st(1)
        fcomp st(1)
        fstp  st(0)
        fld   dword ptr [esp+8]
        fnstsw ax
        fcomp dword ptr [kGenZero]
        fld   dword ptr [esp+4]
        test  ah, 5
        fnstsw ax
        fcomp dword ptr [kGenZero]
        jp    L_xNotSmaller
        test  ah, 41h
        fnstsw ax
        jne   L_yNotPositive1
        test  ah, 41h
        jne   L_yPositiveXNotPositive1
        fld   dword ptr [esp+4]
        fdiv  dword ptr [esp+8]
        fld1
        fpatan
        fsubr dword ptr [kGenHalfPi]
        ret
    L_yPositiveXNotPositive1:
        fld   dword ptr [kGenMinusOne]
        fdiv  dword ptr [esp+8]
        fmul  dword ptr [esp+4]
        fld1
        fpatan
        fadd  dword ptr [kGenHalfPi]
        ret
    L_yNotPositive1:
        test  ah, 41h
        jne   L_yNotPositiveXNotPositive1
        fld   dword ptr [kGenMinusOne]
        fdiv  dword ptr [esp+8]
        fmul  dword ptr [esp+4]
        fld1
        fpatan
        fadd  dword ptr [kGenThreeHalfPi]
        ret
    L_yNotPositiveXNotPositive1:
        fld   dword ptr [esp+4]
        fdiv  dword ptr [esp+8]
        fld1
        fpatan
        fsubr dword ptr [kGenThreeHalfPi]
        ret
    L_xNotSmaller:
        test  ah, 41h
        fnstsw ax
        jne   L_yNotPositive2
        test  ah, 41h
        jne   L_yPositiveXNotPositive2
        fld   dword ptr [esp+8]
        fdiv  dword ptr [esp+4]
        fld1
        fpatan
        ret
    L_yPositiveXNotPositive2:
        fld   dword ptr [kGenMinusOne]
        fdiv  dword ptr [esp+4]
        fmul  dword ptr [esp+8]
        fld1
        fpatan
        fsubr dword ptr [kGenPi]
        ret
    L_yNotPositive2:
        test  ah, 41h
        jne   L_yNotPositiveXNotPositive2
        fld   dword ptr [kGenMinusOne]
        fdiv  dword ptr [esp+4]
        fmul  dword ptr [esp+8]
        fld1
        fpatan
        fsubr dword ptr [kGenTwoPi]
        ret
    L_yNotPositiveXNotPositive2:
        fld   dword ptr [esp+8]
        fdiv  dword ptr [esp+4]
        fld1
        fpatan
        fadd  dword ptr [kGenPi]
        ret
    }
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
// Como el original: transcripción x87 (b*b - a*c*4 sin desbordar a float en registro, 1/a y * 0.5;
// x2 = (sqrt(d) - b) * (1/a) * 0.5 se escribe antes que x1 = (-b - sqrt(d)) * (1/a) * 0.5; un discriminante NaN resuelve).
__declspec(naked) bool CGeneral::SolveQuadratic(float a, float b, float c, float& x1, float& x2) {
    __asm {
        fld   dword ptr [esp+8]
        fmul  dword ptr [esp+8]
        fld   dword ptr [esp+4]
        fmul  dword ptr [esp+0Ch]
        fmul  dword ptr [kGenFour]
        fsubp st(1), st
        fcom  dword ptr [kGenZero]
        fnstsw ax
        test  ah, 5
        jp    L_solve
        xor   al, al
        fstp  st(0)
        ret
    L_solve:
        fsqrt
        mov   eax, dword ptr [esp+14h]
        mov   ecx, dword ptr [esp+10h]
        fld   dword ptr [kGenOne]
        fdiv  dword ptr [esp+4]
        fld   st(1)
        fsub  dword ptr [esp+8]
        fmul  st, st(1)
        fmul  dword ptr [kGenHalf]
        fstp  dword ptr [eax]
        mov   al, 1
        fld   dword ptr [esp+8]
        fchs
        fsub  st, st(2)
        fmul  st, st(1)
        fmul  dword ptr [kGenHalf]
        fstp  dword ptr [ecx]
        fstp  st(0)
        fstp  st(0)
        ret
    }
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
