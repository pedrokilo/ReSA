# Sustitución de QuickTime/QTML por Media Foundation (rama `sustituir-quicktime`)

## Estado real encontrado (partiendo de `48e53f065c`)

- **Quién usa QuickTime**: únicamente `CAEMFDecoder`, que solo construye `CAEUserRadioTrackManager::LoadUserTrack` para las **pistas de usuario**
  (emisora «User Track Player», archivos de `User Tracks`): extensiones `.mp3`, `.aac`, `.m4a` y `.mov` (tipo `AUDIO_FILE_TYPE_QUICKTIME`, tabla `audioExtensionTypes` de 0x8CBB28).
  **No** lo usan los SFX, las voces ni las emisoras del juego (`AUDIO\SFX`, `AUDIO\STREAMS`): esos van por `CAEVorbisDecoder`/`CAEWaveDecoder`/bancos.
  Solo se ejecuta si el jugador tiene pistas de usuario escaneadas (`sa-utrax.dat`/`sa-ufiles.dat`) y sintoniza la emisora de pistas de usuario.
- **Lo que había** (lote EXT1A, `99d9745bf7`, y S1/LNK5A): QuickTime estaba *retirado sin sustituto*. `CAEMFDecoder::InitLibrary` devolvía siempre `false`,
  `Initialise`/`FillBuffer` eran nulos, los `.mp3` se reencaminaban al decoder WMA y `.aac`/`.m4a`/`.mov` **no se cargaban**. Quedaba `QTMLRuntime.cpp` (8 ganchos a
  funciones de QTML del EXE, inalcanzables), `CAEMFDecoder::FillCallback` (0x4E7B80) y el puente nulo `QtmlConverterFillBridge` (0x81A3C0).
- `envolturas-pendientes.csv` seguía listando 0x819840, 0x819770, 0x4E7990 y 0x4E7A80 por herencia de los cohortes históricos (`reversing/OriginalDependencies*`);
  ya no había código fuente que las llamara.
- Imports del standalone: no hay import estático de QuickTime ni de Media Foundation (`link.rsp`); el único rastro era `QTMLRuntime.obj` en el enlace (carga de `QuickTime.qts` por
  `LoadLibrary`, inalcanzable). Los puentes QTML del EXE (0x818430..0x81DAA0) siguen como bytes de la imagen del EXE original, sin llamadores.

## Qué se hizo

- Nuevo `source/game_sa/Audio/Loaders/MfPcmReader.{h,cpp}` (clase `CMfPcmReader`, sin dependencias del juego): lector de PCM con `IMFSourceReader`.
  Las DLL (`mfplat.dll`, `mfreadwrite.dll`) se cargan con `LoadLibrary`/`GetProcAddress` (cuatro funciones); solo se enlaza `mfuuid.lib` (GUIDs, sin DLL). Sin Media Foundation
  (ediciones N de Windows) `InitLibrary` devuelve `false` y el gestor vuelve a pasar los `.mp3` al decoder WMA, como el original sin QuickTime.
- `CAEMFDecoder` usa `CMfPcmReader` (campo `m_reader` en el offset 0x18, antes `m_soundConverter`; tamaño 0xC4 y vtable 0x85F370 intactos).
- Eliminados `QTMLRuntime.cpp/.h` (8 ganchos, filas de `hooks.csv`), `CAEMFDecoder::FillCallback` (0x4E7B80) y `QtmlConverterFillBridge`.
- `contrib/pending_wrappers.py`: lista `RETIRED_ADDRESSES` con 0x819840, 0x819770, 0x4E7990 y 0x4E7A80 (funciones retiradas, no reconstruidas); `envolturas-pendientes.csv` regenerado (10 a 6 filas).

## Mapeo de cada llamada QTML a la nueva

| Original (QuickTime / QTML) | Dirección en el EXE | Nuevo (Media Foundation) |
|---|---|---|
| `InitializeQTML(0)` + `EnterMovies` (`CAEMFDecoder::InitLibrary`) | 0x4E7C70, puentes 0x818650 / 0x8195C0 | `CMfPcmReader::Startup`: `CoInitializeEx(MTA)` + `MFStartup(MF_VERSION)` con cuenta de referencias |
| `ExitMovies` + `TerminateQTML` (`CAEMFDecoder::Shutdown`) | 0x4E7850, 0x819840 / 0x819770 / 0x818670 | `CMfPcmReader::Shutdown`: `MFShutdown` con la última referencia |
| abrir la película y su media de sonido (`NativePathNameToFSSpec`, `OpenMovieFile`, `NewMovieFromFile`, `GetMovieIndTrackType`...) | 0x4E7990 | `MFCreateSourceReaderFromURL` (ruta ANSI de `sa-ufiles.dat` convertida con `CP_ACP`) |
| formato de la media (canales, frecuencia, tamaño de muestra, `compressionID`), 'wave'/'cvbr' | 0x4E7A80, 0x818510 | innecesario: el lector decodifica solo (el flag de compresión variable ya no existe) |
| `SoundConverterOpen`/`SetInfo`/`BeginConversion` a 'NONE', 2 canales, 16 bits, 44100 Hz | 0x818430, 0x818490 | `SetCurrentMediaType` con PCM de 16 bits, 2 canales, 44100 Hz (el lector inserta decodificador, remuestreador y mezclador) |
| `GetMediaDuration` → `m_nLength` (unidades de la media, tratadas como 1/44100 s) | 0x81A180 | `MF_PD_DURATION` (100 ns) convertido a fotogramas a 44100 Hz |
| `SoundConverterFillBuffer` + callback `FillCallback` + `GetMediaSample` | 0x818570, 0x4E7B80, 0x81A3C0 | `CMfPcmReader::Read`: `ReadSample` + cola de sobrantes; `FillBuffer` devuelve los bytes escritos (0 al terminar) |
| posición de lectura `m_nPlayTime` (`SetCursor`) | 0x4E78E0 | `CMfPcmReader::Seek` = `SetCurrentPosition`; `m_nPlayTime` avanza 1 por fotograma entregado |
| `DisposeMovie`, liberar el manejador de origen y `SoundConverterClose` (destructor) | 0x819B20, 0x819270, 0x818450 | `delete m_reader` (`Release` del lector) |

Conservado igual: salida PCM estéreo de 16 bits a 44100 Hz, `GetSampleRate` = 44100, longitud en ms = `m_nLength` · (1000/44100) (mismo cálculo x87 a 24 bits),
descarte de pistas de menos de 7 s, `FillBuffer` = -1 sin inicializar, `GetStreamPlayTimeMs` = `m_nPlayTime` / 44,1, `SetCursor(ms)` = `ms` · 44,1.

Diferencias conocidas: el tiempo de reproducción cuenta fotogramas ya decodificados (el original contaba fotogramas de origen leídos, por delante); si `MF_PD_DURATION` no
existe la duración es 0 y la pista se descarta; la duración que informa Media Foundation para un MP3 CBR sin cabecera Xing es una estimación por tamaño.

## Evidencia

Ver los apartados «Comparación con QuickTime» y «Prueba en el standalone» de este mismo archivo (se rellenan con las medidas reales).
