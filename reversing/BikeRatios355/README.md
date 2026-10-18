<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeRatios355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::DoBurstAndSoftGroundRatios`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-ratios355/20261008-065946-351248/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-044651-461814, passed for the tested snapshot. Latest historical passing report: 20261008-044651-461814.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

DoBurstAndSoftGroundRatios 0x6B6950 VMT871360 slot52:1024 pares pendientes. Wheelstatus0-3, ratios-.25..1.25/NaN, rail178/sand/other, front/rear sizes, RNG deterministic trace, phase crossing both directions, handling flags100000/200000 priority, model432 exclusion, timestep/wet0..1. Modelslots400/432, timestep, wet restored. Floor CRT pure; speed conversion fixtures finite. Cuerpo nativo aplicado hook reversed=false hasta aprobacion. Evidencia Bike_RatiosFull.txt y Bike_RatiosNative.txt.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [Bike_RatiosFull.txt](../VehicleAudio355Evidence/Bike_RatiosFull.txt)
- [Bike_RatiosNative.txt](../VehicleAudio355Evidence/Bike_RatiosNative.txt)

REA pseudocode recovery for CBike::DoBurstAndSoftGroundRatios failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
