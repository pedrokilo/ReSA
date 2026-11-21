#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <intrin.h>
#include <emmintrin.h>
#include <cstdint>
#include <cstring>
#include "CrtMath.h"
#include "CrtPow.h"

// Reconstruccion de la pow SSE2 de la biblioteca matematica de Intel que enlaza el CRT del EXE (0x829B99, 2.861 bytes) y de su envoltura 0x8220F0 (lote CRT3A). Ver CrtPow.h.
//
// Transcripcion instruccion a instruccion con intrinsecos SSE2: cada variable xmmN es el registro del mismo nombre, y cada instruccion del EXE es la misma operacion
// (movlpd conserva la mitad alta; movsd entre registros conserva la alta; las operaciones escalares *sd no tocan la mitad alta). Las tablas (0x8884F0..0x88BE18) estan
// copiadas por valor en CrtPowTables.inc y se indexan con la misma aritmetica de direcciones del EXE. Las etiquetas Lxxxx son las direcciones del EXE (0x829B99 + desplazamiento).
// No se usa la x87 ni valores double por retorno en las constantes (un double devuelto por valor pasa por ST0 y silencia los NaN señalizantes: 0x88BDE8 es uno).
// El unico codigo del EXE que se sigue llamando por direccion es el manejador de errores de la CRT (0x82872C) y la rama x87 de la envoltura (0x822184).

