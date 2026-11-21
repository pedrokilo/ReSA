# Plan de sustitucion de los datos del EXE por partes y lotes

[Medicion y resumen](README.md) · [Lotes (CSV)](plan-lotes.csv) · [Objetos por lote (CSV)](plan-objetos.csv)

Generado por `contrib/datos_exe_plan.py`. 5276 objetos con referencia de la DLL repartidos en 135 lotes y 10 grupos; lotes de hasta 40 objetos o 8 KB del mismo propietario. Los tamanos son cota (tam_origen: tipo declarado, ancho de acceso del EXE o hueco hasta el siguiente objeto, tope 64).

## Grupos

| Grupo | Descripcion | Riesgo | Lotes | Objetos | Bytes (cota) | Bytes de tamano fiable | Hechos | Propietarios |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| G1 (DE01) | Constantes escalares de .rdata (StaticRef tipado) | bajo | 1 | 154 | 632 | 632 | 154 | 28 |
| G2 (DE02) | Constantes de .rdata por otras vias (ayudante, reinterpret_cast, literal, mezcla) | bajo | 6 | 224 | 861 | 781 | 0 | 34 |
| G3 (DE03) | Tablas y cadenas de .rdata del juego (mas de 8 bytes) | bajo | 1 | 36 | 1,236 | 0 | 0 | 35 |
| G4 (DE04) | Datos inicializados (.data) del juego sin lector vivo | medio | 27 | 1065 | 14,497 | 10,672 | 0 | 137 |
| G5 (DE05) | Estado en ejecucion (.bss) del juego sin lector vivo | medio | 61 | 2354 | 66,413 | 51,600 | 0 | 264 |
| G6 (DE06) | Datos del juego que aun lee codigo vivo del EXE (.data/.bss compartidos) | alto | 1 | 20 | 202 | 74 | 0 | 12 |
| G7 (DE07) | Datos de RenderWare y D3D (cualquier seccion) | alto | 16 | 619 | 4,992 | 2,429 | 0 | 24 |
| G8 (DE08) | CRT, x87 y bibliotecas (cualquier seccion) | alto | 5 | 165 | 1,910 | 650 | 0 | 22 |
| G9 (DE09) | vtables de las clases enganchadas (hook_vtable en .rdata/.data) | alto | 12 | 469 | 21,208 | 0 | 0 | 459 |
| G10 (DE10) | Proteccion del EXE (_TEXT_HA, _rwdseg) | descartado | 5 | 170 | 797 | 669 | 0 | 1 |

## Objetos sin referencia de la DLL (no son lotes)

| Categoria | Objetos | Bytes (cota) |
| --- | ---: | ---: |
| .bss · leido por codigo vivo del EXE (crt) | 119 | 954 |
| .bss · leido por codigo vivo del EXE (juego) | 39 | 273 |
| .bss · leido por codigo vivo del EXE (renderware) | 28 | 252 |
| .bss · solo lo leen funciones ya reemplazadas (candidato a huerfano) | 2750 | 26,514 |
| .data · leido por codigo vivo del EXE (crt) | 107 | 1,454 |
| .data · leido por codigo vivo del EXE (juego) | 10 | 40 |
| .data · leido por codigo vivo del EXE (renderware) | 354 | 7,089 |
| .data · solo lo leen funciones ya reemplazadas (candidato a huerfano) | 919 | 9,330 |
| .rdata · leido por codigo vivo del EXE (crt) | 227 | 3,826 |
| .rdata · leido por codigo vivo del EXE (crt+renderware) | 6 | 24 |
| .rdata · leido por codigo vivo del EXE (juego) | 62 | 540 |
| .rdata · leido por codigo vivo del EXE (renderware) | 710 | 19,582 |
| .rdata · solo lo leen funciones ya reemplazadas (candidato a huerfano) | 4080 | 41,959 |
| _TEXT_HA · leido por codigo vivo del EXE (juego) | 30 | 103 |
| _rwdseg · leido por codigo vivo del EXE (crt) | 2 | 8 |

## G1 · Constantes escalares de .rdata (StaticRef tipado)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE01 | Bike, Plane, Cam ... | 154 | 632 | .rdata | 0x00858A28–0x00872C00 | hecho |

