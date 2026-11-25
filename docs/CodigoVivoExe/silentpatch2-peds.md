# SilentPatch, segunda tanda: peds, jugador, animaciones y comportamiento de personajes (rama `silentpatch2-peds`)

Fecha: 25/11/2026. Rama `silentpatch2-peds` (worktree `D:\Original\wv-spp`), base `integracion-fase7b` (09d31673c6). Código de SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent)) leído de `D:\Original\ref_mods\silentpatch\SilentPatch-dev` (solo lectura); de ahí solo se tomó lo mínimo (constantes, secuencias, el generador `rand31`) y se adaptó al C++ propio de `source/game_sa/` (sin `__asm`, sin archivos nuevos por clase salvo el banco de pruebas y `extensions/SilentPatchRandom.h`). Licencia: [../THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md). Tabla de desviaciones: sección 15.4 de [../DesviacionesConocidas.md](../DesviacionesConocidas.md). Clasificación fila por fila: [silentpatch-clasificacion/peds.csv](silentpatch-clasificacion/peds.csv) (51 filas).

Esquema de opciones (el de `source/extensions/SilentPatchConfig.h`): `GTASA_SILENTPATCH=0|1` o `SilentPatch=` en `gta-reversed.ini` para todo; por arreglo `GTASA_SILENTPATCH_<NOMBRE>=0|1` o `SilentPatch_<Nombre>=`. Ninguno de estos arreglos existe en `SilentPatchSA.ini`, así que todos van ACTIVADOS por defecto; ninguno cambia el ritmo del juego. Con el interruptor general apagado cada sitio ejecuta el código del EXE (conservado literal en la rama de la opción apagada).

## Resumen

* 24 viñetas asignadas (más 27 de contexto, por el grep amplio del tema) leídas una a una. Implementadas en esta tanda: L60, L61, L62, L74, L76, L77, L89, L90, L91, L92, L94, L104, L105, L117, L118, L120, L121, L123, L124, L202 y la ampliación de L51 (GIVE_WEAPON_TO_CHAR); L23 por evidencia indirecta (retraso de la destrucción de objetos sujetos).
* Verificadas contra el desensamblado y ya hechas por la primera tanda o por otras ramas: L31 (territorios), L83 y L93 (reinicio de variables y de pickups/generadores de IPL de texto en Nueva Partida), L188 (ratón en el aire), L195-L198.
* Bloqueada: **L82** (animaciones del paracaídas): la copia de SilentPatch no contiene ningún cambio de paracaídas (`grep -i "parachut|PARA"` solo da la munición del truco, `RenderWeapon` y el pipeline de edificios para objetos skinned), así que no hay referencia del defecto. Hipótesis no verificable: lo cubre `RtAnimInterp` (L50), porque el paracaídas es un objeto skinned animado.
* Fuera de mi área (no tocadas): L24, L25 (replay), L30 (`FxMemoryPool_c::Optimise`, gráficos), L84, L193, L194; voces (L125-L146) las lleva otro agente.

## Hallazgos de localización (EXE 1.0 US)

Algunas direcciones del primer inventario eran hipótesis (no estaban en la copia de SilentPatch como dirección literal). Se localizaron con el desensamblado (`gta_sa.exe` original, capstone) y con los parches 1.0 de `SilentPatchSA.cpp:7045-8605`:

| Viñeta | Dirección | Qué hace el EXE | Qué hace SilentPatch |
| --- | --- | --- | --- |
| L117 | 0x64FFAD (`CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor`, caso puerta 18) | `fld [esp+18h]; fadd [esp+24h]`: Z = asiento.z + desplazamiento.z | `fadd` -> `fsub` (D8 44 24 24 -> D8 64 24 24) |
| L118 | 0x64E77B+6 (`CCarEnterExit::GetPositionToOpenCarDoor`) | compara el grupo de animación del handling con 0x65 (101, id ABSOLUTO) | compara con 13 (= `ANIM_VEH_VAN - ANIM_VEH_STD`): el handling guarda el grupo relativo |
| L120 | 0x4613C2 (`CRoadBlocks::GenerateRoadBlockPedsForCar`) | tras el bucle de pandillas compara el modelo con -1 en vez de con 0; en una zona sin pandillas queda el valor inicial 0 = CJ | `cmp X, 0 / jg` |
| L121 | 0x5D3DA8 (`CTagManager::Load`) | llama a 0x5D1270 (guardar) en vez de a 0x5D1300 (cargar) para el contador | llama a la función de cargar |
| L123 | 0x588581, 0x56E2A4, 0x56E2CB | blip y centro del mundo del jugador 2 con el índice del jugador en foco | índice del jugador pedido |
| L60 | 0x6F3E2C (`CCarGenerator::DoInternalProcessing`) | `movzx` + `jge -1`: el contador nunca se decrementa | `movsx` + `jz` |

