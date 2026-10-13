<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1281"></a>

## Globales y helpers: source/game_sa/GxtChar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char* out, const GxtChar* src, size_t size, size_t offset) | [source/game_sa/GxtChar.cpp:15](../../source/game_sa/GxtChar.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcat(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:54](../../source/game_sa/GxtChar.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GxtCharStrlen(const GxtChar* str) | [source/game_sa/GxtChar.cpp:68](../../source/game_sa/GxtChar.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeLowerCase(char* str) | [source/game_sa/GxtChar.cpp:76](../../source/game_sa/GxtChar.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TextCopy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:87](../../source/game_sa/GxtChar.cpp#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharStrcpy(GxtChar* dst, const GxtChar* src) | [source/game_sa/GxtChar.cpp:97](../../source/game_sa/GxtChar.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1282"></a>

## Globales y helpers: source/game_sa/GxtChar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char (&out)\[N\], const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:25](../../source/game_sa/GxtChar.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharToUTF8(const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:29](../../source/game_sa/GxtChar.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharFromAscii(const char* asciiText) | [source/game_sa/GxtChar.h:84](../../source/game_sa/GxtChar.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsciiFromGxtChar(const GxtChar* gxtAsciiText) | [source/game_sa/GxtChar.h:97](../../source/game_sa/GxtChar.h#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator""_gxt(const char* text, size_t) | [source/game_sa/GxtChar.h:101](../../source/game_sa/GxtChar.h#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1283"></a>

## Globales y helpers: source/game_sa/Hud.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeFloat(uintptr_t address) | [source/game_sa/Hud.cpp:39](../../source/game_sa/Hud.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StretchBaseX() | [source/game_sa/Hud.cpp:43](../../source/game_sa/Hud.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| StretchBaseY() | [source/game_sa/Hud.cpp:44](../../source/game_sa/Hud.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| HudRawRect(float left, float bottom, float right, float top) | [source/game_sa/Hud.cpp:1202](../../source/game_sa/Hud.cpp#L1202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CHud::DrawRadar 0x0058A330 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleScaleY(int height, bool widescreen) | [source/game_sa/Hud.cpp:1386](../../source/game_sa/Hud.cpp#L1386) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SubtitleCentreVitals(int width) | [source/game_sa/Hud.cpp:1402](../../source/game_sa/Hud.cpp#L1402) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SubtitleScaleX(int width, bool vitals) | [source/game_sa/Hud.cpp:1426](../../source/game_sa/Hud.cpp#L1426) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1284"></a>

## Globales y helpers: source/game_sa/IdleCam.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IdleMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/IdleCam.cpp:40](../../source/game_sa/IdleCam.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1285"></a>

## Globales y helpers: source/game_sa/InterestingEvents.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InterestingSector(float coordinate, float radius, bool useEnd) | [source/game_sa/InterestingEvents.cpp:162](../../source/game_sa/InterestingEvents.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1286"></a>

## Globales y helpers: source/game_sa/Interior/FurnitureSubGroup_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * SelectFurnitureByWealth(TList_c&lt;Furniture_c&gt;& list, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:42](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1287"></a>

## Globales y helpers: source/game_sa/Interior/InteriorManager_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Interior/InteriorManager_c.cpp:58](../../source/game_sa/Interior/InteriorManager_c.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ModelInfoOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:67](../../source/game_sa/Interior/InteriorManager_c.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| AreaOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:72](../../source/game_sa/Interior/InteriorManager_c.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1288"></a>

