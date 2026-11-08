<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 07/11/2026 (Ciudad de México):** 865 grupos / 9345 funciones reconstruidas; 9345 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1081"></a>

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

<a id="grupo-1082"></a>

## Globales y helpers: source/game_sa/Coronas.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CoronaMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Coronas.cpp:168](../../source/game_sa/Coronas.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoronaWorldPosition(const CRegisteredCorona& c, bool checkBikeLean) | [source/game_sa/Coronas.cpp:178](../../source/game_sa/Coronas.cpp#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SinTimesTenX87(float angle) | [source/game_sa/Coronas.cpp:191](../../source/game_sa/Coronas.cpp#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSunReflectionVertex(uint32 index, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/Coronas.cpp:196](../../source/game_sa/Coronas.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958) |
| SunReflectionDistanceFactor(CVector point, float radius) | [source/game_sa/Coronas.cpp:207](../../source/game_sa/Coronas.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1083"></a>

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

<a id="grupo-1084"></a>

## Globales y helpers: source/game_sa/CullZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPointWithinArbitraryArea(CVector2D p, CVector2D p1, CVector2D p2, CVector2D p3, CVector2D p4) | [source/game_sa/CullZones.cpp:275](../../source/game_sa/CullZones.cpp#L275) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1085"></a>

## Globales y helpers: source/game_sa/CustomCarPlateMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatecharAtoi(const char* s) | [source/game_sa/CustomCarPlateMgr.cpp:114](../../source/game_sa/CustomCarPlateMgr.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResolvePlateType(uint8 plateType) | [source/game_sa/CustomCarPlateMgr.cpp:180](../../source/game_sa/CustomCarPlateMgr.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSet(char c) | [source/game_sa/CustomCarPlateMgr.cpp:193](../../source/game_sa/CustomCarPlateMgr.cpp#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1086"></a>

## Globales y helpers: source/game_sa/CustomRoadsignMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RoadsignSetMaterialAlphaCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:348](../../source/game_sa/CustomRoadsignMgr.cpp#L348) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RoadsignSetMaterialTextureCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:354](../../source/game_sa/CustomRoadsignMgr.cpp#L354) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RoadsignGetLineAndRow(char cLetter, int32* line, int32* row) | [source/game_sa/CustomRoadsignMgr.cpp:400](../../source/game_sa/CustomRoadsignMgr.cpp#L400) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1087"></a>

## Globales y helpers: source/game_sa/CutsceneMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RestorePlayerWeapon(CPed* ped, eWeaponType type, int32 ammo) | [source/game_sa/CutsceneMgr.cpp:165](../../source/game_sa/CutsceneMgr.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aInfoForModel: source/game_sa/Streaming.h:247 (0x008E4CC0) |

<a id="grupo-1088"></a>

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

<a id="grupo-1089"></a>

## Globales y helpers: source/game_sa/Darkel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetGameLogicStateNow(eGameLogicState state) | [source/game_sa/Darkel.cpp:68](../../source/game_sa/Darkel.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GameState: source/game_sa/GameLogic.h:64 (0x0096A8B0); Posible dato fijo TimeOfLastEvent: source/game_sa/GameLogic.h:63 (0x0096A8AC) |

<a id="grupo-1090"></a>

## Globales y helpers: source/game_sa/DecisionMakers/Decision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProbabilityByte(float value) | [source/game_sa/DecisionMakers/Decision.cpp:57](../../source/game_sa/DecisionMakers/Decision.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickTaskFromCumulative(const int32* tasks, const float* cumul, int32 taskToSeek, int16& taskSlot) | [source/game_sa/DecisionMakers/Decision.cpp:85](../../source/game_sa/DecisionMakers/Decision.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1091"></a>

## Globales y helpers: source/game_sa/DecisionMakers/DecisionMakerTypes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsU32(const void* p) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:98](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstanceUnwind(EXCEPTION_RECORD* record, void* establisher, CONTEXT*, void*) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:169](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1092"></a>

## Globales y helpers: source/game_sa/Door.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoorMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Door.cpp:61](../../source/game_sa/Door.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1093"></a>

## Globales y helpers: source/game_sa/Draw.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoFade() | [source/game_sa/Draw.cpp:53](../../source/game_sa/Draw.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FadeBlue: source/game_sa/Draw.h:22 (0x00C3EFAA); Posible dato fijo FadeGreen: source/game_sa/Draw.h:21 (0x00C3EFA9); Posible dato fijo FadeOutTime: source/game_sa/Draw.cpp:24 (0x008D2BD4); Posible dato fijo FadeRed: source/game_sa/Draw.h:20 (0x00C3EFA8); Posible dato fijo FadeTimer: source/game_sa/Draw.cpp:23 (0x00C16EE0); Posible dato fijo FadeValue: source/game_sa/Draw.h:23 (0x00C3EFAB); Posible dato fijo JustLoadedDontFadeInYet: source/game_sa/Draw.h:9 (0x00C16EDC); Posible dato fijo StillToFadeOut: source/game_sa/Draw.h:10 (0x00C16EDD); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1094"></a>

## Globales y helpers: source/game_sa/Entity/Entity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityA_SectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:176](../../source/game_sa/Entity/Entity.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_LodSectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:181](../../source/game_sa/Entity/Entity.cpp#L181) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_ClampBoundRect(CRect& r) | [source/game_sa/Entity/Entity.cpp:188](../../source/game_sa/Entity/Entity.cpp#L188) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & EntityA_LodList(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:202](../../source/game_sa/Entity/Entity.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aLodPtrLists: source/game_sa/World.h:79 (0x00B99EB8) |
| & EntityA_RepeatSector(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:205](../../source/game_sa/Entity/Entity.cpp#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| EntityA_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:209](../../source/game_sa/Entity/Entity.cpp#L209) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_SinTimes(float angle, float factor) | [source/game_sa/Entity/Entity.cpp:217](../../source/game_sa/Entity/Entity.cpp#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_AbsLikeExe(float v) | [source/game_sa/Entity/Entity.cpp:221](../../source/game_sa/Entity/Entity.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupB_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:1035](../../source/game_sa/Entity/Entity.cpp#L1035) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupC_HeadingDegreesPlus(const CMatrix* mat, float placementHeading, float baseAngle) | [source/game_sa/Entity/Entity.cpp:1118](../../source/game_sa/Entity/Entity.cpp#L1118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1095"></a>

## Globales y helpers: source/game_sa/Entity/Object/Object.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjF(uint32 bits) | [source/game_sa/Entity/Object/Object.cpp:43](../../source/game_sa/Entity/Object/Object.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:56](../../source/game_sa/Entity/Object/Object.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjMultiplyPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:65](../../source/game_sa/Entity/Object/Object.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjSqMag(const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:74](../../source/game_sa/Entity/Object/Object.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjFtol(float f) | [source/game_sa/Entity/Object/Object.cpp:79](../../source/game_sa/Entity/Object/Object.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjCeilSteps(float f) | [source/game_sa/Entity/Object/Object.cpp:84](../../source/game_sa/Entity/Object/Object.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjIsInObjectPool(const CObject* object) | [source/game_sa/Entity/Object/Object.cpp:1526](../../source/game_sa/Entity/Object/Object.cpp#L1526) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1096"></a>

## Globales y helpers: source/game_sa/Entity/Ped/CopPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolveModelForCopType(uint32_t typeOrModelID) | [source/game_sa/Entity/Ped/CopPed.cpp:62](../../source/game_sa/Entity/Ped/CopPed.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1097"></a>

## Globales y helpers: source/game_sa/Entity/Ped/Ped.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedE_Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Ped/Ped.cpp:1279](../../source/game_sa/Entity/Ped/Ped.cpp#L1279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedE_TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Ped/Ped.cpp:1288](../../source/game_sa/Entity/Ped/Ped.cpp#L1288) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedE_HeadingAdd(const CEntity* e, float add) | [source/game_sa/Entity/Ped/Ped.cpp:1297](../../source/game_sa/Entity/Ped/Ped.cpp#L1297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PedE_BoneMatrix(CPed* ped, int32 boneTag) | [source/game_sa/Entity/Ped/Ped.cpp:1305](../../source/game_sa/Entity/Ped/Ped.cpp#L1305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootParticlesInRange(CPed* ped, const CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2629](../../source/game_sa/Entity/Ped/Ped.cpp#L2629) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEntity::GetIsOnScreen 0x00534540 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| FootParticleJitter(float base) | [source/game_sa/Entity/Ped/Ped.cpp:2640](../../source/game_sa/Entity/Ped/Ped.cpp#L2640) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitFootRainParticles(CPed* ped, CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2647](../../source/game_sa/Entity/Ped/Ped.cpp#L2647) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |
| EmitFootDustParticles(CPed* ped, CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2666](../../source/game_sa/Entity/Ped/Ped.cpp#L2666) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00); Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| IsPedPointerValid_NotInWorld(CPed* ped) | [source/game_sa/Entity/Ped/Ped.cpp:3834](../../source/game_sa/Entity/Ped/Ped.cpp#L3834) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedCtlExeRand() | [source/game_sa/Entity/Ped/Ped.cpp:4437](../../source/game_sa/Entity/Ped/Ped.cpp#L4437) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedCtlGunflashTimer(int16& alpha, int16 progress) | [source/game_sa/Entity/Ped/Ped.cpp:4447](../../source/game_sa/Entity/Ped/Ped.cpp#L4447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PecCopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Ped/Ped.cpp:5145](../../source/game_sa/Entity/Ped/Ped.cpp#L5145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PecDamageEvent(CPed* ped, CEntity* damager, float damage, uint8 direction) | [source/game_sa/Entity/Ped/Ped.cpp:5160](../../source/game_sa/Entity/Ped/Ped.cpp#L5160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1098"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PedStdBonePositions.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetPedBoneStdPosition(uint32 bone) | [source/game_sa/Entity/Ped/PedStdBonePositions.h:11](../../source/game_sa/Entity/Ped/PedStdBonePositions.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1099"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PlayerPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetButtonSprintResultsKeepEdx(CPlayerPed* self, uint32 entryEdx, eSprintType sprintType) | [source/game_sa/Entity/Ped/PlayerPed.cpp:83](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerPedATanMinus(float x, float y, float compensation) | [source/game_sa/Entity/Ped/PlayerPed.cpp:1922](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L1922) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1100"></a>

## Globales y helpers: source/game_sa/Entity/Physical.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PhysicalSector(float coordinate) | [source/game_sa/Entity/Physical.cpp:174](../../source/game_sa/Entity/Physical.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PhysicalSectorList(CRepeatSector& sector, eEntityType type) | [source/game_sa/Entity/Physical.cpp:177](../../source/game_sa/Entity/Physical.cpp#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:1045](../../source/game_sa/Entity/Physical.cpp#L1045) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColSqMag(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1053](../../source/game_sa/Entity/Physical.cpp#L1053) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColDivide(const CVector& v, float divisor) | [source/game_sa/Entity/Physical.cpp:1057](../../source/game_sa/Entity/Physical.cpp#L1057) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMagXZY(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1062](../../source/game_sa/Entity/Physical.cpp#L1062) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMagZYX(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1066](../../source/game_sa/Entity/Physical.cpp#L1066) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PhysAttachMatPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:2260](../../source/game_sa/Entity/Physical.cpp#L2260) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PhysAttachMat3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:2269](../../source/game_sa/Entity/Physical.cpp#L2269) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
