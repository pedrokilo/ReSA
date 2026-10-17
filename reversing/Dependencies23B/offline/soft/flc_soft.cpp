// g23b: maquina x87 de software. Ver flc_soft.h.
#include "flc_soft.h"
#include <string.h>

// ------------------------------------------------------------------ enteros de 128 bits (hi:lo)
struct FlcU128 { unsigned long long hi, lo; };

static inline int FlcClz64(unsigned long long v) {
    int n = 0;
    if (v == 0) return 64;
    if (!(v >> 32)) { n += 32; v <<= 32; }
    if (!(v >> 48)) { n += 16; v <<= 16; }
    if (!(v >> 56)) { n += 8; v <<= 8; }
    if (!(v >> 60)) { n += 4; v <<= 4; }
    if (!(v >> 62)) { n += 2; v <<= 2; }
    if (!(v >> 63)) { n += 1; }
    return n;
}
static inline FlcU128 FlcMul64(unsigned long long a, unsigned long long b) {
    const unsigned long long a0 = a & 0xFFFFFFFFull, a1 = a >> 32, b0 = b & 0xFFFFFFFFull, b1 = b >> 32;
    const unsigned long long p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
    const unsigned long long mid = (p00 >> 32) + (p01 & 0xFFFFFFFFull) + (p10 & 0xFFFFFFFFull);
    FlcU128 r;
    r.lo = (p00 & 0xFFFFFFFFull) | (mid << 32);
    r.hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
    return r;
}
static inline FlcU128 FlcShl(FlcU128 v, int n) {   // 0 <= n < 128
    if (n == 0) return v;
    if (n >= 64) { v.hi = v.lo << (n - 64); v.lo = 0; return v; }
    v.hi = (v.hi << n) | (v.lo >> (64 - n));
    v.lo <<= n;
    return v;
}
// desplaza a la derecha con "jam": el bit menos significativo recoge todo lo perdido
static inline FlcU128 FlcShrJam(FlcU128 v, int n) {
    if (n <= 0) return v;
    if (n >= 128) { FlcU128 r{0, (v.hi | v.lo) != 0 ? 1ull : 0ull}; return r; }
    FlcU128 r;
    bool lost;
    if (n >= 64) {
        const int s = n - 64;
        lost = v.lo != 0 || (s > 0 && (v.hi & ((1ull << s) - 1)) != 0);
        r.lo = s == 0 ? v.hi : (v.hi >> s);
        r.hi = 0;
    } else {
        lost = (v.lo & ((1ull << n) - 1)) != 0;
        r.lo = (v.lo >> n) | (v.hi << (64 - n));
        r.hi = v.hi >> n;
    }
    if (lost) r.lo |= 1;
    return r;
}
static inline FlcU128 FlcAdd128(FlcU128 a, FlcU128 b, bool* carry) {
    FlcU128 r;
    r.lo = a.lo + b.lo;
    const unsigned long long c0 = r.lo < a.lo ? 1 : 0;
    const unsigned long long t = a.hi + b.hi;
    const bool c1 = t < a.hi;
    r.hi = t + c0;
    *carry = c1 || r.hi < t;
    return r;
}
static inline FlcU128 FlcSub128(FlcU128 a, FlcU128 b) {   // a >= b
    FlcU128 r;
    r.lo = a.lo - b.lo;
    r.hi = a.hi - b.hi - (a.lo < b.lo ? 1 : 0);
    return r;
}
static inline bool FlcGe128(FlcU128 a, FlcU128 b) { return a.hi > b.hi || (a.hi == b.hi && a.lo >= b.lo); }

// ------------------------------------------------------------------ construccion de valores
static inline FlcExt FlcMake(int kind, int neg, unsigned long long sig, int exp) { FlcExt r; r.sig = sig; r.exp = exp; r.kind = (unsigned char)kind; r.neg = (unsigned char)neg; return r; }
static inline FlcExt FlcIndefinite() { return FlcMake(FLC_NAN, 1, 0xC000000000000000ull, 0); }

