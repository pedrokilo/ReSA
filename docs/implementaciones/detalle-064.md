<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1261"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:134](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) | [source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1262"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SunbatheWeatherAllows() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00C8131C |
| SunbatheShouldLoad() | [source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp:53](../../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1263"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TaskComplexUseGogglesTestCode() | [source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp:79](../../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1264"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaitForPedUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1265"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewEmptyPointRoute() | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:55](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddPointUnlessFull(CPointRoute& route, const CVector& pt) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:64](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyPointRoute(CPointRoute& dst, const CPointRoute& src) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:72](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MulMatrixPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp:80](../../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1266"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangHassleSeekRadius() | [source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp:65](../../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1267"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleArrestPed__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp:23](../../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1268"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeHitAtan2(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1269"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Sin(float angle) | [source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp:169](../../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1270"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChokeFinishCallback(CAnimBlendAssociation*, void* data) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:25](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeTime() | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomChokeSpeed(float k, float base) | [source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1271"></a>

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

<a id="grupo-1272"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeadAsinX87(float x) | [source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1273"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskSimpleDie__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp:153](../../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1274"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FightUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightComboInfo(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_aComboData: source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h:60 (0x00C170D0) |
| FightShift(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:63](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightU32(const void* pointer) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:67](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightCleanUpRegs(const CEntity* entity, CEntity** ref, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:79](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pEmptyList: source/game_sa/References.h:16 (0x00BA1768) |
| FightPruneFailedEcx(uint32 ecx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:96](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightRegisterReference(CEntity* entity, CEntity** ref, FightRegs in) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:120](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightControlFightExe(CTaskSimpleFight* self, uint32 entryEdx, CEntity* entity, uint8 command) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:139](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightGetComboAnimGroupIDExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:162](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::GetComboAnimGroupID 0x004ABDA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsComboSetExe(CTaskSimpleFight* self, uint32 /*entryEdx*/) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:169](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsComboSet 0x004ABDC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightIsHitComboSetExe(CTaskSimpleFight* self, uint32 entryEdx) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:177](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskSimpleFight::IsHitComboSet 0x004ABDF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightActiveWeaponType(const CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:251](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L251) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightStrikeDamageExe(CTaskSimpleFight* task, CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:257](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L257) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061C740: fuera del catálogo consultado; ruta efectiva sin resolver |
| * FightHitPedExe(CTaskSimpleFight* task, CPed* creator, CPed* victim, CVector* posn1, CVector* posn2, uint32 component) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:262](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L262) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061CBA0: fuera del catálogo consultado; ruta efectiva sin resolver |
| FightHitObjExe(CTaskSimpleFight* task, CPed* creator, CEntity* object, CVector* posn1, CVector* posn2, uint32 component, uint32 surface) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:267](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L267) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0061D400: fuera del catálogo consultado; ruta efectiva sin resolver |
| * FightStrikeRow(const CTaskSimpleFight* task) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:272](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L272) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FightStrikeRadius(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:276](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L276) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FightStrikeBigHit(const uint8* row, int32 move) | [source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:279](../../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp#L279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1275"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| X87Gt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:304](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L304) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Lt(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:305](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Ge(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:306](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L306) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87Le(float a, float b) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:307](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87IsZero(float value) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:310](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & ActiveCam() | [source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp:313](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp#L313) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |

<a id="grupo-1276"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GunControlHeading(float x, float y) | [source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp:68](../../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1277"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:36](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1278"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1279"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp:39](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1280"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawBoneTag32(const eBoneTag16& bone) | [source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
