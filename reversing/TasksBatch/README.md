<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas: StareAtPed, StealCar, StuckInAir, TrackEntity, TreatAccident

Validadas el 02/10/2026 con **20.480 pares original/candidato dentro de GTA**, todos exactos: las 39 funciones de las cinco clases. 39/39 con `validated=1`.

**Diferencias reales corregidas por el banco:**
- `CTaskComplexStareAtPed`:
  - el constructor solo guarda `m_pPed` si `IsEntityPointerValid`;
  - la espera es `GetRandomIntInRange(500, 1500)` y no [500, 999];
  - `ControlSubTask`: el temporizador agotado aborta; `LookAt` usa 9.999.999 ms (no `&gDefaultTaskTime`); la animación de charla sale con `rand(0,100) == 50` y un ID `GetRandomIntInRange(279, 287)`.
- `CTaskComplexStealCar`:
  - `CreateSubTask` no anula `m_originalriver` al soltar la referencia;
  - `CreateFirstSubTask` sin vehículo devuelve una pausa;
  - `CreateNextSubTask` pasa `ped = nullptr` (nunca reactiva la colisión) y devuelve nulo ante tipos desconocidos.
- `CTaskComplexStuckInAir`:
  - el jugador se reconoce por `m_pPlayerData`;
  - la puntería resta `TheCamera.m_fOrientation` y pasa por `LimitRadianAngle`;
  - `bIsStanding = true` en la espera y en `FLEE_POINT`;
  - la moneda es `rand() & 1`;
  - `MakeAbortable` se llama al final.
- `CTaskComplexTrackEntity`: `m_fMoveRatio = -1` en el constructor; `Clone` pasa el tiempo restante del temporizador; la espera es de 50 ms.
- `CTaskComplexTreatAccident::ComputeHeading`: `LimitRadianAngle(GetRadianAngleBetweenPoints(medio, pos))`, con la cabeza antes que la pelvis.

**Método:** peatones, vehículo, grupo y accidente en memoria propia (con matriz e inteligencia). Las subtareas son falsas, con vtable propia. IK, voz, bloques de animación, streaming, pad, posición de huesos, asociaciones de animación y RNG están controlados. Se compara toda la memoria de los fixtures y de las tareas nuevas, la traza y el retorno.

**Límites:**
- Se excluyen el relleno y los campos que el constructor original no inicializa: `m_bStopped` del temporizador y `float40`/`m_goToPos`/`m_distToTargetSq` de `TrackEntity`, que se calculan antes de usarse.
- Se excluyen el objetivo o la subtarea nulos donde el original los desreferencia sin comprobar.
- No se ejecuta la IA ni la animación reales.

Repetir: `python reversing/TasksBatch/build_probe.py` y `python reversing/run.py --case tasks`. Evidencia: [validation-summary.json](validation-summary.json).
