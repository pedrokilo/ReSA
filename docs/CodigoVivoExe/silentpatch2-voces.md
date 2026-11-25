# SilentPatch, 2.ª tanda: voces y diálogos (rama `silentpatch2-voces`, 25/11/2026)

Port de las correcciones de voces y diálogos de SilentPatch para San Andreas (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent); copia de solo lectura en `D:\Original\ref_mods\silentpatch\SilentPatch-dev`, ver [THIRD_PARTY_NOTICES](../THIRD_PARTY_NOTICES.md)) a nuestro código C++ (`source/game_sa/`). El mismo código sirve de ASI sobre el EXE original y del standalone. Todo son **desviaciones deliberadas** y opcionales (sección 15.7 de [DesviacionesConocidas.md](../DesviacionesConocidas.md)); con la opción apagada el comportamiento es el del EXE.

* Una fila por viñeta con la función de SilentPatch, la nuestra, la opción y el estado: [silentpatch-clasificacion/voces.csv](silentpatch-clasificacion/voces.csv).
* Cada entrada de tabla que cambia SilentPatch, comparada contra las tablas del EXE original: [silentpatch-clasificacion/voces-tablas.csv](silentpatch-clasificacion/voces-tablas.csv) (58 entradas; las genera `contrib/silentpatch_voces/analiza_voces.py` leyendo el `gta_sa.exe` 1.0 US y `SpeechContextsSA.h`).

## Alcance (lectura completa de CHANGELOG-SA.md, 226 líneas)

Mi bloque son las 22 viñetas L125-L146 (Speech Context; confirmado leyendo el changelog entero). Además, por tema: L62 (prostitutas calladas; su causa es el `rand` de 15 bits, la porta la rama `silentpatch2-peds` con la opción `ConsoleRandomness`, no se duplica), L184 (Temple y Queens en el escáner de policía) y L202 (frases de los pasajeros al atropellar). Revisadas y fuera de este alcance: L33 (fuga 1.01 al cambiar de emisora: clase C), L91-L92 (animación de cambio de emisora: vehículos), L95-L96 y L119 (policías motorizados y armas de los policías: peds), L148 (nombre de la emisora: primera tanda), L181-L183 y L220 (pistas de usuario MP3/WAV/FLAC: entrada/audio de otra rama).

## Cómo están hechos los datos

Las tablas de voz del EXE (contextos globales 0x8C6A68; rangos por [contexto específico][voz] de GEN 0x8AF8A8, EMG 0x8BA4D0, jugador 0x8BBEF8, pandillas 0x8BE620, novias/tenderos 0x8C42B0; peds especiales 0x8C65C0/0x8C6948; nombres del escáner 0x8C8198) **no se modifican**: `CAEPedSpeechAudioEntity` y `CAEPoliceScannerAudioEntity` las siguen leyendo igual (ASI y standalone, que lleva la imagen de datos en las mismas direcciones), pero cada consulta pasa por las funciones de `source/game_sa/Audio/Entities/SilentPatchSpeech.{h,cpp}`, que llevan **solo las entradas que cambia SilentPatch** como tablas C++ (`kGlobalRules`: 35 asignaciones de contexto específico a contexto global que en el EXE valen -1; `kMoveRules`: 10 movimientos «contexto con errata -> contexto correcto» por voz) y devuelven el dato del EXE tal cual cuando la opción está apagada o la entrada no cambia. La condición de cada cambio es la de SilentPatch (`setSpecificSpeechContext` solo asigna si el EXE tiene -1; `tryMoveContext` solo mueve si el origen tiene sonidos y el destino está vacío, y deja vacío el origen).

Diferencias de datos encontradas (todas con su viñeta en `voces-tablas.csv`):

