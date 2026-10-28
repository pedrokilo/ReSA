<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CheatFinal3 - banco de la clase CCheat (72 funciones pendientes + HealthCheat)

Banco por pares (hook desactivado = EXE original, hook activado = DLL) para las 72 funciones pendientes de `CCheat` (GTA SA 1.0 US,
`docs/hooks.csv`, `class=CCheat`, `validated=0`) mas `CCheat::HealthCheat` (0x438D60), que no tiene hook y la llama `MoneyArmourHealthCheat`: para ella el
original es el codigo del EXE y el candidato la funcion de la DLL llamada directamente. Estructura copiada de WeaponFinal2 / PadMouse1 (corre dentro del juego
completo y se engancha en `CGame::Process`). `GAME` se redirige en memoria a `E:/JUEGOS/GTA San Andreas - CVehicle Test` en `build_probe.py` y `run_local.py`
(`runtime_runner.py` no se toca). Sin compilar ni ejecutar todavia.

## Uso

    bash reversing/_handoff/bench.sh CheatFinal3 3

Filtro opcional: `cheatfinal3-only.txt` en la carpeta del juego con `CCheat::NombreDeLaFuncion`. El analizador exige todos los pares iguales,
`state_restored`, `layout_ok` y el total de la tabla `fns` (11840 pares, 73 funciones).

## Pares por funcion

| Funcion | Direccion EXE | Casos |
|---|---|---|
| AddToCheatString | 0x438480 | 1024 |
| AdrenalineCheat | 0x439880 | 128 |
| AllCarsAreGreatCheat | 0x4394E0 | 128 |
| AllCarsAreShitCheat | 0x4394B0 | 128 |
| ApacheCheat | 0x43A550 | 64 |
| ApplyCheat | 0x438370 | 256 |
| BeachPartyCheat | 0x439230 | 192 |
| BlackCarsCheat | 0x4390F0 | 128 |
| BlowUpCarsCheat | 0x439D80 | 256 |
| CloudyWeatherCheat | 0x438F60 | 64 |
| CountrysideInvasionCheat | 0x439F60 | 192 |
| DoCheats | 0x439AF0 | 512 |
| DozerCheat | 0x43A660 | 64 |
| DrivebyCheat | 0x4398D0 | 192 |
| DuskCheat | 0x439540 | 128 |
| ElvisLivesCheat | 0x4391D0 | 128 |
| EverybodyAttacksPlayerCheat | 0x439C70 | 256 |
| ExtraSunnyWeatherCheat | 0x438F50 | 64 |
| FastTimeCheat | 0x438F90 | 192 |
| FatCheat | 0x439110 | 128 |
| FlyboyCheat | 0x43A530 | 64 |
| FoggyWeatherCheat | 0x438F80 | 64 |
| FunhouseCheat | 0x439720 | 192 |
| GangLandCheat | 0x4393D0 | 128 |
| GangsCheat | 0x439360 | 128 |
| GolfcartCheat | 0x43A520 | 64 |
| HandleSpecialCheats | 0x439A10 | 512 |
| HearseCheat | 0x43A4F0 | 64 |
| IsZoneStreamingAllowed | 0x407410 | 256 |
| JetpackCheat | 0x439600 | 192 |
| LoveConquersAllCheat | 0x4393F0 | 128 |
| LovefistCheat | 0x43A500 | 64 |
| MayhemCheat | 0x439B20 | 256 |
| MidnightCheat | 0x439510 | 128 |
| MoneyArmourHealthCheat | 0x438E40 | 192 |
| MonsterTruckCheat | 0x43A680 | 64 |
| MuscleCheat | 0x439150 | 128 |
| NinjaCheat | 0x439E50 | 192 |
| NotWantedCheat | 0x4396C0 | 128 |
| ParachuteCheat | 0x4395B0 | 128 |
| PinkCarsCheat | 0x4390D0 | 128 |
| PredatorCheat | 0x4395A0 | 64 |
| QuadCheat | 0x43A560 | 64 |
| RainyWeatherCheat | 0x438F70 | 64 |
| ResetCheats | 0x438450 | 128 |
| RiotCheat | 0x439710 | 128 |
| SandstormCheat | 0x439590 | 64 |
| SkinnyCheat | 0x439190 | 128 |
| SlowTimeCheat | 0x438FC0 | 192 |
| StaminaCheat | 0x439930 | 64 |
| StockCar2Cheat | 0x43A4C0 | 64 |
| StockCar3Cheat | 0x43A4D0 | 64 |
| StockCar4Cheat | 0x43A4E0 | 64 |
| StockCarCheat | 0x43A4B0 | 64 |
| StormCheat | 0x439570 | 64 |
| StuntPlaneCheat | 0x43A670 | 64 |
| SuicideCheat | 0x438FF0 | 192 |
| SunnyWeatherCheat | 0x438F40 | 64 |
| TankCheat | 0x43A4A0 | 64 |
| TankerCheat | 0x43A570 | 192 |
| TrashmasterCheat | 0x43A510 | 64 |
| VehicleCheat | 0x43A0B0 | 1024 |
| VehicleSkillsCheat | 0x4399D0 | 64 |
| VillagePeopleCheat | 0x439DD0 | 192 |
| VortexCheat | 0x43A540 | 64 |
| WantedCheat | 0x4396F0 | 64 |
| WantedLevelDownCheat | 0x438F20 | 64 |
| WantedLevelUpCheat | 0x438E90 | 192 |
| WeaponCheat1 | 0x4385B0 | 192 |
| WeaponCheat2 | 0x438890 | 192 |
| WeaponCheat3 | 0x438B30 | 192 |
| WeaponSkillsCheat | 0x439940 | 64 |
| HealthCheat (sin hook) | 0x438D60 | 256 |

