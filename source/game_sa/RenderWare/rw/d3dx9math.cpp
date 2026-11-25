/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3DX9
 * Reconstrucción: COMPLETA — 17/17 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/17 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <float.h>
#include <emmintrin.h>
#include <xmmintrin.h>

#include "d3dx9math.h"
#include "D3DX9MatrixMultiplyTransposeSse.h"
#include "D3DX9NativeMath.h"
#include "D3DX9NativeAssembler.h"

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
    // lote EXT1A: matematica de D3DX9 del EXE (0x766A32.. son `jmp [tabla]`) sustituida por d3dx9_43.dll; FnD3DX9C (24/11/2026): ahora es C++ propio (D3DX9NativeMath, bit a bit igual que
    // la DLL en el banco) y la DLL solo se carga con --d3dx9-dll/--d3dx9-verificar; la de MatrixMultiplyTranspose es propia desde EXT1A (la DLL redondea distinto)
    RH_ScopedGlobalInstall(D3DXFloat16To32Array, 0x76626B);
    RH_ScopedGlobalInstall(D3DXVec3TransformCoord, 0x766A32);
    RH_ScopedGlobalInstall(D3DXVec3TransformNormal, 0x766AB7);
    RH_ScopedGlobalInstall(D3DXMatrixMultiply, 0x7672E1);
    RH_ScopedGlobalInstall(D3DXMatrixTranspose, 0x7675EE);
    RH_ScopedGlobalInstall(D3DXMatrixMultiplyTranspose, 0x7676C7);

    // lote R21A / E3 (P4): ensamblador y compilador de shaders sustituidos por d3dx9_43.dll. FnD3DX9C: el ensamblador de vertex shaders vs_2_0/vs_2_x del juego es propio (D3DX9NativeAssembler);
    // la DLL queda de respaldo para lo que ese ensamblador no cubre (vs_1_1, compilacion HLSL). Nacieron desactivados (20/11/2026) porque la DLL rechazaba Flags = 4
    // (SKIPOPTIMIZATION) con D3DERR_INVALIDCALL y los modelos con esqueleto salian invisibles; FnD3DX9B enmascaro las banderas y el estado x87/SSE se restaura al cargar la DLL.
    // Reactivados en el lote E3 (P4): ver docs/CodigoVivoExe/ejecucion-p4.md.
    RH_ScopedGlobalInstall(D3DXAssembleShader, 0x7652AE);
    RH_ScopedGlobalInstall(D3DXCompileShaderFromFileA, 0x76534D);
    RH_ScopedGlobalInstall(D3DXCompileShaderFromFileW, 0x765402);
    RH_ScopedGlobalInstall(D3DXCompileShaderFromResourceA, 0x7654B8);
}

