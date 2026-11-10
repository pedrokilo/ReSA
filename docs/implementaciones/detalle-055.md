<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 872 grupos / 9677 funciones reconstruidas; 9677 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1081"></a>

## Globales y helpers: source/game_sa/Collision/CollisionData.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall CollisionDataConstructorExe(CCollisionData* self, void* /*edx*/) | [source/game_sa/Collision/CollisionData.cpp:27](../../source/game_sa/Collision/CollisionData.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCollisionData::Constructor 0x0040F030 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1082"></a>

## Globales y helpers: source/game_sa/Collision/CollisionEventScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Dot(const CVector& a, const CVector& b) | [source/game_sa/Collision/CollisionEventScanner.cpp:64](../../source/game_sa/Collision/CollisionEventScanner.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::DotProduct-v3d*v3d* 0x0040FDB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Collision/CollisionEventScanner.cpp:72](../../source/game_sa/Collision/CollisionEventScanner.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FtolLow(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:81](../../source/game_sa/Collision/CollisionEventScanner.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ImpactDotMoveShift(const CPed* ped) | [source/game_sa/Collision/CollisionEventScanner.cpp:86](../../source/game_sa/Collision/CollisionEventScanner.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampBelowZero(float value) | [source/game_sa/Collision/CollisionEventScanner.cpp:93](../../source/game_sa/Collision/CollisionEventScanner.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCarAI::FloatMax 0x00420800 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DamageFromAttachedVehicle(CPed* victim, CVehicle* vehicle) | [source/game_sa/Collision/CollisionEventScanner.cpp:98](../../source/game_sa/Collision/CollisionEventScanner.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1083"></a>

## Globales y helpers: source/game_sa/Collision/ColourSet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/Collision/ColourSet.cpp:110](../../source/game_sa/Collision/ColourSet.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1084"></a>

## Globales y helpers: source/game_sa/Collision/Sphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CSphere& sp, const CMatrix& transform) -&gt; CSphere | [source/game_sa/Collision/Sphere.cpp:90](../../source/game_sa/Collision/Sphere.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1085"></a>

## Globales y helpers: source/game_sa/CompressedVector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CompressedVectorInjectHooksLote105A() | [source/game_sa/CompressedVector.cpp:48](../../source/game_sa/CompressedVector.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1086"></a>

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

<a id="grupo-1087"></a>

## Globales y helpers: source/game_sa/ControllerConfigManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MouseButtonByte(const CMouseControllerState& state, KeyCode key) | [source/game_sa/ControllerConfigManager.cpp:950](../../source/game_sa/ControllerConfigManager.cpp#L950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KeyCharToGxtChar(uint8 ch) | [source/game_sa/ControllerConfigManager.cpp:1218](../../source/game_sa/ControllerConfigManager.cpp#L1218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x008D5AD8 |

<a id="grupo-1088"></a>

## Globales y helpers: source/game_sa/Conversations/ConversationForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConversationTooFar(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:144](../../source/game_sa/Conversations/ConversationForPed.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationFacingAway(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:152](../../source/game_sa/Conversations/ConversationForPed.cpp#L152) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationMovingTooFast(const CVector& speed) | [source/game_sa/Conversations/ConversationForPed.cpp:157](../../source/game_sa/Conversations/ConversationForPed.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1089"></a>

## Globales y helpers: source/game_sa/Core/List_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TListInjectHooksLote105A() | [source/game_sa/Core/List_c.cpp:74](../../source/game_sa/Core/List_c.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1090"></a>

