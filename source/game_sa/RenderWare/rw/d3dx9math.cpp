/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3DX9
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <emmintrin.h>
#include <xmmintrin.h>

#include "d3dx9math.h"

// Lote R4A (sin validar). Funciones matematicas de D3DX9 enlazadas en el EXE (0x7C1D80..0x7C3A40). Cada cuerpo se transcribio del desensamblado del EXE.
// Las variantes SSE se escriben con intrinsecos _mm_* (misma operacion y mismo orden que el EXE, sin __asm); las 3DNow!/MMX no tienen intrinsecos en MSVC y se
// escriben con aritmetica escalar de 32 bits.

namespace {
inline __m128 Splat(uint32 bits) {
    return _mm_castsi128_ps(_mm_set1_epi32(static_cast<int>(bits)));
}

// Constantes de .rdata del EXE (0x882940..0x882A30 y 0x8DE810..0x8DEB50), copiadas por valor.
constexpr uint32 kAbsMask  = 0x7FFFFFFF;
constexpr uint32 kSignMask = 0x80000000;
constexpr uint32 kHalf     = 0x3F000000;
constexpr uint32 kOne      = 0x3F800000;
constexpr uint32 kHalfPi   = 0x3FC90FDB;
constexpr uint32 kPi       = 0x40490FDB;
constexpr uint32 kAcosC1   = 0x3E2AAAF8; // 0x8829F0 (y lane 1 de 0x882940)
constexpr uint32 kAcosC2   = 0x3D997DCC; // 0x882A00 (y lane 0 de 0x882950)
constexpr uint32 kAcosC3   = 0x3D3A73D8; // 0x882A10 (y lane 1 de 0x882950)
constexpr uint32 kAcosC5   = 0x3CC48F19; // 0x882A20 (y lane 0 de 0x882960)
constexpr uint32 kAcosC4   = 0x3D2DBDCF; // 0x882A30 (y lane 1 de 0x882960)

// Vectores de dos constantes repetidos (0x882940, 0x882950, 0x882960): lanes (0,1,2,3) = (a, b, a, b). _mm_set_ps recibe de la lane 3 a la 0.
inline __m128 Pair(uint32 lane0, uint32 lane1) {
    return _mm_castsi128_ps(_mm_set_epi32(static_cast<int>(lane1), static_cast<int>(lane0), static_cast<int>(lane1), static_cast<int>(lane0)));
}
} // namespace

void D3DX9InjectHooks() {
    RH_ScopedNamespaceName("D3DX9");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(D3DXFloat16To32Array_3DNow, 0x7C1D80);
    RH_ScopedGlobalInstall(D3DXVec2Transform_3DNow, 0x7C203B);
    RH_ScopedGlobalInstall(D3DXVec2TransformNormal_3DNow, 0x7C2090);
    RH_ScopedGlobalInstall(D3DX9AcosPS, 0x7C2AAE);
    RH_ScopedGlobalInstall(D3DX9TwoAcosScalar, 0x7C2CA4);
    RH_ScopedGlobalInstall(D3DX9SinCosPS, 0x7C307E);
    RH_ScopedGlobalInstall(D3DX9SinCosPS_SSE2, 0x7C381F);

    // lote R21A (sin validar): ensamblador y compilador de shaders
    RH_ScopedGlobalInstall(D3DXAssembleShader, 0x7652AE);
    RH_ScopedGlobalInstall(D3DXCompileShaderFromFileA, 0x76534D);
    RH_ScopedGlobalInstall(D3DXCompileShaderFromFileW, 0x765402);
    RH_ScopedGlobalInstall(D3DXCompileShaderFromResourceA, 0x7654B8);
}

