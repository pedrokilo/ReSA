# Pruebas y evidencias de reconstrucción

La [hoja de ruta de PGTASA](../docs/ProjectRoadmap.md) sitúa estas pruebas dentro del objetivo de reconstrucción funcionalmente equivalente e independencia del EXE. Reconstrucción, validación, desbloqueo e independencia son dimensiones separadas; el total de pares no es un total de funciones validadas.

La evolución de la suite debe relacionar cada función y versión con sus bancos, dependencias controladas, configuración y evidencia, y permitir probar una compilación conjunta vigente. El [registro de validaciones](ValidationRegistry/README.md) ya vincula 1.020 funciones a sus pruebas; las tres acreditaciones de la versión anterior quedan pendientes por el cambio de huella global de source durante CustomBuilding. Las 1.020 conservan evidencia vinculada pendiente de revisión de dependencias. El [inventario de las 7.857 reconstruidas](../docs/ValidationInventory.md) identifica también las 6.837 todavía sin evidencia vinculada. El total global sigue parcialmente consolidado. Para matemáticas, física, manejo, IA y cámara se comparan entradas equivalentes, retornos, estados y efectos; las interacciones requieren además regresión de juego. Tras completar las clases, la campaña integral cubrirá nueva partida, guardar/cargar, misiones, conducción, muerte/arresto, interiores, clima, búsqueda policial, combate, streaming, audio y cinemáticas, con partidas desechables y restauración.

## Estado actual del catálogo de hooks

El corte vigente de `PGTASA` contiene **771 clases/grupos y 8.440 entradas**: 7.857 reconstruidas y 583 pendientes. De ellas, **1.020 tienen `validated=1`** porque cuentan con evidencia aprobada vinculada; 0 conservan vigencia acreditada para el árbol actual. La integridad entre fuente, registro global y `docs/hooks.csv` se comprueba con:

```powershell
python contrib/audit_hook_catalog.py --strict
```

Con `--runtime "D:/JUEGOS/GTA San Andreas - CVehicle Test/scripts/runtime-hooks.csv"` compara además el catálogo con lo que el ASI registra de verdad al arrancar. El resultado actual es **OK** en las seis comprobaciones críticas y sin diferencias con el registro real, incluida la nueva detección de llamadas `X::InjectHooks()` que resuelven al `InjectHooks` heredado de una base. El modo `--verbose` lista además owners inactivos, aliases y registros indirectos; esos elementos informativos no se suman automáticamente al progreso.

Esta carpeta sí se utiliza. Ejecutar `PROBAR.cmd` desde la raíz para elegir una prueba; `PROBAR.cmd --list` muestra los casos disponibles. No se necesita un CMD por función.

`PROBAR.cmd --all` ejecuta en serie todos los bancos dentro de GTA. Para evitar la pausa del CMD, usar `python reversing/run.py --all`. Abre y cierra la copia de pruebas para cada banco, vuelve a analizar sus logs nuevos y comprueba la restauración del ASI/PDB y los hashes de las partidas antes de continuar. Los informes conjuntos quedan en `reversing/artifacts/suite-*/summary.json`, con salidas por caso y rutas/hashes de la evidencia. Se omite `pcsave-isolated` porque la misma función ya se prueba dentro del motor.

El catálogo actual contiene **90 casos: 89 dentro del motor y uno aislado**.

Esta serie usa las sondas archivadas de cada trabajo. El caso `conversation-for-ped` comprueba además que fuentes e instrumentación coinciden con su manifiesto; si cambiaron, recompilar con `python reversing/ConversationForPed/build.py --probe` antes de ejecutarlo. La serie no es una prueba de integración de todos los cambios en una única compilación actual ni sustituye la campaña de estabilidad futura.

