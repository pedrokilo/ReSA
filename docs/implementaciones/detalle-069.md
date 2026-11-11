<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 884 grupos / 10050 funciones reconstruidas; 10050 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1361"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:79](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:140](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1362"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntitySeekPosCalculatorDestructorBody(CEntitySeekPosCalculator* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp:25](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1363"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp:12](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1364"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1365"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AttractorPartnerWaitConstructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/, uint32 flag, uint32 value) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Constructor 0x00633250 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexAttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L49) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexAttractorPartnerWait() | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:63](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1366"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeInCoupleDestructorBody(CTaskComplexBeInCouple* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexBeInCouple::Destructor 0x00683780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskComplexBeInCouple() | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1367"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexBeInGroupDestructorBody(CTaskComplexBeInGroup* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexBeInGroup::Destructor 0x00632EA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexBeInGroup() | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1368"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOutDestructorBody(CTaskComplexCarSlowBeDraggedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCarSlowBeDraggedOut::Destructor 0x0064C580 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskComplexCarSlowBeDraggedOut() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1369"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOutAndStandUpDestructorBody(CTaskComplexCarSlowBeDraggedOutAndStandUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCarSlowBeDraggedOutAndStandUp::Destructor 0x00648690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexCarSlowBeDraggedOutAndStandUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimGroupOf(CVehicle* veh) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1370"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexChatDestructorBody(CTaskComplexChat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexChat::Destructor 0x00682D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexChat() | [source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1371"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCrossRoadLookAndAchieveHeadingDestructorBody(CTaskComplexCrossRoadLookAndAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCrossRoadLookAndAchieveHeading::Destructor 0x00631B50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexCrossRoadLookAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1372"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDestroyCarDestructorBody(CTaskComplexDestroyCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexDestroyCar::Destructor 0x00621CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexDestroyCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1373"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DieDestructorBody(CTaskComplexDie* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexDie::Destructor 0x006300C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1374"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDieInCarDestructorBody(CTaskComplexDieInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskComplexDieInCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1375"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDiveFromAttachedEntityAndGetUpDestructorBody(CTaskComplexDiveFromAttachedEntityAndGetUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexDiveFromAttachedEntityAndGetUp::Destructor 0x00492F10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexDiveFromAttachedEntityAndGetUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1376"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDragPedFromCar__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1377"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DriveToPointDestructorBody(CTaskComplexDriveToPoint* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexDriveToPoint::Destructor 0x0063CE70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1378"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote140ACarDriveWanderDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1379"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterAnyCarAsDriverDestructorBody(CTaskComplexEnterAnyCarAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexEnterAnyCarAsDriver::Destructor 0x0063D100 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskComplexEnterAnyCarAsDriver() | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1380"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnterCarDestructorBody(CTaskComplexEnterCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:61](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L61) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexEnterCar::Destructor 0x0063DFA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskComplexEnterCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:75](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddLeaderGroupEvent(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:117](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCarTooFastToEnter(const CVehicle* car, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:126](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fMaxSpeed_CanDragPedOut: source/game_sa/CarEnterExit.h:17 (0x0086F104); Posible dato fijo ms_fMaxSpeed_PlayerCanDragPedOut: source/game_sa/CarEnterExit.h:18 (0x0086F108) |
| OpenTargetCarDoor(CVehicle* car, int32 door) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:136](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyCruiseSpeed(CVehicle* car, float cruiseSpeed) | [source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp:143](../../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
