<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 903 grupos / 10577 funciones reconstruidas; 10577 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn179A: 25 funciones del EXE (lote 179A)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-3` desde main de5af96751 + el commit del lote 9427ae3710 aplicado con cherry-pick; plantilla `Fn173A` con piezas de `Fn168A` (eventos, destructores con borrado, `Rt`), `Fn161A` (operator new[] con guarda de hilo) y `Fn129A`/`Fn144A` (pool y operator delete)). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn179a/20261112-180827-763701` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn179A-20261112-180827-763701`, `comparisons.jsonl.gz` de 65,9 MB). 25 x 16384 = **409600 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `700f3cec577394b6e821a0e7091cde34128bd177eabbc9625a990c98c0c0f6d4`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El lote llegaba reconstruido y enganchado, sin banco (`reversing/PendienteValidar/179A.md` en 9427ae3710). **El banco no encontro ninguna diferencia: no hizo falta corregir ninguna fuente y no se excluyo ninguna funcion** (el lote no saltaba ninguna). Las 25 filas de `docs/hooks.csv` pasan a `validated=1`.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CPool<ColDef>::Flush` (pool de CColStore) | 0x410610 | 54 | S |
| 1 | `CPool<ColDef>::CPool(capacidad, nombre)` | 0x410F80 | 91 | S |
| 2 | `CPool<ColDef>::Flush` (segunda copia identica) | 0x410FE0 | 54 | S |
| 3 | `CPool<ColDef>::New` | 0x411030 | 103 | S |
| 4 | `CPool<ColDef>` destructor con borrado | 0x4110E0 | 71 | S |
| 5 | `CEventPlayerCommandToGroupGather::GetEventType` (76) | 0x609270 | 6 | V |
| 6 | `CEventPlayerCommandToGroupGather::CloneEditable` | 0x609280 | 90 | V |
| 7 | `CEventPlayerCommandToGroupGather` destructor con borrado | 0x6092E0 | 30 | S |
| 8 | `CEventPlayerCommandToGroupGather` cuerpo del destructor | 0x609300 | 11 | S |
| 9 | `CEventNewGangMember::GetEventPriority` (42; el listado decia Gather) | 0x609310 | 6 | V |
| 10 | `CEventDontJoinPlayerGroup::GetEventPriority` (43; el listado decia Gather) | 0x609320 | 6 | V |
| 11 | `CEventPlayerCommandToGroupGather::GetEventPriority` (45) | 0x609330 | 6 | V |
| 12 | `CPlayerPed` cuerpo del destructor (el listado decia Gather) | 0x6093B0 | 217 | S |
| 13 | `CTaskComplexGangFollower::GetTaskType` (1207) | 0x65EBA0 | 6 | V |
| 14 | `CTaskComplexGangFollower::CalculateOffsetPosition` (retorno oculto, x87) | 0x65ED40 | 329 | S |
| 15 | `CTaskSimpleGoToPointFine` destructor con borrado (el listado decia GangFollower) | 0x6634E0 | 30 | S |
| 16 | `CTaskAllocatorKillOnFoot` destructor con borrado | 0x69D9C0 | 59 | S |
| 17 | `CTaskAllocatorAttack` destructor con borrado | 0x69DA00 | 59 | S |
| 18 | `CTaskAllocatorKillThreatsBasic` destructor con borrado | 0x69DA40 | 59 | S |
| 19 | `CTaskAllocatorKillThreatsDriveby` destructor con borrado | 0x69DA80 | 59 | S |
| 20 | `CTaskAllocator` destructor con borrado (base) | 0x69DAC0 | 56 | S |
| 21 | `CTaskAllocatorPlayerCommandAttack` destructor con borrado | 0x69DB00 | 59 | S |
| 22 | `CTaskAllocatorPlayerCommandAttack` cuerpo del destructor | 0x69DB40 | 11 | S |
| 23 | `CAEWeatherAudioEntity::Constructor` (el listado decia CWeather) | 0x72A620 | 17 | S |
| 24 | `CWeather::UpdateWeatherRegion` (antes sin hook) | 0x72A640 | 335 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Fuentes (en `source/game_sa/`): `Collision/ColStore.cpp`, `Events/EventPlayerCommandToGroupGather.cpp/.h`, `Events/EventNewGangMember.cpp/.h`, `Events/EventDontJoinPlayerGroup.cpp/.h`, `Entity/Ped/PlayerPed.cpp`, `Tasks/TaskTypes/TaskComplexGangFollower.cpp/.h` (con `X87Math.cpp/.h`: `Hp179AFollowerFarFromLastCpp`), `Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp`, `Tasks/Allocators/*.cpp`, `Audio/Entities/AEWeatherAudioEntity.cpp`, `Weather.cpp`.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook: `CEvent::operator new` 0x4B5620, `CEvent::operator delete` 0x4B5630, constructor 0x4B23D0 y cuerpo del destructor 0x4B2470 de `CEventPlayerCommandToGroup`, `CAnimManager::RemoveAnimBlockRef` 0x4D3FD0, `CPedGroups::RemoveGroup` 0x5FB870, `~CPed` 0x5E8620, `CVector::FromMultiply3x3` 0x59C6D0, `CTask::operator delete` 0x61A5B0 y los cuerpos 0x65EEF0, 0x69BBF0, 0x69C2D0, 0x69C780 y 0x69CB60; los cuerpos 0x609300 y 0x69DB40 son otras funciones del lote (ruta propia, falso identico). Solo en la direccion del EXE (la DLL los llama por ella) y con guarda de hilo (en otros hilos no hacen nada o llaman al real): operator new[] 0x821195, delete[] 0x8213AE y operator delete 0x8214BD. Memoria global tocada y restaurada (comparada con `gchanges`): puntero del pool de asignadores 0xB744BC, puntero de los grupos de animacion 0xB4EA34, tablas de formacion 0xC196E8 / 0xC19728, camara 0xB6F028 (24 bytes) y palabra 0xC81314.

