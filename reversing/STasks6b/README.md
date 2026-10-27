<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples B (lista de 6)

Validadas el 02/10/2026 con **18.688 pares original/candidato dentro de GTA**, todos exactos: 48 funciones de 8 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleGunControl` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleHurtPedWithCar` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleIKPointArm` | Constructor, Destructor, `Clone`, `CreateIKChain`, `GetTaskType`, `UpdatePointArmInfo` |
| `CTaskSimpleLand` | Constructor, `FinishAnimCB`, `LeftFootLanded`, `MakeAbortable`, `ProcessPed`, `RightFootLanded` |
| `CTaskSimplePlayerOnFire`, `CTaskSimpleSay` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleSetCharDecisionMaker`, `CTaskSimpleSetCharIgnoreWeaponRangeFlag` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |

**Diferencias reales corregidas:**
- `CTaskSimpleGunControl`:
  - el constructor escribe `m_isFirstTime = 1` (0x38) y `m_isLOSBlocked = 0` (0x39), que estaban al revés, y no escribe 0x3A;
  - `MakeAbortable` llama dos veces a `GetEventType`, escribe en la tarea del hueco secundario de ataque y pide dos veces la tarea de agacharse.
- `CTaskSimpleGunControl::ProcessPed` se reescribió siguiendo el EXE:
  - llama a `SetMoveState(PEDMOVE_STILL)` y vuelve a leer el hueco secundario en cada acceso;
  - la animación de banda se activa con los tipos 7..15 o con el tipo 6 quieto y arma que apunta con el brazo;
  - actualiza `m_isLOSBlocked` y obtiene el arma con `GetWeaponInfo(tipo, GetWeaponSkill())`;
  - `END_NOW` y los comandos fuera de rango dejan `NONE` en vez de abortar;
  - el máximo de la ráfaga vuelve a pedir el número aleatorio, como el EXE;
  - `m_isFinished` solo se activa sin tarea de disparo, con la tarea terminada en `END_LEISURE` o tras un disparo que no es ráfaga;
  - el periodo de ataque usa `rand()` en línea, el periodo como entero sin signo y truncado `_ftol2`;
  - la distancia se suma con Z primero, el rumbo usa `fpatan` y la condición para moverse es la del EXE.
- `CTaskSimpleHurtPedWithCar::ProcessPed`:
  - calcula el lado del golpe antes que nada;
  - no marca la respuesta como calculada si el evento no afecta al peatón;
  - multiplica la masa por la constante 1/1400 del EXE en vez de dividir.
- `CTaskSimpleIKPointArm`:
  - el desplazamiento del efector es (0,05, 0, 0), no (0, 0,05, 0);
  - el constructor no escribe 0x14, 0x16, 0x48, 0x4C ni 0x50;
  - el hueso de desplazamiento se guarda y se pasa como dword en el constructor, `Clone`, `UpdatePointArmInfo` y `CreateIKChain`;
  - cualquier brazo distinto de 0 es el izquierdo y el hueco de IK es 1 + (brazo != 0).
- `CTaskSimpleLand::ProcessPed`: el pad del jugador se pide antes de mirar si esprinta.
- `CTaskSimplePlayerOnFire`:
  - el temporizador ya no se inicializa con `{}`;
  - grita la primera vez que el daño es mortal y no después;
  - copia la respuesta de daño al evento y marca `m_bAddedDamageEvent`;
  - lee `CWorld::Players[0]` en línea.
- `CTaskSimpleSay`: el temporizador ya no se inicializa con `{}`; el constructor y `Clone` guardan el contexto como dword completo.
- `CTaskSimpleUseGun` (constructor, que usa `GunControl`): la ráfaga va en 0x34 (`m_BurstLength`) con 0x36 a cero, y el byte 0x0D de los bits de disparo se escribe entero.

**Método:**
- Peatones, clump, inteligencia, asociaciones de animación, vehículos, tarea secundaria y eventos falsos.
- `CTaskSimpleUseGun`, `CTaskSimpleDuck`, `CWeaponInfo`, `IKChain_c`, `CFire` y `CPad` falsos en búferes propios.
- Los constructores corren sobre búferes 0xCD.
- En cada ejecución se restaura el pool de tareas y `CWorld::Players[0]`.
- La `CTaskSimpleUseGun` que crea `GunControl` se compara entera (sin máscaras).
- Simulados y trazados:
  - animación, `SetMoveState`, `Say` y `ApplyMoveForce`;
  - las consultas de tareas de la inteligencia y del gestor de tareas;
  - las órdenes de disparo y la información de armas;
  - el daño (`AffectsPed`, `ComputeDamageResponse`, `ComputeFireDamage`) y `CEventGroup::Add`;
  - el gestor de IK y las cadenas de IK.

Repetir: `python reversing/STasks6b/build_probe.py` y `python reversing/run.py --case stasks6b`. Evidencia: [validation-summary.json](validation-summary.json).
