# SilentPatch: scripts, misiones, entrada, HUD, trucos, guardado y textos (rama `silentpatch-scm`)

Fecha: 25/11/2026. Worktree `D:\Original\wv-sp3`, rama `silentpatch-scm` (sobre `integracion-fase7b`). Origen: SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz "Silent"; ver [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Los arreglos se aplican **cambiando nuestras funciones reconstruidas** (no se copian parches de bytes); cada sitio lleva `// SilentPatch (MIT, Silent): <arreglo>`.

## Cómo apagar

- **Todo SilentPatch** (el comportamiento vuelve al del EXE original): variable de entorno `GTASA_SILENTPATCH=0`, o en `gta-reversed.ini`:

  ```ini
  [SilentPatch]
  SilentPatch=0
  ```

  Valor por defecto: encendido (una sola línea, la última de `SilentPatchEnabled()` en `source/extensions/SilentPatchConfig.cpp`, cambia el valor por defecto; el entorno gana al ini).
- **Un solo arreglo**: `GTASA_SP_<OPCION>=0` (en mayúsculas, p. ej. `GTASA_SP_ENABLESCRIPTFIXES=0`) o `<Opcion>=0` en `[SilentPatch]` del ini. Con el interruptor general apagado `SilentPatchOption()` devuelve siempre `false` y `SilentPatchIntOption()` el valor "apagado".
- Opciones sin clave en `SilentPatchSA.ini` de SilentPatch (arreglos "siempre activos" allí): nombre propio de este proyecto, activadas por defecto. Las opciones que SilentPatch trae apagadas por defecto (`MinimalHUD`, `SlidingMissionTitleText`, `SlidingOddJobText`, `ColouredZoneNames=0`, `TrueInvincibility`, `SmallSteamTexts=-1`) **no se portaron**: su valor por defecto equivale al EXE.

Módulo compartido: `source/extensions/SilentPatchConfig.h/.cpp` (`SilentPatchEnabled`, `SilentPatchOption(name, default)`, `SilentPatchIntOption(name, default, valorApagado)`); lee con `GetPrivateProfileString` en `.\gta-reversed.ini` (con `.\`: sin ruta busca en el directorio de Windows).

## Estado de las viñetas de mi área

Clases: A = se aplica a nuestra función; B = ya lo hace el proyecto; C = no aplica al independiente / es del EXE o de versiones; D = depende de bibliotecas o hardware que no tenemos; E = ambigua o no revisada. "Ejercitado": no (misión concreta) salvo lo indicado en "Pruebas".

### Implementadas (clase A)

| Viñeta del CHANGELOG-SA.md | Función nuestra | Opción (defecto) | Estado |
| --- | --- | --- | --- |
| ⚙️ Gym glitch ("You have worked out enough...") | `CRunningScript::ProcessCommands2100To2199`, `COMMAND_GET_CURRENT_DATE` + `SilentPatchScripts::GymGlitchFixDate` (scripts gymbike/gymbenc/gymtrea/gymdumb: mes -1, día = días jugados + 1; verificado con el desensamblado del EXE 0x470B03: P0 = día, P1 = mes) | `EnableScriptFixes` (1) | implementado, no ejercitado |
| ⚙️ Baloncesto al guardar en la mansión de Madd Dogg | `SilentPatchScripts::BasketballFix`, tras `CTheScripts::Load` y en la misión 0 (`LoadAndLaunchMissionInternal`) | `EnableScriptFixes` | implementado; la misión 0 se ejercita en las pruebas de humo (ver abajo), no el guardado |
| ⚙️ Soft lock de "Mountain Cloud Boys" | `MountainCloudBoysFix` (misión 53) | ídem | implementado, no ejercitado |
| ⚙️ Soft lock inicial de "Sweet's Girl" | `SweetsGirlFix` (misión 18) | ídem | implementado, no ejercitado |
| ⚙️ Conos de las autoescuelas ("Blackboard glitch") | `DrivingSchoolConesFix` (71), `BikeSchoolConesFix` (120) | ídem | implementado, no ejercitado |
| ⚙️ "Air Raid" ya no roba el arma pesada | `AirRaidFix` (72; reensambla el bytecode: guarda/devuelve el arma cargando el modelo) | ídem | implementado, no ejercitado |
| (script ini) pájaros de "Ice Cold Killa" (float invertido) y streaming de "Supply Lines"/"Beefy Baron" | `IceColdKillaFix` (61), `SupplyLinesFix` (73, 10) | ídem | implementado, no ejercitado |
| ⚙️ Quadruple Stunt Bonus | `QuadrupleStuntBonus` (SCM principal; tras cargar partida y en la misión 0) | ídem | implementado, no ejercitado |
| Fixed a broken CREATE_BIRDS script command | `COMMAND_CREATE_BIRDS` (2100To2199): las 6 coordenadas se leen como float (el EXE usaba `fild`) | `FixCreateBirds` (1) | implementado, no ejercitado |
| Fixed an SCM interpreter issue ... biker cop con `PEDTYPE_COP` | `CRunningScript::GetCorrectPedModelIndexForEmergencyServiceType` (caso `MODEL_LAPDM1` -> `COP_TYPE_LAPDM1`) | `FixBikerCopSpawn` (1) | implementado, no ejercitado |
| Script randomness is now 16-bit | `SilentPatchScripts::Random16` en `COMMAND_GENERATE_RANDOM_FLOAT` (100To199) y `GenerateRandomInt` (MathCommands) | `ScriptRandomness16Bit` (1) | implementado, no ejercitado; cambia la secuencia del RNG |
| Reinicio al empezar Nueva Partida: "keep weapons after wasted/busted", respray y bombas gratis, temporizadores de ambulancia/bomberos/persecución, contadores de estadísticas, mensaje "To stop Carl..." | `CGame::ReInitGameObjectVariables` (`SilentPatchNewGame::BeforeReInit`) | `ResetVariablesOnNewGame` (1) | implementado; se ejercita al arrancar partida nueva en el humo, sin verificar los valores |
| Pickups, generadores y saltos acrobáticos de los IPL de texto se reinician en Nueva Partida | `CFileLoader::LoadScene`-bucle de secciones PICK/CARS/JUMP guardan la línea; `SilentPatchNewGame::AfterReInit` las recarga | `ReinitIplObjectsOnNewGame` (1) | implementado, no ejercitado |
| Traveling far away ... extra gang territories / Taxi Driver | `CTheZones::GetCurrentZoneLockedOrUnlocked` (fuera de la rejilla 10x10 devuelve `true`) | `FixTerritoriesOutsideMap` (1) | implementado, no ejercitado |
| Num5 is now bindable | `CMenuManager::CheckCodesForControls` (`MenuManager_Input.cpp`; quita `rsPAD5` de la lista de teclas rechazadas) | `BindableNum5` (1) | implementado, no ejercitado |
| Text boxes can now show together with a Mission Passed text | `CHud::DrawHelpMessage` (ya no se oculta con `m_BigMessage[0]`) | `HelpBoxWithMissionPassed` (1) | implementado, no ejercitado |
| The mouse's vertical axis sensitivity now matches the horizontal axis | `CCamera::GetMouseAccelVerticalPtr` (usado en `Cam.cpp` tres veces y `X87Math.cpp` dos) | `MouseVerticalSensitivityFix` (1) | implementado, no ejercitado |
| Mouse rotates an airborne car only with Steer with Mouse enabled | `Automobile.cpp` (rama de giro en el aire, 0x6B4F04, bajo el nombre `ProcessAI`) y `QuadBike.cpp` | `MouseAirborneRequiresSteerOption` (1) | implementado, no ejercitado |
| Fixed cheat-spawned melee weapons replaced by pickups | `Cheat.cpp`: munición 1 en cuchillo/motosierra (tras `GiveWeaponSet1-3`), katana y paracaídas | `FixCheatMeleeAmmo` (1) | implementado, no ejercitado |
| 'Cars drive on water' cheat is toggleable (SEAROADER) | `CCheat::AddToCheatString` (hash 0xE5FC92C3 en la entrada `CHEAT_CARS_ON_WATER`; el hash viene de SilentPatch, no se recalculó) | `SearoaderCheat` (1) | implementado, no ejercitado |
| Stats counted in kilograms are displayed correctly | `Stats.cpp` `StatsKgText` (`%.0fkg` con el double) | `FixStatsKilograms` (1) | implementado, no ejercitado |
| GANGS and CRIMES section names swapped in stats.html | `CMenuManager::SaveStatsToFile` (`MenuManager.cpp`) | `FixStatsHtmlSectionNames` (1) | implementado, no ejercitado |
| Script draws can no longer cause the radio station name to wrap | `CAERadioTrackManager::DisplayRadioStationName` (`SetCentreSize(ancho)`) | `FixRadioStationNameWrap` (1) | implementado, no ejercitado |
| A misplaced Beagle in Fort Carson replaced with a BMX | `CFileLoader::LoadCarGenerator(CFileCarGenerator*)` (se detecta por los 20 bytes del generador) | `FixFortCarsonBeagle` (1) | implementado, no ejercitado; cambia contenido |
| ⚙️ Made the game select metric/imperial based on system locale | `CLocalisation::Metric` | `Units` (-1) | implementado, no ejercitado |

### Ya hechas por el proyecto (B)

| Viñeta | Evidencia |
| --- | --- |
| Path to the User Files directory via dedicated API call, not the registry | `FileMgr.cpp` usa `SHGetFolderPathW(CSIDL_PERSONAL)` |

### No aplican (C, D) y pendientes (E / A sin hacer)

| Viñeta | Clase | Razón |
| --- | --- | --- |
| Mouse lock-up when exiting the menu; mouse cursor locked to the game window | C (independiente: SDL3; ASI: DirectInput del EXE) | El ratón del independiente no usa el código de DirectInput; ver [ejecucion-raton.md](ejecucion-raton.md). No revisado el bloqueo de cursor en SDL3 |
| A 1.0 no-DVD-only bug (recruiting after replay); 1.01-only radio memory leak; Steam/RGL "Steer with Mouse" default, Steam/RGL saves, monitor selection dialog | C | Problemas de versión / EXE concreto / diálogo de Windows |
| 16:9 resolutions selectable; Steam aspect ratios | C | Lista de modos de vídeo propia del proyecto (otra área: gráficos) |
| Mouse vertical axis not locked during fade-ins; in-car camera mouse behaviour (looking left/right/behind) | A, **pendiente** | Requieren leer `CCam::Process_FollowPed*`/`Process_1stPerson` y las cámaras de coche; no se hicieron |
| Keyboard input latency decreased by one frame | A, **pendiente** | Cambia el orden de `CPad::UpdatePads`/`RsKeyboard`; con los cambios recientes de `Pad.cpp` se deja para una pasada propia |
| Hovering with a jetpack with keyboard (Q+E) | A, **pendiente** | `CTaskSimpleJetPack::ProcessControlInput`: la rama de "hover" necesita contrastarse con el desensamblado 0x67ED2D-0x67EDAF |
| Credits, heat-seeking/gamepad crosshair, map cursor, text-box padding y heat haze escalan con la resolución | A, **pendiente** | Escala de HUD con las constantes 640x448; no se portó (muchas constantes inlineadas en `Hud.cpp`/`X87Math.cpp`) |
| Mission title y 'Mission Passed' con la misma duración en cualquier resolución | E | En SilentPatch solo se activa con `SlidingMissionTitleText` (apagado por defecto): no portado |
| Racing checkpoints bien coloreados sin enex previos; Script sprites con filtrado bilineal; Very long loading times loop loading screens | E | Área gráfica/carga; no revisado |
| Todos los diálogos (CJ, policías, pandilleros, tenderos; WMYSGRD, BMYPIMP, VBFYST2, Maccer, SBFYSTR, SWFYSTR, 'Good Citizen Bonus', etc.) | E | Tablas de contextos de voz (`SpeechContextsSA.h`); gran volumen, no abordado |
| Opciones apagadas por defecto: MinimalHUD, SlidingMissionTitleText, SlidingOddJobText, ColouredZoneNames, SmallSteamTexts, TrueInvincibility | A, no portadas | Valor por defecto = comportamiento del EXE; se pueden portar como opcionales más adelante |

## Pruebas

1. **Configuración** (real, aislada): se compiló `SilentPatchConfig.cpp` con un `StdInc.h` mínimo y un programa de prueba en el bloc de notas del agente (no versionado). Resultado: sin variables, interruptor general = 1, `EnableScriptFixes` = 1, `MinimalHUD` (defecto 0) = 0, `Units` = -1; con `GTASA_SILENTPATCH=0`, todas las opciones devuelven falso y `Units` el valor "apagado" (-2) aunque se pida `GTASA_SP_MINIMALHUD=1`; con `GTASA_SP_ENABLESCRIPTFIXES=0` solo esa se apaga; `GTASA_SP_MINIMALHUD=1` y `GTASA_SP_UNITS=1` encienden/fijan; las claves `SilentPatch=0`, `EnableScriptFixes=0` y `Units=0` de `[SilentPatch]` en `gta-reversed.ini` se leen y el entorno gana al ini. (El primer intento no leía el ini: sin ruta, `GetPrivateProfileString` busca en el directorio de Windows; corregido con `.\`.)
2. **Compilación**: los 24 archivos .cpp tocados o nuevos se compilan con `compilar_over.cmd` (rc=0) y el ASI enlaza.
3. **Humo del ASI** (`prueba_ropero.py` y `prueba_marcadores.py`, `cal_mal.args`): ver "Resultado del humo" al final (no aprobado; ver allí).
4. **Con el interruptor apagado** el código tomado en cada sitio es el original (la condición `SilentPatchOption(...)` cae a la rama del EXE; en `CLocalisation::Metric` el valor -2 cae a la fórmula original). Comprobado solo por lectura del código y por la prueba de configuración: la corrida dinámica con `GTASA_SILENTPATCH=0` no se pudo hacer (ver "Resultado del humo").

No se verificaron los patrones del bytecode contra un `main.scm` real: no hay copia accesible fuera de la carpeta del juego, que no se debe tocar. Los parches no se aplican si el patrón no aparece una sola vez (se anota en el registro con `NOTSA_LOG_INFO`).

## Resultado del humo

ASI enlazado (`reversing\FnFase8si\gta_reversed.asi`, objetos de `wv-intuild` más los 24 archivos propios compilados con `compilar_over.cmd`; la rama se fusionó antes con `integracion-fase7b` para que las cabeceras coincidieran con los objetos: con la base anterior el enlace fallaba por `DatosExe::R5::vB4E3E8/vB4E3F0/vB61100`). Compilación: 24 de 24 con `rc=0`; `python contrib/audit_hook_catalog.py --strict`: OK.

Ejecuciones del humo (honestas, sin interpretar de más):

| Prueba | SilentPatch | Resultado |
| --- | --- | --- |
| `prueba_ropero.py cal_mal.args sp3_on` | encendido | `ERROR no llego al interior` (no llegó a la medición; sin veredicto BIEN/MAL) |
| `prueba_marcadores.py cal_mal.args sp3_on` | encendido | `ERROR sin juego` (el juego no arrancó; la cola de `gta_lock` la ocupaba otro agente) |
| `prueba_ropero.py` / `prueba_marcadores.py` con `GTASA_SILENTPATCH=0` | apagado | **no ejecutadas**: esperaron más de 15 minutos a `gta_lock` (propietario `owner_bis1`) y se cancelaron |

Conclusión: **el humo no está aprobado** para esta rama. El fallo "no llego al interior" con SilentPatch encendido no está atribuido (no hay línea base con la misma DLL y SilentPatch apagado en el mismo entorno, y las pruebas de otras ramas del mismo rato también devolvieron `sin juego` / `el juego se cerro`). Antes de integrar hay que repetir las cuatro corridas cuando `gta_lock` esté libre: si `off` pasa y `on` falla, el sospechoso principal es `SilentPatchNewGame::AfterReInit` (recarga de pickups/generadores/saltos antes de `GenericLoad`), que se apaga con `GTASA_SP_REINITIPLOBJECTSONNEWGAME=0`; el siguiente, `ResetVariablesOnNewGame`.
