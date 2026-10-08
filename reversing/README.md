# Pruebas y evidencias de reconstrucción

La [hoja de ruta de ReSA](../docs/ProjectRoadmap.md) sitúa estas pruebas dentro del objetivo de reconstrucción funcionalmente equivalente e independencia del EXE. Reconstrucción, validación, desbloqueo e independencia son dimensiones separadas; el total de pares no es un total de funciones validadas.

La evolución de la suite debe relacionar cada función y versión con sus bancos, dependencias controladas, configuración y evidencia, y permitir probar una compilación conjunta vigente. El [registro de validaciones](ValidationRegistry/README.md) ya vincula 6.684 funciones a sus pruebas; las acreditaciones históricas se revisan contra las fuentes y dependencias del árbol actual. Las 6.684 conservan evidencia vinculada pendiente de revisión de dependencias. El [inventario de las 8.394 reconstruidas](../docs/ValidationInventory.md) identifica también las 1.710 todavía sin evidencia vinculada. El total global sigue parcialmente consolidado. Para matemáticas, física, manejo, IA y cámara se comparan entradas equivalentes, retornos, estados y efectos; las interacciones requieren además regresión de juego. Tras completar las clases, la campaña integral cubrirá nueva partida, guardar/cargar, misiones, conducción, muerte/arresto, interiores, clima, búsqueda policial, combate, streaming, audio y cinemáticas, con partidas desechables y restauración.

## Estado actual del catálogo de hooks

El corte vigente de `ReSA` contiene **771 clases/grupos y 8.446 entradas**: 8.394 reconstruidas y 52 pendientes. De ellas, **6.684 tienen `validated=1`** porque cuentan con evidencia aprobada vinculada; 0 conservan vigencia acreditada para el árbol actual. La integridad entre fuente, registro global y `docs/hooks.csv` se comprueba con:

```powershell
python contrib/audit_hook_catalog.py --strict
```

Con `--runtime "D:/JUEGOS/GTA San Andreas - CVehicle Test/scripts/runtime-hooks.csv"` compara además el catálogo con lo que el ASI registra de verdad al arrancar. El resultado actual es **OK** en las seis comprobaciones críticas y sin diferencias con el registro real, incluida la nueva detección de llamadas `X::InjectHooks()` que resuelven al `InjectHooks` heredado de una base. El modo `--verbose` lista además owners inactivos, aliases y registros indirectos; esos elementos informativos no se suman automáticamente al progreso.

Esta carpeta sí se utiliza. Ejecutar `PROBAR.cmd` desde la raíz para elegir una prueba; `PROBAR.cmd --list` muestra los casos disponibles. No se necesita un CMD por función.

`PROBAR.cmd --all` ejecuta en serie todos los bancos dentro de GTA. Para evitar la pausa del CMD, usar `python reversing/run.py --all`. Abre y cierra la copia de pruebas para cada banco, vuelve a analizar sus logs nuevos y comprueba la restauración del ASI/PDB y los hashes de las partidas antes de continuar. Los informes conjuntos quedan en `reversing/artifacts/suite-*/summary.json`, con salidas por caso y rutas/hashes de la evidencia. Se omite `pcsave-isolated` porque la misma función ya se prueba dentro del motor.

