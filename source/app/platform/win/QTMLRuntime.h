#pragma once

// Lote R10A (sin validar): arranque y parada de la biblioteca estatica de QuickTime (QTML) enlazada en el EXE, 0x8194B0..0x81DAE0 (el listado las atribuia por vecindad a RpMatFX).
// QuickTime.qts se carga a mano (GetModuleHandleA, registro o carpeta del sistema) y sus tres puntos de entrada (theQuickTimeDispatcher, _CallComponent, _CallComponentFunctionWithStorage) se guardan en 0x8E2A0C..0x8E2A14;
// los puentes de QuickTime (0x818430 ... 0x81DAC0) saltan por 0x8E2A0C con el identificador de funcion en EAX y no se pueden escribir en C++.
void QTMLRuntimeInjectHooks();

// Los llama el codigo del juego (CAEMFDecoder::InitLibrary y Shutdown) en lugar de las funciones del EXE 0x8195C0 y 0x819770.
int16 __cdecl QtmlInitialize(int32 flags);
void __cdecl QtmlTerminate();

// Lote S1 (codigo vivo): sustituyen los puentes de QuickTime 0x818650, 0x818670 y 0x81A3C0 (implementacion nula: OSErr -2095, QuickTime no disponible).
int16 __cdecl QtmlConverterFillBridge(void* media, void* handler, int32 sourceSize, uint32* out, int32 position, int32* c, int32* a, int32 z1, int32 z2, int32 z3, int32* b, int32 z4); // 0x81A3C0
