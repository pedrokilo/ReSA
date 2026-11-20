#pragma once

#include <xmmintrin.h>

// Lote EXT1A: D3DXMatrixMultiplyTranspose del D3DX9 enlazado en el EXE (0x7676C7 = `jmp [0x8D74A0]`), variantes SSE2 (0x7BA139) y SSE (0x7B7FF3), que tienen exactamente la misma secuencia de
// instrucciones (el selector 0x781A87 elige una u otra segun la CPU; no hay diferencia observable). La DLL del sistema (d3dx9_43) redondea distinto (1-2 ulps), asi que esta funcion se escribe aqui.
// Con A = primer operando y B = segundo (4x4, filas contiguas), P = A * B; la salida es la transpuesta de P: out[4*j + i] = P[i][j]. Las filas de B se cargan enteras (movlps/movhps) y la fila i de P es
// ((A[i][0] * B_fila0 + A[i][1] * B_fila1) + A[i][2] * B_fila2) + A[i][3] * B_fila3 (mulps con el escalar repartido por shufps y tres addps en ese orden). Se lee todo antes de escribir: out puede coincidir con A o con B.
// La variante x87 de reserva (0x7ACECB, para CPU sin SSE) y la 3DNow! no se reproducen: ninguna CPU actual las elige.
namespace D3DX9MatrixMultiplyTransposeSseDetail {
// Las instrucciones addps/mulps del EXE tienen el operando destino fijo (el primero); el compilador puede intercambiar los operandos de una suma o un producto, lo que cambiaria la
// carga util y el signo del NaN que se propaga. Se fija la regla de x86 explicitamente: si el primer operando es NaN gana (convertido a silencioso), si no, el segundo; sin NaN de entrada el
// resultado del hardware ya es el correcto (incluido el NaN indefinido de inf-inf o 0*inf).
inline __m128 FirstNaNWins(__m128 x, __m128 y, __m128 hardware) {
    const __m128 quietBit = _mm_castsi128_ps(_mm_set1_epi32(0x00400000));
    const __m128 nanX = _mm_cmpunord_ps(x, x);
    const __m128 nanY = _mm_cmpunord_ps(y, y);
    const __m128 fromY = _mm_or_ps(_mm_and_ps(nanY, _mm_or_ps(y, quietBit)), _mm_andnot_ps(nanY, hardware));
    return _mm_or_ps(_mm_and_ps(nanX, _mm_or_ps(x, quietBit)), _mm_andnot_ps(nanX, fromY));
}
inline __m128 Add(__m128 x, __m128 y) { return FirstNaNWins(x, y, _mm_add_ps(x, y)); }
inline __m128 Mul(__m128 x, __m128 y) { return FirstNaNWins(x, y, _mm_mul_ps(x, y)); }
} // namespace D3DX9MatrixMultiplyTransposeSseDetail

inline float* D3DX9MatrixMultiplyTransposeSse(float* out, const float* a, const float* b) {
    using namespace D3DX9MatrixMultiplyTransposeSseDetail;
    const __m128 b0 = _mm_loadu_ps(b);
    const __m128 b1 = _mm_loadu_ps(b + 4);
    const __m128 b2 = _mm_loadu_ps(b + 8);
    const __m128 b3 = _mm_loadu_ps(b + 12);
    __m128 p[4];
    for (int i = 0; i < 4; ++i) {
        const __m128 t0 = Mul(_mm_set1_ps(a[4 * i + 0]), b0);
        const __m128 t1 = Mul(_mm_set1_ps(a[4 * i + 1]), b1);
        const __m128 t2 = Mul(_mm_set1_ps(a[4 * i + 2]), b2);
        const __m128 t3 = Mul(_mm_set1_ps(a[4 * i + 3]), b3);
        p[i] = Add(Add(Add(t0, t1), t2), t3);
    }
    _MM_TRANSPOSE4_PS(p[0], p[1], p[2], p[3]);
    _mm_storeu_ps(out, p[0]);
    _mm_storeu_ps(out + 4, p[1]);
    _mm_storeu_ps(out + 8, p[2]);
    _mm_storeu_ps(out + 12, p[3]);
    return out;
}
