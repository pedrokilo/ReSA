<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMenuManager grupo B: entrada (15 funciones)

Banco por pares EXE (gta_sa 1.0 US) frente a DLL dentro del motor. Prefijo de caso: `menumgrb`.

| Función | Dirección | Casos |
|---|---|---|
| AdditionalOptionInput | 0x5773D0 | 1024 |
| CentreMousePointer | 0x57C520 | 64 |
| CheckCodesForControls | 0x57DB20 | 1024 |
| CheckForMenuClosing | 0x576B70 | 1024 |
| CheckFrontEndDownInput / LeftInput / RightInput / UpInput | 0x5738B0 / 0x573920 / 0x573990 / 0x573840 | 256 cada una |
| CheckHover | 0x57C4F0 | 512 |
| CheckRedefineControlInput | 0x57E4D0 | 1024 |
| CheckSliderMovement | 0x573440 | 512 |
| ProcessUserInput | 0x57B480 | 1024 |
| RedefineScreenUserInput | 0x57EF50 | 1024 |
| ScrollRadioStations | 0x573A00 | 256 |
| UserInput | 0x57FD70 | 2048 |

Total: 10.560 pares.

**Correcciones hechas en la DLL** (`source/game_sa/Frontend/MenuManager_Input.cpp`):
- `UserInput`: el EXE salta a 0x5803CF también desde la pantalla de redefinir controles; los deslizadores, la rueda y las flechas izquierda/derecha se ejecutan siempre (la DLL los tenía dentro del `else`).
- `ProcessUserInput`: lee `ms_Slots[item - 1]` con `.data()` (el EXE no comprueba el rango; `std::array` abortaría en depuración).

**Método:**
- `CMenuManager` falso (0x1B78 bytes con guardas de 64 bytes); nunca se usa el singleton como `this`.
- Los campos que el EXE y la DLL leen del singleton por dirección absoluta (tiempos de transición, de deslizadores y de acción, `m_bDrawingMap`, `m_nMouseHoverScreen`, `bMouseHoverInitialised`, `m_StatusDisablePlayerControls`, zoom y origen del mapa para CRadar) se fijan por caso; el singleton completo se compara y se restaura (salvo 0xE0..0xE7, que escribe WndProc).
- Se fijan por caso: pad 0/1 (estado nuevo/viejo, `Mode`, `DisablePlayerControls`), teclado, ratón, historial de sticks, reloj, `RsGlobal`, estado de CRadar, ranuras de guardado, `PreviousBriefs`, scripts, replay, cámara, `ms_lodDistScale`.
- Se simulan y trazan audio, `CText::Get`, `GetStringWidth`, `SetGamma`, `CMenuSystem`, `CRadar` (blips), `CControllerConfigManager`, otras funciones de `CMenuManager` (SaveSettings, SwitchToNewScreen, GetNumberOfMenuOptions, ProcessMenuOptions, DrawControllerBound, SaveStatsToFile, Initialise, LoadAllTextures, UnloadTextures), `CPad::Clear*`, `DoRWStuff*`, ratón DirectInput, `CTimer`, `CCamera` (fundido) y `FindPlayerPed` (ped falso).
- `CentreMousePointer` llama de verdad a `ClientToScreen`/`SetCursorPos`; el cursor se restaura al terminar.

**Fuera de la comparación (diferencias conocidas por `FIX_BUGS`):**
- `CheckSliderMovement` / MOUSE_SENS: la DLL también escribe `m_fMouseAccelVertical`; ese campo se restaura pero no se compara.
- `CheckForMenuClosing`: la DLL llama a `InitialiseMouse(false)` donde el EXE llama a `InitialiseMouse(true)` (rama de cierre); el argumento se enmascara en la traza.
- `CentreMousePointer`: con `FIX_BUGS` la DLL no hace nada si la aplicación no está en primer plano; el banco fija `0x8D621C = 1`.
- `GenericFrameRate` debe estar desactivado (valor por defecto); se registra en el evento `player_ready`.

Repetir: `python reversing/MenuMgrB/build_probe.py` y `python reversing/run.py --case menumgrb`.