## Qué se implementó

Todas las rutas son relativas a `source/game_sa/`; cada sitio lleva el comentario `// SilentPatch (MIT, Silent): <arreglo>`.

| L | Opción | Archivo y función | Efecto |
| --- | --- | --- | --- |
| 60 | `CarGenCounterFix` | `CarGenerator.cpp` DoInternalProcessing | el contador de generadores con n fijo se decrementa (salvo 0xFFFF = infinito) |
| 74 | `CarGenInteriors` | `CarGenerator.cpp` DoInternalProcessing | vehículo de un generador con z >= 950: área 13 |
| 61 | `ConsoleRandomness` | `Pathfind.cpp` GeneratePedCreationCoors y _Interior; `extensions/SilentPatchRandom.h` | rand de 31 bits (PS2) en la elección de nodos de peatones |
| 62 | `ConsoleRandomness` | `Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp` | idem en la solicitud (los umbrales 0x1FFFFFFF/0xFFFFFFF suponían 31 bits) |
| 76 | `RCStreaming` | `Renderer.cpp` ConstructRenderList | con vehículo RC, el streaming/LOD usa el vehículo remoto (`FindPlayerEntityWithRC` y `FindPlayerVehicle(-1, true)`) |
| 77 | `RecruitTriangleFix` | `Entity/Ped/PlayerPed.cpp` DrawTriangleForMouseRecruitPed | triángulo orientado para GANG2 (reclutables) |
| 89 | `QuadbikeWind` | `Entity/Ped/Ped.cpp` PreRenderAfterTest | el viento afecta a la ropa de CJ en el quad |
| 90 | `QuadbikeHandlebarAnims` | `Entity/Vehicle/Bike.cpp` ProcessRiderAnims | marcha atrás, drive-by o velocidad < 0.02 anulan la inclinación lateral (el stub de SilentPatch en 0x6B7932 traducido a variables) |
| 91, 92 | `RadioStationChangeAnimBlending` | `Entity/Vehicle/Vehicle.cpp` ProcessDrivingAnims (`RadioTuneAnimDisableBones`) | solo cabeza y brazo derecho; no parcial en barcos de pie; sin animación en el Kart |
| 94 | `CameraCrosshairFix` | `Hud.cpp` DrawCrossHairs | mira de la cámara con la mira de script 2 |
| 104 | `JetpackKeyboardControlsHover` | `Tasks/TaskTypes/TaskSimpleJetPack.cpp` ProcessControlInput | Q + E flotan con teclado y ratón |
| 105 | `RiotDontTargetPlayerGroup` | `Entity/Vehicle/Plane.cpp` DoPlaneGenerationAndRemoval | el helicóptero de policía no elige al grupo del jugador en misiones |
| 117 | `BikeFrontEnterZ` | `Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp` | Z restada en la puerta 18 |
| 118 | `VanRearDoorFix` | `CarEnterExit.cpp` GetPositionToOpenCarDoor | grupo 13 relativo |
| 120 | `RoadblockNoCJClones` | `RoadBlocks.cpp` GenerateRoadBlockPedsForCar | sin clones de CJ (la reconstrucción ya los evitaba; con la opción apagada se reproduce el defecto del EXE) |
| 121 | `TagManagerLoadFix` | `TagManager.cpp` Load | lee el contador de graffitis del guardado |
| 123 | `SecondPlayerBlipFix` | `World_Helper.cpp` FindPlayerCentreOfWorld; `Radar.cpp` DrawBlips | blip/posición del jugador 2 |
| 124 | `GangWarKeepGroupBlips` | `GangWars.cpp` ReleasePedsInAttackWave | no se quitan los blips del grupo del jugador |
| 202 | `PedHitVoiceOnKill` | `Entity/Ped/Ped.cpp` KillPedWithCar; `Entity/Vehicle/Automobile.cpp` VehicleDamage | el pasajero habla al atropellar, no al empujar |
| 23 | `DelayHeldEntityDestroy` | `Entity/Ped/Ped.cpp` DropEntityThatThisPedIsHolding | destrucción diferida de cigarrillos/botellas (`bRemoveFromWorld`) |
| 51 | `ScriptGiveWeaponAmmo` | `Scripts/RunningScriptCommands300To499.cpp` GIVE_WEAPON_TO_CHAR | munición 0 -> 1 |

