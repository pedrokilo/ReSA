# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1401"></a>

## Globales y helpers: source/game_sa/Scripts/RunningScript.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| LocateScriptEntity(CRunningScript* script, const CVector& pos, bool threeD, bool object, bool forceFalse) | [source/game_sa/Scripts/RunningScript.cpp:339](../../source/game_sa/Scripts/RunningScript.cpp#L339) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CRunningScript::LocateCarCommand 0x00487A20 | 0 / 0 | Posible dato fijo ScriptParams: source/game_sa/Scripts/RunningScript.h:81 (0x00A43C78) |
| ScriptPedLocation(CPed* ped) | [source/game_sa/Scripts/RunningScript.cpp:364](../../source/game_sa/Scripts/RunningScript.cpp#L364) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CRunningScript::LocateCharCommand 0x00486D80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LocateScriptPedPair(CRunningScript* script, CPed* ped, const CVector& target, bool threeD, int32 mode) | [source/game_sa/Scripts/RunningScript.cpp:397](../../source/game_sa/Scripts/RunningScript.cpp#L397) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CRunningScript::LocateCharCarCommand 0x00487420 | 0 / 0 | Posible dato fijo ScriptParams: source/game_sa/Scripts/RunningScript.h:81 (0x00A43C78) |
| ScriptAreaTest(CRunningScript* script, const CVector& pos, bool threeD, bool forceFalse) | [source/game_sa/Scripts/RunningScript.cpp:441](../../source/game_sa/Scripts/RunningScript.cpp#L441) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CRunningScript::CarInAreaCheckCommand 0x00488EC0 | 0 / 0 | Posible dato fijo ScriptParams: source/game_sa/Scripts/RunningScript.h:81 (0x00A43C78) |
| MakeScriptAngledBox(bool threeD, int offset) | [source/game_sa/Scripts/RunningScript.cpp:498](../../source/game_sa/Scripts/RunningScript.cpp#L498) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Posible dato fijo ScriptParams: source/game_sa/Scripts/RunningScript.h:81 (0x00A43C78); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ScriptNormalizeAngled(CVector2D& v) | [source/game_sa/Scripts/RunningScript.cpp:570](../../source/game_sa/Scripts/RunningScript.cpp#L570) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptInsideAngled(ScriptAngledBox& box,const CVector& pos,bool flame) | [source/game_sa/Scripts/RunningScript.cpp:574](../../source/game_sa/Scripts/RunningScript.cpp#L574) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptDrawAngled(CRunningScript* script,const ScriptAngledBox& box) | [source/game_sa/Scripts/RunningScript.cpp:583](../../source/game_sa/Scripts/RunningScript.cpp#L583) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1402"></a>

## Globales y helpers: source/game_sa/Scripts/TheScripts.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RawNullEntityRef(notsa::EntityRef&lt;&gt;& ref) | [source/game_sa/Scripts/TheScripts.cpp:45](../../source/game_sa/Scripts/TheScripts.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResetIntroTextLine(tScriptText& t, float wrapX, float centreSize) | [source/game_sa/Scripts/TheScripts.cpp:139](../../source/game_sa/Scripts/TheScripts.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResetIntroRectangle(tScriptRectangle& r) | [source/game_sa/Scripts/TheScripts.cpp:165](../../source/game_sa/Scripts/TheScripts.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeAtan2(float num, float den) | [source/game_sa/Scripts/TheScripts.cpp:1687](../../source/game_sa/Scripts/TheScripts.cpp#L1687) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StepSearchLightTarget(tScriptSearchlight& sl, const CVector& goal) | [source/game_sa/Scripts/TheScripts.cpp:1701](../../source/game_sa/Scripts/TheScripts.cpp#L1701) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DebugProbeVerticalLine(float x, float y) | [source/game_sa/Scripts/TheScripts.cpp:1930](../../source/game_sa/Scripts/TheScripts.cpp#L1930) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptRawRect(const CVector2D& a, const CVector2D& b) | [source/game_sa/Scripts/TheScripts.cpp:1993](../../source/game_sa/Scripts/TheScripts.cpp#L1993) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeMultiply3x3MV(const CMatrix& m, const CVector& v) | [source/game_sa/Scripts/TheScripts.cpp:2268](../../source/game_sa/Scripts/TheScripts.cpp#L2268) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1403"></a>

## Globales y helpers: source/game_sa/ShadowCamera.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * atomicQuickRender(RpAtomic* atomic, void* data) | [source/game_sa/ShadowCamera.cpp:227](../../source/game_sa/ShadowCamera.cpp#L227) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x007491C0 |

<a id="grupo-1404"></a>

## Globales y helpers: source/game_sa/Shadows.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PermanentShadowFadeInteger(double elapsed, double duration, int32 value) | [source/game_sa/Shadows.cpp:194](../../source/game_sa/Shadows.cpp#L194) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CalculateShadowStrength(float currDist, float maxDist, int32 maxStrength) | [source/game_sa/Shadows.cpp:476](../../source/game_sa/Shadows.cpp#L476) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShadowXYDot(float x,float a,float y,float b) | [source/game_sa/Shadows.cpp:841](../../source/game_sa/Shadows.cpp#L841) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 6 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowXYCross(const CVector& p,const CVector& origin,float edgeX,float edgeY) | [source/game_sa/Shadows.cpp:851](../../source/game_sa/Shadows.cpp#L851) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 8 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowXYBlend(float previous,float current,float ratio) | [source/game_sa/Shadows.cpp:864](../../source/game_sa/Shadows.cpp#L864) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 7 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowXYPlaneDistance(const CVector& n,const CVector& vertex) | [source/game_sa/Shadows.cpp:875](../../source/game_sa/Shadows.cpp#L875) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowXYPlaneHeight(const CVector& n,float x,float y,float distance) | [source/game_sa/Shadows.cpp:890](../../source/game_sa/Shadows.cpp#L890) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowXYClip(CVector* positions,CVector* uv,int count,CVector* outPositions,CVector* outUV,const CVector& origin,const CVector& destination) | [source/game_sa/Shadows.cpp:906](../../source/game_sa/Shadows.cpp#L906) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShadowProjectionRatio(float numerator, float value, float multiplier) | [source/game_sa/Shadows.cpp:986](../../source/game_sa/Shadows.cpp#L986) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CShadows::CastShadowEntityXYZ 0x0070A040 | 4 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowProjectionCoordinate(float base, float value, float multiplier) | [source/game_sa/Shadows.cpp:994](../../source/game_sa/Shadows.cpp#L994) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 4 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowNormalOffset(float value) | [source/game_sa/Shadows.cpp:1002](../../source/game_sa/Shadows.cpp#L1002) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 3 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowHeightSum(float a, float b) | [source/game_sa/Shadows.cpp:1160](../../source/game_sa/Shadows.cpp#L1160) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CShadows::CastShadowSectorList&lt;CPtrListSingleLink&lt;CPhysical*&gt;&gt; 0x0070A630 | 3 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowHeightDifference(float a, float b) | [source/game_sa/Shadows.cpp:1167](../../source/game_sa/Shadows.cpp#L1167) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 3 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowRectangleEdge(float position, float front, float side, bool maximum) | [source/game_sa/Shadows.cpp:1481](../../source/game_sa/Shadows.cpp#L1481) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CShadows::GeneratePolysForStaticShadow 0x0070B730 | 4 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowFtol(double value) | [source/game_sa/Shadows.cpp:1495](../../source/game_sa/Shadows.cpp#L1495) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShadowSectorIndex(float coordinate, bool maximum) | [source/game_sa/Shadows.cpp:1499](../../source/game_sa/Shadows.cpp#L1499) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowCameraDistanceSquared(const CVector& position, const CVector& camera) | [source/game_sa/Shadows.cpp:1536](../../source/game_sa/Shadows.cpp#L1536) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CShadows::StoreStaticShadow 0x0070BA00 | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowExtendedProduct(float a, float b) | [source/game_sa/Shadows.cpp:1558](../../source/game_sa/Shadows.cpp#L1558) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 3 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowFadeInteger(double fade, int32 value) | [source/game_sa/Shadows.cpp:1566](../../source/game_sa/Shadows.cpp#L1566) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowVehiclePosition(CVector& position,float extentLength,float maximumY,float shift,float forwardX,float forwardY) | [source/game_sa/Shadows.cpp:1634](../../source/game_sa/Shadows.cpp#L1634) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CShadows::StoreShadowForVehicle 0x0070BDA0 | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowTriangleAlpha(float depth, uint32 intensity) | [source/game_sa/Shadows.cpp:1862](../../source/game_sa/Shadows.cpp#L1862) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| * ShadowRenderTriangleCB(CVector* normal, CVector* trianglePos, _ProjectionParam* param) | [source/game_sa/Shadows.cpp:1888](../../source/game_sa/Shadows.cpp#L1888) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ShadowRenderTriangleRegisterEntry() | [source/game_sa/Shadows.cpp:1931](../../source/game_sa/Shadows.cpp#L1931) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1405"></a>

## Globales y helpers: source/game_sa/Shopping.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetChangingStatIndex(const char* stat) | [source/game_sa/Shopping.cpp:70](../../source/game_sa/Shopping.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1406"></a>

## Globales y helpers: source/game_sa/SpecialFX.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MultiplyPointLikeExe(const CMatrix& m, const CVector& v) | [source/game_sa/SpecialFX.cpp:100](../../source/game_sa/SpecialFX.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1407"></a>

## Globales y helpers: source/game_sa/Sprite.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CalcSpriteScreenZ(float z, float nearZ, float farZ) | [source/game_sa/Sprite.cpp:38](../../source/game_sa/Sprite.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fFarClipZ: source/game_sa/Draw.h:17 (0x00C3EF9C); Posible dato fijo ms_fNearClipZ: source/game_sa/Draw.h:16 (0x00C3EFA0) |
| CalcSpriteColor(uint8 r, uint8 g, uint8 b, uint8 a, int16 intensity) | [source/game_sa/Sprite.cpp:48](../../source/game_sa/Sprite.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSpriteOffScreen(const CornerCoords1D& xs, const CornerCoords1D& ys) | [source/game_sa/Sprite.cpp:60](../../source/game_sa/Sprite.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcSpriteQuadCoeffs(float rotation) | [source/game_sa/Sprite.cpp:78](../../source/game_sa/Sprite.cpp#L78) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CalcSpriteDimensionTerms(float rotation, float sprW, float sprH, float& outWCos, float& outHSin, float& outHCos, float& outWSin) | [source/game_sa/Sprite.cpp:107](../../source/game_sa/Sprite.cpp#L107) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SpriteMatrixTimesPoint(const CMatrix& mat, const CVector& v) | [source/game_sa/Sprite.cpp:133](../../source/game_sa/Sprite.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampSpriteQuadToScreen(CornerCoords1D& xs, CornerCoords1D& ys, CornerCoords1D& us, CornerCoords1D& vs, CVector2D halfSize) | [source/game_sa/Sprite.cpp:148](../../source/game_sa/Sprite.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSpriteVertices(RwIm2DVertex* vertices, const CornerCoords1D& xs, const CornerCoords1D& ys, const CornerCoords1D& us, const CornerCoords1D& vs, float z, float rhw, const std::array&lt;uint32, 4&gt;& colors) | [source/game_sa/Sprite.cpp:173](../../source/game_sa/Sprite.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSpriteVertices(RwIm2DVertex* vertices, const CornerCoords1D& xs, const CornerCoords1D& ys, const CornerCoords1D& us, const CornerCoords1D& vs, float z, float rhw, uint32 color) | [source/game_sa/Sprite.cpp:189](../../source/game_sa/Sprite.cpp#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSpriteVertices(RwIm2DVertex* vertices, const CornerCoords1D& xs, const CornerCoords1D& ys, float z, float rhw, uint32 color) | [source/game_sa/Sprite.cpp:195](../../source/game_sa/Sprite.cpp#L195) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PushSpriteQuad() | [source/game_sa/Sprite.cpp:201](../../source/game_sa/Sprite.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo aTempBufferIndices: source/game_sa/common.h:286 (0x00C4B958); Posible dato fijo nSpriteBufferIndex: source/game_sa/Sprite.cpp:22 (0x00C6A158) |

<a id="grupo-1408"></a>

## Globales y helpers: source/game_sa/Stats.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StatsFtol(float v) | [source/game_sa/Stats.cpp:38](../../source/game_sa/Stats.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & IntStat(eStats stat) | [source/game_sa/Stats.cpp:42](../../source/game_sa/Stats.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo StatTypesInt: source/game_sa/Stats.h:51 (0x00B79000) |
| CounterAsX87(uint32 c) | [source/game_sa/Stats.cpp:46](../../source/game_sa/Stats.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TimeStepMsInt() | [source/game_sa/Stats.cpp:51](../../source/game_sa/Stats.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RegisterLowerStat(eStats stat, int32 value) | [source/game_sa/Stats.cpp:56](../../source/game_sa/Stats.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo StatTypesFloat: source/game_sa/Stats.h:52 (0x00B79380) |
| Ui8_Bridge_0053FB70() | [source/game_sa/Stats.cpp:80](../../source/game_sa/Stats.cpp#L80) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x0053FB70; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_00588B50() | [source/game_sa/Stats.cpp:86](../../source/game_sa/Stats.cpp#L86) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00588B50; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_00558E40() | [source/game_sa/Stats.cpp:92](../../source/game_sa/Stats.cpp#L92) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00558E40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_006A0050() | [source/game_sa/Stats.cpp:98](../../source/game_sa/Stats.cpp#L98) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x006A0050; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_00588BE0() | [source/game_sa/Stats.cpp:104](../../source/game_sa/Stats.cpp#L104) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00588BE0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_00559080() | [source/game_sa/Stats.cpp:110](../../source/game_sa/Stats.cpp#L110) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00559080; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_00821B40() | [source/game_sa/Stats.cpp:116](../../source/game_sa/Stats.cpp#L116) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00821B40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_0055C180() | [source/game_sa/Stats.cpp:122](../../source/game_sa/Stats.cpp#L122) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x0055C180; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Ui8_Bridge_0055B980() | [source/game_sa/Stats.cpp:128](../../source/game_sa/Stats.cpp#L128) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x0055B980; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsFile15EntrySetDir() | [source/game_sa/Stats.cpp:662](../../source/game_sa/Stats.cpp#L662) | Ensamblador · Envoltura al EXE | Histórica del catálogo: CStats::LoadStatUpdateConditions 0x00559860 | 2 / 1 | Trampolín ASM a 0x005387D0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsFile15EntryOpenFile() | [source/game_sa/Stats.cpp:668](../../source/game_sa/Stats.cpp#L668) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00538900; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsFile15EntryLoadLine() | [source/game_sa/Stats.cpp:674](../../source/game_sa/Stats.cpp#L674) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00536F80; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsFile15EntryCloseFile() | [source/game_sa/Stats.cpp:680](../../source/game_sa/Stats.cpp#L680) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x005389D0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsFile15EntrySscanf() | [source/game_sa/Stats.cpp:686](../../source/game_sa/Stats.cpp#L686) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x008220AD; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsModifierMaxExe(float, float) | [source/game_sa/Stats.cpp:942](../../source/game_sa/Stats.cpp#L942) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsModifierMinExe(float, float) | [source/game_sa/Stats.cpp:956](../../source/game_sa/Stats.cpp#L956) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| StatsModifierBody(eStatModAbilities statMod) | [source/game_sa/Stats.cpp:971](../../source/game_sa/Stats.cpp#L971) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats10Increment() | [source/game_sa/Stats.cpp:2294](../../source/game_sa/Stats.cpp#L2294) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055C180; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats10Display() | [source/game_sa/Stats.cpp:2295](../../source/game_sa/Stats.cpp#L2295) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055B980; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats10Ftol() | [source/game_sa/Stats.cpp:2296](../../source/game_sa/Stats.cpp#L2296) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00821B40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats10CheckMessage() | [source/game_sa/Stats.cpp:2297](../../source/game_sa/Stats.cpp#L2297) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00559760; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats10FatMuscle() | [source/game_sa/Stats.cpp:2298](../../source/game_sa/Stats.cpp#L2298) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055C470; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats10SkillIndex() | [source/game_sa/Stats.cpp:2299](../../source/game_sa/Stats.cpp#L2299) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00743CD0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats10Coop() | [source/game_sa/Stats.cpp:2300](../../source/game_sa/Stats.cpp#L2300) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00441390; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats11Fat() | [source/game_sa/Stats.cpp:2583](../../source/game_sa/Stats.cpp#L2583) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055C470; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats11Increment() | [source/game_sa/Stats.cpp:2584](../../source/game_sa/Stats.cpp#L2584) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055C180; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats11Display() | [source/game_sa/Stats.cpp:2585](../../source/game_sa/Stats.cpp#L2585) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055B980; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats11Ftol() | [source/game_sa/Stats.cpp:2586](../../source/game_sa/Stats.cpp#L2586) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00821B40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats11Ceil() | [source/game_sa/Stats.cpp:2587](../../source/game_sa/Stats.cpp#L2587) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00823820; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats9Increment() | [source/game_sa/Stats.cpp:3064](../../source/game_sa/Stats.cpp#L3064) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055C180; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats9Display() | [source/game_sa/Stats.cpp:3065](../../source/game_sa/Stats.cpp#L3065) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0055B980; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| VehicleStats9Ftol() | [source/game_sa/Stats.cpp:3066](../../source/game_sa/Stats.cpp#L3066) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00821B40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1409"></a>

## Globales y helpers: source/game_sa/StencilShadows.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StencilMulPoint(const CMatrix& m, const CVector& v) | [source/game_sa/StencilShadows.cpp:47](../../source/game_sa/StencilShadows.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilTransformFromObjectSpace(const CEntity* entity, const CVector& offset) | [source/game_sa/StencilShadows.cpp:57](../../source/game_sa/StencilShadows.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x0054ECE0: Common::TransformPoint; ruta efectiva sin resolver |
| StencilSignedDistSq(const CVector& cam, CEntity* entity) | [source/game_sa/StencilShadows.cpp:68](../../source/game_sa/StencilShadows.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * StencilFindForOwner(const CEntity* owner) | [source/game_sa/StencilShadows.cpp:82](../../source/game_sa/StencilShadows.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pFirstActiveStencilShadowObject: source/game_sa/StencilShadows.h:10 (0x00C6A16C) |
| StencilObjectInvalid(CEntity* owner) | [source/game_sa/StencilShadows.cpp:92](../../source/game_sa/StencilShadows.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilToggleEdge(RxVertexIndex* edges, uint16& count, uint16 i0, uint16 i1) | [source/game_sa/StencilShadows.cpp:101](../../source/game_sa/StencilShadows.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StencilTransformPoints(CVector* out, int32 count, const CMatrix* matrix, const CVector* in) | [source/game_sa/StencilShadows.cpp:121](../../source/game_sa/StencilShadows.cpp#L121) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0054EEF0: Common::TransformPoints; ruta efectiva sin resolver |
| BuildShadowVolume(CStencilShadowObject* obj, bool forVehicle) | [source/game_sa/StencilShadows.cpp:128](../../source/game_sa/StencilShadows.cpp#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo s_ShadowTrianglePoints: source/game_sa/StencilShadows.cpp:28 (0x00C6A174); Posible dato fijo s_ShadowTrianglePointsUnk: source/game_sa/StencilShadows.cpp:27 (0x00C6A170); Posible dato fijo s_SunPosNrm: source/game_sa/StencilShadows.cpp:30 (0x008D5244); Posible dato fijo s_TransformedShadowTrianglePoints: source/game_sa/StencilShadows.cpp:29 (0x00C6A178) |

<a id="grupo-1410"></a>

## Globales y helpers: source/game_sa/Streaming.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeSector(float v) | [source/game_sa/Streaming.cpp:39](../../source/game_sa/Streaming.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLodSector(float v) | [source/game_sa/Streaming.cpp:41](../../source/game_sa/Streaming.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MemoryUsedBelow(size_t goal) | [source/game_sa/Streaming.cpp:43](../../source/game_sa/Streaming.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_memoryUsedBytes: source/game_sa/Streaming.h:245 (0x008E4CB4) |

<a id="grupo-1411"></a>

## Globales y helpers: source/game_sa/Streaming.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsModelDFF(int32 model) | [source/game_sa/Streaming.h:81](../../source/game_sa/Streaming.h#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelTXD(int32 model) | [source/game_sa/Streaming.h:82](../../source/game_sa/Streaming.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelCOL(int32 model) | [source/game_sa/Streaming.h:83](../../source/game_sa/Streaming.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIPL(int32 model) | [source/game_sa/Streaming.h:84](../../source/game_sa/Streaming.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelDAT(int32 model) | [source/game_sa/Streaming.h:85](../../source/game_sa/Streaming.h#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelIFP(int32 model) | [source/game_sa/Streaming.h:86](../../source/game_sa/Streaming.h#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelRRR(int32 model) | [source/game_sa/Streaming.h:87](../../source/game_sa/Streaming.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsModelSCM(int32 model) | [source/game_sa/Streaming.h:88](../../source/game_sa/Streaming.h#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelType(int32 model) | [source/game_sa/Streaming.h:90](../../source/game_sa/Streaming.h#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DFFToModelId(int32 relativeId) | [source/game_sa/Streaming.h:122](../../source/game_sa/Streaming.h#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TXDToModelId(int32 relativeId) | [source/game_sa/Streaming.h:123](../../source/game_sa/Streaming.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| COLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:124](../../source/game_sa/Streaming.h#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IPLToModelId(int32 relativeId) | [source/game_sa/Streaming.h:125](../../source/game_sa/Streaming.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DATToModelId(size_t relativeId) | [source/game_sa/Streaming.h:126](../../source/game_sa/Streaming.h#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IFPToModelId(int32 relativeId) | [source/game_sa/Streaming.h:127](../../source/game_sa/Streaming.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RRRToModelId(int32 relativeId) | [source/game_sa/Streaming.h:128](../../source/game_sa/Streaming.h#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCMToModelId(int32 relativeId) | [source/game_sa/Streaming.h:129](../../source/game_sa/Streaming.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDFF(int32 absId) | [source/game_sa/Streaming.h:132](../../source/game_sa/Streaming.h#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToTXD(int32 absId) | [source/game_sa/Streaming.h:133](../../source/game_sa/Streaming.h#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToCOL(int32 absId) | [source/game_sa/Streaming.h:134](../../source/game_sa/Streaming.h#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIPL(int32 absId) | [source/game_sa/Streaming.h:135](../../source/game_sa/Streaming.h#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToDAT(int32 absId) | [source/game_sa/Streaming.h:136](../../source/game_sa/Streaming.h#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToIFP(int32 absId) | [source/game_sa/Streaming.h:137](../../source/game_sa/Streaming.h#L137) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToRRR(int32 absId) | [source/game_sa/Streaming.h:138](../../source/game_sa/Streaming.h#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ModelIdToSCM(int32 absId) | [source/game_sa/Streaming.h:139](../../source/game_sa/Streaming.h#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1412"></a>

## Globales y helpers: source/game_sa/StuckCarCheck.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetVehicleNoBoundsCheck(int32 handle) | [source/game_sa/StuckCarCheck.cpp:25](../../source/game_sa/StuckCarCheck.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1413"></a>

## Globales y helpers: source/game_sa/StuntJumpManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RawPool() | [source/game_sa/StuntJumpManager.cpp:52](../../source/game_sa/StuntJumpManager.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo mp_poolStuntJumps: source/game_sa/StuntJumpManager.h:28 (0x00A9A888) |
| * StuntJumpPoolNew(StuntJumpPoolRaw* pool) | [source/game_sa/StuntJumpManager.cpp:57](../../source/game_sa/StuntJumpManager.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTimeStepInMS() | [source/game_sa/StuntJumpManager.cpp:79](../../source/game_sa/StuntJumpManager.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| SetJumpState(eJumpState state) | [source/game_sa/StuntJumpManager.cpp:84](../../source/game_sa/StuntJumpManager.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_jumpState: source/game_sa/StuntJumpManager.h:33 (0x00A9A898) |
| ResetAllJumps() | [source/game_sa/StuntJumpManager.cpp:351](../../source/game_sa/StuntJumpManager.cpp#L351) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StuntJumpTestCode() | [source/game_sa/StuntJumpManager.cpp:359](../../source/game_sa/StuntJumpManager.cpp#L359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1414"></a>

## Globales y helpers: source/game_sa/TagManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsTagInArea(const CRect& area, const tTagDesc& tag) | [source/game_sa/TagManager.cpp:121](../../source/game_sa/TagManager.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1415"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocator | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h:10](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1416"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorChat | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h:33](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h:41](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1417"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h:31](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h#L31) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h:39](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1418"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorFollowLimited | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h:28](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h:36](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1419"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorRandom | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h:30](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h#L30) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h:57](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1420"></a>

## Globales y helpers: source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h:31](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h#L31) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override | [source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h:39](../../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
