<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1421"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PlayerCommandAttackConstructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, CPed* target, int32 groupTargetId, uint32 value) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:98](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::Constructor 0x0069C480 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerCommandAttackDestructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:109](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::DestructorBody 0x0069DB40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PlayerCommandAttackScalarDeletingDestructor(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:116](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::ScalarDeletingDestructor 0x0069DB00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1422"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SequenceDestructorBody(CTaskComplexSequence* self, void* /*edx*/) | [source/game_sa/Tasks/TaskComplexSequence.cpp:31](../../source/game_sa/Tasks/TaskComplexSequence.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexSequence::Destructor 0x006389F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1423"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Tasks/TaskComplexSequence.h:58](../../source/game_sa/Tasks/TaskComplexSequence.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1424"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexUseSequence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote145ABody_CTaskComplexUseSequence() | [source/game_sa/Tasks/TaskComplexUseSequence.cpp:59](../../source/game_sa/Tasks/TaskComplexUseSequence.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1425"></a>

## Globales y helpers: source/game_sa/Tasks/TaskManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeleteTaskAndNull(CTask*& task) | [source/game_sa/Tasks/TaskManager.cpp:58](../../source/game_sa/Tasks/TaskManager.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindFirstTaskOfType(CTask* task, eTaskType type) | [source/game_sa/Tasks/TaskManager.cpp:64](../../source/game_sa/Tasks/TaskManager.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1426"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeInShopDestructorBody(CTaskInteriorBeInShop* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp:34](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskInteriorBeInShop::Destructor 0x00675460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146ABeInShopDestructorBody() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp:48](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1427"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InteriorInfoHeading(const InteriorInfo_t* info) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp:61](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1428"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LieInBedTransformPointXY(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp:124](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1429"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShopKeeperDestructorBody(CTaskInteriorShopKeeper* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp:32](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskInteriorShopKeeper::Destructor 0x006756A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AShopKeeperDestructorBody() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp:46](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1430"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:79](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:140](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1431"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & SitInChairField(CTaskInteriorSitInChair* self, size_t offset) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:43](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairGetTaskType(CTaskInteriorSitInChair* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:48](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::GetTaskType 0x00675C80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall SitInChairClone(CTaskInteriorSitInChair* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:65](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::Clone 0x00675CF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAnimIdOf(const CAnimBlendAssociation* assoc) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:116](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SitInChairBlendAnim(CTaskInteriorSitInChair* self, CPed* ped, int16 animId, float blendDelta) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:122](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairStartTimer(CTaskInteriorSitInChair* self) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:130](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1432"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntitySeekPosCalculatorDestructorBody(CEntitySeekPosCalculator* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp:25](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1433"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComplexArrestPedDestructorBody(CTaskComplexArrestPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L40) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexArrestPed::Destructor 0x0068BA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote171ABody_CTaskComplexArrestPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:171](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1434"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AttractorPartnerWaitConstructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/, uint32 flag, uint32 value) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Constructor 0x00633250 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexAttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L49) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexAttractorPartnerWait() | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:63](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1435"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FieldAt(void* object, uint32 offset) | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AvoidEntityGetTaskType(void* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp:69](../../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAvoidEntity::GetTaskType 0x0066AAD0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1436"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeInCoupleDestructorBody(CTaskComplexBeInCouple* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexBeInCouple::Destructor 0x00683780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote151ABody_CTaskComplexBeInCouple() | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1437"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexBeInGroupDestructorBody(CTaskComplexBeInGroup* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp:26](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp#L26) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexBeInGroup::Destructor 0x00632EA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote147ABody_CTaskComplexBeInGroup() | [source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1438"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOutDestructorBody(CTaskComplexCarSlowBeDraggedOut* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCarSlowBeDraggedOut::Destructor 0x0064C580 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote155ABody_CTaskComplexCarSlowBeDraggedOut() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1439"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexCarSlowBeDraggedOutAndStandUpDestructorBody(CTaskComplexCarSlowBeDraggedOutAndStandUp* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:32](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexCarSlowBeDraggedOutAndStandUp::Destructor 0x00648690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexCarSlowBeDraggedOutAndStandUp() | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:46](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimGroupOf(CVehicle* veh) | [source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp:78](../../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1440"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CTaskComplexChatDestructorBody(CTaskComplexChat* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp#L31) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexChat::Destructor 0x00682D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote153ABody_CTaskComplexChat() | [source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp:45](../../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
