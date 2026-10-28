<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PresetLoad74: RsLoadPresetViews (0x619D60)

Estado: **APROBADO dentro del juego (27/10/2026, worktree `D:\Original\wy-4`, HEAD f13a750cf + `source.patch`). Evidencia final: ver el final de este archivo. Plantillas: `reversing/HeatFire73` (lote 73), `reversing/Helpers66A`. Contrato: `reversing/_handoff/BRIEF.md`.**

| Función | Dirección | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `RsLoadPresetViews()` (devuelve `NumPresetViews != 0`; antes una llamada por dirección) | 0x619D60 | `Rs` / `RsLoadPresetViews` / S (`RH_ScopedGlobalInstall`) | con hook RH (apagado durante el banco): `psPathnameCreate` 0x745470, `psPathnameDestroy` 0x7454E0, `RsErrorMessage` 0x619B00, `CMemoryMgr::Malloc` 0x72F420; reconstruidas sin hook (DLL = función C++, EXE = código original): `RsDestroyPresetViews` 0x6198C0, `CMemoryMgr::Free` 0x72F430; sin cuerpo propio en la DLL (la DLL llama a la dirección del EXE): `isprint` del CRT 0x8228B5; y, durante la llamada, las entradas `fopen`, `fclose`, `fgets`, `sprintf`, `strcpy`, `strlen` y `sscanf` de la tabla de RenderWare (`RwEngineInstance + 0xC4 / + 0xF0`). Falso idéntico en ambos lados. |

Quién valida a cada colaborador (`docs/hooks.csv`, `reversing/validation-registry.json`): `psPathnameCreate/psPathnameDestroy` -> `reversing/Ps22`; `RsErrorMessage` -> `reversing/Rs27`; `CMemoryMgr::Malloc` -> `reversing/DependenciesMemory9`.
**Sin banco propio:** `RsDestroyPresetViews` y `CMemoryMgr::Free` (reconstruidas sin hook, fuera de `docs/hooks.csv`: el falso las reemplaza en ambos lados y este banco NO las valida) y `isprint` del CRT (el falso implementa `0x20 <= c <= 0x7E`: el banco no valida la configuración regional del `isprint` real). La E/S de ficheros y el formato de `sscanf` también son falsos (ver «Lo que NO prueba»).

Fila de `docs/hooks.csv` (la añade el coordinador): `Rs,RsLoadPresetViews,0x00619D60,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh PresetLoad74        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/PresetLoad74/build_probe.py
python -B reversing/PresetLoad74/run_local.py
python -B reversing/PresetLoad74/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aun no existe o no esta completo
python -B reversing/PresetLoad74/fixture_oracle.py <carpetaEvidencia>/comparisons.jsonl          # oraculo independiente (Python): 0 lados distintos del modelo
python -B reversing/PresetLoad74/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-presetload74/<corrida>"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `presetload74-runtime.start`; salida `presetload74-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de esta función respecto a HEAD)

- `app/platform/preset_view.cpp`: `RsLoadPresetViews` pasa de `plugin::CallAndReturn<bool, 0x619D60>()` a C++ puro (con el tipo de `fgets` de `PresetViewFileFunctions` corregido).
- `app/platform/platform.cpp`: `RH_ScopedGlobalInstall(RsLoadPresetViews, 0x619D60)`.

## Revisión contra el EXE (re-desensamblado y contrastado)

**0x619D60 (RsLoadPresetViews).** cdecl sin argumentos. `RsDestroyPresetViews()` (0x6198C0); `CurrentPresetView = -1`; `files = RwOsGetFileInterface()` (0x804130; nunca nulo); `path = psPathnameCreate("./views.txt")`; `f = files->fopen(path, "r")`; `psPathnameDestroy(path)`; `f` nulo -> `return NumPresetViews != 0` (0).
Bucle `while (files->fgets(buf, 0x3FF, f))` (buffer de 0x400 bytes): `rwstrlen(buf)` (resultado sin uso); si `buf[0]`, se filtra el buffer en el sitio con `isprint` (0x8228B5; por byte con `movzx`, la siguiente lectura es `src[1]` antes de incrementar); `n = rwsscanf(buf, "%f%f%f%f%f%f%f %[^]", &f1..&f7, nombre)` (nombre en un buffer de 0x40 bytes del EXE);
`n < 7` -> siguiente línea; `n == 7` -> `rwsprintf(nombre, "%s%d", "View", NumPresetViews)`; `nodo = CMemoryMgr::Malloc(0x24, 0)` (nulo -> `RsErrorMessage("Out of memory - not all preset views loaded")` y `return false` SIN cerrar el fichero); campos `x,y,z = f1..f3`, `rotX = f4`, `rotY = f5`, `near = f6`, `far = f7`
(copiados por palabras); `desc = Malloc(rwstrlen(nombre) + 1, 0)` (nulo -> `RsErrorMessage` + `CMemoryMgr::Free(nodo)` y `return false`, sin cerrar); `rwstrcpy(desc, nombre)`; el nodo pasa a la cabeza de la lista y `NumPresetViews++`. Al terminar: `fclose(f)` y `return NumPresetViews != 0`.