- **Pool (0-4)**: capacidades con signo (cero, negativas, enormes, 1..0x400), almacenamiento y mapa nulos, en arena o crudos, BYTE de propiedad con valores 0, 1, 0x80, 0xFF, 0x100...; `New` con los mapas de 0 libres, 1 libre, 10 %, 50 % y todos libres, cursor desde -1 hasta capacidad - 1 y contadores de 7 bits (incluido 0x7F); el constructor con `cap * 0x2C` que desborda 32 bits.
- **Eventos (5-11)**: banderas de pila (solo cuenta el byte bajo), `CloneEditable` con memoria nula o en arena y los cuatro EAX del constructor falso (se comprueba que la funcion devuelve la memoria y no el EAX del constructor), vtable escrita despues del constructor.
- **CPlayerPed (12)**: grupos 1 y 2 nulos, positivos y negativos (hasta -30), con la misma ranura, bandera 0x200000 puesta o no, y cambio de `[this+0x480]` tras la primera o la segunda llamada (relectura del puntero).
- **CalculateOffsetPosition (14)**: estados de movimiento 0..9 y -1, matriz del lider en arena o nula, distancias cero, alrededor de 3 (un eje y repartida) con +-1 ulp, NaN, infinitos y denormales; la aritmetica x87 de 24 bits (mantisa de 24 bits, exponente ampliado) la modela el oraculo con fracciones exactas; indice de formacion 0..3 y bandera 0x10 puesta o no.
- **Asignadores (16-21)**: pool sintetico con el objeto en la ranura `idx` (almacenamiento = this - 32 * idx, `this` alineado o no), cursor con signo (-1 .. capacidad + 5, bordes de 32 bits), contador de 7 bits; la liberacion en linea del EXE se compara con `CTaskAllocator::operator delete` de la DLL (real en ambos lados).
- **UpdateWeatherRegion (24)**: posicion dada (en arena, alineada o no) o nula (camara con matriz o sin ella), valores sobre los umbrales (+-1 ulp, iguales), NaN, infinitos y denormales, y palabra previa con bits altos para comprobar que solo se escriben 2 bytes.

16 familias de 1024 casos por funcion (`Fixtures.hpp`); `python -I fixture_selfcheck.py` / `selfcheck_per_fn.py` pasan los 409600 casos por el oraculo y cuentan la cobertura de ramas (cada marca de `KEYS` aparece al menos 50 veces por funcion). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn179A` (para ~400k pares hace falta un envoltorio con timeouts largos: la corrida tarda unos 26 minutos de juego). EAX/ECX/EDX tras la llamada no forman parte de la igualdad (solo se registran): difieren en las funciones que dejan ECX/EDX residuales (pool, `CloneEditable`, `CPlayerPed`, `CalculateOffsetPosition`, asignadores, `CAEWeatherAudioEntity`, `UpdateWeatherRegion`); no se ha comprobado llamador por llamador que ninguno los lea.

## Prueba negativa
`mutate.py` (25 mutaciones por ronda, condicionadas a `this`, puntero del pool o puntero de posicion desalineado, algo que el juego real no produce). **Las 25 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261112-183424-904325` (18440 de 51200 pares con diferencias) y ronda 2 `20261112-183807-916669` (18692 de 51200); el lado EXE coincide con el oraculo en todos los pares. Las mutaciones del cuerpo comun de `Flush` (funciones 0, 2 y 4) y de los destructores de asignadores afectan a varias funciones a la vez, pero cada ronda tambien muta cada funcion por separado. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y sonda buena recompilada (manifiesto regenerado).

## Fuera de los fixtures
`this` nulo o crudo, pools y objetos reales construidos por el EXE, ranuras ya libres en el pool de asignadores (la DLL tiene un `assert` en `CPool::Delete`; el EXE solo repite el OR), capacidades positivas enormes en el constructor y en `Flush` con punteros reales, cursor de `New` fuera de [-1, capacidad - 1], grupos de animacion fuera de [-30, 100], matriz del lider nula con estado de movimiento 4, 6 o 7 (el EXE no la comprueba), indices de formacion mayores que 3, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep de `0x...` en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x410F80 -> `ColStore23` (README); 0x65ED40 -> `CTasks15`, `CppTrigClimb` y `Tasks355Evidence/CTaskComplexGangFollower_65ED40.txt` (texto); 0x69DB00 -> `GangEvent1` (banco y README). El resto de direcciones no aparece citado.
