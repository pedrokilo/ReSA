# Estado de todas las listas solicitadas

Trabajo detenido por instrucción del usuario el 08/10/2026. No se iniciaron nuevas reconstrucciones ni regresiones después de detener la ejecución.

**Reconstrucción solicitada hecha en las 67 clases.** Las listas anteriores reúnen 89 identidades aprobadas históricamente; el último lote tiene 354 funciones válidas reconstruidas y una corrección de catálogo. La aprobación de fixtures no certifica todas las funciones preexistentes de una clase ni una campaña completa del juego.

**Último lote: cierre parcial.** 74 de 177 bancos, 117 de 354 funciones y 301.568 pares del alcance quedaron verificados con fuentes, instrumentos, binarios y hashes de ejecución actuales. Quedan 237 funciones sin cierre final, repartidas entre un banco fallido y 102 bancos que no completaron el replay. `CTaskComplexGangFollower::Destructor` difiere en 256/256 casos, en el vptr de `task[0]`; no se resolvió después de la orden de parar.

Las tres desviaciones de [resolutions.json](../reversing/Pending355/resolutions.json), los límites de fixtures/SEH y los 17 pseudocódigos que el proveedor no pudo producir siguen explícitos en sus bancos. El ensamblador sí está disponible. La regresión anterior de cinco bancos y 9.984 pares no se renovó antes de parar.

Las listas anteriores conservan aprobación histórica: 75 de sus 89 identidades solicitadas coinciden con los hashes registrados de fuentes/evidencia; 14 requieren renovación (`CTaskSimpleDead`: 6, `CRealTimeShadowManager`: 2, `CTaskGangHasslePed`: 3, `CTaskUtilityLineUpPedWithCar`: 3). Ese cotejo no acredita dependencias actuales.

[Tabla CSV](EstadoListasSolicitadas.csv), [estado del último lote](../reversing/Pending355/README.md), [registro parcial](../reversing/Pending355/partial-summary.json).

## Último lote: qué quedó verificado ahora

| Clase | Reconstrucción solicitada | Replay final actual | Estado |
|---|---:|---:|---|
| `CAERadioTrackManager` | 14/14 | 0/14 | Pendiente |
| `CAEScriptAudioEntity` | 11/11 | 0/11 | Pendiente |
| `CBike` | 18/18 | 18/18 | Completo |
| `CCam` | 30/30 | 30/30 | Completo |
| `CCamera` | 19/19 | 19/19 | Completo |
| `CCarEnterExit` | 5/5 | 5/5 | Completo |
| `CClothesBuilder` | 8/8 | 8/8 | Completo |
| `CFormation` | 8/8 | 8/8 | Completo |
| `CGenericGameStorage` | 14/14 | 0/14 | Pendiente |
| `CHud` | 8/8 | 0/8 | Pendiente |
| `CInterestingEvents` | 5/5 | 0/5 | Pendiente |
| `CMonsterTruck` | 6/6 | 0/6 | Pendiente |
| `CPedDamageResponseCalculator` | 4/4 | 0/4 | Pendiente |
| `CPedGeometryAnalyser` | 29/29 | 6/29 | Parcial |
| `CPlane` | 9/9 | 0/9 | Pendiente |
| `CProjectileInfo` | 11/11 | 0/11 | Pendiente |
| `CRunningScript` | 15/15 | 0/15 | Pendiente |
| `CShadows` | 10/10 | 0/10 | Pendiente |
| `CTaskComplexFollowLeaderInFormation` | 6/6 | 6/6 | Completo |
| `CTaskComplexGangFollower` | 7/7 | 0/7 | Pendiente; destructor fallido |
| `CTaskComplexGangJoinRespond` | 5/5 | 5/5 | Completo |
| `CTaskComplexHitPedWithCar` | 8/8 | 0/8 | Pendiente |
| `CTaskComplexKillPedFromBoat` | 5/5 | 5/5 | Completo |
| `CTaskComplexPlayHandSignalAnim` | 6/6 | 0/6 | Pendiente |
| `CTaskComplexSmartFleePoint` | 10/10 | 0/10 | Pendiente |
| `CTaskComplexSunbathe` | 6/6 | 0/6 | Pendiente |
| `CTaskComplexWanderStandard` | 6/6 | 0/6 | Pendiente |
| `CTaskGangHassleVehicle` | 6/6 | 0/6 | Pendiente |
| `CTaskGoToVehicleAndLean` | 5/5 | 0/5 | Pendiente |
| `CTrain` | 20/20 | 0/20 | Pendiente |
| `CWaterLevel` | 5/5 | 0/5 | Pendiente |
| `CWeapon` | 6/6 | 0/6 | Pendiente |
| `FxEmitterBP_c` | 7/7 | 7/7 | Completo |
| `InteriorGroup_c` | 22/22 | 0/22 | Pendiente |

## Listas anteriores

