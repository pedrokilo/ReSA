<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainGeneration355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::DoTrainGenerationAndRemoval`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-generation355/20261008-024856-945375/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-024856-945375, passed for the tested snapshot. Latest historical passing report: 20261008-024856-945375.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

DoTrainGenerationAndRemoval 0x6F7900: 1024 isolated EXE/DLL pairs prepared, runtime pending. Tick gates, station proximity, used/free trains, carriage removal/player protection, disabled/status states, streaming loaded/missing, both tracks, bounded node search, tunnel conditions, generation and speed assignment. Full global/vehicle/created hashes and exact dependency trace. Track globals, pool, camera, station positions, clocks, region and streaming state restored. Fixtures ensure node search reaches far nodes and creation returns a valid train; original infinite/no-memory paths excluded. Evidence ../VehicleAudio355Evidence/Train_6F7900.txt.

Runtime repair: EXE filters subtype0x594==6, not type0x590. Source now uses IsSubTrain; isolated fixtures initialize both fields and include selected type/subtype mismatches. Waiting for repeat.

Runtime repeat approved: 20261008-024856-945375, 1,024 pairs. Final coordinator hash verification pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.DoTrainGenerationAndRemoval_6F7900.c](../_handoff/ghidra/CTrain.DoTrainGenerationAndRemoval_6F7900.c)
- [Train_6F7900.txt](../VehicleAudio355Evidence/Train_6F7900.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
