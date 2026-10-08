<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Expanded destructor contract bank

Prepared 2026-10-08. Compiled successfully in the shared frontend probe, not executed; no validation flags changed. The first bank needed a declaration-only MSVC compatibility repair before the successful frozen build. Its final source hash and this expanded bank's hash are captured in `../artifacts/manifest.json`.

Include `Expanded.hpp` and invoke `RequestedTaskLifetimeExpanded::Update()` on the game thread with `requested-task-expanded.start` beside the test EXE. Output is `requested-task-expanded.jsonl`. Root agent owns compilation, replay and promotion.

192 planned pairs: 64 GangJoinRespond destructor, 64 FollowLeaderInFormation destructor, 32 GoToAttractor destructor, 32 SmartFleePoint destructor. Sixteen poison seeds combine both absent/present subtasks, both absent/present animation references, and both absent/present leader identities as applicable.

## Collaborator contracts and trace evidence

- Non-null subtask is a concrete native `CTask` subclass with a real MSVC vtable, its own process-heap allocation and deleting destructor. Both EXE and candidate base destruction call that vtable. Destructor and deallocation are logged in order; the child verifies its parent identity. Parent subtask pointer must end null.
- Gang animation lookup is explicitly replaced at EXE `0x4D3990` and returns one of indexes 3, 17, 26. Name must be exactly `gangs`. Reference removal at EXE `0x4D3FD0` and **the native `CAnimManager::RemoveAnimBlockRef` entry** is patched, because the latter has no catalog hook and directly edits animation globals. Removal logs exact index and decrements an isolated counter. Expected trace is lookup, removal, child destruction/deallocation if present. No game animation counter is touched by the stub.
- Non-null leader is a sentinel identity never dereferenced. `CEntity::CleanUpOldReference` EXE entry `0x571A00` receives a declared fastcall adapter for its thiscall ABI. It checks this identity, pointer-to-reference at parent + `0x10`, and pointee identity. It records removal without nulling the caller's pointer, matching the reviewed collaborator interface. Real reference-list internals are outside this fixture; no game reference list is mutated.

All other reversible hooks remain original during each pair; only the target destructor is enabled for the candidate route. Every call logs actual selected-entry byte checks. Trace equality alone is insufficient: both traces must also equal the explicit expected sequence and all argument checks must pass.

## Vptr and restoration

Whole-object equality and payload equality are separate recorded results. Terminal EXE vptr must be `0x86D48C`, as confirmed in exact base-complex destructor assembly `0x61A3D0..0x61A424`. Terminal candidate vptr must equal the native `CTask` base table obtained from an explicitly base-destructed placement fixture. This certifies expected terminal representation identity, without invoking virtual methods on destroyed objects or asserting that all base-table implementations are equivalent.

Each ad hoc patch saves its original entry bytes and is destroyed before hook restoration. The bank independently compares 52 bytes at every collaborator-patched entry to the snapshot taken after global unhooking. Final checks compare every hook enabled/locked flag, 52 bytes of every simple/virtual EXE and candidate entry, and 11 slots of both target GTA/native vtables against initial snapshots.

## Limits

This expands to all explicit branches in the four reviewed destructors and null/non-null base-owned subtasks. It certifies a declared isolated collaborator contract if replay passes, **not** real animation manager, reference-list or GTA task-pool integration. Child type diversity, exceptions, reentrancy and allocator faults are not covered. Native terminal-vptr identity is recorded, not silently erased. A compiler may inline native collaborator calls: if that happens this bank must be revised or built so the explicit patch points are retained, and must not run with uncontained game-global mutation. Use the intended Debug `/Od` isolated build and inspect the candidate destructor assembly before runtime.

Do not promote flags from this preparation. Preserve any compile error, crash or trace discrepancy. Promotion requires actual frozen-source replay, canonical EXE identity, candidate DLL/source hashes, complete outputs and an explicit dependency review accepting this isolated scope.
