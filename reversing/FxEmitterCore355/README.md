<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FxEmitterCore355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `FxEmitterBP_c::CreateInstance`, `FxEmitterBP_c::Update`, `FxEmitterBP_c::FreePrtFromPrim`, `FxEmitterBP_c::Load`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fx-emitter-core355/20261008-073610-587736/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# FxEmitterCore355

EXE/DLL paired differential bank. Canonical GTA SA EXE SHA256: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Reconstruction evidence is under `reversing/Pending355/combo`; the fixture and comparison contract are in `LiveComparison.hpp`.

- `FxEmitterBP_c::CreateInstance`: 1024 pairs.
- `FxEmitterBP_c::Update`: 2048 pairs.
- `FxEmitterBP_c::FreePrtFromPrim`: 2048 pairs.
- `FxEmitterBP_c::Load`: 1024 pairs.

A 0x40-byte emitter fixture, allocation arena, eight linked particles, three systems and 4096-byte memory pool exercise allocation failure, particle removal and system state combinations. Load uses temporary files with versions 0/100/101/102/103/104/109/110, texture variants, blend IDs and LOD values. Compare returns, normalized pointers, arena/object/particle/system/pool bytes, names, stream position and ordered callbacks. Save and restore the FX memory pool. Virtual dispatch uses table 85A788 slots Load=1, CreateInstance=3, Update=4, FreePrtFromPrim=6. Historical approval includes corrected inherited ABI size 0x40.

Each pair restores the same fixture before original and candidate execution and verifies dispatch switching. Equality is exact; no tolerance accepts divergent results. Scoped collaborator patches and hook states are restored after execution. These finite synthetic fixtures do not prove every asset or live-game situation.

Historical run `20261007-220240-336659`: **passed**, 6144 pairs, all equal, state restored, saves unchanged and configurations restored, as recorded by `summary.json`. Evidence: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fx-emitter-core355/20261007-220240-336659`. Runtime log SHA256: `ac62f4b1f4dc7a3af19a287725721f060e5e7af3ca5837e6f8d971c201757559`.

Final source/hash repeat is pending the coordinator's final build and registration; the historic approval does not certify later source or dependency changes. `build_probe.py` builds the instrument and `run_local.py` executes its isolated probe; the coordinator exclusively owns compilation and game execution.
