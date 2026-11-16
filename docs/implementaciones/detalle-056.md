<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 926 grupos / 11012 funciones reconstruidas; 11012 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1101"></a>

## Globales y helpers: source/game_sa/Clouds.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CloudsFtol(float value) | [source/game_sa/Clouds.cpp:51](../../source/game_sa/Clouds.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CloudsCameraRight() | [source/game_sa/Clouds.cpp:56](../../source/game_sa/Clouds.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1102"></a>

## Globales y helpers: source/game_sa/Collision/BoundingBox.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * BoundingBoxConstructorAliasBody(CBoundingBox* self) | [source/game_sa/Collision/BoundingBox.cpp:27](../../source/game_sa/Collision/BoundingBox.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BoundingBoxConstructorAlias5B2AB0(CBoundingBox* self, void* /*edx*/) | [source/game_sa/Collision/BoundingBox.cpp:36](../../source/game_sa/Collision/BoundingBox.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBoundingBox::ConstructorAlias5B2AB0 0x005B2AB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1103"></a>

## Globales y helpers: source/game_sa/Collision/ColDisk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyThreeDwords(volatile uint32* dst, const volatile uint32* src) | [source/game_sa/Collision/ColDisk.cpp:26](../../source/game_sa/Collision/ColDisk.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1104"></a>

