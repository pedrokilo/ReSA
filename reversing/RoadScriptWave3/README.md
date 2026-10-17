<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RoadScriptWave3: banco por pares de CRoadBlocks y CRunningScript (ultimas funciones pendientes)

Banco simple (flujo Small2/PadMouse1/WorldWave2) para cerrar `CRoadBlocks` y `CRunningScript`. **Sin compilar ni ejecutar todavia** (encargo: no compilar, no ejecutar GTA, no commit).

Uso: `bash reversing/_handoff/bench.sh RoadScriptWave3 3`. Filtro opcional: `roadscriptwave3-only.txt` en la carpeta del juego con `Clase::Funcion`.
`build_probe.py` y `run_local.py` redirigen `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` en memoria (sin tocar `runtime_runner.py`).

## Corpus: 3.200 pares (maximo)

| Funcion | EXE | Pares |
|---|---|---|
| `CRoadBlocks::Init` | 0x461100 | 128 |
| `CRoadBlocks::GenerateRoadBlockPedsForCar` | 0x461170 | 1024 |
| `CRunningScript::GivePedScriptedTask-OG` | 0x465C20 | 1024 |
| `CRunningScript::SetCharCoordinates` | 0x464DC0 | 1024 |

Original = hook de la funcion desactivado (corre el codigo del EXE); candidato = hook activo (corre la fuente). Los dos legs reconstruyen el mismo fixture
desde la misma semilla y se comparan: estado de los buferes falsos (hash), control x87 (24 bits) y la traza completa de colaboradores con argumentos.
Los stubs se instalan con `EntryPatch` en la direccion del EXE y en el cuerpo de la DLL (todos los `Body` se calculan antes de crear ningun parche) y solo
actuan en el hilo del banco. Los registros que pasan llevan `original` y `candidate`.

## Dobles por funcion

- **Init**: NO se abre el fichero real. `CFileMgr::OpenFile/Read/CloseFile` (0x538900/0x538950/0x5389D0) se doblan con un fichero en memoria (entero + 325 `CNodeAddress`);
  los hilos ajenos se reenvian a `fopen/fread/fclose`. Se compara el bloque 0xA43438..0xA43C78 y la traza (ruta, modo, 4 y 0x514 bytes leidos, cierre). Recuentos 0, 1, 2, 17, 100, 300, 324, 325 y aleatorios.
- **GenerateRoadBlockPedsForCar**: vehiculo, jugador, peds (2), matrices, inteligencia, clump, colmodels y arena de 6 tareas son buferes falsos. Doblados: `CPed::operator new`
  (0x5E4720), ctores `CCopPed` (0x5DDC60) y `CCivilianPed` (0x5DDB70) por sus simbolos exportados de la DLL, `CTask::operator new` (0x61A5A0), `CTaskManager::SetTask` (0x681AF0),
  `CEventGroup::Add` (0x4AB420; anula `m_task` del evento de pila porque el EXE real lo clonaria), `CEntity::GetColModel` (0x535300), `GetDefaultCopCarModel` (0x407C50),
  `ChooseGangPedModel` (0x5DE5A0), `CMatrix::SetRotate` (0x59B120), `CWorld::ProcessVerticalLine` (0x5674E0), `CWorld::Add` (0x563220), `SetClumpAlpha` (0x732B00),
  `RegisterReference` (0x571B70), `GiveDelayedWeapon` (0x5E89B0), `SetCurrentWeapon` tipo/ranura (0x5E6280/0x5E61F0), `FindPlayerPed`, `rand` (0x821B1E) y `CGeneral::GetRandomNumber`.
  Se ejecutan de verdad sobre buferes falsos: constructores de `CTaskComplexWanderCop/StandStill/KillPedOnFoot`, `CEventScriptCommand`, `CPedPlacement::FindZCoorForPed` (0x616920 y su version de la DLL) y `GetWeaponInfo`.
  Globales sustituidos y restaurados: puntero de zona 0xC0BC68, `CGangs::Gang` 0xC091F0, estados de carga de los modelos 285..287 y ranuras 596..599 de `ms_modelInfoPtrs`.
  Dominio: `pedType` = COP o PED_TYPE_GANG1..10, posicion 0..2, 0..2 peds (los demas tipos leen la tabla de bandas fuera de rango, igual en el EXE y en la DLL, pero memoria sin control del banco).
  Las tareas se comparan sin su vptr (el EXE pone el suyo, la DLL el de su vtable).
- **GivePedScriptedTask-OG**: script falso de 0x400 bytes (`m_ExternalType` 0xC9, `m_ThisMustBeTheOnlyMissionRunning` 0xDC, variables locales 0x3C y la global 0xA48960), pool de peds falso de
  6 ranuras (puntero 0xB74490 sustituido), tareas falsas con vtable trazada (el destructor con borrado registra `dtor`), grupo falso. Doblados: `GetPedsGroup`, `IsPlayer`, `CScriptedBrainTaskStore::SetTask`,
  `GetVacantSlot`, `CPedScriptedTaskRecordData::Set/SetAsGroupTask/SetAsAttractorScriptTask`, `CPedGroupIntelligence::SetScriptCommandTask/GetTaskScriptCommand`, `CTaskComplexSequence::AddTask`, `CEventGroup::Add`.
  `CEventScriptCommand` se construye y destruye de verdad. Se cubren handle -1, ped con script brain, tipos externos 0/2/3/5/otros, grupo/sin grupo, jugador.
