<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEventHandler: respuesta a la colisión entre peatones (grupo F)

Validadas el 07/10/2026 con **2.048 pares original/candidato dentro de GTA**, todos exactos: 1 función de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventHandler` | `ComputePedCollisionWithPedResponse` |

**Diferencias reales corregidas:**
- `ComputePedCollisionWithPedResponse` estaba sin reconstruir (el hook llevaba `{ .reversed = false }` y la función reenviaba al EXE). Se ha reescrito desde el ensamblador de 0x4BDB80..0x4BE7CB:
  - "delante" es `dot >= 0` y "detrás" es `dot <= 0` (con NaN falsos), con el producto escalar en el orden `(z + y) + x`;
  - con grupo: líder de cada grupo, `bPushOtherPeds` (bit 31 del dword +0x474) al líder si es la víctima o el dueño del manejador, víctima jugador (solo tarea de decir), tiradas 0,25 / 0,33 / 0,66 (mirada o decir) y paso evasivo con la dirección `(fx, -fy, 0)` normalizada o ajuste del radio de la tarea de buscar entidad (0x4BC470) y de la de ir a un punto;
  - sin grupo: ruta alrededor de la víctima (`CTaskComplexFollowPointRoute`), caerse (`CTaskComplexFallAndGetUp`, con las debilidades de defensa del EXE) o `CTaskComplexHitResponse`, con el orden exacto de mirada / tarea de decir / tarea de respuesta de cada rama;
  - al final se escribe siempre `m_EventResponseTask` y, si hay respuesta y la víctima es el jugador, se llama a `IncrementAngerAtPlayer(1)`.

**Método:**
- Original (hook desactivado) frente a candidato con fixtures idénticos; la función se llama en la dirección del EXE con `this` = manejador propio de 0x34 bytes con centinelas en las tareas de respuesta, de decir y parcial.
- Tres peatones falsos (vtable de trampas, matriz propia, inteligencia y estadísticas), dos grupos falsos, diez tareas falsas de 0x80 bytes (vtable propia: destructor, `GetSubTask`, `IsSimple`, `GetTaskType` y `MakeAbortable`) en las ranuras primarias del dueño y un evento escrito a mano.
- Las tareas nuevas del pool de tareas se siguen por hueco (`m_LastFreeSlot` restaurado y huecos libres con 0xCD): se compara el tipo y los 0x80 bytes. Las rutas nuevas del pool de `CPointRoute` se comparan también (cabecera, estados y almacén restaurados en cada ejecución).
- Se simulan y trazan `CPedGroups::GetPedsGroup`, `CPedGroupMembership::GetLeader`, `ComputeEntityHitSide`, `ComputeRouteRoundEntityBoundingBox` (rellena toda la ruta), `IKChainManager_c::LookAt`, `GetMoveStateFromGoToTask`, `IncrementAngerAtPlayer` y el generador aleatorio (rand 0x821B1E y `CGeneral::GetRandomNumber`).
- Se guardan y restauran el reloj, el pool de rutas y `m_LastFreeSlot` del pool de tareas.

Repetir: `python reversing/EvHF/build_probe.py` y `python reversing/run.py --case evhf`. Evidencia: [validation-summary.json](validation-summary.json).
