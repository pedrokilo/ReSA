#pragma once

/*
 * Banco en proceso de PadCheats (desviacion propia 15.P; docs/CodigoVivoExe/trucos-mando.md). Solo actua con la variable de entorno GTASA_PADCHEATS_SELFTEST=1.
 * Escribe logs/padcheats_selftest.log (termina con "SELFTEST FIN"). Fases:
 *   A) detector puro (sin juego): las 87 secuencias, prefijos comunes, reinicio por tiempo, secuencias casi correctas, ruido aleatorio;
 *   B) con el juego: inyecta estados de boton (sin mando fisico) por CCheat::DoCheats y comprueba el efecto por memoria, opcion encendida frente a apagada;
 *   C) en fotogramas reales: escribe el estado del mando en CPad::PCTempJoyState (camino real CPad::Update -> DoCheats) y comprueba el efecto.
 */
namespace PadCheatsSelfTest {
void OnFrame();
} // namespace PadCheatsSelfTest
