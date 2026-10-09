<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrainAddPassenger355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTrain::AddNearbyPedAsRandomPassenger`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-train-addpassenger355/20261008-014535-893141/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-014535-893141, passed for the tested snapshot. Latest historical passing report: 20261008-014535-893141.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

AddNearbyPedAsRandomPassenger 0x6F8170: 1024 isolated EXE/DLL pairs prepared, runtime pending. Player/task gates, temporary passenger cleanup/removal, used/free ped slots, creation/group/type/state/task filters, planar distance/side selection, nearest door, inverse transform, direction captured before door mutation, allocation and sequence order. Full train/ped/intelligence/task hashes and exact dependency trace; ped pool restored. Matrix is valid and first two allocations succeed; third allocation may fail. Existing task ctor/sequence/SetTask EXE dependencies outside this scope are symmetrically isolated; no self-forward. Evidence ../VehicleAudio355Evidence/Train_6F8170.txt.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CTrain.AddNearbyPedAsRandomPassenger_6F8170.c](../_handoff/ghidra/CTrain.AddNearbyPedAsRandomPassenger_6F8170.c)
- [Train_6F8170.txt](../VehicleAudio355Evidence/Train_6F8170.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
