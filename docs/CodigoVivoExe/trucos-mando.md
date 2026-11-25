# Trucos de PS2 con los botones del mando

Desviación propia **15.P** ([DesviacionesConocidas.md](../DesviacionesConocidas.md)): se pueden introducir los trucos clásicos de PS2 con el mando, como hace GInputSA 1.11 de Silent. Opción `GTASA_PAD_CHEATS=0|1` o `PadCheats=0|1` en `gta-reversed.ini` (por defecto **activada**; apagada, el código es el del EXE). Funciona igual en el ejecutable independiente y en la ASI (mismo código).

Atribución: lista de secuencias de botones de PS2 tomada de la documentación de GInputSA 1.11 de Silent (autorización solicitada por el usuario del proyecto); las secuencias son las de la versión PS2 del juego ([THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Solo se usó la documentación pública del paquete (`CheatList for PS.html`, `ReadMe.txt`); **no se estudió `GInputSA.asi`**: el comportamiento del detector (tiempos, umbrales, dos pads) es decisión propia de este proyecto, no la réplica del de GInputSA.

## Cómo funciona

- `source/game_sa/PadCheats.{h,cpp}`: `PadCheats::Detector` (buffer de las últimas 12 pulsaciones, coincidencia del sufijo con cada secuencia registrada) y `PadCheats::Process()`, llamado al final de `CCheat::DoCheats` (el bucle de teclado, `Game.cpp`, solo si el juego no está en pausa ni menú).
- Entrada: `CPad::Update` copia el `PCTempJoyState` de cada mando (sin teclado ni ratón) a `PadCheats::CaptureJoyState` antes de limpiarlo; es el estado ya procesado por `Pad.cpp` (DirectInput o XInput) en los campos de PS2 de `CControllerState` (`ButtonCross/Circle/Square/Triangle`, `LeftShoulder1/2`, `RightShoulder1/2`, `DPadUp/Down/Left/Right`, `ShockButtonL/R`). El teclado no puede introducir trucos por esta vía (sigue teniendo los suyos).
- Eventos por flanco de subida. Un valor (L2/R2 analógicos o cruceta) cuenta como pulsado desde 128 y como soltado por debajo de 64 (histéresis). Si varios botones suben en el mismo fotograma se anotan en el orden fijo L1 L2 R1 R2 ↑ ↓ ← → ✕ ○ □ △ L3 R3.
- Reinicio: 3 s (tiempo de juego) sin pulsar vacían el buffer; también tras una pausa/menú/carga (se saltan fotogramas de `DoCheats`) y al reconocer un truco (como la cadena del teclado). No se activa en cinemáticas (`CCutsceneMgr::ms_running`, como `AddToCheatString`).
- Dos mandos: cada uno tiene su propio buffer; un truco reconocido en cualquiera llama a la misma activación.
- La pulsación **no se consume**: el juego sigue viendo los botones (igual que con el teclado).
- Activación: `CCheat::ActivateCheatByIndex(id)`, extraída literal de `AddToCheatString` (mensaje «Cheat activated»/«deactivated» `CHEAT1`/`CHEAT8`, estadística de trucos, `m_bHasPlayerCheated`, función de la tabla o alternancia de `m_aCheatsActive`). No se duplica lógica de trucos. La tabla de `m_aCheatHashKeys` y la de funciones del EXE tienen los ids 0..91 en el orden de `eCheats` (comprobado: las cadenas de PC de la columna «Cadena PC» dan el hash de su id).

## Riesgo de falsos positivos

Las secuencias tienen de 7 a 12 botones y ninguna es sufijo de otra. Banco: 3 x 400 000 pulsaciones aleatorias (botones al azar, solo cruceta y gatillos, ritmo rápido) y 11 154 secuencias con un botón cambiado: 0 activaciones; y 300 pulsaciones aleatorias en fotogramas reales y 250 con el mando XInput falso: 0 activaciones. Aun así, con el mando en uso, quien repita justo una de esas combinaciones (p. ej. girar la cruceta mientras se aprieta L1/L2) activará el truco; con `GTASA_PAD_CHEATS=0` se anula. El mapeo de botones depende de la configuración de mando del juego (acciones asignadas a cada botón): con un mapeo personalizado los campos de PS2 pueden no corresponder al botón físico con el mismo nombre.

## Diferencias entre la lista y el juego (marcadas)

- **«Ninja Theme» y «Katana»** tienen la misma secuencia en la lista de GInputSA (`✕ ✕ ↓ R2 L2 ○ R1 ○ □`). El EXE no tiene un truco «Katana» aparte (la katana la da `NinjaCheat`): se tratan como alias de `NINJA_THEME` (id 44). No se pudo comparar con una PS2 real.
- **«Faster Cars»** (#51) repite la secuencia de **«Traffic Lights Green»** (#60) (`→ R1 ↑ L2 L2 ← R1 L1 R1 R1`). Una de las dos es un error de la lista; se registra solo «Traffic Lights Green» (id 30, `BGREEN_LIGHTS_CHEAT`) y la otra no activa nada. Probablemente «Faster Cars» sea la entrada del truco `CHEAP_TRAFFIC` (id 46) con la secuencia mal copiada; sin evidencia, el id 46 queda sin secuencia.
- **«Stormy»** (#85) se asocia al clima **Rainy** (id 9) por posición (los ids 6..10 terminan en △ ↓ □ ○ ✕); la lista no tiene «Rainy».
- Las secuencias de **Weapons set 1/2/3**, **Jetpack** y las demás se contrastaron con las de PS2 conocidas (set 1 = R1 R2 L1 R2 ← ↓ → ↑ ← ↓ → ↑; jetpack = ↑ ↓ ← → L1 L2 R1 R2 ↑ ↓ ← →): coinciden (anclas en el banco). El resto no se pudo contrastar con una PS2 real.
- Ids de `eCheats` sin secuencia en la lista (PS2 sin equivalente, o la lista no la trae): ver el final de la tabla.

## Mapeo (87 entradas de la lista de PS2)

Columnas: número en la lista, descripción, secuencia, id `eCheats` (orden del EXE), cadena de teclado de PC (calculada por hash contra la tabla del EXE; «—» = no localizada) y observaciones. Generada por `python contrib/pad_cheats_tabla.py` (también genera `PadCheatsTable.inc.h`).

| # | Descripción PS2 (lista de GInputSA) | Secuencia | Id `eCheats` | Cadena PC (teclado) | Mapeo |
| --- | --- | --- | --- | --- | --- |
| 1 | Weapons set 1 | R1 R2 L1 R2 ← ↓ → ↑ ← ↓ → ↑ | 0 `WEAPON_SET1` | LXGIWYL | ok |
| 2 | Weapons set 2 | R1 R2 L1 R2 ← ↓ → ↑ ← ↓ ↓ ← | 1 `WEAPON_SET2` | KJKSZPJ | ok |
| 3 | Weapons set 3 | R1 R2 L1 R2 ← ↓ → ↑ ← ↓ ↓ ↓ | 2 `WEAPON_SET3` | UZUMYMW | ok |
| 4 | $250000 & Full Health & Armor | R1 R2 L1 ✕ ← ↓ → ↑ ← ↓ → ↑ | 3 `HEALTH_ARMOR_250K` | HESOYAM | ok |
| 5 | Infinite Health | ↓ ✕ → ← → R1 → ↓ ↑ △ | 61 `INFINITE_HEALTH` | BAGUVIX | ok |
| 6 | Infinite Oxygen (Lung Capacity) | ↓ ← L1 ↓ ↓ R2 ↓ L2 ↓ | 62 `INFINITE_OXYGEN` | CVWKXAM | ok |
| 7 | Infinite Ammo | L1 R1 □ R1 ← R2 R1 ← □ ↓ L1 L1 | 72 `INFINITE_AMMO` | FULLCLIP | ok |
| 8 | Commit Suicide | → L2 ↓ R1 ← ← R1 L1 L2 L1 | 29 `SUICIDE` | SZCMAWO | ok |
| 9 | Adrenaline Mode | ✕ ✕ □ R1 L1 ✕ ↓ ← ✕ | 71 `ADRENALINE_MODE` | — | ok |
| 10 | Never Get Hungry | □ L2 R1 △ ↑ □ L2 ↑ ✕ | 68 `NEVER_GET_HUNGRY` | — | ok |
| 11 | Never Wanted | ○ → ○ → ← □ △ ↑ | 65 `I_DO_AS_I_PLEASE` | AEZAKMI | ok |
| 12 | 6 Star Wanted Level | ○ → ○ → ← □ ✕ ↓ | 66 `SIX_WANTED_STARS` | — | ok |
| 13 | Raise Wanted Level | R1 R1 ○ R2 ← → ← → ← → | 4 `WANTED_LEVEL_2STARS` | OSRBLHH | ok |
| 14 | Lower Wanted Level | R1 R1 ○ R2 ↑ ↓ ↑ ↓ ↑ ↓ | 5 `CLEAR_WANTED_LEVEL` | ASNAEB | ok |
| 15 | Recruit Anyone (9MM) | ↓ □ ↑ R2 R2 ↑ → → ↑ | 76 `WANNA_BE_IN_MY_GANG` | WANNABEINMYGANG | ok |
| 16 | Recruit Anyone (Rocket Launcher) | R2 R2 R2 ✕ L2 L1 R2 L1 ↓ ✕ | 78 `ROCKET_MAYHEM` | ROCKETMAYHEM | ok |
| 17 | Hitman Skill for all weapons | ↓ □ ✕ ← R1 R2 ← ↓ ↓ L1 L1 L1 | 82 `HITMAN_LEVEL_FOR_ALL_WEAPONS` | — | ok |
| 18 | Max Driving Skill | □ L2 ✕ R1 L2 L2 ← R1 → L1 L1 L1 | 83 `MAX_DRIVING_SKILLS` | NATURALTALENT | ok |
| 19 | Max Respect | L1 R1 △ ↓ R2 ✕ L1 ↑ L2 L2 L1 L1 | 79 `MAX_RESPECT` | — | ok |
| 20 | Max Sex Appeal | ○ △ △ ↑ ○ R1 L2 ↑ △ L1 L1 L1 | 80 `MAX_SEX_APPEAL` | — | ok |
| 21 | Max Stamina | ↑ ✕ △ ✕ △ ✕ □ R2 → | 81 `MAX_STAMINA` | VKYPQCF | ok |
| 22 | Really Fat CJ | △ ↑ ↑ ← → □ ○ ↓ | 36 `FAT_PLAYER` | BTCDBCB | ok |
| 23 | Really Skinny CJ | △ ↑ ↑ ← → □ ○ → | 38 `SKINNY_PLAYER` | LEANANDMEAN | ok |
| 24 | Really Muscled CJ | △ ↑ ↑ ← → □ ○ ← | 37 `MAX_MUSCLE` | JYSDSOD | ok |
| 25 | Gangs Control the Streets | L2 ↑ R1 R1 ← R1 R1 R2 → ↓ | 43 `GANGS_CONTROLS_THE_STREETS` | BIFBUZZ | ok |
| 26 | Gang Members Everywhere | ← → → → ← ✕ ↓ ↑ □ → ↓ | 42 `GANGMEMBERS_EVERYWHERE` | ONLYHOMIESALLOWED | ok |
| 27 | Dozer | R2 L1 L1 → → ↑ ↑ ✕ L1 ← | 87 `SPAWN_DOZER` | ITSALLBULL | ok |
| 28 | Jetpack | ↑ ↓ ← → L1 L2 R1 R2 ↑ ↓ ← → | 64 `GET_JETPACK` | ROCKETMAN | ok |
| 29 | Parachute | ← → L1 L2 R1 R2 R2 ↑ ↓ → L1 | 63 `GET_PARACHUTE` | AIYPWZQP | ok |
| 30 | Katana | ✕ ✕ ↓ R2 L2 ○ R1 ○ □ | 44 `NINJA_THEME` | NINJATOWN | alias de «Ninja Theme» (#65): misma secuencia y mismo truco; no se registra dos veces |
| 31 | Rhino | ○ ○ L1 ○ ○ ○ L1 L2 R1 △ ○ △ | 17 `SPAWN_RHINO` | AIWPRTON | ok |
| 32 | Rancher | ↑ → → L1 → ↑ □ L2 | 19 `SPAWN_RANCHER` | JQNTDMH | ok |
| 33 | Caddy | ○ L1 ↑ R1 L2 ✕ R1 L1 ○ ✕ | 25 `SPAWN_CADDY` | RZHSUEW | ok |
| 34 | Tanker | R1 ↑ ← → R2 ↑ → □ → L2 L1 L1 | 86 `SPAWN_TANKER_TRUCK` | AMOMHRER | ok |
| 35 | Hotring Racer 1 | R1 ○ R2 → L1 L2 ✕ ✕ □ R1 | 20 `SPAWN_HOTRINGA` | PDNEJOH | ok |
| 36 | Hotring Racer 2 | R2 L1 ○ → L1 R1 → ↑ ○ R2 | 21 `SPAWN_HOTRINGB` | VPJTQWV | ok |
| 37 | Bloodring Banger | ↓ R1 ○ L2 L2 ✕ R1 L1 ← ← | 18 `SPAWN_BLOODRINGBANGER` | OLDSPEEDDEMON | ok |
| 38 | Stretch | R2 ↑ L2 ← ← R1 L1 ○ → | 23 `SPAWN_STRETCH` | KRIJEBR | ok |
| 39 | Romero's Hearse | ↓ R2 ↓ R1 L2 ← R1 L1 ← → | 22 `SPAWN_ROMERO` | AQTBCODX | ok |
| 40 | Trashmaster | ○ R1 ○ R1 ← ← R1 L1 ○ → | 24 `SPAWN_TRASHMASTER` | TRUEGRIME | ok |
| 41 | Quad | ← ← ↓ ↓ ↑ ↑ □ ○ △ R1 R2 | 85 `SPAWN_QUAD` | FOURWHEELFUN | ok |
| 42 | Hydra | △ △ □ ○ ✕ L1 L1 ↓ ↑ | 50 `SPAWN_HYDRA` | JUMPJET | ok |
| 43 | Hunter | ○ ✕ L1 ○ ○ L1 ○ R1 R2 L2 L1 L1 | 84 `SPAWN_HUNTER` | OHDUDE | ok |
| 44 | Vortex | △ △ □ ○ ✕ L1 L2 ↓ ↓ | 51 `SPAWN_VORTEX` | — | ok |
| 45 | Monster Truck | → ↑ R1 R1 R1 ↓ △ △ ✕ ○ L1 L1 | 89 `SPAWN_MONSTER` | MONSTERMASH | ok |
| 46 | Stunt Plane | ○ ↑ L1 L2 ↓ R1 L1 L1 ← ← ✕ △ | 88 `SPAWN_STUNT_PLANE` | FLYINGTOSTUNT | ok |
| 47 | Cars float away when hit | □ R2 ↓ ↓ ← ↓ ← ← L2 ✕ | 54 `CARS_FLOAT_AWAY_WHEN_HIT` | — | ok |
| 48 | Traffic is country vehicles | L1 L1 R1 R1 L2 L1 R2 ↓ ← ↑ | 75 `COUNTRY_TRAFFIC` | — | ok |
| 49 | Invisible Cars | △ L1 △ R2 □ L1 L1 | 27 `INVISIBLE_CAR` | XICWMD | ok |
| 50 | Improved Handling (Hit R3 to jump) | △ R1 R1 ← R1 L1 R2 L1 | 28 `PERFECT_HANDLING` | PGGOMOY | ok |
| 51 | Faster Cars | → R1 ↑ L2 L2 ← R1 L1 R1 R1 | (ninguno) | (ninguna) | **duplicada dudosa**: repite la secuencia de «Traffic Lights Green» (#60); no se registra ni activa nada |
| 52 | Aggressive Traffic | R2 ○ R1 L2 ← R1 L1 R2 L2 | 31 `AGGRESSIVE_DRIVERS` | YLTEICZ | ok |
| 53 | Reduced Traffic | ✕ ↓ ↑ R2 ↓ △ L1 △ ← | 74 `REDUCED_TRAFFIC` | GHOSTTOWN | ok |
| 54 | Drive on Water | → R2 ○ R1 L2 □ R1 R2 | 34 `CARS_ON_WATER` | — | ok |
| 55 | All cars have Nitro | ← △ R1 L1 ↑ □ △ ↓ ○ L2 L1 L1 | 53 `ALL_CARS_HAVE_NITRO` | SPEEDFREAK | ok |
| 56 | Destroy all vehicles | R2 L2 R1 L1 L2 R2 □ △ ○ △ L2 L1 | 26 `BLOW_UP_ALL_CARS` | CPKTNWT | ok |
| 57 | Fast/Rare cars appear regularly | ↑ L1 R1 ↑ → ↑ ✕ L2 ✕ L1 | 47 `FAST_TRAFFIC` | EVERYONEISRICH | ok |
| 58 | Black Traffic | ○ L2 ↑ R1 ← ✕ R1 L1 ← ○ | 33 `BLACK_TRAFFIC` | IOWDLAC | ok |
| 59 | Pink Traffic | ○ L1 ↓ L2 ← ✕ R1 L1 → ○ | 32 `PINK_TRAFFIC` | LLQPFBN | ok |
| 60 | Traffic Lights Green | → R1 ↑ L2 L2 ← R1 L1 R1 R1 | 30 `BGREEN_LIGHTS_CHEAT` | ZEIIVG | ok |
| 61 | Cars Fly | □ ↓ L2 ↑ L1 ○ ↑ ✕ ← | 48 `CARS_FLY` | RIPAZHA | ok |
| 62 | Boats Fly | R2 ○ ↑ L1 → R1 → ↑ □ △ | 35 `BOATS_FLY` | AFSNMSMW | ok |
| 63 | Vehicle of Death | L1 L2 L2 ↑ ↓ ↓ ↑ R1 R2 R2 | 52 `SMASH_N_BOOM` | JCNRUAD | ok |
| 64 | Massive Bunny Hops | △ □ ○ ○ □ ○ ○ L1 L2 L2 R1 R2 | 49 `HUGE_BUNNY_HOP` | JHJOECW | ok |
| 65 | Ninja Theme | ✕ ✕ ↓ R2 L2 ○ R1 ○ □ | 44 `NINJA_THEME` | NINJATOWN | ok |
| 66 | Funhouse Theme | △ △ L1 □ □ ○ □ ↓ ○ | 70 `FUNHOUSE_THEME` | CRAZYTOWN | ok |
| 67 | Elvis Lives | L1 ○ △ L1 L1 □ L2 ↑ ↓ ← | 39 `ELVIS_IS_EVERYWHERE` | BLUESUEDESHOES | ok |
| 68 | Slut Magnet | □ → □ □ L2 ✕ △ ✕ △ | 45 `SLUT_MAGNET` | LOVECONQUERSALL | ok |
| 69 | Full Weapon Aiming Whilst Driving | ↑ ↑ □ L2 → ✕ R1 ↓ R2 ○ | 73 `WEAPON_AIMING_WHILE_DRIVING` | — | ok |
| 70 | Pedestrians Come After You | ↓ ↑ ↑ ↑ ✕ R2 R1 L2 L2 | 15 `HAVE_ABOUNTY_ON_YOUR_HEAD` | BAGOWPG | ok |
| 71 | Pedestrians Attack With Rocket Launchers | ✕ L1 ↑ □ ↓ ✕ L2 △ ↓ R1 L1 L1 | 40 `PEDS_ATTACK_YOU_WITH_ROCKETS` | BGLUAWML | ok |
| 72 | Pedestrians Riot | ↓ ← ↑ ← ✕ R2 R1 L2 L1 | 69 `RIOT_MODE` | STATEOFEMERGENCY | ok |
| 73 | Pedestrians Carry Weapons | R2 R1 ✕ △ ✕ △ ↑ ↓ | 16 `EVERYONE_ARMED` | FOOOXFT | ok |
| 74 | Faster Gameplay | △ ↑ → ↓ L2 L1 □ | 12 `FASTER_GAMEPLAY` | SPEEDITUP | ok |
| 75 | Slower Gameplay | △ ↑ → ↓ □ R2 R1 | 13 `SLOWER_GAMEPLAY` | SLOWITDOWN | ok |
| 76 | Jump High | ↑ ↑ △ △ ↑ ↑ ← → □ R2 R2 | 60 `MEGAJUMP` | KANGAROO | ok |
| 77 | Mega Punch | ↑ ← ✕ △ R1 ○ ○ ○ L2 | 67 `MEGA_PUNCH` | STINGLIKEABEE | ok |
| 78 | Beach Party | ↑ ↑ ↓ ↓ □ ○ L1 R1 △ ↓ | 41 `BEACH_PARTY` | LIFESABEACH | ok |
| 79 | Orange Sky | ← ← L2 R1 → □ □ L1 L2 ✕ | 56 `STOP_GAME_CLOCK_ORANGE_SKY` | — | ok |
| 80 | Speed Up Clock | ○ ○ L1 □ L1 □ □ □ L1 △ ○ △ | 11 `FASTER_CLOCK` | YSOHNUL | ok |
| 81 | Sunny | R2 ✕ L1 L1 L2 L2 L2 △ | 6 `SUNNY_WEATHER` | AFZLLQLL | ok |
| 82 | Really Sunny | R2 ✕ L1 L1 L2 L2 L2 ↓ | 7 `VERY_SUNNY_WEATHER` | ICIKPYH | ok |
| 83 | Cloudy | R2 ✕ L1 L1 L2 L2 L2 □ | 8 `OVERCAST_WEATHER` | ALNSFMZO | ok |
| 84 | Foggy | R2 ✕ L1 L1 L2 L2 L2 ✕ | 10 `FOGGY_WEATHER` | CFVFGMJ | ok |
| 85 | Stormy | R2 ✕ L1 L1 L2 L2 L2 ○ | 9 `RAINY_WEATHER` | — | dudoso: la lista la llama «Stormy»; por su posición (últimos botones △ ↓ □ ○ ✕ en los ids 6..10) es el clima **Rainy** (id 9); el id 57 (thunderstorm) queda sin secuencia |
| 86 | Sandstorm | ↑ ↓ L1 L1 L2 L2 L1 L2 R1 R2 | 58 `SAND_STORM` | CWJXUOC | ok |
| 87 | Always Midnight | □ L1 R1 → ✕ ↑ L1 ← ← | 55 `ALWAYS_MIDNIGHT` | NIGHTPROWLER | ok |

Ids de `eCheats` (0..91) sin secuencia en la lista de PS2: `PEDS_ATTACK_OTHER_WITH_GOLFCLUB` (14), `CHEAP_TRAFFIC` (46), `THUNDER_STORM` (57), `UNUSED` (59), `NO_ONE_CAN_STOP_US` (77), `PROSTITUTES_PAY_YOU` (90), `ALL_TAXIS_NITRO` (91).

## Pruebas

- `GTASA_PADCHEATS_SELFTEST=1` (banco en proceso, `PadCheatsSelfTest.cpp`, log `logs/padcheats_selftest.log`): fase A (detector puro: las 87 secuencias reconocidas con su truco, ninguna sufijo de otra, prefijos de «Weapons set 1/2/3», reinicio por tiempo, basura previa, 11 154 secuencias casi correctas, ruido), fase B (estados inyectados por `CCheat::DoCheats`, efecto por memoria con la opción apagada y encendida: armas 1/2/3 idénticas a la activación directa, dinero+salud+armadura, 6 climas, reloj, estrellas, vida infinita, resistencia, Rhino/Hotring/Caddy y los 24 trucos de simple alternancia) y fase C (fotogramas reales, escribiendo `CPad::PCTempJoyState`). Resultado: PASS=161 FALLO=0.
- Escenario `padcheats` de `reversing/FnJuego/sonda_juego.py` con el `xinput1_4.dll` falso (camino real `XInputPad` → DirectInput-compatible → `CPad`), encendido y con `GTASA_PAD_CHEATS=0` (botones XInput A/B/X/Y, LB/RB, LT/RT y cruceta → campos `Cross/Circle/Square/Triangle/L1/R1/L2/R2/DPad*` de `CPad::NewState`, comprobados uno a uno en memoria). Con la opción encendida: 18 de 18 (vida infinita, nunca buscado, respeto máximo, oxígeno infinito y no tener hambre se activan y se desactivan a la segunda; 5 climas; una pausa de 3,6 s a mitad de secuencia no activa; Rhino aparece; 250 pulsaciones aleatorias, 0 cambios; el teclado sigue funcionando) — evidencia `reversing/FnJuego/ejecuciones/pcxi_on3-*`. Con `GTASA_PAD_CHEATS=0`: ningún efecto (`pcxi_off-*`). Una ejecución anterior perdió una pulsación (un fotograma lento con pulsaciones de 0,18 s): la sonda reintenta una vez tras 3,3 s y lo anota (`intentos`); en la ejecución final todas salieron al primer intento.
- Ejecución del independiente de 120 s (`pc120-*`, escenario `reposo120`): vivo hasta el final; el log registra 1 excepción de arranque capturada en el controlador gráfico de Intel (`igd12um32xel!FreeLibrary`, 0,3 s tras cargar los textos), la misma que aparece en ejecuciones anteriores de otras ramas; no es de este código.
