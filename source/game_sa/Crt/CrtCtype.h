#pragma once

// Clasificacion de caracteres dependiente del locale del CRT estatico del EXE (MSVC 7.1), lote CRT4A:
//   _tolower 0x825D50   isdigit 0x822789   isspace 0x822802   isxdigit 0x8227C3
// Lote CRT4A-locale (21/11/2026): _toupper 0x823A0E, isalpha 0x8226D6, isupper 0x822715, islower 0x82274F, isalnum 0x822876, _atol 0x822506 (_atoi 0x82258E salta a ella),
//   __stricmp 0x8229B6 (__ascii_stricmp 0x82ABC0), __strnicmp 0x835AC7 (__ascii_strnicmp 0x836990), _strlwr 0x836C58, _strupr 0x836FBA.
// El locale (__updatetlocinfo, _isctype, _tolower_lk, __crtLCMapStringA, ...) ya es propio: CrtLocale.{h,cpp}.
// Todas leen el bloque de locale del hilo (__getptd()->ptlocinfo, +0x64; si no es el global 0x8E2F14 se pide __updatetlocinfo 0x825456, que sigue siendo del EXE):
// con mb_cur_max <= 1 (+0x28) usan la tabla de tipos (+0x48, palabras de 16 bits); con mas de 1 llaman a _isctype 0x82AAE2.

namespace ExeCrt {
// Tabla de tipos global (__pctype, 0x8E3780: palabras de 16 bits indexadas por caracter; bit 0x8000 en el segundo byte = byte inicial de doble byte) y
// caracter decimal del locale (0x8E37D4)
inline const unsigned char* CtypeTable() { return *reinterpret_cast<const unsigned char* const*>(0x8E3780); }
inline char DecimalPointChar() { return *reinterpret_cast<const char*>(0x8E37D4); }

int __cdecl Tolower(int c);   // 0x825D50
int __cdecl Isdigit(int c);   // 0x822789
int __cdecl Isspace(int c);   // 0x822802
int __cdecl Isxdigit(int c);  // 0x8227C3
int __cdecl Toupper(int c);   // 0x823A0E
int __cdecl Isalpha(int c);   // 0x8226D6
int __cdecl Isupper(int c);   // 0x822715
int __cdecl Islower(int c);   // 0x82274F
int __cdecl Isalnum(int c);   // 0x822876
int __cdecl Atol(const char* str);                                       // 0x822506
int __cdecl AsciiStricmp(const char* a, const char* b);                  // 0x82ABC0
int __cdecl AsciiStrnicmp(const char* a, const char* b, unsigned int n); // 0x836990
int __cdecl Stricmp(const char* a, const char* b);                       // 0x8229B6
int __cdecl Strnicmp(const char* a, const char* b, unsigned int n);      // 0x835AC7
char* __cdecl Strlwr(char* str);                                         // 0x836C58
char* __cdecl Strupr(char* str);                                         // 0x836FBA
} // namespace ExeCrt

void CrtCtypeInjectHooks();
