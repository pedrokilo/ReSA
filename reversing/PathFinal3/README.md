<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PathFinal3: banco por pares de CPathFind (54 funciones pendientes)

Banco simple (flujo viejo, copiado de `WorldFinal1`/`GeoFinal1`/`PadMouse1`) para las 54 funciones de `CPathFind` con `validated=0` en `docs/hooks.csv`
(`ComputeRoute` y `DoPathSearch` ya estaban validadas y no se tocan). **Escrito sin compilar ni ejecutar**: pendiente de la primera corrida.

Uso: `bash reversing/_handoff/bench.sh PathFinal3 3`. Filtro opcional: `pathfinal3-only.txt` en la carpeta del juego con `CPathFind::Funcion`.

Ficheros: `LiveComparison.hpp` (tabla, comprobacion de disposicion, bucle de pares), `PathWorld.hpp` (utilidades, memoria falsa, dobles trazados,
constructor de mundos), `PathCases.hpp` (un caso por funcion), `build_probe.py`, `run_local.py` (ambos redirigen `runtime_runner.GAME` a la copia de `E:`).

## Corpus: 26880 pares

| Funcion | Direccion EXE | Pares |
|---|---|---|
| `CPathFind::AddDynamicLinkBetween2Nodes_For1Node` | `0x44E000` | 1024 |
| `CPathFind::AddInteriorLink` | `0x44DED0` | 512 |
| `CPathFind::AddInteriorLinkToExternalNode` | `0x44DF30` | 256 |
| `CPathFind::AddNodeToList` | `0x44D1E0` | 512 |
| `CPathFind::AddNodeToNewInterior` | `0x450E90` | 512 |
| `CPathFind::AreNodesLoadedForArea` | `0x44DD10` | 1024 |
| `CPathFind::CountNeighboursToBeSwitchedOff` | `0x4504F0` | 512 |
| `CPathFind::FindLinkBetweenNodes` | `0x451350` | 512 |
| `CPathFind::FindNearestExteriorNodeToInteriorNode` | `0x450F30` | 256 |
| `CPathFind::FindNodeClosestToCoorsFavourDirection` | `0x44FCE0` | 1024 |
| `CPathFind::FindNodeCoorsForScript-LinkedNode` | `0x4505E0` | 1024 |
| `CPathFind::FindNodeCoorsForScript-TwoNodes` | `0x450780` | 1024 |
| `CPathFind::FindRegionForCoors` | `0x44D830` | 1024 |
| `CPathFind::FindStartPointOfRegion-` | `0x44D930` | 256 |
| `CPathFind::FindXCoorsForRegion` | `0x44D8F0` | 128 |
| `CPathFind::FindXRegionForCoors` | `0x44D890` | 1024 |
| `CPathFind::FindYCoorsForRegion` | `0x44D910` | 128 |
| `CPathFind::FindYRegionForCoors` | `0x44D8C0` | 1024 |
| `CPathFind::GetPathNode` | `0x420AC0` | 256 |
| `CPathFind::HaveRequestedNodesBeenLoaded` | `0x450DB0` | 256 |
| `CPathFind::Init` | `0x44D080` | 128 |
| `CPathFind::IsAreaNodesAvailable` | `0x420AA0` | 256 |
| `CPathFind::IsWaterNodeNearby` | `0x450DE0` | 768 |
| `CPathFind::Load` | `0x5D3500` | 256 |
| `CPathFind::LoadPathFindData-Area` | `0x452F40` | 512 |
| `CPathFind::LoadPathFindData-FromStream` | `0x4529F0` | 512 |
| `CPathFind::LoadSceneForPathNodes` | `0x44DE00` | 256 |
| `CPathFind::MakeRequestForNodesToBeLoaded` | `0x450D70` | 512 |
| `CPathFind::MarkRegionsForCoors` | `0x44DB60` | 1024 |
| `CPathFind::MarkRoadNodeAsDontWander` | `0x450560` | 256 |
| `CPathFind::ReInit` | `0x44E4E0` | 64 |
| `CPathFind::ReleaseRequestedNodes` | `0x44DD00` | 64 |
| `CPathFind::RemoveInterior` | `0x44E1A0` | 768 |
| `CPathFind::RemoveNodeFromList` | `0x44D1B0` | 256 |
| `CPathFind::ReturnInteriorNodeIndex` | `0x451300` | 256 |
| `CPathFind::Save` | `0x5D34C0` | 256 |
| `CPathFind::SetLinksBridgeLights` | `0x44D960` | 512 |
| `CPathFind::SetPathsNeededAtPosition` | `0x44DCD0` | 128 |
| `CPathFind::Shutdown` | `0x450950` | 128 |
| `CPathFind::StartNewInterior` | `0x44DE80` | 256 |
| `CPathFind::SwitchOffNodeAndNeighbours` | `0x452160` | 1024 |
| `CPathFind::SwitchPedRoadsOffInArea` | `0x452F00` | 512 |
| `CPathFind::SwitchRoadsOffInArea` | `0x452C80` | 768 |
| `CPathFind::SwitchRoadsOffInAreaForOneRegion` | `0x452820` | 768 |
| `CPathFind::TakeWidthIntoAccountForWandering` | `0x4509A0` | 512 |
| `CPathFind::TestCrossesRoad` | `0x44D790` | 512 |
| `CPathFind::TestForPedTrafficLight` | `0x44D480` | 512 |
| `CPathFind::These2NodesAreAdjacent` | `0x44D230` | 512 |
| `CPathFind::ThisNodeHasToBeSwitchedOff` | `0x44D3E0` | 128 |
| `CPathFind::ThisNodeWillLeadIntoADeadEnd` | `0x44D310` | 512 |
| `CPathFind::TidyUpNodeSwitchesAfterMission` | `0x44D3B0` | 128 |
| `CPathFind::UnLoadPathFindData` | `0x44D0F0` | 256 |
| `CPathFind::UnMarkAllRoadNodesAsDontWander` | `0x44D400` | 256 |
| `CPathFind::UpdateStreaming` | `0x450A60` | 1024 |

