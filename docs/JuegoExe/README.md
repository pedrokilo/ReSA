# Código del juego en el EXE: mapa de las 11,804 funciones

[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)

Región **0x401000–0x74FFFF**: 11,804 funciones, 3,007,877 bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera (se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 426 | 3.6% | 59,790 | 2.0% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 561 | 4.8% | 71,608 | 2.4% |
| B · Falta: método virtual | 911 | 7.7% | 38,541 | 1.3% |
| A · Falta: llamada desde código ya reemplazado | 1,018 | 8.6% | 141,546 | 4.7% |
| Envoltura pendiente (la DLL aún llama al EXE) | 265 | 2.2% | 233,607 | 7.8% |
| Reemplazada por C++ | 8,623 | 73.1% | 2,462,785 | 81.9% |

Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque hay 6,004 llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: [A](pendientes-A.md) · [B](pendientes-B.md) · [C](pendientes-C.md) · [D](pendientes-D.md).

## Por bloque de direcciones (64 KB)

Ordenados de menos a más funciones que faltan.

El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.

| Bloque | Funciones | Reemplazadas | Faltan | Bytes que faltan | Módulos con más faltantes (estimado) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0x520000 | 62 | 100% | 0 | 0 | — |
| 0x580000 | 129 | 99% | 1 | 67 | CRadar (1) |
| 0x510000 | 61 | 84% | 6 | 4,239 | CCam (2) |
| 0x560000 | 212 | 96% | 7 | 1,575 | CPlayerPedData (1) |
| 0x700000 | 138 | 94% | 8 | 1,145 | CShadowCamera (1), CRealTimeShadow (1), CRealTimeShadowManager (1) |
| 0x470000 | 46 | 76% | 11 | 770 | — |
| 0x4E0000 | 246 | 95% | 11 | 307 | CAEMFDecoder (2), CAEMP3BankLoader (1), CAEPedlessSpeechAudioEntity (1) |
| 0x570000 | 110 | 90% | 11 | 358 | CMenuManager (5), CTheZones (1) |
| 0x6A0000 | 112 | 86% | 13 | 2,639 | CAutomobile (9), CEventVehicleDamageCollision (2) |
| 0x6E0000 | 92 | 65% | 15 | 4,836 | CWaterLevel (6), CVehicle (4) |
| 0x6C0000 | 135 | 80% | 16 | 764 | CHeli (4), CBmx (2), CPlane (2) |
| 0x6F0000 | 182 | 90% | 17 | 4,406 | CTrain (6), CBoat (1), cHandlingDataMgr (1) |
| 0x710000 | 158 | 88% | 18 | 1,005 | Common (4), CFont (3), CStencilShadows (1) |
| 0x450000 | 172 | 86% | 19 | 5,764 | CReplay (10), CPathFind (2) |
| 0x5B0000 | 157 | 88% | 19 | 1,189 | CCutsceneObject (1) |
| 0x6B0000 | 83 | 70% | 25 | 1,301 | CBike (22), CAutomobile (2), CBmx (1) |
| 0x420000 | 135 | 32% | 29 | 3,009 | CCarCtrl (4), CTaskComplexLeaveAnyCar (1) |
| 0x500000 | 305 | 90% | 29 | 2,777 | CCam (5), CAEWMADecoder (2), CAudioEngine (2) |
| 0x410000 | 162 | 81% | 30 | 1,190 | CColStore (5), CCollision (2), CCarAI (2) |
| 0x480000 | 111 | 73% | 30 | 15,439 | CTheScripts (2), CTaskSimpleCower (1), CTaskSimpleSay (1) |
| 0x720000 | 203 | 84% | 31 | 4,001 | CMemoryMgr (4), CWeather (2), CCullZones (1) |
| 0x730000 | 278 | 84% | 32 | 1,890 | CTxdStore (6), CVisibilityPlugins (4), RwHelper (1) |
| 0x5A0000 | 116 | 72% | 33 | 3,034 | CClothesBuilder (3), CObject (1) |
| 0x6D0000 | 179 | 77% | 33 | 517 | CVehicle (29) |
| 0x430000 | 200 | 74% | 35 | 1,379 | CEntryExitManager (9), CCheat (1), CConversations (1) |
| 0x460000 | 164 | 76% | 37 | 8,197 | CScriptsForBrains (4), CTaskComplexClimb (1), CTheScripts (1) |
| 0x530000 | 275 | 86% | 38 | 2,452 | CEntity (6), CFileLoader (5), CGame (3) |
| 0x540000 | 176 | 78% | 38 | 2,054 | CPad (3), Common (3), CPlaceable (3) |
| 0x440000 | 194 | 72% | 39 | 6,754 | CPathFind (14), CGarage (10), CGarages (2) |
| 0x490000 | 153 | 65% | 42 | 11,752 | Fx_c (6), CTheScripts (4), CSetPiece (4) |
| 0x4F0000 | 203 | 79% | 42 | 3,202 | CAEUserRadioTrackManager (29), CAETwinLoopSoundEntity (2), CAEVehicleAudioEntity (2) |
| 0x4D0000 | 319 | 86% | 45 | 7,969 | RpAnimBlend (16), CAnimManager (3), CAEAmbienceTrackManager (3) |
| 0x680000 | 184 | 72% | 48 | 3,682 | CTaskManager (2), CTaskSimpleSwim (2), CTaskComplexArrestPed (1) |
| 0x600000 | 237 | 68% | 57 | 10,219 | CPlayerPed (2), CPedIntelligence (1), CDecisionMakerTypesFileLoader (1) |
| 0x5D0000 | 335 | 81% | 62 | 6,323 | CCustomCarEnvMapPipeline (5), JPegPlugin (1), CGenericGameStorage (1) |
| 0x610000 | 327 | 79% | 63 | 7,742 | Rs (4), CEventVehicleDamageWeapon (2), CPedPlacement (1) |
| 0x550000 | 257 | 70% | 73 | 7,383 | CPools (51), CQuadTreeNode<T> (6), SurfaceInfos_c (2) |
| 0x690000 | 310 | 75% | 73 | 7,179 | CTaskComplexSeekEntityAiming (5), CTaskSimpleHoldEntity (2), CTaskSimpleFacial (1) |
| 0x590000 | 255 | 71% | 74 | 8,611 | InteriorManager_c (2), CObject (2), Interior_c (1) |
| 0x620000 | 282 | 70% | 77 | 6,849 | CTaskSimpleStandStill (2), CTaskSimpleDie (2), CTaskSimpleStealthKill (1) |
| 0x640000 | 425 | 79% | 86 | 4,342 | CTaskSimpleCarWaitToSlowDown (1), CTaskSimpleCarSetPedOut (1), CTaskSimpleCarSlowBeDraggedOut (1) |
| 0x400000 | 281 | 68% | 89 | 11,256 | CStreaming (15), CColSphere (9), CdStream (1) |
| 0x740000 | 195 | 43% | 89 | 16,985 | Ps (4), Input (2) |
| 0x660000 | 300 | 69% | 91 | 8,132 | CTaskComplexGoToPointAndStandStill (3), CTaskLeanOnVehicle (2), CTaskComplexGoToAttractor (2) |
| 0x5F0000 | 292 | 66% | 100 | 3,718 | CPedGroupIntelligence (3), CTaskComplexBeStill (1) |
| 0x670000 | 290 | 65% | 101 | 11,150 | CTaskComplexWanderProstitute (2), CTaskSimpleGetUp (2), CTaskComplexGetUpAndStandStill (2) |
| 0x650000 | 387 | 72% | 108 | 5,820 | CTaskSimpleHitHead (2), CTaskSimpleGiveCPR (2), CTaskComplexTreatAccident (2) |
| 0x4C0000 | 371 | 70% | 110 | 15,874 | CModelInfo (19), CVehicleModelInfo (13), CDamageAtomicModelInfo (3) |
| 0x5E0000 | 254 | 51% | 120 | 11,637 | CPedAttractor (13), CPed (5) |
| 0x5C0000 | 139 | 9% | 127 | 27,874 | — |
| 0x630000 | 585 | 75% | 148 | 8,165 | CTaskComplexDie (2), CTaskComplexObserveTrafficLightsAndAchieveHeading (2), CTaskComplexUseGoggles (2) |
| 0x4B0000 | 360 | 49% | 184 | 9,793 | CEventHitByWaterCannon (5), CEventVehicleDied (4), CEventStuckInAir (4) |
| 0x4A0000 | 470 | 40% | 276 | 12,770 | FxInfoManager_c (96), FxEmitterBP_c (9), CEventGunShot (7) |

## Por módulo (estimado)

Los 60 módulos con más bytes por reconstruir, ordenados de menos a más bytes que faltan. Para funciones sin nombre el módulo es la heurística de vecindad descrita arriba; la columna «Confianza alta» cuenta las faltantes cuyo módulo coincide en ambos vecinos.

| Módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan | Confianza alta |
| --- | ---: | ---: | ---: | ---: | ---: |
| CQuaternion | 13 | 2 | 10 | 1,009 | 0 |
| CTaskSimpleBeHit | 16 | 8 | 8 | 1,022 | 0 |
| CGame | 44 | 24 | 15 | 1,037 | 3 |
| CTaskComplexStuckInAir | 17 | 6 | 11 | 1,075 | 1 |
| CTrain | 47 | 40 | 6 | 1,076 | 6 |
| CTaskComplexInvestigateDisturbance | 17 | 8 | 9 | 1,125 | 0 |
| CTaskComplexSeekEntityAnyMeansXYOffset | 9 | 5 | 4 | 1,138 | 0 |
| CObject | 60 | 48 | 9 | 1,171 | 3 |
| Interior_c | 56 | 52 | 4 | 1,200 | 1 |
| CEventVehicleOnFire | 21 | 3 | 18 | 1,226 | 0 |
| CCompressedMatrixNotAligned | 16 | 5 | 11 | 1,229 | 0 |
| CEventVehicleCollision | 19 | 3 | 16 | 1,230 | 0 |
| CTaskSimpleRunTimedAnim | 20 | 7 | 13 | 1,251 | 0 |
| CTaskSimpleHoldEntity | 23 | 14 | 9 | 1,260 | 2 |
| CAttractorScanner | 4 | 1 | 3 | 1,273 | 0 |
| CPed | 158 | 145 | 9 | 1,279 | 5 |
| CClothesBuilder | 36 | 23 | 13 | 1,285 | 3 |
| CPlaceable | 50 | 16 | 28 | 1,336 | 3 |
| CVehicleModelInfo | 82 | 67 | 15 | 1,356 | 13 |
| CTaskComplexAvoidOtherPedWhileWandering | 25 | 15 | 10 | 1,382 | 0 |
| CPlayerInfo | 33 | 28 | 5 | 1,405 | 0 |
| CTaskInteriorUseInfo | 11 | 8 | 3 | 1,483 | 0 |
| CTaskSimplePutDownEntity | 9 | 3 | 6 | 1,595 | 0 |
| CTaskComplexWanderGang | 10 | 4 | 6 | 1,750 | 0 |
| CPedAttractor | 48 | 20 | 28 | 1,774 | 13 |
| CTaskSimpleThrowControl | 20 | 6 | 14 | 1,802 | 0 |
| CTaskComplexUseClosestFreeScriptedAttractor | 23 | 8 | 15 | 1,904 | 0 |
| CCustomRoadsignMgr | 10 | 9 | 1 | 1,939 | 1 |
| SurfaceInfos_c | 49 | 47 | 2 | 2,174 | 2 |
| CEventVehicleHitAndRun | 33 | 3 | 30 | 2,237 | 0 |
| CTaskSimpleFight | 18 | 8 | 7 | 2,403 | 1 |
| CCarCtrl | 109 | 30 | 5 | 2,481 | 4 |
| CEventDamage | 53 | 20 | 33 | 2,525 | 2 |
| CGarage | 24 | 8 | 12 | 2,528 | 10 |
| CTaskComplexTurnToFaceEntityOrCoord | 23 | 9 | 14 | 2,607 | 0 |
| CTaskComplexGoToPointAnyMeans | 13 | 7 | 6 | 2,633 | 1 |
| CAEUserRadioTrackManager | 46 | 15 | 31 | 2,635 | 29 |
| CTaskComplexSeekCoverUntilTargetDead | 20 | 12 | 8 | 2,642 | 0 |
| CEntityScanner | 18 | 5 | 12 | 2,673 | 0 |
| CCullZones | 38 | 22 | 16 | 2,899 | 1 |
| CMaths | 30 | 3 | 27 | 3,109 | 0 |
| CCustomCarEnvMapPipeline | 62 | 37 | 25 | 3,547 | 5 |
| CPedIntelligencePool | 10 | 1 | 9 | 3,617 | 0 |
| CAutomobile | 113 | 99 | 11 | 3,620 | 11 |
| CPools | 71 | 19 | 52 | 3,733 | 51 |
| CPathFind | 87 | 60 | 19 | 4,015 | 16 |
| CWaterLevel | 48 | 33 | 6 | 4,137 | 6 |
| CReplay | 57 | 47 | 10 | 4,172 | 10 |
| CCam | 70 | 53 | 15 | 5,230 | 7 |
| CRunningScript | 43 | 39 | 4 | 5,919 | 0 |
| RpAnimBlend | 59 | 43 | 16 | 6,102 | 16 |
| CScriptsForBrains | 13 | 8 | 5 | 6,352 | 4 |
| FxInfoManager_c | 124 | 4 | 119 | 6,656 | 96 |
| CPedAttractorManager | 85 | 14 | 71 | 8,097 | 0 |
| CTaskComplexSeekEntityRadiusAngleOffset | 15 | 9 | 5 | 8,771 | 1 |
| CEntity | 113 | 69 | 44 | 10,348 | 6 |
| CTheScripts | 87 | 70 | 16 | 10,496 | 7 |
| CWeaponModelInfo | 45 | 4 | 39 | 12,299 | 0 |
| Win | 109 | 20 | 69 | 14,247 | 0 |
| FxEmitterBP_c | 155 | 14 | 141 | 28,395 | 9 |

Mostrados los 60 primeros de 782 módulos; el resto está en el CSV.

## Cómo usarlo

- Filtra `funciones.csv` por `estado` = `A` y ordena por `llamadas_desde_reemplazadas` para empezar por lo más usado por código ya reconstruido.
- Una función de 16 bytes o menos suele ser un stub o thunk; mira `thunk` antes de tratarla como trabajo.
- Los grupos C y D pueden contener código muerto: confirma con análisis dinámico antes de reconstruir.
