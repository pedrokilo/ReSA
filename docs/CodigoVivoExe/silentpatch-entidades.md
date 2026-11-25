# SilentPatch: vehículos, modelos, peds, jugador, fuego y pools (rama `silentpatch-entidades`)

Fecha: 25/11/2026. Rama `silentpatch-entidades` (worktree `D:\Original\wv-sp2`, base `integracion-fase7b`). Código de SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent)) copiado de `D:\Original\ref_mods\silentpatch\SilentPatch-dev` (solo lectura); solo se tomó el mínimo necesario y se reescribió en C++ propio sobre nuestras funciones reconstruidas (sin `__asm`). Licencia y atribución: [../THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md). Deviaciones: [../DesviacionesConocidas.md](../DesviacionesConocidas.md) sección 15.

## Cómo apagar SilentPatch

* **Todo**: variable de entorno `GTASA_SILENTPATCH=0`, o la línea `SilentPatch=0` (en cualquier sección) de `gta-reversed.ini` junto al juego. El entorno gana al ini. Con el interruptor general apagado, `SilentPatchOption()` devuelve siempre `false` y cada sitio ejecuta exactamente la rama del EXE original. Por defecto está ENCENDIDO (constante `kSilentPatchPorDefecto`, una sola línea, en `source/extensions/SilentPatchConfig.cpp`).
* **Uno solo**: `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0` o la línea `SilentPatch_<Nombre>=0` en `gta-reversed.ini` (por ejemplo `GTASA_SILENTPATCH_SECURICARDAMAGE=0`, `SilentPatch_Rand16bit=0`). Los nombres son los de la columna «Opción». El valor por defecto de cada opción es el de `SilentPatchSA.ini` (solo `TrueInvincibility` tiene opción allí y vale 0; el resto no tiene opción en SilentPatch y va activado).
* El módulo de configuración (`SilentPatchConfig.h/.cpp`) es el de la rama `silentpatch-rtanim` (se reutilizó tal cual para no duplicar).

## Resumen

Viñetas del changelog evaluadas en mi área: 96 filas (más 22 de voces/diálogos agrupadas abajo). Por clase: A=72, B=2, C=4, D=4, E=14. Implementadas (clase A con código): 26 filas, cubren 23 opciones.

Estado de compilación y prueba:

* Los 25 archivos modificados compilan (rc=0) con las opciones de la ASI (`compilar` con `reversing/FnFase8/cl.rsp`, sin PCH, bajo `D:\Original\compile_lock`): Automobile, Vehicle, Weapon, Cheat, Ped, CarCtrl, CarAI, Garages, Fire, Game, Streaming, FileLoader, Plane, RunningScript, RunningScriptCommands100To199, MathCommands, TheScripts, PedDamageResponseCalculator, VehicleModelInfo, TaskSimpleUseGun, TaskSimpleGunControl, TaskComplexEnterCar, TaskSimpleCarSetPedOut, TaskComplexCarSlowBeDraggedOut y SilentPatchConfig.
* Enlace del ejecutable independiente: probado con los .obj de `wv-int\build\...\Debug` sustituyendo los míos; los objetos míos resuelven todos sus símbolos. El enlace termina con 9 externos sin resolver, TODOS de objetos obsoletos de `wv-int\build` ajenos a esta rama (`d3dx9math.obj` espera `D3DX9Native::*` y `Pad.obj` espera `PadDiag::*`, que se añadieron después de esa compilación). No se generó ejecutable ni ASI, no se lanzó el juego.

## Tabla: viñeta -> clase -> función nuestra -> opción -> estado

Clases: A = se aplica a nuestra función reconstruida; B = ya corregido en el proyecto u otra rama; C = no aplica al standalone; D = depende de bibliotecas/hardware que aún no tenemos (render/otra rama); E = ambigua o no localizada. «L» es el número de línea de `CHANGELOG-SA.md`.