## Que se compara (original = hook desactivado, candidato = hook activo)

Para cada par se reconstruye el mismo fixture desde la misma semilla en las dos ejecuciones y se compara: valores de retorno y salidas por puntero (los flotantes
por sus bits), el hash de `ThePaths` completo (0x96F050, 0x3C80 bytes), de las globales vecinas (0x96EA98..0x96F050: `ConnectsToGiven`, `X/Y/ZCoorGiven`,
`ToBeStreamed`, `ToBeStreamedForScript`, `NumNodesGiven`, `NewInteriorSlot`...), de `s_pathsNeededPosn` (0x977B70), `aExteriorNodeLinkedTo` (0x977B7C), `gString`,
del arena de areas falsas y del arena de `CMemoryMgr::Malloc`, la traza de todas las llamadas salientes con sus argumentos, el control x87 y que el hook conmute
(`JumpsToDll` despues de `State()`). Cada registro `comparison` aceptado lleva `original` y `candidate`; los fallidos incluyen `blob_diff` (primeros bytes distintos
por segmento). x87 a 24 bits (`_controlfp(_PC_24)`) en cada ejecucion.

**Datos reales intactos.** `ThePaths` y las globales vecinas se guardan con `Snap` antes de empezar, cada ejecucion las sobrescribe con contenido determinista y
se restauran con `RestoreRegions()` despues de cada ejecucion y al final (`state_restored`). Los nodos, enlaces, longitudes e intersecciones de las pruebas viven en un
arena propio del banco (nunca se llama a funciones que lean las areas reales; las tablas de punteros de `ThePaths` apuntan al arena durante la ejecucion). El pool de
vehiculos (0xB74494) se sustituye por un pool falso de 6 vehiculos de 0xA18 bytes y se restaura. `CTimer` (0xB7CB78/0xB7CB84) tambien se guarda y restaura.

