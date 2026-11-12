<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 892 grupos / 10295 funciones reconstruidas; 10295 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Código del juego en el EXE: mapa de las 11,779 funciones

[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)

Región **0x401000–0x74FFFF**: 11,779 funciones, 3,005,739 bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera (se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 329 | 2.8% | 55,610 | 1.9% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 339 | 2.9% | 58,197 | 1.9% |
| B · Falta: método virtual | 386 | 3.3% | 22,207 | 0.7% |
| A · Falta: llamada desde código ya reemplazado | 516 | 4.4% | 84,517 | 2.8% |
| Envoltura pendiente (la DLL aún llama al EXE) | 22 | 0.2% | 7,112 | 0.2% |
| Reemplazada por C++ | 10,187 | 86.5% | 2,778,096 | 92.4% |

Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque hay 6,004 llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: [A](pendientes-A.md) · [B](pendientes-B.md) · [C](pendientes-C.md) · [D](pendientes-D.md).

## Por bloque de direcciones (64 KB)

Ordenados de mayor a menor porcentaje reemplazado.

El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.

## Excluidas del objetivo

25 función(es) del rango del juego no se cuentan en las cifras de arriba porque no son viables en C++ puro (lista en [excluidas.csv](excluidas.csv)):

| Dirección | Bytes | Módulo estimado | Motivo |
| --- | ---: | ---: | ---: |
| 0x0049EA70 | 5 | Fx_c | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x004AFB40 | 5 | CEventAcquaintancePed | no es una funcion con trabajo propio: un unico jmp a CEventAcquaintancePed::AffectsPed (0x4AFA30), sin llamadores ni referencias de datos; excluida el 12/11/2026 (lote 178A) |
| 0x004B5C2A | 197 | CPedDamageResponseCalculator | no es una funcion: bloque interior de CPedDamageResponseCalculator/ComputeDamageResponse (0x4B5AC0), sin prologo; excluida el 12/11/2026 (lote 176A) |
| 0x004C4923 | 48 | CDamageAtomicModelInfo | no es una funcion: fragmento interno de CDamageAtomicModelInfo::CreateInstance (0x4C4910), que ya tiene hook (lote 173A); artefacto del mapa; excluida el 12/11/2026 |
| 0x0059F000 | 5 | CHandObject | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x005DBE40 | 104 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD1E0 | 57 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD220 | 323 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00637320 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637350 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637380 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373B0 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373E0 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637410 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637440 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637470 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655DD0 | 5 | CTaskComplexWalkRoundFire | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655E00 | 5 | CTaskComplexWalkRoundFire | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935B0 | 5 | CTaskSimpleHoldEntity | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935E0 | 5 | CTaskSimpleHoldEntity | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006F55C0 | 5 | CTrain | no es una funcion con trabajo propio: un unico jmp a CVehicle::Render (0x6D0E60), ranura 18 de la vtable 0x872370 de CTrain; sin logica que reconstruir; excluida el 12/11/2026 (lote 174A) |
| 0x00709CF0 | 837 | CShadows | __usercall (EAX=normal, EBX=triangulo, EDI=parametros): C++ no puede engancharla sin __asm; ya reconstruida en source/game_sa/Shadows.cpp pero sin hook; excluida del objetivo el 12/11/2026 |
| 0x00745CC0 | 252 | VideoMode | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00745DC0 | 130 | VideoMode | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00746800 | 110 | VideoPlayer | __usercall anti-manipulacion: lee registros que C++ no puede leer; sin llamadores directos ni vtable; excluida del objetivo el 11/11/2026 (lote 161A, evidencia en reversing/Fn161A) |

| Bloque | Funciones | Reemplazadas | Faltan | Bytes que faltan | Módulos con más faltantes (estimado) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0x4E0000 | 246 | 100% | 0 | 0 | — |
| 0x520000 | 62 | 100% | 0 | 0 | — |
| 0x560000 | 212 | 100% | 0 | 0 | — |
| 0x580000 | 129 | 100% | 0 | 0 | — |
| 0x700000 | 137 | 99% | 1 | 107 | CStencilShadows (1) |
| 0x500000 | 305 | 99% | 3 | 615 | CAEWaterCannonAudioEntity (1) |
| 0x6C0000 | 135 | 98% | 3 | 346 | CPlane (2) |
| 0x6B0000 | 83 | 98% | 2 | 1,048 | CAutomobile (2) |
| 0x710000 | 158 | 97% | 4 | 436 | CStencilShadows (1) |
| 0x570000 | 110 | 97% | 3 | 119 | CTheZones (1) |
| 0x640000 | 425 | 97% | 14 | 1,079 | CTaskSimpleCarFallOut (1) |
| 0x6F0000 | 181 | 97% | 6 | 1,076 | CTrain (6) |
| 0x730000 | 278 | 96% | 10 | 600 | CTxdStore (6) |
| 0x530000 | 275 | 96% | 12 | 853 | CEntity (5), CGame (3), Game (2) |
| 0x480000 | 111 | 95% | 5 | 14,797 | CTheScripts (2) |
| 0x410000 | 162 | 95% | 8 | 422 | CColStore (5) |
| 0x430000 | 200 | 95% | 10 | 603 | CEntryExitManager (9) |
| 0x5B0000 | 157 | 94% | 10 | 523 | — |
| 0x6E0000 | 92 | 93% | 6 | 593 | CVehicle (3), CWaterLevel (1) |
| 0x590000 | 254 | 93% | 17 | 3,122 | InteriorManager_c (2), CObject (2), Interior_c (1) |
| 0x6A0000 | 112 | 93% | 8 | 1,869 | CAutomobile (8) |
| 0x680000 | 184 | 92% | 15 | 1,230 | — |
| 0x510000 | 61 | 92% | 5 | 794 | CCam (2) |
| 0x4D0000 | 319 | 92% | 27 | 6,922 | RpAnimBlend (16), CAEAmbienceTrackManager (3), CAnimManager (2) |
| 0x440000 | 194 | 91% | 17 | 2,367 | CPathFind (10) |
| 0x720000 | 203 | 91% | 18 | 3,026 | CWeather (2), CCullZones (1) |
| 0x610000 | 327 | 91% | 29 | 3,568 | Rs (4) |
| 0x420000 | 135 | 91% | 12 | 2,079 | CCarCtrl (3) |
| 0x600000 | 237 | 91% | 22 | 4,054 | CPlayerPed (2) |
| 0x630000 | 577 | 91% | 54 | 5,130 | — |
| 0x450000 | 172 | 90% | 17 | 4,918 | CReplay (10), CPathFind (1) |
| 0x690000 | 308 | 90% | 31 | 2,739 | CTaskComplexSeekEntityAiming (5), CTaskSimpleHoldEntity (2) |
| 0x650000 | 385 | 88% | 45 | 3,193 | CTaskComplexMedicTreatInjuredPed (2), CTaskComplexUseWaterCannon (2), CTaskComplexGangFollower (2) |
| 0x460000 | 164 | 87% | 21 | 7,205 | CScriptsForBrains (4), CTheScripts (1) |
| 0x470000 | 46 | 87% | 6 | 220 | — |
| 0x5D0000 | 332 | 87% | 44 | 5,397 | CCustomCarEnvMapPipeline (5) |
| 0x620000 | 282 | 86% | 39 | 2,753 | — |
| 0x490000 | 152 | 86% | 22 | 10,695 | CTheScripts (3), CShopping (3), Fx_c (3) |
| 0x4F0000 | 203 | 85% | 30 | 2,533 | CAEUserRadioTrackManager (28) |
| 0x660000 | 300 | 85% | 45 | 6,337 | CTaskComplexGoToPointAndStandStill (3), CTaskComplexGotoDoorAndOpen (2), CTaskGoToVehicleAndLean (1) |
| 0x540000 | 176 | 85% | 27 | 1,314 | CPlaceable (3), CPad (2) |
| 0x6D0000 | 179 | 84% | 29 | 470 | CVehicle (29) |
| 0x400000 | 281 | 83% | 47 | 9,569 | CStreaming (14) |
| 0x670000 | 290 | 82% | 53 | 9,431 | CTaskComplexFallAndGetUp (2), CTaskComplexWander (1), CTaskComplexStuckInAir (1) |
| 0x5A0000 | 116 | 80% | 23 | 2,314 | CClothesBuilder (3), CObject (1) |
| 0x5F0000 | 292 | 79% | 60 | 2,518 | — |
| 0x4C0000 | 370 | 79% | 76 | 14,469 | CModelInfo (19), CVehicleModelInfo (13) |
| 0x4B0000 | 359 | 75% | 89 | 6,979 | CEventScriptCommand (1) |
| 0x550000 | 257 | 74% | 66 | 6,947 | CPools (51), CQuadTreeNode<T> (6), SurfaceInfos_c (2) |
| 0x4A0000 | 469 | 66% | 158 | 7,943 | FxInfoManager_c (96), FxEmitterBP_c (9), CEventKnockOffBike (4) |
| 0x5E0000 | 254 | 54% | 117 | 11,447 | CPedAttractor (13), CPed (4) |
| 0x740000 | 192 | 49% | 77 | 15,888 | Ps (4), Input (2) |
| 0x5C0000 | 139 | 9% | 127 | 27,874 | — |

## Por módulo (estimado)

Módulos con alguna función por reconstruir, de menos a más funciones que faltan. Para funciones sin nombre el módulo es la heurística de vecindad descrita arriba; la columna «Confianza alta» cuenta las faltantes cuyo módulo coincide en ambos vecinos.

| Módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan | Confianza alta |
| --- | ---: | ---: | ---: | ---: | ---: |
| CPedDamageResponseCalculator | 9 | 7 | 2 | 122 | 0 |
| CAudioZones | 11 | 10 | 1 | 139 | 0 |
| IKChain_c | 22 | 20 | 2 | 205 | 0 |
| CWaterLevel | 48 | 47 | 1 | 208 | 1 |
| WaterCreature_c | 5 | 3 | 2 | 211 | 0 |
| CTaskSimpleHoldEntity | 21 | 15 | 6 | 292 | 2 |
| Input | 7 | 5 | 2 | 302 | 2 |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | 9 | 6 | 3 | 304 | 0 |
| CTaskInteriorSitInChair | 5 | 1 | 4 | 304 | 0 |
| FurnitureManager_c | 10 | 7 | 3 | 306 | 0 |
| CAEWaterCannonAudioEntity | 9 | 7 | 2 | 309 | 1 |
| CTaskComplexHitPedWithCar | 14 | 10 | 4 | 319 | 1 |
| CStats | 53 | 51 | 2 | 320 | 1 |
| CTaskComplexOpenPassengerDoor | 8 | 4 | 4 | 327 | 0 |
| CTaskGoToVehicleAndLean | 11 | 8 | 3 | 331 | 1 |
| CTaskComplexDieInCar | 21 | 8 | 13 | 333 | 0 |
| CCopPed | 15 | 7 | 8 | 335 | 0 |
| CEventAcquaintancePedHateBadlyLit | 9 | 2 | 7 | 337 | 0 |
| CPedGeometryAnalyser | 42 | 38 | 4 | 338 | 0 |
| InteriorManager_c | 23 | 21 | 2 | 340 | 2 |
| CQuaternion | 13 | 10 | 3 | 341 | 0 |
| CPlane | 26 | 23 | 3 | 346 | 2 |
| CWeather | 15 | 13 | 2 | 352 | 2 |
| CTaskAllocatorPool | 10 | 3 | 7 | 362 | 0 |
| CTaskComplexGangFollower | 11 | 8 | 3 | 365 | 2 |
| CEventPlayerCommandToGroupGather | 10 | 2 | 8 | 372 | 0 |
| CColStore | 29 | 24 | 5 | 373 | 5 |
| CTaskComplexWalkRoundFire | 15 | 8 | 7 | 375 | 0 |
| CHandObject | 6 | 3 | 3 | 381 | 0 |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | 9 | 7 | 2 | 382 | 1 |
| CAEAmbienceTrackManager | 12 | 7 | 5 | 386 | 3 |
| CTaskSimpleCarFallOut | 14 | 9 | 5 | 388 | 1 |
| CTaskComplexGotoDoorAndOpen | 14 | 8 | 6 | 388 | 2 |
| CTxdStore | 32 | 26 | 6 | 388 | 6 |
| CTaskSimpleHitFromFront | 16 | 3 | 13 | 390 | 0 |
| CEventKnockOffBike | 18 | 11 | 7 | 395 | 4 |
| CPopulation | 76 | 68 | 8 | 399 | 0 |
| CTaskSimpleDieInCar | 10 | 5 | 5 | 403 | 0 |
| CVehicleRecording | 34 | 31 | 3 | 417 | 0 |
| CTaskComplexWalkRoundBuildingAttempt | 19 | 15 | 4 | 421 | 1 |
| CTheZones | 27 | 22 | 5 | 422 | 1 |
| CTaskSimpleIKPointArm | 12 | 7 | 5 | 426 | 0 |
| CGame | 44 | 35 | 9 | 428 | 3 |
| Fx_c | 36 | 28 | 8 | 429 | 3 |
| CAnimManager | 39 | 33 | 6 | 434 | 2 |
| CEventPotentialWalkIntoFire | 14 | 5 | 9 | 440 | 0 |
| CTaskComplexWanderCop | 18 | 9 | 9 | 442 | 0 |
| CObjectPool | 9 | 4 | 5 | 446 | 0 |
| COnscreenCounterEntry | 6 | 3 | 3 | 450 | 0 |
| CTaskComplexSeekEntityAiming | 14 | 8 | 6 | 451 | 5 |
| CTaskComplexPartnerGreet | 17 | 8 | 9 | 458 | 0 |
| Game | 15 | 12 | 3 | 460 | 2 |
| CShopping | 36 | 31 | 5 | 460 | 3 |
| CTaskComplexStealCar | 22 | 16 | 6 | 461 | 0 |
| CTaskSimpleDuckToggle | 9 | 6 | 3 | 488 | 0 |
| ProcSurfaceInfo_c | 8 | 4 | 4 | 489 | 0 |
| CEventAttractor | 13 | 5 | 8 | 490 | 3 |
| CPlayerPed | 68 | 65 | 3 | 496 | 2 |
| CTaskSimpleDuck | 25 | 17 | 8 | 496 | 0 |
| CEventScriptedAttractor | 14 | 1 | 13 | 527 | 0 |
| CPedAttractorPool | 13 | 1 | 12 | 530 | 0 |
| CStreaming | 113 | 96 | 17 | 535 | 14 |
| CStencilShadows | 18 | 13 | 5 | 543 | 2 |
| CTaskComplexRoadRage | 18 | 9 | 9 | 585 | 0 |
| CEventEditableResponse | 26 | 12 | 14 | 607 | 0 |
| Rs | 35 | 31 | 4 | 661 | 4 |
| CEntryExitManager | 29 | 18 | 11 | 675 | 9 |
| CTaskComplexFallAndGetUp | 22 | 9 | 13 | 719 | 2 |
| CQuadTreeNode<T> | 22 | 12 | 10 | 720 | 6 |
| CObject | 60 | 53 | 7 | 728 | 3 |
| CEventScriptCommand | 20 | 9 | 11 | 766 | 1 |
| CTaskSimpleThrowProjectile | 25 | 14 | 11 | 767 | 0 |
| CTaskComplexUseWaterCannon | 20 | 5 | 15 | 769 | 2 |
| CTrafficLights | 17 | 12 | 5 | 770 | 0 |
| CTaskComplexWaitForPed | 7 | 1 | 6 | 772 | 0 |
| CTaskComplexWander | 18 | 14 | 4 | 797 | 2 |
| CModelInfo | 51 | 29 | 22 | 814 | 19 |
| CTaskComplexInvestigateDisturbance | 17 | 10 | 7 | 858 | 0 |
| CVehicle | 200 | 167 | 33 | 861 | 32 |
| CVehicleSaveStructure | 6 | 2 | 4 | 874 | 0 |
| Ps | 29 | 23 | 6 | 876 | 4 |
| CPad | 93 | 82 | 11 | 895 | 3 |
| CEventSeenCop | 36 | 1 | 35 | 902 | 0 |
| CTaskComplexSunbathe | 15 | 9 | 6 | 935 | 0 |
| CEntityScanner | 18 | 13 | 5 | 958 | 0 |
| CTaskComplexMedicTreatInjuredPed | 22 | 9 | 13 | 974 | 2 |
| CTaskComplexGoToPointAndStandStill | 17 | 9 | 8 | 977 | 3 |
| CTaskSimpleBeHit | 16 | 8 | 8 | 1,022 | 0 |
| CEventVehicleHitAndRun | 33 | 18 | 15 | 1,037 | 0 |
| CPed | 158 | 152 | 6 | 1,046 | 4 |
| CCam | 70 | 63 | 7 | 1,059 | 2 |
| CTaskComplexStuckInAir | 17 | 6 | 11 | 1,075 | 1 |
| CTrain | 47 | 41 | 6 | 1,076 | 6 |
| CTaskComplexSeekEntityAnyMeansXYOffset | 9 | 5 | 4 | 1,138 | 0 |
| CPlaceable | 50 | 26 | 24 | 1,138 | 3 |
| CEventVehicleOnFire | 21 | 4 | 17 | 1,145 | 0 |
| CEventVehicleCollision | 19 | 4 | 15 | 1,149 | 0 |
| CTaskSimpleRunTimedAnim | 20 | 8 | 12 | 1,175 | 0 |
| Interior_c | 56 | 52 | 4 | 1,200 | 1 |
| CAttractorScanner | 4 | 1 | 3 | 1,273 | 0 |
| CClothesBuilder | 36 | 23 | 13 | 1,285 | 3 |
| CVehicleModelInfo | 82 | 67 | 15 | 1,356 | 13 |
| CTaskComplexAvoidOtherPedWhileWandering | 25 | 15 | 10 | 1,382 | 0 |
| CTaskInteriorUseInfo | 11 | 8 | 3 | 1,483 | 0 |
| CPedIntelligencePool | 10 | 4 | 6 | 1,552 | 0 |
| CEventDamage | 53 | 31 | 22 | 1,596 | 0 |
| CTaskComplexWanderGang | 10 | 4 | 6 | 1,750 | 0 |
| CPedAttractor | 48 | 20 | 28 | 1,774 | 13 |
| CTaskSimpleThrowControl | 20 | 6 | 14 | 1,802 | 0 |
| CTaskComplexUseClosestFreeScriptedAttractor | 23 | 10 | 13 | 1,858 | 0 |
| CCarCtrl | 109 | 105 | 4 | 1,928 | 3 |
| CPathFind | 87 | 73 | 14 | 2,141 | 11 |
| SurfaceInfos_c | 49 | 47 | 2 | 2,174 | 2 |
| CCullZones | 38 | 25 | 13 | 2,476 | 1 |
| CAEUserRadioTrackManager | 46 | 16 | 30 | 2,533 | 28 |
| CTaskComplexTurnToFaceEntityOrCoord | 23 | 9 | 14 | 2,607 | 0 |
| CTaskComplexGoToPointAnyMeans | 13 | 7 | 6 | 2,633 | 1 |
| CAutomobile | 113 | 103 | 10 | 2,917 | 10 |
| CCustomCarEnvMapPipeline | 62 | 37 | 25 | 3,547 | 5 |
| CPools | 71 | 19 | 52 | 3,733 | 51 |
| CReplay | 57 | 47 | 10 | 4,172 | 10 |
| CRunningScript | 43 | 39 | 4 | 5,919 | 0 |
| RpAnimBlend | 59 | 43 | 16 | 6,102 | 16 |
| CScriptsForBrains | 13 | 9 | 4 | 6,307 | 4 |
| FxInfoManager_c | 124 | 8 | 116 | 6,591 | 96 |
| CPedAttractorManager | 85 | 14 | 71 | 8,097 | 0 |
| CTaskComplexSeekEntityRadiusAngleOffset | 15 | 10 | 5 | 8,771 | 1 |
| CEntity | 113 | 71 | 42 | 9,537 | 5 |
| CTheScripts | 87 | 73 | 14 | 9,864 | 6 |
| CWeaponModelInfo | 45 | 4 | 39 | 12,299 | 0 |
| Win | 109 | 24 | 65 | 14,050 | 0 |
| FxEmitterBP_c | 155 | 14 | 141 | 28,395 | 9 |

132 de 782 módulos tienen algo pendiente.

## Cómo usarlo

- Filtra `funciones.csv` por `estado` = `A` y ordena por `llamadas_desde_reemplazadas` para empezar por lo más usado por código ya reconstruido.
- Una función de 16 bytes o menos suele ser un stub o thunk; mira `thunk` antes de tratarla como trabajo.
- Los grupos C y D pueden contener código muerto: confirma con análisis dinámico antes de reconstruir.
