# RequestedPoolsSixGettersV2

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **repetible solo con revision manual pendiente**; no aprobado en esta ronda.

Se repararon `VTableRanges.hpp` (17 clases `CEvent*`), hashes y `_CL_`. `run_local.py` exige `Diagnostics/borrowed-constructor-proof.json` ligada al ASI; su cadena generadora (`inspect_compiled.py`, `Diagnostics/inspect_constructor_graph.py`, `Diagnostics/prove_borrowed_constructor.py`) falla con el ASI de hoy (`Exception: 0x1a5f8864` al seguir el grafo de constructores: direccion fuera de las secciones leidas). Hace falta rehacer la revision del grafo. Evidencia vigente: `validation-summary.json` de octubre (3072 pares, 6 getters de `CPools`). Las pruebas de `Diagnostics/` quedaron intactas.
