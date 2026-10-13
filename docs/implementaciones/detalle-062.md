<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1221"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Internal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsSaveSlot(eMenuEntryType slot) | [source/game_sa/Frontend/MenuManager_Internal.h:242](../../source/game_sa/Frontend/MenuManager_Internal.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1222"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Pages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StretchXK(float x, uint32 kBits) | [source/game_sa/Frontend/MenuManager_Pages.cpp:19](../../source/game_sa/Frontend/MenuManager_Pages.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| StretchYK(float y, uint32 kBits) | [source/game_sa/Frontend/MenuManager_Pages.cpp:23](../../source/game_sa/Frontend/MenuManager_Pages.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1223"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfo_c | [source/game_sa/Fx/Blueprints/FxInfo.h:10](../../source/game_sa/Fx/Blueprints/FxInfo.h#L10) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1224"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoAnimTexture.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAnimTexture_c | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.h:6](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1225"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindClosestPtOnLine(CVector& out, CVector& lineA, CVector& lineB, CVector& point) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A4880: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1226"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoAttractLine.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractLine_c | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.h:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1227"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoAttractPt.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractPt_c | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.h:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1228"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoColour.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColour_c | [source/game_sa/Fx/Blueprints/FxInfoColour.h:6](../../source/game_sa/Fx/Blueprints/FxInfoColour.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1229"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoColourBright.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourBright_c | [source/game_sa/Fx/Blueprints/FxInfoColourBright.h:6](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1230"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoColourRange.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourRange_c | [source/game_sa/Fx/Blueprints/FxInfoColourRange.h:6](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1231"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoDir.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoDir_c | [source/game_sa/Fx/Blueprints/FxInfoDir.h:6](../../source/game_sa/Fx/Blueprints/FxInfoDir.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1232"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmAngle.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmAngle_c | [source/game_sa/Fx/Blueprints/FxInfoEmAngle.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmAngle.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1233"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmDir.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmDir_c | [source/game_sa/Fx/Blueprints/FxInfoEmDir.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmDir.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1234"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmLife.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmLife_c | [source/game_sa/Fx/Blueprints/FxInfoEmLife.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmLife.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1235"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmPos.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmPos_c | [source/game_sa/Fx/Blueprints/FxInfoEmPos.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1236"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmRate.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmRate_c | [source/game_sa/Fx/Blueprints/FxInfoEmRate.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1237"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmRotation.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmRotation_c | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1238"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmSize.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmSize_c | [source/game_sa/Fx/Blueprints/FxInfoEmSize.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1239"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmSpeed.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmSpeed_c | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.h:6](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1240"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoEmWeather.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmWeather_c | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.h:8](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.h#L8) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
