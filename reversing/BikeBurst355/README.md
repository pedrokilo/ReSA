<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeBurst355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::BurstTyre`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-burst355/20261008-065202-330197/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-052103-559174, passed for the tested snapshot. Latest historical passing report: 20261008-052103-559174.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

BurstTyre VMT 0x871360 slot 44, original 0x6BEB20: 1024 isolated EXE/DLL pairs prepared, runtime pending. Protection flags, raw component mapping/status, repeated bursts, status switch, physical forces, driver/rear impact ratios and speed NaN. Exact return, object/matrix hash and force/audio/RNG trace. Front-event branch after component 13 mapping is unreachable in EXE; component 14 raw status access retained. Evidence ../VehicleAudio355Evidence/Bike_6BEB20.txt (combo reports unavailable REA pseudo; full ASM present). Floating force arithmetic requires runtime comparison before approval.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [Bike_6BEB20.txt](../VehicleAudio355Evidence/Bike_6BEB20.txt)

- [Bike_BurstFull.txt](../VehicleAudio355Evidence/Bike_BurstFull.txt)

REA pseudocode recovery for CBike::BurstTyre failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
