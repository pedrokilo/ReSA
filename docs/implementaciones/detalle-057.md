<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

## Globales y helpers: source/game_sa/Fx/FxTools.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReadFieldImpl(FILESTREAM file, Type& refValue, const char* fieldName = nullptr) | [source/game_sa/Fx/FxTools.h:46](../../source/game_sa/Fx/FxTools.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:60](../../source/game_sa/Fx/FxTools.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;void&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:67](../../source/game_sa/Fx/FxTools.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;int8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:74](../../source/game_sa/Fx/FxTools.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;uint8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:79](../../source/game_sa/Fx/FxTools.h#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;bool&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:84](../../source/game_sa/Fx/FxTools.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/GameLogic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingMinusHalfPi(const CPlaceable& placeable) | [source/game_sa/GameLogic.cpp:67](../../source/game_sa/GameLogic.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LeaveVehicleForResurrection() | [source/game_sa/GameLogic.cpp:647](../../source/game_sa/GameLogic.cpp#L647) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| GetRestartSearchPosition() | [source/game_sa/GameLogic.cpp:669](../../source/game_sa/GameLogic.cpp#L669) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| UseTwoPlayerRestartPoint(CVector& restartPoint, float& restartAngle) | [source/game_sa/GameLogic.cpp:679](../../source/game_sa/GameLogic.cpp#L679) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo f2PlayerStartHeading: source/game_sa/GameLogic.h:50 (0x0096A840); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0); Posible dato fijo vec2PlayerStartLocation: source/game_sa/GameLogic.h:51 (0x0096A9AC) |
| FinishResurrection() | [source/game_sa/GameLogic.cpp:688](../../source/game_sa/GameLogic.cpp#L688) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CountDownToCarsAtStart: source/game_sa/CarCtrl.h:39 (0x009690C0); Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028); Posible dato fijo m_CountDownToPedsAtStart: source/game_sa/Population.h:45 (0x00C0EC23); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/GangWars.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangWaveRandomOffset(float base, float scale, float subtract) | [source/game_sa/GangWars.cpp:242](../../source/game_sa/GangWars.cpp#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWaveInteger(float value, float scale, float base, int32 multiple) | [source/game_sa/GangWars.cpp:245](../../source/game_sa/GangWars.cpp#L245) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWaveCommand(CPed* ped, CTask* task) | [source/game_sa/GangWars.cpp:248](../../source/game_sa/GangWars.cpp#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004AB420: CEventGroup::Add-; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A00: CEventScriptCommand::Constructor; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A50: CEventScriptCommand::Destructor; ruta efectiva sin resolver |
| GangWaveBlip(CPed* ped, eGangID gang, int32 scale) | [source/game_sa/GangWars.cpp:255](../../source/game_sa/GangWars.cpp#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004442D0: CPedPool::GetRef; ruta efectiva sin resolver; Conversión de dirección fija: 0x008D1344, 0x008D1350, 0x008D135C, 0x00B74490 |
| GangDefenderOffset(int32 pedIndex, int32 pedTotal, int32 random1, int32 random2, float baseX, float baseY, float& outAngle, float& outX, float& outY) | [source/game_sa/GangWars.cpp:406](../../source/game_sa/GangWars.cpp#L406) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangPickupOffset(int32 random, float base) | [source/game_sa/GangWars.cpp:414](../../source/game_sa/GangWars.cpp#L414) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWarTimeDelta(bool negative) | [source/game_sa/GangWars.cpp:990](../../source/game_sa/GangWars.cpp#L990) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/Garage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GarageIsPointInsideRegs(CGarage* self, uint32 entryEdx, CVector point) | [source/game_sa/Garage.cpp:30](../../source/game_sa/Garage.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GarageIsPointInsideRadiusRegs(CGarage* self, uint32 entryEdx, CVector point, float radius) | [source/game_sa/Garage.cpp:38](../../source/game_sa/Garage.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CGarage::IsPointInsideGarage-radius 0x004487D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| make_hex_string(TInputIter first, TInputIter last, bool use_uppercase = true, bool insert_spaces = false) | [source/game_sa/GenericGameStorage.cpp:40](../../source/game_sa/GenericGameStorage.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:56](../../source/game_sa/GenericGameStorage.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:65](../../source/game_sa/GenericGameStorage.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SaveVersionName(uint32 version) | [source/game_sa/GenericGameStorage.cpp:237](../../source/game_sa/GenericGameStorage.cpp#L237) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetNameOfSavedGame(int32 slot) | [source/game_sa/GenericGameStorage.h:114](../../source/game_sa/GenericGameStorage.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_SlotFileName: source/game_sa/GenericGameStorage.h:73 (0x00C16368) |
| GetSavedGameState(int32 slot) | [source/game_sa/GenericGameStorage.h:120](../../source/game_sa/GenericGameStorage.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_Slots: source/game_sa/GenericGameStorage.h:74 (0x00C16EBC) |

<a id="grupo-1127"></a>

## Globales y helpers: source/game_sa/Glass.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:52](../../source/game_sa/Glass.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyPoint_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:61](../../source/game_sa/Glass.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindMinMaxZOfVertices(const CVector (&v)\[4\], float& minZ, float& maxZ) | [source/game_sa/Glass.cpp:70](../../source/game_sa/Glass.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindVertexFurthestFromV0(const CVector (&v)\[4\]) | [source/game_sa/Glass.cpp:81](../../source/game_sa/Glass.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlassObjectWithCol(CEntity* entity) | [source/game_sa/Glass.cpp:258](../../source/game_sa/Glass.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/GrassRenderer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GrassDistance(const CVector& a, const CVector& b) | [source/game_sa/GrassRenderer.cpp:69](../../source/game_sa/GrassRenderer.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1129"></a>

## Globales y helpers: source/game_sa/GxtChar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char* out, const GxtChar* src, size_t size, size_t offset) | [source/game_sa/GxtChar.cpp:15](../../source/game_sa/GxtChar.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcat(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:54](../../source/game_sa/GxtChar.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GxtCharStrlen(const GxtChar* str) | [source/game_sa/GxtChar.cpp:68](../../source/game_sa/GxtChar.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeLowerCase(char* str) | [source/game_sa/GxtChar.cpp:76](../../source/game_sa/GxtChar.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TextCopy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:87](../../source/game_sa/GxtChar.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcpy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:97](../../source/game_sa/GxtChar.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/GxtChar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char (&out)\[N\], const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:25](../../source/game_sa/GxtChar.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharToUTF8(const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:29](../../source/game_sa/GxtChar.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharFromAscii(const char* asciiText) | [source/game_sa/GxtChar.h:84](../../source/game_sa/GxtChar.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsciiFromGxtChar(const GxtChar* gxtAsciiText) | [source/game_sa/GxtChar.h:97](../../source/game_sa/GxtChar.h#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator""_gxt(const char* text, size_t) | [source/game_sa/GxtChar.h:101](../../source/game_sa/GxtChar.h#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/Hud.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeFloat(uintptr_t address) | [source/game_sa/Hud.cpp:40](../../source/game_sa/Hud.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StretchBaseX() | [source/game_sa/Hud.cpp:44](../../source/game_sa/Hud.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| StretchBaseY() | [source/game_sa/Hud.cpp:45](../../source/game_sa/Hud.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| HudRawRect(float left, float bottom, float right, float top) | [source/game_sa/Hud.cpp:1203](../../source/game_sa/Hud.cpp#L1203) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CHud::DrawRadar 0x0058A330 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleScaleY(int height, bool widescreen) | [source/game_sa/Hud.cpp:1370](../../source/game_sa/Hud.cpp#L1370) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleCentreVitals(int width) | [source/game_sa/Hud.cpp:1374](../../source/game_sa/Hud.cpp#L1374) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleScaleX(int width, bool vitals) | [source/game_sa/Hud.cpp:1378](../../source/game_sa/Hud.cpp#L1378) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/IdleCam.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IdleMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/IdleCam.cpp:31](../../source/game_sa/IdleCam.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

## Globales y helpers: source/game_sa/InterestingEvents.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InterestingSector(float coordinate, float radius, bool useEnd) | [source/game_sa/InterestingEvents.cpp:162](../../source/game_sa/InterestingEvents.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Interior/FurnitureSubGroup_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * SelectFurnitureByWealth(TList_c&lt;Furniture_c&gt;& list, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:42](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/Interior/InteriorManager_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Interior/InteriorManager_c.cpp:58](../../source/game_sa/Interior/InteriorManager_c.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ModelInfoOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:67](../../source/game_sa/Interior/InteriorManager_c.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| AreaOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:72](../../source/game_sa/Interior/InteriorManager_c.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/Interior/Interior_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & InteriorField(const void* base, ptrdiff_t offset) | [source/game_sa/Interior/Interior_c.cpp:41](../../source/game_sa/Interior/Interior_c.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & InteriorField(uintptr_t base, ptrdiff_t offset) | [source/game_sa/Interior/Interior_c.cpp:45](../../source/game_sa/Interior/Interior_c.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorFloatBits(float value) | [source/game_sa/Interior/Interior_c.cpp:49](../../source/game_sa/Interior/Interior_c.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorBitsFloat(uint32 bits) | [source/game_sa/Interior/Interior_c.cpp:54](../../source/game_sa/Interior/Interior_c.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorRandCpp() | [source/game_sa/Interior/Interior_c.cpp:60](../../source/game_sa/Interior/Interior_c.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorRandomCoordinate(int32 extent) | [source/game_sa/Interior/Interior_c.cpp:66](../../source/game_sa/Interior/Interior_c.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorLocalTileCentre(int32 negativeWidth, int32 negativeHeight, int32 negativeDepth, float offsetX, float offsetY, CVector* output) | [source/game_sa/Interior/Interior_c.cpp:72](../../source/game_sa/Interior/Interior_c.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorProjectionInside(const RwV3d* axis, const CVector* delta, int32 dimension, float bias) | [source/game_sa/Interior/Interior_c.cpp:82](../../source/game_sa/Interior/Interior_c.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorTruncate(float value) | [source/game_sa/Interior/Interior_c.cpp:92](../../source/game_sa/Interior/Interior_c.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorPositionSeed(const CVector* position, int32 extra) | [source/game_sa/Interior/Interior_c.cpp:96](../../source/game_sa/Interior/Interior_c.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorNearEntrance(const CVector* camera, const CVector* centre) | [source/game_sa/Interior/Interior_c.cpp:100](../../source/game_sa/Interior/Interior_c.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorObjectZExact(float offsetZ,float boundMinZ,int32 negativeHeight) | [source/game_sa/Interior/Interior_c.cpp:2357](../../source/game_sa/Interior/Interior_c.cpp#L2357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Interior_c::PlaceObject 0x005934E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorFurniturePositionExact(CVector* position,int32 negativeWidth,int32 negativeDepth,int32 boxHeight,int32 tileX,int32 tileY,int32 shiftX,int32 shiftY,float height,bool ceiling) | [source/game_sa/Interior/Interior_c.cpp:2362](../../source/game_sa/Interior/Interior_c.cpp#L2362) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/IplStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolveAreaCode(int32 ec) | [source/game_sa/IplStore.cpp:169](../../source/game_sa/IplStore.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |
| IsIPLAnInterior(const char* iplFileName) | [source/game_sa/IplStore.cpp:764](../../source/game_sa/IplStore.cpp#L764) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExtractIPLNameFromPath(const char* iplFilePath, char(&out)\[N\]) | [source/game_sa/IplStore.cpp:786](../../source/game_sa/IplStore.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/IplStore.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetLoadedBuildings() | [source/game_sa/IplStore.h:67](../../source/game_sa/IplStore.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNumLoadedBuildings: source/game_sa/IplStore.h:64 (0x00BCC0D8); Posible dato fijo gpLoadedBuildings: source/game_sa/IplStore.h:65 (0x00BCC0E0) |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/LoadingScreen.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClockSeconds() | [source/game_sa/LoadingScreen.cpp:62](../../source/game_sa/LoadingScreen.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LoadingStretchX(float a) | [source/game_sa/LoadingScreen.cpp:70](../../source/game_sa/LoadingScreen.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| LoadingStretchY(float a) | [source/game_sa/LoadingScreen.cpp:71](../../source/game_sa/LoadingScreen.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/Memory/MemoryHeap.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetCommonSize(uint32 size) | [source/game_sa/Memory/MemoryHeap.cpp:12](../../source/game_sa/Memory/MemoryHeap.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:16](../../source/game_sa/Memory/MemoryHeap.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFreeBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:21](../../source/game_sa/Memory/MemoryHeap.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
