<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Pages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StretchXK(float x, uint32 kBits) | [source/game_sa/Frontend/MenuManager_Pages.cpp:19](../../source/game_sa/Frontend/MenuManager_Pages.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| StretchYK(float y, uint32 kBits) | [source/game_sa/Frontend/MenuManager_Pages.cpp:23](../../source/game_sa/Frontend/MenuManager_Pages.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindClosestPtOnLine(CVector& out, CVector& lineA, CVector& lineB, CVector& point) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A4880: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/Fx/Fx.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsFxWithinCameraRange(const CVector& pos, float range) | [source/game_sa/Fx/Fx.cpp:144](../../source/game_sa/Fx/Fx.cpp#L144) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| CreateFxWithinCameraRange(const char* name, const CVector& pos, float range) | [source/game_sa/Fx/Fx.cpp:154](../../source/game_sa/Fx/Fx.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxExeRand() | [source/game_sa/Fx/Fx.cpp:172](../../source/game_sa/Fx/Fx.cpp#L172) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxCameraDistanceSqZYX(const CVector& pos) | [source/game_sa/Fx/Fx.cpp:178](../../source/game_sa/Fx/Fx.cpp#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| FxAddWheelSandOrDust(Fx_c* fx, CVehicle* vehicle, const CVector& pos, bool spinning, float lightMult, bool dust) | [source/game_sa/Fx/Fx.cpp:188](../../source/game_sa/Fx/Fx.cpp#L188) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:35 (0x00A9AE00); Posible dato fijo m_FrameCounter: source/game_sa/Timer.h:31 (0x00B7CB4C); Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FxAddWheelDirt(CVehicle* vehicle, const CVector& pos, float lightMult, FxPrtMult_c mult) | [source/game_sa/Fx/Fx.cpp:238](../../source/game_sa/Fx/Fx.cpp#L238) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:35 (0x00A9AE00); Posible dato fijo m_FrameCounter: source/game_sa/Timer.h:31 (0x00B7CB4C) |
| FxDebrisMult(const RwRGBA& color, float scale) | [source/game_sa/Fx/Fx.cpp:275](../../source/game_sa/Fx/Fx.cpp#L275) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_Color: source/game_sa/Font.h:99 (0x00C71A60) |
| FxDebrisVelocity() | [source/game_sa/Fx/Fx.cpp:291](../../source/game_sa/Fx/Fx.cpp#L291) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RenderAddTri(CVector pos1, CVector pos2, CVector pos3, RwTexCoords coord1, RwTexCoords coord2, RwTexCoords coord3, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3) | [source/game_sa/Fx/Fx.cpp:841](../../source/game_sa/Fx/Fx.cpp#L841) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:35 (0x00A9AE00) |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitter.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitterExeRemainder() | [source/game_sa/Fx/FxEmitter.cpp:183](../../source/game_sa/Fx/FxEmitter.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitterBP.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RenderEmitterParticles355(FxEmitterBP_c& emitter, RwCamera* camera, float& brightness, bool heat) | [source/game_sa/Fx/FxEmitterBP.cpp:58](../../source/game_sa/Fx/FxEmitterBP.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B6FA14; Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:35 (0x00A9AE00); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80); Posible dato fijo m_Color: source/game_sa/Font.h:99 (0x00C71A60); Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/Fx/FxInfoManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewFxInfoInPool() | [source/game_sa/Fx/FxInfoManager.cpp:83](../../source/game_sa/Fx/FxInfoManager.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |

<a id="grupo-1127"></a>

## Globales y helpers: source/game_sa/Fx/FxManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewFxEmitterArray() | [source/game_sa/Fx/FxManager.cpp:92](../../source/game_sa/Fx/FxManager.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/Fx/FxSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CanAddParticle() | [source/game_sa/Fx/FxSystem.cpp:201](../../source/game_sa/Fx/FxSystem.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:35 (0x00A9AE00) |

<a id="grupo-1129"></a>

## Globales y helpers: source/game_sa/Fx/FxTools.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReadLine(FILESTREAM file, char* buffer, int32 bufferSize) | [source/game_sa/Fx/FxTools.cpp:13](../../source/game_sa/Fx/FxTools.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/Fx/FxTools.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReadFieldImpl(FILESTREAM file, Type& refValue, const char* fieldName = nullptr) | [source/game_sa/Fx/FxTools.h:46](../../source/game_sa/Fx/FxTools.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:60](../../source/game_sa/Fx/FxTools.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;void&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:67](../../source/game_sa/Fx/FxTools.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;int8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:74](../../source/game_sa/Fx/FxTools.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;uint8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:79](../../source/game_sa/Fx/FxTools.h#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;bool&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:84](../../source/game_sa/Fx/FxTools.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/GameLogic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingMinusHalfPi(const CPlaceable& placeable) | [source/game_sa/GameLogic.cpp:67](../../source/game_sa/GameLogic.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LeaveVehicleForResurrection() | [source/game_sa/GameLogic.cpp:647](../../source/game_sa/GameLogic.cpp#L647) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| GetRestartSearchPosition() | [source/game_sa/GameLogic.cpp:669](../../source/game_sa/GameLogic.cpp#L669) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| UseTwoPlayerRestartPoint(CVector& restartPoint, float& restartAngle) | [source/game_sa/GameLogic.cpp:679](../../source/game_sa/GameLogic.cpp#L679) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo f2PlayerStartHeading: source/game_sa/GameLogic.h:50 (0x0096A840); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0); Posible dato fijo vec2PlayerStartLocation: source/game_sa/GameLogic.h:51 (0x0096A9AC) |
| FinishResurrection() | [source/game_sa/GameLogic.cpp:688](../../source/game_sa/GameLogic.cpp#L688) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CountDownToCarsAtStart: source/game_sa/CarCtrl.h:39 (0x009690C0); Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo m_CountDownToPedsAtStart: source/game_sa/Population.h:45 (0x00C0EC23); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/GangWars.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangWaveRandomOffset(float base, float scale, float subtract) | [source/game_sa/GangWars.cpp:255](../../source/game_sa/GangWars.cpp#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWaveInteger(float value, float scale, float base, int32 multiple) | [source/game_sa/GangWars.cpp:258](../../source/game_sa/GangWars.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWaveCommand(CPed* ped, CTask* task) | [source/game_sa/GangWars.cpp:261](../../source/game_sa/GangWars.cpp#L261) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004AB420: CEventGroup::Add-; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A00: CEventScriptCommand::Constructor; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A50: CEventScriptCommand::Destructor; ruta efectiva sin resolver |
| GangWaveBlip(CPed* ped, eGangID gang, int32 scale) | [source/game_sa/GangWars.cpp:268](../../source/game_sa/GangWars.cpp#L268) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004442D0: CPedPool::GetRef; ruta efectiva sin resolver; Conversión de dirección fija: 0x008D1344, 0x008D1350, 0x008D135C, 0x00B74490 |
| GangDefenderOffset(int32 pedIndex, int32 pedTotal, int32 random1, int32 random2, float baseX, float baseY, float& outAngle, float& outX, float& outY) | [source/game_sa/GangWars.cpp:419](../../source/game_sa/GangWars.cpp#L419) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangPickupOffset(int32 random, float base) | [source/game_sa/GangWars.cpp:427](../../source/game_sa/GangWars.cpp#L427) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWarTimeDelta(bool negative) | [source/game_sa/GangWars.cpp:1003](../../source/game_sa/GangWars.cpp#L1003) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

## Globales y helpers: source/game_sa/Garage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GarageIsPointInsideRegs(CGarage* self, uint32 entryEdx, CVector point) | [source/game_sa/Garage.cpp:40](../../source/game_sa/Garage.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GarageIsPointInsideRadiusRegs(CGarage* self, uint32 entryEdx, CVector point, float radius) | [source/game_sa/Garage.cpp:48](../../source/game_sa/Garage.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CGarage::IsPointInsideGarage-radius 0x004487D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaragePlayerArrestedOrDiedRegs(CGarage* self, uint32 entryEdx) | [source/game_sa/Garage.cpp:55](../../source/game_sa/Garage.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CGarage::PlayerArrestedOrDied 0x004486C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GaField(const void* base, uint32 offset) | [source/game_sa/Garage.cpp:423](../../source/game_sa/Garage.cpp#L423) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaKey(uint32 address) | [source/game_sa/Garage.cpp:426](../../source/game_sa/Garage.cpp#L426) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaPos(CEntity* entity) | [source/game_sa/Garage.cpp:428](../../source/game_sa/Garage.cpp#L428) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaFpv() | [source/game_sa/Garage.cpp:431](../../source/game_sa/Garage.cpp#L431) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaFpp() | [source/game_sa/Garage.cpp:432](../../source/game_sa/Garage.cpp#L432) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaSlideDoorOpen(CGarage* g) | [source/game_sa/Garage.cpp:434](../../source/game_sa/Garage.cpp#L434) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0044A660: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaSlideDoorClosed(CGarage* g) | [source/game_sa/Garage.cpp:435](../../source/game_sa/Garage.cpp#L435) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0044A750: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaIsPlayerOutside(CGarage* g, float radius) | [source/game_sa/Garage.cpp:436](../../source/game_sa/Garage.cpp#L436) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00448E50: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaIsAnyCarBlockingDoor(CGarage* g) | [source/game_sa/Garage.cpp:437](../../source/game_sa/Garage.cpp#L437) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004494F0: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaCountCarsCenterWithin(CGarage* g, CVehicle* ignored) | [source/game_sa/Garage.cpp:438](../../source/game_sa/Garage.cpp#L438) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004495F0: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaIsAnyOtherPedTouching(CGarage* g, CPed* ignored) | [source/game_sa/Garage.cpp:439](../../source/game_sa/Garage.cpp#L439) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00449100: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaIsAnyOtherCarTouching(CGarage* g, CVehicle* ignored) | [source/game_sa/Garage.cpp:440](../../source/game_sa/Garage.cpp#L440) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00447720: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaThrowCarsNearDoorOut(CGarage* g, CVehicle* ignored) | [source/game_sa/Garage.cpp:441](../../source/game_sa/Garage.cpp#L441) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00449220: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaCalcDistToRectSq(CGarage* g, float x, float y) | [source/game_sa/Garage.cpp:442](../../source/game_sa/Garage.cpp#L442) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00447D80: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaRestoreHideOut(CGarage* g, CStoredCar* cars) | [source/game_sa/Garage.cpp:443](../../source/game_sa/Garage.cpp#L443) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00448550: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaNeatlyLineUp(CGarage* g, CStoredCar* cars) | [source/game_sa/Garage.cpp:444](../../source/game_sa/Garage.cpp#L444) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00448330: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaRestoreImpounding(CGarage* g, CStoredCar* cars) | [source/game_sa/Garage.cpp:445](../../source/game_sa/Garage.cpp#L445) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x004485C0: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaStoreAndRemoveImpounding(CGarage* g, CStoredCar* cars, int32 maxSlot) | [source/game_sa/Garage.cpp:446](../../source/game_sa/Garage.cpp#L446) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00449A50: fuera del catálogo consultado; ruta efectiva sin resolver |
| GaFindDoorsWithGarage(CGarage* g, CObject** first, CObject** second) | [source/game_sa/Garage.cpp:447](../../source/game_sa/Garage.cpp#L447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GaMoney() | [source/game_sa/Garage.cpp:448](../../source/game_sa/Garage.cpp#L448) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B7CE50; Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74) |
| * GaStoredCars(eGarageType type) | [source/game_sa/Garage.cpp:449](../../source/game_sa/Garage.cpp#L449) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x0096ABD8 |
| GaAudio(uint32 event) | [source/game_sa/Garage.cpp:450](../../source/game_sa/Garage.cpp#L450) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90) |
| GaMessage(uint32 key) | [source/game_sa/Garage.cpp:451](../../source/game_sa/Garage.cpp#L451) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaPadControls(bool disable) | [source/game_sa/Garage.cpp:453](../../source/game_sa/Garage.cpp#L453) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaWantedBit(bool set) | [source/game_sa/Garage.cpp:458](../../source/game_sa/Garage.cpp#L458) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaX87QuietFloatBits(uint32 bits) | [source/game_sa/Garage.cpp:1537](../../source/game_sa/Garage.cpp#L1537) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaConstructStored(uint32 model) | [source/game_sa/Garage.cpp:1541](../../source/game_sa/Garage.cpp#L1541) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x006E2D50: CVehicle::operator new-anon; ruta efectiva sin resolver |

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Garages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GarageQuietNan(float value) | [source/game_sa/Garages.cpp:173](../../source/game_sa/Garages.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| make_hex_string(TInputIter first, TInputIter last, bool use_uppercase = true, bool insert_spaces = false) | [source/game_sa/GenericGameStorage.cpp:40](../../source/game_sa/GenericGameStorage.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:56](../../source/game_sa/GenericGameStorage.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:65](../../source/game_sa/GenericGameStorage.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SaveVersionName(uint32 version) | [source/game_sa/GenericGameStorage.cpp:237](../../source/game_sa/GenericGameStorage.cpp#L237) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetNameOfSavedGame(int32 slot) | [source/game_sa/GenericGameStorage.h:114](../../source/game_sa/GenericGameStorage.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_SlotFileName: source/game_sa/GenericGameStorage.h:73 (0x00C16368) |
| GetSavedGameState(int32 slot) | [source/game_sa/GenericGameStorage.h:120](../../source/game_sa/GenericGameStorage.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_Slots: source/game_sa/GenericGameStorage.h:74 (0x00C16EBC) |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/Glass.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:52](../../source/game_sa/Glass.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyPoint_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:61](../../source/game_sa/Glass.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindMinMaxZOfVertices(const CVector (&v)\[4\], float& minZ, float& maxZ) | [source/game_sa/Glass.cpp:70](../../source/game_sa/Glass.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindVertexFurthestFromV0(const CVector (&v)\[4\]) | [source/game_sa/Glass.cpp:81](../../source/game_sa/Glass.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlassObjectWithCol(CEntity* entity) | [source/game_sa/Glass.cpp:258](../../source/game_sa/Glass.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/GrassRenderer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GrassDistance(const CVector& a, const CVector& b) | [source/game_sa/GrassRenderer.cpp:69](../../source/game_sa/GrassRenderer.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/GxtChar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char* out, const GxtChar* src, size_t size, size_t offset) | [source/game_sa/GxtChar.cpp:15](../../source/game_sa/GxtChar.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcat(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:54](../../source/game_sa/GxtChar.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GxtCharStrlen(const GxtChar* str) | [source/game_sa/GxtChar.cpp:68](../../source/game_sa/GxtChar.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeLowerCase(char* str) | [source/game_sa/GxtChar.cpp:76](../../source/game_sa/GxtChar.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TextCopy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:87](../../source/game_sa/GxtChar.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcpy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:97](../../source/game_sa/GxtChar.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/GxtChar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char (&out)\[N\], const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:25](../../source/game_sa/GxtChar.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharToUTF8(const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:29](../../source/game_sa/GxtChar.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharFromAscii(const char* asciiText) | [source/game_sa/GxtChar.h:84](../../source/game_sa/GxtChar.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsciiFromGxtChar(const GxtChar* gxtAsciiText) | [source/game_sa/GxtChar.h:97](../../source/game_sa/GxtChar.h#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator""_gxt(const char* text, size_t) | [source/game_sa/GxtChar.h:101](../../source/game_sa/GxtChar.h#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