| L | Viñeta | Clase | Evidencia (SilentPatch / nuestra función) | Opción | Estado |
| --- | --- | --- | --- | --- | --- |
| 16 | Fallo en explosiones de coche (multi-monitor) | C | Fallo del driver/EXE con varios monitores; nuestro codigo de explosion no lo reproduce | - | no aplica |
| 23 | Fallo al machacar el replay cerca de bandas con objetos | E | CReplay; sin evidencia de que exista en nuestra reconstruccion | - | pendiente |
| 24 | Fallo en cinematica tras replay con otra ropa | E | CClothes::RebuildPlayer / CReplay (SP: ReplayPlayerPedCrashFixes) | - | pendiente |
| 25 | Fallo en replay con otro cuerpo (gordo/musculoso) | E | Idem | - | pendiente |
| 26 | Skimmer no aparece en Windows 11 24H2 | C | Compatibilidad del EXE con el SO | - | no aplica |
| 28 | Trenes en garajes | A | SP: TrainsInGarages (ctor de CAutomobile -> CTrain). Nuestro: CStoredCar::RestoreCar (Garage.cpp, offsets crudos) | TrainsInGarages | pendiente (no implementado: Garage.cpp usa offsets crudos; riesgo alto sin ejercitarlo) |
| 30 | Corrupcion de heap en un sitio | E | No identificado | - | pendiente |
| 31 | Gran distancia: territorios extra / Taxi Driver | E | CGangWars / CTheScripts; no localizado en SP con certeza | - | pendiente |
| 32 | Reclutar tras replay (1.0 no-DVD) | C | CPlayerPedData::operator= mal descifrado del EXE 1.0 no-DVD; nuestro C++ no lo contiene | - | no aplica |
| 33 | Fuga 1.01 al cambiar de radio | C | Solo 1.01 | - | no aplica |
| 50 | Streaming memory bug | B | RtAnimInterpolatorSetCurrentAnim: lo hace silentpatch-rtanim (wv-sp1) | RtAnimInterp | hecho por otra rama |
| 51 | Armas cuerpo a cuerpo de los trucos sustituidas por un pickup | A | SP 0x43890B.. (municion 0 -> 1). Nuestro: CPed::GiveWeaponSet2/3, CCheat::NinjaCheat, ParachuteCheat | MeleeCheatAmmo | implementado, compila; no ejercitado en juego |
| 52 | Paneles que se balancean tras la explosion | A | SP corrige codigo que los reponia al instante; no localizado en nuestro CAutomobile | - | pendiente |
| 54 | Coches aleatorios acaban en el impound | A | SP RecordVehicleDeleted_AndRemoveFromVehicleList. Nuestro: CVehicle::~CVehicle -> CCarCtrl::RemoveFromInterestingVehicleList | ImpoundRandomVehiclesFix | implementado, compila; no ejercitado en juego |
| 55 | Impound solo coches y motos | A | SP RestrictImpoundVehicleTypes. Nuestro: CGarages::StoreCarInNearestImpoundingGarage | ImpoundOnlyCarsAndBikes | implementado, compila; no ejercitado en juego |
| 56 | Muzzle flash con la ultima bala | A | SP 0x61ECDC. Nuestro: CTaskSimpleUseGun::FireGun (usa el retorno de CWeapon::Fire) | LastBulletMuzzleFlash | implementado, compila; no ejercitado en juego |
| 60 | Contadores de generadores de coches con n fijo | A | CTheCarGenerators; no localizado el parche exacto | - | pendiente |
| 61 | Azar: peds no aparecen en algunas zonas | A | SP ConsoleRandomness::rand31 en 0x44E82E y 0x44ECEE; no portado | - | pendiente |
| 62 | Azar: prostitutas calladas | A | SP rand31 en 0x666EA0 (CTaskComplexProstituteSolicit); no portado | - | pendiente |
| 67 | La colision danya las partes correctas en ambos coches | A | SP en CAutomobile; nuestro Automobile.cpp es muy extenso; no portado | - | pendiente |
| 69 | Enemigos demasiado precisos tras haber estado el jugador en coche | A | SP WeaponRangeMult_VehicleCheck (0x73B3AE). Nuestro: CWeapon::TargetWeaponRangeMultiplier (exige bInVehicle) | AIAccuracyFix | implementado, compila; no ejercitado en juego |
| 74 | Generadores de coches en interiores | A | SP CEntity::SetPositionAndAreaCode en CCarGenerator (0x6F3B30); no portado | - | pendiente |
| 75 | Bombas en coches guardados en garajes | A | SP CStoredCar::RestoreCar_LoadBombOwnership; nuestro CStoredCar::RestoreCar con offsets crudos; no portado | - | pendiente |
| 76 | Streaming con vehiculos RC (Supply Lines) | A | SP FindPlayerVehicle_RCWrap / FindPlayerEntityWithRC; no portado | - | pendiente |
| 77 | Triangulo de salud de peds reclutables | A | HUD/peds; no localizado | - | pendiente |
| 78 | BMX en llamas no prende a CJ | A | SP BicycleFire. Nuestro: CFire::ProcessFire | BicycleFire | implementado, compila; no ejercitado en juego |
| 82 | Animaciones de paracaidas | E | Animaciones (zona de wv-sp1); no localizado | - | pendiente |
| 83 | Conservar armas tras wasted/busted se reinicia en New Game | E | No localizado en SP con certeza (no esta en su lista de variables a reiniciar) | - | pendiente |
| 84 | Motos sin motor | E | No localizado en el codigo de SP (probablemente dato) | - | pendiente |
| 85 | extra6 con la regla de componentes 4 | A | SP 0x4C8010. Nuestro: ::ChooseComponent (rama FULL_RANDOM) | Extra6Rule | implementado, compila; no ejercitado en juego |
| 89 | Viento en la ropa de CJ en quad (Wesser) | A | SP: animaciones del quad; no portado | - | pendiente |
| 90 | Manillar del quad sigue las animaciones (Wesser) | A | SP: namespace del quad; no portado | - | pendiente |
| 91 | Animacion de cambio de radio en vehiculos bajos, camiones y botes | A | SP (B1ack_Wh1te); no portado | - | pendiente |
| 92 | Sin animacion de radio en el Kart | A | No portado | - | pendiente |
| 93 | Pickups, generadores y stunt jumps de IPL de texto se reinician en New Game | A | SP VariableResets guarda las lineas leidas y las vuelve a cargar. No portado: solo se porto el reinicio de variables | - | pendiente |
| 94 | Crosshair con francotirador en foto de pandillero | A | No portado | - | pendiente |
| 95 | Policias en moto siguen disparando tras perder el nivel | A | SP BikerCopsDriveByFix. Nuestro: CCarAI::BackToCruisingIfNoWantedLevel | BikerCopsDriveByFix | implementado, compila; no ejercitado en juego |
| 96 | Motorista pedido con PEDTYPE_COP sale policia normal | A | SP GetCorrectPedModel_Lapdm1. Nuestro: CRunningScript::GetCorrectPedModelIndexForEmergencyServiceType | BikerCopSpawn | implementado, compila; no ejercitado en juego |
| 98 | Probabilidad de luces del trafico como en PS2 | A | SP 0x6D5612. Nuestro: CVehicle::GetVehicleLightsStatus | LightsRandomness | implementado, compila; no ejercitado en juego |
| 99 | Vehiculos explotan dos veces si el conductor sale | A | SP RemoveDriverStatusFix. Nuestro: CVehicle::RemoveDriver, CTaskComplexCarSlowBeDraggedOut::CreateFirstSubTask, CTheScripts::RemoveThisPed, CTaskSimpleCarSetPedOut::ProcessPed | VehicleExplodeTwice | implementado, compila; no ejercitado en juego |
| 100 | Azar de scripts de 16 bits | A | SP Rand16bit (0x4674FE, 0x467533). Nuestro: ProcessCommands100To199 GENERATE_RANDOM_FLOAT y MathCommands::GenerateRandomInt | Rand16bit | implementado, compila; no ejercitado en juego |
| 102 | El coche explotado pierde la rueda correcta | A | SP HideDestroyedWheels_SilentPatch y CDamageManager::FuckCarCompletely (0x6C25F5); las llamadas a SpawnFlyingComponent en nuestro Automobile/Heli/Plane tienen otra forma; no portado | - | pendiente |
| 103 | Aviones aparecen en mejores sitios | A | SP FindPlaneCreationCoorsFix. Nuestro: CPlane::FindPlaneCreationCoors | PlaneCreationCoorsFix | implementado, compila; no ejercitado en juego |
| 104 | Flotar con el jetpack con teclado | A | SP JetpackKeyboardControlsHover; no portado | - | pendiente |
| 105 | Disturbios: pandilleros del grupo no son objetivo del helicoptero | A | No portado | - | pendiente |
| 106 | Robar coche por el lado del pasajero mata al conductor | A | SP 0x63F576 (jz -> jnz). Nuestro: CTaskComplexEnterCar::CreateSubTask (caso SHUFFLE) | StealCarFromPassengerSide | implementado, compila; no ejercitado en juego |
| 114 | El nitro se recarga mas rapido marcha atras | A | SP NitrousReverseRechargeFix. Nuestro: CAutomobile::NitrousControl | NitrousReverseRecharge | implementado, compila; no ejercitado en juego |
| 115 | Propulsores del Hydra a veces no salen | A | SP JetThrustersFix (flags de matriz sin inicializar); no portado | - | pendiente |
| 117 | CJ no se hunde en la moto al subir de frente | A | SP 0x64FFAD (fsub); no portado | - | pendiente |
| 118 | Entrar en furgonetas por detras sin teletransporte | A | SP 0x64E77B (ANIM_VEH_VAN); no portado | - | pendiente |
| 119 | Policias no sostienen armas a una mano como pandilleros | A | SP 0x61E52E y 0x625434. Nuestro: CTaskSimpleUseGun::SetMoveAnim y CTaskSimpleGunControl::ProcessPed | CopsGunStance | implementado, compila; no ejercitado en juego |
| 121 | Etiquetas de banda anyadidas por mods en el guardado | E | CGangWars/CTagManager; no localizado | - | pendiente |
| 123 | Blip del segundo jugador | E | CRadar; no localizado | - | pendiente |
| 124 | Guerras de bandas quitan blips | E | CGangWars; no localizado | - | pendiente |
| 149 | Securicars mas duros | A | SP 0x6A806F (jnz -> jmp). Nuestro: CAutomobile::VehicleDamage | SecuricarDamage | implementado, compila; no ejercitado en juego |
| 150 | Beagle de Fort Carson -> BMX | A | SP FortCarsonBeagle. Nuestro: CFileLoader::LoadCarGenerator (comparacion binaria identica) | FortCarsonBeagle | implementado, compila; no ejercitado en juego |
| 151 | Rhino con luces completas (rx) | A | SP; datos de modelo/luces; no portado | - | pendiente |
| 153 | Piezas sueltas con el color/luz del coche | E | Colores de vehiculos: zona del agente de vehiculos negros (wv-color) | - | coordinar con wv-color |
| 154 | Piezas sueltas se dibujan por ambas caras | D | Estados de render (lo cubre silentpatch-graficos) | - | otra rama |
| 156 | Componentes animados extra (capo del Phoenix, cepillos del Sweeper, antena de la Newsvan, radares, flaps) | A | SP ProcessPhoenixBlower/ProcessSweeper/ProcessNewsvan + SVF; no portado (marco de caracteristicas por modelo) | - | pendiente |
| 157 | Componentes de motor solo animan con el motor encendido | A | SP; no portado | - | pendiente |
| 158 | Escalera del Firetruck (firela) | A | SP; no portado | - | pendiente |
| 159 | Remolques artict3 encadenables | A | SP CTrailer::GetTowBarPos; no portado | - | pendiente |
| 160 | Tug con barra de remolque | A | SP; no portado | - | pendiente |
| 161 | Rueda del DFT-30 (errata de jerarquia) | A | SP "wheel_lm vs wheel_lm_dummy"; no portado | - | pendiente |
| 162 | Suspension del Dumper (errata) | A | Idem | - | pendiente |
| 163 | Luces traseras del Uranus (errata) | A | Idem | - | pendiente |
| 167 | Armas visibles a traves de ventanillas | D | RenderWeapon/estados de render (silentpatch-graficos) | - | otra rama |
| 168 | Arma en mano ilumina mal otros objetos | D | Idem | - | otra rama |
| 169 | Vehiculos destrozados bien coloreados y sin brillo | E | Colores: wv-color | - | coordinar con wv-color |
| 170 | Los coches sucios pueden limpiarse | A | SP RemapDirt/ms_aDirtTextures (CVehicleModelInfo); no portado | - | pendiente |
| 171 | Matricula unica por coche | A | SP CCustomCarPlateMgr::GeneratePlateText; no portado | - | pendiente |
| 172 | Matriculas personalizadas en todos los casos | A | Idem | - | pendiente |
| 173 | Matriculas personalizadas en motos | A | Idem | - | pendiente |
| 174 | Matriculas con filtro bilineal | D | Render (graficos) | - | otra rama |
| 175 | Luces de vehiculo no se oscurecen tras encenderse | B | Lo corrigio el agente de vehiculos negros (rama fix-vehiculos-negros: ms_lightsOn / ms_currentCol / m_sfLogLookup) | - | otra rama |
| 177 | Encender luces no hace invisibles las ventanas | E | Colores/materiales: wv-color o graficos | - | pendiente |
| 185 | Garajes de impound funcionan | A | SP: IsPointWithinAnyGarage en 0x425179/0x425369/0x425411. Nuestro: CCarCtrl::PossiblyRemoveVehicle (3 sitios) | ImpoundGaragesFix | implementado, compila; no ejercitado en juego |
| 186 | El impound guarda el coche al ser arrestado | A | SP Nop 0x443292 en CGameLogic::Update; hay varias llamadas a ClearInterestingVehicleList (wasted/busted) y no se pudo identificar cual sin el desensamblado | - | pendiente |
| 187 | Lista de entidades en streaming ampliada | A | SP 0x5B8E55 (12000 -> 15000 bytes). Nuestro: CStreaming::Init2 (1000 -> 1250 enlaces) | StreamedEntityList | implementado, compila; no ejercitado en juego |
| 188 | El raton gira un coche en el aire solo con "Steer with Mouse" | A | SP CVehicle::m_bEnableMouseSteering (0x6B4EC0, 0x6CE827); no portado | - | pendiente |
| 189 | Gancho de la grua tras reparar | A | SP CAutomobile::Fix_SilentPatch (depende de SVF); no portado | - | pendiente |
| 190 | Puertas de avion tras reparar | A | Idem | - | pendiente |
| 191 | Piezas moviles de avion tras reparar | A | Idem | - | pendiente |
| 192 | Parachoques del Rumpo y pieza del Bandito tras reparar | A | Idem (ResetFrames) | - | pendiente |
| 195 | Respray gratis no pasa a New Game | A | SP VariableResets. Nuestro: CGame::ReInitGameObjectVariables | ResetVariablesOnNewGame | implementado, compila; no ejercitado en juego |
| 196 | Temporizadores de ambulancia/bomberos se reinician | A | Idem (LastTimeFireTruckCreated/LastTimeAmbulanceCreated) | ResetVariablesOnNewGame | implementado, compila; no ejercitado en juego |
| 197 | Contadores de estadisticas se reinician | A | Idem (CStats::m_*Counter) | ResetVariablesOnNewGame | implementado, compila; no ejercitado en juego |
| 198 | Mensaje "To stop Carl..." se reinicia | A | Idem (CPlayerPed::bHasDisplayedPlayerQuitEnterCarHelpText) | ResetVariablesOnNewGame | implementado, compila; no ejercitado en juego |
| 199 | Truco SEAROADER (coches sobre agua) | A | SP: hash de la ranura 34 del vector de trucos. Nuestro: CCheat::AddToCheatString | SeaRoaderCheat | implementado, compila; no ejercitado en juego |
| 201 | Rhino no gana ruedas extra tras reparar | A | SP Fix_SilentPatch (RHINO_WHEELS); no portado | - | pendiente |
| 202 | Empujar peds no activa voces del pasajero | A | SP Say_SampleBlackList / PlayPedHitSample; audio, no portado | - | pendiente |
| 203 | Pay 'n Spray no limpia antes de cerrar la puerta | A | SP Nop 0x44ACDC; nuestro Garage.cpp usa offsets crudos, no localizado | - | pendiente |
| 204 | Pinturas no se pierden en coches guardados | A | SP CVehicle::GetRemapIndex; no portado (Garage.cpp ya llama a GetRemapIndex) | - | pendiente |
| 206 | Ruedas traseras dobles | A | SP DoubleRearWheels / CheckDoubleRWheelsList; no portado | - | pendiente |
| 224 | TrueInvincibility (apagada por defecto) | A | SP TrueInvincibility (0x4B322E). Nuestro: CPedDamageResponseCalculator::ComputeWillKillPed | TrueInvincibility (defecto 0) | implementado, compila; no ejercitado en juego |

