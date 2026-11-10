<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 876 grupos / 9794 funciones reconstruidas; 9794 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1421"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1422"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1423"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::Constructor 0x0062FEC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1424"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EvasiveDiveDestructorBody(CTaskSimpleEvasiveDive* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleEvasiveDive::Destructor 0x006535D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AEvasiveDiveDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1425"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall FacialConstructorBody(CTaskSimpleFacial* self, void* /*edx*/, uint32 first, uint32 second) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFacial::Constructor 0x00690C70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SimpleFacialDestructorBody(CTaskSimpleFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleFacialDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:145](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1426"></a>

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

<a id="grupo-1427"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FinishBrainDestructorBody(CTaskSimpleFinishBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFinishBrain::Destructor 0x004636E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1428"></a>

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

<a id="grupo-1429"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToDestructorBody(CTaskSimpleGoTo* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleGoTo::Destructor 0x00667D50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1430"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1431"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitHeadDestructorBodyExe(CTaskSimpleHitHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo kExeDefaultAnimCB: source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:29 (0x004D1390), source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:27 (0x004D1390) |

<a id="grupo-1432"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1433"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1434"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1435"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKManagerDestructorBody(CTaskSimpleIKManager* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKManager::Destructor 0x00633830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleIKManager() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1436"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1437"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:112](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1438"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShakePadIfPlayerVehicle(CVehicle* car) | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1439"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SimpleNoneDestructorBody(CTaskSimpleNone* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleLeaveGroup::Destructor 0x00463560 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleNoneDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1440"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PauseDestructorBody(CTaskSimplePause* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePause::Destructor 0x0048E8A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
