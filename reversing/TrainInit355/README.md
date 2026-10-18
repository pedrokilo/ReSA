<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainInit355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::InitTrains`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-init355/20261008-011852-026176/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-011852-026176, passed for the tested snapshot. Latest historical passing report: 20261008-011852-026176.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

InitTrains0x6F7440:1024 pares. Todas16 combinaciones tracks presentes/ausentes, filename y punteros salida por track, nodos0..7, retornoClosest track0..3 deliberadamente ignorado EXE, node0capturado antesloop6station. Read/Closest simetricos, Closest modifica distancia antes de lectura. Snapshot/restauracion GLOBAL CONTIGUO C37FEC..C3804C incluye totalLength/generation/count/pointers/stationDist, nodosaislados. Sin files/pool real. Nativa aplicada hookfalse pendiente ejecucion.

## Resultado de ejecución

El coordinador confirmó aprobación contra el EXE el 2026-10-08: 1,024 pares, sin diferencias. Identificador y enlace del informe final pendientes de incorporación por el coordinador. Esta aprobación no amplía las ramas ni dependencias descritas arriba.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.InitTrains_6F7440.c](../_handoff/ghidra/CTrain.InitTrains_6F7440.c)
- [Train_6F7440.txt](../VehicleAudio355Evidence/Train_6F7440.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
