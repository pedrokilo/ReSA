# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-10-20 03:26 UTC

## Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. Completar este catálogo es un hito intermedio hacia **ReSA — Zero Original Function Dependency**.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Clases completas | `███████████████████░` | 782/818 · 95.60% |
| Clases validadas (de las completas) | `████████████████████` | 782/782 · 100.00% |
| Entradas de catálogo reconstruidas (ver [cobertura real del EXE](#cobertura-exe)) | `███████████████████░` | 8,729/9,200 · 94.88% |
| Funciones validadas (de las reconstruidas) | `████████████████████` | 8,729/8,729 · 100.00% |
| Funciones reconstruidas escritas solo en C++ (sin ensamblador) | `████████████████████` | 8,728/8,729 · 99.99% |

Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL. La última barra cuenta, de las funciones reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md).

**Migración de ensamblador a C++:** en `source/game_sa` ya no queda ningún `__asm` ni función `naked`: todo lo reconstruido está escrito en C++, con resultados validados contra el EXE dentro del juego. Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Historial y excepciones documentadas: [estado de la migración](../reversing/CppOnlyMigration/README.md) · [FILES.md](../reversing/CppOnlyMigration/FILES.md).

Hay 8,729 entradas catalogadas, 0 bloqueadas y 8,729 sin bloqueo. No constituyen un inventario exhaustivo del EXE. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Las extensiones o adaptaciones NOTSA (4, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

### Siguientes mediciones

Estas dimensiones todavía no tienen un porcentaje útil: indican el trabajo que falta para poder medirlas.

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validaciones vigentes para el árbol actual | 0/8,729 validadas confirmadas como vigentes | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |
| Dependencias de funciones originales eliminadas | Medido (cota inferior): 1,187 funciones del EXE siguen llamándose directamente desde código ya reemplazado (919 del juego, 210 RenderWare/D3D, 58 CRT); cierre transitivo del juego: 1,230 funciones / 334,828 bytes. Ver [Cobertura real del EXE](#cobertura-exe) | Resolver las llamadas indirectas (tablas de punteros, callbacks) para convertir la cota inferior en un valor exacto, y eliminar o sustituir las dependencias directas que quedan. |

**Validación consolidada parcialmente:** 8729 funciones con evidencia aprobada vinculada, 476 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias. Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

## Vista general

| Clases validadas | Completas por validar | Funciones validadas | Reconstruidas por validar | Funciones por reconstruir |
| :---: | :---: | :---: | :---: | :---: |
| **782 / 782** | **0 / 782** | **8,729 / 8,729** | **0 / 8,729** | **471 / 9,200** |
| 100.00% | 0.00% | 100.00% | 0.00% | 5.12% |

Reconstrucción por clases: 782 completas, 13 parciales y 23 sin revertir de 818 (incluye 471 funciones del juego con envoltura al EXE aún sin reconstruir, en 36 clases o grupos).

**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cohorte nuevo por dificultad](#cohorte-nuevo) · [Cómo leer y actualizar](#metodo)

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="cobertura-exe"></a>

## Cobertura real del EXE (estimación)

Las barras de arriba miden **filas del catálogo** (`hooks.csv`), no el EXE. Esta sección mide el propio ejecutable original con el análisis estático ([MemoryMap](MemoryMap/README.md): 17,765 entradas de función detectadas y 3.72 MB de cuerpos de función), cruzado con el catálogo, el registro de validación y las envolturas pendientes. 8,729 filas reconstruidas equivalen a **8,706 funciones distintas del EXE ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo).

| Región del EXE | Funciones | Bytes de código | Reemplazadas por C++ (funciones) | Reemplazadas (bytes) |
| --- | ---: | ---: | --- | --- |
| Juego y plataforma (0x401000–0x74FFFF) | 11,804 | 3,007,877 | `███████████████░░░░░` 8,636 · 73.2% | `████████████████░░░░` 82.3% |
| RenderWare y D3D (0x750000–0x81FFFF) | 1,889 | 600,971 | `█░░░░░░░░░░░░░░░░░░░` 61 · 3.2% | `░░░░░░░░░░░░░░░░░░░░` 0.3% |
| CRT/MSVC y runtime (0x820000 en adelante) | 4,072 | 115,711 | `░░░░░░░░░░░░░░░░░░░░` 9 · 0.2% | `░░░░░░░░░░░░░░░░░░░░` 0.2% |
| **Total** | **17,765** | **3,724,559** | `██████████░░░░░░░░░░` 8,706 · 49.0% | `█████████████░░░░░░░` 66.5% |

**Qué significa para un ejecutable independiente.** No hace falta reconstruir las 17,765 funciones: RenderWare/D3D y el CRT son bibliotecas enlazadas dentro del EXE (5,961 entradas, 19% de los bytes) que se sustituirían enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego (11,804 funciones, 81% de los bytes), y de él ya está reemplazado el **82% de los bytes** (73% de las funciones: las que quedan son más pequeñas que la media). Entre las 11,804 entradas del juego hay 1,835 de 16 bytes o menos (stubs, thunks y helpers).

### Lo que falta del código del juego: 3,168 funciones, 532 KB

Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).

| Grupo (de menos a más funciones) | Funciones | Bytes | Lectura |
| --- | ---: | ---: | --- |
| D. Sin llamadas directas ni vtable | 434 | 61,347 | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |
| C. Llamadas directas solo desde código aún no reemplazado | 577 | 78,751 | Dependen de reconstruir antes A y B; parte puede ser código muerto. |
| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | 927 | 56,914 | Probablemente necesarios: se ejecutan por despacho virtual. |
| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | 1,230 | 334,828 | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. |

Del código reemplazado, **1,187 funciones del EXE siguen llamándose directamente** (919 del juego, 210 de RenderWare/D3D y 58 del CRT). Esa es la lista corta de dependencias reales del original que hay que eliminar o sustituir.

> [!NOTE]
> **Cotas y límites.** Es un análisis estático: hay 6,004 llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.

<a id="no-en-cpp"></a>

### Funciones que ya tenemos y aún no están en C++: 471

Son envolturas: la DLL ya tiene la función y su hook, pero el cuerpo todavía llama al EXE original. Están separadas en tres secciones: **código del juego (270)**, que es el trabajo propio del proyecto, y las bibliotecas RenderWare/D3D (192) y CRT (9), que se sustituirían enlazando equivalentes. Las funciones del EXE sin nombre ni hook (el resto de lo que falta) están en el [mapa del código del juego](JuegoExe/README.md). Fuente: [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

#### Código del juego: 270 funciones en 32 clases o grupos

| Clase o grupo | Pendientes | Funciones (dirección en el EXE) |
| --- | ---: | --- |
| CAutomobile | 1 | `PreRender` 0x006AAB50 |
| CCollisionEventScanner | 1 | `ScanForCollisionEvents` 0x00604500 |
| CEventScanner | 1 | `ScanForEvents` 0x00607E30 |
| CMemoryHeap | 1 | `IntegrityCheck`  |
| CMentalState | 1 | `Process` 0x006008A0 |
| CPed | 1 | `ProcessEntityCollision` 0x005E2530 |
| CPedStuckChecker | 1 | `TestPedStuck` 0x00602C00 |
| CSearchLight | 1 | `IsSpottedEntity` 0x00493900 |
| CTaskSimpleSlideToCoord | 1 | `ProcessPed` 0x0066C4E0 |
| CTrainNode | 1 | `GetLightingFromCollision` 0x006F5F80 |
| CVehiclePotentialCollisionScanner | 1 | `ScanForVehiclePotentialCollisionEvents` 0x00603720 |
| FxInterpInfo255_c | 1 | `GetVal`  |
| FxInterpInfo32_c | 1 | `GetVal`  |
| FxInterpInfoFloat_c | 1 | `GetVal` 0x004A8470 |
| FxInterpInfoU255_c | 1 | `GetVal`  |
| CMissionCleanup | 2 | `CheckIfCollisionHasLoadedForMissionObjects` 0x004652D0<br>`Process` 0x00468560 |
| CRemote | 2 | `GivePlayerRemoteControlledCar` 0x0045AB10<br>`TakeRemoteControlledCarFromPlayer` 0x0045AE80 |
| CStoredCar | 2 | `RestoreCar` 0x00447E40<br>`StoreCar` 0x00449760 |
| CWorldScan | 2 | `SetExtraRectangleToScan` <br>`ScanWorld` 0x0072CAE0 |
| CTaskComplexLeaveCar | 4 | `MakeAbortable` 0x00641100<br>`CreateSubTask` 0x00641530<br>`CreateNextSubTask` 0x006419F0<br>`CreateFirstSubTask` 0x00641FC0 |
| CTaskComplexFollowPedFootsteps | 5 | `MakeAbortable` 0x00694ED0<br>`CreateNextSubTask` 0x00694EE0<br>`CreateFirstSubTask` 0x00695000<br>`ControlSubTask` 0x00695090<br>`CreateSubTask` 0x00695E40 |
| CTaskComplexKillPedOnFoot | 5 | `MakeAbortable` 0x00625E40<br>`CreateSubTask` 0x00625E70<br>`ControlSubTask` 0x00626260<br>`CreateNextSubTask` 0x0062B150<br>`CreateFirstSubTask` 0x0062B490 |
| CWaterLevel | 5 | `TestLineAgainstWater` 0x006E61B0<br>`FillQuadsAndTrianglesList` 0x006E7B30<br>`GetWaterDepth` 0x006EA960<br>`RenderHighDetailWaterRectangle` 0x006EB810<br>`RenderHighDetailWaterTriangle` 0x006EDDC0 |
| CPathFind | 6 | `GeneratePedCreationCoors_Interior` 0x0044ECA0<br>`FindNodeClosestToCoors` 0x0044F460<br>`FindNthNodeClosestToCoors` 0x0044F8C0<br>`FindParkingNodeInArea` 0x004513F0<br>`FindNextNodeWandering` 0x00451B70<br>`CompleteNewInterior` 0x00452270 |
| D3DResourceSystem | 7 | `CreateTexture` 0x00730510<br>`CreateIndexBuffer` 0x007306A0<br>`Init` 0x00730830<br>`CancelBuffering` 0x00730900<br>`Shutdown` 0x00730A00<br>`DestroyTexture` 0x00730B70<br>`DestroyIndexBuffer` 0x00730D30 |
| CGarage | 10 | `TidyUpGarage` <br>`TidyUpGarageClose` <br>`InitDoorsAtStart` 0x00447600<br>`PlayerArrestedOrDied` 0x004486C0<br>`IsEntityEntirelyInside3D` 0x00448BE0<br>`IsEntityEntirelyOutside` 0x00448D30<br>`RemoveCarsBlockingDoorNotInside` 0x00449690<br>`IsStaticPlayerCarEntirelyInside` 0x0044A830<br>`IsGarageEmpty` 0x0044A9C0<br>`Update` 0x0044AA50 |
| CHeli | 11 | `CHeli` 0x006C4190<br>`FindSwatPositionRelativeToHeli` 0x006C4760<br>`ProcessControlInputs` 0x006C4830<br>`ProcessFlyingCarStuff` 0x006C4E60<br>`PreRender` 0x006C5420<br>`SearchLightCone` 0x006C58E0<br>`GenerateHeli` 0x006C6520<br>`SendDownSwat` 0x006C69C0<br>`BlowUpCar` 0x006C6D30<br>`ProcessControl` 0x006C7050<br>`UpdateHelis` 0x006C79A0 |
| CPlayerPed | 13 | `ProcessAnimGroups` 0x006098F0<br>`SetRealMoveAnim` 0x0060A9C0<br>`Compute3rdPersonMouseTarget` 0x0060B650<br>`DrawTriangleForMouseRecruitPed` 0x0060BA80<br>`KeepAreaAroundPlayerClear` 0x0060C1E0<br>`MakeThisPedJoinOurGroup` 0x0060C840<br>`PlayerWantsToAttack` 0x0060CC50<br>`SetInitialState` 0x0060CD20<br>`EvaluateNeighbouringTarget` 0x0060D1C0<br>`ProcessGroupBehaviour` 0x0060D350<br>`ProcessWeaponSwitch` 0x0060D850<br>`FindWeaponLockOnTarget` 0x0060DC50<br>`FindNextWeaponLockOnTarget` 0x0060E530 |
| CVehicle | 17 | `AddReplacementUpgrade` 0x006D3830<br>`ProcessBikeWheel` 0x006D73B0<br>`InflictDamage` 0x006D7C90<br>`AddSingleWheelParticles` 0x006DE880<br>`RemoveVehicleUpgrade` 0x006DF930<br>`AddUpgrade` 0x006DFA20<br>`UpdateTrailerLink` 0x006DFC50<br>`UpdateTractorLink` 0x006E0050<br>`ScanAndMarkTargetForHeatSeekingMissile` 0x006E0400<br>`FireHeatSeakingMissile` 0x006E05C0<br>`PossiblyDropFreeFallBombForPlayer` 0x006E07E0<br>`ProcessSirenAndHorn` 0x006E0950<br>`AddVehicleUpgrade` 0x006E3290<br>`GetPlaneWeaponFiringStatus` 0x006E3440<br>`ProcessWeapons` 0x006E3950<br>`FireFixedMachineGuns` 0x0073DF00<br>`DoDriveByShootings` 0x00741FD0 |
| Fx_c | 17 | `AddBlood` 0x0049EB00<br>`AddWood` 0x0049EE10<br>`AddSparks` 0x0049F040<br>`AddTyreBurst` 0x0049F300<br>`AddBulletImpact` 0x0049F3D0<br>`AddPunchImpact` 0x0049F670<br>`AddDebris` 0x0049F750<br>`AddGlass` 0x0049F970<br>`AddWheelSpray` 0x0049FB30<br>`AddWheelGrass` 0x0049FF20<br>`AddWheelGravel` 0x004A0170<br>`AddWheelMud` 0x004A03C0<br>`AddWheelSand` 0x004A0610<br>`AddWheelDust` 0x004A09C0<br>`TriggerWaterHydrant` 0x004A0D70<br>`TriggerGunshot` 0x004A0DE0<br>`TriggerTankFire` 0x004A0FA0 |
| (helpers y globales sin clase) | 67 | `_rwD3D9SetStreams` <br>`_rwObjectHasFrameSetFrame` <br>`CameraControlGarageDoors` <br>`ComputeTargetDoorToExit` <br>`ExeFree` <br>`FightChooseAttackAIExe` <br>`FightChooseAttackPlayerExe` <br>`FightFindTargetOnGroundExe` <br>`FightGetAvailableComboSetExe` <br>`FightStartAnimExe` <br>`GetFirstTextureCallback` <br>`IndexDataBufferGetTotalDataSize` <br>`IndexDataBufferPop` <br>`IndexDataBufferResize` <br>`operator delete` <br>`operator delete[]` <br>`operator new` <br>`operator new[]` <br>`RpClumpGtaCancelStream` <br>`RpClumpGtaStreamRead1` <br>`RpClumpGtaStreamRead2` <br>`RpMaterialUVAnimApplyUpdate` <br>`RwEngineGetVideoModeInfo` <br>`RwTexDictionaryGtaStreamRead1` <br>`RwTexDictionaryGtaStreamRead2` <br>`RwTextureRead` <br>`RxLockedPipeAddFragment` <br>`StencilTransformPoints` <br>`TextureBufferGetTotalDataSize` <br>`TextureBufferPop` <br>`TextureBufferPush` <br>`TextureBufferPushWithoutIncreasingCounter` <br>`TextureBufferResize` <br>`TextureBufferSetup` <br>`Selector de modelo de ped para ataque de bandas, sin símbolo confirmado` 0x00406E80<br>`CGarage unnamed camera door helper` 0x00449FF0<br>`CPathFind generador de coordenadas de ped sin símbolo confirmado` 0x0044E790<br>`CAEMFDecoder helper abrir movie/media sonido` 0x004E7990<br>`CAEMFDecoder helper obtener formato media` 0x004E7A80<br>`CCam helper arrest camera de from/initialise sin símbolo confirmado` 0x00512EF0<br>`CCamera unnamed geometry helper` 0x00514030<br>`CCam unnamed arrest cop-source helper` 0x00515D80<br>`CCam helper copSource alternativo=true sin símbolo confirmado` 0x00516010<br>`GetMillisecondTime` 0x005617C0<br>`TransformPoint` 0x0059C890<br>`RsLoadPresetViews` 0x00619D60<br>`RsSavePresetView` 0x00619FA0<br>`helper sin símbolo confirmado` 0x0061A3D0<br>`FightStrikeDamageExe` 0x0061C740<br>`FightHitPedExe` 0x0061CBA0<br>`FightHitObjExe` 0x0061D400<br>`CTaskSimpleGangDriveBy helper de animación sin símbolo confirmado` 0x00627B20<br>`CTaskSimpleGangDriveBy helper para shooter sin símbolo confirmado` 0x00627CC0<br>`CreateRouteTask` 0x00655140<br>`RemoveUpgradeCB` 0x006D3300<br>`FindUpgradeCB` 0x006D3370<br>`CWaterLevel helper de agua distante` 0x006E6870<br>`CWaterLevel helper de agua cercana` 0x006E6A10<br>`CWaterLevel helper de envío de agua acumulada sin símbolo confirmado` 0x006E7680<br>`Creación de objeto toalla para sunbathe sin símbolo confirmado` 0x006EABA0<br>`CFont helper de impresión de segmento sin símbolo confirmado` 0x00719B40<br>`RwTextureGtaStreamRead` 0x00730E60<br>`RwTexDictionaryGtaStreamRead` 0x00730FC0<br>`RpClumpGetBoundingSphere` 0x00734FC0<br>`SkinGetBonePositionsToTable` 0x00735360<br>`GetDXVersion` 0x00745650<br>`DIReleaseMouse` 0x00746F70 |
| CCarCtrl | 80 | `SetUpDriverAndPassengersForVehicle` 0x004217C0<br>`ThisVehicleShouldTryNotToTurn` 0x00421FE0<br>`FindPathDirection` 0x00422090<br>`SwitchBetweenPhysicsAndGhost` 0x004222A0<br>`FindSpeedMultiplier` 0x004224E0<br>`StopCarIfNodesAreInvalid` 0x00422590<br>`FindPercDependingOnDistToLink` 0x00422620<br>`FindIntersection2Lines` 0x004226F0<br>`ClitargetOrientationToLink` 0x00422760<br>`SteerAICarBlockingPlayerForwardAndBack` 0x00422B20<br>`FlyAIPlaneInCertainDirection` 0x00423000<br>`SteerAIPlaneTowardsTargetCoors` 0x00423790<br>`SteerAIPlaneToFollowEntity` 0x004237F0<br>`SteerAIPlaneToCrashAndBurn` 0x00423880<br>`SteerAIHeliToCrashAndBurn` 0x004238E0<br>`FlyAIHeliToTarget_FixedOrientation` 0x00423940<br>`GenerateCarCreationCoors2` 0x00424210<br>`ChooseModel` 0x00424CE0<br>`PossiblyRemoveVehicle` 0x00424F80<br>`SlowCarDownForPedsSectorList` 0x00425440<br>`TestCollisionBetween2MovingRects` 0x00425B30<br>`TestCollisionBetween2MovingRects_OnlyFrontBumper` 0x00425F70<br>`WeaveForOtherCar` 0x00426350<br>`WeaveForObject` 0x00426BC0<br>`PickNextNodeToChaseCar` 0x00426EF0<br>`PickNextNodeToFollowPath` 0x00427740<br>`DealWithBend_Racing` 0x00428040<br>`SteerAICarWithPhysicsTryingToBlockTarget_Stop` 0x00428990<br>`SteerAIBoatWithPhysicsHeadingForTarget` 0x00428BE0<br>`SteerAIBoatWithPhysicsAttackingPlayer` 0x00428DE0<br>`SteerAIBoatWithPhysicsCirclingPlayer` 0x00429090<br>`TriggerDogFightMoves` 0x00429300<br>`TestWhetherToFirePlaneGuns` 0x00429520<br>`GetAIPlaneToAttackPlayer` 0x00429780<br>`GetAIPlaneToDoDogFight` 0x00429890<br>`FlyAIHeliInCertainDirection` 0x00429A70<br>`SteerAIHeliTowardsTargetCoors` 0x0042A630<br>`GetAIHeliToFlyInDirection` 0x0042A730<br>`SteerAIHeliToFollowEntity` 0x0042A750<br>`SteerAIHeliAsPoliceHeli` 0x0042AAD0<br>`SteerAIHeliFlyingAwayFromPlayer` 0x0042ACB0<br>`SteerAIHeliToLand` 0x0042AD30<br>`SteerAIHeliToKeepEntityInView` 0x0042AEB0<br>`FireHeliRocketsAtTarget` 0x0042B270<br>`FindLinksToGoWithTheseNodes` 0x0042B470<br>`GenerateOneEmergencyServicesCar` 0x0042B7D0<br>`FindNodesThisCarIsNearestTo` 0x0042BD20<br>`CreatePoliceChase` 0x0042C2B0<br>`CreateConvoy` 0x0042C740<br>`ScanForPedDanger` 0x0042CE40<br>`SlowCarDownForOtherCar` 0x0042D0E0<br>`SlowCarDownForObjectsSectorList` 0x0042D4F0<br>`WeaveThroughCarsSectorList` 0x0042D680<br>`WeaveThroughPedsSectorList` 0x0042D7E0<br>`WeaveThroughObjectsSectorList` 0x0042D950<br>`IsThisAnAppropriateNode` 0x0042DAB0<br>`PickNextNodeRandomly` 0x0042DE80<br>`DragCarToPoint` 0x0042EC90<br>`GetAIPlaneToDoDogFightAgainstPlayer` 0x0042F370<br>`GetAIHeliToAttackPlayer` 0x0042F3C0<br>`JoinCarWithRoadSystem` 0x0042F5A0<br>`JoinCarWithRoadSystemGotoCoors` 0x0042F870<br>`ReconsiderRoute` 0x0042FC40<br>`GenerateOneRandomCar` 0x00430050<br>`SlowCarDownForCarsSectorList` 0x00432420<br>`FindAngleToWeaveThroughTraffic` 0x004325C0<br>`PickNextNodeAccordingStrategy` 0x00432B10<br>`SteerAICarWithPhysicsFollowPreRecordedPath` 0x00432DD0<br>`SteerAICarWithPhysicsHeadingForTarget` 0x00433280<br>`SteerAICarWithPhysicsTryingToBlockTarget` 0x004335E0<br>`SteerAICarTowardsPointInEscort` 0x004336D0<br>`SteerAICarParkParallel` 0x00433BA0<br>`SteerAICarParkPerpendicular` 0x00433EA0<br>`SetCoordsOfScriptCar` 0x004342A0<br>`FindMaximumSpeedForThisCarInTraffic` 0x00434400<br>`SteerAICarWithPhysicsFollowPath` 0x00434900<br>`SteerAICarWithPhysicsFollowPath_Racing` 0x00435830<br>`UpdateCarOnRails` 0x00436540<br>`SteerAICarWithPhysics_OnlyMission` 0x00436A90<br>`SteerAICarWithPhysics` 0x00437C20 |

#### RenderWare y D3D (biblioteca): 192

| Clase o grupo | Funciones | Pendientes |
| --- | ---: | --- |
| (biblioteca) Controlador D3D9 de RenderWare | 58 | `_rxD3D9VideoMemoryRasterListRestore` 0x004CC970, `_rwD3D9RasterConvertToNonPalettized` 0x004CD250, `_rpD3D9AddDynamicGeometry` 0x0074E970, `_rpD3D9RemoveDynamicGeometry` 0x0074E9E0, `_rpD3D9VertexDeclarationInstWeights` 0x00752320, `_rpD3D9VertexDeclarationInstV3d` 0x00752AD0 … y 52 más |
| (biblioteca) RenderWare SDK | 134 | `RpAtomicGetWorldBoundingSphere` 0x00749330, `RpClumpForAllCameras` 0x00749BB0, `RpClumpForAllLights` 0x00749C00, `RpClumpRemoveLight` 0x0074A520, `RpGeometryAddMorphTargets` 0x0074C310, `RpGeometryStreamRead` 0x0074D190 … y 128 más |

<details>
<summary>Lista completa: renderware y d3d (biblioteca)</summary>

**(biblioteca) Controlador D3D9 de RenderWare** (58)

- `_rxD3D9VideoMemoryRasterListRestore` — 0x004CC970
- `_rwD3D9RasterConvertToNonPalettized` — 0x004CD250
- `_rpD3D9AddDynamicGeometry` — 0x0074E970
- `_rpD3D9RemoveDynamicGeometry` — 0x0074E9E0
- `_rpD3D9VertexDeclarationInstWeights` — 0x00752320
- `_rpD3D9VertexDeclarationInstV3d` — 0x00752AD0
- `_rpD3D9VertexDeclarationInstV3dMorph` — 0x00753B60
- `_rpD3D9VertexDeclarationInstV2d` — 0x007544E0
- `_rpD3D9VertexDeclarationInstColor` — 0x00754AE0
- `_rpD3D9VertexDeclarationInstIndicesRemap` — 0x00754C80
- `_rpD3D9VertexDeclarationInstTangent` — 0x00754E20
- `_rpD3D9VertexDeclarationUnInstV3d` — 0x007551F0
- `_rpD3D9VertexDeclarationUnInstV2d` — 0x007555E0
- `_rpD3D9GetMinMaxValuesV2d` — 0x007558F0
- `_rpD3D9FindFormatV2d` — 0x00755AA0
- `_rwD3D9LightsOpen` — 0x00755D80
- `_rwD3D9LightsClose` — 0x00755FE0
- `_rpD3D9VertexShaderCachePurge` — 0x0075EE60
- `_rpD3D9VertexShaderSetEnvMatrix` — 0x00761A10
- `_rpD3D9VertexShaderSetBumpMatrix` — 0x00761B70
- `_rwD3D9VSSetActiveWorldMatrix` — 0x00764650
- `_rwD3D9VSGetComposedTransformMatrix` — 0x007646E0
- `_rwD3D9VSGetWorldViewTransposedMatrix` — 0x00764730
- `_rwD3D9VSGetInverseWorldMatrix` — 0x007647B0
- `_rwD3D9VSGetWorldMultiplyTransposeMatrix` — 0x00764960
- `_rwD3D9VSGetWorldViewMultiplyTransposeMatrix` — 0x007649D0
- `_rwD3D9VSGetWorldNormalizedMultiplyTransposeMatrix` — 0x00764A70
- `_rwD3D9VSGetWorldNormalizedViewMultiplyTransposeMatrix` — 0x00764B50
- `_rwD3D9VSGetWorldNormalizedTransposeMatrix` — 0x00764C60
- `_rwD3D9VSGetNormalInLocalSpace` — 0x00764D30
- `_rwD3D9VSGetPointInLocalSpace` — 0x00764E70
- `_rwD3D9VSGetRadiusInLocalSpace` — 0x00764F60
- `RwD3D9CreateVertexBuffer` — 0x007F5500
- `RwD3D9DestroyVertexBuffer` — 0x007F56A0
- `_rwD3D9DynamicVertexBufferRestore` — 0x007F58D0
- `RwD3D9DynamicVertexBufferCreate` — 0x007F5A00
- `RwD3D9DynamicVertexBufferLock` — 0x007F5B10
- `_rwD3D9SetRenderTarget` — 0x007F5F20
- `_rwD3D9DeviceReleaseVideoMemory` — 0x007F7F70
- `RwD3D9CameraAttachWindow` — 0x007F8D70
- `RwD3D9DeviceSupportsDXTTexture` — 0x007F9C30
- `RwD3D9SetRenderTarget` — 0x007F9E90
- `_rwD3D9SetVertexShader` — 0x007F9FB0
- `_rwD3D9SetPixelShader` — 0x007F9FF0
- `RwD3D9SetStreamSource` — 0x007FA030
- `_rwD3D9DrawIndexedPrimitive` — 0x007FA320
- `RwD3D9GetTransform` — 0x007FA4F0
- `RwD3D9SetTransformWorld` — 0x007FA520
- `RwD3D9CreateVertexDeclaration` — 0x007FAA30
- `RwD3D9DeleteVertexDeclaration` — 0x007FAC10
- `RwD3D9CameraIsSphereFullyInsideFrustum` — 0x007FAD30
- `RwD3D9CameraIsBBoxFullyInsideFrustum` — 0x007FAD90
- `_rwD3D9Im2DRenderOpen` — 0x007FB480
- `_rwD3D9RenderStateFlushCache` — 0x007FC200
- `RwD3D9SetSamplerState` — 0x007FC3C0
- `RwD3D9SetMaterial` — 0x007FC430
- `_rwD3D9RenderStateReset` — 0x007FD100
- `_rwD3D9Im3DRenderOpen` — 0x0080E020

**(biblioteca) RenderWare SDK** (134)

- `RpAtomicGetWorldBoundingSphere` — 0x00749330
- `RpClumpForAllCameras` — 0x00749BB0
- `RpClumpForAllLights` — 0x00749C00
- `RpClumpRemoveLight` — 0x0074A520
- `RpGeometryAddMorphTargets` — 0x0074C310
- `RpGeometryStreamRead` — 0x0074D190
- `_rpMaterialSetDefaultSurfaceProperties` — 0x0074D870
- `RpMaterialStreamRead` — 0x0074DD30
- `RpMaterialStreamGetSize` — 0x0074E010
- `_rpMaterialListStreamGetSize` — 0x0074E450
- `_rpMaterialListStreamRead` — 0x0074E600
- `_rpWorldSectorDeinstanceAll` — 0x0074ECA0
- `_rpWorldSectorDestroyRecurse` — 0x0074ED50
- `_rpSectorDefaultRenderCallBack` — 0x0074EEC0
- `RpWorldUnlock` — 0x0074F210
- `RpWorldSectorGetWorld` — 0x0074F4E0
- `RpWorldForAllMaterials` — 0x0074FBC0
- `RpWorldForAllWorldSectors` — 0x0074FC70
- `RpWorldAddClump` — 0x00751300
- `RpLightForAllWorldSectors` — 0x007519F0
- `RpLightStreamRead` — 0x00751F00
- `_rpBuildMeshCreate` — 0x00758A90
- `_rpBuildMeshDestroy` — 0x00758B80
- `_rpMeshDestroy` — 0x00758BC0
- `_rpBuildMeshAddTriangle` — 0x00758C00
- `_rpMeshHeaderForAllMeshes` — 0x00758D30
- `_rpMeshWrite` — 0x00758D70
- `_rpMeshRead` — 0x00758EC0
- `_rpMeshSize` — 0x00759090
- `_rpTriListMeshGenerate` — 0x0075D530
- `_rpMeshOptimise` — 0x0075D970
- `RpWorldSetDefaultSectorPipeline` — 0x0075E270
- `RpAtomicSetDefaultPipeline` — 0x0075E2A0
- `RpHAnimHierarchyCreate` — 0x007C4C30
- `RpHAnimKeyFrameStreamRead` — 0x007C64C0
- `_rpSkinInitialize` — 0x007C8740
- `_rpSkinSplitDataCreate` — 0x007C8A70
- `_rpSkinSplitDataDestroy` — 0x007C8B10
- `_rpUVAnimCustomDataStreamRead` — 0x007CBF70
- `_rpUVAnimCustomDataStreamWrite` — 0x007CBFD0
- `RpUVAnimDestroy` — 0x007CC0C0
- `RpMaterialSetUVAnim` — 0x007CC3B0
- `RtAnimInterpolatorCreate` — 0x007CD520
- `RtAnimInterpolatorAddAnimTime` — 0x007CD8D0
- `RtDictSchemaInit` — 0x007CED40
- `_rtDictDestruct` — 0x007CEF60
- `RtDictFindNamedEntry` — 0x007CEFE0
- `RtDictForAllEntries` — 0x007CF060
- `RtDictRemoveEntry` — 0x007CF0C0
- `RtDictStreamWrite` — 0x007CF490
- `RtDictSchemaForAllDictionaries` — 0x007CF5B0
- `RtPNGImageRead` — 0x007CF9B0
- `_rwStreamWriteVersionedChunkHeader` — 0x007ED270
- `RwStreamWriteReal` — 0x007ED3D0
- `RwStreamReadReal` — 0x007ED4F0
- `RwStreamReadInt32` — 0x007ED540
- `_rwSqrt` — 0x007EDB30
- `_rwInvSqrt` — 0x007EDB90
- `RwIm3DSetTransformPipeline` — 0x007EFAC0
- `RwIm3DSetRenderPipeline` — 0x007EFB10
- `_rwFramePurgeClone` — 0x007F01A0
- `RwFrameDestroyHierarchy` — 0x007F08A0
- `RwFrameAddChildNoUpdate` — 0x007F09C0
- `_rwMatrixDeterminant` — 0x007F1450
- `_rwMatrixOrthogonalError` — 0x007F14A0
- `_rwMatrixNormalError` — 0x007F1500
- `_rwMatrixIdentityError` — 0x007F1590
- `RwMatrixOptimize` — 0x007F17E0
- `RwMatrixRotateOneMinusCosineSine` — 0x007F1D00
- `RwEngineGetCurrentVideoMode` — 0x007F2D20
- `RwTextureSetMaskName` — 0x007F3910
- `RwRasterGetNumLevels` — 0x007FB160
- `RwFreeListDestroy` — 0x00801B80
- `RwFreeListPurge` — 0x00801E00
- `RwFreeListForAllUsed` — 0x00801E90
- `RwFreeListPurgeAllFreeLists` — 0x00801F90
- `RwImageFreePixels` — 0x00802860
- `RwImageMakeMask` — 0x00802A20
- `RwImageApplyMask` — 0x00802AF0
- `RwImageRead` — 0x00802FD0
- `RwImageReadMaskedImage` — 0x008035C0
- `RwImageGammaCorrect` — 0x00803E30
- `RwImageSetGamma` — 0x00803FE0
- `RwTextureStreamGetSize` — 0x008045A0
- `RwTextureStreamWrite` — 0x008045E0
- `RwTextureStreamRead` — 0x008046E0
- `_rxPipelineClose` — 0x00804F60
- `_rxPipelineOpen` — 0x00804FE0
- `RxPipelineExecute` — 0x00805710
- `RxPipelineNodeCloneDefinition` — 0x008059C0
- `_rwFrameListStreamRead` — 0x00807970
- `RwResourcesFreeResEntry` — 0x00807DE0
- `_rwResourcesPurge` — 0x00807E50
- `RwResourcesEmptyArena` — 0x008081F0
- `_rwPluginRegistryAddPlugin` — 0x008084A0
- `_rwPluginRegistryInitObject` — 0x008086E0
- `_rwPluginRegistryDeInitObject` — 0x00808740
- `_rwPluginRegistryCopyObject` — 0x00808770
- `RwErrorSet` — 0x00808820
- `RwErrorGet` — 0x00808880
- `_rwPluginRegistryAddPluginStream` — 0x008088E0
- `_rwPluginRegistryAddPlgnStrmlwysCB` — 0x00808920
- `_rwPluginRegistryAddPlgnStrmRightsCB` — 0x00808950
- `_rwPluginRegistryReadDataChunks` — 0x00808980
- `_rwPluginRegistryInvokeRights` — 0x00808AB0
- `_rwPluginRegistryGetSize` — 0x00808B00
- `_rwPluginRegistryWriteDataChunks` — 0x00808B40
- `_rwPluginRegistrySkipDataChunks` — 0x00808C10
- `RwCameraStreamRead` — 0x00808DE0
- `RwBBoxCalculate` — 0x00808F60
- `_rwSListCreate` — 0x00809160
- `_rwSListGetNewEntry` — 0x00809240
- `_rwSListDestroy` — 0x00809440
- `_rwFrameSyncDirty` — 0x00809550
- `_rwFrameSyncHierarchyLTM` — 0x008097D0
- `RxHeapFree` — 0x008098B0
- `_rxHeapReset` — 0x00809EC0
- `RxHeapDestroy` — 0x00809F30
- `RxHeapCreate` — 0x00809F90
- `_rwStringOpen` — 0x0080A240
- `_rwStringStreamWrite` — 0x0080A4A0
- `_rwStringStreamFindAndRead` — 0x0080A510
- `RwPalQuantAddImage` — 0x0080AA80
- `RwPalQuantResolvePalette` — 0x0080AF60
- `RwPalQuantMatchImage` — 0x0080BF20
- `RwPalQuantInit` — 0x0080C470
- `RwPalQuantTerm` — 0x0080C520
- `RwImageResample` — 0x0080C600
- `RwImageCreateResample` — 0x0080CD10
- `RxRenderStateVectorSetDefaultRenderStateVector` — 0x0080EA40
- `_rxPacketDestroy` — 0x0080F070
- `RpMatFXMaterialSetBumpMapTexture` — 0x00812160
- `RpMatFXMaterialSetDualBlendModes` — 0x008128C0
- `RpMatFXMaterialSetUVTransformMatrices` — 0x008129E0

</details>

#### CRT y runtime (biblioteca): 9

| Clase o grupo | Funciones | Pendientes |
| --- | ---: | --- |
| (biblioteca) QuickTime/QTML | 2 | `QuickTime runtime shutdown helper` 0x00819770, `QuickTime dispatch thunk` 0x00819840 |
| (biblioteca) CRT / runtime estático | 7 | `helper sin símbolo confirmado` 0x008219F0, `_sscanf` 0x008220AD, `_pow` 0x008220F0, `_free` 0x0082413F, `Realloc` 0x00824269, `__stbuf` 0x00827DEC … y 1 más |

<details>
<summary>Lista completa: crt y runtime (biblioteca)</summary>

**(biblioteca) QuickTime/QTML** (2)

- `QuickTime runtime shutdown helper` — 0x00819770
- `QuickTime dispatch thunk` — 0x00819840

**(biblioteca) CRT / runtime estático** (7)

- `helper sin símbolo confirmado` — 0x008219F0
- `_sscanf` — 0x008220AD
- `_pow` — 0x008220F0
- `_free` — 0x0082413F
- `Realloc` — 0x00824269
- `__stbuf` — 0x00827DEC
- `__ftbuf` — 0x00827E74

</details>

<a id="completas"></a>
## 🟢 Completas · 782 clases

Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (7) ✅ 7 | [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (7) ✅ 7 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (17) ✅ 17 | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (3) ✅ 3 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (8) ✅ 8 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (2) ✅ 2 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (6) ✅ 6 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CMaths](../source/game_sa/Maths.cpp) (3) ✅ 3 | [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) (9) ✅ 9 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (6) ✅ 6 | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (11) ✅ 11 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (10) ✅ 10 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (7) ✅ 7 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CMemoryMgr](../source/game_sa/MemoryMgr.cpp) (5) ✅ 5 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) ✅ 9 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CMenuManager](../source/game_sa/MenuManager.cpp) (61) ✅ 61 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) ✅ 7 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (7) ✅ 7 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (5) ✅ 5 | [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) (8) ✅ 8 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (5) ✅ 5 | [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (4) ✅ 4 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (4) ✅ 4 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (29) ✅ 29 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) ✅ 8 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (7) ✅ 7 | [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) (11) ✅ 11 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (2) ✅ 2 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (12) ✅ 12 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (2) ✅ 2 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (2) ✅ 2 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (5) ✅ 5 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (11) ✅ 11 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (55) ✅ 55 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (2) ✅ 2 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (7) ✅ 7 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (8) ✅ 8 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (6) ✅ 6 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (49) ✅ 49 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (6) ✅ 6 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (7) ✅ 7 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (8) ✅ 8 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) ✅ 2 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (5) ✅ 5 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (20) ✅ 20 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) ✅ 7 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (8) ✅ 8 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (3) ✅ 3 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (7) ✅ 7 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [Common](../source/game_sa/common.cpp) (11) ✅ 11 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (7) ✅ 7 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) (6) ✅ 6 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskComplexWaitForPed](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) (1) ✅ 1 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (6) ✅ 6 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (6) ✅ 6 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (9) ✅ 9 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (10) ✅ 10 | [CPad](../source/game_sa/Pad.cpp) (79) ✅ 79 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (12) ✅ 12 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (13) ✅ 13 | [CPathNode](../source/game_sa/PathFind.cpp) (1) ✅ 1 | [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) (13) ✅ 13 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (7) ✅ 7 | [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) (20) ✅ 20 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (8) ✅ 8 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) (20) ✅ 20 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) ✅ 14 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (4) ✅ 4 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (11) ✅ 11 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (14) ✅ 14 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (58) ✅ 58 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (9) ✅ 9 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (7) ✅ 7 | [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) (6) ✅ 6 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) (17) ✅ 17 | [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) (6) ✅ 6 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (6) ✅ 6 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) (43) ✅ 43 | [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) (38) ✅ 38 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (4) ✅ 4 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) (19) ✅ 19 | [CPedGroup](../source/game_sa/PedGroup.cpp) (12) ✅ 12 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (4) ✅ 4 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (10) ✅ 10 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (2) ✅ 2 | [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) (5) ✅ 5 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (18) ✅ 18 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (2) ✅ 2 | [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) (9) ✅ 9 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (14) ✅ 14 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (2) ✅ 2 | [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) (5) ✅ 5 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (8) ✅ 8 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (2) ✅ 2 | [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) (8) ✅ 8 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) (24) ✅ 24 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (2) ✅ 2 | [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) (7) ✅ 7 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (3) ✅ 3 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (2) ✅ 2 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (8) ✅ 8 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (14) ✅ 14 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (27) ✅ 27 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (8) ✅ 8 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (2) ✅ 2 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (18) ✅ 18 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (6) ✅ 6 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) (4) ✅ 4 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (7) ✅ 7 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (7) ✅ 7 | [CPedGroups](../source/game_sa/PedGroups.cpp) (11) ✅ 11 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) ✅ 7 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) ✅ 15 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (7) ✅ 7 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) (106) ✅ 106 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (53) ✅ 53 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (9) ✅ 9 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (13) ✅ 13 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskInteriorSitInChair](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp) (1) ✅ 1 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (7) ✅ 7 | [CPedList](../source/game_sa/PedList.cpp) (7) ✅ 7 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (8) ✅ 8 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (9) ✅ 9 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) ✅ 9 | [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) (5) ✅ 5 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (3) ✅ 3 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (18) ✅ 18 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (5) ✅ 5 | [CPedPool](../source/game_sa/Pools/PedPool.h) (3) ✅ 3 | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (11) ✅ 11 | [CPedSaveStructure](../source/game_sa/Entity/Ped/PedSaveStructure.cpp) (2) ✅ 2 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (2) ✅ 2 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (3) ✅ 3 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (5) ✅ 5 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (5) ✅ 5 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (20) ✅ 20 | [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) (4) ✅ 4 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (1) ✅ 1 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) ✅ 6 | [CPedStats](../source/game_sa/PedStats.cpp) (4) ✅ 4 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (3) ✅ 3 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) ✅ 8 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (8) ✅ 8 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (14) ✅ 14 | [CPedType](../source/game_sa/PedType.cpp) (13) ✅ 13 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (10) ✅ 10 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (6) ✅ 6 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (57) ✅ 57 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (10) ✅ 10 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (31) ✅ 31 | [CPickup](../source/game_sa/Pickup.cpp) (12) ✅ 12 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (10) ✅ 10 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (9) ✅ 9 | [CPickups](../source/game_sa/Pickups.cpp) (38) ✅ 38 | [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) (13) ✅ 13 |
| [CAttractorScanner](../source/game_sa/AttractorScanner.cpp) (1) ✅ 1 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (16) ✅ 16 | [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) (5) ✅ 5 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (82) ✅ 82 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (9) ✅ 9 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (5) ✅ 5 | [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) (23) ✅ 23 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (9) ✅ 9 |
| [CAutoPilot](../source/game_sa/AutoPilot.cpp) (2) ✅ 2 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (10) ✅ 10 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (34) ✅ 34 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (6) ✅ 6 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) (40) ✅ 40 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (10) ✅ 10 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (10) ✅ 10 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) (10) ✅ 10 | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (20) ✅ 20 | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (9) ✅ 9 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (23) ✅ 23 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) (5) ✅ 5 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (28) ✅ 28 | [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) (5) ✅ 5 |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) ✅ 2 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) ✅ 5 | [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) (5) ✅ 5 |
| [CBridge](../source/game_sa/Bridge.cpp) (4) ✅ 4 | [CPlayerRelationshipRecorder](../source/game_sa/PlayerRelationshipRecorder.cpp) (1) ✅ 1 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (6) ✅ 6 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (6) ✅ 6 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (2) ✅ 2 | [CPointLights](../source/game_sa/PointLights.cpp) (7) ✅ 7 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (9) ✅ 9 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (9) ✅ 9 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPools](../source/game_sa/Pools.cpp) (19) ✅ 19 | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (10) ✅ 10 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (7) ✅ 7 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (9) ✅ 9 | [CPopulation](../source/game_sa/Population.cpp) (67) ✅ 67 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (6) ✅ 6 |
| [CCam](../source/game_sa/Cam.cpp) (48) ✅ 48 | [CPostEffects](../source/game_sa/PostEffects.cpp) (38) ✅ 38 | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (3) ✅ 3 |
| [CCamera](../source/game_sa/Camera.cpp) (118) ✅ 118 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) (7) ✅ 7 |
| [CCarAI](../source/game_sa/CarAI.cpp) (25) ✅ 25 | [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) (12) ✅ 12 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (6) ✅ 6 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) (32) ✅ 32 | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (1) ✅ 1 | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (15) ✅ 15 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) (10) ✅ 10 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (1) ✅ 1 | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (4) ✅ 4 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (8) ✅ 8 | [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) (12) ✅ 12 | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (6) ✅ 6 |
| [CCheat](../source/game_sa/Cheat.cpp) (72) ✅ 72 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (12) ✅ 12 | [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) (6) ✅ 6 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CQuaternion](../source/game_sa/Core/Quaternion.cpp) (10) ✅ 10 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (7) ✅ 7 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CRadar](../source/game_sa/Radar.cpp) (65) ✅ 65 | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (5) ✅ 5 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (3) ✅ 3 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (9) ✅ 9 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (6) ✅ 6 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) (7) ✅ 7 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (1) ✅ 1 |
| [CClothes](../source/game_sa/Clothes.cpp) (11) ✅ 11 | [CRect](../source/game_sa/Core/Rect.cpp) (8) ✅ 8 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (1) ✅ 1 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) (23) ✅ 23 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (15) ✅ 15 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (6) ✅ 6 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (24) ✅ 24 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (8) ✅ 8 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CRenderer](../source/game_sa/Renderer.cpp) (33) ✅ 33 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (8) ✅ 8 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (1) ✅ 1 | [CReplay](../source/game_sa/Replay.cpp) (47) ✅ 47 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (5) ✅ 5 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (61) ✅ 61 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (6) ✅ 6 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (8) ✅ 8 | [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) (8) ✅ 8 | [CTaskSimpleFight](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) (12) ✅ 12 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CRope](../source/game_sa/Rope.cpp) (7) ✅ 7 | [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) (7) ✅ 7 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (9) ✅ 9 | [CRopes](../source/game_sa/Ropes.cpp) (11) ✅ 11 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (2) ✅ 2 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CRT](../source/game_sa/VehicleRecording.cpp) (6) ✅ 6 | [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) (8) ✅ 8 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) (39) ✅ 39 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (5) ✅ 5 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (5) ✅ 5 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (6) ✅ 6 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (23) ✅ 23 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (4) ✅ 4 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) ✅ 6 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (8) ✅ 8 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) (10) ✅ 10 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (6) ✅ 6 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CSetPiece](../source/game_sa/SetPiece.cpp) (3) ✅ 3 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (4) ✅ 4 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (3) ✅ 3 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (19) ✅ 19 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (3) ✅ 3 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (10) ✅ 10 | [CShadows](../source/game_sa/Shadows.cpp) (30) ✅ 30 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (3) ✅ 3 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (7) ✅ 7 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (3) ✅ 3 |
| [CCoronas](../source/game_sa/Coronas.cpp) (11) ✅ 11 | [CShopping](../source/game_sa/Shopping.cpp) (30) ✅ 30 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (3) ✅ 3 |
| [CCover](../source/game_sa/Cover.cpp) (16) ✅ 16 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (3) ✅ 3 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (3) ✅ 3 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (3) ✅ 3 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (3) ✅ 3 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (4) ✅ 4 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (3) ✅ 3 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (35) ✅ 35 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (14) ✅ 14 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (6) ✅ 6 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CStats](../source/game_sa/Stats.cpp) (51) ✅ 51 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (9) ✅ 9 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (16) ✅ 16 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) ✅ 3 | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (8) ✅ 8 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CStencilShadows](../source/game_sa/StencilShadows.cpp) (13) ✅ 13 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (8) ✅ 8 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) (7) ✅ 7 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (6) ✅ 6 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) ✅ 37 | [CStreaming](../source/game_sa/Streaming.cpp) (94) ✅ 94 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (4) ✅ 4 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (14) ✅ 14 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (14) ✅ 14 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (9) ✅ 9 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (8) ✅ 8 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (8) ✅ 8 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (35) ✅ 35 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (7) ✅ 7 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (7) ✅ 7 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (6) ✅ 6 | [CTagManager](../source/game_sa/TagManager.cpp) (19) ✅ 19 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (6) ✅ 6 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (4) ✅ 4 | [CTask](../source/game_sa/Tasks/Task.cpp) (5) ✅ 5 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (3) ✅ 3 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (3) ✅ 3 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (5) ✅ 5 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (4) ✅ 4 |
| [CData](../source/game_sa/Text/Text.cpp) (2) ✅ 2 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (4) ✅ 4 | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (4) ✅ 4 |
| [CDebug](../source/game_sa/Debug.cpp) (1) ✅ 1 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (6) ✅ 6 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (9) ✅ 9 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) (5) ✅ 5 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (2) ✅ 2 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (4) ✅ 4 |
| [CDecisionMakerTypes](../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) (8) ✅ 8 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (5) ✅ 5 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (6) ✅ 6 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (3) ✅ 3 | [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) (12) ✅ 12 |
| [CDirectory](../source/game_sa/Directory.cpp) (10) ✅ 10 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (2) ✅ 2 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) (7) ✅ 7 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (3) ✅ 3 |
| [CDraw](../source/game_sa/Draw.cpp) (2) ✅ 2 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (5) ✅ 5 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (5) ✅ 5 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (9) ✅ 9 | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) ✅ 15 | [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (7) ✅ 7 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (1) ✅ 1 | [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) (3) ✅ 3 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (7) ✅ 7 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (2) ✅ 2 | [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) (9) ✅ 9 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (6) ✅ 6 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (10) ✅ 10 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (2) ✅ 2 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (70) ✅ 70 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (6) ✅ 6 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (6) ✅ 6 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (11) ✅ 11 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (5) ✅ 5 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (1) ✅ 1 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (5) ✅ 5 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (8) ✅ 8 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (11) ✅ 11 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (11) ✅ 11 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (8) ✅ 8 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (18) ✅ 18 | [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) (9) ✅ 9 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (7) ✅ 7 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) ✅ 1 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (7) ✅ 7 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (3) ✅ 3 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (4) ✅ 4 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (8) ✅ 8 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (9) ✅ 9 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (7) ✅ 7 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) (9) ✅ 9 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (11) ✅ 11 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) (9) ✅ 9 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) ✅ 6 |
| [CEvent](../source/game_sa/Events/Event.cpp) (3) ✅ 3 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (10) ✅ 10 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (9) ✅ 9 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (4) ✅ 4 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (10) ✅ 10 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (3) ✅ 3 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) ✅ 1 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (5) ✅ 5 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (5) ✅ 5 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (2) ✅ 2 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (6) ✅ 6 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (6) ✅ 6 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (3) ✅ 3 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (7) ✅ 7 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (1) ✅ 1 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (4) ✅ 4 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (2) ✅ 2 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (4) ✅ 4 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (4) ✅ 4 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (6) ✅ 6 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (19) ✅ 19 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (2) ✅ 2 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (7) ✅ 7 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (6) ✅ 6 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (2) ✅ 2 | [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) (8) ✅ 8 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (20) ✅ 20 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (5) ✅ 5 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (4) ✅ 4 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (4) ✅ 4 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (8) ✅ 8 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (6) ✅ 6 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (1) ✅ 1 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (15) ✅ 15 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (6) ✅ 6 |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (11) ✅ 11 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (3) ✅ 3 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (1) ✅ 1 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (1) ✅ 1 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (9) ✅ 9 | [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) (5) ✅ 5 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (3) ✅ 3 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (3) ✅ 3 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (12) ✅ 12 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (9) ✅ 9 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (5) ✅ 5 | [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) (8) ✅ 8 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (2) ✅ 2 | [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) (8) ✅ 8 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (69) ✅ 69 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) (72) ✅ 72 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (9) ✅ 9 | [CTheZones](../source/game_sa/TheZones.cpp) (22) ✅ 22 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (10) ✅ 10 | [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) (8) ✅ 8 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (2) ✅ 2 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (8) ✅ 8 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (2) ✅ 2 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (9) ✅ 9 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (2) ✅ 2 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (8) ✅ 8 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (9) ✅ 9 | [CTimer](../source/game_sa/Timer.cpp) (13) ✅ 13 |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (6) ✅ 6 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (1) ✅ 1 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (8) ✅ 8 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (13) ✅ 13 |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (8) ✅ 8 | [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) (40) ✅ 40 |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (11) ✅ 11 | [cTransmission](../source/game_sa/cTransmission.cpp) (4) ✅ 4 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (2) ✅ 2 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (8) ✅ 8 | [CTxdStore](../source/game_sa/TxdStore.cpp) (26) ✅ 26 |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (1) ✅ 1 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (8) ✅ 8 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (2) ✅ 2 | [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) (7) ✅ 7 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) ✅ 1 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (19) ✅ 19 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (2) ✅ 2 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (13) ✅ 13 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (3) ✅ 3 | [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) (8) ✅ 8 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (75) ✅ 75 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (1) ✅ 1 | [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) (7) ✅ 7 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (2) ✅ 2 |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (1) ✅ 1 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (13) ✅ 13 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (2) ✅ 2 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (8) ✅ 8 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (2) ✅ 2 |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (2) ✅ 2 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (5) ✅ 5 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) ✅ 84 |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (1) ✅ 1 | [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) (5) ✅ 5 | [CWanted](../source/game_sa/Wanted.cpp) (33) ✅ 33 |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (6) ✅ 6 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (2) ✅ 2 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (11) ✅ 11 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (2) ✅ 2 | [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) (7) ✅ 7 | [CWeapon](../source/game_sa/Weapon.cpp) (41) ✅ 41 |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (3) ✅ 3 | [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) (8) ✅ 8 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (9) ✅ 9 |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (2) ✅ 2 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (7) ✅ 7 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (10) ✅ 10 |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (9) ✅ 9 | [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) (7) ✅ 7 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (3) ✅ 3 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) ✅ 6 | [CWeather](../source/game_sa/Weather.cpp) (13) ✅ 13 |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) (7) ✅ 7 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (2) ✅ 2 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (3) ✅ 3 | [CWorld](../source/game_sa/World.cpp) (95) ✅ 95 |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (3) ✅ 3 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (8) ✅ 8 | [CZoneDef](../source/game_sa/CullZones.cpp) (1) ✅ 1 |
| [CEventSource](../source/game_sa/Events/EventSource.cpp) (1) ✅ 1 | [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) (10) ✅ 10 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (6) ✅ 6 |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (4) ✅ 4 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (7) ✅ 7 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (7) ✅ 7 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (3) ✅ 3 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (4) ✅ 4 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (3) ✅ 3 |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (5) ✅ 5 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (9) ✅ 9 | [FxEmitter_c](../source/game_sa/Fx/FxEmitter.cpp) (4) ✅ 4 |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (2) ✅ 2 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) ✅ 8 | [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) (9) ✅ 9 |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (2) ✅ 2 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) ✅ 7 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (1) ✅ 1 |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (3) ✅ 3 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (6) ✅ 6 | [FxFrustumInfo_c](../source/game_sa/Fx/FxFrustumInfo.cpp) (1) ✅ 1 |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (3) ✅ 3 | [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) (11) ✅ 11 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (5) ✅ 5 |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (3) ✅ 3 | [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) (7) ✅ 7 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (2) ✅ 2 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) ✅ 9 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (5) ✅ 5 |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) (11) ✅ 11 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (2) ✅ 2 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) ✅ 10 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (4) ✅ 4 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (48) ✅ 48 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) ✅ 7 | [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) (35) ✅ 35 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (7) ✅ 7 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (8) ✅ 8 | [Game](../source/app/app_game.cpp) (12) ✅ 12 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (3) ✅ 3 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CFont](../source/game_sa/Font.cpp) (37) ✅ 37 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (8) ✅ 8 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) ✅ 17 |
| [CFormation](../source/game_sa/Formation.cpp) (10) ✅ 10 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (8) ✅ 8 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (6) ✅ 6 |
| [CGame](../source/game_sa/Game.cpp) (22) ✅ 22 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (10) ✅ 10 | [Interior_c](../source/game_sa/Interior/Interior_c.cpp) (52) ✅ 52 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (9) ✅ 9 | [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) (24) ✅ 24 |
| [CGamma](../source/game_sa/Gamma.cpp) (2) ✅ 2 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (8) ✅ 8 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (21) ✅ 21 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (8) ✅ 8 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (5) ✅ 5 |
| [CGangWars](../source/game_sa/GangWars.cpp) (35) ✅ 35 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) (5) ✅ 5 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CGarages](../source/game_sa/Garages.cpp) (26) ✅ 26 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (9) ✅ 9 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (2) ✅ 2 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (4) ✅ 4 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) (19) ✅ 19 | [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) (4) ✅ 4 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (6) ✅ 6 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (8) ✅ 8 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (3) ✅ 3 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (4) ✅ 4 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (4) ✅ 4 | [Ps](../source/app/platform/win/WinPs.cpp) (22) ✅ 22 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) (32) ✅ 32 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (7) ✅ 7 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (11) ✅ 11 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (2) ✅ 2 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (44) ✅ 44 |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (3) ✅ 3 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (9) ✅ 9 | [RpAnisot](../source/game_sa/RenderWare/rw/rpanisot.cpp) (3) ✅ 3 |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (3) ✅ 3 | [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) (9) ✅ 9 | [RpHAnim](../source/game_sa/RenderWare/rw/rphanim.cpp) (1) ✅ 1 |
| [CHud](../source/game_sa/Hud.cpp) (38) ✅ 38 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (13) ✅ 13 | [RpSkin](../source/game_sa/RenderWare/rw/rpskin.cpp) (1) ✅ 1 |
| [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) (11) ✅ 11 | [RpUVAnim](../source/game_sa/RenderWare/rw/rpuvanim.cpp) (2) ✅ 2 |
| [CIdleCam](../source/game_sa/IdleCam.cpp) (14) ✅ 14 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (8) ✅ 8 | [RpWorld](../source/game_sa/RenderWare/rw/rpworld.cpp) (21) ✅ 21 |
| [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (9) ✅ 9 | [Rs](../source/app/platform/platform.cpp) (28) ✅ 28 |
| [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (9) ✅ 9 | [RtAnim](../source/game_sa/RenderWare/rw/rtanim.cpp) (3) ✅ 3 |
| [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (10) ✅ 10 | [RtDict](../source/game_sa/RenderWare/rw/rtdict.cpp) (2) ✅ 2 |
| [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (10) ✅ 10 | [RwCore](../source/game_sa/RenderWare/rw/rwcore.cpp) (29) ✅ 29 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) (7) ✅ 7 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (7) ✅ 7 | [RwHelper](../source/game_sa/RwHelper.cpp) (28) ✅ 28 |
| [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (1) ✅ 1 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (5) ✅ 5 | [RwPlCore](../source/game_sa/RenderWare/rw/rwplcore.cpp) (14) ✅ 14 |
| [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) ✅ 9 | [Securom](../source/app/securom/Securom.cpp) (2) ✅ 2 |
| [CKeyArray](../source/game_sa/Text/Text.cpp) (4) ✅ 4 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (9) ✅ 9 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (47) ✅ 47 |
| [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (13) ✅ 13 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) ✅ 4 |
| [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (10) ✅ 10 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (6) ✅ 6 |
| [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) (7) ✅ 7 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (3) ✅ 3 |
| [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (9) ✅ 9 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) ✅ 7 |
| [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (7) ✅ 7 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (6) ✅ 6 |
| [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) (12) ✅ 12 |  |

</details>

<a id="parciales"></a>
## 🟡 Parciales · 13 clases

Orden de menor a mayor número de envolturas al EXE pendientes (a igualdad, de menor a mayor número de entradas pendientes y alfabético). Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas al EXE»): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

| Clase | Revertidas / total | Pendientes | Envolturas al EXE | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) | 102/102 | **0** | **1** | 100.00% | 102 | 0 |
| [CPed](../source/game_sa/Entity/Ped/Ped.cpp) | 148/148 | **0** | **1** | 100.00% | 148 | 0 |
| [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) | 1/1 | **0** | **5** | 100.00% | 1 | 0 |
| [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) | 1/1 | **0** | **5** | 100.00% | 1 | 0 |
| [CWaterLevel](../source/game_sa/WaterLevel.cpp) | 33/33 | **0** | **5** | 100.00% | 33 | 0 |
| [CPathFind](../source/game_sa/PathFind.cpp) | 60/60 | **0** | **6** | 100.00% | 60 | 0 |
| [D3DResourceSystem](../source/game_sa/RenderWare/D3DResourceSystem.cpp) | 3/3 | **0** | **7** | 100.00% | 3 | 0 |
| [CGarage](../source/game_sa/Garage.cpp) | 8/8 | **0** | **10** | 100.00% | 8 | 0 |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) | 11/11 | **0** | **11** | 100.00% | 11 | 0 |
| [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) | 52/52 | **0** | **13** | 100.00% | 52 | 0 |
| [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) | 149/149 | **0** | **17** | 100.00% | 149 | 0 |
| [Fx_c](../source/game_sa/Fx/Fx.cpp) | 9/9 | **0** | **17** | 100.00% | 9 | 0 |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) | 30/30 | **0** | **80** | 100.00% | 30 | 0 |

<a id="sin-revertir"></a>
## 🔴 Sin revertir · 23 clases

Clases o grupos sin ninguna entrada reconstruida en el catálogo. Las que figuran con envolturas al EXE no están en [hooks.csv](hooks.csv): sus funciones del juego se conocen por el código fuente o por las cohortes de dependencias y siguen delegando en el original.

| Clase | Revertidas / total | Pendientes | Envolturas al EXE | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| CCollisionEventScanner | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CEventScanner | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CMemoryHeap | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CMentalState | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CPedStuckChecker | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CSearchLight | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CTaskSimpleSlideToCoord | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CTrainNode | 0/0 | — | **1** | 0.00% | 0 | 0 |
| CVehiclePotentialCollisionScanner | 0/0 | — | **1** | 0.00% | 0 | 0 |
| FxInterpInfo255_c | 0/0 | — | **1** | 0.00% | 0 | 0 |
| FxInterpInfo32_c | 0/0 | — | **1** | 0.00% | 0 | 0 |
| FxInterpInfoFloat_c | 0/0 | — | **1** | 0.00% | 0 | 0 |
| FxInterpInfoU255_c | 0/0 | — | **1** | 0.00% | 0 | 0 |
| (biblioteca) QuickTime/QTML | 0/0 | — | **2** | 0.00% | 0 | 0 |
| CMissionCleanup | 0/0 | — | **2** | 0.00% | 0 | 0 |
| CRemote | 0/0 | — | **2** | 0.00% | 0 | 0 |
| CStoredCar | 0/0 | — | **2** | 0.00% | 0 | 0 |
| CWorldScan | 0/0 | — | **2** | 0.00% | 0 | 0 |
| CTaskComplexLeaveCar | 0/0 | — | **4** | 0.00% | 0 | 0 |
| (biblioteca) CRT / runtime estático | 0/0 | — | **7** | 0.00% | 0 | 0 |
| (biblioteca) Controlador D3D9 de RenderWare | 0/0 | — | **58** | 0.00% | 0 | 0 |
| (helpers y globales sin clase) | 0/0 | — | **67** | 0.00% | 0 | 0 |
| (biblioteca) RenderWare SDK | 0/0 | — | **134** | 0.00% | 0 | 0 |

<a id="cohorte-nuevo"></a>
## 📋 Cohorte nuevo (17/10/2026): lista por dificultad

Destinos originales explícitos que `source/` sigue referenciando y que no estaban reconstruidos ni validados el 17/10/2026, las dos listas completas van ordenadas de menor a mayor tamaño en bytes (la columna «Rango» conserva la posición por dificultad, de menor a mayor: 1 es la más fácil) (juego: puntuación de tamaño y riesgos —llamadas sin catalogar, indirectas, x87, tablas de saltos y SEH—; biblioteca: solo tamaño). Lista completa y descargable: [lista-por-dificultad.csv](../reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv) · metodología: [README del cohorte](../reversing/OriginalDependencies2026-10-17/README.md). La columna «Estado» se calcula con el catálogo actual: una dirección con fila reversed=1 en hooks.csv figura como reconstruida o validada.

| Nivel | Juego | de ellas pendientes | Biblioteca | de ellas pendientes |
| --- | ---: | ---: | ---: | ---: |
| 1 Muy fácil | 9 | 1 | 77 | 1 |
| 2 Fácil | 25 | 18 | 104 | 104 |
| 3 Media | 75 | 59 | 69 | 69 |
| 4 Difícil | 57 | 51 | 23 | 23 |
| 5 Muy difícil | 19 | 17 | 6 | 6 |

<details>
<summary>Funciones del juego (185), de menor a mayor tamaño en bytes</summary>

| Rango | Nivel | Subcategoría | Función | Dirección | Bytes | Estado |
| ---: | --- | --- | --- | --- | ---: | --- |
| 44 | 2 Fácil | wrapper | CTaskComplexPartnerDeal::CreateFirstSubTask | `0x006823B0` | 5 | ✅ validada |
| 1 | 1 Muy fácil | embedded_call | helper sin símbolo confirmado (usado por FightUnwind) | `0x0061A3A0` | 7 | ✅ validada |
| 20 | 2 Fácil | wrapper | CTaskComplexFollowPedFootsteps::MakeAbortable | `0x00694ED0` | 8 | pendiente |
| 10 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CHud::DrawVitalStats) | `0x00406CE0` | 19 | pendiente |
| 47 | 2 Fácil | wrapper | CCarCtrl::GetAIHeliToFlyInDirection | `0x0042A730` | 28 | pendiente |
| 2 | 1 Muy fácil | embedded_call | CPool&lt;CVehicle&gt;::GetAt (acceso por índice a la ranura de 0xA18 bytes) | `0x0041CC10` | 31 | ✅ validada |
| 4 | 1 Muy fácil | wrapper | CPedGroups::RemoveAllFollowersFromGroup | `0x005FB8A0` | 32 | ✅ validada |
| 5 | 1 Muy fácil | wrapper | CDecisionMakerTypes::FlushDecisionMakerEventResponse | `0x00604490` | 35 | ✅ validada |
| 13 | 2 Fácil | wrapper | CCarCtrl::ThisVehicleShouldTryNotToTurn | `0x00421FE0` | 38 | pendiente |
| 3 | 1 Muy fácil | wrapper | CAutoPilot::RemoveOnePathNode | `0x0041B950` | 45 | ✅ validada |
| 6 | 1 Muy fácil | wrapper | CGarages::CountCarsInHideoutGarage | `0x0044A210` | 47 | ✅ validada |
| 24 | 2 Fácil | wrapper | CTaskComplexKillPedOnFoot::MakeAbortable | `0x00625E40` | 48 | pendiente |
| 14 | 1 Muy fácil | wrapper | CAEStreamingDecoder scalar deleting destructor (operator delete de la clase) | `0x004F2860` | 50 | ✅ validada |
| 48 | 2 Fácil | wrapper | CDecisionMakerTypes::AddEventResponse | `0x006044C0` | 50 | ✅ validada |
| 12 | 1 Muy fácil | embedded_call | helper sin símbolo confirmado (usado por GetAnimHierarchyFromFrame) | `0x00734A70` | 51 | pendiente |
| 8 | 1 Muy fácil | wrapper | FindReplacementUpgradeCB | `0x006D3490` | 53 | ✅ validada |
| 50 | 2 Fácil | wrapper | CTaskComplexPartner::RemoveStreamedAnims | `0x00682370` | 56 | ✅ validada |
| 16 | 3 Media | wrapper | CQuaternion::Get | `0x0059C230` | 57 | ✅ validada |
| 49 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x00606060` | 59 | pendiente |
| 9 | 2 Fácil | wrapper | FindUpgradeCB | `0x006D3370` | 61 | pendiente |
| 27 | 2 Fácil | wrapper | CTaskComplexPartnerChat::MakeAbortable | `0x00682C60` | 65 | ✅ validada |
| 55 | 3 Media | wrapper | CCarCtrl::SteerAIPlaneTowardsTargetCoors | `0x00423790` | 75 | pendiente |
| 90 | 2 Fácil | wrapper | CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer | `0x0042F370` | 76 | pendiente |
| 19 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CameraObbeCinemaCycle) | `0x0050B830` | 77 | pendiente |
| 7 | 2 Fácil | wrapper | CCarCtrl::SteerAIPlaneToCrashAndBurn | `0x00423880` | 81 | pendiente |
| 42 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por WaitForPedUnwind) | `0x0061A3D0` | 85 | pendiente |
| 17 | 3 Media | wrapper | CCarCtrl::SteerAIHeliToCrashAndBurn | `0x004238E0` | 89 | pendiente |
| 53 | 2 Fácil | wrapper | CSearchLight::IsSpottedEntity | `0x00493900` | 90 | pendiente |
| 18 | 2 Fácil | wrapper | CTaskComplexPartner::StreamRequiredAnims | `0x00682310` | 93 | ✅ validada |
| 21 | 3 Media | wrapper | CCarCtrl::FindIntersection2Lines | `0x004226F0` | 100 | pendiente |
| 87 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CAEUserRadioTrackManager::WriteUserTracksFile) | `0x004F4620` | 102 | pendiente |
| 34 | 2 Fácil | wrapper | RemoveUpgradeCB | `0x006D3300` | 108 | pendiente |
| 35 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CTaskSimpleCarDrive::StartRollingDoor) | `0x0063C840` | 108 | pendiente |
| 11 | 2 Fácil | wrapper | CStats::FindLeastFavoriteRadioStation | `0x00559010` | 110 | ✅ validada |
| 62 | 2 Fácil | wrapper | CTaskComplexPartnerDeal::StreamRequiredAnims | `0x006823C0` | 113 | ✅ validada |
| 63 | 3 Media | wrapper | CCarCtrl::SteerAIHeliFlyingAwayFromPlayer | `0x0042ACB0` | 122 | pendiente |
| 23 | 3 Media | embedded_call | TransformPoint | `0x0059C890` | 124 | pendiente |
| 70 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por ProcessPlayerVehicleInteraction) | `0x006919C0` | 126 | pendiente |
| 38 | 2 Fácil | wrapper | CreateRouteTask | `0x00655140` | 130 | pendiente |
| 15 | 2 Fácil | wrapper | CCarCtrl::StopCarIfNodesAreInvalid | `0x00422590` | 137 | pendiente |
| 25 | 2 Fácil | embedded_call | CHeli::FindSwatPositionRelativeToHeli | `0x006C4760` | 141 | pendiente |
| 28 | 3 Media | embedded_call | CGarage::IsGarageEmpty | `0x0044A9C0` | 141 | pendiente |
| 59 | 3 Media | wrapper | CCarCtrl::SteerAIPlaneToFollowEntity | `0x004237F0` | 141 | pendiente |
| 65 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateFirstSubTask | `0x00695000` | 141 | pendiente |
| 30 | 3 Media | wrapper | CCustomBuildingDNPipeline::CustomPipeMaterialSetup | `0x005D7120` | 149 | ✅ validada |
| 85 | 3 Media | wrapper | D3DResourceSystem::CreateIndexBuffer | `0x007306A0` | 153 | pendiente |
| 29 | 3 Media | embedded_call | CTrainNode::GetLightingFromCollision | `0x006F5F80` | 161 | pendiente |
| 31 | 3 Media | wrapper | FightStrikeDamageExe | `0x0061C740` | 164 | pendiente |
| 26 | 3 Media | wrapper | CCarCtrl::FindSpeedMultiplier | `0x004224E0` | 167 | pendiente |
| 76 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CProjectileInfo::AddProjectile) | `0x005A4030` | 167 | pendiente |
| 45 | 3 Media | wrapper | CTaskComplexPartner::CreateFirstSubTask | `0x00681F20` | 177 | ✅ validada |
| 22 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CCam::Process_DW_HeliChaseCam) | `0x0050E180` | 184 | pendiente |
| 43 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CCam::Process_DW_HeliChaseCam) | `0x0050E090` | 192 | pendiente |
| 86 | 3 Media | wrapper | CTaskComplexPartner::ControlSubTask | `0x006840D0` | 192 | ✅ validada |
| 40 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x005BB9F0` | 197 | pendiente |
| 37 | 3 Media | wrapper | CPlayerPed::PlayerWantsToAttack | `0x0060CC50` | 200 | pendiente |
| 39 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CTaskSimpleGangDriveBy::AITarget) | `0x00621B10` | 201 | pendiente |
| 79 | 3 Media | wrapper | D3DResourceSystem::CreateTexture | `0x00730510` | 204 | pendiente |
| 32 | 3 Media | wrapper | CCarCtrl::FindPercDependingOnDistToLink | `0x00422620` | 208 | pendiente |
| 36 | 3 Media | wrapper | CCarCtrl::SwitchBetweenPhysicsAndGhost | `0x004222A0` | 208 | pendiente |
| 72 | 3 Media | wrapper | Fx_c::AddPunchImpact | `0x0049F670` | 209 | pendiente |
| 67 | 3 Media | wrapper | CCarCtrl::TestWhetherToFirePlaneGuns | `0x00429520` | 214 | pendiente |
| 74 | 3 Media | wrapper | diPadInit | `0x007485C0` | 214 | ✅ validada |
| 33 | 3 Media | wrapper | CQuaternion::Get | `0x0059C080` | 215 | ✅ validada |
| 41 | 3 Media | wrapper | CPlantMgr::AtomicCreatePrelitIfNeeded | `0x005DD100` | 221 | ✅ validada |
| 71 | 3 Media | wrapper | CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget | `0x004335E0` | 240 | pendiente |
| 98 | 3 Media | wrapper | CCarCtrl::SteerAIHeliTowardsTargetCoors | `0x0042A630` | 244 | pendiente |
| 121 | 3 Media | wrapper | CCarCtrl::GetAIPlaneToAttackPlayer | `0x00429780` | 261 | pendiente |
| 114 | 3 Media | wrapper | D3DResourceSystem::DestroyIndexBuffer | `0x00730D30` | 263 | pendiente |
| 61 | 3 Media | wrapper | CTaskComplexPartnerShove::GetPartnerSequence | `0x00683120` | 275 | ✅ validada |
| 107 | 3 Media | wrapper | CGarage::IsEntityEntirelyOutside | `0x00448D30` | 281 | pendiente |
| 115 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateNextSubTask | `0x00694EE0` | 283 | pendiente |
| 99 | 3 Media | wrapper | CTaskSimpleFight::MakeAbortable | `0x006239F0` | 284 | ✅ validada |
| 130 | 3 Media | wrapper | CCarCtrl::PickNextNodeAccordingStrategy | `0x00432B10` | 321 | pendiente |
| 46 | 3 Media | embedded_call | FxInterpInfoFloat_c::GetVal | `0x004A8470` | 330 | pendiente |
| 52 | 3 Media | wrapper | CVehicle::AddUpgrade | `0x006DFA20` | 333 | pendiente |
| 77 | 3 Media | embedded_call | CDecision::MakeDecision | `0x006040D0` | 333 | ✅ validada |
| 84 | 3 Media | wrapper | CGarage::IsEntityEntirelyInside3D | `0x00448BE0` | 333 | pendiente |
| 80 | 3 Media | wrapper | CCarCtrl::WeaveThroughCarsSectorList | `0x0042D680` | 341 | pendiente |
| 129 | 3 Media | wrapper | CTaskComplexWalkRoundObject::ControlSubTask | `0x006575F0` | 341 | ✅ validada |
| 136 | 3 Media | wrapper | CTaskComplexWalkRoundObject::CreateNextSubTask | `0x00657220` | 341 | ✅ validada |
| 64 | 3 Media | wrapper | CPed::SpecialEntityCalcCollisionSteps | `0x005E3E90` | 342 | ✅ validada |
| 82 | 3 Media | wrapper | CCarCtrl::WeaveThroughObjectsSectorList | `0x0042D950` | 343 | pendiente |
| 75 | 3 Media | wrapper | CTaskComplexPartnerDeal::GetPartnerSequence | `0x00682440` | 344 | ✅ validada |
| 81 | 3 Media | wrapper | CCarCtrl::WeaveThroughPedsSectorList | `0x0042D7E0` | 346 | pendiente |
| 102 | 3 Media | wrapper | RwTextureGtaStreamRead | `0x00730E60` | 347 | pendiente |
| 89 | 3 Media | wrapper | CVehicle::PossiblyDropFreeFallBombForPlayer | `0x006E07E0` | 359 | pendiente |
| 83 | 3 Media | wrapper | CCarCtrl::SteerAIHeliToLand | `0x0042AD30` | 370 | pendiente |
| 60 | 3 Media | wrapper | SkinGetBonePositionsToTable | `0x00735360` | 381 | pendiente |
| 66 | 3 Media | wrapper | RpClumpGetBoundingSphere | `0x00734FC0` | 384 | pendiente |
| 92 | 3 Media | wrapper | CGarage::IsStaticPlayerCarEntirelyInside | `0x0044A830` | 386 | pendiente |
| 51 | 3 Media | wrapper | CPlayerPed::EvaluateNeighbouringTarget | `0x0060D1C0` | 388 | pendiente |
| 88 | 3 Media | wrapper | CCarCtrl::SlowCarDownForObjectsSectorList | `0x0042D4F0` | 391 | pendiente |
| 145 | 3 Media | wrapper | RsSavePresetView | `0x00619FA0` | 393 | pendiente |
| 68 | 3 Media | embedded_call | CHeli::CHeli | `0x006C4190` | 398 | pendiente |
| 104 | 3 Media | embedded_call | FxEmitter_c::AddParticle | `0x004A4050` | 400 | ✅ validada |
| 112 | 3 Media | wrapper | CCarCtrl::SlowCarDownForCarsSectorList | `0x00432420` | 413 | pendiente |
| 142 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x004650F0` | 415 | pendiente |
| 106 | 3 Media | embedded_call | FxEmitter_c::AddParticle | `0x004A3EA0` | 418 | ✅ validada |
| 108 | 3 Media | wrapper | CVehicle::AddReplacementUpgrade | `0x006D3830` | 423 | pendiente |
| 149 | 3 Media | wrapper | D3DResourceSystem::DestroyTexture | `0x00730B70` | 439 | pendiente |
| 54 | 3 Media | wrapper | CVehicle::ScanAndMarkTargetForHeatSeekingMissile | `0x006E0400` | 445 | pendiente |
| 117 | 3 Media | wrapper | CCarCtrl::GetAIPlaneToDoDogFight | `0x00429890` | 472 | pendiente |
| 126 | 3 Media | wrapper | CCarCtrl::GetAIHeliToAttackPlayer | `0x0042F3C0` | 472 | pendiente |
| 91 | 3 Media | wrapper | CCarCtrl::SteerAIHeliAsPoliceHeli | `0x0042AAD0` | 475 | pendiente |
| 56 | 3 Media | embedded_call | CPathFind::FindParkingNodeInArea | `0x004513F0` | 476 | pendiente |
| 110 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateSubTask | `0x00695E40` | 488 | pendiente |
| 119 | 3 Media | wrapper | FightHitObjExe | `0x0061D400` | 494 | pendiente |
| 57 | 3 Media | wrapper | CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget | `0x00428BE0` | 497 | pendiente |
| 58 | 4 Difícil | wrapper | CCarCtrl::FireHeliRocketsAtTarget | `0x0042B270` | 506 | pendiente |
| 73 | 4 Difícil | wrapper | CCarCtrl::FindPathDirection | `0x00422090` | 515 | pendiente |
| 105 | 4 Difícil | wrapper | CCarCtrl::TriggerDogFightMoves | `0x00429300` | 526 | pendiente |
| 97 | 4 Difícil | wrapper | CVehicle::FireHeatSeakingMissile | `0x006E05C0` | 529 | pendiente |
| 133 | 4 Difícil | wrapper | CRemote::GivePlayerRemoteControlledCar | `0x0045AB10` | 548 | pendiente |
| 109 | 4 Difícil | wrapper | Fx_c::AddWood | `0x0049EE10` | 549 | pendiente |
| 146 | 4 Difícil | wrapper | RsLoadPresetViews | `0x00619D60` | 552 | pendiente |
| 116 | 4 Difícil | embedded_call | CTaskSimpleSlideToCoord::ProcessPed | `0x0066C4E0` | 570 | pendiente |
| 94 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop | `0x00428990` | 583 | pendiente |
| 69 | 4 Difícil | embedded_call | CPed::SpecialEntityPreCollisionStuff | `0x005E3C30` | 603 | ✅ validada |
| 125 | 4 Difícil | embedded_call | helper sin símbolo confirmado (usado por CWeaponEffects::Render) | `0x0056EF90` | 608 | pendiente |
| 101 | 4 Difícil | wrapper | CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer | `0x00429090` | 613 | pendiente |
| 153 | 4 Difícil | wrapper | CTaskComplexWalkRoundObject::CreateFirstSubTask | `0x00657380` | 620 | ✅ validada |
| 127 | 4 Difícil | wrapper | CMentalState::Process | `0x006008A0` | 646 | pendiente |
| 78 | 4 Difícil | wrapper | CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper | `0x00425F70` | 672 | pendiente |
| 96 | 4 Difícil | wrapper | CCarCtrl::ChooseModel | `0x00424CE0` | 672 | pendiente |
| 111 | 4 Difícil | wrapper | CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer | `0x00428DE0` | 684 | pendiente |
| 159 | 4 Difícil | wrapper | CStats::BuildStatLine | `0x00559230` | 735 | ✅ validada |
| 131 | 4 Difícil | wrapper | CCarCtrl::SteerAICarBlockingPlayerForwardAndBack | `0x00422B20` | 745 | pendiente |
| 134 | 4 Difícil | wrapper | CCarCtrl::SteerAICarParkParallel | `0x00433BA0` | 762 | pendiente |
| 95 | 4 Difícil | embedded_call | helper sin símbolo confirmado (usado por CPlayerInfo::Process) | `0x0056EC80` | 776 | pendiente |
| 150 | 4 Difícil | wrapper | CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents | `0x00603720` | 777 | pendiente |
| 113 | 4 Difícil | wrapper | CHeli::BlowUpCar | `0x006C6D30` | 795 | pendiente |
| 128 | 4 Difícil | wrapper | CCarCtrl::SteerAICarParkPerpendicular | `0x00433EA0` | 800 | pendiente |
| 141 | 4 Difícil | wrapper | CCarCtrl::WeaveForObject | `0x00426BC0` | 803 | pendiente |
| 168 | 4 Difícil | wrapper | CTaskComplexKillPedOnFoot::CreateNextSubTask | `0x0062B150` | 831 | pendiente |
| 139 | 4 Difícil | wrapper | CTaskComplexPartnerChat::GetPartnerSequence | `0x00684380` | 836 | ✅ validada |
| 123 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsHeadingForTarget | `0x00433280` | 841 | pendiente |
| 93 | 4 Difícil | wrapper | CCarCtrl::FindLinksToGoWithTheseNodes | `0x0042B470` | 862 | pendiente |
| 138 | 4 Difícil | wrapper | CHeli::GenerateHeli | `0x006C6520` | 867 | pendiente |
| 160 | 4 Difícil | wrapper | CHeli::SendDownSwat | `0x006C69C0` | 869 | pendiente |
| 137 | 4 Difícil | wrapper | CCarCtrl::SteerAIHeliToFollowEntity | `0x0042A750` | 896 | pendiente |
| 103 | 4 Difícil | wrapper | CCarCtrl::ClitargetOrientationToLink | `0x00422760` | 918 | pendiente |
| 156 | 4 Difícil | wrapper | CCarCtrl::IsThisAnAppropriateNode | `0x0042DAB0` | 932 | pendiente |
| 118 | 4 Difícil | wrapper | CCarCtrl::SteerAIHeliToKeepEntityInView | `0x0042AEB0` | 939 | pendiente |
| 157 | 4 Difícil | wrapper | CVehicle::GetPlaneWeaponFiringStatus | `0x006E3440` | 944 | pendiente |
| 151 | 4 Difícil | wrapper | CAutomobile::Render | `0x006A2B10` | 1005 | pendiente |
| 158 | 4 Difícil | wrapper | CPlayerPed::MakeThisPedJoinOurGroup | `0x0060C840` | 1029 | pendiente |
| 144 | 4 Difícil | wrapper | CCarCtrl::SlowCarDownForOtherCar | `0x0042D0E0` | 1032 | pendiente |
| 172 | 4 Difícil | wrapper | CTaskComplexLeaveCar::MakeAbortable | `0x00641100` | 1069 | pendiente |
| 100 | 4 Difícil | wrapper | CCarCtrl::TestCollisionBetween2MovingRects | `0x00425B30` | 1081 | pendiente |
| 165 | 4 Difícil | wrapper | CCarCtrl::CreatePoliceChase | `0x0042C2B0` | 1161 | pendiente |
| 120 | 4 Difícil | wrapper | CCarCtrl::FlyAIHeliToTarget_FixedOrientation | `0x00423940` | 1167 | pendiente |
| 152 | 4 Difícil | wrapper | CCarCtrl::CreateConvoy | `0x0042C740` | 1168 | pendiente |
| 122 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath | `0x00432DD0` | 1177 | pendiente |
| 180 | 4 Difícil | wrapper | CHeli::PreRender | `0x006C5420` | 1204 | pendiente |
| 135 | 4 Difícil | wrapper | CStoredCar::RestoreCar | `0x00447E40` | 1211 | pendiente |
| 143 | 4 Difícil | wrapper | CTaskComplexLeaveCar::CreateSubTask | `0x00641530` | 1214 | pendiente |
| 132 | 4 Difícil | wrapper | CCarCtrl::SteerAICarTowardsPointInEscort | `0x004336D0` | 1223 | pendiente |
| 164 | 4 Difícil | wrapper | FightHitPedExe | `0x0061CBA0` | 1289 | pendiente |
| 124 | 4 Difícil | wrapper | CCarCtrl::FindNodesThisCarIsNearestTo | `0x0042BD20` | 1323 | pendiente |
| 154 | 4 Difícil | wrapper | CCarCtrl::FindAngleToWeaveThroughTraffic | `0x004325C0` | 1339 | pendiente |
| 174 | 4 Difícil | wrapper | CTaskComplexFollowPedFootsteps::ControlSubTask | `0x00695090` | 1341 | pendiente |
| 183 | 4 Difícil | wrapper | CTaskComplexLeaveCar::CreateNextSubTask | `0x006419F0` | 1428 | pendiente |
| 148 | 4 Difícil | embedded_call | FxEmitter_c::CreateParticle | `0x004A2580` | 1434 | ✅ validada |
| 177 | 4 Difícil | wrapper | __MainWndProc | `0x00747EB0` | 1458 | ✅ validada |
| 161 | 4 Difícil | wrapper | CHeli::ProcessFlyingCarStuff | `0x006C4E60` | 1467 | pendiente |
| 171 | 5 Muy difícil | wrapper | CTaskComplexPartner::CreateNextSubTask | `0x00683AD0` | 1521 | ✅ validada |
| 167 | 5 Muy difícil | wrapper | CCarCtrl::WeaveForOtherCar | `0x00426350` | 1558 | pendiente |
| 173 | 5 Muy difícil | wrapper | CHeli::ProcessControlInputs | `0x006C4830` | 1571 | pendiente |
| 147 | 5 Muy difícil | wrapper | CCarCtrl::DragCarToPoint | `0x0042EC90` | 1748 | pendiente |
| 166 | 5 Muy difícil | wrapper | CCarCtrl::SlowCarDownForPedsSectorList | `0x00425440` | 1761 | pendiente |
| 176 | 5 Muy difícil | wrapper | CTaskSimpleFight::ProcessPed | `0x00629920` | 1907 | ✅ validada |
| 140 | 5 Muy difícil | wrapper | CCarCtrl::FlyAIPlaneInCertainDirection | `0x00423000` | 1917 | pendiente |
| 179 | 5 Muy difícil | wrapper | CMissionCleanup::Process | `0x00468560` | 1955 | pendiente |
| 155 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeToChaseCar | `0x00426EF0` | 2111 | pendiente |
| 163 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeToFollowPath | `0x00427740` | 2193 | pendiente |
| 175 | 5 Muy difícil | wrapper | CHeli::ProcessControl | `0x006C7050` | 2352 | pendiente |
| 169 | 5 Muy difícil | wrapper | CCarCtrl::DealWithBend_Racing | `0x00428040` | 2361 | pendiente |
| 162 | 5 Muy difícil | wrapper | CCarCtrl::FlyAIHeliInCertainDirection | `0x00429A70` | 2994 | pendiente |
| 181 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing | `0x00435830` | 3320 | pendiente |
| 184 | 5 Muy difícil | embedded_call | helper sin símbolo confirmado (usado por CameraObbeCinemaCycle) | `0x0051D770` | 3445 | pendiente |
| 170 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeRandomly | `0x0042DE80` | 3591 | pendiente |
| 182 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPath | `0x00434900` | 3729 | pendiente |
| 185 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysics_OnlyMission | `0x00436A90` | 4243 | pendiente |
| 178 | 5 Muy difícil | wrapper | CPed::ProcessEntityCollision | `0x005E2530` | 4342 | pendiente |

</details>

<details>
<summary>Bibliotecas (RenderWare, controlador D3D9, CRT, QuickTime) (279), de menor a mayor tamaño en bytes</summary>

| Rango | Nivel | Subcategoría | Función | Dirección | Bytes | Estado |
| ---: | --- | --- | --- | --- | ---: | --- |
| 1 | 1 Muy fácil | RenderWare SDK | _rwStringClose | `0x0080A440` | 1 | ✅ validada |
| 2 | 1 Muy fácil | RenderWare SDK | _rpSkinDeinitialize | `0x007C8820` | 5 | ✅ validada |
| 3 | 1 Muy fácil | RenderWare SDK | RwMatrixOrthoNormalize | `0x007F1920` | 5 | ✅ validada |
| 4 | 1 Muy fácil | RenderWare SDK | _rwDeviceRegisterPlugin | `0x007F5F60` | 5 | ✅ validada |
| 5 | 1 Muy fácil | RenderWare SDK | RwFreeListCreateAndPreallocateSpace | `0x00801B70` | 5 | ✅ validada |
| 6 | 1 Muy fácil | Controlador D3D9 de RenderWare | RxNodeDefinitionGetD3D9WorldSectorAllInOne | `0x0075E9F0` | 6 | ✅ validada |
| 7 | 1 Muy fácil | RenderWare SDK | _rpUVAnimCustomDataStreamGetSize | `0x007CC010` | 6 | ✅ validada |
| 8 | 1 Muy fácil | RenderWare SDK | RxHeapGetGlobalHeap | `0x008052F0` | 6 | ✅ validada |
| 9 | 1 Muy fácil | RenderWare SDK | _rwSListGetBegin | `0x00809530` | 7 | ✅ validada |
| 10 | 1 Muy fácil | RenderWare SDK | RtDictSchemaGetCurrentDict | `0x007CEEE0` | 8 | ✅ validada |
| 11 | 1 Muy fácil | RenderWare SDK | _rwSListGetNumEntries | `0x008094B0` | 8 | ✅ validada |
| 12 | 1 Muy fácil | RenderWare SDK | RpAnisotGetMaxSupportedMaxAnisotropy | `0x00748F20` | 9 | ✅ validada |
| 13 | 1 Muy fácil | CRT / runtime estático | helper sin símbolo confirmado (usado por ExeAsinSetDomainErrno) | `0x0082B181` | 9 | ✅ validada |
| 14 | 1 Muy fácil | Controlador D3D9 de RenderWare | RwD3D9DeleteVertexShader | `0x007FAC90` | 11 | ✅ validada |
| 15 | 1 Muy fácil | Controlador D3D9 de RenderWare | RwD3D9DeletePixelShader | `0x007FACF0` | 11 | ✅ validada |
| 16 | 1 Muy fácil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationGetSize | `0x007522E0` | 12 | ✅ validada |
| 17 | 1 Muy fácil | RenderWare SDK | RtAnimInterpolatorDestroy | `0x007CD590` | 12 | ✅ validada |
| 18 | 1 Muy fácil | RenderWare SDK | _rwSListDestroyEndEntries | `0x00809400` | 12 | ✅ validada |
| 19 | 1 Muy fácil | RenderWare SDK | _rpMaterialListGetMaterial | `0x0074E2B0` | 14 | ✅ validada |
| 20 | 1 Muy fácil | RenderWare SDK | RpAtomicGetWorld | `0x00751050` | 14 | ✅ validada |
| 21 | 1 Muy fácil | Controlador D3D9 de RenderWare | RpD3D9GeometryGetUsageFlags | `0x007588D0` | 14 | ✅ validada |
| 22 | 1 Muy fácil | Controlador D3D9 de RenderWare | RpD3D9WorldSectorGetUsageFlags | `0x00758900` | 14 | ✅ validada |
| 23 | 1 Muy fácil | RenderWare SDK | _rpMaterialListInitialize | `0x0074E1B0` | 15 | ✅ validada |
| 24 | 1 Muy fácil | RenderWare SDK | RpHAnimFrameGetHierarchy | `0x007C5160` | 15 | ✅ validada |
| 25 | 1 Muy fácil | RenderWare SDK | RwTextureGetMipmapping | `0x007F3550` | 16 | ✅ validada |
| 26 | 1 Muy fácil | RenderWare SDK | RwTextureGetAutoMipmapping | `0x007F3580` | 16 | ✅ validada |
| 27 | 1 Muy fácil | RenderWare SDK | _rwSListGetEnd | `0x00809540` | 16 | ✅ validada |
| 28 | 1 Muy fácil | RenderWare SDK | RwFrameDirty | `0x007F0340` | 17 | ✅ validada |
| 29 | 1 Muy fácil | RenderWare SDK | _rwSListGetEntry | `0x00809510` | 17 | ✅ validada |
| 30 | 1 Muy fácil | Controlador D3D9 de RenderWare | RpD3D9WorldSectorSetUsageFlags | `0x007588E0` | 18 | ✅ validada |
| 31 | 1 Muy fácil | RenderWare SDK | RpBuildMeshGenerateDefaultTriStrip | `0x007591B0` | 18 | ✅ validada |
| 32 | 1 Muy fácil | CRT / runtime estático | _atexit | `0x00821D1E` | 18 | ✅ validada |
| 33 | 1 Muy fácil | CRT / runtime estático | CMemoryMgr::Malloc | `0x00824257` | 18 | ✅ validada |
| 34 | 1 Muy fácil | RenderWare SDK | RpAtomicGetPluginOffset | `0x0074BEC0` | 19 | ✅ validada |
| 35 | 1 Muy fácil | RenderWare SDK | RwEngineGetPluginOffset | `0x007F2BE0` | 19 | ✅ validada |
| 36 | 1 Muy fácil | Controlador D3D9 de RenderWare | RwD3D9DynamicVertexBufferUnlock | `0x007F5C90` | 20 | ✅ validada |
| 37 | 1 Muy fácil | RenderWare SDK | RpAnisotTextureGetMaxAnisotropy | `0x00748F50` | 21 | ✅ validada |
| 38 | 1 Muy fácil | RenderWare SDK | RtDictSchemaDestruct | `0x007CED70` | 22 | ✅ validada |
| 39 | 1 Muy fácil | RenderWare SDK | RwRasterGetCurrentContext | `0x007FAE60` | 22 | ✅ validada |
| 40 | 1 Muy fácil | Controlador D3D9 de RenderWare | RwD3D9RasterIsCompressed | `0x00820C90` | 23 | ✅ validada |
| 41 | 1 Muy fácil | RenderWare SDK | RpAtomicSetStreamAlwaysCallBack | `0x0074BE30` | 24 | ✅ validada |
| 42 | 1 Muy fácil | RenderWare SDK | RpAtomicSetStreamRightsCallBack | `0x0074BE50` | 24 | ✅ validada |
| 43 | 1 Muy fácil | RenderWare SDK | RpAnisotTextureSetMaxAnisotropy | `0x00748F30` | 25 | ✅ validada |
| 44 | 1 Muy fácil | RenderWare SDK | _rpMeshHeaderCreate | `0x00758920` | 26 | ✅ validada |
| 45 | 1 Muy fácil | QuickTime/QTML | QuickTime dispatch thunk (selector 0x20002) | `0x00819840` | 26 | pendiente |
| 46 | 1 Muy fácil | RenderWare SDK | RwStreamWriteInt32 | `0x007ED460` | 28 | ✅ validada |
| 47 | 1 Muy fácil | RenderWare SDK | _rwObjectHasFrameReleaseFrame | `0x00804F40` | 29 | ✅ validada |
| 48 | 1 Muy fácil | Controlador D3D9 de RenderWare | RwD3D9IndexBufferCreate | `0x004C9970` | 30 | ✅ validada |
| 49 | 1 Muy fácil | RenderWare SDK | RwRGBAToPixel | `0x00803740` | 31 | ✅ validada |
| 50 | 1 Muy fácil | RenderWare SDK | RpUVAnimKeyFrameStreamGetSize | `0x007CC9D0` | 32 | ✅ validada |
| 51 | 1 Muy fácil | RenderWare SDK | RpClumpGetNumAtomics | `0x007498E0` | 33 | ✅ validada |
| 52 | 1 Muy fácil | RenderWare SDK | RpGeometryTriangleGetMaterial | `0x0074C760` | 33 | ✅ validada |
| 53 | 1 Muy fácil | RenderWare SDK | _rwFreeListCreate | `0x00801980` | 33 | ✅ validada |
| 54 | 1 Muy fácil | RenderWare SDK | RpWorldRegisterPluginStream | `0x0074FD00` | 34 | ✅ validada |
| 55 | 1 Muy fácil | RenderWare SDK | RpWorldSectorRegisterPluginStream | `0x00761CC0` | 34 | ✅ validada |
| 56 | 1 Muy fácil | Controlador D3D9 de RenderWare | RwD3D9CreatePixelShader | `0x007FACC0` | 34 | ✅ validada |
| 57 | 1 Muy fácil | RenderWare SDK | RwTextureRegisterPluginStream | `0x00804550` | 34 | ✅ validada |
| 58 | 1 Muy fácil | RenderWare SDK | _rwFrameListDeinitialize | `0x00807720` | 34 | ✅ validada |
| 59 | 1 Muy fácil | RenderWare SDK | _rwPluginRegistryGetPluginOffset | `0x00808470` | 34 | ✅ validada |
| 60 | 1 Muy fácil | RenderWare SDK | _rwStringStreamGetSize | `0x0080A470` | 34 | ✅ validada |
| 61 | 1 Muy fácil | RenderWare SDK | RtAnimAnimationGetNumNodes | `0x007CD4F0` | 35 | ✅ validada |
| 62 | 1 Muy fácil | RenderWare SDK | RwTextureRasterGenerateMipmaps | `0x007F3CD0` | 35 | ✅ validada |
| 63 | 1 Muy fácil | Controlador D3D9 de RenderWare | RwD3D9DynamicVertexBufferDestroy | `0x007F5AE0` | 35 | ✅ validada |
| 64 | 1 Muy fácil | CRT / runtime estático | __lock_file2 | `0x00823FD1` | 35 | ✅ validada |
| 65 | 1 Muy fácil | CRT / runtime estático | __unlock_file2 | `0x00824023` | 35 | ✅ validada |
| 66 | 1 Muy fácil | RenderWare SDK | RwEngineRegisterPlugin | `0x007F2BB0` | 36 | ✅ validada |
| 67 | 1 Muy fácil | Controlador D3D9 de RenderWare | _rwD3D9SetDepthStencilSurface | `0x007F5EF0` | 36 | ✅ validada |
| 68 | 1 Muy fácil | RenderWare SDK | _rpMaterialListFindMaterialIndex | `0x0074E420` | 39 | ✅ validada |
| 69 | 1 Muy fácil | RenderWare SDK | RpWorldRegisterPlugin | `0x0074FCD0` | 39 | ✅ validada |
| 70 | 1 Muy fácil | RenderWare SDK | RpLightRegisterPlugin | `0x00751D60` | 39 | ✅ validada |
| 71 | 1 Muy fácil | RenderWare SDK | RpWorldSectorRegisterPlugin | `0x00761C90` | 39 | ✅ validada |
| 72 | 1 Muy fácil | RenderWare SDK | RwCameraRegisterPlugin | `0x007EE450` | 39 | ✅ validada |
| 73 | 1 Muy fácil | RenderWare SDK | RwTextureRegisterPlugin | `0x007F3BB0` | 39 | ✅ validada |
| 74 | 1 Muy fácil | RenderWare SDK | RwTexDictionaryRegisterPlugin | `0x007F3C10` | 39 | ✅ validada |
| 75 | 1 Muy fácil | RenderWare SDK | RwRasterRegisterPlugin | `0x007FB0B0` | 39 | ✅ validada |
| 76 | 1 Muy fácil | RenderWare SDK | RwCameraSetViewOffset | `0x007EE1A0` | 40 | ✅ validada |
| 77 | 1 Muy fácil | RenderWare SDK | RwRasterLockPalette | `0x007FB0E0` | 40 | ✅ validada |
| 78 | 2 Fácil | RenderWare SDK | _rwPluginRegistryAddPlgnStrmlwysCB | `0x00808920` | 41 | pendiente |
| 79 | 2 Fácil | RenderWare SDK | _rwPluginRegistryAddPlgnStrmRightsCB | `0x00808950` | 41 | pendiente |
| 80 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9GetTransform | `0x007FA4F0` | 42 | pendiente |
| 81 | 2 Fácil | RenderWare SDK | RwImageFreePixels | `0x00802860` | 42 | pendiente |
| 82 | 2 Fácil | CRT / runtime estático | __ftbuf | `0x00827E74` | 42 | pendiente |
| 83 | 2 Fácil | RenderWare SDK | RpClumpRemoveLight | `0x0074A520` | 44 | pendiente |
| 84 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9VSGetWorldViewTransposedMatrix | `0x00764730` | 44 | pendiente |
| 85 | 2 Fácil | RenderWare SDK | RtDictSchemaInit | `0x007CED40` | 44 | pendiente |
| 86 | 2 Fácil | RenderWare SDK | RwEngineGetCurrentVideoMode | `0x007F2D20` | 45 | pendiente |
| 87 | 2 Fácil | RenderWare SDK | _rwPluginRegistryDeInitObject | `0x00808740` | 45 | pendiente |
| 88 | 2 Fácil | RenderWare SDK | RpWorldSetDefaultSectorPipeline | `0x0075E270` | 46 | pendiente |
| 89 | 2 Fácil | RenderWare SDK | RpAtomicSetDefaultPipeline | `0x0075E2A0` | 46 | pendiente |
| 90 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9DrawIndexedPrimitive | `0x007FA320` | 50 | pendiente |
| 91 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9SetVertexShader | `0x007F9FB0` | 51 | pendiente |
| 92 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9SetPixelShader | `0x007F9FF0` | 51 | pendiente |
| 93 | 2 Fácil | RenderWare SDK | RwFreeListPurgeAllFreeLists | `0x00801F90` | 51 | pendiente |
| 94 | 2 Fácil | RenderWare SDK | RpMaterialStreamGetSize | `0x0074E010` | 52 | pendiente |
| 95 | 2 Fácil | RenderWare SDK | _rwPluginRegistryCopyObject | `0x00808770` | 52 | pendiente |
| 96 | 2 Fácil | CRT / runtime estático | _sscanf | `0x008220AD` | 52 | pendiente |
| 97 | 2 Fácil | RenderWare SDK | _rpUVAnimCustomDataStreamWrite | `0x007CBFD0` | 53 | pendiente |
| 98 | 2 Fácil | RenderWare SDK | RwRasterGetNumLevels | `0x007FB160` | 54 | pendiente |
| 99 | 2 Fácil | RenderWare SDK | RwTextureStreamGetSize | `0x008045A0` | 54 | pendiente |
| 100 | 2 Fácil | RenderWare SDK | _rwPluginRegistryAddPluginStream | `0x008088E0` | 55 | pendiente |
| 101 | 2 Fácil | RenderWare SDK | _rpSkinSplitDataDestroy | `0x007C8B10` | 56 | pendiente |
| 102 | 2 Fácil | RenderWare SDK | RpUVAnimDestroy | `0x007CC0C0` | 56 | pendiente |
| 103 | 2 Fácil | RenderWare SDK | _rpMeshDestroy | `0x00758BC0` | 58 | pendiente |
| 104 | 2 Fácil | RenderWare SDK | _rpMeshHeaderForAllMeshes | `0x00758D30` | 58 | pendiente |
| 105 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9SetRenderTarget | `0x007F5F20` | 59 | pendiente |
| 106 | 2 Fácil | RenderWare SDK | _rwPluginRegistryGetSize | `0x00808B00` | 61 | pendiente |
| 107 | 2 Fácil | RenderWare SDK | RpWorldForAllMaterials | `0x0074FBC0` | 62 | pendiente |
| 108 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9SetSamplerState | `0x007FC3C0` | 63 | pendiente |
| 109 | 2 Fácil | RenderWare SDK | _rpBuildMeshDestroy | `0x00758B80` | 64 | pendiente |
| 110 | 2 Fácil | RenderWare SDK | _rpMaterialSetDefaultSurfaceProperties | `0x0074D870` | 65 | pendiente |
| 111 | 2 Fácil | RenderWare SDK | RpClumpForAllCameras | `0x00749BB0` | 67 | pendiente |
| 112 | 2 Fácil | RenderWare SDK | RpClumpForAllLights | `0x00749C00` | 67 | pendiente |
| 113 | 2 Fácil | RenderWare SDK | _rwMatrixDeterminant | `0x007F1450` | 69 | pendiente |
| 114 | 2 Fácil | RenderWare SDK | _rwPluginRegistryInvokeRights | `0x00808AB0` | 69 | pendiente |
| 115 | 2 Fácil | RenderWare SDK | _rwSListDestroy | `0x00809440` | 69 | pendiente |
| 116 | 2 Fácil | RenderWare SDK | RwIm3DSetTransformPipeline | `0x007EFAC0` | 71 | pendiente |
| 117 | 2 Fácil | RenderWare SDK | _rpSectorDefaultRenderCallBack | `0x0074EEC0` | 72 | pendiente |
| 118 | 2 Fácil | CRT / runtime estático | _pow | `0x008220F0` | 72 | pendiente |
| 119 | 2 Fácil | RenderWare SDK | RwErrorGet | `0x00808880` | 74 | pendiente |
| 120 | 2 Fácil | RenderWare SDK | _rtDictDestruct | `0x007CEF60` | 75 | pendiente |
| 121 | 2 Fácil | RenderWare SDK | RtDictSchemaForAllDictionaries | `0x007CF5B0` | 75 | pendiente |
| 122 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9DeleteVertexDeclaration | `0x007FAC10` | 76 | pendiente |
| 123 | 2 Fácil | RenderWare SDK | _rpMeshSize | `0x00759090` | 77 | pendiente |
| 124 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9VSGetComposedTransformMatrix | `0x007646E0` | 80 | pendiente |
| 125 | 2 Fácil | RenderWare SDK | RwStreamReadReal | `0x007ED4F0` | 80 | pendiente |
| 126 | 2 Fácil | RenderWare SDK | RwStreamReadInt32 | `0x007ED540` | 80 | pendiente |
| 127 | 2 Fácil | RenderWare SDK | _rwStreamWriteVersionedChunkHeader | `0x007ED270` | 82 | pendiente |
| 128 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9CameraIsSphereFullyInsideFrustum | `0x007FAD30` | 82 | pendiente |
| 129 | 2 Fácil | RenderWare SDK | RxPipelineNodeCloneDefinition | `0x008059C0` | 82 | pendiente |
| 130 | 2 Fácil | RenderWare SDK | _rwPluginRegistryInitObject | `0x008086E0` | 83 | pendiente |
| 131 | 2 Fácil | RenderWare SDK | _rwSqrt | `0x007EDB30` | 84 | pendiente |
| 132 | 2 Fácil | Controlador D3D9 de RenderWare | _rpD3D9RemoveDynamicGeometry | `0x0074E9E0` | 85 | pendiente |
| 133 | 2 Fácil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationInstColor | `0x00754AE0` | 85 | pendiente |
| 134 | 2 Fácil | RenderWare SDK | RtDictForAllEntries | `0x007CF060` | 85 | pendiente |
| 135 | 2 Fácil | RenderWare SDK | RwErrorSet | `0x00808820` | 86 | pendiente |
| 136 | 2 Fácil | RenderWare SDK | _rwInvSqrt | `0x007EDB90` | 87 | pendiente |
| 137 | 2 Fácil | RenderWare SDK | _rxPipelineClose | `0x00804F60` | 88 | pendiente |
| 138 | 2 Fácil | RenderWare SDK | RpWorldForAllWorldSectors | `0x0074FC70` | 89 | pendiente |
| 139 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9SetStreamSource | `0x007FA030` | 90 | pendiente |
| 140 | 2 Fácil | RenderWare SDK | RxHeapDestroy | `0x00809F30` | 90 | pendiente |
| 141 | 2 Fácil | RenderWare SDK | _rpMaterialListStreamGetSize | `0x0074E450` | 91 | pendiente |
| 142 | 2 Fácil | RenderWare SDK | _rwMatrixOrthogonalError | `0x007F14A0` | 91 | pendiente |
| 143 | 2 Fácil | RenderWare SDK | _rxPacketDestroy | `0x0080F070` | 91 | pendiente |
| 144 | 2 Fácil | RenderWare SDK | _rpMeshOptimise | `0x0075D970` | 93 | pendiente |
| 145 | 2 Fácil | RenderWare SDK | _rpUVAnimCustomDataStreamRead | `0x007CBF70` | 94 | pendiente |
| 146 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9SetMaterial | `0x007FC430` | 97 | pendiente |
| 147 | 2 Fácil | RenderWare SDK | _rwFramePurgeClone | `0x007F01A0` | 98 | pendiente |
| 148 | 2 Fácil | Controlador D3D9 de RenderWare | _rpD3D9AddDynamicGeometry | `0x0074E970` | 99 | pendiente |
| 149 | 2 Fácil | RenderWare SDK | RpMatFXMaterialSetUVTransformMatrices | `0x008129E0` | 100 | pendiente |
| 150 | 2 Fácil | RenderWare SDK | _rwStringStreamWrite | `0x0080A4A0` | 102 | pendiente |
| 151 | 2 Fácil | Controlador D3D9 de RenderWare | _rpD3D9VertexShaderCachePurge | `0x0075EE60` | 103 | pendiente |
| 152 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9DynamicVertexBufferRestore | `0x007F58D0` | 103 | pendiente |
| 153 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9VSGetWorldMultiplyTransposeMatrix | `0x00764960` | 104 | pendiente |
| 154 | 2 Fácil | RenderWare SDK | _rxHeapReset | `0x00809EC0` | 104 | pendiente |
| 155 | 2 Fácil | CRT / runtime estático | _free | `0x0082413F` | 104 | pendiente |
| 156 | 2 Fácil | RenderWare SDK | RtDictRemoveEntry | `0x007CF0C0` | 106 | pendiente |
| 157 | 2 Fácil | RenderWare SDK | RwResourcesFreeResEntry | `0x00807DE0` | 106 | pendiente |
| 158 | 2 Fácil | RenderWare SDK | RtAnimInterpolatorCreate | `0x007CD520` | 108 | pendiente |
| 159 | 2 Fácil | RenderWare SDK | RwTextureSetMaskName | `0x007F3910` | 110 | pendiente |
| 160 | 2 Fácil | RenderWare SDK | RwFrameDestroyHierarchy | `0x007F08A0` | 112 | pendiente |
| 161 | 2 Fácil | RenderWare SDK | RpMatFXMaterialSetDualBlendModes | `0x008128C0` | 112 | pendiente |
| 162 | 2 Fácil | RenderWare SDK | _rwPluginRegistrySkipDataChunks | `0x00808C10` | 113 | pendiente |
| 163 | 2 Fácil | RenderWare SDK | RpLightForAllWorldSectors | `0x007519F0` | 114 | pendiente |
| 164 | 2 Fácil | RenderWare SDK | RtDictFindNamedEntry | `0x007CEFE0` | 114 | pendiente |
| 165 | 2 Fácil | RenderWare SDK | RpHAnimKeyFrameStreamRead | `0x007C64C0` | 116 | pendiente |
| 166 | 2 Fácil | RenderWare SDK | RwPalQuantTerm | `0x0080C520` | 116 | pendiente |
| 167 | 2 Fácil | RenderWare SDK | RpMaterialSetUVAnim | `0x007CC3B0` | 118 | pendiente |
| 168 | 2 Fácil | RenderWare SDK | _rwResourcesPurge | `0x00807E50` | 118 | pendiente |
| 169 | 2 Fácil | RenderWare SDK | _rwFrameSyncHierarchyLTM | `0x008097D0` | 127 | pendiente |
| 170 | 2 Fácil | RenderWare SDK | RpWorldSectorGetWorld | `0x0074F4E0` | 129 | pendiente |
| 171 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9CameraIsBBoxFullyInsideFrustum | `0x007FAD90` | 130 | pendiente |
| 172 | 2 Fácil | Controlador D3D9 de RenderWare | _rpD3D9GetMinMaxValuesV2d | `0x007558F0` | 131 | pendiente |
| 173 | 2 Fácil | RenderWare SDK | RwFreeListDestroy | `0x00801B80` | 136 | pendiente |
| 174 | 2 Fácil | CRT / runtime estático | __stbuf | `0x00827DEC` | 136 | pendiente |
| 175 | 2 Fácil | RenderWare SDK | _rwMatrixNormalError | `0x007F1500` | 137 | pendiente |
| 176 | 2 Fácil | RenderWare SDK | RwFreeListPurge | `0x00801E00` | 138 | pendiente |
| 177 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9LightsClose | `0x00755FE0` | 139 | pendiente |
| 178 | 2 Fácil | RenderWare SDK | RwStreamWriteReal | `0x007ED3D0` | 142 | pendiente |
| 179 | 2 Fácil | RenderWare SDK | RpWorldAddClump | `0x00751300` | 143 | pendiente |
| 180 | 2 Fácil | Controlador D3D9 de RenderWare | _rwD3D9VSSetActiveWorldMatrix | `0x00764650` | 143 | pendiente |
| 181 | 2 Fácil | Controlador D3D9 de RenderWare | RwD3D9SetRenderTarget | `0x007F9E90` | 148 | pendiente |
| 182 | 3 Media | RenderWare SDK | _rpSkinSplitDataCreate | `0x007C8A70` | 158 | pendiente |
| 183 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetWorldViewMultiplyTransposeMatrix | `0x007649D0` | 159 | pendiente |
| 184 | 3 Media | RenderWare SDK | RxPipelineExecute | `0x00805710` | 159 | pendiente |
| 185 | 3 Media | RenderWare SDK | _rpWorldSectorDeinstanceAll | `0x0074ECA0` | 165 | pendiente |
| 186 | 3 Media | RenderWare SDK | _rwSListGetNewEntry | `0x00809240` | 166 | pendiente |
| 187 | 3 Media | QuickTime/QTML | QuickTime runtime shutdown helper | `0x00819770` | 168 | pendiente |
| 188 | 3 Media | RenderWare SDK | RwPalQuantInit | `0x0080C470` | 176 | pendiente |
| 189 | 3 Media | RenderWare SDK | RwMatrixOptimize | `0x007F17E0` | 179 | pendiente |
| 190 | 3 Media | Controlador D3D9 de RenderWare | _rpD3D9VertexShaderSetBumpMatrix | `0x00761B70` | 182 | pendiente |
| 191 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetWorldNormalizedTransposeMatrix | `0x00764C60` | 187 | pendiente |
| 192 | 3 Media | RenderWare SDK | RwBBoxCalculate | `0x00808F60` | 190 | pendiente |
| 193 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9RenderStateFlushCache | `0x007FC200` | 195 | pendiente |
| 194 | 3 Media | RenderWare SDK | RwImageMakeMask | `0x00802A20` | 198 | pendiente |
| 195 | 3 Media | RenderWare SDK | _rwMatrixIdentityError | `0x007F1590` | 200 | pendiente |
| 196 | 3 Media | RenderWare SDK | _rwPluginRegistryWriteDataChunks | `0x00808B40` | 201 | pendiente |
| 197 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetRadiusInLocalSpace | `0x00764F60` | 203 | pendiente |
| 198 | 3 Media | RenderWare SDK | _rwSListCreate | `0x00809160` | 205 | pendiente |
| 199 | 3 Media | RenderWare SDK | _rxPipelineOpen | `0x00804FE0` | 210 | pendiente |
| 200 | 3 Media | RenderWare SDK | _rpSkinInitialize | `0x007C8740` | 213 | pendiente |
| 201 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetInverseWorldMatrix | `0x007647B0` | 214 | pendiente |
| 202 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetWorldNormalizedMultiplyTransposeMatrix | `0x00764A70` | 219 | pendiente |
| 203 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9DynamicVertexBufferCreate | `0x007F5A00` | 220 | pendiente |
| 204 | 3 Media | Controlador D3D9 de RenderWare | _rpD3D9FindFormatV2d | `0x00755AA0` | 227 | pendiente |
| 205 | 3 Media | RenderWare SDK | _rpBuildMeshCreate | `0x00758A90` | 234 | pendiente |
| 206 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetPointInLocalSpace | `0x00764E70` | 234 | pendiente |
| 207 | 3 Media | RenderWare SDK | RxRenderStateVectorSetDefaultRenderStateVector | `0x0080EA40` | 234 | pendiente |
| 208 | 3 Media | RenderWare SDK | RwImageRead | `0x00802FD0` | 240 | pendiente |
| 209 | 3 Media | RenderWare SDK | RwResourcesEmptyArena | `0x008081F0` | 240 | pendiente |
| 210 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9DeviceSupportsDXTTexture | `0x007F9C30` | 242 | pendiente |
| 211 | 3 Media | RenderWare SDK | RpHAnimHierarchyCreate | `0x007C4C30` | 247 | pendiente |
| 212 | 3 Media | RenderWare SDK | RwTextureStreamWrite | `0x008045E0` | 250 | pendiente |
| 213 | 3 Media | RenderWare SDK | RwImageCreateResample | `0x0080CD10` | 252 | pendiente |
| 214 | 3 Media | RenderWare SDK | RwFreeListForAllUsed | `0x00801E90` | 254 | pendiente |
| 215 | 3 Media | RenderWare SDK | RwCameraStreamRead | `0x00808DE0` | 263 | pendiente |
| 216 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9RasterConvertToNonPalettized | `0x004CD250` | 265 | pendiente |
| 217 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetWorldNormalizedViewMultiplyTransposeMatrix | `0x00764B50` | 269 | pendiente |
| 218 | 3 Media | RenderWare SDK | RxHeapCreate | `0x00809F90` | 270 | pendiente |
| 219 | 3 Media | RenderWare SDK | _rwStringOpen | `0x0080A240` | 272 | pendiente |
| 220 | 3 Media | RenderWare SDK | RtDictStreamWrite | `0x007CF490` | 283 | pendiente |
| 221 | 3 Media | RenderWare SDK | _rpBuildMeshAddTriangle | `0x00758C00` | 291 | pendiente |
| 222 | 3 Media | RenderWare SDK | _rwPluginRegistryReadDataChunks | `0x00808980` | 299 | pendiente |
| 223 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9VSGetNormalInLocalSpace | `0x00764D30` | 308 | pendiente |
| 224 | 3 Media | RenderWare SDK | RwFrameAddChildNoUpdate | `0x007F09C0` | 313 | pendiente |
| 225 | 3 Media | RenderWare SDK | RxHeapFree | `0x008098B0` | 319 | pendiente |
| 226 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9SetTransformWorld | `0x007FA520` | 320 | pendiente |
| 227 | 3 Media | RenderWare SDK | RwImageSetGamma | `0x00803FE0` | 325 | pendiente |
| 228 | 3 Media | RenderWare SDK | RpAtomicGetWorldBoundingSphere | `0x00749330` | 327 | pendiente |
| 229 | 3 Media | RenderWare SDK | _rpMeshWrite | `0x00758D70` | 329 | pendiente |
| 230 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9DestroyVertexBuffer | `0x007F56A0` | 329 | pendiente |
| 231 | 3 Media | RenderWare SDK | RpLightStreamRead | `0x00751F00` | 338 | pendiente |
| 232 | 3 Media | Controlador D3D9 de RenderWare | _rpD3D9VertexShaderSetEnvMatrix | `0x00761A10` | 349 | pendiente |
| 233 | 3 Media | RenderWare SDK | _rpWorldSectorDestroyRecurse | `0x0074ED50` | 364 | pendiente |
| 234 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9Im2DRenderOpen | `0x007FB480` | 368 | pendiente |
| 235 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9DynamicVertexBufferLock | `0x007F5B10` | 369 | pendiente |
| 236 | 3 Media | RenderWare SDK | RwImageReadMaskedImage | `0x008035C0` | 375 | pendiente |
| 237 | 3 Media | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationInstIndicesRemap | `0x00754C80` | 386 | pendiente |
| 238 | 3 Media | RenderWare SDK | RwImageGammaCorrect | `0x00803E30` | 395 | pendiente |
| 239 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9CreateVertexBuffer | `0x007F5500` | 404 | pendiente |
| 240 | 3 Media | CRT / runtime estático | CMemoryMgr::Realloc | `0x00824269` | 406 | pendiente |
| 241 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9CameraAttachWindow | `0x007F8D70` | 424 | pendiente |
| 242 | 3 Media | RenderWare SDK | _rwFrameSyncDirty | `0x00809550` | 425 | pendiente |
| 243 | 3 Media | RenderWare SDK | RpMatFXMaterialSetBumpMapTexture | `0x00812160` | 438 | pendiente |
| 244 | 3 Media | RenderWare SDK | RpGeometryAddMorphTargets | `0x0074C310` | 443 | pendiente |
| 245 | 3 Media | RenderWare SDK | _rpMeshRead | `0x00758EC0` | 462 | pendiente |
| 246 | 3 Media | RenderWare SDK | RtAnimInterpolatorAddAnimTime | `0x007CD8D0` | 471 | pendiente |
| 247 | 3 Media | Controlador D3D9 de RenderWare | RwD3D9CreateVertexDeclaration | `0x007FAA30` | 480 | pendiente |
| 248 | 3 Media | Controlador D3D9 de RenderWare | _rwD3D9DeviceReleaseVideoMemory | `0x007F7F70` | 488 | pendiente |
| 249 | 3 Media | RenderWare SDK | _rpTriListMeshGenerate | `0x0075D530` | 497 | pendiente |
| 250 | 3 Media | CRT / runtime estático | helper sin símbolo confirmado (usado por CPedGeometryAnalyser::IsWanderPathClear) | `0x008219F0` | 498 | pendiente |
| 251 | 4 Difícil | RenderWare SDK | RwTextureStreamRead | `0x008046E0` | 511 | pendiente |
| 252 | 4 Difícil | RenderWare SDK | _rwPluginRegistryAddPlugin | `0x008084A0` | 515 | pendiente |
| 253 | 4 Difícil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationUnInstV2d | `0x007555E0` | 531 | pendiente |
| 254 | 4 Difícil | Controlador D3D9 de RenderWare | _rwD3D9Im3DRenderOpen | `0x0080E020` | 578 | pendiente |
| 255 | 4 Difícil | Controlador D3D9 de RenderWare | _rwD3D9LightsOpen | `0x00755D80` | 604 | pendiente |
| 256 | 4 Difícil | RenderWare SDK | _rwStringStreamFindAndRead | `0x0080A510` | 609 | pendiente |
| 257 | 4 Difícil | RenderWare SDK | RwIm3DSetRenderPipeline | `0x007EFB10` | 644 | pendiente |
| 258 | 4 Difícil | RenderWare SDK | _rwFrameListStreamRead | `0x00807970` | 671 | pendiente |
| 259 | 4 Difícil | RenderWare SDK | RpWorldUnlock | `0x0074F210` | 710 | pendiente |
| 260 | 4 Difícil | RenderWare SDK | RwMatrixRotateOneMinusCosineSine | `0x007F1D00` | 717 | pendiente |
| 261 | 4 Difícil | RenderWare SDK | RpMaterialStreamRead | `0x0074DD30` | 724 | pendiente |
| 262 | 4 Difícil | RenderWare SDK | RwPalQuantAddImage | `0x0080AA80` | 784 | pendiente |
| 263 | 4 Difícil | RenderWare SDK | _rpMaterialListStreamRead | `0x0074E600` | 870 | pendiente |
| 264 | 4 Difícil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationUnInstV3d | `0x007551F0` | 936 | pendiente |
| 265 | 4 Difícil | RenderWare SDK | RwImageApplyMask | `0x00802AF0` | 936 | pendiente |
| 266 | 4 Difícil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationInstTangent | `0x00754E20` | 973 | pendiente |
| 267 | 4 Difícil | Controlador D3D9 de RenderWare | _rxD3D9VideoMemoryRasterListRestore | `0x004CC970` | 990 | pendiente |
| 268 | 4 Difícil | RenderWare SDK | RtPNGImageRead | `0x007CF9B0` | 1039 | pendiente |
| 269 | 4 Difícil | RenderWare SDK | RwImageResample | `0x0080C600` | 1045 | pendiente |
| 270 | 4 Difícil | RenderWare SDK | RwPalQuantMatchImage | `0x0080BF20` | 1289 | pendiente |
| 271 | 4 Difícil | RenderWare SDK | RwPalQuantResolvePalette | `0x0080AF60` | 1320 | pendiente |
| 272 | 4 Difícil | RenderWare SDK | RpGeometryStreamRead | `0x0074D190` | 1332 | pendiente |
| 273 | 4 Difícil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationInstV2d | `0x007544E0` | 1467 | pendiente |
| 274 | 5 Muy difícil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationInstV3d | `0x00752AD0` | 1700 | pendiente |
| 275 | 5 Muy difícil | Controlador D3D9 de RenderWare | _rwD3D9RenderStateReset | `0x007FD100` | 1806 | pendiente |
| 276 | 5 Muy difícil | CRT / runtime estático | helper sin símbolo confirmado (usado por CrtPrintf) | `0x00827F4B` | 1904 | pendiente |
| 277 | 5 Muy difícil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationInstWeights | `0x00752320` | 1908 | pendiente |
| 278 | 5 Muy difícil | Controlador D3D9 de RenderWare | _rpD3D9VertexDeclarationInstV3dMorph | `0x00753B60` | 2370 | pendiente |
| 279 | 5 Muy difícil | CRT / runtime estático | helper sin símbolo confirmado (usado por FlcCrtPow) | `0x00829B99` | 2861 | pendiente |

</details>

<a id="metodo"></a>
## Cómo leer y actualizar

- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.
- **Validated:** `1` indica evidencia aprobada vinculada para esa entrada. No implica vigencia actual ni independencia del EXE.
- **Locked:** indicador de bloqueo del hook; hay 0 entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.
- **Porcentajes:** las categorías cuentan clases; el avance de cada clase cuenta funciones. Se muestran dos decimales.
- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.

- **Acceso al código:** pulsa el nombre para abrir el archivo que registra sus hooks. Si el registro está repartido, los enlaces adicionales abren los demás archivos. Algunas clases registran sus hooks en una cabecera.
- **Comentarios de estado:** resumen por clase/grupo, con nombres de pendientes y bloqueadas. Los conteos abarcan la clase completa aunque su implementación esté repartida entre archivos.

Sincronizar primero `validated` desde el registro de evidencia:

```powershell
python contrib/sync_validated_flags.py
```

Actualizar los flags de reconstrucción solo después de comparar con el original dentro del motor y regenerar desde la raíz:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
```

Para sincronizar también los comentarios de los archivos fuente:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md --update-source-status
python contrib/validation_inventory.py
```

Los comentarios se generan desde el CSV y la evidencia vinculada; no modifican flags ni acreditan validación. La vigencia se consulta en el [inventario](ValidationInventory.md). Añadir comentarios cambia las huellas de los archivos: el auditor conserva la evidencia histórica y exige revisar su vigencia, sin declarar fallidas las pruebas ni actualizar sus hashes de referencia.

El formato vive en [contrib/hooks2md.py](../contrib/hooks2md.py). La [automatización de CI](../.github/workflows/update-re-progress.yml) también usa este generador. La guía de prioridades es una evaluación separada y requiere revisión cuando cambian las funciones pendientes.
