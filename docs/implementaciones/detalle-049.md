<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-961"></a>

## FxSystem_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxSystem_c::InjectHooks() | [source/game_sa/Fx/FxSystem.cpp:29](../../source/game_sa/Fx/FxSystem.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxSystem_c::Constructor() | [source/game_sa/Fx/FxSystem.cpp:69](../../source/game_sa/Fx/FxSystem.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Constructor 0x004AAF00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxSystem_c::Destructor() | [source/game_sa/Fx/FxSystem.cpp:70](../../source/game_sa/Fx/FxSystem.cpp#L70) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: FxSystem_c::Destructor 0x004AA260 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::FxSystem_c() | [source/game_sa/Fx/FxSystem.cpp:73](../../source/game_sa/Fx/FxSystem.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Constructor 0x004AAF00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::~FxSystem_c() | [source/game_sa/Fx/FxSystem.cpp:81](../../source/game_sa/Fx/FxSystem.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Destructor 0x004AA260 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::Init(FxSystemBP_c* systemBP, const RwMatrix& local, RwMatrix* parent) | [source/game_sa/Fx/FxSystem.cpp:92](../../source/game_sa/Fx/FxSystem.cpp#L92) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: FxSystem_c::Init 0x004AA750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::Exit() | [source/game_sa/Fx/FxSystem.cpp:125](../../source/game_sa/Fx/FxSystem.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Exit 0x004AA840 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::Play() | [source/game_sa/Fx/FxSystem.cpp:135](../../source/game_sa/Fx/FxSystem.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Play 0x004AA2F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::Pause() | [source/game_sa/Fx/FxSystem.cpp:154](../../source/game_sa/Fx/FxSystem.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Pause 0x004AA370 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::Stop() | [source/game_sa/Fx/FxSystem.cpp:166](../../source/game_sa/Fx/FxSystem.cpp#L166) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Stop 0x004AA390 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::PlayAndKill() | [source/game_sa/Fx/FxSystem.cpp:176](../../source/game_sa/Fx/FxSystem.cpp#L176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::PlayAndKill 0x004AA3D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::Kill() | [source/game_sa/Fx/FxSystem.cpp:186](../../source/game_sa/Fx/FxSystem.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Kill 0x004AA3F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::AttachToBone(CEntity* entity, eBoneTag boneId) | [source/game_sa/Fx/FxSystem.cpp:192](../../source/game_sa/Fx/FxSystem.cpp#L192) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::AttachToBone 0x004AA400 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float lightMult, float lightMultLimit, bool createLocal) | [source/game_sa/Fx/FxSystem.cpp:211](../../source/game_sa/Fx/FxSystem.cpp#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::AddParticle-v3d 0x004AA440 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float lightMult, float lightMultLimit, bool createLocal) | [source/game_sa/Fx/FxSystem.cpp:225](../../source/game_sa/Fx/FxSystem.cpp#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::AddParticle-mat 0x004AA540 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::EnablePrim(int32 primIndex, bool enable) | [source/game_sa/Fx/FxSystem.cpp:236](../../source/game_sa/Fx/FxSystem.cpp#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::EnablePrim 0x004AA610 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetMatrix(RwMatrix* matrix) | [source/game_sa/Fx/FxSystem.cpp:241](../../source/game_sa/Fx/FxSystem.cpp#L241) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetMatrix 0x004AA630 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetOffsetPos(const CVector& pos) | [source/game_sa/Fx/FxSystem.cpp:249](../../source/game_sa/Fx/FxSystem.cpp#L249) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetOffsetPos 0x004AA660 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::AddOffsetPos(const CVector& pos) | [source/game_sa/Fx/FxSystem.cpp:255](../../source/game_sa/Fx/FxSystem.cpp#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::AddOffsetPos 0x004AA690 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetConstTime(bool on, float time) | [source/game_sa/Fx/FxSystem.cpp:261](../../source/game_sa/Fx/FxSystem.cpp#L261) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetConstTime 0x004AA6C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetRateMult(float mult) | [source/game_sa/Fx/FxSystem.cpp:267](../../source/game_sa/Fx/FxSystem.cpp#L267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetRateMult 0x004AA6F0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetTimeMult(float mult) | [source/game_sa/Fx/FxSystem.cpp:272](../../source/game_sa/Fx/FxSystem.cpp#L272) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetTimeMult 0x004AA710 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetVelAdd(const CVector& velocity) | [source/game_sa/Fx/FxSystem.cpp:277](../../source/game_sa/Fx/FxSystem.cpp#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetVelAdd 0x004AA730 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetLocalParticles(bool enable) | [source/game_sa/Fx/FxSystem.cpp:282](../../source/game_sa/Fx/FxSystem.cpp#L282) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetLocalParticles 0x004AA910 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetZTestEnable(bool enable) | [source/game_sa/Fx/FxSystem.cpp:287](../../source/game_sa/Fx/FxSystem.cpp#L287) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetZTestEnable 0x004AAC50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetMustCreatePrts(bool enable) | [source/game_sa/Fx/FxSystem.cpp:292](../../source/game_sa/Fx/FxSystem.cpp#L292) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetMustCreatePrts 0x004AAC70 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::CopyParentMatrix() | [source/game_sa/Fx/FxSystem.cpp:297](../../source/game_sa/Fx/FxSystem.cpp#L297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::CopyParentMatrix 0x004AA890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::GetCompositeMatrix(RwMatrix* out) const | [source/game_sa/Fx/FxSystem.cpp:306](../../source/game_sa/Fx/FxSystem.cpp#L306) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::GetCompositeMatrix 0x004AA8C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::GetPlayStatus() const | [source/game_sa/Fx/FxSystem.cpp:314](../../source/game_sa/Fx/FxSystem.cpp#L314) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::GetPlayStatus 0x004AA900 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::ForAllParticles(void(*callback)(Particle_c*, int32, FxBox_c**), FxBox_c* data) | [source/game_sa/Fx/FxSystem.cpp:319](../../source/game_sa/Fx/FxSystem.cpp#L319) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::ForAllParticles 0x004AA930 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::UpdateBoundingBoxCB(Particle_c* particle, int32 a2, FxBox_c** data) | [source/game_sa/Fx/FxSystem.cpp:338](../../source/game_sa/Fx/FxSystem.cpp#L338) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::UpdateBoundingBoxCB 0x004AA9A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::GetBoundingBox(FxBox_c* out) | [source/game_sa/Fx/FxSystem.cpp:353](../../source/game_sa/Fx/FxSystem.cpp#L353) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxSystem_c::GetBoundingSphereWld(FxSphere_c* out) const | [source/game_sa/Fx/FxSystem.cpp:374](../../source/game_sa/Fx/FxSystem.cpp#L374) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::GetBoundingSphereWld 0x004AAAD0 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxSystem_c::GetBoundingSphereLcl(FxSphere_c* out) const | [source/game_sa/Fx/FxSystem.cpp:388](../../source/game_sa/Fx/FxSystem.cpp#L388) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::GetBoundingSphereLcl 0x004AAB50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::SetBoundingSphere(FxSphere_c* sphere) | [source/game_sa/Fx/FxSystem.cpp:397](../../source/game_sa/Fx/FxSystem.cpp#L397) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::SetBoundingSphere 0x004AAB80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::ResetBoundingSphere() | [source/game_sa/Fx/FxSystem.cpp:411](../../source/game_sa/Fx/FxSystem.cpp#L411) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::ResetBoundingSphere 0x004AABF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::DoFxAudio(CVector pos) | [source/game_sa/Fx/FxSystem.cpp:425](../../source/game_sa/Fx/FxSystem.cpp#L425) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::DoFxAudio 0x004AAC90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxSystem_c::IsVisible() const | [source/game_sa/Fx/FxSystem.cpp:449](../../source/game_sa/Fx/FxSystem.cpp#L449) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::IsVisible 0x004AAF30 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxSystem_c::Update(RwCamera* camera, float timeDelta) | [source/game_sa/Fx/FxSystem.cpp:460](../../source/game_sa/Fx/FxSystem.cpp#L460) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxSystem_c::Update 0x004AAF70 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxSystem_c::GetPrims() | [source/game_sa/Fx/FxSystem.cpp:530](../../source/game_sa/Fx/FxSystem.cpp#L530) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| KillAndClear(T*& fx) requires std::is_base_of_v&lt;FxSystem_c, T&gt; | [source/game_sa/Fx/FxSystem.h:125](../../source/game_sa/Fx/FxSystem.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SafeKillAndClear(T*& fx) requires std::is_base_of_v&lt;FxSystem_c, T&gt; | [source/game_sa/Fx/FxSystem.h:131](../../source/game_sa/Fx/FxSystem.h#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-962"></a>

