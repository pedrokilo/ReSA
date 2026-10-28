<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Código del juego en el EXE: mapa de las 11,804 funciones

[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)

Región **0x401000–0x74FFFF**: 11,804 funciones, 3,007,877 bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera (se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 423 | 3.6% | 59,529 | 2.0% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 529 | 4.5% | 67,680 | 2.3% |
| B · Falta: método virtual | 905 | 7.7% | 36,916 | 1.2% |
| A · Falta: llamada desde código ya reemplazado | 1,028 | 8.7% | 138,122 | 4.6% |
| Envoltura pendiente (la DLL aún llama al EXE) | 64 | 0.5% | 86,814 | 2.9% |
| Reemplazada por C++ | 8,855 | 75.0% | 2,618,816 | 87.1% |

Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque hay 6,004 llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: [A](pendientes-A.md) · [B](pendientes-B.md) · [C](pendientes-C.md) · [D](pendientes-D.md).

## Por bloque de direcciones (64 KB)

Ordenados de mayor a menor porcentaje reemplazado.

El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.

| Bloque | Funciones | Reemplazadas | Faltan | Bytes que faltan | Módulos con más faltantes (estimado) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0x520000 | 62 | 100% | 0 | 0 | — |
| 0x580000 | 129 | 99% | 1 | 67 | CRadar (1) |
| 0x560000 | 212 | 98% | 5 | 191 | CPlayerPedData (1) |
| 0x4E0000 | 246 | 96% | 11 | 307 | CAEMFDecoder (2), CAEMP3BankLoader (1), CAEPedlessSpeechAudioEntity (1) |
| 0x700000 | 138 | 94% | 8 | 1,145 | CShadowCamera (1), CRealTimeShadow (1), CRealTimeShadowManager (1) |
| 0x500000 | 305 | 91% | 26 | 2,324 | CCam (3), CAEWMADecoder (2), CAudioEngine (2) |
| 0x6F0000 | 182 | 91% | 17 | 4,406 | CTrain (6), CBoat (1), cHandlingDataMgr (1) |
| 0x570000 | 110 | 90% | 11 | 358 | CMenuManager (5), CTheZones (1) |
| 0x450000 | 172 | 89% | 19 | 5,764 | CReplay (10), CPathFind (2) |
| 0x5B0000 | 157 | 89% | 18 | 992 | CCutsceneObject (1) |
| 0x730000 | 278 | 88% | 31 | 1,839 | CTxdStore (6), CVisibilityPlugins (4) |
| 0x710000 | 158 | 88% | 18 | 1,005 | Common (4), CFont (3), CStencilShadows (1) |
| 0x6A0000 | 112 | 88% | 14 | 3,644 | CAutomobile (10), CEventVehicleDamageCollision (2) |
| 0x530000 | 275 | 86% | 38 | 2,452 | CEntity (6), CFileLoader (5), CGame (3) |
| 0x6C0000 | 135 | 86% | 16 | 764 | CHeli (4), CBmx (2), CPlane (2) |
| 0x4D0000 | 319 | 86% | 45 | 7,969 | RpAnimBlend (16), CAnimManager (3), CAEAmbienceTrackManager (3) |
| 0x510000 | 61 | 85% | 5 | 794 | CCam (2) |
| 0x720000 | 203 | 85% | 31 | 4,001 | CMemoryMgr (4), CWeather (2), CCullZones (1) |
| 0x5D0000 | 335 | 81% | 62 | 6,323 | CCustomCarEnvMapPipeline (5), JPegPlugin (1), CGenericGameStorage (1) |
| 0x410000 | 162 | 81% | 30 | 1,190 | CColStore (5), CCollision (2), CCarAI (2) |
| 0x640000 | 425 | 81% | 80 | 3,716 | CTaskSimpleCarWaitToSlowDown (1), CTaskSimpleCarSetPedOut (1), CTaskSimpleCarSlowBeDraggedOut (1) |
| 0x610000 | 327 | 81% | 63 | 7,742 | Rs (4), CEventVehicleDamageWeapon (2), CPedPlacement (1) |
| 0x6E0000 | 92 | 80% | 15 | 4,836 | CWaterLevel (6), CVehicle (4) |
| 0x430000 | 200 | 80% | 35 | 1,379 | CEntryExitManager (9), CCheat (1), CConversations (1) |
| 0x4F0000 | 203 | 80% | 41 | 3,100 | CAEUserRadioTrackManager (28), CAETwinLoopSoundEntity (2), CAEVehicleAudioEntity (2) |
| 0x440000 | 194 | 79% | 39 | 6,754 | CPathFind (14), CGarage (10), CGarages (2) |
| 0x6D0000 | 179 | 79% | 33 | 517 | CVehicle (29) |
| 0x540000 | 176 | 78% | 38 | 2,054 | CPad (3), Common (3), CPlaceable (3) |
| 0x690000 | 310 | 78% | 68 | 4,732 | CTaskComplexSeekEntityAiming (5), CTaskSimpleHoldEntity (2), CTaskSimpleFacial (1) |
| 0x460000 | 164 | 78% | 36 | 7,782 | CScriptsForBrains (4), CTaskComplexClimb (1), CTheScripts (1) |
| 0x470000 | 46 | 76% | 11 | 770 | — |
| 0x630000 | 585 | 75% | 145 | 7,768 | CTaskComplexDie (2), CTaskComplexObserveTrafficLightsAndAchieveHeading (2), CTaskComplexUseGoggles (2) |
| 0x420000 | 135 | 74% | 28 | 2,930 | CCarCtrl (4), CTaskComplexLeaveAnyCar (1) |
| 0x680000 | 184 | 74% | 48 | 3,682 | CTaskManager (2), CTaskSimpleSwim (2), CTaskComplexArrestPed (1) |
| 0x620000 | 282 | 73% | 75 | 6,603 | CTaskSimpleStandStill (2), CTaskSimpleDie (2), CTaskSimpleStealthKill (1) |
| 0x480000 | 111 | 73% | 30 | 15,439 | CTheScripts (2), CTaskSimpleCower (1), CTaskSimpleSay (1) |
| 0x600000 | 237 | 73% | 56 | 10,160 | CPlayerPed (2), CPedIntelligence (1), CDecisionMakerTypesFileLoader (1) |
| 0x5A0000 | 116 | 72% | 32 | 2,867 | CClothesBuilder (3), CObject (1) |
| 0x650000 | 387 | 72% | 108 | 5,820 | CTaskSimpleHitHead (2), CTaskSimpleGiveCPR (2), CTaskComplexTreatAccident (2) |
| 0x590000 | 255 | 72% | 72 | 8,455 | InteriorManager_c (2), CObject (2), Interior_c (1) |
| 0x550000 | 257 | 72% | 73 | 7,383 | CPools (51), CQuadTreeNode<T> (6), SurfaceInfos_c (2) |
| 0x490000 | 153 | 71% | 42 | 11,752 | Fx_c (6), CTheScripts (4), CSetPiece (4) |
| 0x6B0000 | 83 | 70% | 25 | 1,301 | CBike (22), CAutomobile (2), CBmx (1) |
| 0x4C0000 | 371 | 70% | 110 | 15,874 | CModelInfo (19), CVehicleModelInfo (13), CDamageAtomicModelInfo (3) |
| 0x660000 | 300 | 70% | 91 | 8,132 | CTaskComplexGoToPointAndStandStill (3), CTaskLeanOnVehicle (2), CTaskComplexGoToAttractor (2) |
| 0x400000 | 281 | 69% | 88 | 11,237 | CStreaming (15), CColSphere (9), CdStream (1) |
| 0x5F0000 | 292 | 66% | 100 | 3,718 | CPedGroupIntelligence (3), CTaskComplexBeStill (1) |
| 0x670000 | 290 | 65% | 101 | 11,150 | CTaskComplexWanderProstitute (2), CTaskSimpleGetUp (2), CTaskComplexGetUpAndStandStill (2) |
| 0x5E0000 | 254 | 53% | 120 | 11,637 | CPedAttractor (13), CPed (5) |
| 0x4B0000 | 360 | 49% | 184 | 9,793 | CEventHitByWaterCannon (5), CEventVehicleDied (4), CEventStuckInAir (4) |
| 0x740000 | 195 | 44% | 89 | 16,985 | Ps (4), Input (2) |
| 0x4A0000 | 470 | 41% | 276 | 12,770 | FxInfoManager_c (96), FxEmitterBP_c (9), CEventGunShot (7) |
| 0x5C0000 | 139 | 9% | 127 | 27,874 | — |

## Por módulo (estimado)

Módulos con alguna función por reconstruir, de menos a más funciones que faltan. Para funciones sin nombre el módulo es la heurística de vecindad descrita arriba; la columna «Confianza alta» cuenta las faltantes cuyo módulo coincide en ambos vecinos.

| Módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan | Confianza alta |
| --- | ---: | ---: | ---: | ---: | ---: |
| COcclusion | 6 | 5 | 1 | 1 | 0 |
| CPlaneTrails | 5 | 4 | 1 | 1 | 0 |
| CAEPedlessSpeechAudioEntity | 12 | 11 | 1 | 3 | 1 |
| CTaskComplexGangJoinRespond | 8 | 7 | 1 | 6 | 1 |
| CFormation | 11 | 10 | 1 | 7 | 0 |
| CTaskSimpleSetCharDecisionMaker | 7 | 6 | 1 | 7 | 0 |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | 3 | 2 | 1 | 9 | 0 |
| CPedGroupDefaultTaskAllocatorFollowLimited | 3 | 2 | 1 | 9 | 0 |
| CPedGroupDefaultTaskAllocatorRandom | 3 | 2 | 1 | 9 | 0 |
| CPedGroupDefaultTaskAllocatorStandStill | 3 | 2 | 1 | 9 | 0 |
| CCrime | 3 | 2 | 1 | 10 | 0 |
| CCivilianPed | 4 | 3 | 1 | 11 | 1 |
| CDamageManager | 28 | 27 | 1 | 11 | 0 |
| CTaskComplexEnterCarAsDriver | 4 | 3 | 1 | 11 | 0 |
| CTaskComplexEnterCarAsPassenger | 4 | 3 | 1 | 11 | 0 |
| CTaskComplexFacial | 10 | 9 | 1 | 11 | 1 |
| CTaskComplexPartnerDeal | 2 | 1 | 1 | 11 | 0 |
| CTaskSimpleAchieveHeading | 6 | 5 | 1 | 11 | 1 |
| CTaskSimpleGoTo | 5 | 4 | 1 | 11 | 1 |
| CTaskSimpleSetStayInSamePlace | 6 | 5 | 1 | 11 | 1 |
| BoneNode_c | 18 | 17 | 1 | 12 | 0 |
| CUserDisplay | 3 | 2 | 1 | 15 | 0 |
| CGenericGameStorage | 20 | 19 | 1 | 16 | 1 |
| CPedGroup | 13 | 12 | 1 | 16 | 0 |
| CTaskSimpleChat | 4 | 3 | 1 | 18 | 0 |
| CFileMgr | 18 | 17 | 1 | 19 | 0 |
| CTaskSimpleCarWaitToSlowDown | 7 | 6 | 1 | 21 | 1 |
| CTimeCycle | 22 | 21 | 1 | 26 | 0 |
| BreakObject_c | 10 | 9 | 1 | 27 | 0 |
| CAEVorbisDecoder | 14 | 13 | 1 | 27 | 0 |
| CAEWaveDecoder | 10 | 9 | 1 | 27 | 0 |
| CPlayerPedData | 6 | 5 | 1 | 27 | 1 |
| CRealTimeShadowManager | 8 | 7 | 1 | 27 | 1 |
| CAESmoothFadeThread | 11 | 10 | 1 | 30 | 1 |
| CBoat | 24 | 23 | 1 | 30 | 1 |
| CCutsceneObject | 7 | 6 | 1 | 30 | 1 |
| CDummyObject | 3 | 2 | 1 | 30 | 1 |
| CMonsterTruck | 12 | 11 | 1 | 30 | 1 |
| CTaskComplexBeCop | 4 | 3 | 1 | 30 | 1 |
| CTaskComplexBeInGroup | 11 | 10 | 1 | 30 | 0 |
| CTaskComplexBeStill | 7 | 6 | 1 | 30 | 1 |
| CTaskComplexCarDriveMissionFleeScene | 5 | 4 | 1 | 30 | 1 |
| CTaskComplexCarDriveMissionKillPed | 5 | 4 | 1 | 30 | 1 |
| CTaskComplexClimb | 5 | 4 | 1 | 30 | 1 |
| CTaskComplexDestroyCar | 10 | 9 | 1 | 30 | 0 |
| CTaskComplexDestroyCarArmed | 11 | 10 | 1 | 30 | 0 |
| CTaskComplexDestroyCarMelee | 11 | 10 | 1 | 30 | 0 |
| CTaskComplexDragPedFromCar | 3 | 2 | 1 | 30 | 0 |
| CTaskComplexEnterBoatAsDriver | 9 | 8 | 1 | 30 | 0 |
| CTaskComplexEnterCarAsDriverTimed | 10 | 9 | 1 | 30 | 0 |
| CTaskComplexEnterCarAsPassengerTimed | 10 | 9 | 1 | 30 | 0 |
| CTaskComplexEnterCarAsPassengerWait | 9 | 8 | 1 | 30 | 0 |
| CTaskComplexEvasiveDiveAndGetUp | 10 | 9 | 1 | 30 | 0 |
| CTaskComplexExtinguishFires | 9 | 8 | 1 | 30 | 0 |
| CTaskComplexGoToCarDoorAndStandStill | 12 | 11 | 1 | 30 | 0 |
| CTaskComplexInvestigateDeadPed | 10 | 9 | 1 | 30 | 0 |
| CTaskComplexKillCriminal | 12 | 11 | 1 | 30 | 0 |
| CTaskComplexKillPedGroupOnFoot | 10 | 9 | 1 | 30 | 0 |
| CTaskComplexKillPedOnFootMelee | 11 | 10 | 1 | 30 | 0 |
| CTaskComplexLeaveAnyCar | 8 | 7 | 1 | 30 | 1 |
| CTaskComplexLeaveCarAndFlee | 9 | 8 | 1 | 30 | 0 |
| CTaskComplexLeaveCarAndWander | 9 | 8 | 1 | 30 | 0 |
| CTaskComplexLeaveCarAsPassengerWait | 11 | 10 | 1 | 30 | 0 |
| CTaskComplexMoveBackAndJump | 9 | 8 | 1 | 30 | 0 |
| CTaskComplexPolicePursuit | 14 | 13 | 1 | 30 | 0 |
| CTaskComplexShuffleSeats | 11 | 10 | 1 | 30 | 0 |
| CTaskComplexTrackEntity | 12 | 11 | 1 | 30 | 0 |
| CTaskComplexUseAttractorPartner | 8 | 7 | 1 | 30 | 0 |
| CTaskComplexUseClosestFreeScriptedAttractorRun | 5 | 4 | 1 | 30 | 1 |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | 5 | 4 | 1 | 30 | 1 |
| CTaskComplexUseScriptedAttractor | 8 | 7 | 1 | 30 | 0 |
| CTaskComplexWaitForBackup | 9 | 8 | 1 | 30 | 0 |
| CTaskComplexWalkRoundCar | 14 | 13 | 1 | 30 | 0 |
| CTaskInteriorBeInHouse | 9 | 8 | 1 | 30 | 0 |
| CTaskInteriorGoToInfo | 8 | 7 | 1 | 30 | 0 |
| CTaskInteriorLieInBed | 8 | 7 | 1 | 30 | 0 |
| CTaskInteriorShopKeeper | 8 | 7 | 1 | 30 | 0 |
| CTaskInteriorSitAtDesk | 10 | 9 | 1 | 30 | 0 |
| CTaskSimpleBikeJacked | 9 | 8 | 1 | 30 | 0 |
| CTaskSimpleCarAlign | 11 | 10 | 1 | 30 | 0 |
| CTaskSimpleCarCloseDoorFromInside | 11 | 10 | 1 | 30 | 0 |
| CTaskSimpleCarCloseDoorFromOutside | 11 | 10 | 1 | 30 | 0 |
| CTaskSimpleCarDrive | 15 | 14 | 1 | 30 | 0 |
| CTaskSimpleCarDriveTimed | 6 | 5 | 1 | 30 | 1 |
| CTaskSimpleCarGetIn | 10 | 9 | 1 | 30 | 0 |
| CTaskSimpleCarOpenDoorFromOutside | 11 | 10 | 1 | 30 | 0 |
| CTaskSimpleCarOpenLockedDoorFromOutside | 10 | 9 | 1 | 30 | 0 |
| CTaskSimpleCarSetPedInAsPassenger | 6 | 5 | 1 | 30 | 0 |
| CTaskSimpleCarSlowDragPedOut | 11 | 10 | 1 | 30 | 0 |
| CTaskSimpleEvasiveDive | 9 | 8 | 1 | 30 | 0 |
| CTaskSimpleGunControl | 7 | 6 | 1 | 30 | 0 |
| CTaskSimpleKillPedWithCar | 8 | 7 | 1 | 30 | 0 |
| CTaskSimpleNone | 5 | 4 | 1 | 30 | 0 |
| CTaskSimplePickUpBike | 10 | 9 | 1 | 30 | 0 |
| CTaskSimplePlayerOnFire | 7 | 6 | 1 | 30 | 0 |
| CTaskSimpleSay | 7 | 6 | 1 | 30 | 1 |
| CTaskSimpleSitDown | 9 | 8 | 1 | 30 | 0 |
| CTaskSimpleSitIdle | 8 | 7 | 1 | 30 | 0 |
| CTaskSimpleStandUp | 9 | 8 | 1 | 30 | 0 |
| CRoadBlocks | 9 | 8 | 1 | 31 | 0 |
| CClouds | 22 | 21 | 1 | 32 | 0 |
| CEventEscalator | 2 | 1 | 1 | 32 | 0 |
| CAECollisionAudioEntity | 19 | 18 | 1 | 33 | 1 |
| CTaskComplexUseEffect | 9 | 8 | 1 | 39 | 0 |
| CGeneral | 10 | 9 | 1 | 47 | 0 |
| CAEMP3BankLoader | 14 | 13 | 1 | 48 | 1 |
| CTaskSimpleLookAbout | 4 | 3 | 1 | 48 | 0 |
| CTaskAllocatorAttack | 6 | 5 | 1 | 50 | 0 |
| CAEScriptAudioEntity | 20 | 19 | 1 | 52 | 0 |
| CCollisionData | 9 | 8 | 1 | 52 | 0 |
| CPedPool | 4 | 3 | 1 | 54 | 0 |
| cBuoyancy | 11 | 10 | 1 | 54 | 0 |
| CConversations | 11 | 10 | 1 | 55 | 1 |
| CPathNode | 2 | 1 | 1 | 56 | 0 |
| CShadowCamera | 20 | 19 | 1 | 57 | 1 |
| CSprite | 16 | 15 | 1 | 57 | 0 |
| CCredits | 7 | 6 | 1 | 60 | 0 |
| CVehicleAnimGroupData | 6 | 5 | 1 | 61 | 0 |
| CAEBankLoader | 7 | 6 | 1 | 63 | 1 |
| CTimeInfo | 7 | 6 | 1 | 63 | 0 |
| CEntryInfoList | 2 | 1 | 1 | 65 | 0 |
| CRadar | 66 | 65 | 1 | 67 | 1 |
| CIplDefPool | 2 | 1 | 1 | 71 | 0 |
| CWanted | 34 | 33 | 1 | 76 | 0 |
| CDummy | 2 | 1 | 1 | 78 | 0 |
| CTaskSequences | 3 | 2 | 1 | 79 | 0 |
| CTagManager | 20 | 19 | 1 | 80 | 1 |
| CEventAcquaintancePed | 5 | 4 | 1 | 81 | 1 |
| CTaskSimpleAffectSecondaryBehaviour | 6 | 5 | 1 | 87 | 1 |
| CTaskSimpleCarSetPedOut | 6 | 5 | 1 | 90 | 1 |
| CGangs | 9 | 8 | 1 | 92 | 0 |
| CEntryExit | 12 | 11 | 1 | 96 | 1 |
| CTaskSimpleCarWaitForDoorNotToBeInUse | 8 | 7 | 1 | 101 | 0 |
| CUpsideDownCarCheck | 6 | 5 | 1 | 104 | 0 |
| CPointRoutePool | 2 | 1 | 1 | 106 | 0 |
| JPegPlugin | 6 | 5 | 1 | 108 | 1 |
| CTaskComplexWalkAlongsidePed | 10 | 9 | 1 | 110 | 0 |
| CScripted2dEffects | 6 | 5 | 1 | 116 | 1 |
| CCoronas | 12 | 11 | 1 | 117 | 1 |
| CTaskSimpleDead | 7 | 6 | 1 | 127 | 0 |
| CLoadedCarGroup | 9 | 8 | 1 | 152 | 0 |
| CDecision | 5 | 4 | 1 | 157 | 0 |
| CMouseControllerState | 4 | 3 | 1 | 208 | 0 |
| CGamma | 3 | 2 | 1 | 210 | 0 |
| PipelinePlugin | 4 | 3 | 1 | 273 | 0 |
| CPedPlacement | 4 | 3 | 1 | 283 | 1 |
| CTaskComplexSmartFleeEntity | 8 | 7 | 1 | 328 | 1 |
| CEntitySeekPosCalculatorFixedPos | 2 | 1 | 1 | 335 | 0 |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | 7 | 6 | 1 | 481 | 0 |
| CCamera | 118 | 116 | 1 | 527 | 0 |
| CCustomCarPlateMgr | 15 | 14 | 1 | 829 | 1 |
| CShadows | 31 | 30 | 1 | 837 | 1 |
| CCustomRoadsignMgr | 10 | 9 | 1 | 1,939 | 1 |
| CTaskComplexFallToDeath | 8 | 6 | 2 | 17 | 2 |
| CTaskComplexGetUpAndStandStill | 7 | 5 | 2 | 17 | 2 |
| CTaskComplexUseGoggles | 7 | 5 | 2 | 19 | 2 |
| CLodTimeModelInfo | 4 | 2 | 2 | 25 | 0 |
| CLodAtomicModelInfo | 5 | 3 | 2 | 31 | 0 |
| CPhysical | 59 | 57 | 2 | 32 | 1 |
| CTrailer | 15 | 13 | 2 | 33 | 1 |
| CAnimatedBuilding | 5 | 3 | 2 | 35 | 0 |
| CTaskComplexArrestPed | 9 | 7 | 2 | 36 | 1 |
| CTaskComplexCarDrive | 13 | 11 | 2 | 36 | 1 |
| CTaskGangHassleVehicle | 10 | 8 | 2 | 36 | 1 |
| CCustomBuildingDNPipeline | 17 | 15 | 2 | 39 | 1 |
| CDirectory | 12 | 10 | 2 | 39 | 0 |
| CPedType | 15 | 13 | 2 | 40 | 0 |
| CTaskComplexObserveTrafficLights | 10 | 8 | 2 | 40 | 0 |
| CQuadBike | 14 | 12 | 2 | 41 | 1 |
| CTaskAllocatorKillThreatsBasicRandomGroup | 4 | 2 | 2 | 41 | 0 |
| CTaskComplexAttractorPartnerWait | 7 | 5 | 2 | 41 | 1 |
| CTaskComplexDriveToPoint | 10 | 8 | 2 | 41 | 1 |
| CTaskComplexEnterAnyCarAsDriver | 7 | 5 | 2 | 41 | 1 |
| CTaskComplexGoToPointAndStandStillTimed | 8 | 6 | 2 | 41 | 1 |
| CTaskComplexUseEffectSprinting | 4 | 2 | 2 | 41 | 0 |
| CTaskComplexWanderMedic | 6 | 4 | 2 | 41 | 0 |
| CTaskSimpleTogglePedThreatScanner | 7 | 5 | 2 | 41 | 1 |
| CTimeModelInfo | 4 | 2 | 2 | 41 | 0 |
| ProcObjectMan_c | 11 | 9 | 2 | 41 | 0 |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | 6 | 4 | 2 | 42 | 0 |
| CTaskSimpleJetPack | 16 | 14 | 2 | 48 | 0 |
| CText | 11 | 9 | 2 | 48 | 0 |
| CAEAudioHardware | 57 | 55 | 2 | 54 | 2 |
| CAEPedAudioEntity | 22 | 20 | 2 | 57 | 0 |
| CTaskSimpleTired | 5 | 3 | 2 | 57 | 1 |
| CVehicleAnimGroup | 7 | 5 | 2 | 58 | 0 |
| CAEWMADecoder | 13 | 11 | 2 | 59 | 2 |
| CTaskComplexCarSlowBeDraggedOut | 13 | 11 | 2 | 60 | 0 |
| CTaskComplexFleeAnyMeans | 10 | 8 | 2 | 60 | 0 |
| CTaskComplexFollowPointRoute | 15 | 13 | 2 | 60 | 1 |
| CTaskComplexHitResponse | 9 | 7 | 2 | 60 | 0 |
| CTaskComplexKillPedOnFootArmed | 18 | 15 | 2 | 60 | 0 |
| CTaskComplexOnFire | 11 | 9 | 2 | 60 | 0 |
| CTaskComplexPassObject | 11 | 9 | 2 | 60 | 0 |
| CTaskComplexUseAttractor | 9 | 7 | 2 | 60 | 0 |
| CTaskComplexWanderCriminal | 8 | 6 | 2 | 60 | 1 |
| CTaskInteriorBeInOffice | 10 | 8 | 2 | 60 | 0 |
| CTaskSimpleCreateCarAndGetIn | 8 | 6 | 2 | 60 | 0 |
| CTaskSimpleEvasiveStep | 10 | 8 | 2 | 60 | 0 |
| CBuildingPool | 3 | 1 | 2 | 62 | 0 |
| CDummyPool | 3 | 1 | 2 | 62 | 0 |
| CObjectData | 4 | 2 | 2 | 62 | 0 |
| CPedShelterAttractor | 6 | 4 | 2 | 65 | 0 |
| CPedGroups | 13 | 11 | 2 | 72 | 0 |
| CPedIK | 11 | 9 | 2 | 72 | 0 |
| CCollision | 63 | 61 | 2 | 76 | 2 |
| CTaskSimpleShakeFist | 10 | 8 | 2 | 78 | 0 |
| Securom | 4 | 2 | 2 | 78 | 0 |
| CShotInfo | 7 | 5 | 2 | 80 | 0 |
| CBuilding | 4 | 2 | 2 | 83 | 0 |
| CWorld | 97 | 95 | 2 | 84 | 0 |
| CPedGroupDefaultTaskAllocatorChat | 4 | 2 | 2 | 86 | 0 |
| CAEAudioChannel | 14 | 12 | 2 | 88 | 2 |
| CAEStreamingChannel | 25 | 23 | 2 | 88 | 1 |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | 12 | 10 | 2 | 91 | 1 |
| CTaskComplexKillPedOnFoot | 3 | 1 | 2 | 96 | 0 |
| CTaskSimpleGiveCPR | 8 | 6 | 2 | 99 | 2 |
| CTaskSimpleHitHead | 6 | 4 | 2 | 99 | 2 |
| CInformGroupEvent | 5 | 3 | 2 | 101 | 0 |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | 4 | 2 | 2 | 105 | 0 |
| CTaskSimplePutDownEntity | 9 | 7 | 2 | 106 | 0 |
| CColLine | 3 | 1 | 2 | 110 | 0 |
| CInterestingEvents | 9 | 7 | 2 | 110 | 0 |
| CTaskComplexScreamInCarThenLeave | 11 | 9 | 2 | 118 | 0 |
| CTaskSimpleCarSetPedInAsDriver | 7 | 5 | 2 | 120 | 0 |
| CAnimBlendStaticAssociation | 8 | 6 | 2 | 121 | 0 |
| CTaskComplexPartnerChat | 4 | 2 | 2 | 123 | 0 |
| CTaskComplexUseSwatRope | 9 | 7 | 2 | 126 | 1 |
| CEventHandler | 74 | 72 | 2 | 130 | 2 |
| CAEGlobalWeaponAudioEntity | 8 | 6 | 2 | 135 | 1 |
| CTaskSimpleUseGun | 21 | 19 | 2 | 139 | 1 |
| CBox | 4 | 2 | 2 | 144 | 0 |
| CAETwinLoopSoundEntity | 9 | 7 | 2 | 174 | 2 |
| FxMemoryPool_c | 7 | 5 | 2 | 194 | 0 |
| CTaskSimpleRunAnim | 7 | 5 | 2 | 197 | 0 |
| CPedList | 9 | 7 | 2 | 203 | 0 |
| IKChain_c | 22 | 20 | 2 | 205 | 0 |
| WaterCreature_c | 5 | 3 | 2 | 211 | 0 |
| CTaskSimpleFightingControl | 9 | 7 | 2 | 215 | 0 |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | 7 | 5 | 2 | 229 | 2 |
| CCheat | 74 | 72 | 2 | 246 | 1 |
| CTaskSimpleGangDriveBy | 8 | 6 | 2 | 254 | 0 |
| WaterCreatureManager_c | 9 | 7 | 2 | 259 | 0 |
| CTaskSimpleWaitUntilPedIsInCar | 8 | 6 | 2 | 271 | 0 |
| Input | 7 | 5 | 2 | 302 | 2 |
| CAEWaterCannonAudioEntity | 9 | 7 | 2 | 309 | 1 |
| CStats | 53 | 51 | 2 | 320 | 1 |
| InteriorManager_c | 23 | 21 | 2 | 340 | 2 |
| CWeather | 15 | 13 | 2 | 352 | 2 |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | 9 | 7 | 2 | 382 | 1 |
| VideoMode | 6 | 4 | 2 | 382 | 0 |
| CDecisionMakerTypesFileLoader | 11 | 9 | 2 | 767 | 1 |
| SurfaceInfos_c | 49 | 47 | 2 | 2,174 | 2 |
| CPlayerInfo | 33 | 30 | 3 | 21 | 0 |
| CTaskComplexDie | 8 | 5 | 3 | 23 | 2 |
| CAEStaticChannel | 11 | 8 | 3 | 41 | 1 |
| C2dEffect | 17 | 14 | 3 | 46 | 0 |
| CEventVehicleDamageWeapon | 5 | 2 | 3 | 47 | 2 |
| CTaskComplexTreatAccident | 10 | 7 | 3 | 47 | 2 |
| CTaskComplexWanderStandard | 12 | 9 | 3 | 51 | 0 |
| CTheCarGenerators | 10 | 7 | 3 | 55 | 0 |
| CAudioEngine | 85 | 82 | 3 | 58 | 2 |
| CAEMFDecoder | 15 | 12 | 3 | 60 | 2 |
| CRealTimeShadow | 12 | 9 | 3 | 60 | 1 |
| CEventGlobalGroup | 6 | 3 | 3 | 63 | 2 |
| CTaskComplexGoToPointAiming | 11 | 8 | 3 | 65 | 0 |
| CTaskComplexUseSequence | 9 | 6 | 3 | 66 | 1 |
| CTaskComplexDiveFromAttachedEntityAndGetUp | 10 | 7 | 3 | 67 | 1 |
| CTaskComplexCarDriveWander | 8 | 5 | 3 | 71 | 1 |
| CTaskManager | 21 | 18 | 3 | 71 | 2 |
| CTaskComplexDrivePointRoute | 10 | 7 | 3 | 78 | 0 |
| CTaskComplexUseEffectRunning | 5 | 2 | 3 | 80 | 0 |
| CTaskSimpleFinishBrain | 5 | 2 | 3 | 85 | 0 |
| CTaskComplexPlayHandSignalAnim | 12 | 9 | 3 | 90 | 0 |
| CTaskSimpleCarShuffle | 12 | 9 | 3 | 90 | 0 |
| CTaskComplexUsePairedAttractor | 14 | 11 | 3 | 94 | 0 |
| CTaskComplexFollowNodeRoute | 22 | 19 | 3 | 102 | 0 |
| CPedGroupIntelligence | 30 | 27 | 3 | 117 | 3 |
| CBmx | 13 | 10 | 3 | 118 | 3 |
| CTaskSimpleInAir | 7 | 4 | 3 | 118 | 2 |
| CEventSource | 4 | 1 | 3 | 122 | 0 |
| CTaskSimpleFall | 9 | 6 | 3 | 129 | 2 |
| CEventPool | 4 | 1 | 3 | 137 | 0 |
| CTaskSimpleCarSlowBeDraggedOut | 12 | 9 | 3 | 141 | 1 |
| CTaskLeanOnVehicle | 8 | 5 | 3 | 142 | 2 |
| CTaskSimpleJump | 11 | 8 | 3 | 145 | 2 |
| CTaskSimpleClimb | 18 | 15 | 3 | 152 | 2 |
| CIplStore | 37 | 34 | 3 | 160 | 0 |
| CTaskSimplePlayerOnFoot | 15 | 12 | 3 | 160 | 0 |
| BreakManager_c | 9 | 6 | 3 | 164 | 0 |
| CTaskComplexProstituteSolicit | 14 | 11 | 3 | 166 | 0 |
| CTaskSimpleStealthKill | 11 | 8 | 3 | 169 | 1 |
| CTaskComplexFollowPedFootsteps | 8 | 5 | 3 | 185 | 0 |
| CCutsceneMgr | 38 | 35 | 3 | 204 | 0 |
| CMatrixLinkList | 13 | 10 | 3 | 208 | 0 |
| CTaskComplexWanderProstitute | 8 | 5 | 3 | 223 | 2 |
| VideoPlayer | 9 | 6 | 3 | 230 | 0 |
| CTaskComplexGangLeader | 16 | 13 | 3 | 231 | 2 |
| CAnimBlendHierarchy | 19 | 16 | 3 | 244 | 0 |
| CFont | 41 | 37 | 3 | 251 | 3 |
| CGarages | 33 | 30 | 3 | 253 | 2 |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | 9 | 6 | 3 | 304 | 0 |
| FurnitureManager_c | 10 | 7 | 3 | 306 | 0 |
| CMissionTextOffsets | 4 | 1 | 3 | 335 | 0 |
| CPlane | 26 | 23 | 3 | 346 | 2 |
| CDecisionMakerTypes | 13 | 10 | 3 | 347 | 0 |
| CTaskComplexGangFollower | 11 | 8 | 3 | 365 | 2 |
| CPedClothesDesc | 10 | 7 | 3 | 379 | 0 |
| CVehicleRecording | 34 | 31 | 3 | 417 | 0 |
| CTaskComplexBeInCouple | 14 | 11 | 3 | 425 | 0 |
| Game | 15 | 12 | 3 | 460 | 2 |
| CPlantMgr | 23 | 20 | 3 | 484 | 1 |
| CTaskSimpleDuckToggle | 9 | 6 | 3 | 488 | 0 |
| CPlayerPed | 68 | 56 | 3 | 496 | 2 |
| CTaskComplexPartner | 10 | 7 | 3 | 697 | 0 |
| CAttractorScanner | 4 | 1 | 3 | 1,273 | 0 |
| CTaskInteriorUseInfo | 11 | 8 | 3 | 1,483 | 0 |
| CEventAcquaintancePedHate | 5 | 1 | 4 | 18 | 0 |
| CEventInAir | 6 | 2 | 4 | 22 | 0 |
| RwHelper | 35 | 31 | 4 | 53 | 0 |
| CTaskComplexSequence | 16 | 12 | 4 | 65 | 1 |
| CEventVehicleDamageCollision | 6 | 2 | 4 | 67 | 2 |
| CTaskComplexInAirAndLand | 8 | 4 | 4 | 77 | 0 |
| CTaskSimplePause | 8 | 4 | 4 | 77 | 1 |
| CEventCarUpsideDown | 6 | 2 | 4 | 96 | 4 |
| CEventPedCollisionWithPed | 7 | 3 | 4 | 96 | 4 |
| CEventVehicleToSteal | 6 | 2 | 4 | 96 | 4 |
| CTaskComplexPartnerShove | 5 | 1 | 4 | 101 | 0 |
| CTaskSimpleFacial | 9 | 5 | 4 | 101 | 1 |
| CEventCopCarBeingStolen | 6 | 2 | 4 | 112 | 4 |
| CTaskSimpleHandsUp | 8 | 4 | 4 | 113 | 1 |
| CTaskComplexLeaveCarAndDie | 7 | 3 | 4 | 120 | 0 |
| CTaskSimpleGoToPoint | 10 | 6 | 4 | 120 | 1 |
| CNodeRoutePool | 5 | 1 | 4 | 122 | 0 |
| CTaskSimpleDie | 11 | 7 | 4 | 159 | 2 |
| CTaskSimpleGetUp | 9 | 5 | 4 | 159 | 2 |
| CTaskSimpleLand | 10 | 6 | 4 | 159 | 2 |
| CTaskComplexWaitForBus | 11 | 7 | 4 | 160 | 0 |
| CBaseModelInfo | 38 | 34 | 4 | 165 | 2 |
| CAtomicModelInfo | 13 | 9 | 4 | 167 | 2 |
| CEventPedCollisionWithPlayer | 5 | 1 | 4 | 167 | 0 |
| CEventPlayerCollisionWithPed | 5 | 1 | 4 | 167 | 0 |
| CAnimBlendAssociation | 24 | 20 | 4 | 177 | 1 |
| CTaskComplexSeekEntityStandard | 13 | 9 | 4 | 197 | 1 |
| CSetPiece | 7 | 3 | 4 | 200 | 4 |
| CAECutsceneTrackManager | 12 | 8 | 4 | 219 | 3 |
| CAEDataStream | 23 | 19 | 4 | 224 | 3 |
| CAESoundManager | 18 | 14 | 4 | 244 | 0 |
| CTaskComplexGoToBoatSteeringWheel | 10 | 6 | 4 | 249 | 1 |
| CTaskComplexCopInCar | 13 | 9 | 4 | 252 | 0 |
| CColModel | 13 | 9 | 4 | 253 | 0 |
| CClumpModelInfo | 26 | 22 | 4 | 255 | 2 |
| CAEWeatherAudioEntity | 9 | 5 | 4 | 264 | 0 |
| CStuntJumpManager | 11 | 7 | 4 | 296 | 4 |
| CTaskInteriorSitInChair | 5 | 1 | 4 | 304 | 0 |
| CTaskComplexHitPedWithCar | 14 | 10 | 4 | 319 | 1 |
| CPedDamageResponseCalculator | 10 | 6 | 4 | 320 | 1 |
| CTaskComplexOpenPassengerDoor | 8 | 4 | 4 | 327 | 0 |
| CPedGeometryAnalyser | 42 | 38 | 4 | 338 | 0 |
| CTaskGoToVehicleAndLean | 11 | 7 | 4 | 369 | 1 |
| CHandObject | 7 | 3 | 4 | 386 | 0 |
| CTaskComplexWalkRoundBuildingAttempt | 19 | 15 | 4 | 421 | 1 |
| CTaskComplexWalkRoundObject | 6 | 2 | 4 | 460 | 0 |
| ProcSurfaceInfo_c | 8 | 4 | 4 | 489 | 0 |
| COnscreenCounterEntry | 6 | 2 | 4 | 538 | 0 |
| CTaskComplexWander | 18 | 14 | 4 | 797 | 2 |
| CVehicleSaveStructure | 6 | 2 | 4 | 874 | 0 |
| CTaskComplexSeekEntityAnyMeansXYOffset | 9 | 5 | 4 | 1,138 | 0 |
| Interior_c | 56 | 52 | 4 | 1,200 | 1 |
| CRunningScript | 43 | 39 | 4 | 5,919 | 0 |
| CEventGunShotWhizzedBy | 7 | 2 | 5 | 29 | 0 |
| CMemoryMgr | 10 | 5 | 5 | 31 | 4 |
| FxManager_c | 30 | 25 | 5 | 55 | 3 |
| FxPrimBP_c | 7 | 2 | 5 | 70 | 0 |
| CEventPlayerCommandToGroup | 7 | 2 | 5 | 98 | 4 |
| CdStream | 16 | 11 | 5 | 99 | 1 |
| CEventPotentialWalkIntoPed | 8 | 3 | 5 | 100 | 5 |
| CEventSeenPanickedPed | 7 | 2 | 5 | 100 | 0 |
| CGridRef | 10 | 5 | 5 | 100 | 0 |
| CEventPotentialWalkIntoVehicle | 7 | 2 | 5 | 101 | 4 |
| CEventVehicleDied | 8 | 3 | 5 | 101 | 4 |
| CTaskSimpleCower | 9 | 4 | 5 | 112 | 1 |
| CAEVehicleAudioEntity | 111 | 106 | 5 | 121 | 2 |
| CEventDanger | 9 | 4 | 5 | 124 | 4 |
| CTaskGangHasslePed | 10 | 5 | 5 | 126 | 1 |
| CEventAreaCodes | 8 | 3 | 5 | 137 | 3 |
| CTaskComplexFleePoint | 16 | 11 | 5 | 150 | 0 |
| CTaskComplexSitDownThenIdleThenStandUp | 14 | 9 | 5 | 150 | 0 |
| CTaskSimpleIKChain | 14 | 9 | 5 | 150 | 0 |
| CTaskComplexGoToAttractor | 10 | 5 | 5 | 162 | 2 |
| CTaskSimpleGoToPointFine | 13 | 8 | 5 | 165 | 2 |
| CTaskComplexJump | 11 | 6 | 5 | 175 | 2 |
| CLoadingScreen | 25 | 20 | 5 | 180 | 0 |
| CTaskComplexReactToGunAimedAt | 13 | 8 | 5 | 188 | 0 |
| CDamageAtomicModelInfo | 9 | 4 | 5 | 210 | 3 |
| CMenuManager | 66 | 61 | 5 | 214 | 5 |
| CHeli | 26 | 18 | 5 | 219 | 4 |
| CAEAmbienceTrackManager | 12 | 7 | 5 | 386 | 3 |
| CTaskSimpleCarFallOut | 14 | 9 | 5 | 388 | 1 |
| CTaskSimpleDieInCar | 10 | 5 | 5 | 403 | 0 |
| CTaskSimpleSwim | 16 | 11 | 5 | 406 | 2 |
| CTheZones | 27 | 22 | 5 | 422 | 1 |
| CPedIntelligence | 58 | 53 | 5 | 498 | 1 |
| CStencilShadows | 18 | 13 | 5 | 543 | 2 |
| Rs | 35 | 30 | 5 | 696 | 4 |
| CTrafficLights | 17 | 12 | 5 | 770 | 0 |
| CCarCtrl | 109 | 94 | 5 | 2,481 | 4 |
| CScriptsForBrains | 13 | 8 | 5 | 6,352 | 4 |
| CTaskComplexSeekEntityRadiusAngleOffset | 15 | 10 | 5 | 8,771 | 1 |
| CRect | 14 | 8 | 6 | 113 | 0 |
| App | 13 | 7 | 6 | 116 | 0 |
| FxSystem_c | 41 | 35 | 6 | 147 | 0 |
| CEventPlayerCommandToGroupAttack | 8 | 2 | 6 | 151 | 0 |
| CFileLoader | 54 | 48 | 6 | 158 | 5 |
| CPlayerRelationshipRecorder | 9 | 3 | 6 | 185 | 0 |
| CEventStuckInAir | 10 | 4 | 6 | 201 | 4 |
| CEventObjectCollision | 8 | 2 | 6 | 211 | 4 |
| CEventDontJoinPlayerGroup | 7 | 1 | 6 | 230 | 0 |
| CEventNewGangMember | 7 | 1 | 6 | 230 | 0 |
| CEventLeanOnVehicle | 8 | 2 | 6 | 234 | 0 |
| CTaskSimpleRunNamedAnim | 13 | 7 | 6 | 235 | 0 |
| CTaskSimpleWaitForBus | 12 | 6 | 6 | 239 | 0 |
| cHandlingDataMgr | 17 | 11 | 6 | 299 | 1 |
| CTaskComplexGotoDoorAndOpen | 14 | 8 | 6 | 388 | 2 |
| CTxdStore | 32 | 26 | 6 | 388 | 6 |
| CColStore | 29 | 23 | 6 | 443 | 5 |
| CTaskComplexSeekEntityAiming | 14 | 8 | 6 | 451 | 5 |
| CObjectPool | 9 | 3 | 6 | 457 | 0 |
| CTaskSimpleIKPointArm | 12 | 6 | 6 | 516 | 0 |
| CAudioZones | 11 | 5 | 6 | 522 | 0 |
| CShopping | 36 | 30 | 6 | 549 | 3 |
| D3DResourceSystem | 11 | 5 | 6 | 566 | 0 |
| CTaskComplexWaitForPed | 7 | 1 | 6 | 772 | 0 |
| Ps | 29 | 23 | 6 | 876 | 4 |
| CTaskComplexSunbathe | 15 | 9 | 6 | 935 | 0 |
| CTrain | 47 | 41 | 6 | 1,076 | 6 |
| CTaskComplexWanderGang | 10 | 4 | 6 | 1,750 | 0 |
| CTaskComplexGoToPointAnyMeans | 13 | 7 | 6 | 2,633 | 1 |
| CWaterLevel | 48 | 40 | 6 | 4,137 | 6 |
| CBridge | 11 | 4 | 7 | 35 | 0 |
| CEventLeaderExitedCarAsDriver | 9 | 2 | 7 | 141 | 0 |
| CEventPotentialWalkIntoObject | 9 | 2 | 7 | 197 | 0 |
| CTaskComplexEnterCar | 22 | 15 | 7 | 199 | 2 |
| CTaskComplexDriveFireTruck | 13 | 6 | 7 | 202 | 0 |
| CVisibilityPlugins | 91 | 84 | 7 | 205 | 4 |
| CTaskSimpleStandStill | 10 | 3 | 7 | 252 | 2 |
| CTaskSimpleCarJumpOut | 21 | 14 | 7 | 274 | 1 |
| CCarAI | 33 | 26 | 7 | 289 | 2 |
| CEventVehicleDamage | 12 | 5 | 7 | 303 | 4 |
| CTask | 11 | 4 | 7 | 336 | 0 |
| CPtrNodeDoubleLinkPool | 8 | 1 | 7 | 436 | 0 |
| CTaskComplexStealCar | 22 | 15 | 7 | 495 | 0 |
| CTaskSimpleFight | 18 | 11 | 7 | 2,403 | 1 |
| CEventGunShot | 13 | 5 | 8 | 127 | 7 |
| CEventLeaderQuitEnteringCarAsDriver | 10 | 2 | 8 | 146 | 0 |
| CTaskSimpleHoldEntity | 23 | 15 | 8 | 302 | 2 |
| CCopPed | 15 | 7 | 8 | 335 | 0 |
| CEventAcquaintancePedHateBadlyLit | 10 | 2 | 8 | 342 | 0 |
| CAnimManager | 39 | 31 | 8 | 579 | 3 |
| CTaskSimpleBeHit | 16 | 8 | 8 | 1,022 | 0 |
| CTaskComplexSeekCoverUntilTargetDead | 20 | 12 | 8 | 2,642 | 0 |
| CPedIntelligencePool | 10 | 2 | 8 | 3,558 | 0 |
| CEventLeaderEntryExit | 10 | 1 | 9 | 153 | 0 |
| CAEAudioEntity | 11 | 2 | 9 | 176 | 0 |
| CEventHitByWaterCannon | 11 | 2 | 9 | 234 | 5 |
| CEventLeaderEnteredCarAsDriver | 11 | 2 | 9 | 244 | 0 |
| CEvent | 12 | 3 | 9 | 287 | 5 |
| CTaskComplexWalkRoundFire | 17 | 8 | 9 | 385 | 0 |
| CEventBuildingCollision | 13 | 4 | 9 | 399 | 6 |
| CEventPlayerCommandToGroupGather | 10 | 1 | 9 | 405 | 0 |
| CTaskAllocatorPool | 10 | 1 | 9 | 414 | 0 |
| CTaskComplexWanderCop | 18 | 9 | 9 | 442 | 0 |
| CTaskComplexPartnerGreet | 17 | 8 | 9 | 458 | 0 |
| CEventAttractor | 13 | 4 | 9 | 571 | 4 |
| CTaskComplexRoadRage | 18 | 9 | 9 | 585 | 0 |
| Common | 20 | 11 | 9 | 603 | 7 |
| CTaskComplexGoToPointAndStandStill | 17 | 8 | 9 | 991 | 3 |
| CCompressedMatrixNotAligned | 16 | 7 | 9 | 1,073 | 0 |
| CTaskComplexInvestigateDisturbance | 17 | 8 | 9 | 1,125 | 0 |
| CObject | 60 | 51 | 9 | 1,171 | 3 |
| CPed | 158 | 149 | 9 | 1,279 | 5 |
| CTaskSimpleUninterruptable | 11 | 1 | 10 | 165 | 0 |
| CEventPassObject | 12 | 2 | 10 | 291 | 0 |
| CColSphere | 15 | 5 | 10 | 369 | 9 |
| CPopulation | 76 | 66 | 10 | 532 | 0 |
| CTaskSimpleDuck | 25 | 15 | 10 | 635 | 1 |
| CEventKnockOffBike | 18 | 8 | 10 | 675 | 5 |
| CQuadTreeNode<T> | 22 | 12 | 10 | 720 | 6 |
| CQuaternion | 13 | 3 | 10 | 1,009 | 0 |
| CTaskComplexAvoidOtherPedWhileWandering | 25 | 15 | 10 | 1,382 | 0 |
| CReplay | 57 | 47 | 10 | 4,172 | 10 |
| Fx_c | 37 | 24 | 11 | 448 | 6 |
| CEntryExitManager | 29 | 18 | 11 | 675 | 9 |
| CEventScriptCommand | 20 | 9 | 11 | 766 | 1 |
| CTaskSimpleThrowProjectile | 25 | 14 | 11 | 767 | 0 |
| CTaskComplexStuckInAir | 17 | 6 | 11 | 1,075 | 1 |
| CEventPotentialWalkIntoFire | 14 | 2 | 12 | 514 | 0 |
| CPedAttractorPool | 13 | 1 | 12 | 530 | 0 |
| CCam | 70 | 55 | 12 | 1,409 | 5 |
| CGarage | 24 | 12 | 12 | 2,528 | 10 |
| CEntityScanner | 18 | 6 | 12 | 2,673 | 0 |
| CAutomobile | 113 | 101 | 12 | 4,625 | 12 |
| CEventScriptedAttractor | 14 | 1 | 13 | 527 | 0 |
| CTaskComplexFallAndGetUp | 22 | 9 | 13 | 719 | 2 |
| CTaskComplexMedicTreatInjuredPed | 22 | 9 | 13 | 974 | 2 |
| CPad | 93 | 80 | 13 | 993 | 5 |
| CTaskSimpleRunTimedAnim | 20 | 7 | 13 | 1,251 | 0 |
| CClothesBuilder | 36 | 23 | 13 | 1,285 | 3 |
| CTaskSimpleThrowControl | 20 | 6 | 14 | 1,802 | 0 |
| CTaskComplexTurnToFaceEntityOrCoord | 23 | 9 | 14 | 2,607 | 0 |
| CTaskComplexDieInCar | 21 | 6 | 15 | 406 | 1 |
| CEventEditableResponse | 26 | 11 | 15 | 614 | 1 |
| CTaskComplexUseWaterCannon | 20 | 5 | 15 | 769 | 2 |
| CGame | 44 | 29 | 15 | 1,037 | 3 |
| CVehicleModelInfo | 82 | 67 | 15 | 1,356 | 13 |
| CTaskComplexUseClosestFreeScriptedAttractor | 23 | 8 | 15 | 1,904 | 0 |
| CTheScripts | 87 | 72 | 15 | 10,081 | 7 |
| CEventVehicleCollision | 19 | 3 | 16 | 1,230 | 0 |
| CCullZones | 38 | 22 | 16 | 2,899 | 1 |
| RpAnimBlend | 59 | 43 | 16 | 6,102 | 16 |
| CEventVehicleOnFire | 21 | 3 | 18 | 1,226 | 0 |
| CStreaming | 113 | 94 | 19 | 598 | 15 |
| CPathFind | 87 | 67 | 19 | 4,015 | 16 |
| CTaskSimpleHitFromFront | 24 | 3 | 21 | 430 | 0 |
| CBike | 62 | 40 | 22 | 168 | 22 |
| CModelInfo | 51 | 29 | 22 | 814 | 19 |
| CCustomCarEnvMapPipeline | 62 | 37 | 25 | 3,547 | 5 |
| CMaths | 30 | 3 | 27 | 3,109 | 0 |
| CPlaceable | 50 | 20 | 28 | 1,336 | 3 |
| CPedAttractor | 48 | 20 | 28 | 1,774 | 13 |
| CEventVehicleHitAndRun | 33 | 3 | 30 | 2,237 | 0 |
| CAEUserRadioTrackManager | 46 | 16 | 30 | 2,533 | 28 |
| CEventDamage | 53 | 20 | 33 | 2,525 | 2 |
| CVehicle | 200 | 161 | 34 | 897 | 33 |
| CEventSeenCop | 36 | 1 | 35 | 902 | 0 |
| CWeaponModelInfo | 45 | 4 | 39 | 12,299 | 0 |
| CEntity | 113 | 69 | 44 | 10,348 | 6 |
| CPools | 71 | 19 | 52 | 3,733 | 51 |
| Win | 109 | 20 | 69 | 14,247 | 0 |
| CPedAttractorManager | 85 | 14 | 71 | 8,097 | 0 |
| FxInfoManager_c | 124 | 5 | 119 | 6,656 | 96 |
| FxEmitterBP_c | 155 | 14 | 141 | 28,395 | 9 |

542 de 782 módulos tienen algo pendiente.

## Cómo usarlo

- Filtra `funciones.csv` por `estado` = `A` y ordena por `llamadas_desde_reemplazadas` para empezar por lo más usado por código ya reconstruido.
- Una función de 16 bytes o menos suele ser un stub o thunk; mira `thunk` antes de tratarla como trabajo.
- Los grupos C y D pueden contener código muerto: confirma con análisis dinámico antes de reconstruir.
