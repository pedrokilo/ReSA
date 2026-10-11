<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8547 funciones reconstruidas; 8547 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9226 entradas binarias sin nombre catalogado; 822 destinos originales explícitos fuera del catálogo, 398 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 102 aprobadas dentro del juego; 133 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorBase

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-092631-221476`, 25600 pares. El alcance se limita a los fixtures y colaboradores documentados.

Checks list search, all tile states 0..12, invalid/edge coordinates, positive/zero/negative rectangles, forced/protected changes, window conversion, early whole-function return on status3, corner branches and strip scan directions. GetTileStatus compares the complete EAX int32 return, including upper bytes; CheckTilesEmpty retains its declared int8 contract. Full 0x794-byte object snapshots use lossless hexadecimal strings; result and hook side must match exactly. Object pointer fields are normalized to identical markers only after checking exact receiver binding to the original box buffer and expected list head; both sides must preserve those bindings. Positive width required for GetNumEmptyTiles, whose original never terminates for nonpositive widths. Tile dimensions restricted to 1..30. Fixtures use no real world entities or render events.

Native subordinate GetTileStatus and SetTilesStatus follow the same side as their caller; original and candidate retain the same independent memory initialization. All hook states restored. No hooks activated by this bank before execution.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-092631-221476`: 25,600 pairs across 6 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->
