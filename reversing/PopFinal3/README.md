<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PopFinal3: banco por pares de CPopulation

Banco nuevo (flujo viejo, estilo Small2/PadMouse1/WorldFinal1) para 55 de las 64 funciones pendientes de `CPopulation` (EXE gta-sa.exe 1.0 US).
Sin compilar ni ejecutar todavia. Uso: `bash reversing/_handoff/bench.sh PopFinal3 3`. Filtro opcional: `popfinal3-only.txt` con `CPopulation::<Funcion>`.

Archivos: `LiveComparison.hpp` (tabla, colaboradores, bucle principal), `PopSupport.hpp` (trazas, parches, instantaneas, objetos falsos, dobles),
`PopCases1.hpp` / `PopCases2.hpp` (fixtures y llamadas por funcion), `build_probe.py`, `run_local.py` (ambos redirigen `runtime_runner.GAME` en memoria a
`E:/JUEGOS/GTA San Andreas - CVehicle Test`).

## Metodo

Original = hook desactivado (corre el EXE); candidato = hook activo (corre la DLL). Se compara el valor devuelto, hashes del estado falso tocado y la traza
ordenada de colaboradores con argumentos (`original`/`candidate` van en cada registro). x87 a 24 bits (`_controlfp(_PC_24)`), `switches` comprobado tras `State()`.

