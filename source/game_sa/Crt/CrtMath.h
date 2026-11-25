#pragma once

// floor y ceil del CRT estatico del EXE (MSVC 8, version SSE2 con salida en ST0), lote CRT2A.
//   floor 0x8219F0 (error de dominio 0x3ED)   ceil 0x823820 (0x3EC)
// El EXE entra por la rama SSE2 solo si 0xC9C400 != 0 y las excepciones de MXCSR y de la FPU estan enmascaradas; si no, va a la version x87
// (floor 0x828A60, ceil 0x82BA31): aqui se sustituye por el mismo nucleo de enteros con MXCSR enmascarado (ver CrtMath.cpp). Con NaN el EXE llama a 0x82872C (manejador de error del CRT) y devuelve el argumento.
// Sin __asm. Verificacion: banco fuera del juego (reversing/FnCRT2A).

#include <cstdint>

namespace ExeCrt {
double __cdecl Floor(double x); // 0x8219F0
double __cdecl Ceil(double x);  // 0x823820

// true si el EXE entra por la rama SSE2 (0xC9C400 != 0 y MXCSR/FPU con las excepciones enmascaradas): condicion comun de floor, ceil y pow (CrtPow)
bool UseSse2MathPath();

// Nucleos puros (solo la rama SSE2, sin comprobar el entorno ni llamar al manejador de NaN), para el banco: devuelven true si x es NaN
bool FloorCore(double x, double& result);
bool CeilCore(double x, double& result);

// Lote S4: manejador de errores matematicos 0x82872C (matherr + errno), matherr por defecto 0x833605 (solo por puntero) y equivalente de _ftol2 (0x821B40, argumento en ST0: sin enganche)
void    __cdecl MathError(double* arg1, double* arg2, double* result, int code); // 0x82872C
int     __cdecl DefaultMatherr(void* exception);                                  // 0x833605
int64_t FloatToInt64(double x);                                                   // 0x821B40
} // namespace ExeCrt

void CrtMathInjectHooks();
