<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# MissionCleanup64: CMissionCleanup::Process (0x468560) y CheckIfCollisionHasLoadedForMissionObjects (0x4652D0)

Estado: **APROBADO dentro del juego** (20/10/2026): 4096 pares (2048 por función), cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauración en ambos lados y salida de cada lado igual a la del oráculo independiente. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-missioncleanup6461020-161632-431413`. Worktree `D:\Original\wg-1`, HEAD df26d6fe7 + este banco + la fuente de `MissionCleanup.cpp/.h` y `InjectHooksMain.cpp`.
`source.patch` contiene SOLO los cambios de fuente (`git apply --check` verificado sobre HEAD).

| Función | Dirección | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CMissionCleanup::Process()` | 0x468560 | Simple | 2048 | 58 |
| `CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects()` | 0x4652D0 | Simple | 2048 | 6 |

`CMissionCleanup` no tenía `InjectHooks` ni filas en `docs/hooks.csv`: se añade `CMissionCleanup::InjectHooks()` (solo estas dos funciones; las demás, Init/FindFree/AddEntityToList/RemoveEntityFromList, no se
tocan) y su llamada en `InjectHooksMain.cpp`. Filas necesarias en docs/hooks.csv:

```
CMissionCleanup,Process,0x00468560,1,0,1,S
CMissionCleanup,CheckIfCollisionHasLoadedForMissionObjects,0x004652D0,1,0,1,S
```

## Qué hacen las funciones (desensamblado)

**Process** (se llama una vez desde 0x469471 con ECX = 0xA90850 tras una misión): restablece cientos de ajustes del juego. En orden: desvanecimiento de cámara (si 0xA4448A, y modo ≠ 1/2) y bit 5 de pad0+0x10E;
trenes/aviones; colores de pandillas (0x96AB91); guerras de pandillas; talleres; deseo del jugador (`FindPlayerWanted(-1)+0x14 = 1.0f`); recogidas, controles de carretera, motos de policía; nodos de rutas
(`ThePaths`); grabaciones de vehículos; cámara (pantalla ancha, componentes de guion, `ResetDuckingSystem(0)`); efectos; audio de misión 0..3; clima; 10 modelos especiales; modelos de coche suprimidos;
decenas de globales (0xA44488.., 0xBAA3F8.., etc., todas por dirección en el fuente); grupo del jugador (`ms_groups + idx*0x2D4`: asignador de tareas = 1, tipo de decisión = -1, rango = *0x86C6C0); banderas
y habla del jugador; pad0 (+0x74 = 0 y +0x118..0x11F = 0); `FindTaskByType(3, 0x640)` y, si el tipo de la tarea sigue siendo 0x640, la función 0x634A40(tarea, jugador); omisión de cinemática; punto de reaparición
(según el estado del jugador 0x36/0x37 y 0x96A8B0); fuego de guion; `CStuckCarCheck::Init`; 6 pares de 0xA4892C; si 0xA444A8 == 0: cámara `Restore` (si Players[foco]+0xB0 == 0), CWanted+0x1E &= 0xFA,
`MakePlayerSafe(false, 10000.0f)` y `SetHelpMessage(0, 1, 0, 0)`; las 75 entradas de la lista (tipo → `CleanUpThisVehicle/Ped/Object`, `GetActualScriptThingIndex` + `FxSystem_c::Kill/RemoveScriptEffectSystem`,
`RemoveGroup`, tabla 2D (0xC3A1A0), secuencia de tareas (Flush o bandera +0x38), `UnloadDecisionMaker`, luces de búsqueda, checkpoints, diccionario de texturas) y, para cada una, `RemoveEntityFromList`;
por último 1920 llamadas `ClearPedTypeAsAcquaintance(id, tipo, GetPedFlag(otro))` y 40 `ClearPedTypeAcquaintances`.

**CheckIfCollisionHasLoadedForMissionObjects** (0x46A058): para cada entrada de vehículo (1, pool 0xB74494, 0xA18), peatón (2, 0xB74490, 0x7C4) u objeto (3, 0xB7449C, 0x19C): entidad del pool (byte de banderas
== byte bajo del handle), bit 0x40000 de +0x1C, `CColStore::HasCollisionLoaded(pos, area)` y `CIplStore::HaveIplsLoaded(pos, area)` (pos = matriz+0x30 si hay matriz, si no entidad+4; area = byte +0x2F);
entonces limpia el bit, `GetIsStatic` y, si no es estático, `AddToMovingList`; los vehículos de tipo (+0x590) 0 u 11 llaman a `CAutomobile::PlaceOnRoadProperly` y el 9 a `CBike::PlaceOnRoadProperly`.