## Globales y helpers: source/game_sa/Core/Matrix.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall MatrixGetRightCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:88](../../source/game_sa/Core/Matrix.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetRightCopy 0x0041BD00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:91](../../source/game_sa/Core/Matrix.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::GetForwardCopy 0x0041BD10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixDestructorThunk(CMatrix* self, void* /*edx*/) | [source/game_sa/Core/Matrix.cpp:95](../../source/game_sa/Core/Matrix.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::DestructorThunk 0x0054E8E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote101A() | [source/game_sa/Core/Matrix.cpp:139](../../source/game_sa/Core/Matrix.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote103A() | [source/game_sa/Core/Matrix.cpp:157](../../source/game_sa/Core/Matrix.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CMatrixInjectHooksLote105A() | [source/game_sa/Core/Matrix.cpp:171](../../source/game_sa/Core/Matrix.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EulerAxesFromFlags(uint32 flags) | [source/game_sa/Core/Matrix.cpp:461](../../source/game_sa/Core/Matrix.cpp#L461) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:526](../../source/game_sa/Core/Matrix.cpp#L526) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CVector& b) | [source/game_sa/Core/Matrix.cpp:537](../../source/game_sa/Core/Matrix.cpp#L537) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:546](../../source/game_sa/Core/Matrix.cpp#L546) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Invert(CMatrix& in, CMatrix& out) | [source/game_sa/Core/Matrix.cpp:563](../../source/game_sa/Core/Matrix.cpp#L563) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CMatrix::Invert-1 0x0059B920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(CMatrix from, CMatrix to, float t) | [source/game_sa/Core/Matrix.cpp:587](../../source/game_sa/Core/Matrix.cpp#L587) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1091"></a>

## Globales y helpers: source/game_sa/Core/Quaternion.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DotProduct(const CQuaternion& q1, const CQuaternion& q2) | [source/game_sa/Core/Quaternion.h:122](../../source/game_sa/Core/Quaternion.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:129](../../source/game_sa/Core/Quaternion.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:133](../../source/game_sa/Core/Quaternion.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:137](../../source/game_sa/Core/Quaternion.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:141](../../source/game_sa/Core/Quaternion.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:145](../../source/game_sa/Core/Quaternion.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1092"></a>

## Globales y helpers: source/game_sa/Core/Vector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Vector2DMagnitudeExe(const CVector2D* self, void* /*edx*/) | [source/game_sa/Core/Vector.cpp:54](../../source/game_sa/Core/Vector.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProductInPlaceExe(CVector* out, void* /*edx*/, const CVector* a, const CVector* b) | [source/game_sa/Core/Vector.cpp:101](../../source/game_sa/Core/Vector.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CrossProduct(CVector* out, CVector* a, CVector* b) | [source/game_sa/Core/Vector.cpp:384](../../source/game_sa/Core/Vector.cpp#L384) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::CrossProduct-out 0x0059C730 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Multiply3x3VMExe(CVector* out, const CVector* vector, const CMatrix* matrix) | [source/game_sa/Core/Vector.cpp:429](../../source/game_sa/Core/Vector.cpp#L429) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * VectorTimesFloat(CVector* out, const CVector* vector, float factor) | [source/game_sa/Core/Vector.cpp:504](../../source/game_sa/Core/Vector.cpp#L504) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProduct(const CVector& a, const CVector& b) | [source/game_sa/Core/Vector.cpp:516](../../source/game_sa/Core/Vector.cpp#L516) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(CVector* v1, CVector* v2) | [source/game_sa/Core/Vector.cpp:522](../../source/game_sa/Core/Vector.cpp#L522) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CVector::DotProduct-v3d*v3d* 0x0040FDB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:527](../../source/game_sa/Core/Vector.cpp#L527) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:532](../../source/game_sa/Core/Vector.cpp#L532) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1093"></a>

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

<a id="grupo-1094"></a>

