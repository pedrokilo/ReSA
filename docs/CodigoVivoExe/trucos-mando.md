# Trucos de PS2 con los botones del mando

Desviación propia **15.P** ([DesviacionesConocidas.md](../DesviacionesConocidas.md)): se pueden introducir los trucos clásicos de PS2 con el mando, como hace GInputSA 1.11 de Silent. Opción `GTASA_PAD_CHEATS=0|1` o `PadCheats=0|1` en `gta-reversed.ini` (por defecto **activada**; apagada, el código es el del EXE). Funciona igual en el ejecutable independiente y en la ASI (mismo código). Equivale a la opción `CheatsFromPad=1` de `GInputSA.ini`.

Atribución: lista de secuencias de botones de PS2 y tabla de hashes tomadas de la documentación y de `GInputSA.asi` 1.11 de Silent (autorización del usuario del proyecto); las secuencias son las de la versión PS2 del juego ([THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Solo se usó la lógica de trucos del asi (análisis estático, sin ejecutarlo; catálogo general en [ginput/](ginput/)); la lectura de XInput de GInput **no** se porta: se usa nuestro `Pad.cpp`/`CPad`. El código es propio.

## Origen del comportamiento (análisis estático de `GInputSA.asi`)

El asi sustituye la llamada a `CCheat::DoCheats` en `CGame::Process` (0x53BFB8) por su función 0x10004950, que llama primero a la original (teclado) y después:

| Aspecto | GInputSA (asi) | Esta implementación |
| --- | --- | --- |
| Enganche | Después de `DoCheats`, una vez por fotograma de juego (no en pausa ni menú, porque `DoCheats` no se ejecuta) | `PadCheats::Process()` al final de `CCheat::DoCheats` — **del asi** |
| Cinemáticas | Sale si `CCutsceneMgr::ms_running` (el puntero que lee 0x438481) | Igual — **del asi** |
| Entrada | Máscara de 12 botones del `PCTempJoyState` del **mando 1** (solo el índice 0; el mando 2 no entra), copiada antes de que `CPad::Update` lo mezcle con el teclado | `CPad::Update` copia `PCTempJoyState` del pad 0 a `PadCheats::CaptureJoyState`; el pad 2 se ignora — **del asi** (es lo que arregló la v1.11 con 2 mandos) |
| Qué cuenta como pulsado | Cualquier valor **distinto de cero** (`setne`), también L2/R2 analógicos: sin umbral ni histéresis | Igual (`!= 0`) — **del asi** (mi versión anterior usaba 128/64) |
| Flanco | `pulsado ahora y no en la lectura anterior`; si suben varios botones en el mismo fotograma **solo se anota el primero** por prioridad △ ○ ✕ □ ↑ ↓ ← → L1 L2 R1 R2; los demás se pierden | Igual — **del asi** (antes los anotaba todos en un orden fijo) |
| Letras | △=`T` ○=`C` ✕=`X` □=`S` ↑=`U` ↓=`D` ←=`L` →=`R` L1=`1` L2=`2` R1=`3` R2=`4`. **L3/R3 no cuentan** | Igual — **del asi** |
| Historial | 12 letras, la más reciente primero (como `m_CheatString` de PC); se vacía al reconocer un truco | Igual — **del asi** |
| Reinicio por tiempo | **Ninguno**: el historial no tiene temporizador ni se vacía por pausa o menú | Ninguno (mi versión anterior reiniciaba a los 3 s) — **del asi** |
| Búsqueda | Como `AddToCheatString`: hash CRC32 (tabla estándar, sin pasar a mayúsculas, inicio 0xFFFFFFFF) de la cadena entera y luego recortada por el final hasta 6 letras, contra una tabla de **90 hashes** (`0x10032F00`); el índice es el id del truco | Igual, con `CKeyGen::GetKey` y la misma tabla (`PadCheatsTable.inc.h`, 90 hashes) — **del asi** |
| Activación | Texto GXT `CHEAT1` (activado) / `CHEAT8` (desactivado) como ayuda, sonido de pantalla, `m_bHasPlayerCheated = 1` y `ApplyCheat(id)` (0x438370); **no** incrementa la estadística «veces que has hecho trampas» | `CCheat::ActivateCheatByIndex(id)`, extraída literal de `AddToCheatString` (mensaje, **sí** incrementa la estadística, indicador, función o alternancia) — **decisión mía**: misma activación que el teclado y que la PS2; el sonido no se reproduce (la función de audio que usa el asi, `0x1003AD3C` con el evento 0x89, no se resolvió) |
| No consume la pulsación | Correcto: el asi no toca el estado del mando | Igual |
| Menús | No hace nada propio; no se ejecuta porque el juego en pausa no llama a `DoCheats` | Igual |
| Opción | `CheatsFromPad` (INI), por defecto 1 | `PadCheats` / `GTASA_PAD_CHEATS`, por defecto 1 — nombre **mío** |

## Dónde se engancha (nuestro código)

- `source/game_sa/PadCheats.{h,cpp}`: `PadCheats::Detector` (historial + hash), `MaskOf`/`FirstEdge` (máscara y primer flanco), `CaptureJoyState` y `Process`.
- `Cheat.cpp`: `CCheat::ActivateCheatByIndex` (cuerpo de activación de `AddToCheatString`, compartido con el teclado) y la llamada a `PadCheats::Process()` al final de `DoCheats`. `Pad.cpp`: la copia en `CPad::Update`.
- El mapeo de botones depende de la configuración de mando del juego (acciones asignadas a cada botón; mapa PS2 por defecto de `InitDefaultControlConfigJoyPad`): con un mapeo personalizado los campos de PS2 pueden no corresponder al botón físico del mismo nombre.

## Riesgo de falsos positivos

Al no haber reinicio por tiempo (como GInputSA), el historial de 12 botones se arrastra de una pulsación a otra: cualquier racha de 7 a 12 pulsaciones seguidas que coincida con una secuencia (con las pulsaciones intermedias que sean) activa el truco. Las secuencias son largas y poco naturales; banco: 2 x 600 000 pulsaciones aleatorias y 11 154 secuencias con un botón cambiado, 0 activaciones; 300 pulsaciones aleatorias en fotogramas reales y 250 con el mando XInput falso: 0 activaciones. Aun así, quien juegue con mando y repita justo una de esas combinaciones activará el truco; con `GTASA_PAD_CHEATS=0` se anula.

## Diferencias entre la lista HTML y el asi (resueltas con la tabla de hashes del asi)

Todas las secuencias de la lista HTML de GInputSA (87) se contrastaron calculando su hash con `CKeyGen` contra la tabla de 90 hashes del asi (el índice da el id del truco):

- **85 de 87 coinciden tal cual.** Dos difieren porque el HTML trae botones de más (solo el hash del asi coincide con la secuencia corregida): **Jetpack** son 10 botones, `← → L1 L2 R1 R2 ↑ ↓ ← →` (el HTML añade `↑ ↓` al principio; con el sufijo ambas teclean el mismo truco), y **Gang Members Everywhere** son 10 botones (el HTML añade un `↓` al final). La petición original traía la secuencia larga del jetpack: era la del HTML, no la del juego.
- **Ninja Theme y Katana**: misma secuencia y mismo hash (id 44): no es otro truco; el asi tiene una sola entrada (resuelto).
- **Faster Cars y Traffic Lights Green**: misma secuencia y mismo hash (id 30, `BGREEN_LIGHTS_CHEAT`): el asi tiene una sola entrada, así que «Faster Cars» es un error de etiqueta del HTML (resuelto: no hay un truco «faster cars» con esa secuencia).
- **«Stormy»** es el id 9 (`RAINY_WEATHER`) según el hash (resuelto: la etiqueta del HTML es incorrecta). **«Pedestrians Riot»** es el id **14** (`PEDS_ATTACK_OTHER_WITH_GOLFCLUB`), no el 69 (mi mapeo anterior se equivocaba).
- La tabla del asi tiene **5 hashes sin secuencia en el HTML**: ids 46 (`CHEAP_TRAFFIC`), 57 (`THUNDER_STORM`), 59 (`UNUSED`), 69 (`RIOT_MODE`) y 77 (`NO_ONE_CAN_STOP_US`). Se activan igualmente (el detector usa la tabla de hashes), pero no se pudo recuperar su secuencia: el hash CRC32 no se invierte y la búsqueda por fuerza bruta hasta 9 letras solo da colisiones al azar, y variaciones de una y dos letras de las 87 secuencias conocidas no coinciden. Los ids 90 y 91 (`PROSTITUTES_PAY_YOU`, `ALL_TAXIS_NITRO`) no tienen entrada en el asi (solo 90 hashes).
- Las secuencias de **Weapons set 1/2/3** coinciden con las de PS2 (set 1 = R1 R2 L1 R2 ← ↓ → ↑ ← ↓ → ↑).

## Mapeo (87 entradas de la lista de PS2)

Columnas: número en la lista, descripción, secuencia (con las dos correcciones del asi), id `eCheats` (índice de la tabla de hashes del asi = orden del EXE), cadena de teclado de PC (calculada por hash contra la tabla del EXE; «—» = no localizada) y observaciones. Generada por `python contrib/pad_cheats_tabla.py` (también genera `PadCheatsTable.inc.h`).

| # | Descripción PS2 (lista de GInputSA) | Secuencia | Id `eCheats` | Cadena PC (teclado) | Observaciones |
| --- | --- | --- | --- | --- | --- |
| 1 | Weapons set 1 | R1 R2 L1 R2 ← ↓ → ↑ ← ↓ → ↑ | 0 `WEAPON_SET1` | LXGIWYL | ok |
| 2 | Weapons set 2 | R1 R2 L1 R2 ← ↓ → ↑ ← ↓ ↓ ← | 1 `WEAPON_SET2` | KJKSZPJ | ok |
| 3 | Weapons set 3 | R1 R2 L1 R2 ← ↓ → ↑ ← ↓ ↓ ↓ | 2 `WEAPON_SET3` | UZUMYMW | ok |
| 4 | $250000 & Full Health & Armor | R1 R2 L1 ✕ ← ↓ → ↑ ← ↓ → ↑ | 3 `HEALTH_ARMOR_250K` | HESOYAM | ok |
| 5 | Infinite Health | ↓ ✕ → ← → R1 → ↓ ↑ △ | 61 `INFINITE_HEALTH` | BAGUVIX | ok |
| 6 | Infinite Oxygen (Lung Capacity) | ↓ ← L1 ↓ ↓ R2 ↓ L2 ↓ | 62 `INFINITE_OXYGEN` | CVWKXAM | ok |
| 7 | Infinite Ammo | L1 R1 □ R1 ← R2 R1 ← □ ↓ L1 L1 | 72 `INFINITE_AMMO` | FULLCLIP | ok |
| 8 | Commit Suicide | → L2 ↓ R1 ← ← R1 L1 L2 L1 | 29 `SUICIDE` | SZCMAWO | ok |
| 9 | Adrenaline Mode | ✕ ✕ □ R1 L1 ✕ ↓ ← ✕ | 71 `ADRENALINE_MODE` | MUNASEF | ok |
| 10 | Never Get Hungry | □ L2 R1 △ ↑ □ L2 ↑ ✕ | 68 `NEVER_GET_HUNGRY` | — | ok |
| 11 | Never Wanted | ○ → ○ → ← □ △ ↑ | 65 `I_DO_AS_I_PLEASE` | AEZAKMI | ok |
| 12 | 6 Star Wanted Level | ○ → ○ → ← □ ✕ ↓ | 66 `SIX_WANTED_STARS` | BRINGITON | ok |
| 13 | Raise Wanted Level | R1 R1 ○ R2 ← → ← → ← → | 4 `WANTED_LEVEL_2STARS` | OSRBLHH | ok |
| 14 | Lower Wanted Level | R1 R1 ○ R2 ↑ ↓ ↑ ↓ ↑ ↓ | 5 `CLEAR_WANTED_LEVEL` | ASNAEB | ok |
| 15 | Recruit Anyone (9MM) | ↓ □ ↑ R2 R2 ↑ → → ↑ | 76 `WANNA_BE_IN_MY_GANG` | WANNABEINMYGANG | ok |
| 16 | Recruit Anyone (Rocket Launcher) | R2 R2 R2 ✕ L2 L1 R2 L1 ↓ ✕ | 78 `ROCKET_MAYHEM` | ROCKETMAYHEM | ok |
| 17 | Hitman Skill for all weapons | ↓ □ ✕ ← R1 R2 ← ↓ ↓ L1 L1 L1 | 82 `HITMAN_LEVEL_FOR_ALL_WEAPONS` | PROFESSIONALKILLER | ok |
| 18 | Max Driving Skill | □ L2 ✕ R1 L2 L2 ← R1 → L1 L1 L1 | 83 `MAX_DRIVING_SKILLS` | NATURALTALENT | ok |
| 19 | Max Respect | L1 R1 △ ↓ R2 ✕ L1 ↑ L2 L2 L1 L1 | 79 `MAX_RESPECT` | OGXSDAG | ok |
| 20 | Max Sex Appeal | ○ △ △ ↑ ○ R1 L2 ↑ △ L1 L1 L1 | 80 `MAX_SEX_APPEAL` | EHIBXQS | ok |
| 21 | Max Stamina | ↑ ✕ △ ✕ △ ✕ □ R2 → | 81 `MAX_STAMINA` | VKYPQCF | ok |
| 22 | Really Fat CJ | △ ↑ ↑ ← → □ ○ ↓ | 36 `FAT_PLAYER` | BTCDBCB | ok |
| 23 | Really Skinny CJ | △ ↑ ↑ ← → □ ○ → | 38 `SKINNY_PLAYER` | LEANANDMEAN | ok |
| 24 | Really Muscled CJ | △ ↑ ↑ ← → □ ○ ← | 37 `MAX_MUSCLE` | JYSDSOD | ok |
| 25 | Gangs Control the Streets | L2 ↑ R1 R1 ← R1 R1 R2 → ↓ | 43 `GANGS_CONTROLS_THE_STREETS` | BIFBUZZ | ok |
| 26 | Gang Members Everywhere | ← → → → ← ✕ ↓ ↑ □ → | 42 `GANGMEMBERS_EVERYWHERE` | ONLYHOMIESALLOWED | el HTML trae 11 botones: el hash del asi solo coincide con esta secuencia de 10 |
| 27 | Dozer | R2 L1 L1 → → ↑ ↑ ✕ L1 ← | 87 `SPAWN_DOZER` | ITSALLBULL | ok |
| 28 | Jetpack | ← → L1 L2 R1 R2 ↑ ↓ ← → | 64 `GET_JETPACK` | ROCKETMAN | el HTML trae 12 botones: el hash del asi solo coincide con esta secuencia de 10 |
| 29 | Parachute | ← → L1 L2 R1 R2 R2 ↑ ↓ → L1 | 63 `GET_PARACHUTE` | AIYPWZQP | ok |
| 30 | Katana | ✕ ✕ ↓ R2 L2 ○ R1 ○ □ | 44 `NINJA_THEME` | NINJATOWN | ok |
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
| 44 | Vortex | △ △ □ ○ ✕ L1 L2 ↓ ↓ | 51 `SPAWN_VORTEX` | KGGGDKP | ok |
| 45 | Monster Truck | → ↑ R1 R1 R1 ↓ △ △ ✕ ○ L1 L1 | 89 `SPAWN_MONSTER` | MONSTERMASH | ok |
| 46 | Stunt Plane | ○ ↑ L1 L2 ↓ R1 L1 L1 ← ← ✕ △ | 88 `SPAWN_STUNT_PLANE` | FLYINGTOSTUNT | ok |
| 47 | Cars float away when hit | □ R2 ↓ ↓ ← ↓ ← ← L2 ✕ | 54 `CARS_FLOAT_AWAY_WHEN_HIT` | BSXSGGC | ok |
| 48 | Traffic is country vehicles | L1 L1 R1 R1 L2 L1 R2 ↓ ← ↑ | 75 `COUNTRY_TRAFFIC` | FVTMNBZ | ok |
| 49 | Invisible Cars | △ L1 △ R2 □ L1 L1 | 27 `INVISIBLE_CAR` | XICWMD | ok |
| 50 | Improved Handling (Hit R3 to jump) | △ R1 R1 ← R1 L1 R2 L1 | 28 `PERFECT_HANDLING` | PGGOMOY | ok |
| 51 | Faster Cars | → R1 ↑ L2 L2 ← R1 L1 R1 R1 | 30 `BGREEN_LIGHTS_CHEAT` | ZEIIVG | ok |
| 52 | Aggressive Traffic | R2 ○ R1 L2 ← R1 L1 R2 L2 | 31 `AGGRESSIVE_DRIVERS` | YLTEICZ | ok |
| 53 | Reduced Traffic | ✕ ↓ ↑ R2 ↓ △ L1 △ ← | 74 `REDUCED_TRAFFIC` | GHOSTTOWN | ok |
| 54 | Drive on Water | → R2 ○ R1 L2 □ R1 R2 | 34 `CARS_ON_WATER` | — | ok |
| 55 | All cars have Nitro | ← △ R1 L1 ↑ □ △ ↓ ○ L2 L1 L1 | 53 `ALL_CARS_HAVE_NITRO` | SPEEDFREAK | ok |
| 56 | Destroy all vehicles | R2 L2 R1 L1 L2 R2 □ △ ○ △ L2 L1 | 26 `BLOW_UP_ALL_CARS` | CPKTNWT | ok |
| 57 | Fast/Rare cars appear regularly | ↑ L1 R1 ↑ → ↑ ✕ L2 ✕ L1 | 47 `FAST_TRAFFIC` | EVERYONEISRICH | ok |
| 58 | Black Traffic | ○ L2 ↑ R1 ← ✕ R1 L1 ← ○ | 33 `BLACK_TRAFFIC` | IOWDLAC | ok |
| 59 | Pink Traffic | ○ L1 ↓ L2 ← ✕ R1 L1 → ○ | 32 `PINK_TRAFFIC` | LLQPFBN | ok |
| 60 | Traffic Lights Green | → R1 ↑ L2 L2 ← R1 L1 R1 R1 | 30 `BGREEN_LIGHTS_CHEAT` | ZEIIVG | misma secuencia (y mismo hash) que «Faster Cars»: no es otro truco |
| 61 | Cars Fly | □ ↓ L2 ↑ L1 ○ ↑ ✕ ← | 48 `CARS_FLY` | RIPAZHA | ok |
| 62 | Boats Fly | R2 ○ ↑ L1 → R1 → ↑ □ △ | 35 `BOATS_FLY` | AFSNMSMW | ok |
| 63 | Vehicle of Death | L1 L2 L2 ↑ ↓ ↓ ↑ R1 R2 R2 | 52 `SMASH_N_BOOM` | JCNRUAD | ok |
| 64 | Massive Bunny Hops | △ □ ○ ○ □ ○ ○ L1 L2 L2 R1 R2 | 49 `HUGE_BUNNY_HOP` | JHJOECW | ok |
| 65 | Ninja Theme | ✕ ✕ ↓ R2 L2 ○ R1 ○ □ | 44 `NINJA_THEME` | NINJATOWN | misma secuencia (y mismo hash) que «Katana»: no es otro truco |
| 66 | Funhouse Theme | △ △ L1 □ □ ○ □ ↓ ○ | 70 `FUNHOUSE_THEME` | PRIEBJ | ok |
| 67 | Elvis Lives | L1 ○ △ L1 L1 □ L2 ↑ ↓ ← | 39 `ELVIS_IS_EVERYWHERE` | BLUESUEDESHOES | ok |
| 68 | Slut Magnet | □ → □ □ L2 ✕ △ ✕ △ | 45 `SLUT_MAGNET` | LOVECONQUERSALL | ok |
| 69 | Full Weapon Aiming Whilst Driving | ↑ ↑ □ L2 → ✕ R1 ↓ R2 ○ | 73 `WEAPON_AIMING_WHILE_DRIVING` | OUIQDMW | ok |
| 70 | Pedestrians Come After You | ↓ ↑ ↑ ↑ ✕ R2 R1 L2 L2 | 15 `HAVE_ABOUNTY_ON_YOUR_HEAD` | BAGOWPG | ok |
| 71 | Pedestrians Attack With Rocket Launchers | ✕ L1 ↑ □ ↓ ✕ L2 △ ↓ R1 L1 L1 | 40 `PEDS_ATTACK_YOU_WITH_ROCKETS` | BGLUAWML | ok |
| 72 | Pedestrians Riot | ↓ ← ↑ ← ✕ R2 R1 L2 L1 | 14 `PEDS_ATTACK_OTHER_WITH_GOLFCLUB` | AJLOJYQY | ok |
| 73 | Pedestrians Carry Weapons | R2 R1 ✕ △ ✕ △ ↑ ↓ | 16 `EVERYONE_ARMED` | FOOOXFT | ok |
| 74 | Faster Gameplay | △ ↑ → ↓ L2 L1 □ | 12 `FASTER_GAMEPLAY` | SPEEDITUP | ok |
| 75 | Slower Gameplay | △ ↑ → ↓ □ R2 R1 | 13 `SLOWER_GAMEPLAY` | SLOWITDOWN | ok |
| 76 | Jump High | ↑ ↑ △ △ ↑ ↑ ← → □ R2 R2 | 60 `MEGAJUMP` | LFGMHAL | ok |
| 77 | Mega Punch | ↑ ← ✕ △ R1 ○ ○ ○ L2 | 67 `MEGA_PUNCH` | STINGLIKEABEE | ok |
| 78 | Beach Party | ↑ ↑ ↓ ↓ □ ○ L1 R1 △ ↓ | 41 `BEACH_PARTY` | LIFESABEACH | ok |
| 79 | Orange Sky | ← ← L2 R1 → □ □ L1 L2 ✕ | 56 `STOP_GAME_CLOCK_ORANGE_SKY` | OFVIAC | ok |
| 80 | Speed Up Clock | ○ ○ L1 □ L1 □ □ □ L1 △ ○ △ | 11 `FASTER_CLOCK` | YSOHNUL | ok |
| 81 | Sunny | R2 ✕ L1 L1 L2 L2 L2 △ | 6 `SUNNY_WEATHER` | AFZLLQLL | ok |
| 82 | Really Sunny | R2 ✕ L1 L1 L2 L2 L2 ↓ | 7 `VERY_SUNNY_WEATHER` | ICIKPYH | ok |
| 83 | Cloudy | R2 ✕ L1 L1 L2 L2 L2 □ | 8 `OVERCAST_WEATHER` | ALNSFMZO | ok |
| 84 | Foggy | R2 ✕ L1 L1 L2 L2 L2 ✕ | 10 `FOGGY_WEATHER` | CFVFGMJ | ok |
| 85 | Stormy | R2 ✕ L1 L1 L2 L2 L2 ○ | 9 `RAINY_WEATHER` | — | ok |
| 86 | Sandstorm | ↑ ↓ L1 L1 L2 L2 L1 L2 R1 R2 | 58 `SAND_STORM` | CWJXUOC | ok |
| 87 | Always Midnight | □ L1 R1 → ✕ ↑ L1 ← ← | 55 `ALWAYS_MIDNIGHT` | NIGHTPROWLER | ok |

Ids de `eCheats` con hash en la tabla del asi pero **sin secuencia en la lista HTML** (el asi los activa; su secuencia no se pudo recuperar, ver el texto): `CHEAP_TRAFFIC` (46), `THUNDER_STORM` (57), `UNUSED` (59), `RIOT_MODE` (69), `NO_ONE_CAN_STOP_US` (77). Ids 90 (`PROSTITUTES_PAY_YOU`) y 91 (`ALL_TAXIS_NITRO`): sin entrada en el asi.

## Pruebas

- `GTASA_PADCHEATS_SELFTEST=1` (banco en proceso, `PadCheatsSelfTest.cpp`, log `logs/padcheats_selftest.log`): fase A (detector puro: las 87 secuencias reconocidas con el id que da la tabla de hashes calculada aparte, ninguna sufijo de otra, prefijos de «Weapons set 1/2/3», basura previa, 11 154 secuencias casi correctas, ruido, un flanco por fotograma por prioridad y «distinto de cero»), fase B (estados inyectados por `CCheat::DoCheats`, efecto por memoria con la opción apagada y encendida: armas 1/2/3 idénticas a la activación directa, dinero+salud+armadura, 6 climas, reloj, estrellas, vida infinita, resistencia, Rhino/Hotring/Caddy y los trucos de simple alternancia) y fase C (fotogramas reales, escribiendo `CPad::PCTempJoyState`).
- Escenario `padcheats` de `reversing/FnJuego/sonda_juego.py` con el `xinput1_4.dll` falso (camino real `XInputPad` → `CPad`), encendido y con `GTASA_PAD_CHEATS=0`.