## G2 · Constantes de .rdata por otras vias (ayudante, reinterpret_cast, literal, mezcla)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE02-1 | Cam, Bike, Camera ... | 40 | 157 | .rdata | 0x00858B10–0x00871358 | pendiente |
| DE02-2 | Hud, Garage, EntitySeekPosCalculator ... | 40 | 181 | .rdata | 0x0085862C–0x0085A968 | pendiente |
| DE02-3 | Hud | 40 | 165 | .rdata | 0x00859A44–0x00866C80 | pendiente |
| DE02-4 | ImportThunks, JPegCompress, Hud ... | 40 | 127 | .rdata | 0x00858068–0x0086B818 | pendiente |
| DE02-5 | Radar, Stats, Matrix ... | 40 | 156 | .rdata | 0x00858970–0x00871A70 | pendiente |
| DE02-6 | Vehicle, TaskComplexSunbathe, Weapon ... | 24 | 75 | .rdata | 0x00858978–0x00872C6C | pendiente |

## G3 · Tablas y cadenas de .rdata del juego (mas de 8 bytes)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE03-1 | TaskComplexUseMobilePhone, AEAudioChannel, AEAudioHardware ... | 36 | 1,236 | .rdata | 0x00859A58–0x00887D5C | pendiente |

## G4 · Datos inicializados (.data) del juego sin lector vivo

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE04-1 | AEPedAudioEntity, AEAudioEnvironment, AECutsceneTrackManager ... | 40 | 5,632 | .data | 0x008ABC70–0x008D5E44 | pendiente |
| DE04-2 | AEPedSpeechAudioEntity, AEPoliceScannerAudioEntity, AEPedAudioEntity ... | 40 | 1,179 | .data | 0x008AE6A0–0x008C8B88 | pendiente |
| DE04-3 | AERadioTrackManager, AEVehicleAudioEntity, AESound ... | 40 | 521 | .data | 0x008C8B8C–0x008CCF24 | pendiente |
| DE04-4 | AEWeatherAudioEntity, Automobile, AEVehicleAudioEntity ... | 40 | 297 | .data | 0x008AA5A8–0x008D3394 | pendiente |
| DE04-5 | Bike, Buoyancy, Cam ... | 40 | 315 | .data | 0x008A5B14–0x008D3964 | pendiente |
| DE04-6 | Cam | 40 | 286 | .data | 0x008CC398–0x008CC7DC | pendiente |
| DE04-7 | Cam | 40 | 173 | .data | 0x008CC7E0–0x008CCCA0 | pendiente |
| DE04-8 | Cam | 40 | 160 | .data | 0x008CCCA4–0x008CCD9C | pendiente |
| DE04-9 | Cam | 40 | 157 | .data | 0x008CCDA0–0x008CCEA0 | pendiente |
| DE04-10 | Cam, Camera | 40 | 165 | .data | 0x008CC380–0x008D610C | pendiente |
| DE04-11 | Camera | 40 | 341 | .data | 0x008CC824–0x008CCE14 | pendiente |
| DE04-12 | Camera, CarCtrl, CdStreamInfo ... | 40 | 299 | .data | 0x008A5AB4–0x008E3FFC | pendiente |
| DE04-13 | ClothesBuilder, Clouds, Collision ... | 40 | 987 | .data | 0x008A5B16–0x008D5818 | pendiente |
| DE04-14 | CustomCarEnvMapPipeline, EventScanner, GangWars ... | 40 | 170 | .data | 0x008A5E48–0x008D5CD8 | pendiente |
| DE04-15 | ModelIndices, IplStore, LoadingScreen ... | 40 | 292 | .data | 0x008CD4F4–0x008E3FD0 | pendiente |
| DE04-16 | ModelIndices | 40 | 80 | .data | 0x008CD544–0x008CD5E4 | pendiente |
| DE04-17 | ModelIndices | 40 | 80 | .data | 0x008CD5E8–0x008CD698 | pendiente |
| DE04-18 | ModelIndices | 40 | 86 | .data | 0x008CD69C–0x008CD74C | pendiente |
| DE04-19 | ModelIndices, Ped, Object ... | 40 | 174 | .data | 0x008A6260–0x008D5D74 | pendiente |
| DE04-20 | Physical, Plane, PedIntelligence ... | 40 | 229 | .data | 0x008A5F48–0x008D6080 | pendiente |
| DE04-21 | PostEffects, Plane, PlayerPed ... | 40 | 203 | .data | 0x008CDF20–0x008D50E8 | pendiente |
| DE04-22 | PostEffects, ProjectileInfo | 40 | 222 | .data | 0x008CD75C–0x008D6104 | pendiente |
| DE04-23 | Stats, Rope, Streaming ... | 40 | 809 | .data | 0x008A5A80–0x008E2A14 | pendiente |
| DE04-24 | TaskComplexFallToDeath, Streaming, TaskSimpleFightingControl ... | 40 | 195 | .data | 0x008A5A9C–0x008D2FF4 | pendiente |
| DE04-25 | Vehicle, TrafficLights, Train ... | 40 | 388 | .data | 0x008A6214–0x008D48F8 | pendiente |
| DE04-26 | WaterLevel, VehicleModelInfo, VideoMode ... | 40 | 336 | .data | 0x008A7740–0x008D6248 | pendiente |
| DE04-27 | WeaponEffects, Weather, app_light ... | 25 | 721 | .data | 0x008CDEF0–0x008D6238 | pendiente |

