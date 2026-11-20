#pragma once

// Lote LIB3A: con GTASA_WITH_LIBJPEG (opcion de CMake, apagada por defecto) los callbacks de libjpeg que este codigo instala por direccion del EXE se sustituyen
// por los de la libjpeg 6b oficial de libs/libjpeg-6b (envoltorio wrap/w_<archivo>.c, configuracion del EXE en exe_config.h). Sin la opcion sigue siendo la direccion del EXE.
// Las cabeceras de la libjpeg oficial NO se incluyen aqui (chocarian con las de libjpeg-turbo de la DLL): solo se declara el buscador.
#ifdef GTASA_WITH_LIBJPEG
extern "C" void* jpeg6b_find(const char* file, const char* name);
namespace LibJpegExe {
inline uint32 Callback(uint32 exeAddress, const char* file, const char* name) {
    void* fn = jpeg6b_find(file, name);
    return fn ? static_cast<uint32>(reinterpret_cast<uintptr_t>(fn)) : exeAddress;
}
} // namespace LibJpegExe
#define LIBJPEG_CB(exeAddress, file, name) LibJpegExe::Callback(exeAddress, file, name)
#else
#define LIBJPEG_CB(exeAddress, file, name) (exeAddress)
#endif