### Voces y diálogos (L125 a L146)

Clase E (no evaluadas una a una): son ajustes de `CAEPedSpeechAudioEntity`/`SayFunc` y de las tablas de voces de peds (WMYSGRD, BMYPIMP, VBFYST2, SBFYSTR, taunts de bandas, policías, etc.). No se portó ninguna: dependen de datos de audio (PedSpeechContexts/paks) y de la entidad de voz, que es zona del audio, no de vehículos/peds. Estado: pendiente.

### Viñetas fuera de mi área (no evaluadas aquí)

Gráficos, UI, audio, entrada, cargador y scripts van en las otras ramas SilentPatch (`silentpatch-graficos`, `silentpatch-scm`, `silentpatch-rtanim`) o son del EXE/instalador: L9-L15, L17-L22, L27, L29, L34-L49, L53, L57-L59, L63-L66, L68, L70-L73, L79-L81, L86-L88, L97, L101, L107-L113, L116, L120, L122, L147-L148, L152, L155, L164-L166, L176, L178-L184, L193-L194, L200, L205, L207, L208-L223, L225-L226. L35-L40, L120, L122 y L207 son de scripts (rama `silentpatch-scm`); L116, L147, L205 son de render (rama `silentpatch-graficos`).

## Qué se implementó

Cada sitio lleva el comentario `// SilentPatch (MIT, Silent): <arreglo>` y se activa con `SilentPatchOption("<Opción>", <defecto>)`.

