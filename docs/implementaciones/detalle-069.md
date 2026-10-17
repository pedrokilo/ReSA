<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1361"></a>

## WinInput

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CreateInput() | [source/app/platform/win/WinInput.cpp:31](../../source/app/platform/win/WinInput.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialise() | [source/app/platform/win/WinInput.cpp:44](../../source/app/platform/win/WinInput.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x007485C0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198) |
| ReleaseInput() | [source/app/platform/win/WinInput.cpp:58](../../source/app/platform/win/WinInput.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Shutdown() | [source/app/platform/win/WinInput.cpp:65](../../source/app/platform/win/WinInput.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| diPadSetRanges(LPDIRECTINPUTDEVICE8 dev, DWORD padNum) | [source/app/platform/win/WinInput.cpp:70](../../source/app/platform/win/WinInput.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144) |
| diPadSetPIDVID(LPDIRECTINPUTDEVICE8 dev, DWORD padNum) | [source/app/platform/win/WinInput.cpp:133](../../source/app/platform/win/WinInput.cpp#L133) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144) |
| diMouseInit(bool exclusive) | [source/app/platform/win/WinInput.cpp:151](../../source/app/platform/win/WinInput.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumDevicesCallback_Old( const DIDEVICEINSTANCE* pdidInstance, VOID* pContextData ) | [source/app/platform/win/WinInput.cpp:168](../../source/app/platform/win/WinInput.cpp#L168) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| diPadAddJoyStick(LPDIRECTINPUTDEVICE8 lpDevice, INT padNum) | [source/app/platform/win/WinInput.cpp:221](../../source/app/platform/win/WinInput.cpp#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144) |
| diPadAddJoys() | [source/app/platform/win/WinInput.cpp:269](../../source/app/platform/win/WinInput.cpp#L269) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| diPadInit() | [source/app/platform/win/WinInput.cpp:297](../../source/app/platform/win/WinInput.cpp#L297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144); Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198) |
| EnumDevicesCallback(LPCDIDEVICEINSTANCEA inst, LPVOID) | [source/app/platform/win/WinInput.cpp:349](../../source/app/platform/win/WinInput.cpp#L349) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetMouseSetUp() | [source/app/platform/win/WinInput.cpp:383](../../source/app/platform/win/WinInput.cpp#L383) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| InjectHooks() | [source/app/platform/win/WinInput.cpp:433](../../source/app/platform/win/WinInput.cpp#L433) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsKeyPressed(unsigned int keyCode) | [source/app/platform/win/WinInput.cpp:445](../../source/app/platform/win/WinInput.cpp#L445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsKeyDown(unsigned int keyCode) | [source/app/platform/win/WinInput.cpp:449](../../source/app/platform/win/WinInput.cpp#L449) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1362"></a>

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

<a id="grupo-1363"></a>

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

<a id="grupo-1364"></a>

## cTransmission

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cTransmission::InjectHooks() | [source/game_sa/cTransmission.cpp:24](../../source/game_sa/cTransmission.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::DisplayGearRatios() | [source/game_sa/cTransmission.cpp:42](../../source/game_sa/cTransmission.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::DisplayGearRatios 0x006D0590 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::InitGearRatios() | [source/game_sa/cTransmission.cpp:60](../../source/game_sa/cTransmission.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::InitGearRatios 0x006D0460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateGearForSimpleCar(float speed, uint8& currentGear) | [source/game_sa/cTransmission.cpp:93](../../source/game_sa/cTransmission.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateGearForSimpleCar 0x006D0530 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateDriveAcceleration(const float& gasPedal, uint8& currentGear, float& gearChangeCount, float& velocity, float* a6, float* a7, uint8 allWheelsOnGround, uint8 handlingCheat) | [source/game_sa/cTransmission.cpp:112](../../source/game_sa/cTransmission.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateDriveAcceleration 0x006D05E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1365"></a>

## detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RegionValueOf(float p) | [source/game_sa/PathFind.cpp:150](../../source/game_sa/PathFind.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptWidthScale(uint8 width) | [source/game_sa/PathFind.cpp:604](../../source/game_sa/PathFind.cpp#L604) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptOffsetPos(const CVector& nodePos, float px, float py, float scale) | [source/game_sa/PathFind.cpp:607](../../source/game_sa/PathFind.cpp#L607) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCoorsOfRegion(size_t p, size_t nareas) | [source/game_sa/PathFind.cpp:1551](../../source/game_sa/PathFind.cpp#L1551) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1366"></a>

## details

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * CPtrNodeDoubleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:4](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeDoubleLink__operator_delete(void* data, size_t sz) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:9](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPtrNodeSingleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeSingleLink.cpp:4](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeSingleLink__operator_delete(void* data, size_t sz) | [source/game_sa/Core/PtrNodeSingleLink.cpp:9](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1367"></a>

## details::PtrListIterator

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PtrListIterator(NodeType* node) | [source/game_sa/Core/PtrList.h:23](../../source/game_sa/Core/PtrList.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*() | [source/game_sa/Core/PtrList.h:31](../../source/game_sa/Core/PtrList.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-&gt;() | [source/game_sa/Core/PtrList.h:32](../../source/game_sa/Core/PtrList.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator++() | [source/game_sa/Core/PtrList.h:34](../../source/game_sa/Core/PtrList.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator++(int) | [source/game_sa/Core/PtrList.h:41](../../source/game_sa/Core/PtrList.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const PtrListIterator& a, const PtrListIterator& b) | [source/game_sa/Core/PtrList.h:47](../../source/game_sa/Core/PtrList.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1368"></a>

## notsa

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::InjectWindowedModeHooks() | [source/app/platform/win/WindowedMode.cpp:1879](../../source/app/platform/win/WindowedMode.cpp#L1879) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
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

<a id="grupo-1369"></a>

## notsa::Logging

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::Logging::Logging() | [source/app/app_debug.cpp:276](../../source/app/app_debug.cpp#L276) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::~Logging() | [source/app/app_debug.cpp:300](../../source/app/app_debug.cpp#L300) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::Shutdown() | [source/app/app_debug.cpp:304](../../source/app/app_debug.cpp#L304) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::Create(std::string name, std::optional&lt;spdlog::level::level_enum&gt; level) -&gt; notsa::log_ptr | [source/app/app_debug.cpp:316](../../source/app/app_debug.cpp#L316) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1370"></a>

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

<a id="grupo-1371"></a>

## notsa::SDLWrapper

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatformWindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) | [source/app/platform/SDLWrapper.cpp:16](../../source/app/platform/SDLWrapper.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| PumpEvents(HWND, UINT, UINT_PTR, DWORD) | [source/app/platform/SDLWrapper.cpp:31](../../source/app/platform/SDLWrapper.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetWindow() | [source/app/platform/SDLWrapper.cpp:36](../../source/app/platform/SDLWrapper.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize() | [source/app/platform/SDLWrapper.cpp:40](../../source/app/platform/SDLWrapper.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| Terminate() | [source/app/platform/SDLWrapper.cpp:68](../../source/app/platform/SDLWrapper.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessEvents() | [source/app/platform/SDLWrapper.cpp:85](../../source/app/platform/SDLWrapper.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1372"></a>

## notsa::WindowedMode

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetDesktopRes() | [source/app/platform/win/WindowedMode.cpp:166](../../source/app/platform/win/WindowedMode.cpp#L166) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MainCameraRebuildRaster(void* camera) | [source/app/platform/win/WindowedMode.cpp:174](../../source/app/platform/win/WindowedMode.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038) |
| AdjustPresentParams(D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:255](../../source/app/platform/win/WindowedMode.cpp#L255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C); Posible dato fijo Windowed: source/app/platform/win/WinPlatform.h:44 (0x00C920CC); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| HookDirect3DDeviceReplacerSA() noexcept | [source/app/platform/win/WindowedMode.cpp:411](../../source/app/platform/win/WindowedMode.cpp#L411) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: WindowedModeHooks::notsa::WindowedMode::HookDirect3DDeviceReplacerSA 0x007F6781 | 14 / 1 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AdjustVideoModeOnResize(int32 w, int32 h) | [source/app/platform/win/WindowedMode.cpp:481](../../source/app/platform/win/WindowedMode.cpp#L481) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |

<a id="grupo-1373"></a>

## notsa::WindowedMode::D3D9ProxyDevice

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Proxy_Reset(LPDIRECT3DDEVICE9 self, D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:348](../../source/app/platform/win/WindowedMode.cpp#L348) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |
| Proxy_SetViewport(LPDIRECT3DDEVICE9 self, D3DVIEWPORT9* vp) | [source/app/platform/win/WindowedMode.cpp:375](../../source/app/platform/win/WindowedMode.cpp#L375) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize(LPDIRECT3DDEVICE9 dev) | [source/app/platform/win/WindowedMode.cpp:381](../../source/app/platform/win/WindowedMode.cpp#L381) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1374"></a>

## notsa::WindowedMode::PosChangeNoRecursion

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:158](../../source/app/platform/win/WindowedMode.cpp#L158) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:161](../../source/app/platform/win/WindowedMode.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1375"></a>

## notsa::WindowedMode::ScopedRestoreWindowInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScopedRestoreWindowInfo() noexcept | [source/app/platform/win/WindowedMode.cpp:309](../../source/app/platform/win/WindowedMode.cpp#L309) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C) |
| ~ScopedRestoreWindowInfo() noexcept | [source/app/platform/win/WindowedMode.cpp:325](../../source/app/platform/win/WindowedMode.cpp#L325) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C) |

<a id="grupo-1376"></a>

## notsa::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VerifyMacroImpl(bool result) | [source/app/app_debug.h:13](../../source/app/app_debug.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddressOfFunction(T fn) | [source/game_sa/Scripts/CommandParser/Parser.hpp:103](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOPCommandHandler(Args... args) | [source/game_sa/Scripts/CommandParser/Parser.hpp:109](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1377"></a>

## notsa::details::PtrListDoubleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListDoubleLink.h:19](../../source/game_sa/Core/PtrListDoubleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListDoubleLink.h:33](../../source/game_sa/Core/PtrListDoubleLink.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListDoubleLink.h:60](../../source/game_sa/Core/PtrListDoubleLink.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1378"></a>

## notsa::details::PtrListSingleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListSingleLink.h:19](../../source/game_sa/Core/PtrListSingleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListSingleLink.h:28](../../source/game_sa/Core/PtrListSingleLink.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListSingleLink.h:47](../../source/game_sa/Core/PtrListSingleLink.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1379"></a>

## notsa::envmap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PoolNew(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:104](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolDelete(Pool* pool, const void* obj) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:123](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1380"></a>

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
