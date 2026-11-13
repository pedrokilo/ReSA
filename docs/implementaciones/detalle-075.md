<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 898 grupos / 10446 funciones reconstruidas; 10446 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1481"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarJumpOutDestructorBody(CTaskSimpleCarJumpOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarJumpOut::Destructor 0x00647490 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarJumpOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1482"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedInAsDriverDestructorBody(CTaskSimpleCarSetPedInAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedInAsDriver::Destructor 0x00647170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1483"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedOutDestructorBody(CTaskSimpleCarSetPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedOut::Destructor 0x00647950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1484"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSetPedSlowDraggedOutDestructorBody(CTaskSimpleCarSetPedSlowDraggedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarSetPedSlowDraggedOut::Destructor 0x00648430 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskSimpleCarSetPedSlowDraggedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1485"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarSlowDragPedOutDestructorBody(CTaskSimpleCarSlowDragPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarSlowDragPedOut::Destructor 0x00648070 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleCarSlowDragPedOut() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1486"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitForDoorNotToBeInUseDestructorBody(CTaskSimpleCarWaitForDoorNotToBeInUse* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitForDoorNotToBeInUse::Destructor 0x00646C00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitForDoorNotToBeInUse() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1487"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleCarWaitToSlowDownDestructorBody(CTaskSimpleCarWaitToSlowDown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCarWaitToSlowDown::Destructor 0x00646A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CTaskSimpleCarWaitToSlowDown() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1488"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1489"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleClearLookAtDestructorBody(CTaskSimpleClearLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleClearLookAt::Destructor 0x006346A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleClearLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1490"></a>

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
| CTaskSimpleClimbDestructorBodyExe(CTaskSimpleClimb* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp:131](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1491"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CowerDestructorBody(CTaskSimpleCower* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCower::Destructor 0x0048E900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleCower() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1492"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CreateCarAndGetInDestructorBody(CTaskSimpleCreateCarAndGetIn* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCreateCarAndGetIn::Destructor 0x0064CEA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote171ABody_CTaskSimpleCreateCarAndGetIn() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1493"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1494"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:173](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1495"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDieInCarDestructorBody(CTaskSimpleDieInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleDieInCar::Destructor 0x0062FC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleDieInCar() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1496"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDoHandSignalDestructorBody(CTaskSimpleDoHandSignal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleDoHandSignal::Destructor 0x006608B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskSimpleDoHandSignal() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1497"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::Constructor 0x0062FEC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DrownDestructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::DestructorBody 0x0062FF10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1498"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrownInCarDestructorBody(CTaskSimpleDrownInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrownInCar::DestructorBody 0x0062FF60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1499"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EvasiveDiveDestructorBody(CTaskSimpleEvasiveDive* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleEvasiveDive::Destructor 0x006535D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AEvasiveDiveDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1500"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall FacialConstructorBody(CTaskSimpleFacial* self, void* /*edx*/, uint32 first, uint32 second) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFacial::Constructor 0x00690C70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SimpleFacialDestructorBody(CTaskSimpleFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:143](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleFacial::Destructor 0x00690CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleFacialDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
