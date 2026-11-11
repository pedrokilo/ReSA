<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 884 grupos / 9962 funciones reconstruidas; 9962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1461"></a>

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
| FightControlFightExe(CTaskSimpleFight* self, uint32 entryEdx, CEntity* entity, uint8 command) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:176](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightGetComboAnimGroupIDExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:199](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L199) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::GetComboAnimGroupID 0x004ABDA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsComboSetExe(CTaskSimpleFight* self, uint32 /*entryEdx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:206](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsComboSet 0x004ABDC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsHitComboSetExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:214](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsHitComboSet 0x004ABDF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightActiveWeaponType(const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:276](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L276) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRow(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:282](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L282) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRadius(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:286](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L286) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightStrikeBigHit(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:289](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L289) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FightAt(uintptr_t address) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:342](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L342) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboRow(int32 combo) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:363](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L363) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:540](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L540) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightExeRand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:915](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L915) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightPositionOf(const CPlaceable* placeable) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:919](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L919) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightHitPedTableAt(int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:923](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L923) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightField(const void* base, uintptr_t offset) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:1445](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L1445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightDot(double ax, double ay, double az, const float* b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:1449](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L1449) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1462"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FinishBrainDestructorBody(CTaskSimpleFinishBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFinishBrain::Destructor 0x004636E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1463"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGangDriveByDestructorBody(CTaskSimpleGangDriveBy* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleGangDriveBy() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Gt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:325](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L325) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Lt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:326](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L326) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Ge(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:327](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L327) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Le(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:328](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L328) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87IsZero(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:331](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L331) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveCam() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:334](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L334) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1464"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToDestructorBody(CTaskSimpleGoTo* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleGoTo::Destructor 0x00667D50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1465"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGunControlDestructorBody(CTaskSimpleGunControl* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleGunControl::Destructor 0x0061F4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskSimpleGunControl() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:88](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1466"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HandsUpDestructorBody(CTaskSimpleHandsUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleHandsUp::Destructor 0x0048EA60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskSimpleHandsUp() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1467"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitHeadDestructorBodyExe(CTaskSimpleHitHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:26 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:27 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:30 (0x004D1390) |

<a id="grupo-1468"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldEntityDestructorBody(CTaskSimpleHoldEntity* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L55) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskSimpleHoldEntity() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1469"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1470"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKLookAtDestructorBody(CTaskSimpleIKLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKLookAt::Destructor- 0x00633EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1471"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKManagerDestructorBody(CTaskSimpleIKManager* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKManager::Destructor 0x00633830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleIKManager() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1472"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKPointArmDestructorBody(CTaskSimpleIKPointArm* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKPointArm::Destructor 0x00634240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKPointArm() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1473"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleInAirDestructorBodyExe(CTaskSimpleInAir* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:26 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:27 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp:30 (0x004D1390) |

<a id="grupo-1474"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleJetPackDestructorBody(CTaskSimpleJetPack* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp:60](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp#L60) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleJetPack::Destructor 0x0067E600 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleJetPack() | [source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp:74](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1475"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SimpleJumpDestructorBodyExe(CTaskSimpleJump* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1476"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShakePadIfPlayerVehicle(CVehicle* car) | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1477"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleLandDestructorBody(CTaskSimpleLand* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote148ABody_CTaskSimpleLand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1478"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LookAboutDestructorBody(CTaskSimpleLookAbout* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLookAbout::Destructor 0x0048EA90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1479"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SimpleNoneDestructorBody(CTaskSimpleNone* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLeaveGroup::Destructor 0x00463560 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleNoneDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1480"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PauseDestructorBody(CTaskSimplePause* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePause::Destructor 0x0048E8A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
