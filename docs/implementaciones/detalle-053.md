# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1041"></a>

## Globales y helpers: source/game_sa/CarEnterExit.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputePassengerIndexFromCarDoor_KeepEdx(const CVehicle* vehicle, int32 doorId) | [source/game_sa/CarEnterExit.cpp:35](../../source/game_sa/CarEnterExit.cpp#L35) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1042"></a>

## Globales y helpers: source/game_sa/CdStreamInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectCdStreamHooks() | [source/game_sa/CdStreamInfo.cpp:37](../../source/game_sa/CdStreamInfo.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CdStreamHandleToFileID(CdStreamHandle h) | [source/game_sa/CdStreamInfo.cpp:313](../../source/game_sa/CdStreamInfo.cpp#L313) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1043"></a>

## Globales y helpers: source/game_sa/CdStreamInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CdStreamGetLastPosn() | [source/game_sa/CdStreamInfo.h:94](../../source/game_sa/CdStreamInfo.h#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gLastCdStreamPosn: source/game_sa/CdStreamInfo.cpp:33 (0x008E4898) |

<a id="grupo-1044"></a>

## Globales y helpers: source/game_sa/Clothes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClothesGetTextureDependency(eClothesTexturePart texturePart) | [source/game_sa/Clothes.cpp:34](../../source/game_sa/Clothes.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesGetTextureDependencyKeepRegs() | [source/game_sa/Clothes.cpp:40](../../source/game_sa/Clothes.cpp#L40) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GetClothesModelFromName(const char* name) | [source/game_sa/Clothes.cpp:85](../../source/game_sa/Clothes.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1045"></a>

## Globales y helpers: source/game_sa/ClothesBuilder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClothesBlendPair(float a, float b, float ra, float rb) | [source/game_sa/ClothesBuilder.cpp:246](../../source/game_sa/ClothesBuilder.cpp#L246) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClothesBlendTriple(float a, float b, float c, float ra, float rb, float rc) | [source/game_sa/ClothesBuilder.cpp:259](../../source/game_sa/ClothesBuilder.cpp#L259) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ClothesNormalizeWeights(float* weights) | [source/game_sa/ClothesBuilder.cpp:275](../../source/game_sa/ClothesBuilder.cpp#L275) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| * BlendGeometry(RpClump* clump, std::pair&lt;const char*, float&gt; (&&frameNamesRatios)\[N\]) | [source/game_sa/ClothesBuilder.cpp:299](../../source/game_sa/ClothesBuilder.cpp#L299) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AssertTextureLayouts(std::initializer_list&lt;RwTexture*&gt; textures) | [source/game_sa/ClothesBuilder.cpp:484](../../source/game_sa/ClothesBuilder.cpp#L484) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClothesBlendByte(float v) | [source/game_sa/ClothesBuilder.cpp:553](../../source/game_sa/ClothesBuilder.cpp#L553) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1046"></a>

## Globales y helpers: source/game_sa/Clouds.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CloudsRotationSumX87(int32 value, float delta) | [source/game_sa/Clouds.cpp:59](../../source/game_sa/Clouds.cpp#L59) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CloudsFtol(float value) | [source/game_sa/Clouds.cpp:73](../../source/game_sa/Clouds.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CloudsCameraRight() | [source/game_sa/Clouds.cpp:78](../../source/game_sa/Clouds.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |

<a id="grupo-1047"></a>

## Globales y helpers: source/game_sa/Collision/ColSphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CColSphere& sp, const CMatrix& transform) -&gt; CColSphere | [source/game_sa/Collision/ColSphere.cpp:129](../../source/game_sa/Collision/ColSphere.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1048"></a>

## Globales y helpers: source/game_sa/Collision/ColStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityAreaCode(const CEntity* entity) | [source/game_sa/Collision/ColStore.cpp:34](../../source/game_sa/Collision/ColStore.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1049"></a>

