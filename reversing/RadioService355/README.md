<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadioService355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAERadioTrackManager::Service`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-radio-service355/20261008-032211-414156/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-032211-414156, passed for the tested snapshot. Latest historical passing report: 20261008-032211-414156.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

Service 0x4EB9A0: 1024 isolated EXE/DLL pairs prepared, runtime pending. Eight modes, hardware sentinel play times, all stations/types, initialized pending settings, history selection, mutable dependencies and skip with zero/two recursive service iterations. Full object/options/pad/timer hash and exact dependency trace. Timer span, previous-play-time global and pad restored. Dependencies isolated symmetrically; hardware/audio services are traced fixtures, not full integration certification. Evidence ../VehicleAudio355Evidence/Radio_4EB9A0.txt. Hook remains quarantined.

Runtime repair: Corrected listening timestamp store to offset0x10C and full int32 history collaborator ABI; identifier555 must remain555 rather than int8 43. Repeat approval pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CAERadioTrackManager.Service_4EB9A0.c](../_handoff/ghidra/CAERadioTrackManager.Service_4EB9A0.c)
- [Radio_4EB9A0.txt](../VehicleAudio355Evidence/Radio_4EB9A0.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
