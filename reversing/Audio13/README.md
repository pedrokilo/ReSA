<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco MP3 y decodificador Vorbis (lista de 13)

Validadas el 02/10/2026 con **11.520 pares original/candidato dentro de GTA**, todos exactos: 26 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEMP3BankLoader` | Constructor, `Initialise`, `GetBankSlotBuffer`, `GetSoundHeadroom`, `IsSoundBankLoaded`, `GetSoundBankLoadingStatus`, `GetSoundBuffer`, `IsSoundLoaded`, `GetSoundLoadingStatus`, `UpdateVirtualChannels`, `LoadSoundBank`, `LoadSound`, `Service` |
| `CAEVorbisDecoder` | Constructor, Destructor, `Initialise`, `FillBuffer`, `GetStreamLengthMs`, `GetStreamPlayTimeMs`, `SetCursor`, `GetSampleRate`, `GetStreamID`, `ReadCallback`, `SeekCallback`, `CloseCallback`, `TellCallback` |

**Diferencias reales corregidas:**
- `CAEMP3BankLoader`:
  - el constructor solo escribe `m_IsInitialised` y `m_StreamingChannel`; `Initialise` solo reinicia `SlotInfo`, `Status`, banco, slot y sonido de cada solicitud;
  - IDs de banco, sonido y slot sin signo (`movzx`) frente a contadores y valores guardados con signo: un ID −1 nunca coincide con un banco −1 (`IsSoundBankLoaded`, `IsSoundLoaded`, sus `Get...LoadingStatus`, `GetSoundBuffer`, `GetBankSlotBuffer`);
  - `GetBankSlotBuffer` escribe 0 en la longitud con un slot fuera de rango; `GetSoundHeadroom` multiplica por 0,01f (0x858C58) en vez de dividir por 100;
  - `GetSoundBuffer` no mira el contador de slots ni pasa por `GetBankSlotBuffer`/`GetBankLookup` (sin asserts);
  - `UpdateVirtualChannels`: cualquier slot o sonido negativo invalida el canal, un banco con `NumSounds` negativo acepta cualquier sonido y el último sonido termina en el tamaño del lookup del banco (no en `NumBytes` del slot);
  - `LoadSoundBank`/`LoadSound`: rangos antes de `IsSound...Loaded` (en `LoadSound`, el sonido primero), el duplicado de `LoadSoundBank` ignora el sonido y la solicitud se escribe campo a campo (los punteros de stream y, en las de un sonido, `BankNumBytes` conservan su valor);
  - `Service`: la metadata va a `m_BankSlots[Slot]`, el banco se copia antes que los sonidos, la solicitud terminada vuelve a banco/slot/sonido −1, módulo con signo, posición de CdStream `handle | sector`, lookup directo y estados desconocidos ignorados.
- `CAEVorbisDecoder`:
  - las virtuales se instalaban con `RH_ScopedInstall` plano; ahora `RH_ScopedVirtualClass(0x862D70, 8)` + `RH_ScopedVMTInstall` y `NOTSA_EXPORT_VTABLE`;
  - el constructor guarda el byte bajo de `userTrack` tal cual; el destructor no pone el handle a nulo;
  - `Initialise`: `OggVorbis_File` sin inicializar, la fuente de los callbacks es la copia del stream (+0x20) y una pista de usuario de menos de 7 s deja `m_bInitialized` a falso;
  - `FillBuffer` pasa el bitstream inicializado a −1; `GetStreamID` usa la copia del stream;
  - `GetStreamLengthMs`/`GetStreamPlayTimeMs` multiplican por 1000.0 en x87 y truncan con `_ftol2`; `SetCursor` hace `fild` (+2³² sin signo) · 0,001f en vez de dividir por 1000.

**Método:**
- El hilo de streaming de audio se aparca en `IsForegroundApp` (al principio de su bucle) durante todo el banco: desactivar los hooks del decoder redirige también la vtable de la DLL al EXE.
- `CAEMP3BankLoader` usa slots, lookups, handles, buffer y datos de stream falsos con relleno para índices negativos; se simulan `CdStreamGetStatus`, `CdStreamRead`, `CMemoryMgr::Malloc/Free`, `ConvertFromBytesToMS` y la carga de los tres ficheros de tablas.
- `CAEVorbisDecoder` usa streams, `OggVorbis_File` y `vorbis_info` falsos; vorbisFile.dll se sustituye en la IAT del EXE y la libvorbis estática de la DLL con parches de entrada; `CAEDataStream` se simula en ambos lados. operator new/delete solo se sustituyen en `Initialise` y el destructor, con registros sin reservas.
- Los falsos se ponen y se quitan en cada ejecución (nunca durante la escritura del log); un vigilante registra EIP y pila del hilo principal si una ejecución tarda más de 10 s.
- Se vigilan la IAT de vorbisFile.dll, la vtable 0x862D70 y las entradas de `IsForegroundApp`.
- Desde el 03/10/2026 el decoder ya no tiene desviaciones `FIX_BUGS`: los fixtures incluyen fuentes mono y de más de dos canales en `Initialise` y `FillBuffer`.
- Fuera de los fixtures:
  - banco igual al contador en `LoadSoundBank`/`LoadSound` (el EXE desreferencia el lookup nulo);
  - sonidos negativos en `GetSoundHeadroom` (el EXE lee 786 KB fuera del slot);
  - `size` 0 en `ReadCallback` (división por cero en ambos).

Repetir: `python reversing/Audio13/build_probe.py` y `python reversing/run.py --case audio13`. Evidencia: [validation-summary.json](validation-summary.json).