## Archivos

- `Common.hpp`: infraestructura (trazas `T`, nombres de punteros, `Patches`/EntryPatch, `Snap/RestoreRegions`, `Body()/FollowIn`, `FindHook`).
- `Doubles.hpp`: memoria falsa (3 jugadores con `CPlayerPedData`, `CWanted`, `CPedClothesDesc` e inteligencia propios; pool de peds de hasta 64 y pool de
  vehiculos de hasta 110; lista de referencias), vtable falsa de vehiculo, resumen de eventos y todos los dobles.
- `Cases.hpp`: fixtures aleatorios deterministas (la misma semilla para original y candidato) y llamada a cada funcion en la direccion del EXE.
- `LiveComparison.hpp`: tabla `fns`, calculo de los cuerpos de la DLL, evento `layout` y bucle principal.

## Que se compara

Por caso, con el hook desactivado y luego activado: el estado de todos los globales que tocan los trucos (banderas de trucos, cadena de trucos,
`m_bHasPlayerCheated`, escala de tiempo, reloj del juego, `m_bOnlyCreateRandomGangMembers`, `m_bHeatHazeFX`, lluvia y viento, dinero y maximos de los jugadores,
informacion de streaming de los modelos 322..611, tablas de hashes y de funciones), los buffers falsos de jugadores / pools / vehiculo (hash de todos sus bytes),
el estado de la lista de referencias, el retorno cuando lo hay y la traza ORDENADA de todas las llamadas salientes con sus argumentos. Cada registro
pasante lleva los campos `original` y `candidate` (resumen FNV del JSON completo de cada lado). En los trucos de armas se compara ademas el conjunto de indices
pedidos a `FindPlayerPed` (el EXE pide -1 y 1; la DLL pedia 0 y 1).

## Dobles (EntryPatch en la direccion del EXE y en el cuerpo de la DLL, con el `ret N` del EXE)

