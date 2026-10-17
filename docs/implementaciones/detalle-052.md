<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1021"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DefaultVolumeInt(eAudioEvents event) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:48](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FadeWeatherSoundOut(CAESound* sound) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:53](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1022"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleDoorType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleDoorType v) | [source/game_sa/Audio/Enums/eAEVehicleDoorType.h:14](../../source/game_sa/Audio/Enums/eAEVehicleDoorType.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1023"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleHornType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleHornType v) | [source/game_sa/Audio/Enums/eAEVehicleHornType.h:25](../../source/game_sa/Audio/Enums/eAEVehicleHornType.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1024"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eBassSetting.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eBassSetting v) | [source/game_sa/Audio/Enums/eBassSetting.h:14](../../source/game_sa/Audio/Enums/eBassSetting.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1025"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBank.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBank v) | [source/game_sa/Audio/Enums/eSoundBank.h:740](../../source/game_sa/Audio/Enums/eSoundBank.h#L740) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1026"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBankSlot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBankSlot v) | [source/game_sa/Audio/Enums/eSoundBankSlot.h:87](../../source/game_sa/Audio/Enums/eSoundBankSlot.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1027"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeGainToDecibels(const float* gain) | [source/game_sa/Audio/Hardware/AEAudioHardware.cpp:436](../../source/game_sa/Audio/Hardware/AEAudioHardware.cpp#L436) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1028"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyFromAESound(const CAESound& sound) | [source/game_sa/Audio/Hardware/AEAudioHardware.h:29](../../source/game_sa/Audio/Hardware/AEAudioHardware.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1029"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SecondsToMs(double seconds) | [source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp:34](../../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1030"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEWaveDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BytesToMs(uint32 bytes, uint32 bytesPerSecond) | [source/game_sa/Audio/Loaders/AEWaveDecoder.cpp:32](../../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1031"></a>

## Globales y helpers: source/game_sa/Audio/Managers/AERadioTrackManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResetCommon(CAERadioTrackManager& self) | [source/game_sa/Audio/Managers/AERadioTrackManager.cpp:103](../../source/game_sa/Audio/Managers/AERadioTrackManager.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_nAdvertIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:169 (0x00B620C0); Posible dato fijo m_nDJBanterIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:168 (0x00B61D78); Posible dato fijo m_nIdentIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:170 (0x00B62980); Posible dato fijo m_nMusicTrackIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:171 (0x00B62B40); Posible dato fijo m_nSpecialDJBanterIndex: source/game_sa/Audio/Managers/AERadioTrackManager.h:199 (0x00B62C72); Posible dato fijo m_nSpecialDJBanterPending: source/game_sa/Audio/Managers/AERadioTrackManager.h:200 (0x00B62C73) |
| CalculateRadioVolume355() | [source/game_sa/Audio/Managers/AERadioTrackManager.cpp:599](../../source/game_sa/Audio/Managers/AERadioTrackManager.cpp#L599) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAERadioTrackManager::UpdateRadioVolumes 0x004EA010 | 0 / 0 | Posible dato fijo AEAudioHardware: source/game_sa/Audio/Hardware/AEAudioHardware.cpp:30 (0x00B5F8B8) |

<a id="grupo-1032"></a>

## Globales y helpers: source/game_sa/Audio/OpenAL/OALBase.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OALCheckErrors(std::string_view file, int32 line) | [source/game_sa/Audio/OpenAL/OALBase.cpp:26](../../source/game_sa/Audio/OpenAL/OALBase.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1033"></a>

## Globales y helpers: source/game_sa/Birds.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMaxDistance(eBirdsBiome biome) | [source/game_sa/Birds.cpp:65](../../source/game_sa/Birds.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1034"></a>

## Globales y helpers: source/game_sa/BouncingPanel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_MV_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/BouncingPanel.cpp:52](../../source/game_sa/BouncingPanel.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3_VM_59C810(const CVector& v, const CMatrix& m) | [source/game_sa/BouncingPanel.cpp:61](../../source/game_sa/BouncingPanel.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyByPow(CVector& pos, float damp, float step) | [source/game_sa/BouncingPanel.cpp:71](../../source/game_sa/BouncingPanel.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1035"></a>

## Globales y helpers: source/game_sa/BreakObject_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/BreakObject_c.cpp:49](../../source/game_sa/BreakObject_c.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1036"></a>

## Globales y helpers: source/game_sa/BrightLights.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BrightFtolByte(float f) | [source/game_sa/BrightLights.cpp:34](../../source/game_sa/BrightLights.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1037"></a>

## Globales y helpers: source/game_sa/BulletTraces.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlayFrontEndSoundForTrace(const CVector& fromWorldSpace, const CVector& toWorldSpace) | [source/game_sa/BulletTraces.cpp:62](../../source/game_sa/BulletTraces.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028) |

<a id="grupo-1038"></a>

## Globales y helpers: source/game_sa/Buoyancy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BuoyancyMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Buoyancy.cpp:32](../../source/game_sa/Buoyancy.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BuoyancyMultiplyPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Buoyancy.cpp:41](../../source/game_sa/Buoyancy.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1039"></a>

## Globales y helpers: source/game_sa/C_PcSave.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GxtCharToSafeAscii(const GxtChar* src) | [source/game_sa/C_PcSave.cpp:54](../../source/game_sa/C_PcSave.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1040"></a>

## Globales y helpers: source/game_sa/Cam.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CamExeFloat(uintptr_t address) | [source/game_sa/Cam.cpp:57](../../source/game_sa/Cam.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAddr(T value) | [source/game_sa/Cam.cpp:63](../../source/game_sa/Cam.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamExeSin(float angle) | [source/game_sa/Cam.cpp:72](../../source/game_sa/Cam.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamExeCos(float angle) | [source/game_sa/Cam.cpp:77](../../source/game_sa/Cam.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMultiplyMatrixVector(const CMatrix& m, const CVector& v) | [source/game_sa/Cam.cpp:84](../../source/game_sa/Cam.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMultiplyVectorMatrix(const CVector& v, const CMatrix& m) | [source/game_sa/Cam.cpp:93](../../source/game_sa/Cam.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WrapAngleKeepEdx(float* angle) | [source/game_sa/Cam.cpp:151](../../source/game_sa/Cam.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LookSideOrbit(float directionX,float directionY,float sign,float distance,float targetX,float targetY,float& outputX,float& outputY) | [source/game_sa/Cam.cpp:573](../../source/game_sa/Cam.cpp#L573) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::LookRight 0x00520E40 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamVelocityInverseStep(float step) | [source/game_sa/Cam.cpp:684](../../source/game_sa/Cam.cpp#L684) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamVehicleTargetSpeed(float forwardX,float forwardY,float velocityX,float velocityY) | [source/game_sa/Cam.cpp:691](../../source/game_sa/Cam.cpp#L691) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamVehicleSmoothedSpeed(float previous,float target) | [source/game_sa/Cam.cpp:697](../../source/game_sa/Cam.cpp#L697) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamPowerComplement(float base,float step) | [source/game_sa/Cam.cpp:704](../../source/game_sa/Cam.cpp#L704) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DeadCamWave(float elapsed, const CVector& side, const CVector& target) | [source/game_sa/Cam.cpp:914](../../source/game_sa/Cam.cpp#L914) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAimHeading355(float frontX, float frontY, float headingOffset) | [source/game_sa/Cam.cpp:1234](../../source/game_sa/Cam.cpp#L1234) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAimOffsets355(float fov, float& horizontal, float& vertical) | [source/game_sa/Cam.cpp:1240](../../source/game_sa/Cam.cpp#L1240) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAimTargetAngles355(const CVector& target, const CVector& focal, float& horizontal, float& flat, float& pitch) | [source/game_sa/Cam.cpp:1246](../../source/game_sa/Cam.cpp#L1246) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAimProjection355(CCam* cam, const CVector& focal, uint32 rowOffset) | [source/game_sa/Cam.cpp:1254](../../source/game_sa/Cam.cpp#L1254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAimMeleeBlend355(float desiredHorizontal,float desiredVertical,float desiredBlend) | [source/game_sa/Cam.cpp:1261](../../source/game_sa/Cam.cpp#L1261) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAimMouseAngles355(CCam* cam) | [source/game_sa/Cam.cpp:1266](../../source/game_sa/Cam.cpp#L1266) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamAimVehicleResetPitch355(float clampedZ,float initialAlpha) | [source/game_sa/Cam.cpp:1270](../../source/game_sa/Cam.cpp#L1270) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamPower(float base, float step) | [source/game_sa/Cam.cpp:1513](../../source/game_sa/Cam.cpp#L1513) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MouseCameraStick(const CPad* pad, bool vertical) | [source/game_sa/Cam.cpp:1519](../../source/game_sa/Cam.cpp#L1519) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TwoPlayerCameraVisible(const CVector& source) | [source/game_sa/Cam.cpp:1523](../../source/game_sa/Cam.cpp#L1523) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TwoPlayerCameraGeometry(CCam* cam, float beta, CVector& source, CVector& front, CVector& target) | [source/game_sa/Cam.cpp:1530](../../source/game_sa/Cam.cpp#L1530) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TwoPlayerChangeCamera(const CPad* pad) | [source/game_sa/Cam.cpp:1553](../../source/game_sa/Cam.cpp#L1553) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamShootingSourceZ355(float alpha, float radius, float targetZ) | [source/game_sa/Cam.cpp:1661](../../source/game_sa/Cam.cpp#L1661) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::Process_Cam_TwoPlayer_InCarAndShooting 0x00519810 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamShootingOrbit355(CCam* cam, const CVector& target, float excess) | [source/game_sa/Cam.cpp:1668](../../source/game_sa/Cam.cpp#L1668) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamShootingTilt355(CCam* cam, CVehicle* vehicle, float wantedAlpha) | [source/game_sa/Cam.cpp:1676](../../source/game_sa/Cam.cpp#L1676) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamShootingFov355(CCam* cam, CVehicle* vehicle) | [source/game_sa/Cam.cpp:1682](../../source/game_sa/Cam.cpp#L1682) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamTwoCarsBlend355(CCam* cam,const CVector* firstPos,const CVector* secondPos,const CVector* firstSource,const CVector* secondSource,const CVector* towardSecond,const CVector* towardFirst,const CVector* side,float blend) | [source/game_sa/Cam.cpp:1793](../../source/game_sa/Cam.cpp#L1793) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::Process_Cam_TwoPlayer_Separate_Cars 0x00513510 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamTwoCarsSources355(const CVector& first, const CVector& second, const CVector& between, float firstHeight, float secondHeight, CVector& firstSource, CVector& secondSource) | [source/game_sa/Cam.cpp:1803](../../source/game_sa/Cam.cpp#L1803) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BirdyCameraEase(float ratio, float from, float to) | [source/game_sa/Cam.cpp:1916](../../source/game_sa/Cam.cpp#L1916) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::Process_DW_BirdyCam 0x0051B850 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BirdyLampHeight355(float pointZ,float height,int randomValue) | [source/game_sa/Cam.cpp:1923](../../source/game_sa/Cam.cpp#L1923) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamManFov(const CVector& target, const CVector& source, float progress) | [source/game_sa/Cam.cpp:2016](../../source/game_sa/Cam.cpp#L2016) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliChaseInitialFov(float progress, float duration, float from, float to) | [source/game_sa/Cam.cpp:2070](../../source/game_sa/Cam.cpp#L2070) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliChaseReturnFov355(int32 now, int32 start, int32 end, float from, float to) | [source/game_sa/Cam.cpp:2078](../../source/game_sa/Cam.cpp#L2078) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeliChaseDistanceFov355(const CVector& camera, const CVector& dest, float progress, float duration, float from, float to, bool initial, float nearDistance, float farDistance, float reduction) | [source/game_sa/Cam.cpp:2087](../../source/game_sa/Cam.cpp#L2087) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneOrbitWave(const CVector& up, float amplitude, int32 cycles, int32 now, int32 start, int32 end) | [source/game_sa/Cam.cpp:2202](../../source/game_sa/Cam.cpp#L2202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessPlaneOrbit(CCam* cam, bool second) | [source/game_sa/Cam.cpp:2209](../../source/game_sa/Cam.cpp#L2209) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:40 (0x00B6F028); Posible dato fijo gLastFrameProcessedDWCineyCam: source/game_sa/Cam.cpp:44 (0x008CCB9C); Posible dato fijo pIgnoreEntity: source/game_sa/World.h:67 (0x00B7CD68) |
| PlaneSpotterEase(float ratio,float from,float to) | [source/game_sa/Cam.cpp:2299](../../source/game_sa/Cam.cpp#L2299) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaneSpotterRandomCoordinate(int randomValue, float range, float halfRange, float origin) | [source/game_sa/Cam.cpp:2304](../../source/game_sa/Cam.cpp#L2304) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamApplyUnderwaterBlurInlined() | [source/game_sa/Cam.cpp:2466](../../source/game_sa/Cam.cpp#L2466) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x008CC7A4 |
| CamFollowCarBetaBlend355(float betaInput,float betaCorrection,float maximumSpeed,float smoothing,float previous) | [source/game_sa/Cam.cpp:2564](../../source/game_sa/Cam.cpp#L2564) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::Process_FollowCar_SA 0x005245B0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowCarHistory355(const CVector& focal,const CVector& front,float radius) | [source/game_sa/Cam.cpp:2572](../../source/game_sa/Cam.cpp#L2572) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowCarFinalHistory355(CCam* cam,const CVector& focal,float alpha,float historyDistance,float distance) | [source/game_sa/Cam.cpp:2580](../../source/game_sa/Cam.cpp#L2580) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowCarBetaCorrection355(CCam* cam,CVehicle* vehicle,uint32 rowOffset) | [source/game_sa/Cam.cpp:2591](../../source/game_sa/Cam.cpp#L2591) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowCarActualDistance355(const CVector& focal, const CVector& history) | [source/game_sa/Cam.cpp:2599](../../source/game_sa/Cam.cpp#L2599) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowCarAutoAlpha355(float base,float step,float wanted,float previous,float maximum) | [source/game_sa/Cam.cpp:2605](../../source/game_sa/Cam.cpp#L2605) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowCarAlphaBlend355(CCam* cam,float smoothing,float alphaInput,float maximumSpeed) | [source/game_sa/Cam.cpp:2611](../../source/game_sa/Cam.cpp#L2611) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMouseFinalDistance355(CCam* cam, const CVector& target) | [source/game_sa/Cam.cpp:2825](../../source/game_sa/Cam.cpp#L2825) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::Process_FollowPedWithMouse 0x0050F970 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMouseChanges355(float fov,float inputX,float inputY,bool sticks,float& beta,float& alpha) | [source/game_sa/Cam.cpp:2833](../../source/game_sa/Cam.cpp#L2833) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMouseRepeatedRadius355(float fov,float nearPlane) | [source/game_sa/Cam.cpp:2845](../../source/game_sa/Cam.cpp#L2845) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMousePerpendicularDistance355(const CVector& source,const CVector& front,float radiusFactor) | [source/game_sa/Cam.cpp:2852](../../source/game_sa/Cam.cpp#L2852) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMouseSphereCenter355(CVector& result,const CVector& front,const CVector& source,float nearPlane) | [source/game_sa/Cam.cpp:2859](../../source/game_sa/Cam.cpp#L2859) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMouseInitialSource355(CVector& result,const CVector& target,const CVector& front,float distance) | [source/game_sa/Cam.cpp:2864](../../source/game_sa/Cam.cpp#L2864) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMouseFirstHitDistance355(const CVector& target,const CVector& hit) | [source/game_sa/Cam.cpp:2868](../../source/game_sa/Cam.cpp#L2868) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMousePullSource355(CVector& source,const CVector& target) | [source/game_sa/Cam.cpp:2873](../../source/game_sa/Cam.cpp#L2873) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamMouseRecenterDelta355(const CVector& direction,float orientation,float beta) | [source/game_sa/Cam.cpp:2879](../../source/game_sa/Cam.cpp#L2879) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraRound4(float value) | [source/game_sa/Cam.cpp:2985](../../source/game_sa/Cam.cpp#L2985) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraHeadingMinusHalfPI(CPlaceable* ped) | [source/game_sa/Cam.cpp:2995](../../source/game_sa/Cam.cpp#L2995) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraRoundVector4(CVector& v) | [source/game_sa/Cam.cpp:3003](../../source/game_sa/Cam.cpp#L3003) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedAutoAlpha355(float base,float step,float wanted,float previous,float maximum) | [source/game_sa/Cam.cpp:3005](../../source/game_sa/Cam.cpp#L3005) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedPitchCorrection355(float wantedPitch, float historyPitch, float followWeight, float followMaximum, float taskWeight, float verticalOffset) | [source/game_sa/Cam.cpp:3011](../../source/game_sa/Cam.cpp#L3011) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedDistance355(const CVector& focal,const CVector& history,float distance,float rowMinimum,float minimumDistance) | [source/game_sa/Cam.cpp:3017](../../source/game_sa/Cam.cpp#L3017) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedLimitHistoryPitch355(float separation,float historyPitch,float step) | [source/game_sa/Cam.cpp:3023](../../source/game_sa/Cam.cpp#L3023) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedHistoryBase355(float step,float speed,float wanted) | [source/game_sa/Cam.cpp:3029](../../source/game_sa/Cam.cpp#L3029) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedResetHistory355(const CVector& focal, const CVector& front, float firstDistance, float secondDistance, CVector& first, CVector& second) | [source/game_sa/Cam.cpp:3035](../../source/game_sa/Cam.cpp#L3035) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedHistory355(float historyDelta,float verticalOffset,float beta,const CVector& focal,float historyDistance,float distance,CVector& first,CVector& second) | [source/game_sa/Cam.cpp:3041](../../source/game_sa/Cam.cpp#L3041) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamFollowPedHistoryHeading355(const CVector& front) | [source/game_sa/Cam.cpp:3049](../../source/game_sa/Cam.cpp#L3049) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| M16Asin(float input) | [source/game_sa/Cam.cpp:3250](../../source/game_sa/Cam.cpp#L3250) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| M16Direction(float horizontal, float vertical) | [source/game_sa/Cam.cpp:3255](../../source/game_sa/Cam.cpp#L3255) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamM16Blend355(float smoothing,float betaInput,float alphaInput,float* betaSpeed,float* alphaSpeed,float* betaAngle,float* alphaAngle) | [source/game_sa/Cam.cpp:3262](../../source/game_sa/Cam.cpp#L3262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::Process_M16_1stPerson 0x005105C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamM16Bump355(float* beta,float* alpha,float* horz,float* vert,uint32 elapsed) | [source/game_sa/Cam.cpp:3267](../../source/game_sa/Cam.cpp#L3267) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamM16StickInputs355(bool attached,float horizontal,float vertical,float fov,float step,float* beta,float* alpha) | [source/game_sa/Cam.cpp:3273](../../source/game_sa/Cam.cpp#L3273) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamM16MouseAngles355(float horizontal,float vertical,float fov,float* beta,float* alpha) | [source/game_sa/Cam.cpp:3279](../../source/game_sa/Cam.cpp#L3279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CamM16SideRay355(float beta,float alpha,bool positive,const CVector& source) | [source/game_sa/Cam.cpp:3284](../../source/game_sa/Cam.cpp#L3284) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WheelCameraUp(const CVector& side, const CVector& vertical, uint32 time) | [source/game_sa/Cam.cpp:3650](../../source/game_sa/Cam.cpp#L3650) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCam::Process_WheelCam 0x00512110 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ConvertPedNode2BoneTag(int32 simpleId) | [source/game_sa/Cam.cpp:3796](../../source/game_sa/Cam.cpp#L3796) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsLampPost(eModelID modelId) | [source/game_sa/Cam.cpp:3821](../../source/game_sa/Cam.cpp#L3821) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo MI_BOLLARDLIGHT: source/game_sa/ModelIndices.cpp:90 (0x008CD5FC); Posible dato fijo MI_MLAMPPOST: source/game_sa/ModelIndices.cpp:96 (0x008CD614); Posible dato fijo MI_SINGLESTREETLIGHTS1: source/game_sa/ModelIndices.cpp:33 (0x008CD518); Posible dato fijo MI_SINGLESTREETLIGHTS2: source/game_sa/ModelIndices.cpp:34 (0x008CD51C); Posible dato fijo MI_SINGLESTREETLIGHTS3: source/game_sa/ModelIndices.cpp:35 (0x008CD520); Posible dato fijo MI_STREETLAMP1: source/game_sa/ModelIndices.cpp:37 (0x008CD528); Posible dato fijo MI_STREETLAMP2: source/game_sa/ModelIndices.cpp:38 (0x008CD52C); Posible dato fijo MI_TELPOLE02: source/game_sa/ModelIndices.cpp:99 (0x008CD620); Posible dato fijo MI_TRAFFICLIGHTS_3: source/game_sa/ModelIndices.cpp:29 (0x008CD508); Posible dato fijo MI_TRAFFICLIGHTS_4: source/game_sa/ModelIndices.cpp:30 (0x008CD50C); Posible dato fijo MI_TRAFFICLIGHTS_5: source/game_sa/ModelIndices.cpp:31 (0x008CD510); Posible dato fijo MI_TRAFFICLIGHTS_GAY: source/game_sa/ModelIndices.cpp:32 (0x008CD514); Posible dato fijo MI_TRAFFICLIGHTS_MIAMI: source/game_sa/ModelIndices.cpp:26 (0x008CD4FC); Posible dato fijo MI_TRAFFICLIGHTS_TWOVERTICAL: source/game_sa/ModelIndices.cpp:28 (0x008CD504) |
