<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlRest63C (tanda 3): PickNextNodeAccordingStrategy, JoinCarWithRoadSystem y JoinCarWithRoadSystemGotoCoors

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-4`, HEAD df26d6fe7 + fuentes del lote 63 parte C, tandas 1 a 3). 12288 pares (3 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados y cobertura de ramas medida sobre los eventos de los colaboradores. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlrest63c\20261020-205957-788299`.

Corridas previas: (1) cuelgue del juego por un bucle infinito del GENERADOR de fixtures (defecto del banco, ver abajo); (2) `JoinCarWithRoadSystemGotoCoors` con 4096 diferencias porque `JoinCarWithRoadSystem`, que es a la vez función probada y colaborador,
tiene su hook apagado fuera de su propia prueba (`collaborators_dll` falso en ambos lados; defecto del banco).

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CCarCtrl::PickNextNodeAccordingStrategy` | 0x432B10 | Simple (`RH_ScopedInstall`) | 4096 | `FindPlayerCoors` 0x56E010 (3 llamadas con resultados distintos), `FindPlayerVehicle` 0x56E0D0 (ambos con hook) y las envolturas `PickNextNodeToChaseCar` 0x426EF0, `PickNextNodeToFollowPath` 0x427740 y `PickNextNodeRandomly` 0x42DE80 (sin hook) |
| `CCarCtrl::JoinCarWithRoadSystem` | 0x42F5A0 | Simple | 4096 | `CPathFind::FindNodeClosestToCoorsFavourDirection` 0x44FCE0 (con hook; devolución por puntero oculto) y `FindLinksToGoWithTheseNodes` 0x42B470 (envoltura `plugin::Call`, sin hook) |
| `CCarCtrl::JoinCarWithRoadSystemGotoCoors` | 0x42F870 | Simple | 4096 | `CPathFind::DoPathSearch` 0x4515D0 (con hook; 19 dwords de pila), `RemoveBadStartNode` 0x44E4F0 (sin reconstruir, por dirección), `CAutoPilot::RemoveOnePathNode` 0x41B950 (con hook; el falso copia el vector como el original), `FindLinksToGoWithTheseNodes` 0x42B470 y `JoinCarWithRoadSystem` 0x42F5A0 (esta última con el hook apagado y el cuerpo de la DLL parcheado) |

```
bash reversing/_handoff/bench_here.sh CarCtrlRest63C           # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/CarCtrlRest63C/build_probe.py
python -B reversing/CarCtrlRest63C/run_local.py
python -B reversing/CarCtrlRest63C/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -B reversing/CarCtrlRest63C/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63c/20261020-205957-788299"   # sólo lo ejecuta el coordinador
```
Filas de `docs/hooks.csv` (las añade el coordinador; todas tipo S): `CCarCtrl,<función>,<dirección>,1,0,1,S` para las tres. `SCOPE_NOTE` de `register_hook.py` ya está rellenado. Archivo de arranque en el clon: `carctrlrest63c-runtime.start`.

## Fuente (`source.patch`: sólo los cambios de la tanda 3 respecto a la tanda 2; aplicar tras `CarCtrlRest63/source.patch` y `CarCtrlRest63B/source.patch`)

- `CarCtrl.cpp`: las tres funciones en C++ (antes `plugin::Call*`) y sus tres líneas de hook. Los dos ayudantes x87 (`Cc63cLinkDistanceCpp`, `Cc63cDotNegativeCpp`) están al final de `X87Math.cpp` (prefijo `Cc63c`), pero **declarados en `CarCtrl.cpp`**, no en `X87Math.h`:
  incluir `X87Math.h` desde cientos de archivos obliga a recompilar todo el proyecto en cada cambio. Al fusionar conviene moverlos a `X87Math.h` (una sola vez, al final).
- `RemoveBadStartNode` (0x44E4F0, `void(CPathFind*, CVector, CNodeAddress*, int16*)`) no está reconstruida: se llama por dirección con `plugin::CallMethod`.

## Revisión contra el EXE (re-desensamblado)

**PickNextNodeAccordingStrategy (0x432B10..0x432C50).** Del nodo `m_startingRouteNode` (+0x394, sin comprobar que el área esté cargada) se leen los bytes +0x19 y +0x1A: `+0x3D1 = (byte19 >> 4) & 3` y, si `byte1A >> 4` está en 8..9, `+0x3D1 = 0xFF`. Misión = `movsx byte [+0x3BA]`; `misión - 2` sin signo
`<= 0x42` y tabla de bytes de 0x432C64 (leída del EXE): misiones 2, 4 y 43 -> jugador (FindPlayerCoors, FindPlayerVehicle(-1, 0), FindPlayerCoors, FindPlayerCoors y `PickNextNodeToChaseCar(veh, x de la 1.ª, y de la 3.ª, z de la 2.ª)`; el vehículo del jugador
queda como dword sobrante en la pila, que la función llamada ignora); 15, 17, 52, 54, 61 y 65..68 -> posición del objetivo `+0x41C` (sin comprobar nulo; matriz+0x30 si hay matriz, si no +4) con el mismo `PickNextNodeToChaseCar` (objetivo como dword sobrante);
8 y 12 -> `PickNextNodeToFollowPath`; el resto -> `PickNextNodeRandomly` y `false` (`xor al,al`). El valor de retorno es el del colaborador (AL).

**JoinCarWithRoadSystem (0x42F5A0..0x42F86B).** Escribe 0xFFFF en las áreas de +0x398, +0x390, +0x394 y en los enlaces +0x3A4, +0x3A8, +0x3A6 y 0 en el contador +0x418 antes de nada; lee adelante `[matriz+0x10/+0x14]` y la posición (matriz+0x30; el EXE desreferencia la matriz sin comprobarla)
y pide `FindNodeClosestToCoorsFavourDirection(pos, 0, adelante)`. Área 0xFFFF, área sin cargar o 0 enlaces (`byte[+0x18] & 0xF`, comparación con signo de 16 bits) -> sale. Recorre los enlaces (`m_pNodeLinks[área]`, base `int16 [nodo+0x10]`); los de área sin cargar se saltan; elige el de menor distancia
`sqrt((X2 - X1)² + (Y2 - Y1)²)` (x/y = int16/8; sumas en ese orden; comparación ordenada `<` contra 1000000.0f = 0x497423FE y luego contra el mejor); si no hay ninguno sale. Con adelante.x == 0 y adelante.y == 0 el x pasa a 1.0; el producto escalar `(X1 - X3)·adelante.x + (Y1 - Y3)·adelante.y`
(24 bits por operación) < 0 deja `actual = inicial, siguiente = enlace`; si no `actual = enlace, siguiente = inicial`; después `+0x398 = 0xFFFF` (área), `FindLinksToGoWithTheseNodes` y carriles `+0x3B7 = +0x3B8 = 0`.

**JoinCarWithRoadSystemGotoCoors (0x42F870..0x42F9B6).** Copia (bits) el destino a +0x3EC; `DoPathSearch(tipo 0, posición del vehículo, [0x8A5F44] (dirección inválida), destino, ruta (+0x3F8), contador (+0x418), 8, sin distancia, 1000000.0f, sin nodo de destino, 1000000.0f,
bit 6 de +0x3DB, [0x8A5F44], modelo == 539 (0x21B), bIsBoat)` (el EXE empuja el dword de `bIsBoat` tal cual y reutiliza sus bits altos en el argumento anterior: invisible para el colaborador, que sólo lee el byte); `RemoveBadStartNode` con la posición releída;
contador (releído, con signo) `< 2` -> `JoinCarWithRoadSystem`, contador = 0, `true`; si no `actual = ruta[0]`, área final +0x398 = 0xFFFF, `RemoveOnePathNode`, `siguiente = ruta[0]`, `RemoveOnePathNode`, `FindLinksToGoWithTheseNodes`, carriles a 0 y `false`.

Llamadores y registros: ningún llamador del EXE lee EAX/ECX/EDX de salida más allá de AL de las dos funciones bool (que el banco compara); las rutas de `residue_scan.py` de los llamadores de `JoinCarWithRoadSystem` no leen nada.

## Defectos hallados

- **Banco (corregido):** (1) `MakeC0` (misión de las familias 3/10) hacía un `do { ... } while (misión excluida)` sin avanzar la semilla: bucle infinito del generador (el juego queda «no responde» en el caso 26). (2) `JoinCarWithRoadSystem` como colaborador de `JoinCarWithRoadSystemGotoCoors`
  debía registrarse con `hooked=false` y el cuerpo de la DLL parcheado (el banco apaga su hook fuera de su propia llamada).
- Producción: ninguna discrepancia hallada; las tres funciones coincidieron a la primera con el EXE.

## Suposiciones y riesgos

- `PickNextNodeToChaseCar`, `PickNextNodeToFollowPath` y `PickNextNodeRandomly` siguen siendo envolturas hacia el EXE en este árbol; `FindLinksToGoWithTheseNodes` la reconstruye la tanda 4 (`CarCtrlRest63D`), pero este banco se aprobó con ella como envoltura (falso en la dirección del EXE). Si alguna se reconstruye después, estas funciones la llamarán directamente en C++ y habrá que repetir el banco con el cuerpo de la DLL parcheado.
- `CNodeAddress` (4 bytes) se devuelve de `FindNodeClosestToCoorsFavourDirection` por puntero oculto en ambos lados (el hook de producción ya lo hace así); si el compilador cambiara la ABI de retorno el banco fallaría por la pila.
- Áreas con índice >= 72 o 0xFFFF en enlaces no se prueban (el EXE leería fuera de la tabla).
