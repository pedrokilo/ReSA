# Hooks activados en 39 clases (2026-10-01)

[Inicio](../../README.md) · [Prioridades](../../docs/ReversingPriorities.md#incorporadas-2026-10-01) · [Proceso y continuidad](../MEJORAS_DEL_PROCESO.md#continuidad-vigente)

Clases con `.h` en el código cuyos hooks no se instalaban ni figuraban en `docs/hooks.csv`. Se registraron 39 y se descartaron 10 con motivo. **Alcance: registro, catálogo, compilación y arranque. No hay comparaciones función a función contra el original.**

## Qué se hizo

**Nueve con `InjectHooks()` ya escrito y comentado en `InjectHooksMain.cpp`:** `CTaskComplexEnterCarAsPassengerWait`, `CTaskComplexScreamInCarThenLeave`, `CTaskComplexUsePairedAttractor`, `CTaskSimpleChat`, `CTaskSimpleDead`, `CTaskSimpleDoHandSignal`, `CTaskComplexHitPedWithCar`, `CTaskComplexLeaveCar` y `CTaskComplexPartnerDeal`. Se descomentó la llamada y se añadieron los `#include` que faltaban. `CTaskComplexLeaveCar` no instala ninguna función, así que no aporta filas al CSV.

**Treinta con `InjectHooks()` nuevo:**

- Tareas complejas: `BeInCouple`, `BeStill`, `Climb`, `DriveToPoint`, `CarDriveWander`, `EnterCarAsDriver`, `EnterCarAsPassenger`, `EvasiveCower`, `EvasiveStep`, `ObserveTrafficLights`, `OpenDriverDoor`, `OpenPassengerDoor`, `ProstituteSolicit` y las variantes de `Wander` `Criminal`, `Gang`, `Medic`, `Prostitute` y `Standard`.
- Tareas simples: `CarSetPedInAsDriver`, `CarSetPedInAsPassenger`, `CarSetPedOut`, `DieInCar`, `EvasiveDive`, `EvasiveStep`, `GangDriveBy`, `ScratchHead`, `SetStayInSamePlace` y `TogglePedThreatScanner`.
- Audio: `CAEAudioEntity` y `CAEPedWeaponAudioEntity`.

Los vtables no estaban en el código. Se localizaron en `gta_sa.exe`: se busca en `.rdata` la dirección de una virtual conocida, se resta su índice y se comprueban **todas** las entradas contra las direcciones comentadas y contra las implementaciones base. Las direcciones que solo salían del vtable o del desensamblado se añadieron como comentario `// 0x...`.

**Catálogo:** 216 entradas nuevas en `hooks.csv` (49 de las nueve primeras y 167 de las treinta). Quedan con `reversed=0, locked=1`:

- Las funciones cuyo cuerpo es solo `plugin::Call` hacia su propia dirección; activarlas causaría recursión infinita.
- Tres `Clone` inline que no copian todos los campos del original: `OpenPassengerDoor` (`m_MoveState`), `WanderCriminal` y `WanderStandard` (`m_bWanderSensibly`).

En total, 16 clases quedan parciales con 43 pendientes; se detallan en [Prioridades](../../docs/ReversingPriorities.md#incorporadas-2026-10-01).

## Correcciones de código

- `CTaskComplexCarDriveWander::Clone` creaba un `CTaskComplexCarDrive`, del tipo equivocado. El original (0x63DD00) construye la base y escribe el vtable de Wander; ahora devuelve `CTaskComplexCarDriveWander`.
- `TaskComplexClimb.h`: las direcciones de `Clone` y `GetTaskType` estaban desplazadas una función (ahora 0x46A650 y 0x46A6B0).
- `#pragma once` en `TaskComplexWanderGang.h`, donde solo ponía `#pragma` y rompía la compilación, y en `TaskSimpleTogglePedThreatScanner.h`, donde faltaba.
- `CAEPedWeaponAudioEntity` tiene ahora su propio `InjectHooks()` (`Initialise`, `Terminate`, `AddAudioEvent`, `Service`). Su llamada en `InjectHooksMain` ya no resuelve al `InjectHooks` heredado de `CAEWeaponAudioEntity`. El constructor 0x5DE990 no se duplica: sigue registrado y bloqueado bajo `CAEWeaponAudioEntity`.

## Auditoría

`contrib/audit_hook_catalog.py` incorpora una sexta comprobación crítica: **llamadas `X::InjectHooks()` en las que `X` no declara `InjectHooks` propio**. La llamada compila porque resuelve al de una base, que entonces se ejecuta dos veces (assert por nombres duplicados) o registra los hooks bajo el owner equivocado. Sobre el commit `143b5e7` detecta el caso histórico de `CAEPedWeaponAudioEntity`; sobre el árbol actual da 0. Hay pruebas en `contrib/tests/test_audit_hook_catalog.py`.

Resultado estricto: **OK** en las seis comprobaciones.

## Evidencia y límites

- **Compilación:** Debug sin errores.
- **Arranque:** `InicioNormal/run_local.py` con el ASI de este árbol, sin exclusiones.
  - [`startup-20261001-192521`](../InicioNormal/artifacts/startup-20261001-192521/summary.json), con las nueve primeras: menú estable 10 s.
  - [`startup-20261001-202002`](../InicioNormal/artifacts/startup-20261001-202002/summary.json), con las 39: no llegó al menú en 180 s. Estuvo 132 s en el estado 0 y avanzaba por los logos; sin errores en el registro.
  - [`startup-20261001-202323`](../InicioNormal/artifacts/startup-20261001-202323/summary.json), repetición: menú a los 130 s y estable 10 s; sin asserts ni excepciones.
  - En las tres ejecuciones no cambiaron las partidas ni los INI.
- **ASI instalado:** tras cada prueba se restauró el ASI que había antes. Hoy es `a0c5e7fb…`, el de la entrega de `CAEWeaponAudioEntity`.
- **Sin validar:** no se ha comparado ninguna función con el original y el menú apenas ejecuta tareas de peatones. Las implementaciones activas (constructores, destructores, `Clone`, `GetTaskType`, `MakeAbortable`, fábricas de subtareas, `ProcessPed` de varias tareas simples) están **sin validar**. Hace falta un banco de comparación por clase antes de marcar `validated` o confiar en su comportamiento durante la partida.

## Descartadas

| Clase | Motivo |
| --- | --- |
| `CTaskComplexBeCop`, `CTaskComplexUseMobilePhone`, `CTaskComplexWaitForPed`, `CTaskSimpleNone`, `CTaskSimpleTriggerLookAt` | Sin ninguna dirección en el código; `.cpp` vacío o solo declaraciones. |
| `CTaskAllocatorRetreat`, `CTaskAllocatorPlayerCommandRetreat` | Marcadas `// Android`. En el exe de PC solo hay 7 vtables de allocators y ninguna corresponde a estas. |
| `CPtrNodeSingleLink`, `CPtrNodeDoubleLink` | Plantillas sin funciones con dirección; sus pools ya están catalogados. |

## Hallazgos pendientes fuera de alcance

- `CAEWeaponAudioEntity::InjectHooks` usa el vtable 0x86C2AC, que es el de `CAEPedWeaponAudioEntity`; el suyo es 0x862E68. Funciona porque `UpdateParameters` ocupa el índice 0 en ambos.
- El comentario `// 0x5F68F0` de `TaskAllocatorRetreat.h` corresponde al `GetType` de otra clase.
- Destructores no-deleting sin dirección en el código (`CarDriveWander`, `DriveToPoint`, `EnterCarAs*`, `CarSetPedInAsDriver`, `CarSetPedOut`, `GangDriveBy`, `SetStayInSamePlace`, `TogglePedThreatScanner`). Pueden sacarse del destructor deleting en `vtable[0]`.

## Plantillas de clases sin código (2026-10-01, posterior)

Había 22 clases que no existían en el código ni en `hooks.csv`, salvo como llamadas comentadas en `InjectHooksMain.cpp`. Para 21 se creó **solo una plantilla, sin reconstruir nada**. `CTaskComplexDriveWander` no se creó porque ya existe como `CTaskComplexCarDriveWander` en `TaskComplexDriveWander.h`, registrada en la tanda anterior.

Cada plantilla tiene:

- Un `.h` que hereda de `CTaskSimple` o `CTaskComplex` y declara `Type` y `static void InjectHooks();`, con las virtuales comentadas. No tiene constructor, campos ni `VALIDATE_SIZE`.
- Un `.cpp` con `InjectHooks()` vacío.

Las plantillas no se registran en `InjectHooksMain` ni añaden filas a `hooks.csv`, así que las cifras del catálogo no cambian. `CTaskInteriorSitInChair` está en `Tasks/TaskTypes/Interior/`; las demás, en `Tasks/TaskTypes/`.

| Clase | `eTaskType` | Vtable | `GetTaskType` |
| --- | --- | --- | --- |
| `CTaskInteriorSitInChair` | `TASK_INTERIOR_SIT_ON_CHAIR` | 0x870314 | 0x675C80 |
| `CTaskComplexExtinguishFireOnFoot` | `TASK_COMPLEX_EXTINGUISH_FIRE_ON_FOOT` | 0x86F5FC | 0x6598A0 |
| `CTaskComplexFallAndStayDown` | `TASK_COMPLEX_FALL_AND_STAY_DOWN` | 0x870480 | 0x678A00 |
| `CTaskComplexFollowNodeRouteShooting` | `TASK_COMPLEX_FOLLOW_NODE_ROUTE_SHOOTING` | sin localizar | — |
| `CTaskComplexFollowPatrolRoute` | `TASK_COMPLEX_FOLLOW_PATROL_ROUTE` | 0x870114 | 0x670A60 |
| `CTaskComplexGoToPointUntilCollisionAndStandStill` | sin identificar | sin localizar | — |
| `CTaskComplexKillAllThreats` | `TASK_KILL_ALL_THREATS` | 0x86DA1C | 0x6221B0 |
| `CTaskComplexPresentIDToCop` | `TASK_COMPLEX_PRESENT_ID_TO_COP` | 0x86F544 | 0x659230 |
| `CTaskComplexReactToAttack` | `TASK_COMPLEX_REACT_TO_ATTACK` | 0x86D868 | 0x620B80 |
| `CTaskComplexUseEntryExit` | `TASK_COMPLEX_USE_ENTRYEXIT` | sin localizar | — |
| `CTaskSimpleBeHitWhileMoving` | `TASK_SIMPLE_BE_HIT_WHILE_MOVING` | 0x86D7FC | 0x61FE60 |
| `CTaskSimpleBeKickedOnGround` | `TASK_SIMPLE_BE_KICKED_ON_GROUND` | 0x86D7D8 | 0x61FC40 |
| `CTaskSimpleCarForcePedOut` | `TASK_SIMPLE_CAR_FORCE_PED_OUT` | 0x86EE94 | 0x647770 |
| `CTaskSimpleDetonate` | `TASK_SIMPLE_DETONATE` | sin localizar | — |
| `CTaskSimpleDuckWhileShotsWhizzing` | `TASK_SIMPLE_DUCK_WHILE_SHOTS_WHIZZING` | 0x870BEC | 0x6926E0 |
| `CTaskSimpleHailTaxi` | `TASK_SIMPLE_HAIL_TAXI` | sin localizar | — |
| `CTaskSimpleHailTaxiAndPause` | sin identificar | sin localizar | — |
| `CTaskSimpleOnEscalator` | `TASK_SIMPLE_ON_ESCALATOR` | 0x85B8DC | 0x4B8720 |
| `CTaskSimpleRunAnimLoopedMiddle` | `TASK_SIMPLE_ANIM_LOOPED_MIDDLE` | sin localizar | — |
| `CTaskSimpleTriggerEvent` | `TASK_SIMPLE_TRIGGER_EVENT` | sin localizar | — |
| `CTaskSimpleWaitForPizza` | `TASK_SIMPLE_WAIT_FOR_PIZZA` | 0x86E1DC | 0x632B50 |

**Cómo se localizaron los vtables:** se busca en `.text` el cuerpo `mov eax, <tipo>; ret` del `GetTaskType`, y luego su referencia en `.rdata` en la entrada 4. Las entradas 2 y 3 coinciden con la base: `0x43E300`/`0x43E310` en las simples y `0x421190`/`0x4211A0` en las complejas. El `N` de `RH_ScopedVirtualClass` es el de la base y está **por confirmar**.

En las ocho sin localizar, el patrón no aparece o el tipo no figura en `eTaskType`. En el caso de `UntilCollision`, el único vtable extra que comparte ese `GetTaskType`, 0x86FDD4, pertenece a `CTaskComplexGoToPointAndStandStillTimed`.

CMake usa `GLOB` sin `CONFIGURE_DEPENDS`, así que hay que reconfigurar (`cmake .` en `build/`) para que entren los archivos nuevos. Compilación Debug sin errores y auditoría estricta OK. Un commit por plantilla.

## Segunda tanda: `InjectHooks` sin llamar y tareas con vtable (2026-10-01)

**15 clases cuyo `InjectHooks()` estaba escrito pero nunca se invocaba.** Venían así desde la importación del upstream (`c41108e`) y ningún commit las había llamado: `CHud`, `CPedAttractor`, `CIdleCam`, `CTaskComplexChat`, `CCredits`, `CTaskComplexUseSequence`, `CPedShelterAttractor`, `CPlayerSkin`, `CCoverPoint`, `CDecision`, `CMaths`, `CTaskSequences`, `CLodTimeModelInfo`, `CVehicleSaveStructure` y `CTaskTimer`. Suman 115 hooks. `CVector`, `CVector2D`, `CQuaternion` y `CMatrix` siguen sin llamar a propósito, por rendimiento, según el comentario de `InjectHooksMain`.

Antes de registrarlas se revisaron y se corrigieron dos defectos que habrían roto el arranque:

- `CDecision::Set` (0x600570) era un stub `plugin::CallMethod<0x600570>` instalado como reconstruido. Activarlo habría causado recursión infinita, así que ahora va con `reversed=false`.
- `CLodTimeModelInfo` instalaba `GetModelType`/`GetTimeInfo` en 0x4C5660/0x4C5670, que son las de `CTimeModelInfo` y no están en su vtable 0x85BCF0. El registro habría abortado con "Couldn't find function in vtable". Las propias son 0x4C56C0 (`mov al,3; ret`) y 0x4C56D0 (`lea eax,[ecx+24h]`; `m_lodTimeInfo` está en +0x24).

Además se comprobaron los vtables de las cinco clases virtuales contra el exe y que ninguna dirección estuviera ya enganchada por otra clase.

**Tareas con vtable localizado en el exe que no tenían hooks.** Se buscó el `GetTaskType` de cada `eTaskType` en el exe y se cruzó con los `RH_ScopedVirtualClass` del código.

- Registradas, con instalaciones: `CTaskComplexCrossRoadLookAndAchieveHeading` (0x86DFF0), `CTaskSimpleCarDriveTimed` (0x859E50), `CTaskComplexCarDriveMissionFleeScene` (0x85B95C), `CTaskComplexCarDriveMissionKillPed` (0x85B994), `CTaskComplexBeCop` (0x85A428, N=15) y `CTaskSimpleNone` (0x859CFC).
- Con `InjectHooks()` escrito pero **sin registrar**, porque no tienen ninguna función implementada que instalar: `CTaskComplexWaitForPed`, `CTaskComplexUseMobilePhone`, `CTaskSimpleTriggerLookAt` y `CTaskComplexGoPickUpEntity`.
- Plantilla nueva: `CTaskComplexAvoidEntity` (0x86FF00). No existía en el código y ningún código del exe referencia su vtable.

**Fallo encontrado al probar y corregido.** El primer arranque abrió un assert de ReversibleHooks: "hooked to an address that is already hooked" (`ReversibleHooks.cpp:188`). La causa era `CTaskSimpleNone::Destructor` (0x463560). El enlazador del juego fusionó en una sola función dos destructores idénticos, y esa función ya la instala `CTaskSimpleLeaveGroup`. Se retiró la instalación duplicada.

La comprobación de duplicados recorre ahora todas las instalaciones `RH_Scoped*Install` del código, porque muchas filas `V` de `hooks.csv` tienen dirección 0 y no sirven para detectarlos.

**Catálogo:** 145 entradas nuevas, 144 tras retirar el destructor duplicado. Quedan 8 clases parciales con 17 pendientes, listadas en [Prioridades](../../docs/ReversingPriorities.md#incorporadas-2026-10-01).

**Evidencia:**

- Compilación Debug sin errores.
- 60 s sin diálogos tras arrancar.
- `CAEWeaponAudioEntity/smoke_test.py --seconds 35 --label hooksactivados`: **35 s dentro de partida**, con HUD y tareas de peatones ejecutándose, sin diálogos de assert y con las partidas sin cambios ([resumen](../CAEWeaponAudioEntity/artifacts/smoke-20261001-205508/summary.json)).
- El `runtime-hooks.csv` generado por el juego confirma el registro: `CHud` 38, `CPedAttractor` 20, `CIdleCam` 14, `CLodTimeModelInfo` 2 con las direcciones corregidas, etc.
- Al terminar se restauró el ASI instalado (`a0c5e7fb…`).
- Como en la primera tanda, no hay comparaciones función por función contra el original.

## Exe contra código: qué hay en uno y no en el otro

Se comparó el exe con el código y el catálogo en las dos direcciones. Detectar funciones y vtables por patrones de bytes es una aproximación: puede omitir algunos y contar otros de más.

**Lo que se engancha de verdad frente a lo que dice el catálogo.** `runtime-hooks.csv`, que genera el juego, tiene 8.443 hooks y `docs/hooks.csv` 8.440. Diferencias:

- 3 hooks del grupo `WindowedModeHooks` (modo ventana, código propio NOTSA) se registran pero no están en el catálogo. La auditoría no los ve porque se registran desde `notsa::InjectWindowedModeHooks()` y no desde `X::InjectHooks()`.
- 7 hooks tienen un `reversed` distinto en el catálogo y en el código:
  - `CAEStaticChannel::SetAudioBuffer`, `CCarFXRenderer::InitialiseDirtTexture` y `CConversations::DoneSettingUpConversation` figuran como reconstruidas en el CSV, pero el código sigue con `{ .reversed = false }` y en el juego corre el original.
  - Las cuatro de `CCustomBuildingRenderer` (`AtomicSetup`, `IsCBPCPipelineAttached`, `Update`, `UpdateDayNightBalanceParam`) figuran como pendientes en el CSV, pero el código del árbol actual las instala como reconstruidas y **se ejecutan en el juego** pese a estar documentadas como aplazadas.
  - No se tocaron: son trabajo en curso de otras entregas.

**Direcciones del catálogo que no son inicio de función.** De 6.919 direcciones no nulas, 16 no parecen un inicio de función. Nueve son falsas alarmas: funciones que empiezan justo después de una tabla de saltos o de un `jmp`. Las otras siete, todas heredadas del upstream y activas, apuntan al **medio** de otra función o de una instrucción:

| Hook | Dirección | Qué hay ahí |
| --- | --- | --- |
| `CVisibilityPlugins::SetClumpForAllAtomicsFlag` | 0x732307 | el `ret` final de otra función |
| `CLoadingScreen::IsActive` | 0x744DB5 | el `ret` final de otra función |
| `CVehicleModelInfo::SetRenderPipelinesCB` | 0x4C88F4 | el `ret` de la función que empieza en 0x4C88F0 |
| `CGame::ValidateVersion` | 0x5BA0BC | dentro de un salto condicional |
| `CTaskSimpleGiveCPR::FinishGiveCPRAnimCB` | 0x658910 | dentro de `mov fs:[0], esp` |
| `CConversations::EnableConversation` | 0x43A7F0 | dentro de un bucle de otra función |
| `CConversationNode::ClearRecursively` | 0x43A7A0 | dentro de una instrucción `mov` |

Un salto de 5 bytes escrito ahí altera la función que lo contiene cuando el juego la ejecuta. `CPedDamageResponseCalculator::IsBleedingWeapon` (0x4B5C2A) también cae a mitad de función, pero está con `reversed=0`. **Pendiente:** localizar la dirección real de cada una.

**Lo que hay en el exe y no tenemos.**

- **Funciones:** se detectan unas 14.100 en el código del juego (0x401000–0x7C0000), como destinos de `call` o punteros desde datos; 5.833 están en `hooks.csv`. Otras 6.875 detectadas en el rango de RenderWare/CRT (0x7C0000+) están casi todas fuera del catálogo, como es de esperar. El catálogo no pretende ser un inventario completo del exe.
- **Vtables:** se detectan 570, cargados mediante `mov [reg], vtable`. 430 tienen `RH_ScopedVirtualClass` y **140 no aparecen en el código**; quedan por clasificar. Las 10 `RH_ScopedVirtualClass` que la heurística no encontró son vtables de audio con N=1 cargados de otra forma, y son correctas.
- **Tareas:** ninguna tarea con vtable en el exe queda sin clase.
- **Eventos:** el método no sirve, porque los tipos de evento son números pequeños y el patrón coincide con funciones ajenas. Están sin revisar.
