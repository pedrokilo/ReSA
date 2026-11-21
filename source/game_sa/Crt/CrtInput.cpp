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
#include <cstdint>
#include <cstring>
#include "CrtInput.h"
#include "CrtCtype.h"
#include "CrtHeap.h"
#include "CrtFloatIn.h"

// Reconstruccion de _input (0x8290D4) del CRT del EXE (lote CRT4A). Ver CrtInput.h y reversing/FnCRT4A/README.md.
// Las etiquetas de goto y los comentarios llevan las direcciones del EXE para poder cotejar el flujo con el desensamblado.

namespace ExeCrt {
namespace {
inline int CallFilbuf(File* f) { return reinterpret_cast<int(__cdecl*)(File*)>(0x82AF38)(f); }
inline int CallMbtowc(void* dst, const char* src, unsigned int n) { return reinterpret_cast<int(__cdecl*)(void*, const char*, unsigned int)>(0x8338CE)(dst, src, n); }
inline unsigned int MbCurMax() { return *reinterpret_cast<unsigned int*>(0x8E37D0); }

// 0x829094 (_inc: edx = FILE): siguiente caracter (0..255) o -1
inline int Inc(File* f) {
    if (--f->cnt >= 0) {
        return static_cast<unsigned char>(*f->ptr++);
    }
    return CallFilbuf(f);
}
inline void UnInc(int c, File* f) { UngetcNolock(c, f); }
} // namespace

// 0x83361B: da buffer de 4 KiB a la FILE (o, sin memoria, el de 2 bytes interno _charbuf)
void __cdecl Getbuf(File* f) {
    ++*reinterpret_cast<int*>(0xC9AC4C);
    char* const buffer = static_cast<char*>(Malloc(0x1000));
    f->base = buffer;
    if (buffer) {
        f->flag |= 8;
        f->bufsiz = 0x1000;
    } else {
        f->flag |= 4;
        f->base = reinterpret_cast<char*>(f) + 0x14;
        f->bufsiz = 2;
    }
    f->cnt = 0;
    f->ptr = f->base;
}

// 0x83375D
int __cdecl UngetcNolock(int c, File* f) {
    if (c == -1) {
        return -1;
    }
    const int flag = f->flag;
    if (!((flag & 1) || ((flag & 0x80) && !(flag & 2)))) {
        return -1;
    }
    if (f->base == nullptr) {
        Getbuf(f);
    }
    if (f->ptr == f->base) {
        if (f->cnt != 0) {
            return -1;
        }
        ++f->ptr;
    }
    --f->ptr;
    if (f->flag & 0x40) {
        if (*f->ptr != static_cast<char>(c)) {
            ++f->ptr;
            return -1;
        }
    } else {
        *f->ptr = static_cast<char>(c);
    }
    ++f->cnt;
    f->flag = (f->flag & ~0x10) | 1;
    return c & 0xFF;
}

// 0x8290D4
int __cdecl Input(File* stream, const unsigned char* format, va_list arglist) {
    char* args = reinterpret_cast<char*>(arglist);
    const unsigned char* fmt = format;
    int ch = 0;               // [ebp-0x28]
    int charcount = 0;        // [ebp-0x30]
    int count = 0;            // [ebp-0x34]
    unsigned char convs = 0;  // [ebp-0x29]
    unsigned char tableBuf[0x20];
    unsigned char* const table = tableBuf; // [ebp-0x20]: el EXE lo reserva una vez con _alloca (el malloc de respaldo solo se usa si la pila no basta)

    uint32_t number = 0;        // [ebp-0x38]
    uint64_t num64 = 0;         // [ebp-0x5c]
    unsigned char prevch = 0;   // [ebp-0x39]
    int started = 0;            // [ebp-0x40]
    int widthset = 0;           // [ebp-0x44]
    int width = 0;              // [ebp-0x48]
    unsigned char reject = 0;   // [ebp-0x49]
    unsigned char negative = 0; // [ebp-0x4a]
    unsigned char suppress = 0; // [ebp-0x4b]
    unsigned char done = 0;     // [ebp-0x4c]
    unsigned char wideDest = 0; // [ebp-0x4d]
    signed char widechar = 0;   // [ebp-0x4e]
    signed char longone = 1;    // [ebp-0x4f]
    int integer64 = 0;          // [ebp-0x54]
    char* argSave = nullptr;    // [ebp-0x60]
    char* argument = nullptr;   // [ebp-0x64]
    int comchr = 0;             // [ebp-0x68]
    char floatBuf[0x1C8];       // [ebp-0x1c8]
    char* esi = nullptr;
    char* ebx = nullptr;
    int bx = 0;
    int edi = 0;
    const unsigned char* scanptr = nullptr;
    unsigned char rngch = 0, last = 0;
    unsigned char mbBuf[2] = {0, 0};
    unsigned short wch = 0;

    while (*fmt != 0) {
        // 0x829102
        if (Isspace(*fmt)) {
            --charcount;
            int skipped;  // el EXE lo guarda en esi: la variable ch ([ebp-0x28]) no cambia con los espacios del formato
            do {
                ++charcount;
                skipped = Inc(stream);
            } while (Isspace(skipped));
            if (skipped != -1) {
                UnInc(skipped, stream);
            }
            do {
                ++fmt;
            } while (Isspace(*fmt));
            continue;
        }
        if (*fmt != '%') {
            // 0x829ABD: caracter literal
            ++charcount;
            ch = Inc(stream);
            const int expect = *fmt++;
            if (expect != ch) {
                goto LITERAL_MISMATCH;
            }
            if (CtypeTable()[(ch & 0xFF) * 2 + 1] & 0x80) {
                ++charcount;
                const int second = Inc(stream);
                const int expect2 = *fmt++;
                if (expect2 != second) {
                    if (second != -1) {
                        UnInc(second, stream);
                    }
                    goto LITERAL_MISMATCH;
                }
                --charcount; // 0x829B22
            }
            goto NEXT_ITEM;
        }

        // 0x829164: especificacion de conversion
        number = 0; prevch = 0; started = 0; widthset = 0; width = 0; reject = 0; negative = 0; suppress = 0; done = 0; wideDest = 0; widechar = 0;
        longone = 1; integer64 = 0;
        {
            unsigned int w = 0;
            const unsigned char* p = fmt;
            for (;;) {
                ++p;
                const int c = *p;
                if (Isdigit(c)) {
                    ++widthset;
                    w = w * 10 + static_cast<unsigned int>(c) - '0';
                } else if (c == 'N' || c == 'F') {
                    // modificadores near/far: se ignoran
                } else if (c == '*') {
                    ++suppress;
                } else if (c == 'L') {
                    ++longone;
                } else if (c == 'I') {
                    const unsigned char n1 = p[1];
                    if (n1 == '6' && p[2] == '4') {
                        p += 2;
                        ++integer64;
                        num64 = 0;
                    } else if (n1 == '3' && p[2] == '2') {
                        p += 2;
                    } else if (n1 == 'd' || n1 == 'i' || n1 == 'o' || n1 == 'x' || n1 == 'X') {
                        // 'I' seguido de tipo: se ignora
                    } else {
                        ++done;
                    }
                } else if (c == 'h') {
                    --longone; --widechar;
                } else if (c == 'l') {
                    ++longone; ++widechar;
                } else if (c == 'w') {
                    ++widechar;
                } else {
                    ++done;
                }
                if (done) {
                    break;
                }
            }
            width = static_cast<int>(w);
            fmt = p;
        }
        if (!suppress) {
            argSave = args;
            args += 4;
            std::memcpy(&argument, args - 4, 4);
        }
        ebx = argument;
        done = 0;
        if (widechar == 0) {
            widechar = (*fmt == 'S' || *fmt == 'C') ? 1 : -1;
        }
        edi = *fmt | 0x20;
        comchr = edi;
        if (comchr != 'n') {
            if (comchr == 'c' || comchr == '{') {
                ++charcount;
                ch = Inc(stream);
            } else {
                do {
                    ++charcount;
                    ch = Inc(stream);
                } while (Isspace(ch));
            }
        }
        // 0x8292DB
        if (widthset && width == 0) {
            goto MISMATCH_EXIT; // 0x829541
        }
        if (edi == 'o' || edi == 'd' || edi == 'u') {
            goto INT_SIGN; // 0x829883
        }
        if (edi == 'p') {
            longone = 1;
            goto INT_SIGN;
        }
        if (edi == 'c') {
            if (!widthset) {
                widthset = 1;
                ++width;
            }
            if (widechar > 0) {
                wideDest = 1;
            }
            goto STRING_COMMON;
        }
        if (edi == 's') {
            if (widechar > 0) {
                wideDest = 1;
            }
            goto STRING_COMMON;
        }
        if (edi == 'i') {
            edi = 'd';
            bx = ch;
            goto INT_XI;
        }
        if (edi == 'x') {
            bx = ch;
            goto INT_XI;
        }
        if (edi == 'e' || edi == 'f' || edi == 'g') {
            goto FLOAT_CONV;
        }
        if (edi == 'n') {
            // 0x829326: %n guarda el numero de caracteres leidos
            if (!suppress) {
                number = static_cast<uint32_t>(charcount);
                if (integer64) {
                    std::memcpy(ebx, &num64, 8);
                } else if (longone) {
                    std::memcpy(ebx, &number, 4);
                } else {
                    const unsigned short s16 = static_cast<unsigned short>(number);
                    std::memcpy(ebx, &s16, 2);
                }
            }
            goto CONV_DONE;
        }
        if (edi == '{') {
            goto SCANSET;
        }
        // 0x829536: tipo desconocido; se toma como caracter literal
        if (*fmt == ch) {
            --convs;
            if (!suppress) {
                args = argSave;
            }
            goto CONV_DONE;
        }
        goto MISMATCH_EXIT;

    FLOAT_CONV: {
        // 0x829362
        esi = floatBuf;
        bx = ch;
        bool advance = false;
        if (bx == '-') {
            floatBuf[0] = static_cast<char>(bx);
            esi = floatBuf + 1;
            advance = true;
        } else if (bx == '+') {
            advance = true;
        }
        if (advance) {
            --width;
            ++charcount;
            bx = Inc(stream);
            ch = bx;
        }
        if (!widthset || width > 0x15D) {
            width = 0x15D;
        }
        while (Isdigit(bx)) {
            if (width-- == 0) {
                break;
            }
            ++started;
            *esi++ = static_cast<char>(bx);
            ++charcount;
            bx = Inc(stream);
            ch = bx;
        }
        if (DecimalPointChar() == static_cast<char>(bx)) {
            if (width-- != 0) {
                ++charcount;
                bx = Inc(stream);
                *esi++ = DecimalPointChar();
                ch = bx;
                while (Isdigit(bx)) {
                    if (width-- == 0) {
                        break;
                    }
                    ++started;
                    *esi++ = static_cast<char>(bx);
                    ++charcount;
                    bx = Inc(stream);
                    ch = bx;
                }
            }
        }
        if (started && (bx == 'e' || bx == 'E')) {
            if (width-- != 0) {
                *esi++ = 'e';
                ++charcount;
                bx = Inc(stream);
                ch = bx;
                bool signSeen = false;
                if (bx == '-') {
                    *esi++ = '-';
                    signSeen = true;
                } else if (bx == '+') {
                    signSeen = true;
                }
                if (signSeen) {
                    if (width-- == 0) {
                        width = 0; // 0x829477
                    } else {
                        ++charcount;
                        bx = Inc(stream);
                        ch = bx;
                    }
                }
                while (Isdigit(bx)) {
                    if (width-- == 0) {
                        break;
                    }
                    ++started;
                    *esi++ = static_cast<char>(bx);
                    ++charcount;
                    bx = Inc(stream);
                    ch = bx;
                }
            }
        }
        --charcount;
        if (bx != -1) {
            UnInc(bx, stream);
        }
        if (!started) {
            goto END;
        }
        if (suppress) {
            goto CONV_DONE;
        }
        ++count;
        *esi = 0;
        Fassign(longone - 1, argument, floatBuf);
        goto CONV_DONE;
    }

    SCANSET: {
        // 0x829562
        if (widechar > 0) {
            wideDest = 1;
        }
        ++fmt;
        scanptr = fmt;
        if (*scanptr == '^') {
            ++scanptr;
            reject = 0xFF;
        }
        std::memset(table, 0, 0x20);
        if (*scanptr == ']') {
            prevch = ']';
            ++scanptr;
            table[0xB] = 0x20;
        }
        while (*scanptr != ']') {
            rngch = *scanptr++;
            if (rngch == '-' && prevch != 0 && *scanptr != ']') {
                last = *scanptr++;
                if (prevch < last) {
                    rngch = prevch;
                } else {
                    rngch = last;
                    last = prevch;
                }
                for (unsigned int c = rngch; c <= last; ++c) {
                    table[c >> 3] |= static_cast<unsigned char>(1u << (c & 7));
                }
                prevch = 0;
            } else {
                prevch = rngch;
                table[rngch >> 3] |= static_cast<unsigned char>(1u << (rngch & 7));
            }
        }
        fmt = scanptr;
        edi = comchr;
    }

    STRING_COMMON: {
        // 0x829688
        esi = ebx;
        --charcount;
        if (ch != -1) {
            UnInc(ch, stream);
        }
        for (;;) {
            if (widthset) {
                if (width-- == 0) {
                    goto STRING_END; // 0x829846 (anchura agotada: no se devuelve nada)
                }
            }
            ++charcount;
            ch = Inc(stream);
            if (ch == -1) {
                goto STRING_STOP;
            }
            if (edi != 'c') {
                bool store = false;
                if (edi == 's') {
                    if (!((ch >= 9 && ch <= 0xD) || ch == 0x20)) {
                        store = true;
                    }
                }
                if (!store) {
                    if (edi != '{') {
                        goto STRING_STOP;
                    }
                    const int v = static_cast<signed char>(table[ch >> 3]) ^ static_cast<signed char>(reject);
                    if (!(v & (1 << (ch & 7)))) {
                        goto STRING_STOP;
                    }
                }
            }
            // 0x829714: guardar
            if (suppress) {
                ++esi;
                continue;
            }
            if (wideDest) {
                mbBuf[0] = static_cast<unsigned char>(ch);
                if (CtypeTable()[(ch & 0xFF) * 2 + 1] & 0x80) {
                    ++charcount;
                    mbBuf[1] = static_cast<unsigned char>(Inc(stream));
                }
                wch = 0;
                CallMbtowc(&wch, reinterpret_cast<const char*>(mbBuf), MbCurMax());
                std::memcpy(ebx, &wch, 2);
                ebx += 2;
            } else {
                *ebx++ = static_cast<char>(ch);
            }
            argument = ebx;
        }
    STRING_STOP:
        // 0x829833
        --charcount;
        if (ch != -1) {
            UnInc(ch, stream);
        }
    STRING_END:
        // 0x829846
        if (esi == ebx) {
            goto END;
        }
        if (suppress) {
            goto CONV_DONE;
        }
        ++count;
        if (comchr == 'c') {
            goto CONV_DONE;
        }
        if (wideDest) {
            const unsigned short zero = 0;
            std::memcpy(argument, &zero, 2);
        } else {
            *argument = 0;
        }
        goto CONV_DONE;
    }

    INT_SIGN:
        // 0x829883
        bx = ch;
        if (bx == '-') {
            negative = 1;
            goto SIGN_ADVANCE;
        }
        if (bx == '+') {
            goto SIGN_ADVANCE;
        }
        goto INT_BODY;
    SIGN_ADVANCE:
        // 0x829896
        --width;
        if (width == 0 && widthset) {
            done = 1;
        } else {
            ++charcount;
            bx = Inc(stream);
            ch = bx;
        }
        goto INT_BODY;

    INT_XI:
        // 0x82934D (%i y %x): signo y prefijo 0 / 0x
        if (bx == '-') {
            negative = 1;
            goto XI_ADVANCE;
        }
        if (bx == '+') {
            goto XI_ADVANCE;
        }
        goto XI_PREFIX;
    XI_ADVANCE:
        // 0x829781
        --width;
        if (width == 0 && widthset) {
            done = 1;
        } else {
            ++charcount;
            bx = Inc(stream);
            ch = bx;
        }
    XI_PREFIX:
        // 0x82979F
        if (bx == '0') {
            ++charcount;
            bx = Inc(stream);
            ch = bx;
            if (static_cast<char>(bx) == 'x' || static_cast<char>(bx) == 'X') {
                ++charcount;
                bx = Inc(stream);
                ch = bx;
                if (widthset) {
                    width -= 2;
                    if (width < 1) {
                        ++done;
                    }
                }
                edi = 'x';
                goto INT_BODY;
            }
            started = 1;
            if (edi == 'x') {
                --charcount;
                if (bx != -1) {
                    UnInc(bx, stream);
                }
                bx = '0';
                ch = bx;
                goto INT_BODY;
            }
            if (widthset) {
                if (--width == 0) {
                    ++done;
                }
            }
            edi = 'o';
        }

    INT_BODY:
        // 0x8298B4
        if (integer64) {
            if (!done) {
                for (;;) {
                    bool stop = false;
                    if (edi == 'x' || edi == 'p') {
                        if (!Isxdigit(bx)) {
                            stop = true;
                        } else {
                            num64 <<= 4;
                            if (!Isdigit(bx)) {
                                bx = (bx & ~0x20) - 7;
                            }
                        }
                    } else if (!Isdigit(bx)) {
                        stop = true;
                    } else if (edi == 'o') {
                        if (bx >= '8') {
                            stop = true;
                        } else {
                            num64 <<= 3;
                        }
                    } else {
                        num64 *= 10;
                    }
                    if (stop) {
                        ++done;
                    }
                    if (!done) {
                        ++started;
                        num64 += static_cast<uint64_t>(static_cast<int64_t>(bx - '0'));
                        if (widthset && --width == 0) {
                            done = 1;
                        } else {
                            ++charcount;
                            bx = Inc(stream);
                        }
                    } else {
                        --charcount;
                        if (bx != -1) {
                            UnInc(bx, stream);
                        }
                    }
                    if (done) {
                        break;
                    }
                }
                ch = bx;
            }
            if (negative) {
                num64 = 0 - num64;
            }
        } else {
            if (!done) {
                for (;;) {
                    bool stop = false;
                    if (edi == 'x' || edi == 'p') {
                        if (!Isxdigit(bx)) {
                            stop = true;
                        } else {
                            number <<= 4;
                            if (!Isdigit(bx)) {
                                bx = (bx & ~0x20) - 7;
                            }
                        }
                    } else if (!Isdigit(bx)) {
                        stop = true;
                    } else if (edi == 'o') {
                        if (bx >= '8') {
                            stop = true;
                        } else {
                            number <<= 3;
                        }
                    } else {
                        number = number * 10;
                    }
                    if (stop) {
                        ++done;
                    }
                    if (!done) {
                        ++started;
                        number = number + static_cast<uint32_t>(bx) - '0';
                        if (widthset && --width == 0) {
                            done = 1;
                        } else {
                            ++charcount;
                            bx = Inc(stream);
                        }
                    } else {
                        --charcount;
                        if (bx != -1) {
                            UnInc(bx, stream);
                        }
                    }
                    if (done) {
                        break;
                    }
                }
                ch = bx;
            }
            if (negative) {
                number = 0 - number;
            }
        }
        // 0x829A73
        if (edi == 'F') {
            started = 0;
        }
        if (!started) {
            goto END;
        }
        if (suppress) {
            goto CONV_DONE;
        }
        ++count;
        ebx = argument;
        if (integer64) {
            std::memcpy(ebx, &num64, 8);
        } else if (longone) {
            std::memcpy(ebx, &number, 4);
        } else {
            const unsigned short s16 = static_cast<unsigned short>(number);
            std::memcpy(ebx, &s16, 2);
        }
        goto CONV_DONE;

    CONV_DONE:
        // 0x829AB5
        ++convs;
        ++fmt;
    NEXT_ITEM:
        // 0x829B25
        if (ch != -1) {
            continue;
        }
        if (*fmt != '%') {
            goto END;
        }
        if (fmt[1] == 'n') {
            continue;
        }
        goto END;

    LITERAL_MISMATCH:
        // 0x829B0E
        if (ch == -1) {
            goto END;
        }
        UnInc(ch, stream);
        goto END;
    MISMATCH_EXIT:
        // 0x829541
        if (ch != -1) {
            UnInc(ch, stream);
        }
        goto END;
    }

END:
    // 0x829B41
    if (ch == -1 && count == 0 && convs == 0) {
        return -1;
    }
    return count;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtInputInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Input, 0x8290D4);
    RH_ScopedGlobalInstall(ExeCrt::UngetcNolock, 0x83375D);
    RH_ScopedGlobalInstall(ExeCrt::Getbuf, 0x83361B);
}
#endif
