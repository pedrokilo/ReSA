<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeSetup355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::SetupSuspensionLines`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-setup355/20261008-065702-041609/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-052217-329103, failed or incomplete. No historical passing summary found.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

SetupSuspensionLines 0x6B89B0 VMT871360 slot48:1024 pares pendientes. ASM integral Bike_SetupFull; combo no pseudocodigo. Existing/sentinel lines, front/rear wheels and nested frame transforms, optional rear fork, cached origins/fork offsets/swing length, upper/lower/force levels, sphere radius/contact clamping and bounding updates. Full object/lines/spheres/box hashes and transform trace, modelslot400 restored. Fixtures finite nonzero force and valid four lines; max bounding corner6/8/0 gives exact radius10 to isolate suspension. Invalid frames/empty lines/NaN not covered. New hook false until runtime.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [Bike_SetupFull.txt](../VehicleAudio355Evidence/Bike_SetupFull.txt)

REA pseudocode recovery for CBike::SetupSuspensionLines failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
