#pragma once

// Asignadores del CRT estatico del EXE (MSVC 7.1) para el caso que usa el juego en Windows 2000 o posterior: __active_heap == 1 (montón del sistema, sin
// montón de bloques pequeños, sin cabecera propia, sin redondeo a 16), lote CRT1A. Diseño y razones: reversing/FnCRT1A/DISENO.md.
//
// Estas funciones trabajan SOBRE EL MISMO manejador de montón que las del EXE (_crtheap, 0xC9C2F4): un bloque pedido por una versión se puede liberar,
// redimensionar o medir con la otra sin ninguna conversion. Por eso se pueden enganchar de una en una sin riesgo de mezclar new del EXE con delete propio.
// Estado global del CRT del EXE que leen (se mantiene en las direcciones del EXE hasta que se sustituyan tambien las funciones que lo escriben):
//   0xC9C2F4 _crtheap (HANDLE)   0xC9C2F8 __active_heap (debe valer 1; con 3 hay montón de bloques pequeños y NO se puede sustituir)
//   0xC9ADCC _newmode            0xC9AC68 manejador de new (_set_new_handler), 0 si no hay
//
//   malloc 0x824257 (-> _nh_malloc 0x82422B -> _heap_alloc 0x8241B0)   free 0x82413F   realloc 0x824269   calloc 0x824416
//   operator new 0x82119A (new[] 0x821195 salta a este)   operator delete 0x8214BD (delete[] 0x8213AE salta a 0x8214BD) -> free 0x82413F
//   _set_new_handler 0x826A2D   _query_new_handler 0x826A51   _callnewh 0x826A57   (lote NEWDEL; el EXE no trae _set_new_mode ni las variantes nothrow)
//   0x8212ED (lanza std::bad_alloc: _CxxThrowException con el ThrowInfo 0x8A1ED4) NO se reconstruye: se llama por direccion (ver reversing/FnNEWDEL/README.md)

namespace ExeCrt {
using NewHandlerFn = int(__cdecl*)(unsigned int);
void* __cdecl NhMalloc(unsigned int size, int callNewHandler); // 0x82422B
void* __cdecl Malloc(unsigned int size);                        // 0x824257 (= NhMalloc(size, _newmode))
void  __cdecl Free(void* block);                                // 0x82413F (puntero nulo: no hace nada)
void* __cdecl Realloc(void* block, unsigned int size);          // 0x824269
void* __cdecl Calloc(unsigned int count, unsigned int size);    // 0x824416
void* __cdecl OperatorNew(unsigned int size);                   // 0x82119A (reintenta con el manejador de new; sin memoria y sin manejador: lanza como el EXE)
void* __cdecl OperatorNewArray(unsigned int size);              // 0x821195 (jmp a 0x82119A)
void  __cdecl OperatorDelete(void* block);                      // 0x8214BD (jmp a free 0x82413F)
void  __cdecl OperatorDeleteArray(void* block);                 // 0x8213AE (jmp a 0x8214BD)
unsigned int __cdecl Msize(void* block);                        // 0x828C4A _msize: HeapSize(_crtheap, 0, bloque) (-1 si falla); solo con __active_heap == 1
int   __cdecl CallNewHandler(unsigned int size);                // 0x826A57 _callnewh: 1 si hay manejador y pide reintentar, 0 si no
NewHandlerFn  __cdecl SetNewHandler(NewHandlerFn handler);      // 0x826A2D _set_new_handler: bajo el cerrojo 4 cambia 0xC9AC68 y devuelve el anterior
NewHandlerFn  __cdecl QueryNewHandler();                        // 0x826A51 _query_new_handler

// true si el CRT del EXE usa el montón del sistema (__active_heap == 1), unica condicion en la que estas versiones son equivalentes
bool CrtHeapIsSystemHeap();
} // namespace ExeCrt

void CrtHeapInjectHooks();
