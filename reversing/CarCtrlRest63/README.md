<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlRest63 (tanda 1): siete funciones pequeñas de CCarCtrl

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-4`, HEAD df26d6fe7 + fuentes del lote 63 parte C). 28672 pares (7 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados, `fp_difference` vacío (el resultado en ST0 forma parte del estado x87 comparado) y, donde el modelo es trivial, salida de cada lado igual a la de un oráculo independiente (`ThisVehicleShouldTryNotToTurn`, `StopCarIfNodesAreInvalid`).
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlrest63\20261020-195518-439585`.

Esta carpeta es la tanda 1 de la parte C de CCarCtrl (lote 63). El trabajo parcial sin validar del agente interrumpido (`D:\Original\wf-8`, base 35e7b9d4b) se reutilizó entero tras contrastarlo con el desensamblado
(véase la revisión); la única corrección fue un defecto del BANCO (ver «Defectos hallados»). Las demás funciones de la parte C van en bancos propios, cada uno con su README, su `source.patch` incremental y su estado (aplicar los parches en este orden sobre HEAD; `source-cumulative.patch` de esta carpeta es la suma de las seis tandas):

| Banco | Funciones | Pares | Evidencia |
|---|---|---|---|
| `CarCtrlRest63` | ThisVehicleShouldTryNotToTurn, FindPathDirection, SwitchBetweenPhysicsAndGhost, FindSpeedMultiplier, StopCarIfNodesAreInvalid, FindPercDependingOnDistToLink, FindIntersection2Lines | 28672 | `resultados-carctrlrest6361020-195518-439585` |
| `CarCtrlRest63B` | SetUpDriverAndPassengersForVehicle, SetCoordsOfScriptCar, ScanForPedDanger, FindMaximumSpeedForThisCarInTraffic | 16384 | `resultados-carctrlrest63b61020-201801-699060` |
| `CarCtrlRest63C` | PickNextNodeAccordingStrategy, JoinCarWithRoadSystem, JoinCarWithRoadSystemGotoCoors | 12288 | `resultados-carctrlrest63c61020-205957-788299` |
| `CarCtrlRest63D` | ReconsiderRoute, FindLinksToGoWithTheseNodes | 8192 | `resultados-carctrlrest63d61020-213407-964241` |
| `CarCtrlRest63E` | ChooseModel | 4096 | `resultados-carctrlrest63e61020-213238-930999` |
| `CarCtrlRest63F` | IsThisAnAppropriateNode | 4096 | `resultados-carctrlrest63f61020-214217-759241` |

Pendientes de la asignación (sin reconstruir; siguen siendo envolturas al EXE): ClitargetOrientationToLink, CreateConvoy, CreatePoliceChase, DragCarToPoint, FindAngleToWeaveThroughTraffic, FindNodesThisCarIsNearestTo, GenerateCarCreationCoors2,
GenerateOneEmergencyServicesCar, GenerateOneRandomCar, PickNextNodeRandomly, PickNextNodeToChaseCar, PickNextNodeToFollowPath y PossiblyRemoveVehicle.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CCarCtrl::ThisVehicleShouldTryNotToTurn` | 0x421FE0 | Simple (`RH_ScopedInstall`) | 4096 | ninguno |
| `CCarCtrl::FindPathDirection` | 0x422090 | Simple | 4096 | `CPathNode::GetPosition` 0x420A10 (ocho llamadas, en orden, con la posición perturbada según el orden de llamada) |
| `CCarCtrl::SwitchBetweenPhysicsAndGhost` | 0x4222A0 | Simple | 4096 | `CColStore::HasCollisionLoaded` 0x410CE0, `CDamageManager::SetWheelStatus` 0x6C21A0, `CAutomobile::PlaceOnRoadProperly` 0x6AF420, `CBike::PlaceOnRoadProperly` 0x6BEEB0 |
| `CCarCtrl::FindSpeedMultiplier` | 0x4224E0 | Simple global con nombre (`RH_ScopedNamedGlobalInstall` -> `Cc63SpeedMultiplierCpp`, X87Math) | 4096 | ninguno |
| `CCarCtrl::StopCarIfNodesAreInvalid` | 0x422590 | Simple | 4096 | ninguno |
| `CCarCtrl::FindPercDependingOnDistToLink` | 0x422620 | Simple | 4096 | ninguno |
| `CCarCtrl::FindIntersection2Lines` | 0x4226F0 | Simple global con nombre (`Cc63Intersection2LinesCpp`, X87Math; no toca EDX) | 4096 | ninguno |

```
bash reversing/_handoff/bench_here.sh CarCtrlRest63           # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/CarCtrlRest63/build_probe.py
python -B reversing/CarCtrlRest63/run_local.py
python -B reversing/CarCtrlRest63/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -B reversing/CarCtrlRest63/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63/20261020-195518-439585"   # sólo lo ejecuta el coordinador
```
Filas de `docs/hooks.csv` (las añade el coordinador; todas tipo S): `CCarCtrl,<función>,<dirección>,1,0,1,S` para las siete direcciones de la tabla. `SCOPE_NOTE` de `register_hook.py` ya está rellenado.
Archivo de arranque en el clon: `carctrlrest63-runtime.start`; salida `carctrlrest63-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de estas siete funciones respecto a HEAD; `git apply --check` contra los blobs de HEAD correcto)

