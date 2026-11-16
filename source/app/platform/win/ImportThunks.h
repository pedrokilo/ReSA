#pragma once

// Lote R9A (sin validar): puentes `jmp [ranura de la IAT]` del EXE hacia vorbisfile.dll (7) y WS2_32.dll (7), en 0x818062..0x818086 y 0x81809E..0x8180C2.
void ImportThunksInjectHooks();
