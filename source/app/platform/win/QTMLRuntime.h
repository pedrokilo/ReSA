#pragma once

// Lote R10A (sin validar): arranque y parada de la biblioteca estatica de QuickTime (QTML) enlazada en el EXE, 0x8194B0..0x81DAE0 (el listado las atribuia por vecindad a RpMatFX).
// QuickTime.qts se carga a mano (GetModuleHandleA, registro o carpeta del sistema) y sus tres puntos de entrada (theQuickTimeDispatcher, _CallComponent, _CallComponentFunctionWithStorage) se guardan en 0x8E2A0C..0x8E2A14;
// los puentes de QuickTime (0x818430 ... 0x81DAC0) saltan por 0x8E2A0C con el identificador de funcion en EAX y no se pueden escribir en C++.
void QTMLRuntimeInjectHooks();

// Lote LNK5A: QtmlInitialize (0x8195C0) y QtmlTerminate (0x819770) retiradas (nadie las llamaba desde EXT1A).

// Lote S1 (codigo vivo): sustituyen los puentes de QuickTime 0x818650, 0x818670 y 0x81A3C0 (implementacion nula: OSErr -2095, QuickTime no disponible).
int16 __cdecl QtmlConverterFillBridge(void* media, void* handler, int32 sourceSize, uint32* out, int32 position, int32* c, int32* a, int32 z1, int32 z2, int32 z3, int32* b, int32 z4); // 0x81A3C0

// 0x8194D0 (`mov ax, 0F7D1h; ret`, sin argumentos propios; cdecl llamador-limpia): respuesta de los tres puntos de entrada de QuickTime (0x8E2A0C, 0x8E2A10, 0x8E2A14) cuando QuickTime no esta disponible: OSErr -2095.
int16 __cdecl QtmlUnavailableStub();
