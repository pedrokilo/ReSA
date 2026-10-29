<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FxWoodNeg73: Fx_c::AddWood 0x49EE10 con rand FALSO (restos negativos del idiv) - banco hermano, solo evidencia adicional

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-1`). 16384 pares, cero diferencias, oraculo Python 0 lados distintos. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fxwoodneg73\20261027-014213-354435` (corrida final, con coverage.json ampliado; la anterior equivalente fue 20261027-013946-779980).

Este banco es la version del banco `reversing/FxWood73` anterior al refuerzo del 2026-10-27 (commit 5682a6e22): sustituye el `rand()` del EXE (0x821B1E) por un falso identico en la direccion del EXE y en el cuerpo de la DLL, que devuelve enteros de 32 bits arbitrarios, incluidos negativos y extremos (0x80000000 = INT_MIN, 0xFFFFFFFF = -1, 0xFFFFD8F0 = -10000, multiplos negativos de 10000, 0x7FFFFFFF...). Con el rand real (banco FxWood73) estos restos son inalcanzables (rand >= 0); aqui se comprueba que el EXE (`cdq; idiv 10000`, resto con el signo del dividendo) y la DLL (`% 10000` de C++ con int32) dan el mismo resto, el mismo color, tamano y velocidad. Cubre tambien: amount <= 0, rangos de 625, NaN/inf/desnormales, lightMult especial, cambio de sistema m_Blood.
NO registra nada: no hay `register_hook.py` ni fila de `docs/hooks.csv`; el registro de AddWood sigue siendo el de FxWood73.
Cobertura propia (coverage.json): sorteos negativos (`rand_negative_draw` 2775 casos), INT_MIN (1131), -1 (1112), -10000 (1114), multiplos negativos de 10000 (1117), negativos en los sorteos de color (2692) y en los de desplazamiento (2611).
Nota: en el original, `idiv` con INT_MIN y divisor 10000 no desborda (resto -3648); el oraculo lo modela y EXE/DLL coinciden.

```
bash reversing/_handoff/bench_here.sh FxWoodNeg73
python -B reversing/FxWoodNeg73/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/FxWoodNeg73/fixture_oracle.py <carpeta>/comparisons.jsonl
```
Archivo de arranque en el clon: `fxwoodneg73-runtime.start`; salida `fxwoodneg73-runtime.jsonl`.
