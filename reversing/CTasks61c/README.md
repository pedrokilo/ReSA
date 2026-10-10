<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ocho clases de tareas y atractores (lista 61c)

Banco preparado el 05/10/2026 (sin compilar ni ejecutar todavía): **28.928 pares original/candidato dentro de GTA** previstos para 60 funciones de `CTaskComplexGoToPointAndStandStillAndAchieveHeading`, `CTaskComplexGoToPointAnyMeans`, `CTaskComplexGotoDoorAndOpen`, `CTaskComplexGoToPointAiming`, `CTaskComplexUseClosestFreeScriptedAttractor`, `CPedShelterAttractor`, `CTaskSimpleRunNamedAnim` y `CTaskComplexWalkRoundCar`.

| Clase | Función | Casos |
|---|---|---|
| `CTaskComplexGoToPointAndStandStillAndAchieveHeading` (vtable 0x86FE58) | Constructor (0x668CD0), Destructor (0x668D40) | 256 + 256 |
| | `Clone` (hueco 1), `CreateFirstSubTask` (9) | 256 + 256 |
| | `CreateNextSubTask` (8), `ControlSubTask` (10) | 512 + 512 |
| `CTaskComplexGoToPointAnyMeans` (0x86FF68) | Constructor-1 (0x66B720), Constructor-2 (0x66B790), Destructor (0x66B830) | 256 x 3 |
| | `CreateSubTask` (0x6705D0), `CreateNextSubTask` (8), `ControlSubTask` (10) | 1024 x 3 |
| | `CreateFirstSubTask` (9) | 512 |
| `CTaskComplexGotoDoorAndOpen` (0x86FFC0) | Constructor-0 (0x66BB20), Constructor-1 (0x66BBA0), Destructor (0x66BC00), `Clone` (1) | 256 x 4 |
| | `CreateNextSubTask` (8) | 512 |
| | `CreateFirstSubTask` (9), `ControlSubTask` (10) | 1024 x 2 |
| `CTaskComplexGoToPointAiming` (0x86FE00) | Constructor (0x668790) | 512 |
| | Destructor (0x668870), `CreateSubTask` (0x6688D0), `CreateNextSubTask` (8), `Clone` (1), `GetTaskType` (4) | 256 x 5 |
| | `CreateFirstSubTask` (9), `ControlSubTask` (10) | 1024 x 2 |
| `CTaskComplexUseClosestFreeScriptedAttractor` (0x86E428) | Constructor (0x6346F0), Destructor (0x634720), `Clone`, `GetTaskType`, `CreateNextSubTask`, `ControlSubTask` | 256 x 6 |
| | `CreateFirstSubTask` (9) | 512 |
| | `ComputeClosestFreeScriptedEffect` (0x634740, stdcall) | 1024 |
| `CPedShelterAttractor` (0x86C5B4) | `GetDisplacement` (0x5EF420) | 512 |
| | `ComputeAttractPos` (hueco 3) | 512 |
| | `ComputeAttractHeading` (4) | 256 |
| | `BroadcastDeparture` (5) | 1024 |
| `CTaskSimpleRunNamedAnim` (0x86D54C) | Constructor-Default (0x6674B0), Destructor (0x61BF10), `Clone`, `GetTaskType` | 256 x 4 |
| | Constructor-Anim (0x61A990), `OffsetPedPosition` (0x61AB00) | 512 x 2 |
| | `ProcessPed` (hueco 7) | 1024 |
| `CTaskComplexWalkRoundCar` (0x86F308) | Constructor (0x6541B0), Destructor (0x656B00), `SetNewVehicle` (0x654290), `CreateRouteTask` (0x6542E0), `GoingForDoor` (0x654720), `Clone`, `GetTaskType`, `CreateNextSubTask` | 256 x 8 |
| | `ComputeRoute` (0x657B80) | 512 |
| | `ComputeRouteRoundSmallCar` (0x6544F0), `ComputeRouteRoundBigCar` (0x656BB0), `CreateFirstSubTask` (9), `ControlSubTask` (10) | 1024 x 4 |

