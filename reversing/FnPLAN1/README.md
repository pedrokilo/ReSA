<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

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
