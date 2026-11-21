#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <cstring>
#include "CrtFloatIn.h"
#include "CrtCtype.h"

// Reconstruccion de la conversion de texto a coma flotante del CRT del EXE (lote CRT4A). Ver CrtFloatIn.h y reversing/FnCRT4A/README.md.
// Las etiquetas de goto de _strgtold12 llevan las direcciones del EXE para poder cotejar el flujo con el desensamblado.

namespace ExeCrt {
namespace {
} // namespace

// 0x832875: 1 si todos los bits posteriores a la posicion 'bit' (contando desde el bit mas alto de w[0]) son cero
uint32_t __cdecl Ld12TailZero(const uint32_t* w, int bit) {
    int q = bit / 32;
    const int rem = bit % 32;
    const uint32_t mask = ~(0xFFFFFFFFu << (31 - rem));
    if (w[q] & mask) {
        return 0;
    }
    for (++q; q < 3; ++q) {
        if (w[q] != 0) {
            return 0;
        }
    }
    return 1;
}

// 0x8328A7: suma 1 en la posicion 'bit' y propaga el acarreo hacia los dwords altos; devuelve el acarreo que sale por arriba
uint32_t __cdecl Ld12AddOne(uint32_t* w, int bit) {
    int q = bit / 32;
    const int rem = bit % 32;
    uint32_t carry = Addl(w[q], 1u << (31 - rem), &w[q]);
    --q;
    while (q >= 0 && carry) {
        carry = Addl(w[q], 1, &w[q]);
        --q;
    }
    return carry;
}

// 0x8328F4: redondea a 'bit' bits (empate exacto no sube) y borra lo que sigue; devuelve el acarreo de salida
int __cdecl Ld12Round(uint32_t* w, int bit) {
    int ret = 0;
    int q = bit / 32;
    const int rem = bit % 32;
    const int shift = 31 - rem;
    if (w[q] & (1u << shift)) {
        if (!Ld12TailZero(w, bit)) {
            ret = static_cast<int>(Ld12AddOne(w, bit - 1));
        }
    }
    w[q] &= (0xFFFFFFFFu << shift);
    for (++q; q < 3; ++q) {
        w[q] = 0;
    }
    return ret;
}

// 0x832966
void __cdecl Ld12Copy(uint32_t* dst, const uint32_t* src) {
    dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
}

// 0x832981
void __cdecl Ld12Zero(uint32_t* dst) {
    dst[0] = 0; dst[1] = 0; dst[2] = 0;
}

// 0x83298D
int __cdecl Ld12IsZero(const uint32_t* w) {
    return (w[0] == 0 && w[1] == 0 && w[2] == 0) ? 1 : 0;
}

// 0x8329A6: desplaza los 96 bits (w[0] es la parte alta) 'bits' posiciones a la derecha
void __cdecl Ld12ShiftRight(uint32_t* w, int bits) {
    const int q = bits / 32;
    const int rem = bits % 32;
    const uint32_t lowMask = ~(0xFFFFFFFFu << rem);
    uint32_t carry = 0;
    for (int i = 0; i < 3; ++i) {
        const uint32_t v = w[i];
        const uint32_t low = v & lowMask;
        w[i] = (v >> rem) | carry;
        carry = rem == 0 ? 0u : (low << (32 - rem));
    }
    for (int i = 2; i >= 0; --i) {
        w[i] = (i >= q) ? w[i - q] : 0u;
    }
}

// 0x832A21: ld12 -> IEEE (double: 0x8E39B8; float: 0x8E39D0). Devuelve 0 normal, 1 desbordamiento (infinito), 2 cero o denormal
int __cdecl Ld12Cvt(const Ld12* in, uint32_t* out, const FloatFormat* fmt) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(in);
    const uint32_t se = Load16(p + 10);
    const uint32_t sign = se & 0x8000;
    int exponent = static_cast<int>(se & 0x7FFF) - 0x3FFF;
    uint32_t w[3];
    w[0] = Load32(p + 6);
    w[1] = Load32(p + 2);
    w[2] = static_cast<uint32_t>(Load16(p)) << 16;
    uint32_t tmp[3];
    uint32_t biased = 0;
    int status = 0;

