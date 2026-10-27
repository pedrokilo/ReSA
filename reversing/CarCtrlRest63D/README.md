<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlRest63D (tanda 4): ReconsiderRoute y FindLinksToGoWithTheseNodes

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-4`, HEAD df26d6fe7 + fuentes del lote 63 parte C, tandas 1 a 4). 8192 pares (2 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados y cobertura de ramas medida sobre los eventos de los colaboradores. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlrest63d\20261020-213407-964241`.
(Primera corrida aprobada, `20261020-211505-774897`, con la ruta profunda de `ReconsiderRoute` cubierta sólo en ~5 % de los casos (195 de 4096); se reforzaron las familias 8..13 del generador —cruce de ranura de 2000 ms, misión válida, ninguna condición de salida,
contador > 4, distancia corta y más de un nodo— y se repitió el banco.)

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CCarCtrl::ReconsiderRoute` | 0x42FC40 | Simple (`RH_ScopedInstall`) | 4096 | `FindNodesThisCarIsNearestTo` 0x42BD20 (envoltura `plugin::Call`, sin hook), `FindPlayerCoors` 0x56E010, `CPathFind::DoPathSearch` 0x4515D0, `CAutoPilot::RemoveOnePathNode` 0x41B950 (con hook) y `FindLinksToGoWithTheseNodes` 0x42B470 (esta con el hook apagado y el cuerpo de la DLL parcheado, porque también se prueba aquí) |
| `CCarCtrl::FindLinksToGoWithTheseNodes` | 0x42B470 | Simple | 4096 | `srand` del CRT del EXE 0x821B11 (sin hook) y `CCollision::DistToLine` 0x417610 (con hook) |

```
bash reversing/_handoff/bench_here.sh CarCtrlRest63D           # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/CarCtrlRest63D/build_probe.py
python -B reversing/CarCtrlRest63D/run_local.py
python -B reversing/CarCtrlRest63D/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -B reversing/CarCtrlRest63D/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63d/20261020-213407-964241"   # sólo lo ejecuta el coordinador
```
Filas de `docs/hooks.csv` (las añade el coordinador; ambas tipo S): `CCarCtrl,ReconsiderRoute,0x0042FC40,...,S` y `CCarCtrl,FindLinksToGoWithTheseNodes,0x0042B470,...,S`. `SCOPE_NOTE` de `register_hook.py` ya está rellenado. Archivo de arranque: `carctrlrest63d-runtime.start`.

## Fuente (`source.patch`: sólo los cambios de la tanda 4 respecto a la tanda 3; aplicar tras las de `CarCtrlRest63`, `63B` y `63C`)

`CarCtrl.cpp`: las dos funciones en C++ y sus dos líneas de hook; sin ayudantes x87 nuevos (las posiciones de nodo son enteros de 16 bits / 8, exactos en float, y las comparaciones son ordenadas).

## Revisión contra el EXE (re-desensamblado)

**ReconsiderRoute (0x42FC40..0x42FF96).** `(semilla +0x20 + CTimer 0xB7CB78) / 2000` frente a `(semilla + CTimer 0xB7CB84) / 2000` con la división por constante mágica 0x10624DD3 (sin signo; el producto de 64 bits y `>> 7`): iguales -> sale sin tocar nada.
Misión `movsx byte [+0x3BA]`; tabla de bytes 0x42FFA0 (leída del EXE): 2, 4, 8, 15, 17, 43, 52, 54, 61 siguen; el resto sólo pone a 0 el contador `+0x3C5`. `FindNodesThisCarIsNearestTo(veh, &primero, &segundo)` (locales con área 0xFFFF); `primero.área == 0xFFFF` sale.
Seis condiciones (sobre `actual +0x390`, `siguiente +0x394`, `final +0x398`, con `CNodeAddress::operator==` 0x420920, que compara área y nodo de 16 bits): actual==primero && siguiente==segundo; actual==segundo && siguiente==primero; final==primero && actual==segundo;
final==segundo && actual==primero; actual==segundo; final==segundo -> contador=0 y sale. Si no `++contador` (byte) y sale sin ponerlo a 0 si queda <= 4. Destino: misiones 2, 4 y 43 `FindPlayerCoors(-1)`; 8 `+0x3EC`; 15, 17, 52, 54, 61 posición del objetivo `+0x41C` (matriz+0x30 o +4).
`DoPathSearch(0, pos, segundo, destino, nodos=null, &contador, 0, &distancia, 1000000.0f, null, 1000000.0f, bit 6 de +0x3DB, actual, modelo==539, false)`; `distancia < 90000.0f` (0x85900C, ordenada) y contador `>= 2` (con signo) o contador=0 y sale. Si pasa:
`actual = primero`, `siguiente = segundo`, `FindLinksToGoWithTheseNodes`, `DoPathSearch(0, pos, segundo, destino, +0x3F8, +0x418, 8, null, 1000000.0f, null, 1000000.0f, bit 6, actual (= primero), modelo==539, false)`, `RemoveOnePathNode` y contador=0.

**FindLinksToGoWithTheseNodes (0x42B470..0x42B7CD).** Si la palabra `+0x45E` no es 0: `srand` del EXE (0x821B11). `actual = +0x390`, `siguiente = +0x394`; base = int16 `[nodo actual + 0x10]` (sin comprobar el área); busca (contador de 16 bits con signo, tope 12) el índice de `m_pNodeLinks[área][base + i]` igual al siguiente;
`+0x3A6 = m_pNaviLinks[área][base + i]` (0x96FDF4); `+0x3B6 = -1` si actual < siguiente (área y nodo sin signo; iguales -> 1). Con exactamente un enlace el índice es 0; con ninguno 0; si no se elige el enlace (distinto del siguiente y con el área cargada) más cercano al vehículo con
`CCollision::DistToLine(posición del nodo actual, posición del enlace, posición del vehículo)` (posiciones = int16 / 8; `<` ordenado contra 1000000.0f y el mejor; sin ninguno, 0). `+0x3A4 = m_pNaviLinks[área][base + índice]` y `+0x3B5 = -1` si el enlace elegido < actual (iguales -> 1).

Llamadores y registros: ningún llamador del EXE de estas funciones lee EAX/ECX/EDX de salida (el aviso de `residue_scan.py` en 0x42FEF4 es el `push ecx` de un argumento `bool` cuyo byte alto es basura de ECX: el colaborador sólo lee el byte bajo, como en `JoinCarWithRoadSystemGotoCoors`).

## Defectos hallados

- Producción: ninguna discrepancia hallada (las dos funciones coincidieron a la primera con el EXE).
- Banco: cobertura insuficiente de la ruta profunda en la primera corrida (ver arriba), reforzada.

## Suposiciones y riesgos

- `FindNodesThisCarIsNearestTo` sigue siendo una envoltura hacia el EXE (la validación de `ReconsiderRoute` la sustituye por un falso que escribe dwords completos; el EXE no inicializa la palabra de nodo de sus locales, la DLL sí: se registra sólo la palabra de área).
- `RemoveOnePathNode` se prueba como colaborador con un falso que registra el estado, no con el real.
- Los 12 enlaces de la búsqueda de `FindLinksToGoWithTheseNodes` y la base de enlaces se prueban con bases 0..16 y hasta 15 enlaces (el índice máximo es 31 < 32 entradas por área sintética).
