# CAudioEngine (banco final 4)

Banco par a par dentro del juego completo (original con los hooks de la clase desactivados frente a candidata con hooks activos) para las
**82 funciones** de `CAudioEngine` (`docs/hooks.csv`, `validated=0`): **17.280 pares** previstos. Sin ejecutar todavia: pendiente de compilar y correr.

Comando: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh AudioEngineFinal4 3`

## Correcciones de fuente (con evidencia del EXE)
- `Constructor` 0x507670 (`AudioEngine.h/.cpp`): el envoltorio llamaba al constructor de C++ de `CAudioEngine`, que inicializa por valor y con otros valores lo que el EXE no escribe
  (`CAEPedSpeechAudioEntity` con `PED_TYPE_UNK` y `m_IsInitialized`; `CAECollisionAudioEntity` con 255 / -1 / `TOTAL_NUM_COLLISION_SURFACE_TYPES`; ceros en las dos `CAETwinLoopSoundEntity`).
  Ahora construye en el sitio con las escrituras del EXE: Frontend inlinado (0x507692..0x5076DD y 0x507716..0x507735: `[+0xC4]`, vtable `0x862E54`, `[+0x160]`, word `[+0x1D8]`, `[+0x1F0/1F4]`, `[+0x150]` y `[+0x1F8]` = `0x85F438`,
  `[+0x208]`, word `[+0x280]`, `[+0x298/29C]`), Script 0x5074D0 (vtable `0x862E58`, sub-vtables `0x85998C` y `0x862E60`, `[+0x88]` = 1.0f, bytes `+0x7C/7D/7E`, `+0x218`), Colision (`[0x4CC]`, vtable `0x862E64`),
  Pedless 0x4E6070 -> 0x4E4F10 (bytes `+0x90/98/99/9A`, words `+0x92`, `+0xA4/A6/A8/B0` = -1, `[+0xAC]` = -100.0f, vtable `0x85F330`) y Puerta (`[0x1F60]`, `[0x1FCC..0x1FD4]`, vtable `0x85998C`).
- `Initialise` 0x5B9C60: (1) `CAEAudioUtility::StaticInitialise` (0x5B97F0) es la primera de las nueve inicializaciones estaticas (0x5B9DC6..0x5B9DEE, despues del hardware y los managers); la fuente la llamaba antes del hardware (desviacion NOTSA)
  y ademas llamaba a `CAEWaterCannonAudioEntity::StaticInitialise`, que el EXE no llama; (2) `CAEAudioEntity::StaticInitialise` (inlinada en 0x5B9D59..0x5B9DB0, `AEAudioEntity.cpp`): con `OpenFile` nulo el EXE vuelve sin `CloseFile` (`cmp edi,ebx / je 0x5B9DA5`).
- `Service` 0x507750: el EXE llama a `CAEAudioHardware::GetChannelPlayTimes(canal, nullptr)` (0x50779A..0x5077A6, `movzx eax, word [esi+4] / push 0 / push eax / call 0x4D8820`); la fuente la omitia ("Does nothing").
- Comentarios de direccion que faltaban: `SetEffectsFaderScalingFactor` 0x506E50, `IsLoadingTuneActive` 0x506D90.

MATCH (sin cambios, evidencia en una linea): `AttachMissionAudioTo{Ped,Object,Physical}` 0x507310/20/30 (`add ecx,2A0h / jmp 0x4EC100`); `ClearMissionAudio`, `GetMissionAudio{Event,Position,LoadingStatus}`, `IsMissionAudioSampleFinished`, `PlayLoadedMissionAudio`,
`PreloadMissionAudio`, `SetMissionAudioPosition` (`add ecx,2A0h / jmp` al ScriptAE); `ReportMissionAudioEvent-{obj,ped,veh}` (`push 1.0f / push 0` + 0x4EE960), `-phs` (`jmp 0x4EE960`), `-vec` (`jmp 0x4EE940`);
`Report{Collision,BulletHit,ObjectDestruction,GlassCollisionEvent (tercer argumento 0),WaterSplash-phs,WaterSplash-vec (CVector por valor + float, ret 10h)}` (a la colision en `+0x4BC`); `ReportWeaponEvent` (ecx = `[+0x1E34]`, `jmp 0x4DFAA0`);
`ReportDoorMovement` (`push physical / push 0xA3 / call 0x4DC9F0` con ecx = +0x1F50); `ReportFrontendAudioEvent` (`jmp 0x4DD4A0`); `SayPedless` (ecx = +0x1E38, `jmp 0x4E60D0`); `Set{Music,Effects}MasterVolume` (`movsx / fild / fmul [0x85A7D4]` = 1/64);
`Set{Music,Effects,NonStream,Stream}FaderScalingFactor`, `{Enable,Disable}EffectsLoading`, `{Pause,Resume}AllSounds` (reenvios al hardware 0xB5F8B8); `SetRadioBassSetting` (`push 1.0f`); `SetRadioAutoRetuneOnOff`, `SetBassEnhanceOnOff`, `InitialiseRadioStationID`, `IsRadioOn`,
`HasRadioRetuneJustStarted`, `GetRadioStationName/NameKey/ListenTimes`, `GetCurrentRadioStationID`, `DisplayRadioStationName`, `IsVehicleRadioActive`, `StopRadio`, `ResetStatistics` (reenvios al radio manager 0x8CB6F8);
`IsRadioRetuneInProgress`, `IsLoadingTuneActive`, `StartLoadingTune` (`AddAudioEvent(0x32,0,1.0)` + `SoundManager.Service`); `GetBeatInfo`/`IsBeatInfoPresent` (`+8` y campo `+0xA0`); `Get{Cutscene,Beat}TrackStatus`, `IsCutsceneTrackActive`,
`PlayPreloadedCutsceneTrack`, `PlayPreloadedBeatTrack` (`mov [esi],al`), `PauseBeatTrack`, `StopBeatTrack` (`push 1 / call 0x507080`); `IsAmbienceTrackActive`, `IsAmbienceRadioActive`, `DoesAmbienceTrackOverrideRadio` (`mov al,[0x8AC15C]`);
`StopAmbienceTrack` 0x507220; `StopCutsceneTrack` 0x507080 (signo de `[esi+2]` con `jl`); `PreloadCutsceneTrack` 0x507E30 y `PreloadBeatTrack` 0x507F40 (rama radio/ambiente/cutscene y `jmp 0x4DC0E0` / `call 0x4DC130` con `push 0`);
`RetuneRadio` 0x507E10, `StartRadio-` 0x507DF0, `StartRadio-1` 0x507DC0 (`test al,al / jne` tras `0x4DBEC0`); `ServiceLoadingTune` 0x5078A0 (`fcomp / test ah,41h`, `fadd [0x858B4C]` = 0.005f, `fcom / test ah,5 / jp`);
`Reset` 0x507A90, `ResetSoundEffects` 0x507C30, `Shutdown` 0x507CB0, `InitialisePostLoading` 0x5078F0 (las doce parejas banco/ranura 0x27/2, 0x1B/3, 0x8A/0x13, 0/0x29, 0x3B/0, 0x34/4, 0x8F/5, 0x33/0x1F, 0x69/6, 0x4A/0x11, 0x80/0x20, 0x0D/0x12 coinciden con la fuente),
`Restart` 0x506DB0 (`PauseManually(0)`), `Destructor` 0x506CD0 (sin estado que limpiar: ver limites).

## Tabla de pares
| Casos | Funciones |
|---|---|
| 1024 (4) | `Initialise`, `Reset`, `Service`, `ServiceLoadingTune` |
| 512 (6) | `InitialisePostLoading`, `PreloadBeatTrack`, `PreloadCutsceneTrack`, `ReportCollision`, `Shutdown`, `StopCutsceneTrack` |
| 256 (19) | `ReportBulletHit`, `ReportFrontendAudioEvent`, `ReportGlassCollisionEvent`, `ReportMissionAudioEvent-phs`, `ReportWaterSplash-phs`, `ReportWaterSplash-vec`, `ReportWeaponEvent`, `RetuneRadio`, `SayPedless`, `Set{Effects,Music,NonStream,Stream}FaderScalingFactor`, `Set{Effects,Music}MasterVolume`, `StartRadio-`, `StartRadio-1`, `StopAmbienceTrack`, `StopBeatTrack` |
| 128 (29) | `AttachMissionAudioTo{Object,Ped,Physical}`, `ClearMissionAudio`, `GetBeatInfo`, `Get{Beat,Cutscene}TrackStatus`, `GetCurrentRadioStationID`, `GetMissionAudio{Event,LoadingStatus,Position}`, `GetRadioStationName`, `GetRadioStationNameKey`, `InitialiseRadioStationID`, `IsBeatInfoPresent`, `IsMissionAudioSampleFinished`, `PlayLoadedMissionAudio`, `PlayPreloadedBeatTrack`, `PreloadMissionAudio`, `ReportDoorMovement`, `ReportMissionAudioEvent-{obj,ped,vec,veh}`, `ReportObjectDestruction`, `ResetSoundEffects`, `SetMissionAudioPosition`, `SetRadioBassSetting`, `StopRadio` |
| 64 (24) | `Constructor`, `Destructor`, `Disable/EnableEffectsLoading`, `DisplayRadioStationName`, `DoesAmbienceTrackOverrideRadio`, `GetRadioStationListenTimes`, `HasRadioRetuneJustStarted`, `IsAmbience{Radio,Track}Active`, `IsCutsceneTrackActive`, `IsLoadingTuneActive`, `IsRadioOn`, `IsRadioRetuneInProgress`, `IsVehicleRadioActive`, `PauseAllSounds`, `PauseBeatTrack`, `PlayPreloadedCutsceneTrack`, `ResetStatistics`, `Restart`, `ResumeAllSounds`, `SetBassEnhanceOnOff`, `SetRadioAutoRetuneOnOff`, `StartLoadingTune` |

## Metodo
- `CAudioEngine` FALSO en memoria propia (0x1FD8 bytes, bytes aleatorios con los campos propios en su dominio) y un mundo falso (entidades opacas, vectores finitos, ajustes de vehiculo, nombres, un `CAEGlobalWeaponAudioEntity` con ceros). Nunca se llama a una funcion del motor real.
- Todo lo que llama `CAudioEngine` se sustituye con `EntryPatch` (direccion del EXE y simbolo/cuerpo de la DLL, cuerpos calculados antes de crear ningun parche, conjunto de parches en almacenamiento estatico y cerrado antes de construir el json): CAEAudioHardware, las entidades Frontend / Script / Colision /
  GlobalWeapon / Puerta / Pedless / PedSpeech / PoliceScanner / Weather, los managers de radio, cutscene (incluida `PreloadCutsceneTrack`, 0x4DC0E0, no enganchada: se parchea el simbolo) y ambiente (incluida `StopAmbienceTrack`, 0x4D6CC0), `CAESoundManager`, `FindPlayerPed`, `CUserRadioTrackManager::Shutdown`.
  Para `Initialise` ademas `CLoadingScreen`, `CFileMgr::{OpenFile,Read,CloseFile}`, las nueve inicializaciones estaticas y las `Initialise` de hardware, radio, SoundManager, Frontend, Script, Pedless y Colision. Los dobles respetan la convencion y el `ret N` del EXE e ignoran los hilos ajenos (`Mine()`).
- Cada doble anota (etiqueta, `this` normalizado, argumentos) en una traza sin reservas; los retornos salen de guiones por caso (n valores aleatorios y un valor final fijo: los bucles `while (IsRadioOn...)` siempre terminan). Se comparan retorno, traza completa, hash del engine falso (con `m_GlobalWeaponAE` normalizado), hash del mundo y hash de los globales reales tocados.
- Globales reales guardados y restaurados en cada ejecucion (y comprobados al final): cabecera del `CAudioEngine` real (0xB6BC90), manager de cutscene (0x8AE554), de ambiente (0x8AC15C), tabla de canales del hardware (0xB5F944), pausas de `CTimer` (0xB7CB48/49), `CCutsceneMgr::ms_running` (0xB5F851) y el puntero de volumenes de eventos (0xBD00F8).
- `Initialise`: la parte inlinada del EXE (reserva de canal, manager de cutscene y de ambiente, lectura de `EVENTVOL.DAT` con `OpenFile/Read/CloseFile` simulados) se compara con la DLL, que llama a `AllocateChannels` / `Initialise` de los managers (con el hook desactivado van al EXE; el de ambiente es la DLL real). El hardware real recibe una tabla de canales aleatoria y los dos managers bytes aleatorios; el relleno
  de `tAmbienceSettings` (0x8AC185..87) se pone a cero antes de hashear (la DLL copia la estructura entera, el EXE solo el byte). El buffer de 45401 bytes que reserva `new int8[]` se libera al terminar y se restaura 0xBD00F8.
- `Shutdown`: `operator delete` / `delete[]` (0x8214BD / 0x8213AE) trazados sin liberar; el `CAEGlobalWeaponAudioEntity` falso (ceros, `m_Physical` nulo) se destruye de verdad en ambos lados (destructor del EXE frente al de la DLL) y su hash (con la vtable enmascarada) entra en el del mundo. El buffer de volumenes de la prueba se reserva y libera aparte.
- `InitialisePostLoading`: construccion REAL de `CAEGlobalWeaponAudioEntity` (+ `CPhysical`) en el montón con `CPlaceable::AllocateStaticMatrix` y el azar (0x821B1E) sustituidos; se comparan los bytes que escribe el constructor del EXE (+0x10, +0x7C..7E, +0x84..9F, +0xA4), vtable no nula, `m_Physical` no nulo y su matriz; despues se liberan ambos bloques. `CAudioEngine::Service` (0x507750) se traza (no toca el motor real).
- `Constructor` / `Destructor`: el constructor del EXE se ejecuta una vez al empezar (hooks desactivados) con dos rellenos (0xCD y 0x00) para deducir que bytes escribe y que ranuras son vtables; la candidata solo debe igualar esos bytes (las vtables se comparan como "escrita / no escrita") y no escribir nada mas que ceros.
  El destructor parte de la imagen que deja el constructor del EXE (+ cabecera aleatoria) y compara los bytes que cambian.
- x87 a 24 bits durante cada llamada, como en el juego.

## Cosas enmascaradas / fuera de dominio
- vtables de los objetos embebidos (la DLL usa las suyas en el destructor; el constructor escribe las del EXE) y el vptr del `CAEGlobalWeaponAudioEntity` falso tras `Shutdown`.
- `CAEGlobalWeaponAudioEntity` creado en `InitialisePostLoading`: solo se comparan los bytes que escribe el EXE (la DLL pone a cero el resto por los inicializadores de `CAEAudioEntity`); el `CPhysical` interno no se compara (validado en AnimAudio6).
- Relleno de `tAmbienceSettings` (ver arriba). `PlayPreloadedBeatTrack` / `PauseBeatTrack`: argumentos solo 0/1.
- Fuera del fixture: `m_nBackgroundAudioChannel` negativo en `Service` (la DLL indexa un `std::array` de canales en `GetChannelPlayTimes`; el doble lo sustituye, pero un valor real fuera de rango abortaria en depuracion), `BD00F8` nulo en `Shutdown` (el EXE llama a `delete[]` incondicional), eventos de mision > 0xFFFF (la DLL trunca a `uint16`, el EXE no),
  ids de emisora fuera de `int8`.
- `Destructor`: el estado de partida es el que deja el constructor (referencias `[AE+8]` nulas, `m_IsInUse` de los gemelos a cero); con referencias vivas el destructor del EXE llama a `CEntity::CleanUpOldReference` (0x571A00) sobre el motor real, que no se prueba.

## Supuestos a revisar al compilar
- Nombres de hooks tal como salen de `docs/hooks.csv` (`StartRadio-`, `StartRadio-1`, `ReportMissionAudioEvent-obj/-ped/-phs/-vec/-veh`, `ReportWaterSplash-for-physical/-at-position`, `AddAudioEvent-1`, `StartRadio-manual/-with-settings`).
- `&CAECutsceneTrackManager::PreloadCutsceneTrack` y `&CAEAmbienceTrackManager::StopAmbienceTrack` no son virtuales ni sobrecargados; `&CGeneral::GetRandomNumber` como en VehModelInfoFinal4.
- Las categorias de audio siguen desactivadas al arrancar (`unhook='Audio,...'`), de modo que las llamadas de la DLL a los callees saltan al EXE, donde estan los dobles; los dobles tambien se ponen en el cuerpo de la DLL por si algun callee estuviera activo.
- Se quita la llamada NOTSA a `CAEAudioUtility::StaticInitialise` previa al hardware: si el arranque real de la DLL vuelve a fallar por el reloj relativo (`GetCurrentTimeInMS` ahora es x87 como el EXE, sin division entera), reponer la llamada y ajustar el banco.
- `Constructor` deja de pasar por los constructores de C++ (que siguen existiendo para otros usos); `AudioEngine.h` pierde el cuerpo inline de `Constructor()` (ahora en `AudioEngine.cpp`).

## Aprobación (10/10/2026)

Corrida `20261010-125652-130519`: passed=true, 17280 pares, 82 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_audioenginefinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CAudioEngine queda sin pendientes. Límites declarados en el alcance de cada función y en las secciones anteriores de este README.
