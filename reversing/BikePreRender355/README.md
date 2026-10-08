<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
