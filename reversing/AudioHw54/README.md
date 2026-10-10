<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEAudioHardware (lista de 34)

Banco preparado el 04/10/2026: **9.280 pares original/candidato dentro de GTA** para 53 de las 54 funciones de `CAEAudioHardware` (pendiente de compilar y ejecutar).

| Clase | Funciones |
|---|---|
| `CAEAudioHardware` | Constructor, Destructor, `AllocateChannels`, `GetNumAvailableChannels`, `GetChannelPlayTimes`, `SetChannelVolume`, `SetChannelFlags`, `SetChannelFrequencyScalingFactor`, `SetChannelPosition`, `StopSound`, `RescaleChannelVolumes`, `UpdateReverbEnvironment`, `LoadSoundBank`, `IsSoundBankLoaded`, `GetSoundBankLoadingStatus`, `LoadSound`, `IsSoundLoaded`, `GetSoundLoadingStatus`, `GetSoundHeadroom`, `EnableEffectsLoading`, `DisableEffectsLoading`, `RequestVirtualChannelSoundInfo`, `GetVirtualChannelSoundLengths`, `GetVirtualChannelSoundLoopStartTimes`, `PlayTrack`, `StartTrackPlayback`, `StopTrack`, `GetTrackPlayTime`, `GetTrackLengthMs`, `GetActiveTrackID`, `GetPlayingTrackID`, `GetBeatInfo`, `SetBassSetting`, `EnableBassEq`, `DisableBassEq`, los 9 `Set/Get…ScalingFactor`, `IsStreamingFromDVD`, `GetDVDDriveLetter`, `CheckDVD`, `PauseAllSounds`, `ResumeAllSounds`, `Query3DSoundEffects`, `InitDirectSoundListener`, `Terminate`, `Service` |

**Diferencias reales corregidas:**
- `RescaleChannelVolumes`:
  - `10^(vol · 0,05)` con el `_CIpow` del EXE (0x822130) en vez del `pow` de la CRT de la DLL; el EXE guarda la ganancia con `fst` y, en los canales que no se atenúan, suma el ST0 completo (no la ganancia redondeada);
  - el reparto de los canales que se desvanecen al final (`FLAG_FADE_NEAR_END`) va en x87 (`fild`, `fmul`, `fidiv`): la DLL multiplicaba en doble y redondeaba dos veces;
  - el volumen final es `20 · AudioLog10(...)` con el log del EXE en ST0 sin redondear (la DLL redondeaba el log a float antes de multiplicar) y con `p < 0,00001` o NaN → −100.
- `InitDirectSoundListener`: si `SetFormat` falla no libera el buffer primario (se quita la desviación `FIX_BUGS`).
- `CAEStreamThread::~CAEStreamThread` (lo único que hace el Destructor, 0x4D83A0 → 0x4F11E0): `DeleteCriticalSection` de la sección crítica; la DLL llamaba a `ReleaseMutex` sobre una variable local.

**Método:**
- Todo corre sobre un `CAEAudioHardware` falso de 0x1100 bytes rellenado al azar por caso; nunca se toca `AEAudioHardware`, DirectSound, EAX, cargadores, hilos ni ficheros reales.
- El hilo de streaming real se aparca en `IsForegroundApp` durante todo el banco (como FixAudio).
- Canales falsos con vtable de 9 huecos trazados (`GetPlayTime`/`GetLength` de 32 bits guionizados por canal y por llamada); el canal 0 hace de canal de streaming.
- Se simulan y registran en el EXE y en la DLL: `CAEAudioChannel::SetVolume/SetPosition/SetReverbAndDepth`, `CAEStreamingChannel::SetBassEQ`, los métodos usados de `CAEMP3BankLoader`, `CAEMP3TrackLoader`, `CAEStreamThread` y `CAESmoothFadeThread`, y `CAEAudioEnvironment::GetReverbEnvironmentAndDepth`. `operator delete` y `CMemoryMgr::Free` solo se parchean durante `GetBeatInfo`/`Terminate`.
- DirectSound/EAX con dispositivo, buffer, buffer 3D, `IKsPropertySet` y oyente falsos; los HRESULT y las salidas salen de un guion consumido en orden.
- El hook de `CAEAudioUtility::AudioLog10` se desactiva durante el banco para que el EXE use su propia función.
- Fuera de los fixtures:
  - `Initialise` (construye canales y cargadores reales con los constructores de la DLL, `DirectSoundCreate8` y arranca hilos);
  - NaN, infinitos y sNaN en volúmenes y factores; longitud 0 (16 bits bajos) en canales que se desvanecen (división por cero x87);
  - canales nulos donde el EXE no los comprueba e índices de canal fuera del objeto.

Repetir: `python reversing/AudioHw54/build_probe.py` y `python reversing/run.py --case audiohw54`.
