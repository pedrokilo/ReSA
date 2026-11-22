#pragma once

/*
 * Lote DE05-G5 (docs/DatosExe): variables de .bss del juego como definiciones propias (a cero, como el .bss
 * del EXE). Estado vivo: las lee y escribe solo C++. Generado por contrib/datos_exe_de05.py; definidas en DataBssG5.cpp.
 */

#include "GxtChar.h"


namespace DatosExe::Bss {
extern char bA9A7D8[24]; // 0x00A9A7D8
extern char bB71670[352]; // 0x00B71670
extern char bB71A60[128]; // 0x00B71A60
extern RwRGBA bB72CA0; // 0x00B72CA0
extern RwRGBA bB72CA4; // 0x00B72CA4
extern GxtChar bBAA480[400]; // 0x00BAA480


} // namespace DatosExe::Bss