- `CarCtrl.cpp`: las siete funciones en C++ (antes `plugin::Call*` hacia el EXE), sus siete líneas de hook en `CCarCtrl::InjectHooks` y los `#include` necesarios (`X87Math.h`, `ColStore.h`, `Automobile.h`, `Bike.h`, `DamageManager.h`).
  `CPathNode::GetPosition` se llama fuera de línea (puntero a miembro volátil `cc63GetNodePosition`) para conservar el número y el orden de llamadas del EXE.
- `X87Math.cpp/.h` (bloque «Lote 63: CCarCtrl (parte C)» al final, prefijo `Cc63`; el archivo se compila `/O2 /Oi /arch:IA32 /fp:fast`): `Cc63Intersection2LinesCpp`, `Cc63SpeedMultiplierCpp`, `Cc63PercDistCpp` y
  `Cc63PathDirectionCpp`. Aritmética x87 de 24 bits con exponente extendido: lo que el EXE deja en registro es `double` (cada operación redondea la mantisa a 24 bits); lo que guarda con `fstp/fst dword` es `float`/`volatile float`.

## Revisión contra el EXE (re-desensamblado y contrastado instrucción a instrucción)

**ThisVehicleShouldTryNotToTurn (0x421FE0).** `movsx eax, word [veh+0x22]; add eax, -403; cmp eax, 0x79; ja -> false`; tabla de bytes de 122 entradas (0x422010) y tabla de saltos (0x422008). Leí la tabla del EXE: los únicos índices con 0 (verdadero)
son los modelos 403, 406, 431, 437, 443, 455, 514, 515 y 524 (Linerunner, Dumper, Bus, Coach, Packer, Flatbed, Tanker, Roadtrain y Cement Truck). La comparación sin signo `ja` sobre el modelo de 16 bits extendido con signo equivale al `switch` de la fuente.

**FindPathDirection (0x422090..0x422292).** `*arg4 = false` se escribe antes de nada; después se comprueban las áreas en orden 1, 2, 3 (`0xFFFF` -> 0) y luego los punteros de área cargados en el mismo orden. `CPathNode::GetPosition` se llama ocho veces: nodo 2 (x), 1 (x), 2 (y), 1 (y),
3 (x), 2 (x), 3 (y), 2 (y), cada vez leyendo una sola coordenada y restando con `fsub` + `fstp dword` (d21 = (p2.x - p1.x, p2.y - p1.y), d32 = (p3.x - p2.x, p3.y - p2.y)). Longitud `sqrt(dy² + dx²)` (en ese orden), `== 0` (comparación `test ah,44h/jnp`: NaN continúa) -> 0;
`1.0/len` (0x858624); nx21 se guarda como float y ny21 queda en registro; mismo proceso con la 32; `cross = nx21·ny32 - nx32·ny21` guardado como float y `dot = ny21·ny32 + nx32·nx21` en registro; `dot > 0.4f` (0x858EE8) -> 1;
`dot < -0.3f` (0x858EE4) -> `*arg4 = true`; después 4 si `cross > 0.0f` (ordenada) y 2 si no. Constantes leídas del EXE: 0x858624 = 1.0f, 0x858EE8 = 0x3ECCCCCD, 0x858EE4 = 0xBE99999A.

**SwitchBetweenPhysicsAndGhost (0x4222A0..0x42236C).** Sólo vehículos con `+0x40 & 0x4000`, `+0x4A4 == 2` (misión) y `+0x594` distinto de 5, 4, 3 (barco, avión, helicóptero). Estado = byte `+0x36 >> 3` (cinco bits altos): 3 (física) -> si `CColStore::HasCollisionLoaded(pos, 0)` es falso,
`(byte & 7) | 0x60` y, si `+0x594 == 0` (se vuelve a leer), `SetWheelStatus(rueda, 0)` para las ruedas 0..3 sobre `veh + 0x5A0`; 0xC (fantasma) -> si está cargado, `(byte & 7) | 0x18` y `PlaceOnRoadProperly` del automóvil (`+0x590 == 0`) o de la moto (`+0x590 == 9`; salto de cola).
`pos` = `m_matrix ? matriz+0x30 : veh+4` (el `GetPosition` inline de la DLL hace lo mismo).

