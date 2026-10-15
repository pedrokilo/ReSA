<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# Streaming pendientes

Validación del banco aprobada el 2026-10-07: 2560 pares EXE/DLL, sin diferencias. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-streaming-pending/20261007-191140-299188/summary.json>); comparaciones: [comparisons.jsonl](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-streaming-pending/20261007-191140-299188/comparisons.jsonl>).

Constructor512, PrepareStream1024, Service1024: total2560.

Combo completo en evidence; Service sin pseudo disponible, transcrito del ensamblador0x4F2550..0x4F27ED. Constructor revisado contra base0x4D7890: conserva bytes no escritos. Vtable0x85F3F0:9slots, destructor0, Service1, Play5.

PrepareStream devuelve int32 como EXE0/-1, tercer argumento bool de detener reproducción. Separa buffer bloqueado del decoder, evita dereferencia de bytesPCM como decoder, devuelve estado -2 solo cuando el estado no es -3/-1/-7 y fuerza bit0 en ambas muestras16bits de cada dword. Service reconstruye máquina de estados, cursor, escritura/relleno de mitades, parada y cambio de decoder. Lock usa último slot por 0x60000, como lea slot*3 y shl 17 del EXE.

Fixtures usan objetosCOM/decoder falsos y buffers en memoria; ninguna llamadaDirectSound real. DependenciasFillBuffer/fade/frequency/status/Stop/Prepare y virtualPlay controladas y trazadas. Constructor compara todos los bytes salvo vptr normalizado a tablaEXE. Compara todos los bytes de canal, buffer destino y fuente porFNV, más trazas completas. Gate positivo por función. Restituye hooks y patches; no toca singletons de hardware.

No acredita transmisión real, decoder real ni integración de dependencias interceptadas.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.560 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-audio-streaming-pending\20261007-195440-816486/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
