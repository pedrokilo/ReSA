<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples B (lista de 8)

Validadas el 02/10/2026 con **12.608 pares original/candidato dentro de GTA**, todos exactos: 32 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleJump` | Constructor, `CheckIfJumpBlocked`, `Clone`, `JumpAnimFinishCB`, `Launch`, `MakeAbortable`, `ProcessPed`, `StartLaunchAnim` |
| `CTaskSimpleShakeFist` | Constructor, Destructor, `Clone`, `FinishAnimShakeFistCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleSitDown` | Constructor, Destructor, `Clone`, `FinishAnimSitDownCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleStandUp` | Constructor, Destructor, `Clone`, `FinishAnimStandUpCB`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `StartAnim` |

**Diferencias reales corregidas:**
- `CTaskSimpleJump::Clone`: copia el byte crudo de `m_bCanClimb` (el parámetro `bool` del constructor lo normalizaba).
- `CTaskSimpleJump::ProcessPed`: el `CEventSoundQuiet` vive en la pila y se pasa su dirección a `CEventGroup::Add` (la plantilla `Add(T)` lo copiaba).
- `CTaskSimpleJump::CheckIfJumpBlocked`:
  - la z de la esfera se suma como `((fwd.z·0,25 + pos.z) + centro.z) + 0,25`;
  - la posición elevada usa el orden de sumas de 0x59C890 y la matriz se lee sin comprobar;
  - el número de esferas se lee con signo (`movsx`).
- `CTaskSimpleJump::Launch`:
  - interpolación `(b − a)·t + a` (el `lerp` de la DLL hace `b·t + a·(1 − t)`);
  - `GetFatAndMuscleModifier` se pide dos veces y el megasalto mira el truco antes que `IsPlayer`;
  - la velocidad horizontal se compara al cuadrado (`x² + y² < v²`; la DLL comparaba con `v`);
  - `fsin`/`fcos` con precisión interna completa;
  - huellas de sangre: tamaño `forward·0,26` y lado `right·0,14` (la DLL usaba `forward·0,2·0,26` y `forward·0,14`), z `(v.z − 0,1) + fwd.z·0,2`;
  - el tiempo de muerte se compara sin signo (`jbe`).
- `CTaskSimpleJump::StartLaunchAnim`: en pendiente pronunciada solo cuenta el producto escalar (`(z + y) + x`) y nunca se consulta `IsPlayerAllowedToGoInThisDirection`; una fase NaN elige `JUMP_LAUNCH_R`.
- `CTaskSimpleShakeFist`: constructor escrito por el usuario (el `= default` con `{}` ponía a cero el relleno 0x09–0x0B en el clon y en los `new CTaskSimpleShakeFist{}`); `MakeAbortable` trata cualquier otra prioridad como `LEISURE`.
- `CTaskSimpleSitDown`: `MakeAbortable` inmediato corta la mezcla antes de cambiar el callback y trata cualquier otra prioridad como `LEISURE`; fuera los `assert` que el EXE no tiene.
- `CTaskSimpleStandUp::MakeAbortable`: cualquier otra prioridad se trata como `LEISURE` (la DLL tenía `NOTSA_UNREACHABLE`).

**Método:**
- Peatón falso con matriz, nodos de los pies, inteligencia y datos de jugador propios; `TestCollision` (hueco 13 de la vtable) traza la posición y los radios de las esferas en el momento de la llamada.
- Modelo falso (`CBaseModelInfo` → `CColModel` → `CCollisionData` con esferas propias) colocado solo durante cada llamada en un hueco vacío de `ms_modelInfoPtrs`: nunca se tocan las esferas del modelo real.
- Asociaciones con mezclas en 0,3 y sus vecinos exactos, tiempos totales nulos (fase NaN o infinita), número de esferas negativo y tiempos de muerte 40/41 o con el bit alto.
- Se simulan y trazan animación, `TestSphereAgainstWorld`, `TestForClimb`, `IsSteepSlope`, `IsPlayerAllowedToGoInThisDirection`, `GetCurrentEventType`, `ApplyMoveForce`, las sombras permanentes, RpHAnim/`RwV3dTransformPoints` y el grupo global de eventos.
- Las direcciones de pila en las trazas se marcan como `stack`; los retornos `bool` se comparan como byte distinto de cero (el EXE devuelve el byte crudo del campo, p. ej. 0x80, y la DLL lo normaliza).
- Se guardan y restauran tras cada ejecución el reloj, `CGame::currArea`, el truco de megasalto, el hueco del modelo, el pool de tareas y la lista libre de referencias.

Repetir: `python reversing/STasks8b/build_probe.py` y `python reversing/run.py --case stasks8b`. Evidencia: [validation-summary.json](validation-summary.json).
