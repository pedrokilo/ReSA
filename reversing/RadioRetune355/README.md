<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadioRetune355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAERadioTrackManager::CheckForStationRetune`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-radio-retune355/20261008-032150-220480/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-032150-220480, passed for the tested snapshot. Latest historical passing report: 20261008-032150-220480.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

CheckForStationRetune 0x4EB660: 1024 EXE/DLL pairs prepared, runtime pending. All modes/stations, mutable first/second vehicle options, cutscene/replay, script and pad requests, missing user tracks, pause-clock thresholds/wrap and camera NaN. Full object/options/pad hash and exact stop/start/frontend trace. Real pad snapshot restored; timer/replay/user count/camera restored. Evidence ../VehicleAudio355Evidence/Radio_4EB660.txt. Hook remains quarantined.

Runtime repair: Corrected unordered FCOM branch: NaN chooses delay2000 like AH bits0x41; reads exact threshold float0x858C20. Repeat approval pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAERadioTrackManager.CheckForStationRetune_4EB660.c](../_handoff/ghidra/CAERadioTrackManager.CheckForStationRetune_4EB660.c)
- [Radio_4EB660.txt](../VehicleAudio355Evidence/Radio_4EB660.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
