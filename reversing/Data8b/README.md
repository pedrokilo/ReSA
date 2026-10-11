<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8547 funciones reconstruidas; 8547 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9226 entradas binarias sin nombre catalogado; 822 destinos originales explícitos fuera del catálogo, 398 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 102 aprobadas dentro del juego; 133 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Eventos de bici y script, tareas de script, sombras, rectángulos y coches atascados (lista de 8)

Validadas el 02/10/2026 con **17.024 pares original/candidato dentro de GTA**, todos exactos: 48 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventKnockOffBike` | Constructores (los dos), `AffectsPed`, `ReportCriminalEvent`, `From`, `SetPedOutCar`, `CalcForcesAndAnims`, `SetPedSafePosition` |
| `CEventScriptCommand` | Constructor, `GetEventType`, `GetEventPriority`, `Clone`, `AffectsPed`, `TakesPriorityOver`, `IsValid`, `CloneScriptTask` |
| `CPedScriptedTaskRecordData` | Constructor, `AssociateWithEvent`, `AssociateWithTask`, `Set` (los dos), `SetAsGroupTask`, `SetAsAttractorScriptTask`, `Flush` |
| `CRealTimeShadow` | Constructor, Destructor, `Create`, `Destroy`, `DrawBorderAroundTexture`, `GetShadowRwTexture`, `SetLightProperties`, `Update` |
| `CRect` | `IsFlipped`, `Restrict`, `Resize`, `IsPointInside` (los dos), `SetFromCenter`, `GetCenter`, `StretchToPoint` |
| `CStuckCarCheck` | `Init`, `AddCarToCheck`, `AttemptToWarpVehicle`, `ClearStuckFlagForCar`, `HasCarBeenStuckForAWhile`, `IsCarInStuckCarArray`, `Process`, `RemoveCarFromCheck` |

**Diferencias reales corregidas:**
- `CRect::IsPointInside` (los dos): comparaciones negadas como en el x87; un NaN cuenta como dentro.
- `CEventScriptCommand`:
  - `GetEventPriority` vuelve a pedir `GetTaskType` por la vtable en cada comparación;
  - `TakesPriorityOver` pide el tipo del evento de referencia en cada comparación y su prioridad antes que la propia;
  - `Clone` clona la tarea antes de reservar el evento.
- `CEventKnockOffBike`:
  - el constructor sin argumentos no toca `m_forceKnockOff` y `From` solo copia el bit de `m_isVictimDriver`;
  - `AffectsPed` solo descarta con `CantBeKnockedOffBike == NEVER` (sin mirar `bHasBeenRendered`);
  - `CalcForcesAndAnims`:
    - dirección con signo y animación 15 (sin efectos en el tipo por defecto) fuera de [0, 3];
    - con velocidad NaN toma la rama lenta en las direcciones 1..3;
    - multiplica por 1/70 (0x85B328) en vez de dividir y usa `ApplyMoveForce(x, y, z)` pidiendo antes el número aleatorio;
  - `SetPedSafePosition`:
    - rumbo con `fpatan` en línea y PI exacto (0x40490FDB) redondeado una sola vez; con `up.z` NaN no suma PI;
    - producto escalar (z·z' + y·y') + x·x';
    - en el tipo 0x31 no vuelve a mirar la línea de visión si hubo colisión;
    - la puerta de salida se extiende con signo.
- `CRealTimeShadow`:
  - el constructor no escribe las esferas pero sí `m_bDrawMoreBlur`;
  - `Destroy` destruye primero las dos cámaras;
  - `Update` toma el marco padre del objeto RW también para átomos (sin `GetRpClump`).
- `CStuckCarCheck`:
  - `AddCarToCheck` y `Process` con `GetAtRef` en línea sin comprobar límites; `AddCarToCheck` ocupa el primer hueco con manejador negativo;
  - `HasCarBeenStuckForAWhile` solo mira el primer hueco con ese manejador;
  - `Process`: distancia (z² + y²) + x² y búsqueda de nodos alrededor de la posición actual del vehículo.

**Método:**
- Original con los hooks del lote desactivados frente a candidato con fixtures idénticos.
- Falsos: peatones y vehículos (vtable de trampas, `TestCollision` trazado), tareas y eventos (vtable propia), objetos RW (luz, marcos, objeto RW, cámaras y rasters), un pool de vehículos (5 huecos de 0xA18 con byte de guarda) y un modelo con su colisión.
- Simulados y trazados:
  - `FindPlayerWanted`, `CWanted::RegisterCrime`, `CPhysical::ApplyMoveForce` (las dos), `CGeneral::GetRandomNumberInRange<float>`/`LimitRadianAngle`, `CMatrix::SetRotateZOnly`;
  - `CPed::SetPedPositionInCar`/`PositionPedOutOfCollision`, `CWorld::GetIsLineOfSightClear`/`FindMissionEntitiesIntersectingCube`, `CTaskSimpleCarSetPedOut::ProcessPed` (la tarea de la pila se serializa en el momento), `CCarEnterExit::RemoveCarSitAnim`;
  - `CCamera::IsSphereVisible`, `CCarCtrl::SetCoordsOfScriptCar`, `CUpsideDownCarCheck::IsCarUpsideDown`, `CEntity::GetBoundCentre`, `CPathFind` (`FindNodeCoorsForScript` las dos, `FindNthNodeClosestToCoors`, `FindNodeOrientationForCarPlacement`);
  - `CShadowCamera` (destrucción, creación, luz, centro, actualización, remuestreo, desenfoque y borde), las llamadas RW de luces y marcos, `RwV3dTransformPoints` y el RNG.
- Se guardan y restauran en cada ejecución y al final: el pool de eventos (huecos libres con 0xCD), el reloj, el puntero del pool de vehículos, `CWorld::pIgnoreEntity`, `TheCamera.m_bMirrorActive` y `CModelInfo::ms_modelInfoPtrs[400]`.

Repetir: `python reversing/Data8b/build_probe.py` y `python reversing/run.py --case data8b`. Evidencia: [validation-summary.json](validation-summary.json).