- [AudioCompletion](AudioCompletion/README.md): `CAEAmbienceTrackManager` 7/7; `UpdateAmbienceTrackAndVolume` validada con 7.846 pares. Caso `audio-completion`.
- [HooksActivados](HooksActivados/README.md): 39 clases registradas el 01/10/2026 (9 descomentadas, 30 con `InjectHooks()` nuevo y vtables verificados en el exe); 216 entradas nuevas. Solo compilación y arranque al menú, sin comparaciones contra el original. Incluye plantillas vacías de 21 clases sin código, fuera del catálogo.
- [BreakBatch](BreakBatch/README.md): `BreakablePlugin` y `BreakManager_c`, 12 funciones y 3.232 pares. Caso `break`.
- [AppBatch](AppBatch/README.md): `App`, `CIniFile` y `CIplDefPool`, 9 funciones y 3.008 pares (orquestación con dependencias interceptadas). Caso `app`.
- [SmallBatch](SmallBatch/README.md): diez clases pequeñas (KeyGen, KeyArray, KeyboardState, LinkedUpgradeList, LoadedCarGroup, Acquaintance, AccidentManager, Localisation, ActiveOccluder, LodAtomicModelInfo), 52 funciones y 16.400 pares. Caso `small`.
- [TasksBatch2](TasksBatch2/README.md): TurnToFace, UseEffectRunning/Sprinting y ScriptedAttractorRun/Sprint, 19 funciones y 7.488 pares. Caso `tasks2`.
- [AudioChannel](AudioChannel/README.md): `CAEAudioChannel`, 12 de 12 funciones (EAX reactivado con los datos del EXE) y 5.184 pares. Caso `channel`.
- [LoadMonitor](LoadMonitor/README.md): `CLoadMonitor`, 7 funciones y 4.864 pares exactos (x87 del juego a 24 bits). Caso `loadmon`.
- [AmbienceTrack](AmbienceTrack/README.md): resto de `CAEAmbienceTrackManager` (6 funciones, 4.416 pares) y fórmula de `CAEAudioUtility::GetRandomNumberInRange<int>`. Caso `ambience`.
- [TwoDEffect](TwoDEffect/README.md): `C2dEffect`, 14 funciones y 3.520 pares (cuatro diferencias corregidas en la lectura del plugin). Caso `twod`.
- [PcSave](PcSave/README.md): `C_PcSave`, 5 funciones y 832 pares (`InitUserDirectories`, `DeleteFileA`, slot corrupto). Caso `pcsavebatch`.
- [TasksBatch3](TasksBatch3/README.md): `UseGoggles`, `UseAttractor`, `UseAttractorPartner`, `UseEffect` y `UsePairedAttractor`, 38 funciones y 19.168 pares; corrige el orden de la vtable de `CPedAttractor` y el hook de 0x407180. Caso `tasks3`.
- [Markers](Markers/README.md): `C3dMarker` (6) y `C3dMarkers` (20), 17.056 pares; `fsin` x87, constantes exactas y `CMatrix operator*` con el orden del EXE. Caso `markers`.
- [IplStore](IplStore/README.md): 26 funciones de `CIplStore` y 6.240 pares (`IncludeEntity`, `SetIplsRequired` previo, `EnsureIplsAreInMemory`). Caso `ipl`.
- [IplStore2](IplStore2/README.md): las 8 restantes de `CIplStore` (carga y borrado), 3.008 pares. Caso `ipl2`.
- [Loading](Loading/README.md): `CLoadingScreen`, 20 funciones y 9.328 pares (reescrita fiel al EXE manteniendo las opciones del cargador rápido). Caso `loading`.
- [BreakObj](BreakObj/README.md): `BreakObject_c`, 9 funciones y 10.752 pares (RNG en línea, `RwV3dNormalize`, `acos` del CRT del juego). Caso `breakobj`.
- [Small2](Small2/README.md): 11 clases pequeñas (CMaths, CMatrixLink/List, CLodTimeModelInfo, CMissionTextOffsets, CAEAudioEntity, CAEAudioUtility, CAEAudioEnvironment, CAEExplosionAudioEntity, CAEDoorAudioEntity, CModelInfo), 74 funciones y 32.016 pares. Caso `small2`.
- [Mirrors](Mirrors/README.md): `CMirrors`, 8 funciones y 6.272 pares (búferes globales del EXE, `TypeOfMirror` de 32 bits). Caso `mirrors`.
- [Messages](Messages/README.md): `CMessages`, 28 funciones y 25.152 pares (`CutString`, `~a~`, colas como el original). Caso `messages`.
- [TasksBatch4](TasksBatch4/README.md): `CTaskComplexUse*` (ScriptedAttractor, ScriptedBrain, Sequence, SwatRope, WaterCannon), 33 funciones y 15.872 pares. Caso `tasks4`.
- [TasksBatch5](TasksBatch5/README.md): `WaitAtAttractor`, `WaitForBackup`, `WaitForBus`, `WaitForDryWeather` y `WalkAlongsidePed`, 38 funciones y 12.064 pares; corrige el envoltorio de 0x661DC0 de `CTaskComplexSeekEntity`. Caso `tasks5`.
- [MenuSystem2](MenuSystem2/README.md): las 20 funciones restantes de `CMenuSystem`, 16.640 pares. Caso `menusys2`.
- [Pools56](Pools56/README.md): los 14 `New` de pools y 9 funciones sueltas (lista de 56), 23 funciones y 13.056 pares; corrige el tamaño de slot del pool de atractores. Caso `pools56`.
- [EventsTasks56](EventsTasks56/README.md): constructores de 9 eventos y 9 tareas, 2 `AffectsPed` y 3 virtuales de tareas (lista de 56), 23 funciones y 4.352 pares. Caso `evtask56`.
- [Misc56](Misc56/README.md): 4 calculadores de posición de `SeekEntity`, `PlacePedAtEffect`, `CRegisteredCorona::Update`, `CCheckpoint::Render` y otras 3 (lista de 56), 10 funciones y 6.464 pares. Caso `misc56`.
- [Events70](Events70/README.md): 21 clases de eventos (constructor + `AffectsPed`/`AffectsPedGroup`/`IsValid`/`CloneEditable`), 42 funciones y 13.120 pares. Caso `events70`.
- [Tasks70](Tasks70/README.md): repartidores de tareas de grupo, `CPedGroups`, `CTaskSequences`, tareas sueltas y `CTimeModelInfo`, 28 funciones y 11.296 pares. Caso `tasks70`.
- [Core70](Core70/README.md): cajas, colisiones, pools de objetos y peatones, estructuras de guardado, `CTempColModels`, `ModelIndices`, viento y más, 44 funciones y 14.992 pares; `Reorthogonalise` exacto y `CalculateAspectRatio` en ST0. Caso `core70`.
- [Misc70](Misc70/README.md): streams de audio, `CColourSet`, `CCrime`, interfaz, cristales, gamma, estelas y Securom, 24 funciones y 12.416 pares. Caso `misc70`.
- [Events52](Events52/README.md): `CEvent` y 9 clases de eventos (lista de 52), 30 funciones y 13.888 pares; corrige la desvirtualización de `CEventSoundQuiet::AffectsPed`. Caso `events52`.
- [SimpleTasks52](SimpleTasks52/README.md): 16 tareas simples (golpes, mirar, soltar, quieto, cansado, charla, arrestar), 48 funciones y 11.904 pares. Caso `stasks52`.
- [PedTasks52](PedTasks52/README.md): subir/bajar de coches, ir disparando, repartidores, `CCivilianPed`, cobertura, `CPedPlacement` y `CSetPiece`, 32 funciones y 11.072 pares. Caso `ptasks52`.
- [World52](World52/README.md): edificios animados, paneles, fuego, objeto en la mano, temblor de cámara, ratón, oclusores, estelas, plantas, sombras, plugins de RW y criaturas del agua, 42 funciones y 18.384 pares; `CMatrix::SetRotate` exacto. Caso `world52`.
- [Render23](Render23/README.md): luces y textos brillantes, estelas de movimiento y de aviones, escaleras, puente y piel del jugador (lista de 23), 28 funciones y 9.216 pares. Caso `render23`.
- [Events23](Events23/README.md): 5 eventos, `CEntityScanner`, `CScriptedBrainTaskStore` y `CReferences` (lista de 23), 32 funciones y 11.328 pares; corrige la prioridad invertida de `CEventStuckInAir`. Caso `events23`.
- [Data23](Data23/README.md): audio de armas del peatón, balas, modelos con daño, garajes, estadísticas de peatones, plantas por superficie, recursos de scripts y matrículas especiales (lista de 23), 32 funciones y 13.904 pares. Caso `data23`.
- [InformQueues](InformQueues/README.md): colas de eventos de grupo, 11 funciones y 6.464 pares. Caso `inform`.
- [BoneNodes](BoneNodes/README.md): `BoneNode_c` y `BoneNodeManager_c`, 25 funciones y 12.256 pares exactos bit a bit. Caso `bones`.
- [TasksBatch](TasksBatch/README.md): cinco clases de tareas, 39 funciones y 20.480 pares. Caso `tasks`.
- [GroupTasksCompletion](GroupTasksCompletion/README.md): `CPedGroupDefaultTaskAllocatorRandom` y `CPedGroupDefaultTaskAllocatorSitInLeaderCar` 2/2; sus `AllocateDefaultTasks` y `CTaskComplexGangFollower::SetCanEnterLeadersCar` validadas con 4.608 pares dentro del motor. Caso `group-alloc`.
- [GangLeader](GangLeader/README.md): GetTaskType desbloqueado tras 24 pares; entrega normal aislada e instalada, diez segundos de menú y hook activo comprobados. Caso `gang-leader-type`; el lote aplazado documentado conserva 85 bloqueos.
- `run.py` y `cases.json`: menú y catálogo común.
- `CVehicle`, `C_PcSave`, `CPlayerPed`, `CPed`, `CWeaponEffects`: pruebas repetibles y evidencia de las funciones trabajadas.
- `RpAnimBlend`: actualización de animaciones, validada mediante 652 comparaciones dentro del motor.
- `CBaseModelInfo`: retorno de SwaysInWind, validado con 65.536 combinaciones de flags.
- `FxManager`: destrucción de sistemas y recorrido de Update, validados con 720 comparaciones dentro del motor.
- `CClouds`: RenderBottomFromHeight, validada con 1.170 comparaciones exactas de geometría, proyección, sprites y estados dentro del motor.
- `CMenuSystem`: FillGridWithCarColours, validada con 784 comparaciones dentro del motor, 196 paletas y 50.176 índices de color.
- `CTaskSimpleUseGun`: ProcessPed, validada con 3.232 comparaciones dentro del motor: 64 con dependencias reales y 3.168 con dependencias de animación/apuntado controladas; consultar su alcance específico.
- [CTaskInteriorSitAtDesk](CTaskInteriorSitAtDesk/README.md): ProcessPed, ya `reversed=1` antes de esta tarea; 144 comparaciones dentro del motor con jugador y clump reales. No cubre `m_bTaskAborting` ni `!m_Anim` (ambas gateadas por `InteriorManager_c::AreAnimsLoaded`, que aborta de forma reproducible al ejecutarse de forma aislada); también corrigió una llamada `Interior()` ausente en `InjectHooksMain.cpp` que dejaba sin instalar los hooks de toda la familia `CTaskInterior*`. Caso `sitatdesk`, incluido en `--all`.
- `runtime_runner.py`: ejecución, restauración y registro compartidos; utilizado inicialmente por RpAnimBlend.
- `MEJORAS_DEL_PROCESO.md`: método de trabajo y criterios de validación.

