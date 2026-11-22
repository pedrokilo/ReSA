/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ExeCrt
 * Reconstrucción: COMPLETA — 138/138 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/138 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <intrin.h>
#include <cfloat>
#include <cstdint>
#include <cstring>
#include "CrtMath.h"
#include "CrtLowIo.h"
#include <cmath>
#include <cstddef>

// Reconstruccion de floor/ceil del CRT del EXE (lote CRT2A). Ver CrtMath.h.
// El EXE trunca con desplazamientos de 64 bits sobre la representacion IEEE (psrlq/psllq con cuenta 0x433 - exponente) y corrige con una comparacion
// (cmpltpd / cmpnlepd) segun el signo. Las comparaciones se hacen sobre doubles reales (no se reordenan: con NaN nunca llegan a ejecutarse, salen antes).

namespace ExeCrt {
namespace {
inline uint64_t Bits(double x) { uint64_t b; std::memcpy(&b, &x, 8); return b; }
inline double   FromBits(uint64_t b) { double x; std::memcpy(&x, &b, 8); return x; }
// psrlq/psllq con cuenta >= 64 dan 0
inline uint64_t Shr(uint64_t v, uint32_t n) { return n >= 64 ? 0 : (v >> n); }
inline uint64_t Shl(uint64_t v, uint32_t n) { return n >= 64 ? 0 : (v << n); }

// Suma/resta de doubles con SSE2 (addsd/subsd del EXE): inmunes a la precision de la x87 (el juego la fija a 24 bits)
inline double AddSd(double a, double b) { return _mm_cvtsd_f64(_mm_add_sd(_mm_set_sd(a), _mm_set_sd(b))); }
inline double SubSd(double a, double b) { return _mm_cvtsd_f64(_mm_sub_sd(_mm_set_sd(a), _mm_set_sd(b))); }

inline void RaiseDomain(double& x, int code) {
    MathError(&x, &x, &x, code);
}
} // namespace

// true si el EXE entra por la rama SSE2: 0xC9C400 != 0 y las excepciones de MXCSR y de la FPU enmascaradas (la misma condicion de floor, ceil y pow)
bool UseSse2MathPath() {
    if (*reinterpret_cast<int*>(0xC9C400) == 0) {
        return false;
    }
    if ((_mm_getcsr() & 0x1F80u) != 0x1F80u) {
        return false;
    }
    // palabra de control x87 con las seis excepciones enmascaradas (bit 6 reservado: siempre 1)
    const unsigned int cw = _control87(0, 0);
    return (cw & (_EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT)) ==
           (_EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT);
}

bool FloorCore(double x, double& result) {
    const uint64_t bits = Bits(x);
    const uint32_t e12 = static_cast<uint32_t>(bits >> 52);          // signo (bit 11) y exponente (bits 0..10)
    const uint32_t sh = 0x433u - (e12 & 0x7FFu);                       // cuenta de desplazamiento (puede ser "negativa": >= 64 sin signo)
    const uint64_t trunc = Shl(Shr(bits, sh), sh);                     // truncado hacia cero (conserva el signo)
    if ((e12 & 0x800u) == 0) {                                         // positivo
        if (static_cast<int32_t>(e12) < 0x3FF) { result = 0.0; return false; }
        if (static_cast<int32_t>(e12) <= 0x432) { result = FromBits(trunc); return false; }
    } else {                                                           // negativo
        if (static_cast<int32_t>(e12) < 0xBFF) {                       // |x| < 1: -1.0 si x < 0, -0.0 si x == -0.0
            result = (x < -0.0) ? -1.0 : -0.0;
            return false;
        }
        if (static_cast<int32_t>(e12) <= 0xC32) {
            const double t = FromBits(trunc);
            result = (x < t) ? SubSd(t, 1.0) : t;
            return false;
        }
    }
    result = x;
    return x != x; // |x| >= 2^52, infinito o NaN: el EXE devuelve x (y avisa al manejador con NaN)
}

bool CeilCore(double x, double& result) {
    const uint64_t bits = Bits(x);
    const uint32_t e12 = static_cast<uint32_t>(bits >> 52);
    const uint32_t sh = 0x433u - (e12 & 0x7FFu);
    const uint64_t trunc = Shl(Shr(bits, sh), sh);
    if ((e12 & 0x800u) == 0) {                                         // positivo
        if (static_cast<int32_t>(e12) < 0x3FF) {                       // x < 1: 1.0 si x > 0, 0.0 si x == 0
            result = !(x <= 0.0) ? 1.0 : 0.0;
            return false;
        }
        if (static_cast<int32_t>(e12) <= 0x432) {
            const double t = FromBits(trunc);
            result = !(x <= t) ? AddSd(t, 1.0) : t;
            return false;
        }
    } else {                                                           // negativo
        if (static_cast<int32_t>(e12) < 0xBFF) { result = -0.0; return false; }
        if (static_cast<int32_t>(e12) <= 0xC32) { result = FromBits(trunc); return false; }
    }
    result = x;
    return x != x;
}

// 0x8219F0
double __cdecl Floor(double x) {
    if (!UseSse2MathPath()) {
        return reinterpret_cast<double(__cdecl*)(double)>(0x828A60)(x);
    }
    double r;
    if (FloorCore(x, r)) {
        RaiseDomain(x, 0x3ED);
        return x;
    }
    return r;
}

// 0x823820
double __cdecl Ceil(double x) {
    if (!UseSse2MathPath()) {
        return reinterpret_cast<double(__cdecl*)(double)>(0x82BA31)(x);
    }
    double r;
    if (CeilCore(x, r)) {
        RaiseDomain(x, 0x3EC);
        return x;
    }
    return r;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
// Lote S4 (codigo vivo, parte P3): manejador de errores matematicos (0x82872C), matherr por defecto (0x833605) y _ftol2 (0x821B40).
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace {
// struct _exception del CRT (0x20 bytes en la pila del EXE, de ebp-0x24 a ebp-4): tipo, nombre de la funcion, argumentos y valor devuelto.
struct MathException {
    int         type;
    const char* name;
    double      arg1;
    double      arg2;
    double      retval;
};
static_assert(sizeof(MathException) == 0x20 && offsetof(MathException, retval) == 0x18);

// Una carga y un guardado de double por la x87 (fld/fstp qword) dejan intacto el valor salvo un NaN senalizador, que sale silencioso (bit 51).
inline double X87Pass(double v) {
    if (v != v) {
        return FromBits(Bits(v) | 0x0008000000000000ull);
    }
    return v;
}

enum class MathPath { None, Generic, Domain, DomainCopy, Underflow, ResultOne };
struct MathAction {
    MathPath    path;
    int         type;
    const char* name;
};

// Tabla de 0x82872C: codigo de error -> camino, tipo y nombre (cadenas del CRT: 0x879C34 "pow", 0x879CA0 "log", 0x879CA4 "exp", 0x87E7E0 "exp2", 0x87E768 "log10", 0x87E760 "log2", 0x888468 "exp10",
// 0x87E844 "atan", 0x87E830 "ceil", 0x87E7C4 "floor", 0x87E758 "modf"). Tipos: 1 DOMAIN, 2 SING, 3 OVERFLOW, 4 UNDERFLOW.
MathAction LookupMathAction(int code) {
    switch (code) {
    case 0x002: return { MathPath::Generic, 2, "log" };
    case 0x003: return { MathPath::Domain, 1, "log" };
    case 0x008: return { MathPath::Generic, 2, "log10" };
    case 0x009: return { MathPath::Domain, 1, "log10" };
    case 0x00E: return { MathPath::Generic, 3, "exp" };
    case 0x00F: return { MathPath::Underflow, 4, "exp" };
    case 0x018: return { MathPath::Generic, 3, "pow" };
    case 0x019: return { MathPath::Underflow, 4, "pow" };
    case 0x01A: return { MathPath::ResultOne, 0, nullptr };
    case 0x01B: return { MathPath::Generic, 2, "pow" };
    case 0x01C: return { MathPath::Domain, 1, "pow" };
    case 0x01D: return { MathPath::DomainCopy, 1, "pow" };
    case 0x0A1: return { MathPath::Generic, 3, "exp2" };
    case 0x0A2: return { MathPath::Generic, 4, "exp2" };
    case 0x0A6: return { MathPath::Generic, 3, "exp10" };
    case 0x0AA: return { MathPath::Generic, 2, "log2" };
    case 0x0AB: return { MathPath::Domain, 1, "log2" };
    case 0x3E8: return { MathPath::DomainCopy, 1, "log" };
    case 0x3E9: return { MathPath::DomainCopy, 1, "log10" };
    case 0x3EA: return { MathPath::DomainCopy, 1, "exp" };
    case 0x3EB: return { MathPath::DomainCopy, 1, "atan" };
    case 0x3EC: return { MathPath::DomainCopy, 1, "ceil" };
    case 0x3ED: return { MathPath::DomainCopy, 1, "floor" };
    case 0x3EE: return { MathPath::Domain, 1, "pow" };
    case 0x3EF: return { MathPath::DomainCopy, 1, "modf" };
    default:    return { MathPath::None, 0, nullptr };
    }
}
} // namespace

// 0x833605: matherr por defecto del CRT (`xor eax, eax; ret`): no trata el error. Solo se llega a ella por el puntero 0x8E3128 (el EXE la tiene pegada a la funcion siguiente: sin enganche; la sustituye el puntero, ver CrtMathInjectHooks).
int __cdecl DefaultMatherr(void* /*exception*/) {
    return 0;
}

// 0x82872C (cdecl, (arg1*, arg2*, resultado*, codigo); `ret`, 660 bytes; sin valor de retorno): manejador de errores de la biblioteca matematica (floor, ceil, pow...). Segun el codigo rellena un struct _exception (tipo, nombre, arg1, arg2 y el valor
// devuelto, que es el que ya hay en *resultado), llama a matherr (puntero 0x8E3128) y deja en *resultado el valor que ese devuelva. Con matherr devolviendo 0 pone errno (_errno 0x82B181): ERANGE (0x22) en los caminos de singularidad y
// desbordamiento (tipo fijo, nombre de la tabla), EDOM (0x21) en los de dominio (tipo 1). Camino de dominio con copia: antes de nada *resultado = *arg1. El de subdesbordamiento (tipo 4 con 0x0F y 0x19) no toca errno. Codigo 0x1A: *resultado = 1.0, sin matherr.
// Los doubles pasan por la x87 (fld/fstp): un NaN senalizador sale silencioso. Se quita la cookie de pila (/GS) del EXE.
void __cdecl MathError(double* arg1, double* arg2, double* result, int code) {
    const MathAction action = LookupMathAction(code);
    if (action.path == MathPath::None) {
        return;
    }
    if (action.path == MathPath::ResultOne) {
        *result = 1.0;
        return;
    }
    if (action.path == MathPath::DomainCopy) {
        *result = X87Pass(*arg1);
    }
    MathException exception;
    exception.type   = action.type;
    exception.name   = action.name;
    exception.arg1   = X87Pass(*arg1);
    exception.arg2   = X87Pass(*arg2);
    exception.retval = X87Pass(*result);
    using MatherrFn = int(__cdecl*)(MathException*);
    const MatherrFn matherr = *reinterpret_cast<MatherrFn*>(0x8E3128);
    if (matherr(&exception) == 0) {
        if (action.path == MathPath::Generic) {
            *ErrnoPtr() = 0x22;
        } else if (action.path == MathPath::Domain || action.path == MathPath::DomainCopy) {
            *ErrnoPtr() = 0x21;
        }
    }
    *result = X87Pass(exception.retval);
}

// Equivalente de 0x821B40 (el `_ftol2` de MSVC; el argumento llega en ST0 y el resultado en EDX:EAX, asi que NO se puede enganchar desde C++: sus llamadas del EXE estan en funciones ya reemplazadas y el resto del
// codigo propio ya lo expresa en linea): float -> entero de 64 bits truncado hacia cero. fistp redondea al mas cercano (par en empates; fuera de rango o NaN da 0x8000000000000000, que se devuelve tal cual); despues corrige
// segun el signo y el resto x - r (redondeado a float, a 24 bits): con x >= 0 resta 1 si el resto es negativo no nulo, con x < 0 suma 1 si es positivo no nulo. Con la palabra baja a 0 y el resto sin bits (0 o indefinido) sale sin corregir.
int64_t FloatToInt64(double x) {
    int64_t r;
    if (!(std::fabs(x) < 9223372036854775808.0)) {
        r = INT64_MIN;
    } else {
        r = static_cast<int64_t>(std::nearbyint(x));
    }
    const uint32_t low  = static_cast<uint32_t>(r);
    const uint32_t high = static_cast<uint32_t>(static_cast<uint64_t>(r) >> 32);
    if (low == 0 && (high & 0x7FFFFFFFu) == 0) {
        return r;
    }
    const float fraction = static_cast<float>(x - static_cast<double>(r));
    uint32_t    fractionBits;
    std::memcpy(&fractionBits, &fraction, 4);
    const bool negative = ((low == 0 ? high : static_cast<uint32_t>(Bits(x) >> 32)) & 0x80000000u) != 0; // con low == 0 el EXE prueba el signo del entero (edx)
    if (negative) {
        const uint64_t sum = static_cast<uint64_t>(fractionBits ^ 0x80000000u) + 0x7FFFFFFFull;
        r += (sum >> 32) ? 1 : 0;
    } else {
        const uint64_t sum = static_cast<uint64_t>(fractionBits) + 0x7FFFFFFFull;
        r -= (sum >> 32) ? 1 : 0;
    }
    return r;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtMathInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Floor, 0x8219F0);
    RH_ScopedGlobalInstall(ExeCrt::Ceil, 0x823820);
    RH_ScopedGlobalInstall(ExeCrt::MathError, 0x82872C);

    // 0x833605 (matherr por defecto, 3 bytes pegados a la funcion siguiente: no admite salto): se sustituye el puntero que la usa (0x8E3128) si nadie lo ha cambiado.
    auto& userMatherr = *reinterpret_cast<uintptr_t*>(0x8E3128);
    if (userMatherr != 0) { // el EXE solo trae esta rutina (no hay _matherr de usuario): se sustituye sin comparar con la direccion del EXE (lote LNK5A)
        userMatherr = reinterpret_cast<uintptr_t>(&ExeCrt::DefaultMatherr);
    }
}
#endif
