<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadioFont1: banco por pares de CAERadioTrackManager (29 funciones) y CFont (32 funciones)

Banco simple (flujo de `PadMouse1`/`AudioFinal1`: corre dentro del juego completo con la partida cargada y se engancha en `CGame::Process`).
GTA SA 1.0 US. **Escrito sin compilar ni ejecutar**: la primera ejecucion real es la del llamador.

Uso: `bash reversing/_handoff/bench.sh RadioFont1 3`. Filtro opcional: `radiofont1-only.txt` en la carpeta del juego con `Clase::Funcion`
(p. ej. `CAERadioTrackManager::StartRadio-manual`; con filtro la cobertura total no cuadra, es solo para depurar).

## Corpus: 14976 pares

| Clase | Funcion (EXE) | Pares |
|---|---|---|
| CAERadioTrackManager | AddAdvertIndexToHistory 0x4E9760 / AddDJBanterIndexToHistory 0x4E97B0 / AddIdentIndexToHistory 0x4E9720 / AddMusicTrackIndexToHistory 0x4E96C0 | 512 cada una |
| CAERadioTrackManager | CheckForMissionStatsChanges 0x4E8410 | 1024 |
| CAERadioTrackManager | CheckForPause 0x4EA590, CheckForStationRetuneDuringPause 0x4EB890, ChooseTracksForStation 0x4EB180, DisplayRadioStationName 0x4E9E50, StartRadio-with-settings 0x4EB550 | 512 cada una |
| CAERadioTrackManager | StartRadio-manual 0x4EB3C0 | 1024 |
| CAERadioTrackManager | Initialise 0x5B9390, IsRadioOn 0x4E8350, Reset 0x4E7F80, RetuneRadio 0x4E8290, SetBassEnhanceOnOff 0x4E9DB0, SetBassSetting 0x4E82F0 | 256 cada una |
| CAERadioTrackManager | GetCurrentRadioStationID 0x4E83F0, GetRadioStationName 0x4E9E10, GetRadioStationNameKey 0x4E8380, InitialiseRadioStationID 0x4E8330, Load 0x5D40E0, ResetStatistics 0x4E8200, Save 0x5D3EE0, StartTrackPlayback 0x4EA640 | 128 cada una |
| CAERadioTrackManager | GetRadioStationListenTimes 0x4E83E0, HasRadioRetuneJustStarted 0x4E8370, PlayRadioAnnouncement 0x4E8400, SetRadioAutoRetuneOnOff 0x4E82E0 | 64 cada una |
| CFont | GetCharacterSize 0x719750, GetLetterIdPropValue 0x718770, GetStringWidth 0x71A0E0, ParseToken 0x718F00 | 512 cada una |
| CFont | GetNumberLines 0x71A5E0, GetTextRect 0x71A620, PrintString 0x71A700, PrintStringFromBottom 0x71A820, ProcessStringToDisplay 0x71A600, SetColor 0x719430, SetDropColor 0x719510 | 256 cada una |
| CFont | LoadFontValues 0x7187C0, SetFontStyle 0x719490, SetOrientation 0x719610, SetScale 0x719380, SetScaleForCurrentLanguage 0x7193A0 | 128 cada una |
| CFont | DrawFonts 0x71A210, InitPerFrame 0x719800, Initialise 0x5BA690, Shutdown 0x7189B0, SetAlphaFade 0x719500, SetBackground 0x7195C0, SetBackgroundColor 0x7195E0, SetCentreSize 0x7194E0, SetDropShadowPosition 0x719570, SetEdge 0x719590, SetJustify 0x719600, SetProportional 0x7195B0, SetRightJustifyWrap 0x7194F0, SetSlant 0x719420, SetSlantRefPoint 0x719400, SetWrapx 0x7194D0 | 64 cada una |

Total: 29 funciones de radio + 32 de CFont = 61 funciones. Cada par ejecuta la funcion en el EXE (hook desactivado) y en la DLL (hook activo) con la misma
semilla (`Rng` determinista por funcion y caso) y compara retorno, traza de llamadas con argumentos, hash de todas las regiones globales tocadas y el objeto
falso completo.

## Que se compara

- **Retorno** (`ret`; los `bool`/`eRadioID` se leen de AL, el flotante de ST0 por bits), `rect` de `GetTextRect`, clave de `GetRadioStationNameKey` (16 bytes),
  `color`/`tag`/offset de `ParseToken`, cursor de lineas de `LoadFontValues`.
- **Objeto falso** `CAERadioTrackManager` de 0x370 bytes con guardas de 16 bytes (rellenado con 0xCD y con todos los campos asignados dentro del dominio real).
  Nunca se toca `AERadioTrackManager` (0x8CB6F8).
