/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ExeCrt
 * Reconstrucción: COMPLETA — 63/63 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/63 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstring>
#include "CrtFloat.h"
#include "CrtState.h"
#include "CrtString.h"
#include "CrtCtype.h"

// Reconstruccion de la conversion de coma flotante a texto del CRT del EXE (lote CRT4A). Ver CrtFloat.h y reversing/FnCRT4A/README.md.
// Aritmetica entera pura. Los nombres de los campos y las constantes salen del desensamblado (direcciones en los comentarios).

#include "CrtFloatTables.inc"

namespace ExeCrt {
namespace {
constexpr unsigned int kPow10Base = 0x8E3BF4; // direccion del EXE que corresponde al primer byte de kPow10Table
inline const unsigned char* Pow10Entry(unsigned int exeAddress) { return kPow10Table + (exeAddress - kPow10Base); }

inline uint16_t* W(Ld12* x) { return reinterpret_cast<uint16_t*>(x); }
inline const uint16_t* W(const Ld12* x) { return reinterpret_cast<const uint16_t*>(x); }


// 0x826B7E (_shift, recibe eax = cadena y edi = cantidad): desplaza la cadena n posiciones a la derecha (con su cero final)
void Shift(char* s, int n) {
    if (n != 0) {
        std::memmove(s + n, s, Strlen(s) + 1);
    }
}
} // namespace

// 0x835DAF
uint32_t __cdecl Addl(uint32_t a, uint32_t b, uint32_t* out) {
    const uint32_t sum = a + b;
    *out = sum;
    return (sum < a || sum < b) ? 1u : 0u;
}

// 0x835DD0: suma de 96 bits
void __cdecl Add12(Ld12* a, const Ld12* b) {
    if (Addl(a->d[0], b->d[0], &a->d[0])) {
        if (Addl(a->d[1], 1, &a->d[1])) {
            ++a->d[2];
        }
    }
    if (Addl(a->d[1], b->d[1], &a->d[1])) {
        ++a->d[2];
    }
    Addl(a->d[2], b->d[2], &a->d[2]);
}

// 0x835E2E
void __cdecl Shl12(Ld12* a) {
    const uint32_t d0 = a->d[0], d1 = a->d[1], d2 = a->d[2];
    a->d[0] = d0 << 1;
    a->d[1] = (d1 << 1) | (d0 >> 31);
    a->d[2] = (d2 << 1) | (d1 >> 31);
}

// 0x835E5C
void __cdecl Shr12(Ld12* a) {
    const uint32_t d0 = a->d[0], d1 = a->d[1], d2 = a->d[2];
    a->d[1] = (d1 >> 1) | (d2 << 31);
    a->d[0] = (d0 >> 1) | (d1 << 31);
    a->d[2] = d2 >> 1;
}

// 0x8363D3: a = a * b (mantisas de 80 bits en palabras de 16, columnas 4..8 del producto; redondeo al par con bit pegajoso)
void __cdecl Mulld12(Ld12* a, const Ld12* b) {
    uint16_t* const A = W(a);
    const uint16_t* const B = W(b);
    const unsigned int sa = A[5], sb = B[5];
    const unsigned int sign = (sa ^ sb) & 0x8000;
    const unsigned int ea = sa & 0x7FFF, eb = sb & 0x7FFF;
    uint32_t sum = ea + eb;

    auto overflow = [&]() { // 0x8365E0: infinito con el signo del producto
        A[2] = 0; A[3] = 0; // dword +4 = 0 (palabras 2 y 3)
        A[0] = 0; A[1] = 0; // dword +0 = 0
        Store32(reinterpret_cast<uint8_t*>(a) + 8, sign ? 0xFFFF8000u : 0x7FFF8000u);
    };
    auto zeroAll = [&]() { a->d[0] = 0; a->d[1] = 0; a->d[2] = 0; };

    if (ea >= 0x7FFF || eb >= 0x7FFF || static_cast<uint16_t>(sum) > 0xBFFD) {
        overflow();
        return;
    }
    if (static_cast<uint16_t>(sum) <= 0x3FBF) {
        zeroAll();
        return;
    }
    if (ea == 0) {
        ++sum;
        if ((a->d[2] & 0x7FFFFFFFu) == 0 && a->d[1] == 0 && a->d[0] == 0) {
            A[5] = 0;
            return;
        }
    }
    if (eb == 0) {
        ++sum;
        if ((b->d[2] & 0x7FFFFFFFu) == 0 && b->d[1] == 0 && b->d[0] == 0) {
            zeroAll();
            return;
        }
    }

    uint16_t acc[8] = {};   // [ebp-0x28]: 6 palabras de resultado (+2 de holgura; el acarreo de la palabra 6 no ocurre nunca)
    uint32_t sticky = 0;    // [ebp-0x18]
    for (int i = 0, count = 5; count > 0; ++i, --count) {
        const unsigned int accWord = 2 + i; // palabra a la que apunta [ebp-8]
        const uint16_t* pa = A + i;
        const uint16_t* pb = B + 4;
        for (int m = 0; m < count; ++m) {
            const uint32_t product = static_cast<uint32_t>(pa[m]) * pb[-m];
            uint8_t* dst = reinterpret_cast<uint8_t*>(acc) + (accWord - 2) * 2;
            uint32_t value = Load32(dst);
            const uint32_t carry = Addl(value, product, &value);
            Store32(dst, value);
            if (carry) {
                ++acc[accWord];
            }
        }
    }
    uint8_t* const accb = reinterpret_cast<uint8_t*>(acc);
    auto accShl = [&]() { // 96 bits desde acc[0]
        Ld12 t; std::memcpy(&t, accb, 12); Shl12(&t); std::memcpy(accb, &t, 12);
    };
    auto accShr = [&]() {
        Ld12 t; std::memcpy(&t, accb, 12); Shr12(&t); std::memcpy(accb, &t, 12);
    };

    sum += 0xC002u;
    bool normalized = false;
    if (static_cast<int16_t>(sum) > 0) {
        do {
            if (accb[11] & 0x80) {
                break;
            }
            accShl();
            sum += 0xFFFFu;
        } while (static_cast<int16_t>(sum) > 0);
        if (static_cast<int16_t>(sum) > 0) {
            normalized = true;
        }
    }
    if (!normalized) {
        sum += 0xFFFFu; // 0x836537
        if (static_cast<int16_t>(sum) < 0) {
            uint32_t shift = static_cast<uint16_t>(0u - sum);
            sum += shift;
            do {
                if (accb[0] & 1) {
                    ++sticky;
                }
                accShr();
                --shift;
            } while (shift != 0);
            if (sticky) {
                accb[0] |= 1;
            }
        }
    }
    // redondeo al par (0x836570)
    const uint32_t low = Load32(accb) & 0x1FFFFu;
    if (acc[0] > 0x8000 || low == 0x18000u) {
        uint32_t d26 = Load32(accb + 2);
        if (d26 == 0xFFFFFFFFu) {
            Store32(accb + 2, 0);
            uint32_t d22 = Load32(accb + 6);
            if (d22 == 0xFFFFFFFFu) {
                Store32(accb + 6, 0);
                if (acc[5] == 0xFFFF) {
                    ++sum;
                    acc[5] = 0x8000;
                } else {
                    ++acc[5];
                }
            } else {
                Store32(accb + 6, d22 + 1);
            }
        } else {
            Store32(accb + 2, d26 + 1);
        }
    }
    if (static_cast<uint16_t>(sum) >= 0x7FFF) {
        overflow();
        return;
    }
    A[0] = acc[1];
    A[1] = acc[2]; A[2] = acc[3];
    A[3] = acc[4]; A[4] = acc[5];
    A[5] = static_cast<uint16_t>(sum | sign);
}

// 0x83660B: x = x * 10^power con las tablas de potencias de 10 (grupos de 7 entradas de 12 bytes por cada 3 bits del exponente)
void __cdecl Multtenpow12(Ld12* x, int power, int flag) {
    unsigned int base = 0x8E3C00 - 0x60;
    if (power == 0) {
        return;
    }
    if (power < 0) {
        power = -power;
        base = 0x8E3D60 - 0x60;
    }
    if (flag == 0) {
        W(x)[0] = 0;
    }
    if (power == 0) {
        return;
    }
    do {
        const int idx = power & 7;
        power >>= 3;
        base += 0x54;
        if (idx != 0) {
            Ld12 entry;
            std::memcpy(&entry, Pow10Entry(base + idx * 12), 12);
            if (W(&entry)[0] >= 0x8000) {
                // entrada redondeada hacia arriba: se resta 1 al dword +2 de la mantisa para truncarla
                uint8_t* e = reinterpret_cast<uint8_t*>(&entry);
                Store32(e + 2, Load32(e + 2) - 1);
            }
            Mulld12(x, &entry);
        }
    } while (power != 0);
}

// 0x832D62: double -> 80 bits (bit explicito; el cero pierde el signo; los denormales se normalizan)
void __cdecl Dtold(Ld10* out, const double* in) {
    const uint32_t lo = Load32(in);
    uint32_t hi = Load32(reinterpret_cast<const uint8_t*>(in) + 4);
    const unsigned int top = (hi >> 16) & 0xFFFF; // palabra +6
    const unsigned int sign = top & 0x8000;
    const unsigned int e = (top >> 4) & 0x7FF;
    uint32_t mant = hi & 0xFFFFF;
    uint32_t implicit = 0x80000000u;
    uint32_t exp;
    if (e == 0) {
        if (mant == 0 && lo == 0) {
            out->lo = 0; out->hi = 0; out->se = 0;
            return;
        }
        exp = e + 0x3C01;
        implicit = 0;
    } else if (e == 0x7FF) {
        exp = 0x7FFF;
    } else {
        exp = e + 0x3C00;
    }
    uint32_t outHi = (lo >> 0x15) | (mant << 0xB) | implicit;
    uint32_t outLo = lo << 0xB;
    if (!(outHi & 0x80000000u)) {
        do {
            const uint32_t carry = outLo >> 31;
            outHi = (outHi << 1) | carry;
            outLo = outLo + outLo;
            exp += 0xFFFFu;
        } while (!(outHi & 0x80000000u));
    }
    out->lo = outLo;
    out->hi = outHi;
    out->se = static_cast<uint16_t>(sign | exp);
}

// 0x835F6D: ld80 -> digitos decimales. flags & 1: ndigits cuenta digitos despues del punto; si no, son cifras significativas (maximo 0x15).
// Devuelve 1 para numeros normales y 0 para infinitos y NaN (texto "1#INF", "1#IND", "1#QNAN", "1#SNAN").
unsigned __cdecl I10Output(Ld10 value, int ndigits, unsigned flags, I10Out* out) {
    const unsigned int se = value.se & 0xFFFF;
    const unsigned int signBit = se & 0x8000;
    const unsigned int exp = se & 0x7FFF;
    const uint32_t lo = value.lo, hi = value.hi;
    unsigned int result = 1;

    // 0.1 en 96 bits: extension 0xCCCC, mantisa 0xCCCCCCCCCCCCCCCC, exponente 0x3FFB
    Ld12 tenth;
    {
        uint8_t* t = reinterpret_cast<uint8_t*>(&tenth);
        std::memset(t, 0xCC, 10);
        t[10] = 0xFB; t[11] = 0x3F;
    }
    out->sign = signBit ? '-' : ' ';

    auto zeroResult = [&]() -> unsigned { // 0x8361EC
        out->digits[0] = '0';
        out->digits[1] = 0;
        out->decpt = 0;
        out->sign = ' ';
        out->ndigits = 1;
        return 1;
    };

    if (exp == 0 && hi == 0 && lo == 0) {
        return zeroResult();
    }
    if (exp == 0x7FFF) {
        out->decpt = 1;
        const char* text;
        int len;
        if ((hi == 0x80000000u && lo == 0) || (hi & 0x40000000u)) {
            if (signBit && hi == 0xC0000000u && lo == 0) {
                text = "1#IND"; len = 5;
            } else if (hi == 0x80000000u && lo == 0) {
                text = "1#INF"; len = 5;
            } else {
                text = "1#QNAN"; len = 6;
            }
        } else {
            text = "1#SNAN"; len = 6;
        }
        std::memcpy(out->digits, text, static_cast<size_t>(len) + 1);
        out->ndigits = static_cast<char>(len);
        return 0;
    }

    // estimacion del exponente decimal: ((exp * 0x4D10 + (exp >> 8 + 2 * (mant >> 24)) * 0x4D) - 0x134312F4) >> 16
    const uint32_t estimate = (((exp >> 8) + (hi >> 24) * 2) * 0x4Du) + exp * 0x4D10u - 0x134312F4u;
    int32_t si = static_cast<int16_t>(static_cast<int32_t>(estimate) >> 16);

    Ld12 x;
    W(&x)[0] = 0;
    Store32(reinterpret_cast<uint8_t*>(&x) + 2, lo);
    Store32(reinterpret_cast<uint8_t*>(&x) + 6, hi);
    W(&x)[5] = static_cast<uint16_t>(exp);
    Multtenpow12(&x, -static_cast<int16_t>(si), 1);
    if (W(&x)[5] >= 0x3FFF) {
        ++si;
        Mulld12(&x, &tenth);
    }
    out->decpt = static_cast<int16_t>(si);
    int edi = ndigits;
    if (flags & 1) {
        edi += static_cast<int16_t>(si);
        if (edi <= 0) {
            return zeroResult();
        }
    }
    if (edi > 0x15) {
        edi = 0x15;
    }
    int shift = static_cast<int>(W(&x)[5]) - 0x3FFE;
    W(&x)[5] = 0;
    for (int i = 0; i < 8; ++i) {
        Shl12(&x);
    }
    if (shift < 0) {
        int n = (-shift) & 0xFF;
        while (n > 0) {
            Shr12(&x);
            --n;
        }
    }
    char* p = out->digits;
    for (int count = edi + 1; count > 0; --count) {
        const Ld12 copy = x;
        Shl12(&x);
        Shl12(&x);
        Add12(&x, &copy);
        Shl12(&x);
        uint8_t* xb = reinterpret_cast<uint8_t*>(&x);
        const uint8_t digit = xb[11];
        xb[11] = 0;
        *p++ = static_cast<char>(digit + '0');
    }
    // p apunta despues del ultimo digito generado (digito de redondeo); eax = p - 1
    char* last = p - 1;
    const char roundDigit = *last;
    --last;
    char* const first = out->digits;
    if (roundDigit >= '5') {
        while (last >= first) {
            if (*last == '9') {
                *last = '0';
                --last;
            } else {
                break;
            }
        }
        if (last < first) {
            ++last;
            ++out->decpt;
        }
        ++*last;
    } else {
        while (last >= first && *last == '0') {
            --last;
        }
        if (last < first) {
            first[0] = '0';
            return zeroResult();
        }
    }
    const int n = static_cast<int>(last - first) + 1;
    out->ndigits = static_cast<char>(n);
    out->digits[n] = 0;
    return result;
}

// 0x832E1C: double -> STRFLT con hasta 17 digitos (crea el 80 bits con _dtold y llama a $I10_OUTPUT con 0x11 digitos)
Strflt* __cdecl Fltout2(double value, Strflt* out, char* buffer) {
    Ld10 ld;
    Dtold(&ld, &value);
    I10Out o;
    const unsigned int flag = I10Output(ld, 0x11, 0, &o);
    out->flag = static_cast<int>(flag);
    out->sign = static_cast<signed char>(o.sign);
    out->decpt = o.decpt;
    Strcpy(buffer, o.digits);
    out->mantissa = buffer;
    return out;
}

// 0x832CEB: copia 'digits' digitos (rellena con '0'), redondea con el siguiente y deja el resultado en buffer (con un '1' delante si hubo acarreo total)
void __cdecl Fptostr(char* buffer, int digits, Strflt* flt) {
    const char* src = flt->mantissa;
    char* const first = buffer;
    char* p = buffer + 1;
    *first = '0';
    int remaining = digits;
    int cmpFlag = digits;
    if (digits > 0) {
        while (remaining-- > 0) {
            char c = *src;
            if (c) {
                ++src;
            } else {
                c = '0';
            }
            *p++ = c;
        }
        cmpFlag = 0;
    }
    *p = 0;
    if (cmpFlag >= 0 && *src >= '5') {
        char* q = p - 1;
        while (*q == '9') {
            *q = '0';
            --q;
        }
        ++*q;
    }
    if (*first == '1') {
        ++flt->decpt;
    } else {
        std::memmove(first, buffer + 1, Strlen(buffer + 1) + 1);
    }
}

// 0x826A9F: inserta el punto decimal tras la parte entera (antes de la 'e' si la hay)
void __cdecl Forcdecpt(char* buffer) {
    char* p = buffer;
    if (Tolower(static_cast<signed char>(*p)) != 'e') {
        do {
            ++p;
        } while (Isdigit(static_cast<signed char>(*p)));
    }
    char carry = *p;
    *p++ = DecimalPointChar();
    char previous;
    do {
        previous = *p;
        *p = carry;
        carry = previous;
        previous = *p;
        ++p;
    } while (previous != 0);
}

// 0x826ADB: quita los ceros finales de la parte decimal (y el punto si queda solo)
void __cdecl Cropzeros(char* buffer) {
    const char dp = DecimalPointChar();
    char* p = buffer;
    while (*p != 0 && *p != dp) {
        ++p;
    }
    char c = *p;
    ++p;
    if (c == 0) {
        return;
    }
    while ((c = *p) != 0 && c != 'e' && c != 'E') {
        ++p;
    }
    char* tail = p;
    --p;
    while (*p == '0') {
        --p;
    }
    if (*p == dp) {
        --p;
    }
    do {
        c = *tail;
        ++p;
        ++tail;
        *p = c;
    } while (c != 0);
}

namespace {
// 0x826B9B (_cftoe2): arma "d.ddde+ddd" a partir de los digitos ya colocados. gfmt != 0 cuando viene de %g.
char* Cftoe2(const Strflt* flt, char* buf, int ndec, int caps, int gfmt) {
    if (gfmt != 0) {
        char* at = buf + (flt->sign == '-' ? 1 : 0);
        Shift(at, ndec > 0 ? 1 : 0);
    }
    char* p = buf;
    if (flt->sign == '-') {
        *p++ = '-';
    }
    if (ndec > 0) {
        char* next = p + 1;
        *p = *next;
        p = next;
        *p = DecimalPointChar();
    }
    char* e = Strcpy(p + (gfmt == 0 ? 1 : 0) + ndec, "e+000");
    if (caps) {
        *e = 'E';
    }
    const char* digits = flt->mantissa;
    ++e;
    if (*digits != '0') {
        int exponent = flt->decpt - 1;
        if (exponent < 0) {
            exponent = -exponent;
            *e = '-';
        }
        ++e;
        if (exponent >= 100) {
            *e += static_cast<char>(exponent / 100);
            exponent %= 100;
        }
        ++e;
        if (exponent >= 10) {
            *e += static_cast<char>(exponent / 10);
            exponent %= 10;
        }
        e[1] += static_cast<char>(exponent);
    }
    return buf;
}

// 0x826CBD (_cftof2)
char* Cftof2(const Strflt* flt, char* buf, int ndec, int gfmt) {
    const int exponent = flt->decpt - 1;
    if (gfmt != 0 && exponent == ndec) {
        char* z = buf + exponent + (flt->sign == '-' ? 1 : 0);
        z[0] = '0';
        z[1] = 0;
    }
    char* p = buf;
    if (flt->sign == '-') {
        *p++ = '-';
    }
    if (flt->decpt > 0) {
        p += flt->decpt;
    } else {
        Shift(p, 1);
        *p++ = '0';
    }
    if (ndec > 0) {
        Shift(p, 1);
        *p++ = DecimalPointChar();
        int dec = flt->decpt;
        if (dec < 0) {
            dec = -dec;
            if (gfmt != 0 || ndec >= dec) {
                ndec = dec;
            }
            Shift(p, ndec);
            std::memset(p, '0', static_cast<size_t>(ndec));
        }
    }
    return buf;
}
} // namespace

// 0x826C49
char* __cdecl Cftoe(const double* value, char* buffer, int ndec, int caps) {
    Strflt flt;
    char digits[0x20];
    Fltout2(*value, &flt, digits);
    Fptostr(buffer + (flt.sign == '-' ? 1 : 0) + (ndec > 0 ? 1 : 0), ndec + 1, &flt);
    return Cftoe2(&flt, buffer, ndec, caps, 0);
}

// 0x826D59
char* __cdecl Cftof(const double* value, char* buffer, int ndec) {
    Strflt flt;
    char digits[0x20];
    Fltout2(*value, &flt, digits);
    Fptostr(buffer + (flt.sign == '-' ? 1 : 0), flt.decpt + ndec, &flt);
    return Cftof2(&flt, buffer, ndec, 0);
}

// 0x826DC1
char* __cdecl Cftog(const double* value, char* buffer, int ndec, int caps) {
    Strflt flt;
    char digits[0x20];
    Fltout2(*value, &flt, digits);
    const int before = flt.decpt - 1;
    char* const at = buffer + (flt.sign == '-' ? 1 : 0);
    Fptostr(at, ndec, &flt);
    const int after = flt.decpt - 1;
    if (after >= -4 && after < ndec) {
        if (before < after) {
            // el redondeo subio el exponente: se corta el ultimo digito
            char* q = at;
            while (*q++ != 0) {
            }
            q[-2] = 0;
        }
        return Cftof2(&flt, buffer, ndec, 1);
    }
    return Cftoe2(&flt, buffer, ndec, caps, 1);
}

// 0x826E61
char* __cdecl Cfltcvt(const double* value, char* buffer, int format, int ndec, int caps) {
    if (format == 'e' || format == 'E') {
        return Cftoe(value, buffer, ndec, caps);
    }
    if (format == 'f') {
        return Cftof(value, buffer, ndec);
    }
    return Cftog(value, buffer, ndec, caps);
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtFloatInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Addl, 0x835DAF);
    RH_ScopedGlobalInstall(ExeCrt::Add12, 0x835DD0);
    RH_ScopedGlobalInstall(ExeCrt::Shl12, 0x835E2E);
    RH_ScopedGlobalInstall(ExeCrt::Shr12, 0x835E5C);
    RH_ScopedGlobalInstall(ExeCrt::Mulld12, 0x8363D3);
    RH_ScopedGlobalInstall(ExeCrt::Multtenpow12, 0x83660B);
    RH_ScopedGlobalInstall(ExeCrt::Dtold, 0x832D62);
    RH_ScopedGlobalInstall(ExeCrt::I10Output, 0x835F6D);
    RH_ScopedGlobalInstall(ExeCrt::Fltout2, 0x832E1C);
    RH_ScopedGlobalInstall(ExeCrt::Fptostr, 0x832CEB);
    RH_ScopedGlobalInstall(ExeCrt::Forcdecpt, 0x826A9F);
    RH_ScopedGlobalInstall(ExeCrt::Cropzeros, 0x826ADB);
    RH_ScopedGlobalInstall(ExeCrt::Cftoe, 0x826C49);
    RH_ScopedGlobalInstall(ExeCrt::Cftof, 0x826D59);
    RH_ScopedGlobalInstall(ExeCrt::Cftog, 0x826DC1);
    RH_ScopedGlobalInstall(ExeCrt::Cfltcvt, 0x826E61);
}
#endif