## Globales y helpers: source/game_sa/Collision/Collision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClosestSquaredDistanceBetweenFiniteLines( const CVector& s1p0, const CVector& s2p0, const CVector& s2p1, const CVector& u, float a ) | [source/game_sa/Collision/Collision.cpp:682](../../source/game_sa/Collision/Collision.cpp#L682) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClosestPtPointTriangle( CVector a, CVector b, CVector c, CVector p ) | [source/game_sa/Collision/Collision.cpp:830](../../source/game_sa/Collision/Collision.cpp#L830) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessLineSphere_Internal( const CColLine& line, const CColSphere& sphere, CVector* ip, float* depth ) | [source/game_sa/Collision/Collision.cpp:890](../../source/game_sa/Collision/Collision.cpp#L890) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOTSA_FORCEINLINE ProcessLineTriangle_Internal( const CColLine& line, const CStoredCollPoly& poly, const CColTrianglePlane& plane, float* inOutMaxTouchDist, CVector* outIP, CVector* outPlNorm ) | [source/game_sa/Collision/Collision.cpp:1455](../../source/game_sa/Collision/Collision.cpp#L1455) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNearestDistanceOfPedSphereToCameraNearClip(CPed* ped) | [source/game_sa/Collision/Collision.cpp:3137](../../source/game_sa/Collision/Collision.cpp#L3137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |
| IsPointInPoly2D(CVector2D pt, CVector2D a, CVector2D b, CVector2D c) | [source/game_sa/Collision/Collision.cpp:3298](../../source/game_sa/Collision/Collision.cpp#L3298) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SphereVsEntityBody(CColSphere* /*sphere: ignorada, como en el EXE*/, CEntity* entity) | [source/game_sa/Collision/Collision.cpp:4119](../../source/game_sa/Collision/Collision.cpp#L4119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |

<a id="grupo-1050"></a>

## Globales y helpers: source/game_sa/Collision/ColourSet.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Ftol(float value) | [source/game_sa/Collision/ColourSet.cpp:110](../../source/game_sa/Collision/ColourSet.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1051"></a>

## Globales y helpers: source/game_sa/Collision/Sphere.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformObject(const CSphere& sp, const CMatrix& transform) -&gt; CSphere | [source/game_sa/Collision/Sphere.cpp:54](../../source/game_sa/Collision/Sphere.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1052"></a>

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

<a id="grupo-1053"></a>

## Globales y helpers: source/game_sa/ControllerConfigManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MouseButtonByte(const CMouseControllerState& state, KeyCode key) | [source/game_sa/ControllerConfigManager.cpp:950](../../source/game_sa/ControllerConfigManager.cpp#L950) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KeyCharToGxtChar(uint8 ch) | [source/game_sa/ControllerConfigManager.cpp:1218](../../source/game_sa/ControllerConfigManager.cpp#L1218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x008D5AD8 |

<a id="grupo-1054"></a>

## Globales y helpers: source/game_sa/Conversations/ConversationForPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ConversationTooFar(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:144](../../source/game_sa/Conversations/ConversationForPed.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationFacingAway(const CVector& ped, const CVector& player) | [source/game_sa/Conversations/ConversationForPed.cpp:152](../../source/game_sa/Conversations/ConversationForPed.cpp#L152) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConversationMovingTooFast(const CVector& speed) | [source/game_sa/Conversations/ConversationForPed.cpp:157](../../source/game_sa/Conversations/ConversationForPed.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1055"></a>

## Globales y helpers: source/game_sa/Core/EntryInfoNode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RH_ScopedInstall(operator new, 0x536DC0); RH_ScopedInstall(operator delete, 0x536DD0); } /** * Obtiene almacenamiento del pool global dedicado a CEntryInfoNode. * * El parámetro size forma parte de la firma de operator new, pero el tamaño real * del bloque está fijado por el tipo del pool. */ void* CEntryInfoNode::operator new(unsigned size) | [source/game_sa/Core/EntryInfoNode.cpp:31](../../source/game_sa/Core/EntryInfoNode.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1056"></a>

## Globales y helpers: source/game_sa/Core/Matrix.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| operator*(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:1168](../../source/game_sa/Core/Matrix.cpp#L1168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CMatrix& a, const CVector& b) | [source/game_sa/Core/Matrix.cpp:1192](../../source/game_sa/Core/Matrix.cpp#L1192) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CMatrix& a, const CMatrix& b) | [source/game_sa/Core/Matrix.cpp:1196](../../source/game_sa/Core/Matrix.cpp#L1196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Invert(CMatrix& in, CMatrix& out) | [source/game_sa/Core/Matrix.cpp:1206](../../source/game_sa/Core/Matrix.cpp#L1206) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Invert(const CMatrix& in) | [source/game_sa/Core/Matrix.cpp:1212](../../source/game_sa/Core/Matrix.cpp#L1212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lerp(CMatrix from, CMatrix to, float t) | [source/game_sa/Core/Matrix.cpp:1217](../../source/game_sa/Core/Matrix.cpp#L1217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1057"></a>

## Globales y helpers: source/game_sa/Core/OctTree.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| COctTree | [source/game_sa/Core/OctTree.h:13](../../source/game_sa/Core/OctTree.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_bFailed: source/game_sa/GenericGameStorage.h:69 (0x00C16EFC) |

<a id="grupo-1058"></a>

## Globales y helpers: source/game_sa/Core/OctTreeBase.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| COctTreeBase | [source/game_sa/Core/OctTreeBase.h:11](../../source/game_sa/Core/OctTreeBase.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1059"></a>

## Globales y helpers: source/game_sa/Core/Quaternion.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DotProduct(const CQuaternion& q1, const CQuaternion& q2) | [source/game_sa/Core/Quaternion.h:122](../../source/game_sa/Core/Quaternion.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:129](../../source/game_sa/Core/Quaternion.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-(const CQuaternion& left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:133](../../source/game_sa/Core/Quaternion.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:137](../../source/game_sa/Core/Quaternion.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(float left, const CQuaternion& right) | [source/game_sa/Core/Quaternion.h:141](../../source/game_sa/Core/Quaternion.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator/(const CQuaternion& left, float right) | [source/game_sa/Core/Quaternion.h:145](../../source/game_sa/Core/Quaternion.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1060"></a>

## Globales y helpers: source/game_sa/Core/Vector.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * CrossProduct(CVector* out, CVector* a, CVector* b) | [source/game_sa/Core/Vector.cpp:183](../../source/game_sa/Core/Vector.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CrossProduct(const CVector& a, const CVector& b) | [source/game_sa/Core/Vector.cpp:189](../../source/game_sa/Core/Vector.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(CVector* v1, CVector* v2) | [source/game_sa/Core/Vector.cpp:194](../../source/game_sa/Core/Vector.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:199](../../source/game_sa/Core/Vector.cpp#L199) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DotProduct2D(const CVector& v1, const CVector& v2) | [source/game_sa/Core/Vector.cpp:204](../../source/game_sa/Core/Vector.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