- **Regiones globales** (hash por ejecucion, se restauran antes de cada ejecucion y al final): historiales y estadisticas de radio (0xB61D78..0xB62C74),
  temporizador (0xB7CB48..), reloj (0xB70150..), modo de replay (0xA43088), `m_bWideScreenOn` (0xB6F065), estado de CFont (0xC716A8..0xC71BA8, incluye
  `RenderState`, `pEmptyChar`, `gFontData` y los punteros de sprites), idioma (0xBA67CC), modo de radio del menu (0xBA67F8), resolucion (0xC17044/48),
  recuento de pistas de usuario (0xB6B976) y `gString`.
- **Traza** de los colaboradores sustituidos con sus argumentos (ver tabla). Los hilos ajenos al del banco (audio, streaming) no escriben en la traza (`Mine()`).

## Colaboradores sustituidos (EntryPatch en la direccion del EXE y en el simbolo de la DLL, con el mismo stub)

El `Body()` (cadena de E9 de la DLL, sin entrar nunca en el EXE) de **todos** los sitios se calcula antes de crear ningun `EntryPatch`; los parches de cada
funcion se crean justo antes de su bucle y se deshacen al terminar. Se respeta el `ret N` de cada original (`__fastcall(self,_,args...)` para miembros,
`__cdecl` para estaticas/globales; las estaticas de la DLL que el EXE llama con `ecx` ignorado son `__cdecl` sin argumentos).

- Radio: `CStats::GetStatValue` 0x558E40, `CAEUserRadioTrackManager::GetUserTrackPlayMode` 0x4F3330, `CAEAudioHardware::{SetBassSetting 0x4D94A0, SetChannelFlags 0x4D9500, StartTrackPlayback 0x4D8F30, SetChannelFrequencyScalingFactor 0x4D8960}`, `UpdateRadioVolumes` 0x4EA010, `StaticGetPlayerVehicleAudioSettingsForRadio` 0x4F4ED0, `CAudioEngine::{IsAmbienceRadioActive 0x507280, IsAmbienceTrackActive 0x507210, DoesAmbienceTrackOverrideRadio 0x507270, StopAmbienceTrack 0x507220, ReportFrontendAudioEvent 0x506EA0, StopRadio 0x506F70}`, `CAERadioTrackManager::{StopRadio 0x4E9820, StartRadio manual 0x4EB3C0, IsVehicleRadioActive 0x4E9800, TrackRadioStation 0x4EAC30, ChooseTracksForStation 0x4EB180, QueueUpTracksForStation 0x4EA670}`, `CAEAudioUtility::ResolveProbability` 0x4D9C80, `CText::Get` 0x6A0050, `FindPlayerVehicle` 0x56E0D0, `CGenericGameStorage::{SaveDataToWorkBuffer 0x5D1270, LoadDataFromWorkBuffer 0x5D1300}`, y el RNG (`rand` 0x821B1E + `CGeneral::GetRandomNumber`).
- CFont: `0x719B40` (emision de linea de `ProcessCurrentString`, solo EXE: la DLL la llama por direccion), `CSprite2d::DrawRect` 0x727B60, `CMenuManager::DrawWindow` 0x573EE0, `CSprite::InitSpriteBuffer` 0x70CFB0, `CFont::{RenderFontBuffer 0x719840, PrintString 0x71A700, DrawFonts 0x71A210, LoadFontValues 0x7187C0}`, `CMessages::InsertPlayerControlKeysInString` 0x69E160, `CFileMgr::{SetDir 0x5387D0, OpenFile 0x538900, CloseFile 0x5389D0}`, `CFileLoader::LoadLine<FILESTREAM>` 0x536F80 (el fichero `DATA\FONTS.DAT` se sustituye por lineas del fixture: nunca se abre un fichero real), `CTxdStore::{AddTxdSlot 0x731C80, LoadTxd 0x7320B0, AddRef 0x731A00, PushCurrentTxd 0x7316A0, SetCurrentTxd 0x7319C0, PopCurrentTxd 0x7316B0, FindTxdSlot 0x731850, RemoveTxdSlot 0x731CD0}`, `CSprite2d::{SetTexture 0x7272B0, Delete 0x727240}`.
- El resto de colaboradores se ejecuta de verdad: `AddMusicTrackIndexToHistory` dentro de `ChooseTracksForStation`, los `CFont::Set*` dentro de `DisplayRadioStationName` e `Initialise`, `ProcessCurrentString`/`GetStringWidth`/`GetCharacterSize`/`ParseToken` dentro de las funciones de lineas, `HudColour`, `CMessages::GetStringLength`, `GetScriptLetterSize` (0x719670, solo en `GetStringWidth` con `scriptText=true`).
- Con la ejecucion normal del runner (`unhook='Audio,Global/SurfaceInfos_c'`) los hooks de audio arrancan **desactivados**: las llamadas de la DLL a otras funciones de audio sin sustituir saltan al EXE; solo la funcion bajo prueba conmuta (`hook->State(candidate)` y despues la comprobacion `JumpsToDll`).

