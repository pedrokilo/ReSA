<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 20/10/2026 (Ciudad de México):** 808 grupos / 8772 funciones reconstruidas; 8772 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1281"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1282"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1283"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FightUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:73](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboInfo(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:82](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_aComboData: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:60 (0x00C170D0) |
| FightShift(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:91](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightU32(const void* pointer) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:95](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightCleanUpRegs(const CEntity* entity, CEntity** ref, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:107](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pEmptyList: source/game_sa/References.h:16 (0x00BA1768) |
| FightPruneFailedEcx(uint32 ecx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:124](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightRegisterReference(CEntity* entity, CEntity** ref, FightRegs in) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:148](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightControlFightExe(CTaskSimpleFight* self, uint32 entryEdx, CEntity* entity, uint8 command) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:167](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightGetComboAnimGroupIDExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:190](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::GetComboAnimGroupID 0x004ABDA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsComboSetExe(CTaskSimpleFight* self, uint32 /*entryEdx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:197](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsComboSet 0x004ABDC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsHitComboSetExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:205](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsHitComboSet 0x004ABDF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightActiveWeaponType(const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:262](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightStrikeDamageExe(CTaskSimpleFight* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:268](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L268) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061C740: fuera del catálogo consultado; ruta efectiva sin resolver |
| * FightHitPedExe(CTaskSimpleFight* task, CPed* creator, CPed* victim, CVector* posn1, CVector* posn2, uint32 component) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:273](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L273) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061CBA0: fuera del catálogo consultado; ruta efectiva sin resolver |
| FightHitObjExe(CTaskSimpleFight* task, CPed* creator, CEntity* object, CVector* posn1, CVector* posn2, uint32 component, uint32 surface) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:278](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L278) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061D400: fuera del catálogo consultado; ruta efectiva sin resolver |
| * FightStrikeRow(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:283](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRadius(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:287](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L287) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightStrikeBigHit(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:290](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L290) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FightAt(uintptr_t address) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:318](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L318) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightHitLevel(const char* text) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:323](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L323) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightHitSound(int32 n) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:334](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L334) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboRow(int32 combo) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:339](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L339) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightGetAvailableComboSetExe(CTaskSimpleFight* task, CPed* ped, int32 command) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:345](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L345) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061C7F0: fuera del catálogo consultado; ruta efectiva sin resolver |
| FightFindTargetOnGroundExe(CTaskSimpleFight* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:349](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L349) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061C9B0: fuera del catálogo consultado; ruta efectiva sin resolver |
| FightChooseAttackPlayerExe(CTaskSimpleFight* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:353](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L353) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00624710: fuera del catálogo consultado; ruta efectiva sin resolver |
| FightChooseAttackAIExe(CTaskSimpleFight* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:357](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L357) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00624A40: fuera del catálogo consultado; ruta efectiva sin resolver |
| FightStartAnimExe(CTaskSimpleFight* task, CPed* ped, int32 attack) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:361](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L361) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00623B10: fuera del catálogo consultado; ruta efectiva sin resolver |
| FightTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:537](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L537) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1284"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Gt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:304](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L304) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Lt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:305](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Ge(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:306](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L306) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Le(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:307](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87IsZero(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:310](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveCam() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:313](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L313) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1285"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1286"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1287"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1288"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1289"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1290"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp:112](../../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1291"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShakePadIfPlayerVehicle(CVehicle* car) | [source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1292"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & ActiveWeaponOf(CPlayerPed* player) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:154](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyMatrixWithVectorX87(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:160](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterDirection(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:822](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L822) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FighterProject(CMatrix* matrix, float angle, float magnitude, bool roundTrig) | [source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp:830](../../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp#L830) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1293"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StealthKillAtan2(float y, float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp:64](../../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1294"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WrapSwimAngle(float a) | [source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp:96](../../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1295"></a>

## Globales y helpers: source/game_sa/Text/Text.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToAsciiOriginal(const GxtChar* src, uint8 start) | [source/game_sa/Text/Text.cpp:87](../../source/game_sa/Text/Text.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetGxtName() | [source/game_sa/Text/Text.cpp:190](../../source/game_sa/Text/Text.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| CheckFileEncoding(const char* file, uint16 version, uint16 encoding) | [source/game_sa/Text/Text.cpp:214](../../source/game_sa/Text/Text.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1296"></a>

## Globales y helpers: source/game_sa/TheZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Zones13CellValue(float coord) | [source/game_sa/TheZones.cpp:363](../../source/game_sa/TheZones.cpp#L363) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Zones13LevelResidue(const CVector& pos, uint32 exePos, uint32& ecx, uint32& edx) | [source/game_sa/TheZones.cpp:371](../../source/game_sa/TheZones.cpp#L371) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo MapZoneArray: source/game_sa/TheZones.h:23 (0x00BA1908); Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo TotalNumberOfMapZones: source/game_sa/TheZones.h:22 (0x00BA1900); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1297"></a>

## Globales y helpers: source/game_sa/TimeCycle.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/TimeCycle.cpp:56](../../source/game_sa/TimeCycle.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & TC(A& arr, int32 hour, int32 weather) | [source/game_sa/TimeCycle.cpp:62](../../source/game_sa/TimeCycle.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1298"></a>

## Globales y helpers: source/game_sa/Timer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMillisecondTime() | [source/game_sa/Timer.cpp:53](../../source/game_sa/Timer.cpp#L53) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x005617C0: fuera del catálogo consultado; ruta efectiva sin resolver |
| QueryTimerFrequency(uint32& frequencyLow) | [source/game_sa/Timer.cpp:59](../../source/game_sa/Timer.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1299"></a>

## Globales y helpers: source/game_sa/Timer.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EachFrames(auto count) | [source/game_sa/Timer.h:120](../../source/game_sa/Timer.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1300"></a>

## Globales y helpers: source/game_sa/TrafficLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NaviNodeDot(CVehicle* vehicle, const CCarPathLink& link) | [source/game_sa/TrafficLights.cpp:45](../../source/game_sa/TrafficLights.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShouldCarStopForLightLink(CVehicle* vehicle, CCarPathLinkAddress& info, const CNodeAddress& address, bool bUnkn, int8 side, float range, bool requirePhysics) | [source/game_sa/TrafficLights.cpp:55](../../source/game_sa/TrafficLights.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ThePaths: source/game_sa/PathFind.h:622 (0x0096F050) |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/TrafficLights.cpp:144](../../source/game_sa/TrafficLights.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
