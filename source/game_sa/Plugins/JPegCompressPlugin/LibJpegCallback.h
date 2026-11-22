#pragma once

// Lote LIB3A: con GTASA_WITH_LIBJPEG (opcion de CMake, apagada por defecto) los callbacks de libjpeg que este codigo instala por direccion del EXE se sustituyen
// por los de la libjpeg 6b oficial de libs/libjpeg-6b (envoltorio wrap/w_<archivo>.c, configuracion del EXE en exe_config.h). Con la opcion (por defecto encendida) ya no queda ninguna direccion del EXE en el codigo; sin ella sigue siendo la direccion del EXE.
// Las cabeceras de la libjpeg oficial NO se incluyen aqui (chocarian con las de libjpeg-turbo de la DLL): solo se declara el buscador.
#ifdef GTASA_WITH_LIBJPEG
extern "C" void* jpeg6b_find(const char* file, const char* name);
namespace LibJpegExe {
// Sin direccion de respaldo: el codigo propio ya no contiene ninguna direccion del EXE de la libjpeg (lote LNK5A). Todos los nombres estan en
// libs/libjpeg-6b/wrap/w_<archivo>.c; si falta alguno es un error de programacion y se aborta de forma visible.
inline uint32 Callback(const char* file, const char* name) {
    void* fn = jpeg6b_find(file, name);
    if (!fn)
        __debugbreak();
    return static_cast<uint32>(reinterpret_cast<uintptr_t>(fn));
}
} // namespace LibJpegExe
#define LIBJPEG_CB(exeAddress, file, name) LibJpegExe::Callback(file, name) // exeAddress se descarta en el preprocesador: no queda como inmediato en el .obj
#else
#define LIBJPEG_CB(exeAddress, file, name) (exeAddress)
#endif
