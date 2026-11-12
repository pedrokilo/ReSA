<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 888 grupos / 10227 funciones reconstruidas; 10227 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn165A: 26 funciones diminutas del EXE (lote 165A)

Estado: **APROBADO dentro del juego** (11/11/2026, worktree `wv-4` desde main 284fb34374 + los cambios del lote; plantilla `Fn155A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn165a/20261111-121214-590923` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn165A-20261111-121214-590923`; `comparisons.jsonl.gz` de 51 MB). 26 x 16384 = **425984 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia (salvo ST0 en `GetLocalSoundLevel`, comparado en 80 bits) y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `bbd6a36f2041a9d410ef3bf24289c28330d9aaaf11fdc1ab0a04416b70e97aae`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 26 tenia hook (se comprobo el codigo y `docs/hooks.csv`). Las 26 son viables en C++ puro; no se salto ninguna. Las clases del listado eran en su mayoria incorrectas: los nombres reales salen del desensamblado, de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CAETwinLoopSoundEntity::StopSound` (A, ya reconstruida) | 0x4F2930 | 35 | `Audio/Entities/AETwinLoopSoundEntity.cpp` | S |
| 1 | `CAETwinLoopSoundEntity` cuerpo del destructor (`TwinLoopSoundEntityDestructorExe`) | 0x4F2B80 | 139 | idem | S |
| 2 | `CEventSoundQuiet` cuerpo del destructor (vtable 0x86C2B0; el listado decia CAEAudioEntity) | 0x5DEA00 | 90 | `Events/EventSoundQuiet.cpp` (+ `.h`) | S |
| 3 | `CEventSoundQuiet::GetEventType` (0x3E) | 0x5DEA60 | 6 | idem | V |
| 4 | `CEventSoundQuiet::GetLifeTime` (0) | 0x5DEA70 | 3 | idem | V |
| 5 | `CEventSoundQuiet::GetSourceEntity` (`[this+14h]`) | 0x5DEA80 | 4 | idem | V |
| 6 | `CEventSoundQuiet::GetLocalSoundLevel` (`fld [this+18h]`) | 0x5DEA90 | 4 | idem | V |
| 7 | `CEventSoundQuiet::CanBeInterruptedBySameEvent` (AL = 1) | 0x5DEAA0 | 3 | idem | V |
| 8 | `CEventSoundQuiet` destructor con borrado | 0x5DEAB0 | 30 | idem | S |
| 9 | `CEventSoundQuiet::GetEventPriority` (8) | 0x5DEAD0 | 6 | idem | V |
| 10 | `CPedIntelligence` destructor con borrado (`ScalarDeletingDestructorExe`, sin vtable) | 0x5DEB90 | 30 | `PedIntelligence.cpp` (+ `.h`) | S |
| 11 | `CAnimBlendAssociation::GetNode` | 0x4CEB60 | 16 | `Animation/AnimBlendAssociation.cpp` (+ `.h`) | S |
| 12 | `CAnimBlendStaticAssociation` cuerpo del destructor (vtable 0x85C6CC) | 0x4CEC00 | 20 | `Animation/AnimBlendStaticAssociation.cpp` | S |
| 13 | `CAnimBlendStaticAssociation` destructor con borrado (cuerpo en linea) | 0x4CEF30 | 41 | idem | S |
| 14 | `RpAnimBlendNodeUpdateKeyFrames` (EAX = contexto; `RpAnimBlendNodeUpdateKeyFramesExe`) | 0x4D1570 | 100 | `Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp` | S |
| 15 | `CTaskUtilityLineUpPedWithCar` destructor con borrado (sin vtable) | 0x63A2E0 | 27 | `Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp` (+ `.h`) | S |
| 16 | `CTaskComplexEnterCarAsDriver` destructor con borrado (0x86EAAC) | 0x643C60 | 30 | `.../TaskComplexEnterCarAsDriver.cpp` | S |
| 17 | `CTaskComplexEnterCarAsPassenger` destructor con borrado (0x86EADC) | 0x643C80 | 30 | `.../TaskComplexEnterCarAsPassenger.cpp` | S |
| 18 | `CTaskComplexOpenDriverDoor` destructor con borrado (0x86EB0C) | 0x643CA0 | 30 | `.../TaskComplexOpenDriverDoor.cpp` (+ `.h`) | S |
| 19 | `CTaskComplexOpenPassengerDoor` destructor con borrado (0x86EB3C) | 0x643CC0 | 30 | `.../TaskComplexOpenPassengerDoor.cpp` (+ `.h`) | S |
| 20 | `CTaskComplexDragPedFromCar` destructor con borrado (0x86EB6C) | 0x643CE0 | 30 | `.../TaskComplexDragPedFromCar.cpp` | S |
| 21 | `FurnitureGroup_c::FurnitureGroup_c` (el listado decia CLoadingScreen) | 0x590DE0 | 12 | `Interior/FurnitureGroup_c.cpp` (+ `.h`) | S |
| 22 | `ListItem_c<FurnitureSubGroup_c>` constructor, **codigo muerto** (sin `call` ni dword que lo referencie) | 0x590E00 | 12 | `Interior/FurnitureSubGroup_c.cpp` (`ListItemConstructor`) | S |
| 23 | `FurnitureSubGroup_c::FurnitureSubGroup_c` | 0x590E20 | 68 | idem (+ `.h`) | S |
| 24 | `FurnitureSubGroup_c::~FurnitureSubGroup_c` | 0x590E70 | 76 | idem (+ `.h`) | S |
| 25 | `Furniture_c::Furniture_c` (clase sin `InjectHooks` registrado: se define y se anade a `InjectHooksMain`) | 0x590EC0 | 12 | `Interior/Furniture_c.cpp` (+ `.h`) | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Tipo S = `RH_ScopedNamedGlobalInstall` (cuerpos, destructores con borrado libres), `RH_ScopedInstall` (miembros) y `RH_ScopedGlobalInstall` (0x4D1570). 26 filas nuevas en `docs/hooks.csv`.

## Funciones saltadas
Ninguna. Casos que parecian dificiles: **0x4D1570** recibe el contexto en EAX (`EntryEaxCpp` de `X87Math`, primera sentencia, `runtime_checks` apagado, patron de `PlantLocTri.cpp`) y los otros dos argumentos en la pila (cdecl); la funcion ya reconstruida pasa a comparar el contador **con signo** (`jle`), como el EXE. No se toca `X87Math.cpp` ni `source/CMakeLists.txt`.

## Revision contra el EXE
- **0x4F2B80**: marco SEH omitido; vtable 0x85F438, si el WORD +0x88 no es cero `StopSoundAndForget` (0x4EF850) sobre +0xA0 y +0xA4 poniendo la casilla a cero DESPUES de la llamada y el WORD a cero; vtable 0x859988 y `~CAESound` (0x4EF660) sobre +8. El destructor del compilador no sirve (vtables propias): se escribe a mano, como en `CAEPedAudioEntity::Destructor`.
- **0x4F2930**: la segunda voz (+0xA4) se lee tras la primera llamada (el banco lo observa con un falso que cambia +0xA4).
- **0x5DEA00 / 0x5DEAB0 y las seis virtuales**: igual que los eventos del lote 155A (`CleanUpOldReference` de +0x14 y cuerpo 0x4AC480); las virtuales pasan de inline en la cabecera a `.cpp`.
- **0x5DEB90 / 0x63A2E0**: sin vtable; los destructores `Destructor()` (0x607300, 0x64FC00) son colaboradores con hook; liberan con `CPedIntelligence::operator delete` (0x6074E0) y con `operator delete` del CRT (0x8214BD, sin hook, falso solo en la direccion del EXE).
- **0x4CEB60**: `[ecx+10h] + 0x18 * indice` sin comprobar rango (antes pasaba por `std::span`).
- **0x4CEC00 / 0x4CEF30**: vtable 0x85C6CC y `CMemoryMgr::Free` (0x72F430) del mapa de secuencias (+0xC) sin ponerlo a cero; el destructor con borrado solo mira el bit 0 (no el 1 como 0x4CDF50).
- **0x643C60..0x643CE0**: cuerpos 0x640330, 0x640380 (funciones libres con hook), 0x6403D0, 0x640420 (`Destructor()` con hook) y 0x6404D0 (sin hook: funcion propia `DragPedFromCarDestructorBodyExe`, el falso va en la direccion del EXE y en ella); liberan con `CTask::operator delete` (0x61A5B0).
- **0x590DE0..0x590EC0**: 0x4A8DB0 pone a cero `+0` y `+4`; 0x4A8DD0 `+0`, `+4`, `+8`; 0x4A8DE0 y 0x4A8DC0 son `ret`. El destructor 0x590E70 no escribe nada.

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas (A, B, C; 4 KiB con guardas de 0x40). Memoria global solo `gpAnimBlendClump` (0xB4EA0C) en la funcion 14, restaurada tras cada lado. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (`F`, `U`, `E`, `Rt`): ver `LiveComparison.hpp`. Se observan bytes cambiados de las arenas, eventos de los falsos, valor devuelto (EAX, AL, etiqueta de `this` o ST0 de 80 bits) y estado x87. 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` (sin juego) pasa los 425984 casos por el oraculo con cobertura de ramas >= 50 por marca.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales, **NaN senalizantes en el tiempo de la asociacion de 0x4D1570** (el EXE los pasa por `fld`/`fstp`, que los vuelve silenciosos; el C++ copia bits), mas de 12 frames o mas de 3 arrays de nodos, y la exactitud de los colaboradores falseados.

## Prueba negativa
Dos rondas de 26 mutaciones (`mutate.py`; condicionadas a `this`/objeto desalineado o a banderas que el juego no produce; `FN165A_CASES=2048`). Copias buenas (SHA256, prefijo): `AnimBlendAssociation.cpp` 449a3f82, `AnimBlendStaticAssociation.cpp` d8ddcc29, `AETwinLoopSoundEntity.cpp` 6fe8ae96, `EventSoundQuiet.cpp` 33a41084, `FurnitureGroup_c.h` 58e48170, `FurnitureSubGroup_c.cpp` 100fce5b, `FurnitureSubGroup_c.h` 580e3f07, `Furniture_c.h` 29e32f9e, `PedIntelligence.cpp` 066bbf5f, `RpAnimBlend.cpp` 4ad707ef, `TaskComplexDragPedFromCar.cpp` 70d2ceb8, `TaskComplexEnterCarAsDriver.cpp` f184985d, `...AsPassenger.cpp` bdaed7ed, `TaskComplexOpenDriverDoor.cpp` 8c166759, `TaskComplexOpenPassengerDoor.cpp` 934ff66d, `TaskUtilityLineUpPedWithCar.cpp` fb80bc5c.
**Las 26 funciones fallan en ambas rondas** (`negative-runs/`: ronda 1 `20261111-135557-178149`, 17093 de 53248 pares con diferencias; ronda 2 `20261111-141801-753639`, 14261 de 53248; lado EXE fuera del oraculo 0). Una primera ronda 1 (`20261111-130345-804224`) se descarto: la mutacion de 0x4D1570 (`nFrames - 1`) convertia INT_MIN en un contador enorme y el juego se cayo dentro del banco; se corrigio (`nFrames > 0 ? nFrames - 1 : nFrames`) y se repitio. Fuentes restauradas con `mutate.py restore` (SHA256 igual) tras cada ronda.

## Bancos antiguos que citan estas direcciones (no repetidos)
Solo grep de las direcciones, sin repetir: 0x4F2B80 en `Fn130A`, `Fn131A` y `VehAudioFinal4`; 0x5DEA00 en `AutomobileFinal4`; 0x4D1570 en `CppRpAnimUpdate1`. Las demas direcciones no las cita ningun banco.
