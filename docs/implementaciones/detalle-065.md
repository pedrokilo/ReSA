<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11695 con evidencia histórica y 746 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1281"></a>

## Globales y helpers: source/game_sa/Messages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearMessagesTextAndString(std::array&lt;tMessage, N&gt;& msgs) | [source/game_sa/Messages.cpp:64](../../source/game_sa/Messages.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveMessageAt(std::array&lt;tMessage, N&gt;& msgs, int16 i) | [source/game_sa/Messages.cpp:75](../../source/game_sa/Messages.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MessageTextEquals(const GxtChar* a, const GxtChar* b) | [source/game_sa/Messages.cpp:91](../../source/game_sa/Messages.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindUnusedMsgInArray(std::array&lt;tMessage, N&gt;& arr) | [source/game_sa/Messages.cpp:113](../../source/game_sa/Messages.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1282"></a>

## Globales y helpers: source/game_sa/MissionCleanup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & Raw(uintptr_t address) | [source/game_sa/MissionCleanup.cpp:86](../../source/game_sa/MissionCleanup.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & RawAt(void* base, uintptr_t offset) | [source/game_sa/MissionCleanup.cpp:87](../../source/game_sa/MissionCleanup.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PoolEntity(uintptr_t poolGlobal, int32 handle, int32 entitySize) | [source/game_sa/MissionCleanup.cpp:91](../../source/game_sa/MissionCleanup.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1283"></a>

## Globales y helpers: source/game_sa/ModelIndices.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatchModelStringUint16(char* name, ModelIndex) | [source/game_sa/ModelIndices.cpp:503](../../source/game_sa/ModelIndices.cpp#L503) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1284"></a>

## Globales y helpers: source/game_sa/Models/AtomicModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AtomicModelInfoDestructorBody(CAtomicModelInfo* self, void* /*edx*/) | [source/game_sa/Models/AtomicModelInfo.cpp:48](../../source/game_sa/Models/AtomicModelInfo.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAtomicModelInfo::DestructorBody 0x004C5590 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1285"></a>

## Globales y helpers: source/game_sa/Models/BaseModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetBaseModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/BaseModelInfo.cpp:38](../../source/game_sa/Models/BaseModelInfo.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1286"></a>

## Globales y helpers: source/game_sa/Models/ClumpModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetClumpModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/ClumpModelInfo.cpp:41](../../source/game_sa/Models/ClumpModelInfo.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClumpModelInfoDestructorBody(CClumpModelInfo* self, void* /*edx*/) | [source/game_sa/Models/ClumpModelInfo.cpp:61](../../source/game_sa/Models/ClumpModelInfo.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CClumpModelInfo::DestructorBody 0x004C5750 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetClumpModelInfoFlags(CClumpModelInfo* modelInfo, uint32 dwFlags) | [source/game_sa/Models/ClumpModelInfo.cpp:430](../../source/game_sa/Models/ClumpModelInfo.cpp#L430) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1287"></a>

## Globales y helpers: source/game_sa/Models/DamageAtomicModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DamageAtomicModelInfoDestructorBody(CDamageAtomicModelInfo* self, void* /*edx*/) | [source/game_sa/Models/DamageAtomicModelInfo.cpp:40](../../source/game_sa/Models/DamageAtomicModelInfo.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CDamageAtomicModelInfo::DestructorBody 0x004C55D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1288"></a>

## Globales y helpers: source/game_sa/Models/LodTimeModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LodTimeModelInfoDestructorBody(CLodTimeModelInfo* self, void* /*edx*/) | [source/game_sa/Models/LodTimeModelInfo.cpp:26](../../source/game_sa/Models/LodTimeModelInfo.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CLodTimeModelInfo::Destructor 0x004C56E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1289"></a>

## Globales y helpers: source/game_sa/Models/ModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ConstructModelInfoStore(CStore&lt;T, Capacity&gt;* self, uintptr_t elementConstructor) | [source/game_sa/Models/ModelInfo.cpp:47](../../source/game_sa/Models/ModelInfo.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1290"></a>

## Globales y helpers: source/game_sa/Models/PedModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedModelInfoConstructorBody(CPedModelInfo* self, void* /*edx*/) | [source/game_sa/Models/PedModelInfo.cpp:35](../../source/game_sa/Models/PedModelInfo.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1291"></a>

## Globales y helpers: source/game_sa/Models/TimeModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TimeModelInfoDestructorBody(CTimeModelInfo* self, void* /*edx*/) | [source/game_sa/Models/TimeModelInfo.cpp:27](../../source/game_sa/Models/TimeModelInfo.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CTimeModelInfo::Destructor 0x004C5680 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1292"></a>