| Grupo | Entradas | Viñeta | Qué cambia |
| --- | --- | --- | --- |
| Tabla global, jugador | 12 | L129 | `TAKE_TURF_*` global sin contexto de CJ -> `CTX_PLY_TAKE_TURF_*` |
| Tabla global, novias/tenderos | 18 | L131 | `CONV_IGNORED`, `GUN_COOL`, `LIKE_DISMISS_REPLY`, 12 `PCONV_*`, `SAVED`, `SHOCKED`, `TRAPPED`, `GUN_RUN` -> contextos `CTX_GFD_*` |
| Tabla global, `SHOOT_GENERIC` | 2 | (sin viñeta; lo habilita otro mod) | peatones y novias/tenderos |
| Tabla global, bandas | 3 | L134 | `ABUSE_TRIAD`, `ABUSE_RIFA`, `ABUSE_DA_NANG` -> `CTX_GNG_ABUSE_GANG_*` |
| GEN, voz 141 `VOICE_GEN_VBFYST2` | 4 | L127 | sus sonidos están en `CHA`, `RUN_FROM_FIGH`, `SAVE`, `SHOCKE` (nombres cortados): se mueven a `CHAT`, `RUN_FROM_FIGHT`, `SAVED`, `SHOCKED` |
| GEN, voz 153 `VOICE_GEN_VWMOTR1` | 1 | L133 | `MUGGING` -> `MUGGED` |
| EMG, voces 27 y 28 (`MCOP2`, `MCOP3`; modelo LAPDM1) | 2 | L142 | `ARREST_CRIMINAL` -> `ARREST_CRIM` |
| GFD, voces 1, 2, 3, 13, 14 (`BMOBAR`, `BMYBARB`, `BMYTATT`, `WMYAMMO`, `WMYBARB`) | 5 | L132 | `SHOOT_GENERIC` -> `SHOP_CLOSED` |
| GFD, voz 9 (`MILLIE`) | 1 | L131 | `CAR_ON_FIRE` -> `CAR_FIRE` |
| Jugador, voces `AG`, `AG2`, `CD`, `CD2`, `CG`, `CG2`, `WG`, `WG2` | 8 | L135 | `JACKING_CAR_FEMALE` -> `JACKING_CAR_FEM` |
| Pandillas, voz 16 `VOICE_GNG_FAM5` (modelo FAM3) | 1 | L141 | `ATTACK_PLAYER_LIKE` -> `ATTACK_BY_PLAYER_LIKE` |
| Peds especiales | 1 | L127 | el modelo `maccer` no está en la tabla del EXE (solo `macca`): voz `VOICE_GNG_MACCER`, tipo pandilla |
| Escáner de policía | 2 | L184 | la etiqueta del índice 113 («????») pasa a `WESTP` (Queens) y la del 134 («SUN», duplicada con la 135) pasa a «????» |

Los valores numéricos de todas las enumeraciones de contexto y voz de SilentPatch coinciden con los nuestros (1147 nombres de SilentPatch comparados con los 877 de `PedSpeechContexts.h`: 0 valores distintos).

Los nombres de voz del `peds.ide` con errata (`VOICE_GEN_BMYPI` de BMYPIMP y `VOICE_GEN_WMYSGRAD` de WMYSGRD, que en el EXE son `VOICE_GEN_BYMPI` y `VOICE_GEN_WMYSGRD`; SBFYSTR y SWFYSTR traen `VOICE_GEN_SBFYST`/`VOICE_GEN_SWFYST` en vez de `...STR`) se corrigen al cargar el `peds.ide` en `CFileLoader::LoadPedObject`.

## Cambios de lógica (viñeta -> sitio)

