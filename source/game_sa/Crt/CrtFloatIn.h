#pragma once

#include <cstdint>
#include "CrtFloat.h"

// Conversion de texto a coma flotante del CRT estatico del EXE (MSVC 7.1), lote CRT4A: lo que usa scanf (%e %f %g) a traves de _fassign.
//
//   _fassign 0x826B40 -> _atodbl 0x832C21 / _atoflt 0x832CA8 -> _strgtold12 0x8346E2 (texto -> 96 bits, maquina de 12 estados)
//                                                             -> _ld12tod 0x832B79 / _ld12tof 0x832B8F -> _ld12cvt 0x832A21 (96 bits -> IEEE con redondeo)
//   _mtold12 0x835E89 (digitos -> 96 bits) y los ayudantes de 96 bits de _ld12cvt: 0x832875, 0x8328A7, 0x8328F4, 0x832966, 0x832981, 0x83298D, 0x8329A6.
//
// Aritmetica entera pura, sin x87. Reconstruido desde el desensamblado, sin __asm. Verificacion: reversing/FnCRT4A/README.md.

namespace ExeCrt {
// Descriptor de formato IEEE que usa _ld12cvt (0x8E39B8 para double, 0x8E39D0 para float)
struct FloatFormat {
    int maxExp;    // +0x00
    int minExp;    // +0x04
    int mantBits;  // +0x08
    int expBits;   // +0x0C
    int totalBits; // +0x10
    int bias;      // +0x14
};

uint32_t __cdecl Ld12TailZero(const uint32_t* w, int bit);  // 0x832875
uint32_t __cdecl Ld12AddOne(uint32_t* w, int bit);          // 0x8328A7
int      __cdecl Ld12Round(uint32_t* w, int bit);           // 0x8328F4
void     __cdecl Ld12Copy(uint32_t* dst, const uint32_t* src); // 0x832966
void     __cdecl Ld12Zero(uint32_t* dst);                   // 0x832981
int      __cdecl Ld12IsZero(const uint32_t* w);             // 0x83298D
void     __cdecl Ld12ShiftRight(uint32_t* w, int bits);     // 0x8329A6
int      __cdecl Ld12Cvt(const Ld12* in, uint32_t* out, const FloatFormat* fmt); // 0x832A21
int      __cdecl Ld12ToDouble(const Ld12* in, uint32_t* out);  // 0x832B79
int      __cdecl Ld12ToFloat(const Ld12* in, uint32_t* out);   // 0x832B8F
void     __cdecl Mtold12(const char* digits, unsigned int count, Ld12* out); // 0x835E89
int      __cdecl Strgtold12(Ld12* out, const char** endptr, const char* str, int multflag, int scale, int adjust, int mode); // 0x8346E2
int      __cdecl Atodbl(double* out, const char* str);      // 0x832C21
int      __cdecl Atoflt(float* out, const char* str);       // 0x832CA8
void     __cdecl Fassign(int isLong, void* argument, const char* text); // 0x826B40
} // namespace ExeCrt

void CrtFloatInInjectHooks();
