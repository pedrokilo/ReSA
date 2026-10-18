<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# Arreglos de audio fuera de listas

Validadas el 03/10/2026 con **5.120 pares original/candidato dentro de GTA**, todos exactos: 7 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAESound` | `UpdatePlayTime` |
| `CAEStreamingChannel` | `GetPlayTime`, `GetLength` |
| `CAEStaticChannel` | `GetPlayTime`, `GetLength` |
| `CAECollisionAudioEntity` | `UpdateParameters` |
| `CAEPedAudioEntity` | `UpdateParameters` |

**Diferencias reales corregidas:**
- `CAESound::UpdatePlayTime`: el EXE suma progreso · velocidad y el tiempo actual en float (`fild`/`fmul`/`fiadd`) y trunca después con `_ftol2`; la DLL truncaba el producto antes de sumar. El bugfix reversible de la división por cero solo actúa con longitud 0 (una longitud negativa sigue el `idiv` del EXE).
- `CAEAudioChannel::GetPlayTime`/`GetLength` devuelven 32 bits, como el EXE (la DLL los declaraba `int16`/`uint16`):
  - `CAEStreamingChannel` devuelve el tiempo de reproducción, el estado o el `long` de `GetStreamLengthMs` sin truncar;
  - `CAEStaticChannel` extiende con signo el resultado de `ConvertFromBytesToMS` (`movsx eax,ax`);
  - `CAEAudioHardware::GetChannelPlayTimes` guarda los 16 bits bajos y `RescaleChannelVolumes` usa el tiempo completo.
- `CAECollisionAudioEntity::UpdateParameters` (0x4DA370, hueco 0 de la vtable 0x862E64) no estaba reconstruida (la DLL usaba la vacía de `CAEAudioEntity`):
  - eventos 2, 3 y 128 fijan la velocidad a 1,26 / 0,94 / 0,56 cuando ya pasó el instante de `m_ClientVariable` y `curPlayPos >= 0`;
  - el evento 1 no hace nada;
  - el resto, con `curPlayPos == -1`, limpia la primera entrada de la lista con ese sonido y descuenta `m_NumActiveCollisionSounds`.
- `CAEPedAudioEntity::UpdateParameters`:
  - se instalaba con `RH_ScopedInstall` siendo virtual (bucle infinito al desactivarla); ahora es `RH_ScopedVMTInstall` en la vtable 0x86C2A8 (una entrada);
  - `m_ClientVariable` se convierte con `_ftol2` (64 bits, parte baja) antes de compararla sin signo con la hora;
  - la estela al nadar sube 0,6 dB por llamada hasta el volumen por defecto (la DLL no guardaba la subida) y la comparación con NaN sigue al EXE.
- `CAEVorbisDecoder`: se quitan las desviaciones `FIX_BUGS` (rechazo de más de dos canales en `Initialise`, duplicado de fuentes mono en `FillBuffer`); lo valida de nuevo Audio13 con fuentes mono y de más de dos canales.

**Método:**
- El hilo de streaming de audio se aparca en `IsForegroundApp` durante todo el banco, como en Audio13.
- Todo corre sobre objetos falsos: `CAESound`, canal estático (vtable falsa copiada de 0x85F3CC con `IsSoundPlaying` simulado), canal de streaming (decoder falso con `GetStreamLengthMs` simulado), `CAECollisionAudioEntity` con la lista de 300 entradas apuntando a un pool de sonidos falsos y `CAEPedAudioEntity` con un peatón falso con o sin matriz. Las funciones de vtable se llaman por la entrada real de la vtable del EXE.
- Se simulan y registran en el EXE y en la DLL `CAEAudioUtility::GetCurrentTimeInMS` (en otros hilos devuelve la hora real), `CAEStreamingChannel::UpdatePlayTime`, `CAEAudioChannel::GetCurrentPlaybackPosition`/`ConvertFromBytesToMS` (con valores de 32 bits para probar la extensión de signo), `CAESound::SetPosition` y `StopSoundAndForget`.
- `CTimer::m_snTimeInMilliseconds` se fija por caso y se restaura; se vigilan las vtables tocadas y las entradas de `IsForegroundApp`.
- Fuera de los fixtures:
  - longitud 0 en `UpdatePlayTime` (bugfix reversible de la DLL; el EXE divide por cero);
  - NaN e infinitos en velocidades, volúmenes y `m_ClientVariable`.

Repetir: `python reversing/FixAudio/build_probe.py` y `python reversing/run.py --case fixaudio`. Evidencia: [validation-summary.json](validation-summary.json).
