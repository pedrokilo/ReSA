<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 996 grupos / 12438 funciones reconstruidas; 11156 con evidencia histórica y 1282 pendientes de validación en 142 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn189A: 29 funciones del EXE (lote 189A)

Estado: **APROBADO dentro del juego** (15/11/2026, worktree `wv-3` desde main 9acc2209e0 + cherry-pick del lote a23f2f2dc1 + las correcciones que dio el banco; plantilla `Fn173A`, con `Fn171A` y `Fn148A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn189a/20261115-132006-529478` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn189A-20261115-132006-529478`). 29 x 16384 = **475136 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `7d9850c93b5709b93d673c1b164e1b33c10d4fec3e14de0c476af2e284ef2369`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Detalle de cada funcion, riesgos y saltadas (ninguna): `reversing/PendienteValidar/189A.md`. Las clases del listado eran en su mayoria incorrectas; los nombres reales salen del desensamblado. Filas de `docs/hooks.csv`: las 29 pasan a `validated=1`.

| # | Funcion | Entrada |
|---|---|---|
| 0 | `CDirectory` destructor con borrado | 0x407670 |
| 1-9 | `CStreaming`: IsObjectInCdImage, ForceLayerToRead, SetLoadVehiclesInLoadScene, RemoveUnusedModelsInLoadedList, ImGonnaUseStreamingMemory, IHaveUsedStreamingMemory, GetDefaultAmbulanceModel, GetDefaultFireEngineModel, LoadInitialVehicles | 0x407800, 0x407A10, 0x407A30, 0x407AC0, 0x407BE0, 0x407BF0, 0x407D30, 0x407DC0, 0x407F20 |
| 10 | `CLinkList<CEntity*>::Insert(CEntity* const&)` | 0x408230 |
| 11 | `CPool<CPed,CCopPed>::GetAt` | 0x4082A0 |
| 12-14 | `CStreaming`: StartRenderEntities, SetMissionDoesntRequireSpecialChar, RemoveStreamedScriptModel | 0x4096C0, 0x40B490, 0x40C1C0 |
| 15 | `CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>(lista, banderas)` | 0x40C450 |
| 16 | constructor de `CPool<CVehicleStructure>` | 0x5B8340 |
| 17-28 | ayudantes de la STL de MSVC 6 de los vectores de atractores: fill (4 x3, 20), copy 20, copy_backward 20 y 12, uninitialized_copy (4 x3, 20, 12) | 0x5EAAA0 .. 0x5EAEE0 |

## Correcciones que dio el banco (fuente de produccion)
- **`CStreaming::GetDefaultAmbulanceModel` / `GetDefaultFireEngineModel`**: `eLevelName` es de 8 bits y la DLL leia solo el byte bajo de 0xBA6718; el EXE lee el DWORD. Ahora leen `StaticRef<int32>(0xBA6718)` (330 a 5200 pares distintos por funcion en la primera corrida).
- **`ProcessEntitiesInSectorList` (0x40C450)**: (1) `GetInfo(modelId)` hacia assert y `operator[]` comprobado con indices negativos (movsx): ahora `ms_aInfoForModel.data()[modelId]`; (2) `CBaseModelInfo::GetRwObject()` comprueba el tipo con una virtual y desreferencia el objeto RenderWare (cuelgue con model infos sinteticos): se anade `GetRwObjectNoChecks()` (lectura cruda de +0x1C, como el EXE); (3) `IsInCurrentArea()` comparaba un enum con signo con `CGame::currArea`; el EXE extiende el byte con ceros y compara con el DWORD (330 pares distintos con area actual 0xFF): comparacion escrita como el EXE.
- **Ayudantes de la STL con tres direcciones** (`SArrayFillStride4`, `SArrayUninitCopyStride4`): RH no admite dos hooks hacia la misma funcion de la DLL (el cuerpo de la 2.a y la 3.a se resolvia a la entrada del EXE de la 1.a: el candidato no ejecutaba la DLL). Cada direccion tiene ahora su propia instancia de plantilla.
- Sin cambios necesarios en el resto (CPools ConstructPoolBody, CLinkList Insert, CPedPool::GetAt, funciones vacias, CDirectory, std::copy*).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (0x409C90, 0x4089A0, 0x52CEE0, 0x4087E0) y solo en la direccion del EXE de los que la DLL llama por ella (0x8214BD, 0x821195); `~CDirectory` (0x5322D0) corre real. Memoria global tocada y restaurada: tablas y contadores de CStreaming, nivel actual, area y scan code, tabla de model infos, ranuras de CStreamingInfo controladas, pool de nodos (0xB74484, pool falso para las precondiciones del iterador) y la marca 0x5A de las funciones vacias; la pila de ids de memoria y los contadores de CMemoryMgr solo se vigilan. 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 475136 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn189A` (la corrida dura ~55 min: timeout 9000 s en `run_local.py`).

## Prueba negativa
`mutate.py` (25 mutaciones distintas por ronda = 29 funciones, porque los tres fill de 4 bytes y las tres copias sin inicializar de 4 bytes comparten cuerpo). **Las 29 funciones fallan en ambas rondas** (`negative-runs/`, 2048 casos por funcion): ronda 1 `20261115-141523-291193` (30853 de 59392 pares con diferencias) y ronda 2 `20261115-142222-633560` (30253); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas). Las plantillas de cabecera (`CLinkList::Insert`, `CPool::GetAt`) se mutaron editando `LinkList.h` y `Pool.h`.

## Fuera de los fixtures
Punteros crudos que se desreferencian, listas o pools corruptos, solapes parciales dentro de un elemento de los ayudantes de la STL, indices de `CPool::GetAt` fuera de capacidad (la plantilla de la DLL tiene assert), pools y nodos reales construidos por el EXE, y la exactitud de los falsos.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x407A10, 0x407A30 -> `Cutscene35`; 0x407AC0 -> `Cutscene35`, `GameLogic27`; 0x407BE0 y 0x407BF0 -> `Cutscene35`, `Fn138A`, `MenuMgrD`, `Plant19` (ahora son `ret`: **repetir**); 0x407D30 y 0x407DC0 -> `Car84K`; 0x407F20 -> `Cutscene35`, `Game22`; 0x4096C0 -> `Renderer33`; 0x40B490 -> `MissionCleanup64` (**repetir**: colaborador falseado); 0x5B8340 -> `VehModelInfoFinal4`. Ademas, el cuerpo comun `ConstructPoolBody` lo usa `CPools::Initialise`; `LinkList.h`, `Pool.h` y `BaseModelInfo.h` no cambian de comportamiento (solo se anade `GetRwObjectNoChecks`); `StreamingFinal4` cubre la sobrecarga de 0x40C270, que no se toca.
