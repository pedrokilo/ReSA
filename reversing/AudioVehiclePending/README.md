<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CAEVehicleAudioEntity — cuatro pendientes

Validación del banco aprobada el 2026-10-07: 4096 pares EXE/DLL, sin diferencias. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-vehicle-pending/20261007-190212-323293/summary.json>); comparaciones: [comparisons.jsonl](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-vehicle-pending/20261007-190212-323293/comparisons.jsonl>).

Reconstrucción con combo REA (pseudocódigo y ensamblador completo) de ProcessDummyBicycle 0x4FFDC0, ProcessPlayerBicycle 0x500040, ProcessGenericJet 0x4FF900 y ProcessDummyHovercraft 0x500F50. Evidencia en `evidence/*.combo.txt`. Las cuatro son métodos no virtuales; la tabla de la clase conserva una entrada, UpdateParameters.

Banco de 4096 pares (1024 por función), con objetos, vehículos, matrices, transmisión y globals controlados. Hash FNV de todos los bytes y trazas exactas de argumentos. Alterna hook y exige salto EXE/DLL correcto, actividad positiva por función y restauración completa de hooks, globals de audio y posición de cámara. Stubs emparejados EXE/DLL de carga de bancos, slot, cancelación y generación de sonidos, controles, normalización, volumen de rueda y logaritmo; no se reproduce audio real.

Casos incluyen banco ausente, slots disponibles/agotados, banco asignado, modelos jet compatibles/desconocidos, motor y vehículo destruido, pedaleo/transición, marcha en ambos sentidos, rotación, suavizado inicial/incremental, freno y acelerador. Hovercraft conserva el offset por defecto derivado del ratio y la interpretación de su low word que muestra el EXE; PlayerBicycle usa el banco/slot dummy del EXE.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 4.096 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-audio-vehicle-pending\20261007-195528-197908/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