| L | Función nuestra (dirección EXE) | Cambio |
| --- | --- | --- |
| 125 | `CPedToPlayerConversations::Update` (0x43B0F0): sorteo de tema 0..7 (el EXE 0..6; `push 7` en 0x43B341), respuesta de CJ a un comentario negativo sobre el tiempo = `WEATHER_DISL_REPLY`; `CAEPedSpeechAudioEntity::AddSayEvent` (0x4E6550): si CJ no tiene respuesta de tiempo se reintenta fingiendo el humor CD (solo CD y CF las tienen) | `SpeechWeatherConversations` |
| 126 | `CEventHandler::ComputePedToFleeResponse` (0x4B9B50): `CTaskComplexSmartFleeEntity` con `scream=false` | `SpeechCriminalsFlee` |
| 127, 128 | `CFileLoader::LoadPedObject` (0x5B7420), `GetVoiceAndTypeForSpecialPed` (0x4E4170) y las consultas de rango | `SpeechPedVoiceNames`, `SpeechVBFYST2`, `SpeechMaccerVoice` |
| 129 | `CGangWars::CheerVictory` (0x444040): la celebra CJ (no el miembro más cercano) y la etiqueta comparada es la de texto de la zona (+8), no la de información (+0) | `SpeechTurfCheers` |
| 130 | `CTaskComplexProstituteSolicit::ControlSubTask` (0x6669D0): `Say(AFTER_SEX)` y después `SetCJMood(WR)` | `SpeechSolicitMood` |
| 134 | `CTaskComplexGangLeader::DoGangAbuseSpeech` (0x65E860): caso LSV (`ABUSE_GANG_LSV`) | `SpeechGangTaunts` |
| 136 | `CAutomobile::VehicleDamage` (0x6A7650) y `CBike::VehicleDamage` (0x6B8EC0): si `Say` falla se prueba `CRASH_GENERIC` | `SpeechCrashGenericFallback` |
| 137 | `CPedDamageResponseCalculator::ComputeWillKillPed` (0x4B3210): `PAIN_SPRAYED` con teargas, spray y extintor (no si el golpe es con el bote, `PISTOLWHIP`) | `SpeechPainSprayed` |
| 138 | `CTaskComplexArrestPed::ControlSubTask` (0x68D350): tras `SOLO` del policía, el arrestado a pie dice `CHASED` (retardo 3500 ms, probabilidad 0,6) | `SpeechChasedOneStar` |
| 139 | `CWeapon::Fire` (0x742300): sin objetivo se usa el peatón fijado con el ratón (`CPlayerPed::m_p3rdPersonMouseTarget`, +0x79C) | `SpeechShootMouseKeyboard` |
| 140 | `CTaskComplexKillPedOnFootArmed` (4 sitios, 0x627587, 0x62C96C, 0x62D21B, 0x62CF5A): «tiene grupo» para los policías = más de uno persiguiendo | `SpeechCopsCoverLines` |
| 143 | `CTaskComplexKillPedOnFoot::SayGangAttack` (0x65E9A0): el pandillero que ataca al jugador dice `ATTACK_PLAYER` | `SpeechGangAttackPlayer` |
| 144 | `CPedToPlayerConversations::Update`: tras `WHERE_YOU_FROM_POS` (y tras el ataque en `GANGBANG_YES`) no se cierra: estado `WAITINGFORFINALWORD` y, a los 3 s, el pandillero dice `WHERE_YOU_FROM_POS_REPLY` (en el tema de la banda no dice nada) | `SpeechWhereYouFromReply` |
| 145 | `CCrime::ReportCrime` (0x532010): la víctima «huye de la policía» si su evento actual es `EVENT_PED_TO_FLEE` de un policía (la bandera heredada `bBeingChasedByPolice` no se pone nunca); se da una sola vez, el policía dice `GOOD_CITIZEN`, texto de respaldo si falta `GOODBOY` | `GoodCitizenBonus` |
| 146 | `CPlayerPed::MakeThisPedJoinOurGroup` (0x60C840): el `DRUGGED_IGNORE` lo dice el reclutado, no CJ | `SpeechDruggedRecruit` |
| 184 | `CAEPoliceScannerAudioEntity::AddAudioEvent` (0x4E71E0): `ScannerLabel` | `PoliceScannerNames` |
| 202 | `CAutomobile::VehicleDamage`: sin `CAR_HIT_PED` al empujar; `CPed::KillPedWithCar` (0x5F0360): el pasajero dice `CAR_HIT_PED` (o `TAXI_HIT_PED`) al atropellar con el coche del jugador | `SpeechCarHitPedPassenger` |

