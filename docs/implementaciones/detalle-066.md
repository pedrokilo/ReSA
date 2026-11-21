# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1301"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetNameOfSavedGame(int32 slot) | [source/game_sa/GenericGameStorage.h:114](../../source/game_sa/GenericGameStorage.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_SlotFileName: source/game_sa/GenericGameStorage.h:73 (0x00C16368) |
| GetSavedGameState(int32 slot) | [source/game_sa/GenericGameStorage.h:120](../../source/game_sa/GenericGameStorage.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_Slots: source/game_sa/GenericGameStorage.h:74 (0x00C16EBC) |

<a id="grupo-1302"></a>

## Globales y helpers: source/game_sa/Glass.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:52](../../source/game_sa/Glass.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyPoint_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:61](../../source/game_sa/Glass.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindMinMaxZOfVertices(const CVector (&v)\[4\], float& minZ, float& maxZ) | [source/game_sa/Glass.cpp:70](../../source/game_sa/Glass.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindVertexFurthestFromV0(const CVector (&v)\[4\]) | [source/game_sa/Glass.cpp:81](../../source/game_sa/Glass.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlassObjectWithCol(CEntity* entity) | [source/game_sa/Glass.cpp:258](../../source/game_sa/Glass.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1303"></a>

## Globales y helpers: source/game_sa/GrassRenderer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GrassDistance(const CVector& a, const CVector& b) | [source/game_sa/GrassRenderer.cpp:69](../../source/game_sa/GrassRenderer.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1304"></a>

## Globales y helpers: source/game_sa/GxtChar.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char* out, const GxtChar* src, size_t size, size_t offset) | [source/game_sa/GxtChar.cpp:46](../../source/game_sa/GxtChar.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeLowerCase(char* str) | [source/game_sa/GxtChar.cpp:108](../../source/game_sa/GxtChar.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1305"></a>

## Globales y helpers: source/game_sa/GxtChar.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToUTF8(char (&out)\[N\], const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:33](../../source/game_sa/GxtChar.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharToUTF8(const GxtChar* src, size_t offset = 0) | [source/game_sa/GxtChar.h:37](../../source/game_sa/GxtChar.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GxtCharFromAscii(const char* asciiText) | [source/game_sa/GxtChar.h:92](../../source/game_sa/GxtChar.h#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsciiFromGxtChar(const GxtChar* gxtAsciiText) | [source/game_sa/GxtChar.h:105](../../source/game_sa/GxtChar.h#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator""_gxt(const char* text, size_t) | [source/game_sa/GxtChar.h:109](../../source/game_sa/GxtChar.h#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1306"></a>

## Globales y helpers: source/game_sa/Hud.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeFloat(uintptr_t address) | [source/game_sa/Hud.cpp:43](../../source/game_sa/Hud.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StretchBaseX() | [source/game_sa/Hud.cpp:47](../../source/game_sa/Hud.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| StretchBaseY() | [source/game_sa/Hud.cpp:48](../../source/game_sa/Hud.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| HudRawRect(float left, float bottom, float right, float top) | [source/game_sa/Hud.cpp:1214](../../source/game_sa/Hud.cpp#L1214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CHud::DrawRadar 0x0058A330 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleScaleY(int height, bool widescreen) | [source/game_sa/Hud.cpp:1382](../../source/game_sa/Hud.cpp#L1382) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleCentreVitals(int width) | [source/game_sa/Hud.cpp:1386](../../source/game_sa/Hud.cpp#L1386) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtitleScaleX(int width, bool vitals) | [source/game_sa/Hud.cpp:1390](../../source/game_sa/Hud.cpp#L1390) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1307"></a>

## Globales y helpers: source/game_sa/IdleCam.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IdleMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/IdleCam.cpp:31](../../source/game_sa/IdleCam.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1308"></a>

## Globales y helpers: source/game_sa/InterestingEvents.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InterestingSector(float coordinate, float radius, bool useEnd) | [source/game_sa/InterestingEvents.cpp:164](../../source/game_sa/InterestingEvents.cpp#L164) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1309"></a>

## Globales y helpers: source/game_sa/Interior/FurnitureSubGroup_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall FurnitureSubGroupListItemConstructorExe(ListItem_c&lt;FurnitureSubGroup_c&gt;* self, void* /*edx*/) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:27](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: ListItem_c::Constructor 0x004A8DB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SelectFurnitureByWealth(TList_c&lt;Furniture_c&gt;& list, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:57](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1310"></a>

## Globales y helpers: source/game_sa/Interior/InteriorManager_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Interior/InteriorManager_c.cpp:85](../../source/game_sa/Interior/InteriorManager_c.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ModelInfoOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:94](../../source/game_sa/Interior/InteriorManager_c.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| AreaOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:99](../../source/game_sa/Interior/InteriorManager_c.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1311"></a>

## Globales y helpers: source/game_sa/Interior/Interior_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & InteriorField(const void* base, ptrdiff_t offset) | [source/game_sa/Interior/Interior_c.cpp:44](../../source/game_sa/Interior/Interior_c.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & InteriorField(uintptr_t base, ptrdiff_t offset) | [source/game_sa/Interior/Interior_c.cpp:48](../../source/game_sa/Interior/Interior_c.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorFloatBits(float value) | [source/game_sa/Interior/Interior_c.cpp:52](../../source/game_sa/Interior/Interior_c.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorBitsFloat(uint32 bits) | [source/game_sa/Interior/Interior_c.cpp:57](../../source/game_sa/Interior/Interior_c.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorRandCpp() | [source/game_sa/Interior/Interior_c.cpp:63](../../source/game_sa/Interior/Interior_c.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorRandomCoordinate(int32 extent) | [source/game_sa/Interior/Interior_c.cpp:69](../../source/game_sa/Interior/Interior_c.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorLocalTileCentre(int32 negativeWidth, int32 negativeHeight, int32 negativeDepth, float offsetX, float offsetY, CVector* output) | [source/game_sa/Interior/Interior_c.cpp:75](../../source/game_sa/Interior/Interior_c.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorProjectionInside(const RwV3d* axis, const CVector* delta, int32 dimension, float bias) | [source/game_sa/Interior/Interior_c.cpp:85](../../source/game_sa/Interior/Interior_c.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorTruncate(float value) | [source/game_sa/Interior/Interior_c.cpp:95](../../source/game_sa/Interior/Interior_c.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorPositionSeed(const CVector* position, int32 extra) | [source/game_sa/Interior/Interior_c.cpp:99](../../source/game_sa/Interior/Interior_c.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorNearEntrance(const CVector* camera, const CVector* centre) | [source/game_sa/Interior/Interior_c.cpp:103](../../source/game_sa/Interior/Interior_c.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorObjectZExact(float offsetZ,float boundMinZ,int32 negativeHeight) | [source/game_sa/Interior/Interior_c.cpp:2361](../../source/game_sa/Interior/Interior_c.cpp#L2361) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Interior_c::PlaceObject 0x005934E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InteriorFurniturePositionExact(CVector* position,int32 negativeWidth,int32 negativeDepth,int32 boxHeight,int32 tileX,int32 tileY,int32 shiftX,int32 shiftY,float height,bool ceiling) | [source/game_sa/Interior/Interior_c.cpp:2366](../../source/game_sa/Interior/Interior_c.cpp#L2366) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1312"></a>

## Globales y helpers: source/game_sa/IplStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolveAreaCode(int32 ec) | [source/game_sa/IplStore.cpp:169](../../source/game_sa/IplStore.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |
| IsIPLAnInterior(const char* iplFileName) | [source/game_sa/IplStore.cpp:764](../../source/game_sa/IplStore.cpp#L764) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExtractIPLNameFromPath(const char* iplFilePath, char(&out)\[N\]) | [source/game_sa/IplStore.cpp:786](../../source/game_sa/IplStore.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1313"></a>

## Globales y helpers: source/game_sa/IplStore.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetLoadedBuildings() | [source/game_sa/IplStore.h:67](../../source/game_sa/IplStore.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNumLoadedBuildings: source/game_sa/IplStore.h:64 (0x00BCC0D8); Posible dato fijo gpLoadedBuildings: source/game_sa/IplStore.h:65 (0x00BCC0E0) |

<a id="grupo-1314"></a>

## Globales y helpers: source/game_sa/LoadingScreen.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClockSeconds() | [source/game_sa/LoadingScreen.cpp:62](../../source/game_sa/LoadingScreen.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LoadingStretchX(float a) | [source/game_sa/LoadingScreen.cpp:70](../../source/game_sa/LoadingScreen.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| LoadingStretchY(float a) | [source/game_sa/LoadingScreen.cpp:71](../../source/game_sa/LoadingScreen.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1315"></a>

## Globales y helpers: source/game_sa/Memory/MemoryHeap.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetCommonSize(uint32 size) | [source/game_sa/Memory/MemoryHeap.cpp:30](../../source/game_sa/Memory/MemoryHeap.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:34](../../source/game_sa/Memory/MemoryHeap.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFreeBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:39](../../source/game_sa/Memory/MemoryHeap.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1316"></a>

## Globales y helpers: source/game_sa/MemoryMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * operator new\[\](uint32 size) | [source/game_sa/MemoryMgr.cpp:514](../../source/game_sa/MemoryMgr.cpp#L514) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete\[\](void* memory) | [source/game_sa/MemoryMgr.cpp:523](../../source/game_sa/MemoryMgr.cpp#L523) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(uint32 size) | [source/game_sa/MemoryMgr.cpp:534](../../source/game_sa/MemoryMgr.cpp#L534) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete(void* memory) | [source/game_sa/MemoryMgr.cpp:542](../../source/game_sa/MemoryMgr.cpp#L542) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(size_t size) | [source/game_sa/MemoryMgr.cpp:556](../../source/game_sa/MemoryMgr.cpp#L556) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0082119A: fuera del catálogo consultado; ruta efectiva sin resolver |
| * operator new\[\](size_t size) | [source/game_sa/MemoryMgr.cpp:560](../../source/game_sa/MemoryMgr.cpp#L560) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00821195: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete(void* p) | [source/game_sa/MemoryMgr.cpp:564](../../source/game_sa/MemoryMgr.cpp#L564) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008214BD: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete\[\](void* p) | [source/game_sa/MemoryMgr.cpp:568](../../source/game_sa/MemoryMgr.cpp#L568) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008213AE: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1317"></a>

## Globales y helpers: source/game_sa/MentalHealth.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HealthByte(float value) | [source/game_sa/MentalHealth.cpp:51](../../source/game_sa/MentalHealth.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1318"></a>

## Globales y helpers: source/game_sa/MenuManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMenu() | [source/game_sa/MenuManager.cpp:43](../../source/game_sa/MenuManager.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:41 (0x00BA6748) |

<a id="grupo-1319"></a>

## Globales y helpers: source/game_sa/MenuSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MenuStretchX(float x) | [source/game_sa/MenuSystem.cpp:279](../../source/game_sa/MenuSystem.cpp#L279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuStretchY(float y) | [source/game_sa/MenuSystem.cpp:280](../../source/game_sa/MenuSystem.cpp#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuSumColumnWidths(const CMenuSystem::Menu* menu, int32 count) | [source/game_sa/MenuSystem.cpp:283](../../source/game_sa/MenuSystem.cpp#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1320"></a>

## Globales y helpers: source/game_sa/Messages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearMessagesTextAndString(std::array&lt;tMessage, N&gt;& msgs) | [source/game_sa/Messages.cpp:64](../../source/game_sa/Messages.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveMessageAt(std::array&lt;tMessage, N&gt;& msgs, int16 i) | [source/game_sa/Messages.cpp:75](../../source/game_sa/Messages.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MessageTextEquals(const GxtChar* a, const GxtChar* b) | [source/game_sa/Messages.cpp:91](../../source/game_sa/Messages.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindUnusedMsgInArray(std::array&lt;tMessage, N&gt;& arr) | [source/game_sa/Messages.cpp:113](../../source/game_sa/Messages.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
