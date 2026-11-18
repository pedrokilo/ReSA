<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1561"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleClearLookAtDestructorBody(CTaskSimpleClearLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleClearLookAt::Destructor 0x006346A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleClearLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1562"></a>

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

<a id="grupo-1563"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CowerDestructorBody(CTaskSimpleCower* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCower::Destructor 0x0048E900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABody_CTaskSimpleCower() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1564"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CreateCarAndGetInDestructorBody(CTaskSimpleCreateCarAndGetIn* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp#L30) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleCreateCarAndGetIn::Destructor 0x0064CEA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote171ABody_CTaskSimpleCreateCarAndGetIn() | [source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp:44](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1565"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:81](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1566"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:190](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1567"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDieInCarDestructorBody(CTaskSimpleDieInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleDieInCar::Destructor 0x0062FC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskSimpleDieInCar() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1568"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDoHandSignalDestructorBody(CTaskSimpleDoHandSignal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleDoHandSignal::Destructor 0x006608B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskSimpleDoHandSignal() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1569"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::Constructor 0x0062FEC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DrownDestructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::DestructorBody 0x0062FF10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1570"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrownInCarDestructorBody(CTaskSimpleDrownInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrownInCar::DestructorBody 0x0062FF60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1571"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DuckToggleDestructorBodyExe(CTaskSimpleDuckToggle* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDuckToggle::Destructor 0x0062F6D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote183ABody_CTaskSimpleDuckToggle() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1572"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EvasiveDiveDestructorBody(CTaskSimpleEvasiveDive* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleEvasiveDive::Destructor 0x006535D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AEvasiveDiveDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1573"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall FacialConstructorBody(CTaskSimpleFacial* self, void* /*edx*/, uint32 first, uint32 second) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFacial::Constructor 0x00690C70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SimpleFacialDestructorBody(CTaskSimpleFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:143](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleFacial::Destructor 0x00690CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleFacialDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1574"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleFallDestructorBodyExe(CTaskSimpleFall* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFall::DestructorBody 0x00678310 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp:34 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp:33 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:26 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:31 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:30 (0x004D1390) |

<a id="grupo-1575"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FightUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboInfo(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:91](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_aComboData: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:60 (0x00C170D0) |
| FightShift(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:100](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightU32(const void* pointer) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:104](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightCleanUpRegs(const CEntity* entity, CEntity** ref, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:116](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pEmptyList: source/game_sa/References.h:16 (0x00BA1768) |
| FightPruneFailedEcx(uint32 ecx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:133](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightRegisterReference(CEntity* entity, CEntity** ref, FightRegs in) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:157](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightControlFightExe(CTaskSimpleFight* self, uint32 entryEdx, CEntity* entity, uint8 command) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:177](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::ControlFight 0x0061C5E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightGetComboAnimGroupIDExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:200](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::GetComboAnimGroupID 0x004ABDA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsComboSetExe(CTaskSimpleFight* self, uint32 /*entryEdx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:207](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsComboSet 0x004ABDC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsHitComboSetExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:215](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L215) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsHitComboSet 0x004ABDF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightActiveWeaponType(const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:277](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRow(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:283](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRadius(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:287](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L287) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightStrikeBigHit(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:290](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L290) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FightAt(uintptr_t address) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:343](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L343) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboRow(int32 combo) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:364](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L364) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:541](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L541) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightExeRand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:916](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L916) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightPositionOf(const CPlaceable* placeable) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:920](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L920) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightHitPedTableAt(int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:924](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L924) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightField(const void* base, uintptr_t offset) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:1446](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L1446) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightDot(double ax, double ay, double az, const float* b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:1450](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L1450) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1576"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FinishBrainDestructorBody(CTaskSimpleFinishBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFinishBrain::Destructor 0x004636E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1577"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote145ABody_CTaskSimpleGangDriveBy() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Gt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:346](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L346) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Lt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:347](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L347) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Ge(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:348](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L348) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Le(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:349](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L349) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87IsZero(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:352](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L352) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveCam() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:355](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L355) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1578"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CTaskSimpleGiveCPRScalarDeletingDestructor(CTaskSimpleGiveCPR* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1579"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointFineDestructorBody(CTaskSimpleGoToPointFine* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleGoToPointFine::DestructorBody 0x0065EEF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1580"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGunControlDestructorBody(CTaskSimpleGunControl* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleGunControl::Destructor 0x0061F4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskSimpleGunControl() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:88](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