// 0x7C1D80: medios flotantes -> floats con MMX/3DNow! (`femms` al entrar y salir, __stdcall: `ret 0Ch`). Con count < 4 llama al conversor escalar 0x76626B
// (D3DXFloat16To32Array escalar, __stdcall con los mismos tres argumentos; con count == 0 no llama a nada). Con count >= 4 convierte de cuatro en cuatro; si sobran 1..3
// vuelve a convertir los ultimos cuatro elementos (se solapa con lo ya escrito: mismo resultado). Por elemento w = medio flotante de 16 bits ampliado a 32:
//   z = (w & 0x7C00) == 0 (mascara completa); s = w & 0x8000; m = (w ^ s) + 0x1C000 + (z & 0x400);
//   f = float(m << 13) - float(z & 0x38800000)   (pfsub: exacto, la resta de dos normales cercanos);  resultado = bits(f) | (s << 16).
// Los infinitos y NaN (exponente 31) no se tratan aparte: salen como 65536.0f con mantisa desplazada, como en el EXE.
float* __stdcall D3DXFloat16To32Array_3DNow(float* out, const uint16* in, uint32 count) {
    const int32 n = static_cast<int32>(count); // `cmp ecx,4 / jge`: comparaciones con signo
    if (n < 4) {
        if (n != 0) {
            reinterpret_cast<float*(__stdcall*)(float*, const uint16*, uint32)>(0x76626B)(out, in, count);
        }
        return out;
    }

    const auto convert4 = [](float* dst, const uint16* src) {
        for (int i = 0; i < 4; ++i) {
            const uint32 w = src[i];
            const uint32 z = ((w & 0x7C00u) == 0u) ? 0xFFFFFFFFu : 0u;
            const uint32 s = w & 0x8000u;
            const uint32 m = (w ^ s) + 0x1C000u + (z & 0x400u);
            const uint32 aBits = m << 13;
            const uint32 bBits = z & 0x38800000u;
            float a, b;
            memcpy(&a, &aBits, sizeof(a));
            memcpy(&b, &bBits, sizeof(b));
            const float f = a - b;
            uint32 fBits;
            memcpy(&fBits, &f, sizeof(fBits));
            fBits |= (s << 16);
            memcpy(&dst[i], &fBits, sizeof(fBits));
        }
    };

    float* dst = out;
    const uint16* src = in;
    int32 remaining = n;
    while (remaining >= 4) {
        convert4(dst, src);
        dst += 4;
        src += 4;
        remaining -= 4;
    }
    if (remaining != 0) {
        convert4(out + (n - 4), in + (n - 4));
    }
    return out;
}

// 0x7C203B: D3DXVec2Transform 3DNow!. Con v = (x, y) y M = 16 floats (fila r, columna c = M[4r + c]):
//   out[c] = ((x * M[c]) + M[12 + c]) + (y * M[4 + c]),  c = 0..3  (pfmul/pfadd por pares, mismo orden; lee x e y antes de escribir).
// Redondeo: el EXE usa pfmul/pfadd del 3DNow! (hardware que ya no existe en las CPU actuales); aqui son operaciones IEEE de 32 bits.
float* __stdcall D3DXVec2Transform_3DNow(float* out, const float* v, const float* matrix) {
    const float x = v[0];
    const float y = v[1];
    float r[4];
    for (int c = 0; c < 4; ++c) {
        const float xm = x * matrix[c];
        const float ym = y * matrix[4 + c];
        r[c] = (xm + matrix[12 + c]) + ym;
    }
    out[0] = r[0];
    out[1] = r[1];
    out[2] = r[2];
    out[3] = r[3];
    return out;
}

// 0x7C2090: D3DXVec2TransformNormal 3DNow!: out[c] = (x * M[c]) + (y * M[4 + c]), c = 0..1. Mismo aviso de redondeo que 0x7C203B.
float* __stdcall D3DXVec2TransformNormal_3DNow(float* out, const float* v, const float* matrix) {
    const float x = v[0];
    const float y = v[1];
    const float r0 = (x * matrix[0]) + (y * matrix[4]);
    const float r1 = (x * matrix[1]) + (y * matrix[5]);
    out[0] = r0;
    out[1] = r1;
    return out;
}

