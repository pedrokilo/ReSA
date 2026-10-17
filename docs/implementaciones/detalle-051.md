<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1001"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetVehicleAudioSettings(eModelID model) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:283](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1002"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AudioLog10ScaledExe(float input, float factor) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:36](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSurfaceAudioGrass(eSurfaceType surface) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:57](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| IsSurfaceWet(eSurfaceType surface) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:61](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| GracefullyStopSound(CAESound*& sound) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:65](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StopAndForgetSound(CAESound*& sound) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:72](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNumOfContactWheels(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:78](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLerp(float lo, float hi, float t) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:91](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeInlinePiecewise5(float x, const float (&p)\[5\]\[2\]) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:98](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2Clamp01Exe(float r) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:117](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedalToRaw(float v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:122](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PlayVehicleSound(CAESoundManager::tSoundPlayParams p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:129](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AESoundManager: source/game_sa/Audio/Managers/AESoundManager.cpp:29 (0x00B62CB0) |
| PlayGenericSound( CAEVehicleAudioEntity* self, CAESound*& s, eSoundBankSlot slot, eSoundID& currSfx, eSoundID newSfx, float speed, float volume, float rollOff ) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:2050](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L2050) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftDistantSoundVolume(const CPhysical* v, float baseVol) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3473](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3473) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftCameraPoVFactor(const CPhysical* p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3486](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3486) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3491](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCRotorSpeedFactor(float p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3502](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPropSpeedFactor(float propSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3618](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3618) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePropSpeed(const CPlane* plane, float gas, float brake) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3627](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3627) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePlanePropFreq(CAEVehicleAudioEntity::tVehicleParams& vp, float ac) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3634](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3634) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeClamp01(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3653](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3653) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropSpeedFactor(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3658](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3658) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropOrientation(const CEntity* e, float& R, float& Q) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3664](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3664) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropCamFactor(const CVector& entityPos, const CEntity* vehicle) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3675](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3675) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropDistantVolume(const CVector& entityPos, float pf) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3684](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3684) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliVolFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3915](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3915) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliFreqFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3919](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3919) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliRotorSpeed(const CVehicle* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3924](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3924) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliTilt(const CEntity* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3928](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3928) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliDamageFreq(const CVehicle* v, float f0) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3933](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3933) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorSpeed(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3943](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3943) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVolumeFactorForHeliRotor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3948](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3948) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorFreqFactor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3959](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3959) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliForwardTilt(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3970](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3970) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4371](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4371) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeed(const CTrain* train) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4496](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4496) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainAbsClamp01(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4508](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4508) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeedFactor(const CTrain* train, float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4515](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4515) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainDummyAcFactor(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4525](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4525) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessBicycleAudio(CAEVehicleAudioEntity& self, CAEVehicleAudioEntity::tVehicleParams& params, bool player) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5008](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5008) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2GetFreqForIdle_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, float ratio) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5458](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5458) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3DriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, float revs, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5470](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5470) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3NonDriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5474](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5474) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehAudioPlayerVolume_KeepEcx(const CAEVehicleAudioEntity* self, uint32 /*entryEdx*/, CAEVehicleAudioEntity::tVehicleParams* vp, CAEVehicleAudioEntity::eVehicleEngineSoundType st) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5481](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5481) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1003"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEVehicleAudioEntity | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:42](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L42) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_State: source/game_sa/Conversations/PedToPlayerConversations.h:25 (0x00969A20) |
| GetVehicle() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:754](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L754) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEngineSound(eVehicleEngineSoundType st) const noexcept | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:760](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L760) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:849](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L849) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Destructor() | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:864](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L864) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(CAEVehicleAudioEntity::eAEState s) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:880](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L880) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(CAEVehicleAudioEntity::eAircraftSoundType s) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:899](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L899) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(CAEVehicleAudioEntity::eCarEngineSoundType st) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:913](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L913) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(eAERadioType rt) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:932](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L932) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumToString(eAEVehicleSoundType v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.h:943](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.h#L943) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1004"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEWaterCannonAudioEntity | [source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.h:14](../../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.h#L14) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1005"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWeaponAudioEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEWeaponAudioEntity | [source/game_sa/Audio/Entities/AEWeaponAudioEntity.h:20](../../source/game_sa/Audio/Entities/AEWeaponAudioEntity.h#L20) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1006"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DefaultVolumeInt(eAudioEvents event) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:48](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FadeWeatherSoundOut(CAESound* sound) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:53](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1007"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWeatherAudioEntity.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEWeatherAudioEntity | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.h:11](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1008"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleDoorType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleDoorType v) | [source/game_sa/Audio/Enums/eAEVehicleDoorType.h:14](../../source/game_sa/Audio/Enums/eAEVehicleDoorType.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1009"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleHornType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleHornType v) | [source/game_sa/Audio/Enums/eAEVehicleHornType.h:25](../../source/game_sa/Audio/Enums/eAEVehicleHornType.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1010"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eBassSetting.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eBassSetting v) | [source/game_sa/Audio/Enums/eBassSetting.h:14](../../source/game_sa/Audio/Enums/eBassSetting.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1011"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBank.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBank v) | [source/game_sa/Audio/Enums/eSoundBank.h:740](../../source/game_sa/Audio/Enums/eSoundBank.h#L740) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1012"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBankSlot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBankSlot v) | [source/game_sa/Audio/Enums/eSoundBankSlot.h:87](../../source/game_sa/Audio/Enums/eSoundBankSlot.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1013"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioChannel.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEAudioChannel | [source/game_sa/Audio/Hardware/AEAudioChannel.h:15](../../source/game_sa/Audio/Hardware/AEAudioChannel.h#L15) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_bPaused: source/game_sa/LoadingScreen.h:35 (0x00BAB31A) |
| GetVolume() const | [source/game_sa/Audio/Hardware/AEAudioChannel.h:67](../../source/game_sa/Audio/Hardware/AEAudioChannel.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBufferPlaying() const | [source/game_sa/Audio/Hardware/AEAudioChannel.h:69](../../source/game_sa/Audio/Hardware/AEAudioChannel.h#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1014"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeGainToDecibels(const float* gain) | [source/game_sa/Audio/Hardware/AEAudioHardware.cpp:436](../../source/game_sa/Audio/Hardware/AEAudioHardware.cpp#L436) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1015"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyFromAESound(const CAESound& sound) | [source/game_sa/Audio/Hardware/AEAudioHardware.h:29](../../source/game_sa/Audio/Hardware/AEAudioHardware.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1016"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEStaticChannel.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEStaticChannel | [source/game_sa/Audio/Hardware/AEStaticChannel.h:4](../../source/game_sa/Audio/Hardware/AEStaticChannel.h#L4) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1017"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEStreamingChannel.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEStreamingChannel | [source/game_sa/Audio/Hardware/AEStreamingChannel.h:17](../../source/game_sa/Audio/Hardware/AEStreamingChannel.h#L17) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo AESmoothFadeThread: source/game_sa/Audio/AESmoothFadeThread.cpp:26 (0x00B608D0) |
| * Constructor(IDirectSound* directSound, uint16 channelId) | [source/game_sa/Audio/Hardware/AEStreamingChannel.h:77](../../source/game_sa/Audio/Hardware/AEStreamingChannel.h#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Destructor() | [source/game_sa/Audio/Hardware/AEStreamingChannel.h:82](../../source/game_sa/Audio/Hardware/AEStreamingChannel.h#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DirectSoundBufferFadeToSilence() | [source/game_sa/Audio/Hardware/AEStreamingChannel.h:87](../../source/game_sa/Audio/Hardware/AEStreamingChannel.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AESmoothFadeThread: source/game_sa/Audio/AESmoothFadeThread.cpp:26 (0x00B608D0) |

<a id="grupo-1018"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEDataStream.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEDataStream | [source/game_sa/Audio/Loaders/AEDataStream.h:13](../../source/game_sa/Audio/Loaders/AEDataStream.h#L13) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1019"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEMFDecoder.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEStreamingDecoder | [source/game_sa/Audio/Loaders/AEMFDecoder.h:12](../../source/game_sa/Audio/Loaders/AEMFDecoder.h#L12) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1020"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SecondsToMs(double seconds) | [source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp:34](../../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
