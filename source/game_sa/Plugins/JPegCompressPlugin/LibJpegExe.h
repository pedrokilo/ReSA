#pragma once

/**
 * Ayudantes comunes (solo cabecera) de las rutinas de la libjpeg 6b que lleva el EXE. Las estructuras del EXE
 * (jpeg_common/compress/decompress_struct, jpeg_error_mgr, my_memory_mgr...) no coinciden con las de la libjpeg-turbo
 * de la DLL, asi que se accede a sus campos por desplazamiento. Todas las rutinas del EXE que se llaman son cdecl.
 * jpeg_error_mgr: +0x00 error_exit, +0x04 emit_message, +0x08 output_message, +0x0C format_message, +0x10 reset_error_mgr,
 * +0x14 msg_code, +0x18 msg_parm.i[0], +0x1C msg_parm.i[1].
 */
namespace LibJpegExe {
template<typename T>
inline T& At(void* base, uint32 offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(base) + offset);
}

// ERREXIT: err->msg_code = code; (*err->error_exit)(cinfo). Como el EXE, `err` se relee en cada acceso.
inline void ErrExit(void* cinfo, int32 code) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

// ERREXIT1: ademas msg_parm.i[0] = parm antes de llamar.
inline void ErrExit1(void* cinfo, int32 code, int32 parm) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<int32>(At<void*>(cinfo, 0x00), 0x18) = parm;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

// ERREXIT2: msg_parm.i[0] y msg_parm.i[1].
inline void ErrExit2(void* cinfo, int32 code, int32 parm0, int32 parm1) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<int32>(At<void*>(cinfo, 0x00), 0x18) = parm0;
    At<int32>(At<void*>(cinfo, 0x00), 0x1C) = parm1;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

constexpr int32 JERR_BAD_BUFFER_MODE = 0x04;
constexpr int32 JERR_BAD_LIB_VERSION = 0x0C;
constexpr int32 JERR_BAD_POOL_ID = 0x0E;
constexpr int32 JERR_BAD_STATE = 0x14;
constexpr int32 JERR_BAD_STRUCT_SIZE = 0x15;
constexpr int32 JERR_CANT_SUSPEND = 0x18;
constexpr int32 JERR_CCIR601_NOTIMPL = 0x19;
constexpr int32 JERR_FRACT_SAMPLE_NOTIMPL = 0x26;
constexpr int32 JERR_NO_BACKING_STORE = 0x31;
constexpr int32 JERR_OUT_OF_MEMORY = 0x36;
constexpr int32 JERR_TOO_LITTLE_DATA = 0x43;
constexpr int32 JERR_WIDTH_OVERFLOW = 0x46;
} // namespace LibJpegExe
