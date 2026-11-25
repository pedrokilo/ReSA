#pragma once

/*
 * Banco aislado DENTRO del proceso para los arreglos de SilentPatch de peds/jugador/generadores (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent); docs/THIRD_PARTY_NOTICES.md).
 * Solo actua con la variable de entorno GTASA_SILENTPATCH_SELFTEST_PEDS=1 (sin ella OnFrame solo lee una variable estatica). Escribe logs/silentpatch_peds_selftest.log.
 * Ejecuta cada comprobacion UNA vez, con el juego ya cargado, llamando a nuestras funciones reconstruidas con entradas controladas; el resultado depende de las opciones
 * (GTASA_SILENTPATCH_<NOMBRE>=0|1), asi que se compara una ejecucion con ellas encendidas y otra apagadas.
 */
namespace SilentPatchPedsSelfTest {
void OnFrame();
} // namespace SilentPatchPedsSelfTest