| Opción | Archivo(s) |
| --- | --- |
| MeleeCheatAmmo | Entity/Ped/Ped.cpp (GiveWeaponSet2/3), Cheat.cpp (NinjaCheat, ParachuteCheat) |
| AIAccuracyFix | Weapon.cpp |
| SecuricarDamage | Entity/Vehicle/Automobile.cpp |
| ImpoundOnlyCarsAndBikes | Garages.cpp |
| ImpoundRandomVehiclesFix | Entity/Vehicle/Vehicle.cpp |
| ImpoundGaragesFix | CarCtrl.cpp |
| LastBulletMuzzleFlash | Tasks/TaskTypes/TaskSimpleUseGun.cpp |
| BicycleFire | Fire.cpp |
| StealCarFromPassengerSide | Tasks/TaskTypes/TaskComplexEnterCar.cpp |
| VehicleExplodeTwice | Entity/Vehicle/Vehicle.cpp, Scripts/TheScripts.cpp, Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp, TaskComplexCarSlowBeDraggedOut.cpp |
| NitrousReverseRecharge | Entity/Vehicle/Automobile.cpp |
| CopsGunStance | Tasks/TaskTypes/TaskSimpleUseGun.cpp, TaskSimpleGunControl.cpp |
| Rand16bit | Scripts/RunningScriptCommands100To199.cpp, Scripts/Commands/MathCommands.cpp |
| LightsRandomness | Entity/Vehicle/Vehicle.cpp |
| Extra6Rule | Models/VehicleModelInfo.cpp |
| FortCarsonBeagle | FileLoader.cpp |
| SeaRoaderCheat | Cheat.cpp |
| BikerCopSpawn | Scripts/RunningScript.cpp |
| BikerCopsDriveByFix | CarAI.cpp |
| PlaneCreationCoorsFix | Entity/Vehicle/Plane.cpp |
| StreamedEntityList | Streaming.cpp |
| ResetVariablesOnNewGame | Game.cpp |
| TrueInvincibility (defecto 0) | PedDamageResponseCalculator.cpp |

