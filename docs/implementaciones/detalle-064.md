<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 927 grupos / 11134 funciones reconstruidas; 11134 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1261"></a>

## Globales y helpers: source/game_sa/ModelIndices.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatchModelStringUint16(char* name, ModelIndex) | [source/game_sa/ModelIndices.cpp:503](../../source/game_sa/ModelIndices.cpp#L503) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1262"></a>

## Globales y helpers: source/game_sa/Models/AtomicModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicModelInfoDestructorBody(CAtomicModelInfo* self, void* /*edx*/) | [source/game_sa/Models/AtomicModelInfo.cpp:48](../../source/game_sa/Models/AtomicModelInfo.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAtomicModelInfo::DestructorBody 0x004C5590 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1263"></a>

## Globales y helpers: source/game_sa/Models/BaseModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetBaseModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/BaseModelInfo.cpp:38](../../source/game_sa/Models/BaseModelInfo.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1264"></a>

## Globales y helpers: source/game_sa/Models/ClumpModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetClumpModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/ClumpModelInfo.cpp:41](../../source/game_sa/Models/ClumpModelInfo.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpModelInfoDestructorBody(CClumpModelInfo* self, void* /*edx*/) | [source/game_sa/Models/ClumpModelInfo.cpp:61](../../source/game_sa/Models/ClumpModelInfo.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CClumpModelInfo::DestructorBody 0x004C5750 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetClumpModelInfoFlags(CClumpModelInfo* modelInfo, uint32 dwFlags) | [source/game_sa/Models/ClumpModelInfo.cpp:430](../../source/game_sa/Models/ClumpModelInfo.cpp#L430) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1265"></a>

## Globales y helpers: source/game_sa/Models/DamageAtomicModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DamageAtomicModelInfoDestructorBody(CDamageAtomicModelInfo* self, void* /*edx*/) | [source/game_sa/Models/DamageAtomicModelInfo.cpp:40](../../source/game_sa/Models/DamageAtomicModelInfo.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CDamageAtomicModelInfo::DestructorBody 0x004C55D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1266"></a>

## Globales y helpers: source/game_sa/Models/LodTimeModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LodTimeModelInfoDestructorBody(CLodTimeModelInfo* self, void* /*edx*/) | [source/game_sa/Models/LodTimeModelInfo.cpp:26](../../source/game_sa/Models/LodTimeModelInfo.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CLodTimeModelInfo::Destructor 0x004C56E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1267"></a>

## Globales y helpers: source/game_sa/Models/ModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ConstructModelInfoStore(CStore&lt;T, Capacity&gt;* self, uintptr_t elementConstructor) | [source/game_sa/Models/ModelInfo.cpp:47](../../source/game_sa/Models/ModelInfo.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1268"></a>

## Globales y helpers: source/game_sa/Models/TimeModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TimeModelInfoDestructorBody(CTimeModelInfo* self, void* /*edx*/) | [source/game_sa/Models/TimeModelInfo.cpp:27](../../source/game_sa/Models/TimeModelInfo.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTimeModelInfo::Destructor 0x004C5680 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1269"></a>

