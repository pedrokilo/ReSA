// FnD3DX9C: ver D3DX9NativeMath.h. SSE2 con intrinsecos (sin __asm). Mismo orden de operaciones que las variantes SSE/SSE2 de D3DX9 (y del EXE).
#include <emmintrin.h>
#include <xmmintrin.h>
#include <cstring>

#include "D3DX9NativeMath.h"

namespace D3DX9Native {
namespace {
// addps/mulps tienen el operando destino fijo: si el primero es NaN gana (silenciado), si no, el segundo; sin NaN de entrada vale el resultado del hardware.
inline __m128 FirstNaNWins(__m128 x, __m128 y, __m128 hardware) {
    const __m128 quietBit = _mm_castsi128_ps(_mm_set1_epi32(0x00400000));
    const __m128 nanX = _mm_cmpunord_ps(x, x);
    const __m128 nanY = _mm_cmpunord_ps(y, y);
    const __m128 fromY = _mm_or_ps(_mm_and_ps(nanY, _mm_or_ps(y, quietBit)), _mm_andnot_ps(nanY, hardware));
    return _mm_or_ps(_mm_and_ps(nanX, _mm_or_ps(x, quietBit)), _mm_andnot_ps(nanX, fromY));
}
inline __m128 Add(__m128 x, __m128 y) { return FirstNaNWins(x, y, _mm_add_ps(x, y)); }
inline __m128 Mul(__m128 x, __m128 y) { return FirstNaNWins(x, y, _mm_mul_ps(x, y)); }
inline __m128 Sub(__m128 x, __m128 y) { return FirstNaNWins(x, y, _mm_sub_ps(x, y)); }
inline __m128 Splat(float f) { return _mm_set1_ps(f); }

// fila `r` de la matriz como vector
inline __m128 Row(const float* m, int r) { return _mm_loadu_ps(m + 4 * r); }
} // namespace

// D3DXFloat16To32Array de d3dx9_43.dll: NO trata el exponente 31 como infinito/NaN (lo convierte como un numero normal, p. ej. 0x7C00 -> 65536.0f). Valor = (signo) * m * 2^(max(E,1) - 25),
// con E el exponente de 5 bits y m = mantisa (con el bit oculto 0x400 solo si E != 0): exacto en float, sin redondeo. El camino por bloques de 8 de la DLL (SSE2) da los mismos bits (banco exhaustivo).
float* Float16To32Array(float* out, const uint16_t* in, uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t h = in[i];
        const uint32_t e = h & 0x7C00u;
        const uint32_t m = (h & 0x3FFu) | (e ? 0x400u : 0u);
        const uint32_t scaleBits = (((e > 0x400u ? e : 0x400u) + 0x19800u) << 13) | ((h & 0x8000u) << 16);
        float scale;
        std::memcpy(&scale, &scaleBits, 4);
        out[i] = static_cast<float>(m) * scale;
    }
    return out;
}

// d3dx9_43.dll, variante SSE: (y*fila1 + x*fila0) + z*fila2 (mulps con el vector repartido como operando destino).
void* Vec3TransformNormal(void* out, const void* v, const void* matrix) {
    const float* pv = static_cast<const float*>(v);
    const float* m = static_cast<const float*>(matrix);
    const __m128 vx = Splat(pv[0]), vy = Splat(pv[1]), vz = Splat(pv[2]);
    __m128 r = Add(Mul(vy, Row(m, 1)), Mul(vx, Row(m, 0)));
    r = Add(r, Mul(vz, Row(m, 2)));
    alignas(16) float t[4];
    _mm_store_ps(t, r);
    float* o = static_cast<float*>(out);
    o[0] = t[0]; o[1] = t[1]; o[2] = t[2];
    return out;
}

// d3dx9_43.dll, variante SSE: (y*fila1 + x*fila0) + (z*fila2 + fila3); luego 1/w con rcpps y una iteracion de Newton: r' = (r + r) - (r * w) * r, y xyz *= r'.
void* Vec3TransformCoord(void* out, const void* v, const void* matrix) {
    const float* pv = static_cast<const float*>(v);
    const float* m = static_cast<const float*>(matrix);
    const __m128 vx = Splat(pv[0]), vy = Splat(pv[1]), vz = Splat(pv[2]);
    __m128 a = Add(Mul(vy, Row(m, 1)), Mul(vx, Row(m, 0)));
    const __m128 b = Add(Mul(vz, Row(m, 2)), Row(m, 3));
    const __m128 x = Add(a, b);
    __m128 r = _mm_rcp_ps(x);
    const __m128 t0 = Mul(Mul(r, x), r);
    r = Sub(Add(r, r), t0);
    r = _mm_shuffle_ps(r, r, 0xFF);
    r = Mul(r, x);
    alignas(16) float t[4];
    _mm_store_ps(t, r);
    float* o = static_cast<float*>(out);
    o[0] = t[0]; o[1] = t[1]; o[2] = t[2];
    return out;
}

// d3dx9_43.dll, variante SSE: filas 0 y 3 de A suman por parejas ((t0+t1)+(t2+t3)); filas 1 y 2 en cadena (((t0+t1)+t2)+t3). Para la fila 3 el compilador de la DLL deja la fila de B como
// operando destino de mulps (importa solo para la carga util de un NaN). Todo se lee antes de escribir.
void* MatrixMultiply(void* out, const void* a, const void* b) {
    const float* pa = static_cast<const float*>(a);
    const float* pb = static_cast<const float*>(b);
    const __m128 b0 = Row(pb, 0), b1 = Row(pb, 1), b2 = Row(pb, 2), b3 = Row(pb, 3);
    __m128 p[4];
    p[0] = Add(Add(Mul(Splat(pa[0]), b0), Mul(Splat(pa[1]), b1)), Add(Mul(Splat(pa[2]), b2), Mul(Splat(pa[3]), b3)));
    p[1] = Add(Add(Add(Mul(Splat(pa[4]), b0), Mul(Splat(pa[5]), b1)), Mul(Splat(pa[6]), b2)), Mul(Splat(pa[7]), b3));
    p[2] = Add(Add(Add(Mul(Splat(pa[8]), b0), Mul(Splat(pa[9]), b1)), Mul(Splat(pa[10]), b2)), Mul(Splat(pa[11]), b3));
    p[3] = Add(Add(Mul(b0, Splat(pa[12])), Mul(b1, Splat(pa[13]))), Add(Mul(b2, Splat(pa[14])), Mul(b3, Splat(pa[15]))));
    float* o = static_cast<float*>(out);
    for (int i = 0; i < 4; ++i) _mm_storeu_ps(o + 4 * i, p[i]);
    return out;
}

void* MatrixTranspose(void* out, const void* in) {
    __m128 r0 = Row(static_cast<const float*>(in), 0), r1 = Row(static_cast<const float*>(in), 1), r2 = Row(static_cast<const float*>(in), 2), r3 = Row(static_cast<const float*>(in), 3);
    _MM_TRANSPOSE4_PS(r0, r1, r2, r3);
    float* o = static_cast<float*>(out);
    _mm_storeu_ps(o, r0); _mm_storeu_ps(o + 4, r1); _mm_storeu_ps(o + 8, r2); _mm_storeu_ps(o + 12, r3);
    return out;
}
} // namespace D3DX9Native
