<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
