#pragma once

#include <cstdarg>
#include "CrtState.h"

// Motor de lectura de scanf del CRT estatico del EXE (MSVC 7.1), lote CRT4A:
//   _input 0x8290D4 (%d %i %u %o %x %X %p %c %C %s %S %[...] %n %e %f %g, tamaños h l L w I32 I64, '*', anchura)
//   _ungetc_nolock 0x83375D (el "_un_inc" de _input)   _getbuf 0x83361B
// _inc (0x829094, argumento en edx) esta reconstruida como funcion propia. Dependencias que siguen siendo del EXE (se llaman por su direccion):
//   _filbuf 0x82AF38 (relleno del buffer de un FILE de verdad; una cadena de sscanf nunca llega: devuelve -1),
//   _mbtowc 0x8338CE (propia, CrtLocale; solo %lc/%ls). La conversion del texto de %e/%f/%g (_fassign) es propia: CrtFloatIn.{h,cpp}.
//
// Reconstruido desde el desensamblado, sin __asm. Verificacion: reversing/FnCRT4A/README.md.

namespace ExeCrt {
int __cdecl Input(File* stream, const unsigned char* format, va_list args); // 0x8290D4
int __cdecl UngetcNolock(int c, File* stream);                              // 0x83375D
void __cdecl Getbuf(File* stream);                                          // 0x83361B
} // namespace ExeCrt

void CrtInputInjectHooks();
