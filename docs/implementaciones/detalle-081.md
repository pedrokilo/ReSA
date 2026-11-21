# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1601"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDoHandSignalDestructorBody(CTaskSimpleDoHandSignal* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleDoHandSignal::Destructor 0x006608B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskSimpleDoHandSignal() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1602"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:34](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::Constructor 0x0062FEC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DrownDestructorBody(CTaskSimpleDrown* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrown::DestructorBody 0x0062FF10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1603"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrownInCarDestructorBody(CTaskSimpleDrownInCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleDrownInCar::DestructorBody 0x0062FF60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1604"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DuckToggleDestructorBodyExe(CTaskSimpleDuckToggle* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleDuckToggle::Destructor 0x0062F6D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote183ABody_CTaskSimpleDuckToggle() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1605"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EvasiveDiveDestructorBody(CTaskSimpleEvasiveDive* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleEvasiveDive::Destructor 0x006535D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AEvasiveDiveDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1606"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall FacialConstructorBody(CTaskSimpleFacial* self, void* /*edx*/, uint32 first, uint32 second) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFacial::Constructor 0x00690C70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SimpleFacialDestructorBody(CTaskSimpleFacial* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:143](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CTaskSimpleFacial::Destructor 0x00690CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote128ASimpleFacialDestructorBody() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp:149](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1607"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleFallDestructorBodyExe(CTaskSimpleFall* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFall::DestructorBody 0x00678310 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1608"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FightUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:85](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboInfo(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:94](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_aComboData: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:60 (0x00C170D0) |
| FightShift(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:103](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightU32(const void* pointer) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:107](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightCleanUpRegs(const CEntity* entity, CEntity** ref, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:119](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pEmptyList: source/game_sa/References.h:16 (0x00BA1768) |
| FightPruneFailedEcx(uint32 ecx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:136](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightRegisterReference(CEntity* entity, CEntity** ref, FightRegs in) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:160](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightControlFightExe(CTaskSimpleFight* self, uint32 entryEdx, CEntity* entity, uint8 command) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:180](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::ControlFight 0x0061C5E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightGetComboAnimGroupIDExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:203](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L203) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::GetComboAnimGroupID 0x004ABDA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsComboSetExe(CTaskSimpleFight* self, uint32 /*entryEdx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:210](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L210) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsComboSet 0x004ABDC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsHitComboSetExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:218](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsHitComboSet 0x004ABDF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightActiveWeaponType(const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:280](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRow(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:286](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L286) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRadius(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:290](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L290) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightStrikeBigHit(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:293](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L293) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FightAt(uintptr_t address) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:369](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L369) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboRow(int32 combo) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:390](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L390) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:567](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L567) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightExeRand() | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:942](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L942) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightPositionOf(const CPlaceable* placeable) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:946](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L946) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightHitPedTableAt(int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:950](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightField(const void* base, uintptr_t offset) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:1472](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L1472) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightDot(double ax, double ay, double az, const float* b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:1476](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L1476) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1609"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FinishBrainDestructorBody(CTaskSimpleFinishBrain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFinishBrain::Destructor 0x004636E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1610"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangDriveByFinishAnimCB(CAnimBlendAssociation* anim, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:30](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleGangDriveBy::FinishAnimCB 0x00621BE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleGangDriveBy() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:70](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Gt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:359](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Lt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:360](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L360) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Ge(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:361](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L361) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Le(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:362](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L362) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87IsZero(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:365](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L365) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveCam() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:368](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L368) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1611"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CTaskSimpleGiveCPRScalarDeletingDestructor(CTaskSimpleGiveCPR* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1612"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GoToPointFineDestructorBody(CTaskSimpleGoToPointFine* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleGoToPointFine::DestructorBody 0x0065EEF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1613"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleGunControlDestructorBody(CTaskSimpleGunControl* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:28](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleGunControl::Destructor 0x0061F4D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskSimpleGunControl() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:42](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:88](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1614"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HandsUpDestructorBody(CTaskSimpleHandsUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleHandsUp::Destructor 0x0048EA60 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskSimpleHandsUp() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1615"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleHitHeadDestructorBodyExe(CTaskSimpleHitHead* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1616"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:61](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:70](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskSimpleHoldEntity() | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:91](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1617"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall IKChainDefaultConstructorBody(CTaskSimpleIKChain* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleIKChain::ConstructorDefaultBody 0x0062EC10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:57](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1618"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKLookAtDestructorBody(CTaskSimpleIKLookAt* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKLookAt::Destructor- 0x00633EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKLookAt() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:58](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1619"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKManagerDestructorBody(CTaskSimpleIKManager* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L25) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKManager::Destructor 0x00633830 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskSimpleIKManager() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1620"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleIKPointArmDestructorBody(CTaskSimpleIKPointArm* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskSimpleIKPointArm::Destructor 0x00634240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskSimpleIKPointArm() | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:56](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
