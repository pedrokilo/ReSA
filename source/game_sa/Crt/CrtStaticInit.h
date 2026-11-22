#pragma once

#include <cstdint>

// Inicializadores estaticos de constantes x87 del EXE (lote F2 de la parte P5 del plan de codigo vivo, docs/CodigoVivoExe/fase8-arranque.md).
// El CRT del EXE ejecuta en __cinit (0x823B76) los 1.668 inicializadores de C++ de 0x8A4000..0x8A5A10; 1.487 de ellos son constantes que el compilador no pudo plegar y calcula con la x87:
//   fld A; fdiv B; fstp D        D = A / B            (691)
//   fld A; fmul B; fstp D        D = A * B            (518)
//   fld A; fmul B; fdivr C; fstp D   D = C / (A * B)  (230)
//   fld A; fsub B; fstp D        D = A - B            (48)
// con operandos float o double segun el tamaño del acceso. CrtStaticInitTable.inc (generado por contrib/fase8_arranque.py) lista cada uno en su orden de ejecucion.
// Las dos funciones siguen ese orden. La x87 del CRT arranca con 53 bits de mantisa (_fpinit), igual que el double de C++, y el destino float redondea al guardar.
//   Verify...: compara lo que calcularia el C++ con lo que el EXE ya dejo en memoria (con la DLL sobre el EXE los inicializadores ya corrieron): es la prueba de que el calculo es identico.
//   Run...:    escribe los resultados en las direcciones del destino (arranque del ejecutable independiente; en la DLL sobre el EXE no se llama: el EXE ya los calculo).

namespace ExeCrt {
struct StaticInitReport {
    int      total       = 0;
    int      matched     = 0;
    int      mismatched  = 0;
    uint32_t firstMismatch = 0; // direccion del destino de la primera diferencia (0 si no hay)
};

StaticInitReport __cdecl VerifyX87StaticInitializers();
void             __cdecl RunX87StaticInitializers();
} // namespace ExeCrt
