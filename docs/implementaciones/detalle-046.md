<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-901"></a>

## CustomSpecMapPipeMaterialData

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FromBuffer(const SpecMapPipeMaterialDataBuffer& b) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:70](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ToBuffer() const | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:77](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-902"></a>

## D3DResourceSystem

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| D3DResourceSystem::InjectHooks() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:18](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| D3DResourceSystem::CancelBuffering() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:25](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L25) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730900: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::GetTotalIndexDataSize() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:30](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L30) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x007307F0: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::GetTotalPixelsSize() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:35](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L35) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730660: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::Init() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:40](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L40) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730830: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::SetUseD3DResourceBuffering(bool enabled) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:46](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: D3DResourceSystem::SetUseD3DResourceBuffering 0x00730AC0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Conversión de dirección fija: 0x008D6084 |
| D3DResourceSystem::Shutdown() | [source/game_sa/RenderWare/D3DResourceSystem.cpp:51](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L51) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730A00: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::TidyUpD3DIndexBuffers(uint32 count) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:56](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L56) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730740: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::TidyUpD3DTextures(uint32 count) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:61](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L61) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x007305E0: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:66](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L66) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x007306A0: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::CreateTexture(int32 width, int32 height, uint32 format, void** ppTexture) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:71](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L71) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730510: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::DestroyIndexBuffer(void* pIndexBuffer) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:76](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L76) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730D30: fuera del catálogo consultado; ruta efectiva sin resolver |
| D3DResourceSystem::DestroyTexture(void* texture) | [source/game_sa/RenderWare/D3DResourceSystem.cpp:81](../../source/game_sa/RenderWare/D3DResourceSystem.cpp#L81) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00730B70: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-903"></a>

## EmissionInfo_t

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Process(float mult) | [source/game_sa/Fx/EmissionInfo.h:31](../../source/game_sa/Fx/EmissionInfo.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-904"></a>

## FurnitureGroup_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FurnitureGroup_c::InjectHooks() | [source/game_sa/Interior/FurnitureGroup_c.cpp:26](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::Init() | [source/game_sa/Interior/FurnitureGroup_c.cpp:39](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::Init 0x005910A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::Exit() | [source/game_sa/Interior/FurnitureGroup_c.cpp:45](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::Exit 0x005910B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::AddSubGroup(int32 subGroupId, int32 minWidth, int32 minDepth, int32 maxWidth, int32 maxDepth, uint8 canPlaceInFrontOfWindow, bool isTall, bool canSteal) | [source/game_sa/Interior/FurnitureGroup_c.cpp:57](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::AddSubGroup 0x005910E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureGroup_c::GetFurniture(int32 subGroupId, int16 furnitureId, uint8 wealth) | [source/game_sa/Interior/FurnitureGroup_c.cpp:77](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::GetFurniture 0x00591130 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::GetRandomId(int32 subGroupId, uint8 rating) | [source/game_sa/Interior/FurnitureGroup_c.cpp:85](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::GetRandomId 0x00591170 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureGroup_c::AddFurniture(int32 subGroupId, uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) | [source/game_sa/Interior/FurnitureGroup_c.cpp:93](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureGroup_c::AddFurniture 0x005C0230 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureGroup_c::GetSubGroup(int32 subGroupId) | [source/game_sa/Interior/FurnitureGroup_c.cpp:105](../../source/game_sa/Interior/FurnitureGroup_c.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-905"></a>

## FurnitureManager_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FurnitureManager_c::InjectHooks() | [source/game_sa/Interior/FurnitureManager_c.cpp:26](../../source/game_sa/Interior/FurnitureManager_c.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::Init() | [source/game_sa/Interior/FurnitureManager_c.cpp:44](../../source/game_sa/Interior/FurnitureManager_c.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::Init 0x005C04A0 | 0 / 0 | Posible dato fijo g_currFurnitureId: source/game_sa/Interior/FurnitureManager_c.h:20 (0x00BAB378); Posible dato fijo g_currSubGroupId: source/game_sa/Interior/FurnitureManager_c.h:19 (0x00BAB37C) |
| FurnitureManager_c::Exit() | [source/game_sa/Interior/FurnitureManager_c.cpp:61](../../source/game_sa/Interior/FurnitureManager_c.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::Exit 0x005911A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureManager_c::NewSubGroup() | [source/game_sa/Interior/FurnitureManager_c.cpp:68](../../source/game_sa/Interior/FurnitureManager_c.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_currSubGroupId: source/game_sa/Interior/FurnitureManager_c.h:19 (0x00BAB37C); Posible dato fijo g_subGroupStore: source/game_sa/Interior/FurnitureManager_c.h:21 (0x00BAD3F8) |
| FurnitureManager_c::LoadFurniture() | [source/game_sa/Interior/FurnitureManager_c.cpp:82](../../source/game_sa/Interior/FurnitureManager_c.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::LoadFurniture 0x005C0280 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::GetGroupId(const char* name) | [source/game_sa/Interior/FurnitureManager_c.cpp:132](../../source/game_sa/Interior/FurnitureManager_c.cpp#L132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetGroupId 0x005BFB40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::GetSubGroupId(const char* name) | [source/game_sa/Interior/FurnitureManager_c.cpp:145](../../source/game_sa/Interior/FurnitureManager_c.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetSubGroupId 0x005BFBF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureManager_c::GetFurniture(int32 groupId, int32 subGroupId, int16 id, uint8 wealth) | [source/game_sa/Interior/FurnitureManager_c.cpp:214](../../source/game_sa/Interior/FurnitureManager_c.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetFurniture 0x005911E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureManager_c::GetRandomId(int32 groupId, int32 subGroupId, uint8 wealth) | [source/game_sa/Interior/FurnitureManager_c.cpp:220](../../source/game_sa/Interior/FurnitureManager_c.cpp#L220) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureManager_c::GetRandomId 0x00591220 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-906"></a>

## FurnitureSubGroup_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FurnitureSubGroup_c::InjectHooks() | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:26](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:61](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::AddFurniture 0x005C00C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureSubGroup_c::GetFurniture(int16 id, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:92](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::GetFurniture 0x00590EE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::GetRandomId(uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:103](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::GetRandomId 0x00590FD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::Exit() | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:108](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-907"></a>

## Fx

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrawLight(CVector pos, CVector color) | [source/game_sa/Scripts/Commands/ObjectCommands.cpp:138](../../source/game_sa/Scripts/Commands/ObjectCommands.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddBigGunFlash(CVector origin, CVector target) | [source/game_sa/Scripts/Commands/ObjectCommands.cpp:143](../../source/game_sa/Scripts/Commands/ObjectCommands.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |

<a id="grupo-908"></a>

## FxBox_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxBox_c() | [source/game_sa/Fx/FxBox.h:14](../../source/game_sa/Fx/FxBox.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reset() | [source/game_sa/Fx/FxBox.h:18](../../source/game_sa/Fx/FxBox.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator=(const RwMatrix& right) | [source/game_sa/Fx/FxBox.h:25](../../source/game_sa/Fx/FxBox.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-909"></a>

## FxBufferedMatrix

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyFromRwMatrix(const RwMatrix& mat) | [source/game_sa/Fx/FxPrimBP.h:33](../../source/game_sa/Fx/FxPrimBP.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyFromVectors(const CVector& r, const CVector& u, const CVector& a, const CVector& p) | [source/game_sa/Fx/FxPrimBP.h:40](../../source/game_sa/Fx/FxPrimBP.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyToRwMatrix(RwMatrix& out) | [source/game_sa/Fx/FxPrimBP.h:44](../../source/game_sa/Fx/FxPrimBP.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-910"></a>

## FxEmitterBP_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitterBP_c::InjectHooks() | [source/game_sa/Fx/FxEmitterBP.cpp:36](../../source/game_sa/Fx/FxEmitterBP.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::FxEmitterBP_c() | [source/game_sa/Fx/FxEmitterBP.cpp:52](../../source/game_sa/Fx/FxEmitterBP.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Constructor 0x004A18D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::RenderHeatHaze(RwCamera* camera, uint32 txdHashKey, float brightness) | [source/game_sa/Fx/FxEmitterBP.cpp:196](../../source/game_sa/Fx/FxEmitterBP.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::RenderHeatHaze 0x004A1940 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::UpdateParticle(float deltaTime, FxEmitterPrt_c* emitter) | [source/game_sa/Fx/FxEmitterBP.cpp:202](../../source/game_sa/Fx/FxEmitterBP.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::UpdateParticle 0x004A21D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxEmitterBP_c::CreateInstance() | [source/game_sa/Fx/FxEmitterBP.cpp:248](../../source/game_sa/Fx/FxEmitterBP.cpp#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::CreateInstance 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::Update(float deltaTime) | [source/game_sa/Fx/FxEmitterBP.cpp:256](../../source/game_sa/Fx/FxEmitterBP.cpp#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Update 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80) |
| FxEmitterBP_c::Load(FILESTREAM file, int32 version, FxName32_t* textureNames) | [source/game_sa/Fx/FxEmitterBP.cpp:277](../../source/game_sa/Fx/FxEmitterBP.cpp#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Load 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::LoadTextures(FxName32_t* textureNames, int32 version) | [source/game_sa/Fx/FxEmitterBP.cpp:295](../../source/game_sa/Fx/FxEmitterBP.cpp#L295) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::LoadTextures 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::Render(RwCamera* camera, uint32 txdHashKey, float brightness, bool doHeatHaze) | [source/game_sa/Fx/FxEmitterBP.cpp:327](../../source/game_sa/Fx/FxEmitterBP.cpp#L327) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Render 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80) |
| FxEmitterBP_c::FreePrtFromPrim(FxSystem_c* system) | [source/game_sa/Fx/FxEmitterBP.cpp:344](../../source/game_sa/Fx/FxEmitterBP.cpp#L344) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::FreePrtFromPrim 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80) |
| FxEmitterBP_c::IsFxInfoPresent(eFxInfoType type) const | [source/game_sa/Fx/FxEmitterBP.cpp:357](../../source/game_sa/Fx/FxEmitterBP.cpp#L357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-911"></a>

## FxEmitterPrt_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * FxEmitterPrt_c::operator new\[\](size_t size) | [source/game_sa/Fx/FxEmitterPrt.cpp:26](../../source/game_sa/Fx/FxEmitterPrt.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterPrt_c::operator new\[\] 0x004A18B0 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80) |
| FxEmitterPrt_c::InjectHooks() | [source/game_sa/Fx/FxEmitterPrt.cpp:33](../../source/game_sa/Fx/FxEmitterPrt.cpp#L33) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-912"></a>

## FxEmitter_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitter_c::InjectHooks() | [source/game_sa/Fx/FxEmitter.cpp:10](../../source/game_sa/Fx/FxEmitter.cpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Init(FxPrimBP_c* primBP, FxSystem_c* system) | [source/game_sa/Fx/FxEmitter.cpp:25](../../source/game_sa/Fx/FxEmitter.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Update(float currentTime, float deltaTime) | [source/game_sa/Fx/FxEmitter.cpp:33](../../source/game_sa/Fx/FxEmitter.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Reset() | [source/game_sa/Fx/FxEmitter.cpp:40](../../source/game_sa/Fx/FxEmitter.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:45](../../source/game_sa/Fx/FxEmitter.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A3EA0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80) |
| FxEmitter_c::AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:85](../../source/game_sa/Fx/FxEmitter.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A4050: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80) |
| FxEmitter_c::CreateParticles(float currentTime, float deltaTime) | [source/game_sa/Fx/FxEmitter.cpp:121](../../source/game_sa/Fx/FxEmitter.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A41E0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo Rain: source/game_sa/Weather.h:46 (0x00C81324); Posible dato fijo Wind: source/game_sa/Weather.h:44 (0x00C812F0); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80) |
| * FxEmitter_c::CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:177](../../source/game_sa/Fx/FxEmitter.cpp#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:36 (0x00A9AE80); Posible dato fijo m_Color: source/game_sa/Font.h:99 (0x00C71A60) |

<a id="grupo-913"></a>

## FxFrustumInfo_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxFrustumInfo_c::InjectHooks() | [source/game_sa/Fx/FxFrustumInfo.cpp:4](../../source/game_sa/Fx/FxFrustumInfo.cpp#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxFrustumInfo_c::IsCollision(FxSphere_c&) | [source/game_sa/Fx/FxFrustumInfo.cpp:12](../../source/game_sa/Fx/FxFrustumInfo.cpp#L12) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxFrustumInfo_c::IsCollision 0x004AA030 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-914"></a>

## FxInfoAnimTexture_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAnimTexture_c::FxInfoAnimTexture_c() | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAnimTexture_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAnimTexture_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-915"></a>

## FxInfoAttractLine_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractLine_c::FxInfoAttractLine_c() | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractLine_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractLine_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:29](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-916"></a>

## FxInfoAttractPt_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractPt_c::FxInfoAttractPt_c() | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractPt_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractPt_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-917"></a>

## FxInfoColourBright_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourBright_c::FxInfoColourBright_c() | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourBright_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourBright_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-918"></a>

## FxInfoColourRange_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourRange_c::FxInfoColourRange_c() | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourRange_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourRange_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-919"></a>

## FxInfoColour_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColour_c::FxInfoColour_c() | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColour_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColour_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-920"></a>

## FxInfoDir_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoDir_c::FxInfoDir_c() | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoDir_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
