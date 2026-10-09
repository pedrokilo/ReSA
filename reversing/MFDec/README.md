<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEMFDecoder con QuickTime como el EXE

Validadas el 04/10/2026 con **4.096 pares original/candidato dentro de GTA**, todos exactos: 10 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEMFDecoder` | `Initialise`, `FillBuffer`, `InitLibrary`, Constructor, Destructor, `GetStreamLengthMs`, `GetStreamPlayTimeMs`, `SetCursor`, `GetSampleRate`, `GetStreamID` |

**Diferencias reales corregidas** (decisión del usuario: el decoder se comporta exactamente como gta-sa.exe 1.0 US; se quita Media Foundation):
- `Initialise` (hueco 0 de la vtable `0x85F370`, `0x4E7CB0`) es la transcripción del EXE:
  - abre la película con `0x4E7990` y lee el formato con `0x4E7A80` (ayudantes del EXE, llamados en su dirección);
  - rellena los `SoundComponentData` de entrada (`+0x1C`) y salida (`+0x38`, `'NONE'`, 2 canales, 16 bits, 44100 Hz) y el flag de compresión variable (`+0xAD`, `compressionID == -2`);
  - `SoundConverterOpen` (`0x818430`), `SetInfo` `'cvbr'` y `'wave'` (`0x818510`, acepta 0 y -231), `DisposePtr` del átomo (`0x819210`), `BeginConversion` (`0x818490`), duración (`0x81A180`), Handle de 0x10000 bytes (`0x818DB0`, `0x818D70`, `0x8191F0`, `0x818F90`);
  - bloque del callback `+0x54..+0xB0` (callback `0x4E7B80`), 44100 Hz, divisor 44,1 y duración = `fild · 0x85F390` truncada con `_ftol2`; descarta pistas de menos de 7 s llamando a `GetStreamLengthMs` por la vtable. Los OSErr se comparan a 16 bits.
- `FillBuffer` (`0x4E7860`): `SoundConverterFillBuffer` (`0x818570`) con salidas puestas a 0; devuelve los bytes escritos.
- `InitLibrary` (`0x4E7C70`): `0x8195C0(0)` y `0x819820` solo si el flag `0xB61D70` está a 0; con éxito (o ya iniciado) lo pone a 1.
- Destructor: si estaba inicializado libera con `0x819B20`, `0x819270` y `0x818450` (antes liberaba objetos COM de Media Foundation).
- `Shutdown` (`0x4E7850`, sin hook) llama a `0x819840` y `0x819770` como el EXE.
- Disposición de miembros del EXE (`+0x10` película, `+0x14` media, `+0x18` conversor, `+0x98` Handle, `+0xB0` callback); los offsets de las 7 funciones ya validadas no cambian.
- **Consecuencia:** como en el juego original, las pistas MP3/AAC de user tracks necesitan QuickTime instalado; sin él `InitLibrary` devuelve `false` y el gestor pasa el MP3 al decoder WMA.

**Método:**
- Objeto falso de 0xC4 bytes con vtable falsa (`GetStreamLengthMs` trazado y simulado); las virtuales se llaman por el hueco de la vtable del EXE `0x85F370`, el constructor, el destructor e `InitLibrary` por su dirección.
- Las 15 funciones de pegamento QuickTime y los ayudantes `0x4E7990`/`0x4E7A80` se sustituyen en su dirección del EXE por falsos trazados con valores del fixture (OSErr con basura en los 16 bits altos); el EXE y la DLL pasan por los mismos falsos y nunca se llama a QuickTime real.
- Los falsos registran sin reservar memoria, con un hash del objeto en cada llamada (orden de escritura); el json se crea después de la llamada. `operator new/delete` solo se sustituyen durante el destructor.
- El flag `0xB61D70` se fija por caso y se restaura; el hilo de streaming de audio se aparca en `IsForegroundApp` durante todo el banco.
- Fuera de los fixtures: el callback `0x4E7B80` (código del EXE referenciado solo por puntero) y `Shutdown` (sin hook).

Repetir: `python reversing/MFDec/build_probe.py` y `python reversing/run.py --case mfdec`. Evidencia: [validation-summary.json](validation-summary.json).
