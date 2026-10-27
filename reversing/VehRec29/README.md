<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Grabaciones de vehículos (lista de 28)

Validadas el 04/10/2026 con **11.136 pares original/candidato dentro de GTA**, todos exactos: 29 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CVehicleRecording` | `Init`, `InitAtStartOfGame`, `ShutDown`, `Render`, `RegisterRecordingFile`, `FindIndexWithFileNameNumber`, `HasRecordingFileBeenLoaded`, `RequestRecordingFile`, `RemoveRecordingFile`, `RemoveAllRecordingsThatArentUsed`, `Load`, `SmoothRecording`, `StartPlaybackRecordedCar`, `StopPlaybackRecordedCar`, `StopPlaybackWithIndex`, `SkipToEndAndStopPlaybackRecordedCar`, `PausePlaybackRecordedCar`, `UnpausePlaybackRecordedCar`, `IsPlaybackGoingOnForCar`, `IsPlaybackPausedForCar`, `SetPlaybackSpeed`, `ChangeCarPlaybackToUseAI`, `SetRecordingToPointClosestToCoors`, `SkipForwardInRecording`, `SaveOrRetrieveDataForThisFrame`, `RestoreInfoForCar`, `RestoreInfoForMatrix`, `InterpolateInfoForCar`, `RenderLineSegment` |

**Diferencias reales corregidas:**
- `CVehicleStateEachFrame`: los valores empaquetados son con signo (`movsx`, también giro, acelerador y freno) y se multiplican por el recíproco (1/127, 1/16383,5, 0,05, 0,01) en vez de dividirse; `PlayBackStreamingIndex` tiene 16 entradas (tenía 3).
- `RestoreInfoForCar`:
  - con pausa conserva el ángulo de giro y suelta el freno de mano;
  - en motos limpia `bGettingPickedUp` y `bOnSideStand`;
  - usa la matriz del vehículo sin crearla.
- `InterpolateInfoForCar`: `fila de la grabación * t + fila del vehículo * (1 - t)` en filas y posición, y velocidad `v * t + (1 - t) * velocidad`.
- `SaveOrRetrieveDataForThisFrame`:
  - el tiempo no avanza con la reproducción en pausa;
  - la búsqueda del fotograma recorre punteros como el EXE (antes la referencia al siguiente se fijaba una sola vez) y puede retroceder hasta el principio;
  - `printf` del EXE cuando el paso supera 500.
- `SetRecordingToPointClosestToCoors`: `PlaybackIndex` en bytes (guardaba el número de fotograma), mínimo inicial 0x497423FE y distancia `(dz² + dy²) + dx²`.
- `SmoothRecording` empieza en el fotograma 2 (empezaba en el 4) con comparaciones sin signo y `_ftol2`.
- `Load` recorre en bytes mientras `offset < m_nSize` y reserva con `CMemoryMgr::Malloc(size, 0)`.
- `RequestRecordingFile` pide el modelo aunque no la encuentre y ya no libera la grabación; `HasRecordingFileBeenLoaded` mira la entrada 0 si no la encuentra.
- `RemoveRecordingFile` y `RemoveAllRecordingsThatArentUsed` recorren las 475 entradas (la segunda sin mirar el número de fichero).
- `StopPlaybackWithIndex` y `SkipToEndAndStopPlaybackRecordedCar` liberan al llegar a 0 referencias sin comprobar `m_pData`; `SkipToEnd...` quita `bDisableCollisionForce` (no `bCollidable`) y usa el último fotograma por bytes.
- `SkipForwardInRecording` recorre la distancia restante como el EXE (avanza mientras sea > 0 y solo retrocede si empezó <= 0).
- Comportamiento del EXE sin comprobación (antes eran desviaciones NOTSA):
  - `ChangeCarPlaybackToUseAI`, `PausePlaybackRecordedCar`, `UnpausePlaybackRecordedCar`, `SkipForwardInRecording` y `StartPlaybackRecordedCar` usan el índice 16 si no hay reproducción o hueco;
  - `RegisterRecordingFile` no comprueba el segundo `sscanf`.

**Método:**
- Grabaciones falsas: 8 buffers de hasta 24 fotogramas con holgura legible después del final (el EXE lee el tiempo del fotograma siguiente al último), tiempos crecientes, iguales, desordenados o enormes, y valores empaquetados con extremos.
- Vehículos falsos con desplazamientos crudos del EXE (matriz propia, banderas, piloto automático, tipo y subtipo, banderas de moto) y vtable con trampas; `ProcessControlCollisionCheck` en el hueco 23 se traza.
- Nunca se cargan ficheros carrec ni se usa el streaming o vehículos reales. Se simulan y trazan `CStreaming::RemoveModel`/`RequestModel`, `CEntity::RegisterReference`, `CPhysical::RemoveAndAdd`, `CEntity::UpdateRwMatrix`/`UpdateRwFrame`, `CTrain::FindPositionOnTrackFromCoors`, `MarkSurroundingEntitiesForCollisionWithTrain` y las primitivas RW. `CMemoryMgr::Malloc`/`Free` y `printf` se sustituyen solo durante la llamada, sin reservas.
- Los casos con índice 16 se limitan a los que no salen de la región guardada (en `ChangeCarPlaybackToUseAI` el EXE lee `pVehicleForPlayback[0]` como buffer y `PlaybackIndex[0]` como tamaño: se acota para leer dentro del vehículo falso; con las 16 ranuras ocupadas la grabación tiene datos).
- Se guardan y restauran en cada ejecución y al final 0x97D670..0x97F634 (todas las variables de `CVehicleRecording`, `StreamingArray` y `NumPlayBackFiles`), `CReplay::Mode`, los relojes y los índices temporales de RW.
- El relleno de las matrices (0xC, 0x1C, 0x2C, 0x3C) se enmascara: `InterpolateInfoForCar` lo copia de una temporal sin inicializar.

Repetir: `python reversing/VehRec29/build_probe.py` y `python reversing/run.py --case vehrec29`. Evidencia: [validation-summary.json](validation-summary.json).
