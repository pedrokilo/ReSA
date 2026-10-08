<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPedIntelligence (lista de 34)

Preparado el 04/10/2026: **14.016 pares original/candidato dentro de GTA** para las 52 funciones de `CPedIntelligence`
(pendiente de compilar y ejecutar).

| Grupo | Funciones |
|---|---|
| Ciclo de vida (2) | Constructor, Destructor |
| Datos y rangos (11) | `GetPedEntities`, `SetPedDecisionMakerType`, `SetPedDecisionMakerTypeInGroup`, `RestorePedDecisionMakerType`, `SetHearingRange`, `SetSeeingRange`, `GetEffectInUse`, `SetEffectInUse`, `RecordEventForScript`, `IsInterestingEntity`, `GetActivePrimaryTask` |
| Consultas de tareas (14) | `FindTaskByType`, `GetTaskFighting`, `GetTaskUseGun`, `GetTaskThrow`, `GetTaskHold`, `GetTaskSwim`, `GetTaskDuck`, `GetTaskJetPack`, `GetTaskInAir`, `GetTaskClimb`, `GetUsingParachute`, `IsInACarOrEnteringOne`, `GetMoveStateFromGoToTask`, `IsPedGoingForCarDoor` |
| Tareas y eventos (10) | `AddTaskPhysResponse`, `AddTaskEventResponseTemp`, `AddTaskEventResponseNonTemp`, `AddTaskPrimaryMaybeInGroup`, `SetTaskDuckSecondary`, `ClearTaskDuckSecondary`, `ClearTasks`, `FlushImmediately`, `FlushIntelligence`, `IsRespondingToEvent` |
| Relaciones y percepción (8) | `IsInSeeingRange`, `FindRespectedFriendInInformRange`, `IsFriendlyWith`, `IsThreatenedBy`, `Respects`, `AreFriends`, `TestForStealthKill`, `CanSeeEntityWithLights` |
| Proceso (7) | `ProcessFirst`, `Process`, `ProcessAfterProcCol`, `ProcessAfterPreRender`, `ProcessEventHandler`, `ProcessStaticCounter`, `LookAtInterestingEntities` |

**Diferencias reales corregidas:**
- `GetActivePrimaryTask` (0x4B85B0): devuelve la primera tarea no nula de los huecos 0, 1 y 2; la DLL miraba siempre el hueco 0.
- `IsInACarOrEnteringOne` (0x601CC0): devuelve el vehículo en EAX (los llamadores del EXE, 0x5FABAB y 0x69A368, hacen `test eax,eax`); la DLL devolvía `bool`.
- `GetUsingParachute` (0x6011B0): recorre todas las asociaciones hasta encontrar una del bloque "parachute"; la DLL solo comprobaba la primera.
- `TestForStealthKill` (0x601E00):
  - `GetBonePosition` se llama antes de mirar si se agacha o la salud;
  - producto `y*fy + z*fz + x*fx`;
  - el evento actual del objetivo y el evento actual de su grupo se prueban por separado (cada uno basta para devolver falso si odia o le cae mal); la DLL exigía los dos.
- `CanSeeEntityWithLights` (0x605550): distancia `z² + x² + y²`, términos `(d / 30) * L * L` en ese orden con las constantes del EXE, y NaN o no positivo pasa a la segunda prueba y acaba en 0.
- `ProcessStaticCounter` (0x605650): comparaciones sin signo con 4 (`jbe`/`ja`), suma modular, la tarea se vuelve a buscar por la inteligencia del peatón, distancia `z² + y² + x²`.
- `IsInSeeingRange` (0x600C60): `z² + y² + x²` y `z*fz + y*fy + x*fx`.
- `FindRespectedFriendInInformRange` (0x600CF0): número de peatones con signo, máximo 16 entradas y distancia `z² + x² + y²`.
- `ProcessFirst` (0x6073A0): producto `z*fz + y*fy + x*fx` de la velocidad del choque.
- `LookAtInterestingEntities` (0x6020D0): tiradas con `CGeneral::GetRandomIntInRange` (0x407180).
- `FlushIntelligence` (0x601DA0): primero `ResetResponse` y después `ResetHistory`.
- `FlushImmediately` (0x601640): el tipo de objeto y la visibilidad se restauran sobre la entidad de la tarea clonada.
- Constructor (0x607140):
  - el estado mental pone a cero el enfado y arranca su temporizador con intervalo 0;
  - `CEventScanner` (0x605300, llamado por el constructor):
    - `field_0 = 1`, efecto y `field_14` nulos, y `Clear` en el escáner de atractores;
    - el escáner de conocidos permite peatones de script;
    - el próximo escaneo es `tiempo - ftol((rand() & 0xFFFF) / 32768 * -3000)`.

**Método:**
- Dos inteligencias en buffers propios y seis peatones falsos con offsets crudos del EXE.
- Tareas falsas (tipo, subtarea y retornos en el objeto; vtable trazada), clones en un array fijo, eventos falsos (borrado y `GetSourceEntity` trazados), grupos falsos.
- Vehículo, objeto, matrices, clump/frame, asociaciones y jerarquías de animación falsas.
- Se simulan y se trazan (EXE y cuerpo de la DLL):
  - `CLoadMonitor`, `CPedGroups::GetPedsGroup`, `CPedGroupIntelligence::SetScriptCommandTask`/`ComputeDefaultTasks`;
  - `CEventGroup::Add`/`HasScriptCommandOfTaskType`/`Flush`, `CEventHandler::HandleEvents`/`FlushImmediately`, `CTaskManager::ManageTasks`, `CPedScriptedTaskRecord::Process`;
  - `GetWanderTaskByPedType`, `CTaskSimpleUseGun::ClearAnim`, `CTaskSimpleDuck::ProcessPed`;
  - `GetBonePosition`, `GetLightingTotal`, `IsLooking`/`LookAt`, `GetIsOnScreen`, `UpdateRwFrame`, `FindObjectsInRange`, `rand`/`GetRandomNumber`, asociaciones de animación, `FixHandsToBars`;
  - escáneres de vehículos, entidades, eventos, atascos y colisiones, `Clear` de escáneres y atractores, registro de relaciones con el jugador.
- `CTask::IsTaskPtr` de la DLL devuelve cierto (las tareas falsas no están en el pool).
- Pool de tareas: `m_LastFreeSlot` restaurado y huecos libres con 0xCD en cada ejecución; las tareas reales que quedan vivas se vuelcan (tipo en vez del vptr) y se liberan.
- Constructor y destructor: búfer con 0xCD; los vptrs del grupo de eventos y de los escáneres se comparan como "vt".
- Fuera de los fixtures:
  - el camino del FX del cóctel molotov en `ProcessAfterPreRender` (arma 18 siempre sin FX);
  - `ClearTasks` en vehículo sin tarea por defecto (el EXE no la comprueba);
  - entidades sostenidas que no son punteros en `FlushImmediately`.

Repetir: `python reversing/PedIntel52/build_probe.py` y `python reversing/run.py --case pedintel52`.
