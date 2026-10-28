<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMenuManager grupo A: dibujo (19 funciones)

Banco por pares EXE (gta_sa 1.0 US) frente a DLL dentro del motor. Prefijo de caso: `menumgra`. **Sin ejecutar todavía** (revisión y banco escritos sin compilar).

| Función | Dirección | Casos |
|---|---|---|
| DrawBackground | 0x57B750 | 1024 |
| DrawFrontEnd | 0x57C290 | 512 |
| DrawStandardMenus | 0x5794A0 | 2048 |
| DrawWindow | 0x573EE0 | 512 |
| DrawWindowedText | 0x578F50 | 512 |
| DrawQuitGameScreen | 0x57D860 | 128 |
| DrawControllerBound | 0x57E6E0 | 1024 |
| DrawControllerScreenExtraText | 0x57D8D0 | 512 |
| DrawControllerSetupScreen | 0x57F300 | 1024 |
| DisplaySlider | 0x576860 | 1024 |
| DisplayHelperText | 0x57E240 | 1024 |
| PrintBriefs | 0x576320 | 512 |
| PrintMap | 0x575130 | 2048 |
| PrintRadioStationList | 0x5746F0 | 256 |
| PrintStats | 0x574900 | 1024 |
| MessageScreen | 0x579330 | 256 |
| SmallMessageScreen | 0x574010 | 512 |
| StretchX | 0x5733E0 | 256 |
| StretchY | 0x573410 | 256 |

Total: 14.464 pares.

**Correcciones hechas en la DLL** (`source/game_sa/Frontend/MenuManager_Draw.cpp`, solo `DrawStandardMenus`):
- Deslizadores (brillo, volumen de radio y efectos, distancia de dibujo, sensibilidad del ratón):
  - la barra usa las constantes precalculadas del EXE (valor/640 y valor/448) con el atajo 640x448, no `StretchX/StretchY` (difieren en el último bit);
  - los valores son `brillo * (1/384)` y `(distancia - 0.925) * (1/0.875)`, no divisiones;
  - si el cursor está en la zona derecha pero fuera de los límites del deslizador, el EXE vuelve a `NONE` (la DLL dejaba el valor derecho); el deslizador del ratón solo comprueba la X.
- Packs de misiones: sin comprobación de rango del índice (el EXE no la hace; la DLL abortaba con `NOTSA_UNREACHABLE`).
- Columna derecha: radar (>2), idioma (>4) y calidad de efectos (>3) fuera de rango no dejan texto (la DLL abortaba); tipo de control solo da texto con 0 y 1; el modo de pistas de usuario ignora valores negativos; la entrada `MOUSEJOYPAD` usa "distinto de 0" para el mando.
- Ranura de guardado vacía: `sprintf` en vez de `sprintf_s` (que rellena con 0xFE en depuración el resto de `gString`).
- El producto líneas*29 se suma como entero sin signo.

**Funciones sin diferencias** frente al desensamblado (constantes, orden de llamadas y comparaciones x87 comprobados): StretchX, StretchY, DrawWindow, DrawWindowedText, DrawQuitGameScreen, DrawControllerBound, DrawControllerScreenExtraText, DrawControllerSetupScreen, DisplaySlider, DisplayHelperText, PrintBriefs, PrintMap, PrintRadioStationList, PrintStats, MessageScreen, SmallMessageScreen, DrawFrontEnd y DrawBackground.

**Método:**
- `CMenuManager` falso (0x1B78 bytes con guardas de 64 bytes); nunca se usa el singleton como `this`.
- Las 19 funciones se conmutan a la vez (original = EXE de principio a fin; candidato = DLL), porque se llaman entre sí: cada caso ejecuta toda la cadena.
- El EXE lee y escribe por dirección absoluta 14 campos del singleton (0x1B24..0x1B63: tiempos, `m_bViewRadar`, `m_BackgroundSprite`, `m_LastBlinkTime`, `m_HelperTextUpdatedTime`, banderas de error y del escaneo...); además `CRadar::TransformRadarPointToScreenSpace` y `LimitRadarPoint` leen del singleton `m_bDrawingMap`, `m_fMapZoom` y `m_vMapOrigin`. Antes de cada ejecución se copian del objeto falso al singleton; el estado efectivo del original es el del singleton y el del candidato el del objeto falso; se comprueba que la otra copia no se toca y que el resto del singleton queda igual; el singleton se restaura entero.
- Se fijan por caso, se comparan y se restauran: resolución (`RsGlobal`; se combinan 640 y 448 con otros valores), reloj y contador de fotogramas, pad, teclado, ratón, tabla `aScreens` (con mutaciones de la pantalla actual: acciones, nombres, tipos, posiciones y alineación), estáticos 0x8CDFF4..0x8CE003, `gString`, `gGxtString`, `gGxtString2`, ranuras de guardado (estado y nombres, también de 254 o más caracteres), `PreviousBriefs`, estadísticas, zonas y leyenda del mapa, banderas de scripts, estado de CRadar, sensibilidad del ratón, ratón invertido, estado del escaneo de pistas y texto de error de CD.
- Se simulan y trazan: `CFont`, `CSprite2d`, `CText::Get`, `RsCameraBeginUpdate`, `DefinedState2d`, `SetHelperText`, `ResetHelperText`, `CheckHover`, `SaveSettings`, `SetFrontEndRenderStates`, audio de radio, pistas de usuario, `CControllerConfigManager`, `GetFxQuality`, `GetVideoModeList`, `CMemoryMgr::Calloc/Free`, `CStats`, `CMessages`, `GetSavedGameDateAndTime`, `CMenuSystem`, `CRadar` (dibujo del mapa y leyenda), `CPlaceName::GetForMap`, `CTheZones::GetCurrentZoneLockedOrUnlocked`, `FindPlayerCentreOfWorld_NoSniperShift` y `RsEventHandler`. `CRadar::Transform*` y `LimitRadarPoint` se ejecutan de verdad.
- Al empezar se registra un evento `layout` que compara los desplazamientos de los campos usados con los del EXE.

**Fuera de la comparación (diferencias conocidas):**
- `DrawBackground`: con `USE_BUILD_INFORMATION` (definido en `config.h`) la DLL llama a `DrawBuildInfo` (NOTSA, el EXE no lo hace). El banco anula `CMenuManager::DrawBuildInfo` en la DLL.
- `PrintMap` pasa `true` como parámetro `unused` de `DrawWindow` (el EXE pasa 3): se ignora en la función y no es observable.
- Estados no alcanzables con datos válidos (no se prueban): `m_RedefiningControls` distinto de 0/1 (el EXE usa basura de pila), `m_ControlMethod` distinto de 0/1 en las pantallas de mandos, `DrawControllerBound` con acción COMBOLOCK (ningún caso la genera), `PrintBriefs` con fila < 3 (lee antes de `PreviousBriefs`, memoria ajena), packs de misiones en las filas 0 y 1 (lee antes de `m_MissionPacks`), `m_nSelectedRow` > 19.

Repetir: `python reversing/MenuMgrA/build_probe.py` y `python reversing/run.py --case menumgra`.
