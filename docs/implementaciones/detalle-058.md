<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1141"></a>

## Globales y helpers: source/game_sa/IdleCam.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IdleMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/IdleCam.cpp:31](../../source/game_sa/IdleCam.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1142"></a>

## Globales y helpers: source/game_sa/InterestingEvents.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InterestingSector(float coordinate, float radius, bool useEnd) | [source/game_sa/InterestingEvents.cpp:162](../../source/game_sa/InterestingEvents.cpp#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1143"></a>

## Globales y helpers: source/game_sa/Interior/FurnitureSubGroup_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * SelectFurnitureByWealth(TList_c&lt;Furniture_c&gt;& list, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:42](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1144"></a>

## Globales y helpers: source/game_sa/Interior/InteriorManager_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Interior/InteriorManager_c.cpp:58](../../source/game_sa/Interior/InteriorManager_c.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ModelInfoOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:67](../../source/game_sa/Interior/InteriorManager_c.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_modelInfoPtrs: source/game_sa/Models/ModelInfo.h:30 (0x00A9B0C8) |
| AreaOf(const CEntity* entity) | [source/game_sa/Interior/InteriorManager_c.cpp:72](../../source/game_sa/Interior/InteriorManager_c.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1145"></a>

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

<a id="grupo-1146"></a>

## Globales y helpers: source/game_sa/IplStore.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolveAreaCode(int32 ec) | [source/game_sa/IplStore.cpp:169](../../source/game_sa/IplStore.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |
| IsIPLAnInterior(const char* iplFileName) | [source/game_sa/IplStore.cpp:764](../../source/game_sa/IplStore.cpp#L764) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExtractIPLNameFromPath(const char* iplFilePath, char(&out)\[N\]) | [source/game_sa/IplStore.cpp:786](../../source/game_sa/IplStore.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1147"></a>

## Globales y helpers: source/game_sa/IplStore.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetLoadedBuildings() | [source/game_sa/IplStore.h:67](../../source/game_sa/IplStore.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gNumLoadedBuildings: source/game_sa/IplStore.h:64 (0x00BCC0D8); Posible dato fijo gpLoadedBuildings: source/game_sa/IplStore.h:65 (0x00BCC0E0) |

<a id="grupo-1148"></a>

## Globales y helpers: source/game_sa/LoadingScreen.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClockSeconds() | [source/game_sa/LoadingScreen.cpp:62](../../source/game_sa/LoadingScreen.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LoadingStretchX(float a) | [source/game_sa/LoadingScreen.cpp:70](../../source/game_sa/LoadingScreen.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| LoadingStretchY(float a) | [source/game_sa/LoadingScreen.cpp:71](../../source/game_sa/LoadingScreen.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1149"></a>

## Globales y helpers: source/game_sa/Memory/MemoryHeap.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetCommonSize(uint32 size) | [source/game_sa/Memory/MemoryHeap.cpp:30](../../source/game_sa/Memory/MemoryHeap.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:34](../../source/game_sa/Memory/MemoryHeap.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFreeBlockDesc(void* mem) | [source/game_sa/Memory/MemoryHeap.cpp:39](../../source/game_sa/Memory/MemoryHeap.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1150"></a>

## Globales y helpers: source/game_sa/MemoryMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * operator new\[\](uint32 size) | [source/game_sa/MemoryMgr.cpp:510](../../source/game_sa/MemoryMgr.cpp#L510) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete\[\](void* memory) | [source/game_sa/MemoryMgr.cpp:519](../../source/game_sa/MemoryMgr.cpp#L519) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(uint32 size) | [source/game_sa/MemoryMgr.cpp:530](../../source/game_sa/MemoryMgr.cpp#L530) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator delete(void* memory) | [source/game_sa/MemoryMgr.cpp:538](../../source/game_sa/MemoryMgr.cpp#L538) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * operator new(size_t size) | [source/game_sa/MemoryMgr.cpp:552](../../source/game_sa/MemoryMgr.cpp#L552) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x0082119A: fuera del catálogo consultado; ruta efectiva sin resolver |
| * operator new\[\](size_t size) | [source/game_sa/MemoryMgr.cpp:556](../../source/game_sa/MemoryMgr.cpp#L556) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00821195: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete(void* p) | [source/game_sa/MemoryMgr.cpp:560](../../source/game_sa/MemoryMgr.cpp#L560) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008214BD: fuera del catálogo consultado; ruta efectiva sin resolver |
| operator delete\[\](void* p) | [source/game_sa/MemoryMgr.cpp:564](../../source/game_sa/MemoryMgr.cpp#L564) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x008213AE: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1151"></a>

## Globales y helpers: source/game_sa/MentalHealth.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HealthByte(float value) | [source/game_sa/MentalHealth.cpp:51](../../source/game_sa/MentalHealth.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1152"></a>

## Globales y helpers: source/game_sa/MenuManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetMenu() | [source/game_sa/MenuManager.cpp:42](../../source/game_sa/MenuManager.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |

<a id="grupo-1153"></a>

## Globales y helpers: source/game_sa/MenuSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MenuStretchX(float x) | [source/game_sa/MenuSystem.cpp:279](../../source/game_sa/MenuSystem.cpp#L279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuStretchY(float y) | [source/game_sa/MenuSystem.cpp:280](../../source/game_sa/MenuSystem.cpp#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| MenuSumColumnWidths(const CMenuSystem::Menu* menu, int32 count) | [source/game_sa/MenuSystem.cpp:283](../../source/game_sa/MenuSystem.cpp#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1154"></a>

## Globales y helpers: source/game_sa/Messages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearMessagesTextAndString(std::array&lt;tMessage, N&gt;& msgs) | [source/game_sa/Messages.cpp:64](../../source/game_sa/Messages.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RemoveMessageAt(std::array&lt;tMessage, N&gt;& msgs, int16 i) | [source/game_sa/Messages.cpp:75](../../source/game_sa/Messages.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MessageTextEquals(const GxtChar* a, const GxtChar* b) | [source/game_sa/Messages.cpp:91](../../source/game_sa/Messages.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FindUnusedMsgInArray(std::array&lt;tMessage, N&gt;& arr) | [source/game_sa/Messages.cpp:113](../../source/game_sa/Messages.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1155"></a>

## Globales y helpers: source/game_sa/MissionCleanup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & Raw(uintptr_t address) | [source/game_sa/MissionCleanup.cpp:83](../../source/game_sa/MissionCleanup.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & RawAt(void* base, uintptr_t offset) | [source/game_sa/MissionCleanup.cpp:84](../../source/game_sa/MissionCleanup.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PoolEntity(uintptr_t poolGlobal, int32 handle, int32 entitySize) | [source/game_sa/MissionCleanup.cpp:88](../../source/game_sa/MissionCleanup.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1156"></a>

## Globales y helpers: source/game_sa/ModelIndices.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatchModelStringUint16(char* name, ModelIndex) | [source/game_sa/ModelIndices.cpp:502](../../source/game_sa/ModelIndices.cpp#L502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TestModelIndices() | [source/game_sa/ModelIndices.cpp:507](../../source/game_sa/ModelIndices.cpp#L507) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1157"></a>

## Globales y helpers: source/game_sa/Models/BaseModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetBaseModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/BaseModelInfo.cpp:38](../../source/game_sa/Models/BaseModelInfo.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1158"></a>

## Globales y helpers: source/game_sa/Models/ClumpModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetClumpModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) | [source/game_sa/Models/ClumpModelInfo.cpp:41](../../source/game_sa/Models/ClumpModelInfo.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetClumpModelInfoFlags(CClumpModelInfo* modelInfo, uint32 dwFlags) | [source/game_sa/Models/ClumpModelInfo.cpp:398](../../source/game_sa/Models/ClumpModelInfo.cpp#L398) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1159"></a>

## Globales y helpers: source/game_sa/Models/VehicleModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChooseComponent(int32 rule, int32 comps) | [source/game_sa/Models/VehicleModelInfo.cpp:1697](../../source/game_sa/Models/VehicleModelInfo.cpp#L1697) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicleModelInfo::ChooseComponent-Global 0x004C7FB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1160"></a>

## Globales y helpers: source/game_sa/Occluder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MultiplyMatrixWithVector_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Occluder.cpp:36](../../source/game_sa/Occluder.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcScreenCoorsWithScale(const CVector& in, CVector& out) | [source/game_sa/Occluder.cpp:45](../../source/game_sa/Occluder.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcClippedScreenCoors(int32 idxFrom, int32 idxTo, CVector& out) | [source/game_sa/Occluder.cpp:52](../../source/game_sa/Occluder.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo OccluderCoors: source/game_sa/Occluder.h:17 (0x00C798E0); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| RawAngle(const COccluder* o, size_t offset) | [source/game_sa/Occluder.cpp:65](../../source/game_sa/Occluder.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
