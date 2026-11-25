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
/*
 * Opcion numerica (p. ej. Units=-1|0|1): variable GTASA_SILENTPATCH_<NAME EN MAYUSCULAS>=<entero> o linea `SilentPatch_<name>=<entero>` en el ini.
 * Con el interruptor general apagado devuelve disabledValue (el comportamiento del EXE original).
 *
 * ESQUEMA DE NOMBRES (unico, vigente para todas las ramas de SilentPatch):
 *   general      GTASA_SILENTPATCH=0|1            |  SilentPatch=0|1
 *   por opcion   GTASA_SILENTPATCH_<NOMBRE>=0|1   |  SilentPatch_<Nombre>=0|1
 * donde <Nombre> es la cadena que se pasa a SilentPatchOption / SilentPatchIntOption (sin distinguir mayusculas en el ini; en el entorno se pasa a mayusculas).
 */
int SilentPatchIntOption(const char* name, int defaultValue, int disabledValue);
