<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples (lista de 43)

Validadas el 02/10/2026 con **18.880 pares original/candidato dentro de GTA**, todos exactos: 40 funciones de 8 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleAchieveHeading` | `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetUpIK` |
| `CTaskSimpleAffectSecondaryBehaviour` | Constructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleDieInCar` | Constructor, Destructor, `Clone`, `GetTaskType`, `ProcessPed` |
| `CTaskSimpleFacial` | `Clone`, `GetAnimId`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleGetUp` | Constructor, `FinishGetUpAnimCB`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleRunAnim` | Constructor (dos sobrecargas), `Clone`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleSetStayInSamePlace` | Constructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleTogglePedThreatScanner` | Constructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |

**Diferencias reales corregidas:**
- `CTaskSimpleAchieveHeading`:
  - `MakeAbortable` usa 2π exacto (0x40C90FDB; el `TWO_PI` de `common.h` es impreciso);
  - `SetUpIK` llama dos veces a `GetTaskType` del padre (primero contra `AVOID_OTHER_PED_WHILE_WANDERING`) y calcula el punto de mirada con `fsin`/`fcos` x87 y el redondeo de la FPU.
- `CTaskSimpleAffectSecondaryBehaviour`:
  - destructor (0x6912C0) que borra la tarea guardada;
  - `Clone` (0x692910) clona la tarea guardada antes de reservar la nueva;
  - `ProcessPed` termina si no hay tarea secundaria y, si la hay, pide abortar (LEISURE) y sigue esperando (estaba invertido).
- `CTaskSimpleDie` (llamada desde el `ProcessPed` de `CTaskSimpleDieInCar`): los constructores solo limpian los dos bits bajos de las banderas (`and byte ptr [esi+20h],0FCh`) y `StartAnim` solo quita `ANIMATION_IS_FINISH_AUTO_REMOVE` (`and byte ptr [eax+2Eh],0F7h`).
- `CTaskSimpleFacial::ProcessPed`: la tirada es `(int)((rand() & 0xFFFF) * (1/32768) * 100) < 40` y el temporizador se arranca sin comprobar que la duración sea >= 0.
- `CTaskSimpleGetUp`:
  - `MakeAbortable` compara el tiempo transcurrido sin signo con `(uint32)(int)(paso * 0,02 * 1000) * 3,0` en float;
  - `StartAnim` solo vuelve a probar la colisión uno de cada 8 fotogramas; los demás sigue atascado.

**Método:**
- Original con los hooks del lote y `CTaskSimpleDie::StartAnim` desactivados (el `ProcessPed` de `DieInCar` salta a `CTaskSimpleDie::ProcessPed` 0x6397E0, que llama a 0x637520).
- Constructores llamados en la dirección del EXE sobre un búfer rellenado con 0xCD (se compara el tipo en vez del vptr); virtuales llamados por la vtable del EXE.
- Falsos en búferes propios: peatones (vtable trazada, hueco 23 `SetMoveAnim`), clump, inteligencia, estadísticas, asociaciones de animación, jerarquía, entidad, dos vehículos con matriz, tareas padre, subtareas (vtable trazada) y evento.
- Simulados y trazados:
  - `CAnimManager::BlendAnimation` (las dos sobrecargas), `RpAnimBlendClumpGetAssociation`/`GetFirstAssociation` y `SetFinishCallback`;
  - `CPed::SetMoveState`/`SetPedState`/`ClearAll`/`StopNonPartialAnims`, `FindPlayerPed`, `CEntity::GetIsOnScreen` e `IKChainManager_c`;
  - `CTaskManager::GetTaskSecondary`/`SetTaskSecondary`, `CPedPlacement::IsPositionClearOfCars`, `IsVehiclePointerValid`, `CCollision::ProcessColModels`;
  - `CEventDamage::AffectsPed`, `CPedDamageResponseCalculator::ComputeDamageResponse`, `CEventGroup::Add` y el generador aleatorio (`rand` 0x821B1E y `CGeneral::GetRandomNumber`).
- Se fijan y restauran el reloj, `ms_fTimeStep`, el contador de fotogramas y `Pads[0].DisablePlayerControls`; las tareas nuevas del pool se siguen (`m_LastFreeSlot` restaurado y huecos libres con 0xCD).

Repetir: `python reversing/STasks43/build_probe.py` y `python reversing/run.py --case stasks43`. Evidencia: [validation-summary.json](validation-summary.json).