Además: banco aislado `SilentPatchPedsSelfTest.cpp/.h` (solo con `GTASA_SILENTPATCH_SELFTEST_PEDS=1`, enganchado en `Idle`) y `reversing/FnJuego/ejecutar_sp2peds.ps1` (lanza el standalone con el candado `gta_lock`, espera a `SELFTEST FIN` y solo mata su propio proceso).

## Pruebas (standalone de la rama, `reversing\FnFase8\salida\gta_sa_standalone.exe`, instalación `D:\Original\_juego_run`)

Banco aislado en proceso (`logs/silentpatch_peds_selftest.log`): se ejecutó una vez con las opciones encendidas (`on4-20261125-054044`) y otra con `GTASA_SILENTPATCH=0` (`off4-20261125-054124`), ambas en `reversing/FnJuego/ejecuciones/`, con CJ en su casa (área 3, z 1014). El banco llama a las funciones reconstruidas con entradas controladas.

| L | Medida | Encendido | Apagado (= EXE) |
| --- | --- | --- | --- |
| 60 | contador de un generador con n=2 tras generar 1 coche | 1 | 2 (nunca decrece) |
| 74 | área del coche de un generador a z = 1014 | 13 | 0 |
| 120 | peds nuevos de `GenerateRoadBlockPedsForCar(pandilla)` en una zona sin pandillas | 0 | 2 clones de CJ (modelo 0) |
| 118 | `GetPositionToOpenCarDoor(Burrito, puerta 9).x` (m_nAnimGroup = 13, desplazamiento de asiento 0.2) | 0.1209 | 0.3209 |
| 117 | `GetLocalPositionToOpenCarDoor(puerta 18).z` (asiento.z = -0.1282, desplazamiento sintético 0.25) | -0.3782 | 0.1218 |
| 124 | `bClearRadarBlipOnDeath` de un ped del grupo del jugador tras `ReleasePedsInAttackWave(fin)` | 1 | 0 |
| 123 | `FindPlayerCentreOfWorld(1)` con el ped 2 en un vehículo | posición del vehículo | posición del ped |
| 61 | 1500 llamadas a `GeneratePedCreationCoors`: nodos distintos / índice máximo | 335 / 1853 | 91 / 1094 |
| 104 | `m_ThrustStop` tras `ProcessControlInput` con Q+E, ratón en 3ª persona, jetpack en el aire | 1 (flota) | 0 |
| 83 | `bPenaltyForDeathApplies/ArrestApplies` tras `BeforeReInit` | 1 / 1 | 0 / 0 |
| 93 | pickups usados tras `AfterReInit` (426 al empezar) | 430 (4 pickups de los IPL de texto) | 426 |
| 121 | `ms_numTags` tras cargar la partida | 100 | 100 |

Límites que hay que tener presentes:

* El banco no es una partida: no se ejecutó una Nueva Partida completa (L83/L93 se ejercitaron con las funciones de reinicio), ni cooperativo, ni un replay, ni disturbios, ni atropellos con pasajero, ni el Kart/quad/jetpack con teclado real. Esos arreglos están **implementados, no ejercitados en juego** (ver estados en el CSV).
* L60: con el contador a 2, la segunda pasada del banco quedó bloqueada por `CheckForBlockage` (efecto del escenario, no del arreglo), así que lo medido es el primer decremento (2 -> 1 frente a 2 -> 2), no la parada tras n coches.
* L117: las animaciones de subir a una moto no están cargadas en la casa (`ComputeAnimDoorOffsets` leería una asociación nula), así que el banco rellena el desplazamiento memorizado del grupo del Burrito con un valor sintético; la lógica comprobada es la de la suma frente a la resta.
* L121: con un guardado sin mods el arreglo no cambia nada (el contador en memoria ya es 100). El caso útil (etiquetas anadidas por mods) y el posible fallo que vio Pirulax sin el resto de SilentPatch no se ejercitaron: riesgo medio.
* No se comparó con el EXE original puro (el banco es código nuestro); «apagado» es la rama del EXE conservada literal.

## Compilación y verificación final