Todas las rutas son relativas a `source/game_sa/`. Salvo `TrueInvincibility`, ninguna opción existe en `SilentPatchSA.ini`, así que van activadas por defecto.

## Prueba y evidencia

* **Interruptor general y opciones** (probado): `contrib/silentpatch_selftest/t.cpp` enlaza `SilentPatchConfig.cpp` y consulta seis opciones; resultado guardado en `docs/CodigoVivoExe/silentpatch-entidades-selftest.txt`. Con `GTASA_SILENTPATCH=0` todas devuelven 0; con 1 devuelven su valor por defecto; `GTASA_SILENTPATCH_AIACCURACYFIX=0` apaga solo esa; `TrueInvincibility` (defecto 0) se enciende con `GTASA_SILENTPATCH_TRUEINVINCIBILITY=1`.
* **«Con el interruptor apagado el comportamiento es el de antes»**: demostrado por construcción y revisión, NO por una corrida del juego: en cada sitio la rama con `SilentPatchOption(...)==false` reproduce la expresión original (p. ej. `GetRandomIntInRange(0, 5)`, `1.f - m_GasPedal`, `pedType == PED_TYPE_COP`, `1000` enlaces, etc.). No se ejecutó el juego.
* **Ejercitado en juego**: ninguno. Todos los arreglos están «implementados, compilan; no ejercitados». No se lanzó el humo (`prueba_ropero.py`/`prueba_marcadores.py`) porque requiere una ASI completa (cmake) y el candado del juego, y el equipo de compilación estaba ocupado; el arreglo de coches robados, impound, Securicar, explosión doble, etc. requiere situaciones concretas del juego (misiones, garajes, policía) que no se pueden reproducir con las sondas FnTraf/FnJuego sin trabajo adicional.

