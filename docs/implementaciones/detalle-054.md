<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 888 grupos / 10227 funciones reconstruidas; 10227 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1061"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DefaultVolumeInt(eAudioEvents event) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:48](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FadeWeatherSoundOut(CAESound* sound) | [source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp:53](../../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1062"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleDoorType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleDoorType v) | [source/game_sa/Audio/Enums/eAEVehicleDoorType.h:14](../../source/game_sa/Audio/Enums/eAEVehicleDoorType.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1063"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleHornType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleHornType v) | [source/game_sa/Audio/Enums/eAEVehicleHornType.h:25](../../source/game_sa/Audio/Enums/eAEVehicleHornType.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1064"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eBassSetting.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eBassSetting v) | [source/game_sa/Audio/Enums/eBassSetting.h:14](../../source/game_sa/Audio/Enums/eBassSetting.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1065"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBank.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBank v) | [source/game_sa/Audio/Enums/eSoundBank.h:740](../../source/game_sa/Audio/Enums/eSoundBank.h#L740) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1066"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBankSlot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBankSlot v) | [source/game_sa/Audio/Enums/eSoundBankSlot.h:87](../../source/game_sa/Audio/Enums/eSoundBankSlot.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1067"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeGainToDecibels(const float* gain) | [source/game_sa/Audio/Hardware/AEAudioHardware.cpp:487](../../source/game_sa/Audio/Hardware/AEAudioHardware.cpp#L487) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1068"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyFromAESound(const CAESound& sound) | [source/game_sa/Audio/Hardware/AEAudioHardware.h:29](../../source/game_sa/Audio/Hardware/AEAudioHardware.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1069"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEStaticChannel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StaticChannelDestructorBody(CAEStaticChannel* self, void* /*edx*/) | [source/game_sa/Audio/Hardware/AEStaticChannel.cpp:28](../../source/game_sa/Audio/Hardware/AEStaticChannel.cpp#L28) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote142AStaticChannelDestructorBody() | [source/game_sa/Audio/Hardware/AEStaticChannel.cpp:33](../../source/game_sa/Audio/Hardware/AEStaticChannel.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1070"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEStreamingChannel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AEStreamingChannelDestructorBody(CAEStreamingChannel* self, void* /*edx*/) | [source/game_sa/Audio/Hardware/AEStreamingChannel.cpp:600](../../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp#L600) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CAEStreamingChannel::Destructor 0x004F2200 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CAEStreamingChannel() | [source/game_sa/Audio/Hardware/AEStreamingChannel.cpp:614](../../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp#L614) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1071"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEDataStream.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AEDataStreamScalarDeletingDestructor(CAEDataStream* self, void* /*edx*/, uint8 flags) | [source/game_sa/Audio/Loaders/AEDataStream.cpp:225](../../source/game_sa/Audio/Loaders/AEDataStream.cpp#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAEDataStream::ScalarDeletingDestructor 0x004DC550 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1072"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEMFDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MFDecoderDestructorBody(CAEMFDecoder* self, void* /*edx*/) | [source/game_sa/Audio/Loaders/AEMFDecoder.cpp:306](../../source/game_sa/Audio/Loaders/AEMFDecoder.cpp#L306) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CAEMFDecoder::Destructor 0x004E77C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AMFDecoderDestructorBody() | [source/game_sa/Audio/Loaders/AEMFDecoder.cpp:320](../../source/game_sa/Audio/Loaders/AEMFDecoder.cpp#L320) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1073"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SecondsToMs(double seconds) | [source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp:34](../../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1074"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEWMADecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEWMADecoderDestructorBody(CAEWMADecoder* self, void* /*edx*/) | [source/game_sa/Audio/Loaders/AEWMADecoder.cpp:321](../../source/game_sa/Audio/Loaders/AEWMADecoder.cpp#L321) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CAEWMADecoder::Destructor 0x00502760 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CAEWMADecoder() | [source/game_sa/Audio/Loaders/AEWMADecoder.cpp:336](../../source/game_sa/Audio/Loaders/AEWMADecoder.cpp#L336) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1075"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEWaveDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BytesToMs(uint32 bytes, uint32 bytesPerSecond) | [source/game_sa/Audio/Loaders/AEWaveDecoder.cpp:32](../../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1076"></a>

## Globales y helpers: source/game_sa/Audio/Managers/AERadioTrackManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResetCommon(CAERadioTrackManager& self) | [source/game_sa/Audio/Managers/AERadioTrackManager.cpp:103](../../source/game_sa/Audio/Managers/AERadioTrackManager.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_nAdvertIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:169 (0x00B620C0); Posible dato fijo m_nDJBanterIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:168 (0x00B61D78); Posible dato fijo m_nIdentIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:170 (0x00B62980); Posible dato fijo m_nMusicTrackIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:171 (0x00B62B40); Posible dato fijo m_nSpecialDJBanterIndex: source/game_sa/Audio/Managers/AERadioTrackManager.h:199 (0x00B62C72); Posible dato fijo m_nSpecialDJBanterPending: source/game_sa/Audio/Managers/AERadioTrackManager.h:200 (0x00B62C73) |
| CalculateRadioVolume355() | [source/game_sa/Audio/Managers/AERadioTrackManager.cpp:599](../../source/game_sa/Audio/Managers/AERadioTrackManager.cpp#L599) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAERadioTrackManager::UpdateRadioVolumes 0x004EA010 | 0 / 0 | Posible dato fijo AEAudioHardware: source/game_sa/Audio/Hardware/AEAudioHardware.cpp:30 (0x00B5F8B8) |

<a id="grupo-1077"></a>

## Globales y helpers: source/game_sa/Audio/OpenAL/OALBase.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OALCheckErrors(std::string_view file, int32 line) | [source/game_sa/Audio/OpenAL/OALBase.cpp:26](../../source/game_sa/Audio/OpenAL/OALBase.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1078"></a>

## Globales y helpers: source/game_sa/AutoPilot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SubtractRoundedOnce(double a, double b) | [source/game_sa/AutoPilot.cpp:76](../../source/game_sa/AutoPilot.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1079"></a>

## Globales y helpers: source/game_sa/Birds.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMaxDistance(eBirdsBiome biome) | [source/game_sa/Birds.cpp:65](../../source/game_sa/Birds.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1080"></a>

## Globales y helpers: source/game_sa/BouncingPanel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_MV_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/BouncingPanel.cpp:52](../../source/game_sa/BouncingPanel.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3_VM_59C810(const CVector& v, const CMatrix& m) | [source/game_sa/BouncingPanel.cpp:61](../../source/game_sa/BouncingPanel.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyByPow(CVector& pos, float damp, float step) | [source/game_sa/BouncingPanel.cpp:71](../../source/game_sa/BouncingPanel.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
