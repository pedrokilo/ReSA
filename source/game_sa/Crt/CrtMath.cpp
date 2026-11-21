#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <intrin.h>
#include <cfloat>
#include <cstdint>
#include <cstring>
#include "CrtMath.h"

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
    reinterpret_cast<void(__cdecl*)(void*, void*, void*, int)>(0x82872C)(&x, &x, &x, code);
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
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtMathInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Floor, 0x8219F0);
    RH_ScopedGlobalInstall(ExeCrt::Ceil, 0x823820);
}
#endif