## Globales y helpers: source/game_sa/Interior/Interior_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InteriorRandomCoordinate(int32 extent) | [source/game_sa/Interior/Interior_c.cpp:38](../../source/game_sa/Interior/Interior_c.cpp#L38) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | Conversión de dirección fija: 0x00858B14; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorLocalTileCentre(int32 negativeWidth, int32 negativeHeight, int32 negativeDepth, float offsetX, float offsetY, CVector* output) | [source/game_sa/Interior/Interior_c.cpp:53](../../source/game_sa/Interior/Interior_c.cpp#L53) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorProjectionInside(const RwV3d* axis, const CVector* delta, int32 dimension, float bias) | [source/game_sa/Interior/Interior_c.cpp:74](../../source/game_sa/Interior/Interior_c.cpp#L74) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorTruncate(float value) | [source/game_sa/Interior/Interior_c.cpp:101](../../source/game_sa/Interior/Interior_c.cpp#L101) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorPositionSeed(const CVector* position, int32 extra) | [source/game_sa/Interior/Interior_c.cpp:109](../../source/game_sa/Interior/Interior_c.cpp#L109) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorNearEntrance(const CVector* camera, const CVector* centre) | [source/game_sa/Interior/Interior_c.cpp:121](../../source/game_sa/Interior/Interior_c.cpp#L121) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00407180() | [source/game_sa/Interior/Interior_c.cpp:255](../../source/game_sa/Interior/Interior_c.cpp#L255) | Ensamblador · Envoltura al EXE | Histórica del catálogo: Interior_c::Bedroom_AddTableItem 0x00593F10 | 1 / 1 | Trampolín ASM a 0x00407180; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall005911E0() | [source/game_sa/Interior/Interior_c.cpp:257](../../source/game_sa/Interior/Interior_c.cpp#L257) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005911E0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00591220() | [source/game_sa/Interior/Interior_c.cpp:259](../../source/game_sa/Interior/Interior_c.cpp#L259) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00591220; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00591700() | [source/game_sa/Interior/Interior_c.cpp:261](../../source/game_sa/Interior/Interior_c.cpp#L261) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00591700; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall005917C0() | [source/game_sa/Interior/Interior_c.cpp:263](../../source/game_sa/Interior/Interior_c.cpp#L263) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005917C0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall005918E0() | [source/game_sa/Interior/Interior_c.cpp:265](../../source/game_sa/Interior/Interior_c.cpp#L265) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005918E0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00591C50() | [source/game_sa/Interior/Interior_c.cpp:267](../../source/game_sa/Interior/Interior_c.cpp#L267) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00591C50; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00591D20() | [source/game_sa/Interior/Interior_c.cpp:269](../../source/game_sa/Interior/Interior_c.cpp#L269) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00591D20; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00591E40() | [source/game_sa/Interior/Interior_c.cpp:271](../../source/game_sa/Interior/Interior_c.cpp#L271) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00591E40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00592AA0() | [source/game_sa/Interior/Interior_c.cpp:273](../../source/game_sa/Interior/Interior_c.cpp#L273) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00592AA0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00593120() | [source/game_sa/Interior/Interior_c.cpp:275](../../source/game_sa/Interior/Interior_c.cpp#L275) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00593120; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00593340() | [source/game_sa/Interior/Interior_c.cpp:277](../../source/game_sa/Interior/Interior_c.cpp#L277) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00593340; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall005934E0() | [source/game_sa/Interior/Interior_c.cpp:279](../../source/game_sa/Interior/Interior_c.cpp#L279) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005934E0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00593F10() | [source/game_sa/Interior/Interior_c.cpp:281](../../source/game_sa/Interior/Interior_c.cpp#L281) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00593F10; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00596930() | [source/game_sa/Interior/Interior_c.cpp:283](../../source/game_sa/Interior/Interior_c.cpp#L283) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00596930; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00597240() | [source/game_sa/Interior/Interior_c.cpp:285](../../source/game_sa/Interior/Interior_c.cpp#L285) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00597240; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00597430() | [source/game_sa/Interior/Interior_c.cpp:287](../../source/game_sa/Interior/Interior_c.cpp#L287) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00597430; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall005974E0() | [source/game_sa/Interior/Interior_c.cpp:289](../../source/game_sa/Interior/Interior_c.cpp#L289) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005974E0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall005975C0() | [source/game_sa/Interior/Interior_c.cpp:291](../../source/game_sa/Interior/Interior_c.cpp#L291) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005975C0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00821B1E() | [source/game_sa/Interior/Interior_c.cpp:293](../../source/game_sa/Interior/Interior_c.cpp#L293) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00821B1E; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeCall00821B40() | [source/game_sa/Interior/Interior_c.cpp:295](../../source/game_sa/Interior/Interior_c.cpp#L295) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00821B40; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeBedroom_AddTableItem() | [source/game_sa/Interior/Interior_c.cpp:298](../../source/game_sa/Interior/Interior_c.cpp#L298) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeFurnishBedroom() | [source/game_sa/Interior/Interior_c.cpp:414](../../source/game_sa/Interior/Interior_c.cpp#L414) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::FurnishBedroom 0x00593FC0 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeKitchen_FurnishEdges() | [source/game_sa/Interior/Interior_c.cpp:1836](../../source/game_sa/Interior/Interior_c.cpp#L1836) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::Kitchen_FurnishEdges 0x00596930 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeFurnishKitchen() | [source/game_sa/Interior/Interior_c.cpp:3106](../../source/game_sa/Interior/Interior_c.cpp#L3106) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::FurnishKitchen 0x005970B0 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeLounge_AddTV() | [source/game_sa/Interior/Interior_c.cpp:3422](../../source/game_sa/Interior/Interior_c.cpp#L3422) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::Lounge_AddTV 0x00597240 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeLounge_AddHifi() | [source/game_sa/Interior/Interior_c.cpp:3712](../../source/game_sa/Interior/Interior_c.cpp#L3712) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::Lounge_AddHifi 0x00597430 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeLounge_AddChairInfo() | [source/game_sa/Interior/Interior_c.cpp:3830](../../source/game_sa/Interior/Interior_c.cpp#L3830) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::Lounge_AddChairInfo 0x005974E0 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeLounge_AddSofaInfo() | [source/game_sa/Interior/Interior_c.cpp:3974](../../source/game_sa/Interior/Interior_c.cpp#L3974) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::Lounge_AddSofaInfo 0x005975C0 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorHomeNativeFurnishLounge() | [source/game_sa/Interior/Interior_c.cpp:4264](../../source/game_sa/Interior/Interior_c.cpp#L4264) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::FurnishLounge 0x00597740 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorObjectZExact(float offsetZ,float boundMinZ,int32 negativeHeight) | [source/game_sa/Interior/Interior_c.cpp:7738](../../source/game_sa/Interior/Interior_c.cpp#L7738) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Interior_c::PlaceObject 0x005934E0 | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InteriorFurniturePositionExact(CVector* position,int32 negativeWidth,int32 negativeDepth,int32 boxHeight,int32 tileX,int32 tileY,int32 shiftX,int32 shiftY,float height,bool ceiling) | [source/game_sa/Interior/Interior_c.cpp:7750](../../source/game_sa/Interior/Interior_c.cpp#L7750) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1289"></a>

