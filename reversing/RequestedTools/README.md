<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 883 grupos / 9938 funciones reconstruidas; 9938 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RequestedTools: utilidades para repetir los bancos `Requested*` (REPARAR-REQUESTED, 10/11/2026)

Los bancos `reversing/Requested*` se prepararon y congelaron en octubre de 2026. Con el codigo actual de `main` dejan de ser repetibles por causas mecanicas del banco (no de produccion):

| Sintoma | Causa | Herramienta |
| --- | --- | --- |
| `unknown_vtable_category X` en el preflight | `VTableRanges.hpp` congelado: faltan clases con hooks virtuales posteriores | `vtable_ranges_sync.py` |
| `AssertionError` en `build_probe.py` (hash de un fichero) | el congelado guarda SHA256 de ficheros CRLF o de versiones antiguas de ficheros compartidos | `refreeze_generic.py` |
| compilacion con `/MP6` | `_CL_` forzado | `sed` sobre `build_probe*.py`: `os.environ.get('_CL_','/MP6')` |
| `run_local.py` rechaza la prueba estatica del ASI (`asi_sha256 != manifest['asi']`) | la prueba estatica era un paso manual ligado al hash del ASI | `add_prove.py` (anade `prove()` a `build_probe.py`) |
| `--bank` obligatorio | bancos con subbancos (`RequestedPadMutationRemaining3*`) | `bench_here_args.sh` con `BENCH_ARGS="--bank X"` |
| `snapshot_ranges != 17692` | constante de octubre; hoy son ~20.600 rangos | en `run_local.py`: `< 2*hooks_snapshotted` |

Uso (desde la raiz del repo):

```
python reversing/RequestedTools/vtable_ranges_sync.py            # informe (clases ausentes o distintas por banco)
python reversing/RequestedTools/vtable_ranges_sync.py --apply    # anade las ausentes (nunca elimina filas)
python reversing/RequestedTools/refreeze_generic.py <Banco> ...  # recalcula hashes del congelado (tras tocar cualquier fichero del banco)
bash reversing/_handoff/bench_here.sh <Banco>                    # compilar + correr (con candados)
BENCH_ARGS="--bank shake" bash reversing/RequestedTools/bench_here_args.sh <Banco>
python reversing/RequestedTools/refresh_bank.py <Banco> <carpeta de evidencia> <manifest.json> [resumen.json]
python reversing/RequestedTools/refresh_funcsummaries.py <Banco> <id de corrida>   # resumenes por funcion (aggregate_parent)
python reversing/RequestedTools/archive_run.py <Banco> <carpeta de evidencia>      # archivo + limpieza de crudos
```

`refresh_bank.py` es la version de `reversing/_handoff/` ampliada para resumenes de bancos `Requested*` (sin `all_equal`/`pairs_per_function`, manifiesto con `tested_sources`).
