# Estado comprobado de ReSA · 8 de octubre de 2026

**Corte documental: 8 de octubre de 2026, después del replay aprobado de las 12:41, hora de Ciudad de México.** Los contadores corresponden al catálogo sincronizado con 25 aprobaciones nuevas. Las sondas posteriores pendientes no se suman.

El catálogo contiene **771 clases, namespaces o grupos / 8446 entradas**, todas marcadas como reconstruidas. Hay **6761 entradas con evidencia histórica vinculada** y **1685 pendientes de validación**, distribuidas en **66 grupos**. El alcance solicitado inicial fue de 77 grupos y 1710 funciones; 25 identidades nuevas quedaron aprobadas para sus fixtures. Reconstrucción no significa validación ni independencia del ejecutable.

**Vigencia actual: 0 funciones acreditadas.** Las validaciones nuevas tienen `dependency_review=null`: son evidencia histórica aprobada, con límites explícitos. Las 52 de Interior_c mantienen sus resultados históricos; su huella de dependencias no coincide con el árbol actual. Esta documentación no cambia huellas de evidencia ni acredita dependencias.

El **snapshot de arranque histórico del 8 de octubre de 2026** seleccionó **694 grupos completos / 6136 funciones** y desactivó 2313 hooks del inventario de ejecución (incluye 3 extras NOTSA), sin discrepancias entre selección y estados. El ASI de ese arranque fue `f33a7f5ca0ebdb08f4f77d91c2f8142c4a2776761191c3e683eb352fc3438cae`. Es un snapshot fechado; no identifica la última instalación ni certifica comportamiento conjunto. Las sondas posteriores restauran la instalación que encontraron. La whitelist histórica de ese arranque permanece fija; las nuevas aprobaciones no se añadieron automáticamente. El argumento `--historical-hooks` es explícito y no implica validación vigente. [Registro de arranque y correspondencias](../reversing/RequestedValidation20261008/historical-launch/README.md).

## Veinticinco aprobaciones nuevas, con alcance limitado