`CTaskComplexGoToPointAndStandStillAndAchieveHeading::GetTaskType` ya estaba validada y no se repite.

## Reconstrucción desde el EXE 1.0 US (sin `FIX_BUGS`)

**`CTaskComplexGoToPointAndStandStillAndAchieveHeading`**
- `CreateFirstSubTask` (0x66E030): apaga el bit 0 de la bandera (+0x2C) y salta a `CreateSubTask(GO_TO_POINT)` (0x668D50).
- `ControlSubTask` (0x668E80): con el bit 0 llama al hueco 9 (`CreateFirstSubTask`); si no, con subtarea `GO_TO_POINT` copia `m_MoveState` al +8 de la subtarea y devuelve `m_pSubTask`.
- `Clone` (0x66CFD0) y `CreateNextSubTask` (0x66DFD0) ya coincidían (solo se quitó `.reversed = false`).

**`CTaskComplexGoToPointAnyMeans`**
- `CreateSubTask` (0x6705D0): `ENTER_CAR_AS_DRIVER` -> `CTaskComplexEnterCarAsDriver(veh)`; `LEAVE_CAR` -> `CTaskComplexLeaveCar(veh del peatón, 0, 0, true, false)`; `CAR_DRIVE_TO_POINT` -> `CTaskComplexDriveToPoint(veh, pos, -1, 0, -1, -1, 0)`; `CREATE_CAR_AND_GET_IN` -> `CTaskSimpleCreateCarAndGetIn(posición del peatón, modelo)`; `GO_TO_POINT_AND_STAND_STILL` -> `(estado, pos, radio, 2.0 (0x86FC88), false, false)`; `FOLLOW_NODE_ROUTE` -> `(estado, pos, radio, 3.0 (0x86FCA4), 2.0 (0x86FCA8), false, -1, true)`; el resto (incluido `TASK_FINISHED`) nulo.
- `CreateNextSubTask` (0x6728A0): tras `CREATE_CAR_AND_GET_IN` -> `CAR_DRIVE_TO_POINT` si el peatón está en coche (bit 8 de 0x46C) o `FOLLOW_NODE_ROUTE`; tras `ENTER_CAR_AS_DRIVER` lo mismo exigiendo además `m_pVehicle`; tras `CAR_DRIVE_TO_POINT` -> `LEAVE_CAR`; tras `LEAVE_CAR` -> `GO_TO_POINT_AND_STAND_STILL`; tras éste y `FOLLOW_NODE_ROUTE` -> `CreateSubTask(FINISHED)` (nulo); otro tipo, nulo.
- `ControlSubTask` (0x672A50): solo con subtarea `FOLLOW_NODE_ROUTE` (0x38A). Con modelo distinto de -1 arma un temporizador de 3000 ms la primera vez (+0x28/+0x2C/+0x30); si vencido (comparación sin signo) crea `CREATE_CAR_AND_GET_IN` y rearma. Si no, con el peatón a más de 50 (cuadrado `(dz² + dy²) + dx²` > 2500) y un coche en el escáner de vehículos (+0xD4 de la inteligencia, closest +0x4C) distinto del actual que `IsVehicleStealable` acepta, escribe el puntero SIN liberar la referencia anterior, crea `ENTER_CAR_AS_DRIVER` y registra la referencia después.
- `CreateFirstSubTask` (0x6729C0) ya coincidía.