## Globales y helpers: source/game_sa/IplStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetIfIplIsRequired(const CVector2D& posn, IplDef* def) | [source/game_sa/IplStore.cpp:96](../../source/game_sa/IplStore.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIfIplIsRequiredReducedBB(const CVector2D& posn, IplDef* def) | [source/game_sa/IplStore.cpp:106](../../source/game_sa/IplStore.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_currentIPLAreaCode: source/game_sa/IplStore.cpp:36 (0x008E3EF8) |
| SetIfInteriorIplIsRequired(const CVector2D& posn, IplDef* def) | [source/game_sa/IplStore.cpp:123](../../source/game_sa/IplStore.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_currentIPLAreaCode: source/game_sa/IplStore.cpp:36 (0x008E3EF8) |
| ResolveAreaCode(int32 ec) | [source/game_sa/IplStore.cpp:169](../../source/game_sa/IplStore.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |
| IsIPLAnInterior(const char* iplFileName) | [source/game_sa/IplStore.cpp:764](../../source/game_sa/IplStore.cpp#L764) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExtractIPLNameFromPath(const char* iplFilePath, char(&out)\[N\]) | [source/game_sa/IplStore.cpp:786](../../source/game_sa/IplStore.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1290"></a>

## Globales y helpers: source/game_sa/IplStore.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetLoadedBuildings() | [source/game_sa/IplStore.h:67](../../source/game_sa/IplStore.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNumLoadedBuildings: source/game_sa/IplStore.h:64 (0x00BCC0D8); Posible dato fijo gpLoadedBuildings: source/game_sa/IplStore.h:65 (0x00BCC0E0) |

<a id="grupo-1291"></a>

## Globales y helpers: source/game_sa/LoadingScreen.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClockSeconds() | [source/game_sa/LoadingScreen.cpp:62](../../source/game_sa/LoadingScreen.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LoadingStretchX(float a) | [source/game_sa/LoadingScreen.cpp:70](../../source/game_sa/LoadingScreen.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| LoadingStretchY(float a) | [source/game_sa/LoadingScreen.cpp:71](../../source/game_sa/LoadingScreen.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1292"></a>

## Globales y helpers: source/game_sa/Memory/MemoryHeap.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetCommonSize(uint32 size) | [source/game_sa/Memory/MemoryHeap.cpp:12](../../source/game_sa/Memory/MemoryHeap.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:16](../../source/game_sa/Memory/MemoryHeap.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFreeBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:21](../../source/game_sa/Memory/MemoryHeap.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1293"></a>

## Globales y helpers: source/game_sa/MemoryMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __cdecl Memory9NativeMalloc(uint32) | [source/game_sa/MemoryMgr.cpp:264](../../source/game_sa/MemoryMgr.cpp#L264) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00824257; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| * __cdecl Memory9NativeRealloc(void*, uint32) | [source/game_sa/MemoryMgr.cpp:270](../../source/game_sa/MemoryMgr.cpp#L270) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x00824269; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Memory9NativeFree(void*) | [source/game_sa/MemoryMgr.cpp:276](../../source/game_sa/MemoryMgr.cpp#L276) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x0082413F; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| * operator new\[\](uint32 size) | [source/game_sa/MemoryMgr.cpp:534](../../source/game_sa/MemoryMgr.cpp#L534) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete\[\](void* memory) | [source/game_sa/MemoryMgr.cpp:543](../../source/game_sa/MemoryMgr.cpp#L543) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(uint32 size) | [source/game_sa/MemoryMgr.cpp:554](../../source/game_sa/MemoryMgr.cpp#L554) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete(void* memory) | [source/game_sa/MemoryMgr.cpp:562](../../source/game_sa/MemoryMgr.cpp#L562) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(size_t size) | [source/game_sa/MemoryMgr.cpp:576](../../source/game_sa/MemoryMgr.cpp#L576) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0082119A: fuera del catálogo consultado; ruta efectiva sin resolver |
| * operator new\[\](size_t size) | [source/game_sa/MemoryMgr.cpp:580](../../source/game_sa/MemoryMgr.cpp#L580) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00821195: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete(void* p) | [source/game_sa/MemoryMgr.cpp:584](../../source/game_sa/MemoryMgr.cpp#L584) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008214BD: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete\[\](void* p) | [source/game_sa/MemoryMgr.cpp:588](../../source/game_sa/MemoryMgr.cpp#L588) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008213AE: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1294"></a>

## Globales y helpers: source/game_sa/MenuManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMenu() | [source/game_sa/MenuManager.cpp:41](../../source/game_sa/MenuManager.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:39 (0x00BA6748) |

<a id="grupo-1295"></a>

## Globales y helpers: source/game_sa/MenuSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MenuStretchX(float x) | [source/game_sa/MenuSystem.cpp:279](../../source/game_sa/MenuSystem.cpp#L279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuStretchY(float y) | [source/game_sa/MenuSystem.cpp:280](../../source/game_sa/MenuSystem.cpp#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuSumColumnWidths(const CMenuSystem::Menu* menu, int32 count) | [source/game_sa/MenuSystem.cpp:283](../../source/game_sa/MenuSystem.cpp#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1296"></a>

## Globales y helpers: source/game_sa/Messages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearMessagesTextAndString(std::array&lt;tMessage, N&gt;& msgs) | [source/game_sa/Messages.cpp:64](../../source/game_sa/Messages.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveMessageAt(std::array&lt;tMessage, N&gt;& msgs, int16 i) | [source/game_sa/Messages.cpp:75](../../source/game_sa/Messages.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MessageTextEquals(const GxtChar* a, const GxtChar* b) | [source/game_sa/Messages.cpp:91](../../source/game_sa/Messages.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindUnusedMsgInArray(std::array&lt;tMessage, N&gt;& arr) | [source/game_sa/Messages.cpp:113](../../source/game_sa/Messages.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1297"></a>

## Globales y helpers: source/game_sa/ModelIndices.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatchModelStringUint16(char* name, ModelIndex) | [source/game_sa/ModelIndices.cpp:502](../../source/game_sa/ModelIndices.cpp#L502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TestModelIndices() | [source/game_sa/ModelIndices.cpp:507](../../source/game_sa/ModelIndices.cpp#L507) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1298"></a>

## Globales y helpers: source/game_sa/Models/AtomicModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetAtomicModelInfoFlags(CAtomicModelInfo* modelInfo, uint32 dwFlags) | [source/game_sa/Models/AtomicModelInfo.cpp:249](../../source/game_sa/Models/AtomicModelInfo.cpp#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1299"></a>

## Globales y helpers: source/game_sa/Models/AtomicModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAtomicModelInfo | [source/game_sa/Models/AtomicModelInfo.h:22](../../source/game_sa/Models/AtomicModelInfo.h#L22) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetRwModelType() const override | [source/game_sa/Models/AtomicModelInfo.h:34](../../source/game_sa/Models/AtomicModelInfo.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1300"></a>

## Globales y helpers: source/game_sa/Models/BaseModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetBaseModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/BaseModelInfo.cpp:38](../../source/game_sa/Models/BaseModelInfo.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