## Desviaciones frente a SilentPatch

* `LastBulletMuzzleFlash`: SilentPatch sustituye una comparación del estado del arma (6 bytes) por `test al, al`; asumimos que `al` es el retorno de `CWeapon::Fire` y lo tomamos de ahí. No verificado contra el desensamblado.
* `CopsGunStance`: SilentPatch cambia dos `cmp` con `COP` por `DEALER` y `CRIMINAL` en dos funciones; en nuestro C++ la condición estaba como un solo `== COP`, por lo que se usa `DEALER || CRIMINAL`.
* `Rand16bit`: en nuestro C++ el comando `GENERATE_RANDOM_INT` es un handler NOTSA (`MathCommands.cpp`) y `GENERATE_RANDOM_FLOAT` está en `ProcessCommands100To199`; se aplicó la combinación `rand() | ((rand() & 1) << 15)` a ambos. Asumimos que `0x4674FE` es el `rand` usado de `GENERATE_RANDOM_FLOAT` y `0x467533` el de `GENERATE_RANDOM_INT` (sin desensamblado).
* `StreamedEntityList`: 12000 bytes del EXE son 1000 enlaces de 12 bytes; 15000 bytes son 1250.
* `ImpoundOnlyCarsAndBikes`: SilentPatch lo hace donde se consulta `IsThisVehicleInteresting` (dos sitios de `CWorld`); nuestro código solo llama a `StoreCarInNearestImpoundingGarage` desde `CWorld::RemoveVehicleAndItsOccupants`, y el filtro se pone ahí dentro, que cubre todas las llamadas.
* `ResetVariablesOnNewGame`: se reinician las variables al principio de `ReInitGameObjectVariables` (SilentPatch lo hace antes de llamar a la original en los dos puntos de `InitialiseWhenRestarting`, equivalentes). `TimeNextMadDriverChaseCreated` ya lo reinicia `CCarCtrl::ReInit`. No se portó la recarga de pickups/generadores/stunt jumps de IPL de texto (L93).