## Hallazgos

1. **Formato de `sscanf`.** El literal del EXE (0x86D3E4) es `"%f%f%f%f%f%f%f %[^" + chr(0) + "]"`: el conjunto queda sin cerrar tras el cero, asi que el texto C que ve `sscanf` termina en `%[^`. La primera version de produccion escribia `"...%[^]"`; el banco lo detecto (18271 de 20480 pares con otro hash del formato) y se corrigio a `"...%[^" "\0]"`.
2. **Defecto del banco (no de produccion):** el falso de `Malloc` devolvia el desplazamiento absoluto del montón sin sumar la guarda (los nodos caian en la guarda de 0x40 bytes); el oraculo lo detecto y se corrigio.
3. Los dos caminos de memoria insuficiente del EXE vuelven con `false` sin cerrar el fichero (replicado), y el de la descripcion libera el nodo con `CMemoryMgr::Free`; los floats y el nombre se copian por palabras/`strcpy` (los SNaN conservan su carga).
4. El campo `fgets` de `PresetViewFileFunctions` era `void*`; ahora tiene el tipo `char*(__cdecl*)(char*, int32, void*)`.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Por caso: los eventos en orden con los hashes FNV de cada texto (ruta, modo, buffer filtrado, formato, nombre), el recuento y el hash de las llamadas a `isprint`, el montón de pruebas byte a byte (nodos y descripciones; sólo los dos punteros de cada nodo son «cualquier valor»),
`NumPresetViews`, `CurrentPresetView`, `PresetViews`, AL, ABI, ruta, `collaborators_dll` y estado restaurado. El oráculo Python (`fixture_oracle.py`) reproduce la traza, el filtrado, los nodos y el valor de retorno desde la entrada.

**Familias (1024 casos, 20480 pares):** `fopen` nulo; fichero vacío; todas las líneas sin nombre / con nombre / ninguna válida; fallo del primer, segundo y N-ésimo `Malloc` (con y sin `Free` del nodo); líneas solo imprimibles, cualquier byte, bytes altos, controles y espacios; líneas vacías; floats especiales (NaN no canónicos,
SNaN, infinitos, desnormales, ceros con signo); ruta nula; sin vistas previas; contador previo extremo; nombres de 40 caracteres; líneas de 224 bytes; aleatorio.

## Lo que NO prueba este banco

- La E/S de ficheros real (`fopen`/`fgets`/`fclose` del RenderWare/EXE), el formato real de `sscanf` (el falso devuelve los valores configurados y escribe los floats y el nombre) ni `sprintf`/`strcpy`/`strlen` reales: se prueba la secuencia de llamadas, sus argumentos y el uso de sus resultados.
- El `isprint` real del CRT (configuración regional) ni la asignación y liberación reales de memoria; `RsDestroyPresetViews` y `CMemoryMgr::Free` no tienen banco propio.
- Nombres de más de 63 caracteres (el EXE desborda su buffer de 0x40 bytes), `RwOsGetFileInterface` nulo y líneas de más de 1022 bytes (el `fgets` real las partiría).
- `RsSavePresetView` (la repite `Helpers66A` con el mismo falso también en el cuerpo de la DLL).

## Negativa

Mutacion de produccion `if (count < 7) {` -> `if (count < 8) {` (las lineas con exactamente 7 numeros dejan de crear vista): **FALLO por diferencia de resultado** (10958 de 20480 pares difieren, ninguna caida; informe `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-presetload74\20261027-040204-678610`), fuente restaurada y banco repetido hasta APROBADO.

## Evidencia final

`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-presetload74\20261027-044311-743203`: 20480 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados, `fp_difference` vacio, cobertura por familia (20 x 1024) y de ramas (`coverage.json`) y el oraculo (`fixture_oracle.py`) coincide con los dos lados de los 20480 pares. 
