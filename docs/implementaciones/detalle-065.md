<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1281"></a>

## Globales y helpers: source/game_sa/IdleCam.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IdleMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/IdleCam.cpp:31](../../source/game_sa/IdleCam.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1282"></a>

## Globales y helpers: source/game_sa/InterestingEvents.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InterestingSector(float coordinate, float radius, bool useEnd) | [source/game_sa/InterestingEvents.cpp:162](../../source/game_sa/InterestingEvents.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1283"></a>

## Globales y helpers: source/game_sa/Interior/FurnitureSubGroup_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * SelectFurnitureByWealth(TList_c&lt;Furniture_c&gt;& list, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:42](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1284"></a>

## Globales y helpers: source/game_sa/Interior/InteriorManager_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Interior/InteriorManager_c.cpp:58](../../source/game_sa/Interior/InteriorManager_c.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ModelInfoOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:67](../../source/game_sa/Interior/InteriorManager_c.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| AreaOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:72](../../source/game_sa/Interior/InteriorManager_c.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1285"></a>

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

<a id="grupo-1286"></a>

## Globales y helpers: source/game_sa/IplStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetIfIplIsRequired(const CVector2D& posn, IplDef* def) | [source/game_sa/IplStore.cpp:96](../../source/game_sa/IplStore.cpp#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIfIplIsRequiredReducedBB(const CVector2D& posn, IplDef* def) | [source/game_sa/IplStore.cpp:106](../../source/game_sa/IplStore.cpp#L106) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_currentIPLAreaCode: source/game_sa/IplStore.cpp:36 (0x008E3EF8) |
| SetIfInteriorIplIsRequired(const CVector2D& posn, IplDef* def) | [source/game_sa/IplStore.cpp:123](../../source/game_sa/IplStore.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_currentIPLAreaCode: source/game_sa/IplStore.cpp:36 (0x008E3EF8) |
| ResolveAreaCode(int32 ec) | [source/game_sa/IplStore.cpp:169](../../source/game_sa/IplStore.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |
| IsIPLAnInterior(const char* iplFileName) | [source/game_sa/IplStore.cpp:764](../../source/game_sa/IplStore.cpp#L764) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExtractIPLNameFromPath(const char* iplFilePath, char(&out)\[N\]) | [source/game_sa/IplStore.cpp:786](../../source/game_sa/IplStore.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1287"></a>

## Globales y helpers: source/game_sa/IplStore.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetLoadedBuildings() | [source/game_sa/IplStore.h:67](../../source/game_sa/IplStore.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNumLoadedBuildings: source/game_sa/IplStore.h:64 (0x00BCC0D8); Posible dato fijo gpLoadedBuildings: source/game_sa/IplStore.h:65 (0x00BCC0E0) |

<a id="grupo-1288"></a>

## Globales y helpers: source/game_sa/LoadingScreen.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClockSeconds() | [source/game_sa/LoadingScreen.cpp:62](../../source/game_sa/LoadingScreen.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LoadingStretchX(float a) | [source/game_sa/LoadingScreen.cpp:70](../../source/game_sa/LoadingScreen.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| LoadingStretchY(float a) | [source/game_sa/LoadingScreen.cpp:71](../../source/game_sa/LoadingScreen.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1289"></a>

## Globales y helpers: source/game_sa/Memory/MemoryHeap.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetCommonSize(uint32 size) | [source/game_sa/Memory/MemoryHeap.cpp:12](../../source/game_sa/Memory/MemoryHeap.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:16](../../source/game_sa/Memory/MemoryHeap.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFreeBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:21](../../source/game_sa/Memory/MemoryHeap.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1290"></a>

## Globales y helpers: source/game_sa/MemoryMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * operator new\[\](uint32 size) | [source/game_sa/MemoryMgr.cpp:492](../../source/game_sa/MemoryMgr.cpp#L492) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete\[\](void* memory) | [source/game_sa/MemoryMgr.cpp:501](../../source/game_sa/MemoryMgr.cpp#L501) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(uint32 size) | [source/game_sa/MemoryMgr.cpp:512](../../source/game_sa/MemoryMgr.cpp#L512) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete(void* memory) | [source/game_sa/MemoryMgr.cpp:520](../../source/game_sa/MemoryMgr.cpp#L520) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(size_t size) | [source/game_sa/MemoryMgr.cpp:534](../../source/game_sa/MemoryMgr.cpp#L534) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0082119A: fuera del catálogo consultado; ruta efectiva sin resolver |
| * operator new\[\](size_t size) | [source/game_sa/MemoryMgr.cpp:538](../../source/game_sa/MemoryMgr.cpp#L538) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00821195: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete(void* p) | [source/game_sa/MemoryMgr.cpp:542](../../source/game_sa/MemoryMgr.cpp#L542) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008214BD: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete\[\](void* p) | [source/game_sa/MemoryMgr.cpp:546](../../source/game_sa/MemoryMgr.cpp#L546) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008213AE: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1291"></a>

## Globales y helpers: source/game_sa/MenuManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMenu() | [source/game_sa/MenuManager.cpp:42](../../source/game_sa/MenuManager.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |

<a id="grupo-1292"></a>

## Globales y helpers: source/game_sa/MenuSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MenuStretchX(float x) | [source/game_sa/MenuSystem.cpp:279](../../source/game_sa/MenuSystem.cpp#L279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuStretchY(float y) | [source/game_sa/MenuSystem.cpp:280](../../source/game_sa/MenuSystem.cpp#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuSumColumnWidths(const CMenuSystem::Menu* menu, int32 count) | [source/game_sa/MenuSystem.cpp:283](../../source/game_sa/MenuSystem.cpp#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1293"></a>

## Globales y helpers: source/game_sa/Messages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearMessagesTextAndString(std::array&lt;tMessage, N&gt;& msgs) | [source/game_sa/Messages.cpp:64](../../source/game_sa/Messages.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveMessageAt(std::array&lt;tMessage, N&gt;& msgs, int16 i) | [source/game_sa/Messages.cpp:75](../../source/game_sa/Messages.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MessageTextEquals(const GxtChar* a, const GxtChar* b) | [source/game_sa/Messages.cpp:91](../../source/game_sa/Messages.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindUnusedMsgInArray(std::array&lt;tMessage, N&gt;& arr) | [source/game_sa/Messages.cpp:113](../../source/game_sa/Messages.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1294"></a>

## Globales y helpers: source/game_sa/ModelIndices.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatchModelStringUint16(char* name, ModelIndex) | [source/game_sa/ModelIndices.cpp:502](../../source/game_sa/ModelIndices.cpp#L502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TestModelIndices() | [source/game_sa/ModelIndices.cpp:507](../../source/game_sa/ModelIndices.cpp#L507) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1295"></a>

## Globales y helpers: source/game_sa/Models/AtomicModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetAtomicModelInfoFlags(CAtomicModelInfo* modelInfo, uint32 dwFlags) | [source/game_sa/Models/AtomicModelInfo.cpp:249](../../source/game_sa/Models/AtomicModelInfo.cpp#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1296"></a>

## Globales y helpers: source/game_sa/Models/AtomicModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAtomicModelInfo | [source/game_sa/Models/AtomicModelInfo.h:22](../../source/game_sa/Models/AtomicModelInfo.h#L22) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetRwModelType() const override | [source/game_sa/Models/AtomicModelInfo.h:34](../../source/game_sa/Models/AtomicModelInfo.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1297"></a>

## Globales y helpers: source/game_sa/Models/BaseModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetBaseModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/BaseModelInfo.cpp:38](../../source/game_sa/Models/BaseModelInfo.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1298"></a>

## Globales y helpers: source/game_sa/Models/BaseModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CBaseModelInfo | [source/game_sa/Models/BaseModelInfo.h:88](../../source/game_sa/Models/BaseModelInfo.h#L88) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~CBaseModelInfo() | [source/game_sa/Models/BaseModelInfo.h:149](../../source/game_sa/Models/BaseModelInfo.h#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Get2dEffects() | [source/game_sa/Models/BaseModelInfo.h:179](../../source/game_sa/Models/BaseModelInfo.h#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsVehicleModelInfoPtr() | [source/game_sa/Models/BaseModelInfo.h:187](../../source/game_sa/Models/BaseModelInfo.h#L187) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsPedModelInfoPtr() | [source/game_sa/Models/BaseModelInfo.h:188](../../source/game_sa/Models/BaseModelInfo.h#L188) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AsWeaponModelInfoPtr() | [source/game_sa/Models/BaseModelInfo.h:189](../../source/game_sa/Models/BaseModelInfo.h#L189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetColModel() const | [source/game_sa/Models/BaseModelInfo.h:191](../../source/game_sa/Models/BaseModelInfo.h#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetIsDrawLast() const | [source/game_sa/Models/BaseModelInfo.h:195](../../source/game_sa/Models/BaseModelInfo.h#L195) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasBeenPreRendered() const | [source/game_sa/Models/BaseModelInfo.h:196](../../source/game_sa/Models/BaseModelInfo.h#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasComplexHierarchy() const | [source/game_sa/Models/BaseModelInfo.h:197](../../source/game_sa/Models/BaseModelInfo.h#L197) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBackfaceCulled() const | [source/game_sa/Models/BaseModelInfo.h:198](../../source/game_sa/Models/BaseModelInfo.h#L198) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsLod() const | [source/game_sa/Models/BaseModelInfo.h:199](../../source/game_sa/Models/BaseModelInfo.h#L199) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRoad() const | [source/game_sa/Models/BaseModelInfo.h:200](../../source/game_sa/Models/BaseModelInfo.h#L200) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetHasBeenPreRendered(int32 bPreRendered) | [source/game_sa/Models/BaseModelInfo.h:202](../../source/game_sa/Models/BaseModelInfo.h#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetIsLod(int32 bLod) | [source/game_sa/Models/BaseModelInfo.h:203](../../source/game_sa/Models/BaseModelInfo.h#L203) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetOwnsColModel(int32 bOwns) | [source/game_sa/Models/BaseModelInfo.h:204](../../source/game_sa/Models/BaseModelInfo.h#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IncreaseAlpha() | [source/game_sa/Models/BaseModelInfo.h:205](../../source/game_sa/Models/BaseModelInfo.h#L205) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelName() const noexcept | [source/game_sa/Models/BaseModelInfo.h:211](../../source/game_sa/Models/BaseModelInfo.h#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetModelName(const char* modelName) | [source/game_sa/Models/BaseModelInfo.h:212](../../source/game_sa/Models/BaseModelInfo.h#L212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetModelNameAsString() | [source/game_sa/Models/BaseModelInfo.h:222](../../source/game_sa/Models/BaseModelInfo.h#L222) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSwayInWind1() const | [source/game_sa/Models/BaseModelInfo.h:230](../../source/game_sa/Models/BaseModelInfo.h#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSwayInWind2() const | [source/game_sa/Models/BaseModelInfo.h:231](../../source/game_sa/Models/BaseModelInfo.h#L231) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlassType1() const | [source/game_sa/Models/BaseModelInfo.h:234](../../source/game_sa/Models/BaseModelInfo.h#L234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlassType2() const | [source/game_sa/Models/BaseModelInfo.h:235](../../source/game_sa/Models/BaseModelInfo.h#L235) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlass() const | [source/game_sa/Models/BaseModelInfo.h:236](../../source/game_sa/Models/BaseModelInfo.h#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTagModel() const | [source/game_sa/Models/BaseModelInfo.h:238](../../source/game_sa/Models/BaseModelInfo.h#L238) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGarageDoor() const | [source/game_sa/Models/BaseModelInfo.h:239](../../source/game_sa/Models/BaseModelInfo.h#L239) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBreakableStatuePart() const | [source/game_sa/Models/BaseModelInfo.h:241](../../source/game_sa/Models/BaseModelInfo.h#L241) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCrane() const | [source/game_sa/Models/BaseModelInfo.h:242](../../source/game_sa/Models/BaseModelInfo.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CreateInstanceAddRef() | [source/game_sa/Models/BaseModelInfo.h:247](../../source/game_sa/Models/BaseModelInfo.h#L247) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetRwObject() const noexcept | [source/game_sa/Models/BaseModelInfo.h:254](../../source/game_sa/Models/BaseModelInfo.h#L254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetRpClump() const noexcept | [source/game_sa/Models/BaseModelInfo.h:255](../../source/game_sa/Models/BaseModelInfo.h#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetRpAtomic() const noexcept | [source/game_sa/Models/BaseModelInfo.h:256](../../source/game_sa/Models/BaseModelInfo.h#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1299"></a>

## Globales y helpers: source/game_sa/Models/ClumpModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetClumpModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/ClumpModelInfo.cpp:41](../../source/game_sa/Models/ClumpModelInfo.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetClumpModelInfoFlags(CClumpModelInfo* modelInfo, uint32 dwFlags) | [source/game_sa/Models/ClumpModelInfo.cpp:398](../../source/game_sa/Models/ClumpModelInfo.cpp#L398) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1300"></a>

## Globales y helpers: source/game_sa/Models/ClumpModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CClumpModelInfo | [source/game_sa/Models/ClumpModelInfo.h:38](../../source/game_sa/Models/ClumpModelInfo.h#L38) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CClumpModelInfo() | [source/game_sa/Models/ClumpModelInfo.h:48](../../source/game_sa/Models/ClumpModelInfo.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetRwModelType() const override | [source/game_sa/Models/ClumpModelInfo.h:55](../../source/game_sa/Models/ClumpModelInfo.h#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
