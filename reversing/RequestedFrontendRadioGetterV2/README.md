# RequestedFrontendRadioGetterV2

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **repetible solo con revision manual pendiente**; no aprobado en esta ronda.

Se repararon `VTableRanges.hpp` (17 clases `CEvent*`), hashes del congelado y `_CL_`. Pero `run_local.py` exige `Diagnostics/constructor-proof.json` ligada al hash del ASI compilado, y su guion generador (`Diagnostics/prove_constructor.py`) comprueba bytes en direcciones absolutas del ASI de octubre (`0x1122C61C`, `0x1121E9E7`, `0x1121A477`): con el ASI de hoy falla en la primera aserción (`AssertionError`). Regenerarla exige repetir la revision manual del grafo del constructor sobre el ASI nuevo (no se fuerza ni se copia la prueba antigua). Evidencia vigente: `validation-summary.json` de octubre (2048 pares). Las pruebas de `Diagnostics/` quedaron intactas (se revirtio la sobrescritura del intento).
