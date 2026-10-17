// g23b: funciones auxiliares pequenas del EXE como secuencias x87 de la maquina FlcX87 (ver flc_macros.h).
#include "flc_macros.h"
#include <string.h>

#include <math.h>
static void FlcSetExeErrno(int value) {
    using ExeErrnoFn = int*(__cdecl*)();
    *reinterpret_cast<ExeErrnoFn>(0x0082B181)() = value;   // _errno() del EXE
}
FlcWorld FlcReadWorld() {
    FlcWorld w;
    w.sse2Pow = *reinterpret_cast<const int*>(0x00C9C400);
    w.matherrFlag = *reinterpret_cast<const int*>(0x00C9ABFC);
    return w;
}

// ======================================================================================================================== CVector
void FlcVecAdd(FlcX87& fp, float* out, const float* a, const float* b) {
    fp.Fld(a[2]); fp.Fadd(b[2]);
    fp.Fld(a[1]); fp.Fadd(b[1]);
    fp.Fld(a[0]); fp.Fadd(b[0]);
    fp.Fstp(out[0]); fp.Fstp(out[1]); fp.Fstp(out[2]);
}
void FlcVecSub(FlcX87& fp, float* out, const float* a, const float* b) {
    fp.Fld(a[2]); fp.Fsub(b[2]);
    fp.Fld(a[1]); fp.Fsub(b[1]);
    fp.Fld(a[0]); fp.Fsub(b[0]);
    fp.Fstp(out[0]); fp.Fstp(out[1]); fp.Fstp(out[2]);
}
void FlcVecMulLeft(FlcX87& fp, float* out, const float& f, const float* v) {
    fp.Fld(f); fp.Fmul(v[2]);
    fp.Fld(f); fp.Fmul(v[1]);
    fp.Fld(f); fp.Fmul(v[0]);
    fp.Fstp(out[0]); fp.Fstp(out[1]); fp.Fstp(out[2]);
}
void FlcVecMulRight(FlcX87& fp, float* out, const float* v, const float& f) { FlcVecMulLeft(fp, out, f, v); }   // 0x40FEC0: misma secuencia (f * v.z, f * v.y, f * v.x)
void FlcVecScale(FlcX87& fp, float* v, const float& f) {
    fp.Fld(f); fp.Fmul(v[0]); fp.Fstp(v[0]);
    fp.Fld(f); fp.Fmul(v[1]); fp.Fstp(v[1]);
    fp.Fld(f); fp.Fmul(v[2]); fp.Fstp(v[2]);
}
void FlcVecAddTo(FlcX87& fp, float* v, const float* a) {
    fp.Fld(a[0]); fp.Fadd(v[0]); fp.Fstp(v[0]);
    fp.Fld(a[1]); fp.Fadd(v[1]); fp.Fstp(v[1]);
    fp.Fld(a[2]); fp.Fadd(v[2]); fp.Fstp(v[2]);
}
static void FlcSumSquaresLeaveSt0(FlcX87& fp, const float* v) {      // 0x4082C0 / 0x406DA0 hasta el segundo faddp: ST0 = (xx + yy) + zz ; ST1 = x ; ST2 = y ; ST3 = z
    fp.Fld(v[2]); fp.Fld(v[1]); fp.Fld(v[0]);
    fp.FldSt(0); fp.Arith('*', 0, 0, 1, false);
    fp.FldSt(2); fp.Arith('*', 0, 0, 3, false);
    fp.Arith('+', 1, 1, 0, true);
    fp.FldSt(3); fp.Arith('*', 0, 0, 4, false);
    fp.Arith('+', 1, 1, 0, true);
}
void FlcVecMagnitude(FlcX87& fp, const float* v) {
    FlcSumSquaresLeaveSt0(fp, v);
    fp.Fsqrt();
    fp.FstpSt(3); fp.Pop(); fp.Pop();
}
void FlcVecMagnitudeSquared(FlcX87& fp, const float* v) {
    FlcSumSquaresLeaveSt0(fp, v);
    fp.FstpSt(3); fp.Pop(); fp.Pop();
}
void FlcVecDot(FlcX87& fp, const float* a, const float* b) {
    fp.Fld(a[2]); fp.Fmul(b[2]);
    fp.Fld(a[1]); fp.Fmul(b[1]);
    fp.Arith('+', 1, 1, 0, true);
    fp.Fld(a[0]); fp.Fmul(b[0]);
    fp.Arith('+', 1, 1, 0, true);
}
static const float kFlcZero = 0.0f;
static const float kFlcOneF = 1.0f;
void FlcVecNormalise(FlcX87& fp, float* v) {
    FlcSumSquaresLeaveSt0(fp, v);                                    // ST0 = suma de cuadrados
    fp.FstpSt(3); fp.Pop(); fp.Pop();
    fp.FcomMem(kFlcZero, false);                                     // fcom (sin sacar)
    const unsigned ah = fp.Ah();
    const bool parityEven = (((ah & 0x41) == 0) || ((ah & 0x41) == 0x41));
    if (!parityEven) {                                               // suma <= 0
        fp.Pop();
        unsigned int one = 0x3F800000u;
        memcpy(&v[0], &one, 4);
        return;
    }
    fp.Fsqrt();
    fp.Fdivr(kFlcOneF);
    fp.FldSt(0); fp.Fmul(v[0]); fp.Fstp(v[0]);
    fp.FldSt(0); fp.Fmul(v[1]); fp.Fstp(v[1]);
    fp.Fmul(v[2]); fp.Fstp(v[2]);
}
void FlcVecCross(FlcX87& fp, float* out, const float* a, const float* b) {
    float t[3];
    fp.Fld(b[2]); fp.Fmul(a[1]);
    fp.Fld(a[2]); fp.Fmul(b[1]);
    fp.Arith('-', 1, 1, 0, true); fp.Fstp(t[0]);
    fp.Fld(a[2]); fp.Fmul(b[0]);
    fp.Fld(b[2]); fp.Fmul(a[0]);
    fp.Arith('-', 1, 1, 0, true); fp.Fstp(t[1]);
    fp.Fld(a[0]); fp.Fmul(b[1]);
    fp.Fld(b[0]); fp.Fmul(a[1]);
    fp.Arith('-', 1, 1, 0, true); fp.Fstp(t[2]);
    memcpy(out, t, 12);
}
void FlcMatMul3x3(FlcX87& fp, float* out, const float* m, const float* v) {
    float t[3];
    fp.Fld(m[8]);  fp.Fmul(v[2]); fp.Fld(m[4]);  fp.Fmul(v[1]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[0]); fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[0]);
    fp.Fld(m[9]);  fp.Fmul(v[2]); fp.Fld(m[1]);  fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[5]); fp.Fmul(v[1]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[1]);
    fp.Fld(m[10]); fp.Fmul(v[2]); fp.Fld(m[2]);  fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[6]); fp.Fmul(v[1]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[2]);
    memcpy(out, t, 12);
}
void FlcMatInvMul3x3(FlcX87& fp, float* out, const float* m, const float* v) {
    float t[3];
    fp.Fld(m[1]); fp.Fmul(v[1]); fp.Fld(m[2]); fp.Fmul(v[2]); fp.Arith('+', 1, 1, 0, true); fp.Fld(v[0]); fp.Fmul(m[0]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[0]);
    fp.Fld(m[5]); fp.Fmul(v[1]); fp.Fld(m[4]); fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[6]); fp.Fmul(v[2]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[1]);
    fp.Fld(m[9]); fp.Fmul(v[1]); fp.Fld(m[8]); fp.Fmul(v[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(m[10]); fp.Fmul(v[2]); fp.Arith('+', 1, 1, 0, true); fp.Fstp(t[2]);
    memcpy(out, t, 12);
}
void FlcFloatMax(FlcX87& fp, const float& a, const float& b) {      // 0x420800: fld a; fcomp b; test ah,0x41; jne -> ST0 = b ; si no ST0 = a
    fp.Fld(a);
    fp.FcomMem(b, true);
    if (fp.Ah() & 0x41) fp.Fld(b); else fp.Fld(a);
}
void FlcFloatMin(FlcX87& fp, const float& a, const float& b) {      // 0x404330: fld a; fcomp b; test ah,5; jp -> ST0 = b ; si no ST0 = a
    fp.Fld(a);
    fp.FcomMem(b, true);
    const unsigned ah = fp.Ah();
    const bool parityEven = (((ah & 5) == 0) || ((ah & 5) == 5));
    if (parityEven) fp.Fld(b); else fp.Fld(a);
}

// ======================================================================================================================== CRT
static unsigned int FlcHighDword(const FlcExt& x) { return (unsigned int)(FlcToDouble(x) >> 32); }

// Cola comun de _CIasin / _CIacos tras calcular el resultado (0x821EB6 / 0x8223C8 / 0x821F1C / 0x82242C): si el CRT comprueba las excepciones (matherrFlag == 0)
// y el x87 lleva PE, 0x828EA7 guarda ST0 como qword y lo recarga (redondeo a 53 bits); con el dominio invalido ademas pone IE y el errno del EXE.
static void FlcMathTail(FlcX87& fp, bool domainError, bool nanInput) {
    const FlcWorld w = FlcReadWorld();
    if (w.matherrFlag != 0) return;
    if (!domainError && !nanInput) {
        if (!((FlcHwSticky() | fp.state) & FLC_PE)) return;
    }
    double d;
    fp.FstD(d);
    if (domainError) { fp.state |= FLC_IE; }
    if (domainError || nanInput) FlcSetExeErrno(33);
    fp.Pop();
    fp.FldD(d);
}
void FlcCrtAsin(FlcX87& fp) {
    const FlcExt x = fp.St(0);
    const unsigned int hi = FlcHighDword(x);
    const unsigned int expo = hi & 0x7FF00000u;
    if (x.kind == FLC_NAN) { FlcMathTail(fp, false, true); return; }
    if (x.kind == FLC_INF || expo > 0x3FF00000u || (expo == 0x3FF00000u && (FlcToDouble(x) & 0xFFFFFFFFFFFFFull) != 0)) {
        fp.Pop();
        fp.FldExt(FlcExt{0xC000000000000000ull, 0, FLC_NAN, 1});            // indefinido real (0x8E3130)
        FlcMathTail(fp, true, false);
        return;
    }
    if (expo == 0x3FF00000u) {                                              // +-1 exacto: +-pi/2 de 64 bits (0x8E313A)
        const int neg = x.neg;
        fp.Pop();
        fp.FldExt(FlcExt{0xC90FDAA22168C235ull, 0, FLC_NORMAL, (unsigned char)neg});
        FlcMathTail(fp, false, false);
        return;
    }
    fp.FldOne(); fp.Arith('+', 0, 0, 1, false);                             // 1 + x
    fp.FldOne(); fp.Arith('-', 0, 0, 2, false);                             // 1 - x
    fp.Arith('*', 1, 1, 0, true);
    fp.Fsqrt();
    fp.Fpatan();                                                            // atan2(x, raiz)
    FlcMathTail(fp, false, false);
}
void FlcCrtAcos(FlcX87& fp) {
    const FlcExt x = fp.St(0);
    const unsigned int hi = FlcHighDword(x);
    const unsigned int expo = hi & 0x7FF00000u;
    if (x.kind == FLC_NAN) { FlcMathTail(fp, false, true); return; }
    if (x.kind == FLC_INF || expo > 0x3FF00000u || (expo == 0x3FF00000u && (FlcToDouble(x) & 0xFFFFFFFFFFFFFull) != 0)) {
        fp.Pop();
        fp.FldExt(FlcExt{0xC000000000000000ull, 0, FLC_NAN, 1});
        FlcMathTail(fp, true, false);
        return;
    }
    if (expo == 0x3FF00000u) {                                              // +-1 exacto: 0 (fldz) o pi (fldpi)
        const int neg = x.neg;
        fp.Pop();
        if (neg) fp.FldExt(FlcExt{0xC90FDAA22168C235ull, 1, FLC_NORMAL, 0}); else fp.FldZero();
        FlcMathTail(fp, false, false);
        return;
    }
    fp.FldOne(); fp.Arith('+', 0, 0, 1, false);
    fp.FldOne(); fp.Arith('-', 0, 0, 2, false);
    fp.Arith('*', 1, 1, 0, true);
    fp.Fsqrt();
    fp.Fxch(1);
    fp.Fpatan();                                                            // atan2(raiz, x)
    FlcMathTail(fp, false, false);
}

// _CIpow con la rama SSE2 del CRT del EXE (0x829B80): fxch; fstp qword; fstp qword; call 0x829B99 (pow en SSE2, argumentos double en la pila, resultado en ST0 por fld qword); la rama SSE2 se toma si
// [0xC9C400] != 0 y las excepciones del x87 y del SSE estan enmascaradas. El nucleo SSE2 (tablas de pow, MXCSR y errno incluidos) es codigo de la CRT del EXE y se llama por su direccion:
// reproducirlo en C++ exigiria copiar sus tablas (0x8884F0..0x88A540) y dejaria las banderas de MXCSR y errno distintas en los casos limite.
void FlcCrtPow(FlcX87& fp) {
    fp.Fxch(1);
    double base, exponent;
    fp.FstpD(base);
    fp.FstpD(exponent);
    using PowCore = double(__cdecl*)(double, double);
    const double result = reinterpret_cast<PowCore>(0x00829B99)(base, exponent);
    fp.FldD(result);
}

int FlcCrtFtol2(FlcX87& fp) {
    float f18;
    long long r;
    fp.FldSt(0);
    fp.Fst(f18);
    fp.Fistp64(r);
    fp.FldI64(r);
    unsigned int edx, eax = (unsigned int)(unsigned long long)r;
    memcpy(&edx, &f18, 4);
    unsigned int hi = (unsigned int)((unsigned long long)r >> 32);
    if (eax == 0 && (hi & 0x7FFFFFFFu) == 0) {                               // 0x821B9F: R == 0 (o R = -2^63 con hi...)
        // el EXE saca R y v con dos fstp dword
        float sink;
        fp.Fstp(sink);
        fp.Fstp(sink);
        return 0;
    }
    fp.Arith('-', 1, 1, 0, true);                                            // fsubp: v - R
    float frac;
    fp.Fstp(frac);
    unsigned int fracBits;
    memcpy(&fracBits, &frac, 4);
    if ((int)edx < 0) {                                                       // v negativo: xor ecx,0x80000000; add ecx,0x7FFFFFFF; adc eax,0; adc edx,0
        const unsigned int t = fracBits ^ 0x80000000u;
        const unsigned int ecx = t + 0x7FFFFFFFu;
        const unsigned int carry = ecx < t ? 1u : 0u;
        return (int)(eax + carry);
    }
    const unsigned int ecx = fracBits + 0x7FFFFFFFu;                          // add ecx,0x7FFFFFFF; sbb eax,0; sbb edx,0
    const unsigned int carry = ecx < fracBits ? 1u : 0u;
    return (int)(eax - carry);
}
