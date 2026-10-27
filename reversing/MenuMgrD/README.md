<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
