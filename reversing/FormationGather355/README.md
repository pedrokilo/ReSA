<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FormationGather355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CFormation::GenerateGatherDestinations_AroundCar`, `CFormation::FindCoverPoints`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-formation-gather355/20261008-073531-980231/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# FormationGather355

2048 contiguous EXE/candidate pairs: GenerateGatherDestinations_AroundCar (`0x69A770`) and FindCoverPoints (`0x69B860`), 1024 each. Canonical evidence is under `reversing/Pending355/combo/CFormation/`.

The bank compares every generated point as float bits, the complete contiguous formation global region `0xC1A2C0..0xC1A554`, complete fake vehicle/object pools and all fixture matrices. It restores formation state, real pool pointers and the substituted model slot. Fixtures vary gathering counts (including zero and the 24-point capacity), bounding boxes, normalized vehicle axes, nearby objects/vehicles, occupied/free pool slots, drivers, motion and object orientation. Both positive and absent-result gates remain required.

Col-model lookup, object cover eligibility and separate per-object cover-box generation are isolated identically for both paths. Their output fixtures and traces are compared; these collaborator implementations are outside this bank. Matrix/vector arithmetic runs normally.

Report `20261008-012902-092243`: approved2048 pairs, zero differences; AroundCar990 positive/34 negative, FindCover888 positive/136 negative. Root owns final hook activation and source-hash repeats. This report predates the later DistributeDestinations centroid helper change in the same Formation.cpp file, so final source verification must repeat the bank.

Latest coordinator run `20261008-014909-150577` approved 2048 pairs after centroid correction; all comparisons equal. Final activation/source-manifest repeat remains coordinator-owned.
