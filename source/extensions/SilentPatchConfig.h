#pragma once

// SilentPatch (MIT, Silent): modulo compartido de configuracion de los arreglos portados de SilentPatch para San Andreas.
// Interruptor general: variable de entorno GTASA_SILENTPATCH=0|1 o clave SilentPatch=0|1 en la seccion [SilentPatch] de gta-reversed.ini (por defecto: 1).
// Opcion individual: GTASA_SP_<NOMBRE>=0|1 o <Nombre>=0|1 en [SilentPatch]; el valor por defecto es el de Config\SilentPatchSA.ini de SilentPatch.
// Con el interruptor general apagado, SilentPatchOption() devuelve siempre false (comportamiento del EXE original).

bool SilentPatchEnabled();
bool SilentPatchOption(const char* name, bool defaultValue);
