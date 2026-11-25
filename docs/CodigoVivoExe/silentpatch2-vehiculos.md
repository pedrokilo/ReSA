# SilentPatch, segunda tanda: vehículos (rama `silentpatch2-vehiculos`)

Fecha: 25/11/2026. Base: `integracion-fase7b` (`09d31673c6`). Licencia y atribución: SilentPatch (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent)); ver [../THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md). Los arreglos son desviaciones deliberadas y opcionales del EXE: con `GTASA_SILENTPATCH=0` (o `SilentPatch=0` en `gta-reversed.ini`) el código antiguo se conserva literal (rama «opción apagada»). Una opción suelta se apaga con `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0` o `SilentPatch_<Nombre>=0`. Ninguna tiene clave en `SilentPatchSA.ini` (que solo define `SunSizeHack`, `TrueInvincibility`... y las listas `RotorFixExceptions`, `DoubleRearWheels`, `LightbeamFixExceptions`): todas van activadas, y la lista `RotorFixExceptions` por defecto (`maverick`, `skimmer`) se respeta en el código.

* Clasificación fila a fila (todas las viñetas de vehículos que he encontrado leyendo las 210 líneas): [silentpatch-clasificacion/vehiculos.csv](silentpatch-clasificacion/vehiculos.csv).
* Código nuevo: `source/game_sa/Entity/Vehicle/SilentPatchVehicles.h/.cpp` (tabla SVF, ruedas de explosión, reparación, animaciones extra, matrículas, rotores). El resto son hunks mínimos con el comentario `// SilentPatch (MIT, Silent): ...` en el sitio.
* Filas de [../DesviacionesConocidas.md](../DesviacionesConocidas.md), sección 15.4.

## El «SVF» (características especiales por modelo)

SilentPatch lo guarda en un `multimap` rellenable por ini y por otros mods. Aquí es una tabla estática de ids de modelo (`kStockFeatures`), solo con los vehículos de serie de SA:

| Modelo | Id | Característica | Uso |
| --- | --- | --- | --- |
| Rhino | 432 | `RHINO_WHEELS` | `CAutomobile::Fix` devuelve las ruedas intermedias a su estado |
| Beagle | 511 | `EXTRA_AILERONS1` | alerones extra copian la rotación (nodos 18->21) |
| Stuntplane | 513 | `EXTRA_AILERONS2` | nodos 19->23 y 20->24 |
| Towtruck | 525 | `TOWTRUCK_HOOK` | `Fix` conserva el panel del gancho (nodo 21) |
| Tractor | 531 | `TRACTOR_HOOK` | ídem (nodo 17) |
| Vortex | 539 | `VORTEX_EXHAUST` | `CPlane::Fix` conserva el primer panel |
| Firela | 544 | `FIRELA_LADDER` | escalera controlable y cámara sin alfa |
| Sweeper | 574 | `SWEEPER_BRUSHES` | cepillos giran con el motor encendido |
| Newsvan | 582 | `NEWSVAN_DISH` | antena giratoria |
| artict3 | 591 | `DOUBLE_TRAILER` | enganche trasero (nodo 21) |
| Phoenix | 603 | `PHOENIX_FLUTTER` | tomas de aire del capó |

No se portan el registro por ini ni por nombre de modelo (`RegisterSpecialVehicleFeature` / `DoubleRearWheels=`): no hay lectura de ini de modelos en el standalone.

## Opciones de este área

Todas con valor por defecto 1 (activadas).

