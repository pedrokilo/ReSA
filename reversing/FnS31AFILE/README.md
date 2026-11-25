<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnS31AFILE: cierre de S31A (tiras de triangulos de RpWorld) y revision de la API publica de FILE del CRT (21/11/2026)

Rama `s31a-file` (partiendo de `origin/main` 77c4422b1b; sin subir). Decision del 20/11: se reconstruye y se engancha (hook activo), se compila y no se valida con bancos en juego. El juego no se ha ejecutado en este lote. Aun asi se ha hecho una comprobacion fuera del juego (banco con el EXE mapeado en memoria, solo lectura), descrita abajo.

## 1. Las tres raices S31A que faltaban (rpworld_s30a31a.cpp)

| Direccion | Funcion | Bytes | Como se reconstruyo |
|---|---|---:|---|
| 0x75B780 | `RpTriStripMeshTunnel` | 922 | a mano (x87 pasa a `float`/`double`; ver nota) |
| 0x75C380 | `RpBuildMeshGenerateTriStrip` | 731 | transcripcion literal (`lift.py`) |
| 0x75DAE0 | `RpGeometrySortByMaterial` | 1858 | transcripcion literal |

Dependencias internas (todas con gancho activo, `validated=0` en `docs/hooks.csv`, clase `RpWorld`):

| Direccion | Funcion (nombre propio) | Bytes | Forma |
|---|---|---:|---|
| 0x75BB20 | `_rpTriStripTunnelToggle` (invierte las aristas de tira del tunel) | 48 | a mano |
| 0x75BB50 | `_rpTriStripPolygonCountStripEdges` | 48 | a mano |
| 0x75BB80 | `_rpTriStripTunnelIsValid` (el tunel no cierra un ciclo de tira) | 256 | a mano |
| 0x75BC80 | `_rpTriStripTunnelCost` (suma el coste con `costCB`, sube la marca de prueba global 0x8D6390) | 256 | a mano |
| 0x75C660 | `TriStripMeshCreate` (RpTriStripMesh a partir de triangulos: poligonos, aristas compartidas por cubos de vertice menor) | 880 | transcripcion |
| 0x75C9D0 | `TriStripEmit` (de las tiras a la lista de indices con triangulos degenerados y control del giro) | 2160 | transcripcion |
| 0x75D240 | `TriStripNeighbours` (siguiente/siguiente-siguiente de una tira y 3 vertices) | 640 | transcripcion |
| 0x75E230 | `GeometrySortCompare` (comparador de qsort de SortByMaterial) | 64 | a mano |
| 0x74BFD0 | `_rpGeometryPluginFirstEntry` (primera entrada del registro de plugins de geometria, 0x8D629C) | 6 | a mano |

Cambios en otros archivos: `rpworld.cpp` (los tres cuerpos por direccion pasan a comentario; `BuildMeshTriangleCompare` deja de ser `static` y se declara en `rpworld.h`, porque la raiz nueva lo pasa a `Qsort`), `docs/hooks.csv` (+12 filas). Los ganchos estan al final de `RpWorldInjectHooksS30A31A`. `Qsort` y `Pow` ya eran propios (`ExeCrt`).

### Las transcripciones literales (`lift.py`)

`TriStripEmit` (el generador de tiras con las correcciones de orientacion), `TriStripNeighbours`, `TriStripMeshCreate`, `RpBuildMeshGenerateTriStrip` y `RpGeometrySortByMaterial` tienen mucha logica de saltos y de ranuras de pila reutilizadas; en lugar de interpretarlas a ojo se transcriben instruccion a instruccion con una herramienta (`lift.py` + `gen_lifted.py`): los registros pasan a variables `uint32_t`, la pila a una matriz de bytes local `fr` con desplazamientos estaticos desde el ESP de entrada (los parametros de salida son punteros a esa matriz), las banderas se calculan en cada instruccion y los saltos son `goto` a etiquetas por direccion; cada linea lleva la instruccion original como comentario. No hay `__asm` ni `naked`, pero el estilo es de transcripcion (no refactorizado): una limpieza a nombres de dominio queda para cuando haya tiempo, con el banco de abajo como red de seguridad. `splice.py` regenera ese bloque dentro de `rpworld_s30a31a.cpp` (parte desde `HEAD`), junto a `parte_manual.cpp`.

### Nota sobre `RpTriStripMeshTunnel` (x87)

El EXE calcula `acumulado = aciertos / poligonos + acumulado / 2` con la x87 (`fild` de 64 bits, `fidiv`, `fmul 0.5`, `faddp`, `fstp` a float) y el umbral `pow(0.001, calidad)` con `Pow`. El juego deja la x87 en precision de 24 bits (Direct3D9): el cociente se redondea a `float` y la suma una vez mas; `RpTriStripMeshTunnel` lo reproduce asi (SSE2 con `double`). Solo se perderia exactitud en un empate al bit con el umbral. Sin comprobacion de memoria (como el EXE) y con el detalle `limite 0 -> devuelve la malla sin tocarla`.

## 2. Banco fuera del juego (EXE original contra codigo propio): 0 diferencias

