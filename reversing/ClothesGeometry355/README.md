# ClothesGeometry355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CClothesBuilder::CopyGeometry`, `CClothesBuilder::BlendGeometry-2`, `CClothesBuilder::BlendGeometry-3`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clothes-geometry355/20261008-073210-737889/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ClothesGeometry355

EXE/DLL paired differential bank. Canonical GTA SA EXE SHA256: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Reconstruction evidence is under `reversing/Pending355/combo`; the fixture and comparison contract are in `LiveComparison.hpp`.

- `CClothesBuilder::CopyGeometry`: 1024 pairs.
- `CClothesBuilder::BlendGeometry-2`: 2048 pairs.
- `CClothesBuilder::BlendGeometry-3`: 2048 pairs.

Three mesh fixtures with sixteen vertices, normals, UVs, bone indices and weights. Vary vertex counts and binary blend ratios for copy and two/three-source blending. Compare complete mesh storage, returned destination and ordered callbacks; require nonempty geometry and weighted cases. Geometry enumeration, locking, skin access and reference changes are isolated equally.

Each pair restores the same fixture before original and candidate execution and verifies dispatch switching. Equality is exact; no tolerance accepts divergent results. Scoped collaborator patches and hook states are restored after execution. These finite synthetic fixtures do not prove every asset or live-game situation.

Historical run `20261007-222251-030863`: **passed**, 5120 pairs, all equal, state restored, saves unchanged and configurations restored, as recorded by `summary.json`. Evidence: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clothes-geometry355/20261007-222251-030863`. Runtime log SHA256: `a6ceb638bded596891046ef787a91e22693ef38bfc55fcad10846e2a330aea97`.

Final source/hash repeat is pending the coordinator's final build and registration; the historic approval does not certify later source or dependency changes. `build_probe.py` builds the instrument and `run_local.py` executes its isolated probe; the coordinator exclusively owns compilation and game execution.