| Opción | Viñetas | Qué cambia |
| --- | --- | --- |
| `ExplosionPanelsSwing` | L52 | `FuckCarCompletely` ya no pone a 0 el estado de los paneles |
| `ExplodedCarWheels` | L102 | rango de rueda perdida -4 y separación/ocultación de las ruedas que marca el DamageManager (coche, helicóptero, avión) |
| `CarCollisionDamagePieces` | L67 | `SetDamagedPieceRecord` usa la pieza B con distancia <= 0 |
| `JetThrustersFix` | L115 | banderas del `RwMatrix` temporal de los propulsores del Hydra a 0 |
| `HunterDoorRenderFix` | L116 | `ATOMIC_LEFT` fuera de `door_lf` de los vehículos con animaciones del Rustler |
| `RhinoLights` | L151 | dos comparaciones con 432 desactivadas (luces y haces) |
| `DetachedPartsColour`, `DetachedPartsBothSides` | L153, L154 | color (SetEditableMaterials con restauración) y luz direccional de las piezas sueltas; sin culling |
| `ExtraVehicleAnimations` | L156 | Phoenix, Sweeper, Newsvan y alerones extra |
| `HierarchyTypoFix` | L156, L161-L163 | erratas de jerarquía aceptadas |
| `EngineComponentsOnlyWhenOn` | L157 | paso de tiempo 0 en los componentes del motor con el motor apagado |
| `FirelaLadder` | L158 | escalera del Firela |
| `DoubleTrailer` | L159 | remolques artict3 encadenables |
| `TugTowBar` | L160 | barra de remolque del Tug (nodo 20 o 21) |
| `BlownUpVehicleShine` | L169 | vehículo destrozado y luz direccional sin iluminar átomos: sin brillo ni reflejos |
| `CleanDirt` | L170 | lista de materiales de suciedad (el EXE 1.0 no la rellenaba) |
| `UniqueNumberPlates`, `BilinearNumberPlates` | L171-L174 | matrícula por vehículo, también en motos; filtro bilineal |
| `VehicleLightsStayLit`, `HeadlightBeamStateRestore`, `VehicleLightsNotOverwritten` | L175, L177, L179 | restauración de propiedades de superficie, de estados de dibujo del haz y del índice de luz D3D |
| `FixKeepsComponents` | L189-L192, L201 | `Fix` conserva las matrices originales, el gancho de la grúa, las puertas y reinicia las ruedas del Rhino |
| `PaintjobsInGarageFix` | L204 | pintura pendiente en `GetRemapIndex` |
| `RotorBlur` | L205 | rotores y hélices con desenfoque (salvo Maverick y Skimmer) |
| `DoubleRearWheels` | L206 | ruedas dobles en remolques y matriz nueva sin marca de identidad |
| `CargenCounterFix`, `CargenInteriorAreaCode` | L60, L74 | contador de generadores y código de área de interiores |
| `RCVehicleStreaming` | L76 | el RC cuenta como jugador en `ConstructRenderList` |
| `RiotHeliIgnoresPlayerGroup` | L105 | helicóptero de policía no apunta al grupo del jugador en misiones |
| `QuadbikeWindAnims`, `QuadbikeHandlebarAnims`, `RadioChangeAnim` | L89-L92 | cuatrimoto y animación de la radio |
| `BikeJumpOnFromFront`, `VanRearDoors` | L117, L118 | posición Z al subir a la moto; grupo de animación de furgonetas |
| `CollisionDataInit` | L16 | datos de colisión sin basura |

## Decisiones y cosas que conviene saber

* **Una sola edición para varias viñetas.** L153/L154 son la misma función (`Render_DetachedPartRenderingFix`); la clasificación anterior daba L154 a gráficos. Está hecha aquí, con una sola edición en `CObject::Render`. L161-L163 y la parte de barcos/Dodo de L156 son la tabla de erratas de `FindFrameFromNameWithoutIdCB`.
* **Matrículas.** El EXE 1.0 creaba una matrícula en `SetClump` y todos los coches de un modelo compartían textura y diseño. SilentPatch deja un marcador no nulo en `m_pPlateMaterial` (para que sigan valiendo las comprobaciones del generador de coches), genera la textura en cada vehículo y aplica `carplate`/`carpback` solo mientras se dibuja (también motos). El byte de diseño se guarda en el relleno anterior a `m_pCustomCarPlate`. Una diferencia con SilentPatch: la textura de la matrícula usa el mismo diseño que la pieza `carpback` (SilentPatch usaba el tipo del modelo, -1 = región, y podía no coincidir en el 5 % de los coches). La aleatoriedad del diseño usa un generador local para no consumir la secuencia del juego, pero el texto de la matrícula sí llama a `GeneratePlateText` del juego (como SilentPatch): cada vehículo creado consume 8 llamadas a `rand`.
* **Suciedad.** En el EXE 1.0 `m_apDirtMaterials` nunca se rellena (la 1.01 sí): `SetDirtTextures` recorría 16 punteros nulos y los coches nunca se limpiaban ni ensuciaban visualmente. Se rellena en `SetClump` con los materiales `vehiclegrunge256` del clump y de los extras (hasta 32).
* **Rotores.** Se respeta `RotorFixExceptions` de `SilentPatchSA.ini` por defecto (Maverick 487 y Skimmer 460 conservan el comportamiento del EXE).
* **Lo que no se portó de la zona de luces de SilentPatch**: la anulación del difuso/ambiente de `g_GameLight` (0x5D88D1, «no incluir un D3DLIGHT extra en vehículos») va con el arreglo de luz direccional de gráficos; las excepciones de modelo del haz (`LightbeamFixExceptions`) no existen sin ini.
* **Fuera del changelog.** Hay en SilentPatch un arreglo del daño de luces traseras (`LEFT_TAIL_LIGHT` en lugar de `RIGHT_TAIL_LIGHT`, 0x6E1D4E) sin viñeta en `CHANGELOG-SA.md`; no se ha portado.

## Estado de prueba