Todos los sitios llevan el comentario `// SilentPatch (MIT, Silent): <arreglo>`. Valor por defecto de todas las opciones: activadas (SilentPatchSA.ini no tiene opciones de voz; ninguna cambia el ritmo del juego). Interruptor general `GTASA_SILENTPATCH=0|1`; por opción `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0|1` o `SilentPatch_<Nombre>=0|1` en `gta-reversed.ini`.

## Pruebas

Las pruebas se hicieron con el standalone enlazado desde esta rama (`reversing\FnFase8\salida\gta_sa_standalone.exe`, `.obj` de `wv-int\build` más los `over\*.obj` de los 19 archivos tocados) y con el ASI de la rama para el humo. Registros copiados a [silentpatch-clasificacion/evidencia-voces/](silentpatch-clasificacion/evidencia-voces/).

### 1. Banco en proceso (`GTASA_SILENTPATCH_SELFTEST_VOCES=1`, `logs/silentpatch_voces_selftest.log`)

`SilentPatchSpeech::AutoTest`, llamado desde `CAEPedSpeechAudioEntity::StaticInitialise`, **reimplementa de forma independiente** el efecto de SilentPatch (copias locales de las cinco tablas de rangos y de la tabla global leídas del EXE, y el código de `tryMoveContext`/`setSpecificSpeechContext` portado literalmente) y lo compara con lo que devuelven las funciones del juego (`ProbeSoundRange` sobre `ExeRange`/`ExeGenRange`, `GetSpecificSpeechContext`) con la opción **apagada** (debe ser el dato del EXE) y **encendida** (debe ser el de SilentPatch), para todos los contextos y todas las voces:

| Tabla | Entradas comparadas | Entradas que SilentPatch cambia | Diferencias (apagada / encendida) |
| --- | --- | --- | --- |
| GEN (209 voces) | 21 736 | 10 | 0 / 0 |
| EMG | 1 610 | 4 | 0 / 0 |
| Jugador | 2 240 | 16 | 0 / 0 |
| Pandillas | 5 876 | 2 | 0 / 0 |
| Novias y tenderos | 2 178 | 12 | 0 / 0 |
| Global (contexto × tipo de voz) | 1 800 | 35 | 0 / 0 |

Extremo a extremo (`GetSoundAndBankIDs` con una entidad de pruebas, opción apagada / encendida): VBFYST2 `CHAT`, `RUN_FROM_FIGHT`, `SAVED`, `SHOCKED` (mudo / suena, rangos 0-4, 5-9, 10-11, 12-13), VWMOTR1 `MUGGED` (mudo / suena, 118-120), LAPDM1 `ARREST_CRIM` (voz `MCOP2`, mudo / suena), FAM3 `ATTACK_BY_PLAYER_LIKE` (voz `FAM5`, mudo / suena), tenderos `SHOP_CLOSED` (mudo / suena), `CAR_FIRE` de MILLIE (mudo / suena). Las 22 asignaciones de contexto global sin usar que tienen voces con datos pasan de mudo a sonar con la opción encendida (las 18 de novias/tenderos, `SHOOT_GENERIC` de peatones con la voz 75 y las 3 de bandas: Triadas con la voz 7, Rifa con la 7 y Da Nang Boys con la 27); la única que sigue muda es `SHOOT_GENERIC` de novias/tenderos, porque SilentPatch mueve sus datos a `SHOP_CLOSED` (efecto esperado). Cuántas voces tienen datos en cada contexto: [voces-cobertura.csv](silentpatch-clasificacion/voces-cobertura.csv).

