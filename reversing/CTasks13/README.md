# Seguir ruta, líder de banda, persecución policial y secuencia (lista de 13)

Validadas el 02/10/2026 con **16.384 pares original/candidato dentro de GTA**, todos exactos: 51 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexFollowPointRoute` | Constructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask`, `GetSubTaskType`, `CreateTask`, `GetLastWaypoint`, `GetNextWaypoint`, `CalcGoToTaskType`, `CalcBlendRatio` |
| `CTaskComplexGangLeader` | Constructor, Destructor, `Clone`, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask`, `ScanForStuff`, `GetRandomGangAmbientAnim`, `ShouldLoadGangAnims`, `DoGangAbuseSpeech`, `TryToPassObject` |
| `CTaskComplexPolicePursuit` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask`, `CreateSubTask`, `SetWeapon`, `ClearPursuit`, `SetPursuit`, `PersistPursuit` |
| `CTaskComplexSequence` | Constructor, `Clone`, `MakeAbortable`, `CreateNextSubTask` (virtual e interna), `CreateFirstSubTask`, `ControlSubTask`, `AddTask` (dos versiones), `Flush`, `Contains`, `SetCanBeEmptied`, `DeRegister` |

**Diferencias reales corregidas:**
- `CTaskComplexFollowPointRoute`:
  - el constructor solo escribe los bits 0, 2 y 3 de las banderas y los valores de mezcla solo con mezcla (sin inicializadores); la ruta nueva sale de `CPointRoute::operator new` y copia solo los puntos usados;
  - `GetNextWaypoint` en modo bucle devolvía el vector nulo; `GetLastWaypoint` compara las vueltas con signo y no aborta con modos fuera de rango;
  - `CalcGoToTaskType`: borra "va a frenar" (no "usar mezcla"), sigue con el punto 0, usa `CVector::Normalise` con z = 0, las constantes 0.707 y 0x3F15F880 del EXE (multiplicación, no división) y mira el bit de "frenando";
  - `CalcBlendRatio`: distancias `(z² + y²) + x²`, `fcos` x87 y π del EXE; `CreateFirstSubTask`: longitud del tramo con `fsqrt`, sumas en el orden del EXE y sin `assert`;
  - `CreateNextSubTask`: tras `TASK_NONE` o `STAND_STILL` termina siempre y pide el tipo de la subtarea en cada comprobación; `ControlSubTask`: una ruta nueva vuelve a `CreateFirstSubTask`, acelerar mide desde el punto anterior y aplica la proporción, y pasar a frenar activa "frenando";
  - `CreateTask` copia los 4 bits de objetivo rodeado de la subtarea `GO_TO_POINT` (antes los ponía a 0) y devuelve `nullptr` en tipos desconocidos; `MakeAbortable` pide el tipo de la subtarea aunque no haya vehículo.
- `CTaskComplexGangLeader`:
  - `TryToPassObject` es cdecl estática (el miembro thiscall descuadraba la pila del EXE) y usa `GetClosestGroupPed` (< 4) y si el que pasa no es jugador;
  - `ControlSubTask`: sin objeto en la mano llamaba a `SetTaskSecondary(m_pSubTask)` casi siempre; ahora 51..55 charla, 100 señal y el resto nada; borra `bDontAcceptIKLookAts` al empezar, mirada con `(int)(rand·100/32768) > 95`, tiempo antes que el miembro, animación de la mano 1 de cada 100 y distancia del camino estricta;
  - `ScanForStuff`: explora también con el temporizador sin arrancar, mira el subtipo del vehículo, el comando de unirse va al peatón explorado y llama a `Process`, distancias al cuadrado entre 16 y 100, `GetRandomIntInRange` del EXE y eventos locales (antes se copiaban y se borraban dos veces);
  - `CreateNextSubTask`: miembro, probabilidad e intervalo `[15000, 30000)` con las fórmulas del EXE; `ShouldLoadGangAnims` compara con 0.04 y mira el vehículo antes del streaming; `GetRandomGangAmbientAnim` usa las animaciones de fumar con el porro;
  - temporizadores sin `{}` (relleno sin escribir).
- `CTaskComplexPolicePursuit`:
  - `CreateFirstSubTask` guarda el perseguidor (sin referencia; el destructor ya no la limpia) y `SetPursuit` solo guarda el perseguido si `SetPursuitCop` acepta;
  - `ControlSubTask`: el coche a distancia al cuadrado de 25 o más da `STAND_STILL` (antes `TASK_NONE`); el constructor por defecto es del usuario (el `Clone` ya no pone a cero el byte de banderas); `CreateSubTask` sin perseguido o con tipos desconocidos devuelve `nullptr` y el radio sale del modelo;
  - `SetWeapon` usa la habilidad estándar y niveles con signo; `PersistPursuit` mira la bandera antes de `NoPolice`; NaN en `SetPursuit`.
- `CTaskComplexSequence`:
  - `Contains` se salta los huecos vacíos; `CreateNextSubTask` interna admite modos de N repeticiones; `AddTask` devuelve si se añadió y no comprueba la tarea;
  - `MakeAbortable` propio (compartía `CTaskComplex::MakeAbortable` con otro hook); `Flush` borra antes de poner a nulo.

**Método:**
- 6 peatones con inteligencia propia (el 4 y el 5 hacen de jugadores 0 y 1: `CWorld::Players[0/1].m_pPed` se cambian y se restauran en cada ejecución), 2 vehículos con vtable de trampas y un modelo con colisión real, 10 tareas falsas con `GetTaskType`, `Clone`, `MakeAbortable` y destructor trazados, evento falso, `CPedGroup` y `CWanted` falsos, clump, frame, asociaciones y FX falsos.
- La ruta de puntos vive dentro de un buffer mayor: las lecturas fuera de rango del EXE caen en bytes conocidos.
- Simulados con traza: RNG, `FindPlayerWanted`, `CWanted` (perseguidores), `NoPolice`, `SetCurrentWeapon`, `Say`, `IsEntityBlockingTarget`, streaming, referencias de bloques de animación, `GetDistSqOfClosestPathNodeToPed`, FX, `IKChainManager`, `GetEntityThatThisPedIsHolding`, `IsPlayingHandSignal`, asociaciones, `SetTaskSecondary`/`ChangeTaskInSlot`, `GetClosestGroupPed`, `AddEvent`, `CEventGroup::Add` (eventos serializados por campos), `GetPedsGroup`, `AddFollower` y `CPedGroup::Process`.
- Pools de tareas, rutas de puntos y eventos restaurados en cada ejecución (slots libres con 0xCD); lo nuevo se serializa y se libera sin destructor. Constructores sobre buffers con 0xCD comparados enteros; cambios de todos los buffers de fixtures comparados palabra a palabra.
- Fuera del banco: varios vehículos válidos en `ScanForStuff` (con `FIX_BUGS` el DLL para tras el primer evento; el escáner lleva un solo vehículo) y la rama de `ControlSubTask` de `GangLeader` que en el EXE devuelve `StandStill(500)` tras `SetDefaultTaskAllocatorType` (el DLL devuelve `nullptr` a propósito, ver el comentario del código).

Repetir: `python reversing/CTasks13/build_probe.py` y `python reversing/run.py --case ctasks13`. Evidencia: [validation-summary.json](validation-summary.json).
