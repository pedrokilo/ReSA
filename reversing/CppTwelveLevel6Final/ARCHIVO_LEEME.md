# Evidencia pesada archivada fuera del repositorio

Los volcados completos de los bancos de validación (y algunos binarios de sondas) pesaban ~18 GB y
llevaron el árbol de git a ~19 GB; el CI de GitHub (windows-2025, ~14 GB libres) se quedaba sin disco en el
checkout. Se sacaron del árbol versionado **sin reescribir el historial**: siguen en los commits anteriores y
en un archivo local.

- Archivados: **1108 archivos, 18.05 GB** (`comparisons.jsonl[.gz|.xz]`, `actual-bank.jsonl`,
  `raw-bank.jsonl`, `state.jsonl`, `Diagnostics/**/*.jsonl`, `reversing/FnTraf/**/gta_sa_standalone.exe|.map`
  y dos `.json` de más de 20 MB). Regla exacta: `contrib/archivar_evidencia_pesada.py` (`es_pesado`).
- Se **quedan** en git: los resúmenes (`summary.json`, `RESULTADO.txt`, `validation-summary.json`), el registro
  `reversing/validation-registry`, los scripts y todo el código. Ninguna herramienta del repositorio
  (`validation_registry.py`, `sync_validated_flags.py`, `audit_hook_catalog.py`, `hooks2md.py`, `sync_docs.py`,
  `progress_report.py`, tests) lee ni hashea los archivados: el registro verifica el hash del resumen pequeño.
- Integridad: `ARCHIVO_MANIFIESTO.csv` (ruta relativa, bytes, sha256, id de blob de git, ubicación). Cada archivo
  se comprobó contra el blob de git antes de moverlo.

| Directorio | Archivos | GB |
| --- | ---: | ---: |
| `reversing/CppTwelveLevel6Final` | 844 | 15.55 |
| `reversing/FnTraf` | 9 | 0.58 |
| `reversing/RequestedFxEmitterTwo` | 4 | 0.25 |
| `reversing/RequestedArrestRemainingFour` | 3 | 0.25 |
| `reversing/CppTwelveLevel6Unified` | 67 | 0.17 |
| `reversing/RequestedPoolsSixGettersV2` | 2 | 0.09 |
| `reversing/RequestedPadSelectors` | 2 | 0.08 |
| `reversing/RequestedPreflightTaskCtorDtor` | 3 | 0.07 |

## Dónde está el archivo

`D:\Original\evidencia_validacion_archivo\` (mismo disco que el repositorio; conserva las rutas relativas, por ejemplo
`...\evidencia_validacion_archivo\reversing\CppTwelveLevel6Final\runs\<Fn...>\comparisons.jsonl.gz`).
Ese directorio **no está en git**: haga copia si le importa la evidencia.

## Cómo restaurar

```
python contrib/restaurar_evidencia_archivada.py --verificar          # comprueba el archivo local contra el manifiesto
python contrib/restaurar_evidencia_archivada.py                      # copia todo a su sitio (verifica sha256)
python contrib/restaurar_evidencia_archivada.py --filtro runs/Anim16 # solo lo que contenga esa cadena
python contrib/restaurar_evidencia_archivada.py --archivo OTRA_RUTA  # el archivo está en otro sitio
```

Desde el historial de git (sin el archivo local): el último commit que los contiene es `a00752a002`, padre de la
rama `repo-liviano`.

```
python contrib/restaurar_evidencia_archivada.py --desde-git a00752a002
git checkout a00752a002 -- reversing/CppTwelveLevel6Final/runs/<Fn...>/comparisons.jsonl.gz   # un archivo
```

(`git checkout <commit> -- <ruta>` deja el archivo en el índice; los `.gitignore` de `reversing/` impiden volver a
añadirlo por error, pero un `git add -f` lo permitiría: no lo haga. Con `git restore --source=a00752a002 --worktree -- <ruta>`
solo se escribe en el árbol de trabajo.) Los archivos restaurados quedan ignorados por git; la restauración
verifica sha256 y no deja nada a medias si no coincide.
