<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1061"></a>

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

<a id="grupo-1062"></a>

## Globales y helpers: source/game_sa/Coronas.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CoronaMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Coronas.cpp:168](../../source/game_sa/Coronas.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoronaWorldPosition(const CRegisteredCorona& c, bool checkBikeLean) | [source/game_sa/Coronas.cpp:178](../../source/game_sa/Coronas.cpp#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SinTimesTenX87(float angle) | [source/game_sa/Coronas.cpp:191](../../source/game_sa/Coronas.cpp#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSunReflectionVertex(uint32 index, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/Coronas.cpp:196](../../source/game_sa/Coronas.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958) |
| SunReflectionDistanceFactor(CVector point, float radius) | [source/game_sa/Coronas.cpp:207](../../source/game_sa/Coronas.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |

<a id="grupo-1063"></a>

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
| CoverCross(const CVector& a, const CVector& b) | [source/game_sa/Cover.cpp:76](../../source/game_sa/Cover.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:81](../../source/game_sa/Cover.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:90](../../source/game_sa/Cover.cpp#L90) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverNormalise(CVector& v) | [source/game_sa/Cover.cpp:99](../../source/game_sa/Cover.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1064"></a>

## Globales y helpers: source/game_sa/CullZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPointWithinArbitraryArea(CVector2D p, CVector2D p1, CVector2D p2, CVector2D p3, CVector2D p4) | [source/game_sa/CullZones.cpp:249](../../source/game_sa/CullZones.cpp#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1065"></a>

## Globales y helpers: source/game_sa/CustomCarPlateMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolvePlateType(uint8 plateType) | [source/game_sa/CustomCarPlateMgr.cpp:117](../../source/game_sa/CustomCarPlateMgr.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSet(char c) | [source/game_sa/CustomCarPlateMgr.cpp:130](../../source/game_sa/CustomCarPlateMgr.cpp#L130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1066"></a>

## Globales y helpers: source/game_sa/CustomRoadsignMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RoadsignSetMaterialAlphaCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:348](../../source/game_sa/CustomRoadsignMgr.cpp#L348) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RoadsignSetMaterialTextureCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:354](../../source/game_sa/CustomRoadsignMgr.cpp#L354) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RoadsignGetLineAndRow(char cLetter, int32* line, int32* row) | [source/game_sa/CustomRoadsignMgr.cpp:400](../../source/game_sa/CustomRoadsignMgr.cpp#L400) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1067"></a>

## Globales y helpers: source/game_sa/CutsceneMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RestorePlayerWeapon(CPed* ped, eWeaponType type, int32 ammo) | [source/game_sa/CutsceneMgr.cpp:165](../../source/game_sa/CutsceneMgr.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aInfoForModel: source/game_sa/Streaming.h:247 (0x008E4CC0) |
| FindCutsceneAudioTrackId(const char* cutsceneName) | [source/game_sa/CutsceneMgr.cpp:1189](../../source/game_sa/CutsceneMgr.cpp#L1189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1068"></a>

## Globales y helpers: source/game_sa/DamageManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PanelShift(uint32 panel) | [source/game_sa/DamageManager.cpp:28](../../source/game_sa/DamageManager.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LightShift(uint32 light) | [source/game_sa/DamageManager.cpp:29](../../source/game_sa/DamageManager.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AeroplaneShift(uint32 frame) | [source/game_sa/DamageManager.cpp:30](../../source/game_sa/DamageManager.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPanelStatusRaw(const CDamageManager* self, uint32 panel) | [source/game_sa/DamageManager.cpp:32](../../source/game_sa/DamageManager.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetPanelStatusRaw(CDamageManager* self, uint32 panel, uint32 status) | [source/game_sa/DamageManager.cpp:36](../../source/game_sa/DamageManager.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLightStatusRaw(const CDamageManager* self, uint32 light) | [source/game_sa/DamageManager.cpp:41](../../source/game_sa/DamageManager.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetLightStatusRaw(CDamageManager* self, uint32 light, uint32 status) | [source/game_sa/DamageManager.cpp:45](../../source/game_sa/DamageManager.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetComponentGroupRaw(int32 comp, int32& outGroup, uint8& outIdx) | [source/game_sa/DamageManager.cpp:52](../../source/game_sa/DamageManager.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyDamageRaw(CDamageManager* self, CAutomobile* vehicle, int32 compId, float fIntensity) | [source/game_sa/DamageManager.cpp:78](../../source/game_sa/DamageManager.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo s_afDamageMultByGroup: source/game_sa/DamageManager.cpp:25 (0x008D32A0) |
| * __fastcall Init_Hook(CDamageManager* self) | [source/game_sa/DamageManager.cpp:124](../../source/game_sa/DamageManager.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ResetDamageStatusAndWheelDamage_Hook(CDamageManager* self) | [source/game_sa/DamageManager.cpp:129](../../source/game_sa/DamageManager.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyDamage_Hook(CDamageManager* self, void*, CAutomobile* vehicle, int32 compId, float fIntensity, float fColDmgMult) | [source/game_sa/DamageManager.cpp:134](../../source/game_sa/DamageManager.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetComponentGroup_Hook(int32 comp, int32* outGroup, uint8* outIdx) | [source/game_sa/DamageManager.cpp:139](../../source/game_sa/DamageManager.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDoorStatus_Zx(const CDamageManager* self, void*, int32 door) | [source/game_sa/DamageManager.cpp:146](../../source/game_sa/DamageManager.cpp#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDoorStatus_Component_Zx(const CDamageManager* self, void*, uint32 comp) | [source/game_sa/DamageManager.cpp:151](../../source/game_sa/DamageManager.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetDoorStatus_Hook(CDamageManager* self, void*, int32 door, uint32 status) | [source/game_sa/DamageManager.cpp:157](../../source/game_sa/DamageManager.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetDoorStatus_Component_Hook(CDamageManager* self, void*, uint32 comp, uint32 status) | [source/game_sa/DamageManager.cpp:163](../../source/game_sa/DamageManager.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPanelStatus_Hook(const CDamageManager* self, void*, uint32 panel) | [source/game_sa/DamageManager.cpp:169](../../source/game_sa/DamageManager.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetPanelStatus_Hook(CDamageManager* self, void*, uint32 panel, uint32 status) | [source/game_sa/DamageManager.cpp:173](../../source/game_sa/DamageManager.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLightStatus_Hook(const CDamageManager* self, void*, uint32 light) | [source/game_sa/DamageManager.cpp:177](../../source/game_sa/DamageManager.cpp#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetLightStatus_Hook(CDamageManager* self, void*, uint32 light, uint32 status) | [source/game_sa/DamageManager.cpp:181](../../source/game_sa/DamageManager.cpp#L181) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarNodeIndexFromPanel_Hook(uint32 panel) | [source/game_sa/DamageManager.cpp:186](../../source/game_sa/DamageManager.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarNodeIndexFromDoor_Hook(uint32 door) | [source/game_sa/DamageManager.cpp:190](../../source/game_sa/DamageManager.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1069"></a>

## Globales y helpers: source/game_sa/Darkel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetGameLogicStateNow(eGameLogicState state) | [source/game_sa/Darkel.cpp:68](../../source/game_sa/Darkel.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GameState: source/game_sa/GameLogic.h:64 (0x0096A8B0); Posible dato fijo TimeOfLastEvent: source/game_sa/GameLogic.h:63 (0x0096A8AC) |

<a id="grupo-1070"></a>

## Globales y helpers: source/game_sa/DecisionMakers/Decision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProbabilityByte(float value) | [source/game_sa/DecisionMakers/Decision.cpp:54](../../source/game_sa/DecisionMakers/Decision.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1071"></a>

## Globales y helpers: source/game_sa/DecisionMakers/DecisionMakerTypes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsU32(const void* p) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:82](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstanceUnwind(EXCEPTION_RECORD* record, void* establisher, CONTEXT*, void*) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:131](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1072"></a>

## Globales y helpers: source/game_sa/Door.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoorMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Door.cpp:61](../../source/game_sa/Door.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1073"></a>

## Globales y helpers: source/game_sa/Draw.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoFade() | [source/game_sa/Draw.cpp:53](../../source/game_sa/Draw.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FadeBlue: source/game_sa/Draw.h:22 (0x00C3EFAA); Posible dato fijo FadeGreen: source/game_sa/Draw.h:21 (0x00C3EFA9); Posible dato fijo FadeOutTime: source/game_sa/Draw.cpp:24 (0x008D2BD4); Posible dato fijo FadeRed: source/game_sa/Draw.h:20 (0x00C3EFA8); Posible dato fijo FadeTimer: source/game_sa/Draw.cpp:23 (0x00C16EE0); Posible dato fijo FadeValue: source/game_sa/Draw.h:23 (0x00C3EFAB); Posible dato fijo JustLoadedDontFadeInYet: source/game_sa/Draw.h:9 (0x00C16EDC); Posible dato fijo StillToFadeOut: source/game_sa/Draw.h:10 (0x00C16EDD); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |

<a id="grupo-1074"></a>

## Globales y helpers: source/game_sa/Entity/Entity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityA_LodSectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:181](../../source/game_sa/Entity/Entity.cpp#L181) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_ClampBoundRect(CRect& r) | [source/game_sa/Entity/Entity.cpp:188](../../source/game_sa/Entity/Entity.cpp#L188) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & EntityA_LodList(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:202](../../source/game_sa/Entity/Entity.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aLodPtrLists: source/game_sa/World.h:79 (0x00B99EB8) |
| & EntityA_RepeatSector(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:205](../../source/game_sa/Entity/Entity.cpp#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| EntityA_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:209](../../source/game_sa/Entity/Entity.cpp#L209) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_SinTimes(float angle, float factor) | [source/game_sa/Entity/Entity.cpp:217](../../source/game_sa/Entity/Entity.cpp#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_AbsLikeExe(float v) | [source/game_sa/Entity/Entity.cpp:221](../../source/game_sa/Entity/Entity.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupB_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:1035](../../source/game_sa/Entity/Entity.cpp#L1035) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupC_HeadingDegreesPlus(const CMatrix* mat, float placementHeading, float baseAngle) | [source/game_sa/Entity/Entity.cpp:1118](../../source/game_sa/Entity/Entity.cpp#L1118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1075"></a>

## Globales y helpers: source/game_sa/Entity/Object/Object.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjF(uint32 bits) | [source/game_sa/Entity/Object/Object.cpp:43](../../source/game_sa/Entity/Object/Object.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:56](../../source/game_sa/Entity/Object/Object.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjMultiplyPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:65](../../source/game_sa/Entity/Object/Object.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjSqMag(const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:74](../../source/game_sa/Entity/Object/Object.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjFtol(float f) | [source/game_sa/Entity/Object/Object.cpp:79](../../source/game_sa/Entity/Object/Object.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjCeilSteps(float f) | [source/game_sa/Entity/Object/Object.cpp:84](../../source/game_sa/Entity/Object/Object.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjIsInObjectPool(const CObject* object) | [source/game_sa/Entity/Object/Object.cpp:1524](../../source/game_sa/Entity/Object/Object.cpp#L1524) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1076"></a>

## Globales y helpers: source/game_sa/Entity/Ped/CopPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolveModelForCopType(uint32_t typeOrModelID) | [source/game_sa/Entity/Ped/CopPed.cpp:62](../../source/game_sa/Entity/Ped/CopPed.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1077"></a>

## Globales y helpers: source/game_sa/Entity/Ped/Ped.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedE_Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Ped/Ped.cpp:1268](../../source/game_sa/Entity/Ped/Ped.cpp#L1268) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedE_TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Ped/Ped.cpp:1277](../../source/game_sa/Entity/Ped/Ped.cpp#L1277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedE_HeadingAdd(const CEntity* e, float add) | [source/game_sa/Entity/Ped/Ped.cpp:1286](../../source/game_sa/Entity/Ped/Ped.cpp#L1286) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PedE_BoneMatrix(CPed* ped, int32 boneTag) | [source/game_sa/Entity/Ped/Ped.cpp:1294](../../source/game_sa/Entity/Ped/Ped.cpp#L1294) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootParticlesInRange(CPed* ped, const CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2618](../../source/game_sa/Entity/Ped/Ped.cpp#L2618) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEntity::GetIsOnScreen 0x00534540 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |
| FootParticleJitter(float base) | [source/game_sa/Entity/Ped/Ped.cpp:2629](../../source/game_sa/Entity/Ped/Ped.cpp#L2629) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitFootRainParticles(CPed* ped, CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2636](../../source/game_sa/Entity/Ped/Ped.cpp#L2636) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |
| EmitFootDustParticles(CPed* ped, CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2655](../../source/game_sa/Entity/Ped/Ped.cpp#L2655) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00); Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| IsPedPointerValid(CPed* ped) | [source/game_sa/Entity/Ped/Ped.cpp:3807](../../source/game_sa/Entity/Ped/Ped.cpp#L3807) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedPointerValid_NotInWorld(CPed* ped) | [source/game_sa/Entity/Ped/Ped.cpp:3819](../../source/game_sa/Entity/Ped/Ped.cpp#L3819) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SayJacked(CPed* jacked, CVehicle* vehicle, uint32 timeDelay) | [source/game_sa/Entity/Ped/Ped.cpp:4781](../../source/game_sa/Entity/Ped/Ped.cpp#L4781) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SayJacking(CPed* jacker, CPed* jacked, CVehicle* vehicle, uint32 timeDelay) | [source/game_sa/Entity/Ped/Ped.cpp:4794](../../source/game_sa/Entity/Ped/Ped.cpp#L4794) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1078"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PedStdBonePositions.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetPedBoneStdPosition(uint32 bone) | [source/game_sa/Entity/Ped/PedStdBonePositions.h:11](../../source/game_sa/Entity/Ped/PedStdBonePositions.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1079"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PlayerPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetButtonSprintResultsKeepEdx(CPlayerPed* self, uint32 entryEdx, eSprintType sprintType) | [source/game_sa/Entity/Ped/PlayerPed.cpp:52](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerPedATanMinus(float x, float y, float compensation) | [source/game_sa/Entity/Ped/PlayerPed.cpp:1056](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L1056) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1080"></a>

## Globales y helpers: source/game_sa/Entity/Physical.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PhysicalSector(float coordinate) | [source/game_sa/Entity/Physical.cpp:173](../../source/game_sa/Entity/Physical.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PhysicalSectorList(CRepeatSector& sector, eEntityType type) | [source/game_sa/Entity/Physical.cpp:176](../../source/game_sa/Entity/Physical.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:1044](../../source/game_sa/Entity/Physical.cpp#L1044) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColSqMag(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1052](../../source/game_sa/Entity/Physical.cpp#L1052) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColDivide(const CVector& v, float divisor) | [source/game_sa/Entity/Physical.cpp:1056](../../source/game_sa/Entity/Physical.cpp#L1056) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMagXZY(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1061](../../source/game_sa/Entity/Physical.cpp#L1061) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMagZYX(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1065](../../source/game_sa/Entity/Physical.cpp#L1065) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PhysAttachMatPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:2259](../../source/game_sa/Entity/Physical.cpp#L2259) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PhysAttachMat3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:2268](../../source/game_sa/Entity/Physical.cpp#L2268) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