**`CTaskComplexGotoDoorAndOpen`**
- `CreateFirstSubTask` (0x66BD40): el jugador marca `bPlayerOnInteriorTransition` (byte 0x10E del pad, bit 3) y el bit 3 de la tarea; añade un `CEventAreaCodes(ped)` de la pila al grupo global (`GetEventGlobalGroup()->Add(&ev, false)`); arma tiempo 1000 (+0x28/+0x2C/+0x30). Sin puerta: con bit 0 devuelve nulo, si no bit 1 y `GoToPoint(WALK, fin, 0.5)`. Con puerta (matriz leída sin guarda): producto `(fy·dy + fz·dz) + fx·dx` (> 0), punto de paso `puerta + derecha · 0,75`, inicio/fin = punto -/+ (-0,5, 2,0) · delantero con signo según el lado; si el bit 3 de las banderas físicas de la puerta está activo: bit 2 de la tarea y se apagan los bits 3 y 2 de la puerta; si el peatón reproduce la animación 2 se mezcla la animación 0 con 1000; `GoToPoint(WALK, inicio, 0.35)`.
- `CreateNextSubTask` (0x66C0D0): solo tras `GO_TO_POINT`; con el bit 1 avisa al pad del jugador y devuelve nulo; si no, crea `CTaskSimpleRunAnim(grupo 0, anim 0x99, 8.0)` como secundaria 4 si falta, `GoToPoint(WALK, fin, 0.2)` y activa el bit 1.
- `ControlSubTask` (0x66C1F0): con el temporizador activo (+0x30) y su tiempo cumplido (sin signo), o sin puerta con el bit 0, intenta `MakeAbortable(ped, 1, nulo)` de la subtarea (nulo y aviso al pad del jugador si lo logra); si no devuelve la subtarea.
- `Clone` (0x66BCA0) ya coincidía.

**`CTaskComplexGoToPointAiming`**
- `CreateSubTask` (0x6688D0): `GO_TO_POINT_AND_STAND_STILL` -> `CTaskComplexGoToPointAndStandStill(estado, destino, radio, distancia, false, false)`; `GUN_CTRL` -> `CTaskSimpleGunControl(entidad, &apuntado, nulo, 3 si `GetTaskType() == 0x385` o 0, 1, 600000)`; resto nulo.
- `CreateNextSubTask` (0x66DD70): el caso por defecto devuelve nulo (antes `NOTSA_UNREACHABLE`).
- `CreateFirstSubTask` (0x66DDB0): apaga el bit 0; tipo del arma activa (índice con signo), `GetWeaponSkill()` y `GetWeaponInfo`; con el bit 1 de las banderas del arma (`bAimWithArm`) crea la tarea de disparo secundaria 0 si falta (`CTaskSimpleUseGun(entidad, apuntado, 3, 3)` si `0x385`, `(…, 1, 1)` si no) o aborta la existente, y devuelve `GoToPointAndStandStill`; con el bit 0 y disparo instantáneo/de área devuelve `CreateSubTask(GUN_CTRL)`; si no `GoToPointAndStandStill`.
- `ControlSubTask` (0x6689E0): con el bit 0 llama al hueco 9; con subtarea `GoToPointAndStandStill` actualiza el destino (`GoToPoint(destino, 0.5, 2.0, false)`) y gestiona la tarea secundaria (crea, `ControlGun(ped, entidad, 2 con 1/64 de azar si es 0x385, si no 1)` o aborta); con subtarea `GUN_CTRL` y `GetTaskUseGun()` calcula el desplazamiento (derecha, -delantero) normalizado si su cuadrado supera el del radio y llama a `ControlGunMove`.

**`CTaskComplexUseClosestFreeScriptedAttractor`**
- `ComputeClosestFreeScriptedEffect` (0x634740, **stdcall**: el EXE no limpia la pila; la declaración cdecl anterior descuadraba la pila): recorre los 64 efectos activos (0xC3A1A0), con lista de usuarios (+0x20 de 0xC3A200) exige modelo del peatón (con signo de 16 bits) o marca -2 con su tipo, y elige el de menor distancia al cuadrado (estricto; NaN no entra) cuyo atractor tenga hueco (`GetPedAttractorManager()->HasEmptySlot(fx, nulo)`).
- `CreateFirstSubTask` (0x639530): `CInterestingEvents::Add(3, ped)` y, según `m_MoveState`, `CTaskComplexUseEffect(fx, nulo)` (4 y cualquier otro), `...Running` (6) o `...Sprinting` (7).