Otros resultados del banco: `maccer` sin voz / con voz 23 de pandilla (mujer 0), `maddogg` sin voz en ambos casos y `macca` igual en ambos; `GetVoice("VOICE_GEN_BMYPI")` -1 / 33 y `GetVoice("VOICE_GEN_WMYSGRAD")` -1 / 203; `FixPedModelVoiceNames` convierte `VOICE_GEN_SBFYST` en `...SBFYSTR` (y SWFYST en SWFYSTR) solo para esos dos modelos; `TurfContext` con zonas de pruebas (la etiqueta de información «CHC» con texto «XXX» dice `TAKE_TURF_LAS_COLINAS` apagada y nada encendida; con texto «CHC» e información «XXX» ocurre al revés); escáner de policía índices 113 («????» -> «WESTP») y 134 («SUN» -> «????»), índices 112 y 135 sin cambios.

### 2. Banco en juego (`GTASA_SILENTPATCH_SELFTEST_VOCES=2`, `logs/silentpatch_voces_juego.log`)

Con el jugador ya en la calle (`ejecutar_juego.ps1 -Escenario base,armas`), cada acción con la opción apagada y encendida en el mismo proceso:

* **Voces de los modelos del `peds.ide`** (se leen del `ModelInfo` cargado; ejecuciones con SilentPatch encendido y con `GTASA_SILENTPATCH=0`): WMYSGRD (modelo 71) voz -1 -> 203; BMYPIMP (249) -1 -> 33; SBFYSTR (256) 93 -> 94; SWFYSTR (257) 125 -> 126; VBFYST2 (244) 141 en ambos (la voz ya era correcta: lo que cambia son sus contextos).
* **CJ dice frases por el camino completo** (`CPed::Say` -> `AddSayEvent` -> `GetSoundAndBankIDs` -> hueco de voz): `WEATHER_DISL_REPLY` y `WEATHER_LIKE_REPLY` con humor CR: mudo / suena (reintento con el humor CD); `TAKE_TURF_GANTON` y `TAKE_TURF_TEMPLE`: mudo / suena; `JACKING_CAR_FEM` con humor CG y WG: sin la corrección CJ usa la voz del humor CR (índice de sonido hasta 8), con ella su propio rango (0-4); rangos de CJ de las voces AG, CD, CG y WG: (-1,-1) -> (62,66), (147,152), (173,177), (78,81); la voz CR no cambia.
* **`CGangWars::CheerVictory`** con una zona de pruebas (etiqueta de texto «CHC»): CJ habla 0 / 1 (sin SilentPatch no hay miembros de grupo cerca y no dice nada).
* **Conversación «de dónde eres»** (se pone a un peatón de la calle en `PEDHASOPENED`, tema 8, apertura positiva, y se pulsa «sí» en el pad 0): apagada -> `INACTIVE` (se cierra); encendida -> `WAITINGFORFINALWORD` y, a los 3,5 s, `WAITINGTOFINISH` tras decir la respuesta.
* **Good Citizen Bonus** (víctima con un evento `EVENT_PED_TO_FLEE` de un policía y el jugador la golpea con `CCrime::ReportCrime`): apagada +0 $; encendida +50 $ al primer golpe (bandera «bonus dado» puesta) y +0 $ al segundo.

Con el interruptor general apagado (`GTASA_SILENTPATCH=0`) el standalone arranca y juega igual (150 s, sin excepciones), y el banco en proceso sigue dando 0 diferencias con las opciones forzadas. WASAPI (`audio_wasapi.txt`) registra picos de audio reales del proceso (por ejemplo 0,0759) durante las ejecuciones; no se aisló qué frase suena en cada pico: la comprobación de «suena» es la del propio motor (`AddSayEvent` devuelve el sonido elegido y ocupa el hueco de voz).

### 3. Humo con el ASI de la rama (desde `wv-8\reversing\FnBIS1`, `AGUA_ASI=<ASI de wv-spo>`)