La caché local `CVehicle/conan-cache` sigue referenciada por CMake y las dependencias. No es una carpeta abandonada y no se debe borrar mientras el build dependa de ella. Los binarios, registros extensos y cachés quedan fuera del commit mediante las reglas de Git existentes.

Se eliminaron los siete lanzadores específicos de la raíz y tres copias temporales de fuentes ya restauradas. No se eliminaron informes de pruebas ni código necesario para repetirlas.

- `CPed`: 145/145 entradas registradas; banco `cped-completion` con 28.672 pares de las seis ultimas funciones. Consultar su README y validation-summary.json para dependencias controladas y limites.

- [CPedToPlayerConversations](PedToPlayerConversations/README.md): 3/3; Update contrastado con 6.266 pares. Caso `ped-to-player`; dependencias de audio, entrada e IK controladas.
- [CConversationForPed](ConversationForPed/README.md): 2/2; 4.702 pares exactos (Update: 2.288; posición: 2.414), cierre natural y restauración. Caso `conversation-for-ped`; comparación con dependencias controladas. Binario normal instalado tras tres arranques de tres segundos, ambos hooks comprobados y cierres con código 0; partidas y configuración conservadas.
- `CConversations`: 10/10 (se retiró `EnableConversation`, inexistente en el EXE); 2.048 pares de `DoneSettingUpConversation`. Configuración de tablas y referencias, sin reproducción de diálogos. El subsistema incluye además `CConversationNode` 1/1, `CPedToPlayerConversations` 3/3 y `CConversationForPed` 2/2; consultar [su README](CConversations/README.md).
- CPedList: 7/7; 4.096 pares de sus cuatro funciones pendientes. Fixtures de grupos/pool y consultas de inteligencia controladas.

