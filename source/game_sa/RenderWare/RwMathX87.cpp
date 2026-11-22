/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RwMathX87
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifndef RWMATHX87_BENCH
#include "StdInc.h"
#endif
#include <xmmintrin.h>
#include <float.h>
#include <stdint.h>
#include "RwMathX87.h"

// Lote S3 (codigo vivo, parte P3). Ver RwMathX87.h.
//
// Aritmetica: el EXE usa la x87 con la precision de 24 bits (0x857404), donde cada fmul/fadd redondea su resultado a float: igual que una
// operacion escalar SSE de un float (un solo redondeo, y la misma suma conmutativa). Se usan intrinsecos y no el operador de C++ para que el
// orden de las operaciones no dependa del modelo de coma flotante (/fp:fast) ni de si el compilador usa la x87 o SSE.
// La unica diferencia posible es en el rango subnormal / desbordado (la x87 lleva exponente extendido hasta el fstp final).
namespace RwMathX87 {
namespace {
inline float Mul(float a, float b) { return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(a), _mm_set_ss(b))); }
inline float Add(float a, float b) { return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a), _mm_set_ss(b))); }

// Elementos de RwMatrix por posicion (dwords): right = 0..2, up = 4..6, at = 8..10, pos = 12..14 (flags y relleno en 3, 7, 11, 15).
inline const float* M(const RwMatrix* matrix) { return reinterpret_cast<const float*>(matrix); }

// Un punto (traslacion = true) o un vector con la matriz: ((m0j * x + m1j * y) + m2j * z) [+ m3j]. Lee la entrada completa antes de escribir (la salida puede ser la entrada).
inline void Transform(float* out, const float* in, const float* m, bool translate) {
    const float x = in[0], y = in[1], z = in[2];
    float r[3];
    for (int j = 0; j < 3; ++j) {
        float v = Add(Add(Mul(m[j], x), Mul(m[4 + j], y)), Mul(m[8 + j], z));
        if (translate) {
            v = Add(v, m[12 + j]);
        }
        r[j] = v;
    }
    out[0] = r[0];
    out[1] = r[1];
    out[2] = r[2];
}

inline RwV3d* TransformArray(RwV3d* out, const RwV3d* in, int32 count, const RwMatrix* matrix, bool translate) {
    float*       dst = reinterpret_cast<float*>(out);
    const float* src = reinterpret_cast<const float*>(in);
    // Como el EXE: bucle `dec ecx; jne` (con cuenta 0 daria 2^32 vueltas), un punto por iteracion.
    uint32_t remaining = static_cast<uint32_t>(count);
    do {
        Transform(dst, src, M(matrix), translate);
        src += 3;
        dst += 3;
    } while (--remaining != 0);
    return const_cast<RwV3d*>(in);
}

// Palabra de control x87 "cruda" (la de fnstcw) a partir de la vista del CRT (_controlfp_s) y al reves. Bits crudos: 0 IM, 1 DM, 2 ZM, 3 OM, 4 UM, 5 PM (excepciones enmascaradas
// si valen 1), 6 reservado (siempre 1), 8-9 control de precision (00 = 24 bits, 10 = 53, 11 = 64), 10-11 redondeo, 12 infinito.
constexpr uint32_t kCrtMaskAll = _MCW_EM | _MCW_RC | _MCW_PC | _MCW_IC;
uint32_t ReadRawControlWord() {
    unsigned int crt = 0;
    _controlfp_s(&crt, 0, 0);
    uint32_t raw = 0x40;
    if (crt & _EM_INVALID)    raw |= 0x01;
    if (crt & _EM_DENORMAL)   raw |= 0x02;
    if (crt & _EM_ZERODIVIDE) raw |= 0x04;
    if (crt & _EM_OVERFLOW)   raw |= 0x08;
    if (crt & _EM_UNDERFLOW)  raw |= 0x10;
    if (crt & _EM_INEXACT)    raw |= 0x20;
    raw |= (crt & _MCW_RC) << 2;
    switch (crt & _MCW_PC) {
    case _PC_24: break;
    case _PC_53: raw |= 0x200; break;
    default:     raw |= 0x300; break;
    }
    if (crt & _MCW_IC) {
        raw |= 0x1000;
    }
    return raw;
}
void WriteRawControlWord(uint32_t raw) {
    unsigned int crt = 0;
    if (raw & 0x01) crt |= _EM_INVALID;
    if (raw & 0x02) crt |= _EM_DENORMAL;
    if (raw & 0x04) crt |= _EM_ZERODIVIDE;
    if (raw & 0x08) crt |= _EM_OVERFLOW;
    if (raw & 0x10) crt |= _EM_UNDERFLOW;
    if (raw & 0x20) crt |= _EM_INEXACT;
    crt |= (raw >> 2) & _MCW_RC;
    switch ((raw >> 8) & 3u) {
    case 0:  crt |= _PC_24; break;
    case 2:  crt |= _PC_53; break;
    default: crt |= _PC_64; break; // 1 es reservado: la x87 lo trata como 64 bits
    }
    if (raw & 0x1000) {
        crt |= _MCW_IC;
    }
    // _controlfp_s toca tambien MXCSR: el EXE (fldcw) solo cambia la x87.
    const unsigned int csr = _mm_getcsr();
    unsigned int current = 0;
    _controlfp_s(&current, crt, kCrtMaskAll);
    _mm_setcsr(csr);
}
inline uint32_t& ControlWordSlot() { return *reinterpret_cast<uint32_t*>(0xCAF000); }
inline uint32_t& SavedControlWordSlot() { return *reinterpret_cast<uint32_t*>(0xCAF004); }
} // namespace