`FindPlayerPed` (0x56E210; -1 = jugador en foco, 0 = jugador cero literal, 1 = segundo jugador o nulo), `FindPlayerVehicle` (0x56E0D0),
`CStreaming::RequestModel` (0x4087E0, con su efecto real `m_Flags |= flags`), `LoadAllRequestedModels` (0x40EA10), `SetModelIsDeletable` (0x409C10),
`SetModelTxdIsDeletable` (0x409C70), `StreamPedsIntoRandomSlots` (0x40BDA0), `ReclassifyLoadedCars` (0x40AFA0), `CPedClothesDesc::SetTextureAndModel`
(0x5A8080), `CClothes::RebuildPlayer` (0x5A82C0), `CWeather::ForceWeatherNow` (0x72A4F0) y `ReleaseWeather` (0x72A510), `CStats::SetStatValue` (0x55A070) e
`IncrementStat` (0x55C180), `CHud::SetHelpMessage` (0x588BE0), `CText::Get` (0x6A0050), `CPed::GiveWeapon` (0x5E6080), `SetCurrentWeapon(eWeaponType)`
(0x5E6280), `GiveDelayedWeapon` (0x5E89B0), `CPlayerPed::CheatWantedLevel` (0x609F50), `CPickups::GivePlayerGoodiesWithPickUpMI` (0x4564F0),
`CVehicle::operator new` (0x6E2D50, devuelve nulo), `CTask::operator new/delete` (0x61A5A0 / 0x61A5B0), `CPedIntelligence::GetTaskJetPack` (0x601110),
`CEventGroup::Add` (0x4AB420), `CEventDamage::AffectsPed` (0x4B35A0, por la ranura 5 de la vtable 0x85AD78), `CPedDamageResponseCalculator::ComputeDamageResponse`
(0x4B5AC0), `CAcquaintance::SetAsAcquaintance` (0x608DA0), `CPedType::SetPedTypeAsAcquaintance` (0x608E20) y, segun la funcion probada, las demas funciones de
CCheat que la llaman (`BeachPartyCheat`, `FunhouseCheat`, `CountrysideInvasionCheat`, `HandleSpecialCheats`, `EverybodyAttacksPlayerCheat`, `HealthCheat`,
`AddToCheatString`, `VehicleCheat`): la funcion que se esta probando nunca se dobla. `ExtraSunnyWeatherCheat` y `RainyWeatherCheat` NO se doblan: se ejecutan de
verdad y la traza sale de `ForceWeatherNow`, igual que el EXE llama a 0x72A4F0 directamente. Los hilos ajenos (audio, etc.) no trazan ni cambian estado (`Mine()`).

## Seguridad (nada del juego real se toca)

- Jugadores, vehiculo del jugador, peds y vehiculos de los pools son buffers del banco; los pools de peds y de vehiculos (0xB74490 / 0xB74494) se intercambian
  SOLO durante la llamada (RAII `Scope`) y se restauran; los eventos reales (`CEventAcquaintancePedHate`, `CEventScriptCommand`, `CEventDamage`) se construyen y
  destruyen de verdad sobre peds falsos con `CReferences::pEmptyList` intercambiado por una lista falsa de 64 nodos.
- `CEventGroup::Add` se dobla y registra un resumen campo a campo del evento (vptr y relleno no se comparan); no se toca ningun grupo de eventos real.
- Jetpack: `CTask::operator new` devuelve un buffer del banco (relleno con 0xCD) y `operator delete` solo traza; se comparan los campos que el constructor de
  `CTaskSimpleJetPack` fija a partir de los argumentos (0x48..0x50, 0x54, 0x58, 0x60). Tambien se prueba el caso de `new` nulo y el de tarea ya existente.
- Todo global tocado se guarda al principio y se restaura tras CADA ejecucion (`Snap/RestoreRegions`) y al final (`state_restored`): cadena y banderas de
  trucos, tablas de hashes y de funciones, escala de tiempo, reloj, banderas de poblacion y postproceso, clima, jugadores (0x320 bytes), foco, streaming 322..611,
  entradas 400..611 de `ms_modelInfoPtrs`, punteros de pools, `ms_running`, `MI_PICKUP_ADRENALINE`, teclado (nuevo y viejo) y `pEmptyList`.
