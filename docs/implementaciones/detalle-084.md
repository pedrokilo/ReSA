<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 927 grupos / 11134 funciones reconstruidas; 11134 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1661"></a>

## WinInput

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CreateInput() | [source/app/platform/win/WinInput.cpp:32](../../source/app/platform/win/WinInput.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialise() | [source/app/platform/win/WinInput.cpp:45](../../source/app/platform/win/WinInput.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198) |
| InputShutdown() | [source/app/platform/win/WinInput.cpp:61](../../source/app/platform/win/WinInput.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Input::InputShutdown 0x00747000 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReleaseInput() | [source/app/platform/win/WinInput.cpp:68](../../source/app/platform/win/WinInput.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Shutdown() | [source/app/platform/win/WinInput.cpp:75](../../source/app/platform/win/WinInput.cpp#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| diPadSetRanges(LPDIRECTINPUTDEVICE8 dev, DWORD padNum) | [source/app/platform/win/WinInput.cpp:80](../../source/app/platform/win/WinInput.cpp#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144) |
| diMouseInit(bool exclusive) | [source/app/platform/win/WinInput.cpp:143](../../source/app/platform/win/WinInput.cpp#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| diPadEnumerate() | [source/app/platform/win/WinInput.cpp:155](../../source/app/platform/win/WinInput.cpp#L155) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Input::diPadEnumerate 0x007477C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Colaborador por dirección 0x00746D80: fuera del catálogo consultado; ruta efectiva sin resolver; Conversión de dirección fija: 0x00747020; Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| diPadInitBody() | [source/app/platform/win/WinInput.cpp:187](../../source/app/platform/win/WinInput.cpp#L187) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144); Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| diPadInit() | [source/app/platform/win/WinInput.cpp:233](../../source/app/platform/win/WinInput.cpp#L233) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Input::diPadInit 0x007485C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumDevicesCallback(LPCDIDEVICEINSTANCEA inst, LPVOID) | [source/app/platform/win/WinInput.cpp:246](../../source/app/platform/win/WinInput.cpp#L246) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __stdcall GetMouseSetUpExe(CMouseControllerState* out) | [source/app/platform/win/WinInput.cpp:285](../../source/app/platform/win/WinInput.cpp#L285) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Input::GetMouseSetUpExe 0x0053F2D0 | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| GetMouseSetUp() | [source/app/platform/win/WinInput.cpp:332](../../source/app/platform/win/WinInput.cpp#L332) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InjectPadHooks() | [source/app/platform/win/WinInput.cpp:340](../../source/app/platform/win/WinInput.cpp#L340) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InjectHooks() | [source/app/platform/win/WinInput.cpp:356](../../source/app/platform/win/WinInput.cpp#L356) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsKeyPressed(unsigned int keyCode) | [source/app/platform/win/WinInput.cpp:367](../../source/app/platform/win/WinInput.cpp#L367) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsKeyDown(unsigned int keyCode) | [source/app/platform/win/WinInput.cpp:371](../../source/app/platform/win/WinInput.cpp#L371) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1662"></a>

## cBuoyancy

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cBuoyancy::InjectHooks() | [source/game_sa/Buoyancy.cpp:50](../../source/game_sa/Buoyancy.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::cBuoyancy() | [source/game_sa/Buoyancy.cpp:70](../../source/game_sa/Buoyancy.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: cBuoyancy::Constructor 0x006C2740 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::ProcessBuoyancy(CPhysical* entity, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) | [source/game_sa/Buoyancy.cpp:76](../../source/game_sa/Buoyancy.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::ProcessBuoyancy 0x006C3EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::ProcessBuoyancyBoat(CVehicle* vehicle, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce, bool bUnderwater) | [source/game_sa/Buoyancy.cpp:116](../../source/game_sa/Buoyancy.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::ProcessBuoyancyBoat 0x006C3030 | 0 / 0 | Posible dato fijo afBoatVolumeDistribution: source/game_sa/Buoyancy.h:61 (0x008D32CC); Posible dato fijo afBoatVolumeDistributionDinghy: source/game_sa/Buoyancy.h:64 (0x008D3338); Posible dato fijo afBoatVolumeDistributionSail: source/game_sa/Buoyancy.h:63 (0x008D3314); Posible dato fijo afBoatVolumeDistributionSpeed: source/game_sa/Buoyancy.h:65 (0x008D335C); Posible dato fijo fBuoyancyBoatNormalWeight: source/game_sa/Buoyancy.cpp:27 (0x008D3384); Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::CalcBuoyancyForce(CPhysical* entity, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) | [source/game_sa/Buoyancy.cpp:204](../../source/game_sa/Buoyancy.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::CalcBuoyancyForce 0x006C2750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::PreCalcSetup(CPhysical* entity, float fBuoyancy) | [source/game_sa/Buoyancy.cpp:225](../../source/game_sa/Buoyancy.cpp#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::PreCalcSetup 0x006C2B90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::AddSplashParticles(CPhysical* entity, CVector vecFrom, CVector vecTo, CVector vecSplashDir, uint8 bReduceParticleSize) | [source/game_sa/Buoyancy.cpp:334](../../source/game_sa/Buoyancy.cpp#L334) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::AddSplashParticles 0x006C34E0 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo fBuoyancySplashMaxSpeed: source/game_sa/Buoyancy.cpp:29 (0x008D3388); Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |
| cBuoyancy::SimpleCalcBuoyancy(CPhysical* entity) | [source/game_sa/Buoyancy.cpp:431](../../source/game_sa/Buoyancy.cpp#L431) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::SimpleCalcBuoyancy 0x006C3B00 | 0 / 0 | Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::SimpleSumBuoyancyData(CVector* vecWaterOffset, tWaterLevel ePointState) | [source/game_sa/Buoyancy.cpp:514](../../source/game_sa/Buoyancy.cpp#L514) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::SimpleSumBuoyancyData 0x006C2970 | 0 / 0 | Posible dato fijo calcStruct: source/game_sa/Buoyancy.h:60 (0x00C1C858); Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::FindWaterLevel(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState) | [source/game_sa/Buoyancy.cpp:555](../../source/game_sa/Buoyancy.cpp#L555) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::FindWaterLevel 0x006C2810 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::FindWaterLevelNorm(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState, CVector* outVecNormal) | [source/game_sa/Buoyancy.cpp:575](../../source/game_sa/Buoyancy.cpp#L575) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::FindWaterLevelNorm 0x006C28C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Buoyancy.h:85](../../source/game_sa/Buoyancy.h#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::Constructor 0x006C2740 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1663"></a>

## cHandlingDataMgr

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cHandlingDataMgr::InjectHooks() | [source/game_sa/cHandlingDataMgr.cpp:39](../../source/game_sa/cHandlingDataMgr.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::LoadHandlingData() | [source/game_sa/cHandlingDataMgr.cpp:214](../../source/game_sa/cHandlingDataMgr.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::LoadHandlingData 0x005BD830 | 0 / 0 | Posible dato fijo VehicleNames: source/game_sa/cHandlingDataMgr.cpp:24 (0x008D3978); Posible dato fijo m_vehicleAnimGroups: source/game_sa/VehicleAnimGroupData.h:146 (0x00C1CDC0) |
| cHandlingDataMgr::Initialise() | [source/game_sa/cHandlingDataMgr.cpp:370](../../source/game_sa/cHandlingDataMgr.cpp#L370) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: cHandlingDataMgr::Initialise 0x005BF3D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetFlyingPointer(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:381](../../source/game_sa/cHandlingDataMgr.cpp#L381) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetFlyingPointer 0x006F52D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetBoatPointer(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:389](../../source/game_sa/cHandlingDataMgr.cpp#L389) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetBoatPointer 0x006F5300 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetVehiclePointer(uint32 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:396](../../source/game_sa/cHandlingDataMgr.cpp#L396) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetBikeHandlingPointer(uint32 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:400](../../source/game_sa/cHandlingDataMgr.cpp#L400) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::HasFrontWheelDrive(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:407](../../source/game_sa/cHandlingDataMgr.cpp#L407) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::HasFrontWheelDrive 0x006A0480 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::HasRearWheelDrive(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:412](../../source/game_sa/cHandlingDataMgr.cpp#L412) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::HasRearWheelDrive 0x006A04B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::GetHandlingId(const char* nameToFind) | [source/game_sa/cHandlingDataMgr.cpp:418](../../source/game_sa/cHandlingDataMgr.cpp#L418) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetHandlingId 0x006F4FD0 | 0 / 0 | Posible dato fijo VehicleNames: source/game_sa/cHandlingDataMgr.cpp:24 (0x008D3978) |
| cHandlingDataMgr::ConvertDataToWorldUnits(tHandlingData* h) | [source/game_sa/cHandlingDataMgr.cpp:445](../../source/game_sa/cHandlingDataMgr.cpp#L445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertDataToWorldUnits 0x006F5010 | 0 / 0 | Posible dato fijo s_HandlingAccelConst: source/game_sa/cHandlingDataMgr.cpp:430 (0x00C2B9B4); Posible dato fijo s_HandlingVelocityConst: source/game_sa/cHandlingDataMgr.cpp:431 (0x00C2B9BC) |
| cHandlingDataMgr::ConvertDataToGameUnits(tHandlingData* h) | [source/game_sa/cHandlingDataMgr.cpp:460](../../source/game_sa/cHandlingDataMgr.cpp#L460) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertDataToGameUnits 0x006F5080 | 0 / 0 | Posible dato fijo s_HandlingAccelConst: source/game_sa/cHandlingDataMgr.cpp:430 (0x00C2B9B4); Posible dato fijo s_HandlingVelocityConst: source/game_sa/cHandlingDataMgr.cpp:431 (0x00C2B9BC) |
| cHandlingDataMgr::ConvertBikeDataToWorldUnits(tBikeHandlingData* bikeHandling) | [source/game_sa/cHandlingDataMgr.cpp:472](../../source/game_sa/cHandlingDataMgr.cpp#L472) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertBikeDataToWorldUnits 0x006F5240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::ConvertBikeDataToGameUnits(tBikeHandlingData* bikeHandling) | [source/game_sa/cHandlingDataMgr.cpp:480](../../source/game_sa/cHandlingDataMgr.cpp#L480) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertBikeDataToGameUnits 0x006F5290 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::FindExactWord(const char* name, const char* nameTable, uint32 entrySize, uint32 entryCount) | [source/game_sa/cHandlingDataMgr.cpp:488](../../source/game_sa/cHandlingDataMgr.cpp#L488) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::FindExactWord 0x006F4F30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1664"></a>

## cTransmission

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cTransmission::InjectHooks() | [source/game_sa/cTransmission.cpp:28](../../source/game_sa/cTransmission.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::DisplayGearRatios() | [source/game_sa/cTransmission.cpp:48](../../source/game_sa/cTransmission.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::DisplayGearRatios 0x006D0590 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::InitGearRatios() | [source/game_sa/cTransmission.cpp:66](../../source/game_sa/cTransmission.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::InitGearRatios 0x006D0460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateGearForSimpleCar(float speed, uint8& currentGear) | [source/game_sa/cTransmission.cpp:99](../../source/game_sa/cTransmission.cpp#L99) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateGearForSimpleCar 0x006D0530 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateDriveAcceleration(const float& gasPedal, uint8& currentGear, float& gearChangeCount, float& velocity, float* a6, float* a7, uint8 allWheelsOnGround, uint8 handlingCheat) | [source/game_sa/cTransmission.cpp:118](../../source/game_sa/cTransmission.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateDriveAcceleration 0x006D05E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1665"></a>

## detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RegionValueOf(float p) | [source/game_sa/PathFind.cpp:166](../../source/game_sa/PathFind.cpp#L166) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeRand() | [source/game_sa/PathFind.cpp:173](../../source/game_sa/PathFind.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptWidthScale(uint8 width) | [source/game_sa/PathFind.cpp:990](../../source/game_sa/PathFind.cpp#L990) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptOffsetPos(const CVector& nodePos, float px, float py, float scale) | [source/game_sa/PathFind.cpp:993](../../source/game_sa/PathFind.cpp#L993) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCoorsOfRegion(size_t p, size_t nareas) | [source/game_sa/PathFind.cpp:2307](../../source/game_sa/PathFind.cpp#L2307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1666"></a>

## details

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * CPtrNodeDoubleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:6](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPtrNodeDoubleLinkPool::OperatorNew 0x005523C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeDoubleLink__operator_delete(void* data) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:13](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPtrNodeDoubleLinkPool::OperatorDelete 0x005523D0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrListDoubleLink__FlushExe(void* list, void* /*edx*/) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:23](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPtrNodeDoubleLinkPool::Flush 0x00552470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeDoubleLink__AddToListExe(void* node, void* /*edx*/, void** head) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:56](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPtrNodeDoubleLinkPool::AddToList 0x005329A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPtrNodeSingleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeSingleLink.cpp:6](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPtrNodeSingleLinkPool::OperatorNew 0x00552380 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeSingleLink__operator_delete(void* data) | [source/game_sa/Core/PtrNodeSingleLink.cpp:13](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPtrNodeSingleLinkPool::OperatorDelete 0x00552390 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrListSingleLink__FlushExe(void* list, void* /*edx*/) | [source/game_sa/Core/PtrNodeSingleLink.cpp:23](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPtrNodeSingleLinkPool::Flush 0x00552400 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrListSingleLink__FlushJumpExe(void* list, void* edx) | [source/game_sa/Core/PtrNodeSingleLink.cpp:56](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPtrNodeSingleLinkPool::FlushJump 0x0043E1A0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1667"></a>

## details::PtrListIterator

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PtrListIterator(NodeType* node) | [source/game_sa/Core/PtrList.h:23](../../source/game_sa/Core/PtrList.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*() | [source/game_sa/Core/PtrList.h:31](../../source/game_sa/Core/PtrList.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-&gt;() | [source/game_sa/Core/PtrList.h:32](../../source/game_sa/Core/PtrList.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator++() | [source/game_sa/Core/PtrList.h:34](../../source/game_sa/Core/PtrList.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator++(int) | [source/game_sa/Core/PtrList.h:41](../../source/game_sa/Core/PtrList.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const PtrListIterator& a, const PtrListIterator& b) | [source/game_sa/Core/PtrList.h:47](../../source/game_sa/Core/PtrList.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1668"></a>

## notsa

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::InjectWindowedModeHooks() | [source/app/platform/win/WindowedMode.cpp:1878](../../source/app/platform/win/WindowedMode.cpp#L1878) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
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

<a id="grupo-1669"></a>

## notsa::Logging

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::Logging::Logging() | [source/app/app_debug.cpp:300](../../source/app/app_debug.cpp#L300) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::~Logging() | [source/app/app_debug.cpp:328](../../source/app/app_debug.cpp#L328) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::Shutdown() | [source/app/app_debug.cpp:332](../../source/app/app_debug.cpp#L332) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::Create(std::string name, std::optional&lt;spdlog::level::level_enum&gt; level) -&gt; notsa::log_ptr | [source/app/app_debug.cpp:344](../../source/app/app_debug.cpp#L344) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1670"></a>

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

<a id="grupo-1671"></a>

## notsa::SDLWrapper

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatformWindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) | [source/app/platform/SDLWrapper.cpp:16](../../source/app/platform/SDLWrapper.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| PumpEvents(HWND, UINT, UINT_PTR, DWORD) | [source/app/platform/SDLWrapper.cpp:31](../../source/app/platform/SDLWrapper.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetWindow() | [source/app/platform/SDLWrapper.cpp:36](../../source/app/platform/SDLWrapper.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize() | [source/app/platform/SDLWrapper.cpp:40](../../source/app/platform/SDLWrapper.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| Terminate() | [source/app/platform/SDLWrapper.cpp:68](../../source/app/platform/SDLWrapper.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessEvents() | [source/app/platform/SDLWrapper.cpp:85](../../source/app/platform/SDLWrapper.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1672"></a>

## notsa::WindowedMode

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetDesktopRes() | [source/app/platform/win/WindowedMode.cpp:165](../../source/app/platform/win/WindowedMode.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MainCameraRebuildRaster(void* camera) | [source/app/platform/win/WindowedMode.cpp:173](../../source/app/platform/win/WindowedMode.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038) |
| AdjustPresentParams(D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:254](../../source/app/platform/win/WindowedMode.cpp#L254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C); Posible dato fijo Windowed: source/app/platform/win/WinPlatform.h:44 (0x00C920CC); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| HookDirect3DDeviceReplacerSA() noexcept | [source/app/platform/win/WindowedMode.cpp:410](../../source/app/platform/win/WindowedMode.cpp#L410) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: WindowedModeHooks::notsa::WindowedMode::HookDirect3DDeviceReplacerSA 0x007F6781 | 14 / 1 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AdjustVideoModeOnResize(int32 w, int32 h) | [source/app/platform/win/WindowedMode.cpp:480](../../source/app/platform/win/WindowedMode.cpp#L480) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |

<a id="grupo-1673"></a>

## notsa::WindowedMode::D3D9ProxyDevice

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Proxy_Reset(LPDIRECT3DDEVICE9 self, D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:347](../../source/app/platform/win/WindowedMode.cpp#L347) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |
| Proxy_SetViewport(LPDIRECT3DDEVICE9 self, D3DVIEWPORT9* vp) | [source/app/platform/win/WindowedMode.cpp:374](../../source/app/platform/win/WindowedMode.cpp#L374) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize(LPDIRECT3DDEVICE9 dev) | [source/app/platform/win/WindowedMode.cpp:380](../../source/app/platform/win/WindowedMode.cpp#L380) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1674"></a>

## notsa::WindowedMode::PosChangeNoRecursion

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:157](../../source/app/platform/win/WindowedMode.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:160](../../source/app/platform/win/WindowedMode.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1675"></a>

## notsa::WindowedMode::ScopedRestoreWindowInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScopedRestoreWindowInfo() noexcept | [source/app/platform/win/WindowedMode.cpp:308](../../source/app/platform/win/WindowedMode.cpp#L308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C) |
| ~ScopedRestoreWindowInfo() noexcept | [source/app/platform/win/WindowedMode.cpp:324](../../source/app/platform/win/WindowedMode.cpp#L324) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C) |

<a id="grupo-1676"></a>

## notsa::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VerifyMacroImpl(bool result) | [source/app/app_debug.h:13](../../source/app/app_debug.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddressOfFunction(T fn) | [source/game_sa/Scripts/CommandParser/Parser.hpp:103](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOPCommandHandler(Args... args) | [source/game_sa/Scripts/CommandParser/Parser.hpp:109](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1677"></a>

## notsa::details::PtrListDoubleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListDoubleLink.h:19](../../source/game_sa/Core/PtrListDoubleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListDoubleLink.h:33](../../source/game_sa/Core/PtrListDoubleLink.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListDoubleLink.h:60](../../source/game_sa/Core/PtrListDoubleLink.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1678"></a>

## notsa::details::PtrListSingleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListSingleLink.h:19](../../source/game_sa/Core/PtrListSingleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListSingleLink.h:28](../../source/game_sa/Core/PtrListSingleLink.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListSingleLink.h:47](../../source/game_sa/Core/PtrListSingleLink.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1679"></a>

## notsa::envmap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PoolNew(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:104](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolDelete(Pool* pool, const void* obj) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:123](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1680"></a>

## notsa::pool_raw

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReleaseSlot(void* pool, const void* obj, int32 elementSize) | [source/game_sa/Core/PoolRaw.h:17](../../source/game_sa/Core/PoolRaw.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
