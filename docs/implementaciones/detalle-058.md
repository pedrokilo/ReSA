<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1141"></a>

## Globales y helpers: source/game_sa/CdStreamInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectCdStreamHooks() | [source/game_sa/CdStreamInfo.cpp:42](../../source/game_sa/CdStreamInfo.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CdStreamHandleToFileID(CdStreamHandle h) | [source/game_sa/CdStreamInfo.cpp:331](../../source/game_sa/CdStreamInfo.cpp#L331) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1142"></a>

## Globales y helpers: source/game_sa/ClothesBuilder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClothesBlendPair(float a, float b, float ra, float rb) | [source/game_sa/ClothesBuilder.cpp:272](../../source/game_sa/ClothesBuilder.cpp#L272) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesBlendTriple(float a, float b, float c, float ra, float rb, float rc) | [source/game_sa/ClothesBuilder.cpp:278](../../source/game_sa/ClothesBuilder.cpp#L278) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesNormalizeWeights(float* weights) | [source/game_sa/ClothesBuilder.cpp:286](../../source/game_sa/ClothesBuilder.cpp#L286) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * BlendGeometry(RpClump* clump, std::pair&lt;const char*, float&gt; (&&frameNamesRatios)\[N\]) | [source/game_sa/ClothesBuilder.cpp:298](../../source/game_sa/ClothesBuilder.cpp#L298) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AssertTextureLayouts(std::initializer_list&lt;RwTexture*&gt; textures) | [source/game_sa/ClothesBuilder.cpp:483](../../source/game_sa/ClothesBuilder.cpp#L483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesBlendByte(float v) | [source/game_sa/ClothesBuilder.cpp:552](../../source/game_sa/ClothesBuilder.cpp#L552) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1143"></a>

## Globales y helpers: source/game_sa/Clouds.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CloudsFtol(float value) | [source/game_sa/Clouds.cpp:56](../../source/game_sa/Clouds.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CloudsCameraRight() | [source/game_sa/Clouds.cpp:61](../../source/game_sa/Clouds.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:50 (0x00B6F028) |

<a id="grupo-1144"></a>

## Globales y helpers: source/game_sa/Collision/BoundingBox.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * BoundingBoxConstructorAliasBody(CBoundingBox* self) | [source/game_sa/Collision/BoundingBox.cpp:27](../../source/game_sa/Collision/BoundingBox.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BoundingBoxConstructorAlias537550(CBoundingBox* self, void* /*edx*/) | [source/game_sa/Collision/BoundingBox.cpp:32](../../source/game_sa/Collision/BoundingBox.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBoundingBox::ConstructorAlias537550 0x00537550 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BoundingBoxConstructorAlias537560(CBoundingBox* self, void* /*edx*/) | [source/game_sa/Collision/BoundingBox.cpp:34](../../source/game_sa/Collision/BoundingBox.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBoundingBox::ConstructorAlias537560 0x00537560 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BoundingBoxConstructorAlias537570(CBoundingBox* self, void* /*edx*/) | [source/game_sa/Collision/BoundingBox.cpp:36](../../source/game_sa/Collision/BoundingBox.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBoundingBox::ConstructorAlias537570 0x00537570 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall BoundingBoxConstructorAlias5B2AB0(CBoundingBox* self, void* /*edx*/) | [source/game_sa/Collision/BoundingBox.cpp:39](../../source/game_sa/Collision/BoundingBox.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBoundingBox::ConstructorAlias5B2AB0 0x005B2AB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1145"></a>

## Globales y helpers: source/game_sa/Collision/ColDisk.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyThreeDwords(volatile uint32* dst, const volatile uint32* src) | [source/game_sa/Collision/ColDisk.cpp:26](../../source/game_sa/Collision/ColDisk.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1146"></a>

## Globales y helpers: source/game_sa/Collision/ColPoint.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ColLightingCurrentExe(unsigned int lighting, float scale) | [source/game_sa/Collision/ColPoint.cpp:44](../../source/game_sa/Collision/ColPoint.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: tColLighting::GetCurrentLighting 0x0059F0C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPointSurfaceTypeBExe(const CColPoint* self, void* /*edx*/) | [source/game_sa/Collision/ColPoint.cpp:72](../../source/game_sa/Collision/ColPoint.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::GetSurfaceTypeB 0x0047E050 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ColPointNormalAddressExe(CColPoint* self, void* /*edx*/) | [source/game_sa/Collision/ColPoint.cpp:79](../../source/game_sa/Collision/ColPoint.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::GetNormalAddress 0x00411BA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPointSetSurfaceTypeAExe(CColPoint* self, void* /*edx*/, uint8 surfaceType) | [source/game_sa/Collision/ColPoint.cpp:86](../../source/game_sa/Collision/ColPoint.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CColPoint::SetSurfaceTypeA 0x00411BF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CColPointInjectHooksLote105A() | [source/game_sa/Collision/ColPoint.cpp:92](../../source/game_sa/Collision/ColPoint.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| tColLightingInjectHooksLote116A() | [source/game_sa/Collision/ColPoint.cpp:104](../../source/game_sa/Collision/ColPoint.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1147"></a>

