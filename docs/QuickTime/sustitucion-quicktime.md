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

### Comparación con QuickTime (mismo archivo, PCM de 16 bits, estéreo, 44100 Hz)

QuickTime 7 está instalado en el equipo. `reversing/FnQT/qt_ref.cpp` carga `QTMLClient.dll` y decodifica con `MovieAudioExtraction` (mismo decodificador de QuickTime que usaba el
juego, pero no el camino exacto `SoundConverterFillBuffer` + callback); `reversing/FnQT/mf_probe.cpp` hace lo mismo con `CMfPcmReader` (el código de producción);
`cmp_pcm.py` alinea y compara. Resultados completos en `reversing/FnQT/evidencia/comparacion/resultados.txt` (solo sumas de control, no se guarda audio):

| Archivo | QuickTime | Media Foundation | Comparación (tras alinear) |
|---|---|---|---|
| MP3 CBR de 94891 B con cabecera Xing | 244168 fotogramas, 976672 bytes, CRC32 02FD7414, pico 18725, película 5536 ms | 242544 fotogramas, 970176 bytes, CRC32 A687D795, pico 18725, duración informada 5511 ms | diferencia máxima 1 LSB, 50 % de muestras idénticas, SNR 71,4 dB |
| MP3 CBR sin Xing de 15,5 s (tres pistas unidas) | 685388 fotogramas, 2741552 bytes, CRC32 E8B8AC94, película 15541 ms | 684912 fotogramas, 2739648 bytes, CRC32 1F8297B1, duración informada 15308 ms | diferencia máxima 1 LSB, 50,05 % idénticas, SNR 80,1 dB |

Conclusión honesta: el PCM **no es idéntico bit a bit** (los CRC difieren), pero cada muestra coincide con la de QuickTime con error de ±1 LSB (redondeo/dither del decodificador MP3) en toda
la pista, con el mismo formato, frecuencia y canales. Difiere la longitud: Media Foundation entrega 1,6 mil fotogramas menos (37 ms en la primera pista, 11 ms en la segunda, el recorte
del último fotograma MP3) y, para MP3 CBR sin cabecera Xing, `MF_PD_DURATION` es una estimación por tamaño (233 ms por debajo de la película de QuickTime en la segunda pista).
Ese desfase de duración afecta solo a `GetStreamLengthMs` (bucle y descarte de menos de 7 s).
Una prueba con un archivo hecho uniendo pistas con cabecera Xing a mitad de flujo mostró un fotograma extra de 1152 en uno de los dos decodificadores por cada unión
(artefacto del archivo sintético, no usado en las cifras). La lectura por trozos de 4096 y de 100000 bytes da el mismo CRC (la cola de sobrantes es correcta) y `Seek` a 2 s devuelve datos.

### Prueba en el standalone (`reversing/FnQT/ejecutar_qt.ps1`, sonda `usertrack` en `FnJuego/sonda_juego.py`)

Carpeta propia `D:\Original\_qt_run` (`preparar_ejecucion.ps1`): partida copiada, `SaveDirectoryPath` redirigido, pista de usuario de prueba (el MP3 de 15,5 s) registrada a mano en
`sa-utrax.dat`/`sa-ufiles.dat`. La sonda mete a CJ en un coche con radio, escribe `RADIO_USER_TRACKS` (12) en la petición de script de `CAERadioTrackManager` (0x8CB774, la variable
que escribe `RetuneRadio`) y espera 60 s; `pico_audio.exe` mide el pico WASAPI de la sesión de audio del PID.

| | standalone nuevo (`mf-juego`) | control: standalone de `integracion-fase7b` sin QuickTime (`control-juego`) |
|---|---|---|
| decoders soportados (0xB6B978..) | QuickTime/Media Foundation = **1** | = **0** |
| pistas de usuario cargadas | 1 | 1 |
| emisora pedida y activa | 12 (pistas de usuario) | 12 |
| módulos cargados antes de pedir la emisora | `mfplat.dll`, `mfreadwrite.dll` (por `InitLibrary`) | ninguno de MF |
| módulos tras pedirla | + `mfcore.dll`, `mfsrcsnk.dll` (origen MP3), `mp3dmod.dll` (decodificador MP3) | ninguno de MF |
| vivo al terminar, sin errores nuevos en el log | sí | sí |
| pico WASAPI (media / máximo, 74 muestras de 1 s) | 0,129 / 0,560 (73 de 74 muestras > 0,01) | 0,260 / 0,729 (74 de 74) |

La carga de `mfsrcsnk.dll` y `mp3dmod.dll` solo ocurre en la ejecución nueva y solo después de sintonizar la emisora: es la prueba de que el juego abrió y decodificó el MP3 por
`CAEMFDecoder` + Media Foundation. **El pico de audio no distingue las dos ejecuciones** (el juego tiene ruido de fondo propio, los coches generados fueron distintos y la pista de prueba es
música ordinaria): demuestra que la sesión de audio del proceso sigue activa y con señal, no que se oyera la pista de usuario.

No se probó en juego: `.aac`, `.m4a` y `.mov` (no hay archivos de muestra; Media Foundation los abre en Windows 10/11, `.mov` no está garantizado), cambio de pista al terminar, bucle
(`SetCursor` desde el hilo de streaming), pistas a 48 kHz u otros canales (el original tomaba las unidades de la media como 1/44100 s; aquí se remuestrea a 44100 Hz, así que la longitud es correcta),
ni la escucha real de la pista.
