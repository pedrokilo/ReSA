<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Decisión: cifra de validadas y evidencia aprobada (21/11/2026)

## Síntoma

`contrib/sync_docs.py` y `contrib/hooks2md.py` abortaban con
`validated=1 (11801) no coincide con evidencia aprobada (9168)`.

## Causa real

`docs/hooks.csv` traía 11.801 filas con `validated=1`, todas registradas en
`reversing/validation-registry/`. El registro guarda el sha256 de cada evidencia
(`validation-summary.json`) y `contrib/validation_registry.py` lo recalculaba sobre los bytes
del archivo tal cual. 2.630 de las 2.633 entradas rechazadas tienen el hash de la **misma copia con saltos CRLF**:
se registraron en copias de trabajo de Windows (`core.autocrlf=true`), y `.gitattributes` fija
`* text=auto eol=lf`, así que cualquier checkout limpio (y los worktrees) tiene LF y otro hash.
No faltaba evidencia ni había lotes de RenderWare/CRT/datos marcados sin respaldo: las 11.801 filas
tienen registro y ninguna `validated=1` carecía de él.

Las otras 3 entradas (`CBaseModelInfo::SwaysInWind`, `FxManager_c::DestroyAllFxSystems`,
`FxManager_c::Update`) sí tienen evidencia **modificada** después de registrarse (su hash no coincide ni con LF ni con CRLF).

## Decisión

- No se infla nada: `validated=1` sigue significando evidencia aprobada con hash comprobado.
- `validation_registry.py` acepta como idéntico un archivo de **texto** cuyo hash coincide con el del mismo
  contenido con CRLF (`digest_crlf`); un solo byte distinto, o un archivo binario, sigue invalidando.
  Se aplica a evidencias, fuentes probadas y revisión de dependencias.
- Las 3 funciones con evidencia alterada pasan a `validated=0`: siguen reconstruidas y con gancho activo
  (`reversed=1`), pero sin validar. Cifra resultante: **11.798 de 13.401** (antes 11.801 declaradas de 13.311).
- La política vigente («no validar más, todo activo») no cambia: no se marcó ninguna función nueva.
- `sync_current_markdown.py` comprobaba hashes de 28 Markdown congelados con el mismo defecto: ahora acepta CRLF y avisa (sin abortar) de
  los 2 que no se pueden verificar en ningún formato (`RequestedFxEmitterTwo/DESIGN.md`,
  `RequestedArrestRemainingFour/DESIGN.md`); esos se omiten y nunca se reescriben.
- «Vigentes» (evidencia + fuentes + dependencias coinciden) sigue en 0: el código fuente cambió desde las pruebas.

## Reproducir

```
python contrib/sync_validated_flags.py      # alinea validated con la evidencia
python contrib/sync_docs.py                 # sin --update-source-status (cambia las huellas de las fuentes)
python contrib/sync_readme_counts.py
python contrib/inventario_comandos_script.py
python contrib/audit_cpp_assembly.py        # luego git checkout de FILES.md/files.csv/inventory.csv si solo cambian los saltos de línea
python contrib/sync_current_markdown.py --date AAAA-MM-DD
python contrib/audit_hook_catalog.py --strict
```
