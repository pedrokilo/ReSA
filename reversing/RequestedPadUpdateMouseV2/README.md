# RequestedPadUpdateMouseV2: NO repetible, sustituido por V3 (CPad::UpdateMouse 0x53F3C0)

Estado: **rechazado y congelado el 08/10/2026; no repetible y no tiene sentido repararlo**: su fallo se debia a dos defectos del arnes (`LiveComparison.hpp`) que corrige [`RequestedPadUpdateMouseV3`](../RequestedPadUpdateMouseV3/README.md), el mismo paquete con otro nombre y las correcciones, que **si es repetible y esta aprobado** (9.216 pares, cero diferencias, 10/11/2026).

## Prueba

- Su unica corrida, `runs/20261008-232556-013193`, termino en FALLO con **0 comparaciones**: `errors = ['helper unresolved', 'missing/excess corpus', 'terminal']` y `helpers_known = false` en el registro `begin` (ver `runs/*/enriched-summary.json`). No hubo una sola comparacion EXE/DLL, de modo que el banco no demostro nada sobre la funcion.
- El README de V3 explica el motivo: V2 desenganchaba `CheckForInput` antes de calcular su entrada y su cuerpo (el hook reescribe el salto de la DLL a 0x53F270, de modo que el cuerpo "propio" acababa en el nativo, `pureCandidateKnown` era falso y el bucle de 9.216 casos no corria) y capturaba los 128 bytes de 0x53F270 con el hook ya quitado, con lo que la restauracion final daba un falso negativo. V3 corrige ambos defectos y ademas pasa de buscar los colaboradores por nombre de hook a parchearlos por direccion (V3.1).
- Repetir V2 reproduciria el fallo documentado (defecto del arnes) o, si se le aplicaran las correcciones, seria V3. Con la copia actual del repositorio tampoco compila el paquete: `build_probe.py` aborta con `AssertionError` en el SHA256 de sus entradas (CRLF y versiones de octubre de `ProbeRuntime.hpp`/`runtime_runner.py`).

## Que otro banco cubre sus funciones

`CPad::UpdateMouse` (0x53F3C0): **RequestedPadUpdateMouseV3** (mismo corpus de 9.216 casos y mismo modelo), `PadMouse1` (9.216 pares) y `UnlockInput` (2.048 pares de `UpdateMouse` y 4.096 de `ProcessPad`), los tres aprobados el 10/11/2026.
