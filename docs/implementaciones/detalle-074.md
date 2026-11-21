# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1461"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TaskAllocatorDestructorBody(CTaskAllocator* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/TaskAllocator.cpp:26](../../source/game_sa/Tasks/Allocators/TaskAllocator.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocator::Destructor 0x0069BB40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall TaskAllocatorScalarDeletingDestructor(CTaskAllocator* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocator.cpp:32](../../source/game_sa/Tasks/Allocators/TaskAllocator.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocator::ScalarDeletingDestructor 0x0069DAC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1462"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AttackScalarDeletingDestructor(CTaskAllocatorAttack* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp:57](../../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorAttack::ScalarDeletingDestructor 0x0069DA00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1463"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestThreatDistance(const CVector& a, const CVector& b) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp:92](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall KillThreatsBasicScalarDeletingDestructor(CTaskAllocatorKillThreatsBasic* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp:163](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorKillThreatsBasic::ScalarDeletingDestructor 0x0069DA40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1464"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| KillThreatsBasicRandomGroupDestructorBody(CTaskAllocatorKillThreatsBasicRandomGroup* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp:83](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorKillThreatsBasicRandomGroup::Destructor 0x005F6920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1465"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall KillThreatsDrivebyScalarDeletingDestructor(CTaskAllocatorKillThreatsDriveby* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp:119](../../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorKillThreatsDriveby::ScalarDeletingDestructor 0x0069DA80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1466"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PlayerCommandAttackConstructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, CPed* target, int32 groupTargetId, uint32 value) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:98](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::Constructor 0x0069C480 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerCommandAttackDestructorBody(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:109](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::DestructorBody 0x0069DB40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PlayerCommandAttackScalarDeletingDestructor(CTaskAllocatorPlayerCommandAttack* self, void* /*edx*/, uint8 flags) | [source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp:116](../../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskAllocatorPlayerCommandAttack::ScalarDeletingDestructor 0x0069DB00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1467"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SequenceDestructorBody(CTaskComplexSequence* self, void* /*edx*/) | [source/game_sa/Tasks/TaskComplexSequence.cpp:31](../../source/game_sa/Tasks/TaskComplexSequence.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexSequence::Destructor 0x006389F0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1468"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexSequence.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsActive() const | [source/game_sa/Tasks/TaskComplexSequence.h:58](../../source/game_sa/Tasks/TaskComplexSequence.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1469"></a>

## Globales y helpers: source/game_sa/Tasks/TaskComplexUseSequence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote145ABody_CTaskComplexUseSequence() | [source/game_sa/Tasks/TaskComplexUseSequence.cpp:59](../../source/game_sa/Tasks/TaskComplexUseSequence.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1470"></a>

## Globales y helpers: source/game_sa/Tasks/TaskManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DeleteTaskAndNull(CTask*& task) | [source/game_sa/Tasks/TaskManager.cpp:58](../../source/game_sa/Tasks/TaskManager.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindFirstTaskOfType(CTask* task, eTaskType type) | [source/game_sa/Tasks/TaskManager.cpp:64](../../source/game_sa/Tasks/TaskManager.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1471"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BeInShopDestructorBody(CTaskInteriorBeInShop* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp:34](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp#L34) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskInteriorBeInShop::Destructor 0x00675460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146ABeInShopDestructorBody() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp:48](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1472"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InteriorInfoHeading(const InteriorInfo_t* info) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp:61](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1473"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LieInBedTransformPointXY(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp:124](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1474"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ShopKeeperDestructorBody(CTaskInteriorShopKeeper* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp:32](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp#L32) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskInteriorShopKeeper::Destructor 0x006756A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AShopKeeperDestructorBody() | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp:46](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1475"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SitAtDeskRandom(float scale) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:79](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp:140](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1476"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & SitInChairField(CTaskInteriorSitInChair* self, size_t offset) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:43](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairGetTaskType(CTaskInteriorSitInChair* /*self*/, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:48](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::GetTaskType 0x00675C80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall SitInChairClone(CTaskInteriorSitInChair* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:65](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::Clone 0x00675CF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitAnimIdOf(const CAnimBlendAssociation* assoc) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:134](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SitInChairBlendAnim(CTaskInteriorSitInChair* self, CPed* ped, int16 animId, float blendDelta) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:140](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairStartTimer(CTaskInteriorSitInChair* self) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:148](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SitInChairProcessPed(CTaskInteriorSitInChair* self, void* /*edx*/, CPed* ped) | [source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp:167](../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskInteriorSitInChair::ProcessPed 0x00676D30 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Colaborador por dirección 0x0059C890: CMatrix::operator*-Vec; ruta efectiva sin resolver; Posible dato fijo g_ikChainMan: source/game_sa/Ragdoll/IKChainManager.cpp:29 (0x00C15448); Posible dato fijo g_interiorMan: source/game_sa/Interior/InteriorManager_c.h:102 (0x00BAF670) |

<a id="grupo-1477"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntitySeekPosCalculatorDestructorBody(CEntitySeekPosCalculator* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp:29](../../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1478"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits) | [source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp:14](../../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;::SetMaxEntityDist2D 0x004BC470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1479"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComplexArrestPedDestructorBody(CTaskComplexArrestPed* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L40) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CTaskComplexArrestPed::Destructor 0x0068BA00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote171ABody_CTaskComplexArrestPed() | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:54](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeSurePedHasWeaponInHand(CPed* ped) | [source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp:173](../../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1480"></a>

## Globales y helpers: source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AttractorPartnerWaitConstructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/, uint32 flag, uint32 value) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:31](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Constructor 0x00633250 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:40](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CTaskComplexAttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:49](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L49) | C++ · Cuerpo fuente con errores de análisis | Revalidada en migración: CTaskComplexAttractorPartnerWait::Destructor 0x00633290 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CTaskComplexAttractorPartnerWait() | [source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp:63](../../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