FlcExt FlcFromFloat(unsigned int bits, unsigned int* flags) {
    const int neg = (int)(bits >> 31);
    const unsigned int e = (bits >> 23) & 0xFF, m = bits & 0x7FFFFF;
    if (e == 0xFF) {
        if (m == 0) return FlcMake(FLC_INF, neg, 1ull << 63, 0);
        if (flags && !(m & 0x400000)) *flags |= FLC_IE;                       // senalizante: IE y sale silenciado
        return FlcMake(FLC_NAN, neg, (1ull << 63) | (((unsigned long long)m | 0x400000ull) << 40), 0);
    }
    if (e == 0) {
        if (m == 0) return FlcMake(FLC_ZERO, neg, 0, 0);
        if (flags) *flags |= FLC_DE;                                          // desnormal: DE y se normaliza
        const int lz = FlcClz64((unsigned long long)m);
        return FlcMake(FLC_NORMAL, neg, (unsigned long long)m << lz, (63 - lz) - 149);
    }
    return FlcMake(FLC_NORMAL, neg, (1ull << 63) | ((unsigned long long)m << 40), (int)e - 127);
}
FlcExt FlcFromDouble(unsigned long long bits, unsigned int* flags) {
    const int neg = (int)(bits >> 63);
    const unsigned int e = (unsigned int)((bits >> 52) & 0x7FF);
    const unsigned long long m = bits & 0xFFFFFFFFFFFFFull;
    if (e == 0x7FF) {
        if (m == 0) return FlcMake(FLC_INF, neg, 1ull << 63, 0);
        if (flags && !(m & 0x8000000000000ull)) *flags |= FLC_IE;
        return FlcMake(FLC_NAN, neg, (1ull << 63) | ((m | 0x8000000000000ull) << 11), 0);
    }
    if (e == 0) {
        if (m == 0) return FlcMake(FLC_ZERO, neg, 0, 0);
        if (flags) *flags |= FLC_DE;
        const int lz = FlcClz64(m);
        return FlcMake(FLC_NORMAL, neg, m << lz, (63 - lz) - 1074);
    }
    return FlcMake(FLC_NORMAL, neg, (1ull << 63) | (m << 11), (int)e - 1023);
}
FlcExt FlcFromInt(int value) {
    if (value == 0) return FlcMake(FLC_ZERO, 0, 0, 0);
    const int neg = value < 0;
    const unsigned long long mag = neg ? (unsigned long long)(-(long long)value) : (unsigned long long)value;
    const int lz = FlcClz64(mag);
    return FlcMake(FLC_NORMAL, neg, mag << lz, 63 - lz);
}
FlcExt FlcFromDoubleExact(double d) {
    unsigned long long b;
    memcpy(&b, &d, 8);
    return FlcFromDouble(b, nullptr);
}

// ------------------------------------------------------------------ redondeo a 24 bits de (hi:lo) normalizado con peso 2^exp en el bit 127
static FlcExt FlcRound24(int neg, FlcU128 v, int exp, unsigned int* state, bool* c1) {
    // v.hi tiene el bit 63 a 1. Se conservan 24 bits.
    unsigned long long keep = v.hi >> 40;
    const unsigned long long rem = v.hi & ((1ull << 40) - 1);
    const bool guard = (rem >> 39) & 1;
    const bool sticky = (rem & ((1ull << 39) - 1)) != 0 || v.lo != 0;
    const bool inexact = rem != 0 || v.lo != 0;
    bool up = false;
    if (guard && (sticky || (keep & 1))) up = true;
    if (up) {
        ++keep;
        if (keep == (1ull << 24)) { keep = 1ull << 23; ++exp; }
    }
    if (inexact) *state |= FLC_PE;
    *c1 = inexact && up;
    return FlcMake(FLC_NORMAL, neg, keep << 40, exp);
}

// NaN: reglas del x87
static FlcExt FlcPickNaN(const FlcExt& a, const FlcExt& b, unsigned int* state) {
    const bool an = a.kind == FLC_NAN, bn = b.kind == FLC_NAN;
    const bool aSig = an && !(a.sig & (1ull << 62)), bSig = bn && !(b.sig & (1ull << 62));
    if (aSig || bSig) *state |= FLC_IE;
    FlcExt r;
    if (an && bn) {
        if (aSig != bSig) r = aSig ? b : a;                  // QNaN con senalizante: gana la silenciosa
        else if (a.sig != b.sig) r = a.sig > b.sig ? a : b;  // misma clase: la de mayor mantisa
        else r = a.neg ? b : a;                              // misma mantisa y signo distinto: gana la positiva (medido en el hardware)
    } else {
        r = an ? a : b;
    }
    r.sig |= (1ull << 62);
    return r;
}

