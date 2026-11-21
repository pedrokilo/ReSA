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
#include "CrtFormat.h"
#include "CrtFloat.h"
#include "CrtHeap.h"
#include "CrtString.h"
#include "CrtCtype.h"

// Reconstruccion de _output (0x827F4B) del CRT del EXE (lote CRT4A). Ver CrtFormat.h y reversing/FnCRT4A/README.md.

#include "CrtFormatTable.inc"

namespace ExeCrt {
namespace {
// Banderas (en [ebp-8] del EXE)
constexpr unsigned int FL_SIGN = 0x1, FL_SIGNSP = 0x2, FL_LEFT = 0x4, FL_LEADZERO = 0x8, FL_LONG = 0x10, FL_SHORT = 0x20, FL_SIGNED = 0x40,
                       FL_ALTERNATE = 0x80, FL_NEGATIVE = 0x100, FL_FORCEOCTAL = 0x200, FL_WIDECHAR = 0x800, FL_I64 = 0x8000;

// Funciones del EXE que quedan por direccion (ver CrtFormat.h)
inline int CallFlsbuf(int ch, File* f) { return reinterpret_cast<int(__cdecl*)(int, File*)>(0x828B31)(ch, f); }
inline int CallWctomb(char* dst, int wc) { return reinterpret_cast<int(__cdecl*)(char*, int)>(0x83353C)(dst, wc); }

// 0x827E9E (write_char: ecx = FILE, esi = &numwritten, al = caracter)
void WriteChar(unsigned char ch, File* f, int* numWritten) {
    if ((f->flag & 0x40) && f->base == nullptr) { // _IOSTRG sin buffer: solo se cuenta
        ++*numWritten;
        return;
    }
    int r;
    if (--f->cnt >= 0) {
        *f->ptr++ = static_cast<char>(ch);
        r = ch;
    } else {
        r = CallFlsbuf(static_cast<signed char>(ch), f);
    }
    if (r == -1) {
        *numWritten = -1;
    } else {
        ++*numWritten;
    }
}

// 0x827ED1 (write_multi_char)
void WriteMultiChar(unsigned char ch, int count, File* f, int* numWritten) {
    while (count > 0) {
        --count;
        WriteChar(ch, f, numWritten);
        if (*numWritten == -1) {
            return;
        }
    }
}

// 0x827EF5 (write_string)
void WriteString(const char* s, int len, File* f, int* numWritten) {
    if ((f->flag & 0x40) && f->base == nullptr) {
        *numWritten += len;
        return;
    }
    while (len > 0) {
        --len;
        WriteChar(static_cast<unsigned char>(*s), f, numWritten);
        ++s;
        if (*numWritten == -1) {
            return;
        }
    }
}

inline char*& NullString()     { return *reinterpret_cast<char**>(0x8E3120); } // "(null)"
inline char*& WideNullString() { return *reinterpret_cast<char**>(0x8E3124); } // L"(null)"
} // namespace

// 0x827F4B
int __cdecl Output(File* f, const char* format, va_list args) {
    char* ap = reinterpret_cast<char*>(args); // va_list de x86 = puntero a la pila
    int charsOut = 0;
    int textLen = 0;
    char* heapBuffer = nullptr;
    char buffer[0x200];

    unsigned char ch = static_cast<unsigned char>(*format);
    if (ch == 0) {
        return 0;
    }

    unsigned int flags = 0;
    int precision = -1;
    int fieldWidth = 0;
    int prefixLen = 0;
    char prefix[2] = {0, 0};
    int bufferIsWide = 0;
    int noOutput = 0;
    int hexAdd = 0;
    int capExp = 0;
    int radix = 0;
    char* text = nullptr;
    int state = 0;
    const char* fmt = format;
    char mbBuf[8];

    do {
        const int previous = state;
        ++fmt;
        if (charsOut < 0) {
            break;
        }
        // clase del caracter y siguiente estado
        int cls;
        if (static_cast<signed char>(ch) < 0x20 || static_cast<signed char>(ch) > 0x78) {
            cls = 0;
        } else {
            cls = static_cast<signed char>(kOutputStateTable[static_cast<signed char>(ch) - 0x20]) & 0xF;
        }
        state = static_cast<signed char>(kOutputStateTable[previous + cls * 8]) >> 4;

        switch (static_cast<unsigned int>(state)) {
        case 0: // ST_NORMAL
        NORMAL_CHAR:
            bufferIsWide = 0;
            if (CtypeTable()[ch * 2 + 1] & 0x80) { // caracter de doble byte: primer byte y despues el siguiente
                WriteChar(ch, f, &charsOut);
                ch = static_cast<unsigned char>(*fmt++);
            }
            WriteChar(ch, f, &charsOut);
            break;

        case 1: // ST_PERCENT
            precision = -1;
            fieldWidth = 0;
            prefixLen = 0;
            flags = 0;
            capExp = 0;
            noOutput = 0;
            bufferIsWide = 0;
            break;

        case 2: // ST_FLAG
            switch (static_cast<signed char>(ch)) {
            case ' ': flags |= FL_SIGNSP; break;
            case '#': flags |= FL_ALTERNATE; break;
            case '+': flags |= FL_SIGN; break;
            case '-': flags |= FL_LEFT; break;
            case '0': flags |= FL_LEADZERO; break;
            default: break;
            }
            break;

        case 3: // ST_WIDTH
            if (ch == '*') {
                ap += 4;
                fieldWidth = *reinterpret_cast<int*>(ap - 4);
                if (fieldWidth < 0) {
                    flags |= FL_LEFT;
                    fieldWidth = -fieldWidth;
                }
            } else {
                fieldWidth = fieldWidth * 10 + static_cast<signed char>(ch) - '0';
            }
            break;

        case 4: // ST_DOT
            precision = 0;
            break;

        case 5: // ST_PRECIS
            if (ch == '*') {
                ap += 4;
                precision = *reinterpret_cast<int*>(ap - 4);
                if (precision < 0) {
                    precision = -1;
                }
            } else {
                precision = precision * 10 + static_cast<signed char>(ch) - '0';
            }
            break;

        case 6: // ST_SIZE
            if (ch == 'I') {
                const unsigned char n0 = static_cast<unsigned char>(fmt[0]);
                if (n0 == '6' && fmt[1] == '4') {
                    fmt += 2;
                    flags |= FL_I64;
                } else if (n0 == '3' && fmt[1] == '2') {
                    fmt += 2;
                    flags &= ~FL_I64;
                } else if (n0 == 'd' || n0 == 'i' || n0 == 'o' || n0 == 'u' || n0 == 'x' || n0 == 'X') {
                    // 'I' seguido de un tipo: se ignora
                } else {
                    state = 0;
                    goto NORMAL_CHAR;
                }
            } else if (ch == 'h') {
                flags |= FL_SHORT;
            } else if (ch == 'l') {
                flags |= FL_LONG;
            } else if (ch == 'w') {
                flags |= FL_WIDECHAR;
            }
            break;

        case 7: { // ST_TYPE
            const int type = static_cast<signed char>(ch);
            bool integerPath = false;   // pasa por el bloque comun de enteros (0x828295)
            bool finish = false;        // salta al bloque de salida (0x82859E)
            int lowered = type;

            if (type == 'e' || type == 'E' || type == 'f' || type == 'g' || type == 'G') {
                if (type == 'E' || type == 'G') {
                    capExp = 1;
                    lowered = type + 0x20;
                }
                flags |= FL_SIGNED;
                text = buffer;
                if (precision < 0) {
                    precision = 6;
                } else if (precision == 0) {
                    if (lowered == 'g') {
                        precision = 1;
                    }
                } else {
                    if (precision > 0x200) {
                        precision = 0x200;
                    }
                    if (precision > 0xA3) {
                        char* big = static_cast<char*>(Malloc(static_cast<unsigned int>(precision) + 0x15D));
                        if (big) {
                            heapBuffer = big;
                            text = big;
                        } else {
                            precision = 0xA3;
                        }
                    }
                }
                double value;
                std::memcpy(&value, ap, 8);
                ap += 8;
                Cfltcvt(&value, text, lowered, precision, capExp);
                if ((flags & FL_ALTERNATE) && precision == 0) {
                    Forcdecpt(text);
                }
                if (lowered == 'g' && !(flags & FL_ALTERNATE)) {
                    Cropzeros(text);
                }
                if (*text == '-') {
                    flags |= FL_NEGATIVE;
                    ++text;
                }
                textLen = static_cast<int>(Strlen(text));
                finish = true;
            } else if (type == 'c' || type == 'C') {
                if (type == 'C' && !(flags & (FL_SHORT | FL_LONG | FL_WIDECHAR))) {
                    flags |= FL_WIDECHAR;
                }
                ap += 4;
                if (flags & (FL_LONG | FL_WIDECHAR)) {
                    const int wc = *reinterpret_cast<short*>(ap - 4);
                    textLen = CallWctomb(buffer, wc);
                    if (textLen < 0) {
                        noOutput = 1;
                    }
                } else {
                    buffer[0] = *(ap - 4);
                    textLen = 1;
                }
                text = buffer;
                finish = true;
            } else if (type == 's' || type == 'S') {
                if (type == 'S' && !(flags & (FL_SHORT | FL_LONG | FL_WIDECHAR))) {
                    flags |= FL_WIDECHAR;
                }
                int limit = precision;
                if (limit == -1) {
                    limit = 0x7FFFFFFF;
                }
                ap += 4;
                text = *reinterpret_cast<char**>(ap - 4);
                if (flags & (FL_LONG | FL_WIDECHAR)) {
                    if (text == nullptr) {
                        text = WideNullString();
                    }
                    bufferIsWide = 1;
                    const unsigned short* w = reinterpret_cast<const unsigned short*>(text);
                    while (limit > 0) {
                        --limit;
                        if (*w == 0) {
                            break;
                        }
                        ++w;
                    }
                    textLen = static_cast<int>((reinterpret_cast<const char*>(w) - text) >> 1);
                } else {
                    if (text == nullptr) {
                        text = NullString();
                    }
                    const char* s = text;
                    while (limit > 0) {
                        --limit;
                        if (*s == 0) {
                            break;
                        }
                        ++s;
                    }
                    textLen = static_cast<int>(s - text);
                }
                finish = true;
            } else if (type == 'Z') {
                ap += 4;
                char* const* ansi = reinterpret_cast<char* const*>(*reinterpret_cast<char**>(ap - 4));
                if (ansi == nullptr || ansi[1] == nullptr) {
                    text = NullString();
                    textLen = static_cast<int>(Strlen(text));
                } else {
                    const short length = *reinterpret_cast<const short*>(ansi);
                    text = ansi[1];
                    if (flags & FL_WIDECHAR) {
                        textLen = (length - (length >> 31)) >> 1; // cdq/sub/sar: division entre 2 que redondea hacia cero
                        bufferIsWide = 1;
                    } else {
                        textLen = length;
                        bufferIsWide = 0;
                    }
                }
                finish = true;
            } else if (type == 'n') {
                ap += 4;
                void* const target = *reinterpret_cast<void**>(ap - 4);
                if (flags & FL_SHORT) {
                    *static_cast<short*>(target) = static_cast<short>(charsOut);
                } else {
                    *static_cast<int*>(target) = charsOut;
                }
                noOutput = 1;
                finish = true;
                // el EXE salta directamente al final (0x82868D): no imprime nada
            } else if (type == 'd' || type == 'i') {
                flags |= FL_SIGNED;
                radix = 10;
                integerPath = true;
            } else if (type == 'u') {
                radix = 10;
                integerPath = true;
            } else if (type == 'o') {
                radix = 8;
                if (flags & FL_ALTERNATE) {
                    flags |= FL_FORCEOCTAL;
                }
                integerPath = true;
            } else if (type == 'p' || type == 'X' || type == 'x') {
                if (type == 'p') {
                    precision = 8;
                }
                hexAdd = (type == 'x') ? 0x27 : 7;
                radix = 16;
                if (flags & FL_ALTERNATE) {
                    prefix[0] = '0';
                    prefix[1] = static_cast<char>(hexAdd + 0x51);
                    prefixLen = 2;
                }
                integerPath = true;
            } else {
                finish = true; // tipo desconocido: no escribe nada (0x82859E sin texto previo)
            }

            if (integerPath) {
                uint64_t number;
                if (flags & FL_I64) {
                    std::memcpy(&number, ap, 8);
                    ap += 8;
                } else {
                    ap += 4;
                    if (flags & FL_SHORT) {
                        number = (flags & FL_SIGNED) ? static_cast<uint64_t>(static_cast<int64_t>(*reinterpret_cast<short*>(ap - 4)))
                                                     : static_cast<uint64_t>(*reinterpret_cast<unsigned short*>(ap - 4));
                    } else {
                        number = (flags & FL_SIGNED) ? static_cast<uint64_t>(static_cast<int64_t>(*reinterpret_cast<int*>(ap - 4)))
                                                     : static_cast<uint64_t>(*reinterpret_cast<unsigned int*>(ap - 4));
                    }
                }
                if ((flags & FL_SIGNED) && static_cast<int64_t>(number) < 0) {
                    number = 0 - number;
                    flags |= FL_NEGATIVE;
                }
                if (!(flags & FL_I64)) {
                    number &= 0xFFFFFFFFull;
                }
                if (precision < 0) {
                    precision = 1;
                } else {
                    flags &= ~FL_LEADZERO;
                    if (precision > 0x200) {
                        precision = 0x200;
                    }
                }
                if (number == 0) {
                    prefixLen = 0;
                }
                char* const end = buffer + 0x1FF;
                char* p = end;
                while (true) {
                    const int pr = precision--;
                    if (!(pr > 0 || number != 0)) {
                        break;
                    }
                    int digit = static_cast<int>(number % static_cast<uint64_t>(radix)) + '0';
                    number /= static_cast<uint64_t>(radix);
                    if (digit > '9') {
                        digit += hexAdd;
                    }
                    *p-- = static_cast<char>(digit);
                }
                textLen = static_cast<int>(end - p);
                text = p + 1;
                if ((flags & FL_FORCEOCTAL) && (*text != '0' || textLen == 0)) {
                    *--text = '0';
                    ++textLen;
                }
                finish = true;
            }
            (void)finish;

            if (!noOutput) {
                if (flags & FL_SIGNED) {
                    if (flags & FL_NEGATIVE) {
                        prefix[0] = '-';
                        prefixLen = 1;
                    } else if (flags & FL_SIGN) {
                        prefix[0] = '+';
                        prefixLen = 1;
                    } else if (flags & FL_SIGNSP) {
                        prefix[0] = ' ';
                        prefixLen = 1;
                    }
                }
                int padding = fieldWidth - prefixLen - textLen;
                if (!(flags & (FL_LEFT | FL_LEADZERO))) {
                    WriteMultiChar(' ', padding, f, &charsOut);
                }
                WriteString(prefix, prefixLen, f, &charsOut);
                if ((flags & FL_LEADZERO) && !(flags & FL_LEFT)) {
                    WriteMultiChar('0', padding, f, &charsOut);
                }
                if (bufferIsWide && textLen > 0) {
                    const unsigned short* w = reinterpret_cast<const unsigned short*>(text);
                    int remaining = textLen;
                    do {
                        --remaining;
                        const int n = CallWctomb(mbBuf, *w++);
                        if (n <= 0) {
                            break;
                        }
                        WriteString(mbBuf, n, f, &charsOut);
                    } while (remaining != 0);
                } else {
                    WriteString(text, textLen, f, &charsOut);
                }
                if (flags & FL_LEFT) {
                    WriteMultiChar(' ', padding, f, &charsOut);
                }
            }
            if (heapBuffer) {
                Free(heapBuffer);
                heapBuffer = nullptr;
            }
            break;
        }
        default:
            break;
        }
        ch = static_cast<unsigned char>(*fmt);
    } while (ch != 0);
    return charsOut;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtFormatInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Output, 0x827F4B);
}
#endif
