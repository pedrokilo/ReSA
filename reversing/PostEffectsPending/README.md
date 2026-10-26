<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
