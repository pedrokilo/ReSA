<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CWorld, grupo C: jugadores, sectores, Add/Remove y blancos desprevenidos

Validadas el 07/10/2026 con **13.120 pares original/candidato dentro de GTA**, todos exactos: 27 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CWorld` (mundo) | `Initialise`, `Add`, `Remove`, `ClearForRestart`, `ClearScanCodes`, `CastShadowSectorList<CPtrListDoubleLink<CPhysical*>>`, `CameraToIgnoreThisObject` |
| `CWorld` (sectores) | `GetCurrentScanCode`, `GetSector`, `GetRepeatSector`, `GetLodPtrList` |
| `CWorld` (jugador, funciones globales) | `FindPlayerPed`, `FindPlayerVehicle`, `FindPlayerEntity`, `FindPlayerTrain`, `FindPlayerCoors`, `FindPlayerSpeed`, `FindPlayerHeading`, `FindPlayerWanted`, `FindPlayerCentreOfWorld`, `FindPlayerCentreOfWorldForMap`, `FindPlayerCentreOfWorld_NoSniperShift` |
| `CWorld` (ranuras de jugador) | `FindPlayerSlotWithPedPointer`, `FindPlayerSlotWithRemoteVehiclePointer` (0x563FD0), `FindPlayerSlotWithVehiclePointer` (0x564000) |
| `CWorld` (blancos) | `FindUnsuspectingTargetCar`, `FindUnsuspectingTargetPed` |

**Diferencias reales corregidas:**
- `FindPlayerSlotWithVehiclePointer`:
  - el hook estaba instalado sobre 0x563FD0, que en el EXE es `FindPlayerSlotWithRemoteVehiclePointer` (compara `Players[i].m_pRemoteVehicle`); la función de vehículo está en 0x564000. Ahora ambas tienen su hook (y `docs/hooks.csv` recoge las dos direcciones);
  - la versión del EXE compara `CPed::m_pVehicle` de cada ped no nulo (sin mirar `bInVehicle`) y, si no hay coincidencia, devuelve 0 (no -1).
- `GetRepeatSector`: el índice es `coord & 0xF` con enteros con signo (-1 da 15); la DLL usaba `abs(coord) % 16` (-1 daba 1).
- `FindUnsuspectingTargetCar` y `FindUnsuspectingTargetPed`:
  - distancia inicial 9999,9 (0x864F90 / 0x461C3F9A), no `FLT_MAX`;
  - el EXE recorre el pool de mayor a menor índice (los empates los gana el índice más alto);
  - los NaN descartan el candidato (`fcomp` + `test ah,5` / `test ah,41h`), no lo aceptan;
  - el producto escalar suma `(dz·fz + dy·fy) + dx·fx`;
  - peatones: se aceptan los tipos `CIVMALE` y `GANG1`..`GANG10` ambos inclusive (la DLL excluía GANG1 y GANG10), y se descartan los que YA están en `TASK_COMPLEX_WANDER` (la condición estaba invertida).
- `CameraToIgnoreThisObject`: el índice de modelo se pasa a `IsModelIndexADoor` con extensión de signo (`movsx word`); con 0xFFFF la DLL pasaba 65535.
- `FindPlayerVehicle`: el vehículo remoto sale de `Players[playerId]`, no de la ranura del ped (`GetPlayerInfoForThisPlayerPed`).
- `FindPlayerCentreOfWorld`: el vehículo remoto se mira sin exigir `bInVehicle` (el control remoto no lo tiene puesto) y el vehículo normal se busca siempre en el jugador en foco; la DLL usaba `FindPlayerVehicle(playerId, true)`.
- `FindPlayerCentreOfWorld_NoSniperShift`: vehículo remoto y ped final con el índice crudo (sin sustituir negativos por `PlayerInFocus`), vector global a cero 0xB9B970 cuando no hay ped.
- `FindPlayerHeading`: transcripción exacta (`naked`): `fpatan` devuelto en ST0 sin redondear, remoto con el índice crudo y sin exigir `bInVehicle`, vehículo normal con el índice resuelto.
- `ClearForRestart`: recorre las 16×16 celdas de repetición (la DLL recorría 120×120 vueltas).

**Sin diferencia (MATCH):** `Initialise`, `Add`, `Remove`, `ClearScanCodes`, `CastShadowSectorList`, `GetCurrentScanCode`, `GetSector`, `GetLodPtrList`, `FindPlayerPed`, `FindPlayerEntity`, `FindPlayerTrain`, `FindPlayerCoors`, `FindPlayerSpeed`, `FindPlayerWanted`, `FindPlayerCentreOfWorldForMap`, `FindPlayerSlotWithPedPointer`, `FindPlayerSlotWithRemoteVehiclePointer`.

**Diferencia no reproducible (fuera de banco):** `FindPlayerEntity` y `FindPlayerSpeed` en el EXE desreferencian el ped sin comprobar nulo (caen si no hay ped); la DLL devuelve nulo. `FindPlayerHeading` y `FindPlayerCentreOfWorld_NoSniperShift` leen `Players[-1]` con un índice negativo en los accesos sin resolver: solo se prueban índices 0 y 1.

**Método:**
- Jugadores, cámara (posición y matriz), `PlayerInFocus` y `bCarsGeneratedAroundCamera` falsos; peatones y vehículos de pools falsos con los campos en sus desplazamientos reales, empates, NaN y orientaciones nulas.
- Pools de nodos locales y pools de entidades falsos para las listas de sectores, repetición y LOD (la DLL de depuración valida nodos e ítems); vtable falsa de entidad (destructor, `Add`, `Remove` trazados).
- Dependencias simuladas y trazadas en el EXE y en la DLL (`IsAlive`, `GetActiveTask`, `UpdateRwFrame`, `UpdateRwMatrix`, listas de movimiento, `IsModelIndexADoor`, `GetPositionRelativeToOutsideWorld`, `CIplStore::Initialise`, borrado de cinemática/proyectiles/objetos, `CPickups::ReInit`, `CPools::CheckPoolsEmpty`).
- `FindPlayerHeading` se compara con los 80 bits de ST0.
- Todo el estado global tocado se guarda y se restaura por ejecución y al final.

Repetir: `python reversing/WldC/build_probe.py` y `python reversing/run.py --case wldc`. Evidencia: [validation-summary.json](validation-summary.json).