## Fuente (MissionCleanup.cpp)

C++ puro (sin `__asm`), en el mismo orden que el EXE; los globales sin nombre se escriben por dirección con `Raw<T>(addr)`. El pool se lee en crudo como el EXE (`flags[handle >> 8] == (uint8)handle`, byte completo,
y `base + (handle >> 8) * tamaño`, sin comprobar el rango). `RemoveEntityFromList` relee tipo y handle tras el borrado porque los `CleanUp*` reales pueden modificar la lista. No hay aritmética x87 en ninguna
de las dos (constantes de coma flotante como bits).

## Registros

Los únicos llamadores (0x469471 y 0x46A058) no leen EAX/ECX/EDX tras la llamada; solo se registran. EBX/ESI/EDI/EBP, ESP, DF, SEH y x87 (con los registros vacíos crudos: `kCompareDeadX87Registers=true`) se comparan.

## Banco

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con solo ese hook encendido. Los 58 + 6 colaboradores son falsos idénticos en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL
cuando existe: `SetMissionDoesntRequireSpecialChar` y `RemoveEntityFromList` no tienen hook, su cuerpo se obtiene por puntero; `0x634A40` no está reconstruida y la DLL la llama por dirección). Los argumentos se
registran enmascarados al ancho declarado (bool/uint8/uint16) y los punteros como fichas simbólicas (`PED`, `PD`, `W`, `TASK`, `CLEAN`, `V<i>`, `P<i>`, `O<i>`, `M<i>+off`). Las 1960 llamadas finales de Process
(`GetPedFlag`, `ClearPedTypeAsAcquaintance`, `ClearPedTypeAcquaintances`) se comparan por cuenta y hash FNV-1a de todos sus argumentos y del valor devuelto por el falso.
Se comparan: eventos EN ORDEN, cambios de toda la arena (región `X` = cualquier cambio inesperado), bytes cambiados de la memoria real del EXE (≈65 regiones: todos los operandos absolutos de 0x468560..0x468D0C,
pad0 completo, filas 0..3 de las tablas de grupos, secuencias, efectos y 2D) y las compuertas de ABI. El jugador (`0xB7CD98`) y los tres punteros de pool (`0xB74490/94/9C`) se desvían a objetos de la arena
mientras dura cada llamada y se restauran. Cada caso rellena las regiones reales con un fondo distinto (todo cambio, incluso a cero, es visible) y fija los controles que leen las ramas.

`fixture_oracle.py` modela las funciones desde el desensamblado (independiente del fuente); `python -I fixture_oracle.py` autoprueba, `python -I fixture_mirror.py` compila `Fixtures.hpp` con `cl`, pasa los 4096 casos
por el oráculo, contrasta las constantes de disposición y la cobertura con `minimums.json`.

## Suposiciones y límites

- Todos los colaboradores son falsos: no se validan sus cuerpos ni la integración real (cámara, audio, scripts, pools, rutas, etc.). `CPad::GetPad(0)->SetDrunkInputDelay` y las escrituras al mando 0 se ejecutan reales en ambos lados.
- No se prueban los caminos en que el EXE desreferencia sin comprobar: jugador sin datos (`+0x480` nulo) o sin CWanted (0xA444A8 == 0 y `pd[0]` nulo), handles de pool con índice fuera de rango, índices de guion mayores
  que 3 (leerían fuera de las filas del banco de las tablas 0xA44114/0xC178F0/0xC17898/0xA43F68/0xC3A1A0), índice de grupo fuera de 0..3, foco del jugador fuera de 0..1.
- Los tipos de entrada fuera de 1..13 (incluido el signo negativo) solo ejercen el camino `default` + `RemoveEntityFromList`.
- Una sola configuración del juego: la copia de pruebas `E:\JUEGOS\GTA San Andreas - CVehicle Test` con partida cargada.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh MissionCleanup64   # compila la sonda y corre en el juego (candado global)
python -I reversing/MissionCleanup64/fixture_oracle.py       # autoprueba del oráculo
python -I reversing/MissionCleanup64/fixture_mirror.py       # coherencia de las fixtures + cobertura mínima (sin el juego)
python -B reversing/MissionCleanup64/run_local.py --analyze <comparisons.jsonl>
python reversing/MissionCleanup64/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Depuración: un archivo `missioncleanup64-cases.txt` en la carpeta del juego con «función primero último» (0 = Process, 1 = CheckIf) limita la corrida (el analizador final fallará por el número de pares).

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