namespace ExeCrt {
namespace {
alignas(16) const uint64_t kTab[] = {
#include "CrtPowTables.inc"
};
constexpr uint32_t kTabBase = 0x8884F0;
static_assert(sizeof(kTab) == 0x88BE18 - 0x8884F0, "tabla de pow incompleta");

using V = __m128d;
inline __m128i I(V v) { return _mm_castpd_si128(v); }
inline V       D(__m128i v) { return _mm_castsi128_pd(v); }

inline const uint8_t* At(uint32_t addr) { return reinterpret_cast<const uint8_t*>(kTab) + (addr - kTabBase); }
inline V LdS(uint32_t addr) { return _mm_load_sd(reinterpret_cast<const double*>(At(addr))); } // operando de memoria de 64 bits de un *sd (solo cuenta la parte baja)
inline V LdP(uint32_t addr) { return _mm_loadu_pd(reinterpret_cast<const double*>(At(addr))); } // operando de 128 bits
inline V LoadLTab(V dst, uint32_t addr) { return _mm_loadl_pd(dst, reinterpret_cast<const double*>(At(addr))); } // movlpd xmm, [tabla]
inline V LoadLArg(V dst, const double& arg) { return _mm_loadl_pd(dst, &arg); }                                   // movlpd xmm, [esp+4 / esp+0xC]
inline V MulSdArg(V a, const double& arg) { return _mm_mul_sd(a, _mm_load_sd(&arg)); }
inline V Zero() { return _mm_setzero_pd(); }

template <int N> inline V Srl(V a) { return D(_mm_srli_epi64(I(a), N)); }  // psrlq xmm, imm
template <int N> inline V Sll(V a) { return D(_mm_slli_epi64(I(a), N)); }  // psllq xmm, imm
template <int N> inline uint32_t Pextrw(V a) { return static_cast<uint32_t>(_mm_extract_epi16(I(a), N)); }
template <int N> inline V Pinsrw(V a, uint32_t v) { return D(_mm_insert_epi16(I(a), static_cast<int>(v), N)); }
template <int N> inline V Pshufd(V a) { return D(_mm_shuffle_epi32(I(a), N)); }
inline uint32_t Movd(V a) { return static_cast<uint32_t>(_mm_cvtsi128_si32(I(a))); }          // movd r32, xmm
inline V MovdTo(uint32_t v) { return D(_mm_cvtsi32_si128(static_cast<int>(v))); }             // movd xmm, r32 (pone a cero el resto)
inline V Movsd(V dst, V src) { return _mm_move_sd(dst, src); }                                 // movsd xmm, xmm
inline V Xorp(V a, V b) { return _mm_xor_pd(a, b); }
inline V Orp(V a, V b) { return _mm_or_pd(a, b); }
inline V Andp(V a, V b) { return _mm_and_pd(a, b); }
inline uint32_t Pmovmskb(V a) { return static_cast<uint32_t>(_mm_movemask_epi8(I(a))); }
inline V Pcmpeqd(V a, V b) { return D(_mm_cmpeq_epi32(I(a), I(b))); }
inline V Psubd(V a, V b) { return D(_mm_sub_epi32(I(a), I(b))); }
inline V Psubq(V a, V b) { return D(_mm_sub_epi64(I(a), I(b))); }
inline V Psllq(V a, V count) { return D(_mm_sll_epi64(I(a), I(count))); }                      // cuenta >= 64: cero
inline V Pmaxsw(V a, V b) { return D(_mm_max_epi16(I(a), I(b))); }

// Escribe en la salida estandar de "errores matematicos": 0x82872C(&x, &y, &resultado, codigo); el resultado vuelve en ST0 con lo que deje el manejador
double RaiseError(double& x, double& y, V result, uint32_t code) {
    double res;
    _mm_storel_pd(&res, result);
    reinterpret_cast<void(__cdecl*)(void*, void*, void*, int)>(0x82872C)(&x, &y, &res, static_cast<int>(code));
    return res;
}
inline double Ret(V v) { return _mm_cvtsd_f64(v); }
} // namespace

// 0x829B99 (pow en SSE2; argumentos double en la pila [esp+4] y [esp+0xC], resultado en ST0)
double __cdecl PowSse2(double x, double y) {
    V x0 = Zero(), x1 = Zero(), x2 = Zero(), x3 = Zero(), x4 = Zero(), x5 = Zero(), x6 = Zero(), x7 = Zero();
    uint32_t eax = 0, ecx = 0, edx = 0, esi = 0, edi = 0;
    volatile double sink; // para los divsd que solo levantan banderas

    // ----------------------------------------------------------------------------- 0x829B99
    x0 = LoadLArg(x0, x);
    x7 = LoadLTab(x7, 0x88BD80);
    x2 = LoadLTab(x2, 0x88BD90);
    x7 = Andp(x7, x0);
    x4 = Movsd(x4, x0);
    x0 = Srl<0x2C>(x0);
    eax = Pextrw<0>(x0);
    x7 = Orp(x7, x2);
    ecx = Pextrw<3>(x4);
    eax &= 0xFF; eax += 1; eax &= 0x1FE;
    x7 = _mm_mul_sd(x7, LdS(0x8884F0 + eax * 4));
    x5 = LoadLTab(x5, 0x8884F0 + eax * 4);
    eax += eax;
    x6 = LdP(0x888900 + eax * 4);
    edx = 0x7FEF;
    edx -= ecx;
    ecx -= 0x10;
    ecx |= edx;
    if (ecx >= 0x80000000u) goto L9EE7;
    ecx = 0;
    edx = 0x3FE7F;
L9C16:
    x1 = MovdTo(edx);
    x0 = Psubq(x0, x1);
    x0 = Srl<8>(x0);
    x0 = _mm_cvtepi32_pd(I(x0));
    x1 = LoadLTab(x1, 0x88BDD8);
    x3 = Movsd(x3, x7);
    x7 = Srl<0x26>(x7);
    eax = Pextrw<0>(x7);
    x4 = Andp(x4, LdP(0x88BD80));
    eax &= 0xFF; eax += 1; eax &= 0x1FE;
    x3 = _mm_mul_sd(x3, LdS(0x889110 + eax * 4));
    x5 = _mm_mul_sd(x5, LdS(0x889110 + eax * 4));
    eax += eax;
    x6 = _mm_add_pd(x6, LdP(0x889520 + eax * 4));
    x4 = Orp(x4, LdP(0x88BD90));
    x6 = _mm_add_sd(x6, x0);
    x1 = Andp(x1, x4);
    x2 = Movsd(x2, x3);
    x3 = Srl<0x1F>(x3);
    eax = Pextrw<0>(x3);
    x0 = LoadLTab(x0, 0x88BDD8);
    x4 = _mm_sub_sd(x4, x1);
    x7 = LoadLTab(x7, 0x88BDE0);
    eax &= 0x1FF; eax += 1; eax &= 0x3FE;
    x5 = _mm_mul_sd(x5, LdS(0x889D30 + eax * 4));
    x2 = _mm_mul_sd(x2, LdS(0x889D30 + eax * 4));
    x6 = _mm_add_pd(x6, LdP(0x88A540 + eax * 8));
    x0 = Andp(x0, x5);
    x5 = _mm_sub_sd(x5, x0);
    x7 = _mm_add_sd(x7, x2);
    x3 = Movsd(x3, x0);
    x0 = _mm_mul_sd(x0, x1);
    x1 = _mm_mul_sd(x1, x5);
    x3 = _mm_mul_sd(x3, x4);
    x2 = _mm_sub_sd(x2, x0);
    x4 = _mm_mul_sd(x4, x5);
    x0 = Movsd(x0, x6);
    x2 = _mm_sub_sd(x2, x1);
    x6 = _mm_add_sd(x6, x7);
    x1 = LoadLArg(x1, y);
    eax = Pextrw<3>(x1);
    x2 = _mm_sub_sd(x2, x3);
    x0 = _mm_sub_sd(x0, x6);
    x3 = LoadLTab(x3, 0x88BDD8);
    edx = Pextrw<3>(x6);
    x2 = _mm_sub_sd(x2, x4);
    x4 = Movsd(x4, x6);
    x0 = _mm_add_sd(x0, x7);
    x7 = _mm_sub_sd(x7, x2);
    x6 = _mm_sub_sd(x6, x2);
    x7 = _mm_unpacklo_pd(x7, x7);
    eax &= 0x7FF0;
    if (eax >= 0x7FF0u) goto LA258;
    edx &= 0x7FF0;
    eax -= 0x3FF0;
    eax += edx;
    edx = 0x40A0;
    edx -= eax;
    eax -= 0x3C70;
    edx |= eax;
    if (edx >= 0x80000000u) goto LA641;
L9D68:
    x4 = _mm_sub_sd(x4, x6);
    x2 = _mm_sub_sd(x2, x4);
    x4 = LoadLTab(x4, 0x88BDD8);
    x3 = Andp(x3, x1);
    x4 = Andp(x4, x6);
    x0 = _mm_sub_sd(x0, x2);
    x2 = Xorp(x2, x2);
    edx = 0x4060;
    x2 = Pinsrw<3>(x2, edx);
    x5 = Movsd(x5, x3);
    x3 = _mm_mul_sd(x3, x4);
    x6 = _mm_sub_sd(x6, x4);
    x1 = _mm_sub_sd(x1, x5);
    x3 = _mm_mul_sd(x3, x2);
    x5 = _mm_mul_sd(x5, x6);
    x2 = LdP(0x88B550);
    x4 = _mm_mul_sd(x4, x1);
    eax = static_cast<uint32_t>(_mm_cvtsd_si32(x3));
    x6 = _mm_mul_sd(x6, x1);
    x1 = LdP(0x88B560);
    x5 = _mm_add_sd(x5, x4);
    x4 = Pshufd<0xEE>(x6);
    x5 = _mm_add_sd(x5, x6);
    edx = 0x1FF7F;
    edx -= eax;
    eax += 0x1E1FF;
    edx |= eax;
    eax -= 0x1E1FF;
    if (static_cast<int32_t>(edx) <= 0) goto LA3D1;
    ecx += eax;
    eax &= 0x7F;
    ecx &= 0xFFFFFF80u;
    ecx += 0x1FF80;
    x4 = _mm_add_sd(x4, x0);
    x0 = Movsd(x0, x3);
    x3 = _mm_add_sd(x3, LdS(0x88BD70));
    x2 = _mm_mul_pd(x2, x7);
    x3 = _mm_sub_sd(x3, LdS(0x88BD70));
    x7 = _mm_mul_pd(x7, x7);
    x0 = _mm_sub_sd(x0, x3);
    eax *= 16;
    x3 = LdP(0x88B570 + eax);
    x6 = LdP(0x88BDB0);
    x1 = _mm_mul_pd(x1, x7);
    x2 = _mm_add_pd(x2, x1);
    x1 = Pshufd<0xEE>(x2);
    x2 = _mm_mul_sd(x2, x7);
    x7 = Xorp(x7, x7);
    edx = 0x3F80;
    x2 = _mm_add_sd(x2, x1);
    x7 = Pinsrw<3>(x7, edx);
    x2 = _mm_add_sd(x2, x4);
    x4 = LoadLTab(x4, 0x88BDD0);
    x1 = MovdTo(ecx);
    x2 = MulSdArg(x2, y);
    x0 = _mm_mul_sd(x0, x7);
    x1 = Sll<0x2D>(x1);
    x1 = Pshufd<0x44>(x1);
    x7 = LdP(0x88BDC0);
    x5 = _mm_add_sd(x5, x2);
    x3 = _mm_mul_pd(x3, x1);
    x0 = _mm_add_sd(x0, x5);
    x0 = _mm_unpacklo_pd(x0, x0);
    x6 = _mm_mul_pd(x6, x0);
    x4 = _mm_mul_sd(x4, x0);
    x0 = _mm_mul_pd(x0, x0);
    x7 = _mm_add_pd(x7, x6);
    x7 = _mm_mul_pd(x7, x0);
    x0 = _mm_mul_sd(x0, x3);
    x6 = Pshufd<0xEE>(x7);
    x0 = _mm_mul_sd(x0, x7);
    x5 = Pshufd<0xEE>(x3);
    x6 = _mm_mul_sd(x6, x3);
    x4 = _mm_mul_sd(x4, x3);
    x0 = _mm_add_sd(x0, x5);
    x0 = _mm_add_sd(x0, x6);
    x0 = _mm_add_sd(x0, x4);
    x0 = _mm_add_sd(x0, x3);
    return Ret(x0);

    // ----------------------------------------------------------------------------- 0x829EE7
L9EE7:
    x1 = LoadLArg(x1, y);
    x3 = LoadLTab(x3, 0x88BDA0);
    eax = Movd(x1);
    x2 = Movsd(x2, x1);
    x1 = Andp(x1, x3);
    x1 = Srl<0x20>(x1);
    ecx = Movd(x1);
    if (ecx >= 0x7FF00000u) goto L9FF0;
    eax |= ecx;
    if (eax == 0) goto LA334;
    if (static_cast<int32_t>(edx) >= 0) goto LA02F;
    edx = 0u - edx;
    edx += 0x7FEF;
    x3 = Sll<0x34>(x3);
    x2 = Orp(x2, x3);
    ecx = 0x3F3;
    x3 = MovdTo(ecx);
    x1 = Srl<0x14>(x1);
    x1 = Psubd(x1, x3);
    x3 = D(_mm_setzero_si128());
    x1 = Pmaxsw(x1, x3);
    x2 = Psllq(x2, x1);
    x2 = Pcmpeqd(x2, x3);
    eax = Pmovmskb(x2);
    ecx = edx;
    edx &= 0x7FFF;
    if (edx >= 0x7FF0u) goto LA118;
    eax &= 0xFF;
    if (eax != 0xFF) goto LA211;
    x1 = LoadLArg(x1, y);
    x2 = LoadLArg(x2, y);
    ecx = 0x3F4;
    x3 = MovdTo(ecx);
    x1 = Andp(x1, LdP(0x88BDA0));
    x1 = Srl<0x34>(x1);
    x1 = Psubd(x1, x3);
    x3 = LoadLTab(x3, 0x88BE00);
    x2 = Psllq(x2, x1);
    x2 = Pcmpeqd(x2, x3);
    eax = Pmovmskb(x2);
    eax &= 0xFF;
    ecx = 0x3FF01;
    ecx += eax;
    ecx &= 0x40000;
    if (edx < 0x10u) goto LA034;
    edx = 0xBFE7F;
    x3 = LoadLTab(x3, 0x88BD80);
    x2 = LoadLTab(x2, 0x88BD90);
    goto L9C16;

    // ----------------------------------------------------------------------------- 0x829FF0
L9FF0:
    x7 = LoadLArg(x7, x);
    x4 = LoadLArg(x4, x);
    edx = Movd(x7);
    x7 = Srl<0x20>(x7);
    eax = Movd(x7);
    ecx = eax;
    eax &= 0x7FFFFFFF;
    if (eax < 0x7FF00000u) goto LA2AC;
    if (eax > 0x7FF00000u) goto LA1FF;
    if (edx > 0u) goto LA1FF;
    goto LA2AC;
LA02F:
    ecx = 0;
LA034:
    x0 = Xorp(x0, x0);
    eax = 0x43F0;
    x0 = Pinsrw<3>(x0, eax);
    x7 = LoadLTab(x7, 0x88BD80);
    x2 = LoadLTab(x2, 0x88BD90);
    x0 = _mm_mul_sd(x0, x4);
    edx = Movd(x4);
    x4 = Srl<0x20>(x4);
    eax = Movd(x4);
    if (edx == 0) goto LA0BA;
LA068:
    x7 = Andp(x7, x0);
    x4 = Movsd(x4, x0);
    x0 = Andp(x0, LdP(0x88BDA0));
    x0 = Srl<0x2C>(x0);
    eax = Pextrw<0>(x0);
    x7 = Orp(x7, x2);
    eax &= 0xFF; eax += 1; eax &= 0x1FE;
    x7 = _mm_mul_sd(x7, LdS(0x8884F0 + eax * 4));
    x5 = LoadLTab(x5, 0x8884F0 + eax * 4);
    eax += eax;
    x6 = LdP(0x888900 + eax * 4);
    edx = 0x43E7F;
    goto L9C16;
LA0BA:
    edx = eax;
    edx &= 0x7FFFFFFF;
    if (edx != 0) goto LA068;
    {
        uint32_t yHigh;
        std::memcpy(&yHigh, reinterpret_cast<const uint8_t*>(&y) + 4, 4); // dword [esp+0x10]
        edx = yHigh;
    }
    edx &= 0x80000000u;
    if (edx == 0) goto LA107;
    ecx <<= 13;
    eax &= ecx;
    edx = 0x7FF00000;
    edx |= eax;
    x0 = MovdTo(edx);
    x0 = Sll<0x20>(x0);
    x1 = LoadLTab(x1, 0x88BD90);
    x2 = LoadLArg(x2, x);
    x1 = _mm_div_sd(x1, x2); // solo levanta la bandera de division por cero (el resultado no se usa)
    sink = _mm_cvtsd_f64(x1);
    edx = 0x1B;
    goto LA3A1;
LA107:
    ecx <<= 13;
    eax &= ecx;
    if (eax != 0) goto LA1B9;
    return 0.0; // fldz
LA118:
    x3 = LoadLTab(x3, 0x88BD80);
    x1 = Xorp(x1, x1);
    x3 = Andp(x3, x4);
    x1 = Pcmpeqd(x1, x3);
    ecx = Pmovmskb(x1);
    ecx &= 0xFF;
    if (ecx != 0xFF) goto LA1FF;
    ecx = Pextrw<3>(x4);
    ecx &= 0x8000;
    if (ecx == 0) goto LA1E3;
    eax &= 0xFF;
    if (eax != 0xFF) goto LA1C7;
    x1 = LoadLArg(x1, y);
    x2 = LoadLArg(x2, y);
    ecx = 0x3F4;
    x3 = MovdTo(ecx);
    x1 = Andp(x1, LdP(0x88BDA0));
    x1 = Srl<0x34>(x1);
    x1 = Psubd(x1, x3);
    x3 = D(_mm_setzero_si128());
    x2 = Psllq(x2, x1);
    x2 = Pcmpeqd(x2, x3);
    eax = Pmovmskb(x2);
    eax &= 0xFF;
    if (eax == 0xFF) goto LA1C7;
    x1 = LoadLArg(x1, y);
    eax = Pextrw<3>(x1);
    eax &= 0x8000;
    if (eax == 0) goto LA1C0;
LA1B9:
    return Ret(LdS(0x88BE00)); // -0.0
LA1C0:
    return Ret(LdS(0x88BDF8)); // -inf
LA1C7:
    x1 = LoadLArg(x1, y);
    eax = Pextrw<3>(x1);
    eax &= 0x8000;
    if (eax == 0) goto LA31E;
    return 0.0; // fldz
LA1E3:
    x1 = LoadLArg(x1, y);
    eax = Pextrw<3>(x1);
    eax &= 0x8000;
    if (eax == 0) goto LA31E;
    return 0.0; // fldz
LA1FF:
    x4 = _mm_add_sd(x4, x4);
    x0 = Movsd(x0, x4);
    edx = 0x3EE;
    goto LA3A1;
LA211:
    x2 = LoadLArg(x2, x);
    eax = Movd(x2);
    x2 = Srl<0x20>(x2);
    edx = Movd(x2);
    edx &= 0x7FFFFFFF;
    eax |= edx;
    ecx = 0;
    if (eax == 0) goto LA0BA;
    x1 = LoadLTab(x1, 0x88BDE8);
    x0 = LoadLTab(x0, 0x88BE10);
    x1 = _mm_mul_sd(x1, x1); // el SNaN de 0x88BDE8 levanta la bandera de operacion invalida (resultado no usado)
    sink = _mm_cvtsd_f64(x1);
    edx = 0x1C;
    goto LA3A1;
LA258:
    x4 = LoadLArg(x4, x);
    x2 = LoadLArg(x2, y);
    eax = Movd(x4);
    if (eax != 0) goto LA2AC;
    x4 = Srl<0x20>(x4);
    edx = Movd(x4);
    if (edx == 0x3FF00000u) goto LA38A;
    if (edx != 0xBFF00000u) goto LA2AC;
    x3 = LoadLTab(x3, 0x88BD80);
    x1 = Xorp(x1, x1);
    x3 = Andp(x3, x2);
    x1 = Pcmpeqd(x1, x3);
    eax = Pmovmskb(x1);
    if (eax != 0xFF) goto LA325;
    return 1.0; // fld1
LA2AC:
    x3 = LoadLTab(x3, 0x88BD80);
    x1 = Xorp(x1, x1);
    x3 = Andp(x3, x2);
    x1 = Pcmpeqd(x1, x3);
    eax = Pmovmskb(x1);
    eax &= 0xFF;
    if (eax != 0xFF) goto LA325;
    eax = Pextrw<3>(x2);
    x4 = LoadLArg(x4, x);
    eax &= 0x8000;
    ecx ^= 0xBFF00000u;
    edx |= ecx;
    if (edx == 0) goto LA38A;
    if (eax == 0) goto LA30A;
    eax = Pextrw<3>(x4);
    eax &= 0x7FF0;
    if (eax < 0x3FF0u) goto LA31E;
    return 0.0; // fldz
LA30A:
    eax = Pextrw<3>(x4);
    eax &= 0x7FF0;
    if (eax >= 0x3FF0u) goto LA31E;
    return 0.0; // fldz
LA31E:
    return Ret(LdS(0x88BDF0)); // +inf
LA325:
    x2 = _mm_add_sd(x2, x2);
    x0 = Movsd(x0, x2);
    edx = 0x3EE;
    goto LA3A1;
LA334:
    eax = Movd(x4);
    x4 = Srl<0x20>(x4);
    edx = Movd(x4);
    edx &= 0x7FFFFFFF;
    ecx = eax;
    eax |= edx;
    x0 = LoadLTab(x0, 0x88BD90);
    edx = 0x1A;
    if (eax == 0) goto LA3A1;
    eax = Movd(x4);
    edx = 0x1D;
    eax &= 0x7FFFFFFF;
    if (eax > 0x7FF00000u) goto LA3A1;
    if (eax < 0x7FF00000u) goto LA379;
    if (ecx > 0u) goto LA3A1;
LA379:
    return Ret(x0);
LA38A:
    x1 = LoadLTab(x1, 0x88BE10);
    x0 = LoadLArg(x0, y);
    x0 = _mm_add_sd(x0, x1);
    edx = 0x1C;
LA3A1:
    return RaiseError(x, y, x0, edx);

    // ----------------------------------------------------------------------------- 0x82A3D1
LA3D1:
    if (static_cast<int32_t>(eax) <= 0) goto LA3FE;
    if (eax >= 0x40000u) goto LA61D;
    edx = eax;
    eax &= 0x7F;
    ecx += 0x3FF00;
    edx -= 0x80;
    edx &= 0xFFFFFF80u;
    edi = 0x3FF0;
    goto LA424;
LA3FE:
    if (static_cast<int32_t>(eax) <= static_cast<int32_t>(0xFFFC0200u)) goto LA5FA;
    edx = eax;
    eax &= 0x7F;
    ecx += 0x80;
    edx &= 0xFFFFFF80u;
    edx += 0x3FE80;
    edi = 0;
LA424:
    x4 = _mm_add_sd(x4, x0);
    x0 = Movsd(x0, x3);
    x3 = _mm_add_sd(x3, LdS(0x88BD70));
    esi = edx;
    esi -= 0x1FF80;
    x2 = _mm_mul_pd(x2, x7);
    x3 = _mm_sub_sd(x3, LdS(0x88BD70));
    x7 = _mm_mul_pd(x7, x7);
    x0 = _mm_sub_sd(x0, x3);
    eax *= 16;
    x3 = LdP(0x88B570 + eax);
    x6 = LdP(0x88BDB0);
    x1 = _mm_mul_pd(x1, x7);
    x2 = _mm_add_pd(x2, x1);
    x1 = Pshufd<0xEE>(x2);
    x2 = _mm_mul_sd(x2, x7);
    x2 = _mm_add_sd(x2, x1);
    x2 = _mm_add_sd(x2, x4);
    x4 = LoadLTab(x4, 0x88BDD0);
    x1 = MovdTo(edx);
    edx -= 0x1FF80;
    edx = 0u - edx;
    edx = static_cast<uint32_t>(static_cast<int32_t>(edx) >> 7);
    edx += 2;
    eax = edx;
    eax &= 0x20;
    edx += eax;
    x7 = Xorp(x7, x7);
    eax = 0x3F80;
    x7 = Pinsrw<3>(x7, eax);
    x2 = MulSdArg(x2, y);
    x0 = _mm_mul_sd(x0, x7);
    x1 = Sll<0x2D>(x1);
    x1 = Pshufd<0x44>(x1);
    x7 = LdP(0x88BDC0);
    x5 = _mm_add_sd(x5, x2);
    x3 = _mm_mul_pd(x3, x1);
    x0 = _mm_add_sd(x0, x5);
    x0 = _mm_unpacklo_pd(x0, x0);
    x6 = _mm_mul_pd(x6, x0);
    x4 = _mm_mul_sd(x4, x0);
    x0 = _mm_mul_pd(x0, x0);
    x7 = _mm_add_pd(x7, x6);
    x7 = _mm_mul_pd(x7, x0);
    x0 = _mm_mul_sd(x0, x3);
    x6 = Pshufd<0xEE>(x7);
    x0 = _mm_mul_sd(x0, x7);
    x5 = Pshufd<0xEE>(x3);
    x6 = _mm_mul_sd(x6, x3);
    x4 = _mm_mul_sd(x4, x3);
    x7 = MovdTo(ecx);
    x7 = Sll<0x2D>(x7);
    x2 = MovdTo(edx);
    x1 = Pcmpeqd(x1, x1);
    x1 = Psllq(x1, x2);
    x0 = _mm_add_sd(x0, x5);
    x0 = _mm_add_sd(x0, x6);
    x1 = Andp(x1, x3);
    x0 = _mm_add_sd(x0, x4);
    x6 = Xorp(x6, x6);
    x4 = Pcmpeqd(x4, x4);
    x4 = Psllq(x4, x2);
    x3 = _mm_sub_sd(x3, x1);
    x2 = Movsd(x2, x1);
    x1 = _mm_add_sd(x1, x0);
    x1 = Andp(x1, x4);
    x6 = Pinsrw<3>(x6, edi);
    x2 = _mm_sub_sd(x2, x1);
    x0 = _mm_add_sd(x0, x2);
    x0 = _mm_add_sd(x0, x3);
    if (static_cast<int32_t>(esi) > 0) goto LA5B0;
    x0 = _mm_mul_sd(x0, x7);
    x1 = _mm_mul_sd(x1, x7);
    x0 = _mm_add_sd(x0, x1);
    x6 = _mm_mul_sd(x6, x0);
    x0 = _mm_add_sd(x0, x6);
    goto LA577;
LA5B0:
    x0 = _mm_add_sd(x0, x1);
    x0 = _mm_mul_sd(x0, x7);
    x6 = _mm_mul_sd(x6, x0);
    x0 = _mm_add_sd(x0, x6);
LA577:
    eax = Pextrw<3>(x0);
    eax &= 0x7FF0;
    edx = 0x18;
    if (eax == 0x7FF0u) goto LA3A1;
    edx = 0x19;
    if (eax == 0) goto LA3A1;
    return Ret(x0);
LA5FA:
    x0 = LoadLTab(x0, 0x88BE08);
    x1 = MovdTo(ecx);
    x0 = _mm_mul_sd(x0, x0);
    x1 = Sll<0x2D>(x1);
    x0 = Orp(x0, x1);
    edx = 0x19;
    goto LA3A1;
LA61D:
    edx = 0x18;
    if (ecx == 0) goto LA634;
    x0 = LoadLTab(x0, 0x88BDF8);
    goto LA3A1;
LA634:
    x0 = LoadLTab(x0, 0x88BDF0);
    goto LA3A1;
LA641:
    x4 = Pshufd<0x44>(x1);
    x4 = _mm_mul_pd(x4, x6);
    eax = Pextrw<3>(x4);
    eax &= 0x7FF0;
    edx = 0x40A0;
    edx -= eax;
    eax -= 0x3C70;
    edx |= eax;
    if (edx < 0x80000000u) goto L9D68;
    if (eax < 0x80000000u) goto LA695;
    ecx |= 0x1FF80;
    x0 = MovdTo(ecx);
    x0 = Sll<0x2D>(x0);
    return Ret(x0);
LA695:
    x4 = LoadLArg(x4, x);
    edx = Pextrw<3>(x4);
    edx &= 0x7FF0;
    edx -= 0x3FF0;
    eax = Pextrw<3>(x1);
    eax ^= edx;
    eax &= 0x8000;
    if (eax != 0) goto LA5FA;
    goto LA61D;
}

// 0x8220F0: si el entorno es el del juego (0xC9C400 != 0 y excepciones enmascaradas) va al nucleo SSE2 de arriba; si no, a la rama x87 (0x822184, un cuerpo con su propio `ret`
// al que el EXE entra con la misma pila, asi que se invoca como funcion cdecl(double, double))
double __cdecl Pow(double x, double y) {
    if (UseSse2MathPath()) {
        return PowSse2(x, y);
    }
    return reinterpret_cast<double(__cdecl*)(double, double)>(0x822184)(x, y);
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtPowInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Pow, 0x8220F0);
    RH_ScopedGlobalInstall(ExeCrt::PowSse2, 0x829B99);
}
#endif
