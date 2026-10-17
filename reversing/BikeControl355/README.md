<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikeControl355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::ProcessControl`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-control355/20261008-065219-724488/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-023146-075310, passed for the tested snapshot. Latest historical passing report: 20261008-023146-075310.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

# CBike::ProcessControl

Native reconstruction at 0x6B9250, VMT 0x871360 slot10. Bank: 1,024 isolated
comparisons, awaiting compilation and runtime approval. Hook remains disabled
for normal startup. Full canonical ASM and recovered REA pseudocode are retained
in VehicleAudio355Evidence/Bike_ControlFull.txt and
Pending355/artifacts/vehicle-rea-recovery/CBike.Control_6B9250.c.

This large routine is transcribed as native DLL assembly to retain x87 stack,
rounding, integer flags, stack argument layout and exception-state updates.
The auditable generator draft_bike_control_asm.py retains 3,758 instructions,
relocates all 219 direct branch destinations, and verifies every destination
against a unique parsed PC. There are no indirect jump tables. All 52 external
direct callees are explicitly listed in BikeControl355Calls. No call or branch
enters the original 0x6B9250..0x6BC92C body.

relocate_bike_control_seh.py copies the seven-dword VC7 FuncInfo, its one unwind
entry, handler and fire-event destructor funclet into the DLL. Original handler
0x8482EB, metadata0x8A1068/map0x8A1060 and funclet0x8482E0 are unused. Only shared
frame dispatcher0x8215A0 and verified destructor0x4B1170 remain dependencies.
The current Windows build has GTASA_NO_EDIT_AND_CONTINUE=OFF, which already
links /SAFESEH:NO. The deliberate FS:0 registration suppresses C4733 locally.
This is a native implementation, not a forwarding EXE wrapper.

The bank substitutes 37 external collaborator entries and four virtual slots,
using explicit x86 argument/return contracts. Remaining canonical dependencies
are shared vector/matrix arithmetic and CRT x87 primitives. Objects, collision
lines/data/model, matrices, handling, RW frames, driver, animation, event group
and FX storage are isolated. Fixtures cover all principal statuses, player
driver transitions, special handling flags, four suspension contacts, wheel
states, velocity/steering, braking, grounded versus airborne ratios, sleeping
physics, low-health fire creation and wrapped control flow. Callbacks record
arguments and order and mutate forces, gear, spring limits and wheel state.

Selected low-health cases throw integer355 from event-group insertion after
the event lifetime begins. Both implementations must unwind its destructor
once and reach the same outer catch. Approval additionally requires positive
physics calls, wheel calls and successful exception/unwind cases; empty traces
or only early exits cannot pass the bank. Full object/matrix/handling/frame
bytes, global regions, exception result and ordered trace are compared.

Limits: isolated collaborators do not certify real audio, physics, collisions,
events, passengers or rendering. Fixtures use initialized finite values and
valid model/handling/collision storage. They do not cover invalid pointers,
signaling NaNs, arbitrary exception types, nested unwind failures, every
animation or all possible callback mutations. Exceptional semantics require
the configured x86 runtime and its existing SafeSEH setting. All overwritten
global regions and hook states are restored after completion.

The first runtime produced identical object/matrix/handling/global/wheel results,
842 physical calls, 1,292 wheel calls and 16 successful exception unwinds, but
564 trace differences exposed an invalid read in the bank of spring damping
output storage before the callee initialized it. EXE0x543D60 takes seven stack
arguments and returns ret0x1C; the final float reference is output-only. The
bank now records initialized inputs, writes this output, and compares its
subsequent use by damping and object state. Source ABI and seven arguments
remain intact. Repeat approval pending.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CBike.ProcessControl_6B9250.c](../_handoff/ghidra/CBike.ProcessControl_6B9250.c)
- [Bike_ControlFull.txt](../VehicleAudio355Evidence/Bike_ControlFull.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