// 0x7C2AAE: acos de cuatro floats (SSE). Con a = |x|:
//   todas las lanes con a <= 0.5:  acos = pi/2 - x * P(a^2);   P = (1 + a^4 (c2 + c5 a^4)) + a^2 (c1 + a^4 (c3 + c4 a^4))
//   todas con a > 0.5:  z = 0.5 (1 - a); s = sqrt(z); acos = (((P(z) * 2s) | signo(x)) + (x < 0 ? pi : 0)),  con P evaluado en y^2 = z
//   mezcla: y = (a <= 0.5) ? a : sqrt(0.5 (1 - a)); se calcula P en y y se escoge por lane (pequena: pi/2 - (P*y | signo); grande: 2*P*y | signo + (x < 0 ? pi : 0)).
// Los punteros deben estar alineados a 16 bytes (movaps).
void __stdcall D3DX9AcosPS(const float* in, float* out) {
    const __m128 absMask = Splat(kAbsMask);
    const __m128 half    = Splat(kHalf);
    const __m128 one     = Splat(kOne);
    const __m128 c1      = Splat(kAcosC1);
    const __m128 c2      = Splat(kAcosC2);
    const __m128 c3      = Splat(kAcosC3);
    const __m128 c4      = Splat(kAcosC4);
    const __m128 c5      = Splat(kAcosC5);

    __m128 x = _mm_load_ps(in);                        // xmm3
    __m128 a = _mm_and_ps(absMask, x);                 // xmm2 = |x|
    const __m128 sign = _mm_andnot_ps(absMask, x);     // xmm5 = signo de x
    const __m128 le = _mm_cmple_ps(a, half);           // xmm1
    const int mask = _mm_movemask_ps(le);

    if (mask == 0xF) {
        // todas pequenas
        __m128 a2 = _mm_mul_ps(a, a);                  // xmm1
        __m128 a4 = _mm_mul_ps(a2, a2);                // xmm0
        __m128 t = _mm_mul_ps(c4, a4);                 // xmm2
        __m128 p3 = _mm_add_ps(c3, t);                 // xmm4
        __m128 t2 = _mm_mul_ps(a4, p3);                // xmm2
        __m128 p1 = _mm_add_ps(c1, t2);                // xmm4
        __m128 t3 = _mm_mul_ps(c5, a4);                // xmm2
        a2 = _mm_mul_ps(a2, p1);                       // xmm1
        __m128 p2 = _mm_add_ps(c2, t3);                // xmm4
        a4 = _mm_mul_ps(a4, p2);                       // xmm0
        __m128 s = _mm_add_ps(one, a4);                // xmm2
        s = _mm_add_ps(s, a2);
        s = _mm_mul_ps(s, x);
        _mm_store_ps(out, _mm_sub_ps(Splat(kHalfPi), s));
        return;
    }

    const __m128 negMask = _mm_cmplt_ps(x, _mm_setzero_ps()); // xmm3 (sobrescribe x)

    if (mask == 0) {
        // todas grandes
        const __m128 oneMinusA = _mm_sub_ps(one, a);   // xmm1
        __m128 z = _mm_mul_ps(half, oneMinusA);        // xmm0
        const __m128 s = _mm_sqrt_ps(z);               // xmm2
        const __m128 twoS = _mm_add_ps(s, s);          // xmm4
        __m128 z2 = _mm_mul_ps(z, z);                  // xmm1
        __m128 t = _mm_mul_ps(c4, z2);                 // xmm2
        __m128 p3 = _mm_add_ps(c3, t);                 // xmm6
        __m128 t2 = _mm_mul_ps(z2, p3);                // xmm2
        __m128 p1 = _mm_add_ps(c1, t2);                // xmm6
        __m128 t3 = _mm_mul_ps(c5, z2);                // xmm2
        z = _mm_mul_ps(z, p1);                         // xmm0
        __m128 p2 = _mm_add_ps(c2, t3);                // xmm6
        z2 = _mm_mul_ps(z2, p2);                       // xmm1
        __m128 poly = _mm_add_ps(one, z2);             // xmm2
        poly = _mm_add_ps(poly, z);
        poly = _mm_mul_ps(poly, twoS);
        poly = _mm_or_ps(poly, sign);
        const __m128 negPi = _mm_and_ps(negMask, Splat(kPi));
        _mm_store_ps(out, _mm_add_ps(negPi, poly));
        return;
    }

    // mezcla de lanes pequenas y grandes
    __m128 z = _mm_mul_ps(half, _mm_sub_ps(one, a));   // xmm0 = 0.5 (1 - a)
    z = _mm_sqrt_ps(z);                                // xmm0 = s
    const __m128 largeS = _mm_andnot_ps(le, z);        // ~le & s
    __m128 y = _mm_and_ps(le, a);                      // xmm4 = le & a
    y = _mm_or_ps(y, largeS);
    __m128 y2 = _mm_mul_ps(y, y);                      // xmm2
    __m128 y4 = _mm_mul_ps(y2, y2);                    // xmm0
    __m128 t4 = _mm_mul_ps(c4, y4);                    // xmm6
    __m128 p3 = _mm_add_ps(c3, t4);                    // xmm7
    __m128 t2 = _mm_mul_ps(y4, p3);                    // xmm6
    __m128 p1 = _mm_add_ps(c1, t2);                    // xmm7
    y2 = _mm_mul_ps(y2, p1);                           // xmm2
    __m128 t5 = _mm_mul_ps(c5, y4);                    // xmm6
    __m128 p2 = _mm_add_ps(c2, t5);                    // xmm7
    y4 = _mm_mul_ps(y4, p2);                           // xmm0
    __m128 poly = _mm_add_ps(one, y4);                 // xmm6
    poly = _mm_add_ps(poly, y2);                       // xmm0 (copia)
    poly = _mm_mul_ps(poly, y);                        // P * y
    __m128 doubled = _mm_add_ps(poly, poly);           // xmm2
    doubled = _mm_or_ps(doubled, sign);
    const __m128 negPi = _mm_and_ps(negMask, Splat(kPi));
    const __m128 largeResult = _mm_add_ps(negPi, doubled);    // xmm3
    const __m128 largePart = _mm_andnot_ps(le, largeResult);  // xmm2
    const __m128 signedPoly = _mm_or_ps(poly, sign);          // xmm0
    const __m128 smallResult = _mm_sub_ps(Splat(kHalfPi), signedPoly); // xmm3
    const __m128 smallPart = _mm_and_ps(le, smallResult);
    _mm_store_ps(out, _mm_or_ps(smallPart, largePart));
}

