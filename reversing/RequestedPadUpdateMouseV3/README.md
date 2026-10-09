<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedPadUpdateMouseV3 (CPad::UpdateMouse 0x53F3C0)

Preparado, congelado, NO compilado y NO ejecutado.

V3 reemplaza al banco `RequestedPadUpdateMouseV2`, que quedó RECHAZADO y congelado
(`reversing/RequestedPadUpdateMouseV2/runs/20261008-232556-013193`). Ese fallo se debió a dos
defectos del arnés en `LiveComparison.hpp`, no a la función reversada.

## Defectos del arnés corregidos

1. `Update()` ejecutaba `check->LockState(false); check->State(false)` ANTES de calcular
   `checkOwn`/`checkBody`. Desenganchar reescribe nuestro thunk con `E9 -> 0x53F270`, de modo que
   `Body(checkOwn)` terminaba en 0x53F270 (nativo), `pureCandidateKnown` era falso,
   `helpers_known=false` y el bucle de 9216 casos se omitía (`comparisons=0`).
   Ahora `checkOwn` y `checkBody` se calculan antes de desenganchar (con el hook activo se lee el
   thunk intacto; si ya estaba desenganchado se decodifica `m_LibOriginalFunctionContent`).
2. `checkNativeBefore` (128 bytes de 0x53F270) se capturaba con el hook ya quitado, y la
   restauración final lo comparaba contra el estado reenganchado, dando falso negativo aunque la
   restauración fuera correcta. Ahora se captura además `checkHookedNative` (estado inicial, con
   hook) y es el que se usa en la comparación final. `checkNativeBefore` (desenganchado) se
   conserva para `checkUnchanged`.

## Alcance (idéntico a V2)

- 9.216 casos finitos y aislados en CPU de `CPad::UpdateMouse`; mismo corpus y mismo oráculo
  (`model.py`), sin cambios.
- Cubre: CPad completo + mouse, guards, dispositivo, preferencias, reloj; `CheckForInput` nativo
  real usado por ambos lados; identidad y ABI de helpers trazadas; puertas estrictas de FP y
  restauración de los 17692 rangos globales.
- NO cubre: NaN/Inf/INT_MIN, COM/DirectInput real, dispositivos reales, entrada real, integración
  con actores ni acreditación de los helpers.

## Renombres V2 -> V3

Carpeta, prefijo `pad-update-mouse-v3` (ASI/PDB, log `-runtime.jsonl`, harness, lock,
`resultados-*`). Fuentes de `source/` sin cambios. `prepared-freeze.json` regenerado.

## Comandos

```
python -B reversing/RequestedPadUpdateMouseV3/build_probe.py
python -B reversing/RequestedPadUpdateMouseV3/inspect_compiled.py
python -B reversing/RequestedPadUpdateMouseV3/Diagnostics/verify_actual_calls.py
python -B reversing/RequestedPadUpdateMouseV3/run_local.py
```
