<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Interior_c canonical evidence

The 52 cataloged methods and two uncataloged ABI entries have complete exact canonical EXE disassembly in `Interior_<function>.txt`. Canonical EXE SHA256: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

REA/Ghidra pseudocode accompanies the evidence. The cached `Shop_FurnishCeiling` pseudo resolves to `FurnishOffice` (0x599AF0), so it is collaborator/alias evidence, not an exact decompilation of 0x59A130. The exact ASM at 0x59A130 is `ret`, which establishes the ceiling method's no-op contract. `Furnish` pseudo follows tail jumps into furnishing collaborators; only its own 0x591590..0x5915C8 ASM establishes its dispatch contract. Constructor, destructor and Furnish pseudo were recovered in a private REA session; the global cache/config was not modified.

Base proposal: `artifacts/base-draft.json`, 23 cataloged C++ bodies and two uncataloged ABI wrappers. Preserve the original return contracts: GetTileStatus and Init return complete int32 EAX; GetRandomTile returns the selected Y coordinate in EAX; GetTileCentre returns the pointer returned by RwV3dTransformPoints. Exit reads matrix position at 0x48; IsVisible's entrance-distance branch reads m_position at 0x400. Native Constructor preserves NodeId bytes 0x3F2..0x3F3; ordinary default C++ construction additionally initializes that member. Bonus ABI wrappers do not add catalog rows.

Eight comparison banks: InteriorBase (6), InteriorSpatial (7), InteriorLifecycleCore (6), InteriorLifecycleRuntime (4), InteriorPlacement (4), InteriorHome (9), InteriorCommerce (16), and InteriorAbi (2 bonus entries). The first seven cover all 52 cataloged methods exactly once. Runtime approval is bound to each bank's actual report, source/instrument hashes and archived binary; prepared drafts and pseudocode are not runtime validation. Symmetric collaborator fixtures do not validate collaborator internals or the full game.

Static source/ABI audit: `audit_source.py` writes `source-audit.json`. Final publication uses `close_validation.py` (read-only preflight; `--apply` after all eight current reports pass). `scope.json` preserves the exact 52 cataloged entries and two bonus ABI addresses. Initial integration proposals remain local under ignored `artifacts/`; the reconstructed production source is authoritative.
