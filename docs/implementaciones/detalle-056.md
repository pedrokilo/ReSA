<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 893 grupos / 10321 funciones reconstruidas; 10321 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1101"></a>

## Globales y helpers: source/game_sa/Collision/ColStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityAreaCode(const CEntity* entity) | [source/game_sa/Collision/ColStore.cpp:34](../../source/game_sa/Collision/ColStore.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1102"></a>

## Globales y helpers: source/game_sa/Collision/Collision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestPtPointTriangle( CVector a, CVector b, CVector c, CVector p ) | [source/game_sa/Collision/Collision.cpp:840](../../source/game_sa/Collision/Collision.cpp#L840) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessLineSphere_Internal( const CColLine& line, const CColSphere& sphere, CVector* ip, float* depth ) | [source/game_sa/Collision/Collision.cpp:900](../../source/game_sa/Collision/Collision.cpp#L900) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOTSA_FORCEINLINE ProcessLineTriangle_Internal( const CColLine& line, const CStoredCollPoly& poly, const CColTrianglePlane& plane, float* inOutMaxTouchDist, CVector* outIP, CVector* outPlNorm ) | [source/game_sa/Collision/Collision.cpp:1465](../../source/game_sa/Collision/Collision.cpp#L1465) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInPoly2D(CVector2D pt, CVector2D a, CVector2D b, CVector2D c) | [source/game_sa/Collision/Collision.cpp:3297](../../source/game_sa/Collision/Collision.cpp#L3297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SphereVsEntityBody(CColSphere* /*sphere: ignorada, como en el EXE*/, CEntity* entity) | [source/game_sa/Collision/Collision.cpp:4118](../../source/game_sa/Collision/Collision.cpp#L4118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1103"></a>

## Globales y helpers: source/game_sa/Collision/CollisionData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CollisionDataConstructorExe(CCollisionData* self, void* /*edx*/) | [source/game_sa/Collision/CollisionData.cpp:27](../../source/game_sa/Collision/CollisionData.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCollisionData::Constructor 0x0040F030 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1104"></a>

## Globales y helpers: source/game_sa/Collision/CollisionEventScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Dot(const CVector& a, const CVector& b) | [source/game_sa/Collision/CollisionEventScanner.cpp:64](../../source/game_sa/Collision/CollisionEventScanner.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::DotProduct-v3d*v3d* 0x0040FDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/CollisionEventScanner.cpp:72](../../source/game_sa/Collision/CollisionEventScanner.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FtolLow(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:81](../../source/game_sa/Collision/CollisionEventScanner.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ImpactDotMoveShift(const CPed* ped) | [source/game_sa/Collision/CollisionEventScanner.cpp:86](../../source/game_sa/Collision/CollisionEventScanner.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampBelowZero(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:93](../../source/game_sa/Collision/CollisionEventScanner.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCarAI::FloatMax 0x00420800 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DamageFromAttachedVehicle(CPed* victim, CVehicle* vehicle) | [source/game_sa/Collision/CollisionEventScanner.cpp:98](../../source/game_sa/Collision/CollisionEventScanner.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1105"></a>

## Globales y helpers: source/game_sa/Collision/ColourSet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/Collision/ColourSet.cpp:110](../../source/game_sa/Collision/ColourSet.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1106"></a>

## Globales y helpers: source/game_sa/Collision/Sphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CSphere& sp, const CMatrix& transform) -&gt; CSphere | [source/game_sa/Collision/Sphere.cpp:90](../../source/game_sa/Collision/Sphere.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1107"></a>

## Globales y helpers: source/game_sa/CompressedVector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressedVectorInjectHooksLote105A() | [source/game_sa/CompressedVector.cpp:48](../../source/game_sa/CompressedVector.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1108"></a>

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

<a id="grupo-1109"></a>

## Globales y helpers: source/game_sa/ControllerConfigManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MouseButtonByte(const CMouseControllerState& state, KeyCode key) | [source/game_sa/ControllerConfigManager.cpp:950](../../source/game_sa/ControllerConfigManager.cpp#L950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KeyCharToGxtChar(uint8 ch) | [source/game_sa/ControllerConfigManager.cpp:1217](../../source/game_sa/ControllerConfigManager.cpp#L1217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CFont::MapExtendedChar 0x00719360 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1110"></a>

## Globales y helpers: source/game_sa/Conversations/ConversationForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConversationTooFar(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:144](../../source/game_sa/Conversations/ConversationForPed.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationFacingAway(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:152](../../source/game_sa/Conversations/ConversationForPed.cpp#L152) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationMovingTooFast(const CVector& speed) | [source/game_sa/Conversations/ConversationForPed.cpp:157](../../source/game_sa/Conversations/ConversationForPed.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1111"></a>

## Globales y helpers: source/game_sa/Core/List_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TListInjectHooksLote105A() | [source/game_sa/Core/List_c.cpp:74](../../source/game_sa/Core/List_c.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1112"></a>

