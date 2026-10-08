<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaneGeneration355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::DoPlaneGenerationAndRemoval`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-generation355/20261008-045741-690243/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-045741-690243, passed for the tested snapshot. Latest historical passing report: 20261008-045741-690243.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

DoPlaneGenerationAndRemoval 0x6CD2F0:1024 paired comparisons prepared; native source applied, pending runtime. REA canonical pseudo recovered by root plus full ASM6CD2F0..6CDC08. Gates frame/active/count/area/cutscene/zones; wanted pool traversal and Hydra quota; weather/gang/mission/riots and modular clock selection; model load requests; helicopter/plane creation, preserved creation heading/height, x87 rotation/sin/cos velocity, generated missions/speeds/gear/engine, random target selection, references and danger events. Symmetric fixtures patch 29 dependency pairs; allocator/CHeli constructor6C4190 and CPlane constructor6C8E20 are canonical EXE dependencies outside target6CD2F0. Those constructors are isolated stubs, not certified here. OOM/null allocation subroute excluded (EXE subsequently dereferences null). Full created vehicle/matrix bytes and exact traces; globals/timer/model load bytes restored. Callbacks mutate current model/time/position to check rereads and captured position pointer. Event payload records all fields explicitly, excluding vtable and uninitialized ABI padding; reference stack addresses normalized. Scoped hook false until runtime approval.

Runtime20261008-025122-180121:AV41BDD0 afterGear6CAC20. Canonical Gear preservesECX=this; caller immediately uses41BDD0 without resettingECX. Bank callback now uses named helper plus naked pushad/popad to preserve actual caller register contract. Repeat pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CPlane.DoPlaneGenerationAndRemoval_6CD2F0.c](../_handoff/ghidra/CPlane.DoPlaneGenerationAndRemoval_6CD2F0.c)
- [Plane_6CD2F0.txt](../VehicleAudio355Evidence/Plane_6CD2F0.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