- **SetCharCoordinates**: ped, vehiculo y grupo falsos (vtable falsa con la ranura 14 = `Teleport` trazada). Doblados: `FindGroundZForCoord`, `GetDistanceFromCentreOfMassToBaseOfModel`, `ClearSpaceForMissionEntity`,
  `GetPedsGroup`, `CPedGroupMembership::IsLeader`, `CPedGroup::Teleport`. Z incluye -100 exacto, +-1 ulp, -0.0, NaN, -1e30.

El evento `layout` comprueba bits y desplazamientos de la DLL frente al EXE (banderas de CPed, `m_bDontPursuit` 0x79C, `m_nNumPedsForRoadBlock` 0x4AE, radio del colmodel 0x24, tamano de `CGangInfo`, etc.).

## Correcciones de la fuente (evidencia asm)

`source/game_sa/RoadBlocks.cpp`
1. 0x461592 `mov ecx,eax / push 16h / call 5E6280`: el camino de civiles y bandas tambien llama a `SetCurrentWeapon(WEAPON_PISTOL)` (la DLL solo lo hacia para CITYCOP). Anadido.
2. 0x461637 `mov byte ptr [esi+79Ch],1` antes de reservar la tarea de cop: `CCopPed::m_bDontPursuit = true`. Anadido (WanderCop lo lee).
3. 0x46168D..0x4616B4: el EXE solo escribe los campos de WanderCop que el constructor de la DLL ya pone a cero; las asignaciones `= {}` de los temporizadores copiaban relleno. Eliminadas.
4. 0x46180B..0x46181A `push 1 / push edi / call 743C60 / mov edx,[eax+14h] / call 5E61F0`: el arma de banda se fija con `SetCurrentWeapon(ranura)` (GetWeaponInfo(arma, STD)->m_nSlot), no con la sobrecarga por tipo (0x5E6280).
5. 0x4613F8..0x461419: orden `GetColModel` del vehiculo y despues `GetDefaultCopCarModel`; separado en dos sentencias (C++ no fija el orden de los operandos).
6. 0x4617C1..0x4617C7 `[ecx+0C091F4h]`: `CGangs::Gang.data()[...]` (sin comprobacion de rango, como el EXE).

`source/game_sa/Scripts/RunningScript.cpp` (`SetCharCoordinates`)
7. 0x464DE2..0x464DED `fcomp [859014h](-100.0) / test ah,41h / jp`: con z NaN el EXE tambien llama a `FindGroundZForCoord` (`PutToGroundIfTooLow` usa `<=`). Inlinado como `!(z > -100)`.
8. 0x464E80..0x464E93 / 0x464E10: `fadd` de la distancia solo si `offset`; la DLL sumaba `0.0f` (convierte -0.0 en +0.0). Corregido.

MATCH sin cambios: `Init` (rep stos 0x51 dwords + 1 byte, OpenFile "rb", Read 4 y 0x514, CloseFile, bucle de IsActive 0xA43AD0..0xA43C90; solo difiere la rama NOTSA de fallo de apertura) y
`GivePedScriptedTask-OG` (0x465C20: ramas -1 / script brain / evento / grupo con las mismas llamadas y orden).

## Limites y supuestos a revisar al compilar

- El EXE llama dos veces a `FindPlayerVehicle` (0x4612E2/0x4612F0) y descarta el resultado; es una funcion pura, no se reproduce ni se dobla.
- Si falla `Init` al abrir el fichero la DLL aborta (`NOTSA_UNREACHABLE`) y el EXE desreferencia NULL: fuera de dominio.
- `CMatrix::SetRotate` se dobla (stub determinista que anula la posicion, como el original) para aislar `GenerateRoadBlockPedsForCar`; su equivalencia con el EXE es de otra funcion.
- Los ctores de ped se parchean por su simbolo exportado (`??0CCopPed@@QAE@I@Z`, `??0CCivilianPed@@QAE@W4ePedType@@I@Z`); la build es Debug (sin inlining).
- Si alguna direccion del EXE de los dobles no coincide con el cuerpo esperado de la DLL, el banco termina con `error` (`DLL body not found or overlapping`) y lista cual.

## Segunda aprobacion (09/10/2026)

Corrida `20261009-110441-788318`: passed=true, 3200 pares, 4 funciones con 0 fallos, evento complete. Se registra solo CRunningScript::SetCharCoordinates (1024 pares) con `register_roadscriptwave3_b.py` (`validation-summary-b.json`); las otras tres ya estaban registradas. CRunningScript queda completa.