- `CPlayerInfo`: tres funciones, 16.896 pares dentro del motor; 28/28 registradas y habilitadas; constructor corregido con otros 4.224 pares. Ver README y validation-summary.json.

- `FurnitureSubGroup_c`: 3/3, seleccion por ID/riqueza y almacenamiento; 20.480 comparaciones dentro del motor, --case furniture.


Banco añadido: [CClothes, CScripted2dEffects y Sprint](ClothesScriptedSprint/README.md), 8.448 pares; `python reversing/run.py --case clothes-scripted-sprint`. Integrado en `--all`.

Banco añadido: [CGeneral: RNG float](CGeneral/README.md), 65.536 pares exactos y desbloqueo; `python reversing/run.py --case general`, incluido en `--all`.

[CAESound: copia y referencias](CAESound/README.md): 1.024 pares exactos, Audio activo, caso `sound-copy`. Los ejecutores compartidos activan la carga rápida solo durante las pruebas y restauran ambos INI. [Inicio normal y diagnóstico del cierre](InicioNormal/README.md).

## Diez clases del Top 10 completadas

[Informe conjunto y límites](Top10/README.md): **51.712 pares exactos**, 22 entradas habilitadas y dos funciones auxiliares revalidadas. Los cuatro bancos se repitieron con las diez clases habilitadas antes de instalar el ASI normal.