static FlcExt FlcInfOrIndefinite(int neg, bool invalid, unsigned int* state) {
    if (invalid) { *state |= FLC_IE; return FlcIndefinite(); }
    return FlcMake(FLC_INF, neg, 1ull << 63, 0);
}

// ------------------------------------------------------------------ suma
FlcExt FlcAdd(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcPickNaN(a, b, state);
    if (a.kind == FLC_INF || b.kind == FLC_INF) {
        if (a.kind == FLC_INF && b.kind == FLC_INF && a.neg != b.neg) return FlcInfOrIndefinite(0, true, state);
        return a.kind == FLC_INF ? a : b;
    }
    if (a.kind == FLC_ZERO && b.kind == FLC_ZERO) return FlcMake(FLC_ZERO, (a.neg && b.neg) ? 1 : 0, 0, 0);
    if (a.kind == FLC_ZERO || b.kind == FLC_ZERO) {                       // 0 + x: x redondeado a 24 bits
        const FlcExt& o = a.kind == FLC_ZERO ? b : a;
        return FlcRound24(o.neg, FlcU128{o.sig, 0}, o.exp, state, c1);
    }
    // |x| >= |y|
    FlcExt x = a, y = b;
    if (y.exp > x.exp || (y.exp == x.exp && y.sig > x.sig)) { x = b; y = a; }
    const int d = x.exp - y.exp;
    FlcU128 xv{x.sig, 0}, yv{y.sig, 0};
    yv = FlcShrJam(yv, d);
    FlcU128 r;
    int exp = x.exp;
    if (x.neg == y.neg) {
        bool carry;
        r = FlcAdd128(xv, yv, &carry);
        if (carry) { r = FlcShrJam(r, 1); r.hi |= 1ull << 63; ++exp; }
    } else {
        r = FlcSub128(xv, yv);
        if (r.hi == 0 && r.lo == 0) return FlcMake(FLC_ZERO, 0, 0, 0);      // x - x = +0 (redondeo al par)
        const int lz = r.hi ? FlcClz64(r.hi) : 64 + FlcClz64(r.lo);
        r = FlcShl(r, lz);
        exp -= lz;
    }
    return FlcRound24(x.neg, r, exp, state, c1);
}

// ------------------------------------------------------------------ producto
FlcExt FlcMul(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcPickNaN(a, b, state);
    const int neg = a.neg ^ b.neg;
    if (a.kind == FLC_INF || b.kind == FLC_INF) {
        if (a.kind == FLC_ZERO || b.kind == FLC_ZERO) return FlcInfOrIndefinite(0, true, state);
        return FlcMake(FLC_INF, neg, 1ull << 63, 0);
    }
    if (a.kind == FLC_ZERO || b.kind == FLC_ZERO) return FlcMake(FLC_ZERO, neg, 0, 0);
    FlcU128 p = FlcMul64(a.sig, b.sig);
    int exp = a.exp + b.exp;
    if (p.hi >> 63) ++exp; else p = FlcShl(p, 1);
    return FlcRound24(neg, p, exp, state, c1);
}

// ------------------------------------------------------------------ division
FlcExt FlcDiv(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcPickNaN(a, b, state);
    const int neg = a.neg ^ b.neg;
    if (a.kind == FLC_INF) {
        if (b.kind == FLC_INF) return FlcInfOrIndefinite(0, true, state);
        return FlcMake(FLC_INF, neg, 1ull << 63, 0);
    }
    if (b.kind == FLC_INF) return FlcMake(FLC_ZERO, neg, 0, 0);
    if (b.kind == FLC_ZERO) {
        if (a.kind == FLC_ZERO) return FlcInfOrIndefinite(0, true, state);
        *state |= FLC_ZE;
        return FlcMake(FLC_INF, neg, 1ull << 63, 0);
    }
    if (a.kind == FLC_ZERO) return FlcMake(FLC_ZERO, neg, 0, 0);
    // q = floor(a.sig * 2^64 / b.sig): division larga bit a bit (resto de hasta 65 bits)
    FlcU128 q{0, 0};
    unsigned long long r = 0;
    bool rc = false;
    for (int i = 127; i >= 0; --i) {
        const unsigned long long bit = i >= 64 ? ((a.sig >> (i - 64)) & 1) : 0;
        const bool newc = (r >> 63) & 1;
        r = (r << 1) | bit;
        rc = newc;
        q = FlcShl(q, 1);
        if (rc || r >= b.sig) { r -= b.sig; rc = false; q.lo |= 1; }
    }
    const bool remainder = r != 0;
    const int topbit = q.hi ? 64 + (63 - FlcClz64(q.hi)) : (63 - FlcClz64(q.lo));
    q = FlcShl(q, 127 - topbit);
    if (remainder) q.lo |= 1;
    return FlcRound24(neg, q, topbit - 64 + (a.exp - b.exp), state, c1);
}

