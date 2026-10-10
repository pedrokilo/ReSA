<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples (lista de 7)

Validadas el 02/10/2026 con **19.520 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleCarWaitForDoorNotToBeInUse` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition` |
| `CTaskSimpleDie` | Constructores 1, 2 y 3, `FinishAnimDieCB`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleKillPedWithCar` | Constructor, Destructor, `Clone`, `DamageCarBonnet`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleRunTimedAnim` | Constructores 1 y 2, Destructor, `Clone`, `GetTaskType`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleSitIdle` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleStealthKill` | Constructor, `Clone`, `FinishAnimStealthKillCB`, `GetTaskType`, `MakeAbortable`, `ManageAnim`, `ProcessPed` |

**Diferencias reales corregidas:**
- `CTaskSimpleCarWaitForDoorNotToBeInUse::SetPedPosition`: sin vehículo no se toca al peatón (la DLL llamaba a `SetPedPositionInCar` con un peatón en vehículo nulo).
- `CTaskSimpleDie`: `MakeAbortable` y `ProcessPed` ponen y limpian el bit 5 de +0x478 (`bIsDyingStuck`), no el bit 4.
- `CTaskSimpleKillPedWithCar::ProcessPed`:
  - la posición del peatón en el espacio del coche usa productos escalares (`InverseTransformVector`), no `TransformPoint`;
  - la marcha adelante exige producto > 0 y se lee la caja del modelo en línea;
  - sobre el capó: `fz < -0,2` y `fz > 0,1` estrictos, `max.z` (no `max.x`) en la primera rama y longitud según la marcha en la segunda;
  - el golpe frontal es dirección 5 (cabeza), no 4, con arma 50 (atropello): solo el capó levanta al peatón;
  - el lado de impacto se compara como dword completo, la pieza se corrige tras construir el evento y el evento de atropello vive hasta el final;
  - vibración del mando con `1/masa` y fuerza con `min(1, masa·(1/1400))·-100` por componente.
- `CTaskSimpleKillPedWithCar::DamageCarBonnet`: signo con `rand()·(1/32767) > 0,5` y solo se niega la parte de `right`; la velocidad del capó es `(velocidad del peatón ± right·0,1) + up·0,5` (no parte de la del capó).
- `CTaskSimpleRunTimedAnim::ProcessPed`: el temporizador se arranca sin exigir duración >= 0.
- `CTaskSimpleSitIdle`:
  - el constructor no escribe +0x09 ni el relleno del temporizador;
  - `StartAnim` arranca el temporizador sin exigir duración >= 0;
  - `MakeAbortable` anula `m_anim`, comprueba `IsLooking` antes de abortar y pone la duración a -1;
  - `ProcessPed`: tirada `(int)(r/32768·100) < 5`, tiempo `[3000, 8000)`, la tirada del 25 % después del producto escalar, orden de sumas del punto y `IsLooking` antes de `AbortLookAt`.
- `CTaskSimpleStealthKill`:
  - el clon copia los campos del original (leía los propios sin construir) y se añade el destructor del EXE (0x622680);
  - `MakeAbortable` corta la mezcla, cambia el callback de borrado y anula la animación (también en urgente, con evento nulo comprobado);
  - `ManageAnim` mezcla la animación antes del daño, el causante es el objetivo (hueco con signo), marca `m_bAnimAdded` (no la dirección) y suma `ms_fTimeStep·0,02·1000`;
  - `ProcessPed`: rumbos con `fpatan` (sin objetivo vivo, del objetivo al peatón) y módulo `(z² + x²) + y²`;
  - `FinishAnimStealthKillCB` solo activa `m_bIsAborting`, nunca lo limpia.

**Método:**
- Peatones, vehículos (con matriz y modelo con colisión real), capó, asociaciones, jerarquía y evento falsos; el coche se orienta con `fz` exactos en los umbrales y el peatón se coloca en coordenadas locales del coche.
- `CAnimManager::ms_aAnimAssocGroups` se cambia por grupos y bloques falsos solo durante la llamada.
- El generador aleatorio admite valores forzados (16383/16384, 1638/1639...) para los umbrales.
- Se guardan y restauran el reloj, `ms_fTimeStep`, los grupos de animación, el pool de tareas y la lista libre de referencias.
- Fuera de los fixtures: peatón sin matriz en `SitIdle` (el `GetForwardVector` NOTSA de `CPlaceable` usa `std::sin/cos`), hueco de arma negativo, masa nula y objetivo nulo en `ManageAnim` (el EXE lo lee sin comprobar).

Repetir: `python reversing/STasks7/build_probe.py` y `python reversing/run.py --case stasks7`. Evidencia: [validation-summary.json](validation-summary.json).
