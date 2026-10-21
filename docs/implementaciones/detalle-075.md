<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 20/10/2026 (Ciudad de México):** 812 grupos / 8792 funciones reconstruidas; 8792 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1481"></a>

## tScriptSearchlight

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetId() | [source/game_sa/Scripts/TheScripts.h:239](../../source/game_sa/Scripts/TheScripts.h#L239) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:242](../../source/game_sa/Scripts/TheScripts.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1482"></a>

## tScriptSequence

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptSequence() | [source/game_sa/Scripts/TheScripts.h:119](../../source/game_sa/Scripts/TheScripts.h#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId() const | [source/game_sa/Scripts/TheScripts.h:125](../../source/game_sa/Scripts/TheScripts.h#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:128](../../source/game_sa/Scripts/TheScripts.h#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1483"></a>

## tScriptSphere

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tScriptSphere() | [source/game_sa/Scripts/TheScripts.h:262](../../source/game_sa/Scripts/TheScripts.h#L262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetId() const | [source/game_sa/Scripts/TheScripts.h:271](../../source/game_sa/Scripts/TheScripts.h#L271) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsActive() const | [source/game_sa/Scripts/TheScripts.h:274](../../source/game_sa/Scripts/TheScripts.h#L274) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1484"></a>

## tSpecificSpeechContextInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsUseable() const noexcept | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h:111](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNumSounds() const noexcept | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h:112](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1485"></a>

## tStreamingChannel

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsIdle() const noexcept | [source/game_sa/Streaming.h:177](../../source/game_sa/Streaming.h#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsReading() const noexcept | [source/game_sa/Streaming.h:178](../../source/game_sa/Streaming.h#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsStarted() const noexcept | [source/game_sa/Streaming.h:179](../../source/game_sa/Streaming.h#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1486"></a>

## tStreamingFileDesc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tStreamingFileDesc(const char* name, bool bNotPlayerImg) | [source/game_sa/Streaming.h:151](../../source/game_sa/Streaming.h#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInUse() const noexcept | [source/game_sa/Streaming.h:158](../../source/game_sa/Streaming.h#L158) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1487"></a>

## tUser3dMarker

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tUser3dMarker::Render(RpClump* clump) const | [source/game_sa/3dMarkers.cpp:781](../../source/game_sa/3dMarkers.cpp#L781) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_angleDiamondDeg: source/game_sa/3dMarkers.h:55 (0x00C7C700); Posible dato fijo m_user3dMarkerColor: source/game_sa/3dMarkers.h:53 (0x00C7C620) |
| GetColor() const | [source/game_sa/3dMarkers.h:24](../../source/game_sa/3dMarkers.h#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsInUse() const | [source/game_sa/3dMarkers.h:26](../../source/game_sa/3dMarkers.h#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1488"></a>

## tVehicleAudioSettings

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsHeli() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:30](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPlane() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:31](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPlaneOrHeli() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:32](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSeaplane() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:33](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsAircraftEngineSound() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:34](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCar() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:35](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBike() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:36](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCarEngineSound() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:37](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBMX() const | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:38](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1489"></a>

## tVirtualChannelSettings

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| tVirtualChannelSettings() | [source/game_sa/Audio/Loaders/AEMP3BankLoader.h:14](../../source/game_sa/Audio/Loaders/AEMP3BankLoader.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1490"></a>

## traits::fx_field_scanf_format&lt;char\[size\]&gt;

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * value() | [source/game_sa/Fx/FxTools.h:36](../../source/game_sa/Fx/FxTools.h#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| num_parts() | [source/game_sa/Fx/FxTools.h:40](../../source/game_sa/Fx/FxTools.h#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1491"></a>

## traits::fx_field_scanf_format&lt;float&gt;

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * value() | [source/game_sa/Fx/FxTools.h:27](../../source/game_sa/Fx/FxTools.h#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| num_parts() | [source/game_sa/Fx/FxTools.h:31](../../source/game_sa/Fx/FxTools.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1492"></a>

## traits::fx_field_scanf_format&lt;int32&gt;

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * value() | [source/game_sa/Fx/FxTools.h:18](../../source/game_sa/Fx/FxTools.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| num_parts() | [source/game_sa/Fx/FxTools.h:22](../../source/game_sa/Fx/FxTools.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