// 0x7C2CA4: 2 * acos(value) con el valor repartido a las cuatro lanes (movss + shufps 0); solo se escribe la lane 0 (movss). Como 0x7C2AAE pero con los
// coeficientes en pares (0x882940: 1, c1; 0x882950: c2, c3; 0x882960: c5, c4) y el resultado duplicado (`addps x,x`) al final. Los caminos pequeno (a <= 0.5) y grande salen del
// movmskps de toda la lane: 0xF o distinto de 0 -> pequeno (todas las lanes valen lo mismo, asi que solo ocurre 0 o 0xF).
void __stdcall D3DX9TwoAcosScalar(float value, float* out) {
    const __m128 absMask = Splat(kAbsMask);
    const __m128 half    = Splat(kHalf);
    const __m128 one     = Splat(kOne);
    const __m128 pairA   = Pair(kOne, kAcosC1);    // 0x882940
    const __m128 pairB   = Pair(kAcosC2, kAcosC3); // 0x882950
    const __m128 pairC   = Pair(kAcosC5, kAcosC4); // 0x882960

    __m128 x = _mm_shuffle_ps(_mm_load_ss(&value), _mm_load_ss(&value), 0); // xmm2
    __m128 sign = _mm_andnot_ps(absMask, x);                                // xmm0
    __m128 a = _mm_and_ps(absMask, x);                                      // xmm1
    const int mask = _mm_movemask_ps(_mm_cmple_ps(a, half));

    if (mask != 0) {
        // camino pequeno (0x7C2D4A)
        __m128 a2 = _mm_mul_ps(a, a);                                       // xmm1
        __m128 a4 = _mm_mul_ps(a2, a2);                                     // xmm4
        __m128 v = _mm_mul_ps(pairC, a4);                                   // xmm0
        v = _mm_add_ps(v, pairB);
        a4 = _mm_mul_ps(a4, v);
        a4 = _mm_add_ps(a4, pairA);                                         // lane 0: 1 + a^4 (c2 + c5 a^4); lane 1: c1 + a^4 (c3 + c4 a^4)
        const __m128 lane1 = _mm_shuffle_ps(a4, a4, 0x55);                  // xmm3
        a2 = _mm_mul_ss(a2, lane1);                                         // mulss: solo la lane 0
        a4 = _mm_add_ps(a4, a2);
        a4 = _mm_mul_ps(a4, x);
        __m128 r = _mm_sub_ps(Splat(kHalfPi), a4);
        r = _mm_add_ps(r, r);
        _mm_store_ss(out, r);
        return;
    }

    // camino grande
    const __m128 negMask = _mm_cmplt_ps(x, _mm_setzero_ps());               // xmm2
    __m128 z = _mm_sub_ps(one, a);                                          // xmm5
    z = _mm_mul_ps(z, half);
    __m128 twoS = _mm_sqrt_ps(z);                                           // xmm6
    twoS = _mm_add_ps(twoS, twoS);
    __m128 z2 = _mm_mul_ps(z, z);                                           // xmm7
    __m128 v = _mm_mul_ps(pairC, z2);                                       // xmm1
    v = _mm_add_ps(v, pairB);
    z2 = _mm_mul_ps(z2, v);
    z2 = _mm_add_ps(z2, pairA);                                             // xmm7
    const __m128 lane1 = _mm_shuffle_ps(z2, z2, 0x55);                      // xmm4
    z = _mm_mul_ps(z, lane1);                                               // xmm5
    z2 = _mm_add_ps(z2, z);
    z2 = _mm_mul_ps(z2, twoS);
    z2 = _mm_or_ps(z2, sign);
    __m128 r = _mm_and_ps(negMask, Splat(kPi));
    r = _mm_add_ps(r, z2);
    r = _mm_add_ps(r, r);
    _mm_store_ss(out, r);
}

