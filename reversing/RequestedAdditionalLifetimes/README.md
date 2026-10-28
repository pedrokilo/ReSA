<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Additional task lifetime differential bank

Prepared 2026-10-08. **Compiled successfully, not executed. No source correction, catalog or registry promotion made.** Frozen hashes are in `artifacts/manifest.json`; 20 frozen inputs, instrumented frontend/DLL sources and normal ASI/PDB restoration were checked. The candidate ASI exports all three vtables used by the bank.

Four exact pending identities: `CTaskSimpleFightingControl::Constructor` 0x61DC10, `Destructor` 0x61DCA0; `CTaskComplexProstituteSolicit::Constructor` 0x661A60, `Destructor` 0x661AF0. Full combo output is in `evidence/`.

## Two static discrepancies to reproduce before correcting

- FightingControl EXE constructor writes float 1.0 at +0x18 (`m_unk3`); current candidate initializes 0. EXE leaves byte +9 (`m_bool2`) untouched; candidate clears it. Source/header changes must follow preserved differential replay.
- ProstituteSolicit EXE constructor writes flags at +0x2C as `(old & 0xE2C0) | 0x02C0`. Candidate only sets b07/b08/b10 and does not clear the other bits cleared by the EXE. Poison patterns distinguish these paths.

## Concrete root-owned build/run

`build_probe.py` temporarily instruments `FrontendIdle`, captures runtime failures, compiles Debug with effective `_CL_=/MP6` and one outer build process, saves source/bank/DLL/PDB/EXE fingerprints, and restores original frontend/DLL sources and normal bin files. It does not install or launch the game. Run it only when other compiler/instrumentation work has finished.

`run_local.py` installs the frozen binary into the test copy through the guarded shared runner, disables automatic save loading and preserves success or failure evidence. It verifies the frozen inputs before runtime and saves linked enriched summaries. No game process is launched by importing either script.

672 planned comparisons: Fighting constructor 256 (128 seeds × null/non-null target), Soliciting constructor 128 (nonnull client contract), Fighting destructor 32 (16 seeds × null/non-null target), Soliciting destructor 256 (16 seeds × player present/absent × prostitute present/absent × camera bit clear/set × owned subtask present/absent).

## Isolation and evidence

Reference registration/cleanup are explicitly stubbed at 0x571B70 and 0x571A00, with exact entity identity, reference-field location and pointee checks. Native implementations redirect to these original entries because all other hooks remain disabled. The source must still retain those call boundaries in the isolated build; inspect optimized/inlined code before running any changed compiler configuration.

FindPlayerPed at 0x56E210 is an isolated stub returning a bounded ABI fixture or null and checking player index -1. There is no menu-wide player guard. The fixture owns full-sized zeroed player/data storage, supplies only CPed player-data pointer +0x480 and prostitute pointer +0xA4, and never invokes player constructors, virtual methods, engine pools or game actor services. This certifies the reviewed memory-layout contract, not real actor integration. The player-data byte output is compared across routes.

Subtasks are concrete native child objects with a real deleting destructor, process heap and traced parent/deallocation checks, reused from the frozen expanded bank. Original and candidate traces must equal an explicit expected sequence. Constructor rows preserve every payload byte and differing offsets, including padding/unwritten storage, rather than hiding them. Vptr is checked independently against exported native class tables and canonical GTA tables. Destructor terminal vptr is checked against exported native CTask base table and GTA 0x86D48C.

Restore checks independently compare all hook flags, 52-byte EXE/candidate entry snapshots, target native/GTA tables and all collaborator patch entries. Runtime faults and mismatches remain failures. Passing a later corrected replay warrants only the documented isolated contract scope plus an explicit dependency review, not blanket real-world subsystem equivalence.
