<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMenuManager grupo D: estado, texturas y ajustes (13 funciones)

Banco por pares dentro de GTA (EXE gta_sa 1.0 US frente a la DLL) con un `CMenuManager` FALSO (0x1B78 bytes con guardas de 64 bytes; el constructor y el destructor se llaman sobre un bufer relleno de 0xCD y se comparan enteros). Prefijo de caso: `menumgrd`. Pendiente de compilar y ejecutar por quien lo integre.

Funciones: Constructor, Destructor, Initialise, LoadAllTextures, UnloadTextures, SwapTexturesRound, SetFrontEndRenderStates, SetDefaultPreferences, LoadSettings, SaveSettings, SaveStatsToFile, ResetHelperText, SetHelperText (6.784 pares).

**Diferencias reales corregidas en `MenuManager.cpp`:**
- `Constructor`: origen del mapa Y = 224.0f (0x43600000), no `APP_MINIMAL_HEIGHT / 2` (240).
- `SetDefaultPreferences` (audio) y `LoadSettings`: volumen de radio, SFX, ecualizador y autosintonia se leen de `FrontEndMenuManager` (0xBA6795..0xBA6799), no de `this`.
- `SwapTexturesRound`: el indicador es el de `FrontEndMenuManager` (0xBA8268), no el de `this`.
- `LoadSettings`: el bufer de 29 bytes ya no se pone a cero.
- `SaveStatsToFile`: el porcentaje usa `fild/fmul/fmul` (el entero entra exacto en el x87).

**Se simula y se traza:** CTxdStore, CSprite2d, CStreaming, CGame::TidyUpMemory, CTimer, CAudioEngine, Fx_c, CGamma, CFileMgr, CText, CControllerConfigManager, CStats, RwRenderStateSet y el RNG. `gta_sa.set` vive en memoria; `stats.html` se escribe en un fichero temporal del CRT (candidato) o se captura con fopen/fprintf/fclose del EXE sustituidos (original). Nunca se tocan ficheros reales del usuario.

**Fuera / limites:** texto de stats solo ASCII (la DLL convierte GXT a UTF-8, el EXE no: desviacion intencionada); `_strdate` real (dos ejecuciones seguidas; solo falla al cruzar la medianoche); `OutputDebugStringA` real; `FIX_BUGS` no se cubre.

Repetir: `python reversing/MenuMgrD/build_probe.py` y `python reversing/run.py --case menumgrd` (registrar el caso en `reversing/cases.json`). Filtro: `menumgrd-only.txt` con `CMenuManager::<Funcion>`.