    if (exponent == -0x3FFF) {
        biased = 0;
        if (!Ld12IsZero(w)) {
            w[0] = 0; w[1] = 0; w[2] = 0;
            status = 2;
        }
    } else {
        Ld12Copy(tmp, w);
        if (Ld12Round(w, fmt->mantBits)) {
            ++exponent;
        }
        if (exponent < fmt->minExp - fmt->mantBits) {
            w[0] = 0; w[1] = 0; w[2] = 0;
            biased = 0;
            status = 2;
        } else if (exponent <= fmt->minExp) {
            Ld12Copy(w, tmp);
            Ld12ShiftRight(w, fmt->minExp - exponent);
            Ld12Round(w, fmt->mantBits);
            Ld12ShiftRight(w, fmt->expBits + 1);
            biased = 0;
            status = 2;
        } else if (exponent >= fmt->maxExp) {
            w[0] = 0x80000000u; w[1] = 0; w[2] = 0;
            Ld12ShiftRight(w, fmt->expBits);
            biased = static_cast<uint32_t>(fmt->bias + fmt->maxExp);
            status = 1;
        } else {
            biased = static_cast<uint32_t>(fmt->bias + exponent);
            w[0] &= 0x7FFFFFFFu;
            Ld12ShiftRight(w, fmt->expBits);
            status = 0;
        }
    }
    uint32_t hi = biased << (31 - fmt->expBits);
    hi |= sign ? 0x80000000u : 0u;
    hi |= w[0];
    if (fmt->totalBits == 0x40) {
        out[1] = hi;
        out[0] = w[1];
    } else if (fmt->totalBits == 0x20) {
        out[0] = hi;
    }
    return status;
}

// 0x832B79 / 0x832B8F: formatos del EXE (0x8E39B8 y 0x8E39D0)
int __cdecl Ld12ToDouble(const Ld12* in, uint32_t* out) {
    static const FloatFormat kDouble = {0x400, -0x3FF, 0x35, 0xB, 0x40, 0x3FF};
    return Ld12Cvt(in, out, &kDouble);
}
int __cdecl Ld12ToFloat(const Ld12* in, uint32_t* out) {
    static const FloatFormat kFloat = {0x80, -0x7F, 0x18, 8, 0x20, 0x7F};
    return Ld12Cvt(in, out, &kFloat);
}

// 0x835E89: digitos decimales (valores 0..9) -> ld12 normalizado
void __cdecl Mtold12(const char* digits, unsigned int count, Ld12* out) {
    uint32_t exponent = 0x404E;
    out->d[0] = 0; out->d[1] = 0; out->d[2] = 0;
    if (count != 0) {
        for (unsigned int i = 0; i < count; ++i) {
            const Ld12 copy = *out;
            Shl12(out);
            Shl12(out);
            Add12(out, &copy);
            Shl12(out);
            Ld12 digit;
            digit.d[0] = static_cast<uint32_t>(static_cast<int>(static_cast<signed char>(digits[i])));
            digit.d[1] = 0; digit.d[2] = 0;
            Add12(out, &digit);
        }
        if (out->d[2] == 0) {
            do {
                exponent += 0xFFF0u;
                const uint32_t newHigh = out->d[1] >> 16;
                const uint32_t newMid = (out->d[0] >> 16) | (out->d[1] << 16);
                out->d[0] <<= 16;
                out->d[1] = newMid;
                if (newHigh != 0) {
                    out->d[2] = newHigh;
                    break;
                }
            } while (true);
        }
    }
    while (!(out->d[2] & 0x8000u)) {
        Shl12(out);
        exponent += 0xFFFFu;
    }
    Store16(reinterpret_cast<uint8_t*>(out) + 10, static_cast<uint16_t>(exponent));
}

