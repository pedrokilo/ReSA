# Revisión del banco (10/10/2026)

Resultado de partida: `20261010-143839-960710/comparisons.jsonl`.

## FIXED: ruido de ABI de G6

`G6_Vol` era un falso C++ que podía destruir ECX. El EXE `GetVolForPlayerEngineSound` (0x4F5D00..0x4F5E2D) nunca escribe ECX: solo EAX y EDX. Su llamador `ProcessPlayerVehicleEngine` ejecuta `call 0x4F5D00` en 0x4FC9C1 y `call 0x4F8070` en 0x4FC9CD sin recargar ECX; igual en 0x4FC903/0x4FC90F y otras ramas. Esto explica la traza 197 con receptor original 0xE0000000 y candidato w.ent. Se añadió wrapper naked que salva/restaura ECX, mantiene ST0 y limpia ocho bytes con ret 8. No se oculta la diferencia; se corrige el falso.

## Diagnóstico: diferencias reales de solicitudes

Las primeras muestras completas muestran diferencias en CAESound +0x14 (volumen, generalmente 1–2 ULP), no relleno ni punteros absolutos, en AddAudioEvent-1, ProcessMovingParts, ProcessVehicleSkidding, ProcessVehicleFlatTyre, ProcessReverseGear, ProcessAircraft, ProcessDummyProp, ProcessPlayerHeli, ProcessPlayerSeaPlane, ProcessDummySeaPlane, ProcessDummyRCPlane, ProcessDummyRCHeli, ProcessTrainTrackSound y ProcessDummyTrainEngine. ProcessVehicleRoadNoise caso 0 difiere en +0x1C (rolloff original 0x41B287E2, candidato 0x3F800000). ProcessPlayerTrainBrakes caso 3 difiere 1 ULP en argumento final de la traza 185. Requieren corrección de fuente; no se han enmascarado.

`world` solo mostraba un hash. Se añadió `diff_world` en registros de fallo: hasta 64 bytes con offset relativo a World, sin modificar el criterio de igualdad, excluyendo únicamente m_tempSound, ya excluido por WorldHash. Permitirá clasificar estados de sonidos y entidades en la siguiente corrida.

Pendiente: ejecutar banco en serie por el coordinador. Este agente no compiló, no ejecutó juego, no tocó Git, fuente ni runner.