## Dominio de los fixtures (cosas que se evitan a proposito)

- Sin sNaN en floats (el x87 las quieta con `fld/fstp` y la DLL copia en bruto). Si hay qNaN e infinitos en los setters de CFont y en `SetBassSetting`.
- Textos GXT bien formados: toda `~` abre una etiqueta que se cierra. El EXE (y ahora la DLL, 0x71A190 y 0x719176) no comprueba el NUL al buscar la `~` de cierre y leeria basura de la pila.
- `GetRadioStationNameKey` solo con identificadores 0..13 (la DLL tiene un `assert` NOTSA fuera de rango; el EXE desbordaria el buffer de 8 bytes con identificadores mayores). `StartRadio` solo con identificadores >= 0 (el EXE indexa el array de estados con signo, la DLL aborta en depuracion con `std::array`).
- `Shutdown`: el stub de `FindTxdSlot` siempre devuelve una ranura valida (7). `CTxdStore::SafeRemoveTxdSlot` (FIX_BUGS de la DLL) omite `RemoveTxdSlot(-1)`, que el EXE si ejecuta; esa rama queda fuera del banco.
- `LoadFontValues`: identificador de fuente 0 o 1 (indices mayores escriben fuera de `gFontData`), secciones completas (un `sscanf(NULL)` revienta ambos), sin lineas solo de espacios (el EXE compararia `attrib` sin inicializar).
- Los bytes de relleno de `tRadioSettings`/`tRadioState` no se enmascaran: Reset/Initialise los copian con `rep movsd` en el EXE y con `memcpy` de 60 bytes en la DLL, asi que deben coincidir; el relleno propio no se escribe en ninguno de los dos lados.
- Ruido NOTSA evitado: no hay `NOTSA_LOG_*` en las ramas probadas; los `assert` de `std::array`/`GetStatValue<T>` de la fuente anterior se eliminaron al reconstruir fiel al EXE.

## Incidencias de fuente corregidas (resumen; detalle y direcciones en el informe y en los comentarios `// Como el original:`)

CAERadioTrackManager: Reset (`m_nSavedTimeMs` a 0 en 0x4E80F5, `BassGain` pedido intacto, copia completa con relleno), Initialise (era `*this = {}`; el EXE guarda el handle, modo 7, `field_368 = 0` y el modo de pistas de usuario), GetCurrentRadioStationID (lee `m_ActiveSettings`, 0xE9), CheckForMissionStatsChanges (indicadores con signo, comparacion `fild`+`fcompp` con el float, `_ftol2`), DisplayRadioStationName (constantes 0x3B124925/0x3ACCCCCD, `x = ancho/2` entero, `GetRadioStationName` antes de los setters), GetRadioStationName (id <= 0 devuelve NULL), StartRadio manual (rama OFF solo vacia cola/tipos/indices), StartRadio con ajustes (horas/dias con signo y diferencia de dias), ChooseTracksForStation (`TrackIndices[-1]`), Add{Ident,Advert,DJBanter}IndexToHistory (indice dword).

CFont: SetColor y SetDropColor (factor 0x3B808081, condicion `!(alpha >= 255)`), SetOrientation (switch; otros valores no escriben), InitPerFrame (borra `m_nFontOutlineSize`, `m_wFontTexture = 0xFFFF`), Shutdown (solo `ButtonSprite[1..4]`), GetStringWidth (copia `min(len, 0x18F)` sin `strncpy_s`, longitud de 16 bits, sin comprobar NUL), ParseToken (sin NUL), GetTextRect (`right = x + 4` en alineacion derecha, `((c*0.5)+4)+x`, lineas como byte, `((alto*lineas)+y)+4`), PrintString (solo r,g,b), GetCharacterSize (borde con signo, indices sin comprobar), GetHeight (`(y*32)*0.5 + (y+y)`), GetLetterIdPropValue (`==1`, indice `int16`, sin comprobar rango), LoadFontValues (`fontId` byte, `%d`).

## Aprobación (09/10/2026)

Corrida `20261009-161801-628784`: passed=true, 14976 pares, 61 funciones con 0 fallos (29 de CAERadioTrackManager y 32 de CFont), evento complete (state_restored), begin con layout y sitios correctos, saves y configuraciones intactos. Registrada con `register_radiofont1.py` (evidencia en `runs/`, `validation-summary.json`). Con esto CAERadioTrackManager y CFont quedan COMPLETAS. Límites: textos GXT bien formados; GetRadioStationNameKey solo con identificadores 0..13; StartRadio solo con identificadores >= 0; Shutdown con FindTxdSlot siempre válido (la rama de SafeRemoveTxdSlot(-1) queda fuera); LoadFontValues con fuente 0 o 1 y sin fichero real; objeto falso (nunca el AERadioTrackManager real); colaboradores doblados sin crédito.