namespace {
// Seno y coseno de cuatro floats por reduccion a cuadrantes (usada por 0x7C307E y 0x7C381F, que hacen lo mismo con distinto codigo):
// k = redondeo(|x| * 2/pi) con el truco del 2^23 (suma y resta de 0x4B000000), r = |x| - k*pi/2 con cuatro piezas de pi/2 restadas en orden, q = entero de k (&0x7FFFFF).
// q impar: se cambian seno y coseno; los signos salen de los bits 1 de (q -/+ (q & 1)) mas el signo de x. Polinomios: seno r (1 + r2 (...)), coseno 1 + r2 (...).
void SinCosFour(const float* in, float* outSin, float* outCos) {
    const __m128 x = _mm_load_ps(in);
    const __m128 absX = _mm_and_ps(x, Splat(kAbsMask));              // xmm0 -> xmm2
    const __m128 signX = _mm_and_ps(x, Splat(kSignMask));            // xmm1
    __m128 k = _mm_mul_ps(absX, Splat(0x3F22F983));                   // 2/pi (0x8DE920)
    __m128 p4 = Splat(0x3FC90000);                                    // 1.5703125 (0x8DE910)
    __m128 p5 = Splat(0x39FDA000);                                    // 0x8DE900
    __m128 p6 = Splat(0x33A22000);                                    // 0x8DE8F0
    __m128 p7 = Splat(0x2C34611A);                                    // 0x8DE8E0
    const __m128 magic = Splat(0x4B000000);                           // 8388608.0f (0x8DE8D0)
    k = _mm_add_ps(k, magic);
    const __m128 kBits = k;                                           // xmm3
    k = _mm_sub_ps(k, magic);
    p4 = _mm_mul_ps(p4, k);
    p5 = _mm_mul_ps(p5, k);
    p6 = _mm_mul_ps(p6, k);
    p7 = _mm_mul_ps(p7, k);
    __m128 r = absX;                                                  // xmm2
    r = _mm_sub_ps(r, p4);
    r = _mm_sub_ps(r, p5);
    r = _mm_sub_ps(r, p6);
    r = _mm_sub_ps(r, p7);
    const __m128 r2 = _mm_mul_ps(r, r);                               // xmm4

    const __m128i q = _mm_and_si128(_mm_castps_si128(kBits), _mm_set1_epi32(0x007FFFFF));
    const __m128i odd = _mm_and_si128(q, _mm_set1_epi32(1));
    const __m128i two = _mm_set1_epi32(2);
    __m128i signA = _mm_slli_epi32(_mm_and_si128(_mm_sub_epi32(q, odd), two), 30);
    const __m128i signB = _mm_slli_epi32(_mm_and_si128(_mm_add_epi32(q, odd), two), 30);
    signA = _mm_xor_si128(signA, _mm_castps_si128(signX));
    const __m128 oddMask = _mm_cmpneq_ps(_mm_castsi128_ps(_mm_slli_epi32(odd, 30)), _mm_setzero_ps());

    __m128 sinP = Splat(0xB94CA1F0);                                  // -0.000195152825
    __m128 cosP = Splat(0x37CCF5CE);                                  // 2.44331568e-05
    sinP = _mm_mul_ps(sinP, r2);
    cosP = _mm_mul_ps(cosP, r2);
    sinP = _mm_add_ps(sinP, Splat(0x3C08839D));
    cosP = _mm_add_ps(cosP, Splat(0xBAB6061A));
    sinP = _mm_mul_ps(sinP, r2);
    cosP = _mm_mul_ps(cosP, r2);
    sinP = _mm_add_ps(sinP, Splat(0xBE2AAAA3));
    cosP = _mm_add_ps(cosP, Splat(0x3D2AAAA5));
    sinP = _mm_mul_ps(sinP, r2);
    cosP = _mm_mul_ps(cosP, r2);
    sinP = _mm_add_ps(sinP, Splat(0x3F800000));
    cosP = _mm_add_ps(cosP, Splat(0xBF000000));
    sinP = _mm_mul_ps(sinP, r);
    cosP = _mm_mul_ps(cosP, r2);
    cosP = _mm_add_ps(cosP, Splat(0x3F800000));

    __m128 sinOut = _mm_or_ps(_mm_andnot_ps(oddMask, sinP), _mm_and_ps(oddMask, cosP));
    __m128 cosOut = _mm_or_ps(_mm_and_ps(oddMask, sinP), _mm_andnot_ps(oddMask, cosP));
    cosOut = _mm_xor_ps(cosOut, _mm_castsi128_ps(signB));
    sinOut = _mm_xor_ps(sinOut, _mm_castsi128_ps(signA));
    _mm_store_ps(outSin, sinOut);
    _mm_store_ps(outCos, cosOut);
}
} // namespace

