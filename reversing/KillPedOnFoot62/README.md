<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco KillPedOnFoot62: CTaskComplexKillPedOnFoot (ControlSubTask 0x626260, CreateFirstSubTask 0x62B490, CreateNextSubTask 0x62B150, CreateSubTask 0x625E70, MakeAbortable 0x625E40)

**Estado: APROBADO en el juego** (19-20/10/2026, worktree `D:\Original\we-3`, HEAD 0ac292a76): 5120 pares, cero diferencias, ABI, rutas, colaboradores y estado restaurados en ambos lados. Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-killpedonfoot62/20261020-003910-234884/` (`python -B reversing/KillPedOnFoot62/run_local.py --analyze <carpeta>/comparisons.jsonl`). Prueba de sensibilidad del banco: con cinco mutaciones deliberadas de la fuente (una por funcion: no limpiar el bit de `MakeAbortable`, `<=` en lugar de `<` en la distancia de 5.0 de CreateFirst, `!` invertido del `m_bEverybodyBackOff` de CreateNext, `Start(2001)` en Control, `Pause(2001)` en CreateSubTask) el banco dio FALLO en las cinco (58/2048, 3/768, 48/768, 94/1024 y 262/512 pares distintos; carpeta `20261020-003226-237148`); la fuente se restauro byte a byte (md5 `f2ba12e1...`) y la corrida limpia aprobo. Plantillas: `reversing/PartnerNext60` y `reversing/PartnerControl60` (CTaskComplexPartner), contrato `reversing/_handoff/BRIEF.md`, lecciones de los lotes 28-61.

## Comandos

```
bash reversing/_handoff/bench_here.sh KillPedOnFoot62
# o a mano, siempre en serie y con el candado global de GTA:
python -B reversing/KillPedOnFoot62/build_probe.py
python -B reversing/KillPedOnFoot62/run_local.py
python -B reversing/KillPedOnFoot62/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aun no existe o no esta completo
python -I reversing/KillPedOnFoot62/fixture_oracle.py                                            # autoprueba contra los bytes del EXE: «ok»
python -I reversing/KillPedOnFoot62/fixture_mirror.py                                            # compila Fixtures.hpp con cl: arenas == suma a mano, sin fixtures peligrosas
python -B reversing/KillPedOnFoot62/register_hook.py <carpetaEvidencia>                          # solo tras aprobar (SCOPE_NOTE ya relleno)
```
Archivo de arranque en el clon: `killpedonfoot62-runtime.start`; salida `killpedonfoot62-runtime.jsonl`. 5120 pares (Control 2048, CreateFirst 768, CreateNext 768, CreateSubTask 1024, MakeAbortable 512).

Filas de `docs/hooks.csv` (las anade el coordinador): `CTaskComplexKillPedOnFoot,ControlSubTask,0x00000000,1,0,1,V`, `...,CreateFirstSubTask,0x00000000,1,0,1,V`, `...,CreateNextSubTask,0x00000000,1,0,1,V`,
`...,MakeAbortable,0x00000000,1,0,1,V` y `CTaskComplexKillPedOnFoot,CreateSubTask,0x00625E70,1,0,1,S` (CreateSubTask NO es virtual: lo llaman por direccion las otras cuatro). La direccion real de los virtuales va en `evidence.original_address`.

## Fuente (`source.patch`)

`source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp`: `RH_ScopedInstall(CreateSubTask, 0x625E70)`, `RH_ScopedVMTInstall` de `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask` y `ControlSubTask`
(clase virtual 0x86D894), el ayudante `KillNewTask<T>` (reserva con `CTask::operator new` 0x61A5A0 y, si el pool esta agotado, no construye y devuelve nulo, como el EXE) y los cuerpos. `source.patch` contiene
SOLO los cambios de este archivo respecto a HEAD (`git apply --check` contra el blob de HEAD ok). La cabecera no cambia.

## Fase 2: revision contra el EXE

Re-desensamblado 0x625E40..0x626260 (MakeAbortable + CreateSubTask), 0x626260..0x626A80 (ControlSubTask), 0x62B150..0x62B490 (CreateNextSubTask) y 0x62B490..0x62B790 (CreateFirstSubTask) y contrastado rama
a rama con el borrador del lote 27 (`git diff HEAD` del arbol principal). **Discrepancias halladas y corregidas:**
1. El borrador usaba `new T(...)`: con el pool agotado `CTask::operator new` devuelve nulo y el EXE (`test eax,eax; je`) no construye y devuelve 0; `new` simple no comprueba el nulo (la rama depende de
   `/Zc:throwingNew`). Ahora `KillNewTask<T>` (mismo idioma que `PartnerNewTask`). La unica excepcion es CTaskComplexKillPedOnFootArmed, cuya reserva nula el EXE no comprueba (escribe `nulo+0x35`): el C++ la protege
   y el banco no la prueba (no puede: el EXE se cae).
Sin mas discrepancias en el resto: ramas, constantes (0.0 en 0x858B50, 0.1f en 0x858B1C, 5.0f en 0x858C80), orden de las llamadas, banderas del byte +0xC (bits 0-6), tiempos sin signo (`jbe`), `ja` sin signo del
word `CWanted+0x1C`, comparacion del tiempo con `ja`/`jle` con signo en `m_time`, pila x87 (`sqrt` con `fcomp` y `test ah,41h/05h`, NaN: en vehiculo `> 0.1` no se toma, `<= 0.1` no continua; distancia `< 5`).

## Que se compara

Original = EXE con el hook RH de la funcion apagado; candidato = DLL con SOLO ese hook encendido (los otros cuatro de la clase y `0x407180` quedan APAGADOS en ambos lados: sus llamadas entran en el codigo del EXE).
**Sustituidos** por el mismo falso en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto con el hook encendido ANTES de parchear; lecciones 2 y 3 del lote 61): FindPlayerWanted 0x56E230,
FindPlayerPed 0x56E210, CCullZones::NoPolice 0x72DD50, CWeapon::IsTypeMelee 0x73B1C0, CPed::Say 0x5EFFE0, GiveWeapon 0x5E6080, SetCurrentWeapon (tipo 0x5E6280 y ranura 0x5E61F0), SetMoveState 0x5DEC00,
DropEntityThatThisPedIsHolding 0x5E0360, CVehicle::CanPedOpenLocks 0x6D1E20, IsDriver 0x6D1C40, IsPassenger 0x6D1BD0, CCarEnterExit::ComputeTargetDoorToExit 0x64F110, IsRoomForPedToLeaveCar 0x6504C0,
IsVehicleHealthy 0x64EEC0, CPedGroups::GetPedsGroup 0x5F7E80, CPedGroupMembership::GetLeader 0x5F69A0, CTask::operator new 0x61A5A0, CTaskSimpleStandStill::ProcessPed 0x62F370 (hook virtual: cuerpo
`GetHookOurAddress()`), CEventGroup::Add 0x4AB420 y 0x65E9A0 (sin reconstruir: solo parche en el EXE, la DLL lo llama por direccion). **Aleatorio:** `CGeneral::GetRandomIntInRange` (0x407180) se APAGA para el EXE original,
que pide el numero a rand del EXE (0x821B1E, falso solo en el EXE); la DLL lo calcula en linea y pide `CGeneral::GetRandomNumber` (falso solo en la DLL): los dos devuelven el mismo numero (con el hook encendido
el EXE llamaria al `rand` de la DLL y el estado del generador no seria comparable). **Reales** en ambos lados (deben ser saltos a la DLL si tienen hook en HEAD, y codigo del EXE si no): `CPed::IsPlayer` 0x5DF8F0,
`CTaskTimer::IsOutOfTime` 0x420E30, `IsPedTypeGang` 0x5FE9C0 (sin hook; en linea en la DLL), los constructores de las tareas creadas (StandStill 0x62F310, LeaveGroup 0x463540, Pause 0x48E750, Melee 0x621000, Armed 0x621190,
CarDriveTimed 0x5FF940, DestroyCar 0x621C00, SignalAtPed 0x660A30, CEventAreaCodes 0x4B2190; sin hook en HEAD y por tanto codigo del EXE en el original: DragPedFromCar 0x640430, LeaveCar 0x63B8C0 y el destructor del evento 0x4B2210).
Las tareas se construyen en una region de la arena que devuelve el falso de `operator new`; se compara byte a byte y, ademas, el tipo (llamada virtual a `GetTaskType`, ranura 4, porque la vtable de un objeto que
construye el EXE es la del EXE y la de uno que construye la DLL es la de la DLL). `this` lleva la vtable REAL 0x86D894 (ControlSubTask llama a CreateFirstSubTask por la ranura 9); la subtarea lleva una vtable falsa
(GetTaskType ranura 4 y MakeAbortable ranura 6, con respuestas por mascara de bits por llamada). Los falsos devuelven las respuestas booleanas con BASURA en los bits 8..31 de EAX (los llamadores solo leen AL).

Observado: retorno (etiqueta de region+desplazamiento; en MakeAbortable solo AL), eventos ordenados con argumentos, TODOS los bytes que cambian en el mundo (una sola arena con guardas de 0x20 bytes entre regiones: this,
ped, objetivo, matrices, vehiculos, inteligencia, wanted, datos de jugador, entidades de contacto, grupo, subtarea, reserva...), tipo del objeto reservado, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado),
ruta, `collaborators_dll`, estado y tiempo restaurados (`globals_restored` se comprueba tras quitar los parches).

## Fixtures: 5120 = Control 2048 + CreateFirst 768 + CreateNext 768 + CreateSubTask 1024 + MakeAbortable 512

16 familias por ruta (ver `Fixtures.hpp`): Control: objetivo nulo, subtarea de pausa con banderas de espera, tiempo agotado (con acarreo), objetivo nuevo, objetivo muerto (salud <= 0 con ceros con signo y NaN),
objetivo vivo jugador (a salvo / policia con m_bDontPursuit), subtareas melee / armada (municion, busqueda de ranura) / arrastrar / destruir con el escalon completo de vehiculo (subtipo avion/helicoptero, moto, barco,
velocidad alrededor de 0.1f +-1 ulp, NaN, IsDriver/IsPassenger, CanPedOpenLocks, sitio para sacar, temporizador), tipos sin rama y mezcla con eventos de codigo de area; CreateFirst: escalon de vehiculo y pistola de
pandillero con la distancia al borde de 5.0f; CreateNext: pausa / arrastrar / salir del coche / resto de subtareas; CreateSubTask: un `taskId` por familia (+ al azar); MakeAbortable: con y sin subtarea, prioridades y evento.
Reserva nula (pool agotado) solo en CreateSubTask (con CTaskComplexKillPedOnFootArmed el EXE se cae). El objetivo en vehiculo con vehiculo nulo se evita cuando el EXE lo desreferencia sin comprobar.

## Suposiciones y riesgos

- **Arenas** (suma a mano; las comprueba `fixture_mirror.py` contra `fixture_oracle.region_offsets()`): una guarda inicial de 0x20 y, por region, el tamano redondeado a 16 mas una guarda de 0x20. Regiones:
  this 0x38 (->0x40), ped y objetivo 0x7A0 (CPed 0x79C + el byte +0x79C del CCopPed), matrices 0x48 (->0x50), vehiculos 0x5A0, inteligencia 0x80, wanted 0x30, datos de jugador 0x40, wanted del objetivo 0x30, contactos 0x40,
  grupo 0x40, subtarea 0x40, reserva 0x70 (la tarea mayor, CTaskSimpleCarDriveTimed, mide 0x70), otra 0x40, etiquetas 0x40; total 8400 bytes. Los tamanos de las tareas creadas se comprueban con `static_assert`.
- **Opciones de compilacion.** El archivo se compila con las opciones por defecto del proyecto (sin `/fp:fast` ni x87 por archivo): las comparaciones contra 0.1f y 5.0f van por `sqrtf` en `float`; si algun dia se compila con `/fp:fast`, `sqrt(s) > 0.1f` podria transformarse en `s > 0.01f` (ver PROCESO.md) y habria que repetir el banco.
- **Coma flotante.** Los EXE usan x87 con la palabra de control a 24 bits; el C++ del archivo (Debug, SSE) redondea cada operacion a `float`. Coinciden mientras no haya desbordamiento/denormales intermedios que
  cambien una decision: las comparaciones son contra 0.0, 0.1f y 5.0f (monotonas), por lo que un valor que en x87 sigue finito y en float desborda a inf (o al reves en denormales) decide igual; los casos limite
  (+-1 ulp de 0.1f y 5.0f, NaN, inf, cero con signo, denormales) estan en las fixtures. El banco prueba lo observable; cualquier diferencia real aparece como par distinto.
- **GetRandomIntInRange.** El EXE original (0x407180, hook apagado) usa `fild/fmul 2^-15/fimul/_ftol2` con la FCW a 24 bits; la DLL `float(double(r) * 2^-15 * range)`. El producto exacto se redondea una sola vez a 24
  bits en ambos; las fixtures incluyen 0, 1, 0x7FFF, 0x8000 y 0xFFFF.
- **Entidades de arena.** El objetivo, los vehiculos y las tareas registran referencias (`CEntity::RegisterReference`): se configuran como edificio sin IPL (ni temporal ni procedural) y con `m_pReferences` vacia, de
  modo que no tocan `CReferences` y el destructor de `CEventAreaCodes` no recorre listas ajenas.
- **Constructores sin hook** (DragPedFromCar 0x640430, LeaveCar 0x63B8C0): el EXE original ejecuta el constructor del EXE y el candidato el de la DLL; si el de la DLL difiere se vera como par distinto (no es un fallo
  de las cinco funciones, pero impide aprobar el banco hasta corregirlo).
- Si la DLL inlinease un colaborador sustituido (LTCG), el falso del cuerpo de la DLL no se invocaria y faltaria el evento: el banco falla con claridad (no da falso positivo).
- **EAX/ECX/EDX de salida.** Los unicos llamadores del EXE de CreateSubTask (0x626404, 0x626A4B, 0x62B16B..0x62B46B, 0x62B76A, 0x62B7C2...) usan EAX como retorno y sobrescriben ECX/EDX; las otras cuatro las llama el
  despachador generico de tareas por la vtable (EAX es el retorno). **Ningun llamador lee ECX/EDX de salida**: solo se registran. En MakeAbortable solo AL es significativo (se compara AL, con basura en los bits altos).

## Modelo independiente y cobertura

`fixture_model.py` modela las cinco funciones desde el DESENSAMBLADO (no desde la DLL): nombres de eventos en orden, tamano pedido a `operator new`, contextos de `Say`, clase del retorno y ramas alcanzadas; la coma flotante
se calcula con redondeo a 24 bits de mantisa. `run_local.py` exige que AMBOS lados (EXE y DLL) coincidan con el modelo en los 5120 casos (cosa que ocurre: el modelo reproduce lo que hace el EXE) y mide la cobertura con
las 296 claves (eventos, tamanos de reserva, tipos de tarea creada, `branch:*` por rama del modelo) con minimos del 35 % de lo medido (minimo 1). Todas las ramas modeladas se alcanzan al menos una vez; las menos frecuentes
(unas pocas decenas): armada sin municion y sin arma, melee / armada contra avion o helicoptero, destruir con / sin sitio, temporizador de arrastre y de destruir, bici y barco en CreateFirst, `Say` del policia al morir el objetivo.

## Defectos del propio banco hallados y corregidos durante la preparacion

1. Primera corrida: AV en 0x6260D5 (`mov [eax+0x35],dl` de CTaskComplexKillPedOnFootArmed con reserva nula): la fixture de pool agotado (`allocNull`) llegaba tambien a CreateNext/CreateFirst/Control, que pueden crear la tarea armada
   (el EXE no comprueba el nulo): ahora la reserva nula solo se prueba en CreateSubTask y nunca con `taskId` 0x3EA.
2. Cobertura: el escalon de vehiculo se alcanzaba pocas veces; se anadio el escenario `Favor` (coche normal, quieto, comprobaciones a favor, temporizador vencido con bordes exactos 3000 / 3001 / 2999 ms) y subtipos 3/4 (aire).
Sin cambios de produccion por estos dos puntos.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_model.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
La carpeta `artifacts/` (manifiesto y sonda compilada) es salida local, no se versiona; `register_hook.py` lee `artifacts/build-manifest.json`.