| Caso autónomo | Banco | Pares |
| --- | --- | ---: |
| `shooting-resources` | [Shooting y recursos](ShootingResources/README.md) | 18.432 |
| `leave-car-bus` | [Salir del coche y autobús](LeaveCarBus/README.md) | 5.632 |
| `population-factory` | [Población, fábrica y amenazas](PopulationFactory/README.md) | 18.432 |
| `scanner-drag-fx` | [Escáner, expulsión y FX](ScannerDragFx/README.md) | 9.216 |

Ejemplo: `python reversing/run.py --case scanner-drag-fx`. Integrados en el catálogo y `--all`, conservando un solo PROBAR.cmd. Esta tarea no repitió toda la suite histórica.


## Desbloqueos y plataforma conmutable

[Informe conjunto](Unlocks/README.md): **24 candados retirados**, 50.432 pares en siete bancos y nueve arranques de plataforma (324 pares repetidos, 18 rutas de entrada). CGeneral conserva su desbloqueo anterior. Incluye correcciones de ABI, pools, física, audio, radio, conducción y SDL/ImGui.

Casos nuevos: `unlock-core`, `unlock-tasks`, `unlock-physical`, `unlock-audio`, `unlock-radio`, `unlock-driving`, `unlock-input` y `unlock-platform`. Todos disponibles en el único PROBAR.cmd y `--all`. Para las nueve combinaciones: `python reversing/UnlockPlatform/run_matrix.py`.

