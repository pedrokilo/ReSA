<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CPostEffects pendientes



Validación del banco aprobada el 2026-10-07: 2304 pares EXE/DLL, sin diferencias. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-post-effects-pending/20261007-193953-828846/summary.json>); comparaciones: [comparisons.jsonl](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-post-effects-pending/20261007-193953-828846/comparisons.jsonl>).



Tres funciones reconstruidas desde combo REA completo en `combo`: HeatHazeFX 0x701780, Radiosity 0x702080 y SpeedFX 0x7030A0. La reconstrucción conserva las escrituras parciales y peculiares del EXE en vértices (incluido el recipCameraZ de HeatHaze de máscara), los tipos enteros de resolución de radiosidad y de la tabla de velocidad, orden de estados, UV y RNG.



Banco de 2304 pares: 256 HeatHaze, 1024 Radiosity, 1024 SpeedFX. Cámara, rasters y dispositivo ficticios; todos los accesos de render, cámara, partículas, modos y RNG sustituidos EXE+DLL. Sin dibujo sobre el dispositivo real. Compara hashes completos de doce regiones de estado, vértices/índices, cámara/dispositivo/raster y trazas exactas de estados y primitivas. Puertas de hook y actividad positiva. Conserva y restaura todas las regiones y hooks. Casos cubren máscara/no máscara, profundidad16/32, intensidad y resolución, clipping y reinicio de los180puntos, filtro/render0..4 y modos de cámara0..3, umbrales de velocidad y jitter.



Pendiente de compilación y ejecución del agente principal. No aprobado sin resultados runtime.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.304 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-post-effects-pending\20261007-200758-885598/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
