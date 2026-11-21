#pragma once

// pow del CRT estatico del EXE (biblioteca matematica de Intel en SSE2), lote CRT3A.
//   0x8220F0 pow(x, y): si 0xC9C400 != 0 y las excepciones de MXCSR/FPU estan enmascaradas salta (jmp) al nucleo SSE2; si no, va a la rama x87 (0x822184), que se invoca por direccion
//   0x829B99 nucleo SSE2 (2.861 bytes, tablas en 0x8884F0..0x88BE18 copiadas por valor en CrtPowTables.inc). Lo llama tambien _CIpow (0x829B80) con los argumentos en la pila
// Resultado en ST0. Los casos especiales (inf, NaN, 0, desbordamiento, resultado subnormal) terminan en el manejador de errores de la CRT (0x82872C) como en el EXE.
// Sin __asm. Verificacion: banco fuera del juego (reversing/FnCRT3A).

namespace ExeCrt {
double __cdecl Pow(double x, double y);     // 0x8220F0
double __cdecl PowSse2(double x, double y); // 0x829B99
} // namespace ExeCrt

void CrtPowInjectHooks();
