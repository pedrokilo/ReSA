<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BikePreRender355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CBike::PreRender`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-pre-render355/20261008-065420-101227/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CBike::PreRender — prepared, awaiting EXE validation

Original `0x6BD090`; canonical vtable `0x871360`, slot **17** (verified directly against the canonical executable). Native source reconstruction covers suspension interpolation, both wheel particle dispatches, exhaust/damage conditions, HPV1000 police corona/light phases, vehicle lights/shadow, wheel rotation integration and the fork/wheel/mudguard/chassis frame transforms. Hook remains disabled until root approval.

Evidence: `reversing/VehicleAudio355Evidence/Bike_PreRenderFull.txt` contains the complete 965-line assembly, through `0x6BDE18`. Recovered REA pseudocode: `reversing/Pending355/artifacts/vehicle-rea-recovery/CBike.PreRender_6BD090.c`. Canonical executable SHA256: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Bank `BikePreRender355` provides 1024 contiguous original/candidate pairs. It compares the complete bike fixture (constant fixture pointers normalized), all ten complete RwFrames, the vehicle matrix, touched globals, and ordered collaborator traces with argument float bits. Fixtures vary status, suspension, contact/history ratios, wheel states and damaged tires, bloody/skid states, optional nodes, steering/lean, movement, handling flags, police phases and timestamps. Required positive collaborator gates remain enabled.

Isolated identically for both paths: base CVehicle PreRender, col-model lookup, virtual suspension update, lean-matrix update, wheel/exhaust/damage particles, lights, shadow, corona, point light and wheel rotation. Their output fixtures and semantic arguments are compared; their own rendering/particle implementations are outside this bank. Matrix, vector, quaternion and frame calculations execute normally. The bank keeps mandatory front/rear wheel and mudguard nodes valid because the EXE unconditionally dereferences them. No actual render submission is claimed.

Root alone compiles and runs GTA. Source and bank are frozen after preparation; this README does not claim runtime approval.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-024735-723122`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bike-pre-render355/20261008-024735-723122/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