// 0x8346E2: texto -> ld12. Devuelve un estado: 0 correcto, 1 subdesbordamiento, 2 desbordamiento, 4 sin numero.
// multflag es el tercer argumento de _multtenpow12; scale/adjust se suman/restan al exponente decimal cuando no hubo exponente / parte entera; mode distingue
// el tratamiento del signo del exponente en el estado 11 (todos los llamadores de scanf pasan 0).
int __cdecl Strgtold12(Ld12* out, const char** endptr, const char* str, int multflag, int scale, int adjust, int mode) {
    unsigned char digits[0x20];
    unsigned char* esi = digits;
    unsigned char* digitsEnd = digits;   // [ebp-8]
    int expSign = 1;            // [ebp-0x1c]
    unsigned int ndig = 0;      // [ebp-0xc]
    int dropped = 0;            // [ebp-0x10]
    int found = 0;              // [ebp-0x14]
    int status = 0;             // [ebp-0x18]
    int expValue = 0;           // [ebp-0x20]
    int expPresent = 0;         // [ebp-0x24]
    int zeroSeen = 0;           // [ebp-0x28]
    uint32_t signMask = 0;      // [ebp-0x2c]
    const char* edi = str;
    const char* restore = str;  // [ebp+0x10]
    signed char bl = 0;
    unsigned int state = 0;
    uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0; // edx, ebx, esi, eax del resultado
    Ld12 mant;
    char dp = DecimalPointChar();
    std::memset(digits, 0, sizeof digits);

    for (;;) {
        const char s = *edi;
        if (s == ' ' || s == 9 || s == 0xA || s == 0xD) {
            ++edi;
        } else {
            break;
        }
    }
    dp = DecimalPointChar();

    for (;;) {
        // 0x83473C
        bl = *edi++;
        switch (state) {
        case 0: // 0x83474F
            if (bl >= '1' && bl <= '9') { state = 3; --edi; continue; }
            if (bl == dp) { state = 5; continue; }
            if (bl == '+') { signMask = 0; state = 2; continue; }
            if (bl == '-') { state = 2; signMask = 0x8000; continue; }
            if (bl == '0') { state = 1; continue; }
            digitsEnd = esi; --edi; goto FINISH_STORE;
        case 1: // 0x8347A2
            found = 1;
            if (bl >= '1' && bl <= '9') { state = 3; --edi; continue; }
            if (bl == dp) { state = 4; continue; }
            if (bl == '+' || bl == '-') { --edi; state = 11; continue; }
            if (bl == '0') { continue; }
            goto AFTER_DIGITS_CHECK_E;
        case 2: // 0x834803
            if (bl >= '1' && bl <= '9') { state = 3; --edi; continue; }
            if (bl == dp) { state = 5; continue; }
            if (bl == '0') { state = 1; continue; }
            edi = restore; goto FINISH;
        case 3: // 0x83482E
            found = 1;
            goto INT_DIGITS;
        case 4: // 0x834869
            found = 1;
            zeroSeen = 1;
            if (ndig == 0) {
                while (bl == '0') {
                    --dropped;
                    bl = *edi++;
                }
            }
            goto FRAC_DIGITS;
        case 5: // 0x8348C1
            zeroSeen = 1;
            if (!Isdigit(static_cast<unsigned char>(bl))) { edi = restore; goto FINISH; }
            state = 4; --edi; continue;
        case 6: // 0x8348E1
            restore = edi - 2;
            if (bl >= '1' && bl <= '9') { state = 9; --edi; continue; }
            if (bl == '+') { state = 7; continue; }
            if (bl == '-') { expSign = -1; state = 7; continue; }
            if (bl == '0') { state = 8; continue; }
            digitsEnd = esi; edi = restore; goto FINISH_STORE;
        case 7: // 0x83496E
            if (bl >= '1' && bl <= '9') { state = 9; --edi; continue; }
            if (bl == '0') { state = 8; continue; }
            edi = restore; goto FINISH;
        case 8: // 0x834919
            expPresent = 1;
            while (bl == '0') {
                bl = *edi++;
            }
            if (bl >= '1' && bl <= '9') { state = 9; --edi; continue; }
            goto FINISH_DEC;
        case 9: { // 0x8349D3
            digitsEnd = esi;
            expPresent = 1;
            int e = 0;
            while (Isdigit(static_cast<unsigned char>(bl))) {
                e = e * 10 + bl - '0';
                if (e > 0x1450) {
                    e = 0x1451;
                    break;
                }
                bl = *edi++;
            }
            expValue = e;
            unsigned char c2 = static_cast<unsigned char>(bl);
            while (Isdigit(c2)) {
                c2 = static_cast<unsigned char>(*edi++);
            }
            esi = digitsEnd;
            --edi;
            goto FINISH_STORE;
        }
        case 11: // 0x83498B
            if (mode == 0) { --edi; state = 10; goto TERMINAL_CHECK; }
            restore = edi - 1;
            if (bl == '+') { state = 7; continue; }
            if (bl == '-') { expSign = -1; state = 7; continue; }
            digitsEnd = esi; edi = restore; goto FINISH_STORE;
        default:
        TERMINAL_CHECK:
            if (state == 10) {
                goto FINISH; // 0x834935
            }
            continue;
        }

    INT_DIGITS:
        // 0x83484E
        while (Isdigit(static_cast<unsigned char>(bl))) {
            if (ndig < 0x19) {
                ++ndig;
                *esi++ = static_cast<unsigned char>(bl - '0');
            } else {
                ++dropped;
            }
            bl = *edi++;
        }
        if (bl == dp) { state = 4; continue; }
        goto AFTER_DIGITS_SIGN;

    FRAC_DIGITS:
        // 0x83489C
        while (Isdigit(static_cast<unsigned char>(bl))) {
            if (ndig < 0x19) {
                ++ndig;
                *esi++ = static_cast<unsigned char>(bl - '0');
                --dropped;
            }
            bl = *edi++;
        }
        // 0x8348AA
    AFTER_DIGITS_SIGN:
        if (bl == '+' || bl == '-') { --edi; state = 11; continue; }
    AFTER_DIGITS_CHECK_E:
        // 0x8347D4
        if (bl <= 'C') { goto FINISH_DEC; }
        if (bl <= 'E') { state = 6; continue; }
        if (bl <= 'c') { goto FINISH_DEC; }
        if (bl > 'e') { goto FINISH_DEC; }
        state = 6;
        continue;

    FINISH_DEC:
        // 0x834934
        --edi;
    FINISH:
        // 0x834935
        digitsEnd = esi;
    FINISH_STORE:
        // 0x834938
        *endptr = edi;
        esi = digitsEnd;
        if (!found) {
            status = 4;
            goto ZERO_RESULT;
        }
        if (ndig > 0x18) {
            if (digits[0x17] >= 5) {
                ++digits[0x17];
            }
            --esi;
            ++dropped;
            ndig = 0x18;
        }
        if (ndig == 0) {
            goto ZERO_RESULT;
        }
        for (;;) {
            --esi;
            if (*esi == 0) {
                --ndig;
                ++dropped;
                continue;
            }
            break;
        }
        Mtold12(reinterpret_cast<const char*>(digits), ndig, &mant);
        {
            int e = expValue;
            if (expSign < 0) {
                e = -e;
            }
            e += dropped;
            if (!expPresent) {
                e += scale;
            }
            if (!zeroSeen) {
                e -= adjust;
            }
            if (e > 0x1450) {
                r1 = 0; r3 = 0x7FFF; r2 = 0x80000000u; r0 = 0; status = 2;
                goto STORE_RESULT;
            }
            if (e < -0x1450) {
                status = 1;
                goto ZERO_RESULT;
            }
            Multtenpow12(&mant, e, multflag);
            const uint8_t* m = reinterpret_cast<const uint8_t*>(&mant);
            r0 = Load32(m);
            r1 = Load32(m + 2);
            r2 = Load32(m + 6);
            r3 = Load16(m + 10);
        }
        goto STORE_RESULT;
    ZERO_RESULT:
        r0 = 0; r1 = 0; r2 = 0; r3 = 0;
    STORE_RESULT: {
        uint8_t* o = reinterpret_cast<uint8_t*>(out);
        r3 |= signMask;
        Store32(o + 2, r1);
        Store32(o + 6, r2);
        Store16(o + 10, static_cast<uint16_t>(r3));
        Store16(o, static_cast<uint16_t>(r0));
        return status;
    }
    }
}

