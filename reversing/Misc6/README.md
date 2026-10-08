<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Pájaros, créditos, curvas, cronómetros, horarios, muebles y ventana (lista de 6)

Validadas el 02/10/2026 con **19.456 pares original/candidato dentro de GTA**, todos exactos: 41 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CBirds` | `Init`, `Shutdown`, `HandleGunShot`, `CreateNumberOfBirds`, `Update`, `Render` |
| `CCredits` | `Start`, `Stop`, `Render`, `PrintCreditText`, `PrintCreditSpace`, `RenderCredits` |
| `CCurves` | `TestCurves`, `DistForLineToCrossOtherLine`, `CalcSpeedVariationInBend`, `CalcSpeedScaleFactor`, `CalcCorrectedDist`, `CalcCurvePoint` |
| `COnscreenTimer` | `AddClock`, `AddCounter`, `ClearClock`, `ClearCounter`, `SetClockBeepCountdownSecs`, `SetCounterFlashWhenFirstDisplayed` |
| `CTimeInfo` | `FindOtherTimeModel`, `GetOtherTimeModel`, `GetTimeOn`, `GetTimeOff`, `SetOtherTimeModel`, `SetTimes` |
| `FurnitureGroup_c` | `Init`, `Exit`, `AddSubGroup`, `GetFurniture`, `GetRandomId`, `AddFurniture` |
| `Win` | `GTATranslateKey`, `GTATranslateShiftKey`, `IsAlreadyRunning`, `IsForegroundApp`, `MessageLoop` |

**Diferencias reales corregidas:**
- `CBirds`:
  - `CreateNumberOfBirds`: velocidad `r · 0,02 + base` (no `/ 50`); colores normales `rand & 0x7F + 0x80`; ángulo con `fpatan`; un bioma desconocido no fija velocidad, colores ni tamaño; sin pájaro libre sigue con la siguiente iteración; el punto de comprobación sustituye al destino en las iteraciones siguientes.
  - `Update`: horas 6..21 y contador con signo; `[1, 7 - n)` de 0x407180; agua si la distancia es `< 30`; en el desierto como mucho `3 - n` pájaros (no `min(num, n)`); `(rand & 0xFF) - 128` con signo más `atan2` sin redondear; seno y coseno x87 para el punto de aparición y el giro; paso `ms_fTimeStep · 0,02` y giro `· 0,002`; borrado por cuadrados (`max² < dx² + dy²`); se actualiza solo si el tiempo es estrictamente mayor que `m_nUpdateAfterMS`.
  - `Render`: constante 6,28 del EXE (no 2π) y ángulo `r · (6,28 / ws)`; seno y coseno x87 con el segundo seno dividido sin redondear; transparencia `1 - (d - 0,7·max) / (0,3·max)`; contadores del búfer temporal de 32 bits.
- `CCredits`:
  - `PrintCreditText`: x con división entera del ancho y `trunc(scaleY · 20 + pos)` (la DLL truncaba la escala antes).
  - `PrintCreditSpace`: `trunc(espacio · 25 + pos)`.
  - `RenderCredits`: desplazamiento `· (1/23)`; color de sombra negro opaco (la DLL lo ponía azul con alfa 0); fin con suma entera `pos + alto`.
- `CCurves`:
  - `DistForLineToCrossOtherLine`: `dist · (-1 / dir)`.
  - `CalcSpeedVariationInBend`: 1/3 del EXE (0x3EAAAAAA) y `dot · (1/0,7)`.
  - `CalcSpeedScaleFactor` y `CalcCurvePoint`: suma `(2·m + s2) + s1`.
  - `CalcCorrectedDist`: transcripción x87 con `fcos`/`fsin` y las constantes del EXE.
  - `CalcCurvePoint`: tiempo NaN a 0; NaN va al tramo recto o al final; `(dir · t) · m`; velocidad `lerp · total / (ms · 0,001)` sin el factor `(1 - t)`.
- `COnscreenTimer`:
  - `AddClock`/`AddCounter` con `strncpy` (rellena con ceros, sin terminador si llena);
  - `ClearCounter` borra solo el primer byte del texto;
  - `SetClockBeepCountdownSecs` recorre cuatro entradas de 0x40 desde el reloj (error del original: escribe en los contadores).
- `CTimeInfo::FindOtherTimeModel`: bucle propio que exige información horaria antes de comparar la clave (no `GetModelInfoFromHashKey`) y `strcpy`/`strncpy`.
- `FurnitureGroup_c`:
  - `AddSubGroup`: contador de subgrupos de 16 bits (0xBAB37C);
  - `AddFurniture` añade a todos los subgrupos con ese ID y siempre devuelve cierto.
- `Win`:
  - `GTATranslateKey`: 16 bits bajos de `MapVirtualKeyA` y estrictamente menor que 0xFF;
  - `GTATranslateShiftKey` usa `*keyCode` como almacenamiento y se lo pasa al manejador;
  - `IsAlreadyRunning` compara la ventana con NULL (no con `INVALID_HANDLE_VALUE`);
  - `IsForegroundApp` lee `ForegroundApp` como dword.

**Método:**
- Se simulan y se trazan:
  - `rand` del EXE y `CGeneral::GetRandomNumber` (cuerpo real, también desde el `GetRandomNumberInRange<float>` desnudo);
  - la línea de visión, el nivel del agua, `CalcScreenCoors`, `fpRenderStateSet` y el volcado de geometría;
  - `CFont`, `CText::Get`, `DefinedState2d`, los bordes panorámicos y `RenderCredits` (en `Render`);
  - las funciones de `FurnitureSubGroup_c` y `RsKeyboardEventHandler`;
  - las API de Win32 en la IAT del EXE y de la DLL (`PeekMessageA`, `TranslateMessage`, `DispatchMessageA`, `CreateEventA`, `GetLastError`, `FindWindowA`, `SetForegroundWindow`, `GetKeyState`, `MapVirtualKeyA/W`), solo en el hilo principal. No se toca la ventana ni la cola de mensajes reales.
- `FindOtherTimeModel` usa model infos falsos en huecos vacíos de `ms_modelInfoPtrs` (los reales se recorren sin cambios).
- Se guardan y restauran los pájaros, el búfer temporal, el almacén y el contador de subgrupos, el reloj, el paso, el contador de frames, el área, la hora, la región, la cámara (agua, frente, panorámico), la textura de nubes, `fpRenderStateSet`, los créditos, el menú, `RsGlobal` (ancho, alto, `quit`), la versión de Windows y `ForegroundApp`.
- Fuera de los fixtures:
  - `NOTSA_WinMain` (punto de entrada del juego, no se puede llamar);
  - más de 6 pájaros por llamada (el EXE lee fuera de la tabla y la DLL tiene un `assert` NOTSA), y por eso tampoco contadores negativos en `Update`;
  - `m_nWingStillness` nulo o de más de 2^24 y diferencias de tiempo de más de 2^24 ms (conversión a float).
- El fichero `misc6-only.txt` en la carpeta del juego limita el banco a una función.

Repetir: `python reversing/Misc6/build_probe.py` y `python reversing/run.py --case misc6`. Evidencia: [validation-summary.json](validation-summary.json).