## G5 · Estado en ejecucion (.bss) del juego sin lector vivo

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE05-1 | 3dMarkers, AEGlobalWeaponAudioEntity, AEPedSpeechAudioEntity ... | 40 | 264 | .bss | 0x00B5F898–0x00C80444 | pendiente |
| DE05-2 | AEPedSpeechAudioEntity, AEPoliceScannerAudioEntity, AERadioTrackManager | 40 | 214 | .bss | 0x00B613E6–0x00B62C5A | pendiente |
| DE05-3 | AERadioTrackManager, AESmoothFadeThread, AESoundManager ... | 40 | 200 | .bss | 0x00B608D0–0x00BA67F8 | pendiente |
| DE05-4 | AEVehicleAudioEntity, AEWeatherAudioEntity, AudioZones ... | 40 | 463 | .bss | 0x00B4EA2C–0x00B9B7D0 | pendiente |
| DE05-5 | Bike, Automobile, AudioZones ... | 40 | 363 | .bss | 0x0096914C–0x00C81308 | pendiente |
| DE05-6 | Cam, BulletInfo, C_PcSave ... | 40 | 664 | .bss | 0x009655E5–0x00C888AC | pendiente |
| DE05-7 | Cam | 40 | 124 | .bss | 0x00B6EC77–0x00B6F960 | pendiente |
| DE05-8 | Cam | 40 | 294 | .bss | 0x00B6F96C–0x00B6FFD8 | pendiente |
| DE05-9 | Cam | 40 | 193 | .bss | 0x00B6FFDC–0x00B700C8 | pendiente |
| DE05-10 | Cam, Camera | 40 | 181 | .bss | 0x009655A0–0x00C8A860 | pendiente |
| DE05-11 | Camera | 40 | 161 | .bss | 0x00B6EC65–0x00B6EEE0 | pendiente |
| DE05-12 | Camera | 40 | 260 | .bss | 0x00B6EEE4–0x00B7002C | pendiente |
| DE05-13 | Camera, CarCtrl | 40 | 163 | .bss | 0x008E4CD0–0x00C73C58 | pendiente |
| DE05-14 | CarCtrl, CarEnterExit, CdStreamInfo ... | 40 | 240 | .bss | 0x008E4000–0x00C2B974 | pendiente |
| DE05-15 | Clock, Clouds, ClothesBuilder ... | 40 | 2,813 | .bss | 0x008E4090–0x00C7F158 | pendiente |
| DE05-16 | Collision, ColAccel, Clouds ... | 40 | 207 | .bss | 0x00965554–0x00C6E974 | pendiente |
| DE05-17 | CullZones, Coronas, Conversations ... | 40 | 395 | .bss | 0x009691C4–0x00C92144 | pendiente |
| DE05-18 | CutsceneMgr, CustomCarEnvMapPipeline, CustomCarPlateMgr ... | 40 | 3,687 | .bss | 0x00AF00FF–0x00C8FF00 | pendiente |
| DE05-19 | CutsceneMgr, Darkel, CutsceneObject ... | 35 | 5,374 | .bss | 0x00969A48–0x00BC4058 | pendiente |
| DE05-20 | Darkel, EntryExitManager, Draw ... | 40 | 3,576 | .bss | 0x00969A50–0x00C3EFAA | pendiente |
| DE05-21 | Font, FileMgr, FileLoader ... | 40 | 1,974 | .bss | 0x0096913F–0x00C92368 | pendiente |
| DE05-22 | Game, Font, GameLogic ... | 40 | 516 | .bss | 0x0096A840–0x00C71AD8 | pendiente |
| DE05-23 | GangWars, GameLogic, Gamma | 40 | 561 | .bss | 0x0096A8A4–0x00C92134 | pendiente |
| DE05-24 | Garages, GenericGameStorage, Garage ... | 40 | 886 | .bss | 0x008E6940–0x00C16EBC | pendiente |
| DE05-25 | Glass, GenericGameStorage, Heli ... | 40 | 811 | .bss | 0x008E72F0–0x00C72FB0 | pendiente |
| DE05-26 | Hud | 40 | 134 | .bss | 0x00BA17C8–0x00BAA44C | pendiente |
| DE05-27 | Hud | 40 | 227 | .bss | 0x00BAA450–0x00BAB246 | pendiente |
| DE05-28 | JPegCompress, LoadingScreen, Interior_c ... | 40 | 434 | .bss | 0x0096F854–0x00C1B100 | pendiente |
| DE05-29 | LoadingScreen, MemoryMgr, Localisation ... | 27 | 1,376 | .bss | 0x00B74238–0x00C87C18 | pendiente |
| DE05-30 | MemoryMgr | 1 | 16,384 | .bss | 0x00C8E0C8–0x00C8E0C8 | pendiente |
| DE05-31 | MissionCleanup, Mirrors, MenuSystem ... | 40 | 160 | .bss | 0x009654B0–0x00C803D8 | pendiente |
| DE05-32 | MissionCleanup, ModelInfo, MotionBlurStreaks | 40 | 237 | .bss | 0x00A44B64–0x00C7C70D | pendiente |
| DE05-33 | Pad, Occluder, Occlusion ... | 40 | 558 | .bss | 0x00B6F19C–0x00C87C5C | pendiente |
| DE05-34 | PathFind, Pad, PedGroupDefaultTaskAllocators ... | 40 | 1,314 | .bss | 0x0096EA98–0x00C8CFB8 | pendiente |
| DE05-35 | Pickups, PedToPlayerConversations, Physical ... | 40 | 530 | .bss | 0x009691B0–0x00C1CADC | pendiente |
| DE05-36 | PlantMgr, PlayerPed, Plane ... | 40 | 1,093 | .bss | 0x0096917C–0x00C81320 | pendiente |
| DE05-37 | Pools, PlayerPed, PopCycle ... | 40 | 274 | .bss | 0x00B6F9AC–0x00C3F6E0 | pendiente |
| DE05-38 | Population, PopCycle | 40 | 850 | .bss | 0x00969159–0x00C0FCBC | pendiente |
| DE05-39 | PostEffects, Population | 40 | 2,257 | .bss | 0x00B6F1C4–0x00C4031C | pendiente |
| DE05-40 | Radar, PostEffects, ProjectileInfo ... | 40 | 438 | .bss | 0x00B745BC–0x00C9ABDC | pendiente |
| DE05-41 | Renderer, Replay, RenderBuffer ... | 40 | 1,398 | .bss | 0x0097F66D–0x00C815B8 | pendiente |
| DE05-42 | Replay | 40 | 135 | .bss | 0x0097FA80–0x0097FB60 | pendiente |
| DE05-43 | Restart, Replay, RoadBlocks ... | 40 | 656 | .bss | 0x0097FB64–0x00B781F8 | pendiente |
| DE05-44 | Shadows, Securom, Scripted2dEffects ... | 40 | 1,011 | .bss | 0x00A957F8–0x00C8CE68 | pendiente |
| DE05-45 | Shopping, Shadows, SpecialFX ... | 40 | 763 | .bss | 0x00A97258–0x00C89690 | pendiente |
| DE05-46 | Stats, Sprite2d, Sprite | 40 | 2,172 | .bss | 0x00969180–0x00C80468 | pendiente |
| DE05-47 | Streaming, StencilShadows, Stats | 40 | 399 | .bss | 0x008E48A0–0x00C6A899 | pendiente |
| DE05-48 | Streaming, StuntJumpManager, TagManager ... | 40 | 155 | .bss | 0x008E4C68–0x00C1A554 | pendiente |
| DE05-49 | TempColModels, TaskSimpleFight, TheCarGenerators ... | 40 | 770 | .bss | 0x00961ADC–0x00C27AD0 | pendiente |
| DE05-50 | TheScripts | 40 | 427 | .bss | 0x00A43F50–0x00A476AC | pendiente |
| DE05-51 | TheScripts, TheZones, TimeCycle | 40 | 711 | .bss | 0x00A476B0–0x00BA6724 | pendiente |
| DE05-52 | TimeCycle | 40 | 2,500 | .bss | 0x00B79F10–0x00B7BA70 | pendiente |
| DE05-53 | TimeCycle, Timer, TrafficLights | 40 | 1,138 | .bss | 0x00A9AD94–0x00B7CB89 | pendiente |
| DE05-54 | Vehicle, Train, TrafficLights ... | 40 | 355 | .bss | 0x00A9ADB0–0x00C88018 | pendiente |
| DE05-55 | VehicleModelInfo, VehicleRecording, Vehicle ... | 40 | 641 | .bss | 0x0097D670–0x00C1CDC0 | pendiente |
| DE05-56 | VisibilityPlugins, WaterLevel, VideoPlayer ... | 40 | 299 | .bss | 0x00B7C514–0x00C92118 | pendiente |
| DE05-57 | WaterLevel | 40 | 552 | .bss | 0x00C21170–0x00C4B954 | pendiente |
| DE05-58 | Weather, WeaponEffects, Weapon ... | 40 | 162 | .bss | 0x00B6F1A8–0x00C8AAB8 | pendiente |
| DE05-59 | Weather, WinPlatform, WinPs ... | 40 | 304 | .bss | 0x00C812EC–0x00C9BCE0 | pendiente |
| DE05-60 | World, app_light, app ... | 40 | 187 | .bss | 0x00B72C68–0x00C9246C | pendiente |
| DE05-61 | common, cTransmission, preset_view | 11 | 828 | .bss | 0x00B71510–0x00C1CB54 | pendiente |