**`CPedShelterAttractor`**
- `GetDisplacement` (0x5EF420): devuelve un **puntero** al elemento (el prototipo devolvía un `CVector` por valor, ABI distinta); la primera vez rellena 5 desplazamientos `(cos·r, sin·r, 0)` con ángulo `rand · 1/32767 · 2π` y radio `2 · rand · 1/32767` (fcos/fsin x87), reintentando si alguno está a menos de 1 (la z del existente se toma sin restar).
- `ComputeAttractHeading` (0x5E9690): `rand · 3.0518509e-5 · 6.2831855` (constantes del EXE) en vez de `GetRandomNumberInRange(0, TWO_PI)`.
- `BroadcastDeparture` (0x5EF570): busca al peatón en la cola (`m_ArrivedPeds`), borra su pareja de tarea y su hueco, y a cada atraído le asigna `CTaskComplexGoToAttractor(this, pos, rumbo, tiempo, hueco, WALK)` con `SetTaskForPed` (sin adelantar a los de la cola, a diferencia de la base).
- `ComputeAttractPos` (0x5EFC40) ya coincidía.

**`CTaskSimpleRunNamedAnim`**
- `ProcessPed` (0x61BF20), `StartAnim` (0x61BB10, ahora método) y `FinishRunAnimMovePedCB` (0x61AAA0, ahora método estático): transcritos del EXE. Con la tarea terminada sin desplazamiento final, si el fundido > 100 y el peatón está a más de 1 del hueso 3, la asociación pasa a autoeliminación con fundido negativo; en marcha: purga de eventos con `m_bDontInterrupt`, temporizador, y `StartAnim` si no hay animación y no hay asociación con datos de callback.
- Constructor-Anim: `strcpy` (antes `strcpy_s`, que rellena con 0xFE en depuración y comprueba tamaño). `OffsetPedPosition`: la multiplicación de matriz por vector sigue el orden de sumas de `Multiply3x3` (0x59C790), no el de `CMatrix::TransformVector`.

**`CTaskComplexWalkRoundCar`**
- Constructor (0x6541B0): ya no inicializa por valor los campos que el EXE no escribe (0x30-0x53, nibbles altos de las banderas, relleno de la ruta: solo se pone a cero su contador); `SetNewVehicle` sin la comprobación `FIX_BUGS`.
- `ComputeRouteRoundSmallCar` (0x6544F0): esquinas y planos del vehículo, lado del peatón (resultado sin uso), con `m_bIsPedGoingForCarDoor` las 4 distancias al plano (orden de sumas distinto por plano) y el empuje del destino por el lado; después `ComputeRouteRoundEntityBoundingBox` con el nibble con signo del byte 0xE y guarda el resultado en los bits 2-5 del byte 0xD. Devuelve `planesDot[0]` por ST0 (basura del EXE que sus llamantes descartan).
- `ComputeRouteRoundBigCar` (0x656BB0): mismo lado -> ruta de dos puntos; lados distintos -> mueve temporalmente al peatón y el destino a los puntos de superficie, calcula la ruta pequeña y recompone la ruta; devuelve 0.0.
- `CreateFirstSubTask` (0x658200) y `ControlSubTask` (0x654370): transcritos (tiempo 15000/20000 por 4 con remolque, `NormaliseAndMag`, umbrales 2/4/6, `FindTaskByType(3, ENTER_CAR_AS_DRIVER)`, `CTaskSimpleStandStill` de la pila con `ProcessPed`, `CTaskSimpleAchieveHeading`; en `Control` comparación 0,0625 de distancia al cuadrado y 0,9 de producto escalar de los ejes). El hueco 9 se llama como virtual aunque la clase es `final`.

## Método

