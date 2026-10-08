<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaneFlying355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::ProcessFlyingCarStuff`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-flying355/20261008-045721-607107/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-045721-607107, passed for the tested snapshot. Latest historical passing report: 20261008-045721-607107.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

# CPlane::ProcessFlyingCarStuff

Native reconstruction at 0x6CB7C0, VMT 0x871948 slot 68. The bank prepares
1,024 isolated comparisons against the canonical EXE. Runtime approval is
pending; the hook remains disabled for normal startup.

Full assembly is in ../VehicleAudio355Evidence/Plane_FlyingFull.txt through
the return at 0x6CC489. REA pseudocode was recovered at
../Pending355/artifacts/missing-rea-recovery/CPlane.FlyingControl_6CB7C0.c.
The catalog identity is ProcessFlyingCarStuff; FlyingControl is the dependency
at 0x6D85F0, not this target.

Fixtures cover positive/zero/negative time steps, wrapping wave counters,
ordinary planes, RC Baron, Hydra, player/physics/remote and other statuses,
low health, each damaged component, absent nodes, matching panels, drowned
engines, positive/negative acceleration, propeller decay and ceiling calls.
Callbacks record exact argument bits and order, mutate position, time step,
node presence and the temporary flying-handling pointer. Full object, matrix
and frame bytes are compared after normalizing fixture pointers. Particle
multiplier bytes and both vectors are recorded; all seven emission random
draws, including the discarded draw, are observable in the ordered trace.

Damage queries, roll, height, handling lookup, flight physics, RNG and particle
emission are symmetric isolated dependencies. This bank does not certify their
implementations or actual flight/particle rendering. Matrix transformation at
0x59C890 is a shared canonical dependency with initialized matrices. Fixtures
use finite ordinary values, valid model/handling storage, and damage states
0..2. They do not certify signaling NaNs, invalid node/matrix pointers, runtime
model reloads or all possible external callback mutations. Camera placement,
smoke pointer, Hydra model pointer and time step are restored after execution.
An approval also requires positive flight and particle calls, not only empty
or early-return cases.

Runtime20261008-025056-205220:390 differences limited to multiplier bytes. Original RNG draw at6CBFF0 is normalized and stored at6CC003 stack+0x98, overwriting FxPrtMult life field (constructor base0x80+0x18). Native now stores exact x87 normalized draw in m_fLife; it is not discarded. Repeat pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CPlane.ProcessFlyingCarStuff_6CB7C0.c](../_handoff/ghidra/CPlane.ProcessFlyingCarStuff_6CB7C0.c)
- [Plane_FlyingFull.txt](../VehicleAudio355Evidence/Plane_FlyingFull.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