## Globales y helpers: source/game_sa/Collision/ColPoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ColLightingCurrentExe(unsigned int lighting, float scale) | [source/game_sa/Collision/ColPoint.cpp:44](../../source/game_sa/Collision/ColPoint.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: tColLighting::GetCurrentLighting 0x0059F0C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo m_fDNBalanceParam: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:24 (0x008D12C0) |
| ColPointSurfaceTypeBExe(const CColPoint* self, void* /*edx*/) | [source/game_sa/Collision/ColPoint.cpp:72](../../source/game_sa/Collision/ColPoint.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::GetSurfaceTypeB 0x0047E050 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ColPointNormalAddressExe(CColPoint* self, void* /*edx*/) | [source/game_sa/Collision/ColPoint.cpp:79](../../source/game_sa/Collision/ColPoint.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::GetNormalAddress 0x00411BA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPointSetSurfaceTypeAExe(CColPoint* self, void* /*edx*/, uint8 surfaceType) | [source/game_sa/Collision/ColPoint.cpp:86](../../source/game_sa/Collision/ColPoint.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::SetSurfaceTypeA 0x00411BF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CColPointInjectHooksLote105A() | [source/game_sa/Collision/ColPoint.cpp:92](../../source/game_sa/Collision/ColPoint.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tColLightingInjectHooksLote116A() | [source/game_sa/Collision/ColPoint.cpp:104](../../source/game_sa/Collision/ColPoint.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1105"></a>

## Globales y helpers: source/game_sa/Collision/ColSphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CColSphere& sp, const CMatrix& transform) -&gt; CColSphere | [source/game_sa/Collision/ColSphere.cpp:129](../../source/game_sa/Collision/ColSphere.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1106"></a>

## Globales y helpers: source/game_sa/Collision/ColStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityAreaCode(const CEntity* entity) | [source/game_sa/Collision/ColStore.cpp:34](../../source/game_sa/Collision/ColStore.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlushBody(ColPoolRaw* pool) | [source/game_sa/Collision/ColStore.cpp:511](../../source/game_sa/Collision/ColStore.cpp#L511) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlush(ColPoolRaw* pool, void* /*edx*/) | [source/game_sa/Collision/ColStore.cpp:525](../../source/game_sa/Collision/ColStore.cpp#L525) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolFlush 0x00410610 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlushSecondCopy(ColPoolRaw* pool, void* /*edx*/) | [source/game_sa/Collision/ColStore.cpp:530](../../source/game_sa/Collision/ColStore.cpp#L530) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolFlushSecondCopy 0x00410FE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ColPoolScalarDeletingDestructor(ColPoolRaw* pool, void* /*edx*/, uint8 flags) | [source/game_sa/Collision/ColStore.cpp:536](../../source/game_sa/Collision/ColStore.cpp#L536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolScalarDeletingDestructor 0x004110E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1107"></a>

## Globales y helpers: source/game_sa/Collision/Collision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestPtPointTriangle( CVector a, CVector b, CVector c, CVector p ) | [source/game_sa/Collision/Collision.cpp:840](../../source/game_sa/Collision/Collision.cpp#L840) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessLineSphere_Internal( const CColLine& line, const CColSphere& sphere, CVector* ip, float* depth ) | [source/game_sa/Collision/Collision.cpp:900](../../source/game_sa/Collision/Collision.cpp#L900) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOTSA_FORCEINLINE ProcessLineTriangle_Internal( const CColLine& line, const CStoredCollPoly& poly, const CColTrianglePlane& plane, float* inOutMaxTouchDist, CVector* outIP, CVector* outPlNorm ) | [source/game_sa/Collision/Collision.cpp:1465](../../source/game_sa/Collision/Collision.cpp#L1465) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInPoly2D(CVector2D pt, CVector2D a, CVector2D b, CVector2D c) | [source/game_sa/Collision/Collision.cpp:3297](../../source/game_sa/Collision/Collision.cpp#L3297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SphereVsEntityBody(CColSphere* /*sphere: ignorada, como en el EXE*/, CEntity* entity) | [source/game_sa/Collision/Collision.cpp:4118](../../source/game_sa/Collision/Collision.cpp#L4118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1108"></a>

## Globales y helpers: source/game_sa/Collision/CollisionData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CollisionDataConstructorExe(CCollisionData* self, void* /*edx*/) | [source/game_sa/Collision/CollisionData.cpp:27](../../source/game_sa/Collision/CollisionData.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCollisionData::Constructor 0x0040F030 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1109"></a>

## Globales y helpers: source/game_sa/Collision/CollisionEventScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Dot(const CVector& a, const CVector& b) | [source/game_sa/Collision/CollisionEventScanner.cpp:64](../../source/game_sa/Collision/CollisionEventScanner.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::DotProduct-v3d*v3d* 0x0040FDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/CollisionEventScanner.cpp:72](../../source/game_sa/Collision/CollisionEventScanner.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FtolLow(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:81](../../source/game_sa/Collision/CollisionEventScanner.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ImpactDotMoveShift(const CPed* ped) | [source/game_sa/Collision/CollisionEventScanner.cpp:86](../../source/game_sa/Collision/CollisionEventScanner.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampBelowZero(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:93](../../source/game_sa/Collision/CollisionEventScanner.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCarAI::FloatMax 0x00420800 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DamageFromAttachedVehicle(CPed* victim, CVehicle* vehicle) | [source/game_sa/Collision/CollisionEventScanner.cpp:98](../../source/game_sa/Collision/CollisionEventScanner.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1110"></a>

## Globales y helpers: source/game_sa/Collision/ColourSet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/Collision/ColourSet.cpp:110](../../source/game_sa/Collision/ColourSet.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1111"></a>

## Globales y helpers: source/game_sa/Collision/Sphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CSphere& sp, const CMatrix& transform) -&gt; CSphere | [source/game_sa/Collision/Sphere.cpp:90](../../source/game_sa/Collision/Sphere.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1112"></a>

## Globales y helpers: source/game_sa/CompressedVector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressedVectorInjectHooksLote105A() | [source/game_sa/CompressedVector.cpp:48](../../source/game_sa/CompressedVector.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1113"></a>

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

<a id="grupo-1114"></a>

## Globales y helpers: source/game_sa/ControllerConfigManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MouseButtonByte(const CMouseControllerState& state, KeyCode key) | [source/game_sa/ControllerConfigManager.cpp:950](../../source/game_sa/ControllerConfigManager.cpp#L950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KeyCharToGxtChar(uint8 ch) | [source/game_sa/ControllerConfigManager.cpp:1217](../../source/game_sa/ControllerConfigManager.cpp#L1217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CFont::MapExtendedChar 0x00719360 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1115"></a>

## Globales y helpers: source/game_sa/Conversations/ConversationForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConversationTooFar(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:144](../../source/game_sa/Conversations/ConversationForPed.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationFacingAway(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:152](../../source/game_sa/Conversations/ConversationForPed.cpp#L152) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationMovingTooFast(const CVector& speed) | [source/game_sa/Conversations/ConversationForPed.cpp:157](../../source/game_sa/Conversations/ConversationForPed.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1116"></a>

## Globales y helpers: source/game_sa/Core/List_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TListInjectHooksLote105A() | [source/game_sa/Core/List_c.cpp:74](../../source/game_sa/Core/List_c.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1117"></a>

## Globales y helpers: source/game_sa/Core/Matrix.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall MatrixGetRightCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:99](../../source/game_sa/Core/Matrix.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetRightCopy 0x0041BD00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:102](../../source/game_sa/Core/Matrix.cpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetForwardCopy 0x0041BD10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixDestructorThunk(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:106](../../source/game_sa/Core/Matrix.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::DestructorThunk 0x0054E8E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote101A() | [source/game_sa/Core/Matrix.cpp:150](../../source/game_sa/Core/Matrix.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote103A() | [source/game_sa/Core/Matrix.cpp:168](../../source/game_sa/Core/Matrix.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote105A() | [source/game_sa/Core/Matrix.cpp:182](../../source/game_sa/Core/Matrix.cpp#L182) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixScaleAllExe(CMatrix* self, void* /*edx*/, float factor) | [source/game_sa/Core/Matrix.cpp:206](../../source/game_sa/Core/Matrix.cpp#L206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote166A() | [source/game_sa/Core/Matrix.cpp:219](../../source/game_sa/Core/Matrix.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixCopyFromRw(volatile uint32* dst, const volatile uint32* src) | [source/game_sa/Core/Matrix.cpp:318](../../source/game_sa/Core/Matrix.cpp#L318) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EulerAxesFromFlags(uint32 flags) | [source/game_sa/Core/Matrix.cpp:538](../../source/game_sa/Core/Matrix.cpp#L538) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:603](../../source/game_sa/Core/Matrix.cpp#L603) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CVector& b) | [source/game_sa/Core/Matrix.cpp:614](../../source/game_sa/Core/Matrix.cpp#L614) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:623](../../source/game_sa/Core/Matrix.cpp#L623) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Invert(CMatrix& in, CMatrix& out) | [source/game_sa/Core/Matrix.cpp:640](../../source/game_sa/Core/Matrix.cpp#L640) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::Invert-1 0x0059B920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(CMatrix from, CMatrix to, float t) | [source/game_sa/Core/Matrix.cpp:664](../../source/game_sa/Core/Matrix.cpp#L664) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1118"></a>

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

<a id="grupo-1119"></a>

## Globales y helpers: source/game_sa/Core/Quaternion.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DotProduct(const CQuaternion& q1, const CQuaternion& q2) | [source/game_sa/Core/Quaternion.h:122](../../source/game_sa/Core/Quaternion.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:129](../../source/game_sa/Core/Quaternion.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:133](../../source/game_sa/Core/Quaternion.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:137](../../source/game_sa/Core/Quaternion.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:141](../../source/game_sa/Core/Quaternion.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:145](../../source/game_sa/Core/Quaternion.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1120"></a>

## Globales y helpers: source/game_sa/Core/Vector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Vector2DMagnitudeExe(const CVector2D* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:54](../../source/game_sa/Core/Vector.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProductInPlaceExe(CVector* out, void* /*edx*/, const CVector* a, const CVector* b) | [source/game_sa/Core/Vector.cpp:101](../../source/game_sa/Core/Vector.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall VectorDefaultConstructorExe(CVector* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:125](../../source/game_sa/Core/Vector.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::DefaultConstructor 0x00404350 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CrossProduct(CVector* out, CVector* a, CVector* b) | [source/game_sa/Core/Vector.cpp:410](../../source/game_sa/Core/Vector.cpp#L410) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::CrossProduct-out 0x0059C730 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Multiply3x3VMExe(CVector* out, const CVector* vector, const CMatrix* matrix) | [source/game_sa/Core/Vector.cpp:455](../../source/game_sa/Core/Vector.cpp#L455) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * VectorTimesFloat(CVector* out, const CVector* vector, float factor) | [source/game_sa/Core/Vector.cpp:530](../../source/game_sa/Core/Vector.cpp#L530) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProduct(const CVector& a, const CVector& b) | [source/game_sa/Core/Vector.cpp:542](../../source/game_sa/Core/Vector.cpp#L542) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(CVector* v1, CVector* v2) | [source/game_sa/Core/Vector.cpp:548](../../source/game_sa/Core/Vector.cpp#L548) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::DotProduct-v3d*v3d* 0x0040FDB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:553](../../source/game_sa/Core/Vector.cpp#L553) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:558](../../source/game_sa/Core/Vector.cpp#L558) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