**Carga de ficheros.** `LoadPathFindData` no abre ningun `nodes%d.dat` real: `RwStreamOpen/Read/Close` (0x7ECEF0/0x7EC9D0/0x7ECE20) son dobles que sirven un bufer en
memoria con el formato del fichero (5 contadores, nodos, `CCarPathLink`, enlaces + 192 dinamicos, enlaces de navegacion, longitudes e intersecciones) y
`CMemoryMgr::Malloc/Free` (0x72F420/0x72F430) devuelven memoria del arena del banco. Con `CTimer::Suspend/Resume` y `sprintf` real se prueba tambien la variante `-Area`
(se compara el nombre de fichero pedido a `RwStreamOpen` y el contenido completo de `gString`).

## Colaboradores simulados y trazados (parche en la direccion del EXE y en el cuerpo de la DLL, solo durante cada ejecucion)

`CMemoryMgr::Malloc/Free` (los hilos ajenos se reenvian al CRT 0x824257/0x82413F), `RwStreamOpen/Read/Close`, `CTimer::Suspend/Resume` (0x5619D0/0x561A00),
`CPathFind::FindNodeClosestToCoors` (0x44F460, thiscall con retorno oculto, `ret 2Ch`; devuelve la direccion programada), `CStreaming::RequestModel/RemoveModel`
(0x4087E0/0x4089A0), `FindPlayerPed/FindPlayerCoors` (0x56E210/0x56E010), `CGenericGameStorage::SaveDataToWorkBuffer/LoadDataFromWorkBuffer` (0x5D1270/0x5D1300).
Todos los dobles solo trazan en el hilo del banco (`owner`/`Mine()`). Los cuerpos de la DLL se calculan antes de crear ningun `EntryPatch` (`Body()` sigue los `E9`).
Las llamadas anidadas a otras funciones de `CPathFind` (p. ej. `SwitchOffNodeAndNeighbours` desde `ForOneRegion`) usan el hook en su estado inicial (activo); cada una se
valida por separado.

## Dominio de los fixtures y exclusiones documentadas

- Indices de area 0..63 (mapa) y 64..71 (interiores); direcciones de nodo siempre validas cuando la funcion las dereferencia; areas sin cargar generadas con ranura nula.
- `SwitchOffNodeAndNeighbours` y los que la llaman (`ForOneRegion`, `SwitchRoadsOffInArea`, `SwitchPedRoadsOffInArea`, `LoadPathFindData`): el original escribe en
  `*outNext2` sin comprobar nulo (0x452248/0x452250), por lo que los grafos de esos fixtures son bidireccionales (a lo sumo un vecino candidato) o con <= 1 enlace por nodo.
  En la DLL ese caso esta protegido por `FIX_BUGS` (`else if (outNext2)`).
- `SwitchOffNodeAndNeighbours`: durante sus dos ejecuciones el hook de `CountNeighboursToBeSwitchedOff` se desactiva. El original conserva ECX (0x4521D0 sigue
  usando `ecx` tras `call 0x4504F0` dentro del bucle); la version de la DLL no, de modo que el original mezclado con el hook activo no es ejecutable.
  Se restaura el hook despues de cada funcion. Recomendacion: si se desactiva solo `SwitchOffNodeAndNeighbours` en el juego, bloquear tambien `CountNeighbours...`.
- `m_wAreaId/m_wNodeId` de los nodos se aleatorizan solo donde el original usa la direccion recibida y no el campo del nodo (`These2NodesAreAdjacent`, `TestCrossesRoad`,
  `TestForPedTrafficLight`, `FindLinkBetweenNodes`, `FindNodeCoorsForScript` x2, `TakeWidth...`, `AddDynamicLink...`, `RemoveInterior`, `FindNodeClosestToCoorsFavourDirection`);
  donde el original indexa con el campo del nodo (`CountNeighbours`, `SwitchOff...`, `ForOneRegion`, `ThisNodeWillLeadIntoADeadEnd`) se mantienen coherentes.
- `FindLinkBetweenNodes`: el original busca sin limite de `m_nNumLinks` (0x4513B8..0x4513C7); los fixtures solo piden enlaces existentes o direcciones con area sin cargar.
  La DLL limita la busqueda a los enlaces del nodo (solo cambia el caso no valido).
