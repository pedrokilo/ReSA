/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: LibJpegError
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "LibJpegCallback.h"

#include "LibJpegError.h"
#include "LibJpegExe.h"

// Lote 235A (sin validar): jerror.c de la libjpeg 6b del EXE. jpeg_error_mgr: +0x00 error_exit, +0x04 emit_message, +0x08 output_message, +0x0C format_message, +0x10 reset_error_mgr,
// +0x14 msg_code, +0x68 trace_level, +0x6C num_warnings, +0x70 jpeg_message_table, +0x74 last_jpeg_message, +0x78 addon_message_table, +0x7C first_addon_message, +0x80 last_addon_message.
namespace {
using namespace LibJpegExe;

// 0x5CD6B0 (primeros 28 de los 80 B del listado, el resto es output_message): error_exit(cinfo) de jerror. (*err->output_message)(cinfo) (+0x08), jpeg_destroy (0x5C6B90, enganchada
// en LibJpegCompress) y exit(1) del EXE (0x823CAE), que no vuelve.
void ExeErrorExit(void* cinfo) {
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x08)(cinfo);
    reinterpret_cast<void(__cdecl*)(void*)>(0x5C6B90)(cinfo);
    reinterpret_cast<void(__cdecl*)(int32)>(0x823CAE)(1);
}

// 0x5CD6D0 (resto de los 80 B del listado de 0x5CD6B0): output_message(cinfo) de jerror. Buffer de 200 bytes (JMSG_LENGTH_MAX), (*err->format_message)(cinfo, buffer) (+0x0C) y
// fprintf del EXE (0x823A30) sobre el FILE 0x8E2C70 (stderr) con el formato de 0x862EB0.
void ExeOutputMessage(void* cinfo) {
    char buffer[200];
    At<void(__cdecl*)(void*, char*)>(At<void*>(cinfo, 0x00), 0x0C)(cinfo, buffer);
    reinterpret_cast<int32(__cdecl*)(void*, const char*, ...)>(0x823A30)(reinterpret_cast<void*>(0x8E2C70), reinterpret_cast<const char*>(0x862EB0), buffer);
}

// 0x5CD810 (76 B): jpeg_std_error(err) de jerror. Metodos: error_exit 0x5CD6B0, emit_message 0x5CD700, output_message 0x5CD6D0, format_message 0x5CD740, reset_error_mgr 0x5CD7F0 (por direccion);
// trace_level = 0, num_warnings = 0, msg_code = 0, jpeg_message_table = 0x86B7C8, last_jpeg_message = 1 (valor del EXE), addon_message_table = first_addon_message = last_addon_message = 0.
// Devuelve err.
void* ExeJpegStdError(void* err) {
    At<uint32>(err, 0x00) = 0x5CD6B0;
    At<uint32>(err, 0x04) = LIBJPEG_CB(0x5CD700, "jerror", "emit_message");
    At<uint32>(err, 0x08) = 0x5CD6D0;
    At<uint32>(err, 0x0C) = LIBJPEG_CB(0x5CD740, "jerror", "format_message");
    At<uint32>(err, 0x10) = LIBJPEG_CB(0x5CD7F0, "jerror", "reset_error_mgr");
    At<uint32>(err, 0x68) = 0;
    At<uint32>(err, 0x6C) = 0;
    At<uint32>(err, 0x14) = 0;
    At<uint32>(err, 0x70) = 0x86B7C8;
    At<uint32>(err, 0x74) = 1;
    At<uint32>(err, 0x78) = 0;
    At<uint32>(err, 0x7C) = 0;
    At<uint32>(err, 0x80) = 0;
    return err;
}
} // namespace

/**
 * Registra las rutinas de jerror de la libjpeg del EXE (lote 235A, sin validar).
 */
void LibJpegError::InjectHooks() {
    RH_ScopedClass(LibJpegError);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ExeErrorExit, "error_exit", 0x5CD6B0);           // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeOutputMessage, "output_message", 0x5CD6D0);   // lote 235A (sin validar, parte de los 80 B del listado)
    RH_ScopedNamedGlobalInstall(ExeJpegStdError, "jpeg_std_error", 0x5CD810);    // lote 235A (sin validar)
}
