<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 883 grupos / 9938 funciones reconstruidas; 9938 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-961"></a>

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

<a id="grupo-962"></a>

## FurnitureSubGroup_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FurnitureSubGroup_c::InjectHooks() | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:26](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:61](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::AddFurniture 0x005C00C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FurnitureSubGroup_c::GetFurniture(int16 id, uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:92](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::GetFurniture 0x00590EE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::GetRandomId(uint8 wealth) | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:103](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FurnitureSubGroup_c::GetRandomId 0x00590FD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FurnitureSubGroup_c::Exit() | [source/game_sa/Interior/FurnitureSubGroup_c.cpp:108](../../source/game_sa/Interior/FurnitureSubGroup_c.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-963"></a>

## Fx

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DrawLight(CVector pos, CVector color) | [source/game_sa/Scripts/Commands/ObjectCommands.cpp:138](../../source/game_sa/Scripts/Commands/ObjectCommands.cpp#L138) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddBigGunFlash(CVector origin, CVector target) | [source/game_sa/Scripts/Commands/ObjectCommands.cpp:143](../../source/game_sa/Scripts/Commands/ObjectCommands.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |

<a id="grupo-964"></a>

## FxBox_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxBox_c() | [source/game_sa/Fx/FxBox.h:14](../../source/game_sa/Fx/FxBox.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reset() | [source/game_sa/Fx/FxBox.h:18](../../source/game_sa/Fx/FxBox.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator=(const RwMatrix& right) | [source/game_sa/Fx/FxBox.h:25](../../source/game_sa/Fx/FxBox.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-965"></a>

## FxBufferedMatrix

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyFromRwMatrix(const RwMatrix& mat) | [source/game_sa/Fx/FxPrimBP.h:33](../../source/game_sa/Fx/FxPrimBP.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyFromVectors(const CVector& r, const CVector& u, const CVector& a, const CVector& p) | [source/game_sa/Fx/FxPrimBP.h:40](../../source/game_sa/Fx/FxPrimBP.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CopyToRwMatrix(RwMatrix& out) | [source/game_sa/Fx/FxPrimBP.h:44](../../source/game_sa/Fx/FxPrimBP.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-966"></a>

## FxEmitterBP_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitterBP_c::InjectHooks() | [source/game_sa/Fx/FxEmitterBP.cpp:36](../../source/game_sa/Fx/FxEmitterBP.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::FxEmitterBP_c() | [source/game_sa/Fx/FxEmitterBP.cpp:52](../../source/game_sa/Fx/FxEmitterBP.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Constructor 0x004A18D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::RenderHeatHaze(RwCamera* camera, uint32 txdHashKey, float brightness) | [source/game_sa/Fx/FxEmitterBP.cpp:196](../../source/game_sa/Fx/FxEmitterBP.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::RenderHeatHaze 0x004A1940 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::UpdateParticle(float deltaTime, FxEmitterPrt_c* emitter) | [source/game_sa/Fx/FxEmitterBP.cpp:202](../../source/game_sa/Fx/FxEmitterBP.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::UpdateParticle 0x004A21D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxEmitterBP_c::CreateInstance() | [source/game_sa/Fx/FxEmitterBP.cpp:248](../../source/game_sa/Fx/FxEmitterBP.cpp#L248) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::CreateInstance 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::Update(float deltaTime) | [source/game_sa/Fx/FxEmitterBP.cpp:256](../../source/game_sa/Fx/FxEmitterBP.cpp#L256) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Update 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterBP_c::Load(FILESTREAM file, int32 version, FxName32_t* textureNames) | [source/game_sa/Fx/FxEmitterBP.cpp:277](../../source/game_sa/Fx/FxEmitterBP.cpp#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Load 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::LoadTextures(FxName32_t* textureNames, int32 version) | [source/game_sa/Fx/FxEmitterBP.cpp:295](../../source/game_sa/Fx/FxEmitterBP.cpp#L295) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::LoadTextures 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitterBP_c::Render(RwCamera* camera, uint32 txdHashKey, float brightness, bool doHeatHaze) | [source/game_sa/Fx/FxEmitterBP.cpp:327](../../source/game_sa/Fx/FxEmitterBP.cpp#L327) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Render 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterBP_c::FreePrtFromPrim(FxSystem_c* system) | [source/game_sa/Fx/FxEmitterBP.cpp:344](../../source/game_sa/Fx/FxEmitterBP.cpp#L344) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::FreePrtFromPrim 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterBP_c::IsFxInfoPresent(eFxInfoType type) const | [source/game_sa/Fx/FxEmitterBP.cpp:357](../../source/game_sa/Fx/FxEmitterBP.cpp#L357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/FxEmitterBP.h:26](../../source/game_sa/Fx/FxEmitterBP.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterBP_c::Constructor 0x004A18D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-967"></a>

## FxEmitterPrt_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * FxEmitterPrt_c::operator new\[\](size_t size) | [source/game_sa/Fx/FxEmitterPrt.cpp:26](../../source/game_sa/Fx/FxEmitterPrt.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterPrt_c::operator new\[\] 0x004A18B0 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitterPrt_c::InjectHooks() | [source/game_sa/Fx/FxEmitterPrt.cpp:33](../../source/game_sa/Fx/FxEmitterPrt.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/FxEmitterPrt.h:36](../../source/game_sa/Fx/FxEmitterPrt.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitterPrt_c::Constructor 0x004A94E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-968"></a>

## FxEmitter_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitter_c::InjectHooks() | [source/game_sa/Fx/FxEmitter.cpp:30](../../source/game_sa/Fx/FxEmitter.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Init(FxPrimBP_c* primBP, FxSystem_c* system) | [source/game_sa/Fx/FxEmitter.cpp:46](../../source/game_sa/Fx/FxEmitter.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Update(float currentTime, float deltaTime) | [source/game_sa/Fx/FxEmitter.cpp:54](../../source/game_sa/Fx/FxEmitter.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Reset() | [source/game_sa/Fx/FxEmitter.cpp:61](../../source/game_sa/Fx/FxEmitter.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:69](../../source/game_sa/Fx/FxEmitter.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitter_c::AddParticle-vector 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitter_c::AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:113](../../source/game_sa/Fx/FxEmitter.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxEmitter_c::AddParticle-matrix 0x00000000 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitter_c::CreateParticles(float currentTime, float deltaTime) | [source/game_sa/Fx/FxEmitter.cpp:147](../../source/game_sa/Fx/FxEmitter.cpp#L147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxEmitter_c::CreateParticles 0x004A41E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo Rain: source/game_sa/Weather.h:46 (0x00C81324); Posible dato fijo Wind: source/game_sa/Weather.h:44 (0x00C812F0); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| * FxEmitter_c::CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:190](../../source/game_sa/Fx/FxEmitter.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxEmitter_c::CreateParticle 0x004A2580 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Conversión de dirección fija: 0x00BB3E00; Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80); Posible dato fijo m_Color: source/game_sa/Font.h:99 (0x00C71A60) |

<a id="grupo-969"></a>

## FxFrustumInfo_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxFrustumInfo_c::InjectHooks() | [source/game_sa/Fx/FxFrustumInfo.cpp:23](../../source/game_sa/Fx/FxFrustumInfo.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxFrustumInfo_c::IsCollisionRegs(FxSphere_c& sphere) | [source/game_sa/Fx/FxFrustumInfo.cpp:34](../../source/game_sa/Fx/FxFrustumInfo.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCollision(FxSphere_c& sphere) | [source/game_sa/Fx/FxFrustumInfo.h:22](../../source/game_sa/Fx/FxFrustumInfo.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxFrustumInfo_c::IsCollision 0x004AA030 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-970"></a>

## FxInfoAnimTexture_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAnimTexture_c::FxInfoAnimTexture_c() | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAnimTexture_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAnimTexture_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-971"></a>

## FxInfoAttractLine_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractLine_c::FxInfoAttractLine_c() | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractLine_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractLine_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:29](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-972"></a>

## FxInfoAttractPt_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractPt_c::FxInfoAttractPt_c() | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractPt_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractPt_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-973"></a>

## FxInfoColourBright_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourBright_c::FxInfoColourBright_c() | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourBright_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourBright_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-974"></a>

## FxInfoColourRange_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourRange_c::FxInfoColourRange_c() | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourRange_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourRange_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-975"></a>

## FxInfoColour_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColour_c::FxInfoColour_c() | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColour_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColour_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-976"></a>

## FxInfoDir_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoDir_c::FxInfoDir_c() | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoDir_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-977"></a>

## FxInfoEmAngle_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmAngle_c::FxInfoEmAngle_c() | [source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmAngle_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmAngle_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-978"></a>

## FxInfoEmDir_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmDir_c::FxInfoEmDir_c() | [source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmDir_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-979"></a>

## FxInfoEmLife_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmLife_c::FxInfoEmLife_c() | [source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmLife_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmLife_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-980"></a>

## FxInfoEmPos_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmPos_c::FxInfoEmPos_c() | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
