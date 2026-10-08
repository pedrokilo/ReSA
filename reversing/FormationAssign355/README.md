<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FormationAssign355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CFormation::DistributeDestinations`, `CFormation::DistributeDestinations_CoverPoints`, `CFormation::DistributeDestinations_PedsToAttack`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-formation-assign355/20261008-073434-929286/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# FormationAssign355

3072 contiguous EXE/candidate pairs: DistributeDestinations (`0x69B240`), DistributeDestinations_CoverPoints (`0x69B5B0`) and DistributeDestinations_PedsToAttack (`0x69B700`), 1024 each. Canonical pseudo and complete assembly are under `reversing/Pending355/combo/CFormation/`.

Fixtures vary zero through seven participating peds, one through seven targets, eight through24 destinations, direct placement versus entity matrices, nontrivial positions and claimed destinations. The bank compares the full formation global region `0xC1A2C0..0xC1A554`, hook switches, and every ordered chooser argument as exact float bits. It restores the entire region and original hook state. Each function requires both896 positive and128 zero-ped cases.

The separate executable chooser (`0x69B1B0`) is isolated with the same deterministic cost/claim fixture for both paths. Its arguments and resulting assignment state are compared; its own distance/random selection implementation is outside this bank. The three scoped distribution bodies and centroid/radius/scaling arithmetic execute normally.

Report `20261008-012842-557727`: CoverPoints and PedsToAttack each1024 equal; DistributeDestinations401 differences, limited to one-ULP Y chooser arguments. The subsequent centroid correction preserves the EXE's resident x87 sums/inverse and prescribed float spills: destination Y/Z sums spill before division, ped Z spills, and ped Y/Z use the spilled inverse. Existing resident mean-radius, radius-scale and position-scale helpers are preserved. Revised validation is pending; no full-bank approval is claimed yet.

Latest coordinator run `20261008-014849-773744` approved 3072 pairs after centroid correction; all comparisons equal. Final activation/source-manifest repeat remains coordinator-owned.
