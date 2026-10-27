<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeDamage355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::VehicleDamage`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-damage355/20261008-065302-845061/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-044551-700442, passed for the tested snapshot. Latest historical passing report: 20261008-044551-700442.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

VehicleDamage 0x6B8EC0 VMT871360 slot56:1024 pares pendientes. ASM completo Bike_DamageFull. Input positive/0/negative, stored collision thresholds, progress0/50/100, local damage before mutable progress, gate damage/invulnerability, building/vehicle/ped and directional impact, statuses, driver speech, repeated player queries, protected health factors/clamp, fire setup/references. Paired Knock/Progress/PlayerVehicle/PlayerPed/Reference/Wanted/Say, player query/speech mutate health to check timing. Whole bike hash and dependency trace. Finite moderate damage/speed fixtures; NaN/overflow not certified. No globals changed, hooksfalse until validation.

Strict semantic pointer outputs added before byte-hash cleanup: NULL, fixture region+byte offset, or unknown_exact+full uintptr value. Unknown values never collapse into a shared sentinel. Current matrix/driver/physical damager/last damage entity/vehicle damager are compared; handling identity is also compared. The existing full remaining-object byte hash and ordered collaborator traces remain mandatory. Raw fixture pointers are then cleared only after semantic identity capture. No production source change.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [Bike_DamageFull.txt](../VehicleAudio355Evidence/Bike_DamageFull.txt)

REA pseudocode recovery for CBike::VehicleDamage failed in both recorded attempts; full native ASM remains available. [Recovery diagnostics](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index.json), [first attempt](../VehicleAudio355Evidence/artifacts/link-pseudo-recovery/index-first-failure.json). This entry is not claimed as complete REA pseudocode + ASM.

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
