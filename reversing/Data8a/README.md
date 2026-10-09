<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Pistas de cinemática, canal estático, generador de coches, checkpoints, colisión y bandas (lista de 8)

Validadas el 02/10/2026 con **18.176 pares original/candidato dentro de GTA**, todos exactos: 47 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAECutsceneTrackManager` | `Initialise`, `PlayPreloadedCutsceneTrack`, `StopCutsceneTrack`, `IsCutsceneTrackActive`, `GetCutsceneTrackStatus`, `PauseTrack`, `PreloadBeatTrack`, `Service` |
| `CAEStaticChannel` (VMT) | `Service`, `IsSoundPlaying`, `GetPlayTime`, `GetLength`, `Play`, `SynchPlayback`, `Stop` |
| `CCarGenerator` | `CalcNextGen`, `SwitchOff`, `SwitchOn`, `Setup`, `CheckForBlockage`, `CheckIfWithinRangeOfAnyPlayers`, `DoInternalProcessing`, `Process` |
| `CCheckpoints` | `Init`, `Shutdown`, `SetHeading`, `Update`, `PlaceMarker`, `UpdatePos`, `DeleteCP`, `Render` |
| `CCollisionData` | `RemoveCollisionVolumes`, `Copy`, `CalculateTrianglePlanes`, `RemoveTrianglePlanes`, `GetTrianglePoint`, `GetShadTrianglePoint`, `SetLinkPtr`, `GetLinkPtr` |
| `CGangs` | `Initialise`, `Load`, `Save`, `SetGangWeapons`, `ChooseGangPedModel`, `GetWillAttackPlayerWithCops`, `SetWillAttackPlayerWithCops`, `SetGangPedModelOverride` |

**Diferencias reales corregidas:**
- `CAEStaticChannel`:
  - `Service` compara sin signo el tiempo transcurrido desde la sincronización (`jbe`) y libera el buffer antes de anular el puntero;
  - `Stop` pone `m_bPaused` a falso al final.
- `CCarGenerator`:
  - `Setup` usa la constante del EXE para el ángulo (0x8722E8, no es el float más cercano a 256/360);
  - `CheckIfWithinRangeOfAnyPlayers` reproduce las comparaciones con NaN del EXE (distancia, altura 950, distancia − 20 y producto escalar con la velocidad);
  - `DoInternalProcessing`: de noche es hora > 21 o < 7 (`ClockHoursInRange(21, 7)` nunca era cierto), `NumParkedCars` con signo, descarta la clase `BIG` (no los barcos), sin colisión no mira la longitud, ángulo sin signo con la constante 0x859BBC y el tractor pone el radio de borrado ampliado a 255.
- `CCheckpoints`:
  - `SetHeading` usa `fcos`/`fsin` del x87;
  - `Update` pide la posición del jugador una sola vez, el paso de `TORUSTHROUGH` es 0,01 y la igualdad baja el toro pero NaN no, como en el EXE;
  - `PlaceMarker` recibe el color como cuatro argumentos sueltos (12 argumentos, como los llamadores del EXE), solo reutiliza checkpoints en uso y compara `fsqrt(d²) < 2` con cuatro llamadas a `FindPlayerCoors`.
- `CCollisionData`:
  - `RemoveCollisionVolumes` y `RemoveTrianglePlanes` solo llaman a `Free` con punteros no nulos;
  - `Copy` reescrita como el EXE: los vértices se reservan siempre que haya triángulos (también con índice máximo 0) y antes que los triángulos, las cajas copian 27 bytes, los bits de discos y sombras se copian siempre y devuelve `this`;
  - `CalculateTrianglePlanes` lee el contador con signo y pasa la pista 0 a `Malloc`.
- `CGangs`:
  - `SetGangWeapons` no escribe armas negativas;
  - `ChooseGangPedModel` lee el contador del grupo con signo y la banda como palabra con signo;
  - `Get/SetWillAttackPlayerWithCops` comprueban el rango `PED_TYPE_GANG1..GANG10` en lugar del `assert`.

**Método:**
- Las pistas y los canales son objetos falsos; `CAEAudioHardware`, `CAESmoothFadeThread::RequestFade` y las conversiones de `CAEAudioChannel` se simulan solo durante cada caso, y los buffers DirectSound son objetos COM falsos.
- Los generadores usan model infos y colisiones falsos en huecos nulos de `ms_modelInfoPtrs`, entidades falsas y un pool de vehículos falso; `DoInternalProcessing` se prueba hasta antes de crear el vehículo (`ProcessVerticalLine` e `IsBoatModel` simulados devuelven falso) y en `Process` se sustituye por un stub.
- Checkpoints y bandas se prueban sobre sus arrays reales, guardados y restaurados; `CCheckpoint::Render`, `RwRenderStateSet`, el work buffer y las secuencias de `CCarCtrl` se simulan.
- La colisión usa objetos y arrays propios y una arena fija para `CMemoryMgr`.
- Se guardan y restauran el reloj, las pausas, los contadores de canales y coches aparcados, la hora, la cámara, el área, el pool de vehículos, los checkpoints, las bandas, la zona y el streaming de los huecos usados.
- Fuera de los fixtures:
  - la creación de vehículos de `DoInternalProcessing`;
  - el `assert` NOTSA de `GetLinkPtr` (el enlace siempre apunta al objeto);
  - contadores de 16 bits negativos en `Copy` (el EXE recorre 4 G elementos).

Repetir: `python reversing/Data8a/build_probe.py` y `python reversing/run.py --case data8a`. Evidencia: [validation-summary.json](validation-summary.json).
