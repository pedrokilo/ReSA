<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11736 con evidencia histórica y 705 pendientes de validación en 29 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

## Globales y helpers: source/game_sa/Collision/ColDisk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyThreeDwords(volatile uint32* dst, const volatile uint32* src) | [source/game_sa/Collision/ColDisk.cpp:26](../../source/game_sa/Collision/ColDisk.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/Collision/ColPoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ColLightingCurrentExe(unsigned int lighting, float scale) | [source/game_sa/Collision/ColPoint.cpp:44](../../source/game_sa/Collision/ColPoint.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: tColLighting::GetCurrentLighting 0x0059F0C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_fDNBalanceParam: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:24 (0x008D12C0) |
| ColPointSurfaceTypeBExe(const CColPoint* self, void* /*edx*/) | [source/game_sa/Collision/ColPoint.cpp:72](../../source/game_sa/Collision/ColPoint.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::GetSurfaceTypeB 0x0047E050 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ColPointNormalAddressExe(CColPoint* self, void* /*edx*/) | [source/game_sa/Collision/ColPoint.cpp:79](../../source/game_sa/Collision/ColPoint.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::GetNormalAddress 0x00411BA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPointSetSurfaceTypeAExe(CColPoint* self, void* /*edx*/, uint8 surfaceType) | [source/game_sa/Collision/ColPoint.cpp:86](../../source/game_sa/Collision/ColPoint.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::SetSurfaceTypeA 0x00411BF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CColPointInjectHooksLote105A() | [source/game_sa/Collision/ColPoint.cpp:92](../../source/game_sa/Collision/ColPoint.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tColLightingInjectHooksLote116A() | [source/game_sa/Collision/ColPoint.cpp:104](../../source/game_sa/Collision/ColPoint.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/Collision/ColSphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CColSphere& sp, const CMatrix& transform) -&gt; CColSphere | [source/game_sa/Collision/ColSphere.cpp:129](../../source/game_sa/Collision/ColSphere.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/Collision/ColStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityAreaCode(const CEntity* entity) | [source/game_sa/Collision/ColStore.cpp:34](../../source/game_sa/Collision/ColStore.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlushBody(ColPoolRaw* pool) | [source/game_sa/Collision/ColStore.cpp:511](../../source/game_sa/Collision/ColStore.cpp#L511) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlush(ColPoolRaw* pool, void* /*edx*/) | [source/game_sa/Collision/ColStore.cpp:525](../../source/game_sa/Collision/ColStore.cpp#L525) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolFlush 0x00410610 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlushSecondCopy(ColPoolRaw* pool, void* /*edx*/) | [source/game_sa/Collision/ColStore.cpp:530](../../source/game_sa/Collision/ColStore.cpp#L530) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolFlushSecondCopy 0x00410FE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ColPoolScalarDeletingDestructor(ColPoolRaw* pool, void* /*edx*/, uint8 flags) | [source/game_sa/Collision/ColStore.cpp:536](../../source/game_sa/Collision/ColStore.cpp#L536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolScalarDeletingDestructor 0x004110E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/Collision/Collision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestPtPointTriangle( CVector a, CVector b, CVector c, CVector p ) | [source/game_sa/Collision/Collision.cpp:840](../../source/game_sa/Collision/Collision.cpp#L840) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessLineSphere_Internal( const CColLine& line, const CColSphere& sphere, CVector* ip, float* depth ) | [source/game_sa/Collision/Collision.cpp:900](../../source/game_sa/Collision/Collision.cpp#L900) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOTSA_FORCEINLINE ProcessLineTriangle_Internal( const CColLine& line, const CStoredCollPoly& poly, const CColTrianglePlane& plane, float* inOutMaxTouchDist, CVector* outIP, CVector* outPlNorm ) | [source/game_sa/Collision/Collision.cpp:1465](../../source/game_sa/Collision/Collision.cpp#L1465) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInPoly2D(CVector2D pt, CVector2D a, CVector2D b, CVector2D c) | [source/game_sa/Collision/Collision.cpp:3297](../../source/game_sa/Collision/Collision.cpp#L3297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SphereVsEntityBody(CColSphere* /*sphere: ignorada, como en el EXE*/, CEntity* entity) | [source/game_sa/Collision/Collision.cpp:4118](../../source/game_sa/Collision/Collision.cpp#L4118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/Collision/CollisionData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CollisionDataConstructorExe(CCollisionData* self, void* /*edx*/) | [source/game_sa/Collision/CollisionData.cpp:27](../../source/game_sa/Collision/CollisionData.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCollisionData::Constructor 0x0040F030 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1127"></a>

## Globales y helpers: source/game_sa/Collision/CollisionEventScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Dot(const CVector& a, const CVector& b) | [source/game_sa/Collision/CollisionEventScanner.cpp:64](../../source/game_sa/Collision/CollisionEventScanner.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::DotProduct-v3d*v3d* 0x0040FDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/CollisionEventScanner.cpp:72](../../source/game_sa/Collision/CollisionEventScanner.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FtolLow(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:81](../../source/game_sa/Collision/CollisionEventScanner.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ImpactDotMoveShift(const CPed* ped) | [source/game_sa/Collision/CollisionEventScanner.cpp:86](../../source/game_sa/Collision/CollisionEventScanner.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampBelowZero(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:93](../../source/game_sa/Collision/CollisionEventScanner.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCarAI::FloatMax 0x00420800 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DamageFromAttachedVehicle(CPed* victim, CVehicle* vehicle) | [source/game_sa/Collision/CollisionEventScanner.cpp:98](../../source/game_sa/Collision/CollisionEventScanner.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/Collision/ColourSet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/Collision/ColourSet.cpp:110](../../source/game_sa/Collision/ColourSet.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1129"></a>

## Globales y helpers: source/game_sa/Collision/Sphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CSphere& sp, const CMatrix& transform) -&gt; CSphere | [source/game_sa/Collision/Sphere.cpp:90](../../source/game_sa/Collision/Sphere.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/CompressedVector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressedVectorInjectHooksLote105A() | [source/game_sa/CompressedVector.cpp:48](../../source/game_sa/CompressedVector.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/CompressedVector.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressVector(const CVector& v) | [source/game_sa/CompressedVector.h:12](../../source/game_sa/CompressedVector.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressVector(const CompressedVector& v) | [source/game_sa/CompressedVector.h:16](../../source/game_sa/CompressedVector.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CompressLargeVector(const CVector& v) | [source/game_sa/CompressedVector.h:21](../../source/game_sa/CompressedVector.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressLargeVector(const CompressedLargeVector& v) | [source/game_sa/CompressedVector.h:25](../../source/game_sa/CompressedVector.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CompressUnitVector(const CVector& v) | [source/game_sa/CompressedVector.h:30](../../source/game_sa/CompressedVector.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressUnitVector(const CompressedUnitVector& v) | [source/game_sa/CompressedVector.h:34](../../source/game_sa/CompressedVector.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CompressFxVector(const CVector& v) | [source/game_sa/CompressedVector.h:39](../../source/game_sa/CompressedVector.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UncompressFxVector(const CompressedFxVector& v) | [source/game_sa/CompressedVector.h:43](../../source/game_sa/CompressedVector.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/ControllerConfigManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MouseButtonByte(const CMouseControllerState& state, KeyCode key) | [source/game_sa/ControllerConfigManager.cpp:950](../../source/game_sa/ControllerConfigManager.cpp#L950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KeyCharToGxtChar(uint8 ch) | [source/game_sa/ControllerConfigManager.cpp:1217](../../source/game_sa/ControllerConfigManager.cpp#L1217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CFont::MapExtendedChar 0x00719360 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

## Globales y helpers: source/game_sa/Conversations/ConversationForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConversationTooFar(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:144](../../source/game_sa/Conversations/ConversationForPed.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationFacingAway(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:152](../../source/game_sa/Conversations/ConversationForPed.cpp#L152) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationMovingTooFast(const CVector& speed) | [source/game_sa/Conversations/ConversationForPed.cpp:157](../../source/game_sa/Conversations/ConversationForPed.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Core/ListItem_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ListItemConstructorExe(ListItemLinksExe* self, void* /*edx*/) | [source/game_sa/Core/ListItem_c.cpp:31](../../source/game_sa/Core/ListItem_c.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ListItemDestructorExe(ListItemLinksExe* /*self*/, void* /*edx*/) | [source/game_sa/Core/ListItem_c.cpp:39](../../source/game_sa/Core/ListItem_c.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: ListItem_c::Destructor 0x004A8DC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ListItemInjectHooksLote218A() | [source/game_sa/Core/ListItem_c.cpp:43](../../source/game_sa/Core/ListItem_c.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/Core/List_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TListInjectHooksLote105A() | [source/game_sa/Core/List_c.cpp:175](../../source/game_sa/Core/List_c.cpp#L175) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/Core/Matrix.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall MatrixGetRightCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:110](../../source/game_sa/Core/Matrix.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetRightCopy 0x0041BD00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:113](../../source/game_sa/Core/Matrix.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetForwardCopy 0x0041BD10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixDestructorThunk(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:117](../../source/game_sa/Core/Matrix.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::DestructorThunk 0x0054E8E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote101A() | [source/game_sa/Core/Matrix.cpp:161](../../source/game_sa/Core/Matrix.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote103A() | [source/game_sa/Core/Matrix.cpp:179](../../source/game_sa/Core/Matrix.cpp#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote105A() | [source/game_sa/Core/Matrix.cpp:193](../../source/game_sa/Core/Matrix.cpp#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixScaleAllExe(CMatrix* self, void* /*edx*/, float factor) | [source/game_sa/Core/Matrix.cpp:217](../../source/game_sa/Core/Matrix.cpp#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote166A() | [source/game_sa/Core/Matrix.cpp:230](../../source/game_sa/Core/Matrix.cpp#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixCopyFromRw(volatile uint32* dst, const volatile uint32* src) | [source/game_sa/Core/Matrix.cpp:329](../../source/game_sa/Core/Matrix.cpp#L329) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EulerAxesFromFlags(uint32 flags) | [source/game_sa/Core/Matrix.cpp:549](../../source/game_sa/Core/Matrix.cpp#L549) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:614](../../source/game_sa/Core/Matrix.cpp#L614) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CVector& b) | [source/game_sa/Core/Matrix.cpp:625](../../source/game_sa/Core/Matrix.cpp#L625) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:634](../../source/game_sa/Core/Matrix.cpp#L634) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Invert(CMatrix& in, CMatrix& out) | [source/game_sa/Core/Matrix.cpp:651](../../source/game_sa/Core/Matrix.cpp#L651) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::Invert-1 0x0059B920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(CMatrix from, CMatrix to, float t) | [source/game_sa/Core/Matrix.cpp:675](../../source/game_sa/Core/Matrix.cpp#L675) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/Core/OctTree.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetExeVtable(COctTree* tree) | [source/game_sa/Core/OctTree.cpp:32](../../source/game_sa/Core/OctTree.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReleaseChildren(COctTree* self) | [source/game_sa/Core/OctTree.cpp:39](../../source/game_sa/Core/OctTree.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_octTreePool: source/game_sa/Core/OctTree.h:25 (0x00BC12E4) |
| * __fastcall OctTreeConstructorExe(COctTree* self, void* /*edx*/) | [source/game_sa/Core/OctTree.cpp:55](../../source/game_sa/Core/OctTree.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: COctTree::Constructor 0x005A6DB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall OctTreeScalarDeletingDestructorExe(COctTree* self, void* /*edx*/, uint8 flags) | [source/game_sa/Core/OctTree.cpp:63](../../source/game_sa/Core/OctTree.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: COctTree::ScalarDeletingDestructor 0x005A7490 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo ms_octTreePool: source/game_sa/Core/OctTree.h:25 (0x00BC12E4) |
| OctTreeInsertTreeExe(COctTree* self, void* /*edx*/, uint8 red, uint8 green, uint8 blue) | [source/game_sa/Core/OctTree.cpp:80](../../source/game_sa/Core/OctTree.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_bFailed: source/game_sa/Core/OctTree.h:23 (0x00BC12DC), source/game_sa/GenericGameStorage.h:69 (0x00C16EFC); Posible dato fijo ms_level: source/game_sa/Core/OctTree.h:24 (0x00BC12E0); Posible dato fijo ms_octTreePool: source/game_sa/Core/OctTree.h:25 (0x00BC12E4) |
| * __fastcall OctTreePoolNewExe(COctTreePool* pool, void* /*edx*/) | [source/game_sa/Core/OctTree.cpp:124](../../source/game_sa/Core/OctTree.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: COctTree::PoolNew 0x005A7350 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| OctTreePoolDeleteExe(COctTreePool* pool, void* /*edx*/, COctTree* obj) | [source/game_sa/Core/OctTree.cpp:129](../../source/game_sa/Core/OctTree.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: COctTree::PoolDelete 0x005A73C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolFlushBody(void* self) | [source/game_sa/Core/OctTree.cpp:136](../../source/game_sa/Core/OctTree.cpp#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/Core/Quaternion.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DotProduct(const CQuaternion& q1, const CQuaternion& q2) | [source/game_sa/Core/Quaternion.h:122](../../source/game_sa/Core/Quaternion.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:129](../../source/game_sa/Core/Quaternion.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:133](../../source/game_sa/Core/Quaternion.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:137](../../source/game_sa/Core/Quaternion.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:141](../../source/game_sa/Core/Quaternion.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:145](../../source/game_sa/Core/Quaternion.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/Core/Vector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Vector2DMagnitudeExe(const CVector2D* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:54](../../source/game_sa/Core/Vector.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Vector2DNormaliseExe(CVector2D* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:62](../../source/game_sa/Core/Vector.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProductInPlaceExe(CVector* out, void* /*edx*/, const CVector* a, const CVector* b) | [source/game_sa/Core/Vector.cpp:119](../../source/game_sa/Core/Vector.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall VectorDefaultConstructorExe(CVector* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:143](../../source/game_sa/Core/Vector.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::DefaultConstructor 0x00404350 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CrossProduct(CVector* out, CVector* a, CVector* b) | [source/game_sa/Core/Vector.cpp:428](../../source/game_sa/Core/Vector.cpp#L428) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::CrossProduct-out 0x0059C730 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Multiply3x3VMExe(CVector* out, const CVector* vector, const CMatrix* matrix) | [source/game_sa/Core/Vector.cpp:473](../../source/game_sa/Core/Vector.cpp#L473) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * VectorTimesFloat(CVector* out, const CVector* vector, float factor) | [source/game_sa/Core/Vector.cpp:548](../../source/game_sa/Core/Vector.cpp#L548) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProduct(const CVector& a, const CVector& b) | [source/game_sa/Core/Vector.cpp:560](../../source/game_sa/Core/Vector.cpp#L560) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(CVector* v1, CVector* v2) | [source/game_sa/Core/Vector.cpp:566](../../source/game_sa/Core/Vector.cpp#L566) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::DotProduct-v3d*v3d* 0x0040FDB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:571](../../source/game_sa/Core/Vector.cpp#L571) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:576](../../source/game_sa/Core/Vector.cpp#L576) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/Core/Vector.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator-(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:231](../../source/game_sa/Core/Vector.h#L231) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:235](../../source/game_sa/Core/Vector.h#L235) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:239](../../source/game_sa/Core/Vector.h#L239) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector& vecOne, const CVector& vecTwo) | [source/game_sa/Core/Vector.h:243](../../source/game_sa/Core/Vector.h#L243) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector& vec, float notEqualTo) | [source/game_sa/Core/Vector.h:247](../../source/game_sa/Core/Vector.h#L247) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector& vec, float equalTo) | [source/game_sa/Core/Vector.h:251](../../source/game_sa/Core/Vector.h#L251) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector& vecLeft, const CVector& vecRight) | [source/game_sa/Core/Vector.h:255](../../source/game_sa/Core/Vector.h#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CVector& vec, float dividend) | [source/game_sa/Core/Vector.h:260](../../source/game_sa/Core/Vector.h#L260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float multiplier, const CVector& vec) | [source/game_sa/Core/Vector.h:264](../../source/game_sa/Core/Vector.h#L264) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CVector& vec) | [source/game_sa/Core/Vector.h:268](../../source/game_sa/Core/Vector.h#L268) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPoints(const CVector& pointOne, const CVector& pointTwo) | [source/game_sa/Core/Vector.h:272](../../source/game_sa/Core/Vector.h#L272) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPointsSquared(const CVector& pointOne, const CVector& pointTwo) | [source/game_sa/Core/Vector.h:276](../../source/game_sa/Core/Vector.h#L276) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(const CVector& vecOne, const CVector& vecTwo, float fProgress) | [source/game_sa/Core/Vector.h:280](../../source/game_sa/Core/Vector.h#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clamp(CVector val, CVector min, CVector max) | [source/game_sa/Core/Vector.h:285](../../source/game_sa/Core/Vector.h#L285) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Pow(const CVector& vec, float fPow) | [source/game_sa/Core/Vector.h:292](../../source/game_sa/Core/Vector.h#L292) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Normalized(CVector v) | [source/game_sa/Core/Vector.h:310](../../source/game_sa/Core/Vector.h#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProjectVector(const CVector& what, const CVector& onto) | [source/game_sa/Core/Vector.h:311](../../source/game_sa/Core/Vector.h#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Core/Vector.h:315](../../source/game_sa/Core/Vector.h#L315) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Core/Vector.h:316](../../source/game_sa/Core/Vector.h#L316) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyMatrixWithVector(const CMatrix& mat, const CVector& vec) | [source/game_sa/Core/Vector.h:317](../../source/game_sa/Core/Vector.h#L317) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
