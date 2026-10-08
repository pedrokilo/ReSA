<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadioTrack355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAERadioTrackManager::TrackRadioStation`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-radio-track355/20261008-032100-192298/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-032100-192298, passed for the tested snapshot. Latest historical passing report: 20261008-032100-192298.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

TrackRadioStation 0x4EAC30: 1024 EXE/DLL pairs prepared, runtime pending. All stations, saved/invalid game dates, previous-month wrap, stale state, pause elapsed boundaries, skipping, all saved track types, missing user track, queue failures and probability order. Exact return, full object hash and dependency trace; frontend mode/pause clock/game date restored. Raw adjacent saved-state copies preserve EXE behavior. Evidence ../VehicleAudio355Evidence/Radio_4EAC30.txt.

Runtime repair: Corrected offset0x110 pause-clock read (declared m_iTimeInMs); fixtures initialize that exact field and preserve coverage of queue branches. Repeat approval pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAERadioTrackManager.TrackRadioStation_4EAC30.c](../_handoff/ghidra/CAERadioTrackManager.TrackRadioStation_4EAC30.c)
- [Radio_4EAC30.txt](../VehicleAudio355Evidence/Radio_4EAC30.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