// ------------------------------------------------------------------ raiz cuadrada
FlcExt FlcSqrt(const FlcExt& a, unsigned int* state, bool* c1) {
    *c1 = false;
    if (a.kind == FLC_NAN) return FlcPickNaN(a, a, state);
    if (a.kind == FLC_ZERO) return a;
    if (a.neg) return FlcInfOrIndefinite(0, true, state);
    if (a.kind == FLC_INF) return a;
    const int e = a.exp;
    // radicando N de 128 bits: sig << 63 si el exponente es par, sig << 64 si es impar; la raiz tiene siempre el bit 63 a 1 y el exponente es e >> 1
    FlcU128 n = (e & 1) ? FlcU128{a.sig, 0} : FlcU128{a.sig >> 1, a.sig << 63};
    FlcU128 res{0, 0}, bit{1ull << 62, 0};
    while (bit.hi || bit.lo) {
        bool c;
        const FlcU128 trial = FlcAdd128(res, bit, &c);
        const FlcU128 half{res.hi >> 1, (res.lo >> 1) | (res.hi << 63)};
        if (FlcGe128(n, trial)) { n = FlcSub128(n, trial); res = FlcAdd128(half, bit, &c); }
        else res = half;
        bit = FlcU128{bit.hi >> 2, (bit.lo >> 2) | (bit.hi << 62)};
    }
    const bool inexact = n.hi != 0 || n.lo != 0;
    return FlcRound24(0, FlcU128{res.lo, inexact ? 1ull : 0ull}, e >> 1, state, c1);
}

// ------------------------------------------------------------------ conversiones de salida
unsigned int FlcToFloat(const FlcExt& v, unsigned int* flags, bool* roundedUp) {
    *roundedUp = false;
    const unsigned int s = (unsigned int)v.neg << 31;
    switch (v.kind) {
    case FLC_ZERO: return s;
    case FLC_INF:  return s | 0x7F800000u;
    case FLC_NAN:  return s | 0x7F800000u | (unsigned int)((v.sig >> 40) & 0x7FFFFFu);
    }
    int e = v.exp;
    const unsigned long long sig = v.sig;
    if (e >= -126) {
        unsigned long long keep = sig >> 40;
        const unsigned long long rem = sig & ((1ull << 40) - 1), half = 1ull << 39;
        const bool inexact = rem != 0;
        const bool up = rem > half || (rem == half && (keep & 1));
        if (up) { ++keep; if (keep == (1ull << 24)) { keep = 1ull << 23; ++e; } }
        if (e > 127) { *flags |= FLC_OE | FLC_PE; *roundedUp = true; return s | 0x7F800000u; }
        if (inexact) { *flags |= FLC_PE; *roundedUp = up; }
        return s | ((unsigned int)(e + 127) << 23) | (unsigned int)(keep & 0x7FFFFF);
    }
    // desnormal: unidad = 2^-149; keep = parte entera de sig * 2^(e + 86)
    const int sh = -e - 86;                                            // >= 41
    unsigned long long keep, rem, half;
    if (sh >= 65) { keep = 0; rem = 1; half = 2; }                     // por debajo de 2^-151: redondea a cero (inexacto)
    else if (sh == 64) { keep = 0; rem = sig; half = 1ull << 63; }
    else { keep = sig >> sh; rem = sig & ((1ull << sh) - 1); half = 1ull << (sh - 1); }
    const bool inexact = rem != 0;
    const bool up = rem > half || (rem == half && (keep & 1));
    if (up) ++keep;
    if (inexact) { *flags |= FLC_UE | FLC_PE; *roundedUp = up; }
    return s | (unsigned int)keep;
}

