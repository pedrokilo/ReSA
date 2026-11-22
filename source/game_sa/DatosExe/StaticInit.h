#pragma once

// Inicializadores estaticos de _initterm que dependen de DatosExe (generado por contrib/datos_exe_staticinit.py).
// RunStaticInitializers() construye los objetos con constructor (std::construct_at sobre su direccion), registra sus destructores con
// el atexit del CRT propio y escribe los valores iniciales, en el orden de la tabla del EXE. Solo para el ejecutable independiente:
// con la DLL sobre el EXE original el _initterm del EXE ya los ejecuto y esta funcion no se llama.

#include <cstddef>
#include <cstdint>
#include <string>

namespace DatosExe {
void RunStaticInitializers();

// Resultado de VerifyStaticInitializers (ver el .cpp): compara, fila a fila, lo que construiria el C++ propio con lo que dejo el _initterm del EXE.
struct StaticInitReport {
    int      rows           = 0; // filas con tamano comprobable
    int      equal          = 0; // iguales (salvo palabras de vtable del EXE, contadas aparte)
    int      different      = 0; // con alguna diferencia que no es una vtable
    int      vtableWords    = 0; // palabras que son vtable del EXE (.rdata) en el EXE y otra cosa en el C++ propio
    uint32_t firstDifferent = 0; // indice C++ de la primera fila distinta (0 si no hay)
    std::string detail;          // primeras diferencias (fila, direccion, byte del EXE, byte del C++); vacio si no hay
};
StaticInitReport VerifyStaticInitializers();
const StaticInitReport& LastStaticInitReport(); // resultado de la ultima llamada
} // namespace DatosExe
