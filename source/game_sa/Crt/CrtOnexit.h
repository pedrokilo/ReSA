#pragma once

// _onexit del CRT estatico del EXE (MSVC 7.1), lote CRT3A.
//   0x821CE6 __onexit(func): bloquea (lock 8, 0x823B2E / 0x823B37 = __lock(8) / _unlock(8)), registra func en la tabla de atexit y desbloquea; devuelve func, o NULL si no cabe
//   0x821C3E nucleo (__onexit_nolock): recibe func en EDI (convencion propia; solo lo llama 0x821CE6). Se reconstruye entero en C++ (OnexitNoLock), asi no hace falta llamarlo.
// Estado (se mantiene en las direcciones del EXE): 0xC9D42C inicio de la tabla (malloc), 0xC9D428 fin (siguiente hueco). La tabla la crea 0x821CBE (__onexitinit) al arrancar el CRT.
// Crecimiento: tamaño actual (_msize 0x828C4A) + min(tamaño actual, 0x800) (realloc); si falla, +0x10 sobre el tamaño actual; si tambien falla, devuelve NULL.
// El `try/finally` con SEH del EXE (tabla 0x887E20) se sustituye por un objeto RAII (camino normal identico).

namespace ExeCrt {
void* __cdecl OnexitNoLock(void* func); // 0x821C3E (en el EXE: argumento en EDI, no se puede enganchar; solo para __onexit y el banco)
void* __cdecl Onexit(void* func);       // 0x821CE6
} // namespace ExeCrt

void CrtOnexitInjectHooks();