## Globales y helpers: source/game_sa/Core/Matrix.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall MatrixGetRightCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:99](../../source/game_sa/Core/Matrix.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetRightCopy 0x0041BD00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:102](../../source/game_sa/Core/Matrix.cpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetForwardCopy 0x0041BD10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixDestructorThunk(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:106](../../source/game_sa/Core/Matrix.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::DestructorThunk 0x0054E8E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote101A() | [source/game_sa/Core/Matrix.cpp:150](../../source/game_sa/Core/Matrix.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote103A() | [source/game_sa/Core/Matrix.cpp:168](../../source/game_sa/Core/Matrix.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote105A() | [source/game_sa/Core/Matrix.cpp:182](../../source/game_sa/Core/Matrix.cpp#L182) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote166A() | [source/game_sa/Core/Matrix.cpp:205](../../source/game_sa/Core/Matrix.cpp#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixCopyFromRw(volatile uint32* dst, const volatile uint32* src) | [source/game_sa/Core/Matrix.cpp:302](../../source/game_sa/Core/Matrix.cpp#L302) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EulerAxesFromFlags(uint32 flags) | [source/game_sa/Core/Matrix.cpp:522](../../source/game_sa/Core/Matrix.cpp#L522) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:587](../../source/game_sa/Core/Matrix.cpp#L587) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CVector& b) | [source/game_sa/Core/Matrix.cpp:598](../../source/game_sa/Core/Matrix.cpp#L598) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:607](../../source/game_sa/Core/Matrix.cpp#L607) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Invert(CMatrix& in, CMatrix& out) | [source/game_sa/Core/Matrix.cpp:624](../../source/game_sa/Core/Matrix.cpp#L624) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::Invert-1 0x0059B920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(CMatrix from, CMatrix to, float t) | [source/game_sa/Core/Matrix.cpp:648](../../source/game_sa/Core/Matrix.cpp#L648) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1113"></a>

## Globales y helpers: source/game_sa/Core/Quaternion.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DotProduct(const CQuaternion& q1, const CQuaternion& q2) | [source/game_sa/Core/Quaternion.h:122](../../source/game_sa/Core/Quaternion.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:129](../../source/game_sa/Core/Quaternion.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:133](../../source/game_sa/Core/Quaternion.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:137](../../source/game_sa/Core/Quaternion.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:141](../../source/game_sa/Core/Quaternion.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:145](../../source/game_sa/Core/Quaternion.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1114"></a>

## Globales y helpers: source/game_sa/Core/Vector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Vector2DMagnitudeExe(const CVector2D* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:54](../../source/game_sa/Core/Vector.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProductInPlaceExe(CVector* out, void* /*edx*/, const CVector* a, const CVector* b) | [source/game_sa/Core/Vector.cpp:101](../../source/game_sa/Core/Vector.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall VectorDefaultConstructorExe(CVector* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:125](../../source/game_sa/Core/Vector.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::DefaultConstructor 0x00404350 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CrossProduct(CVector* out, CVector* a, CVector* b) | [source/game_sa/Core/Vector.cpp:396](../../source/game_sa/Core/Vector.cpp#L396) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::CrossProduct-out 0x0059C730 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Multiply3x3VMExe(CVector* out, const CVector* vector, const CMatrix* matrix) | [source/game_sa/Core/Vector.cpp:441](../../source/game_sa/Core/Vector.cpp#L441) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * VectorTimesFloat(CVector* out, const CVector* vector, float factor) | [source/game_sa/Core/Vector.cpp:516](../../source/game_sa/Core/Vector.cpp#L516) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProduct(const CVector& a, const CVector& b) | [source/game_sa/Core/Vector.cpp:528](../../source/game_sa/Core/Vector.cpp#L528) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(CVector* v1, CVector* v2) | [source/game_sa/Core/Vector.cpp:534](../../source/game_sa/Core/Vector.cpp#L534) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::DotProduct-v3d*v3d* 0x0040FDB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:539](../../source/game_sa/Core/Vector.cpp#L539) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:544](../../source/game_sa/Core/Vector.cpp#L544) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1115"></a>

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

<a id="grupo-1116"></a>