## G6 · Datos del juego que aun lee codigo vivo del EXE (.data/.bss compartidos)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE06-1 | VideoMode, DelayLoadThunks, Plane ... | 20 | 202 | .bss/.data | 0x008D6220–0x00C97C34 | pendiente |

## G7 · Datos de RenderWare y D3D (cualquier seccion)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE07-1 | D3D9RasterDriver, D3D9RasterFormat, D3DResourceSystem ... | 40 | 263 | .bss/.data/.rdata | 0x00858B50–0x00C9C068 | pendiente |
| DE07-2 | WindowedMode, rpmatfx, rphanim ... | 40 | 292 | .bss/.data/.rdata | 0x008852F0–0x00C9BF00 | pendiente |
| DE07-3 | rpmatfx, rpskin, rpuvanim | 40 | 296 | .bss/.data/.rdata | 0x00882ED4–0x00C9ABC8 | pendiente |
| DE07-4 | rpworld, rpuvanim | 40 | 536 | .bss/.data/.rdata | 0x00874EF8–0x00C9B8D0 | pendiente |
| DE07-5 | rpworld | 40 | 352 | .bss | 0x00C924B4–0x00C92648 | pendiente |
| DE07-6 | rpworld | 40 | 160 | .bss | 0x00C9264C–0x00C926EC | pendiente |
| DE07-7 | rpworld, rpworld_s30a31a, rtanim | 40 | 697 | .bss/.data | 0x008D624C–0x00C9BFFC | pendiente |
| DE07-8 | rwcore, rtquat | 40 | 216 | .data/.rdata | 0x0085C675–0x008E2530 | pendiente |
| DE07-9 | rwcore | 40 | 369 | .bss/.data | 0x008E2534–0x00C97904 | pendiente |
| DE07-10 | rwcore | 40 | 348 | .bss | 0x00C9793C–0x00C980B0 | pendiente |
| DE07-11 | rwcore | 40 | 212 | .bss | 0x00C980B4–0x00C99298 | pendiente |
| DE07-12 | rwcore | 40 | 160 | .bss | 0x00C9929C–0x00C9A4D0 | pendiente |
| DE07-13 | rwcore | 40 | 256 | .bss | 0x00C9A4D4–0x00C9A6B0 | pendiente |
| DE07-14 | rwcore, rwplcore, rwcore_dds | 40 | 287 | .bss/.data/.rdata | 0x00884F6A–0x00C9C07C | pendiente |
| DE07-15 | rwplcore, rwtexdict, zlibinflate | 40 | 391 | .bss/.data/.rdata | 0x00884160–0x00C9BC5C | pendiente |
| DE07-16 | zlibinflate | 19 | 157 | .data/.rdata | 0x00884358–0x008E21B8 | pendiente |