## Globales y helpers: source/game_sa/Collision/ColSphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CColSphere& sp, const CMatrix& transform) -&gt; CColSphere | [source/game_sa/Collision/ColSphere.cpp:132](../../source/game_sa/Collision/ColSphere.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1148"></a>

## Globales y helpers: source/game_sa/Collision/ColStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityAreaCode(const CEntity* entity) | [source/game_sa/Collision/ColStore.cpp:35](../../source/game_sa/Collision/ColStore.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlushBody(ColPoolRaw* pool) | [source/game_sa/Collision/ColStore.cpp:512](../../source/game_sa/Collision/ColStore.cpp#L512) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlush(ColPoolRaw* pool, void* /*edx*/) | [source/game_sa/Collision/ColStore.cpp:526](../../source/game_sa/Collision/ColStore.cpp#L526) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolFlush 0x00410610 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ColPoolFlushSecondCopy(ColPoolRaw* pool, void* /*edx*/) | [source/game_sa/Collision/ColStore.cpp:531](../../source/game_sa/Collision/ColStore.cpp#L531) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolFlushSecondCopy 0x00410FE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ColPoolScalarDeletingDestructor(ColPoolRaw* pool, void* /*edx*/, uint8 flags) | [source/game_sa/Collision/ColStore.cpp:537](../../source/game_sa/Collision/ColStore.cpp#L537) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CColStore::PoolScalarDeletingDestructor 0x004110E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1149"></a>

## Globales y helpers: source/game_sa/Collision/Collision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestPtPointTriangle( CVector a, CVector b, CVector c, CVector p ) | [source/game_sa/Collision/Collision.cpp:846](../../source/game_sa/Collision/Collision.cpp#L846) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessLineSphere_Internal( const CColLine& line, const CColSphere& sphere, CVector* ip, float* depth ) | [source/game_sa/Collision/Collision.cpp:906](../../source/game_sa/Collision/Collision.cpp#L906) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOTSA_FORCEINLINE ProcessLineTriangle_Internal( const CColLine& line, const CStoredCollPoly& poly, const CColTrianglePlane& plane, float* inOutMaxTouchDist, CVector* outIP, CVector* outPlNorm ) | [source/game_sa/Collision/Collision.cpp:1471](../../source/game_sa/Collision/Collision.cpp#L1471) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInPoly2D(CVector2D pt, CVector2D a, CVector2D b, CVector2D c) | [source/game_sa/Collision/Collision.cpp:3303](../../source/game_sa/Collision/Collision.cpp#L3303) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SphereVsEntityBody(CColSphere* /*sphere: ignorada, como en el EXE*/, CEntity* entity) | [source/game_sa/Collision/Collision.cpp:4124](../../source/game_sa/Collision/Collision.cpp#L4124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:50 (0x00B6F028) |

<a id="grupo-1150"></a>

## Globales y helpers: source/game_sa/Collision/CollisionData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CollisionDataConstructorExe(CCollisionData* self, void* /*edx*/) | [source/game_sa/Collision/CollisionData.cpp:27](../../source/game_sa/Collision/CollisionData.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCollisionData::Constructor 0x0040F030 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1151"></a>

## Globales y helpers: source/game_sa/Collision/CollisionEventScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Dot(const CVector& a, const CVector& b) | [source/game_sa/Collision/CollisionEventScanner.cpp:65](../../source/game_sa/Collision/CollisionEventScanner.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::DotProduct-v3d*v3d* 0x0040FDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/CollisionEventScanner.cpp:73](../../source/game_sa/Collision/CollisionEventScanner.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FtolLow(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:82](../../source/game_sa/Collision/CollisionEventScanner.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ImpactDotMoveShift(const CPed* ped) | [source/game_sa/Collision/CollisionEventScanner.cpp:87](../../source/game_sa/Collision/CollisionEventScanner.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampBelowZero(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:94](../../source/game_sa/Collision/CollisionEventScanner.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCarAI::FloatMax 0x00420800 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DamageFromAttachedVehicle(CPed* victim, CVehicle* vehicle) | [source/game_sa/Collision/CollisionEventScanner.cpp:99](../../source/game_sa/Collision/CollisionEventScanner.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1152"></a>

## Globales y helpers: source/game_sa/Collision/ColourSet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/Collision/ColourSet.cpp:110](../../source/game_sa/Collision/ColourSet.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1153"></a>

## Globales y helpers: source/game_sa/Collision/Sphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CSphere& sp, const CMatrix& transform) -&gt; CSphere | [source/game_sa/Collision/Sphere.cpp:91](../../source/game_sa/Collision/Sphere.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1154"></a>

