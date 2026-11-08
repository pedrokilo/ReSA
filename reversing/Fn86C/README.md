<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 07/11/2026 (Ciudad de México):** 865 grupos / 9341 funciones reconstruidas; 9341 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn86C: `CPathFind::GeneratePedCreationCoors` (lote 86C)

Estado: **APROBADO dentro del juego** (03/11/2026, arbol principal sobre HEAD 90a47919c + los cambios del lote). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauracion en ambos lados, y cada lado coincide con el oraculo independiente (`fixture_oracle.py`). Una funcion de codigo de juego reconstruida en C++ puro y enganchada (tipo S).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn86c/20261103-020848-414794` (archivada en `reversing/CppTwelveLevel6Final/runs/`).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `CPathFind::GeneratePedCreationCoors` (antes la llamada `plugin::CallMethodAndReturn<bool,0x44E790>` de `Population.cpp`) | 0x44E790 | `CPathFind` / `GeneratePedCreationCoors` / S | `PathFind.cpp` |

La llamada del EXE desaparece: `CPopulation::AddToPopulation` (0x614B1C) llama al metodo `ThePaths.GeneratePedCreationCoors(...)`. Es un metodo real de `CPathFind` (thiscall, 12 argumentos en pila, `ret 30h`, devuelve AL).
Mide 0x50E B (0x44E790..0x44EC9C) y su unico llamador en el EXE es `CPopulation::AddToPopulation`. La declaracion de `PathFind.h` pasa de `void ... CMatrix*` a `bool ... RwMatrix*`: el EXE reenvia la matriz tal cual a `CCamera::IsSphereVisible`.

## Revision contra el EXE (0x44E790..0x44EC9C)
- Argumentos: x, y, minDist1, maxDist1, minDist2, maxDist2, salida de coordenadas, dos direcciones de nodo, salida de rumbo, byte de trafico bajo y matriz.
- Umbral de trafico: `ftol(float(rand() & 0xFFFF) * 2^-15 * 15.0f)` (0x858B14, 0x858B48; `_ftol2`: 32 bits bajos del entero de 64 bits truncado) y `maxReach = maxDist1 + 30.0f` (0x858CA4), `maxReachSq = maxReach * maxReach`.
- Area = `FindRegionForCoors(x, y)` (0x44D830), la misma para las 300 iteraciones. Sin nodos cargados o sin nodos de peaton se pasa a la siguiente iteracion.
- Nodo = `nVeh + (rand() >> 6) % nPed` (0x1C bytes); posicion = int16 * 0.125 (0x858C48); se exige `(y - py)^2 + (x - px)^2 < maxReachSq` (comparacion ordenada), trafico (byte +0x1A & 0xF) mayor que el umbral con signo y enlaces (byte +0x18 & 0xF) distintos de cero.
- Por enlace (base = int16 +0x10): se salta si el byte de `m_pPathIntersections[area][base + i]` tiene el bit 0 (`m_bRoadCross`); destino = `m_pNodeLinks[area][base + i]`: area >= 0x40 o sin cargar se salta; si el nodo origen o el destino llevan el bit 0x20 del byte +0x18 (apagado) hace falta que el byte bajo de `bLowTraffic` sea distinto de cero; trafico del destino mayor que el umbral; se sigue si `d1 < maxDist1` (d1 = distancia del nodo origen) o `sqrt(suma2) < maxDist1` (suma2 = distancia al cuadrado del destino).
- Hasta 5 intentos por enlace: `t = float(rand() & 0xFF) * 2^-8` (0x859AA0) se guarda en `*outOrientation` en CADA intento (no es una orientacion); X = `(1 - t) * xo + float(t * xd)`, Y igual, Z = `float((1 - t) * zo) + float(t * zd)`; dist = `sqrt((Y - py)^2 + (X - px)^2)`.
- `TheCamera.IsSphereVisible((X, Y, Z), 2.0f, matriz)` (0x420C40) con matriz no nula o `IsSphereVisible((X, Y, Z), 2.0f)` (0x420D40) sin ella: visible exige `minDist1 < dist < maxDist1`; no visible exige `minDist2 < dist < maxDist2` y `rand() & 1` (la tirada solo se hace si pasan las dos comparaciones).
- Si se acepta: `*outAddress1` / `*outAddress2` = dword +0x12 del origen y del destino, `*outCoords` = (X, Y, Z) y `CWorld::FindGroundZFor3DCoord(X, Y, Z + 2.0f, &encontrado, 0)` (0x5696C0): sin suelo se sigue con el intento siguiente; con suelo, `|suelo - Z| > 3.0f` (0x858B3C, ordenada: con NaN no se rechaza) devuelve **falso ya** (no se sigue buscando) y si no `outCoords->z = suelo` y devuelve verdadero. Tras 300 iteraciones devuelve falso.
- Las cuatro lecturas `CPathNode::GetPosition` (0x420A10) del nodo destino del EXE dan el mismo valor; la DLL lee una vez.

## Banco
Hook `CPathFind` / `GeneratePedCreationCoors` (S) apagado en el lado EXE y encendido en el lado DLL, 16384 pares. El objeto `this` es una arena propia (nodos, enlaces e intersecciones de hasta 3 areas con guardas; nunca `ThePaths`).
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: rand() del CRT del EXE 0x821B1E (cdecl, sin hook: la DLL lo llama por su direccion), `CWorld::FindGroundZFor3DCoord` 0x5696C0 (cdecl, ST0), `CCamera::IsSphereVisible` 0x420C40 (thiscall, `ret 0xC`) y 0x420D40 (thiscall, `ret 8`).
`FindRegionForCoors` (0x44D830) y `CPathNode::GetPosition` (0x420A10) son codigo real (con hook en la DLL, ambos lados usan el de la DLL).
Se observa: AL, las salidas (coordenadas, rumbo, direcciones de nodo), los cambios de las arenas de salida y del mundo, y los eventos de los falsos con todos sus argumentos (los 24 primeros y un hash de todos).
16 familias de 1024 casos (tipico, matriz no nula, siempre visible, nunca visible, borde del suelo, sin suelo, trafico bajo, cruces de calle, enlaces invalidos y base negativa, valores especiales, bordes exactos de las ventanas, umbrales de trafico, sin nodos, mezcla).
`coverage.json` = 80 % de lo medido sobre el modelo (minimo 100 en las ramas exigidas, ver `REQUIRED` de `run_local.py`).

## Fuera de los fixtures
Argumentos NaN senalizadores o con cargas distintas de 0xFFC00000, bytes altos de `bLowTraffic` fuera de los generados, matrices de transformacion reales (la matriz es una arena; solo se compara el puntero), nodos de enlaces fuera de las arenas
(areas cargadas con nodo fuera de rango), salidas nulas (el EXE escribe en `*outOrientation`, `*outAddress1` y `*outAddress2` sin comprobarlos) y estados de la x87 distintos de 24 bits.

## Prueba negativa
Produccion mutada en siete puntos: `maxDist1 + 30.0f` a `+ 31.0f`, la raiz de `nodeDist` (que el EXE reaplica sobre el valor anterior en cada enlace) leida siempre de `distSq`, `>` a `>=` en el trafico del nodo origen, `<` a `<=` en la ventana visible, `> 3.0f` a `>= 3.0f` en el suelo, `rand() & 1` a `rand() & 2` y 4 en vez de 5 intentos por enlace. El banco falla en 5005 de 16384 pares, sin accesos invalidos (`resultados-fn86c/20261103-020523-400038`);
en esos 5005 el EXE coincide con el oraculo y la DLL no. Restaure desde copia y comprobe con `cmp` y con el hash (`d9de430daafc18fd`) antes de la corrida final.

## Bancos repetidos
`PopulationPending` (cita 0x44E790 en `AddFixture.hpp`: el falso solo estaba en la direccion del EXE y ahora la DLL llama al metodo): adaptado (`spawnC` falsea tambien el cuerpo de la DLL de `GeneratePedCreationCoors`) y **APROBADO**, 2048 pares (`resultados-population-pending/20261103-022441-499305`).
Para repetirlo hubo que adaptar ademas dos colaboradores ya convertidos que el banco antiguo solo falseaba en la direccion del EXE: `CPopulation::RandomIntInLimitRange` (0x610DB0) y `CScriptsForBrains::CheckIfNewEntityNeedsScript` (0x46FF20); sin esos dos el banco fallaba 256/1024 en `AddToPopulation` (64 casos de pandilla y 192 con el evento `brain`), independientemente de este lote.
Ningun otro banco cita 0x44E790 ni el simbolo.

## Hallazgo del banco
El EXE guarda la raiz `sqrt(distSq)` de la distancia del nodo origen en el MISMO float de la pila (`[esp+20h]`) donde guardo la suma de cuadrados, y la calcula dentro del bucle de enlaces: cada enlace que llega a esa raiz la reaplica sobre el valor anterior
(`d1 = sqrt(suma)`, luego `sqrt(d1)`, ...). El primer borrador (y la DLL) calculaban una sola raiz; el banco lo detecto en 372 de 16384 pares (EXE y oraculo coincidian entre si tras corregir el modelo; el oraculo corregido coincide con el EXE en los 16384 casos) y se corrigio en produccion y en el oraculo.
