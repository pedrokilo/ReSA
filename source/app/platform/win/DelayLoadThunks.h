#pragma once

// Lote R8A (sin validar): puentes de las importaciones de carga retardada del EXE (DINPUT8.dll y DSOUND.dll). Cada puente es un `jmp [ranura de la IAT]`; la ranura apunta al
// ayudante de carga retardada (0x817508 / 0x818020) hasta la primera llamada y despues a la funcion real.
void DelayLoadThunksInjectHooks();
