#pragma once

// Clasificacion de caracteres dependiente del locale del CRT estatico del EXE (MSVC 7.1), lote CRT4A:
//   _tolower 0x825D50   isdigit 0x822789   isspace 0x822802   isxdigit 0x8227C3
// Todas leen el bloque de locale del hilo (__getptd()->ptlocinfo, +0x64; si no es el global 0x8E2F14 se pide __updatetlocinfo 0x825456, que sigue siendo del EXE):
// con mb_cur_max <= 1 (+0x28) usan la tabla de tipos (+0x48, palabras de 16 bits); con mas de 1 llaman a _isctype 0x82AAE2 del EXE.
// _tolower solo tiene camino propio para el locale "C" (+0x14 == 0); con otro locale llama a _tolower_lk 0x825C88 del EXE.

namespace ExeCrt {
// Tabla de tipos global (__pctype, 0x8E3780: palabras de 16 bits indexadas por caracter; bit 0x8000 en el segundo byte = byte inicial de doble byte) y
// caracter decimal del locale (0x8E37D4)
inline const unsigned char* CtypeTable() { return *reinterpret_cast<const unsigned char* const*>(0x8E3780); }
inline char DecimalPointChar() { return *reinterpret_cast<const char*>(0x8E37D4); }

int __cdecl Tolower(int c);   // 0x825D50
int __cdecl Isdigit(int c);   // 0x822789
int __cdecl Isspace(int c);   // 0x822802
int __cdecl Isxdigit(int c);  // 0x8227C3
} // namespace ExeCrt

void CrtCtypeInjectHooks();