- x87 a 24 bits durante la llamada. Entradas dentro del dominio real: `m_nPedState` y banderas del jugador reales, indices de truco 0..91, nivel de busqueda 0..6,
  jugador en foco 0 o 1; las escalas de tiempo incluyen NaN/Inf/denormal (la condicion de la DLL sigue el asm `fcomp + test ah`). Dinero cerca de INT_MAX para
  comprobar la envoltura de `+= 250000`.
- Relleno enmascarado: del evento de dano no se compara el vptr ni los bits 5..7 del byte 0x25; de los eventos de acquaintance/script solo los campos reales.

## Correcciones hechas en `source/game_sa/Cheat.cpp` (todas con direccion del EXE)

- `AddToCheatString` (0x438480): el EXE prueba la cadena completa y va acortandola por el final (0x438500..0x438527, hash de la copia local con 0x53CF00) y se
  queda con el PRIMER indice de la tabla (0x438510 `je 00438530`); la DLL probaba longitudes crecientes y se quedaba con el ULTIMO indice. `m_bHasPlayerCheated`
  se pone antes de ejecutar el truco (0x438576) y la funcion/toggle se ejecuta en linea (sin pasar por `ApplyCheat`).
- `AdrenalineCheat` (0x4398A8): al desactivar solo limpia el bit 1 de `[m_pPlayerData+0x34]`; la DLL llamaba a `ClearAdrenaline` (ponia el tiempo a 0 y `ResetTimeScale`).
- `BeachPartyCheat` / `FunhouseCheat`: sin la llamada previa `SetTextureAndModel(nullptr,nullptr,SPECIAL)` (no existe entre 0x439261 y 0x439289 / 0x439776).
- `FunhouseCheat` (0x439735 `je 00439875`): la rama activa carga peds, viste y pone `m_bHeatHazeFX = true`; la desactivacion solo lo pone a false. La DLL estaba invertida.
- `NinjaCheat` (0x439E68 `je 00439F4D`): activo = peds + apagar otros temas + katana; inactivo = solo quitar trafico negro. La DLL estaba invertida.
- `VillagePeopleCheat` (0x439DE5 `je 00439E3D`): activo = peds + `EverybodyAttacksPlayerCheat` + `Enable(EVERYONE_ARMED)`; inactivo = `Disable(EVERYONE_ARMED)` + `EverybodyAttacksPlayerCheat`. Invertida.
- `DuskCheat` (0x43954C `je 00439569`): pone las 21:00:00 al ACTIVAR; la DLL lo hacia al desactivar.
- `MayhemCheat` (0x439B4A `je 00439C5E`): sin `std::swap` de tablas de tipos (NOTSA) ni rama de desactivacion; el pool de peds se recorre de la ultima ranura a la primera.
- `EverybodyAttacksPlayerCheat` (0x439CB4..), `BlowUpCarsCheat` (0x439D90..): recorrido del pool del ultimo slot al primero (la DLL iba de menor a mayor).
- `NotWantedCheat` (0x4396C0..0x4396E2): sin `bWantedByPolice = false`.
- `PredatorCheat` (0x4395A0): el EXE es un `ret`; la DLL spawneaba un vehiculo.
- `HealthCheat` (0x438D60) y `MoneyArmourHealthCheat` (0x438E40): vida y armadura maximas del jugador 0 (`[0B7CEE7h]`/`[0B7CEE8h]`), dinero del jugador en foco
  (`[0B7CD74h]*190h+0B7CE50h`); la DLL buscaba la info del ped.
- `WantedLevelUpCheat` (0x438E90): sin la comprobacion de puntero nulo que no existe en el EXE.
- `WeaponCheat1/2/3`: primer jugador = `FindPlayerPed(-1)` (0x43863D/0x43890D/0x438B9D) en vez de 0; `WeaponCheat1` daba el juego al jugador 0 en vez de al 1 (0x438736) y
  hacia deletable el `MODEL_COLT45` (el EXE hace deletable 9 modelos, en otro orden: 0x438825..0x43887F).