**FindSpeedMultiplier (0x4224E0..0x422586).** a1 se lleva a [-pi, pi] (`pi` = 0x858CB8, `-pi` = 0x858CC0, `2pi` = 0x858CBC; bucles `fadd`/`fsub` con comparaciones ordenadas: NaN no entra), `|a1|` (`fchs` si < 0), `d = |a1| - a2` y `d = 0` si `0 > d`;
`r = a3 - a2`, `v = 1 - (d/r)·(1 - a4)` guardado como float (`fstp`); devuelve `a4` (`fld [esp+10h]`) si `d > r` (ordenada) y `float(v)` si no.

**StopCarIfNodesAreInvalid (0x422590..0x422618).** Palabras de `+0x3A4` y `+0x3A6` (enlaces: el área se busca con `>> 10`) y `+0x390` y `+0x394` (nodos: la palabra de 16 bits entera, sin acotar el índice) se comprueban en ese orden contra `0xFFFF` y contra el puntero de área de `ThePaths`;
cualquier fallo -> `[veh+0x3DC] |= 1` y `true`; si no, `false`.

**FindPercDependingOnDistToLink (0x422620..0x4226EF).** Enlace de 14 bytes en `m_pNaviNodes[area]` (`area = addr >> 10`, índice `addr & 0x3FF`), sin comprobar que el área esté cargada; `X = (int16)[link+0] · 0.125`, `Y = (int16)[link+2] · 0.125`; `dist = sqrt((Y - py)² + (X - px)²)` (24 bits por operación);
`dist < 5.0f` (registro) -> 0.5f (0x858B8C); con `dist` redondeado a float (`fst dword`): no menor que 15.0f (o NaN) -> 1.0f; si no `(dist - 5.0f) · 0.05f + 0.5f`.

**FindIntersection2Lines (0x4226F0..0x422753).** `det = a3·a8 - a4·a7`; `det == 0` (`test ah,44h/jnp`: con NaN se divide) -> `t = 0.0f`, si no `t = ((a5 - a1)·a8 - (a6 - a2)·a7) / det`; `*out1 = a3·t + a1` y después `*out2 = t·a4 + a2`; `t` se queda en registro (sin pasar por float).
EAX y ECX cargan los punteros de salida; **EDX no se toca**, y el llamador 0x4287C6 (`DealWithBend_Racing`) lee `[edx]` tras la llamada, por eso la banca compara EDX en esta función.

Llamadores y registros de salida: ningún llamador del EXE de las otras seis funciones lee EAX/ECX/EDX de salida más allá de `AL`/ST0 (que sí se comparan); EDX tras `FindIntersection2Lines` sí se compara.

## Defectos hallados

- **Banco (corregido):** la familia 13 de `FindSpeedMultiplier` podía sembrar `a1 = ±3.4e38` o infinito: el bucle de ±2pi del EXE (`x + 2pi == x`) no termina y el juego quedaba colgado («GTA no responde», caso 285). Ahora `a1` nunca supera 2^24 en esa familia (las demás ya estaban acotadas: familia 2 hasta ~5e5).
  No es una diferencia de la función: tanto el EXE como la DLL no terminan con ese argumento (comportamiento original); simplemente no se prueba.
- Fuente de producción: ninguna discrepancia hallada en esta revisión; la reconstrucción del agente interrumpido coincide con el desensamblado.

## Suposiciones y riesgos

- Falsos de colaboradores (ver tabla) con parche de entrada en la dirección del EXE y en el cuerpo de la DLL resuelto antes de escribir ningún parche; sus cuerpos reales no se validan aquí.
- `FindPercDependingOnDistToLink` lee `m_pNaviNodes[area]` sin comprobar nulidad (igual que el EXE); las fixtures siempre dejan el área válida (el EXE desreferencia el puntero sin comprobar).
- Bits altos de los argumentos de 16 bits (`CCarPathLinkAddress`): se siembran con basura (`junkHi`) y el EXE y la DLL deben ignorarlos igual; aprobado con ella.
- Bucle de `FindSpeedMultiplier` con |a1| grande: tarda O(|a1|/6.28) iteraciones, no probado más allá de ~5e5 (los argumentos mayores no terminan en práctica).