`python prueba_ropero.py cal_mal.args <etiqueta>` y `python prueba_marcadores.py cal_mal.args <etiqueta>`, con SilentPatch encendido (por defecto) y apagado (`GTASA_SILENTPATCH=0`), con el ASI reenlazado con los `.obj` definitivos de la rama (`D:\Original\_humo_spo5.log`, copia en la carpeta de evidencias): las cuatro dieron **VEREDICTO BIEN**.

| Prueba | SilentPatch | Resultado |
| --- | --- | --- |
| prueba_ropero | encendido | VEREDICTO BIEN (primer intento) |
| prueba_marcadores | encendido | VEREDICTO BIEN (segundo intento) |
| prueba_ropero | apagado | VEREDICTO BIEN (primer intento) |
| prueba_marcadores | apagado | VEREDICTO BIEN (primer intento) |

Antes, con un ASI anterior al último arreglo (reintento con el humor CD en `AddSayEvent`), ya habían dado BIEN las mismas cuatro pruebas (`_humo_spo.log`, `_humo_spo2.log`, `_humo_spo4.log`). Varios intentos terminaron en «sin juego» o «el juego se cerró» (tanto con SilentPatch encendido como apagado): coinciden con ejecuciones de otras ramas que usan la misma carpeta de pruebas (en `FnBIS1\salidas` hay carpetas `Xspe_*` de la rama de entrada creadas en el mismo segundo). Se repitieron con el candado libre y no se contaron ni como fallo ni como éxito hasta que dieron veredicto (en `humo_spo5` el único intento caído fue el primero de marcadores encendido).

### 4. Ejecución del standalone y auditorías

* Ejecuciones de 150 s del standalone (`fase8_arranque.log`; carpetas `voces4_*` y `voces5_*` de `reversing/FnJuego/ejecuciones`): con SilentPatch encendido `gGameState` 9, `codigo-original-distinto=0 hits=0` y `excepciones=0` (voces4 y voces5); con `GTASA_SILENTPATCH=0` igual, salvo una ejecución (voces5) con `excepciones=1`, que es la excepción del controlador de vídeo Intel (`igd12um32xel` dentro de `Direct3DCreate9Ex`, registrada al crear el dispositivo, antes de cualquier código del juego); aparece también en ejecuciones de otras ramas y no depende de las opciones de SilentPatch.
* `python contrib/audit_hook_catalog.py --strict`: OK. `python contrib/auditoria_datos/tamanos_objetos.py lint`: OK (2134 objetos, 0 fallos).
* Compilación: los 19 `.cpp` tocados con `compilar_over.cmd` sin errores; enlazado del standalone y del ASI (`link exit=0`, sin símbolos sin resolver).

### 5. Lo que no se ha verificado

* Con el juego real no se ejercitaron: L126 (criminal huyendo de la policía), L130 (sesión con una prostituta), L136 (comentarios de choque en moto), L137 (spray, extintor y gas lacrimógeno), L138 (1 estrella: `CHASED`), L139 (disparos con ratón y teclado), L140 (policías: rodear y cubrirme), L143 (pandilleros atacando a CJ), L146 (reclutar un GSF drogado), L184 (escáner en una partida) y L202 (atropellar con pasajeros): están implementados y compilados con la opción apagada = código del EXE, pero requieren una escena de misión o de persecución que las sondas no reproducen. La lógica de L125 para peatones (sorteo del tema del tiempo, respuesta `WEATHER_DISL_REPLY` de CJ) tampoco se observó: en las ejecuciones ninguno de los peatones de la calle podía hablar del tiempo (`WillPedChatAboutTopic(7)`).
* No se comparó el audio audible con el del EXE original; la prueba es de datos y de ruta de código (contextos, rangos, voz elegida, estado de la conversación, dinero), no de forma de onda.
* L62 (azar de las prostitutas) no se porta aquí: lo hace `silentpatch2-peds`.

