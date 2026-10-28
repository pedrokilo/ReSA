<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainCreate355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::CreateMissionTrain`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-create355/20261008-024917-862972/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-024917-862972, passed for the tested snapshot. Latest historical passing report: 20261008-024917-862972.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

CreateMissionTrain 0x6F7550: 1024 EXE/DLL pairs prepared, runtime pending. Ten configs, closest/explicit node, tracks/directions, mission/random, first/last output alias, carriage spacing, virtual control, references, reverse world reinsert, driver creation and streak flags. Full objects/transforms/driver/intelligence hash plus exact dependency trace. Constructor resolves actual DLL body before symmetric patching, fails safely if unresolved. Track globals and five model slots restored. Valid allocation and mandatory first output fixtures; original OOM/null-output crashes excluded. Evidence ../VehicleAudio355Evidence/Train_6F7550.txt.

Runtime repair: temporarily unlock/enable the Constructor wrapper only while resolving its compiled native CTrain constructor body, then restore its prior hook/lock state before comparisons. The previous disabled wrapper led resolution back to the EXE and stopped the bank before cases. Waiting for repeat.

Runtime repeat approved: 20261008-024917-862972, 1,024 pairs. Final coordinator hash verification pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.CreateMissionTrain_6F7550.c](../_handoff/ghidra/CTrain.CreateMissionTrain_6F7550.c)
- [Train_6F7550.txt](../VehicleAudio355Evidence/Train_6F7550.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