// 0x7C1D80: medios flotantes -> floats con MMX/3DNow! (`femms` al entrar y salir, __stdcall: `ret 0Ch`). Con count < 4 llama al conversor escalar 0x76626B
// (D3DXFloat16To32Array escalar, desde EXT1A el de d3dx9_43.dll; __stdcall con los mismos tres argumentos; con count == 0 no llama a nada). Con count >= 4 convierte de cuatro en cuatro; si sobran 1..3
// vuelve a convertir los ultimos cuatro elementos (se solapa con lo ya escrito: mismo resultado). Por elemento w = medio flotante de 16 bits ampliado a 32:
//   z = (w & 0x7C00) == 0 (mascara completa); s = w & 0x8000; m = (w ^ s) + 0x1C000 + (z & 0x400);
//   f = float(m << 13) - float(z & 0x38800000)   (pfsub: exacto, la resta de dos normales cercanos);  resultado = bits(f) | (s << 16).
// Los infinitos y NaN (exponente 31) no se tratan aparte: salen como 65536.0f con mantisa desplazada, como en el EXE.
float* __stdcall D3DXFloat16To32Array_3DNow(float* out, const uint16* in, uint32 count) {
    const int32 n = static_cast<int32>(count); // `cmp ecx,4 / jge`: comparaciones con signo
    if (n < 4) {
        if (n != 0) {
            D3DX9System::Float16To32Array(out, in, count); // antes 0x76626B (conversor escalar del EXE); ahora d3dx9_43.dll (lote EXT1A)
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
// Lote EXT1A: D3DX9 del sistema. Las 19 funciones de D3DX9 enlazadas estaticamente en el EXE a las que llamaba el codigo propio (compilador/ensamblador de shaders, matematica de
// matrices y vectores y la conversion de medios flotantes) se sustituyen por las funciones exportadas de d3dx9_43.dll (SDK de junio de 2010, instalada en SysWOW64).
// Las 13 rutinas internas del EXE (objeto de fuente 0x76B89B/0x76B940/0x76DF03/0x76DF6C/0x76DFD1/0x76E5FB/0x76E664, ensamblador 0x76FAEB/0x772BB1/0x771F63 y compilador
// 0x773022/0x77EE77/0x77303B) no tienen exportacion propia: solo las usaban los cuatro envoltorios D3DXAssembleShader / D3DXCompileShaderFromFileA / FromFileW / FromResourceA, que ahora
// llaman a la exportacion del mismo nombre. Ver reversing/FnEXT1A/README.md.
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace D3DX9System {
namespace {
constexpr int32 kD3DERR_NOTAVAILABLE = static_cast<int32>(0x8876086A);

HMODULE LoadSystemDll() {
    // Al cargarse, d3dx9_43.dll (y su CRT) puede reiniciar el estado de coma flotante del hilo: la precision x87 que fija Direct3D al crear el dispositivo (24 bits) y el control
    // de SSE. El juego depende de ese estado en todo su calculo con esqueletos (los modelos de CJ y los peatones salian cortados o en pose T con la DLL cargada a mitad del
    // dibujado). Se guarda antes de cargar y se restaura despues.
    unsigned int fpuAntes = 0;
    _controlfp_s(&fpuAntes, 0, 0);
    const unsigned int mxcsrAntes = _mm_getcsr();

    // Primero la copia de la carpeta del sistema (SysWOW64 en un proceso de 32 bits); si no esta, la busqueda normal (copia local junto al juego).
    HMODULE module = LoadLibraryExA("d3dx9_43.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module == nullptr) {
        module = LoadLibraryA("d3dx9_43.dll");
    }

    unsigned int fpuDespues = 0;
    _controlfp_s(&fpuDespues, 0, 0);
    const unsigned int mxcsrDespues = _mm_getcsr();
    if (fpuDespues != fpuAntes || mxcsrDespues != mxcsrAntes) {
        NOTSA_LOG_WARN("D3DX9: al cargar d3dx9_43.dll cambio el estado de coma flotante (x87 0x{:08X} -> 0x{:08X}, MXCSR 0x{:08X} -> 0x{:08X}); se restaura", fpuAntes, fpuDespues, mxcsrAntes, mxcsrDespues);
        unsigned int ignorado = 0;
        _controlfp_s(&ignorado, fpuAntes, _MCW_EM | _MCW_RC | _MCW_PC | _MCW_IC | _MCW_DN);
        _mm_setcsr(mxcsrAntes);
    }
    if (module == nullptr) {
        NOTSA_LOG_ERR("D3DX9: no se pudo cargar d3dx9_43.dll (instale el Runtime de usuario final de DirectX de junio de 2010); las funciones de D3DX9 devolveran error");
    }
    return module;
}

template <typename Fn>
Fn Get(const char* name) {
    static const HMODULE module = LoadSystemDll();
    FARPROC address = module != nullptr ? GetProcAddress(module, name) : nullptr;
    if (address == nullptr && module != nullptr) {
        NOTSA_LOG_ERR("D3DX9: d3dx9_43.dll no exporta {}", name);
    }
    return reinterpret_cast<Fn>(address);
}
} // namespace

// FnD3DX9C: modo de las funciones de matematica y del ensamblador. 0 = propio (por defecto: C++ propio, sin d3dx9_43.dll), 1 = DLL (--d3dx9-dll, comportamiento anterior),
// 2 = verificar (--d3dx9-verificar: ejecuta el propio y la DLL, compara bit a bit y registra las diferencias; carga la DLL, solo diagnostico).
int s_Mode = 0;
namespace {
enum Fn { kF16, kCoord, kNormal, kMul, kTranspose, kMulTranspose, kAssemble, kAssembleDll, kCount };
constexpr const char* kFnNames[kCount] = {"D3DXFloat16To32Array", "D3DXVec3TransformCoord", "D3DXVec3TransformNormal", "D3DXMatrixMultiply", "D3DXMatrixTranspose",
                                          "D3DXMatrixMultiplyTranspose (siempre propia)", "D3DXAssembleShader (propio)", "D3DXAssembleShader (DLL)"};
uint32 s_Calls[kCount];
uint32 s_Mismatches[kCount];
// Contadores de uso: con --d3dx9-log se registra la primera llamada y cada potencia de dos (1, 2, 4, 8, ...).
void Count(Fn fn) {
    const uint32 n = ++s_Calls[fn];
    if (s_LogCalls && (n & (n - 1)) == 0) {
        NOTSA_LOG_INFO("D3DX9 uso: {} llamada #{} (modo {})", kFnNames[fn], n, s_Mode == 0 ? "propio" : s_Mode == 1 ? "DLL" : "verificar");
    }
}
// Modo verificar: compara la salida propia con la de la DLL (bit a bit).
void Compare(Fn fn, const void* own, const void* dll, size_t bytes) {
    if (std::memcmp(own, dll, bytes) != 0) {
        const uint32 n = ++s_Mismatches[fn];
        if (n <= 20) NOTSA_LOG_ERR("D3DX9 VERIFICAR: {} difiere de d3dx9_43.dll (diferencia #{}, llamada #{})", kFnNames[fn], n, s_Calls[fn]);
    } else if (s_LogCalls && (s_Calls[fn] & (s_Calls[fn] - 1)) == 0) {
        NOTSA_LOG_INFO("D3DX9 VERIFICAR: {} igual que d3dx9_43.dll en la llamada #{} (diferencias hasta ahora: {})", kFnNames[fn], s_Calls[fn], s_Mismatches[fn]);
    }
}
} // namespace

float* Float16To32Array(float* out, const uint16* in, uint32 count) {
    Count(kF16);
    if (s_Mode == 0) return D3DX9Native::Float16To32Array(out, in, count);
    static const auto fn = Get<float*(__stdcall*)(float*, const uint16*, uint32)>("D3DXFloat16To32Array");
    if (s_Mode == 1) return fn ? fn(out, in, count) : out;
    std::vector<uint16> copy(in, in + count);
    std::vector<float> ref(count);
    D3DX9Native::Float16To32Array(out, copy.data(), count);
    if (fn) { fn(ref.data(), copy.data(), count); Compare(kF16, out, ref.data(), count * sizeof(float)); }
    return out;
}
void* Vec3TransformCoord(void* out, const void* v, const void* matrix) {
    Count(kCoord);
    if (s_Mode == 0) return D3DX9Native::Vec3TransformCoord(out, v, matrix);
    static const auto fn = Get<void*(__stdcall*)(void*, const void*, const void*)>("D3DXVec3TransformCoord");
    if (s_Mode == 1) return fn ? fn(out, v, matrix) : out;
    float cv[3], cm[16], ref[3];
    std::memcpy(cv, v, sizeof cv); std::memcpy(cm, matrix, sizeof cm);
    D3DX9Native::Vec3TransformCoord(out, cv, cm);
    if (fn) { fn(ref, cv, cm); Compare(kCoord, out, ref, sizeof ref); }
    return out;
}
void* Vec3TransformNormal(void* out, const void* v, const void* matrix) {
    Count(kNormal);
    if (s_Mode == 0) return D3DX9Native::Vec3TransformNormal(out, v, matrix);
    static const auto fn = Get<void*(__stdcall*)(void*, const void*, const void*)>("D3DXVec3TransformNormal");
    if (s_Mode == 1) return fn ? fn(out, v, matrix) : out;
    float cv[3], cm[16], ref[3];
    std::memcpy(cv, v, sizeof cv); std::memcpy(cm, matrix, sizeof cm);
    D3DX9Native::Vec3TransformNormal(out, cv, cm);
    if (fn) { fn(ref, cv, cm); Compare(kNormal, out, ref, sizeof ref); }
    return out;
}
void* MatrixMultiply(void* out, const void* a, const void* b) {
    Count(kMul);
    if (s_Mode == 0) return D3DX9Native::MatrixMultiply(out, a, b);
    static const auto fn = Get<void*(__stdcall*)(void*, const void*, const void*)>("D3DXMatrixMultiply");
    if (s_Mode == 1) return fn ? fn(out, a, b) : out;
    float ca[16], cb[16], ref[16];
    std::memcpy(ca, a, sizeof ca); std::memcpy(cb, b, sizeof cb);
    D3DX9Native::MatrixMultiply(out, ca, cb);
    if (fn) { fn(ref, ca, cb); Compare(kMul, out, ref, sizeof ref); }
    return out;
}
void* MatrixTranspose(void* out, const void* in) {
    Count(kTranspose);
    if (s_Mode == 0) return D3DX9Native::MatrixTranspose(out, in);
    static const auto fn = Get<void*(__stdcall*)(void*, const void*)>("D3DXMatrixTranspose");
    if (s_Mode == 1) return fn ? fn(out, in) : out;
    float ci[16], ref[16];
    std::memcpy(ci, in, sizeof ci);
    D3DX9Native::MatrixTranspose(out, ci);
    if (fn) { fn(ref, ci); Compare(kTranspose, out, ref, sizeof ref); }
    return out;
}
// Excepcion del lote EXT1A: la DLL redondea distinto que el EXE (1-2 ulps) en esta funcion, asi que se usa la rutina propia que reproduce al EXE bit a bit (D3DX9MatrixMultiplyTransposeSse.h).
void* MatrixMultiplyTranspose(void* out, const void* a, const void* b) {
    Count(kMulTranspose);
    return D3DX9MatrixMultiplyTransposeSse(static_cast<float*>(out), static_cast<const float*>(a), static_cast<const float*>(b));
}
namespace {
// FnD3DX9B: diagnostico (--d3dx9-log). ID3DXBuffer por tabla virtual: [3] GetBufferPointer, [4] GetBufferSize (igual que lo usa el EXE en 0x760C74).
struct IBufferLike { void** vtbl; };
const void* BufferPtr(void* b) { return b ? reinterpret_cast<const void*(__stdcall*)(void*)>(static_cast<IBufferLike*>(b)->vtbl[3])(b) : nullptr; }
uint32      BufferSize(void* b) { return b ? reinterpret_cast<uint32(__stdcall*)(void*)>(static_cast<IBufferLike*>(b)->vtbl[4])(b) : 0; }

std::string Escape(const void* data, size_t len) {
    std::string out;
    const auto* p = static_cast<const char*>(data);
    for (size_t i = 0; data && i < len && i < 200; ++i) {
        const char c = p[i];
        if (c == '\n') out += "\\n"; else if (c == '\r') out += "\\r"; else if (c >= 32 && c < 127) out += c; else out += '.';
    }
    return out;
}
std::string BufferText(void* b) {
    if (!b) return {};
    return Escape(BufferPtr(b), std::min<uint32>(BufferSize(b), 400));
}
} // namespace

bool s_LogCalls = false; // --d3dx9-log

namespace {
// Retorna el contenido de un puntero a fuente (ANSI) o de un nombre limitado a 200 bytes.
std::string Head(const char* s, size_t n = SIZE_MAX) { return s ? Escape(s, n == SIZE_MAX ? strnlen(s, 200) : n) : "(null)"; }
} // namespace

// Causa de EXT1A/R21A (FnD3DX9B): d3dx9_43.dll rechaza con D3DERR_INVALIDCALL (0x8876086C) cualquier bandera de D3DXAssembleShader distinta de D3DXSHADER_DEBUG (1) y
// D3DXSHADER_SKIPVALIDATION (2). El juego (0x760C66, generador de vertex shaders de piel/mundo) llama con Flags = 4 (D3DXSHADER_SKIPOPTIMIZATION), que el ensamblador antiguo
// del EXE ignoraba: la DLL no devolvia ningun shader y los modelos con esqueleto salian invisibles o cortados. Se enmascara a DEBUG|SKIPVALIDATION; si aun asi la validacion
// de la DLL rechaza el codigo (mas estricta que la del EXE) se reintenta sin validar, como hacia el EXE (el controlador valida otra vez en CreateVertexShader).
namespace {
int32 AssembleViaDll(const void* srcData, uint32 srcDataLen, const void* defines, void* include, uint32 flags, void** shader, void** errorMsgs) {
    static const auto fn = Get<int32(__stdcall*)(const void*, uint32, const void*, void*, uint32, void**, void**)>("D3DXAssembleShader");
    if (!fn) return kD3DERR_NOTAVAILABLE;
    constexpr uint32 kAllowed = 1u | 2u; // D3DXSHADER_DEBUG | D3DXSHADER_SKIPVALIDATION
    const uint32 used = flags & kAllowed;
    void* localShader = nullptr;
    int32 hr = fn(srcData, srcDataLen, defines, include, used, shader ? shader : &localShader, errorMsgs);
    bool retried = false;
    if (hr < 0 && !(used & 2u)) {
        retried = true;
        const auto firstError = s_LogCalls ? BufferText(errorMsgs ? *errorMsgs : nullptr) : std::string{};
        if (s_LogCalls) NOTSA_LOG_WARN("D3DX9 AssembleShader: validacion rechazada hr=0x{:08X} error='{}'; se reintenta con SKIPVALIDATION", static_cast<uint32>(hr), firstError);
        if (errorMsgs && *errorMsgs) { // liberar el buffer de errores del primer intento (Release = vtable[2])
            reinterpret_cast<uint32(__stdcall*)(void*)>(static_cast<IBufferLike*>(*errorMsgs)->vtbl[2])(*errorMsgs);
            *errorMsgs = nullptr;
        }
        hr = fn(srcData, srcDataLen, defines, include, used | 2u, shader ? shader : &localShader, errorMsgs);
    }
    if (s_LogCalls) {
        static uint32 s_Count = 0;
        NOTSA_LOG_INFO("D3DX9 AssembleShader #{}: len={} flagsEXE=0x{:X} flagsDLL=0x{:X}{} hr=0x{:08X} bytecode={} bytes errores='{}' fuente(200)='{}'",
                       ++s_Count, srcDataLen, flags, used, retried ? "+SKIPVALIDATION" : "", static_cast<uint32>(hr),
                       BufferSize(shader ? *shader : nullptr), BufferText(errorMsgs ? *errorMsgs : nullptr), Escape(srcData, srcDataLen));
    }
    if (!shader && localShader) {
        reinterpret_cast<uint32(__stdcall*)(void*)>(static_cast<IBufferLike*>(localShader)->vtbl[2])(localShader);
    }
    return hr;
}

// FnD3DX9C: ID3DXBuffer propio (IUnknown + GetBufferPointer + GetBufferSize, mismo orden de tabla virtual que el de D3DX9: el juego lo usa por ranuras 2 (Release) y 3 (GetBufferPointer)).
class NativeBuffer {
public:
    explicit NativeBuffer(std::vector<uint32>&& code) : m_Code(std::move(code)) {}
    virtual long __stdcall QueryInterface(const GUID& iid, void** object) {
        // IID_IUnknown {00000000-0000-0000-C000-000000000046}, IID_ID3DXBuffer {8BA5FB08-5195-40E2-AC58-0D989C3A0102}
        static constexpr GUID kUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
        static constexpr GUID kBuffer = {0x8BA5FB08, 0x5195, 0x40E2, {0xAC, 0x58, 0x0D, 0x98, 0x9C, 0x3A, 0x01, 0x02}};
        if (object && (std::memcmp(&iid, &kUnknown, sizeof iid) == 0 || std::memcmp(&iid, &kBuffer, sizeof iid) == 0)) {
            *object = this;
            ++m_Refs;
            return 0;
        }
        if (object) *object = nullptr;
        return static_cast<long>(0x80004002); // E_NOINTERFACE
    }
    virtual unsigned long __stdcall AddRef() { return ++m_Refs; }
    virtual unsigned long __stdcall Release() {
        const unsigned long refs = --m_Refs;
        if (refs == 0) delete this;
        return refs;
    }
    virtual void* __stdcall GetBufferPointer() { return m_Code.data(); }
    virtual unsigned long __stdcall GetBufferSize() { return static_cast<unsigned long>(m_Code.size() * sizeof(uint32)); }
    virtual ~NativeBuffer() = default;

private:
    unsigned long m_Refs = 1;
    std::vector<uint32> m_Code;
};
static_assert(sizeof(void*) == 4, "la tabla virtual debe ser la de COM de 32 bits");
} // namespace

// FnD3DX9C: D3DXAssembleShader. Por defecto usa el ensamblador propio (D3DX9NativeAssembler) para la gramatica que genera el juego (vs_2_0/vs_2_x sin defines, include ni DEBUG); lo demas
// (vs_1_1, otras versiones, instrucciones no cubiertas) cae en d3dx9_43.dll como antes, y si la DLL no esta devuelve D3DERR_NOTAVAILABLE. Las banderas del EXE (4 = SKIPOPTIMIZATION) no afectan
// al bytecode: el ensamblador antiguo del EXE las ignoraba.
int32 AssembleShader(const void* srcData, uint32 srcDataLen, const void* defines, void* include, uint32 flags, void** shader, void** errorMsgs) {
    if (s_Mode != 1 && shader && srcData && !defines && !include && !(flags & 1u)) {
        std::vector<uint32> code;
        std::string why;
        if (D3DX9Native::AssembleVertexShader(static_cast<const char*>(srcData), srcDataLen, code, why) == D3DX9Native::AssembleResult::Ok) {
            Count(kAssemble);
            if (s_Mode == 2) {
                void* dllShader = nullptr;
                const int32 hr = AssembleViaDll(srcData, srcDataLen, defines, include, flags, &dllShader, nullptr);
                if (hr < 0 || !dllShader) {
                    NOTSA_LOG_ERR("D3DX9 VERIFICAR: la DLL no pudo ensamblar un shader que el propio si (hr=0x{:08X}) fuente='{}'", static_cast<uint32>(hr), Escape(srcData, srcDataLen));
                } else {
                    const uint32 dllSize = BufferSize(dllShader);
                    if (dllSize != code.size() * sizeof(uint32)) {
                        ++s_Mismatches[kAssemble];
                        NOTSA_LOG_ERR("D3DX9 VERIFICAR: tamano distinto del bytecode (propio {} B, DLL {} B) fuente='{}'", code.size() * sizeof(uint32), dllSize, Escape(srcData, srcDataLen));
                    } else {
                        Compare(kAssemble, code.data(), BufferPtr(dllShader), dllSize);
                    }
                    reinterpret_cast<uint32(__stdcall*)(void*)>(static_cast<IBufferLike*>(dllShader)->vtbl[2])(dllShader);
                }
            }
            if (errorMsgs) *errorMsgs = nullptr;
            *shader = new NativeBuffer(std::move(code));
            if (s_LogCalls) {
                NOTSA_LOG_INFO("D3DX9 AssembleShader propio #{}: len={} flagsEXE=0x{:X} hr=0x00000000 bytecode={} bytes", s_Calls[kAssemble], srcDataLen, flags, static_cast<NativeBuffer*>(*shader)->GetBufferSize());
            }
            return 0;
        }
        static uint32 s_Unsupported = 0;
        if (++s_Unsupported <= 20) NOTSA_LOG_WARN("D3DX9 AssembleShader: el ensamblador propio no cubre este shader ({}); se usa d3dx9_43.dll", why);
    }
    Count(kAssembleDll);
    return AssembleViaDll(srcData, srcDataLen, defines, include, flags, shader, errorMsgs);
}
int32 CompileShaderFromFileA(const char* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    static const auto fn = Get<int32(__stdcall*)(const char*, const void*, void*, const char*, const char*, uint32, void**, void**, void**)>("D3DXCompileShaderFromFileA");
    const int32 hr = fn ? fn(srcFile, defines, include, functionName, target, flags, shader, errorMsgs, constantTable) : kD3DERR_NOTAVAILABLE;
    if (s_LogCalls) NOTSA_LOG_INFO("D3DX9 CompileShaderFromFileA: archivo='{}' funcion='{}' destino='{}' flags=0x{:X} hr=0x{:08X} bytecode={} bytes errores='{}'", Head(srcFile), Head(functionName), Head(target), flags, static_cast<uint32>(hr), BufferSize(shader ? *shader : nullptr), BufferText(errorMsgs ? *errorMsgs : nullptr));
    return hr;
}
int32 CompileShaderFromFileW(const wchar_t* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    static const auto fn = Get<int32(__stdcall*)(const wchar_t*, const void*, void*, const char*, const char*, uint32, void**, void**, void**)>("D3DXCompileShaderFromFileW");
    const int32 hr = fn ? fn(srcFile, defines, include, functionName, target, flags, shader, errorMsgs, constantTable) : kD3DERR_NOTAVAILABLE;
    if (s_LogCalls) NOTSA_LOG_INFO("D3DX9 CompileShaderFromFileW: funcion='{}' destino='{}' flags=0x{:X} hr=0x{:08X} bytecode={} bytes errores='{}'", Head(functionName), Head(target), flags, static_cast<uint32>(hr), BufferSize(shader ? *shader : nullptr), BufferText(errorMsgs ? *errorMsgs : nullptr));
    return hr;
}
int32 CompileShaderFromResourceA(void* srcModule, const char* srcResource, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    static const auto fn = Get<int32(__stdcall*)(void*, const char*, const void*, void*, const char*, const char*, uint32, void**, void**, void**)>("D3DXCompileShaderFromResourceA");
    const int32 hr = fn ? fn(srcModule, srcResource, defines, include, functionName, target, flags, shader, errorMsgs, constantTable) : kD3DERR_NOTAVAILABLE;
    if (s_LogCalls) NOTSA_LOG_INFO("D3DX9 CompileShaderFromResourceA: recurso='{}' funcion='{}' destino='{}' flags=0x{:X} hr=0x{:08X} bytecode={} bytes errores='{}'", Head(srcResource), Head(functionName), Head(target), flags, static_cast<uint32>(hr), BufferSize(shader ? *shader : nullptr), BufferText(errorMsgs ? *errorMsgs : nullptr));
    return hr;
}
} // namespace D3DX9System

// Lote EXT1A: las demas funciones matematicas del EXE (__stdcall, devuelven `out`) llaman a la exportacion de d3dx9_43.dll.
float* __stdcall D3DXFloat16To32Array(float* out, const uint16* in, uint32 count) { return D3DX9System::Float16To32Array(out, in, count); } // 0x76626B
void* __stdcall D3DXVec3TransformCoord(void* out, const void* v, const void* matrix) { return D3DX9System::Vec3TransformCoord(out, v, matrix); } // 0x766A32
void* __stdcall D3DXVec3TransformNormal(void* out, const void* v, const void* matrix) { return D3DX9System::Vec3TransformNormal(out, v, matrix); } // 0x766AB7
void* __stdcall D3DXMatrixMultiply(void* out, const void* a, const void* b) { return D3DX9System::MatrixMultiply(out, a, b); } // 0x7672E1
void* __stdcall D3DXMatrixTranspose(void* out, const void* in) { return D3DX9System::MatrixTranspose(out, in); } // 0x7675EE

// 0x7676C7: D3DXMatrixMultiplyTranspose (`jmp [0x8D74A0]` en el EXE). Rutina propia: reproduce las variantes SSE2/SSE del EXE (misma secuencia), ver D3DX9MatrixMultiplyTransposeSse.h.
void* __stdcall D3DXMatrixMultiplyTranspose(void* out, const void* a, const void* b) {
    return D3DX9System::MatrixMultiplyTranspose(out, a, b);
}

// 0x7652AE: D3DXAssembleShader(pSrcData, SrcDataLen, pDefines, pInclude, Flags, ppShader, ppErrorMsgs). __stdcall, 7 argumentos (`ret 1Ch`). Llama a la exportacion de d3dx9_43.dll.
int32 __stdcall D3DXAssembleShader(const void* srcData, uint32 srcDataLen, const void* defines, void* include, uint32 flags, void** shader, void** errorMsgs) {
    return D3DX9System::AssembleShader(srcData, srcDataLen, defines, include, flags, shader, errorMsgs);
}

// 0x76534D: D3DXCompileShaderFromFileA (9 argumentos, `ret 24h`). Llama a la exportacion de d3dx9_43.dll.
int32 __stdcall D3DXCompileShaderFromFileA(const char* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    return D3DX9System::CompileShaderFromFileA(srcFile, defines, include, functionName, target, flags, shader, errorMsgs, constantTable);
}

// 0x765402: D3DXCompileShaderFromFileW (9 argumentos, `ret 24h`). Llama a la exportacion de d3dx9_43.dll.
int32 __stdcall D3DXCompileShaderFromFileW(const wchar_t* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    return D3DX9System::CompileShaderFromFileW(srcFile, defines, include, functionName, target, flags, shader, errorMsgs, constantTable);
}

// 0x7654B8: D3DXCompileShaderFromResourceA (10 argumentos, `ret 28h`). Llama a la exportacion de d3dx9_43.dll.
int32 __stdcall D3DXCompileShaderFromResourceA(void* srcModule, const char* srcResource, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable) {
    return D3DX9System::CompileShaderFromResourceA(srcModule, srcResource, defines, include, functionName, target, flags, shader, errorMsgs, constantTable);
}
