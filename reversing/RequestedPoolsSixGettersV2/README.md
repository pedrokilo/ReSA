<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 884 grupos / 10015 funciones reconstruidas; 10015 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedPoolsSixGettersV2

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **repetible solo con revision manual pendiente**; no aprobado en esta ronda.

Se repararon `VTableRanges.hpp` (17 clases `CEvent*`), hashes y `_CL_`. `run_local.py` exige `Diagnostics/borrowed-constructor-proof.json` ligada al ASI; su cadena generadora (`inspect_compiled.py`, `Diagnostics/inspect_constructor_graph.py`, `Diagnostics/prove_borrowed_constructor.py`) falla con el ASI de hoy (`Exception: 0x1a5f8864` al seguir el grafo de constructores: direccion fuera de las secciones leidas). Hace falta rehacer la revision del grafo. Evidencia vigente: `validation-summary.json` de octubre (3072 pares, 6 getters de `CPools`). Las pruebas de `Diagnostics/` quedaron intactas.