## Globales y helpers: source/game_sa/CompressedVector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressedVectorInjectHooksLote105A() | [source/game_sa/CompressedVector.cpp:48](../../source/game_sa/CompressedVector.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1155"></a>

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

<a id="grupo-1156"></a>

## Globales y helpers: source/game_sa/ControllerConfigManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MouseButtonByte(const CMouseControllerState& state, KeyCode key) | [source/game_sa/ControllerConfigManager.cpp:951](../../source/game_sa/ControllerConfigManager.cpp#L951) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KeyCharToGxtChar(uint8 ch) | [source/game_sa/ControllerConfigManager.cpp:1218](../../source/game_sa/ControllerConfigManager.cpp#L1218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CFont::MapExtendedChar 0x00719360 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1157"></a>

## Globales y helpers: source/game_sa/Conversations/ConversationForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConversationTooFar(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:146](../../source/game_sa/Conversations/ConversationForPed.cpp#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationFacingAway(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:154](../../source/game_sa/Conversations/ConversationForPed.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationMovingTooFast(const CVector& speed) | [source/game_sa/Conversations/ConversationForPed.cpp:159](../../source/game_sa/Conversations/ConversationForPed.cpp#L159) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1158"></a>

## Globales y helpers: source/game_sa/Core/ListItem_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall ListItemConstructorExe(ListItemLinksExe* self, void* /*edx*/) | [source/game_sa/Core/ListItem_c.cpp:32](../../source/game_sa/Core/ListItem_c.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: ListItem_c::Constructor 0x004A8DB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ListItemDestructorExe(ListItemLinksExe* /*self*/, void* /*edx*/) | [source/game_sa/Core/ListItem_c.cpp:40](../../source/game_sa/Core/ListItem_c.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: ListItem_c::Destructor 0x004A8DC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ListItemInjectHooksLote218A() | [source/game_sa/Core/ListItem_c.cpp:44](../../source/game_sa/Core/ListItem_c.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1159"></a>

## Globales y helpers: source/game_sa/Core/List_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TListInjectHooksLote105A() | [source/game_sa/Core/List_c.cpp:175](../../source/game_sa/Core/List_c.cpp#L175) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1160"></a>

## Globales y helpers: source/game_sa/Core/Matrix.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall MatrixGetRightCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:111](../../source/game_sa/Core/Matrix.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetRightCopy 0x0041BD00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:114](../../source/game_sa/Core/Matrix.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetForwardCopy 0x0041BD10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixDestructorThunk(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:118](../../source/game_sa/Core/Matrix.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::DestructorThunk 0x0054E8E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote101A() | [source/game_sa/Core/Matrix.cpp:162](../../source/game_sa/Core/Matrix.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote103A() | [source/game_sa/Core/Matrix.cpp:180](../../source/game_sa/Core/Matrix.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote105A() | [source/game_sa/Core/Matrix.cpp:194](../../source/game_sa/Core/Matrix.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixAddTranslationExe(CMatrix* self, void* /*edx*/, const CVector* offset) | [source/game_sa/Core/Matrix.cpp:207](../../source/game_sa/Core/Matrix.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::AddTranslation 0x0046AE00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixScaleAllExe(CMatrix* self, void* /*edx*/, float factor) | [source/game_sa/Core/Matrix.cpp:219](../../source/game_sa/Core/Matrix.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote166A() | [source/game_sa/Core/Matrix.cpp:232](../../source/game_sa/Core/Matrix.cpp#L232) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixCopyFromRw(volatile uint32* dst, const volatile uint32* src) | [source/game_sa/Core/Matrix.cpp:332](../../source/game_sa/Core/Matrix.cpp#L332) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EulerAxesFromFlags(uint32 flags) | [source/game_sa/Core/Matrix.cpp:564](../../source/game_sa/Core/Matrix.cpp#L564) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:631](../../source/game_sa/Core/Matrix.cpp#L631) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CVector& b) | [source/game_sa/Core/Matrix.cpp:642](../../source/game_sa/Core/Matrix.cpp#L642) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * MatrixTimesPointExe(CVector* out, const CMatrix* matrix, const CVector* point) | [source/game_sa/Core/Matrix.cpp:648](../../source/game_sa/Core/Matrix.cpp#L648) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:656](../../source/game_sa/Core/Matrix.cpp#L656) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Invert(CMatrix& in, CMatrix& out) | [source/game_sa/Core/Matrix.cpp:673](../../source/game_sa/Core/Matrix.cpp#L673) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::Invert-1 0x0059B920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(CMatrix from, CMatrix to, float t) | [source/game_sa/Core/Matrix.cpp:697](../../source/game_sa/Core/Matrix.cpp#L697) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
