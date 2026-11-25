#pragma once

// SilentPatch (MIT, Silent): reinicio de variables y de pickups/generadores/saltos de los IPL de texto al empezar Nueva Partida (ver docs/THIRD_PARTY_NOTICES.md).
// Opciones (sin clave en SilentPatchSA.ini: activadas): ResetVariablesOnNewGame, ReinitIplObjectsOnNewGame; interruptor general SilentPatch/GTASA_SILENTPATCH.
namespace SilentPatchNewGame {
//! Al principio de CGame::ReInitGameObjectVariables: pone a su valor inicial las variables que el juego original no reiniciaba
void BeforeReInit();
//! Al final de CGame::ReInitGameObjectVariables: vuelve a cargar los pickups, generadores de coches y saltos acrobaticos de los IPL de texto
void AfterReInit();
//! Llamadas desde CFileLoader al leer las lineas de los IPL de texto
void RecordPickupLine(const char* line);
void RecordCarGeneratorLine(const char* line);
void RecordStuntJumpLine(const char* line);
} // namespace SilentPatchNewGame