unsigned long long FlcToDoubleFlags(const FlcExt& v, unsigned int* flags, bool* roundedUp) {
    *roundedUp = false;
    const unsigned long long s = (unsigned long long)v.neg << 63;
    switch (v.kind) {
    case FLC_ZERO: return s;
    case FLC_INF:  return s | 0x7FF0000000000000ull;
    case FLC_NAN:  return s | 0x7FF0000000000000ull | ((v.sig >> 11) & 0xFFFFFFFFFFFFFull);
    }
    int e = v.exp;
    unsigned long long keep, rem, half;
    if (e >= -1022) {
        keep = v.sig >> 11; rem = v.sig & 0x7FF; half = 0x400;
    } else {                                                           // desnormal en double: unidad = 2^-1074
        const int sh = -e - 1022 + 11;                                 // bits que se descartan (> 11)
        if (sh >= 65) { keep = 0; rem = 1; half = 2; }
        else if (sh == 64) { keep = 0; rem = v.sig; half = 1ull << 63; }
        else { keep = v.sig >> sh; rem = v.sig & ((1ull << sh) - 1); half = 1ull << (sh - 1); }
    }
    const bool inexact = rem != 0;
    const bool up = rem > half || (rem == half && (keep & 1));
    if (up) {
        ++keep;
        if (e >= -1022 && keep == (1ull << 53)) { keep = 1ull << 52; ++e; }
    }
    if (inexact) { *flags |= FLC_PE | (e < -1022 ? FLC_UE : 0); *roundedUp = up; }
    if (e > 1023) { *flags |= FLC_OE | FLC_PE; *roundedUp = true; return s | 0x7FF0000000000000ull; }
    if (e < -1022) return s | keep;                                    // desnormal (keep ya es la mantisa)
    return s | ((unsigned long long)(e + 1023) << 52) | (keep & 0xFFFFFFFFFFFFFull);
}
unsigned long long FlcToDouble(const FlcExt& v) {
    unsigned int f = 0;
    bool up;
    return FlcToDoubleFlags(v, &f, &up);
}
double FlcToDoubleValue(const FlcExt& v) {
    const unsigned long long b = FlcToDouble(v);
    double d;
    memcpy(&d, &b, 8);
    return d;
}

unsigned int FlcCompare(const FlcExt& a, const FlcExt& b, unsigned int* state, bool unorderedSignals) {
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) {
        const bool aSig = a.kind == FLC_NAN && !(a.sig & (1ull << 62)), bSig = b.kind == FLC_NAN && !(b.sig & (1ull << 62));
        if (unorderedSignals || aSig || bSig) *state |= FLC_IE;
        return FLC_C0 | FLC_C2 | FLC_C3;
    }
    // orden total sobre finitos e infinitos
    auto cmp = [](const FlcExt& x, const FlcExt& y) -> int {
        auto sign = [](const FlcExt& v) { return v.kind == FLC_ZERO ? 0 : (v.neg ? -1 : 1); };
        const int sx = sign(x), sy = sign(y);
        if (sx != sy) return sx < sy ? -1 : 1;
        if (sx == 0) return 0;
        int mag;                                                       // |x| frente a |y|
        if (x.kind == FLC_INF || y.kind == FLC_INF) mag = (x.kind == FLC_INF ? 1 : 0) - (y.kind == FLC_INF ? 1 : 0);
        else if (x.exp != y.exp) mag = x.exp > y.exp ? 1 : -1;
        else mag = x.sig == y.sig ? 0 : (x.sig > y.sig ? 1 : -1);
        return sx > 0 ? mag : -mag;
    };
    const int c = cmp(a, b);
    return c < 0 ? FLC_C0 : (c == 0 ? FLC_C3 : 0);
}
bool FlcSameBits(const FlcExt& a, const FlcExt& b) {
    if (a.kind != b.kind || a.neg != b.neg) return false;
    if (a.kind == FLC_ZERO || a.kind == FLC_INF) return true;
    return a.sig == b.sig && (a.kind == FLC_NAN || a.exp == b.exp);
}

