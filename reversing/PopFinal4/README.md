<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PopFinal4: banco por pares de las 9 funciones de CPopulation que quedaron fuera de PopFinal3

Funciones (pares): AddPed 1024, AddDeadPedInFrontOfCar 256, AddPedInCar 512, AddPedAtAttractor 256, PlaceCouple 512, CreateWaitingCoppers 512,
GeneratePedsAtAttractors 512, PopulateInterior 256, TestSafeForRealObject 256 (total 4096). Comando: `bash reversing/_handoff/bench.sh PopFinal4 3`.
Filtro: `popfinal4-only.txt` con `CPopulation::<Funcion>`. Sin compilar todavia (la primera pasada puede traer errores de nombre/sobrecarga en `Body(&...)`).

## Metodo
Reutiliza `reversing/PopFinal3/{PopSupport,PopCases1,PopCases2}.hpp` (sin editarlos) y anade `Pop4Support.hpp` (dobles y memoria falsa) y `Pop4Cases.hpp` (fixtures).
- `CPed::operator new` (0x5E4720), `CVehicle::operator new` (0x6E2D50) y `CTask::operator new` (0x61A5A0) devuelven huecos de buferes propios rellenos con 0xCD.
- Constructores de CCivilianPed (0x5DDB70), CCopPed (0x5DDC60), CEmergencyPed (0x5DE340) y CAutomobile (0x6B0A90): doblados en el EXE y en el simbolo exportado de la DLL
  (`??0CCivilianPed@@QAE@W4ePedType@@I@Z`, ...); fijan solo los campos que el codigo posterior lee (vtable, modelo, tipo, inteligencia, matriz, clump tipo 2).
- Tareas y eventos de pila son REALES (constructores/destructores del EXE y de la DLL); `CTaskManager::SetTask` (0x681AF0) y `CEventGroup::Add` (0x4AB420) se doblan y
  describen el tipo y las palabras escritas (los 0xCD de la arena sin escribir se enmascaran; en eventos solo los campos 0x10/0x12/0x14/0x18 y 0x1C/0x20 del atractor).
- Doblados con traza (EXE + cuerpo de la DLL, Body() antes de EntryPatch): CWorld::Add/Remove, FindPlayerPed/Vehicle, rand, RegisterReference/CleanUpOldReference, SetOrientation,
  SetPosn, armas, inteligencia (decision maker, ProcessEventHandler, Process), GetWalkAnimSpeed, atractores, CCarEnterExit, FindGroundZFor3DCoord, IsPositionClearForPed,
  IsSphereVisible(3), GetIsLineOfSightClear, FindObjectsInRange, ProcessColModels, SetClumpAlpha, CVector::Normalise (misma formula en ambos lados), rutas (FindNthNodeClosest,
  GeneratePedCreationCoors_Interior, nodos falsos en ThePaths.m_pPathNodes[5..7]), streaming/colstore, y las funciones de CPopulation que cada caso no prueba.
- `CMatrix::operator*` y `CPed::SetHeading` corren sin doblar (matrices identidad / ped sin matriz). Listas de sectores 0xB992B8 (3072 bytes), cabeceras de pools, ThePaths y todos los
  globales tocados se respaldan y restauran por caso. `JoinCarWithRoadSystem` (0x42F5A0) solo se parchea en el EXE (la DLL llama al EXE con plugin::Call).

## Hallazgos de fuente (esta ronda)
- CreateWaitingCoppers 0x6133F0: el EXE llama a 0x6D8470 = `CVehicle::UsesSiren`, no a `IsLawEnforcementVehicle` (0x6D2370). Corregido.

## Supuestos a revisar al compilar
- `CNodeAddress` se devuelve por puntero oculto en `FindNthNodeClosestToCoors` (EXE `push` previo a la llamada 0x44F8C0); si la DLL lo devuelve en EAX el doble de la DLL necesita otra firma.
- El offset de `m_nSurfaceTypeB` es 0x23 (corregido en PopFinal3); el evento `layout` valida los campos de CPed/CEntity/atractor/CWanted usados por offset crudo.
- Nombres exportados de los constructores (`??0CEmergencyPed@@QAE@W4ePedType@@I@Z` es una deduccion): si falta alguno el banco emite `error` con la lista.

## Aprobación (09/10/2026)

Corrida `20261009-214531-885595`: passed=true, 4096 pares, 9 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_popfinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CPopulation queda sin pendientes. Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.
