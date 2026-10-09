<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Eventos y tareas (lista de 56)

Validadas el 02/10/2026 con **4.352 pares original/candidato dentro de GTA**, todos exactos: 23 funciones con `validated=1`.

| Grupo | Funciones |
|---|---|
| Constructores de eventos (9) | `CEventAcquaintancePedHate` (Constructor2), `CEventDontJoinPlayerGroup`, `CEventLeaderEntryExit`, `CEventNewGangMember`, `CEventPedCollisionWithPlayer`, `CEventPlayerCollisionWithPed`, `CEventPlayerCommandToGroupGather`, `CEventScriptedAttractor`, `CEventSeenCop` (Constructor1) |
| `AffectsPed` (2) | `CEventEscalator`, `CEventOnFire` |
| Constructores de tareas (9) | `CTaskComplexFollowPedFootsteps`, `CTaskComplexKillPedOnFoot`, `CTaskComplexPartner`, `CTaskComplexPartnerChat`, `CTaskComplexPartnerDeal`, `CTaskComplexPartnerShove`, `CTaskComplexWalkRoundObject`, `CTaskSimpleUninterruptable`, `CScriptedBrainTaskEntry` |
| Virtuales (3) | `CTaskSimpleAnim::MakeAbortable`, `CTaskSimpleDrown::ProcessPed`, `CTaskSimpleDrownInCar::ProcessPed` |

**Diferencias reales corregidas:**
- `CEventScriptedAttractor`: el constructor del EXE recibe 2 argumentos (`ret 8`) y la envoltura de la DLL recibía 3, lo que desbalanceaba la pila de las llamadas del EXE.
- `CTaskComplexPartner`: registraba la referencia sobre el parámetro de la pila en lugar del miembro (+0x38).
- `CTaskComplexFollowPedFootsteps` y `CTaskComplexWalkRoundObject`: las rutas de puntos se reservan sin inicializar por valor (el EXE solo pone a cero el número de entradas).
- `CTaskComplexKillPedOnFoot`: los bits 0, 1 y 7 del byte +0xC no se escriben (también afecta a la variante sigilosa).
- `CTaskComplexPartnerChat`/`Deal`: `strcpy` en vez de `strcpy_s` (relleno 0xFE en depuración).
- `CEventEscalator::AffectsPed`: modelo con signo frente a `MI_*` sin signo, como el EXE.
- `CTaskSimpleAnim::MakeAbortable`: la comprobación de `TASK_SIMPLE_NAMED_ANIM` en el comando de script también se hace en la ruta IMMEDIATE.
- `CTaskSimpleDrownInCar::ProcessPed` llama a `FindPlayerPed` una segunda vez antes de cambiar el estado del vehículo.

**Método:**
- Constructores llamados en un búfer rellenado con 0xCD, comparado entero (con el tipo de evento/tarea en vez de la vtable).
- Peatones, entidad, vehículo, inteligencia, animación y clump falsos.
- Se simulan y se trazan `SetPedState`, `FindPlayerPed`, `IncrementStat`, `CEventGroup::Add`, `SetFinishCallback`, `BlendAnimation` y `CTaskSimpleDie::ProcessPed`.
- El pool de rutas de puntos, el reloj y `MI_ESCALATORSTEP` se restauran en cada ejecución.

Repetir: `python reversing/EventsTasks56/build_probe.py` y `python reversing/run.py --case evtask56`. Evidencia: [validation-summary.json](validation-summary.json).
