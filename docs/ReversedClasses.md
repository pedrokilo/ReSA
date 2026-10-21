# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-10-21 17:54 UTC

## Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. **La meta de este informe es completar todas las funciones del juego** (11,804 funciones y 3.01 MB de código en el EXE: el código propio del juego y su capa de plataforma). RenderWare, Direct3D y el CRT son bibliotecas enlazadas dentro del EXE: no forman parte de la meta, no se cuentan en las tablas y solo aparecen en la tabla de [cobertura real del EXE](#cobertura-exe) para aclarar qué queda fuera. Completar el juego es un hito hacia **ReSA — Zero Original Function Dependency**.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Funciones del juego reemplazadas por C++ en el EXE (meta del proyecto) | `███████████████░░░░░` | 8,799/11,804 · 74.54% |
| Bytes de código del juego reemplazados | `█████████████████░░░` | 2,583,163/3,007,877 · 85.88% |
| Clases del juego completas | `████████████████████` | 803/811 · 99.01% |
| Clases del juego validadas (de las completas) | `████████████████████` | 803/803 · 100.00% |
| Entradas de catálogo del juego reconstruidas | `████████████████████` | 8,822/8,952 · 98.55% |
| Entradas del juego validadas (de las reconstruidas) | `████████████████████` | 8,822/8,822 · 100.00% |
| Entradas del juego reconstruidas escritas solo en C++ (sin ensamblador) | `████████████████████` | 8,820/8,822 · 99.98% |