## G8 · CRT, x87 y bibliotecas (cualquier seccion)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE08-1 | Cam, CrtException, CrtFile ... | 40 | 449 | .bss/.data/.rdata | 0x00858B1C–0x00C9AC4C | pendiente |
| DE08-2 | CrtPow, CrtLowIo, CrtHeap ... | 40 | 812 | .bss/.data/.rdata | 0x008884F0–0x00C9D42C | pendiente |
| DE08-3 | X87Math, CrtPow, CrtStdio ... | 40 | 349 | .bss/.data/.rdata | 0x00858624–0x00C9D420 | pendiente |
| DE08-4 | X87Math | 40 | 280 | .bss/.data/.rdata | 0x008631B8–0x00B6F8B0 | pendiente |
| DE08-5 | X87Math | 5 | 20 | .bss | 0x00B6F8B4–0x00C9ABFC | pendiente |

## G9 · vtables de las clases enganchadas (hook_vtable en .rdata/.data)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE09-1 | AudioEngine, AEAudioChannel, AEDataStream ... | 40 | 1,516 | .rdata | 0x008585C8–0x00872A74 | pendiente |
| DE09-2 | DummyObject, EmergencyPed, Entity ... | 40 | 2,164 | .rdata | 0x00858E68–0x0086FD18 | pendiente |
| DE09-3 | EventHighAngerAtPlayer, EventHitByWaterCannon, EventInAir ... | 40 | 2,560 | .rdata | 0x00859540–0x008701A8 | pendiente |
| DE09-4 | Task, EventVehicleCollision, EventVehicleDamage ... | 40 | 1,748 | .rdata | 0x0085A788–0x00871AE8 | pendiente |
| DE09-5 | TaskComplexCarDriveMission, TaskAllocatorKillThreatsDriveby, TaskAllocatorPlayerCommandAttack ... | 40 | 1,852 | .rdata | 0x00859DCC–0x00870EA8 | pendiente |
| DE09-6 | TaskComplexEvasiveDiveAndGetUp, TaskComplexEvasiveStep, TaskComplexExtinguishFireOnFoot ... | 40 | 1,800 | .rdata | 0x0086E040–0x00870DD0 | pendiente |
| DE09-7 | TaskComplexPresentIDToCop, TaskComplexJump, TaskComplexKillAllThreats ... | 40 | 1,824 | .rdata | 0x00858EB0–0x00870D78 | pendiente |
| DE09-8 | TaskComplexSeekEntityAnyMeansXYOffset, TaskComplexSeekEntityRadiusAngleOffset, TaskComplexSeekEntityStandard ... | 40 | 1,792 | .rdata | 0x00859DF8–0x00870DA4 | pendiente |
| DE09-9 | TaskComplexWalkRoundObject, TaskComplexWander, TaskComplexWanderCop ... | 40 | 1,748 | .rdata | 0x00859C80–0x00870B08 | pendiente |
| DE09-10 | TaskSimpleCarOpenLockedDoorFromOutside, TaskSimpleCarSetPedInAsDriver, TaskSimpleCarSetPedInAsPassenger ... | 40 | 1,464 | .rdata | 0x00859D20–0x00870BEC | pendiente |
| DE09-11 | TaskSimpleHitByGunFromFront, TaskSimpleHitByGunFromLeft, TaskSimpleHitByGunFromRear ... | 40 | 1,524 | .rdata | 0x00859CFC–0x00870B74 | pendiente |
| DE09-12 | TaskSimpleSetStayInSamePlace, TaskSimpleShakeFist, TaskSimpleSitDown ... | 29 | 1,216 | .rdata | 0x00859580–0x00872370 | pendiente |

## G10 · Proteccion del EXE (_TEXT_HA, _rwdseg)

| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |
| --- | --- | ---: | ---: | --- | --- | --- |
| DE10-1 | ExeProtectionStub | 40 | 217 | _TEXT_HA | 0x00C9E068–0x00C9E573 | pendiente |
| DE10-2 | ExeProtectionStub | 40 | 160 | _TEXT_HA | 0x00C9E577–0x00C9E744 | pendiente |
| DE10-3 | ExeProtectionStub | 40 | 160 | _TEXT_HA | 0x00C9E752–0x00C9E8DA | pendiente |
| DE10-4 | ExeProtectionStub | 40 | 160 | _TEXT_HA | 0x00C9E8DE–0x00C9E986 | pendiente |
| DE10-5 | ExeProtectionStub | 10 | 100 | _TEXT_HA | 0x00C9E98A–0x00C9E9CF | pendiente |
