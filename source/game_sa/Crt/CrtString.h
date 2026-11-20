#pragma once

// Funciones de cadenas del CRT estatico del EXE (MSVC 7.1) que no dependen del montón, de x87 ni del estado por hilo (lote CRT1A).
// Son versiones ensambladas del CRT (lecturas de 4 bytes alineadas, repne scasb); el resultado observable es el del C estandar con bytes sin signo.
// Validadas par a par contra el EXE fuera del juego: reversing/FnCRT1A.
//
//   strlen 0x826330   strcmp 0x8263C0   strncmp 0x8214D0   strcpy 0x826590
//   strcat 0x8265A0   strncpy 0x821F40  strncat 0x826450   strstr 0x822650

namespace ExeCrt {
unsigned int __cdecl Strlen(const char* str);                                  // 0x826330
int          __cdecl Strcmp(const char* a, const char* b);                     // 0x8263C0 (devuelve -1, 0 o 1; compara bytes sin signo)
int          __cdecl Strncmp(const char* a, const char* b, unsigned int count);// 0x8214D0 (devuelve -1, 0 o 1)
char*        __cdecl Strcpy(char* dest, const char* src);                      // 0x826590
char*        __cdecl Strcat(char* dest, const char* src);                      // 0x8265A0
char*        __cdecl Strncpy(char* dest, const char* src, unsigned int count); // 0x821F40 (rellena con ceros hasta count)
char*        __cdecl Strncat(char* dest, const char* src, unsigned int count); // 0x826450 (siempre escribe el cero final)
char*        __cdecl Strstr(const char* haystack, const char* needle);         // 0x822650
} // namespace ExeCrt

void CrtStringInjectHooks();
