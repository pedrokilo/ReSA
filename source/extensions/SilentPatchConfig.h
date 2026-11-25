#pragma once

/*
 * Interruptor general de las correcciones tomadas de SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent); ver docs/THIRD_PARTY_NOTICES.md).
 * Todas son DESVIACIONES DELIBERADAS del EXE original y opcionales; con el interruptor apagado el comportamiento es el del EXE.
 *
 * Interruptor general: variable de entorno GTASA_SILENTPATCH=0|1, o linea `SilentPatch=0|1` (cualquier seccion) en gta-reversed.ini del directorio del juego.
 * El entorno gana al ini. POR DEFECTO ENCENDIDO: para cambiar el valor por defecto, edite kSilentPatchPorDefecto en SilentPatchConfig.cpp (una linea).
 * Opcion individual `name`: variable GTASA_SILENTPATCH_<NAME EN MAYUSCULAS>=0|1 o linea `SilentPatch_<name>=0|1` en el ini; por defecto `defaultValue`.
 * Con el general apagado toda opcion devuelve false.
 */
bool SilentPatchEnabled();
bool SilentPatchOption(const char* name, bool defaultValue);
