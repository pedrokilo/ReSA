<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 08/11/2026 (Ciudad de México):** 866 grupos / 9356 funciones reconstruidas; 9356 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1341"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Sin(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:170](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1342"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedInAsDriverDestructorBody(CTaskSimpleCarSetPedInAsDriver* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedInAsDriver::Destructor 0x00647170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1343"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CarSetPedOutDestructorBody(CTaskSimpleCarSetPedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleCarSetPedOut::Destructor 0x00647950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1344"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1345"></a>

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

<a id="grupo-1346"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1347"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1348"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::Constructor 0x0062FEC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1349"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall FacialConstructorBody(CTaskSimpleFacial* self, void* /*edx*/, uint32 first, uint32 second) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFacial::Constructor 0x00690C70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1350"></a>

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

<a id="grupo-1351"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Gt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:305](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Lt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:306](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L306) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Ge(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:307](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Le(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:308](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87IsZero(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:311](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveCam() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:314](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L314) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1352"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToDestructorBody(CTaskSimpleGoTo* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleGoTo::Destructor 0x00667D50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1353"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1354"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1355"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1356"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1357"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1358"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:112](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1359"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShakePadIfPlayerVehicle(CVehicle* car) | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1360"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall HandSignalConstructorBody(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/, uint32 animId, uint32 blendBits, uint32 fatHands, uint32 holdLastFrame) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimplePlayHandSignalAnim::Constructor 0x0061AE50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
