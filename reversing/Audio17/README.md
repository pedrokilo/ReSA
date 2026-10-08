<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Audio de colisiones (lista de 17)

Validadas el 03/10/2026 con **11.088 pares original/candidato dentro de GTA**, todos exactos: 17 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAECollisionAudioEntity` | `Initialise`, `InitialisePostLoading`, `Reset`, `Service`, `AddCollisionSoundToList`, `GetCollisionSoundStatus`, `ChooseCollisionSoundID`, `PlayOneShotCollisionSound`, `PlayLoopingCollisionSound`, `UpdateLoopingCollisionSound`, `PlayBulletHitCollisionSound`, `ReportGlassCollisionEvent`, `ReportWaterSplash` (posición y físico), `ReportObjectDestruction`, `ReportCollision`, `ReportBulletHit` |

**Diferencias reales corregidas:**
- `Initialise` escribe cada entrada de la lista campo a campo; la asignación de un temporal pisaba también el byte de relleno +0x13, que el EXE no toca.
- `ReportGlassCollisionEvent`, `ReportWaterSplash` (las dos), `ReportObjectDestruction`:
  - preparan `m_tempSound` con `CAESound::Initialise` y lo piden con `RequestNewSound`, como el EXE (la DLL usaba una voz local y dejaba `m_tempSound` sin tocar);
  - los campos se escriben después de `Initialise` en el orden del EXE: velocidad 0, hora de arranque, evento y flags (el segundo sonido del invernadero se inicializa con velocidad 0,56 y luego se pone a 0);
  - `ReportWaterSplash` del físico registra la entidad en `m_tempSound` antes de poner el evento en cada uno de los dos sonidos.
- `ChooseCollisionSoundID` repite el sorteo sin límite hasta salir un id distinto del historial, como el EXE (la DLL se rendía a los 100 intentos y devolvía el mínimo).
- `ReportCollision` y `ReportObjectDestruction` comparan el modelo de la entidad extendido con signo con `MI_*` sin signo (`movsx`/`movzx`): un `MI_*` no cargado (0xFFFF) ya no coincide con una entidad de modelo 0xFFFF.

**Método:**
- Todo corre sobre una `CAECollisionAudioEntity` falsa (vtable del EXE, contenido aleatorio por caso, lista de 300 entradas con sonidos de un pool falso) y entidades falsas con los offsets crudos del EXE (tipo, modelo, matriz, velocidades, flags físicos, inteligencia, subtipo de vehículo).
- Se simulan y registran en el EXE y en la DLL `CAESound::Initialise` (escribe los mismos campos en las dos pasadas), `RegisterWithPhysicalEntity`, `SetPosition`, `StopSoundAndForget`, `RequestNewSound` (registra la voz y si es `m_tempSound` o local), `AreSoundsOfThisEventPlayingForThisEntityAndPhysical`, `IsSoundBankLoaded`, `LoadSoundBank`, `IsLoadingTuneActive`, `GetTaskSwim`, `rand` y `CGeneral::GetRandomNumber`.
- `ReportCollision` corre con `PlayOneShot`/`PlayLooping`/`UpdateLoopingCollisionSound` simulados (se comparan sus argumentos; `GetCollisionSoundStatus` es real) y `ReportBulletHit` con `PlayBulletHitCollisionSound` simulado.
- Los falsos solo registran en el hilo principal y se ponen y quitan en cada pasada. Se fijan y restauran la hora, `MI_BASKETBALL`, `MI_PUNCHBAG`, `MI_GRASSHOUSE`, `MI_IMY_GRAY_CRATE` y la entrada 19990 de `ms_modelInfoPtrs`.
- Fuera de los fixtures:
  - superficies >= 179 en `PlayLoopingCollisionSound` y `PlayBulletHitCollisionSound` (arreglos `GenericOOB` y `InvalidSurfaceType` de la DLL; el EXE lee fuera de la tabla);
  - superficies >= 195 en el resto (el EXE lee fuera de `gCollisionLookup` y del historial);
  - edificios con superficie especial en `ReportCollision` (`GetFrictionEffect` fuera de tabla);
  - impulsos NaN o enormes en `PlayOneShotCollisionSound`.
- `UpdateParameters` (0x4DA370, hueco 0 de la vtable 0x862E64) no está en la lista: se reconstruyó el 03/10/2026 y se valida en el banco [FixAudio](../FixAudio/README.md).

Repetir: `python reversing/Audio17/build_probe.py` y `python reversing/run.py --case audio17`. Evidencia: [validation-summary.json](validation-summary.json).