// ================================================================== la maquina
#include <stdio.h>
int gFlcTrace = 0;
static unsigned int gFlcLastState = 0;
static void FlcTraceState(const FlcX87& m, const char* what) {
    if (!gFlcTrace) return;
    if (m.state != gFlcLastState) { printf("   [maquina] %s -> estado %02x (antes %02x) prof %d\n", what, m.state, gFlcLastState, m.depth); gFlcLastState = m.state; }
}
FlcX87::FlcX87() : depth(0), maxDepth(0), state(0), cc(0), ccValid(false), c1(false), lastClass(0), lastFloat(0.0f), pendingDE(0) {
    memset(reg, 0, sizeof(reg));
    memset(written, 0, sizeof(written));
}
void FlcX87::Push(const FlcExt& v) {
    reg[depth] = v;
    written[depth] = true;
    ++depth;
    if (depth > maxDepth) maxDepth = depth;
    c1 = false;
}
void FlcX87::Write(int i, const FlcExt& v) {
    reg[depth - 1 - i] = v;
    written[depth - 1 - i] = true;
}
static FlcExt FlcMemFloat(unsigned int bits, unsigned int* pendingDE) {
    unsigned int f = 0;
    FlcExt e = FlcFromFloat(bits, &f);
    *pendingDE |= f & FLC_DE;
    if (e.kind == FLC_NAN && !(bits & 0x400000u)) e.sig &= ~(1ull << 62);   // el operando de memoria senalizante conserva su clase hasta resolver el NaN
    return e;
}
static FlcExt FlcMemDouble(unsigned long long bits, unsigned int* pendingDE) {
    unsigned int f = 0;
    FlcExt e = FlcFromDouble(bits, &f);
    *pendingDE |= f & FLC_DE;
    if (e.kind == FLC_NAN && !(bits & 0x8000000000000ull)) e.sig &= ~(1ull << 62);
    return e;
}
FlcExt FlcX87::MemOperand(const float& m) { unsigned int b; memcpy(&b, &m, 4); return FlcMemFloat(b, &pendingDE); }
FlcExt FlcX87::MemOperandD(const double& m) { unsigned long long b; memcpy(&b, &m, 8); return FlcMemDouble(b, &pendingDE); }

void FlcX87::Fld(const float& m) { unsigned int b, f = 0; memcpy(&b, &m, 4); const FlcExt e = FlcFromFloat(b, &f); state |= f; Push(e); lastClass = 3; FlcTraceState(*this, "Fld"); }
void FlcX87::FldRet(const float& m) { unsigned int b, f = 0; memcpy(&b, &m, 4); Push(FlcFromFloat(b, &f)); lastClass = 3; }
void FlcX87::FldD(const double& m) { unsigned long long b; unsigned int f = 0; memcpy(&b, &m, 8); const FlcExt e = FlcFromDouble(b, &f); state |= f; Push(e); lastClass = 3; }
void FlcX87::FldI(int value) { Push(FlcFromInt(value)); lastClass = 3; }
void FlcX87::FldSt(int i) { const FlcExt e = S(i); Push(e); lastClass = 3; }
void FlcX87::FldZero() { Push(FlcMake(FLC_ZERO, 0, 0, 0)); lastClass = 3; }
void FlcX87::FldExt(const FlcExt& v) { Push(v); lastClass = 3; }
void FlcX87::Fst(float& m) {
    bool up;
    const unsigned int b = FlcToFloat(Top(), &state, &up);
    memcpy(&m, &b, 4);
    c1 = up;
    lastClass = 3;
}
void FlcX87::Fstp(float& m) {
    bool up;
    const unsigned int b = FlcToFloat(Top(), &state, &up);
    memcpy(&m, &b, 4);
    c1 = up;
    --depth;
    lastClass = 2;
}
void FlcX87::FstpSt(int i) {
    if (i != 0) Write(i, Top());
    --depth;
    c1 = false;
    lastClass = 3;
}
void FlcX87::FstSt(int i) { if (i != 0) Write(i, Top()); c1 = false; lastClass = 3; }
void FlcX87::Pop() { --depth; c1 = false; lastClass = 3; }
void FlcX87::Fxch(int i) { const FlcExt t = Top(); Write(0, S(i)); Write(i, t); c1 = false; lastClass = 3; }
void FlcX87::Fchs() { FlcExt v = Top(); v.neg ^= 1; Write(0, v); c1 = false; lastClass = 3; }
void FlcX87::Fabs() { FlcExt v = Top(); v.neg = 0; Write(0, v); c1 = false; lastClass = 3; }

