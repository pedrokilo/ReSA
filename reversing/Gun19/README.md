<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Usar arma (lista de 19)

Validadas el 03/10/2026 con **9.920 pares original/candidato dentro de GTA**, todos exactos: 18 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleUseGun` | `AbortIK`, `AimGun`, `ClearAnim`, `Clone`, `ControlGun`, `ControlGunMove`, `FinishGunAnimCB`, `FireGun`, `GetTaskType`, `MakeAbortable`, `PlayerPassiveControlGun`, `RemoveStanceAnims`, `RequirePistolWhip`, `Reset`, `SetMoveAnim`, `SetPedPosition`, `StartAnim`, `StartCountDown` |

**Diferencias reales corregidas:**
- `AimGun`:
  - velocidad y tiempo de mezcla de la IK salen de variables del EXE (0x8D2E70/0x8D2E74) y apuntar inmediato se consume (`m_IsAimImmediate = false`);
  - con objetivo peatón el hueso de `LookAt`/`PointArm` es el del jugador que apunta (columna si no es jugador) y -1 si el objetivo no es peatón;
  - con objetivo peatón, el punto sale del hueso y desplazamiento objetivo del **que apunta** (si tiene datos de jugador) transformados en el objetivo, o de la columna del objetivo (no de la posición de la entidad);
  - sin objetivo: con `m_TargetPos` no marca la IK de mirada; con el punto (0, 2, 0) mira solo si no miraba, con el propio peatón como entidad y el hombro derecho (22) para los dos brazos;
  - los que no son jugador apuntan con `m_fLookDirection`;
  - con pistolas dobles y brazo izquierdo bloqueado aborta el brazo izquierdo (no el derecho).
- `FireGun`: `IsFacingTarget` se consulta con el brazo en uso (estaba invertido) y sin huesos actualizados el arma se da por orientada.
- `SetMoveAnim`:
  - sin movimiento: `SetMoveState(PEDMOVE_STILL)` al apuntar con el brazo, bandas 7..15 (no GANG10), reposo con el grupo del peatón, se paran adelante/izquierda/atrás/derecha (faltaba adelante y atrás iba dos veces) y después se reinicia su tiempo;
  - izquierda con `x < -0,75` (no `<=`), atrás solo con `y > 0` (con NaN, adelante) y el blend del reposo solo se satura cuando es menor que 0 (NaN y -0 se conservan).
- `RemoveStanceAnims`: compara la duración con el tiempo actual (sin dividir), lee cada eje del pad dos veces y al caminar devuelve sin mezclar el reposo.
- `RequirePistolWhip`: posición de la cabeza antes de `IsAlive`, productos escalares en orden z, y, x, constante exacta 0,5773 y el escáner recorrido como el EXE.
- `ControlGun` aborta la IK al cambiar de objetivo.
- `ControlGunMove` compara la diferencia con el máximo (Y primero; con NaN copia el destino).
- `StartAnim`: la recarga y `FinishGunAnimCB` comparan la ráfaga con signo; el golpe con la pistola consulta `IsTaskInUseByOtherTasks` y lee el grupo justo después de la tabla de combos sin el assert de `std::array`.
- `MakeAbortable` trata como ociosa cualquier prioridad que no sea urgente o inmediata; `SetPedPosition` pone a cero el byte entero de los bits de disparo; `PlayerPassiveControlGun` compara con signo.

**Método:**
- Seis peatones falsos (desplazamientos crudos: inteligencia 0x47C, datos de jugador 0x480, armas 0x5A0, hueco activo 0x718), una entidad que no es peatón, la tarea en un buffer propio con la vtable del EXE y guarda a 0xCD, 11 asociaciones y 4 jerarquías falsas y un `CWeaponInfo` falso (el real de la tabla en `SetPedPosition`).
- Se simulan y trazan la IK (`IKChainManager_c`), huesos, `IsAlive`, `Say`, `SetMoveState`, `DoGunFlash`, `CWeapon::Fire`, `CPedIK`, la cámara, el pad, el pato, `GetTaskUseGun` y las funciones de animación; `GetSimplestActiveTask` es real sobre una tarea simple falsa con tipo trazado.
- Los callbacks de las asociaciones se comparan por identidad (0x4D1390/0x61F3A0 frente a los de la DLL).
- Por ejecución se restauran `m_LastFreeSlot` y los estados del pool de tareas, los huecos libres a 0xCD, el paso de tiempo y el modo de la cámara.
- Fuera de los fixtures:
  - `SetMoveAnim` sin movimiento con `m_MoveCmd` distinto de cero (FIX_BUGS: la DLL no lo pone a cero);
  - tareas o asociaciones nulas que el EXE usa sin comprobar.

Repetir: `python reversing/Gun19/build_probe.py` y `python reversing/run.py --case gun19`. Evidencia: [validation-summary.json](validation-summary.json).