El catálogo actual contiene **297 casos: 296 dentro del motor y uno aislado**.

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
- [STasks19](STasks19/README.md): 10 tareas simples (encogerse, manos arriba, cajero, pausa, ninguna, golpe en la cabeza, en el aire, ir a, recoger, esperar al líder) (lista de 19), 40 funciones y 18.432 pares; corrige el tipo de `CTaskSimpleUseAtm`. Caso `stasks19`.
- [CTasks19](CTasks19/README.md): `CTask`, `CTaskAllocatorKillOnFoot` y 7 tareas complejas (misiones de coche, trepar, caer y aterrizar, abrir la puerta, paseos de pandillero y médico) (lista de 19), 36 funciones y 12.544 pares; `operator delete` de las tareas como el EXE. Caso `ctasks19`.
- [Misc8](Misc8/README.md): `cTransmission`, `CWaterCannons`, `FxInfoManager_c`, `FxSphere_c` y `ProcSurfaceInfo_c` (lista de 8), 20 funciones y 11.712 pares. Caso `misc8`.
- [RenderVideo8](RenderVideo8/README.md): `RenderBuffer`, `RwCore` y `VideoMode` (lista de 8), 12 funciones y 6.912 pares; corrige la estructura `RwGlobals`. Caso `rendervideo8`.
- [STasks43](STasks43/README.md): 8 tareas simples (lista de 43), 40 funciones y 18.880 pares. Caso `stasks43`.
- [CTasks43](CTasks43/README.md): 8 tareas complejas (lista de 43), 40 funciones y 11.072 pares. Caso `ctasks43`.
- [Events43](Events43/README.md): eventos de disparo y daño a vehículo, asignadores de ataque, tareas de scripts, datos del jugador, coches volcados y grupos de animación de vehículos (lista de 43), 45 funciones y 13.824 pares. Caso `events43`.
- [World43](World43/README.md): zonas de audio, esferas de colisión, grúas, escaleras, cuadrícula, colores del HUD, oclusión, escenas y efectos 2D de scripts (lista de 43), 44 funciones y 14.912 pares. Caso `world43`.
- [Fx43](Fx43/README.md): audio del clima, trazas de balas, fuego, cosas en movimiento, disparos, cañón de agua, pool de memoria de efectos, entrada y JPEG (lista de 43), 44 funciones y 15.552 pares. Caso `fx43`.
- [AnimAudio6](AnimAudio6/README.md): bancos de audio, armas globales, datos de animación de clumps, asociaciones estáticas, objetos de cinemática, puertas y vídeo (lista de 6), 42 funciones y 13.568 pares. Caso `animaudio6`.
- [Misc6](Misc6/README.md): pájaros, créditos, curvas, cronómetros, horarios, muebles y ventana (lista de 6), 41 funciones y 19.456 pares. Caso `misc6`.
- [CTasks6a](CTasks6a/README.md): 7 tareas complejas A (lista de 6), 42 funciones y 14.784 pares. Caso `ctasks6a`.
- [CTasks6b](CTasks6b/README.md): salto, huida, tienda y tareas de coche (lista de 6), 42 funciones y 13.952 pares. Caso `ctasks6b`.
- [STasks6a](STasks6a/README.md): 7 tareas simples A (lista de 6), 42 funciones y 21.312 pares. Caso `stasks6a`.
- [STasks6b](STasks6b/README.md): 8 tareas simples B (lista de 6), 48 funciones y 18.688 pares. Caso `stasks6b`.
- [STasks6c](STasks6c/README.md): 8 tareas simples C (lista de 6), 48 funciones y 15.232 pares. Caso `stasks6c`.
- [CTasks7a](CTasks7a/README.md): charla, salto desde entidad, ruta de puntos en coche, ir y quedarse quieto, respuesta a golpe y en el agua (lista de 7), 42 funciones y 13.120 pares. Caso `ctasks7a`.
- [CTasks7b](CTasks7b/README.md): matar con sigilo, salir de cualquier coche, saludo de pareja, buscar apuntando y huida inteligente (lista de 7), 35 funciones y 10.496 pares. Caso `ctasks7b`.
- [STasks7](STasks7/README.md): esperar puerta libre, morir, atropellar, animación temporizada, sentado y muerte sigilosa (lista de 7), 42 funciones y 19.520 pares. Caso `stasks7`.
- [Data7](Data7/README.md): policía, ropa, listas de peatones, plantas, marcas de derrape, efectos especiales y streaming (lista de 7), 45 funciones y 13.760 pares. Caso `data7`.
- [Audio7](Audio7/README.md): audio del fuego, cargador de pistas MP3, sonidos dobles, cañón de agua, pipeline de edificios y cargador de decisiones (lista de 7), 42 funciones y 11.776 pares. Caso `audio7`.
- [Mgr7](Mgr7/README.md): saltos acrobáticos, generadores de coches, muebles, criaturas del agua y tareas de interior (lista de 7), 49 funciones y 15.488 pares. Caso `mgr7`.
- [Data8a](Data8a/README.md): pistas de cinemática. canal estático. generador de coches. checkpoints. colisión y bandas (lista de 8). 47 funciones y 18.176 pares. Caso `data8a`.
- [Data8b](Data8b/README.md): eventos de bici y script. tareas de script. sombras. rectángulos y coches atascados (lista de 8). 48 funciones y 17.024 pares. Caso `data8b`.
- [Fx8](Fx8/README.md): efectos. objetos procedurales y tareas de interior (lista de 8). 40 funciones y 15.552 pares. Caso `fx8`.
- [CTasks8a](CTasks8a/README.md): tareas complejas A (lista de 8). 48 funciones y 14.144 pares. Caso `ctasks8a`.
- [CTasks8b](CTasks8b/README.md): tareas complejas B (lista de 8). 48 funciones y 12.736 pares. Caso `ctasks8b`.
- [CTasks8c](CTasks8c/README.md): tareas complejas C y golpes (lista de 8). 48 funciones y 16.896 pares. Caso `ctasks8c`.
- [STasks8a](STasks8a/README.md): tareas simples A (lista de 8). 40 funciones y 12.224 pares. Caso `stasks8a`.
- [STasks8b](STasks8b/README.md): tareas simples B (lista de 8). 32 funciones y 12.608 pares. Caso `stasks8b`.
- [Core9](Core9/README.md): decodificador WAV. reloj. CdStream. CGeneral y comunes (lista de 9). 45 funciones y 19.968 pares. Caso `core9`.
- [Models9](Models9/README.md): modelos atómicos y de peatón. colisión. carteles y textos (lista de 9). 45 funciones y 14.400 pares. Caso `models9`.
- [Ped9](Ped9/README.md): iK del peatón. efectos de armas. cadena IK. lanzar y recoger bici (lista de 9). 45 funciones y 14.720 pares. Caso `ped9`.
- [CTasks9a](CTasks9a/README.md): tareas complejas A (lista de 9). 45 funciones y 14.592 pares. Caso `ctasks9a`.
- [CTasks9b](CTasks9b/README.md): tareas complejas B (lista de 9). 45 funciones y 13.696 pares. Caso `ctasks9b`.
- [CTasks9c](CTasks9c/README.md): tareas complejas C (lista de 9). 45 funciones y 19.840 pares. Caso `ctasks9c`.
- [CTasks9d](CTasks9d/README.md): búsqueda de entidad y sentarse en escritorio (lista de 9). 36 funciones y 13.568 pares. Caso `ctasks9d`.
- [STasks9](STasks9/README.md): tareas simples de coche (lista de 9). 45 funciones y 16.448 pares. Caso `stasks9`.
- [Sys10](Sys10/README.md): decodificador MF. fundido de audio. conversaciones. directorio e historial de eventos (lista de 10). 46 funciones y 17.088 pares. Caso `sys10`.
- [Data10](Data10/README.md): datos de armas. plantillas de efectos y estar en grupo (lista de 10). 30 funciones y 8.896 pares. Caso `data10`.
- [CTasks10a](CTasks10a/README.md): tareas complejas A (lista de 10). 40 funciones y 15.488 pares. Caso `ctasks10a`.
- [CTasks10b](CTasks10b/README.md): tareas de cobertura. cambio de asiento y coche (lista de 10). 40 funciones y 16.256 pares. Caso `ctasks10b`.
- [STasks10](STasks10/README.md): tareas simples de coche (lista de 10). 50 funciones y 19.136 pares. Caso `stasks10`.
- [CTasks11](CTasks11/README.md): respuesta editable, conducir, ser arrastrado y huir a un punto (lista de 11). 44 funciones y 12.672 pares. Caso `ctasks11`.
- [Audio11](Audio11/README.md): voz sin peatón, decodificador WMA, ropa y aceleración de colisiones (lista de 11). 43 funciones y 13.632 pares. Caso `audio11`.
- [World11](World11/README.md): coronas, entradas/salidas, reaparición y manejo de vehículos (lista de 11). 44 funciones y 16.704 pares. Caso `world11`.
- [Mix11](Mix11/README.md): helicóptero, ir a la puerta del coche y nadar (lista de 11). 33 funciones y 19.520 pares. Caso `mix11`.
- [Sys12](Sys12/README.md): scripts en streaming, `Game` y árbol cuaternario (lista de 12). 34 funciones y 12.320 pares. Caso `sys12`.
- [World12](World12/README.md): semáforos, pickups y explosiones (lista de 12). 36 funciones y 11.392 pares. Caso `world12`.
- [Mix12](Mix12/README.md): grupo de eventos, rodear edificio y hierba (lista de 12). 36 funciones y 15.488 pares. Caso `mix12`.
- [Audio13](Audio13/README.md): banco MP3 y decodificador Vorbis (lista de 13). 26 funciones y 11.520 pares. Caso `audio13`.
- [CTasks13](CTasks13/README.md): seguir ruta, líder de banda, persecución policial y secuencia (lista de 13). 51 funciones y 16.384 pares. Caso `ctasks13`.
- [Sys13](Sys13/README.md): tipos de peatón, temporizador y remolque (lista de 13). 39 funciones y 15.872 pares. Caso `sys13`.
- [Audio14](Audio14/README.md): gestor de sonidos e hilo de streaming de audio (lista de 14). 28 funciones y 10.240 pares. Caso `audio14`.
- [Render14](Render14/README.md): secuencias de animación y pipeline día/noche de edificios (lista de 14). 28 funciones y 6.784 pares. Caso `render14`.
- [Mix14](Mix14/README.md): gestor de atractores y matrículas (lista de 14). 28 funciones y 11.904 pares. Caso `mix14`.
- [CTasks14a](CTasks14a/README.md): entrar en el coche y deambular (lista de 14). 27 funciones y 11.776 pares. Caso `ctasks14a`.
- [CTasks14b](CTasks14b/README.md): sostener objeto y jetpack (lista de 14). 27 funciones y 12.352 pares. Caso `ctasks14b`.
- [Audio15](Audio15/README.md): radio de usuario y grupos de animaciones (lista de 15). 30 funciones y 11.008 pares. Caso `audio15`.
- [World15](World15/README.md): ciclo de población y sprites (lista de 15). 30 funciones y 17.312 pares. Caso `world15`.
- [CTasks15](CTasks15/README.md): evitar peatones al deambular, trepar y agacharse (lista de 15). 45 funciones y 19.712 pares. Caso `ctasks15`.
- [Anim16](Anim16/README.md): jerarquías y nodos de animación (lista de 16). 32 funciones y 14.080 pares. Caso `anim16`.
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
- [Audio17](Audio17/README.md): audio de colisiones (lista de 17). 17 funciones y 11.088 pares. Caso `audio17`.
- [FixAudio](FixAudio/README.md): arreglos de audio fuera de listas: `UpdatePlayTime`, `GetPlayTime`/`GetLength` de 32 bits y `UpdateParameters`. 7 funciones y 5.120 pares. Caso `fixaudio`.
- [FixCore](FixCore/README.md): arreglos de núcleo fuera de listas: `CMatrix::Rotate*`/`SetRotate*`, `CPool`, `CColBox::operator=` y contadores del búfer temporal. 17 funciones y 11.776 pares. Caso `fixcore`.
- [FixPeds](FixPeds/README.md): arreglos de peatones y grupos fuera de listas: virtuales y cuerpo de `CPedAttractor`, `FlushTasks` y `CancelPlayerEnteringCars` como thiscall. 21 funciones y 9.664 pares. Caso `fixpeds`.
- [Sys17](Sys17/README.md): gestor de ficheros y cadenas IK (lista de 17). 34 funciones y 13.248 pares. Caso `sys17`.
- [World16](World16/README.md): coberturas y CPlaceable (lista de 16). 32 funciones y 19.680 pares. Caso `world16`.
- [FixHooks](FixHooks/README.md): hooks duplicados y virtuales con hook plano pasados a hooks de vtable. 20 funciones y 6.144 pares. Caso `fixhooks`.
- [Tasks18](Tasks18/README.md): gestor de tareas (lista de 18). 18 funciones y 12.160 pares. Caso `tasks18`.
- [Audio18](Audio18/README.md): sonidos `CAESound` (lista de 18). 16 funciones y 9.216 pares. Caso `audio18`.
- [Group18](Group18/README.md): miembros de grupos de peatones (lista de 18). 18 funciones y 10.112 pares. Caso `group18`.
- [World18](World18/README.md): gestor de entradas y salidas (lista de 18). 17 funciones y 7.808 pares. Caso `world18`.
- [Plant19](Plant19/README.md): gestor de plantas (lista de 19). 19 funciones y 10.688 pares. Caso `plant19`.
- [Game19](Game19/README.md): Darkel y grafitis (lista de 19). 38 funciones y 17.792 pares. Caso `game19`.
- [Gun19](Gun19/README.md): usar arma `CTaskSimpleUseGun` (lista de 19). 18 funciones y 9.920 pares. Caso `gun19`.
- [Route19](Route19/README.md): seguir ruta de nodos (lista de 19). 19 funciones y 11.584 pares. Caso `route19`.
- [Render19](Render19/README.md): cámara de sombras y cristales (lista de 19). 37 funciones y 9.920 pares. Caso `render19`.
- [Anim20](Anim20/README.md): asociaciones de animación (lista de 20). 20 funciones y 10.368 pares. Caso `anim20`.
- [Time20](Time20/README.md): ciclo horario (lista de 20). 20 funciones y 7.488 pares. Caso `time20`.
- [IK20](IK20/README.md): cadenas IK (lista de 20). 19 funciones y 11.264 pares. Caso `ik20`.
- [Audio20](Audio20/README.md): flujo de datos y audio de armas (lista de 20). 24 funciones y 10.176 pares. Caso `audio20`.
- [Event20](Event20/README.md): evento de daño `CEventDamage` (lista de 20). 20 funciones y 6.976 pares. Caso `event20`.
- [FixBugs](FixBugs/README.md): `FIX_BUGS` quitados en funciones validadas (comportamiento del EXE). 21 funciones y 10.304 pares. Caso `fixbugs`.
- [WeapAudio](WeapAudio/README.md): constructores de `CAEWeaponAudioEntity` y `CAEPedWeaponAudioEntity` (catálogo corregido). 2 funciones y 1.024 pares. Caso `weapaudio`.
- [Buoy9](Buoy9/README.md): flotabilidad `cBuoyancy` (lista de 9). 9 funciones y 6.656 pares. Caso `buoy9`.
- [Garages21](Garages21/README.md): garajes `CGarages` (lista de 21). 21 funciones y 7.936 pares. Caso `garages21`.
- [Zones21](Zones21/README.md): zonas y zonas de culling `CCullZones`/`CTheZones` (lista de 21). 42 funciones y 14.352 pares. Caso `zones21`.
- [ColStore23](ColStore23/README.md): almacén de colisiones `CColStore` (lista de 23). 23 funciones y 10.752 pares. Caso `colstore23`.
- [Ps22](Ps22/README.md): capa de plataforma `Ps` (lista de 22). 22 funciones y 6.720 pares. Caso `ps22`.
- [Game22](Game22/README.md): `CGame` (lista de 22). 22 funciones y 5.824 pares. Caso `game22`.
- [Clump24](Clump24/README.md): modelos de clump `CClumpModelInfo` (lista de 24). 24 funciones y 12.256 pares. Caso `clump24`.
- [Interior21](Interior21/README.md): gestor de interiores `InteriorManager_c` (lista de 21). 21 funciones y 10.112 pares. Caso `interior21`.
- [RwHelper24](RwHelper24/README.md): ayudantes de RenderWare (lista de 24). 24 funciones y 11.328 pares. Caso `rwhelper24`.
- [CarCtrl22](CarCtrl22/README.md): control de coches `CCarCtrl` (lista de 22). 22 funciones y 10.304 pares. Caso `carctrl22`.
- [Fire24](Fire24/README.md): gestor de fuegos `CFireManager` (lista de 24). 24 funciones y 9.344 pares. Caso `fire24`.
- [Clouds21](Clouds21/README.md): nubes `CClouds` (lista de 21). 20 funciones y 8.384 pares. Caso `clouds21`.
- [Boat23](Boat23/README.md): barcos `CBoat` (lista de 23). 23 funciones y 12.288 pares. Caso `boat23`.
- [EnExInit](EnExInit/README.md): `CEntryExitManager::Init` como el EXE (400 entradas), `Save` y `Shutdown`. 3 funciones y 2.048 pares. Caso `enexinit`.
- [PGI26](PGI26/README.md): inteligencia de grupos de peatones `CPedGroupIntelligence` (lista de 26). 25 funciones y 10.688 pares. Caso `pgi26`.
- [Txd26](Txd26/README.md): almacén de TXD `CTxdStore` (lista de 26). 24 funciones y 8.064 pares. Caso `txd26`.
- [Fx25](Fx25/README.md): gestor de efectos `FxManager_c` (lista de 25). 23 funciones y 14.336 pares. Caso `fx25`.
- [CarAI25](CarAI25/README.md): IA de coches `CCarAI` (lista de 25). 25 funciones y 11.520 pares. Caso `carai25`.
- [Light27](Light27/README.md): luces `Light` de RenderWare (lista de 27). 27 funciones y 7.936 pares. Caso `light27`.
- [Rs27](Rs27/README.md): capa `Rs` de RenderWare (lista de 27). 27 funciones y 6.464 pares. Caso `rs27`.
- [Damage27](Damage27/README.md): gestor de daños `CDamageManager` (lista de 27). 27 funciones y 7.744 pares. Caso `damage27`.
- [GameLogic27](GameLogic27/README.md): lógica de juego `CGameLogic` (lista de 27). 27 funciones y 13.888 pares. Caso `gamelogic27`.
- [Wanted33](Wanted33/README.md): nivel de búsqueda `CWanted` (lista de 33). 33 funciones y 15.616 pares. Caso `wanted33`.
- [Sprite2d33](Sprite2d33/README.md): sprites 2D `CSprite2d` (lista de 33). 33 funciones y 11.136 pares. Caso `sprite2d33`.
- [Stats32](Stats32/README.md): estadísticas `CStats` (lista de 32). 32 funciones y 13.056 pares. Caso `stats32`.
- [Player28](Player28/README.md): información del jugador `CPlayerInfo` (lista de 28). 24 funciones y 7.552 pares. Caso `player28`.
- [VehRec29](VehRec29/README.md): grabaciones de vehículos `CVehicleRecording` (lista de 29). 29 funciones y 11.136 pares. Caso `vehrec29`.
- [AnimMgr31](AnimMgr31/README.md): gestor de animaciones `CAnimManager` (lista de 31). 31 funciones y 11.072 pares. Caso `animmgr31`.
- [Shop30](Shop30/README.md): tiendas `CShopping` (lista de 30). 30 funciones y 9.024 pares. Caso `shop30`.
- [Renderer33](Renderer33/README.md): renderizador `CRenderer` (lista de 33). 33 funciones y 18.112 pares. Caso `renderer33`.
- [MFDec](MFDec/README.md): `CAEMFDecoder` con QuickTime como el EXE. 10 funciones y 4.096 pares. Caso `mfdec`.
- [GameIdle](GameIdle/README.md): `Game::Idle` y `FrontendIdle` como el EXE (limitador de fotogramas incluido). 2 funciones y 2.048 pares. Caso `gameidle`.
- [Model34](Model34/README.md): modelo base `CBaseModelInfo` (lista de 34). 33 funciones y 13.280 pares. Caso `model34`.
- [Cutscene35](Cutscene35/README.md): cinemáticas `CCutsceneMgr` (lista de 35). 35 funciones y 13.184 pares. Caso `cutscene35`.
- [AudioHw54](AudioHw54/README.md): hardware de audio `CAEAudioHardware` (lista de 54). 53 funciones y 9.280 pares. Caso `audiohw54`.
- [EnvMap37](EnvMap37/README.md): tubería de reflejos de coches `CCustomCarEnvMapPipeline` (lista de 37). 37 funciones y 10.880 pares. Caso `envmap37`.
- [PedSpeech58](PedSpeech58/README.md): Voz de peatón (CAEPedSpeechAudioEntity, lista de 34) Caso `pedspeech58`.
- [PedIntel52](PedIntel52/README.md): CPedIntelligence validada por pares contra el EXE. Caso `pedintel52`.
- [Surface47](Surface47/README.md): SurfaceInfos_c validada por pares contra el EXE. Caso `surface47`.
- [PlayerPed51](PlayerPed51/README.md): CPlayerPed validada por pares contra el EXE. Caso `playerped51`.
- [CtrlConfig58](CtrlConfig58/README.md): CControllerConfigManager validada por pares contra el EXE. Caso `ctrlconfig58`.
- [RpAnim43](RpAnim43/README.md): RpAnimBlend validado por pares contra el EXE. Caso `rpanim43`.
- [Replay47](Replay47/README.md): CReplay validada por pares contra el EXE. Caso `replay47`.
- [Object48](Object48/README.md): CObject validada por pares contra el EXE. Caso `object48`.
- [Physical56](Physical56/README.md): CPhysical validada por pares contra el EXE. Caso `physical56`.
- [FileLoader47](FileLoader47/README.md): CFileLoader validada por pares contra el EXE. Caso `fileloader47`.
- [ColGeom59a](ColGeom59a/README.md): Geometría pura de CCollision (25 funciones) validada contra el EXE. Caso `colgeom59a`.
- [ColWorld59c](ColWorld59c/README.md): Colisión de mundo y cámara de CCollision (20 funciones) validada contra el EXE. Caso `colworld59c`.
- [ColProc59b](ColProc59b/README.md): Procesado de colisión de CCollision (13 funciones) validado contra el EXE. Caso `colproc59b`.
- [BeCop60](BeCop60/README.md): CTaskComplexBeCop reconstruida (Clone) y validada contra el EXE. Caso `becop60`.
- [DragOut60](DragOut60/README.md): CTaskComplexCarSlowBeDraggedOutAndStandUp reconstruida (6 funciones) y validada contra el EXE. Caso `dragout60`.
- [CTasks61b](CTasks61b/README.md): Nueve tareas (WanderCriminal, WanderProstitute, EvasiveCower, EvasiveStep, BeInCouple, SignalAtPed, CrossRoad, DestroyCar, Choking) reconstruidas y validadas contra el EXE. Caso `ctasks61b`.
- [CTasks61c](CTasks61c/README.md): Ocho clases (GoToPoint*, GotoDoor, Aiming, atractores, RunNamedAnim, WalkRoundCar) reconstruidas y validadas contra el EXE. Caso `ctasks61c`.
- [Misc61a](Misc61a/README.md): Scripts de cerebros, grupos de peatones, clima y cuerdas (CScriptsForBrains, CPedGroup, CWeather, CRopes) reconstruidos y validados contra el EXE. Caso `misc61a`.
- [CTasks61a](CTasks61a/README.md): Nueve tareas de coche (CarDrive, CarSetPedOut/In, DriveToPoint, EnterCarAsPassengerWait, OpenPassengerDoor, LeanOnVehicle) reconstruidas y validadas contra el EXE. Caso `ctasks61a`.
- [Misc61b](Misc61b/README.md): CBmx, CQuadBike, CCarFXRenderer y CStencilShadows reconstruidos y validados contra el EXE. Caso `misc61b`.
- [StencilReg62](StencilReg62/README.md): CStencilShadows::RegisterStencilShadows validada contra el EXE con pool de vehículos y sectores falsos. Caso `stencilreg62`.
- [VehCtor62](VehCtor62/README.md): Constructores de CBmx y CQuadBike validados contra el EXE (bases simuladas). Caso `vehctor62`.
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