## Globales y helpers: source/game_sa/Models/VehicleModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleModelInfoDestructorBody(CVehicleModelInfo* self, void* /*edx*/) | [source/game_sa/Models/VehicleModelInfo.cpp:62](../../source/game_sa/Models/VehicleModelInfo.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleModelInfo::DestructorBody 0x004C5920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall VehicleStructureConstructorExe(CVehicleStructure* self, void* /*edx*/) | [source/game_sa/Models/VehicleModelInfo.cpp:105](../../source/game_sa/Models/VehicleModelInfo.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall VehicleStructurePoolNewExe(CPool&lt;CVehicleStructure&gt;* pool, void* /*edx*/) | [source/game_sa/Models/VehicleModelInfo.cpp:111](../../source/game_sa/Models/VehicleModelInfo.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleStructure::PoolNew 0x004C94C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehicleStructureOperatorDeleteExe(void* data) | [source/game_sa/Models/VehicleModelInfo.cpp:117](../../source/game_sa/Models/VehicleModelInfo.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleStructure::OperatorDelete 0x004C9580 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_pInfoPool: source/game_sa/Models/VehicleModelInfo.h:190 (0x00B4E680) |
| * __fastcall VehicleStructureScalarDeletingDestructorExe(CVehicleStructure* self, void* /*edx*/, uint8 flags) | [source/game_sa/Models/VehicleModelInfo.cpp:123](../../source/game_sa/Models/VehicleModelInfo.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleStructure::ScalarDeletingDestructor 0x004C9840 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_pInfoPool: source/game_sa/Models/VehicleModelInfo.h:190 (0x00B4E680) |
| ChooseComponent(int32 rule, int32 comps) | [source/game_sa/Models/VehicleModelInfo.cpp:1811](../../source/game_sa/Models/VehicleModelInfo.cpp#L1811) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicleModelInfo::ChooseComponent-Global 0x004C7FB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1270"></a>

## Globales y helpers: source/game_sa/Models/WeaponModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WeaponModelInfoDestructorBody(CWeaponModelInfo* self, void* /*edx*/) | [source/game_sa/Models/WeaponModelInfo.cpp:38](../../source/game_sa/Models/WeaponModelInfo.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CWeaponModelInfo::DestructorBody 0x004C5790 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1271"></a>

## Globales y helpers: source/game_sa/ObjectScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjectScannerDestructorBody(CObjectScanner* self, void* /*edx*/) | [source/game_sa/ObjectScanner.cpp:42](../../source/game_sa/ObjectScanner.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CObjectScanner::Destructor 0x00606050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1272"></a>

## Globales y helpers: source/game_sa/Occluder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MultiplyMatrixWithVector_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Occluder.cpp:36](../../source/game_sa/Occluder.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcScreenCoorsWithScale(const CVector& in, CVector& out) | [source/game_sa/Occluder.cpp:45](../../source/game_sa/Occluder.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcClippedScreenCoors(int32 idxFrom, int32 idxTo, CVector& out) | [source/game_sa/Occluder.cpp:52](../../source/game_sa/Occluder.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo OccluderCoors: source/game_sa/Occluder.h:17 (0x00C798E0); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| RawAngle(const COccluder* o, size_t offset) | [source/game_sa/Occluder.cpp:65](../../source/game_sa/Occluder.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1273"></a>

## Globales y helpers: source/game_sa/Pad.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PadLeftShoulder1PressedExe(const CPad* self, void* /*edx*/) | [source/game_sa/Pad.cpp:53](../../source/game_sa/Pad.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPad::IsLeftShoulder1Pressed 0x0053EDC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PadRightShoulder1PressedExe(const CPad* self, void* /*edx*/) | [source/game_sa/Pad.cpp:55](../../source/game_sa/Pad.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPad::IsRightShoulder1Pressed 0x0053EE20 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PadDPadUpPressedExe(const CPad* self, void* /*edx*/) | [source/game_sa/Pad.cpp:57](../../source/game_sa/Pad.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPad::IsDPadUpPressed 0x0053EE60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PadTrianglePressedExe(const CPad* self, void* /*edx*/) | [source/game_sa/Pad.cpp:59](../../source/game_sa/Pad.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPad::IsTrianglePressed 0x0053EF40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PadCirclePressedExe(const CPad* self, void* /*edx*/) | [source/game_sa/Pad.cpp:61](../../source/game_sa/Pad.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPad::IsCirclePressed 0x0053EF60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PadDPadRightPressedExe(const CPad* self, void* /*edx*/) | [source/game_sa/Pad.cpp:64](../../source/game_sa/Pad.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PadCrossPressedExe(const CPad* self, void* /*edx*/) | [source/game_sa/Pad.cpp:67](../../source/game_sa/Pad.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPad::IsCrossPressed 0x004D59E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PadStandardKeyJustPressedExe(const CPad* /*self*/, void* /*edx*/, uint8 key) | [source/game_sa/Pad.cpp:70](../../source/game_sa/Pad.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPad::IsStandardKeyJustPressed 0x004D59B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo NewKeyState: source/game_sa/Pad.h:109 (0x00B73190); Posible dato fijo OldKeyState: source/game_sa/Pad.h:108 (0x00B72F20) |
| NormalizePadAxis(LONG value) | [source/game_sa/Pad.cpp:382](../../source/game_sa/Pad.cpp#L382) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InitialiseMouse(bool exclusive) | [source/game_sa/Pad.cpp:1604](../../source/game_sa/Pad.cpp#L1604) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1274"></a>

## Globales y helpers: source/game_sa/PatrolRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyZ(char* dst, const char* src) | [source/game_sa/PatrolRoute.cpp:87](../../source/game_sa/PatrolRoute.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1275"></a>

## Globales y helpers: source/game_sa/PedAttractorManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorsRemoveEffect(const C2dEffectPedAttractor* fx, const SArray&lt;CPedAttractor*&gt;* attractors) | [source/game_sa/PedAttractorManager.cpp:44](../../source/game_sa/PedAttractorManager.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedAttractorManager::RemoveEffectCPedAttractor 0x005EB5F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorsIsPedRegistered(CPed* ped, const SArray&lt;CPedAttractor*&gt;* attractors) | [source/game_sa/PedAttractorManager.cpp:57](../../source/game_sa/PedAttractorManager.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedAttractorManager::IsPedRegisteredCPedAttractor 0x005EB640 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorsIsPedRegisteredWithEffect(CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity, const SArray&lt;CPedAttractor*&gt;* attractors) | [source/game_sa/PedAttractorManager.cpp:70](../../source/game_sa/PedAttractorManager.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedAttractorManager::IsPedRegisteredWithEffectCPedAttractor 0x005EB690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorListContains(const AttractorList& attractors, const CPedAttractor* attractor) | [source/game_sa/PedAttractorManager.cpp:132](../../source/game_sa/PedAttractorManager.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveFromListAndDelete(AttractorList& attractors, CPedAttractor* attractor) | [source/game_sa/PedAttractorManager.cpp:144](../../source/game_sa/PedAttractorManager.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValidAttractorType(int32 type) | [source/game_sa/PedAttractorManager.cpp:274](../../source/game_sa/PedAttractorManager.cpp#L274) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/PedAttractorManager.cpp:448](../../source/game_sa/PedAttractorManager.cpp#L448) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/PedAttractorManager.cpp:457](../../source/game_sa/PedAttractorManager.cpp#L457) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1276"></a>

## Globales y helpers: source/game_sa/PedGeometryAnalyser.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPointRouteInjectHooksLote109A() | [source/game_sa/PedGeometryAnalyser.cpp:842](../../source/game_sa/PedGeometryAnalyser.cpp#L842) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1277"></a>

## Globales y helpers: source/game_sa/PedGroupPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GroupOriginCanBeUsed(const CVector& origin, float radius) | [source/game_sa/PedGroupPlacer.cpp:39](../../source/game_sa/PedGroupPlacer.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| InstallGroupPed(CPedGroup& group, CPed* ped, int32 groupId, bool leader, bool setLeader) | [source/game_sa/PedGroupPlacer.cpp:48](../../source/game_sa/PedGroupPlacer.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaceCircularGroup(ePedType type, uint32 numPeds, const CVector& origin, bool random) | [source/game_sa/PedGroupPlacer.cpp:55](../../source/game_sa/PedGroupPlacer.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1278"></a>

## Globales y helpers: source/game_sa/PedGroups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeEdxAfterRemoveMember(const CPedGroup& group, int32 idx, uint32 edx) | [source/game_sa/PedGroups.cpp:43](../../source/game_sa/PedGroups.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeEdxAfterFlush(const CPedGroup& group, uint32 edx) | [source/game_sa/PedGroups.cpp:66](../../source/game_sa/PedGroups.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B744A8 |
| EmptyGroupLikeExe(CPedGroup& group, ExeRegs* regs) | [source/game_sa/PedGroups.cpp:81](../../source/game_sa/PedGroups.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddGroupCore(ExeRegs* regs) | [source/game_sa/PedGroups.cpp:101](../../source/game_sa/PedGroups.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedGroups::AddGroup 0x005FB800 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo ms_activeGroups: source/game_sa/PedGroups.h:17 (0x00C098E0); Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| InitCore(ExeRegs* regs) | [source/game_sa/PedGroups.cpp:119](../../source/game_sa/PedGroups.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedGroups::Init 0x005FB8C0 | 0 / 0 | Posible dato fijo ScriptReferenceIndex: source/game_sa/DecisionMakers/DecisionMakerTypes.h:45 (0x00C0AFF4), source/game_sa/PedGroups.h:16 (0x00C098D0), source/game_sa/Scripts/Scripted2dEffects.h:66 (0x00C3A120); Posible dato fijo ms_activeGroups: source/game_sa/PedGroups.h:17 (0x00C098E0); Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| CleanUpForShutDownCore(ExeRegs* regs) | [source/game_sa/PedGroups.cpp:133](../../source/game_sa/PedGroups.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedGroups::CleanUpForShutDown 0x005FB930 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo ms_groups: source/game_sa/PedGroups.h:20 (0x00C09920) |
| AddGroupExe(uint32 entryEcx, uint32 entryEdx) | [source/game_sa/PedGroups.cpp:143](../../source/game_sa/PedGroups.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InitExe(uint32 entryEcx, uint32 entryEdx) | [source/game_sa/PedGroups.cpp:149](../../source/game_sa/PedGroups.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CleanUpForShutDownExe(uint32 entryEcx, uint32 entryEdx) | [source/game_sa/PedGroups.cpp:157](../../source/game_sa/PedGroups.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1279"></a>

## Globales y helpers: source/game_sa/PedIK.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SinCosExe(float angle, float& outSin, float& outCos) | [source/game_sa/PedIK.cpp:45](../../source/game_sa/PedIK.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateXExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:50](../../source/game_sa/PedIK.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RotateYExe(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:54](../../source/game_sa/PedIK.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1280"></a>

## Globales y helpers: source/game_sa/PedIntelligence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IncrementAngerAtPlayerRegs(CPedIntelligence* self, uint32 entryEdx, uint8 anger) | [source/game_sa/PedIntelligence.cpp:1066](../../source/game_sa/PedIntelligence.cpp#L1066) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |
