#pragma once

#include <cstdint>
#include <cstring>

// Conversion de coma flotante a texto del CRT estatico del EXE (MSVC 7.1), lote CRT4A. Es la pila que usa _output para %e %E %f %g %G:
//
//   _cfltcvt 0x826E61 -> _cftoe 0x826C49 / _cftof 0x826D59 / _cftog 0x826DC1 (+ ayudantes _cftoe2 0x826B9B, _cftof2 0x826CBD y _shift 0x826B7E,
//   que reciben argumentos en registros: no se enganchan, su codigo esta reconstruido dentro de las tres funciones publicas)
//   _fltout2 0x832E1C -> _dtold 0x832D62 + $I10_OUTPUT 0x835F6D (aritmetica entera de 96 bits con tablas de potencias de 10:
//   _multtenpow12 0x83660B, _mulld12 0x8363D3, _shl12 0x835E2E, _shr12 0x835E5C, _add12 0x835DD0, _addl 0x835DAF)
//   _fptostr 0x832CEB (redondeo a n digitos), _forcdecpt 0x826A9F, _cropzeros 0x826ADB.
//
// Todo es aritmetica entera (nada de x87): el resultado es identico en cualquier palabra de control de la FPU.
// Reconstruido desde el desensamblado, sin __asm. Verificacion: reversing/FnCRT4A/README.md.

namespace ExeCrt {
// Lecturas y escrituras sin alineacion (el EXE lee dwords y words en cualquier posicion de los bloques de 12 bytes)
inline uint32_t Load32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
inline void Store32(void* p, uint32_t v) { std::memcpy(p, &v, 4); }
inline uint16_t Load16(const void* p) { uint16_t v; std::memcpy(&v, p, 2); return v; }
inline void Store16(void* p, uint16_t v) { std::memcpy(p, &v, 2); }

// Entero de 96 bits (little endian) que el EXE llama "ld12": palabra 0 = extension, palabras 1..4 = mantisa de 64 bits (bit explicito), palabra 5 = signo y exponente.
struct Ld12 {
    uint32_t d[3];
};
static_assert(sizeof(Ld12) == 12);

// Valor de 80 bits tal como llega a $I10_OUTPUT (se apila entero: 12 bytes; solo los 10 primeros cuentan)
struct Ld10 {
    uint32_t lo;
    uint32_t hi;
    uint32_t se; // signo (bit 15) y exponente (bits 0..14) en la palabra baja
};
static_assert(sizeof(Ld10) == 12);

// Salida de $I10_OUTPUT: posicion del punto decimal, signo (' ' o '-'), numero de digitos y los digitos (cadena terminada en cero)
struct I10Out {
    int16_t decpt;      // +0
    char    sign;       // +2
    char    ndigits;    // +3
    char    digits[0x20]; // +4
};

// STRFLT del CRT: lo que _fltout2 entrega a _fptostr / _cftoe...
struct Strflt {
    int   sign;     // +0 (' ' o '-')
    int   decpt;    // +4
    int   flag;     // +8 (1 = numero normal; 0 = infinito o NaN)
    char* mantissa; // +0xC (apunta al buffer de digitos)
};

uint32_t __cdecl Addl(uint32_t a, uint32_t b, uint32_t* out);   // 0x835DAF (devuelve el acarreo)
void     __cdecl Add12(Ld12* a, const Ld12* b);                  // 0x835DD0
void     __cdecl Shl12(Ld12* a);                                 // 0x835E2E
void     __cdecl Shr12(Ld12* a);                                 // 0x835E5C
void     __cdecl Mulld12(Ld12* a, const Ld12* b);                // 0x8363D3
void     __cdecl Multtenpow12(Ld12* x, int power, int flag);     // 0x83660B
void     __cdecl Dtold(Ld10* out, const double* in);             // 0x832D62
unsigned __cdecl I10Output(Ld10 value, int ndigits, unsigned flags, I10Out* out); // 0x835F6D
Strflt*  __cdecl Fltout2(double value, Strflt* out, char* buffer);  // 0x832E1C
void     __cdecl Fptostr(char* buffer, int digits, Strflt* flt);    // 0x832CEB
void     __cdecl Forcdecpt(char* buffer);                           // 0x826A9F
void     __cdecl Cropzeros(char* buffer);                           // 0x826ADB
char*    __cdecl Cftoe(const double* value, char* buffer, int ndec, int caps); // 0x826C49
char*    __cdecl Cftof(const double* value, char* buffer, int ndec);           // 0x826D59
char*    __cdecl Cftog(const double* value, char* buffer, int ndec, int caps); // 0x826DC1
char*    __cdecl Cfltcvt(const double* value, char* buffer, int format, int ndec, int caps); // 0x826E61
} // namespace ExeCrt

void CrtFloatInjectHooks();
