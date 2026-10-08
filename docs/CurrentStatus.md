# Estado comprobado de ReSA · 8 de octubre de 2026

El catálogo contiene **771 clases, namespaces o grupos / 8446 entradas**, todas marcadas como reconstruidas. Hay **6736 entradas con evidencia histórica vinculada** y **1710 pendientes de validación**, distribuidas en 77 grupos. Reconstrucción no significa validación ni independencia del ejecutable.

El perfil de arranque histórico selecciona **694 grupos completos / 6136 funciones** y desactiva 2313 hooks del inventario de ejecución (incluye 3 extras NOTSA). El informe final no encontró discrepancias de estado. No es una prueba de comportamiento conjunta ni una validación vigente. La última instalación en la copia CVehicle Test usa el ASI SHA-256 `f33a7f5ca0ebdb08f4f77d91c2f8142c4a2776761191c3e683eb352fc3438cae`; sin carga automática de partidas. El argumento `--historical-hooks` es explícito; sin él se conserva el arranque normal. Las correspondencias de 14 entradas están documentadas en [el registro de arranque](../reversing/RequestedValidation20261008/historical-launch/README.md).

REA local fue corregido para admitir el EXE x86 canónico de GTA SA y conserva la admisión del EXE específico de GTA IV. Una conexión nueva verificó importación, bytes, pseudocódigo y ensamblado de CDecision::From y análisis de WinMain. Eso verifica la herramienta, no valida las 77 clases. Una conexión iniciada antes de la reparación puede necesitar reinicio. Véase [la reparación](../reversing/RequestedValidation20261008/REA_REPAIR.md).

El objetivo standalone existe en CMake, pero WinMain sólo permite exportar hooks y termina fuera de ese modo. Persisten llamadas a direcciones del original y globales ligados a su memoria. No existe evidencia de un juego independiente terminado. Las 17765 entradas detectadas por Ghidra no son un censo semántico exacto; los 9326 cuerpos sin nombre cruzado no equivalen automáticamente a funciones necesarias pendientes ni a nuevas clases.

**Vigencia actual: 0 funciones acreditadas.** Las 52 de Interior_c mantienen sus resultados históricos, pero la huella de dependencias ya no coincide tras los cambios del selector y la documentación.

Esta actualización conserva las mediciones históricas de cada banco. No reescribe huellas de evidencia ni aprueba funciones. Modificar documentación incluida en una revisión de dependencias puede retirar su vigencia sin convertir una prueba histórica en fallida.

## Pendientes por clase

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
| CDecision | 1 |
| CFont | 32 |
| CFormation | 1 |
| CGangWars | 32 |
| CGenericGameStorage | 5 |
| CGroupEventHandler | 30 |
| CHud | 30 |
| CIdleCam | 13 |
| CInterestingEvents | 2 |
| CMonsterTruck | 5 |
| CPad | 75 |
| CPathFind | 54 |
| CPedDamageResponseCalculator | 2 |
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
| CTaskComplexFollowLeaderInFormation | 1 |
| CTaskComplexGangJoinRespond | 2 |
| CTaskComplexGoToAttractor | 2 |
| CTaskComplexHitPedWithCar | 2 |
| CTaskComplexKillCriminal | 9 |
| CTaskComplexKillPedFromBoat | 2 |
| CTaskComplexKillPedOnFootArmed | 9 |
| CTaskComplexPlayHandSignalAnim | 3 |
| CTaskComplexProstituteSolicit | 7 |
| CTaskComplexSmartFleePoint | 2 |
| CTaskComplexSunbathe | 2 |
| CTaskComplexWanderStandard | 3 |
| CTaskGangHasslePed | 2 |
| CTaskGangHassleVehicle | 2 |
| CTaskGoToVehicleAndLean | 2 |
| CTaskSimpleFightingControl | 6 |
| CTaskSimpleGangDriveBy | 1 |
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
