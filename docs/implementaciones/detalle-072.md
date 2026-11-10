<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 883 grupos / 9890 funciones reconstruidas; 9890 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1421"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangHassleSeekRadius() | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp:83](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1422"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AchieveHeadingDestructorBody(CTaskSimpleAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAchieveHeading::Destructor 0x00667E70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1423"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall SimpleAnimConstructorBody(CTaskSimpleAnim* self, void* /*edx*/, uint32 holdLastFrame) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleAnim::Constructor 0x0061A6C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleAnimDestructorBody(CTaskSimpleAnim* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L42) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleAnim() | [source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1424"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ArrestPedConstructorBody(CTaskSimpleArrestPed* self, void* /*edx*/, CEntity* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleArrestPed::Constructor 0x0068B620 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskSimpleArrestPed__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1425"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitAtan2(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1426"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleBikeJackedDestructorBody(CTaskSimpleBikeJacked* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleBikeJacked::Destructor 0x00648C40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleBikeJacked() | [source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1427"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote152ACallback_CarDriveRollDoor() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:73](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Sin(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:186](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1428"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::Destructor 0x00646E50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1429"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedInAsDriverDestructorBody(CTaskSimpleCarSetPedInAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedInAsDriver::Destructor 0x00647170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1430"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedOutDestructorBody(CTaskSimpleCarSetPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedOut::Destructor 0x00647950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1431"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitForDoorNotToBeInUseDestructorBody(CTaskSimpleCarWaitForDoorNotToBeInUse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitForDoorNotToBeInUse::Destructor 0x00646C00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitForDoorNotToBeInUse() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1432"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitToSlowDownDestructorBody(CTaskSimpleCarWaitToSlowDown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitToSlowDown::Destructor 0x00646A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitToSlowDown() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1433"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1434"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClimbHeadingOf(float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbAddHeading(float add, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:77](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbSubHeading(float val, float fx, float fy) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbHeadingSub(float fx, float fy, float sub) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:87](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbMatTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:92](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbSectorIndex(float v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:101](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbIsFlatNormal(const CVector& n) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:106](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClimbNormalFacesPed(const CPed* ped, const CColPoint& cp, CVector& outNormal) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:111](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1435"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CowerDestructorBody(CTaskSimpleCower* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCower::Destructor 0x0048E900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleCower() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1436"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1437"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1438"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDoHandSignalDestructorBody(CTaskSimpleDoHandSignal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleDoHandSignal::Destructor 0x006608B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskSimpleDoHandSignal() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1439"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::Constructor 0x0062FEC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1440"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EvasiveDiveDestructorBody(CTaskSimpleEvasiveDive* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleEvasiveDive::Destructor 0x006535D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AEvasiveDiveDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
