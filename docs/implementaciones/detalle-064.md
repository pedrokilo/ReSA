<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1261"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitter.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitterExeRemainder() | [source/game_sa/Fx/FxEmitter.cpp:208](../../source/game_sa/Fx/FxEmitter.cpp#L208) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1262"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitterBP.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __cdecl FxEmitterBPOperatorNew(int32 size) | [source/game_sa/Fx/FxEmitterBP.cpp:42](../../source/game_sa/Fx/FxEmitterBP.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxEmitterBP_c::OperatorNew 0x004A1900 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| RenderEmitterParticles355(FxEmitterBP_c& emitter, RwCamera* camera, float& brightness, bool heat) | [source/game_sa/Fx/FxEmitterBP.cpp:92](../../source/game_sa/Fx/FxEmitterBP.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B6FA14; Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80); Posible dato fijo m_Color: source/game_sa/Font.h:85 (0x00C71A60); Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |

<a id="grupo-1263"></a>

## Globales y helpers: source/game_sa/Fx/FxInfoManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoManagerDestructorBody(FxInfoManager_c* /*self*/, void* /*edx*/) | [source/game_sa/Fx/FxInfoManager.cpp:71](../../source/game_sa/Fx/FxInfoManager.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInfoManager_c::DestructorBody 0x004A4950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewFxInfoInPool() | [source/game_sa/Fx/FxInfoManager.cpp:91](../../source/game_sa/Fx/FxInfoManager.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |

<a id="grupo-1264"></a>

## Globales y helpers: source/game_sa/Fx/FxManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewFxEmitterArray() | [source/game_sa/Fx/FxManager.cpp:93](../../source/game_sa/Fx/FxManager.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1265"></a>

## Globales y helpers: source/game_sa/Fx/FxPrimBP.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall FxPrimBPConstructorBody(FxPrimBP_c* self, void* /*edx*/) | [source/game_sa/Fx/FxPrimBP.cpp:28](../../source/game_sa/Fx/FxPrimBP.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxPrimBPDestructorBody(FxPrimBP_c* self, void* /*edx*/) | [source/game_sa/Fx/FxPrimBP.cpp:45](../../source/game_sa/Fx/FxPrimBP.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: TList_c::ListDestructorExe 0x004A8DE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1266"></a>

## Globales y helpers: source/game_sa/Fx/FxSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxOpenFileThunk(const char* path, const char* mode) | [source/game_sa/Fx/FxSystem.cpp:33](../../source/game_sa/Fx/FxSystem.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxSystem_c::OpenFileThunk 0x004AB240 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxReadLineThunk(FILESTREAM file, char* str, int32 num) | [source/game_sa/Fx/FxSystem.cpp:35](../../source/game_sa/Fx/FxSystem.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxSystem_c::ReadLineThunk 0x004AB250 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxCloseFileThunk(FILESTREAM file) | [source/game_sa/Fx/FxSystem.cpp:37](../../source/game_sa/Fx/FxSystem.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxSystem_c::CloseFileThunk 0x004AB260 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanAddParticle() | [source/game_sa/Fx/FxSystem.cpp:230](../../source/game_sa/Fx/FxSystem.cpp#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |

<a id="grupo-1267"></a>

## Globales y helpers: source/game_sa/Fx/FxTools.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReadLine(FILESTREAM file, char* buffer, int32 bufferSize) | [source/game_sa/Fx/FxTools.cpp:13](../../source/game_sa/Fx/FxTools.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1268"></a>

## Globales y helpers: source/game_sa/Fx/FxTools.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReadFieldImpl(FILESTREAM file, Type& refValue, const char* fieldName = nullptr) | [source/game_sa/Fx/FxTools.h:46](../../source/game_sa/Fx/FxTools.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:60](../../source/game_sa/Fx/FxTools.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;void&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:67](../../source/game_sa/Fx/FxTools.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;int8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:74](../../source/game_sa/Fx/FxTools.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;uint8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:79](../../source/game_sa/Fx/FxTools.h#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;bool&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:84](../../source/game_sa/Fx/FxTools.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1269"></a>

## Globales y helpers: source/game_sa/GameLogic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingMinusHalfPi(const CPlaceable& placeable) | [source/game_sa/GameLogic.cpp:67](../../source/game_sa/GameLogic.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LeaveVehicleForResurrection() | [source/game_sa/GameLogic.cpp:647](../../source/game_sa/GameLogic.cpp#L647) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| GetRestartSearchPosition() | [source/game_sa/GameLogic.cpp:669](../../source/game_sa/GameLogic.cpp#L669) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| UseTwoPlayerRestartPoint(CVector& restartPoint, float& restartAngle) | [source/game_sa/GameLogic.cpp:679](../../source/game_sa/GameLogic.cpp#L679) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo f2PlayerStartHeading: source/game_sa/GameLogic.h:50 (0x0096A840); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0); Posible dato fijo vec2PlayerStartLocation: source/game_sa/GameLogic.h:51 (0x0096A9AC) |
| FinishResurrection() | [source/game_sa/GameLogic.cpp:688](../../source/game_sa/GameLogic.cpp#L688) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CountDownToCarsAtStart: source/game_sa/CarCtrl.h:39 (0x009690C0); Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo m_CountDownToPedsAtStart: source/game_sa/Population.h:45 (0x00C0EC23); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1270"></a>

## Globales y helpers: source/game_sa/GangWars.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangWaveRandomOffset(float base, float scale, float subtract) | [source/game_sa/GangWars.cpp:256](../../source/game_sa/GangWars.cpp#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWaveInteger(float value, float scale, float base, int32 multiple) | [source/game_sa/GangWars.cpp:259](../../source/game_sa/GangWars.cpp#L259) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWaveCommand(CPed* ped, CTask* task) | [source/game_sa/GangWars.cpp:262](../../source/game_sa/GangWars.cpp#L262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004AB420: CEventGroup::Add-; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A00: CEventScriptCommand::Constructor; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A50: CEventScriptCommand::Destructor; ruta efectiva sin resolver |
| GangWaveBlip(CPed* ped, eGangID gang, int32 scale) | [source/game_sa/GangWars.cpp:269](../../source/game_sa/GangWars.cpp#L269) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004442D0: CPedPool::GetRef; ruta efectiva sin resolver; Conversión de dirección fija: 0x008D1344, 0x008D1350, 0x008D135C, 0x00B74490 |
| GangDefenderOffset(int32 pedIndex, int32 pedTotal, int32 random1, int32 random2, float baseX, float baseY, float& outAngle, float& outX, float& outY) | [source/game_sa/GangWars.cpp:421](../../source/game_sa/GangWars.cpp#L421) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangPickupOffset(int32 random, float base) | [source/game_sa/GangWars.cpp:429](../../source/game_sa/GangWars.cpp#L429) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWarTimeDelta(bool negative) | [source/game_sa/GangWars.cpp:1006](../../source/game_sa/GangWars.cpp#L1006) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1271"></a>

## Globales y helpers: source/game_sa/Garage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GarageIsPointInsideRegs(CGarage* self, uint32 entryEdx, CVector point) | [source/game_sa/Garage.cpp:50](../../source/game_sa/Garage.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CGarage::IsPointInsideGarage-point 0x00448740 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GarageIsPointInsideRadiusRegs(CGarage* self, uint32 entryEdx, CVector point, float radius) | [source/game_sa/Garage.cpp:58](../../source/game_sa/Garage.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CGarage::IsPointInsideGarage-radius 0x004487D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaragePlayerArrestedOrDiedRegs(CGarage* self, uint32 entryEdx) | [source/game_sa/Garage.cpp:65](../../source/game_sa/Garage.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CGarage::PlayerArrestedOrDied 0x004486C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GaField(const void* base, uint32 offset) | [source/game_sa/Garage.cpp:477](../../source/game_sa/Garage.cpp#L477) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaKey(uint32 address) | [source/game_sa/Garage.cpp:481](../../source/game_sa/Garage.cpp#L481) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaPos(CEntity* entity) | [source/game_sa/Garage.cpp:483](../../source/game_sa/Garage.cpp#L483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaFpv() | [source/game_sa/Garage.cpp:486](../../source/game_sa/Garage.cpp#L486) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaFpp() | [source/game_sa/Garage.cpp:487](../../source/game_sa/Garage.cpp#L487) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaFindDoorsWithGarage(CGarage* g, CObject** first, CObject** second) | [source/game_sa/Garage.cpp:489](../../source/game_sa/Garage.cpp#L489) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GaMoney() | [source/game_sa/Garage.cpp:490](../../source/game_sa/Garage.cpp#L490) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B7CE50; Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74) |
| * GaStoredCars(eGarageType type) | [source/game_sa/Garage.cpp:491](../../source/game_sa/Garage.cpp#L491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x0096ABD8 |
| GaAudio(uint32 event) | [source/game_sa/Garage.cpp:492](../../source/game_sa/Garage.cpp#L492) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90) |
| GaMessage(uint32 key) | [source/game_sa/Garage.cpp:493](../../source/game_sa/Garage.cpp#L493) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaPadControls(bool disable) | [source/game_sa/Garage.cpp:495](../../source/game_sa/Garage.cpp#L495) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaWantedBit(bool set) | [source/game_sa/Garage.cpp:500](../../source/game_sa/Garage.cpp#L500) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GaX87QuietFloatBits(uint32 bits) | [source/game_sa/Garage.cpp:1894](../../source/game_sa/Garage.cpp#L1894) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GaConstructStored(uint32 model) | [source/game_sa/Garage.cpp:1898](../../source/game_sa/Garage.cpp#L1898) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x006E2D50: CVehicle::operator new-anon; ruta efectiva sin resolver |

<a id="grupo-1272"></a>

## Globales y helpers: source/game_sa/Garages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GarageQuietNan(float value) | [source/game_sa/Garages.cpp:184](../../source/game_sa/Garages.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1273"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| make_hex_string(TInputIter first, TInputIter last, bool use_uppercase = true, bool insert_spaces = false) | [source/game_sa/GenericGameStorage.cpp:40](../../source/game_sa/GenericGameStorage.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:56](../../source/game_sa/GenericGameStorage.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:65](../../source/game_sa/GenericGameStorage.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SaveVersionName(uint32 version) | [source/game_sa/GenericGameStorage.cpp:245](../../source/game_sa/GenericGameStorage.cpp#L245) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1274"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetNameOfSavedGame(int32 slot) | [source/game_sa/GenericGameStorage.h:114](../../source/game_sa/GenericGameStorage.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_SlotFileName: source/game_sa/GenericGameStorage.h:73 (0x00C16368) |
| GetSavedGameState(int32 slot) | [source/game_sa/GenericGameStorage.h:120](../../source/game_sa/GenericGameStorage.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_Slots: source/game_sa/GenericGameStorage.h:74 (0x00C16EBC) |

<a id="grupo-1275"></a>

## Globales y helpers: source/game_sa/Glass.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:52](../../source/game_sa/Glass.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyPoint_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:61](../../source/game_sa/Glass.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindMinMaxZOfVertices(const CVector (&v)\[4\], float& minZ, float& maxZ) | [source/game_sa/Glass.cpp:70](../../source/game_sa/Glass.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindVertexFurthestFromV0(const CVector (&v)\[4\]) | [source/game_sa/Glass.cpp:81](../../source/game_sa/Glass.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlassObjectWithCol(CEntity* entity) | [source/game_sa/Glass.cpp:258](../../source/game_sa/Glass.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1276"></a>

## Globales y helpers: source/game_sa/GrassRenderer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GrassDistance(const CVector& a, const CVector& b) | [source/game_sa/GrassRenderer.cpp:69](../../source/game_sa/GrassRenderer.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1277"></a>

## Globales y helpers: source/game_sa/GxtChar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char* out, const GxtChar* src, size_t size, size_t offset) | [source/game_sa/GxtChar.cpp:46](../../source/game_sa/GxtChar.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeLowerCase(char* str) | [source/game_sa/GxtChar.cpp:108](../../source/game_sa/GxtChar.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1278"></a>

## Globales y helpers: source/game_sa/GxtChar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char (&out)\[N\], const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:33](../../source/game_sa/GxtChar.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharToUTF8(const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:37](../../source/game_sa/GxtChar.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharFromAscii(const char* asciiText) | [source/game_sa/GxtChar.h:92](../../source/game_sa/GxtChar.h#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsciiFromGxtChar(const GxtChar* gxtAsciiText) | [source/game_sa/GxtChar.h:105](../../source/game_sa/GxtChar.h#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator""_gxt(const char* text, size_t) | [source/game_sa/GxtChar.h:109](../../source/game_sa/GxtChar.h#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1279"></a>

## Globales y helpers: source/game_sa/Hud.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeFloat(uintptr_t address) | [source/game_sa/Hud.cpp:40](../../source/game_sa/Hud.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StretchBaseX() | [source/game_sa/Hud.cpp:44](../../source/game_sa/Hud.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| StretchBaseY() | [source/game_sa/Hud.cpp:45](../../source/game_sa/Hud.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| HudRawRect(float left, float bottom, float right, float top) | [source/game_sa/Hud.cpp:1211](../../source/game_sa/Hud.cpp#L1211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CHud::DrawRadar 0x0058A330 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleScaleY(int height, bool widescreen) | [source/game_sa/Hud.cpp:1379](../../source/game_sa/Hud.cpp#L1379) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleCentreVitals(int width) | [source/game_sa/Hud.cpp:1383](../../source/game_sa/Hud.cpp#L1383) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleScaleX(int width, bool vitals) | [source/game_sa/Hud.cpp:1387](../../source/game_sa/Hud.cpp#L1387) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1280"></a>

## Globales y helpers: source/game_sa/IdleCam.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IdleMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/IdleCam.cpp:31](../../source/game_sa/IdleCam.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