* **Compilación** (`reversing\FnFase8\compilar_over.cmd`, bajo `D:\Original\compile_lock`): los 20 archivos tocados de `source/game_sa/` (+ `source/app/app_game.cpp` y el banco `SilentPatchPedsSelfTest.cpp`) compilan con EXIT=0 (los .obj quedan en `reversing\FnFase8\over`, ignorado por git). Se compiló `SilentPatchPedsSelfTest.cpp` varias veces mientras se afinaba el banco y `Pathfind.cpp` otra vez al sacar la lectura de la opción del bucle de 300 intentos.
* **Enlace del standalone** (`enlazar.cmd` con los .obj de `D:\Original\wv-intuild\source\gta_reversed.dir\Debug`): `link exit=0`, 0 símbolos sin resolver (`link.log`: 0 coincidencias de LNK2019/LNK2001). ASI enlazada con el mismo método que las ramas anteriores (1194 objetos, 21 de `over`; `_enlazar_asi_spp.py` en `D:\Original`).
* `python contrib/audit_hook_catalog.py --strict`: **OK**. `python contrib/auditoria_datos/tamanos_objetos.py lint`: **OK** (2134 objetos declarados, 1975 mínimos verificados, 0 fallos).
* **Humo con la ASI de la rama** (`AGUA_ASI=D:\Original\wv-spp
eversing\FnFase8si\gta_reversed.asi`, desde `wv-8
eversing\FnBIS1`, `cal_mal.args`): `prueba_ropero.py` -> **VEREDICTO BIEN** (etiqueta `spp_rop5`, 06:48; puerta 2877 y marcador presentes, 15 entidades, 12 marcadores) y `prueba_marcadores.py` -> **VEREDICTO BIEN** (`spp_mar4`, 06:50; 12 marcadores en memoria, 12 cambian en 1 s). Salidas en `wv-8
eversing\FnBIS1\salidas\Xspp_rop5-*` y `Xspp_mar4-*`; resumen en `reversing/FnJuego/ejecuciones/humo_sp2peds_resumen.txt`. Intentos fallidos antes de esos dos: `ropero` cuatro veces «el juego se cerró» (el juego vivió ~5 s y salió con código 1) y `marcadores` tres («sin juego»); coinciden con arranques simultáneos de pruebas de otros agentes sobre la misma copia de pruebas (carpetas `Xspe_*`, `Xspo3_*`, `Xspv2*` creadas en el mismo segundo), y también uno (`spp_mar1`) con un reenlace mío de la ASI en marcha. La atribución a interferencia es una inferencia (no se aisló con una prueba aparte); los dos intentos que pasaron usan el mismo `.asi` (sha256 distinto al de los primeros intentos solo porque se reenlazó tras sacar la opción de `Pathfind.cpp` del bucle).
* **Ejecución del standalone sin excepciones**: `ejecutar_juego.ps1 -Escenario conduccion,armas -Limite 160` con el exe final (`reversing/FnJuego/ejecuciones/spp120c-20261125-065530`): 80 latidos hasta 158,8 s, todos con `excepciones=0`, sin ningún bloque «UNHANDLED EXCEPTION» en `log.log`, conducción y armas (uzi con ratón, teclado y mando) con el proceso vivo hasta el límite (salida 0xDEAD0002 = límite propio de 160 s). Ejecuciones previas: `spp120-20261125-054358` (exe anterior al último cambio de `Pathfind.cpp`; 94 latidos, 187 s, `excepciones=0`) y `spp120b-20261125-065053` (exe final; **2 excepciones de primera oportunidad en el arranque, dentro del controlador de vídeo de Intel (`igd12um32xel` -> `d3d9on12` -> `Direct3DCreate9Ex` desde `GetVideoMemInfo`)**, no reproducidas en las otras dos; el juego siguió y el escenario de conducción terminó con el proceso vivo). El mensaje «Se intentó cargar una partida guardada de otra versión» del log es anterior a esta rama (la partida de prueba es «v1 modificada»).
* **No verificado**: ningún arreglo de esta tanda se ha comparado con el EXE original puro (el banco es código nuestro; «apagado» es la rama literal del EXE); no hay prueba con partida nueva completa, cooperativo, replay, disturbios, quad, Kart ni atropellos con pasajero (ver los estados del CSV); el rendimiento no se midió (las opciones se consultan por llamada con una caché por nombre; en `Pathfind.cpp` y `World_Helper.cpp` se leen una sola vez).
