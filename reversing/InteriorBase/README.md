<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorBase

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-092631-221476`, 25600 pares. El alcance se limita a los fixtures y colaboradores documentados.

Checks list search, all tile states 0..12, invalid/edge coordinates, positive/zero/negative rectangles, forced/protected changes, window conversion, early whole-function return on status3, corner branches and strip scan directions. GetTileStatus compares the complete EAX int32 return, including upper bytes; CheckTilesEmpty retains its declared int8 contract. Full 0x794-byte object snapshots use lossless hexadecimal strings; result and hook side must match exactly. Object pointer fields are normalized to identical markers only after checking exact receiver binding to the original box buffer and expected list head; both sides must preserve those bindings. Positive width required for GetNumEmptyTiles, whose original never terminates for nonpositive widths. Tile dimensions restricted to 1..30. Fixtures use no real world entities or render events.

Native subordinate GetTileStatus and SetTilesStatus follow the same side as their caller; original and candidate retain the same independent memory initialization. All hook states restored. No hooks activated by this bank before execution.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-092631-221476`: 25,600 pairs across 6 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->
