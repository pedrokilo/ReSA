<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 902 grupos / 10558 funciones reconstruidas; 10558 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedFrontendRadioGetterV2

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **repetible solo con revision manual pendiente**; no aprobado en esta ronda.

Se repararon `VTableRanges.hpp` (17 clases `CEvent*`), hashes del congelado y `_CL_`. Pero `run_local.py` exige `Diagnostics/constructor-proof.json` ligada al hash del ASI compilado, y su guion generador (`Diagnostics/prove_constructor.py`) comprueba bytes en direcciones absolutas del ASI de octubre (`0x1122C61C`, `0x1121E9E7`, `0x1121A477`): con el ASI de hoy falla en la primera aserción (`AssertionError`). Regenerarla exige repetir la revision manual del grafo del constructor sobre el ASI nuevo (no se fuerza ni se copia la prueba antigua). Evidencia vigente: `validation-summary.json` de octubre (2048 pares). Las pruebas de `Diagnostics/` quedaron intactas (se revirtio la sobrescritura del intento).
