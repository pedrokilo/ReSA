<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de sonidos e hilo de streaming de audio (lista de 14)

Validadas el 02/10/2026 con **10.240 pares original/candidato dentro de GTA**, todos exactos: 28 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAESoundManager` | `Initialise`, `Terminate`, `Reset`, `PauseManually`, `Service`, `RequestNewSound`, `AreSoundsPlayingInBankSlot`, `AreSoundsOfThisEventPlayingForThisEntity`, `AreSoundsOfThisEventPlayingForThisEntityAndPhysical`, `CancelSoundsInBankSlot`, `CancelSoundsOfThisEventPlayingForThisEntity`, `CancelSoundsOfThisEventPlayingForThisEntityAndPhysical`, `CancelSoundsOwnedByAudioEntity`, `GetVirtualChannelForPhysicalChannel` |
| `CAEStreamThread` | `Initialise`, `Start`, `Stop`, `Pause`, `Resume`, `WaitForExit`, `PlayTrack`, `StopTrack`, `GetTrackPlayTime`, `GetTrackLengthMs`, `GetActiveTrackID`, `GetPlayingTrackID`, `MainLoop`, `Service` |

**Diferencias reales corregidas:**
- `CAESoundManager`:
  - `Initialise`: la lista física es un `new[]` con cookie (número de elementos en un dword delante, `count*2+4` bytes) y el aviso final va por el printf del EXE (spdlog reservaba memoria en su backtrace); `Terminate` la libera desde la cookie con `operator delete` escalar y libera siempre las otras dos listas;
  - `Service`: el play time en porcentaje multiplica por 0,01f en x87 (no divide por 100); `UpdatePlayTime` va en línea y suma el avance en float antes de truncar; `SoundHasFinished` mira el retardo de frames y no `m_IsAudioHardwareAware`; `UpdateParameters` y `CalculateVolume` en dos pasadas; la búsqueda del último hueco de prioridad mira también el hueco `numPrioritisedSounds` (antes leía la voz −1) y la inserción tenía la comparación de `PlayPhysically` invertida (NaN incluido); el retardo de frames se decrementa solo si es positivo como byte con signo;
  - `GetVirtualChannelForPhysicalChannel` resta en 16 bits; `PauseManually` guarda el byte del argumento tal cual (sin normalizar a bool);
  - `RequestNewSound`: la información de depuración NOTSA solo se guarda con voz libre (índice sin inicializar).
- `CAEStreamThread`:
  - `GetPlayTime`/`GetLength` del canal se leen como int32 completos (en el canal de streaming son 32 bits) y `GetTrackPlayTime` devuelve int32;
  - `Service`: el cursor es `field_1C` módulo la duración sin signo (no la pista siguiente) y la petición de parada se baja antes de `Stop`;
  - `MainLoop` usa solo los 32 bits bajos del tiempo.

**Método:**
- `GetCurrentTimeInMS` solo se simula en `MainLoop` y solo para el hilo principal (los demás hilos reciben la hora real).
- El hilo de streaming real se aparca en `IsForegroundApp` durante todo el banco; en el hilo principal ese falso devuelve una secuencia guionizada en `MainLoop`.
- `CAESoundManager` trabaja sobre un gestor falso con 300 voces, entidades y audio entities falsas; se simulan `AEAudioHardware`, Doppler y posición relativa, `CAESound::SetPosition/CalculateVolume/NewVPSLEntry/operator=` y las referencias de `CEntity`; lo que el EXE mete en línea corre de verdad en la DLL. `CTimer` se fija por caso y se restaura.
- `CAEStreamThread` usa hilo, canal, cargador, streams y decoders falsos; las API de hilos, secciones críticas y `Sleep` se sustituyen en las IAT del EXE y de la DLL solo alrededor de cada llamada y solo en el hilo principal (nunca se crea un hilo); los huecos de los decoders Vorbis se sustituyen en las vtables del EXE y de la DLL.
- operator new/delete solo durante la llamada, con registros sin reservas.
- Fuera de los fixtures:
  - duración 0 con bucle en `Service` (el EXE divide por cero; la DLL conserva el bugfix NOTSA);
  - duración 0 del decoder en `CAEStreamThread::Service` (división por cero en ambos);
  - `operator new` nulo (el EXE lo comprueba, la DLL no).

Repetir: `python reversing/Audio14/build_probe.py` y `python reversing/run.py --case audio14`. Evidencia: [validation-summary.json](validation-summary.json).