- `VehicleCheat` (0x43A0B0): cuenta de vehiculos > 50 -> nulo (0x43A0E2..0x43A105), banderas de streaming leidas antes de `RequestModel` (0x43A128/0x43A14E),
  tabla de saltos por tipo (0x43A17B: CAutomobile para tipo 0/6/7/8/otros, no `CCarGenerator::CreateVehicle`), `pi/2` exacto del EXE (0x858FE4) y
  `PlaceOnRoadProperly` solo CBike/CAutomobile (0x43A426..0x43A456; el tren ya no usa `FindPositionOnTrackFromCoors`).
- `TankerCheat` (0x43A570): comprobacion de nulo tras `new CTrailer` (0x43A5F3) y angulo literal 0x405F66F3 (0x43A616).
- `InjectHooks`: la macro `RH_ScopedNamedGlobalInstall` usa `&fn`; con `cheat.method` (un `void*`) instalaba el salto en la direccion del CAMPO de la tabla (datos), no en la
  de la funcion. Ahora se llama a `ReversibleHooks::Install` con el puntero a la funcion.

## Fuera del banco (con evidencia)

- **Cola de `VehicleCheat` / `TankerCheat`** (posicion, orientacion, estado, `m_nDoorLock`, `CWorld::Add`, `ClearSpaceForMissionEntity`, `PlaceOnRoadProperly`,
  `SetTowLink`): requieren que el constructor del vehiculo (EXE 0x6B0A90 CAutomobile, 0x6C8D60 CMonsterTruck, 0x6CE370 CQuadBike, 0x6C4190 CHeli, 0x6C8E20 CPlane,
  0x6F2940 CBoat, 0x6BF430 CBike, 0x6BF820 CBmx, 0x6D03A0 CTrailer) se ejecute, y la DLL llama al constructor de C++ directamente en la expresion `new` (no hay simbolo
  parcheable para doblarlo); ejecutarlo de verdad crearia un vehiculo real. Por eso `CVehicle::operator new` (0x6E2D50) devuelve nulo y el banco comprueba: area del
  jugador (0x43A0D5 `test cl,cl`), cuenta de vehiculos (0x43A102 `cmp edi,32h`), banderas de streaming (0x43A128 y 0x43A14E `test bl,2`), llamadas a streaming y el TAMANO
  pasado a `operator new` por tipo de vehiculo (0x43A182 `push 99Ch` .. 0x43A302 `push 988h`; tren, helicoptero falso y avion falso -> CAutomobile) y, en `TankerCheat`, la peticion
  de `MODEL_PETROTR` (0x43A59E `push 0`/`push 248h`) y el tamano 0x9F4. El resto de la cola se verifico solo por lectura del ensamblador.

## Suposiciones a revisar al compilar

- `&CVehicle::operator new` (con `static_cast<void*(*)(unsigned)>`) y `&CTask::operator new/delete` se pueden tomar como direcciones de funcion.
- El compilador emite la comprobacion de nulo tras `operator new` en `new CMonsterTruck(...)` etc. (no hay `/Zc:throwingNew` en CMake).
- Cada `Body()` de la DLL esta dentro de la DLL (se comprueba al empezar) y no hay dos colaboradores con el mismo cuerpo (nota `bodies compartidos`).
- `CEventDamage::AffectsPed` se obtiene de la ranura 5 de la vtable 0x85AD78 (como WeaponFinal2); con el hook VMT desactivado sale la direccion del EXE y no se parchea dos veces.
- Los offsets de `layout` (incluidos `CBike::m_BlowUpTimer` 0x7BC, `CAutomobile::m_fBurnTimer` 0x8E4, `bOnSideStand` en el byte 0x614 bit 0x10 y el escaner de peds en
  `CPedIntelligence+0x124+0x4C`) salen del ensamblador; si alguno falla el evento `layout` lo dice con su valor esperado.

## Aprobación (09/10/2026)

Corrida `20261009-200638-812639`: passed=true, 11840 pares, 72 funciones registradas con 0 fallos (HealthCheat, sin hook, probada pero no registrada), evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_cheatfinal3.py` (evidencia en `runs/`, `validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.
