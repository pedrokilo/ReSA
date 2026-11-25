#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include "Vista.h"

/*
 * Ronda 5 de DatosExe, lote A (.rdata): objetos que el codigo propio citaba por direccion fija, como definiciones
 * propias con el valor inicial del EXE canonico. Generado por contrib/datos_exe_r5.py; definidas en DataR5A.cpp.
 * Se incluye despues de StdInc.h del consumidor. No se leen del EXE en ejecucion.
 */

namespace DatosExe::R5 {
extern uint32_t v858B44; // 0x00858B44
extern uint32_t v858C28; // 0x00858C28
extern uint32_t v858C54; // 0x00858C54
extern uint32_t v858C80; // 0x00858C80
extern uint32_t v858CBC; // 0x00858CBC
extern uint32_t v858CC0; // 0x00858CC0
extern uint32_t v858CE0; // 0x00858CE0
extern uint32_t v858CEC; // 0x00858CEC
extern uint32_t v858F34; // 0x00858F34
extern uint32_t v858FC4; // 0x00858FC4
extern uint32_t v859B50; // 0x00859B50
extern bool v859CF8; // 0x00859CF8
extern uint32_t v85A5C4; // 0x0085A5C4
extern uint32_t v862F14; // 0x00862F14
extern uint32_t v862F30; // 0x00862F30
extern uint32_t v862F78; // 0x00862F78
extern uint32_t v862F7C; // 0x00862F7C
extern uint32_t v862F88; // 0x00862F88
extern uint32_t v8630FC; // 0x008630FC
extern uint32_t v863100; // 0x00863100
extern uint32_t v8631B0; // 0x008631B0
extern uint32_t v8631B4; // 0x008631B4
extern uint32_t v8631B8; // 0x008631B8
extern uint32_t v8631C0; // 0x008631C0
extern float v871640; // 0x00871640

} // namespace DatosExe::R5
