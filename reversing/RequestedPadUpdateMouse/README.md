<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 888 grupos / 10227 funciones reconstruidas; 10227 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedPadUpdateMouse (V1): NO repetible, sustituido por V3 (CPad::UpdateMouse 0x53F3C0)

Estado: **no repetible, y no tiene sentido repararlo**: es la primera version (V1) del banco de `CPad::UpdateMouse` que se preparo y congelo el 08/10/2026, nunca produjo una corrida y quedo reemplazada por `RequestedPadUpdateMouseV2` (rechazado) y por [`RequestedPadUpdateMouseV3`](../RequestedPadUpdateMouseV3/README.md), que **si es repetible y esta aprobado** (9.216 pares, cero diferencias, 10/11/2026).

## Prueba

- `prepared-freeze.json`: `prepared_frozen_not_compiled_not_executed`; no hay `runs/`, `artifacts/` ni `validation-summary.json` (no existe evidencia de V1).
- La V2 dice por que se descarto V1: `RequestedPadUpdateMouseV2/prepared-freeze.json`, `revision_reason`: "Only copied VTableRanges namespace repaired to match Preflight; unchanged 9216 corpus and functional sources. V1 failed build archived byte-exact." Es decir, la compilacion de V1 fallaba por construccion (el espacio de nombres de su `VTableRanges.hpp` no coincidia con el de `Preflight.hpp`).
- Ademas, sus helpers se buscaban por nombre de hook (`Find("WinInput","diMouseInit")`, `Find("CPad","GetMouseState")`), que V3 sustituyo por direccion; y, como en V2 y V3, `build_probe.py` abortaba con `AssertionError` en el SHA256 de sus entradas (CRLF y versiones de octubre de `ProbeRuntime.hpp`/`runtime_runner.py`, ver el README de V3).
- Reparar V1 equivaldria a rehacer V3 (las correcciones de V2 a V3 estan en `LiveComparison.hpp` de V3).

## Que otro banco cubre sus funciones

`CPad::UpdateMouse` (0x53F3C0): **RequestedPadUpdateMouseV3** (9.216 pares, mismo corpus y modelo), `PadMouse1` (9.216 pares) y `UnlockInput` (2.048 pares de `UpdateMouse` y 4.096 de `ProcessPad`), los tres aprobados el 10/11/2026.
