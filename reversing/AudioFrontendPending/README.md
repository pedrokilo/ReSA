<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CAEFrontendAudioEntity pendientes

Validación del banco aprobada el 2026-10-07: 3072 pares EXE/DLL, sin diferencias. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-frontend-pending/20261007-182922-625332/summary.json>); comparaciones: [comparisons.jsonl](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-frontend-pending/20261007-182922-625332/comparisons.jsonl>).

| Función | EXE | Casos |
|---|---|---:|
| AddAudioEvent | 0x4DD4A0 | 2048 |
| UpdateParameters | 0x4DEDA0; vtable 0x862E54, slot 0 | 1024 |

`evidence/*.combo.txt` combina pseudocódigo REA y ensamblador. Ghidra cubre AddAudioEvent; UpdateParameters solo tiene ensamblador completo hasta RET 8 en 0x4DEF2D. La vtable contiene una entrada y coincide con el registro.

AddAudioEvent reconstruye todos los casos del EXE, incluidos los ausentes en la fuente anterior: retune, cancelaciones, loading stop y sonidos de misión. Corrige bank slot de carga, IDs de click, velocidad alterna y etiquetado de eventos. Usa CAESound temporal reutilizado como el EXE.

UpdateParameters multiplica por el float exacto 0x3B3B3EE7, duplica el producto y escribe la trayectoria en Z; la fuente previa dividía y escribía Y.

Banco: objetos falsos, tabla de volúmenes falsa, reloj restaurado; dependencias de hardware, peticiones y consultas de voces, RNG, callbacks de sonido y twin loops interceptadas tanto en EXE como DLL. Compara hashes FNV de todos los bytes de entidad y sonido, más trazas completas de argumentos y peticiones. Los fixtures de AddAudioEvent recorren 0..101, ambos estados de pausa, bancos disponibles/no disponibles, límites de frame, rotación de bancos, voces existentes y los twin loops. UpdateParameters cubre los límites -1, 0, 1, 349, 350, 351 y posiciones interiores. Gate positivo exige peticiones, carga de bancos y twin loops; para UpdateParameters exige cambios de posición y paradas. Sin reproducción de audio real.

La igualdad del banco acredita solo el comportamiento con las dependencias controladas; no acredita reproducción real ni regresión completa de audio.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 3.072 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-audio-frontend-pending\20261007-195246-913373/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