## Globales y helpers: source/game_sa/Models/VehicleModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleModelInfoDestructorBody(CVehicleModelInfo* self, void* /*edx*/) | [source/game_sa/Models/VehicleModelInfo.cpp:62](../../source/game_sa/Models/VehicleModelInfo.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleModelInfo::DestructorBody 0x004C5920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall VehicleStructureConstructorExe(CVehicleStructure* self, void* /*edx*/) | [source/game_sa/Models/VehicleModelInfo.cpp:105](../../source/game_sa/Models/VehicleModelInfo.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall VehicleStructurePoolNewExe(CPool&lt;CVehicleStructure&gt;* pool, void* /*edx*/) | [source/game_sa/Models/VehicleModelInfo.cpp:111](../../source/game_sa/Models/VehicleModelInfo.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleStructure::PoolNew 0x004C94C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehicleStructureOperatorDeleteExe(void* data) | [source/game_sa/Models/VehicleModelInfo.cpp:117](../../source/game_sa/Models/VehicleModelInfo.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleStructure::OperatorDelete 0x004C9580 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_pInfoPool: source/game_sa/Models/VehicleModelInfo.h:190 (0x00B4E680) |
| * __fastcall VehicleStructureScalarDeletingDestructorExe(CVehicleStructure* self, void* /*edx*/, uint8 flags) | [source/game_sa/Models/VehicleModelInfo.cpp:123](../../source/game_sa/Models/VehicleModelInfo.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVehicleStructure::ScalarDeletingDestructor 0x004C9840 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_pInfoPool: source/game_sa/Models/VehicleModelInfo.h:190 (0x00B4E680) |
| ChooseComponent(int32 rule, int32 comps) | [source/game_sa/Models/VehicleModelInfo.cpp:1811](../../source/game_sa/Models/VehicleModelInfo.cpp#L1811) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicleModelInfo::ChooseComponent-Global 0x004C7FB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1293"></a>

## Globales y helpers: source/game_sa/Models/WeaponModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WeaponModelInfoDestructorBody(CWeaponModelInfo* self, void* /*edx*/) | [source/game_sa/Models/WeaponModelInfo.cpp:38](../../source/game_sa/Models/WeaponModelInfo.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CWeaponModelInfo::DestructorBody 0x004C5790 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1294"></a>

## Globales y helpers: source/game_sa/ObjectScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjectScannerDestructorBody(CObjectScanner* self, void* /*edx*/) | [source/game_sa/ObjectScanner.cpp:42](../../source/game_sa/ObjectScanner.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CObjectScanner::Destructor 0x00606050 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1295"></a>

## Globales y helpers: source/game_sa/Occluder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MultiplyMatrixWithVector_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Occluder.cpp:36](../../source/game_sa/Occluder.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcScreenCoorsWithScale(const CVector& in, CVector& out) | [source/game_sa/Occluder.cpp:45](../../source/game_sa/Occluder.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcClippedScreenCoors(int32 idxFrom, int32 idxTo, CVector& out) | [source/game_sa/Occluder.cpp:52](../../source/game_sa/Occluder.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo OccluderCoors: source/game_sa/Occluder.h:17 (0x00C798E0); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| RawAngle(const COccluder* o, size_t offset) | [source/game_sa/Occluder.cpp:65](../../source/game_sa/Occluder.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1296"></a>

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

<a id="grupo-1297"></a>

## Globales y helpers: source/game_sa/PatrolRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyZ(char* dst, const char* src) | [source/game_sa/PatrolRoute.cpp:101](../../source/game_sa/PatrolRoute.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1298"></a>

## Globales y helpers: source/game_sa/PedAttractorManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorsRemoveEffect(const C2dEffectPedAttractor* fx, const SArray&lt;CPedAttractor*&gt;* attractors) | [source/game_sa/PedAttractorManager.cpp:44](../../source/game_sa/PedAttractorManager.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedAttractorManager::RemoveEffectCPedAttractor 0x005EB5F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorsIsPedRegistered(CPed* ped, const SArray&lt;CPedAttractor*&gt;* attractors) | [source/game_sa/PedAttractorManager.cpp:57](../../source/game_sa/PedAttractorManager.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedAttractorManager::IsPedRegisteredCPedAttractor 0x005EB640 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorsIsPedRegisteredWithEffect(CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity, const SArray&lt;CPedAttractor*&gt;* attractors) | [source/game_sa/PedAttractorManager.cpp:70](../../source/game_sa/PedAttractorManager.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPedAttractorManager::IsPedRegisteredWithEffectCPedAttractor 0x005EB690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AttractorListContains(const AttractorList& attractors, const CPedAttractor* attractor) | [source/game_sa/PedAttractorManager.cpp:132](../../source/game_sa/PedAttractorManager.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveFromListAndDelete(AttractorList& attractors, CPedAttractor* attractor) | [source/game_sa/PedAttractorManager.cpp:144](../../source/game_sa/PedAttractorManager.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValidAttractorType(int32 type) | [source/game_sa/PedAttractorManager.cpp:344](../../source/game_sa/PedAttractorManager.cpp#L344) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/PedAttractorManager.cpp:518](../../source/game_sa/PedAttractorManager.cpp#L518) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/PedAttractorManager.cpp:527](../../source/game_sa/PedAttractorManager.cpp#L527) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1299"></a>

## Globales y helpers: source/game_sa/PedGeometryAnalyser.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPointRouteInjectHooksLote109A() | [source/game_sa/PedGeometryAnalyser.cpp:842](../../source/game_sa/PedGeometryAnalyser.cpp#L842) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1300"></a>

## Globales y helpers: source/game_sa/PedGroupPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GroupOriginCanBeUsed(const CVector& origin, float radius) | [source/game_sa/PedGroupPlacer.cpp:39](../../source/game_sa/PedGroupPlacer.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| InstallGroupPed(CPedGroup& group, CPed* ped, int32 groupId, bool leader, bool setLeader) | [source/game_sa/PedGroupPlacer.cpp:48](../../source/game_sa/PedGroupPlacer.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaceCircularGroup(ePedType type, uint32 numPeds, const CVector& origin, bool random) | [source/game_sa/PedGroupPlacer.cpp:55](../../source/game_sa/PedGroupPlacer.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
