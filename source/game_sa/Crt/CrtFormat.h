#pragma once

#include <cstdarg>
#include "CrtState.h"

// Motor de formato de printf del CRT estatico del EXE (MSVC 7.1), lote CRT4A:
//   _output 0x827F4B (maquina de estados: banderas, anchura, precision, tamaños h/l/w/I64, tipos d i u o x X p c C s S Z n e E f g G)
// Los ayudantes de escritura (write_char 0x827E9E, write_multi_char 0x827ED1, write_string 0x827EF5) reciben argumentos en registros: no se enganchan,
// estan reconstruidos como funciones propias dentro de este archivo. La conversion de coma flotante esta en CrtFloat.{h,cpp}.
// Dependencias que siguen siendo del EXE y se llaman por su direccion (no se reconstruyeron): _flsbuf 0x828B31 (solo cuando la FILE se queda sin
// buffer: sprintf nunca llega a el) y _wctomb 0x83353C (propia, CrtLocale; solo %c con 'l'/'w' y %ls/%Z con cadena ancha).
//
// Reconstruido desde el desensamblado, sin __asm. Verificacion: reversing/FnCRT4A/README.md.

namespace ExeCrt {
int __cdecl Output(File* stream, const char* format, va_list args); // 0x827F4B
} // namespace ExeCrt

void CrtFormatInjectHooks();