- 3 peatones con matriz (casi siempre; siempre en `Aiming`, `WalkRoundCar` y `RunNamedAnim`, donde el EXE la lee sin guarda), inteligencia con tareas primaria 3 y secundarias, escáner de vehículos y grupo de eventos, clump falso (tipo 2), tipo de peatón (jugador o no), armas, vehículo y grupo de animación; 2 vehículos con matriz, banderas (`bIsBig`, 0x400 del dword 0x428), conductor y remolques; una puerta (objeto con matriz y banderas físicas); 4 tareas falsas con vtable trazada (destructor, `GetTaskType`, `MakeAbortable` por máscara y `CreateFirstSubTask`); asociaciones, jerarquías de animación e información de arma falsas; atractor falso con vtable propia (huecos 3 y 4 trazados) y los tres `SArray` en buffers propios.
- Simulados con traza: RNG (rand del EXE y `CGeneral::GetRandomNumber`), `CAnimManager::GetAnimationBlock`/`GetAnimation`/`BlendAnimation` (2 variantes), `RpAnimBlendClumpGetAssociation` (2 variantes), `CEntity::UpdateRpHAnim`, `CPed::GetBonePosition`, `CEventGroup` (`Add`, `RemoveInvalidEvents`, `Reorganise`), `GetEventGlobalGroup`, `SetTaskSecondary` (EXE) / `ChangeTaskInSlot` (DLL, el inline llama a éste), `CTaskManager::FindTaskByType`, `CPed::GetWeaponSkill`, `CWeaponInfo::GetWeaponInfo`, `CPedIntelligence::GetTaskUseGun`, `CTaskSimpleUseGun::ControlGun`/`ControlGunMove`, `CCarEnterExit::IsVehicleStealable`/`IsPlayerToQuitCarEnter`, `GetPedAttractorManager`, `CPedAttractorManager::HasEmptySlot`, `CInterestingEvents::Add`, `CPedAttractor::SetTaskForPed`, `CTaskSimpleStandStill::ProcessPed` (cuerpo de la DLL resuelto con `OurVirtual` antes de parchear) y las funciones de `CPedGeometryAnalyser` que usa `WalkRoundCar` (esquinas, planos, lados, puntos de superficie y ruta alrededor). Se dejan reales `GetTaskSecondary` (se leen los huecos de la inteligencia falsa), `GoToPoint` (solo actualiza campos y lo usa el constructor de la DLL), `CTaskTimer::IsOutOfTime`, `NormaliseAndMag`, `GetRadianAngleBetweenPoints`, `LimitRadianAngle`, `SetFinishCallback`/`SetDeleteCallback`, `IsDriver` e `IsPlayer`.
- Las subtareas creadas salen del pool real (constructores del EXE en el original y de la DLL en el candidato) y se serializan enteras; igual las rutas de puntos y de nodos nuevas. Pools de tareas, rutas de puntos y rutas de nodos: `m_LastFreeSlot` y estados restaurados y slots libres con 0xCD en cada ejecución. Se guardan y restauran el reloj, los bytes 0x100-0x120 del pad 0 (se compara su estado tras cada llamada), la distancia de impacto 0x8D22B0, el `SArray` de desplazamientos (0xC0986C, con almacenamiento propio de 8 elementos: sin asignaciones) y los tres arrays de 2dfx scripted. Constructores sobre buffer con 0xCD comparados enteros (tipo en vez de vptr).
- `ControlSubTask` y las funciones que llaman a `GetTaskType`/`CreateFirstSubTask` sobre `this` usan una vtable falsa en el objeto (huecos 4 y 9 trazados) para no depender de la función hermana.

## Fuera del banco

- `CTaskSimpleRunNamedAnim::FinishRunAnimMovePedCB` y `StartAnim` no tienen fila en `hooks.csv` ni hook: `StartAnim` se ejercita desde `ProcessPed` (se comprueban las direcciones de callback pasadas a la asociación: `cb_move_ped`, `cb_finish`, `cb_default` sustituyen a las del EXE y de la DLL), pero el cuerpo del callback no se llama.
- `CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateSubTask` (0x668D50) no tiene hook ni fila: queda cubierto por los resultados de `CreateFirstSubTask`/`CreateNextSubTask` (el EXE usa el suyo y la DLL el propio).
- `GetDisplacement` con la tabla vacía **y sin almacenamiento** (puntero de inicio nulo) no se prueba: el `vector` reservaría memoria con `operator new`; el banco empieza siempre con almacenamiento propio de 8 elementos.
- `CTaskComplexGoToPointAiming` con el destino de ruta del `GunControl` y la tarea de apuntar: las constructoras de `CTaskSimpleUseGun` y `CTaskSimpleGunControl` se comparan enteras; si la de la DLL difiere del EXE, el fallo saldrá en `tasks`.

Repetir: `python reversing/CTasks61c/build_probe.py` y `python reversing/run.py --case ctasks61c`.