// 0x857000 (cdecl, (salida, entrada, cuenta, matriz); `ret`, 127 bytes; devuelve la entrada): transforma `cuenta` puntos con traslacion.
RwV3d* __cdecl TransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, int32 numPoints, const RwMatrix* matrix) {
    return TransformArray(pointsOut, pointsIn, numPoints, matrix, true);
}

// 0x85707F (cdecl, (salida, entrada, cuenta, matriz); `ret`, 112 bytes; devuelve la entrada): transforma `cuenta` vectores sin traslacion.
RwV3d* __cdecl TransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, int32 numVectors, const RwMatrix* matrix) {
    return TransformArray(vectorsOut, vectorsIn, numVectors, matrix, false);
}

// 0x8570EF (cdecl, (salida, entrada, matriz); `ret`, 113 bytes; devuelve la entrada): un punto con traslacion.
RwV3d* __cdecl TransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix) {
    Transform(reinterpret_cast<float*>(pointOut), reinterpret_cast<const float*>(pointIn), M(matrix), true);
    return const_cast<RwV3d*>(pointIn);
}

// 0x857160 (cdecl, (salida, entrada, matriz); `ret`, 98 bytes; devuelve la entrada): un vector sin traslacion.
RwV3d* __cdecl TransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix) {
    Transform(reinterpret_cast<float*>(vectorOut), reinterpret_cast<const float*>(vectorIn), M(matrix), false);
    return const_cast<RwV3d*>(vectorIn);
}

// 0x8572A0 (cdecl, (destino, A, B); `ret`, 355 bytes; sin valor de retorno; instalada SIEMPRE como _rwMatrixMultFn): destino = A * B. Por filas de A: columna j = ((x * b0j + y * b1j) + z * b2j) [+ b3j en la fila de traslacion].
// Las filas se leen de A antes de escribir la fila del destino y B se relee en cada fila, asi que destino == A funciona (destino == B daria el mismo resultado que en el EXE). No escribe los dwords 3, 7, 11 y 15 (banderas y relleno).
void __cdecl MatrixMultiply(RwMatrix* dst, const RwMatrix* a, const RwMatrix* b) {
    float*       d  = reinterpret_cast<float*>(dst);
    const float* ma = M(a);
    const float* mb = M(b);
    for (int row = 0; row < 4; ++row) {
        const float x = ma[row * 4], y = ma[row * 4 + 1], z = ma[row * 4 + 2];
        float r[3];
        for (int j = 0; j < 3; ++j) {
            float v = Add(Add(Mul(mb[j], x), Mul(mb[4 + j], y)), Mul(mb[8 + j], z));
            if (row == 3) {
                v = Add(v, mb[12 + j]);
            }
            r[j] = v;
        }
        d[row * 4]     = r[0];
        d[row * 4 + 1] = r[1];
        d[row * 4 + 2] = r[2];
    }
}

// 0x857404 (cdecl sin argumentos; `ret`, 39 bytes): fnstcw -> [0xCAF000] (solo la palabra baja del dword), copia el dword a [0xCAF004], precision de 24 bits (bits 8-9 a 0) y las seis excepciones enmascaradas (| 0x3F) en [0xCAF000], y fldcw.
void __cdecl SetFpuControlWord() {
    uint32_t value = (ControlWordSlot() & 0xFFFF0000u) | ReadRawControlWord();
    SavedControlWordSlot() = value;
    value = (value & 0xFFFFFCFFu) | 0x3Fu;
    ControlWordSlot() = value;
    WriteRawControlWord(value & 0xFFFFu);
}

// 0x85742B (cdecl sin argumentos; `fldcw [0xCAF004]; ret`): restaura la palabra de control guardada por 0x857404.
void __cdecl RestoreFpuControlWord() {
    WriteRawControlWord(SavedControlWordSlot() & 0xFFFFu);
}
} // namespace RwMathX87

#ifndef RWMATHX87_BENCH
void RwMathX87InjectHooks() {
    RH_ScopedCategory("RenderWare");
    RH_ScopedNamespaceName("RwMathX87");

    RH_ScopedGlobalInstall(RwMathX87::TransformPoints, 0x857000);
    RH_ScopedGlobalInstall(RwMathX87::TransformVectors, 0x85707F);
    RH_ScopedGlobalInstall(RwMathX87::TransformPoint, 0x8570EF);
    RH_ScopedGlobalInstall(RwMathX87::TransformVector, 0x857160);
    RH_ScopedGlobalInstall(RwMathX87::MatrixMultiply, 0x8572A0);
    RH_ScopedGlobalInstall(RwMathX87::SetFpuControlWord, 0x857404);
    RH_ScopedGlobalInstall(RwMathX87::RestoreFpuControlWord, 0x85742B);
}
#endif
