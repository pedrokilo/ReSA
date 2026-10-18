<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ClothesSkin355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CClothesBuilder::ConstructGeometryAndSkinArrays`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clothes-skin355/20261008-073316-942555/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ClothesSkin355

EXE/DLL paired differential bank. Canonical GTA SA EXE SHA256: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Reconstruction evidence is under `reversing/Pending355/combo`; the fixture and comparison contract are in `LiveComparison.hpp`.

- `CClothesBuilder::ConstructGeometryAndSkinArrays`: 2048 pairs.

Eleven mesh fixtures provide up to 128 vertices, normals, UVs, triangles, bone indices and weights. Ten input slots vary presence and count 0–10. Dedicated allocation arenas capture weights/bones, allocation sizes and bounded callback records. Compare all mesh and arena bytes, outputs, vertex count and trace; overflow fails. Require populated and empty geometry. RW creation/material/skin access and allocation are isolated.

Each pair restores the same fixture before original and candidate execution and verifies dispatch switching. Equality is exact; no tolerance accepts divergent results. Scoped collaborator patches and hook states are restored after execution. These finite synthetic fixtures do not prove every asset or live-game situation.

Historical run `20261007-224858-092204`: **passed**, 2048 pairs, all equal, state restored, saves unchanged and configurations restored, as recorded by `summary.json`. Evidence: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-clothes-skin355/20261007-224858-092204`. Runtime log SHA256: `8700708975cc0f43454afce51146bcc0be50e09965ee4d15fe2ea7230cfb8acd`.

Final source/hash repeat is pending the coordinator's final build and registration; the historic approval does not certify later source or dependency changes. `build_probe.py` builds the instrument and `run_local.py` executes its isolated probe; the coordinator exclusively owns compilation and game execution.