## Globales y helpers: source/game_sa/Core/Vector2D.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator-(const CVector2D& lhs) | [source/game_sa/Core/Vector2D.h:229](../../source/game_sa/Core/Vector2D.h#L229) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:233](../../source/game_sa/Core/Vector2D.h#L233) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:237](../../source/game_sa/Core/Vector2D.h#L237) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:241](../../source/game_sa/Core/Vector2D.h#L241) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:245](../../source/game_sa/Core/Vector2D.h#L245) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CVector2D& vec, float dividend) | [source/game_sa/Core/Vector2D.h:249](../../source/game_sa/Core/Vector2D.h#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float multiplier, const CVector2D& vec) | [source/game_sa/Core/Vector2D.h:253](../../source/game_sa/Core/Vector2D.h#L253) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:257](../../source/game_sa/Core/Vector2D.h#L257) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector2D& vec, float notEqualTo) | [source/game_sa/Core/Vector2D.h:261](../../source/game_sa/Core/Vector2D.h#L261) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector2D& vec, float equalTo) | [source/game_sa/Core/Vector2D.h:265](../../source/game_sa/Core/Vector2D.h#L265) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector2D& vecLeft, const CVector2D& vecRight) | [source/game_sa/Core/Vector2D.h:269](../../source/game_sa/Core/Vector2D.h#L269) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPoints2D(const CVector2D& pointOne, const CVector2D& pointTwo) | [source/game_sa/Core/Vector2D.h:273](../../source/game_sa/Core/Vector2D.h#L273) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPointsSquared2D(const CVector2D& pointOne, const CVector2D& pointTwo) | [source/game_sa/Core/Vector2D.h:277](../../source/game_sa/Core/Vector2D.h#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector2D& v1, const CVector2D& v2) | [source/game_sa/Core/Vector2D.h:281](../../source/game_sa/Core/Vector2D.h#L281) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointWithinBounds2D(CVector2D min, CVector2D max, CVector2D point) | [source/game_sa/Core/Vector2D.h:285](../../source/game_sa/Core/Vector2D.h#L285) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInRectangle2D(CVector2D rectTopLeft, CVector2D rectSize, CVector2D point) | [source/game_sa/Core/Vector2D.h:290](../../source/game_sa/Core/Vector2D.h#L290) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Normalized2D(CVector2D v) | [source/game_sa/Core/Vector2D.h:294](../../source/game_sa/Core/Vector2D.h#L294) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1117"></a>

## Globales y helpers: source/game_sa/Coronas.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CoronaMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Coronas.cpp:173](../../source/game_sa/Coronas.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoronaWorldPosition(const CRegisteredCorona& c, bool checkBikeLean) | [source/game_sa/Coronas.cpp:183](../../source/game_sa/Coronas.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SinTimesTenX87(float angle) | [source/game_sa/Coronas.cpp:196](../../source/game_sa/Coronas.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSunReflectionVertex(uint32 index, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/Coronas.cpp:201](../../source/game_sa/Coronas.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:289 (0x00C4D958) |

<a id="grupo-1118"></a>

## Globales y helpers: source/game_sa/Cover.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RawPoints() | [source/game_sa/Cover.cpp:62](../../source/game_sa/Cover.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_Points: source/game_sa/Cover.h:21 (0x00C197C8) |
| & Raw(CCoverPoint& cpt) | [source/game_sa/Cover.cpp:63](../../source/game_sa/Cover.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Raw(const CCoverPoint& cpt) | [source/game_sa/Cover.cpp:64](../../source/game_sa/Cover.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsCoverPoint(CoverPointRaw& raw) | [source/game_sa/Cover.cpp:65](../../source/game_sa/Cover.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & NumPoints() | [source/game_sa/Cover.cpp:66](../../source/game_sa/Cover.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_NumPoints: source/game_sa/Cover.h:20 (0x00C197A4) |
| & At(const void* base, size_t offset) | [source/game_sa/Cover.cpp:69](../../source/game_sa/Cover.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ColModelOf(const CEntity* entity) | [source/game_sa/Cover.cpp:71](../../source/game_sa/Cover.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| CoverCross(const CVector& a, const CVector& b) | [source/game_sa/Cover.cpp:76](../../source/game_sa/Cover.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::CrossProduct-out 0x0059C730 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:81](../../source/game_sa/Cover.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::Multiply3x3Exe 0x0059C790 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:90](../../source/game_sa/Cover.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverNormalise(CVector& v) | [source/game_sa/Cover.cpp:99](../../source/game_sa/Cover.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::Normalise 0x0059C910 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1119"></a>

## Globales y helpers: source/game_sa/CullZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPointWithinArbitraryArea(CVector2D p, CVector2D p1, CVector2D p2, CVector2D p3, CVector2D p4) | [source/game_sa/CullZones.cpp:275](../../source/game_sa/CullZones.cpp#L275) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1120"></a>

## Globales y helpers: source/game_sa/CustomCarPlateMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatecharAtoi(const char* s) | [source/game_sa/CustomCarPlateMgr.cpp:117](../../source/game_sa/CustomCarPlateMgr.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResolvePlateType(uint8 plateType) | [source/game_sa/CustomCarPlateMgr.cpp:183](../../source/game_sa/CustomCarPlateMgr.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSet(char c) | [source/game_sa/CustomCarPlateMgr.cpp:196](../../source/game_sa/CustomCarPlateMgr.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomCarPlateMgr::GetCharacterPositionInCharSetExe 0x006FD7C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSetExe(uint32* outCol, uint32* outRow) | [source/game_sa/CustomCarPlateMgr.cpp:307](../../source/game_sa/CustomCarPlateMgr.cpp#L307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