// 0x832C21
int __cdecl Atodbl(double* out, const char* str) {
    Ld12 ld;
    const char* end;
    Strgtold12(&ld, &end, str, 0, 0, 0, 0);
    return Ld12ToDouble(&ld, reinterpret_cast<uint32_t*>(out));
}

// 0x832CA8
int __cdecl Atoflt(float* out, const char* str) {
    Ld12 ld;
    const char* end;
    Strgtold12(&ld, &end, str, 0, 0, 0, 0);
    return Ld12ToFloat(&ld, reinterpret_cast<uint32_t*>(out));
}

// 0x826B40: _fassign(flag_double, destino, texto). Copia por palabras (nada pasa por la x87).
void __cdecl Fassign(int isLong, void* argument, const char* text) {
    if (isLong) {
        double value;
        Atodbl(&value, text);
        std::memcpy(argument, &value, 8);
    } else {
        float value;
        Atoflt(&value, text);
        std::memcpy(argument, &value, 4);
    }
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtFloatInInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Ld12TailZero, 0x832875);
    RH_ScopedGlobalInstall(ExeCrt::Ld12AddOne, 0x8328A7);
    RH_ScopedGlobalInstall(ExeCrt::Ld12Round, 0x8328F4);
    RH_ScopedGlobalInstall(ExeCrt::Ld12Copy, 0x832966);
    RH_ScopedGlobalInstall(ExeCrt::Ld12Zero, 0x832981);
    RH_ScopedGlobalInstall(ExeCrt::Ld12IsZero, 0x83298D);
    RH_ScopedGlobalInstall(ExeCrt::Ld12ShiftRight, 0x8329A6);
    RH_ScopedGlobalInstall(ExeCrt::Ld12Cvt, 0x832A21);
    RH_ScopedGlobalInstall(ExeCrt::Ld12ToDouble, 0x832B79);
    RH_ScopedGlobalInstall(ExeCrt::Ld12ToFloat, 0x832B8F);
    RH_ScopedGlobalInstall(ExeCrt::Mtold12, 0x835E89);
    RH_ScopedGlobalInstall(ExeCrt::Strgtold12, 0x8346E2);
    RH_ScopedGlobalInstall(ExeCrt::Atodbl, 0x832C21);
    RH_ScopedGlobalInstall(ExeCrt::Atoflt, 0x832CA8);
    RH_ScopedGlobalInstall(ExeCrt::Fassign, 0x826B40);
}
#endif
