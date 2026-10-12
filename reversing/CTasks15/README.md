<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Evitar peatones al deambular, trepar y agacharse (lista de 15)

Validadas el 02/10/2026 con **19.712 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexAvoidOtherPedWhileWandering` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask`, `QuitIK`, `SetUpIK`, `ComputeSphere`, `NearbyPedsInSphere`, `ComputeAvoidSphere`, `ComputeRouteRoundSphere`, `ComputeDetourTarget` |
| `CTaskSimpleClimb` | Constructor, `MakeAbortable`, `ProcessPed`, `StartAnim`, `StartSpeech`, `DeleteAnimCB`, `GetCameraStickModifier`, `GetCameraTargetPos`, `ScanToGrab`, `ScanToGrabSectorList`, `TestForClimb`, `TestForStandUp`, `TestForVault`, `CreateColModel`, `Shutdown` |
| `CTaskSimpleDuck` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `CanPedDuck`, `ControlDuckMove`, `DeleteDuckAnimCB`, `ForceStopMove`, `IsTaskInUseByOtherTasks`, `RestartTask`, `SetControlType`, `SetDuckTimer`, `SetMoveAnim` |

**Diferencias reales corregidas:**
- `CTaskComplexAvoidOtherPedWhileWandering`:
  - el constructor ya no pone a cero `m_StartPt`, `m_NewTargetPt` ni el relleno de los temporizadores; `Clone` usa el constructor con los campos propios (el constructor de copia leía los del objeto nuevo) y copia el bit de objetivo móvil;
  - `ControlSubTask`: al abortar la subtarea sale sin `QuitIK`; altura `> 3` y distancias `> 1` y `> 7,25` (antes `>=`); la tarea más simple es la del peatón a evitar y el temporizador se mira antes que `IsGoToTask`; producto `(z + y) + x` contra `0.923f`; sin objetivo móvil recalcula el desvío en vez de salir; `UpdatePoint(m_DetourTargetPt, 0.5, false)` (antes el objetivo); `StandStill` de `GetRandomIntInRange(0, 2000) + 500`;
  - `SetUpIK`: sale si ya hay entidad mirada o tarea secundaria de IK (estaban invertidas), sigue sin padre, `GetIsOnScreen` antes de `m_IsDoingIK`, normaliza con `Normalise`, NaN sale, el tipo del padre se pide dos veces (una por comparación) y la altura del punto de mirada parte del peatón;
  - `ComputeSphere`: radio `sqrt + 1,05`, centro por `1/n`, máximo inicial 0 sin NaN y la esfera se escribe con `Set` (sin tocar `m_nLight`);
  - `NearbyPedsInSphere` guardaba un nulo en vez del peatón y abandonaba el recorrido sin hueco; `ComputeAvoidSphere` escribe la esfera inicial antes del escáner; `CreateFirstSubTask` calcula el desvío en línea y crea la tarea antes del temporizador.
- `CTaskSimpleClimb`:
  - `fsin`/`fcos`/`fpatan` del x87 sin redondear y `CMatrix * CVector` con el orden de sumas de 0x59C890 en todas las posiciones de agarre (`GetHandholdPosAndAngleForEntity`, `GetClimbOffset3D`);
  - `ProcessPed`: el paso fino solo con `|d|² < 0,1` (NaN va al grueso), `operator/` multiplica por `1/paso`, el contador suma `paso * 0,02 * 1000` truncado con comparaciones de 16 bits con signo, la velocidad del tren es el módulo 3D, al acabar `frente * 0,02` (no `/ 50`) sin sumar 0 a x/y y el mando se consulta antes que el forzado;
  - `ScanToGrabSectorList`: nado y peso una sola vez, distancia `sqrt((dx² + dz²) + dy²)` con NaN fuera, tren con módulo 3D, producto `(z + y) + x`, normal plana con NaN, normal orientada con `+ n.z * 0`, ángulos con `fpatan` y restauración de esferas con signo de 16 bits;
  - `ScanToGrab`: sectores con `* 0,02 + 60` (no `/ 50`), sector repetido con `& 15`, centro y vuelta a local por 0x59C890 y rumbo restado sin redondear;
  - `GetCameraStickModifier`: la caída del salto solo se calcula si es negativa y con el desplazamiento de salto; el rumbo es el de la entidad del argumento con las constantes del EXE (`HALF_PI`, `PI`, `TWO_PI`, `1,7707964`);
  - `GetCameraTargetPos`: los lerps mezclaban horizontal con vertical; `StartAnim` comprueba la animación antes de soltarla; `CreateColModel` usa `CColSphere::Set` (no copia `m_nLight`); `DeleteAnimCB` sin comprobación de tipo; `ScanToGrabSectorList` recorre los nodos directamente (como el EXE, sin validarlos).
  - `CTask::IsGoToTask` (compartida, sin hook) pide el tipo dos veces como 0x61A360; lo usa `ControlSubTask`.
