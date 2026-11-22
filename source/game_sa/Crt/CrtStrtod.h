#pragma once

#include "CrtFloat.h"

// strtod del CRT estatico del EXE (MSVC 7.1), lote E2-cadenas (parte P4 del plan de codigo vivo). Es la unica rutina de texto a double que faltaba: usa los bloques de CrtFloatIn (CRT4A).
//   _fltin2 0x82BF62(_flt* salida, const char* texto, int longitud, int escala, int decpt): texto -> 96 bits (_strgtold12 0x8346E2) -> double (_ld12tod 0x832B79) y rellena la _flt
//   strtod  0x826071(const char* texto, char** fin): salta espacios (isspace 0x822802), llama a _fltin2 y traduce las banderas; devuelve el double en st0 (ABI cdecl normal)
// Banderas de _flt.flags: 0x001 y 0x080 desbordamiento (+-inf, errno = ERANGE 0x22), 0x100 desbordamiento por abajo (0.0, errno = 0x22), 0x040 y 0x200 sin numero (0.0, *fin = texto).
// El infinito es el valor del EXE en 0x8E39B0 (0x7FF0000000000000).

namespace ExeCrt {
// Estructura _flt del CRT (los campos que escribe _fltin2; +8 y +0xC no se tocan)
struct Flt {
    int    flags;   // +0x00
    int    nbytes;  // +0x04 bytes consumidos del texto
    int    lval;    // +0x08 (no lo escribe _fltin2)
    int    pad;     // +0x0C
    double dval;    // +0x10
};
static_assert(sizeof(Flt) == 0x18);

Flt*   __cdecl Fltin2(Flt* out, const char* str, int len, int scale, int decpt); // 0x82BF62
double __cdecl Strtod(const char* str, char** end);                              // 0x826071
} // namespace ExeCrt

void CrtStrtodInjectHooks();
