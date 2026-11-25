#pragma once

/*
 * Correcciones tomadas de SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent)).
 * Aviso y texto de la licencia: docs/THIRD_PARTY_NOTICES.md.
 *
 * DESVIACION DELIBERADA del EXE (reversible): GTASA_SILENTPATCH=0 (general) o GTASA_SILENTPATCH_RTANIMINTERP=0 restituyen el comportamiento exacto del EXE.
 * Ver extensions/SilentPatchConfig.h (entorno e ini). Por defecto ACTIVADAS. Catalogo: docs/DesviacionesConocidas.md (seccion 15).
 */
namespace SilentPatchFixes {
/** SilentPatchOption("RtAnimInterp", true) (se lee una sola vez). */
bool Activo();

/** Solo para GTASA_SILENTPATCH_SELFTEST=1: fuerza (1/0) o libera (-1) el estado de Activo(). */
void ForzarParaPrueba(int estado);

/** Banco aislado dentro del proceso (GTASA_SILENTPATCH_SELFTEST=1, una vez, escribe logs/silentpatch_selftest.log): llena de 0xCC los fotogramas interpolados,
 *  llama a RtAnimInterpolatorSetCurrentAnim con una RtAnimAnimation RpAnimBlend ficticia cuyo pFrames es una direccion alta (0x5A000000) con el arreglo apagado
 *  y encendido, y cuenta floats 0xCCCCCCCC / NaN / enormes en cada caso. */
void AutoPrueba();
}
