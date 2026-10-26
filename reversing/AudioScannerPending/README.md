<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# Scanner pendientes

Validación del banco aprobada el 2026-10-07: 4096 pares EXE/DLL, sin diferencias. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-scanner-pending/20261007-183230-914631/summary.json>); comparaciones: [comparisons.jsonl](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-scanner-pending/20261007-183230-914631/comparisons.jsonl>).

Cuatro funciones, 1024 pares cada una: AddAudioEvent 0x4E71E0, LoadSlots 0x4E6CD0, PlayLoadedDialogue 0x4E6F60, Service 0x4E7630. Vtable 0x85F368: una entrada, UpdateParameters 0x4E7590; coincide con count1.

Combo guardado en `evidence`. LoadSlots devuelve pseudocódigo de Service por agrupamiento Ghidra, por lo que sus límites exactos se verifican con ensamblador 0x4E6CD0..0x4E6DAB. Todos los rangos incluyen retorno o salto final.

AddAudioEvent reconstruye voz de crimen, zona y orientación, desplazamiento del jugador, clase de vehículo, remap/color, swim/jetpack. Las tablas de nombres/IDs originales se leen sin modificarlas. Se corrige State a int16: el EXE escribe word en 0xB61D4C, seguido por el bool de deshabilitación en 0xB61D4E. Se corrige además CanWePlayNewScannerDialogue invertido en fuente anterior; este banco no certifica esa dependencia. PlayLoadedDialogue ahora activa la implementación y restaura el plazo de abortar segunda sección. Service usa las constantes exactas y volumen click mutable del EXE.

Fixtures: entidad, ped, vehículo y zona falsos; globals de scanner, reloj, widescreen, radio y tabla de volúmenes guardados/restaurados. Intercepta hardware, streaming, RNG, zona, jugador, tareas, remap, creación de voces y frontend tanto en EXE como DLL. Service intercepta sus dependencias LoadSlots/PlayLoadedDialogue; AddAudioEvent intercepta PlayPoliceScannerDialogue. Hash FNV de todos los bytes de globals y objetos, más trazas completas. Exige casos con trabajo positivo por función. No reproduce audio real y no acredita integración de las dependencias interceptadas.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 4.096 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-audio-scanner-pending\20261007-195359-829903/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