Mundo falso: pools de peatones (zancada 0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras (cabecera de 0x14 bytes; el puntero global 0xB74490/0xB7449C/0xB744A0 se
intercambia solo durante la ejecucion); model infos falsos para IDs 0..319 y dos reservados (19990 vehiculo, 19991 objeto) en `ms_modelInfoPtrs`; vtable falsa
(hueco 0 destructor, hueco 4 SetIsStatic, hueco 1 de model info = AsAtomicModelInfoPtr); peatones con bits de guion en +0x7B0. Dobles con `EntryPatch` en la direccion del EXE y
en el cuerpo de la DLL (todos los `Body()` se calculan antes de crear parches; `Mine()` filtra hilos ajenos): rand (0x821B1E + `CGeneral::GetRandomNumber`), CWorld::Add/Remove,
FindPlayerPed/Vehicle/CentreOfWorld, IsPlayer/CanBeDeleted/GetIsOnScreen, Darkel/GangWars, GetClumpAlpha, streaming, ProcessVerticalLine, surface, PickRandomCar, PlaceGroup,
CullZones, FindObjectsKindaColliding, GetBoundCentre, y las funciones de CPopulation que cada caso no prueba (ManageObject/Dummy/Ped, ConvertTo*, AddToPopulation, ...).
Se guardan y restauran todos los globales tocados (`state_restored`). Evento `layout`: comprueba desplazamientos/bits/enums esperados.

## Tabla de pares

| Funcion | Pares | Funcion | Pares |
|---|---|---|---|
| AddExistingPedInCar | 64 | ManageDummy | 256 |
| ArePedStatsCompatible | 1024 | ManagePed | 1024 |
| CanJeerAtStripper | 128 | ManagePopulation | 256 |
| CanSolicitPlayerInCar | 128 | MoveCarsAndPedsOutOfAbandonedZones | 64 |
| CanSolicitPlayerOnFoot | 64 | PedCreationDistMultiplier | 128 |
| ChooseCivilianCoupleOccupations | 512 | PedMICanBeCreatedAtAttractor | 128 |
| ChooseCivilianOccupation | 1024 | PedMICanBeCreatedAtThisAttractor | 1024 |
| ChooseCivilianOccupationForVehicle | 512 | PedMICanBeCreatedInInterior | 128 |
| ChooseGangOccupation | 64 | PickGangCar | 128 |
| ChoosePolicePedOccupation | 64 | PickRiotRoadBlockCar | 256 |
| ConvertAllObjectsToDummyObjects | 128 | PlaceGangMembers | 128 |
| ConvertToDummyObject | 256 | PlaceMallPedsAsStationaryGroup | 128 |
| ConvertToRealObject | 256 | RemoveAllRandomPeds | 128 |
| DealWithZoneChange | 64 | RemovePed | 128 |
| DoesCarGroupHaveModelId | 256 | RemovePedsIfThePoolGetsFull | 256 |
| FindCarMultiplierMotorway | 64 | RemoveSpecificDriverModelsForCar | 128 |
| FindDistanceToNearestPedOfType | 256 | Shutdown | 64 |
| FindDummyDistForModel | 256 | TestRoomForDummyObject | 256 |
| FindNumberOfPedsWeCanPlaceOnBenches | 512 | Update | 512 |
| FindPedDensityMultiplierCullZone | 64 | UpdatePedCount | 256 |
| FindPedMultiplierMotorway | 64 | IsCorrectTimeOfDayForEffect | 256 |
| FindPedRaceFromName | 256 | IsFemale / IsMale | 128 + 128 |
| FindSpecificDriverModelForCar_ToUse | 256 | IsSecurityGuard | 64 |
| GeneratePedsAtStartOfGame | 128 | IsSkateable | 256 |
| Initialise | 128 | IsSunbather-ModelID | 128 |
| LoadCarGroups / LoadPedGroups | 64 + 64 | LoadSpecificDriverModelsForCar | 128 |
| ManageAllPopulation | 256 | | |

Total 13376 pares (55 funciones).

## Fuera del banco (9) y por que

`AddPed` (0x612710), `AddDeadPedInFrontOfCar` (0x612CD0), `AddPedInCar` (0x613A00), `AddPedAtAttractor` (0x614210), `PlaceCouple` (0x613D60),
`CreateWaitingCoppers` (0x6133F0), `GeneratePedsAtAttractors` (0x615970), `PopulateInterior` (0x616470) y `TestSafeForRealObject` (0x6123A0).
Todas construyen peatones/coches reales: `call 0x5E4720`/`0x6E2D50` (operator new de pool) seguido de constructores del EXE (`call 0x5DDB70`, `0x5DDC60`,
`0x5DE340`, `0x6B0A90`) cuyo cuerpo en la DLL solo se alcanza por simbolo exportado, mas eventos y tareas reales apilados (`call 0x4AEDF0`, `0x4B0A00`, `0x4AF350`, `0x6836F0`,
`0x620E30`, `0x630040`), `CMatrix::UpdateRwMatrix`/`SetRotateZOnly`/`operator*` (EXE y DLL distintos: hooks de CMatrix no instalados), fsin/fcos/fpatan reales y, en
`TestSafeForRealObject`, las listas de sectores `0xB992B8` con nodos de pool reales y matrices asignadas bajo demanda (`call 0x54F560`). No se pueden doblar con seguridad sin
compilar para iterar; su fuente esta corregida (ver informe) y queda pendiente de banco.

## Ruido y supuestos a revisar al compilar

- Los nombres `Body(&Clase::Metodo)` de colaboradores sobrecargados o inline (p. ej. `CVisibilityPlugins::GetClumpAlpha`, `CWorld::ProcessVerticalLine`, `CGlass::IsObjectGlass`) pueden
  necesitar `static_cast`; si falta algun cuerpo el banco emite `error` con la lista y no ejecuta nada.
- `LoadPedGroups`/`LoadCarGroups` leen los ficheros de datos reales del juego (solo lectura); las tablas se rellenan con basura aleatoria antes y se comparan completas.
- Los EXE leen locales sin inicializar en caminos que el fixture evita (`ChooseCivilianCoupleOccupations` recibe `-1` en ambas salidas). `PickRiotRoadBlockCar` recorre las 23 ranuras sin
  mirar el recuento (como el EXE): los grupos del fixture solo contienen IDs de vehiculo validos.
- `TestRoomForDummyObject`/`ConvertTo*`: el EXE no comprueba el dummy nulo en `0x612320`; el fixture siempre lo da.
- Bits del peaton falso en +0x7B0 (guion de IsPlayer/CanBeDeleted/GetIsOnScreen): zona libre de campos reales de CPed (tamano 0x79C).

## Aprobación (09/10/2026)

Corrida `20261009-202521-590640`: passed=true, 13376 pares, 55 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_popfinal3.py` (evidencia en `runs/`, `validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.
