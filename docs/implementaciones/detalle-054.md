<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1061"></a>

## Globales y helpers: source/game_sa/Coronas.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CoronaMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Coronas.cpp:167](../../source/game_sa/Coronas.cpp#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoronaWorldPosition(const CRegisteredCorona& c, bool checkBikeLean) | [source/game_sa/Coronas.cpp:177](../../source/game_sa/Coronas.cpp#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SinTimesTenX87(float angle) | [source/game_sa/Coronas.cpp:190](../../source/game_sa/Coronas.cpp#L190) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SetSunReflectionVertex(uint32 index, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/Coronas.cpp:202](../../source/game_sa/Coronas.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958) |
| SunReflectionDistanceFactor(CVector point, float radius) | [source/game_sa/Coronas.cpp:213](../../source/game_sa/Coronas.cpp#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |

<a id="grupo-1062"></a>

## Globales y helpers: source/game_sa/Cover.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * RawPoints() | [source/game_sa/Cover.cpp:61](../../source/game_sa/Cover.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_Points: source/game_sa/Cover.h:21 (0x00C197C8) |
| & Raw(CCoverPoint& cpt) | [source/game_sa/Cover.cpp:62](../../source/game_sa/Cover.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & Raw(const CCoverPoint& cpt) | [source/game_sa/Cover.cpp:63](../../source/game_sa/Cover.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsCoverPoint(CoverPointRaw& raw) | [source/game_sa/Cover.cpp:64](../../source/game_sa/Cover.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & NumPoints() | [source/game_sa/Cover.cpp:65](../../source/game_sa/Cover.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_NumPoints: source/game_sa/Cover.h:20 (0x00C197A4) |
| & At(const void* base, size_t offset) | [source/game_sa/Cover.cpp:68](../../source/game_sa/Cover.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ColModelOf(const CEntity* entity) | [source/game_sa/Cover.cpp:70](../../source/game_sa/Cover.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| CoverCross(const CVector& a, const CVector& b) | [source/game_sa/Cover.cpp:75](../../source/game_sa/Cover.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:80](../../source/game_sa/Cover.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverTransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Cover.cpp:89](../../source/game_sa/Cover.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverNormalise(CVector& v) | [source/game_sa/Cover.cpp:98](../../source/game_sa/Cover.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CoverDirDot(float deltaX, float deltaY, float deltaZ, int32 dirIndex) | [source/game_sa/Cover.cpp:112](../../source/game_sa/Cover.cpp#L112) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1063"></a>

## Globales y helpers: source/game_sa/CullZones.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPointWithinArbitraryArea(CVector2D p, CVector2D p1, CVector2D p2, CVector2D p3, CVector2D p4) | [source/game_sa/CullZones.cpp:249](../../source/game_sa/CullZones.cpp#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1064"></a>

## Globales y helpers: source/game_sa/Curves.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CalcCorrectedDistX87(float current, float total, float speedVariation, float* outT) | [source/game_sa/Curves.cpp:140](../../source/game_sa/Curves.cpp#L140) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

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
| RoadsignGenerateTextRaster(char* roadName, int32 numLetters, RwRaster* charsetRaster, int32 unused, RwRaster* signRaster) | [source/game_sa/CustomRoadsignMgr.cpp:360](../../source/game_sa/CustomRoadsignMgr.cpp#L360) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pCharsetLockedRaster: source/game_sa/CustomRoadsignMgr.h:12 (0x00C3EF88) |
| RoadsignGetLineAndRow(char cLetter, int32* line, int32* row) | [source/game_sa/CustomRoadsignMgr.cpp:400](../../source/game_sa/CustomRoadsignMgr.cpp#L400) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1067"></a>

## Globales y helpers: source/game_sa/CutsceneMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RestorePlayerWeapon(CPed* ped, eWeaponType type, int32 ammo) | [source/game_sa/CutsceneMgr.cpp:164](../../source/game_sa/CutsceneMgr.cpp#L164) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aInfoForModel: source/game_sa/Streaming.h:247 (0x008E4CC0) |
| FindCutsceneAudioTrackId(const char* cutsceneName) | [source/game_sa/CutsceneMgr.cpp:1202](../../source/game_sa/CutsceneMgr.cpp#L1202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

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
| ProbabilityByte(float value) | [source/game_sa/DecisionMakers/Decision.cpp:54](../../source/game_sa/DecisionMakers/Decision.cpp#L54) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1071"></a>

## Globales y helpers: source/game_sa/DecisionMakers/DecisionMakerTypes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Decision6SetPed() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:17](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L17) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00600B50; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Decision6SetGroup() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:23](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L23) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005F7340; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Decision6Assign() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:29](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L29) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x006006B0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Decision6ReadEvents() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:35](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L35) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005BB9F0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Task11AllocateDecisionTypes() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:112](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L112) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x0082119A; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Task11ConstructDecisionTypes() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:118](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L118) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x004650F0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Decision7One() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:166](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L166) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x006040D0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Decision7Many() | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:172](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L172) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00606060; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1072"></a>

## Globales y helpers: source/game_sa/Door.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoorSinX87(float a) | [source/game_sa/Door.cpp:60](../../source/game_sa/Door.cpp#L60) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| DoorCosX87(float a) | [source/game_sa/Door.cpp:70](../../source/game_sa/Door.cpp#L70) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| DoorMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Door.cpp:81](../../source/game_sa/Door.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoorMultiplyByPow(float& angVel, float rate, float step) | [source/game_sa/Door.cpp:90](../../source/game_sa/Door.cpp#L90) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1073"></a>

## Globales y helpers: source/game_sa/Draw.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoFade() | [source/game_sa/Draw.cpp:53](../../source/game_sa/Draw.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FadeBlue: source/game_sa/Draw.h:22 (0x00C3EFAA); Posible dato fijo FadeGreen: source/game_sa/Draw.h:21 (0x00C3EFA9); Posible dato fijo FadeOutTime: source/game_sa/Draw.cpp:24 (0x008D2BD4); Posible dato fijo FadeRed: source/game_sa/Draw.h:20 (0x00C3EFA8); Posible dato fijo FadeTimer: source/game_sa/Draw.cpp:23 (0x00C16EE0); Posible dato fijo FadeValue: source/game_sa/Draw.h:23 (0x00C3EFAB); Posible dato fijo JustLoadedDontFadeInYet: source/game_sa/Draw.h:9 (0x00C16EDC); Posible dato fijo StillToFadeOut: source/game_sa/Draw.h:10 (0x00C16EDD); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |

<a id="grupo-1074"></a>

## Globales y helpers: source/game_sa/Entity/AnimatedBuilding.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimatedBuilding | [source/game_sa/Entity/AnimatedBuilding.h:14](../../source/game_sa/Entity/AnimatedBuilding.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1075"></a>

## Globales y helpers: source/game_sa/Entity/Building.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsBuildingPointerValid(CBuilding* building) | [source/game_sa/Entity/Building.cpp:67](../../source/game_sa/Entity/Building.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1076"></a>

## Globales y helpers: source/game_sa/Entity/Building.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CBuilding | [source/game_sa/Entity/Building.h:17](../../source/game_sa/Entity/Building.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1077"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/Dummy.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEntity | [source/game_sa/Entity/Dummy/Dummy.h:17](../../source/game_sa/Entity/Dummy/Dummy.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1078"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/DummyObject.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CDummyObject | [source/game_sa/Entity/Dummy/DummyObject.h:17](../../source/game_sa/Entity/Dummy/DummyObject.h#L17) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1079"></a>

## Globales y helpers: source/game_sa/Entity/Entity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityA_LodSectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:180](../../source/game_sa/Entity/Entity.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_ClampBoundRect(CRect& r) | [source/game_sa/Entity/Entity.cpp:187](../../source/game_sa/Entity/Entity.cpp#L187) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & EntityA_LodList(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:201](../../source/game_sa/Entity/Entity.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aLodPtrLists: source/game_sa/World.h:79 (0x00B99EB8) |
| & EntityA_RepeatSector(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:204](../../source/game_sa/Entity/Entity.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| EntityA_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:208](../../source/game_sa/Entity/Entity.cpp#L208) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_SinTimes(float angle, float factor) | [source/game_sa/Entity/Entity.cpp:216](../../source/game_sa/Entity/Entity.cpp#L216) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| EntityA_AbsLikeExe(float v) | [source/game_sa/Entity/Entity.cpp:227](../../source/game_sa/Entity/Entity.cpp#L227) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupB_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:1041](../../source/game_sa/Entity/Entity.cpp#L1041) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupC_HeadingDegreesPlus(const CMatrix* mat, float placementHeading, float baseAngle) | [source/game_sa/Entity/Entity.cpp:1124](../../source/game_sa/Entity/Entity.cpp#L1124) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 2 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1080"></a>

## Globales y helpers: source/game_sa/Entity/Entity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPlaceable | [source/game_sa/Entity/Entity.h:50](../../source/game_sa/Entity/Entity.h#L50) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTypeBuilding() | [source/game_sa/Entity/Entity.h:145](../../source/game_sa/Entity/Entity.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTypeVehicle() | [source/game_sa/Entity/Entity.h:146](../../source/game_sa/Entity/Entity.h#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTypePed() | [source/game_sa/Entity/Entity.h:147](../../source/game_sa/Entity/Entity.h#L147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTypeObject() | [source/game_sa/Entity/Entity.h:148](../../source/game_sa/Entity/Entity.h#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetTypeDummy() | [source/game_sa/Entity/Entity.h:149](../../source/game_sa/Entity/Entity.h#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsTypeBuilding() const | [source/game_sa/Entity/Entity.h:151](../../source/game_sa/Entity/Entity.h#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsTypeVehicle() const | [source/game_sa/Entity/Entity.h:152](../../source/game_sa/Entity/Entity.h#L152) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsTypePed() const | [source/game_sa/Entity/Entity.h:153](../../source/game_sa/Entity/Entity.h#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsTypeObject() const | [source/game_sa/Entity/Entity.h:154](../../source/game_sa/Entity/Entity.h#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsTypeDummy() const | [source/game_sa/Entity/Entity.h:155](../../source/game_sa/Entity/Entity.h#L155) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsTypePhysical() const | [source/game_sa/Entity/Entity.h:156](../../source/game_sa/Entity/Entity.h#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetType(eEntityType type) | [source/game_sa/Entity/Entity.h:158](../../source/game_sa/Entity/Entity.h#L158) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEntity::GetIsTypePhysical 0x004DA030 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const noexcept | [source/game_sa/Entity/Entity.h:159](../../source/game_sa/Entity/Entity.h#L159) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetStatus(eEntityStatus status) | [source/game_sa/Entity/Entity.h:161](../../source/game_sa/Entity/Entity.h#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetStatus() const noexcept | [source/game_sa/Entity/Entity.h:162](../../source/game_sa/Entity/Entity.h#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetUsesCollision(bool usesCollision) | [source/game_sa/Entity/Entity.h:169](../../source/game_sa/Entity/Entity.h#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetUsesCollision() const | [source/game_sa/Entity/Entity.h:170](../../source/game_sa/Entity/Entity.h#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetCollisionProcessed(bool collisionProcessed) | [source/game_sa/Entity/Entity.h:171](../../source/game_sa/Entity/Entity.h#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCollisionProcessed() const | [source/game_sa/Entity/Entity.h:172](../../source/game_sa/Entity/Entity.h#L172) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsStatic(bool isStatic) | [source/game_sa/Entity/Entity.h:173](../../source/game_sa/Entity/Entity.h#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsStatic() const | [source/game_sa/Entity/Entity.h:174](../../source/game_sa/Entity/Entity.h#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetHasContacted(bool hasContacted) | [source/game_sa/Entity/Entity.h:175](../../source/game_sa/Entity/Entity.h#L175) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEntity::GetIsStatic 0x004633E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHasContacted() const | [source/game_sa/Entity/Entity.h:176](../../source/game_sa/Entity/Entity.h#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsStuck(bool isStuck) | [source/game_sa/Entity/Entity.h:177](../../source/game_sa/Entity/Entity.h#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsStuck() const | [source/game_sa/Entity/Entity.h:178](../../source/game_sa/Entity/Entity.h#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsInSafePosition(bool isInSafePosition) | [source/game_sa/Entity/Entity.h:179](../../source/game_sa/Entity/Entity.h#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsInSafePosition() const | [source/game_sa/Entity/Entity.h:180](../../source/game_sa/Entity/Entity.h#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetWasPostponed(bool wasPostponed) | [source/game_sa/Entity/Entity.h:181](../../source/game_sa/Entity/Entity.h#L181) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWasPostponed() const | [source/game_sa/Entity/Entity.h:182](../../source/game_sa/Entity/Entity.h#L182) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsVisible(bool isVisible) | [source/game_sa/Entity/Entity.h:183](../../source/game_sa/Entity/Entity.h#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsVisible() const | [source/game_sa/Entity/Entity.h:184](../../source/game_sa/Entity/Entity.h#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetHasHitWall(bool hasHitWall) | [source/game_sa/Entity/Entity.h:185](../../source/game_sa/Entity/Entity.h#L185) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHasHitWall() const | [source/game_sa/Entity/Entity.h:186](../../source/game_sa/Entity/Entity.h#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsBackfaceCulled(bool backfaceCulled) | [source/game_sa/Entity/Entity.h:187](../../source/game_sa/Entity/Entity.h#L187) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsBackfaceCulled() const | [source/game_sa/Entity/Entity.h:188](../../source/game_sa/Entity/Entity.h#L188) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsUnimportantStream(bool unimportantStream) | [source/game_sa/Entity/Entity.h:189](../../source/game_sa/Entity/Entity.h#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetScanCode(uint16 scanCode) | [source/game_sa/Entity/Entity.h:191](../../source/game_sa/Entity/Entity.h#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetScanCode() const | [source/game_sa/Entity/Entity.h:192](../../source/game_sa/Entity/Entity.h#L192) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetAreaCode(eAreaCodes areaCode) | [source/game_sa/Entity/Entity.h:193](../../source/game_sa/Entity/Entity.h#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAreaCode() const | [source/game_sa/Entity/Entity.h:194](../../source/game_sa/Entity/Entity.h#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIplIndex(uint8 iplIndex) | [source/game_sa/Entity/Entity.h:195](../../source/game_sa/Entity/Entity.h#L195) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIplIndex() | [source/game_sa/Entity/Entity.h:196](../../source/game_sa/Entity/Entity.h#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelIndex() const | [source/game_sa/Entity/Entity.h:200](../../source/game_sa/Entity/Entity.h#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelId() const | [source/game_sa/Entity/Entity.h:201](../../source/game_sa/Entity/Entity.h#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsVisibleComplex() | [source/game_sa/Entity/Entity.h:230](../../source/game_sa/Entity/Entity.h#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessControl() | [source/game_sa/Entity/Entity.h:232](../../source/game_sa/Entity/Entity.h#L232) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessCollision() | [source/game_sa/Entity/Entity.h:233](../../source/game_sa/Entity/Entity.h#L233) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessShift() | [source/game_sa/Entity/Entity.h:234](../../source/game_sa/Entity/Entity.h#L234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TestCollision(bool applySpeed) | [source/game_sa/Entity/Entity.h:235](../../source/game_sa/Entity/Entity.h#L235) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Teleport(CVector newCoors, bool clearOrientation) | [source/game_sa/Entity/Entity.h:236](../../source/game_sa/Entity/Entity.h#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SpecialEntityPreCollisionStuff(CPhysical* colPhysical, bool doingShift, bool& skipTestEntirely, bool& skipCol, bool& forceBuildingCol, bool& forceSoftCol) | [source/game_sa/Entity/Entity.h:237](../../source/game_sa/Entity/Entity.h#L237) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SpecialEntityCalcCollisionSteps(bool& doPreCheckAtFullSpeed, bool& doPreCheckAtHalfSpeed) | [source/game_sa/Entity/Entity.h:238](../../source/game_sa/Entity/Entity.h#L238) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlagToDestroyWhenNextProcessed() | [source/game_sa/Entity/Entity.h:274](../../source/game_sa/Entity/Entity.h#L274) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetLodIndex(uint32 lodIndex) | [source/game_sa/Entity/Entity.h:282](../../source/game_sa/Entity/Entity.h#L282) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLodIndex() | [source/game_sa/Entity/Entity.h:283](../../source/game_sa/Entity/Entity.h#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetLod(CEntity* lod) | [source/game_sa/Entity/Entity.h:284](../../source/game_sa/Entity/Entity.h#L284) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetLod() | [source/game_sa/Entity/Entity.h:285](../../source/game_sa/Entity/Entity.h#L285) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddLodChildren() | [source/game_sa/Entity/Entity.h:287](../../source/game_sa/Entity/Entity.h#L287) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveLodChildren() | [source/game_sa/Entity/Entity.h:288](../../source/game_sa/Entity/Entity.h#L288) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNumLodChildren() | [source/game_sa/Entity/Entity.h:289](../../source/game_sa/Entity/Entity.h#L289) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddLodChildrenRendered() | [source/game_sa/Entity/Entity.h:291](../../source/game_sa/Entity/Entity.h#L291) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResetLodChildrenRendered() | [source/game_sa/Entity/Entity.h:292](../../source/game_sa/Entity/Entity.h#L292) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasLodChildrenRendered() | [source/game_sa/Entity/Entity.h:293](../../source/game_sa/Entity/Entity.h#L293) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNumLodChildrenRendered() | [source/game_sa/Entity/Entity.h:294](../../source/game_sa/Entity/Entity.h#L294) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetCannotLodChildrenRender() | [source/game_sa/Entity/Entity.h:295](../../source/game_sa/Entity/Entity.h#L295) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanLodChildrenRender() | [source/game_sa/Entity/Entity.h:296](../../source/game_sa/Entity/Entity.h#L296) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetRwObject() const noexcept | [source/game_sa/Entity/Entity.h:318](../../source/game_sa/Entity/Entity.h#L318) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClearRwObjectPointer() noexcept | [source/game_sa/Entity/Entity.h:319](../../source/game_sa/Entity/Entity.h#L319) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetRpClump() const noexcept | [source/game_sa/Entity/Entity.h:320](../../source/game_sa/Entity/Entity.h#L320) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetRpAtomic() const noexcept | [source/game_sa/Entity/Entity.h:321](../../source/game_sa/Entity/Entity.h#L321) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetColData() | [source/game_sa/Entity/Entity.h:329](../../source/game_sa/Entity/Entity.h#L329) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClearReference(T*& ref) requires std::is_base_of_v&lt;CEntity, T&gt; | [source/game_sa/Entity/Entity.h:336](../../source/game_sa/Entity/Entity.h#L336) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ChangeEntityReference(T*& inOutRef, Y* entity) | [source/game_sa/Entity/Entity.h:349](../../source/game_sa/Entity/Entity.h#L349) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetEntityReference(T*& inOutRef, Y* entity) | [source/game_sa/Entity/Entity.h:360](../../source/game_sa/Entity/Entity.h#L360) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RegisterReference(T*& ref) requires std::is_base_of_v&lt;CEntity, T&gt; | [source/game_sa/Entity/Entity.h:367](../../source/game_sa/Entity/Entity.h#L367) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CleanUpOldReference(T*& ref) requires std::is_base_of_v&lt;CEntity, T&gt; | [source/game_sa/Entity/Entity.h:372](../../source/game_sa/Entity/Entity.h#L372) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SafeRegisterRef(T*& e) requires std::is_base_of_v&lt;CEntity, T&gt; | [source/game_sa/Entity/Entity.h:377](../../source/game_sa/Entity/Entity.h#L377) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SafeCleanUpRef(T*& e) requires std::is_base_of_v&lt;CEntity, T&gt; | [source/game_sa/Entity/Entity.h:384](../../source/game_sa/Entity/Entity.h#L384) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRCCar() const | [source/game_sa/Entity/Entity.h:394](../../source/game_sa/Entity/Entity.h#L394) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsPhysical() | [source/game_sa/Entity/Entity.h:396](../../source/game_sa/Entity/Entity.h#L396) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsVehicle() | [source/game_sa/Entity/Entity.h:397](../../source/game_sa/Entity/Entity.h#L397) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsVehicle() const | [source/game_sa/Entity/Entity.h:398](../../source/game_sa/Entity/Entity.h#L398) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsAutomobile() | [source/game_sa/Entity/Entity.h:399](../../source/game_sa/Entity/Entity.h#L399) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsAutomobile() const | [source/game_sa/Entity/Entity.h:400](../../source/game_sa/Entity/Entity.h#L400) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsBike() | [source/game_sa/Entity/Entity.h:401](../../source/game_sa/Entity/Entity.h#L401) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsBike() const | [source/game_sa/Entity/Entity.h:402](../../source/game_sa/Entity/Entity.h#L402) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsBmx() | [source/game_sa/Entity/Entity.h:403](../../source/game_sa/Entity/Entity.h#L403) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsBmx() const | [source/game_sa/Entity/Entity.h:404](../../source/game_sa/Entity/Entity.h#L404) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsBoat() | [source/game_sa/Entity/Entity.h:405](../../source/game_sa/Entity/Entity.h#L405) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsBoat() const | [source/game_sa/Entity/Entity.h:406](../../source/game_sa/Entity/Entity.h#L406) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsTrain() | [source/game_sa/Entity/Entity.h:407](../../source/game_sa/Entity/Entity.h#L407) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsTrain() const | [source/game_sa/Entity/Entity.h:408](../../source/game_sa/Entity/Entity.h#L408) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsPed() | [source/game_sa/Entity/Entity.h:409](../../source/game_sa/Entity/Entity.h#L409) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsPed() const | [source/game_sa/Entity/Entity.h:410](../../source/game_sa/Entity/Entity.h#L410) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsObject() | [source/game_sa/Entity/Entity.h:411](../../source/game_sa/Entity/Entity.h#L411) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsBuilding() | [source/game_sa/Entity/Entity.h:412](../../source/game_sa/Entity/Entity.h#L412) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AsDummy() | [source/game_sa/Entity/Entity.h:413](../../source/game_sa/Entity/Entity.h#L413) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Entity/Entity.h:423](../../source/game_sa/Entity/Entity.h#L423) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Entity/Entity.h:428](../../source/game_sa/Entity/Entity.h#L428) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