* **Compilación**: los 22 `.cpp` modificados y `SilentPatchVehicles.cpp` compilan (rc=0) con `compilar_over.cmd` bajo `compile_lock`; ASI (`reversing/FnFase8/asi/gta_reversed.asi`) y standalone (`reversing/FnFase8/salida/gta_sa_standalone.exe`) enlazan con los `.obj` de `wv-int` sin símbolos sin resolver. `contrib/audit_hook_catalog.py --strict`: OK. `tamanos_objetos.py lint`: OK (0 fallos).
* **Humo con la ASI de esta rama** (desde `wv-8/reversing/FnBIS1`, `AGUA_ASI`): `prueba_ropero.py cal_mal.args` VEREDICTO BIEN (entidades 15, marcadores 12, puerta y marcador); `prueba_marcadores.py cal_mal.args` VEREDICTO BIEN (12 marcadores en memoria, 12 cambian en 1 s). Hubo varios «sin juego»/«el juego se cerró» por interferencia con otros agentes (candado del juego tomado) y se repitieron hasta obtener BIEN.
* **Ejecución del standalone** (escenario `sp2veh`, 220 s de juego con la opción encendida y 215 s con `GTASA_SILENTPATCH=0`): sin cierres, `codigo-original-distinto=0`; el contador `excepciones` vale 1 desde el segundo 2 del arranque y no cambia en toda la ejecución, tampoco con SilentPatch apagado (no la causa esta rama). `ejecutar_traf.ps1` (120 s) no se pudo usar: falta `D:\Original\_trafico_run\gta-reversed.ini.partida2`; el escenario sp2veh lo sustituye.
* **Ejercitado con evidencia** (escenario nuevo `sp2veh` en `reversing/FnJuego/sonda_juego.py`: 6 rondas de coches generados por trucos + BIGBANG, estado leído de la memoria del DamageManager): con SilentPatch encendido 36 coches destruidos, ruedas perdidas [delantera-izq, trasera-izq, delantera-der, trasera-der] = [11, 8, 9, 8] y 36/36 con paneles dañados; apagado 30 coches, [10, 11, 9, 0] y 0/30. Es decir, L102 (la trasera derecha ahora se pierde) y L52 (los paneles ya no se reparan al instante) se observan por memoria. Capturas (Hydra, Rhino, voladuras) en `reversing/FnJuego/ejecuciones/sp2veh_on-*` y `sp2veh_off-*`.
* **En el log del juego** (`log.log` de `sp2veh_on-20261125-060057`) aparecen leídas, sin fallos, `CollisionDataInit`, `HierarchyTypoFix`, `DoubleRearWheels`, `HunterDoorRenderFix`, `CleanDirt`, `UniqueNumberPlates`, `BilinearNumberPlates`, `RhinoLights`, `ExtraVehicleAnimations`, `RotorBlur`, `CargenCounterFix`, `CargenInteriorAreaCode`, `FirelaLadder`, `BlownUpVehicleShine`, `VehicleLightsNotOverwritten`, `VanRearDoors`, `DetachedPartsColour`, `DetachedPartsBothSides`, `ExplodedCarWheels`, `ExplosionPanelsSwing`, `QuadbikeHandlebarAnims`, `QuadbikeWindAnims`, `VehicleLightsStayLit`, `HeadlightBeamStateRestore`, `RCVehicleStreaming`, `CarCollisionDamagePieces`, `VehicleExplodeTwice`: la ruta de código se recorrió con la ASI/standalone enlazados, pero su efecto visual (matrículas distintas, suciedad, rotores borrosos, luces) **no se midió**; las capturas están, sin comparación automática.
* **No ejercitado**: L90 y L115 (cuatrimoto en marcha, Hydra en vuelo con captura de los propulsores), L105, L158-L163 (Firela, Tug, artict3, erratas: hace falta cargar esos modelos en el juego y mirarlos), L189-L192 y L201 (reparar: no hay sonda que repare un vehículo), y todo lo demás de la tabla marcado «implementado+compilado».
* **No comparado con el EXE original puro** (`_comparar/orig`): no se lanzó, por la cola del candado del juego (la comparación apagado/encendido sí se hizo con el mismo standalone).

## Bloqueado

* **L84 (motos sin motor)**: no se localizó en el código de SilentPatch (se buscó `bike`, `Bike`, `engine`, `bEngine` y direcciones 0x6B7xx-0x6BFxx en `SilentPatchSA.cpp`, `VehicleSA.cpp`, `ModelInfoSA.cpp`, `ScriptSA.cpp`, `GeneralSA.cpp` y `Common.cpp`). Haría falta el historial de git de SilentPatch (el commit de esa viñeta) para saber qué rutina toca; no se ha inventado el arreglo.
* **L28, L75, L186, L203** (trenes/bombas/impound al ser arrestado/Pay n Spray): son de `Garage*.cpp`/`StoredCar*.cpp`/`GameLogic`, que no se editan aquí (agente de garajes).
* **L135, L136, L202** (frases): agente de voces. **L167** (armas a través de la ventana): gráficos.