- `FindNodeClosestToCoorsFavourDirection`: si no se encuentra ningun nodo el original devuelve en la mitad alta ([esp+1Ah], solo escrito en 0x44FE9B al aceptar un nodo)
  basura de pila; el banco normaliza esa mitad cuando el area es 0xFFFF y lo documenta aqui.
- `RemoveInterior` reproduce un error de R* (0x44E272..0x44E27F): el desplazamiento de los enlaces restantes indexa `this + 0xA44 + (base+k)*4` (el array de punteros
  `m_pNodeLinks`) en vez del array de enlaces del area. El fixture limita `base+k` a 4..56 y rellena los punteros no usados con valores reconocibles para que la
  corrupcion sea observable y no toque areas cargadas.
- `AddInteriorLink` incluye filas llenas (el original escribe en `row[6]`, el primer byte de la fila siguiente, 0x44DEE6..0x44DEF7).
- Sin sNaN en los fixtures (el x87 los silencia al copiar y la DLL no). `NaN` silencioso solo en `SetPathsNeededAtPosition` (copia entera en ambos).
- Aserciones de la DLL (`GetPathNode`, `LoadPathFindData`, `GetCarPathLink`) y `NOTSA_LOG_*` quedan fuera de las ramas probadas: los fixtures no violan sus condiciones.
- `UpdateStreaming`/`MakeRequestForNodesToBeLoaded` leen `FindPlayerPed` real solo a traves del doble; el pool de vehiculos y los tiempos son falsos.
- `NumTempExternalNodes` (0x96F02C) y el resto de globales de la clase entran en el hash de globales.

## Correcciones de la fuente (`source/game_sa/PathFind.cpp/.h`) hechas antes del banco

Cada una con la evidencia exacta del EXE:

- `Init` 0x44D080 (`mov [0096F02Ch],edx`): `NumTempExternalNodes` era un `static` local; ahora es la global 0x96F02C.
- `ThisNodeWillLeadIntoADeadEnd` 0x44D310 (`0x44D36B..0x44D378`: `shr dl,4; test dl,dl; jbe; cmp dl,0Ah; jbe`): cuenta el comportamiento 0 y > 10 (la DLL decia `== 4`).
- `TidyUpNodeSwitchesAfterMission` 0x44D3B0 (`cmp eax,36h; jl`): minimo con signo.
- `AddNodeToList` 0x44D1E0 (`and eax,1FFh; movsx eax,ax`): cubo `dist & 511` (con `%` una distancia negativa daba indice negativo); trunca a 16 bits sin aserto.
- `FindXRegionForCoors`/`FindYRegionForCoors`/`FindRegionForCoors` 0x44D890/0x44D8C0/0x44D830 (`fsub [859A90h]` = -3000, `fmul [859A8Ch]` = 0x3AAEC33E, `call 0x821B40`, `setle`
  + `dec` + `and`, `cmp 7`): `(p+3000) * 0.0013333333f` truncado y acotado con signo a [0,7]; la DLL dividia por 750 y convertia a `uint32`
  (coordenadas < -3000 daban 7).
- `MarkRegionsForCoors` 0x44DB60 (`lea edx,[ebx+ebp*8+96EFD0h]`, `rep stos`): `ToBeStreamed` se rellena por columnas (`[x*8+y]`) con cuatro regiones sueltas; la DLL usaba `y*8+x`.
- `LoadSceneForPathNodes` 0x44DE00 (0x44DE48..0x44DE5D): recorre x por fuera e y por dentro, modelo `0x63A7 + x + y*8`, con el indice por columnas (orden de `RequestModel` distinto).
- `UpdateStreaming` 0x450A60: `ToBeStreamedForScript` es la global 0x96EF90 (se ponia a cero en `0x450AA7`; la DLL usaba un local), pool de vehiculos de la ultima ranura a la primera
  (0x450B40), rectangulo pedido con `rep stos` por columnas en ambos arrays (0x450CA5/0x450CBF) y bucle final x/y con modelo `x + y*8` (0x450D00..0x450D4D); sin avisos de depuracion.
