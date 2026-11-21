#pragma once

// floor y ceil del CRT estatico del EXE (MSVC 8, version SSE2 con salida en ST0), lote CRT2A.
//   floor 0x8219F0 (error de dominio 0x3ED)   ceil 0x823820 (0x3EC)
// El EXE entra por la rama SSE2 solo si 0xC9C400 != 0 y las excepciones de MXCSR y de la FPU estan enmascaradas; si no, va a la version x87
// (floor 0x828A60, ceil 0x82BA31), que se invoca por su direccion. Con NaN el EXE llama a 0x82872C (manejador de error del CRT) y devuelve el argumento.
// Sin __asm. Verificacion: banco fuera del juego (reversing/FnCRT2A).

namespace ExeCrt {
double __cdecl Floor(double x); // 0x8219F0
double __cdecl Ceil(double x);  // 0x823820

// Nucleos puros (solo la rama SSE2, sin comprobar el entorno ni llamar al manejador de NaN), para el banco: devuelven true si x es NaN
bool FloorCore(double x, double& result);
bool CeilCore(double x, double& result);
} // namespace ExeCrt

void CrtMathInjectHooks();
