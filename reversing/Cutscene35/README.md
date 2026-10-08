<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CCutsceneMgr (lista de 34)

Banco del 04/10/2026: **13.184 pares original/candidato dentro de GTA** para las 35 funciones de `CCutsceneMgr` (pendiente de compilar y ejecutar).

| Grupo | Funciones |
|---|---|
| Carga | `LoadCutsceneData`, `LoadCutsceneData_overlay`, `LoadCutsceneData_preload`, `LoadCutsceneData_loading`, `LoadCutsceneData_postload`, `LoadAnimationUncompressed`, `AppendToNextCutscene`, `BuildCutscenePlayer` |
| Objetos | `CreateCutsceneObject`, `UpdateCutsceneObjectBoundingBox`, `CalculateBoundingSphereRadiusCB`, `SetCutsceneAnim`, `SetCutsceneAnimToLoop`, `SetHeadAnim`, `AddCutsceneHead`, `AttachObjectToBone`, `AttachObjectToFrame`, `AttachObjectToParent` |
| Reproducción | `StartCutscene`, `SetupCutsceneToStart`, `HideRequestedObjects`, `Update`, `Update_overlay`, `IsCutsceneSkipButtonBeingPressed`, `SkipCutscene`, `FinishCutscene`, `HasCutsceneFinished`, `GetCutsceneTimeInMilleseconds` |
| Limpieza | `DeleteCutsceneData`, `DeleteCutsceneData_overlay`, `RemoveCutscenePlayer`, `RemoveEverythingBecauseCutsceneDoesntFitInMemory`, `LoadEverythingBecauseCutsceneDeletedAllOfIt`, `Initialise`, `Shutdown` |

**Diferencias reales corregidas:**
- `LoadCutsceneData_preload`:
  - se reescribe el lector del `.CUT` como el EXE: líneas separadas por `\n`/`\r`, secciones por igualdad exacta, `sscanf` con comas (`%d,%d,%s`, `%d,%d,%d`, `%f,%f,%f,%s`), `strtok`/`atoi`/`atof` en `peffect`, `offset` leído desde el carácter 7 y mayúsculas ASCII del texto;
  - el nombre del `.CUT` se forma en `gString`;
  - descarga los modelos 300-319 (el 319 faltaba), con `m_nRefCount` con signo, y cambia la clave aunque el modelo no esté cargado;
  - los objetos añadidos por script se copian desde el índice 0 de la lista de añadidos (antes desde el índice de destino);
  - un solo `FindPlayerPed`; el wanted sale de `m_pPlayerData` (puede ser nulo);
  - al sacar al jugador del coche la tarea se crea con `bWarpingOutOfCar` (el EXE lo pone tras el constructor).
- `LoadCutsceneData_loading`: solo espera a los modelos especiales 300-319; los efectos usan `atoi` y `RwFrameGetLTM`; el adjunto es el objeto `attachment` sobre el ped (antes el ped sobre sí mismo).
- `LoadCutsceneData_postload`: nombres `.IFP`/`.DAT` en `gString`; desplazamiento con los 32 bits de la posición; `ms_animLoaded` se escribe al final.
- `DeleteCutsceneData_overlay`:
  - pone `ms_useCutsceneShadows` a 1;
  - borra los objetos del último al primero bajando el contador;
  - con `m_bDontClearZone` recarga peds, armas y vehículos iniciales y devuelve las armas guardadas (antes era un `assert`);
  - `CPad::Clear` tras `DrasticTidyUpMemory`; `CWorld::Players[PlayerInFocus]` para `MakePlayerSafe`.
- `HideRequestedObjects`: el contador se pone a cero y se rellena con las entidades ocultadas (antes crecía mientras se recorría); se registra la referencia sin limpiar la anterior.
- `UpdateCutsceneObjectBoundingBox` y `CreateCutsceneObject`: el radio parte del valor previo del col model, se limita a `max(previo, 20)`; `CreateCutsceneObject` lo pone a 0 antes y llama a `SetModelIndex` antes de guardar el objeto.
- `CalculateBoundingSphereRadiusCB`: sincroniza la esfera dos veces, usa `RwV3dTransformPoints` y solo guarda si es estrictamente mayor (NaN no actualiza).
- `SetCutsceneAnim`: `SetCurrentTime(0)` (no `Start`) y después quita `IS_PLAYING` y pone `CAN_EXTRACT_VELOCITY`.
- `Update_overlay`: paso `* 0.02f`, ms con `_ftol2` de 64 bits y comparaciones sin signo (fin de efecto estrictamente mayor, texto estrictamente mayor); estados de reproducción fuera de rango no cambian; cualquier estado de carga distinto de 1/2 no hace nada.
- `FinishCutscene`: tiempo sin signo `* 0.001f` en la x87; `GetCutsceneTimeInMilleseconds`: `_ftol2(segundos * 1000)` (antes truncaba antes de multiplicar).
- `IsCutsceneSkipButtonBeingPressed`: espacio recién pulsado (nueva y no anterior) y `isForeground` como dword.
- `RemoveEverythingBecauseCutsceneDoesntFitInMemory` y `LoadEverythingBecauseCutsceneDeletedAllOfIt`: implementadas como el EXE (antes `NOTSA_UNREACHABLE`/`assert`).
- `AppendToNextCutscene`, `LoadAnimationUncompressed` y `LoadCutsceneData_overlay`: `strcpy`/`_strlwr` sin límites (sin relleno 0xFE de `strcpy_s`).

**Método:**
- Objetos de cinemática, clumps (con datos de animación en `ClumpOffset`), asociaciones, jerarquías, frames, atomics, entidades, sistemas de partículas, model infos 300-319 y el directorio `CUTS.IMG` son falsos; el `.CUT` se genera en memoria con secciones al azar.
- Se simulan y trazan con un registro fijo (sin reservas) todas las llamadas externas, el constructor de `CCutsceneObject` (EXE y DLL, exportado) con una vtable trazada, `CObject::operator new` y `operator new/delete` del EXE durante la llamada.
- Se guardan y restauran todos los globales de `CCutsceneMgr`, los col models de cinemática, densidades, `gString`, el pad 0 y los teclados/ratón, `isForeground`, el mensaje grande, los punteros de model info 300-319, la información de streaming 290-399, el modo de cámara, el paso de tiempo y el color extra.
- Fuera de los fixtures:
  - el jugador dentro de un vehículo (`CTaskSimpleCarSetPedOut` real);
  - el primer objeto de la lista marcado como repetido (el EXE usa un puntero de pila sin inicializar);
  - líneas del `.CUT` incompletas (el EXE lee variables sin inicializar o pasa `NULL` a `sscanf`/`strncpy`);
  - decimales no exactos en el `.CUT` (CRT del EXE frente a la de la DLL).

Repetir: `python reversing/Cutscene35/build_probe.py` y `python reversing/run.py --case cutscene35`.