- `AreNodesLoadedForArea` 0x44DD10: sin `CRect` (la DLL abortaba con limites invertidos), mismas regiones sueltas.
- `SetLinksBridgeLights` 0x44D960 (`fcom [esp+14h]; test ah,41h; jne`, `fcomp [esp+18h]; test ah,5; jp`): limites **estrictos** (x > xMin && x < xMax, idem y), sin `CRect`
  (inclusivo) ni `CCarPathLinkAddress` (truncaba el indice a 10 bits).
- `SwitchRoadsOffInAreaForOneRegion` 0x452820 / `SwitchRoadsOffInArea` 0x452C80: comparaciones ordenadas como el asm (un NaN excluye); copia de 0x1C bytes completos (`rep movs`) en el desplazamiento.
- `SwitchOffNodeAndNeighbours` 0x452160 (0x4521F4..0x452239): con `bBackToOriginal` se salta el vecino con `m_isSwitchedOff == m_isSwitchedOffOriginal` (la DLL lo ignoraba) y
  `CountNeighbours` se calcula sobre el **vecino** (`push esi; call 0x4504F0`), no sobre el nodo.
- `TakeWidthIntoAccountForWandering` 0x4509A0 (`and edx,0Fh`, `sar eax,4; and eax,0Fh`, `fmul [859A9Ch]`): `& 0xF` y `>> 4` con signo en vez de `%`/`/` de C++; solo comprueba el area (`cmp ax,0FFFFh`).
- `FindNodeCoorsForScript` (nodo enlazado) 0x4505E0: el primer enlace se lee de `m_pNodeLinks[area de la direccion][base]` (la DLL indexaba `m_pNodeLinks[base]`, el array de punteros);
  normalizacion de 2 componentes (0x44E480), `x < 0` invierte el vector y perpendicular izquierdo; escala `ancho*0.0625 + 2.7` (0x402CCCCD).
- `FindNodeCoorsForScript` (dos nodos) 0x450780: rumbo = `fpatan(-dx, dy) * 57.29578` (0x42652EE0) con la direccion **sin normalizar** y un solo redondeo (asm); el vector se
  normaliza con `CVector::Normalise` de **3 componentes** (0x59C910) usando la z del nodo A (resto de pila de `GetPosition`) y el desplazamiento es el perpendicular derecho
  `(dir.y, -dir.x)` sin invertir (la DLL normalizaba en 2D y usaba el perpendicular izquierdo).
- `IsWaterNodeNearby` 0x450DE0 (0x450E1E..0x450E58): distancia **2D** `sqrt(dy*dy+dx*dx) < radius` estricta; la DLL comparaba el cuadrado 3D con `<=`.
- `MarkRoadNodeAsDontWander` 0x450560 (`cmp ax,0FFFFh`): solo comprueba el area.
- `AddNodeToNewInterior` 0x450E90 (`mov [ecx+0096EC40h],al`, `lea eax,[ecx+ecx*2]; shl eax,1`): guarda `bDontWander` en el array de bytes 0x96EC40 (se ignoraba) y `ConnectsToGiven` tiene
  filas de 6 bytes (la DLL declaraba 8, solapando `ZCoorGiven`).
- `AddInteriorLink` 0x44DED0: filas de 6 bytes, primer byte negativo con signo, escritura sin limite en `row[6]` (la DLL buscaba `== -1` en filas de 8).
- `FindNearestExteriorNodeToInteriorNode` 0x450F30 (`mov ecx,96F050h`): la busqueda se hace sobre `ThePaths`.
- `ReturnInteriorNodeIndex` 0x451300 (`ret 0Ch`): sin el parametro `unkn` (la DLL descuadraba la pila); sin coincidencia devuelve area -1 y en el nodo la mitad alta del identificador (0x451321).
- `AddDynamicLinkBetween2Nodes_For1Node` 0x44E000 (0x44E043: `cmp word [esi],0FFFFh; je`): busca el primer bloque libre (area -1) en lugar del primero con enlace valido;
  comparaciones con signo (`jge`, `cmp [esi],0; jl`).