Se conserva la configuración de inicio normal. Los bancos usan carga rápida temporal, restauran ASI/PDB e INI y verifican partidas. Los binarios históricos de bancos no prueban conjuntamente toda la compilación actual; la campaña integral sigue aplazada.


## Entrega parcial: DataStream y TrackEntity

[Informe y trabajo aplazado](UnlockBatch2/README.md): diez candados retirados, 15.360 pares y 64 combinaciones de DataStream. Casos `unlock-data-stream` y `unlock-track-entity` añadidos a cases.json, PROBAR.cmd y `--all`. Los borradores de CFileMgr/Game quedan archivados fuera de la compilación; no se añaden como bancos aprobados. Aquella entrega dejó 86 bloqueos aplazados; tras completar [GetTaskType de GangLeader](GangLeader/README.md), la cola vigente conserva 85.


## CEntryExit: reserva agotada

[Diagnóstico y corrección](CEntryExitPool/README.md): reserva ampliada deliberadamente de 400 a 512. Banco de 1.854 comprobaciones, con 21 diferencias conocidas de FIX_BUGS; reproducción de las mismas 19 altas fallidas en original y candidato a 400, ninguna a 512. Integración de aquella entrega: 120,24 s y 3.404 fotogramas sin fallos de CEntryExit; otros avisos documentados. Caso `entryexit-pool` disponible en PROBAR.cmd y `--all`. No cambió ningún indicador del CSV.

## CEntryExit: TransitionFinished dentro del motor

[Informe y límites](CEntryExitTransition/README.md): `TransitionFinished` (`0x004404A0`) ya figuraba `reversed=1` antes de esta tarea; se aportó la comparación dentro del motor que faltaba. Fase 1: 960 pares originales/candidatos exactos (192 por cada uno de cinco escenarios de la máquina de estados), sin diferencias. Fase 2 (por pedido expreso de validar también lo invasivo): 9 pares más con el jugador real ejecutando de verdad la transición a `EXIT_ENTER_STATE_3` y el bloque final completo (streaming, teleporte, población, scripts, `CGame::TidyUpMemory`) una vez, restaurado byte a byte después. 969 pares en total, 0 diferencias; no fue necesario corregir el código. Caso `enextransition` disponible en PROBAR.cmd y `--all`. No cambió ningún indicador del CSV.

## Peatones: desplazamiento de animación sin inicializar

[Diagnóstico y corrección](FallenPeds/README.md): el constructor con parámetros de CTaskSimpleRunNamedAnim dejaba un desplazamiento y su bandera pendiente sin inicializar. Banco aprobado de 8.832 pares contra el original; control anterior con 8.160 diferencias de inicialización y 64 saltos de posición. Caso `namedanim-init` integrado en PROBAR.cmd y `--all`. No completa la clase ni cambia el CSV.

## IK y datos incompletos

[Diagnóstico y reparación](IKDataWarnings/README.md): se evita buscar el pivote deliberadamente excluido de la cadena; 131 pares únicos coincidentes con el original. Datos reparados solo en la copia de pruebas, con respaldo: clima, skimmer y grupos de coches. Las tablas de clima y entradas activas de grupos coinciden con el original usando los datos corregidos. Integración: 121,16 s y 3.605 fotogramas, cero avisos de IK/datos/peatones y cero fallos de CEntryExit. Caso `ik-data` integrado en el único PROBAR.cmd y `--all`: **41 casos, 40 de motor y uno aislado**. No se repitió toda la suite ni cambió el CSV; no se afirma estabilidad general.

## Recuperación del ratón al arrancar

[Observación con el dispositivo real](MouseRecovery/README.md): el fallo inicial de Acquire se reproduce con la ventana oculta y sin foco; a los 1,485 s la adquisición y lectura son correctas, seguidas de 120 actualizaciones válidas. No requiere cambio del código normal. Este diagnóstico no suma pares ni casos al catálogo diferencial. La misma entrega revisa todos los Markdown propios y sus referencias vigentes.
