#pragma once

// Memoria y cadenas del CRT estatico del EXE (MSVC 7.1) que NO se reescriben a mano: se enlazan al CRT de MSVC (lote E2-cadenas de la parte P4 del plan de codigo vivo).
// El EXE trae versiones ensambladas (rep movs / lecturas de 4 bytes) de memcpy, memmove, memset y strchr; su resultado observable es el del C estandar:
//   - 0x8266F0 memcpy y 0x822A20 memmove son el MISMO algoritmo (los dos copian hacia atras si el destino cae dentro de [origen, origen + n)): ambos se enlazan a memmove,
//     que es el superconjunto seguro (misma salida con solapamiento y sin el).
//   - 0x82D4C0 memset (devuelve el destino; con n == 0 no escribe nada) y 0x8213D0 strchr (el cero final cuenta como caracter buscable; el caracter se compara como unsigned char).
// _strdup 0x82514C reserva con el malloc del EXE (ExeCrt::Malloc, el montón del CRT del EXE, para que el bloque se pueda liberar con free/operator delete del EXE) y copia con strcpy.
// En el ejecutable independiente estas cuatro rutinas desaparecen: el enlazador aporta las del CRT de MSVC y los llamadores propios usan las mismas (ver docs/CodigoVivoExe/fase8-arranque.md).

#include <cstddef>

namespace ExeCrt {
void* __cdecl Memcpy(void* dest, const void* src, size_t count);   // 0x8266F0
void* __cdecl Memmove(void* dest, const void* src, size_t count);  // 0x822A20
void* __cdecl Memset(void* dest, int value, size_t count);         // 0x82D4C0
char* __cdecl Strchr(const char* str, int ch);                     // 0x8213D0
char* __cdecl Strdup(const char* str);                             // 0x82514C
} // namespace ExeCrt

void CrtMemInjectHooks();