| Clase | Reconstruidas / catálogo (actual y HEAD) | Identidades en registro | Hash de fuentes y evidencia actualmente coincidente | Evidencia |
|---|---:|---:|---:|---|
| CTaskSimpleDead | 6/6 | 6 | 0 | `reversing/CTaskDead/validation-summary.json` |
| CIdleCam | 14/14 | 1 | 1 | `reversing/IdleAttractorPending/validation-summary.json` |
| CPedAttractor | 20/20 | 20 | 1 | `reversing/FixPeds/validation-summary.json`; `reversing/IdleAttractorPending/validation-summary.json` |
| CRoadBlocks | 8/8 | 1 | 1 | `reversing/RoadBlockPending/validation-summary.json` |
| CTaskSimpleFightingControl | 7/7 | 1 | 1 | `reversing/FightingControlPending/validation-summary.json` |
| CTaskSimplePlayerOnFoot | 12/12 | 1 | 1 | `reversing/PlayerFighterPending/validation-summary.json` |
| CAEFrontendAudioEntity | 6/6 | 2 | 2 | `reversing/AudioFrontendPending/validation-summary.json` |
| CDecision | 3/3 | 2 | 2 | `reversing/DecisionPending/validation-summary.json` |
| CGroupEventHandler | 32/32 | 2 | 2 | `reversing/GroupEventPending/validation-summary.json` |
| CPathFind | 56/56 | 2 | 2 | `reversing/PathFindPending/validation-summary.json` |
| CPickups | 37/37 | 2 | 2 | `reversing/PickupsPending/validation-summary.json` |
| CPopulation | 66/66 | 2 | 2 | `reversing/PopulationPending/validation-summary.json` |
| CRealTimeShadowManager | 7/7 | 2 | 0 | `reversing/ShadowManagerPending/validation-summary.json` |
| CRope | 7/7 | 2 | 2 | `reversing/RopePending/validation-summary.json` |
| CTaskComplexKillCriminal | 11/11 | 2 | 2 | `reversing/CriminalPending/validation-summary.json` |
| CTaskComplexKillPedOnFootArmed | 11/11 | 2 | 2 | `reversing/ArmedNextPending/validation-summary.json`; `reversing/ArmedPending/validation-summary.json` |
| CAEStreamingChannel | 24/24 | 6 | 3 | `reversing/AudioStreamingPending/validation-summary.json`; `reversing/FixAudio/validation-summary.json`; `reversing/FixHooks/validation-summary.json` |
| CGangWars | 35/35 | 3 | 3 | `reversing/GangWarsPending/validation-summary.json` |
| CPedGroupPlacer | 4/4 | 3 | 3 | `reversing/GroupPlacerPending/validation-summary.json` |
| CPools | 19/19 | 3 | 3 | `reversing/PoolsPending/validation-summary.json` |
| CPostEffects | 38/38 | 3 | 3 | `reversing/PostEffectsPending/validation-summary.json` |
| CTaskComplexArrestPed | 7/7 | 3 | 3 | `reversing/ArrestPending/validation-summary.json` |
| CTaskComplexGoToAttractor | 5/5 | 3 | 3 | `reversing/AttractorTaskPending/validation-summary.json` |
| CTaskGangHasslePed | 5/5 | 3 | 0 | `reversing/GangHasslePending/validation-summary.json` |
| CTaskSimpleGangDriveBy | 4/4 | 3 | 3 | `reversing/DriveByPending/validation-summary.json` |
| CTaskUtilityLineUpPedWithCar | 5/5 | 3 | 0 | `reversing/LineUpPending/validation-summary.json`; `reversing/LineUpProcessPending/validation-summary.json` |
| CAEPedAudioEntity | 20/20 | 5 | 4 | `reversing/AudioPedPending/validation-summary.json`; `reversing/FixAudio/validation-summary.json` |
| CAEPoliceScannerAudioEntity | 17/17 | 4 | 4 | `reversing/AudioScannerPending/validation-summary.json` |
| CAEVehicleAudioEntity | 106/106 | 4 | 4 | `reversing/AudioVehiclePending/validation-summary.json` |
| CCustomBuildingRenderer | 7/7 | 4 | 4 | `reversing/RendererPending/validation-summary.json` |
| CFont | 36/36 | 4 | 4 | `reversing/FontPending/validation-summary.json` |
| CTaskComplexProstituteSolicit | 11/11 | 4 | 4 | `reversing/ProstituteControlPending/validation-summary.json`; `reversing/ProstitutePending/validation-summary.json` |
| FxSystem_c | 35/35 | 4 | 4 | `reversing/FxSystemPending/validation-summary.json` |

La columna de registro incluye validaciones anteriores a estos pedidos (por ejemplo, 19 funciones de CPedAttractor y tres de CAEStreamingChannel). No atribuye esas validaciones adicionales a esta campaña. La comprobación de hashes indicada compara tested_sources y el SHA de validation-summary.json del registro; no ejecuta nuevos bancos, no verifica los binarios archivados y no reemplaza el registro estricto final.

De las 89 identidades solicitadas/revisadas en estas listas (78 + cinco + seis), 75 conservan los hashes de fuentes/evidencia registrados. Pendientes de renovar por fuentes posteriores: CTaskSimpleDead (seis), CRealTimeShadowManager (dos), CTaskGangHasslePed (tres) y CTaskUtilityLineUpPedWithCar (tres). Las ejecuciones históricas están aprobadas; el cierre actual de estos casos no debe declararse estricto sin repetir/registrar lo afectado. Los 23 registros adicionales anteriores explican 112 identidades totales en la tabla; varias de esas evidencias anteriores también están caducadas.

Límites: equivalencia de fixtures positivos descritos en cada banco; las dependencias aisladas no acreditan su implementación, no es una campaña integral de juego y no demuestra independencia del EXE. Se detuvieron reconstrucción y pruebas por solicitud del usuario. La actualización documental/Git se coordina con la raíz y no autoriza más reconstrucción ni nuevos replays.