static FlcExt FlcSub(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1) {
    if (a.kind == FLC_NAN || b.kind == FLC_NAN) return FlcAdd(a, b, state, c1);   // el NaN sale sin cambiar de signo
    FlcExt nb = b;
    nb.neg ^= 1;
    return FlcAdd(a, nb, state, c1);
}
void FlcX87::ArithReg(char op, FlcExt a, FlcExt b, int dest, bool pop, bool) {
    bool up = false;
    const unsigned int de = (a.kind == FLC_NAN || b.kind == FLC_NAN) ? 0 : pendingDE;
    pendingDE = 0;
    unsigned int local = 0;
    FlcExt r;
    switch (op) {
    case '+': r = FlcAdd(a, b, &local, &up); break;
    case '-': r = FlcSub(a, b, &local, &up); break;
    case '*': r = FlcMul(a, b, &local, &up); break;
    default:  r = FlcDiv(a, b, &local, &up); break;
    }
    if (!(local & (FLC_IE | FLC_ZE))) local |= de;                         // IE y ZE tienen prioridad sobre DE
    state |= local;
    FlcTraceState(*this, "Arith");
    Write(dest, r);
    c1 = up;
    if (pop) --depth;
    lastClass = 3;
}
void FlcX87::Arith(char op, int dest, int srcA, int srcB, bool pop) { ArithReg(op, S(srcA), S(srcB), dest, pop, false); }
// ST0 = ST0 op m
void FlcX87::Fadd(const float& m)  { ArithReg('+', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fsub(const float& m)  { ArithReg('-', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fsubr(const float& m) { ArithReg('-', MemOperand(m), Top(), 0, false, true); }
void FlcX87::Fmul(const float& m)  { ArithReg('*', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fdiv(const float& m)  { ArithReg('/', Top(), MemOperand(m), 0, false, true); }
void FlcX87::Fdivr(const float& m) { ArithReg('/', MemOperand(m), Top(), 0, false, true); }
void FlcX87::FaddD(const double& m)  { ArithReg('+', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FsubD(const double& m)  { ArithReg('-', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FsubrD(const double& m) { ArithReg('-', MemOperandD(m), Top(), 0, false, true); }
void FlcX87::FmulD(const double& m)  { ArithReg('*', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FdivD(const double& m)  { ArithReg('/', Top(), MemOperandD(m), 0, false, true); }
void FlcX87::FdivrD(const double& m) { ArithReg('/', MemOperandD(m), Top(), 0, false, true); }
void FlcX87::FimulI(int v) { ArithReg('*', Top(), FlcFromInt(v), 0, false, true); }
void FlcX87::FidivI(int v) { ArithReg('/', Top(), FlcFromInt(v), 0, false, true); }
void FlcX87::FiaddI(int v) { ArithReg('+', Top(), FlcFromInt(v), 0, false, true); }

void FlcX87::FcomMem(const float& m, bool pop) {
    const FlcExt b = MemOperand(m);
    if (!(Top().kind == FLC_NAN || b.kind == FLC_NAN)) state |= pendingDE;
    pendingDE = 0;
    cc = FlcCompare(Top(), b, &state, true);
    ccValid = true;
    c1 = false;
    lastClass = pop ? 1 : 3;
    if (pop) --depth;
}
void FlcX87::FcomSt(int i, bool pop) {
    cc = FlcCompare(Top(), S(i), &state, true);
    ccValid = true;
    c1 = false;
    lastClass = 3;
    if (pop) --depth;
}
void FlcX87::Fcompp() {
    cc = FlcCompare(Top(), S(1), &state, true);
    ccValid = true;
    c1 = false;
    lastClass = 3;
    depth -= 2;
}
void FlcX87::Fsqrt() {
    bool up;
    const FlcExt r = FlcSqrt(Top(), &state, &up);
    Write(0, r);
    c1 = up;
    lastClass = 3;
}
unsigned int FlcX87::Ah() const { return (cc | (c1 ? FLC_C1 : 0)) >> 8; }
float FlcX87::StFloat(int i) {
    bool up;
    unsigned int f = 0;
    const unsigned int b = FlcToFloat(St(i), &f, &up);
    float r;
    memcpy(&r, &b, 4);
    return r;
}

FlcExt FlcFromInt64(long long value) {
    if (value == 0) return FlcMake(FLC_ZERO, 0, 0, 0);
    const int neg = value < 0;
    const unsigned long long mag = neg ? (0ull - (unsigned long long)value) : (unsigned long long)value;
    const int lz = FlcClz64(mag);
    return FlcMake(FLC_NORMAL, neg, mag << lz, 63 - lz);
}
void FlcX87::FldOne() { Push(FlcMake(FLC_NORMAL, 0, 1ull << 63, 0)); lastClass = 3; }
void FlcX87::FldI64(long long v) { Push(FlcFromInt64(v)); lastClass = 3; }
void FlcX87::FstD(double& m) {
    bool up;
    const unsigned long long out = FlcToDoubleFlags(Top(), &state, &up);
    memcpy(&m, &out, 8);
    c1 = up;
    lastClass = 3;
}
void FlcX87::FstpD(double& m) { FstD(m); --depth; }
void FlcX87::Fistp64(long long& m) {
    const FlcExt v = Top();
    unsigned long long out = 0x8000000000000000ull;
    bool inexact = false, up = false, invalid = false;
    if (v.kind == FLC_NAN || v.kind == FLC_INF) invalid = true;
    else if (v.kind == FLC_ZERO) out = 0;
    else {
        if (v.exp >= 63) {
            // |v| >= 2^63: solo -2^63 exacto es valido
            if (v.neg && v.exp == 63 && v.sig == (1ull << 63)) out = 0x8000000000000000ull; else invalid = true;
        } else if (v.exp < -1) {
            // |v| < 0.5: redondea a cero (inexacto)
            out = 0; inexact = true; up = false;
        } else {
            const int sh = 63 - v.exp;                    // bits fraccionarios en sig (>= 1)
            unsigned long long ip, rem, half;
            if (sh >= 64) { ip = 0; rem = v.sig; half = 1ull << 63; }
            else { ip = v.sig >> sh; rem = v.sig & ((1ull << sh) - 1); half = 1ull << (sh - 1); }
            inexact = rem != 0;
            up = rem > half || (rem == half && (ip & 1));
            if (up) ++ip;
            out = v.neg ? (0ull - ip) : ip;
        }
    }
    if (invalid) state |= FLC_IE;
    else if (inexact) state |= FLC_PE;
    m = (long long)out;
    c1 = !invalid && inexact && up;
    --depth;
    lastClass = 3;
}

// ---- fsin / fcos / fpatan: el resultado lo calcula el x87 real (53 bits de mantisa aqui)
static bool FlcOutOfTrigRange(const FlcExt& v) { return v.kind == FLC_NORMAL && v.exp >= 63; }
void FlcX87::Fsin() {
    const FlcExt v = Top();
    FlcExt r = v;
    if (v.kind == FLC_NAN) { r.sig |= (1ull << 62); }
    else if (v.kind == FLC_INF) { state |= FLC_IE; r = FlcMake(FLC_NAN, 1, 0xC000000000000000ull, 0); }
    else if (v.kind == FLC_ZERO) {}
    else if (FlcOutOfTrigRange(v)) { cc |= FLC_C2; }
    else { r = FlcFromDoubleExact(FlcHwSin(FlcToDoubleValue(v))); state |= FLC_PE; cc &= ~FLC_C2; }
    Write(0, r);
    c1 = false;
    lastClass = 3;
}
void FlcX87::Fcos() {
    const FlcExt v = Top();
    FlcExt r = v;
    if (v.kind == FLC_NAN) { r.sig |= (1ull << 62); }
    else if (v.kind == FLC_INF) { state |= FLC_IE; r = FlcMake(FLC_NAN, 1, 0xC000000000000000ull, 0); }
    else if (v.kind == FLC_ZERO) { r = FlcMake(FLC_NORMAL, 0, 1ull << 63, 0); }
    else if (FlcOutOfTrigRange(v)) { cc |= FLC_C2; }
    else { r = FlcFromDoubleExact(FlcHwCos(FlcToDoubleValue(v))); state |= FLC_PE; cc &= ~FLC_C2; }
    Write(0, r);
    c1 = false;
    lastClass = 3;
}
void FlcX87::Fpatan() {
    const FlcExt x = S(0), y = S(1);                         // atan2(ST1, ST0)
    FlcExt r;
    if (x.kind == FLC_NAN || y.kind == FLC_NAN) r = FlcPickNaN(y, x, &state);
    else {
        r = FlcFromDoubleExact(FlcHwAtan2(FlcToDoubleValue(y), FlcToDoubleValue(x)));
        if (r.kind != FLC_ZERO) state |= FLC_PE;
    }
    Write(1, r);
    --depth;
    c1 = false;
    lastClass = 3;
}
