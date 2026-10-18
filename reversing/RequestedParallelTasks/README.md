<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Task lifetime pending bank

Prepared 2026-10-08. **Compiled successfully, not replayed. No validation flag changed.** The frozen frontend ASI/PDB and source hashes are in `artifacts/manifest.json`; normal ASI/PDB and instrumented sources were restored after compilation. Failed compile attempts remain in `artifacts/build-attempt1.log` and `build-attempt2.log`.

Seven exact catalog identities:

| Class | Function | EXE address |
|---|---|---|
| CTaskComplexGangJoinRespond | Constructor | 0x6616F0 |
| CTaskComplexGangJoinRespond | Destructor | 0x661720 |
| CTaskComplexGoToAttractor | Constructor | 0x66B640 |
| CTaskComplexGoToAttractor | Destructor | 0x66B6A0 |
| CTaskComplexSmartFleePoint | Constructor | 0x65BD20 |
| CTaskComplexSmartFleePoint | Destructor | 0x65BDB0 |
| CTaskComplexFollowLeaderInFormation | Destructor | 0x694A40 |

The evidence directory contains full combo pseudocode and assembly for all seven entries, obtained from the canonical EXE with `reversing/_handoff/combo.sh`. Fresh REA evidence is coordinated by the root agent; these files alone are not fresh REA captures.

## Prepared runtime contract

Include `TaskLifetimeComparison.hpp` in the isolated probe translation unit and invoke `RequestedTaskLifetime::Update()` once on the game thread. The test copy must contain `requested-task-lifetime.start`; output is `requested-task-lifetime.jsonl` beside its EXE. Compilation/runtime remain the root agent's responsibility.

128 poison patterns and input seeds per identity, 896 planned comparisons. Finite positions, boolean response, movement states walk/run, timer sentinel -1 and finite intervals. Constructor input storage is retained for byte comparison, including padding and untouched fields. All reversible hooks are temporarily disabled to prevent reconstructed collaborators from contaminating the EXE oracle; only the individual target is enabled for its candidate call.

The first pointer-sized field is **not silently normalized**. Each row records both vptr addresses and whole-object equality separately from payload equality. Constructor rows check EXE vptr against the hardcoded class table, candidate vptr against the table of a valid native library object, return-this, and GetTaskType/IsSimple/GetSubTask on both objects. Differing vtable slot indices are recorded. This does **not** certify all remaining virtual methods.

Destructor fixtures have null subtasks. Gang animation references are false; formation leader is null. They exercise destructor/base cleanup but do not cover deletion of owned subtasks, gang animation-reference cleanup, or leader reference cleanup. Destructor terminal vptr is recorded and expressly remains uncertified. Passing these rows cannot on its own justify full-function validation flags for destructors.

The bank compares pre/post enabled and locked flags for every hook, 52 bytes of original and candidate entries for every simple/virtual hook, and all 11 slots of both GTA and native tables for the four target classes. `state_restored` is calculated from these comparisons. No literal success substitutes for restoration evidence. Script-command memory and unrelated global data are not independently snapshotted; their flags are checked. No exceptions/SEH crash recovery is implemented: a faulted run must be preserved as failure and never promoted.

## Static review

- Gang constructor writes response and clears anim-reference flag, leaving attempts untouched; source agrees for canonical bool inputs. Arbitrary byte response is outside C++ bool contract.
- GoToAttractor constructor stores each field without using the attractor; source agrees. Destructor tail-jumps base cleanup.
- SmartFlee constructor leaves initial position untouched, sets movement to numeric 7, clears timer/flags, and samples clock only when duration is not -1; source agrees structurally. Replay must check poison padding and identical frame-clock value.
- FollowLeader destructor cleans a non-null leader reference then invokes complex-task base destruction. Current null-leader bank excludes the non-null branch.

Full certification requires additional collaborator fixtures and vtable equivalence review plus a frozen-source build, replay, source/EXE/DLL hashes, and trustworthy linked summaries.

## Replay status, 2026-10-08

The first frontend lifetime replay was attempted and did not complete: the game process exited with code 0 before the bank completed. [Preserved summary](runs/lifetime/20261008-114235-326299/enriched-summary.json). No lifetime identity was promoted from this attempt. The expanded bank remains compiled preparation unless a separate successful linked replay is recorded.
