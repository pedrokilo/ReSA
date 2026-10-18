<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaneDamage355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::VehicleDamage`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-damage355/20261008-045800-253106/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-045800-253106, passed for the tested snapshot. Latest historical passing report: 20261008-045800-253106.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

VehicleDamage 0x6CC4B0 VMT 0x871948 slot56: 1024 comparison pairs prepared, pending runtime. Paired dependency calls exercise gates, automobile delegation, player queries with mutable health/damager, collision damage, blow threshold, node selection, RNG, progression/status reread, 4 raw ABI panel records, visibility, particles with node mutation, old effect kill, fresh smoke position, matrix/factory null, Play with changed current effect/speed, velocity/local/random timer. Full object, frame, matrix and effect bytes plus exact ordered traces. Quarter coordinates plus 256 nonaligned/nonquarter 3D node configurations exercise the original unrounded x87 square-distance comparison and square root (EXE406DA0 / 6CC819..6CC839), reconstructed by PlaneSelectCloser355.

LIMITED DEVIATION: accepted explosive-to-smoke EXE reads uninitialized [esp+14h] (fld 6CC915, fmul 6CC919, store 6CC923); native uses defined +0 force. This subroute is excluded from deterministic comparisons. Accepted explosive-to-BlowUpCar and collision-to-smoke remain exercised. No full equivalence claim for explosive-to-smoke. See ../VehicleAudio355Evidence/plane-damage-validationlimitations.json and source comment. Scoped hook false pending approval.

64 targeted cases i%16==4 use identical off-axis vectors (0.0031638420186936855,0.4666784703731537,0.702342689037323), all13 nodes present with safe collision gates. Exact squared distance ~0.7110840574503235, rounded closest0.8432580232620239 squared ~0.7110840937957761; both rounded squares equal float0.7110840678215027. EXE therefore repeatedly selects later nodes while the former rounded implementation would retain node12. Ordered progress trace captures selected node24. Other nonaligned fixtures remain included.

Applied threshold correction: pushEDI6CC51A moves earlier stack locals; subsequent part threshold is500*max(mass/10000,1), destroy threshold5000*(mass/10000). Targeted64 nearest fixtures now use collision600/health1000/mass2000 to exercise part damage without blow. No explosive-to-smoke equivalence claim. Native source corrected after coordinator compile freeze release; repeat pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CPlane.VehicleDamage_6CC4B0.c](../_handoff/ghidra/CPlane.VehicleDamage_6CC4B0.c)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