| Función | Pares aprobados | Evidencia conservada | Alcance |
| --- | ---: | --- | --- |
| CDecision::From | 512 | [Resumen](../reversing/RequestedLeafValidation/LiveBatch/validation-summary.json) | Objetos separados y autoasignación; excluye solapamiento parcial. |
| CTaskSimpleGangDriveBy::GetTaskType | 768 | [Resumen](../reversing/RequestedLeafValidation/LiveBatch/validation-summary.json) | Entrada directa, tabla GTA y despacho virtual; 256 combinaciones de campos. |
| CPedDamageResponseCalculator::Constructor | 1024 | [Resumen](../reversing/RequestedMenuBatch/runs/constructor/20261008-111101-552593/enriched-summary.json) | Campos/padding y retorno this; no acredita cálculo de daño ni actores reales. |
| CInterestingEvents::InvalidateNonVisibleEvents | 384 | [Resumen](../reversing/RequestedMenuBatch/runs/visibility/20261008-112224-200531/enriched-summary.json) | Ocho eventos, cámaras y visibilidad; LOS y limpieza de referencias aislados con trazas. |
| CFormation::FindCoverPointsBehindBox | 4096 | [Resumen](../reversing/RequestedMenuBatch/runs/formation-expanded/20261008-113522-938438/enriched-summary.json) | Cajas no degeneradas, transformaciones rígidas y límites; comparación completa de salida. |
| CTaskComplexWanderStandard::GetWanderType | 1280 | [Resumen](../reversing/RequestedPreflightWander/runs/20261008-115205-491044/enriched-summary.json) | Estado y despacho del fixture; ScanForStuff usa colaboradores trazados y decisiones modeladas. Sin integración con actores reales. |
| CTaskComplexGangJoinRespond::Constructor | 128 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexGoToAttractor::Constructor | 128 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSmartFleePoint::Constructor | 128 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexGangJoinRespond::Destructor | 192 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexGoToAttractor::Destructor | 160 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSmartFleePoint::Destructor | 160 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexFollowLeaderInFormation::Destructor | 192 | [Resumen](../reversing/RequestedPreflightTasks/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexKillPedFromBoat::Constructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexKillPedFromBoat::Destructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexHitPedWithCar::Constructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexHitPedWithCar::Destructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSunbathe::Constructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexSunbathe::Destructor | 256 | [Resumen](../reversing/RequestedPreflightTaskCtorDtor/validation-summary.json) | Estado privado y terminal CTask; referencias/animaciones aisladas y child privado. Sin integración de juego. |
| CTaskComplexWanderStandard::Constructor | 2048 | [Resumen](../reversing/RequestedWanderRemaining/validation-summary.json) | Estado y despacho del fixture; ScanForStuff usa colaboradores trazados y decisiones modeladas. Sin integración con actores reales. |
| CTaskComplexWanderStandard::ScanForStuff | 4096 | [Resumen](../reversing/RequestedWanderRemaining/validation-summary.json) | Estado y despacho del fixture; ScanForStuff usa colaboradores trazados y decisiones modeladas. Sin integración con actores reales. |
| CTaskSimpleFightingControl::Constructor | 256 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |
| CTaskComplexProstituteSolicit::Constructor | 128 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |
| CTaskSimpleFightingControl::Destructor | 32 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |
| CTaskComplexProstituteSolicit::Destructor | 256 | [Resumen](../reversing/RequestedPreflightAdditionalLifetimes/runs/20261008-124111-185817/enriched-summary.json) | Bytes de ctor/dtor, tablas nativas/DLL y trazas de referencias/jugador/child aislados; CRT y entradas restaurados. Sin actores reales. |

Los resúmenes incluyen el EXE y DLL ejecutados, fuentes/instrumentos congelados, comparaciones y comprobaciones de restauración. Los errores previos permanecen conservados. La aprobación de una función no acredita el resto de su clase; los bancos preparados o fallidos no suman validaciones. [Inventario por función y vigencia](ValidationInventory.md).

## REA y dependencia del EXE

REA local admite el EXE x86 canónico de GTA SA y conserva la admisión del EXE específico de GTA IV. La conexión nueva verificó importación, bytes, pseudocódigo y ensamblado de CDecision::From y análisis de WinMain. La campaña estática resolvió direcciones para las **1710 identidades iniciales**, obtuvo **1395 dossiers** y conservó **315 fallos de búsqueda de procedimiento**. Esto no es aprobación dinámica ni eliminación de dependencias. [Resumen completo de campaña](../reversing/RequestedValidation20261008/rea-campaign-summary.json) y [reparación del proveedor](../reversing/RequestedValidation20261008/REA_REPAIR.md). GetTaskType de GangDriveBy conserva la limitación de reconocimiento en Ghidra: su cuerpo completo de seis bytes se acreditó con bytes y ensamblado, sin inventar pseudocódigo.

El objetivo standalone existe en CMake, pero WinMain sólo permite exportar hooks y termina fuera de ese modo. Persisten llamadas a direcciones del original y globales ligados a su memoria. No existe evidencia de un juego independiente terminado. Las 17765 entradas detectadas por Ghidra no son un censo semántico exacto; los 9326 cuerpos sin nombre cruzado no equivalen automáticamente a funciones necesarias pendientes ni a nuevas clases.

Esta actualización conserva las mediciones históricas de cada banco. Modificar documentación incluida en una revisión de dependencias puede retirar su vigencia sin convertir una prueba histórica en fallida.

## Pendientes por clase

Tabla generada desde `docs/hooks.csv`; cuenta `validated=0`, independientemente del bloqueo del hook.

| Clase/grupo | Funciones sin validar |
| --- | ---: |
| CAEAudioHardware | 1 |
| CAEFrontendAudioEntity | 4 |
| CAEPedAudioEntity | 15 |
| CAEPoliceScannerAudioEntity | 13 |
| CAERadioTrackManager | 29 |
| CAEScriptAudioEntity | 8 |
| CAEStreamingChannel | 18 |
| CAEVehicleAudioEntity | 102 |
| CAudioEngine | 82 |
| CAutomobile | 99 |
| CBike | 22 |
| CCam | 14 |
| CCamera | 91 |
| CCarEnterExit | 27 |
| CCheat | 72 |
| CClothesBuilder | 15 |
| CCollision | 1 |
| CCustomBuildingRenderer | 3 |
| CFont | 32 |
| CGangWars | 32 |
| CGenericGameStorage | 5 |
| CGroupEventHandler | 30 |
| CHud | 30 |
| CIdleCam | 13 |
| CInterestingEvents | 1 |
| CMonsterTruck | 5 |
| CPad | 75 |
| CPathFind | 54 |
| CPedDamageResponseCalculator | 1 |
| CPedGeometryAnalyser | 9 |
| CPedGroupPlacer | 1 |
| CPickups | 35 |
| CPlane | 14 |
| CPlayerPed | 1 |
| CPools | 16 |
| CPopulation | 64 |
| CPostEffects | 35 |
| CProjectileInfo | 1 |
| CRadar | 63 |
| CRealTimeShadowManager | 5 |
| CRoadBlocks | 7 |
| CRope | 5 |
| CRunningScript | 24 |
| CShadows | 21 |
| CStreaming | 94 |
| CTaskComplexArrestPed | 4 |
| CTaskComplexKillCriminal | 9 |
| CTaskComplexKillPedOnFootArmed | 9 |
| CTaskComplexPlayHandSignalAnim | 3 |
| CTaskComplexProstituteSolicit | 5 |
| CTaskGangHasslePed | 2 |
| CTaskGangHassleVehicle | 2 |
| CTaskGoToVehicleAndLean | 2 |
| CTaskSimpleFightingControl | 4 |
| CTaskSimplePlayerOnFoot | 11 |
| CTaskUtilityLineUpPedWithCar | 2 |
| CTheScripts | 69 |
| CTrain | 20 |
| CVehicleModelInfo | 75 |
| CVisibilityPlugins | 84 |
| CWaterLevel | 25 |
| CWeapon | 35 |
| FxEmitterBP_c | 2 |
| FxSystem_c | 31 |
| InteriorGroup_c | 1 |
| Win | 1 |
