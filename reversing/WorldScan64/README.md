<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WorldScan64: CWorldScan::ScanWorld (0x72CAE0) y CWorldScan::SetExtraRectangleToScan (0x72D5E0)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wg-4`, HEAD df26d6fe7 + fuente del lote 64). 8192 pares (2 x 4096), cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauración en ambos lados y salida de cada lado igual a la del oráculo independiente; cobertura mínima cumplida. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-worldscan6461020-154849-447199`.

`source.patch` contiene SOLO los cambios de fuente de este lote (`source/game_sa/Renderer.cpp`, `Renderer.h`, `InjectHooksMain.cpp`) respecto de HEAD (`git apply --cached --check` verificado).

| Función | Dirección | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CWorldScan::ScanWorld(CVector2D*, int32, tScanFunction)` | 0x72CAE0 | Simple | 4096 (14 familias) | `CGeneral::GetATanOfXY` (0x53CC70); la devolución de llamada `scanFunction` es un falso del banco |
| `CWorldScan::SetExtraRectangleToScan(float x4)` | 0x72D5E0 | Simple | 4096 (8 familias) | ninguno |

`CWorldScan` no tenía `InjectHooks` ni fila en `docs/hooks.csv`: se añade `CWorldScan::InjectHooks()` (Renderer.cpp, llamado tras `CRenderer::InjectHooks()` en InjectHooksMain.cpp) con `RH_ScopedClass(CWorldScan)` + `RH_ScopedCategoryGlobal()`.
Filas necesarias en `docs/hooks.csv` (las añade el coordinador): `CWorldScan,ScanWorld,0x0072CAE0,1,0,1,S` y `CWorldScan,SetExtraRectangleToScan,0x0072D5E0,1,0,1,S`.
(No confundir con `CRenderer::ScanWorld`, 0x554FE0, función distinta que ya estaba hookeada.)

## Qué hacen (desensamblado 0x72CAE0..0x72D5D0 y 0x72D5E0..0x72D6A7)

`ScanWorld(points, count, scanFunction)` (cdecl, void, el llamador limpia 12 bytes) rellena un polígono (2 a 5 puntos en coordenadas de sector) y llama `scanFunction(x, y)` por cada celda:
1. **0x72CAE0** quita puntos repetidos (comparación exacta de x e y contra el punto `i-1`), desplazando el arreglo DEL LLAMADOR y reduciendo `count`.
2. **0x72CBA3** elige el punto de menor y (el primero en caso de empate) y **0x72CC64** hace una envoltura de regalo (hasta 5 vértices en la pila) con `GetATanOfXY(dx, dy)` menos la suma de ángulos previos, normalizado a [0, 2pi) con bucles de ±2pi; se detiene al volver a un punto ya visitado. Menos de 3 vértices: sale sin vaciar las celdas extra.
3. **0x72CD5C** fila mínima/máxima (`floor`+`_ftol2`), **0x72CEC0/0x72CF47** cadenas izquierda/derecha (saltan vértices de la misma fila), **0x72CFC1** pendientes y x de la primera fila.
4. **0x72D0F0** bucle por filas: llama `scanFunction(x, y)` de `minX` a `maxX`; tras cada llamada quita de las celdas extra (0xC81598 Y[4], 0xC815A8 X[4], contador 0xC815B8) la primera celda igual (desplazando; el EXE fija el índice a 4 y sale: solo se quita una por llamada), y calcula `minX`/`maxX` de la fila siguiente (rutas 0x72D1B8, 0x72D214, 0x72D329, 0x72D380 a la izquierda; 0x72D3AC, 0x72D3EB, 0x72D50A, 0x72D559, 0x72D566 a la derecha).
5. **0x72D593** llama `scanFunction(X[i], Y[i])` con las celdas extra que quedan (releyendo el contador en cada vuelta) y pone el contador a 0.

`SetExtraRectangleToScan(minX, maxX, minY, maxY)` (cdecl, void) apila en las tablas de celdas extra las celdas `x` de `floor(minX)` a `ceil(maxX)-1` por `y` de `floor(minY)` a `ceil(maxY)-1` (sin comprobar el límite de 4). `floor` = 0x8219F0, `ceil` = 0x823820, `_ftol2` = 0x821B40.

## Fuente (Renderer.cpp)

C++ puro con la estructura de saltos del EXE: etiquetas `L_xxxx` con la dirección y `goto`. Aritmética float redondeada en cada operación (el x87 del EXE trabaja a 24 bits; la DLL usa SSE: equivalentes para + - * /), comparaciones con la misma semántica que `fcom`/`test ah` (con NaN no se actualizan mínimos), las constantes del EXE (2pi = 0x40C90FDB; ángulo inicial 0x47C34FF3 = 99999.9...), `floor`/`ceil` del CRT y `int64 → int32` como `_ftol2` (NaN/inf/fuera de rango dan 0). Las celdas extra se indexan con punteros crudos a las direcciones del EXE (sin comprobar límites, como el original). Detalles del EXE conservados: `[esp+38h]` (banderas de puntos visitados) se lee como índice en `ebx` si ningún ángulo mejora; el escaneo de celdas extra sale tras la primera coincidencia (`mov ecx,4`); la suma de ángulos se resta ANTES de normalizar.

## Registros (EAX/ECX/EDX)

Llamadores de `ScanWorld` en el EXE: 0x554E8F (sobrescribe EAX justo después), 0x55555E, 0x555664, 0x555C9B y 0x6E6E8C (CWaterLevel): ninguno lee EAX/ECX/EDX después de la llamada. `SetExtraRectangleToScan` no tiene llamadores en el EXE (ni `call` ni `jmp` ni la dirección como dato): código muerto. Solo se registran (`kCompareVolatileRegisters=false`).

## Banco

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con solo ese hook encendido. `GetATanOfXY` se sustituye por un falso idéntico en ambos lados (parche en la entrada 0x53CC70 del EXE y en el cuerpo de la DLL, calculado ANTES de crear las rutas/parches y restaurado en orden inverso): devuelve `float(atan2(double(y), double(x)))` y registra `[1, bits(x), bits(y), bits(ret)]`. La devolución de llamada pasada a `ScanWorld` registra `[2, x, y]`. Se comparan, por lado: eventos EN ORDEN, `nevents`, cambios del arreglo de puntos del llamador (desplazamiento por duplicados), cambios de la región de celdas extra (0xC81598..0xC815BC) y las compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 normalizado, ruta del hook, entradas de los colaboradores sin tocar, restauración del estado real).

Fixtures (`Fixtures.hpp`): ScanWorld, 14 familias (cuadriláteros aleatorios, rectángulos alineados con 4 órdenes de esquinas, triángulos, 5 puntos, duplicados, coordenadas enteras, trapecios con aristas horizontales, patrón de 5 puntos de `CWaterLevel` con último = primero, celdas extra dentro/fuera del polígono, gran desplazamiento, degenerados de 2 puntos o colineales, láminas de una sola fila, decimales de mantisa completa); SetExtraRectangleToScan, 8 familias (límites enteros, fracciones, contador previo, rectángulos vacíos en x o en y, negativos, un ulp alrededor de enteros).
`fixture_oracle.py` modela las dos funciones DESDE el desensamblado (mapa de pila, `fcom`/`test ah`/`jp` literales, float32 redondeado por operación, `floor`+`_ftol2`); `fixture_mirror.py` compila `Fixtures.hpp` con `cl` sin el juego, pasa los 8192 casos por el oráculo, comprueba que el `atan2` del falso coincide con el de Python y la cobertura mínima. `host_check.py` compila la fuente real de Renderer.cpp en el anfitrión (celdas extra en un arreglo propio y GetATanOfXY falso) y la compara con el oráculo: **8192 casos, cero diferencias** (detecta erratas de transcripción antes de correr el juego; no sustituye al banco).

## Suposiciones y límites (alcance)

- `GetATanOfXY` y `scanFunction` son falsos: no se validan sus cuerpos ni la integración real con los sectores del mundo (ScanSectorList* etc.).
- Sólo 2 a 5 puntos finitos (|coordenada| < 700), sin NaN ni infinitos: con NaN el EXE usa como índice el dword de banderas (lectura fuera de rango) y con más de 5 puntos escribe sobre sus propias variables de pila. No se prueban n < 2 (el EXE lee `points[1]` aunque `count` sea 0 o 1), ni más de 4 celdas extra (el EXE no comprueba el límite: `Y[4]` es `X[0]`).
- Los float de entrada son siempre finitos y nunca señalizadores: el EXE copia con `fld`/`fstp` (silencia sNaN) donde la DLL copia el patrón de bits.
- Se evitan magnitudes que lleven los productos x87 de 24 bits fuera del rango de float (el registro x87 conserva exponente extendido hasta guardar): coordenadas moderadas.
- `floor`/`ceil`/`_ftol2` del CRT son reales en ambos lados (el de la DLL es el del CRT de la DLL, no el 0x8219F0 del EXE).
- Se compara EAX/ECX/EDX solo como registro (sin llamadores que los lean). `kCompareDeadX87Registers=false` como los bancos aprobados (GarageUpdate63, D3DResource63): los registros x87 vacíos no se comparan (el EXE y la DLL dejan restos distintos por construcción); sí se comparan FCW, TOP, etiquetas y registros válidos.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh WorldScan64   # compila la sonda y corre en el juego (candado global)
python -I reversing/WorldScan64/fixture_oracle.py       # autoprueba del oráculo
python -I reversing/WorldScan64/fixture_mirror.py       # coherencia de las fixtures + cobertura mínima (sin el juego)
python -I reversing/WorldScan64/host_check.py           # fuente real frente al oráculo, sin el juego
python -B reversing/WorldScan64/run_local.py --analyze <comparisons.jsonl>
python reversing/WorldScan64/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `host_check.py`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
