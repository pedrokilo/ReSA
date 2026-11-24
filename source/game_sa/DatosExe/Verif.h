#pragma once

/*
 * Verificacion en ejecucion de 'dato en dos sitios' (rama datos-g2g4). Con la variable de entorno DATOSEXE_VERIF=<archivo de registro>
 * un hilo compara periodicamente cada objeto DatosExe::* con la memoria de su direccion del EXE (en el ASI: la del gta-sa.exe; en el
 * independiente: la imagen de datos). Una diferencia significa que quien escribe y quien lee usan sitios distintos.
 * La tabla {direccion, objeto, tamano} la genera contrib/datos_exe_verif_gen.py en VerifTabla.cpp.
 * Variables: DATOSEXE_VERIF_MS (periodo de exploracion, 2000 por defecto), DATOSEXE_VERIF_RETARDO (ms antes de la primera, 15000).
 */

namespace DatosExe::Verif {
struct Entrada {
    unsigned    addr;
    const char* nombre;
    const void* objeto;
    unsigned    tam;
};
struct Cadena {
    unsigned    addr;
    const char* nombre;
    const char* texto;
};
extern const Entrada kTabla[];
extern const unsigned kTablaN;
extern const Cadena kCadenas[];
extern const unsigned kCadenasN;

// Arranca el hilo si DATOSEXE_VERIF esta definida (idempotente). Se llama sola al cargar el modulo.
void Iniciar();
} // namespace DatosExe::Verif
