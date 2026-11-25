#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

/*
 * Ronda 5 de DatosExe, lote C (.bss): objetos que el codigo propio citaba por direccion fija, como definiciones
 * propias con el valor inicial del EXE canonico. Generado por contrib/datos_exe_r5.py; definidas en DataR5C.cpp.
 * Se incluye despues de StdInc.h del consumidor. No se leen del EXE en ejecucion.
 */

namespace DatosExe::R5 {
extern alignas(16) unsigned char r9654B4[4]; // 0x009654B4
extern uint8_t v96917A; // 0x0096917A
extern uint8_t v96917C; // 0x0096917C
extern uint8_t v96917D; // 0x0096917D
extern uint8_t v96917E; // 0x0096917E
extern uint8_t v969180; // 0x00969180
extern alignas(16) unsigned char r96A7D0[4]; // 0x0096A7D0
extern alignas(16) unsigned char rA9A888[4]; // 0x00A9A888
extern uint8_t vB4E3E8[4]; // 0x00B4E3E8
extern uint8_t vB4E3F0[4]; // 0x00B4E3F0
extern uint32_t vB61100[100]; // 0x00B61100
extern alignas(16) unsigned char rB6B99C[4]; // 0x00B6B99C
extern float vB6EC10; // 0x00B6EC10
extern float vB6EC14; // 0x00B6EC14
extern float vB6EC18; // 0x00B6EC18
extern bool vB73402; // 0x00B73402
extern alignas(16) unsigned char rB745BC[4]; // 0x00B745BC
extern alignas(16) unsigned char rB745CC[4]; // 0x00B745CC
extern alignas(16) unsigned char rB745D0[4]; // 0x00B745D0
extern uint8_t vBAA38A; // 0x00BAA38A
extern alignas(16) unsigned char rBC4094[4]; // 0x00BC4094
extern int32_t vC02C20; // 0x00C02C20
extern alignas(16) unsigned char rC02D28[4]; // 0x00C02D28
extern alignas(16) unsigned char rC02D2C[4]; // 0x00C02D2C
extern alignas(16) unsigned char rC02D30[4]; // 0x00C02D30
extern uint32_t vC1BFD8; // 0x00C1BFD8
extern bool vC1CC01; // 0x00C1CC01
extern alignas(16) unsigned char rC1CC50[12]; // 0x00C1CC50
extern std::array<int32_t, 180> vC3FE08; // 0x00C3FE08
extern float vC4032C; // 0x00C4032C
extern uint32_t vC4B8C8; // 0x00C4B8C8
extern alignas(16) unsigned char rC79A88[4]; // 0x00C79A88
extern uint8_t vC88658; // 0x00C88658
extern alignas(16) unsigned char rC8CFB0[8]; // 0x00C8CFB0
extern alignas(16) unsigned char rC8CFB8[8]; // 0x00C8CFB8

} // namespace DatosExe::R5
