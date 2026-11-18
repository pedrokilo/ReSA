<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11652 con evidencia histórica y 789 pendientes de validación en 38 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1141"></a>

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

<a id="grupo-1142"></a>

## Globales y helpers: source/game_sa/Coronas.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CoronaMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Coronas.cpp:173](../../source/game_sa/Coronas.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoronaWorldPosition(const CRegisteredCorona& c, bool checkBikeLean) | [source/game_sa/Coronas.cpp:183](../../source/game_sa/Coronas.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SinTimesTenX87(float angle) | [source/game_sa/Coronas.cpp:196](../../source/game_sa/Coronas.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSunReflectionVertex(uint32 index, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/Coronas.cpp:201](../../source/game_sa/Coronas.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:289 (0x00C4D958) |

<a id="grupo-1143"></a>

## Globales y helpers: source/game_sa/Cover.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RawPoints() | [source/game_sa/Cover.cpp:74](../../source/game_sa/Cover.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_Points: source/game_sa/Cover.h:21 (0x00C197C8) |
| & Raw(CCoverPoint& cpt) | [source/game_sa/Cover.cpp:75](../../source/game_sa/Cover.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Raw(const CCoverPoint& cpt) | [source/game_sa/Cover.cpp:76](../../source/game_sa/Cover.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsCoverPoint(CoverPointRaw& raw) | [source/game_sa/Cover.cpp:77](../../source/game_sa/Cover.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & NumPoints() | [source/game_sa/Cover.cpp:78](../../source/game_sa/Cover.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_NumPoints: source/game_sa/Cover.h:20 (0x00C197A4) |
| & At(const void* base, size_t offset) | [source/game_sa/Cover.cpp:81](../../source/game_sa/Cover.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ColModelOf(const CEntity* entity) | [source/game_sa/Cover.cpp:83](../../source/game_sa/Cover.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| CoverCross(const CVector& a, const CVector& b) | [source/game_sa/Cover.cpp:88](../../source/game_sa/Cover.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::CrossProduct-out 0x0059C730 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:93](../../source/game_sa/Cover.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::Multiply3x3Exe 0x0059C790 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:102](../../source/game_sa/Cover.cpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverNormalise(CVector& v) | [source/game_sa/Cover.cpp:111](../../source/game_sa/Cover.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::Normalise 0x0059C910 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1144"></a>

## Globales y helpers: source/game_sa/CullZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPointWithinArbitraryArea(CVector2D p, CVector2D p1, CVector2D p2, CVector2D p3, CVector2D p4) | [source/game_sa/CullZones.cpp:311](../../source/game_sa/CullZones.cpp#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1145"></a>

## Globales y helpers: source/game_sa/CustomCarPlateMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatecharAtoi(const char* s) | [source/game_sa/CustomCarPlateMgr.cpp:117](../../source/game_sa/CustomCarPlateMgr.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResolvePlateType(uint8 plateType) | [source/game_sa/CustomCarPlateMgr.cpp:183](../../source/game_sa/CustomCarPlateMgr.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSet(char c) | [source/game_sa/CustomCarPlateMgr.cpp:196](../../source/game_sa/CustomCarPlateMgr.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomCarPlateMgr::GetCharacterPositionInCharSetExe 0x006FD7C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCharacterPositionInCharSetExe(uint32* outCol, uint32* outRow) | [source/game_sa/CustomCarPlateMgr.cpp:307](../../source/game_sa/CustomCarPlateMgr.cpp#L307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1146"></a>

## Globales y helpers: source/game_sa/CustomRoadsignMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RoadsignSetMaterialAlphaCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:350](../../source/game_sa/CustomRoadsignMgr.cpp#L350) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RoadsignSetMaterialTextureCB(RpMaterial* material, void* data) | [source/game_sa/CustomRoadsignMgr.cpp:356](../../source/game_sa/CustomRoadsignMgr.cpp#L356) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RoadsignGetLineAndRow(char cLetter, int32* line, int32* row) | [source/game_sa/CustomRoadsignMgr.cpp:402](../../source/game_sa/CustomRoadsignMgr.cpp#L402) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CCustomRoadsignMgr::RoadsignGetLineAndRowExe 0x006FE260 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RoadsignGetLineAndRowExe(int32* line, int32* row) | [source/game_sa/CustomRoadsignMgr.cpp:762](../../source/game_sa/CustomRoadsignMgr.cpp#L762) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1147"></a>

## Globales y helpers: source/game_sa/CutsceneMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RestorePlayerWeapon(CPed* ped, eWeaponType type, int32 ammo) | [source/game_sa/CutsceneMgr.cpp:165](../../source/game_sa/CutsceneMgr.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aInfoForModel: source/game_sa/Streaming.h:247 (0x008E4CC0) |

<a id="grupo-1148"></a>

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

<a id="grupo-1149"></a>

## Globales y helpers: source/game_sa/Darkel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetGameLogicStateNow(eGameLogicState state) | [source/game_sa/Darkel.cpp:68](../../source/game_sa/Darkel.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GameState: source/game_sa/GameLogic.h:64 (0x0096A8B0); Posible dato fijo TimeOfLastEvent: source/game_sa/GameLogic.h:63 (0x0096A8AC) |

<a id="grupo-1150"></a>

## Globales y helpers: source/game_sa/DecisionMakers/Decision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProbabilityByte(float value) | [source/game_sa/DecisionMakers/Decision.cpp:58](../../source/game_sa/DecisionMakers/Decision.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1151"></a>

## Globales y helpers: source/game_sa/DecisionMakers/DecisionMaker.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DecisionMakerConstructorExe(CDecisionMaker* self, void* /*edx*/) | [source/game_sa/DecisionMakers/DecisionMaker.cpp:46](../../source/game_sa/DecisionMakers/DecisionMaker.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1152"></a>

## Globales y helpers: source/game_sa/DecisionMakers/DecisionMakerTypes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsU32(const void* p) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:115](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L115) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DefaultRandomPedAssignExe(CDecisionMakerTypes* self, void* /*edx*/, const CDecisionMaker* source) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:121](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CDecisionMakerTypes::AssignDefaultRandomPed 0x005BD510 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstanceUnwind(EXCEPTION_RECORD* record, void* establisher, CONTEXT*, void*) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:226](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L226) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1153"></a>

## Globales y helpers: source/game_sa/Door.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoorMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Door.cpp:61](../../source/game_sa/Door.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1154"></a>

## Globales y helpers: source/game_sa/Entity/AnimatedBuilding.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AnimatedBuildingEntityDestructor(CEntity* self, void* /*edx*/) | [source/game_sa/Entity/AnimatedBuilding.cpp:29](../../source/game_sa/Entity/AnimatedBuilding.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AnimatedBuildingDestructorBody(CAnimatedBuilding* self, void* /*edx*/) | [source/game_sa/Entity/AnimatedBuilding.cpp:33](../../source/game_sa/Entity/AnimatedBuilding.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAnimatedBuilding::DestructorBody 0x00537A70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote140AAnimatedBuildingEntityDestructor() | [source/game_sa/Entity/AnimatedBuilding.cpp:60](../../source/game_sa/Entity/AnimatedBuilding.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1155"></a>

## Globales y helpers: source/game_sa/Entity/Building.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BuildingEntityDestructor(CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Building.cpp:27](../../source/game_sa/Entity/Building.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BuildingDestructorBody(CBuilding* self, void* /*edx*/) | [source/game_sa/Entity/Building.cpp:31](../../source/game_sa/Entity/Building.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBuilding::DestructorBody 0x00404180 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABuildingEntityDestructor() | [source/game_sa/Entity/Building.cpp:58](../../source/game_sa/Entity/Building.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1156"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/Dummy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote130ADummyDestructorBody() | [source/game_sa/Entity/Dummy/Dummy.cpp:61](../../source/game_sa/Entity/Dummy/Dummy.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1157"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/DummyObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DummyObjectConstructorBody(CDummyObject* self, void* /*edx*/) | [source/game_sa/Entity/Dummy/DummyObject.cpp:110](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CDummyObject::Constructor 0x0059EA00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DummyObjectAdoptObject(CDummyObject* self, CObject* obj) | [source/game_sa/Entity/Dummy/DummyObject.cpp:120](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote125ADummyObjectDestructorBody() | [source/game_sa/Entity/Dummy/DummyObject.cpp:153](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1158"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/DummyPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DummyPedDestructorBody(CDummyPed* self, void* /*edx*/) | [source/game_sa/Entity/Dummy/DummyPed.cpp:35](../../source/game_sa/Entity/Dummy/DummyPed.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CDummyPed::DestructorBody 0x005DE300 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1159"></a>

## Globales y helpers: source/game_sa/Entity/Entity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityGetStatusExe(const CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Entity.cpp:47](../../source/game_sa/Entity/Entity.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntity::GetStatus 0x004043E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGetModelIndexExe(const CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Entity.cpp:53](../../source/game_sa/Entity/Entity.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntity::GetModelIndex 0x00404420 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityIsInCurrentAreaExe(const CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Entity.cpp:60](../../source/game_sa/Entity/Entity.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_SectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:229](../../source/game_sa/Entity/Entity.cpp#L229) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_LodSectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:234](../../source/game_sa/Entity/Entity.cpp#L234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_ClampBoundRect(CRect& r) | [source/game_sa/Entity/Entity.cpp:241](../../source/game_sa/Entity/Entity.cpp#L241) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & EntityA_LodList(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:255](../../source/game_sa/Entity/Entity.cpp#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aLodPtrLists: source/game_sa/World.h:79 (0x00B99EB8) |
| & EntityA_RepeatSector(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:258](../../source/game_sa/Entity/Entity.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| EntityA_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:262](../../source/game_sa/Entity/Entity.cpp#L262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_SinTimes(float angle, float factor) | [source/game_sa/Entity/Entity.cpp:270](../../source/game_sa/Entity/Entity.cpp#L270) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_AbsLikeExe(float v) | [source/game_sa/Entity/Entity.cpp:274](../../source/game_sa/Entity/Entity.cpp#L274) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupB_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:1088](../../source/game_sa/Entity/Entity.cpp#L1088) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupC_HeadingDegreesPlus(const CMatrix* mat, float placementHeading, float baseAngle) | [source/game_sa/Entity/Entity.cpp:1171](../../source/game_sa/Entity/Entity.cpp#L1171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1160"></a>

## Globales y helpers: source/game_sa/Entity/Object/CutsceneObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CutsceneObjectDestructorBody(CCutsceneObject* self, void* /*edx*/) | [source/game_sa/Entity/Object/CutsceneObject.cpp:253](../../source/game_sa/Entity/Object/CutsceneObject.cpp#L253) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote125ACutsceneObjectDestructorBody() | [source/game_sa/Entity/Object/CutsceneObject.cpp:259](../../source/game_sa/Entity/Object/CutsceneObject.cpp#L259) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
