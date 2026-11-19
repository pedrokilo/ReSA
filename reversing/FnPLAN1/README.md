# FnPLAN1: plan de sustitución de las raíces de código (18/11/2026)

Informe: [docs/ExeIndependiente/plan-sustitucion.md](../../docs/ExeIndependiente/plan-sustitucion.md). Solo análisis estático: no ejecuta el juego ni compila.

## Reproducir (en este orden)

```
python -I raices.py --exe "<gta_sa.exe>" --ind0a <carpeta datos de IND0A> --repo <raiz>   # docs/ExeIndependiente/raices.csv (890 filas), datos/grafo_raices.csv, datos/cierre_por_raiz.csv
python -I lotes.py --repo <raiz> [--ejecutadas <lista de direcciones de IND1A>]             # lotes S1A.., curva, tabla_lotes.md, columnas lote/oleada/tipo/arranque en raices.csv
python -I verificacion_muestra.py --repo <raiz>                                             # datos/verificacion_muestra.csv (55 filas revisadas a mano)
python -I muestra.py --exe "<gta_sa.exe>" --repo <raiz> --fija                              # desensamblado de esa muestra (sin --fija vuelve a sortear)
python -I avance.py --repo <raiz> --datos <datos de una IND0A nueva>                        # raíces abiertas por categoría y lote
```

`raices.py` necesita `refs_codigo.csv` de IND0A (7,5 MB, no versionado; se regenera con `reversing/FnIND0A/scan_obj.py`) y los CSV de `docs/`. Tarda ~30 s.

## Archivos

| Archivo | Para qué |
|---|---|
| `raices.py` | Tabla de raíces: módulo, tamaño, categoría, quién la llama (archivo:línea), cierre por raíz, flags x87/SEH/SSE, llamadoras en el EXE, comprobación estructural. Lleva la lista `PROMOVER` (9 de ruido a probable por revisión manual) |
| `lotes.py` | Agrupa en lotes S1A.. (≤ 32), calcula dependencias, oleada de `plan-validacion.md`, curva de cierre y la columna de ejecución hasta el menú si se le da la lista de IND1A |
| `verificacion_muestra.py`, `muestra.py` | Muestra de 55 filas: veredicto manual y desensamblado |
| `avance.py` | Métrica de avance 788 → 0 |
| `datos/` | `grafo_raices.csv`, `cierre_por_raiz.csv`, `curva_cierre.csv`, `tabla_lotes.md`, `resumen_*.json`, `verificacion_muestra.csv`, `verificacion_resumen.json` |

Salidas en `docs/ExeIndependiente/`: `raices.csv`, `lotes-sustitucion.csv`, `plan-sustitucion.md`.