## Fx_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Fx_c::InjectHooks() | [source/game_sa/Fx/Fx.cpp:40](../../source/game_sa/Fx/Fx.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Fx_c::Constructor() | [source/game_sa/Fx/Fx.cpp:88](../../source/game_sa/Fx/Fx.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Fx_c::Destructor() | [source/game_sa/Fx/Fx.cpp:89](../../source/game_sa/Fx/Fx.cpp#L89) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::InitStaticSystems() | [source/game_sa/Fx/Fx.cpp:93](../../source/game_sa/Fx/Fx.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::ExitStaticSystems() | [source/game_sa/Fx/Fx.cpp:114](../../source/game_sa/Fx/Fx.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::InitEntitySystems() | [source/game_sa/Fx/Fx.cpp:135](../../source/game_sa/Fx/Fx.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::ExitEntitySystems() | [source/game_sa/Fx/Fx.cpp:156](../../source/game_sa/Fx/Fx.cpp#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::Init() | [source/game_sa/Fx/Fx.cpp:168](../../source/game_sa/Fx/Fx.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Wind: source/game_sa/Weather.h:44 (0x00C812F0); Posible dato fijo WindDir: source/game_sa/Weather.h:43 (0x00C813E0); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::Exit() | [source/game_sa/Fx/Fx.cpp:180](../../source/game_sa/Fx/Fx.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::Reset() | [source/game_sa/Fx/Fx.cpp:187](../../source/game_sa/Fx/Fx.cpp#L187) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::CreateEntityFx(CEntity* entity, const char* fxName, const CVector& pos, RwMatrix* transform) | [source/game_sa/Fx/Fx.cpp:194](../../source/game_sa/Fx/Fx.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::DestroyEntityFx(CEntity* entity) | [source/game_sa/Fx/Fx.cpp:208](../../source/game_sa/Fx/Fx.cpp#L208) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::Update(RwCamera* camera, float timeDelta) | [source/game_sa/Fx/Fx.cpp:221](../../source/game_sa/Fx/Fx.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::Render(RwCamera* camera, bool heatHaze) | [source/game_sa/Fx/Fx.cpp:228](../../source/game_sa/Fx/Fx.cpp#L228) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| Fx_c::CreateMatFromVec(RwMatrix* mat, const CVector* pos, const CVector* dir) | [source/game_sa/Fx/Fx.cpp:236](../../source/game_sa/Fx/Fx.cpp#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Fx_c::CreateMatFromVec 0x0049E950 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::SetFxQuality(FxQuality_e quality) | [source/game_sa/Fx/Fx.cpp:275](../../source/game_sa/Fx/Fx.cpp#L275) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::GetFxQuality() const | [source/game_sa/Fx/Fx.cpp:280](../../source/game_sa/Fx/Fx.cpp#L280) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddBlood(const CVector& pos, const CVector& direction, int32 amount, float lightMult) | [source/game_sa/Fx/Fx.cpp:286](../../source/game_sa/Fx/Fx.cpp#L286) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddWood(const CVector& pos, const CVector& direction, int32 amount, float lightMult) | [source/game_sa/Fx/Fx.cpp:291](../../source/game_sa/Fx/Fx.cpp#L291) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddSparks(const CVector& origin, const CVector& direction, float force, int32 amount, CVector across, eSparkType sparksType, float spread, float life) | [source/game_sa/Fx/Fx.cpp:296](../../source/game_sa/Fx/Fx.cpp#L296) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddTyreBurst(const CVector& posn, const CVector& velocity) | [source/game_sa/Fx/Fx.cpp:301](../../source/game_sa/Fx/Fx.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddBulletImpact(const CVector& posn, const CVector& direction, int32 bulletFxType, int32 amount, float arg4) | [source/game_sa/Fx/Fx.cpp:306](../../source/game_sa/Fx/Fx.cpp#L306) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddPunchImpact(const CVector& pos, const CVector& velocity, int32 num) | [source/game_sa/Fx/Fx.cpp:311](../../source/game_sa/Fx/Fx.cpp#L311) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddDebris(const CVector& pos, const RwRGBA& color, float scale, int32 amount) | [source/game_sa/Fx/Fx.cpp:316](../../source/game_sa/Fx/Fx.cpp#L316) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddGlass(const CVector& pos, const RwRGBA& color, float scale, int32 amount) | [source/game_sa/Fx/Fx.cpp:321](../../source/game_sa/Fx/Fx.cpp#L321) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddWheelSpray(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, bool bInWater, float lightMult) | [source/game_sa/Fx/Fx.cpp:326](../../source/game_sa/Fx/Fx.cpp#L326) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddWheelGrass(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) | [source/game_sa/Fx/Fx.cpp:331](../../source/game_sa/Fx/Fx.cpp#L331) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddWheelGravel(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) | [source/game_sa/Fx/Fx.cpp:336](../../source/game_sa/Fx/Fx.cpp#L336) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddWheelMud(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) | [source/game_sa/Fx/Fx.cpp:341](../../source/game_sa/Fx/Fx.cpp#L341) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddWheelSand(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) | [source/game_sa/Fx/Fx.cpp:346](../../source/game_sa/Fx/Fx.cpp#L346) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::AddWheelDust(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) | [source/game_sa/Fx/Fx.cpp:351](../../source/game_sa/Fx/Fx.cpp#L351) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::TriggerWaterHydrant(const CVector& pos) | [source/game_sa/Fx/Fx.cpp:356](../../source/game_sa/Fx/Fx.cpp#L356) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::TriggerGunshot(CEntity* entity, const CVector& origin, const CVector& target, bool doGunflash) | [source/game_sa/Fx/Fx.cpp:361](../../source/game_sa/Fx/Fx.cpp#L361) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::TriggerTankFire(const CVector& pos, const CVector& dir) | [source/game_sa/Fx/Fx.cpp:366](../../source/game_sa/Fx/Fx.cpp#L366) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::TriggerWaterSplash(const CVector& pos) | [source/game_sa/Fx/Fx.cpp:371](../../source/game_sa/Fx/Fx.cpp#L371) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::TriggerWaterSplash 0x004A1070 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::TriggerBulletSplash(const CVector& pos) | [source/game_sa/Fx/Fx.cpp:376](../../source/game_sa/Fx/Fx.cpp#L376) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::TriggerBulletSplash 0x004A10E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Fx_c::TriggerFootSplash(const CVector& pos) | [source/game_sa/Fx/Fx.cpp:381](../../source/game_sa/Fx/Fx.cpp#L381) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::TriggerFootSplash 0x004A1150 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RenderBegin(RwRaster* newRaster, RwMatrix* transform, uint32 transformRenderFlags) | [source/game_sa/Fx/Fx.cpp:387](../../source/game_sa/Fx/Fx.cpp#L387) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::RenderBegin 0x004A13B0 | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958); Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |
| RenderAddTri_( float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float u1, float v1, float u2, float v2, float u3, float v3, int32 r1, int32 g1, int32 b1, int32 a1, int32 r2, int32 g2, int32 b2, int32 a2, int32 r3, int32 g3, int32 b3, int32 a3 ) | [source/game_sa/Fx/Fx.cpp:404](../../source/game_sa/Fx/Fx.cpp#L404) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::RenderAddTri_ 0x004A1410 | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958); Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |
| RenderEnd() | [source/game_sa/Fx/Fx.cpp:491](../../source/game_sa/Fx/Fx.cpp#L491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::RenderEnd 0x004A1600 | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958); Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |
| RotateVecIntoVec(RwV3d& vecRes, const RwV3d& vec, const RwV3d& vecAlign) | [source/game_sa/Fx/Fx.cpp:506](../../source/game_sa/Fx/Fx.cpp#L506) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::RotateVecIntoVec 0x004A1660 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| RotateVecAboutVec(RwV3d& vecRes, const RwV3d& vec, const RwV3d& axis, float angle) | [source/game_sa/Fx/Fx.cpp:536](../../source/game_sa/Fx/Fx.cpp#L536) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Fx_c::RotateVecAboutVec 0x004A1780 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-963"></a>

## Globales y helpers: source/game_sa/3dMarker.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarker.cpp:39](../../source/game_sa/3dMarker.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetAtomicForMarkerType(e3dMarkerType type) | [source/game_sa/3dMarker.cpp:53](../../source/game_sa/3dMarker.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pRpClumpArray: source/game_sa/3dMarkers.h:57 (0x00C7C6DC) |

<a id="grupo-964"></a>

## Globales y helpers: source/game_sa/3dMarkers.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeStdSize(float sz, float dist) | [source/game_sa/3dMarkers.cpp:38](../../source/game_sa/3dMarkers.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarkers.cpp:55](../../source/game_sa/3dMarkers.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-965"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendAssociation.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendAssociation | [source/game_sa/Animation/AnimBlendAssociation.h:127](../../source/game_sa/Animation/AnimBlendAssociation.h#L127) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetBlendAmount(float a) | [source/game_sa/Animation/AnimBlendAssociation.h:156](../../source/game_sa/Animation/AnimBlendAssociation.h#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetBlendAmount(float weight = 1.f) const | [source/game_sa/Animation/AnimBlendAssociation.h:157](../../source/game_sa/Animation/AnimBlendAssociation.h#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetBlendDelta() const | [source/game_sa/Animation/AnimBlendAssociation.h:158](../../source/game_sa/Animation/AnimBlendAssociation.h#L158) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimId() const | [source/game_sa/Animation/AnimBlendAssociation.h:160](../../source/game_sa/Animation/AnimBlendAssociation.h#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPlaying() const | [source/game_sa/Animation/AnimBlendAssociation.h:162](../../source/game_sa/Animation/AnimBlendAssociation.h#L162) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsLooped() const | [source/game_sa/Animation/AnimBlendAssociation.h:163](../../source/game_sa/Animation/AnimBlendAssociation.h#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPartial() const | [source/game_sa/Animation/AnimBlendAssociation.h:164](../../source/game_sa/Animation/AnimBlendAssociation.h#L164) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSyncronised() const | [source/game_sa/Animation/AnimBlendAssociation.h:165](../../source/game_sa/Animation/AnimBlendAssociation.h#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanExtractXVelocity() const | [source/game_sa/Animation/AnimBlendAssociation.h:166](../../source/game_sa/Animation/AnimBlendAssociation.h#L166) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanExtractVelocity() const | [source/game_sa/Animation/AnimBlendAssociation.h:167](../../source/game_sa/Animation/AnimBlendAssociation.h#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFacial() const | [source/game_sa/Animation/AnimBlendAssociation.h:168](../../source/game_sa/Animation/AnimBlendAssociation.h#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetBlendDelta(float value) | [source/game_sa/Animation/AnimBlendAssociation.h:178](../../source/game_sa/Animation/AnimBlendAssociation.h#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCurrentTime() const | [source/game_sa/Animation/AnimBlendAssociation.h:182](../../source/game_sa/Animation/AnimBlendAssociation.h#L182) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetDefaultDeleteCallback() | [source/game_sa/Animation/AnimBlendAssociation.h:185](../../source/game_sa/Animation/AnimBlendAssociation.h#L185) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetDefaultFinishCallback() | [source/game_sa/Animation/AnimBlendAssociation.h:188](../../source/game_sa/Animation/AnimBlendAssociation.h#L188) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetFlag(eAnimationFlags flag, bool value = true) | [source/game_sa/Animation/AnimBlendAssociation.h:204](../../source/game_sa/Animation/AnimBlendAssociation.h#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasFlag(eAnimationFlags flag) const | [source/game_sa/Animation/AnimBlendAssociation.h:211](../../source/game_sa/Animation/AnimBlendAssociation.h#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FromLink(CAnimBlendLink* link) | [source/game_sa/Animation/AnimBlendAssociation.h:213](../../source/game_sa/Animation/AnimBlendAssociation.h#L213) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSpeed() const | [source/game_sa/Animation/AnimBlendAssociation.h:217](../../source/game_sa/Animation/AnimBlendAssociation.h#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSpeed(float speed) | [source/game_sa/Animation/AnimBlendAssociation.h:218](../../source/game_sa/Animation/AnimBlendAssociation.h#L218) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetNode(int32 nodeIndex) | [source/game_sa/Animation/AnimBlendAssociation.h:221](../../source/game_sa/Animation/AnimBlendAssociation.h#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetNodesPtr() | [source/game_sa/Animation/AnimBlendAssociation.h:222](../../source/game_sa/Animation/AnimBlendAssociation.h#L222) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetLink() | [source/game_sa/Animation/AnimBlendAssociation.h:224](../../source/game_sa/Animation/AnimBlendAssociation.h#L224) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHier() const | [source/game_sa/Animation/AnimBlendAssociation.h:225](../../source/game_sa/Animation/AnimBlendAssociation.h#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor0() | [source/game_sa/Animation/AnimBlendAssociation.h:231](../../source/game_sa/Animation/AnimBlendAssociation.h#L231) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor1(RpClump* clump, CAnimBlendHierarchy* animHierarchy) | [source/game_sa/Animation/AnimBlendAssociation.h:232](../../source/game_sa/Animation/AnimBlendAssociation.h#L232) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor2(CAnimBlendAssociation& assoc) | [source/game_sa/Animation/AnimBlendAssociation.h:233](../../source/game_sa/Animation/AnimBlendAssociation.h#L233) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor3(CAnimBlendStaticAssociation& assoc) | [source/game_sa/Animation/AnimBlendAssociation.h:234](../../source/game_sa/Animation/AnimBlendAssociation.h#L234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-966"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendStaticAssociation.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendStaticAssociation | [source/game_sa/Animation/AnimBlendStaticAssociation.h:23](../../source/game_sa/Animation/AnimBlendStaticAssociation.h#L23) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHashKey() const noexcept | [source/game_sa/Animation/AnimBlendStaticAssociation.h:35](../../source/game_sa/Animation/AnimBlendStaticAssociation.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsValid() const | [source/game_sa/Animation/AnimBlendStaticAssociation.h:37](../../source/game_sa/Animation/AnimBlendStaticAssociation.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAnimHierarchy() const | [source/game_sa/Animation/AnimBlendStaticAssociation.h:38](../../source/game_sa/Animation/AnimBlendStaticAssociation.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-967"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RoundSectionSize(uint32 size) | [source/game_sa/Animation/AnimManager.cpp:641](../../source/game_sa/Animation/AnimManager.cpp#L641) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewAnimSequences(int32 n) | [source/game_sa/Animation/AnimManager.cpp:646](../../source/game_sa/Animation/AnimManager.cpp#L646) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FloatToKeyFrameWord(float v) | [source/game_sa/Animation/AnimManager.cpp:660](../../source/game_sa/Animation/AnimManager.cpp#L660) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-968"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsClumpSkinned(RpClump *clump) | [source/game_sa/Animation/AnimManager.h:126](../../source/game_sa/Animation/AnimManager.h#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-969"></a>

## Globales y helpers: source/game_sa/Attractors/PedAtmAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedATMAttractor | [source/game_sa/Attractors/PedAtmAttractor.h:5](../../source/game_sa/Attractors/PedAtmAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedATMAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedAtmAttractor.h:10](../../source/game_sa/Attractors/PedAtmAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedAtmAttractor.h:27](../../source/game_sa/Attractors/PedAtmAttractor.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-970"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedAttractor | [source/game_sa/Attractors/PedAttractor.h:12](../../source/game_sa/Attractors/PedAttractor.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const | [source/game_sa/Attractors/PedAttractor.h:36](../../source/game_sa/Attractors/PedAttractor.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~CPedAttractor() | [source/game_sa/Attractors/PedAttractor.h:38](../../source/game_sa/Attractors/PedAttractor.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNoOfRegisteredPeds() const | [source/game_sa/Attractors/PedAttractor.h:51](../../source/game_sa/Attractors/PedAttractor.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetArrivedPeds() | [source/game_sa/Attractors/PedAttractor.h:55](../../source/game_sa/Attractors/PedAttractor.h#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSizeOfQueue() const | [source/game_sa/Attractors/PedAttractor.h:56](../../source/game_sa/Attractors/PedAttractor.h#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetAttractPeds() | [source/game_sa/Attractors/PedAttractor.h:57](../../source/game_sa/Attractors/PedAttractor.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEffect() const | [source/game_sa/Attractors/PedAttractor.h:58](../../source/game_sa/Attractors/PedAttractor.h#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetEntity() const | [source/game_sa/Attractors/PedAttractor.h:59](../../source/game_sa/Attractors/PedAttractor.h#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HasEmptySlot() const | [source/game_sa/Attractors/PedAttractor.h:61](../../source/game_sa/Attractors/PedAttractor.h#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComputeAttractPos(int32 pedId) | [source/game_sa/Attractors/PedAttractor.h:70](../../source/game_sa/Attractors/PedAttractor.h#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ComputeAttractHeading(int32 bQueue) | [source/game_sa/Attractors/PedAttractor.h:73](../../source/game_sa/Attractors/PedAttractor.h#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| classof(const From* f) | [source/game_sa/Attractors/PedAttractor.h:80](../../source/game_sa/Attractors/PedAttractor.h#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Attractors/PedAttractor.h:89](../../source/game_sa/Attractors/PedAttractor.h#L89) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-971"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractorPedPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:34](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:43](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-972"></a>

## Globales y helpers: source/game_sa/Attractors/PedLookAtAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedLookAtAttractor | [source/game_sa/Attractors/PedLookAtAttractor.h:5](../../source/game_sa/Attractors/PedLookAtAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedLookAtAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedLookAtAttractor.h:10](../../source/game_sa/Attractors/PedLookAtAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedLookAtAttractor.h:28](../../source/game_sa/Attractors/PedLookAtAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-973"></a>

## Globales y helpers: source/game_sa/Attractors/PedParkAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedParkAttractor | [source/game_sa/Attractors/PedParkAttractor.h:5](../../source/game_sa/Attractors/PedParkAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedParkAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedParkAttractor.h:10](../../source/game_sa/Attractors/PedParkAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedParkAttractor.h:28](../../source/game_sa/Attractors/PedParkAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-974"></a>

## Globales y helpers: source/game_sa/Attractors/PedPizzaAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedPizzaAttractor | [source/game_sa/Attractors/PedPizzaAttractor.h:5](../../source/game_sa/Attractors/PedPizzaAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedPizzaAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedPizzaAttractor.h:10](../../source/game_sa/Attractors/PedPizzaAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedPizzaAttractor.h:28](../../source/game_sa/Attractors/PedPizzaAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeadOfQueueWaitTime() const | [source/game_sa/Attractors/PedPizzaAttractor.h:31](../../source/game_sa/Attractors/PedPizzaAttractor.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-975"></a>

## Globales y helpers: source/game_sa/Attractors/PedScriptedAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedScriptedAttractor | [source/game_sa/Attractors/PedScriptedAttractor.h:5](../../source/game_sa/Attractors/PedScriptedAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedScriptedAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedScriptedAttractor.h:10](../../source/game_sa/Attractors/PedScriptedAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedScriptedAttractor.h:28](../../source/game_sa/Attractors/PedScriptedAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-976"></a>

## Globales y helpers: source/game_sa/Attractors/PedSeatAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedSeatAttractor | [source/game_sa/Attractors/PedSeatAttractor.h:5](../../source/game_sa/Attractors/PedSeatAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedSeatAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedSeatAttractor.h:10](../../source/game_sa/Attractors/PedSeatAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedSeatAttractor.h:28](../../source/game_sa/Attractors/PedSeatAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-977"></a>

## Globales y helpers: source/game_sa/Attractors/PedShelterAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedShelterAttractor | [source/game_sa/Attractors/PedShelterAttractor.h:5](../../source/game_sa/Attractors/PedShelterAttractor.h#L5) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedShelterAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedShelterAttractor.h:13](../../source/game_sa/Attractors/PedShelterAttractor.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedShelterAttractor.h:31](../../source/game_sa/Attractors/PedShelterAttractor.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsAtHeadOfQueue(CPed* ped) override | [source/game_sa/Attractors/PedShelterAttractor.h:34](../../source/game_sa/Attractors/PedShelterAttractor.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-978"></a>

## Globales y helpers: source/game_sa/Attractors/PedStepAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedStepAttractor | [source/game_sa/Attractors/PedStepAttractor.h:5](../../source/game_sa/Attractors/PedStepAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedStepAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedStepAttractor.h:10](../../source/game_sa/Attractors/PedStepAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedStepAttractor.h:28](../../source/game_sa/Attractors/PedStepAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-979"></a>

## Globales y helpers: source/game_sa/Attractors/PedStopAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedStopAttractor | [source/game_sa/Attractors/PedStopAttractor.h:5](../../source/game_sa/Attractors/PedStopAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedStopAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedStopAttractor.h:10](../../source/game_sa/Attractors/PedStopAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedStopAttractor.h:28](../../source/game_sa/Attractors/PedStopAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-980"></a>

## Globales y helpers: source/game_sa/Attractors/PedTriggerScriptAttractor.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CPedTriggerScriptAttractor | [source/game_sa/Attractors/PedTriggerScriptAttractor.h:5](../../source/game_sa/Attractors/PedTriggerScriptAttractor.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedTriggerScriptAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedTriggerScriptAttractor.h:10](../../source/game_sa/Attractors/PedTriggerScriptAttractor.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetType() const override | [source/game_sa/Attractors/PedTriggerScriptAttractor.h:28](../../source/game_sa/Attractors/PedTriggerScriptAttractor.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