- `These2NodesAreAdjacent` 0x44D230 / `FindLinkBetweenNodes` 0x451350: el array de enlaces es el del area de la **direccion**, no el campo del nodo.
- `RemoveInterior` 0x44E1A0: reescrita: solo las 64 areas del mapa y sus nodos de peaton (0x44E1F1), el id se libera antes del recorrido, desplazamiento erroneo de R* (ver arriba),
  anulacion del area (word bajo) de los 12 enlaces, registro de enlaces dinamicos de la propia area, y liberacion de nodos/enlaces/longitudes/intersecciones con
  `CMemoryMgr::Free` sin comprobar nulos (la DLL liberaba dos veces las longitudes y nunca los enlaces).
- `LoadPathFindData(stream, area)` 0x4529F0 / `UnLoadPathFindData` 0x44D0F0: `CMemoryMgr::Malloc/Free` (0x72F420/0x72F430) en lugar de `new[]/delete[]` con constructores;
  al final se reinician solo los 16 enlaces dinamicos de **esa** area (0x452C4B: `shl edi,6`; la DLL recorria las filas 0..15).
- `LoadPathFindData(area)` 0x452F40: `sprintf` del CRT (no `sprintf_s`, que rellena con 0xFE) y `RwStreamClose(stream, 0)` (0x452F7D) que faltaba.
- `FindNodeClosestToCoorsFavourDirection` 0x44FCE0: reescrita con las operaciones x87 en orden (`((|dz|*3 + |dy|) + |dx|)`, normalizacion con `x = 1.0` si la longitud es 0,
  comparaciones `fcomp + test ah,5 + jp` estrictas), mejor puntuacion inicial 10000.0 (0x461C4000, no FLT_MAX) y direccion `{area, indice de recorrido}`, no `m_wNodeId`.

Funciones sin cambios (MATCH): `GetPathNode`, `IsAreaNodesAvailable`, `ReInit`, `ReleaseRequestedNodes`, `SetPathsNeededAtPosition`, `StartNewInterior`,
`AddInteriorLinkToExternalNode`, `RemoveNodeFromList`, `ThisNodeHasToBeSwitchedOff`, `UnMarkAllRoadNodesAsDontWander`, `TestCrossesRoad`, `TestForPedTrafficLight`,
`HaveRequestedNodesBeenLoaded`, `MakeRequestForNodesToBeLoaded`, `CountNeighboursToBeSwitchedOff`, `Shutdown`, `SwitchPedRoadsOffInArea`, `Save`, `Load`, `FindXCoorsForRegion`,
`FindYCoorsForRegion`, `FindStartPointOfRegion-`.

## Supuestos a revisar al compilar

- Las firmas de los dobles y los moldes (`__thiscall`, retorno oculto, `ret N`) estan deducidas del asm; `FindNodeClosestToCoors` (0x44F460) es `ret 2Ch` (11 dwords con el puntero de retorno).
- `Body(static_cast<...>(&CStreaming::RequestModel))`, `&FindPlayerPed`, `&CGenericGameStorage::SaveDataToWorkBuffer` etc.: la seleccion de sobrecarga puede necesitar ajuste.
- La disposicion (evento `layout`) comprueba desplazamientos de `CPathFind`, bits de `CPathNode`, `CCarPathLink::m_bridgeLights`, `CNodeAddress`, `CCarPathLinkAddress`, `CVehicle`
  (0x4A4/0x594), `sizeof(CHeli)` = 0xA18 y que el arreglo `CPathFind_SwitchRoadsOffInArea_StrayAreas` esta desactivado (por defecto lo esta).
- Tiempo: 26880 pares con dos ejecuciones cada uno (copia de unos 100 KB de estado por ejecucion); si supera `timeout=1500` bajar casos en la tabla.

## Aprobación (09/10/2026)

Corrida `20261009-195539-016084`: passed=true, 26880 pares, 54 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_pathfinal3.py` (evidencia en `runs/`, `validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.