`bench_tristrip.cpp` (+ `common.h`, `reserva_imagen.asm`, `extract_bench.py`, `build_bench.cmd`; mismo metodo que FnLIB4A): un proceso de 32 bits mapea el `gta_sa.exe` original (solo lectura) en 0x400000, instala un asignador del motor falso en la tabla `RwEngineInstance + 0x134/0x138/0x13C` y llama por direccion a la funcion del EXE y despues a la propia, partiendo de la misma arena determinista (misma direccion, relleno 0xCD). Se compara el valor devuelto, la huella de **toda la arena** (mallas, aristas, bufferes y fugas incluidas) y el registro de reservas/liberaciones. FPU a 24 bits. `extract_bench.py` compila el bloque propio tal como esta en `rpworld_s30a31a.cpp`.

| Escenario | Comparaciones por escenario | Resultado (2 semillas x 4.000 escenarios sobre el codigo final) |
|---|---|---|
| `TriStripMeshCreate` 0x75C660 | 1 | 0 diferencias |
| `RpTriStripMeshTunnel` 0x75B780 (todo el recorrido con sus 4 ayudantes; calidades 0.05..5, limite 0..40, mallas de 1 a 150 triangulos con vertices compartidos) | 1 | 0 |
| `TriStripEmit` 0x75C9D0 (con y sin respetar el giro, sobre mallas ya tuneladas) | 2 | 0 |
| raiz `RpBuildMeshGenerateTriStrip` 0x75C380 (con triangulos degenerados inyectados, 1-3 materiales) | 1 | 0 |
| `RpGeometrySortByMaterial` 0x75DAE0 (geometria simulada: 1-3 morph targets, prelit, normales, 0-2 juegos de coordenadas, plugins con datos de usuario, callback nulo o no, fallos inyectados de Create/AddMorphTarget/MaterialListCopy/Unlock; ~86 % con resultado) | 1 | 0 |

Total: 8.000 escenarios, 40.000 comparaciones de tiras y 8.000 de geometria, 0 diferencias (el codigo previo con el cociente en `double` tambien dio 0 en ~33.000 escenarios). Controles negativos: con `0x75BB80` sustituida por `xor eax,eax; ret` en el EXE el Tunnel da 74 diferencias de 100; con el comparador 0x75E230 anulado, `SortByMaterial` da 196 de 200. Limite: `RpGeometry` y sus funciones (`RpGeometryCreate`, `Lock`, `Unlock`, `AddMorphTarget`, `Destroy`, `MaterialListCopy`, `RpMorphTargetCalcBoundingSphere`) son simuladas por el banco (el EXE se parchea hacia las mismas), asi que no se ha probado contra la implementacion real del motor.

## 3. API publica de FILE del CRT (lote 2): no hay nada que reconstruir

`fputc`/`putc`, `ungetc`, `rewind`, `fscanf`, `tmpfile`, `remove`/`rename`/`_unlink` **no existen en el `gta_sa.exe`**: el CRT va enlazado estaticamente y solo trae lo que el juego usa (comprobado leyendo el EXE; ver [FnCRT6A](../FnCRT6A/README.md)). Los 11 llamadores de `_lock_file` ya estaban hechos; `_input` solo lo llama `sscanf`; `_flsbuf` solo `_output`, `snprintf`, `scprintf`, `fwrite_nolock` y `__ftbuf`; `_lseek` solo `fseek`, `ftell` y `_flsbuf`; el EXE no importa `MoveFileA`, `GetTempPathA` ni `GetTempFileNameA` y `DeleteFileA` solo se usa en 0x619103 (codigo del juego) y en un trampolin de importacion. No se anadio codigo ni ganchos.

## 4. Verificacion y lo que queda

- Compilacion del proyecto (Debug, `_build_S31AFILE.sh` con el candado `compile_lock`): `EXIT=0` (`D:\Original\_build_S31AFILE.log`); `contrib/audit_hook_catalog.py --strict` OK; `audit_cpp_assembly.py`: 0 ocurrencias de `__asm`/`naked`.
- Sesion de humo (`prueba_humo.py`, copia de `FnBIS1/prueba_ropero.py` generada por `mk_humo.py` que prueba la DLL de este arbol con `jugar.ps1 -Variante X`; espero a que el juego del usuario terminara): `cal_mal.args`, **VEREDICTO BIEN** (puerta True, marcador True, 15 entidades, 12 marcadores; sin cierres del juego). Ojo: estas funciones solo corren al generar tiras de triangulos (`RpMeshSetTriStripMethod` con el generador de tunel o por defecto) y al ordenar geometrias por material, y no hay garantia de que esa escena las ejerza; la evidencia fuerte de equivalencia es el banco de arriba.
- Pendiente: limpieza a nombres de dominio de las cinco transcripciones; `RpGeometrySortByMaterial` contra el motor real (hace falta un banco con RenderWare inicializado).
- Herramientas: `desens.py`/`nombres.py`/`fns.py` (desensamblado con capstone y mapa de funciones), `dis_*.txt` (desensamblados con nombres), `chk.bat`/`chk.ps1` (comprobacion de sintaxis `cl /Zs`).