- [CTaskDead](CTaskDead/README.md): CTaskSimpleDead, seis funciones y 1.792 pares exactos; dependencias controladas y estado restaurado. Caso `ctaskdead`.

- [PlayerFighterPending](PlayerFighterPending/README.md): CTaskSimplePlayerOnFoot::PlayerControlFighter, 4.096 pares exactos; dependencias controladas y estado restaurado. Caso `player-fighter-pending`.

- [IdleAttractorPending](IdleAttractorPending/README.md): CIdleCam::ProcessFOVZoom y CPedAttractor::AbortPedTasks, 2.048 pares exactos; dependencias controladas y estado restaurado. Caso `idle-attractor-pending`.

- [FightingControlPending](FightingControlPending/README.md): CTaskSimpleFightingControl::ProcessPed, 1.024 pares exactos y gate de once ramas; dependencias controladas y estado restaurado. Caso `fighting-control-pending`.

- [RoadBlockPending](RoadBlockPending/README.md): CRoadBlocks::CreateRoadBlockBetween2Points, 1.024 pares exactos y gate de nueve ramas; dependencias controladas y estado restaurado. Caso `roadblock-pending`.

## Lote de 78 pendientes (07/10/2026)

[Reconstrucción y validación del lote](PendingBatch/README.md): 78 funciones de 27 clases, 30 bancos y 81.664 pares EXE/DLL. Se conservan las limitaciones de cada banco; no acredita validación completa de todas las funciones de esas clases. Las fuentes finales se recompilan con /MP6 y se comprueba la restauración del juego.

<!-- USER LISTS STOP STATUS -->
Estado de las listas solicitadas al detener el trabajo: [reconstrucción y validación parcial](../docs/EstadoListasSolicitadas.md). Las 67 clases tienen la reconstrucción solicitada aplicada; el último lote quedó con 117/354 funciones verificadas con las fuentes finales y un fallo pendiente en el destructor de GangFollower.
<!-- /USER LISTS STOP STATUS -->
