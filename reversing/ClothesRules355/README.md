<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ClothesRules355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CClothesBuilder::PreprocessClothesDesc`, `CClothesBuilder::ReleaseGeometry`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clothes-rules355/20261008-073233-409056/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ClothesRules355

EXE/DLL paired differential bank. Canonical GTA SA EXE SHA256: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Reconstruction evidence is under `reversing/Pending355/combo`; the fixture and comparison contract are in `LiveComparison.hpp`.

- `CClothesBuilder::PreprocessClothesDesc`: 4096 pairs.
- `CClothesBuilder::ReleaseGeometry`: 1024 pairs.

Clothes descriptors contain ten model keys and eighteen texture keys; generated rule words exercise dependent texture/model updates and inactive rules. Compare full descriptor bytes, complete rule array, tag count and ordered setter/deletion callbacks. Require both active and inactive paths; save and restore rule globals.

Each pair restores the same fixture before original and candidate execution and verifies dispatch switching. Equality is exact; no tolerance accepts divergent results. Scoped collaborator patches and hook states are restored after execution. These finite synthetic fixtures do not prove every asset or live-game situation.

Historical run `20261007-214324-727580`: **passed**, 5120 pairs, all equal, state restored, saves unchanged and configurations restored, as recorded by `summary.json`. Evidence: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clothes-rules355/20261007-214324-727580`. Runtime log SHA256: `e78c1131ac2ce4669a2e133a0c83094437dca3914e306dc15a924ec3eb24c4eb`.

Final source/hash repeat is pending the coordinator's final build and registration; the historic approval does not certify later source or dependency changes. `build_probe.py` builds the instrument and `run_local.py` executes its isolated probe; the coordinator exclusively owns compilation and game execution.
