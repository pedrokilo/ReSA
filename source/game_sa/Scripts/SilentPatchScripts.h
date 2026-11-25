#pragma once

// SilentPatch (MIT, Silent): arreglos de bytecode SCM y del comando GET_CURRENT_DATE portados de SilentPatchSA (ver docs/THIRD_PARTY_NOTICES.md).
// Opcion: EnableScriptFixes (SilentPatchSA.ini: 1 por defecto); interruptor general SilentPatch/GTASA_SILENTPATCH.
#include "extensions/SilentPatchConfig.h"

namespace SilentPatchScripts {
//! Tras cargar una partida (CTheScripts::Load): arreglo del baloncesto y del bono de salto cuadruple
void OnScriptsLoaded();
//! Tras escribir el script de mision en MissionBlock y borrar sus variables locales (LOAD_AND_LAUNCH_MISSION_INTERNAL)
void OnMissionStarted(int32 missionId);
//! GET_CURRENT_DATE: arreglo del "gym glitch". Devuelve true si cambio los valores (dia, mes)
bool GymGlitchFixDate(const char* scriptName, int32& day, int32& month);
} // namespace SilentPatchScripts