Las dos primeras filas miden el propio EXE (análisis estático, ver [cobertura real del EXE](#cobertura-exe)) y son la medida del avance hacia la meta; las demás miden las entradas del catálogo, que solo conoce una parte del código del juego, por eso su porcentaje es mayor. Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL. La última barra cuenta, de las funciones reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md).

**Migración de ensamblador a C++:** en `source/game_sa` ya no queda ningún `__asm` ni función `naked`: todo lo reconstruido está escrito en C++, con resultados validados contra el EXE dentro del juego. Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Historial y excepciones documentadas: [estado de la migración](../reversing/CppOnlyMigration/README.md) · [FILES.md](../reversing/CppOnlyMigration/FILES.md).

Hay 8,822 entradas catalogadas del juego, 0 bloqueadas y 8,822 sin bloqueo. No constituyen un inventario exhaustivo del código del juego; las filas de RenderWare y del CRT del catálogo (79) quedan fuera de este informe. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Las extensiones o adaptaciones NOTSA (4, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

### Siguientes mediciones

Estas dimensiones todavía no tienen un porcentaje útil: indican el trabajo que falta para poder medirlas.

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validaciones vigentes para el árbol actual | 0/8,822 validadas confirmadas como vigentes | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |
| Dependencias de funciones originales eliminadas | Medido (cota inferior): 914 funciones del juego siguen llamándose directamente desde código del juego ya reemplazado; cierre transitivo: 1,116 funciones / 244,404 bytes. Ver [Cobertura real del EXE](#cobertura-exe) | Resolver las llamadas indirectas (tablas de punteros, callbacks) para convertir la cota inferior en un valor exacto, y eliminar o sustituir las dependencias directas que quedan. |

**Validación consolidada parcialmente:** 8822 funciones del juego con evidencia aprobada vinculada, 440 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias. Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

## Vista general

| Clases validadas | Completas por validar | Funciones validadas | Reconstruidas por validar | Funciones por reconstruir |
| :---: | :---: | :---: | :---: | :---: |
| **803 / 803** | **0 / 803** | **8,822 / 8,822** | **0 / 8,822** | **130 / 8,952** |
| 100.00% | 0.00% | 100.00% | 0.00% | 1.45% |

Reconstrucción por clases: 803 completas, 6 parciales y 2 sin revertir de 811 (incluye 130 funciones del juego con envoltura al EXE aún sin reconstruir, en 8 clases o grupos).

**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cohorte nuevo por dificultad](#cohorte-nuevo) · [Cómo leer y actualizar](#metodo)

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="cobertura-exe"></a>

## Cobertura real del EXE (estimación)

Esta tabla es la única parte del informe que habla de lo que no es código del juego: sirve para aclarar qué queda fuera de la meta. Las dos primeras barras de progreso salen de aquí (región del juego); las demás miden filas del catálogo (`hooks.csv`). Se mide el propio ejecutable original con el análisis estático ([MemoryMap](MemoryMap/README.md): 17,765 entradas de función detectadas y 3.72 MB de cuerpos de función), cruzado con el catálogo, el registro de validación y las envolturas pendientes. 8,901 filas reconstruidas equivalen a **8,869 funciones distintas del EXE ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo).

| Región del EXE | Funciones | Bytes de código | Reemplazadas por C++ (funciones) | Reemplazadas (bytes) | Funciones llamadas directamente desde código ya reemplazado |
| --- | ---: | ---: | --- | --- | ---: |
| Juego y plataforma (0x401000–0x74FFFF) | 11,804 | 3,007,877 | `███████████████░░░░░` 8,799 · 74.5% | `█████████████████░░░` 85.9% | 914 |
| RenderWare y D3D (0x750000–0x81FFFF) | 1,889 | 600,971 | `█░░░░░░░░░░░░░░░░░░░` 61 · 3.2% | `░░░░░░░░░░░░░░░░░░░░` 0.3% | 224 |
| CRT/MSVC y runtime (0x820000 en adelante) | 4,072 | 115,711 | `░░░░░░░░░░░░░░░░░░░░` 9 · 0.2% | `░░░░░░░░░░░░░░░░░░░░` 0.2% | 58 |
| **Total** | **17,765** | **3,724,559** | `██████████░░░░░░░░░░` 8,869 · 49.9% | `██████████████░░░░░░` 69.4% | **1,196** |

**Qué significa para un ejecutable independiente.** No hace falta reconstruir las 17,765 funciones: RenderWare/D3D y el CRT son bibliotecas enlazadas dentro del EXE (5,961 entradas, 19% de los bytes) que se sustituirían enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego (11,804 funciones, 81% de los bytes), y de él ya está reemplazado el **86% de los bytes** (75% de las funciones: las que quedan son más pequeñas que la media). Entre las 11,804 entradas del juego hay 1,835 de 16 bytes o menos (stubs, thunks y helpers).

### Lo que falta del código del juego: 3,005 funciones, 425 KB

Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).

| Grupo (de mayor a menor tamaño) | Funciones | Bytes | Lectura |
| --- | ---: | ---: | --- |
| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | 1,116 | 244,404 | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. |
| C. Llamadas directas solo desde código aún no reemplazado | 544 | 71,124 | Dependen de reconstruir antes A y B; parte puede ser código muerto. |
| D. Sin llamadas directas ni vtable | 426 | 59,754 | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |
| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | 919 | 49,432 | Probablemente necesarios: se ejecutan por despacho virtual. |

Del código del juego ya reemplazado, **914 funciones del juego siguen llamándose directamente desde el EXE**. Esa es la lista corta de dependencias reales que hay que eliminar. Las que caen en RenderWare/D3D o el CRT se muestran solo en la última columna de la tabla de cobertura.

> [!NOTE]
> **Cotas y límites.** Es un análisis estático: hay 6,004 llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.

<a id="no-en-cpp"></a>

### Funciones del juego que ya tenemos y aún no están en C++: 130

Son envolturas: la DLL ya tiene la función y su hook, pero el cuerpo todavía llama al EXE original. Solo se listan funciones del juego, que son la meta del proyecto. Todas las tablas van **de mayor a menor tamaño**: «Bytes» es la suma de los cuerpos de las funciones pendientes (análisis estático) y «Tamaño» clasifica la función más grande de la fila (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B; una instrucción x86 ocupa entre 1 y 15 bytes, unos 3–4 de media). Las funciones del EXE sin nombre ni hook (el resto de lo que falta) están en el [mapa del código del juego](JuegoExe/README.md). Fuente: [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

#### Código del juego: 130 funciones en 8 clases o grupos · 110,173 bytes

| Clase o grupo | Funciones | Bytes | Función más grande | Tamaño | Funciones (de mayor a menor) |
| --- | ---: | ---: | ---: | --- | --- |
| CCarCtrl | 18 | 41,577 | 7,915 | muy grande | `GenerateOneRandomCar` 0x00430050 · 7,915 B<br>`SteerAICarWithPhysics_OnlyMission` 0x00436A90 · 4,243 B<br>`SteerAICarWithPhysicsFollowPath` 0x00434900 · 3,729 B<br>`PickNextNodeRandomly` 0x0042DE80 · 3,591 B<br>`SteerAICarWithPhysicsFollowPath_Racing` 0x00435830 · 3,320 B<br>`GenerateCarCreationCoors2` 0x00424210 · 2,751 B<br>`PickNextNodeToFollowPath` 0x00427740 · 2,193 B<br>`PickNextNodeToChaseCar` 0x00426EF0 · 2,111 B<br>`DragCarToPoint` 0x0042EC90 · 1,748 B<br>`SteerAICarWithPhysics` 0x00437C20 · 1,717 B<br>`FindAngleToWeaveThroughTraffic` 0x004325C0 · 1,339 B<br>`FindNodesThisCarIsNearestTo` 0x0042BD20 · 1,323 B<br>`PossiblyRemoveVehicle` 0x00424F80 · 1,204 B<br>`CreateConvoy` 0x0042C740 · 1,168 B<br>`CreatePoliceChase` 0x0042C2B0 · 1,161 B<br>`GenerateOneEmergencyServicesCar` 0x0042B7D0 · 1,146 B<br>`ClitargetOrientationToLink` 0x00422760 · 918 B<br>`GenerateEmergencyServicesCar` sin dirección |
| (helpers y globales sin clase) | 60 | 15,804 | 3,445 | muy grande | `helper sin símbolo confirmado` 0x0051D770 · 3,445 B<br>`CCamera unnamed geometry helper` 0x00514030 · 2,084 B<br>`CTaskSimpleGangDriveBy helper para shooter sin símbolo confirmado` 0x00627CC0 · 1,672 B<br>`CFont helper de impresión de segmento sin símbolo confirmado` 0x00719B40 · 1,440 B<br>`FightHitPedExe` 0x0061CBA0 · 1,289 B<br>`CPathFind generador de coordenadas de ped sin símbolo confirmado` 0x0044E790 · 1,274 B<br>`CCam helper arrest camera de from/initialise sin símbolo confirmado` 0x00512EF0 · 804 B<br>`CCam unnamed arrest cop-source helper` 0x00515D80 · 651 B<br>`CWaterLevel helper de agua cercana` 0x006E6A10 · 646 B<br>`Creación de objeto toalla para sunbathe sin símbolo confirmado` 0x006EABA0 · 627 B<br>`RsLoadPresetViews` 0x00619D60 · 552 B<br>`FightHitObjExe` 0x0061D400 · 494 B<br>`CTaskSimpleGangDriveBy helper de animación sin símbolo confirmado` 0x00627B20 · 414 B<br>`CWaterLevel helper de agua distante` 0x006E6870 · 412 B<br>`ComputeTargetDoorToExit` sin dirección<br>`ExeFree` sin dirección<br>`FightChooseAttackAIExe` sin dirección<br>`FightChooseAttackPlayerExe` sin dirección<br>`FightFindTargetOnGroundExe` sin dirección<br>`FightGetAvailableComboSetExe` sin dirección<br>`FightStartAnimExe` sin dirección<br>`GaCalcDistToRectSq` sin dirección<br>`GaCountCarsCenterWithin` sin dirección<br>`GaIsAnyCarBlockingDoor` sin dirección<br>`GaIsAnyOtherCarTouching` sin dirección<br>`GaIsAnyOtherPedTouching` sin dirección<br>`GaIsPlayerOutside` sin dirección<br>`GaNeatlyLineUp` sin dirección<br>`GaRestoreHideOut` sin dirección<br>`GaRestoreImpounding` sin dirección<br>`GaSlideDoorClosed` sin dirección<br>`GaSlideDoorOpen` sin dirección<br>`GaStoreAndRemoveImpounding` sin dirección<br>`GaThrowCarsNearDoorOut` sin dirección<br>`GetFirstTextureCallback` sin dirección<br>`IndexDataBufferGetTotalDataSize` sin dirección<br>`IndexDataBufferPop` sin dirección<br>`IndexDataBufferResize` sin dirección<br>`RpClumpGtaCancelStream` sin dirección<br>`RpClumpGtaStreamRead1` sin dirección<br>`RpClumpGtaStreamRead2` sin dirección<br>`RpMaterialUVAnimApplyUpdate` sin dirección<br>`RwEngineGetVideoModeInfo` sin dirección<br>`RwTexDictionaryGtaStreamRead1` sin dirección<br>`RwTexDictionaryGtaStreamRead2` sin dirección<br>`RwTextureRead` sin dirección<br>`RxLockedPipeAddFragment` sin dirección<br>`StencilTransformPoints` sin dirección<br>`TextureBufferGetTotalDataSize` sin dirección<br>`TextureBufferPop` sin dirección<br>`TextureBufferPush` sin dirección<br>`TextureBufferPushWithoutIncreasingCounter` sin dirección<br>`TextureBufferResize` sin dirección<br>`TextureBufferSetup` sin dirección<br>`_rwD3D9SetStreams` sin dirección<br>`_rwObjectHasFrameSetFrame` sin dirección<br>`operator delete` sin dirección<br>`operator delete[]` sin dirección<br>`operator new` sin dirección<br>`operator new[]` sin dirección |
| CPlayerPed | 11 | 14,074 | 2,711 | muy grande | `SetRealMoveAnim` 0x0060A9C0 · 2,711 B<br>`FindWeaponLockOnTarget` 0x0060DC50 · 2,263 B<br>`DrawTriangleForMouseRecruitPed` 0x0060BA80 · 1,593 B<br>`FindNextWeaponLockOnTarget` 0x0060E530 · 1,362 B<br>`Compute3rdPersonMouseTarget` 0x0060B650 · 1,062 B<br>`MakeThisPedJoinOurGroup` 0x0060C840 · 1,029 B<br>`ProcessWeaponSwitch` 0x0060D850 · 1,012 B<br>`ProcessAnimGroups` 0x006098F0 · 909 B<br>`KeepAreaAroundPlayerClear` 0x0060C1E0 · 817 B<br>`SetInitialState` 0x0060CD20 · 727 B<br>`ProcessGroupBehaviour` 0x0060D350 · 589 B |
| CHeli | 9 | 12,924 | 3,121 | muy grande | `SearchLightCone` 0x006C58E0 · 3,121 B<br>`ProcessControl` 0x006C7050 · 2,352 B<br>`ProcessControlInputs` 0x006C4830 · 1,571 B<br>`ProcessFlyingCarStuff` 0x006C4E60 · 1,467 B<br>`PreRender` 0x006C5420 · 1,204 B<br>`SendDownSwat` 0x006C69C0 · 869 B<br>`GenerateHeli` 0x006C6520 · 867 B<br>`BlowUpCar` 0x006C6D30 · 795 B<br>`UpdateHelis` 0x006C79A0 · 678 B |
| CVehicle | 12 | 12,859 | 2,878 | muy grande | `AddSingleWheelParticles` 0x006DE880 · 2,878 B<br>`ProcessBikeWheel` 0x006D73B0 · 2,062 B<br>`InflictDamage` 0x006D7C90 · 1,581 B<br>`UpdateTrailerLink` 0x006DFC50 · 1,017 B<br>`GetPlaneWeaponFiringStatus` 0x006E3440 · 944 B<br>`UpdateTractorLink` 0x006E0050 · 941 B<br>`FireFixedMachineGuns` 0x0073DF00 · 822 B<br>`DoDriveByShootings` 0x00741FD0 · 814 B<br>`FireHeatSeakingMissile` 0x006E05C0 · 529 B<br>`ScanAndMarkTargetForHeatSeekingMissile` 0x006E0400 · 445 B<br>`AddReplacementUpgrade` 0x006D3830 · 423 B<br>`ProcessWeapons` 0x006E3950 · 403 B |
| Fx_c | 13 | 8,729 | 1,003 | grande | `AddWheelSpray` 0x0049FB30 · 1,003 B<br>`AddWheelSand` 0x004A0610 · 929 B<br>`AddWheelDust` 0x004A09C0 · 929 B<br>`AddBlood` 0x0049EB00 · 773 B<br>`AddSparks` 0x0049F040 · 704 B<br>`AddBulletImpact` 0x0049F3D0 · 670 B<br>`AddWheelGrass` 0x0049FF20 · 587 B<br>`AddWheelGravel` 0x004A0170 · 587 B<br>`AddWheelMud` 0x004A03C0 · 587 B<br>`AddWood` 0x0049EE10 · 549 B<br>`AddDebris` 0x0049F750 · 531 B<br>`TriggerGunshot` 0x004A0DE0 · 445 B<br>`AddGlass` 0x0049F970 · 435 B |
| CTaskComplexLeaveCar | 4 | 4,206 | 1,428 | grande | `CreateNextSubTask` 0x006419F0 · 1,428 B<br>`CreateSubTask` 0x00641530 · 1,214 B<br>`MakeAbortable` 0x00641100 · 1,069 B<br>`CreateFirstSubTask` 0x00641FC0 · 495 B |
| CTaskComplexGoPickUpEntity | 3 | 0 | — | sin dato | `ControlSubTask` sin dirección<br>`CreateFirstSubTask` sin dirección<br>`CreateNextSubTask` sin dirección |

<a id="completas"></a>
## 🟢 Completas · 803 clases

Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (7) ✅ 7 | [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) (7) ✅ 7 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (17) ✅ 17 | [CMaths](../source/game_sa/Maths.cpp) (3) ✅ 3 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (9) ✅ 9 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CMatrix](../source/game_sa/Core/Matrix.cpp) (1) ✅ 1 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (7) ✅ 7 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) (12) ✅ 12 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (6) ✅ 6 | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (10) ✅ 10 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (7) ✅ 7 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CMemoryHeap](../source/game_sa/Memory/MemoryHeap.cpp) (1) ✅ 1 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (8) ✅ 8 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CMemoryMgr](../source/game_sa/MemoryMgr.cpp) (5) ✅ 5 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (6) ✅ 6 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CMentalState](../source/game_sa/MentalHealth.cpp) (1) ✅ 1 | [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) (9) ✅ 9 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CMenuManager](../source/game_sa/MenuManager.cpp) (61) ✅ 61 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (11) ✅ 11 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (5) ✅ 5 | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (7) ✅ 7 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (5) ✅ 5 | [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) ✅ 9 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) ✅ 7 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CMissionCleanup](../source/game_sa/MissionCleanup.cpp) (2) ✅ 2 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (7) ✅ 7 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (7) ✅ 7 | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) (8) ✅ 8 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (12) ✅ 12 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (29) ✅ 29 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (4) ✅ 4 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (2) ✅ 2 | [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) (11) ✅ 11 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (4) ✅ 4 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) ✅ 8 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (55) ✅ 55 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (2) ✅ 2 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (2) ✅ 2 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (6) ✅ 6 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (2) ✅ 2 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (5) ✅ 5 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (11) ✅ 11 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (8) ✅ 8 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (49) ✅ 49 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (7) ✅ 7 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (20) ✅ 20 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (8) ✅ 8 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) ✅ 2 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (6) ✅ 6 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (3) ✅ 3 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (7) ✅ 7 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (5) ✅ 5 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) (6) ✅ 6 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) ✅ 7 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (6) ✅ 6 | [Common](../source/game_sa/common.cpp) (11) ✅ 11 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (8) ✅ 8 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (12) ✅ 12 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (7) ✅ 7 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (13) ✅ 13 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (6) ✅ 6 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (7) ✅ 7 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (7) ✅ 7 | [CPad](../source/game_sa/Pad.cpp) (80) ✅ 80 | [CTaskComplexWaitForPed](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) (1) ✅ 1 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) (20) ✅ 20 | [CPathFind](../source/game_sa/PathFind.cpp) (66) ✅ 66 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (9) ✅ 9 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (11) ✅ 11 | [CPathNode](../source/game_sa/PathFind.cpp) (1) ✅ 1 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (12) ✅ 12 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (58) ✅ 58 | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (149) ✅ 149 | [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) (13) ✅ 13 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) (20) ✅ 20 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (8) ✅ 8 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) (17) ✅ 17 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) ✅ 14 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (5) ✅ 5 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) (43) ✅ 43 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (14) ✅ 14 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) (19) ✅ 19 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (9) ✅ 9 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (10) ✅ 10 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (7) ✅ 7 | [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) (6) ✅ 6 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (18) ✅ 18 | [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) (6) ✅ 6 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (6) ✅ 6 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (14) ✅ 14 | [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) (38) ✅ 38 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (4) ✅ 4 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (8) ✅ 8 | [CPedGroup](../source/game_sa/PedGroup.cpp) (12) ✅ 12 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (4) ✅ 4 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) (24) ✅ 24 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (2) ✅ 2 | [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) (5) ✅ 5 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (3) ✅ 3 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (2) ✅ 2 | [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) (9) ✅ 9 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (14) ✅ 14 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (2) ✅ 2 | [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) (5) ✅ 5 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (2) ✅ 2 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (2) ✅ 2 | [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) (8) ✅ 8 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (2) ✅ 2 | [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) (7) ✅ 7 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (7) ✅ 7 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (2) ✅ 2 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (8) ✅ 8 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) ✅ 15 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (27) ✅ 27 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (8) ✅ 8 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) (106) ✅ 106 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (18) ✅ 18 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (6) ✅ 6 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (13) ✅ 13 | [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) (4) ✅ 4 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (7) ✅ 7 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (7) ✅ 7 | [CPedGroups](../source/game_sa/PedGroups.cpp) (11) ✅ 11 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) ✅ 7 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (9) ✅ 9 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (7) ✅ 7 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (53) ✅ 53 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (9) ✅ 9 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (5) ✅ 5 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskInteriorSitInChair](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp) (1) ✅ 1 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (11) ✅ 11 | [CPedList](../source/game_sa/PedList.cpp) (7) ✅ 7 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (8) ✅ 8 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (3) ✅ 3 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) ✅ 9 | [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) (5) ✅ 5 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (3) ✅ 3 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (18) ✅ 18 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (20) ✅ 20 | [CPedPool](../source/game_sa/Pools/PedPool.h) (3) ✅ 3 | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) ✅ 6 | [CPedSaveStructure](../source/game_sa/Entity/Ped/PedSaveStructure.cpp) (2) ✅ 2 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (2) ✅ 2 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (5) ✅ 5 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (5) ✅ 5 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (14) ✅ 14 | [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) (4) ✅ 4 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (1) ✅ 1 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (6) ✅ 6 | [CPedStats](../source/game_sa/PedStats.cpp) (4) ✅ 4 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (3) ✅ 3 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (31) ✅ 31 | [CPedStuckChecker](../source/game_sa/PedStuckChecker.cpp) (1) ✅ 1 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) ✅ 8 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (9) ✅ 9 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (8) ✅ 8 |
| [CAttractorScanner](../source/game_sa/AttractorScanner.cpp) (1) ✅ 1 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (10) ✅ 10 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (82) ✅ 82 | [CPedType](../source/game_sa/PedType.cpp) (13) ✅ 13 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (10) ✅ 10 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (5) ✅ 5 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (57) ✅ 57 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (10) ✅ 10 |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (103) ✅ 103 | [CPickup](../source/game_sa/Pickup.cpp) (12) ✅ 12 | [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) (14) ✅ 14 |
| [CAutoPilot](../source/game_sa/AutoPilot.cpp) (2) ✅ 2 | [CPickups](../source/game_sa/Pickups.cpp) (38) ✅ 38 | [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) (5) ✅ 5 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (34) ✅ 34 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (16) ✅ 16 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (9) ✅ 9 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) (40) ✅ 40 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (9) ✅ 9 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) (23) ✅ 23 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (10) ✅ 10 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) (10) ✅ 10 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (6) ✅ 6 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (23) ✅ 23 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (10) ✅ 10 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (10) ✅ 10 |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) ✅ 2 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (9) ✅ 9 |
| [CBridge](../source/game_sa/Bridge.cpp) (4) ✅ 4 | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (20) ✅ 20 | [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) (5) ✅ 5 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) (5) ✅ 5 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (2) ✅ 2 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (28) ✅ 28 | [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) (5) ✅ 5 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) ✅ 5 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (6) ✅ 6 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPlayerRelationshipRecorder](../source/game_sa/PlayerRelationshipRecorder.cpp) (1) ✅ 1 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (6) ✅ 6 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (9) ✅ 9 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (9) ✅ 9 | [CPointLights](../source/game_sa/PointLights.cpp) (7) ✅ 7 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (9) ✅ 9 |
| [CCam](../source/game_sa/Cam.cpp) (51) ✅ 51 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (10) ✅ 10 |
| [CCamera](../source/game_sa/Camera.cpp) (119) ✅ 119 | [CPools](../source/game_sa/Pools.cpp) (19) ✅ 19 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (7) ✅ 7 |
| [CCarAI](../source/game_sa/CarAI.cpp) (25) ✅ 25 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (6) ✅ 6 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) (32) ✅ 32 | [CPopulation](../source/game_sa/Population.cpp) (67) ✅ 67 | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (3) ✅ 3 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) (10) ✅ 10 | [CPostEffects](../source/game_sa/PostEffects.cpp) (38) ✅ 38 | [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) (7) ✅ 7 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (8) ✅ 8 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (6) ✅ 6 |
| [CCheat](../source/game_sa/Cheat.cpp) (72) ✅ 72 | [CProjectile](../source/game_sa/Entity/Object/Projectile.cpp) (1) ✅ 1 | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (15) ✅ 15 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) (12) ✅ 12 | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (4) ✅ 4 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (1) ✅ 1 | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (6) ✅ 6 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (3) ✅ 3 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (1) ✅ 1 | [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) (6) ✅ 6 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) (12) ✅ 12 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (7) ✅ 7 |
| [CClothes](../source/game_sa/Clothes.cpp) (11) ✅ 11 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (12) ✅ 12 | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (5) ✅ 5 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) (23) ✅ 23 | [CQuaternion](../source/game_sa/Core/Quaternion.cpp) (10) ✅ 10 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (6) ✅ 6 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CRadar](../source/game_sa/Radar.cpp) (65) ✅ 65 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (1) ✅ 1 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (24) ✅ 24 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (9) ✅ 9 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (1) ✅ 1 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) (7) ✅ 7 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (15) ✅ 15 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (1) ✅ 1 | [CRect](../source/game_sa/Core/Rect.cpp) (8) ✅ 8 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (6) ✅ 6 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (61) ✅ 61 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (8) ✅ 8 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (8) ✅ 8 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (8) ✅ 8 |
| [CCollisionEventScanner](../source/game_sa/Collision/CollisionEventScanner.cpp) (1) ✅ 1 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (5) ✅ 5 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CRemote](../source/game_sa/Remote.cpp) (2) ✅ 2 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (6) ✅ 6 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (9) ✅ 9 | [CRenderer](../source/game_sa/Renderer.cpp) (33) ✅ 33 | [CTaskSimpleFight](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) (13) ✅ 13 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CReplay](../source/game_sa/Replay.cpp) (47) ✅ 47 | [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) (7) ✅ 7 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (2) ✅ 2 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) (8) ✅ 8 | [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) (9) ✅ 9 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (23) ✅ 23 | [CRope](../source/game_sa/Rope.cpp) (7) ✅ 7 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (5) ✅ 5 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CRopes](../source/game_sa/Ropes.cpp) (11) ✅ 11 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (6) ✅ 6 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) (39) ✅ 39 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (4) ✅ 4 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (5) ✅ 5 | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) ✅ 6 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (8) ✅ 8 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (6) ✅ 6 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (4) ✅ 4 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (10) ✅ 10 | [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) (10) ✅ 10 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (3) ✅ 3 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (7) ✅ 7 | [CSearchLight](../source/game_sa/SearchLight.cpp) (1) ✅ 1 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (3) ✅ 3 |
| [CCoronas](../source/game_sa/Coronas.cpp) (11) ✅ 11 | [CSetPiece](../source/game_sa/SetPiece.cpp) (3) ✅ 3 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (3) ✅ 3 |
| [CCover](../source/game_sa/Cover.cpp) (16) ✅ 16 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (3) ✅ 3 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (19) ✅ 19 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (3) ✅ 3 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CShadows](../source/game_sa/Shadows.cpp) (30) ✅ 30 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (3) ✅ 3 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (3) ✅ 3 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CShopping](../source/game_sa/Shopping.cpp) (30) ✅ 30 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (3) ✅ 3 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (3) ✅ 3 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (4) ✅ 4 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (3) ✅ 3 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (14) ✅ 14 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (16) ✅ 16 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (6) ✅ 6 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (9) ✅ 9 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) (7) ✅ 7 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (35) ✅ 35 | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (8) ✅ 8 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) ✅ 37 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (8) ✅ 8 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (14) ✅ 14 | [CStats](../source/game_sa/Stats.cpp) (51) ✅ 51 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (6) ✅ 6 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (9) ✅ 9 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) ✅ 3 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (4) ✅ 4 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (35) ✅ 35 | [CStencilShadows](../source/game_sa/StencilShadows.cpp) (13) ✅ 13 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (14) ✅ 14 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (6) ✅ 6 | [CStoredCar](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (8) ✅ 8 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (4) ✅ 4 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (7) ✅ 7 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CStreaming](../source/game_sa/Streaming.cpp) (94) ✅ 94 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (6) ✅ 6 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CData](../source/game_sa/Text/Text.cpp) (2) ✅ 2 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (8) ✅ 8 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (3) ✅ 3 |
| [CDebug](../source/game_sa/Debug.cpp) (1) ✅ 1 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (7) ✅ 7 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (4) ✅ 4 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) (5) ✅ 5 | [CTagManager](../source/game_sa/TagManager.cpp) (19) ✅ 19 | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (4) ✅ 4 |
| [CDecisionMaker](../source/game_sa/DecisionMakers/DecisionMaker.cpp) (1) ✅ 1 | [CTask](../source/game_sa/Tasks/Task.cpp) (5) ✅ 5 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (9) ✅ 9 |
| [CDecisionMakerTypes](../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) (9) ✅ 9 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (3) ✅ 3 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (4) ✅ 4 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (5) ✅ 5 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (6) ✅ 6 |
| [CDirectory](../source/game_sa/Directory.cpp) (10) ✅ 10 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (4) ✅ 4 | [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) (12) ✅ 12 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (6) ✅ 6 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (2) ✅ 2 |
| [CDraw](../source/game_sa/Draw.cpp) (2) ✅ 2 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (2) ✅ 2 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (3) ✅ 3 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (9) ✅ 9 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (5) ✅ 5 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (5) ✅ 5 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (1) ✅ 1 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (3) ✅ 3 | [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (7) ✅ 7 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (2) ✅ 2 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (7) ✅ 7 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CTaskComplex](../source/game_sa/Tasks/Task.cpp) (1) ✅ 1 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (6) ✅ 6 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (70) ✅ 70 | [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) (7) ✅ 7 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (2) ✅ 2 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (5) ✅ 5 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) ✅ 15 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) (3) ✅ 3 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) (9) ✅ 9 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (1) ✅ 1 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (10) ✅ 10 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (5) ✅ 5 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (11) ✅ 11 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (6) ✅ 6 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (8) ✅ 8 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (18) ✅ 18 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (11) ✅ 11 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (8) ✅ 8 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (7) ✅ 7 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleSlideToCoord](../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp) (1) ✅ 1 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (3) ✅ 3 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (5) ✅ 5 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (8) ✅ 8 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (11) ✅ 11 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (7) ✅ 7 |
| [CEvent](../source/game_sa/Events/Event.cpp) (3) ✅ 3 | [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) (9) ✅ 9 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (11) ✅ 11 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (4) ✅ 4 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (7) ✅ 7 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) ✅ 6 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) ✅ 1 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (4) ✅ 4 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (9) ✅ 9 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (2) ✅ 2 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (9) ✅ 9 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (3) ✅ 3 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (3) ✅ 3 | [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) (9) ✅ 9 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (5) ✅ 5 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (4) ✅ 4 | [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) (9) ✅ 9 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (6) ✅ 6 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (4) ✅ 4 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (10) ✅ 10 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (1) ✅ 1 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (2) ✅ 2 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (10) ✅ 10 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (4) ✅ 4 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (2) ✅ 2 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (5) ✅ 5 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (19) ✅ 19 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (20) ✅ 20 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (6) ✅ 6 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (6) ✅ 6 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (4) ✅ 4 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (7) ✅ 7 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (1) ✅ 1 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (2) ✅ 2 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (4) ✅ 4 |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (11) ✅ 11 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (6) ✅ 6 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (6) ✅ 6 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (1) ✅ 1 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (7) ✅ 7 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (6) ✅ 6 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (3) ✅ 3 | [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) (8) ✅ 8 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (1) ✅ 1 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (12) ✅ 12 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (5) ✅ 5 | [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) (5) ✅ 5 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (5) ✅ 5 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (8) ✅ 8 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (2) ✅ 2 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (15) ✅ 15 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) (72) ✅ 72 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (3) ✅ 3 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (10) ✅ 10 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (9) ✅ 9 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (69) ✅ 69 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (2) ✅ 2 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (3) ✅ 3 | [CTheZones](../source/game_sa/TheZones.cpp) (22) ✅ 22 |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (2) ✅ 2 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (9) ✅ 9 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (8) ✅ 8 | [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) (8) ✅ 8 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) (8) ✅ 8 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (2) ✅ 2 |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (1) ✅ 1 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (9) ✅ 9 | [CTimer](../source/game_sa/Timer.cpp) (14) ✅ 14 |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) (8) ✅ 8 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (8) ✅ 8 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (13) ✅ 13 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (2) ✅ 2 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (9) ✅ 9 | [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) (40) ✅ 40 |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (1) ✅ 1 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (9) ✅ 9 | [CTrainNode](../source/game_sa/TrainNode.cpp) (1) ✅ 1 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (2) ✅ 2 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (6) ✅ 6 | [cTransmission](../source/game_sa/cTransmission.cpp) (4) ✅ 4 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) ✅ 1 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (8) ✅ 8 | [CTxdStore](../source/game_sa/TxdStore.cpp) (26) ✅ 26 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (2) ✅ 2 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (8) ✅ 8 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (3) ✅ 3 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (11) ✅ 11 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (1) ✅ 1 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (8) ✅ 8 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (1) ✅ 1 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (8) ✅ 8 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (2) ✅ 2 | [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) (7) ✅ 7 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (75) ✅ 75 |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (2) ✅ 2 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (19) ✅ 19 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (2) ✅ 2 |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (1) ✅ 1 | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (6) ✅ 6 | [CVehiclePotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (13) ✅ 13 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (2) ✅ 2 | [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) (8) ✅ 8 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (2) ✅ 2 |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (2) ✅ 2 | [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) (7) ✅ 7 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) ✅ 84 |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (3) ✅ 3 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (13) ✅ 13 | [CWanted](../source/game_sa/Wanted.cpp) (33) ✅ 33 |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (2) ✅ 2 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (8) ✅ 8 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (5) ✅ 5 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (9) ✅ 9 | [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) (5) ✅ 5 | [CWaterLevel](../source/game_sa/WaterLevel.cpp) (39) ✅ 39 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) ✅ 1 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (6) ✅ 6 | [CWeapon](../source/game_sa/Weapon.cpp) (41) ✅ 41 |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) ✅ 1 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (11) ✅ 11 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (9) ✅ 9 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (2) ✅ 2 | [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) (7) ✅ 7 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (10) ✅ 10 |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (3) ✅ 3 | [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) (8) ✅ 8 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (3) ✅ 3 |
| [CEventSource](../source/game_sa/Events/EventSource.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (7) ✅ 7 | [CWeather](../source/game_sa/Weather.cpp) (13) ✅ 13 |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (4) ✅ 4 | [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) (7) ✅ 7 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (3) ✅ 3 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) ✅ 6 | [CWorld](../source/game_sa/World.cpp) (95) ✅ 95 |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) (7) ✅ 7 | [CWorldScan](../source/game_sa/Renderer.cpp) (2) ✅ 2 |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (2) ✅ 2 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (3) ✅ 3 | [CZoneDef](../source/game_sa/CullZones.cpp) (1) ✅ 1 |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (2) ✅ 2 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (8) ✅ 8 | [D3DResourceSystem](../source/game_sa/RenderWare/D3DResourceSystem.cpp) (12) ✅ 12 |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (3) ✅ 3 | [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) (10) ✅ 10 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (6) ✅ 6 |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (3) ✅ 3 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (7) ✅ 7 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (7) ✅ 7 |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (3) ✅ 3 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (4) ✅ 4 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (3) ✅ 3 |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (2) ✅ 2 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (9) ✅ 9 | [FxEmitter_c](../source/game_sa/Fx/FxEmitter.cpp) (4) ✅ 4 |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) ✅ 8 | [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) (9) ✅ 9 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) ✅ 7 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (1) ✅ 1 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (48) ✅ 48 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (6) ✅ 6 | [FxFrustumInfo_c](../source/game_sa/Fx/FxFrustumInfo.cpp) (1) ✅ 1 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) (11) ✅ 11 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (5) ✅ 5 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) (7) ✅ 7 | [FxInterpInfo255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp) (1) ✅ 1 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) ✅ 9 | [FxInterpInfo32_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp) (1) ✅ 1 |
| [CFont](../source/game_sa/Font.cpp) (37) ✅ 37 | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (6) ✅ 6 | [FxInterpInfoFloat_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp) (1) ✅ 1 |
| [CFormation](../source/game_sa/Formation.cpp) (10) ✅ 10 | [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) (11) ✅ 11 | [FxInterpInfoU255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp) (1) ✅ 1 |
| [CGame](../source/game_sa/Game.cpp) (22) ✅ 22 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) ✅ 10 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) ✅ 7 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (5) ✅ 5 |
| [CGamma](../source/game_sa/Gamma.cpp) (2) ✅ 2 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (7) ✅ 7 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (2) ✅ 2 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (8) ✅ 8 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (4) ✅ 4 |
| [CGangWars](../source/game_sa/GangWars.cpp) (36) ✅ 36 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (3) ✅ 3 | [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) (35) ✅ 35 |
| [CGarage](../source/game_sa/Garage.cpp) (19) ✅ 19 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (8) ✅ 8 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CGarages](../source/game_sa/Garages.cpp) (26) ✅ 26 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (8) ✅ 8 | [Game](../source/app/app_game.cpp) (12) ✅ 12 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (10) ✅ 10 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) (19) ✅ 19 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (9) ✅ 9 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) ✅ 17 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (8) ✅ 8 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (6) ✅ 6 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (8) ✅ 8 | [Interior_c](../source/game_sa/Interior/Interior_c.cpp) (52) ✅ 52 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) (5) ✅ 5 | [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) (24) ✅ 24 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) (32) ✅ 32 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (9) ✅ 9 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (21) ✅ 21 |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (11) ✅ 11 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (4) ✅ 4 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (5) ✅ 5 |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (3) ✅ 3 | [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) (4) ✅ 4 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (3) ✅ 3 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (6) ✅ 6 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (2) ✅ 2 |
| [CHud](../source/game_sa/Hud.cpp) (39) ✅ 39 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (3) ✅ 3 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (4) ✅ 4 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [CIdleCam](../source/game_sa/IdleCam.cpp) (14) ✅ 14 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (7) ✅ 7 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (8) ✅ 8 |
| [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (2) ✅ 2 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (4) ✅ 4 |
| [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (9) ✅ 9 | [Ps](../source/app/platform/win/WinPs.cpp) (23) ✅ 23 |
| [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) (9) ✅ 9 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (13) ✅ 13 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (44) ✅ 44 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) (7) ✅ 7 | [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) (11) ✅ 11 | [RpAnisot](../source/game_sa/RenderWare/rw/rpanisot.cpp) (3) ✅ 3 |
| [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (1) ✅ 1 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (8) ✅ 8 | [Rs](../source/app/platform/platform.cpp) (29) ✅ 29 |
| [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (9) ✅ 9 | [RwHelper](../source/game_sa/RwHelper.cpp) (33) ✅ 33 |
| [CKeyArray](../source/game_sa/Text/Text.cpp) (4) ✅ 4 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (9) ✅ 9 | [Securom](../source/app/securom/Securom.cpp) (2) ✅ 2 |
| [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (10) ✅ 10 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (47) ✅ 47 |
| [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (10) ✅ 10 | [UserTracksLegacyVector](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (1) ✅ 1 |
| [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (7) ✅ 7 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) ✅ 4 |
| [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (5) ✅ 5 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (6) ✅ 6 |
| [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) ✅ 9 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (3) ✅ 3 |
| [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (9) ✅ 9 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) ✅ 7 |
| [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (13) ✅ 13 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (6) ✅ 6 |
| [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (3) ✅ 3 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (10) ✅ 10 |  |

</details>

<a id="parciales"></a>
## 🟡 Parciales · 6 clases

Orden de mayor a menor tamaño de las envolturas al EXE pendientes (suma de bytes de sus cuerpos; a igualdad, alfabético). «Función más grande» y «Tamaño» (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B) indican cuánto pesa la función más difícil de la clase. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas al EXE»): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

| Clase | Revertidas / total | Pendientes | Envolturas al EXE | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) | 93/93 | **0** | **18** | 41,577 | 7,915 | muy grande | 100.00% | 93 | 0 |
| [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) | 54/54 | **0** | **11** | 14,074 | 2,711 | muy grande | 100.00% | 54 | 0 |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) | 13/13 | **0** | **9** | 12,924 | 3,121 | muy grande | 100.00% | 13 | 0 |
| [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) | 156/156 | **0** | **12** | 12,859 | 2,878 | muy grande | 100.00% | 156 | 0 |
| [Fx_c](../source/game_sa/Fx/Fx.cpp) | 13/13 | **0** | **13** | 8,729 | 1,003 | grande | 100.00% | 13 | 0 |
| [CTaskComplexGoPickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp) | 2/2 | **0** | **3** | 0 | — | sin dato | 100.00% | 2 | 0 |

<a id="sin-revertir"></a>
## 🔴 Sin revertir · 2 clases

Orden de mayor a menor tamaño de las envolturas pendientes. Clases o grupos sin ninguna entrada reconstruida en el catálogo. Las que figuran con envolturas al EXE no están en [hooks.csv](hooks.csv): sus funciones del juego se conocen por el código fuente o por las cohortes de dependencias y siguen delegando en el original.

| Clase | Revertidas / total | Pendientes | Envolturas al EXE | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |
| (helpers y globales sin clase) | 0/0 | — | **60** | 15,804 | 3,445 | muy grande | 0.00% | 0 | 0 |
| CTaskComplexLeaveCar | 0/0 | — | **4** | 4,206 | 1,428 | grande | 0.00% | 0 | 0 |

<a id="cohorte-nuevo"></a>
## 📋 Cohorte nuevo (17/10/2026): lista por dificultad

Destinos originales explícitos que `source/` sigue referenciando y que no estaban reconstruidos ni validados el 17/10/2026, la lista completa va ordenada de mayor a menor tamaño en bytes (la columna «Rango» conserva la posición por dificultad, de menor a mayor: 1 es la más fácil) (puntuación de tamaño y riesgos —llamadas sin catalogar, indirectas, x87, tablas de saltos y SEH—). Solo se listan funciones del juego. Lista completa y descargable: [lista-por-dificultad.csv](../reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv) · metodología: [README del cohorte](../reversing/OriginalDependencies2026-10-17/README.md). La columna «Estado» se calcula con el catálogo actual: una dirección con fila reversed=1 en hooks.csv figura como reconstruida o validada.

| Nivel | Funciones del juego | de ellas pendientes |
| --- | ---: | ---: |
| 1 Muy fácil | 9 | 0 |
| 2 Fácil | 25 | 0 |
| 3 Media | 75 | 4 |
| 4 Difícil | 57 | 22 |
| 5 Muy difícil | 19 | 10 |

<details>
<summary>Funciones del juego (185), de mayor a menor tamaño en bytes</summary>

| Rango | Nivel | Subcategoría | Función | Dirección | Bytes | Estado |
| ---: | --- | --- | --- | --- | ---: | --- |
| 178 | 5 Muy difícil | wrapper | CPed::ProcessEntityCollision | `0x005E2530` | 4342 | ✅ validada |
| 185 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysics_OnlyMission | `0x00436A90` | 4243 | pendiente |
| 182 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPath | `0x00434900` | 3729 | pendiente |
| 170 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeRandomly | `0x0042DE80` | 3591 | pendiente |
| 184 | 5 Muy difícil | embedded_call | helper sin símbolo confirmado (usado por CameraObbeCinemaCycle) | `0x0051D770` | 3445 | pendiente |
| 181 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing | `0x00435830` | 3320 | pendiente |
| 162 | 5 Muy difícil | wrapper | CCarCtrl::FlyAIHeliInCertainDirection | `0x00429A70` | 2994 | ✅ validada |
| 169 | 5 Muy difícil | wrapper | CCarCtrl::DealWithBend_Racing | `0x00428040` | 2361 | ✅ validada |
| 175 | 5 Muy difícil | wrapper | CHeli::ProcessControl | `0x006C7050` | 2352 | pendiente |
| 163 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeToFollowPath | `0x00427740` | 2193 | pendiente |
| 155 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeToChaseCar | `0x00426EF0` | 2111 | pendiente |
| 179 | 5 Muy difícil | wrapper | CMissionCleanup::Process | `0x00468560` | 1955 | ✅ validada |
| 140 | 5 Muy difícil | wrapper | CCarCtrl::FlyAIPlaneInCertainDirection | `0x00423000` | 1917 | ✅ validada |
| 176 | 5 Muy difícil | wrapper | CTaskSimpleFight::ProcessPed | `0x00629920` | 1907 | ✅ validada |
| 166 | 5 Muy difícil | wrapper | CCarCtrl::SlowCarDownForPedsSectorList | `0x00425440` | 1761 | ✅ validada |
| 147 | 5 Muy difícil | wrapper | CCarCtrl::DragCarToPoint | `0x0042EC90` | 1748 | pendiente |
| 173 | 5 Muy difícil | wrapper | CHeli::ProcessControlInputs | `0x006C4830` | 1571 | pendiente |
| 167 | 5 Muy difícil | wrapper | CCarCtrl::WeaveForOtherCar | `0x00426350` | 1558 | ✅ validada |
| 171 | 5 Muy difícil | wrapper | CTaskComplexPartner::CreateNextSubTask | `0x00683AD0` | 1521 | ✅ validada |
| 161 | 4 Difícil | wrapper | CHeli::ProcessFlyingCarStuff | `0x006C4E60` | 1467 | pendiente |
| 177 | 4 Difícil | wrapper | __MainWndProc | `0x00747EB0` | 1458 | ✅ validada |
| 148 | 4 Difícil | embedded_call | FxEmitter_c::CreateParticle | `0x004A2580` | 1434 | ✅ validada |
| 183 | 4 Difícil | wrapper | CTaskComplexLeaveCar::CreateNextSubTask | `0x006419F0` | 1428 | pendiente |
| 174 | 4 Difícil | wrapper | CTaskComplexFollowPedFootsteps::ControlSubTask | `0x00695090` | 1341 | ✅ validada |
| 154 | 4 Difícil | wrapper | CCarCtrl::FindAngleToWeaveThroughTraffic | `0x004325C0` | 1339 | pendiente |
| 124 | 4 Difícil | wrapper | CCarCtrl::FindNodesThisCarIsNearestTo | `0x0042BD20` | 1323 | pendiente |
| 164 | 4 Difícil | wrapper | FightHitPedExe | `0x0061CBA0` | 1289 | pendiente |
| 132 | 4 Difícil | wrapper | CCarCtrl::SteerAICarTowardsPointInEscort | `0x004336D0` | 1223 | ✅ validada |
| 143 | 4 Difícil | wrapper | CTaskComplexLeaveCar::CreateSubTask | `0x00641530` | 1214 | pendiente |
| 135 | 4 Difícil | wrapper | CStoredCar::RestoreCar | `0x00447E40` | 1211 | ✅ validada |
| 180 | 4 Difícil | wrapper | CHeli::PreRender | `0x006C5420` | 1204 | pendiente |
| 122 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath | `0x00432DD0` | 1177 | ✅ validada |
| 152 | 4 Difícil | wrapper | CCarCtrl::CreateConvoy | `0x0042C740` | 1168 | pendiente |
| 120 | 4 Difícil | wrapper | CCarCtrl::FlyAIHeliToTarget_FixedOrientation | `0x00423940` | 1167 | ✅ validada |
| 165 | 4 Difícil | wrapper | CCarCtrl::CreatePoliceChase | `0x0042C2B0` | 1161 | pendiente |
| 100 | 4 Difícil | wrapper | CCarCtrl::TestCollisionBetween2MovingRects | `0x00425B30` | 1081 | ✅ validada |
| 172 | 4 Difícil | wrapper | CTaskComplexLeaveCar::MakeAbortable | `0x00641100` | 1069 | pendiente |
| 144 | 4 Difícil | wrapper | CCarCtrl::SlowCarDownForOtherCar | `0x0042D0E0` | 1032 | ✅ validada |
| 158 | 4 Difícil | wrapper | CPlayerPed::MakeThisPedJoinOurGroup | `0x0060C840` | 1029 | pendiente |
| 151 | 4 Difícil | wrapper | CAutomobile::Render | `0x006A2B10` | 1005 | pendiente |
| 157 | 4 Difícil | wrapper | CVehicle::GetPlaneWeaponFiringStatus | `0x006E3440` | 944 | pendiente |
| 118 | 4 Difícil | wrapper | CCarCtrl::SteerAIHeliToKeepEntityInView | `0x0042AEB0` | 939 | ✅ validada |
| 156 | 4 Difícil | wrapper | CCarCtrl::IsThisAnAppropriateNode | `0x0042DAB0` | 932 | ✅ validada |
| 103 | 4 Difícil | wrapper | CCarCtrl::ClitargetOrientationToLink | `0x00422760` | 918 | pendiente |
| 137 | 4 Difícil | wrapper | CCarCtrl::SteerAIHeliToFollowEntity | `0x0042A750` | 896 | ✅ validada |
| 160 | 4 Difícil | wrapper | CHeli::SendDownSwat | `0x006C69C0` | 869 | pendiente |
| 138 | 4 Difícil | wrapper | CHeli::GenerateHeli | `0x006C6520` | 867 | pendiente |
| 93 | 4 Difícil | wrapper | CCarCtrl::FindLinksToGoWithTheseNodes | `0x0042B470` | 862 | ✅ validada |
| 123 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsHeadingForTarget | `0x00433280` | 841 | ✅ validada |
| 139 | 4 Difícil | wrapper | CTaskComplexPartnerChat::GetPartnerSequence | `0x00684380` | 836 | ✅ validada |
| 168 | 4 Difícil | wrapper | CTaskComplexKillPedOnFoot::CreateNextSubTask | `0x0062B150` | 831 | ✅ validada |
| 141 | 4 Difícil | wrapper | CCarCtrl::WeaveForObject | `0x00426BC0` | 803 | ✅ validada |
| 128 | 4 Difícil | wrapper | CCarCtrl::SteerAICarParkPerpendicular | `0x00433EA0` | 800 | ✅ validada |
| 113 | 4 Difícil | wrapper | CHeli::BlowUpCar | `0x006C6D30` | 795 | pendiente |
| 150 | 4 Difícil | wrapper | CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents | `0x00603720` | 777 | ✅ validada |
| 95 | 4 Difícil | embedded_call | helper sin símbolo confirmado (usado por CPlayerInfo::Process) | `0x0056EC80` | 776 | pendiente |
| 134 | 4 Difícil | wrapper | CCarCtrl::SteerAICarParkParallel | `0x00433BA0` | 762 | ✅ validada |
| 131 | 4 Difícil | wrapper | CCarCtrl::SteerAICarBlockingPlayerForwardAndBack | `0x00422B20` | 745 | ✅ validada |
| 159 | 4 Difícil | wrapper | CStats::BuildStatLine | `0x00559230` | 735 | ✅ validada |
| 111 | 4 Difícil | wrapper | CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer | `0x00428DE0` | 684 | ✅ validada |
| 78 | 4 Difícil | wrapper | CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper | `0x00425F70` | 672 | ✅ validada |
| 96 | 4 Difícil | wrapper | CCarCtrl::ChooseModel | `0x00424CE0` | 672 | ✅ validada |
| 127 | 4 Difícil | wrapper | CMentalState::Process | `0x006008A0` | 646 | ✅ validada |
| 153 | 4 Difícil | wrapper | CTaskComplexWalkRoundObject::CreateFirstSubTask | `0x00657380` | 620 | ✅ validada |
| 101 | 4 Difícil | wrapper | CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer | `0x00429090` | 613 | ✅ validada |
| 125 | 4 Difícil | embedded_call | helper sin símbolo confirmado (usado por CWeaponEffects::Render) | `0x0056EF90` | 608 | pendiente |
| 69 | 4 Difícil | embedded_call | CPed::SpecialEntityPreCollisionStuff | `0x005E3C30` | 603 | ✅ validada |
| 94 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop | `0x00428990` | 583 | ✅ validada |
| 116 | 4 Difícil | embedded_call | CTaskSimpleSlideToCoord::ProcessPed | `0x0066C4E0` | 570 | ✅ validada |
| 146 | 4 Difícil | wrapper | RsLoadPresetViews | `0x00619D60` | 552 | pendiente |
| 109 | 4 Difícil | wrapper | Fx_c::AddWood | `0x0049EE10` | 549 | pendiente |
| 133 | 4 Difícil | wrapper | CRemote::GivePlayerRemoteControlledCar | `0x0045AB10` | 548 | ✅ validada |
| 97 | 4 Difícil | wrapper | CVehicle::FireHeatSeakingMissile | `0x006E05C0` | 529 | pendiente |
| 105 | 4 Difícil | wrapper | CCarCtrl::TriggerDogFightMoves | `0x00429300` | 526 | ✅ validada |
| 73 | 4 Difícil | wrapper | CCarCtrl::FindPathDirection | `0x00422090` | 515 | ✅ validada |
| 58 | 4 Difícil | wrapper | CCarCtrl::FireHeliRocketsAtTarget | `0x0042B270` | 506 | ✅ validada |
| 57 | 3 Media | wrapper | CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget | `0x00428BE0` | 497 | ✅ validada |
| 119 | 3 Media | wrapper | FightHitObjExe | `0x0061D400` | 494 | pendiente |
| 110 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateSubTask | `0x00695E40` | 488 | ✅ validada |
| 56 | 3 Media | embedded_call | CPathFind::FindParkingNodeInArea | `0x004513F0` | 476 | ✅ validada |
| 91 | 3 Media | wrapper | CCarCtrl::SteerAIHeliAsPoliceHeli | `0x0042AAD0` | 475 | ✅ validada |
| 117 | 3 Media | wrapper | CCarCtrl::GetAIPlaneToDoDogFight | `0x00429890` | 472 | ✅ validada |
| 126 | 3 Media | wrapper | CCarCtrl::GetAIHeliToAttackPlayer | `0x0042F3C0` | 472 | ✅ validada |
| 54 | 3 Media | wrapper | CVehicle::ScanAndMarkTargetForHeatSeekingMissile | `0x006E0400` | 445 | pendiente |
| 149 | 3 Media | wrapper | D3DResourceSystem::DestroyTexture | `0x00730B70` | 439 | ✅ validada |
| 108 | 3 Media | wrapper | CVehicle::AddReplacementUpgrade | `0x006D3830` | 423 | pendiente |
| 106 | 3 Media | embedded_call | FxEmitter_c::AddParticle | `0x004A3EA0` | 418 | ✅ validada |
| 142 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x004650F0` | 415 | pendiente |
| 112 | 3 Media | wrapper | CCarCtrl::SlowCarDownForCarsSectorList | `0x00432420` | 413 | ✅ validada |
| 104 | 3 Media | embedded_call | FxEmitter_c::AddParticle | `0x004A4050` | 400 | ✅ validada |
| 68 | 3 Media | embedded_call | CHeli::CHeli | `0x006C4190` | 398 | ✅ validada |
| 145 | 3 Media | wrapper | RsSavePresetView | `0x00619FA0` | 393 | ✅ validada |
| 88 | 3 Media | wrapper | CCarCtrl::SlowCarDownForObjectsSectorList | `0x0042D4F0` | 391 | ✅ validada |
| 51 | 3 Media | wrapper | CPlayerPed::EvaluateNeighbouringTarget | `0x0060D1C0` | 388 | ✅ validada |
| 92 | 3 Media | wrapper | CGarage::IsStaticPlayerCarEntirelyInside | `0x0044A830` | 386 | ✅ validada |
| 66 | 3 Media | wrapper | RpClumpGetBoundingSphere | `0x00734FC0` | 384 | ✅ validada |
| 60 | 3 Media | wrapper | SkinGetBonePositionsToTable | `0x00735360` | 381 | ✅ validada |
| 83 | 3 Media | wrapper | CCarCtrl::SteerAIHeliToLand | `0x0042AD30` | 370 | ✅ validada |
| 89 | 3 Media | wrapper | CVehicle::PossiblyDropFreeFallBombForPlayer | `0x006E07E0` | 359 | ✅ validada |
| 102 | 3 Media | wrapper | RwTextureGtaStreamRead | `0x00730E60` | 347 | ✅ validada |
| 81 | 3 Media | wrapper | CCarCtrl::WeaveThroughPedsSectorList | `0x0042D7E0` | 346 | ✅ validada |
| 75 | 3 Media | wrapper | CTaskComplexPartnerDeal::GetPartnerSequence | `0x00682440` | 344 | ✅ validada |
| 82 | 3 Media | wrapper | CCarCtrl::WeaveThroughObjectsSectorList | `0x0042D950` | 343 | ✅ validada |
| 64 | 3 Media | wrapper | CPed::SpecialEntityCalcCollisionSteps | `0x005E3E90` | 342 | ✅ validada |
| 80 | 3 Media | wrapper | CCarCtrl::WeaveThroughCarsSectorList | `0x0042D680` | 341 | ✅ validada |
| 129 | 3 Media | wrapper | CTaskComplexWalkRoundObject::ControlSubTask | `0x006575F0` | 341 | ✅ validada |
| 136 | 3 Media | wrapper | CTaskComplexWalkRoundObject::CreateNextSubTask | `0x00657220` | 341 | ✅ validada |
| 52 | 3 Media | wrapper | CVehicle::AddUpgrade | `0x006DFA20` | 333 | ✅ validada |
| 77 | 3 Media | embedded_call | CDecision::MakeDecision | `0x006040D0` | 333 | ✅ validada |
| 84 | 3 Media | wrapper | CGarage::IsEntityEntirelyInside3D | `0x00448BE0` | 333 | ✅ validada |
| 46 | 3 Media | embedded_call | FxInterpInfoFloat_c::GetVal | `0x004A8470` | 330 | ✅ validada |
| 130 | 3 Media | wrapper | CCarCtrl::PickNextNodeAccordingStrategy | `0x00432B10` | 321 | ✅ validada |
| 99 | 3 Media | wrapper | CTaskSimpleFight::MakeAbortable | `0x006239F0` | 284 | ✅ validada |
| 115 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateNextSubTask | `0x00694EE0` | 283 | ✅ validada |
| 107 | 3 Media | wrapper | CGarage::IsEntityEntirelyOutside | `0x00448D30` | 281 | ✅ validada |
| 61 | 3 Media | wrapper | CTaskComplexPartnerShove::GetPartnerSequence | `0x00683120` | 275 | ✅ validada |
| 114 | 3 Media | wrapper | D3DResourceSystem::DestroyIndexBuffer | `0x00730D30` | 263 | ✅ validada |
| 121 | 3 Media | wrapper | CCarCtrl::GetAIPlaneToAttackPlayer | `0x00429780` | 261 | ✅ validada |
| 98 | 3 Media | wrapper | CCarCtrl::SteerAIHeliTowardsTargetCoors | `0x0042A630` | 244 | ✅ validada |
| 71 | 3 Media | wrapper | CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget | `0x004335E0` | 240 | ✅ validada |
| 41 | 3 Media | wrapper | CPlantMgr::AtomicCreatePrelitIfNeeded | `0x005DD100` | 221 | ✅ validada |
| 33 | 3 Media | wrapper | CQuaternion::Get | `0x0059C080` | 215 | ✅ validada |
| 67 | 3 Media | wrapper | CCarCtrl::TestWhetherToFirePlaneGuns | `0x00429520` | 214 | ✅ validada |
| 74 | 3 Media | wrapper | diPadInit | `0x007485C0` | 214 | ✅ validada |
| 72 | 3 Media | wrapper | Fx_c::AddPunchImpact | `0x0049F670` | 209 | ✅ validada |
| 32 | 3 Media | wrapper | CCarCtrl::FindPercDependingOnDistToLink | `0x00422620` | 208 | ✅ validada |
| 36 | 3 Media | wrapper | CCarCtrl::SwitchBetweenPhysicsAndGhost | `0x004222A0` | 208 | ✅ validada |
| 79 | 3 Media | wrapper | D3DResourceSystem::CreateTexture | `0x00730510` | 204 | ✅ validada |
| 39 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CTaskSimpleGangDriveBy::AITarget) | `0x00621B10` | 201 | ✅ validada |
| 37 | 3 Media | wrapper | CPlayerPed::PlayerWantsToAttack | `0x0060CC50` | 200 | ✅ validada |
| 40 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x005BB9F0` | 197 | ✅ validada |
| 43 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CCam::Process_DW_HeliChaseCam) | `0x0050E090` | 192 | ✅ validada |
| 86 | 3 Media | wrapper | CTaskComplexPartner::ControlSubTask | `0x006840D0` | 192 | ✅ validada |
| 22 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CCam::Process_DW_HeliChaseCam) | `0x0050E180` | 184 | ✅ validada |
| 45 | 3 Media | wrapper | CTaskComplexPartner::CreateFirstSubTask | `0x00681F20` | 177 | ✅ validada |
| 26 | 3 Media | wrapper | CCarCtrl::FindSpeedMultiplier | `0x004224E0` | 167 | ✅ validada |
| 76 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CProjectileInfo::AddProjectile) | `0x005A4030` | 167 | ✅ validada |
| 31 | 3 Media | wrapper | FightStrikeDamageExe | `0x0061C740` | 164 | ✅ validada |
| 29 | 3 Media | embedded_call | CTrainNode::GetLightingFromCollision | `0x006F5F80` | 161 | ✅ validada |
| 85 | 3 Media | wrapper | D3DResourceSystem::CreateIndexBuffer | `0x007306A0` | 153 | ✅ validada |
| 30 | 3 Media | wrapper | CCustomBuildingDNPipeline::CustomPipeMaterialSetup | `0x005D7120` | 149 | ✅ validada |
| 25 | 2 Fácil | embedded_call | CHeli::FindSwatPositionRelativeToHeli | `0x006C4760` | 141 | ✅ validada |
| 28 | 3 Media | embedded_call | CGarage::IsGarageEmpty | `0x0044A9C0` | 141 | ✅ validada |
| 59 | 3 Media | wrapper | CCarCtrl::SteerAIPlaneToFollowEntity | `0x004237F0` | 141 | ✅ validada |
| 65 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateFirstSubTask | `0x00695000` | 141 | ✅ validada |
| 15 | 2 Fácil | wrapper | CCarCtrl::StopCarIfNodesAreInvalid | `0x00422590` | 137 | ✅ validada |
| 38 | 2 Fácil | wrapper | CreateRouteTask | `0x00655140` | 130 | ✅ validada |
| 70 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por ProcessPlayerVehicleInteraction) | `0x006919C0` | 126 | ✅ validada |
| 23 | 3 Media | embedded_call | TransformPoint | `0x0059C890` | 124 | ✅ validada |
| 63 | 3 Media | wrapper | CCarCtrl::SteerAIHeliFlyingAwayFromPlayer | `0x0042ACB0` | 122 | ✅ validada |
| 62 | 2 Fácil | wrapper | CTaskComplexPartnerDeal::StreamRequiredAnims | `0x006823C0` | 113 | ✅ validada |
| 11 | 2 Fácil | wrapper | CStats::FindLeastFavoriteRadioStation | `0x00559010` | 110 | ✅ validada |
| 34 | 2 Fácil | wrapper | RemoveUpgradeCB | `0x006D3300` | 108 | ✅ validada |
| 35 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CTaskSimpleCarDrive::StartRollingDoor) | `0x0063C840` | 108 | ✅ validada |
| 87 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CAEUserRadioTrackManager::WriteUserTracksFile) | `0x004F4620` | 102 | ✅ validada |
| 21 | 3 Media | wrapper | CCarCtrl::FindIntersection2Lines | `0x004226F0` | 100 | ✅ validada |
| 18 | 2 Fácil | wrapper | CTaskComplexPartner::StreamRequiredAnims | `0x00682310` | 93 | ✅ validada |
| 53 | 2 Fácil | wrapper | CSearchLight::IsSpottedEntity | `0x00493900` | 90 | ✅ validada |
| 17 | 3 Media | wrapper | CCarCtrl::SteerAIHeliToCrashAndBurn | `0x004238E0` | 89 | ✅ validada |
| 42 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por WaitForPedUnwind) | `0x0061A3D0` | 85 | ✅ validada |
| 7 | 2 Fácil | wrapper | CCarCtrl::SteerAIPlaneToCrashAndBurn | `0x00423880` | 81 | ✅ validada |
| 19 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CameraObbeCinemaCycle) | `0x0050B830` | 77 | ✅ validada |
| 90 | 2 Fácil | wrapper | CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer | `0x0042F370` | 76 | ✅ validada |
| 55 | 3 Media | wrapper | CCarCtrl::SteerAIPlaneTowardsTargetCoors | `0x00423790` | 75 | ✅ validada |
| 27 | 2 Fácil | wrapper | CTaskComplexPartnerChat::MakeAbortable | `0x00682C60` | 65 | ✅ validada |
| 9 | 2 Fácil | wrapper | FindUpgradeCB | `0x006D3370` | 61 | ✅ validada |
| 49 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x00606060` | 59 | ✅ validada |
| 16 | 3 Media | wrapper | CQuaternion::Get | `0x0059C230` | 57 | ✅ validada |
| 50 | 2 Fácil | wrapper | CTaskComplexPartner::RemoveStreamedAnims | `0x00682370` | 56 | ✅ validada |
| 8 | 1 Muy fácil | wrapper | FindReplacementUpgradeCB | `0x006D3490` | 53 | ✅ validada |
| 12 | 1 Muy fácil | embedded_call | helper sin símbolo confirmado (usado por GetAnimHierarchyFromFrame) | `0x00734A70` | 51 | ✅ validada |
| 14 | 1 Muy fácil | wrapper | CAEStreamingDecoder scalar deleting destructor (operator delete de la clase) | `0x004F2860` | 50 | ✅ validada |
| 48 | 2 Fácil | wrapper | CDecisionMakerTypes::AddEventResponse | `0x006044C0` | 50 | ✅ validada |
| 24 | 2 Fácil | wrapper | CTaskComplexKillPedOnFoot::MakeAbortable | `0x00625E40` | 48 | ✅ validada |
| 6 | 1 Muy fácil | wrapper | CGarages::CountCarsInHideoutGarage | `0x0044A210` | 47 | ✅ validada |
| 3 | 1 Muy fácil | wrapper | CAutoPilot::RemoveOnePathNode | `0x0041B950` | 45 | ✅ validada |
| 13 | 2 Fácil | wrapper | CCarCtrl::ThisVehicleShouldTryNotToTurn | `0x00421FE0` | 38 | ✅ validada |
| 5 | 1 Muy fácil | wrapper | CDecisionMakerTypes::FlushDecisionMakerEventResponse | `0x00604490` | 35 | ✅ validada |
| 4 | 1 Muy fácil | wrapper | CPedGroups::RemoveAllFollowersFromGroup | `0x005FB8A0` | 32 | ✅ validada |
| 2 | 1 Muy fácil | embedded_call | CPool&lt;CVehicle&gt;::GetAt (acceso por índice a la ranura de 0xA18 bytes) | `0x0041CC10` | 31 | ✅ validada |
| 47 | 2 Fácil | wrapper | CCarCtrl::GetAIHeliToFlyInDirection | `0x0042A730` | 28 | ✅ validada |
| 10 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CHud::DrawVitalStats) | `0x00406CE0` | 19 | ✅ validada |
| 20 | 2 Fácil | wrapper | CTaskComplexFollowPedFootsteps::MakeAbortable | `0x00694ED0` | 8 | ✅ validada |
| 1 | 1 Muy fácil | embedded_call | helper sin símbolo confirmado (usado por FightUnwind) | `0x0061A3A0` | 7 | ✅ validada |
| 44 | 2 Fácil | wrapper | CTaskComplexPartnerDeal::CreateFirstSubTask | `0x006823B0` | 5 | ✅ validada |

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
