#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include "Vista.h"

/*
 * Ronda 5 de DatosExe, lote B (.data): objetos que el codigo propio citaba por direccion fija, como definiciones
 * propias con el valor inicial del EXE canonico. Generado por contrib/datos_exe_r5.py; definidas en DataR5B.cpp.
 * Se incluye despues de StdInc.h del consumidor. No se leen del EXE en ejecucion.
 */

namespace DatosExe::R5 {
extern int32_t v8D2534; // 0x008D2534

} // namespace DatosExe::R5
