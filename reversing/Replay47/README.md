<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CReplay (lista de 47)

Banco preparado el 04/10/2026: **18.208 pares original/candidato dentro de GTA** para las 47 funciones de `CReplay` (pendiente de compilar y ejecutar).

| Grupo | Funciones |
|---|---|
| Tablas y utilidades | `FindPoolIndexForPed/Vehicle`, `CanWeFindPoolIndexForPed/Vehicle`, `FindSizeOfPacket`, `ShouldStandardCameraBeProcessed`, `EnableReplays`, `DisableReplays` |
| Grabación | `RecordThisFrame`, `StorePedUpdate`, `StorePedAnimation`, `StoreClothesDesc`, `RecordPedDeleted`, `RecordVehicleDeleted`, `GoToNextBlock`, `EmptyReplayBuffer`, `MarkEverythingAsNew`, `Init` |
| Reproducción | `PlayBackThisFrameInterpolation`, `PlayBackThisFrame`, `FastForwardToTime`, `ProcessPedUpdate`, `RetrievePedAnimation`, `RestoreClothesDesc`, `DealWithNewPedPacket`, `CreatePlayerPed`, `TriggerPlayback`, `FinishPlayback` |
| Estado guardado | `StoreStuffInMem`, `RestoreStuffFromMem`, `StorePlayerInfoVariables`, `RestorePlayerInfoVariables`, `EmptyPedsAndVehiclePools_NoDestructors` |
| Recorridos de buffers | `IsThisPedUsedInRecording`, `IsThisVehicleUsedInRecording`, `InitialisePed/VehiclePoolConversionTable`, `InitialisePoolConversionTables`, `FindFirstFocusCoordinate`, `NumberFramesAvailableToPlay`, `StreamAllNecessaryCarsAndPeds` |
| Cámara, HD y bucle | `ProcessReplayCamera`, `ProcessLookAroundCam`, `Display`, `SaveReplayToHD`, `PlayReplayFromHD`, `Update` |

**Diferencias reales corregidas (resumen):**
- Paquetes: se escriben en el buffer campo a campo como el EXE (sin poner a cero el relleno, el byte 5 de la primera animación ni los bits altos de los flags); los bits del paquete de peatón son `m_bIsVisible`, `m_bTunnel` y `m_bTunnelTransition` (no hablar/polígono/colisión); el de cámara copia `TheCamera.m_mCameraMatrix`; los guardados de coche, moto, BMX y tren (0x45B730, 0x45C070, 0x45BF90, 0x45CD10) se transcriben en el sitio.
- Cámaras: `ProcessReplayCamera`, `ProcessLookAroundCam` y la reproducción del paquete de cámara usan `m_mCameraMatrix` (+0x974); la cámara fija usa `up × dir` y normaliza los tres vectores; la vista cenital pone la posición con +15 en el frame; mirar alrededor se activa con |x| o |y| > 0,01 (ratón × 0,005), `fsin/fcos/fpatan` del x87, distancia z,y,x, choque desplazado 0,25 en los tres ejes y `FindPlayerCoors`/`FindPlayer*` repetidos como el EXE.
- Reproducción: tras crear un vehículo se vuelve a mirar el pool y se extrae en el mismo frame; `CreatePlayerPed` no corta en buffers vacíos o no disponibles; `TriggerPlayback` empieza en el buffer 1 si no hay ninguno en uso, lee la emisora antes de parar la radio y guarda 0 sin vehículo; `PlayReplayFromHD` cierra el fichero antes de `TriggerPlayback` y lee sin límite de 8 buffers.
- Recorridos: `IsThis*UsedInRecording` solo miran buffers disponibles; `GetIndex` dividía dos veces (todas las búsquedas filtradas miraban el buffer 0); `NumberFramesAvailableToPlay` no avanzaba (bucle infinito); `FindFirstFocusCoordinate` pone la salida a cero; conversión de peatones antes que la de vehículos, con los límites exactos del EXE.
- Estado: `StoreStuffInMem`/`RestoreStuffFromMem` usan memoria cruda (sin constructor ni destructor de `CCamera`); la hora del replay vive en 0x97FAEF/0x97FAEE; no se restauran bandas ni camellos (FIX_BUGS quitado); el área se extiende sin signo; `OldRadioStation` es un dword.
- Otros: `Update` usa `GetIsKeyboardKeyJustDown`; `Display` usa el contador de 16 bits del EXE (bit 5) y las escalas 1/640 y 1/448; las animaciones se descomprimen multiplicando por 1/63,75, 1/85 y 1/127,5; ropa con estadísticas con signo; recorridos de pools del último al primero donde el EXE lo hace.

**Método:**
- Pools falsos de peatones (140) y vehículos (110) con objetos crudos, matrices propias, vtable falsa (borrado y `GetRideAnimData` trazados) y byte de guarda en el índice -1.
- Ventanas de 4 KB al principio y al final de cada buffer, rellenadas en cada ejecución; los 800 KB se guardan y restauran.
- Se guardan y restauran por ejecución los datos de `CReplay`, la cámara, el jugador 0, `CTimer`, reloj, tiempo, población, trazas de bala, ratón, flags de menú/pausa/corte y 100 entradas de `ms_aInfoForModel`.
- Stubs trazados para mundo, streaming, referencias, animaciones, ficheros, fuentes, audio, cámara RW y fundidos; las funciones de `CReplay` que crearían entidades reales (o para aislar `Update`, `TriggerPlayback`, `FinishPlayback`...) se sustituyen solo dentro de su caso.
- Fuera de los fixtures: creación real de peatones y vehículos (`DealWithNewPedPacket` con modelo cargado, paquetes de vehículo con slot libre y modelo cargado), extracción de vehículos existentes (`tReplay*Block::Extract*`, no catalogadas), `REPLAY_PACKET_PARTICLE` y tipos ≥ 20 (el EXE entra en bucle infinito) y el desbordamiento de la tabla de vehículos (el EXE puede quedarse colgado).

Repetir: `python reversing/Replay47/build_probe.py` y `python reversing/run.py --case replay47`.
