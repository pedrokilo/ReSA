<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Decodificador MF, fundido de audio, conversaciones, directorio e historial de eventos (lista de 10)

Validadas el 02/10/2026 con **17.088 pares original/candidato dentro de GTA**, todos exactos: 46 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEMFDecoder` | Constructor, Destructor, `GetStreamLengthMs`, `GetStreamPlayTimeMs`, `SetCursor`, `GetSampleRate`, `GetStreamID` |
| `CAESmoothFadeThread` | `CancelFade`, `Initialise`, `InitialiseRequestSlots`, `RequestFade`, `Service`, `SetBufferVolume`, `SmoothFadeProc`, `Start`, `Stop`, `WaitForExit` |
| `CConversations` | `AwkwardSay`, `Clear`, `IsConversationAtNode`, `IsConversationGoingOn`, `IsPlayerInPositionForConversation`, `RemoveConversationForPed`, `SetUpConversationNode`, `StartSettingUpConversation`, `Update` |
| `CDirectory` | `AddItem`, constructores (vacío y con capacidad), Destructor, `FindItem` (nombre, nombre con salida y hash), `Init`, `ReadDirFile`, `WriteDirFile` |
| `CEventHandlerHistory` | `ClearAllEvents`, `ClearNonTempEvent`, `ClearTempEvent`, `GetCurrentEvent`, `GetCurrentEventPriority`, `IsRespondingToEvent`, `RecordCurrentEvent`, `StoreActiveEvent`, `TakesPriorityOverCurrentEvent`, `TickStoredEvent` |

**Diferencias reales corregidas:**
- `CAEMFDecoder`:
  - las virtuales se instalaban con hooks planos (bucle por el thunk virtual): ahora `RH_ScopedVirtualClass(CAEMFDecoder, 0x85F370, 8)` y `RH_ScopedVMTInstall`;
  - el constructor solo escribe nombre, flag, `+0x10`, `+0x14` y pista;
  - el destructor borra el nombre con `operator delete` escalar sin anularlo y solo libera el decoder si estaba inicializado;
  - `GetStreamPlayTimeMs` es tiempo / divisor en x87 y `SetCursor` solo guarda pos · divisor (sin signo), ambos truncados con `_ftol2`; el seek de Media Foundation pasa a `FillBuffer`, que además devuelve -1 sin inicializar.
- `CAESmoothFadeThread`:
  - `Service` solo avanza el estado 1 (los demás estados no cancelados se ignoran) y la curva es `inicio + 20·log10(1 + paso·progreso)` (faltaba el `+1`), con el `_CIpow` del EXE, `diff·0.05` y fyl2x;
  - los volúmenes leídos son `fild · 0.01f` (no `/100`) y los escritos se truncan a 64 bits;
  - `RequestFade` compara el número de buffers con signo;
  - `Initialise` devuelve si se creó el hilo.
- `CConversations`:
  - `SetUpConversationNode` no pasa a mayúsculas la clave «sí» (solo la vacía);
  - `IsConversationAtNode` pasa a mayúsculas la cadena del llamador y compara con `strcmp` (no `stricmp`);
  - `Update` actualiza las conversaciones con cualquier estado distinto de 1 y 2.
- `CDirectory`:
  - el constructor vacío no escribe `m_bOwnsEntries`; el de capacidad usa `operator new` escalar sin construir entradas y el destructor `operator delete` escalar;
  - `AddItem`, `FindItem` y `FindItem` por hash comparan el número de entradas con signo;
  - `ReadDirFile` no inicializa el contador ni la entrada; `FindItem` por nombre escribe el tamaño antes que la posición.

**Método:**
- Objetos falsos para el decodificador, el hilo de fundido (0x81C bytes), el directorio, el historial y los eventos; las virtuales del decodificador se llaman por el hueco de la vtable del EXE.
- IDirectSoundBuffer y eventos con vtables propias trazadas; CreateThread, SetThreadPriority, ResumeThread, WaitForSingleObject, Sleep y QueryPerformanceCounter por IAT solo en el hilo principal.
- Se simulan `CFileMgr`, `operator new/delete` (solo durante la llamada), `CEntity::RegisterReference`, `CConversationForPed::Update/IsPlayerInPositionForConversation` y el audio de misión.
- Se guardan y restauran la región de `CConversations` (0x9691C4..0x969A20) y `CTimer::m_snTimeInMilliseconds`; `g_numSoundChannelsUsed` se compara por diferencia y se devuelve a su valor.
- Fuera de los fixtures:
  - `Initialise`, `FillBuffer` e `InitLibrary` de `CAEMFDecoder` (QuickTime en el EXE, Media Foundation en la DLL) y su destructor inicializado;
  - el `CreateThread` nulo de `CAESmoothFadeThread::Initialise` (comprobación NOTSA);
  - nombres duplicados en `CDirectory::AddItem`/`ReadDirFile` (FIX_BUGS); el `printf` del EXE por directorio lleno no se traza (la DLL usa su log);
  - conversaciones sin peatón en `IsConversationAtNode` (el EXE se cae).

Repetir: `python reversing/Sys10/build_probe.py` y `python reversing/run.py --case sys10`. Evidencia: [validation-summary.json](validation-summary.json).