## Globales y helpers: source/game_sa/Core/Vector2D.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator-(const CVector2D& lhs) | [source/game_sa/Core/Vector2D.h:228](../../source/game_sa/Core/Vector2D.h#L228) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:232](../../source/game_sa/Core/Vector2D.h#L232) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:236](../../source/game_sa/Core/Vector2D.h#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:240](../../source/game_sa/Core/Vector2D.h#L240) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:244](../../source/game_sa/Core/Vector2D.h#L244) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CVector2D& vec, float dividend) | [source/game_sa/Core/Vector2D.h:248](../../source/game_sa/Core/Vector2D.h#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float multiplier, const CVector2D& vec) | [source/game_sa/Core/Vector2D.h:252](../../source/game_sa/Core/Vector2D.h#L252) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector2D& vecOne, const CVector2D& vecTwo) | [source/game_sa/Core/Vector2D.h:256](../../source/game_sa/Core/Vector2D.h#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator!=(const CVector2D& vec, float notEqualTo) | [source/game_sa/Core/Vector2D.h:260](../../source/game_sa/Core/Vector2D.h#L260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector2D& vec, float equalTo) | [source/game_sa/Core/Vector2D.h:264](../../source/game_sa/Core/Vector2D.h#L264) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const CVector2D& vecLeft, const CVector2D& vecRight) | [source/game_sa/Core/Vector2D.h:268](../../source/game_sa/Core/Vector2D.h#L268) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPoints2D(const CVector2D& pointOne, const CVector2D& pointTwo) | [source/game_sa/Core/Vector2D.h:272](../../source/game_sa/Core/Vector2D.h#L272) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DistanceBetweenPointsSquared2D(const CVector2D& pointOne, const CVector2D& pointTwo) | [source/game_sa/Core/Vector2D.h:276](../../source/game_sa/Core/Vector2D.h#L276) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector2D& v1, const CVector2D& v2) | [source/game_sa/Core/Vector2D.h:280](../../source/game_sa/Core/Vector2D.h#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointWithinBounds2D(CVector2D min, CVector2D max, CVector2D point) | [source/game_sa/Core/Vector2D.h:284](../../source/game_sa/Core/Vector2D.h#L284) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInRectangle2D(CVector2D rectTopLeft, CVector2D rectSize, CVector2D point) | [source/game_sa/Core/Vector2D.h:289](../../source/game_sa/Core/Vector2D.h#L289) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Normalized2D(CVector2D v) | [source/game_sa/Core/Vector2D.h:293](../../source/game_sa/Core/Vector2D.h#L293) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1095"></a>

## Globales y helpers: source/game_sa/Coronas.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CoronaMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Coronas.cpp:173](../../source/game_sa/Coronas.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoronaWorldPosition(const CRegisteredCorona& c, bool checkBikeLean) | [source/game_sa/Coronas.cpp:183](../../source/game_sa/Coronas.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SinTimesTenX87(float angle) | [source/game_sa/Coronas.cpp:196](../../source/game_sa/Coronas.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSunReflectionVertex(uint32 index, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/Coronas.cpp:201](../../source/game_sa/Coronas.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958) |

<a id="grupo-1096"></a>

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

<a id="grupo-1097"></a>

## Globales y helpers: source/game_sa/CullZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPointWithinArbitraryArea(CVector2D p, CVector2D p1, CVector2D p2, CVector2D p3, CVector2D p4) | [source/game_sa/CullZones.cpp:275](../../source/game_sa/CullZones.cpp#L275) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1098"></a>

## Globales y helpers: source/game_sa/CustomCarPlateMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatecharAtoi(const char* s) | [source/game_sa/CustomCarPlateMgr.cpp:117](../../source/game_sa/CustomCarPlateMgr.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResolvePlateType(uint8 plateType) | [source/game_sa/CustomCarPlateMgr.cpp:183](../../source/game_sa/CustomCarPlateMgr.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSet(char c) | [source/game_sa/CustomCarPlateMgr.cpp:196](../../source/game_sa/CustomCarPlateMgr.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomCarPlateMgr::GetCharacterPositionInCharSetExe 0x006FD7C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSetExe(uint32* outCol, uint32* outRow) | [source/game_sa/CustomCarPlateMgr.cpp:307](../../source/game_sa/CustomCarPlateMgr.cpp#L307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1099"></a>

## Globales y helpers: source/game_sa/CustomRoadsignMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RoadsignSetMaterialAlphaCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:350](../../source/game_sa/CustomRoadsignMgr.cpp#L350) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RoadsignSetMaterialTextureCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:356](../../source/game_sa/CustomRoadsignMgr.cpp#L356) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RoadsignGetLineAndRow(char cLetter, int32* line, int32* row) | [source/game_sa/CustomRoadsignMgr.cpp:402](../../source/game_sa/CustomRoadsignMgr.cpp#L402) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomRoadsignMgr::RoadsignGetLineAndRowExe 0x006FE260 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RoadsignGetLineAndRowExe(int32* line, int32* row) | [source/game_sa/CustomRoadsignMgr.cpp:762](../../source/game_sa/CustomRoadsignMgr.cpp#L762) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1100"></a>

## Globales y helpers: source/game_sa/CutsceneMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RestorePlayerWeapon(CPed* ped, eWeaponType type, int32 ammo) | [source/game_sa/CutsceneMgr.cpp:165](../../source/game_sa/CutsceneMgr.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aInfoForModel: source/game_sa/Streaming.h:247 (0x008E4CC0) |
