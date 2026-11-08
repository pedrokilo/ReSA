<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 08/11/2026 (Ciudad de México):** 868 grupos / 9463 funciones reconstruidas; 9463 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-401"></a>

## CPedIntelligencePool

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectHooks() | [source/game_sa/Pools/PedIntelligencePool.h:37](../../source/game_sa/Pools/PedIntelligencePool.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * New() | [source/game_sa/Pools/PedIntelligencePool.h:51](../../source/game_sa/Pools/PedIntelligencePool.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedIntelligencePool::New 0x00605EC0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-402"></a>

## CPedList

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedList::InjectHooks() | [source/game_sa/PedList.cpp:23](../../source/game_sa/PedList.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::Empty() | [source/game_sa/PedList.cpp:37](../../source/game_sa/PedList.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedList::Empty 0x00699DB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::BuildListFromGroup_NoLeader(CPedGroupMembership& groupMembership) | [source/game_sa/PedList.cpp:42](../../source/game_sa/PedList.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedList::BuildListFromGroup_NoLeader 0x00699DD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::ExtractPedsWithGuns(CPedList& from) | [source/game_sa/PedList.cpp:51](../../source/game_sa/PedList.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedList::ExtractPedsWithGuns 0x0069A4C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::FillUpHoles() | [source/game_sa/PedList.cpp:64](../../source/game_sa/PedList.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::BuildListFromGroup_NotInCar_NoLeader(CPedGroupMembership* pedGroupMembership) | [source/game_sa/PedList.cpp:69](../../source/game_sa/PedList.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedList::BuildListFromGroup_NotInCar_NoLeader 0x0069A340 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::BuildListOfPedsOfPedType(int32 pedType) | [source/game_sa/PedList.cpp:81](../../source/game_sa/PedList.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedList::BuildListOfPedsOfPedType 0x0069A3B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::RemovePedsAttackingPedType(int32 pedType) | [source/game_sa/PedList.cpp:93](../../source/game_sa/PedList.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedList::RemovePedsAttackingPedType 0x0069A450 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::RemovePedsThatDontListenToPlayer() | [source/game_sa/PedList.cpp:106](../../source/game_sa/PedList.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedList::RemovePedsThatDontListenToPlayer 0x0069A420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::ClearUnused() | [source/game_sa/PedList.cpp:121](../../source/game_sa/PedList.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::AddMember(CPed* ped) | [source/game_sa/PedList.cpp:126](../../source/game_sa/PedList.cpp#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedList::RemoveMemberNoFill(int32 i) | [source/game_sa/PedList.cpp:135](../../source/game_sa/PedList.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPedList::Get(int32 i) | [source/game_sa/PedList.cpp:140](../../source/game_sa/PedList.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPeds() const | [source/game_sa/PedList.h:38](../../source/game_sa/PedList.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-403"></a>

## CPedLookAtAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedLookAtAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedLookAtAttractor.h:10](../../source/game_sa/Attractors/PedLookAtAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedLookAtAttractor.h:28](../../source/game_sa/Attractors/PedLookAtAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-404"></a>

## CPedModelInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedModelInfo::InjectHooks() | [source/game_sa/Models/PedModelInfo.cpp:26](../../source/game_sa/Models/PedModelInfo.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedModelInfo::GetModelType() | [source/game_sa/Models/PedModelInfo.cpp:42](../../source/game_sa/Models/PedModelInfo.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::GetModelType 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedModelInfo::DeleteRwObject() | [source/game_sa/Models/PedModelInfo.cpp:48](../../source/game_sa/Models/PedModelInfo.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::DeleteRwObject 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedModelInfo::SetClump(RpClump* clump) | [source/game_sa/Models/PedModelInfo.cpp:60](../../source/game_sa/Models/PedModelInfo.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::SetClump 0x00000000 | 0 / 0 | Posible dato fijo m_pPedIds: source/game_sa/Models/PedModelInfo.h:48 (0x008A6268) |
| CPedModelInfo::AddXtraAtomics(RpClump* clump) | [source/game_sa/Models/PedModelInfo.cpp:72](../../source/game_sa/Models/PedModelInfo.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::AddXtraAtomics 0x004C6D40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedModelInfo::SetFaceTexture(RwTexture* texture) | [source/game_sa/Models/PedModelInfo.cpp:77](../../source/game_sa/Models/PedModelInfo.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::SetFaceTexture 0x004C6D50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedModelInfo::CreateHitColModelSkinned(RpClump* clump) | [source/game_sa/Models/PedModelInfo.cpp:88](../../source/game_sa/Models/PedModelInfo.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::CreateHitColModelSkinned 0x004C6D90 | 0 / 0 | Posible dato fijo m_pColNodeInfos: source/game_sa/Models/PedModelInfo.h:49 (0x008A6308); Posible dato fijo m_pWorkingMatrix1: source/game_sa/Game.h:22 (0x00B72920); Posible dato fijo m_pWorkingMatrix2: source/game_sa/Game.h:23 (0x00B72924) |
| * CPedModelInfo::AnimatePedColModelSkinned(RpClump* clump) | [source/game_sa/Models/PedModelInfo.cpp:122](../../source/game_sa/Models/PedModelInfo.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::AnimatePedColModelSkinned 0x004C6F70 | 0 / 0 | Posible dato fijo m_pColNodeInfos: source/game_sa/Models/PedModelInfo.h:49 (0x008A6308); Posible dato fijo m_pWorkingMatrix1: source/game_sa/Game.h:22 (0x00B72920); Posible dato fijo m_pWorkingMatrix2: source/game_sa/Game.h:23 (0x00B72924) |
| * CPedModelInfo::AnimatePedColModelSkinnedWorld(RpClump* clump) | [source/game_sa/Models/PedModelInfo.cpp:161](../../source/game_sa/Models/PedModelInfo.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::AnimatePedColModelSkinnedWorld 0x004C7170 | 0 / 0 | Posible dato fijo m_pColNodeInfos: source/game_sa/Models/PedModelInfo.h:49 (0x008A6308) |
| CPedModelInfo::IncrementVoice() | [source/game_sa/Models/PedModelInfo.cpp:193](../../source/game_sa/Models/PedModelInfo.cpp#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedModelInfo::IncrementVoice 0x004C7300 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedModelInfo() | [source/game_sa/Models/PedModelInfo.h:54](../../source/game_sa/Models/PedModelInfo.h#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~CPedModelInfo() | [source/game_sa/Models/PedModelInfo.h:55](../../source/game_sa/Models/PedModelInfo.h#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetRace() const | [source/game_sa/Models/PedModelInfo.h:68](../../source/game_sa/Models/PedModelInfo.h#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedType() const | [source/game_sa/Models/PedModelInfo.h:69](../../source/game_sa/Models/PedModelInfo.h#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedStatType() const | [source/game_sa/Models/PedModelInfo.h:70](../../source/game_sa/Models/PedModelInfo.h#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanPedDriveVehicleClass(eVehicleClass cls) const | [source/game_sa/Models/PedModelInfo.h:71](../../source/game_sa/Models/PedModelInfo.h#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale() const noexcept | [source/game_sa/Models/PedModelInfo.h:72](../../source/game_sa/Models/PedModelInfo.h#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-405"></a>

## CPedParkAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedParkAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedParkAttractor.h:10](../../source/game_sa/Attractors/PedParkAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedParkAttractor.h:28](../../source/game_sa/Attractors/PedParkAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-406"></a>

## CPedPizzaAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedPizzaAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedPizzaAttractor.h:10](../../source/game_sa/Attractors/PedPizzaAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedPizzaAttractor.h:28](../../source/game_sa/Attractors/PedPizzaAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeadOfQueueWaitTime() const | [source/game_sa/Attractors/PedPizzaAttractor.h:31](../../source/game_sa/Attractors/PedPizzaAttractor.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-407"></a>

## CPedPlacement

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedPlacement::InjectHooks() | [source/game_sa/PedPlacement.cpp:29](../../source/game_sa/PedPlacement.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedPlacement::FindZCoorForPed(CVector pos) | [source/game_sa/PedPlacement.cpp:40](../../source/game_sa/PedPlacement.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedPlacement::IsPositionClearForPed(const CVector& pos, float radius, int32 maxHitEntities, CEntity** outHitEntities, bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects) | [source/game_sa/PedPlacement.cpp:60](../../source/game_sa/PedPlacement.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedPlacement::IsPositionClearForPed-OG 0x00616860 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedPlacement::IsPositionClearForPed( const CVector& pos, float radius, std::initializer_list&lt;CEntity*&gt; ignoreEntities, bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects ) | [source/game_sa/PedPlacement.cpp:79](../../source/game_sa/PedPlacement.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedPlacement::IsPositionClearForPed-OG 0x00616860 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPedPlacement::IsPositionClearOfCars(const CVector* pos) | [source/game_sa/PedPlacement.cpp:103](../../source/game_sa/PedPlacement.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedPlacement::IsPositionClearOfCars-Pos 0x006168E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPedPlacement::IsPositionClearOfCars(const CPed* ped) | [source/game_sa/PedPlacement.cpp:108](../../source/game_sa/PedPlacement.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedPlacement::IsPositionClearOfCars-Ped 0x00616A40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-408"></a>

## CPedPool

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedPool::GetRef(const CPed* ped) | [source/game_sa/Pools.cpp:51](../../source/game_sa/Pools.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedPool::GetRef 0x004442D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InjectHooks() | [source/game_sa/Pools/PedPool.h:37](../../source/game_sa/Pools/PedPool.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * New() | [source/game_sa/Pools/PedPool.h:53](../../source/game_sa/Pools/PedPool.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedPool::New 0x005E45E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-409"></a>

## CPedSaveStructure

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedSaveStructure::Extract(CPed* ped) | [source/game_sa/Entity/Ped/PedSaveStructure.cpp:27](../../source/game_sa/Entity/Ped/PedSaveStructure.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSaveStructure::Extract 0x005D44B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedSaveStructure::Construct(CPed* ped) | [source/game_sa/Entity/Ped/PedSaveStructure.cpp:75](../../source/game_sa/Entity/Ped/PedSaveStructure.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedSaveStructure::Construct 0x005D43D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedSaveStructure::InjectHooks() | [source/game_sa/Entity/Ped/PedSaveStructure.cpp:114](../../source/game_sa/Entity/Ped/PedSaveStructure.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-410"></a>

## CPedScanner

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedScanner::ScanForPedsInRange(const CPed& ped) | [source/game_sa/PedScanner.cpp:31](../../source/game_sa/PedScanner.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPedScanner::GetClosestPedInRange() const | [source/game_sa/PedScanner.cpp:37](../../source/game_sa/PedScanner.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedScanner::GetClosestPedInRange 0x005FFF20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-411"></a>

## CPedScriptedAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedScriptedAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedScriptedAttractor.h:10](../../source/game_sa/Attractors/PedScriptedAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedScriptedAttractor.h:28](../../source/game_sa/Attractors/PedScriptedAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-412"></a>

## CPedScriptedTaskRecord

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedScriptedTaskRecord::InjectHooks() | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:45](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPedScriptedTaskRecord::GetRecordAssociatedWithEvent(CEvent* event) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:148](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecord::GetRecordAssociatedWithEvent 0x00608580 | 0 / 0 | Posible dato fijo ms_scriptedTasks: source/game_sa/Tasks/PedScriptedTaskRecord.h:49 (0x00C0B1E8) |
| CPedScriptedTaskRecord::GetStatus(CPed* ped) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:158](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L158) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecord::GetStatus- 0x00608750 | 0 / 0 | Posible dato fijo ms_scriptedTasks: source/game_sa/Tasks/PedScriptedTaskRecord.h:49 (0x00C0B1E8) |
| CPedScriptedTaskRecord::GetStatus(CPed* ped, int32 opcode) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:168](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecord::GetStatus-opcode 0x00608710 | 0 / 0 | Posible dato fijo ms_scriptedTasks: source/game_sa/Tasks/PedScriptedTaskRecord.h:49 (0x00C0B1E8) |
| CPedScriptedTaskRecord::GetVacantSlot() | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:178](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecord::GetVacantSlot 0x00608540 | 0 / 0 | Posible dato fijo ms_scriptedTasks: source/game_sa/Tasks/PedScriptedTaskRecord.h:49 (0x00C0B1E8) |
| CPedScriptedTaskRecord::Process() | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:189](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecord::Process 0x006085E0 | 0 / 0 | Posible dato fijo ms_scriptedTasks: source/game_sa/Tasks/PedScriptedTaskRecord.h:49 (0x00C0B1E8) |

<a id="grupo-413"></a>

## CPedScriptedTaskRecordData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedScriptedTaskRecordData::InjectHooks() | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:30](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::CPedScriptedTaskRecordData() | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:57](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPedScriptedTaskRecordData::Constructor() | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:67](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::Constructor 0x00608330 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::AssociateWithTask(CTask* task) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:74](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::AssociateWithTask 0x00608520 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::AssociateWithEvent(CEventScriptCommand* event) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:82](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::AssociateWithEvent 0x00608500 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::Set(CPed* ped, int32 opcode, CEventScriptCommand* event) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:89](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::Set-CEventScriptCommand 0x00608390 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::Set(CPed* ped, int32 opcode, CTask* task) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:101](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::Set-CTask 0x006083E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::SetAsGroupTask(CPed* ped, int32 opcode, CTask* task) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:113](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::SetAsGroupTask 0x006084A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::SetAsAttractorScriptTask(CPed* ped, int32 opcode, CTask* task) | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:125](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::SetAsAttractorScriptTask 0x00608440 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedTaskRecordData::Flush() | [source/game_sa/Tasks/PedScriptedTaskRecord.cpp:137](../../source/game_sa/Tasks/PedScriptedTaskRecord.cpp#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedScriptedTaskRecordData::Flush 0x00608350 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-414"></a>

## CPedSeatAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedSeatAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedSeatAttractor.h:10](../../source/game_sa/Attractors/PedSeatAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedSeatAttractor.h:28](../../source/game_sa/Attractors/PedSeatAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-415"></a>

## CPedShelterAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & CPedShelterAttractor::GetDisplacement(int32 pedId) | [source/game_sa/Attractors/PedShelterAttractor.cpp:27](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedShelterAttractor::GetDisplacement 0x005EF420 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo ms_displacements: source/game_sa/Attractors/PedShelterAttractor.h:7 (0x00C0986C) |
| CPedShelterAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) | [source/game_sa/Attractors/PedShelterAttractor.cpp:68](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedShelterAttractor::ComputeAttractPos 0x005EFC40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedShelterAttractor::ComputeAttractHeading(int32 bQueue, float& heading) | [source/game_sa/Attractors/PedShelterAttractor.cpp:78](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedShelterAttractor::ComputeAttractHeading 0x005E9690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedShelterAttractor::BroadcastDeparture(CPed* ped) | [source/game_sa/Attractors/PedShelterAttractor.cpp:84](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedShelterAttractor::BroadcastDeparture 0x005EF570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedShelterAttractor::InjectHooks() | [source/game_sa/Attractors/PedShelterAttractor.cpp:117](../../source/game_sa/Attractors/PedShelterAttractor.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedShelterAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedShelterAttractor.h:13](../../source/game_sa/Attractors/PedShelterAttractor.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedShelterAttractor.h:31](../../source/game_sa/Attractors/PedShelterAttractor.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsAtHeadOfQueue(CPed* ped) override | [source/game_sa/Attractors/PedShelterAttractor.h:34](../../source/game_sa/Attractors/PedShelterAttractor.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-416"></a>

## CPedStats

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedStats::InjectHooks() | [source/game_sa/PedStats.cpp:27](../../source/game_sa/PedStats.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedStats::Initialise() | [source/game_sa/PedStats.cpp:39](../../source/game_sa/PedStats.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedStats::Initialise 0x005BF9D0 | 0 / 0 | Posible dato fijo ms_apPedStats: source/game_sa/PedStats.h:25 (0x00C0BBEC) |
| CPedStats::Shutdown() | [source/game_sa/PedStats.cpp:59](../../source/game_sa/PedStats.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedStats::Shutdown 0x00608850 | 0 / 0 | Posible dato fijo ms_apPedStats: source/game_sa/PedStats.h:25 (0x00C0BBEC) |
| CPedStats::LoadPedStats() | [source/game_sa/PedStats.cpp:64](../../source/game_sa/PedStats.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedStats::LoadPedStats 0x005BB890 | 0 / 0 | Posible dato fijo ms_apPedStats: source/game_sa/PedStats.h:25 (0x00C0BBEC) |
| CPedStats::GetPedStatType(const char* statName) | [source/game_sa/PedStats.cpp:119](../../source/game_sa/PedStats.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedStats::GetPedStatType 0x006088D0 | 0 / 0 | Posible dato fijo ms_apPedStats: source/game_sa/PedStats.h:25 (0x00C0BBEC) |
| * CPedStats::GetPedStatInfo(const char* statName) | [source/game_sa/PedStats.cpp:130](../../source/game_sa/PedStats.cpp#L130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_apPedStats: source/game_sa/PedStats.h:25 (0x00C0BBEC) |
| * CPedStats::GetPedStatByArrayIndex(uint32 statIndex) | [source/game_sa/PedStats.cpp:141](../../source/game_sa/PedStats.cpp#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_apPedStats: source/game_sa/PedStats.h:25 (0x00C0BBEC) |
| CPedStats::FindIndexWithPedStat(void* a1) | [source/game_sa/PedStats.cpp:147](../../source/game_sa/PedStats.cpp#L147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStats::FindIndexWithPedStat 0x00608940 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo ms_apPedStats: source/game_sa/PedStats.h:25 (0x00C0BBEC) |

<a id="grupo-417"></a>

## CPedStepAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedStepAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedStepAttractor.h:10](../../source/game_sa/Attractors/PedStepAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedStepAttractor.h:28](../../source/game_sa/Attractors/PedStepAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-418"></a>

## CPedStopAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedStopAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedStopAttractor.h:10](../../source/game_sa/Attractors/PedStopAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedStopAttractor.h:28](../../source/game_sa/Attractors/PedStopAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-419"></a>

## CPedStuckChecker

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedStuckChecker::InjectHooks() | [source/game_sa/PedStuckChecker.cpp:29](../../source/game_sa/PedStuckChecker.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedStuckChecker::TestPedStuck(CPed* ped, CEventGroup* eventGroup) | [source/game_sa/PedStuckChecker.cpp:40](../../source/game_sa/PedStuckChecker.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedStuckChecker::TestPedStuck 0x00602C00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_FrameCounter: source/game_sa/Timer.h:31 (0x00B7CB4C); Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| CPedStuckChecker() | [source/game_sa/PedStuckChecker.h:23](../../source/game_sa/PedStuckChecker.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetState() const | [source/game_sa/PedStuckChecker.h:30](../../source/game_sa/PedStuckChecker.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-420"></a>

## CPedTaskPair

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedTaskPair::InjectHooks() | [source/game_sa/PedTaskPair.cpp:23](../../source/game_sa/PedTaskPair.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedTaskPair::Flush() | [source/game_sa/PedTaskPair.cpp:31](../../source/game_sa/PedTaskPair.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedTaskPair::Flush 0x005E95B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
