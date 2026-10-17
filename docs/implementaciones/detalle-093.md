<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1841"></a>

## UpsideDownCar

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| UpsideDownCar() | [source/game_sa/UpsideDownCarCheck.h:15](../../source/game_sa/UpsideDownCarCheck.h#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clear() | [source/game_sa/UpsideDownCarCheck.h:19](../../source/game_sa/UpsideDownCarCheck.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1842"></a>

## WaterCreatureManager_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaterCreatureManager_c::InjectHooks() | [source/game_sa/WaterCreatureManager_c.cpp:25](../../source/game_sa/WaterCreatureManager_c.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::WaterCreatureManager_c() | [source/game_sa/WaterCreatureManager_c.cpp:38](../../source/game_sa/WaterCreatureManager_c.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::Init() | [source/game_sa/WaterCreatureManager_c.cpp:42](../../source/game_sa/WaterCreatureManager_c.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::Init 0x006E3F90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::Exit() | [source/game_sa/WaterCreatureManager_c.cpp:53](../../source/game_sa/WaterCreatureManager_c.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::Exit 0x006E3FD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::GetRandomWaterCreatureId() | [source/game_sa/WaterCreatureManager_c.cpp:65](../../source/game_sa/WaterCreatureManager_c.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::GetRandomWaterCreatureId 0x006E4040 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::TryToFreeUpWaterCreatures(int32 numToFree) | [source/game_sa/WaterCreatureManager_c.cpp:83](../../source/game_sa/WaterCreatureManager_c.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::TryToFreeUpWaterCreatures 0x006E40E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::CanAddWaterCreatureAtPos(int32 id, CVector pos) | [source/game_sa/WaterCreatureManager_c.cpp:101](../../source/game_sa/WaterCreatureManager_c.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::CanAddWaterCreatureAtPos 0x006E4510 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::TryToExitGroup(WaterCreature_c* waterCreature) | [source/game_sa/WaterCreatureManager_c.cpp:118](../../source/game_sa/WaterCreatureManager_c.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::TryToExitGroup 0x006E45B0 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |
| WaterCreatureManager_c::Update(float deltaTime) | [source/game_sa/WaterCreatureManager_c.cpp:151](../../source/game_sa/WaterCreatureManager_c.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::Update 0x006E4F10 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |
| & GetCreatureInfo(uint32/*eWaterCreatureType*/ type) | [source/game_sa/WaterCreatureManager_c.h:62](../../source/game_sa/WaterCreatureManager_c.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_waterCreatureInfos: source/game_sa/WaterCreatureManager_c.h:37 (0x008D3698) |

<a id="grupo-1843"></a>

## WaterCreature_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaterCreature_c::InjectHooks() | [source/game_sa/WaterCreature_c.cpp:24](../../source/game_sa/WaterCreature_c.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreature_c::Init(int32 nType, CVector* vecPos, WaterCreature_c* parent, float fWaterLevel, float fWaterDepth) | [source/game_sa/WaterCreature_c.cpp:92](../../source/game_sa/WaterCreature_c.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreature_c::Init 0x006E4160 | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914) |
| WaterCreature_c::Exit() | [source/game_sa/WaterCreature_c.cpp:167](../../source/game_sa/WaterCreature_c.cpp#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreature_c::Exit 0x006E3E60 | 0 / 0 | Posible dato fijo g_waterCreatureMan: source/game_sa/WaterCreatureManager_c.cpp:23 (0x00C1DF30) |
| WaterCreature_c::Update(float fTimeStep) | [source/game_sa/WaterCreature_c.cpp:178](../../source/game_sa/WaterCreature_c.cpp#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreature_c::Update 0x006E4670 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028); Posible dato fijo g_waterCreatureMan: source/game_sa/WaterCreatureManager_c.cpp:23 (0x00C1DF30) |
| WaterCreature_c() | [source/game_sa/WaterCreature_c.h:23](../../source/game_sa/WaterCreature_c.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetObject() const | [source/game_sa/WaterCreature_c.h:48](../../source/game_sa/WaterCreature_c.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsJellyfish() const | [source/game_sa/WaterCreature_c.h:49](../../source/game_sa/WaterCreature_c.h#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSmallFish() const | [source/game_sa/WaterCreature_c.h:50](../../source/game_sa/WaterCreature_c.h#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1844"></a>

## cBuoyancy

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cBuoyancy::InjectHooks() | [source/game_sa/Buoyancy.cpp:50](../../source/game_sa/Buoyancy.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::ProcessBuoyancy(CPhysical* entity, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) | [source/game_sa/Buoyancy.cpp:67](../../source/game_sa/Buoyancy.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::ProcessBuoyancy 0x006C3EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::ProcessBuoyancyBoat(CVehicle* vehicle, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce, bool bUnderwater) | [source/game_sa/Buoyancy.cpp:107](../../source/game_sa/Buoyancy.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::ProcessBuoyancyBoat 0x006C3030 | 0 / 0 | Posible dato fijo afBoatVolumeDistribution: source/game_sa/Buoyancy.h:61 (0x008D32CC); Posible dato fijo afBoatVolumeDistributionDinghy: source/game_sa/Buoyancy.h:64 (0x008D3338); Posible dato fijo afBoatVolumeDistributionSail: source/game_sa/Buoyancy.h:63 (0x008D3314); Posible dato fijo afBoatVolumeDistributionSpeed: source/game_sa/Buoyancy.h:65 (0x008D335C); Posible dato fijo fBuoyancyBoatNormalWeight: source/game_sa/Buoyancy.cpp:27 (0x008D3384); Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::CalcBuoyancyForce(CPhysical* entity, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) | [source/game_sa/Buoyancy.cpp:195](../../source/game_sa/Buoyancy.cpp#L195) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::CalcBuoyancyForce 0x006C2750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::PreCalcSetup(CPhysical* entity, float fBuoyancy) | [source/game_sa/Buoyancy.cpp:216](../../source/game_sa/Buoyancy.cpp#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::PreCalcSetup 0x006C2B90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::AddSplashParticles(CPhysical* entity, CVector vecFrom, CVector vecTo, CVector vecSplashDir, uint8 bReduceParticleSize) | [source/game_sa/Buoyancy.cpp:325](../../source/game_sa/Buoyancy.cpp#L325) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::AddSplashParticles 0x006C34E0 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo fBuoyancySplashMaxSpeed: source/game_sa/Buoyancy.cpp:29 (0x008D3388); Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |
| cBuoyancy::SimpleCalcBuoyancy(CPhysical* entity) | [source/game_sa/Buoyancy.cpp:422](../../source/game_sa/Buoyancy.cpp#L422) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::SimpleCalcBuoyancy 0x006C3B00 | 0 / 0 | Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::SimpleSumBuoyancyData(CVector* vecWaterOffset, tWaterLevel ePointState) | [source/game_sa/Buoyancy.cpp:505](../../source/game_sa/Buoyancy.cpp#L505) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::SimpleSumBuoyancyData 0x006C2970 | 0 / 0 | Posible dato fijo calcStruct: source/game_sa/Buoyancy.h:60 (0x00C1C858); Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::FindWaterLevel(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState) | [source/game_sa/Buoyancy.cpp:546](../../source/game_sa/Buoyancy.cpp#L546) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::FindWaterLevel 0x006C2810 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::FindWaterLevelNorm(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState, CVector* outVecNormal) | [source/game_sa/Buoyancy.cpp:566](../../source/game_sa/Buoyancy.cpp#L566) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::FindWaterLevelNorm 0x006C28C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1845"></a>

## cHandlingDataMgr

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cHandlingDataMgr::InjectHooks() | [source/game_sa/cHandlingDataMgr.cpp:32](../../source/game_sa/cHandlingDataMgr.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::LoadHandlingData() | [source/game_sa/cHandlingDataMgr.cpp:204](../../source/game_sa/cHandlingDataMgr.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::LoadHandlingData 0x005BD830 | 0 / 0 | Posible dato fijo VehicleNames: source/game_sa/cHandlingDataMgr.cpp:24 (0x008D3978); Posible dato fijo m_vehicleAnimGroups: source/game_sa/VehicleAnimGroupData.h:146 (0x00C1CDC0) |
| cHandlingDataMgr::Initialise() | [source/game_sa/cHandlingDataMgr.cpp:359](../../source/game_sa/cHandlingDataMgr.cpp#L359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetFlyingPointer(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:370](../../source/game_sa/cHandlingDataMgr.cpp#L370) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetFlyingPointer 0x006F52D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetBoatPointer(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:378](../../source/game_sa/cHandlingDataMgr.cpp#L378) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetBoatPointer 0x006F5300 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetVehiclePointer(uint32 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:385](../../source/game_sa/cHandlingDataMgr.cpp#L385) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetBikeHandlingPointer(uint32 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:389](../../source/game_sa/cHandlingDataMgr.cpp#L389) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::HasFrontWheelDrive(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:396](../../source/game_sa/cHandlingDataMgr.cpp#L396) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::HasFrontWheelDrive 0x006A0480 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::HasRearWheelDrive(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:401](../../source/game_sa/cHandlingDataMgr.cpp#L401) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::HasRearWheelDrive 0x006A04B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::GetHandlingId(const char* nameToFind) | [source/game_sa/cHandlingDataMgr.cpp:407](../../source/game_sa/cHandlingDataMgr.cpp#L407) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetHandlingId 0x006F4FD0 | 0 / 0 | Posible dato fijo VehicleNames: source/game_sa/cHandlingDataMgr.cpp:24 (0x008D3978) |
| cHandlingDataMgr::ConvertDataToWorldUnits(tHandlingData* h) | [source/game_sa/cHandlingDataMgr.cpp:434](../../source/game_sa/cHandlingDataMgr.cpp#L434) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertDataToWorldUnits 0x006F5010 | 0 / 0 | Posible dato fijo s_HandlingAccelConst: source/game_sa/cHandlingDataMgr.cpp:419 (0x00C2B9B4); Posible dato fijo s_HandlingVelocityConst: source/game_sa/cHandlingDataMgr.cpp:420 (0x00C2B9BC) |
| cHandlingDataMgr::ConvertDataToGameUnits(tHandlingData* h) | [source/game_sa/cHandlingDataMgr.cpp:449](../../source/game_sa/cHandlingDataMgr.cpp#L449) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertDataToGameUnits 0x006F5080 | 0 / 0 | Posible dato fijo s_HandlingAccelConst: source/game_sa/cHandlingDataMgr.cpp:419 (0x00C2B9B4); Posible dato fijo s_HandlingVelocityConst: source/game_sa/cHandlingDataMgr.cpp:420 (0x00C2B9BC) |
| cHandlingDataMgr::ConvertBikeDataToWorldUnits(tBikeHandlingData* bikeHandling) | [source/game_sa/cHandlingDataMgr.cpp:461](../../source/game_sa/cHandlingDataMgr.cpp#L461) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertBikeDataToWorldUnits 0x006F5240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::ConvertBikeDataToGameUnits(tBikeHandlingData* bikeHandling) | [source/game_sa/cHandlingDataMgr.cpp:469](../../source/game_sa/cHandlingDataMgr.cpp#L469) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertBikeDataToGameUnits 0x006F5290 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::FindExactWord(const char* name, const char* nameTable, uint32 entrySize, uint32 entryCount) | [source/game_sa/cHandlingDataMgr.cpp:477](../../source/game_sa/cHandlingDataMgr.cpp#L477) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::FindExactWord 0x006F4F30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1846"></a>

## cTransmission

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cTransmission::InjectHooks() | [source/game_sa/cTransmission.cpp:24](../../source/game_sa/cTransmission.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::DisplayGearRatios() | [source/game_sa/cTransmission.cpp:42](../../source/game_sa/cTransmission.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::DisplayGearRatios 0x006D0590 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::InitGearRatios() | [source/game_sa/cTransmission.cpp:60](../../source/game_sa/cTransmission.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::InitGearRatios 0x006D0460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateGearForSimpleCar(float speed, uint8& currentGear) | [source/game_sa/cTransmission.cpp:93](../../source/game_sa/cTransmission.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateGearForSimpleCar 0x006D0530 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateDriveAcceleration(const float& gasPedal, uint8& currentGear, float& gearChangeCount, float& velocity, float* a6, float* a7, uint8 allWheelsOnGround, uint8 handlingCheat) | [source/game_sa/cTransmission.cpp:112](../../source/game_sa/cTransmission.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateDriveAcceleration 0x006D05E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1847"></a>

## detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RegionValueOf(float p) | [source/game_sa/PathFind.cpp:150](../../source/game_sa/PathFind.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptWidthScale(uint8 width) | [source/game_sa/PathFind.cpp:604](../../source/game_sa/PathFind.cpp#L604) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptOffsetPos(const CVector& nodePos, float px, float py, float scale) | [source/game_sa/PathFind.cpp:607](../../source/game_sa/PathFind.cpp#L607) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCoorsOfRegion(size_t p, size_t nareas) | [source/game_sa/PathFind.cpp:1551](../../source/game_sa/PathFind.cpp#L1551) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1848"></a>

## details

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * CPtrNodeDoubleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:4](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeDoubleLink__operator_delete(void* data, size_t sz) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:9](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPtrNodeSingleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeSingleLink.cpp:4](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeSingleLink__operator_delete(void* data, size_t sz) | [source/game_sa/Core/PtrNodeSingleLink.cpp:9](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1849"></a>

## details::PtrListIterator

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PtrListIterator(NodeType* node) | [source/game_sa/Core/PtrList.h:23](../../source/game_sa/Core/PtrList.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*() | [source/game_sa/Core/PtrList.h:31](../../source/game_sa/Core/PtrList.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-&gt;() | [source/game_sa/Core/PtrList.h:32](../../source/game_sa/Core/PtrList.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator++() | [source/game_sa/Core/PtrList.h:34](../../source/game_sa/Core/PtrList.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator++(int) | [source/game_sa/Core/PtrList.h:41](../../source/game_sa/Core/PtrList.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const PtrListIterator& a, const PtrListIterator& b) | [source/game_sa/Core/PtrList.h:47](../../source/game_sa/Core/PtrList.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1850"></a>

## notsa

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & PoolOf&lt;CColModel&gt;() | [source/game_sa/Pools/Pools.h:75](../../source/game_sa/Pools/Pools.h#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CPedIntelligence&gt;() | [source/game_sa/Pools/Pools.h:80](../../source/game_sa/Pools/Pools.h#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CEntryInfoNode&gt;() | [source/game_sa/Pools/Pools.h:91](../../source/game_sa/Pools/Pools.h#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CPointRoute&gt;() | [source/game_sa/Pools/Pools.h:96](../../source/game_sa/Pools/Pools.h#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CNodeRoute&gt;() | [source/game_sa/Pools/Pools.h:104](../../source/game_sa/Pools/Pools.h#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CTaskAllocator&gt;() | [source/game_sa/Pools/Pools.h:109](../../source/game_sa/Pools/Pools.h#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CPedAttractor&gt;() | [source/game_sa/Pools/Pools.h:114](../../source/game_sa/Pools/Pools.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:123](../../source/game_sa/Pools/Pools.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:129](../../source/game_sa/Pools/Pools.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:135](../../source/game_sa/Pools/Pools.h#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:141](../../source/game_sa/Pools/Pools.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:147](../../source/game_sa/Pools/Pools.h#L147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:153](../../source/game_sa/Pools/Pools.h#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:159](../../source/game_sa/Pools/Pools.h#L159) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1851"></a>

## notsa::Route

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetAll() | [source/game_sa/Route.hpp:13](../../source/game_sa/Route.hpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAll() const | [source/game_sa/Route.hpp:14](../../source/game_sa/Route.hpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEmpty() const | [source/game_sa/Route.hpp:17](../../source/game_sa/Route.hpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFull() const | [source/game_sa/Route.hpp:20](../../source/game_sa/Route.hpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Add(T_Entries&&... entry) | [source/game_sa/Route.hpp:24](../../source/game_sa/Route.hpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddUnlessFull(const CVector& entry) | [source/game_sa/Route.hpp:27](../../source/game_sa/Route.hpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddMultipleUnlessFull(T_Points&&... entry) | [source/game_sa/Route.hpp:35](../../source/game_sa/Route.hpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reverse() | [source/game_sa/Route.hpp:38](../../source/game_sa/Route.hpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clear() | [source/game_sa/Route.hpp:41](../../source/game_sa/Route.hpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSize() const | [source/game_sa/Route.hpp:44](../../source/game_sa/Route.hpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResizeTo(uint32 s) | [source/game_sa/Route.hpp:47](../../source/game_sa/Route.hpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator\[\](size_t idx) | [source/game_sa/Route.hpp:49](../../source/game_sa/Route.hpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator\[\](size_t idx) const | [source/game_sa/Route.hpp:50](../../source/game_sa/Route.hpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1852"></a>

## notsa::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddressOfFunction(T fn) | [source/game_sa/Scripts/CommandParser/Parser.hpp:103](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOPCommandHandler(Args... args) | [source/game_sa/Scripts/CommandParser/Parser.hpp:109](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1853"></a>

## notsa::details::PtrListDoubleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListDoubleLink.h:19](../../source/game_sa/Core/PtrListDoubleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListDoubleLink.h:33](../../source/game_sa/Core/PtrListDoubleLink.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListDoubleLink.h:60](../../source/game_sa/Core/PtrListDoubleLink.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1854"></a>

## notsa::details::PtrListSingleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListSingleLink.h:19](../../source/game_sa/Core/PtrListSingleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListSingleLink.h:28](../../source/game_sa/Core/PtrListSingleLink.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListSingleLink.h:47](../../source/game_sa/Core/PtrListSingleLink.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1855"></a>

## notsa::envmap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PoolNew(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:104](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolDelete(Pool* pool, const void* obj) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:123](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1856"></a>

## notsa::script

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetScriptCommandName(eScriptCommands cmd) | [source/game_sa/Enums/eScriptCommands.h:2904](../../source/game_sa/Enums/eScriptCommands.h#L2904) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommandHasCustomHandler() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:21](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GTAProcessCommand(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:40](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHandlerOfCommand() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:46](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IterateCommandIDs(auto&& functor) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:56](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GenerateLUT() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:61](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Read(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:111](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo UsedObjectArray: source/game_sa/Scripts/TheScripts.h:384 (0x00A44B70) |
| StoreArg(CRunningScript* S, T arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:18](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const T& arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:30](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const CVector& v3) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:53](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const CVector2D& v2) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:59](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, CompareFlagUpdate flag) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:65](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, T&& value) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:74](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, T value) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:94](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const MultiRet&lt;Ts...&gt;& arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:102](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| return_multiple(Ts&&... args) | [source/game_sa/Scripts/CommandParser/Utility.hpp:20](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FixAngleDegrees(float deg) | [source/game_sa/Scripts/CommandParser/Utility.hpp:38](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedOrItsVehicle(CPed& ped) -&gt; CPhysical& | [source/game_sa/Scripts/CommandParser/Utility.hpp:49](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1857"></a>

## notsa::script::commands::basic

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::basic::RegisterHandlers() | [source/game_sa/Scripts/Commands/BasicCommands.cpp:230](../../source/game_sa/Scripts/Commands/BasicCommands.cpp#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1858"></a>

## notsa::script::commands::camera

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::camera::RegisterHandlers() | [source/game_sa/Scripts/Commands/CameraCommands.cpp:308](../../source/game_sa/Scripts/Commands/CameraCommands.cpp#L308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1859"></a>

## notsa::script::commands::character

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::character::RegisterHandlers() | [source/game_sa/Scripts/Commands/CharacterCommands.cpp:4046](../../source/game_sa/Scripts/Commands/CharacterCommands.cpp#L4046) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1860"></a>

## notsa::script::commands::cleo::audiostream

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::audiostream::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp:89](../../source/game_sa/Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
