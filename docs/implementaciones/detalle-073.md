<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1441"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShopKeeperDestructorBody(CTaskInteriorShopKeeper* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp:32](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskInteriorShopKeeper::Destructor 0x006756A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AShopKeeperDestructorBody() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp:46](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1442"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:79](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:140](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1443"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & SitInChairField(CTaskInteriorSitInChair* self, size_t offset) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:43](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairGetTaskType(CTaskInteriorSitInChair* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:48](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::GetTaskType 0x00675C80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall SitInChairClone(CTaskInteriorSitInChair* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:65](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::Clone 0x00675CF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAnimIdOf(const CAnimBlendAssociation* assoc) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:116](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SitInChairBlendAnim(CTaskInteriorSitInChair* self, CPed* ped, int16 animId, float blendDelta) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:122](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairStartTimer(CTaskInteriorSitInChair* self) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:130](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairProcessPed(CTaskInteriorSitInChair* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:149](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::ProcessPed 0x00676D30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Colaborador por dirección 0x0059C890: CMatrix::operator*-Vec; ruta efectiva sin resolver; Posible dato fijo g_ikChainMan: source/game_sa/Ragdoll/IKChainManager.cpp:29 (0x00C15448); Posible dato fijo g_interiorMan: source/game_sa/Interior/InteriorManager_c.h:102 (0x00BAF670) |

<a id="grupo-1444"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntitySeekPosCalculatorDestructorBody(CEntitySeekPosCalculator* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp:29](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1445"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp:14](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;::SetMaxEntityDist2D 0x004BC470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1446"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComplexArrestPedDestructorBody(CTaskComplexArrestPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L40) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexArrestPed::Destructor 0x0068BA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote171ABody_CTaskComplexArrestPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:173](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1447"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AttractorPartnerWaitConstructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/, uint32 flag, uint32 value) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Constructor 0x00633250 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexAttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L49) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexAttractorPartnerWait() | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:63](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1448"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AvoidEntityGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAvoidEntity::GetTaskType 0x0066AAD0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1449"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeInCoupleDestructorBody(CTaskComplexBeInCouple* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexBeInCouple::Destructor 0x00683780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskComplexBeInCouple() | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1450"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexBeInGroupDestructorBody(CTaskComplexBeInGroup* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexBeInGroup::Destructor 0x00632EA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexBeInGroup() | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1451"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOutDestructorBody(CTaskComplexCarSlowBeDraggedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCarSlowBeDraggedOut::Destructor 0x0064C580 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskComplexCarSlowBeDraggedOut() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1452"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOutAndStandUpDestructorBody(CTaskComplexCarSlowBeDraggedOutAndStandUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCarSlowBeDraggedOutAndStandUp::Destructor 0x00648690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexCarSlowBeDraggedOutAndStandUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimGroupOf(CVehicle* veh) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1453"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexChatDestructorBody(CTaskComplexChat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexChat::Destructor 0x00682D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexChat() | [source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1454"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCrossRoadLookAndAchieveHeadingDestructorBody(CTaskComplexCrossRoadLookAndAchieveHeading* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCrossRoadLookAndAchieveHeading::Destructor 0x00631B50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote157ABody_CTaskComplexCrossRoadLookAndAchieveHeading() | [source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp:41](../../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1455"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDestroyCarDestructorBody(CTaskComplexDestroyCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp:29](../../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexDestroyCar::Destructor 0x00621CB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexDestroyCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp:43](../../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1456"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DieDestructorBody(CTaskComplexDie* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp:37](../../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexDie::Destructor 0x006300C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1457"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CTaskComplexDieInCarConstructorBody(CTaskComplexDieInCar* self, void* /*edx*/, eWeaponType weaponType) | [source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote159ABody_CTaskComplexDieInCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1458"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDiveFromAttachedEntityAndGetUpDestructorBody(CTaskComplexDiveFromAttachedEntityAndGetUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexDiveFromAttachedEntityAndGetUp::Destructor 0x00492F10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexDiveFromAttachedEntityAndGetUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1459"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DragPedFromCarDestructorBodyExe(CTaskComplexDragPedFromCar* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp:27](../../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote165ABody_CTaskComplexDragPedFromCar() | [source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexDragPedFromCar__InjectHooks() | [source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp:47](../../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1460"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexDriveFireTruckDestructorBodyExe(CTaskComplexDriveFireTruck* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp:38](../../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