// 0x7C307E: seno/coseno de cuatro floats; el EXE pasa los enteros por una zona de pila alineada (movaps + operaciones escalares). Devuelve outSin (EAX).
float* __stdcall D3DX9SinCosPS(const float* in, float* outSin, float* outCos) {
    SinCosFour(in, outSin, outCos);
    return outSin;
}

// 0x7C381F: igual que 0x7C307E con los enteros en registros SSE2 (psubd/paddd/pslld). Devuelve outSin (EAX).
float* __stdcall D3DX9SinCosPS_SSE2(const float* in, float* outSin, float* outCos) {
    SinCosFour(in, outSin, outCos);
    return outSin;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Lote R21A (sin validar): ensamblador y compilador de shaders de D3DX9 (0x7652AE, 0x76534D, 0x765402, 0x7654B8). Cuatro funciones __stdcall con objetos locales de C++ (sin SEH) cuyos metodos
// (__thiscall, en el EXE y sin reconstruir) se llaman por direccion fija sobre buffers de pila del tamano que reserva el EXE:
//   fuente (0x90 bytes; ctor 0x76B89B, dtor 0x76B940; su subobjeto de errores esta en +4: 0x76E664 devuelve su campo +8, 0x76E5FB(destino) copia el buffer de errores al ID3DXBuffer** pedido),
//   ensamblador (0x80 bytes; ctor 0x76FAEB, ejecucion 0x772BB1, dtor 0x771F63) y compilador (0x54 bytes; ctor 0x773022, ejecucion 0x77EE77, dtor 0x77303B).
// Cargadores de la fuente: 0x76DFD1(datos, longitud, defines, include), 0x76DF03(fichero, modo 0 = ANSI / 1 = Unicode, defines, include) y 0x76DF6C(modulo, recurso, modo, defines, include).
// El HRESULT es el del paso que falla (comparacion con signo `< 0`); si todo va bien pero el objeto de errores no esta vacio (0x76E664 != 0) devuelve 0x88760B59.
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace {
constexpr int32  kD3DXErrorInvalidData = static_cast<int32>(0x88760B59);
constexpr size_t kShaderSourceSize     = 0x90;
constexpr size_t kShaderAssemblerSize  = 0x80;
constexpr size_t kShaderCompilerSize   = 0x54;

using ObjectVoidFn         = void*(__thiscall*)(void* self);
using ObjectIntFn          = int32(__thiscall*)(void* self);
using SourceFromMemoryFn   = int32(__thiscall*)(void* self, const void* data, uint32 length, const void* defines, void* include);
using SourceFromFileFn     = int32(__thiscall*)(void* self, const void* file, int32 wide, const void* defines, void* include);
using SourceFromResourceFn = int32(__thiscall*)(void* self, void* module, const void* resource, int32 wide, const void* defines, void* include);
using ErrorsCopyFn         = void*(__thiscall*)(void* self, void** destination);
using AssemblerRunFn       = int32(__thiscall*)(void* self, void* source, uint32 flags, int32 zero, void** shader);
using CompilerRunFn        = int32(__thiscall*)(void* self, void* source, int32 zero0, const char* functionName, int32 zero1, const char* target, uint32 flags, void** shader, void** constantTable);

inline void SourceConstruct(void* source) { reinterpret_cast<ObjectVoidFn>(0x76B89B)(source); }
inline void SourceDestruct(void* source) { reinterpret_cast<ObjectVoidFn>(0x76B940)(source); }
inline int32 SourceErrorsPending(void* source) { return reinterpret_cast<ObjectIntFn>(0x76E664)(static_cast<uint8_t*>(source) + 4); }
inline void SourceErrorsCopy(void* source, void** destination) { reinterpret_cast<ErrorsCopyFn>(0x76E5FB)(static_cast<uint8_t*>(source) + 4, destination); }
inline void AssemblerConstruct(void* assembler) { reinterpret_cast<ObjectVoidFn>(0x76FAEB)(assembler); }
inline void AssemblerDestruct(void* assembler) { reinterpret_cast<ObjectVoidFn>(0x771F63)(assembler); }
inline void CompilerConstruct(void* compiler) { reinterpret_cast<ObjectVoidFn>(0x773022)(compiler); }
inline void CompilerDestruct(void* compiler) { reinterpret_cast<ObjectVoidFn>(0x77303B)(compiler); }

// Tramo comun de las tres funciones de compilacion: la carga ya esta hecha en `source` con `hr`; compila, comprueba el objeto de errores, copia los errores y destruye la fuente.
int32 CompileLoadedSource(uint8_t* source, int32 hr, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    if (hr >= 0) {
        alignas(4) uint8_t compiler[kShaderCompilerSize];
        CompilerConstruct(compiler);
        hr = reinterpret_cast<CompilerRunFn>(0x77EE77)(compiler, source, 0, functionName, 0, target, flags, shader, constantTable);
        CompilerDestruct(compiler);
        if (hr >= 0 && SourceErrorsPending(source) != 0) {
            hr = kD3DXErrorInvalidData;
        }
    }
    if (errorMsgs != nullptr) {
        SourceErrorsCopy(source, errorMsgs);
    }
    SourceDestruct(source);
    return hr;
}
} // namespace

// 0x7652AE (lote R21A, sin validar; nombre de la API publica inferido; __stdcall, 7 argumentos, `ret 1Ch`, 159 bytes): *ppShader = NULL si no es NULL; carga la fuente de memoria (0x76DFD1); con exito,
// ensamblador (0x772BB1(fuente, flags, 0, ppShader)); con exito y errores pendientes -> 0x88760B59; copia los errores a ppErrorMsgs si no es NULL; destruye la fuente; devuelve el HRESULT.
int32 __stdcall D3DXAssembleShader(const void* srcData, uint32 srcDataLen, const void* defines, void* include, uint32 flags, void** shader, void** errorMsgs) {
    if (shader != nullptr) {
        *shader = nullptr;
    }
    alignas(4) uint8_t source[kShaderSourceSize];
    SourceConstruct(source);
    int32 hr = reinterpret_cast<SourceFromMemoryFn>(0x76DFD1)(source, srcData, srcDataLen, defines, include);
    if (hr >= 0) {
        alignas(4) uint8_t assembler[kShaderAssemblerSize];
        AssemblerConstruct(assembler);
        hr = reinterpret_cast<AssemblerRunFn>(0x772BB1)(assembler, source, flags, 0, shader);
        AssemblerDestruct(assembler);
        if (hr >= 0 && SourceErrorsPending(source) != 0) {
            hr = kD3DXErrorInvalidData;
        }
    }
    if (errorMsgs != nullptr) {
        SourceErrorsCopy(source, errorMsgs);
    }
    SourceDestruct(source);
    return hr;
}

// 0x76534D (lote R21A, sin validar; nombre inferido; __stdcall, 9 argumentos, `ret 24h`, 181 bytes): *ppShader y *ppConstantTable = NULL si no son NULL; fuente de fichero ANSI (0x76DF03 con modo 0) y
// compilacion (0x77EE77(fuente, 0, funcion, 0, destino, flags, ppShader, ppConstantTable)).
int32 __stdcall D3DXCompileShaderFromFileA(const char* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    if (shader != nullptr) {
        *shader = nullptr;
    }
    if (constantTable != nullptr) {
        *constantTable = nullptr;
    }
    alignas(4) uint8_t source[kShaderSourceSize];
    SourceConstruct(source);
    const int32 hr = reinterpret_cast<SourceFromFileFn>(0x76DF03)(source, srcFile, 0, defines, include);
    return CompileLoadedSource(source, hr, functionName, target, flags, shader, errorMsgs, constantTable);
}

// 0x765402 (lote R21A, sin validar; nombre inferido; __stdcall, 9 argumentos, `ret 24h`, 182 bytes): igual que 0x76534D con el modo 1 (ruta Unicode) en el cargador 0x76DF03.
int32 __stdcall D3DXCompileShaderFromFileW(const wchar_t* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    if (shader != nullptr) {
        *shader = nullptr;
    }
    if (constantTable != nullptr) {
        *constantTable = nullptr;
    }
    alignas(4) uint8_t source[kShaderSourceSize];
    SourceConstruct(source);
    const int32 hr = reinterpret_cast<SourceFromFileFn>(0x76DF03)(source, srcFile, 1, defines, include);
    return CompileLoadedSource(source, hr, functionName, target, flags, shader, errorMsgs, constantTable);
}

// 0x7654B8 (lote R21A, sin validar; nombre inferido; __stdcall, 10 argumentos, `ret 28h`, 184 bytes): igual que 0x76534D con la fuente de recurso (0x76DF6C(modulo, recurso, 0, defines, include)).
int32 __stdcall D3DXCompileShaderFromResourceA(void* srcModule, const char* srcResource, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    if (shader != nullptr) {
        *shader = nullptr;
    }
    if (constantTable != nullptr) {
        *constantTable = nullptr;
    }
    alignas(4) uint8_t source[kShaderSourceSize];
    SourceConstruct(source);
    const int32 hr = reinterpret_cast<SourceFromResourceFn>(0x76DF6C)(source, srcModule, srcResource, 0, defines, include);
    return CompileLoadedSource(source, hr, functionName, target, flags, shader, errorMsgs, constantTable);
}
