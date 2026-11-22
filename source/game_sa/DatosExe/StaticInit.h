#pragma once

// Inicializadores estaticos de _initterm que dependen de DatosExe (generado por contrib/datos_exe_staticinit.py).
// RunStaticInitializers() construye los objetos con constructor (std::construct_at sobre su direccion), registra sus destructores con
// el atexit del CRT propio y escribe los valores iniciales, en el orden de la tabla del EXE. Solo para el ejecutable independiente:
// con la DLL sobre el EXE original el _initterm del EXE ya los ejecuto y esta funcion no se llama.

namespace DatosExe {
void RunStaticInitializers();
} // namespace DatosExe