- `CTaskSimpleDuck`:
  - `ProcessPed`: duración estrictamente superada y aborto `LEISURE`; contador de disparos `duración - paso * 0,02 * 1000`; antes de agacharse desvanece `IDLE_TIRED` (-2) y la otra animación de agacharse (no siempre `WEAPON_CROUCH`); `SetDeleteCallback` (no `SetFinishCallback`); la cuenta atrás compara el valor previo;
  - `MakeAbortable` acepta el contador de disparos 0; `RestartTask` reinicia la hora y usa `1000 - rand * (1/32768) * -1500`; `SetDuckTimer` reinicia la hora;
  - `ControlDuckMove` devuelve 1, NaN copia la dirección y `|x| > 0,9`; `SetMoveAnim` conserva NaN en la velocidad (asignación directa) y NaN en y va a la parada; `IsTaskInUseByOtherTasks` trata NaN como mezclada; `CanPedDuck` usa `GetWeaponSkill()` y `>= RUN`;
  - `MakeAbortable` y `ProcessPed` piden `GetTaskUseGun` dos veces antes de `ClearAnim`, como el EXE;
  - `DeleteDuckAnimCB` sale con datos o animación nulos (antes `assert`); `AbortBecauseOfOtherDuck` soltaba la animación de agacharse ya nula.

**Método:**
- 4 peatones con vtable falsa (`SetMoveAnim` trazado), matriz, inteligencia, escáner de peatones y tareas falsas en el gestor de tareas (`GetSubTask`, `GetTaskType` y `MakeAbortable` trazados); 6 entidades (2 edificios, 2 objetos, 2 vehículos) con matriz y un modelo real con colisión (solo se lee su radio); asociaciones de animación, jerarquía, mando y evento falsos.
- Mundo falso para trepar: las listas de los sectores 59..61 × 59..61 y de sus sectores repetidos se cambian por cadenas de nodos propias y `CCollision::ProcessColModels` se simula con un guion de contactos (punto, normal, superficie y pieza).
- Simulados con traza: RNG, `FindPlayerPed`, animaciones (`BlendAnimation`, `GetAssociation`, callbacks), `SetMoveState`, `Say`, `GetWeaponSkill`, `ApplyMoveForce`, `GetSpeed`, `SetOrientation`, `ProcessVerticalLine`, `GetIsOnScreen`, `GetBoundCentre`, `GetColModel`, IK, `GetTaskSecondary`, `GetTaskSwim`, `FindTaskByType`, `GetTaskUseGun`/`ClearAnim`, `UpdatePoint`, `AreInSameGroup`, `ComputeRouteRoundSphere`, `CalculateOffsetPosition`, `CEventGroup::Add`, `GetFatAndMuscleModifier`, `CanClimb`, `GetPadFromPlayer` y `GetExitVehicle`.
- Se guardan y restauran el reloj, el paso, el código de búsqueda, `m_aTempColPts`, la matriz estática del EXE (0xC195E8), el estático de `ControlSubTask`, el umbral de mirada, `DisablePlayerControls`, las listas de sectores y los 4 modelos de colisión de trepar (`CreateColModel` y `Shutdown` se prueban sobre copias que se liberan).
- Las entidades de las listas siempre tienen matriz (el EXE y el DLL la asignarían en el pool real con `GetMatrix`); los edificios sin matriz solo aparecen en `GetCameraStickModifier`/`GetCameraTargetPos`.
- Constructores sobre buffers con 0xCD comparados enteros salvo el vptr; tareas nuevas del pool serializadas y borradas.
- Máscaras: los bits 1-7 del byte de flags y los campos de sombras (0x20..0x2F) de `CCollisionData` (`AllocateData`, 0x40F870, no los escribe y quedan con lo que devuelva `CMemoryMgr::Malloc`), el byte `m_nLight` de las esferas (ninguno de los dos lo escribe) y la esfera local de `ComputeDetourTarget` (en el EXE sin inicializar) se trazan sin ese byte.
- Fuera del banco: la rama `FIX_BUGS` del paso fino de `CTaskSimpleClimb::ProcessPed` (límite de velocidad 0,2): los casos usan un paso de tiempo ≥ 1,6, con el que el desplazamiento fino nunca supera 0,2.

Repetir: `python reversing/CTasks15/build_probe.py` y `python reversing/run.py --case ctasks15`. Evidencia: [validation-summary.json](validation-summary.json).
