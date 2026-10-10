<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeAI355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::ProcessAI`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-ai355/20261008-065039-140758/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# BikeAI355

Preparado: 1.024 pares completos CBike::ProcessAI (0x6BC930), banco aislado; hook false y aprobación pendiente del coordinador. Fuente reconstruida desde ASM completa Bike_AIFull.txt y pseudocódigo privado tasks355-bike-pseudo/CBike.ProcessAI_6BC930.c, EXE canónico72ae59e4.

Sesenta y cuatro fixtures por estado 0..15, con área cargada/no cargada, conductor jugador1/jugador2/ausente, pasajeros, lean positivo/negativo, freno, contacto, IA, cambios de marcha y ruedas. Compara retorno, flags completos, todos los bytes de CBike, dos CPed, modelInfo, datos de handling, matriz, cinco regiones globales contiguas y trazas. Restaura globals y hook.

Se aíslan por igual EXE/DLL FindPlayerPed, controles virtuales, horn virtual, stat modifier, ApplyTurnForce, PruneReferences, drive-by, resistencia al terreno, entrada de bomba, ActivateBomb, UpdateCarAI, ProcessControl del físico, rails/steering, transmisión y rotación de rueda. Se conservan operaciones matemáticas del objetivo. La aprobación de este banco no acredita por sí sola esos colaboradores ni la integración en una partida. Sólo root compila y ejecuta GTA de manera secuencial.

Coordinator result: 20261008-015031-936853 approved 1024 pairs. Final manifest repeat remains pending.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-015031-936853`, 1024 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-ai355/20261008-015031-936853/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-015031-936853`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-ai355/20261008-015031-936853/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
