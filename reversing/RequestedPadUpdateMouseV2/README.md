<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10344 funciones reconstruidas; 10344 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedPadUpdateMouseV2: NO repetible, sustituido por V3 (CPad::UpdateMouse 0x53F3C0)

Estado: **rechazado y congelado el 08/10/2026; no repetible y no tiene sentido repararlo**: su fallo se debia a dos defectos del arnes (`LiveComparison.hpp`) que corrige [`RequestedPadUpdateMouseV3`](../RequestedPadUpdateMouseV3/README.md), el mismo paquete con otro nombre y las correcciones, que **si es repetible y esta aprobado** (9.216 pares, cero diferencias, 10/11/2026).

## Prueba

- Su unica corrida, `runs/20261008-232556-013193`, termino en FALLO con **0 comparaciones**: `errors = ['helper unresolved', 'missing/excess corpus', 'terminal']` y `helpers_known = false` en el registro `begin` (ver `runs/*/enriched-summary.json`). No hubo una sola comparacion EXE/DLL, de modo que el banco no demostro nada sobre la funcion.
- El README de V3 explica el motivo: V2 desenganchaba `CheckForInput` antes de calcular su entrada y su cuerpo (el hook reescribe el salto de la DLL a 0x53F270, de modo que el cuerpo "propio" acababa en el nativo, `pureCandidateKnown` era falso y el bucle de 9.216 casos no corria) y capturaba los 128 bytes de 0x53F270 con el hook ya quitado, con lo que la restauracion final daba un falso negativo. V3 corrige ambos defectos y ademas pasa de buscar los colaboradores por nombre de hook a parchearlos por direccion (V3.1).
- Repetir V2 reproduciria el fallo documentado (defecto del arnes) o, si se le aplicaran las correcciones, seria V3. Con la copia actual del repositorio tampoco compila el paquete: `build_probe.py` aborta con `AssertionError` en el SHA256 de sus entradas (CRLF y versiones de octubre de `ProbeRuntime.hpp`/`runtime_runner.py`).

## Que otro banco cubre sus funciones

`CPad::UpdateMouse` (0x53F3C0): **RequestedPadUpdateMouseV3** (mismo corpus de 9.216 casos y mismo modelo), `PadMouse1` (9.216 pares) y `UnlockInput` (2.048 pares de `UpdateMouse` y 4.096 de `ProcessPad`), los tres aprobados el 10/11/2026.
