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
