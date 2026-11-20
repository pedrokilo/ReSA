#pragma once

// qsort del CRT estatico del EXE (MSVC 7.1), 0x8247E0, con su ayudante de ordenacion corta 0x824770 (lote CRT1A).
// No es estable y el orden de los elementos iguales depende de este algoritmo exacto (mediana de tres, corte en 8 elementos, pila explicita de 30 pares):
// no se puede sustituir por std::sort ni por el qsort del compilador sin cambiar el resultado del juego (rpworld, rpskin).
// Validado par a par contra el EXE fuera del juego: reversing/FnCRT1A.

namespace ExeCrt {
using QsortCompare = int(__cdecl*)(const void*, const void*);

// 0x8247E0 (__cdecl, retorno void; el EXE no comprueba comparador nulo; con num < 2 o width == 0 no hace nada)
void __cdecl Qsort(void* base, unsigned int num, unsigned int width, QsortCompare compare);
} // namespace ExeCrt

void CrtQsortInjectHooks();
