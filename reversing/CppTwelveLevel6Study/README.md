<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Oleadas 1 y 2 del nivel 6: los 25 archivos restantes sin ensamblador (16/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador en línea y todas las declaraciones `naked` de los 25 archivos que quedaban en [FILES.md](../CppOnlyMigration/FILES.md): oleada 1 (`AEVehicleAudioEntity`, `TaskSimpleJetPack`, `PathFind`, `TaskSimpleSwim`, `Bmx`, `PedIK`, `WaterLevel`, `MemoryMgr`, `Physical`, `Radar`, `Pad` y `Matrix`) y oleada 2 (`Automobile`, `TaskSimpleFight`, `Vehicle`, `DecisionMakerTypes`, `Garage`, `Formation`, `Bike`, `Camera`, `Plane`, `Interior_c`, `Shadows`, `Stats` y `Cam`). Con esto `source/game_sa` queda con 0 apariciones de ensamblador y 0 declaraciones `naked` (se comprobó con `grep` sobre el árbol de trabajo y con `contrib/audit_cpp_assembly.py`). Además se convirtieron tres bloques sueltos (`CWorld::Process`, `CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed` y `CMenuManager::SaveStatsToFile`), los dos `naked` y el getter de `source/app/platform/platform.cpp` (`rsCommandLine_Eax`, `rsPreInitCommandLine_Eax` y `psGetMemoryFunctions`) y el bloque `fptan` de `source/app/app_game.cpp` (`ExeViewWindow`), y se añadieron dos hooks: `CStats::UpdateSexAppealStat` (banco `DependenciesStatsSex16`) y `CPlayerPed::ControlButtonSprint` (banco `DependenciesTaskSprint16`; antes delegaba en el EXE). El diff de `source/` suma 14653 líneas añadidas y 28861 eliminadas en 35 archivos; `X87Math.cpp` recibe 7131 de esas líneas y `X87Math.h` 720 (los ayudantes compartidos, sin copias en las clases).

## Excepción documentada: `WindowedMode.cpp`

`source/app/platform/win/WindowedMode.cpp` conserva `__asm` en `HookDirect3DDeviceReplacerSA`. No es código reconstruido del EXE sino una extensión NOTSA (modo ventana) y está fuera de `source/game_sa`, así que no entra en los conteos de [FILES.md](../CppOnlyMigration/FILES.md). Decisión del propietario (opción A): se deja como excepción a la política de [PROCESO.md](../CppOnlyMigration/PROCESO.md), anotada allí y en el README de `CppOnlyMigration`.

## Qué se hizo

- **Ayudantes x87 compartidos.** Cada `__asm` se sustituyó por una función C++ en `X87Math.cpp/.h` (precisión completa de `fsin`, `fcos`, `fpatan`, `fptan`, `fsqrt`, `_CIpow`, `_CIasin`, `_ftol2`...) o por código normal; las clases no copian helpers. Las envolturas que el EXE llama con argumentos en registros pasaron de `naked` a funciones con `EntryEaxCpp`/`ResidueRegsCpp` (técnicas de [PROCESO.md](../CppOnlyMigration/PROCESO.md)). Las nuevas envolturas `...Exe` y los ayudantes de residuo los vigila `contrib/audit_register_residue.py`, que amplió su lista `OBJECTS` con `MemoryMgr`, `Pad`, `PathFind`, `AEVehicleAudioEntity`, `WaterLevel`, `Radar`, `Stats`, `Vehicle`, `TaskSimpleFight`, `DecisionMakerTypes`, `Garage`, `Interior_c` y `platform`.
- **Opciones de compilación por archivo** (`source/CMakeLists.txt`, nunca globales): `Stats.cpp` y `DecisionMakerTypes.cpp` pasan a `/Zi` sin comprobaciones de ejecución, con `#pragma optimize` sólo en la función que necesita el marco de pila del EXE (`CStats::LoadStatUpdateConditions` y `CDecisionMakerTypes::LoadEventIndices`, cuyos colaboradores dejan en ECX una dirección de su propio marco que los bancos comparan); `Interior_c.cpp` ya no tiene `__asm` ni `naked` pero conserva `/arch:IA32` y `float_control(precise, on)` para que todo `float` sea x87 con redondeo a 24 bits.
- **`MemoryMgr`.** `DependenciesMemory9` exigía instrucciones idénticas a las del EXE y no sirve para aceptar C++ con otro código generado: queda retirado. El banco de `CMemoryMgr::Malloc`, `Realloc`, `MallocAlign` y `FreeAlign` es `DependenciesMemoryCpp16` (8192 pares: 2048 por función; conserva argumentos, pila, no volátiles, payload, alineamiento y balance de recursos).
- **Hook retirado.** `CShadows::ShadowRenderTriangleCB` (0x709CF0) ya no se engancha; `HookUnlockAudit` usa `retired.py` (420 revisiones y 1 baja justificada). El cuerpo sigue convertido en `Shadows.cpp`.
- **Bancos nuevos.** `CppAutomobileX87Stress1`, `CppBmxX87Stress1`, `CppCamX87Stress1`, `CppCameraX87Stress1`, `CppPlaneX87Stress1` (ayudantes x87 con patrones de estrés: NaN de distinto signo y carga, señalizantes, ±inf, ±0, subnormales, ±1e30, FLT_MAX; comparación en bits, ESP y no volátiles, pila y palabra de control x87 y `errno`), `CppMatrixExact1` (diez funciones de `CMatrix`), `CppFormationNumeric`, `CppVehicleTransmission1`, `DependenciesStatsSex16`, `DependenciesTaskSprint16`, `DependenciesMemoryCpp16` y `DependenciesGarageSphere15`. Los bancos existentes se actualizaron con las interceptaciones de la DLL y los gates que exigían las fuentes nuevas (97 carpetas de `reversing/` con cambios en el árbol de trabajo); ningún banco relajó una comparación para aprobar.

## Qué se encontró

- `DependenciesWorld4` y `CppIntegerWorld4` (`CGarages::FindGarageForObject`, 4096 de 4096 pares con `helper_route_ok`) fallaban porque un arreglo global de `OriginalDependencies499/HookRoute.hpp` pisaba el cuerpo C++ explícito del banco de `Garage`. El arreglo global se revirtió y el de `DependenciesStatsFile15` quedó local (usa `StatsLoadStatUpdateConditionsExe` como candidato).
- `RequestedPadDistanceTrainNopHexV2`: el diagnóstico inicial (falta de `comtypes`) era erróneo, porque `comtypes` 1.4.17 sí estaba instalado; vuelve a fallar en `check_train_abi.py` con `ModuleNotFoundError: No module named 'comtypes'` en la última pasada (`final_raw4.txt`), así que la causa queda sin determinar.
- `ShadowSectors355` se colgó en una pasada («GTA no responde», 30 s) y aprobó al repetir.
- `PlaneBlow355` dejó un `gta_sa.exe` huérfano que bloqueó la cola; el proceso que parecía girar era en realidad `PowEquivalence` (banco de unos 40 minutos de CPU) de la cola anterior, que se mató por error y hay que repetir. Regla: no matar procesos `gta_sa` sin comprobar con el sello de tiempo del `.asi` de `scripts\` a qué banco pertenecen.
- La regresión final se detuvo una vez (54 bancos de 148) y se reanudó tras aplicar los parches de `World.cpp`, `TaskSimpleCarSetPedSlowDraggedOut.cpp`, `MenuManager.cpp`, `platform.cpp`, `app_game.cpp`, sexappeal, sprint y `HookUnlockAudit`; los bancos que vigilan esas fuentes (`WldD`, `CTasks6b`, `MenuMgrA`..`MenuMgrD`, `Rs27`, `GameIdle`, `HookUnlockGame`, `CppFormationNumeric`, `DependenciesFormationMemory8` y los que listan `World.cpp`) se repiten.

## Estado de la regresión final

Cola de 193 bancos: 190 con corrida aprobada en `reversing/CppTwelveLevel6Final/runs` con el código final. En [approvals.json](../CppOnlyMigration/approvals.json) se registró sólo evidencia de corridas aprobadas (la más reciente de cada banco: 120 de `CppTwelveLevel6Final/runs` y 7 de `CppTwelveLevel6Unified/runs`, que son corridas aprobadas posteriores al arreglo del assert de arranque; las corridas fallidas y las anteriores invalidadas por ese assert no se registran). Bancos de la cola sin corrida aprobada con el código final en el momento de escribir esto:

- `RequestedMenuBatch`: la sonda no llego a ejecutarse (error de arranque, ver final_raw4.txt)
- `RequestedParallelTasks`: la sonda no llego a ejecutarse (error de arranque, ver final_raw4.txt)
- `UnlockFileMgr`: corrida fallida (FALLO: Desbloqueos: archivos del motor); sin corrida aprobada

Funciones convertidas que todavía no tienen evidencia registrada por este motivo:

- `CPad::ProcessPad` (0x00746A10) en `source/game_sa/Pad.cpp`: bancos que nombran la función o su dirección (puede que sólo la mencionen): `PadMouse1`, `RequestedPadMutationNext`, `RequestedPadProcessPad`, `RequestedPadSelectors`, `RequestedPadUpdateMouse`, `RequestedPadUpdateMouseV2`, `RequestedPadUpdateMouseV3`
- `CVehicle::ProcessDrivingAnims` (0x006DF4A0) en `source/game_sa/Entity/Vehicle/Vehicle.cpp`: bancos que nombran la función o su dirección (puede que sólo la mencionen): `AutomobileFinal4`, `CTasks61a`, `CVehD`, `CVehicle`, `Misc61b`, `VehWave2`

Para registrarlas cuando aprueben los bancos que faltan basta volver a ejecutar el generador del registro (es idempotente: borra y regenera las entradas de este lote con las corridas aprobadas que haya en ese momento).

## Bancos con evidencia registrada

| Banco | Pares | Corrida | Directorio |
| --- | ---: | --- | --- |
| AudioVehiclePending | 4096 | 20261015-185750-852724 | Unified |
| AutomobileFinal4 | 24192 | 20261016-122154-502438 | Final |
| BikeBlow355 | 1024 | 20261016-193922-158352 | Final |
| BikeControl355 | 1024 | 20261016-220732-341236 | Final |
| BikeDamage355 | 1024 | 20261016-220829-670390 | Final |
| BikeHands355 | 1024 | 20261016-193033-927467 | Final |
| BikeRoad355 | 1024 | 20261016-193116-100368 | Final |
| BikeWave4 | 2816 | 20261016-193321-493697 | Final |
| CTasks14b | 12352 | 20261016-112626-652350 | Final |
| CTasks6b | 13952 | 20261016-194350-854678 | Final |
| CVehB | 11520 | 20261016-185950-921873 | Final |
| CVehC | 8640 | 20261016-190208-599328 | Final |
| CVehE | 8128 | 20261016-190257-466548 | Final |
| CamArrest355 | 12288 | 20261016-182226-257992 | Final |
| CamAttached355 | 4096 | 20261016-182312-140146 | Final |
| CamBirdy355 | 16384 | 20261016-182353-567801 | Final |
| CamControl355 | 24576 | 20261016-194603-643482 | Final |
| CamDead355 | 4096 | 20261016-182616-984565 | Final |
| CamFirstPerson355 | 1024 | 20261016-120158-359660 | Final |
| CamFlyBy355 | 4096 | 20261016-182658-575847 | Final |
| CamHeliChase355 | 16384 | 20261016-183135-965899 | Final |
| CamHud1 | 18688 | 20261016-184927-000394 | Final |
| CamLookBehind355 | 8192 | 20261016-183310-299737 | Final |
| CamM16355 | 1024 | 20261016-183440-010146 | Final |
| CamMan355 | 4096 | 20261016-183620-180975 | Final |
| CamMouse355 | 1024 | 20261016-183702-367727 | Final |
| CamPlane355 | 2048 | 20261016-183836-169558 | Final |
| CamRotate355 | 4096 | 20261016-184138-352578 | Final |
| CamShooting355 | 1024 | 20261016-184222-808383 | Final |
| CamSpotter355 | 2048 | 20261016-184410-210264 | Final |
| CamTwoPlayer355 | 1024 | 20261016-184656-145465 | Final |
| CamWheel355 | 16384 | 20261016-184836-292388 | Final |
| CameraCollision355 | 4096 | 20261016-180605-750467 | Final |
| CameraDerived355 | 2048 | 20261016-180346-470577 | Final |
| CameraFinal4 | 19648 | 20261016-175826-241231 | Final |
| CameraFrustum355 | 2048 | 20261016-180300-633957 | Final |
| CameraImprove355 | 4096 | 20261016-180428-764600 | Final |
| CameraLinear355 | 7168 | 20261016-180047-117325 | Final |
| CameraNear355 | 2048 | 20261016-180003-365936 | Final |
| CameraNewMode355 | 16384 | 20261016-221015-576250 | Final |
| CameraProcess355 | 16384 | 20261016-180140-809502 | Final |
| CppAutomobileX87Stress1 | 22528 | 20261016-122459-784512 | Final |
| CppBmxX87Stress1 | 24576 | 20261016-193743-732966 | Final |
| CppCamX87Stress1 | 43008 | 20261016-185152-121368 | Final |
| CppFormationNumeric | 8256 | 20261016-194724-003969 | Final |
| CppIntegerWorld10 | 4096 | 20261016-181003-181645 | Final |
| CppMatrixExact1 | 21504 | 20261016-113237-723354 | Final |
| CppPlaneX87Stress1 | 47104 | 20261016-192510-497984 | Final |
| CppShadowSqrtStatic | 4096 | 20261016-125609-770665 | Final |
| CppTrigCompound6Swim | 1024 | 20261016-193834-125655 | Final |
| CppTrigJetPack | 1536 | 20261015-190040-040616 | Unified |
| CppTrigSwim | 2048 | 20261016-112309-394177 | Final |
| CppVehicleTransmission1 | 1024 | 20261016-185457-654578 | Final |
| Data23 | 13904 | 20261016-123702-661817 | Final |
| DependenciesDecisionPed7 | 8192 | 20261016-122608-961860 | Final |
| DependenciesDecisionRelationship6 | 6272 | 20261016-122721-058572 | Final |
| DependenciesFightLeaf2 | 6144 | 20261016-121448-943058 | Final |
| DependenciesFormationMemory8 | 8256 | 20261016-194810-622526 | Final |
| DependenciesGarage2 | 2560 | 20261016-123900-615906 | Final |
| DependenciesGarageSphere15 | 2048 | 20261016-185730-117758 | Final |
| DependenciesLeaf2 | 2048 | 20261016-120347-027634 | Final |
| DependenciesMemoryCpp16 | 8192 | 20261016-120649-844833 | Final |
| DependenciesPadCamera7 | 20480 | 20261016-115747-239543 | Final |
| DependenciesPathDensity13 | 2048 | 20261015-185308-477835 | Unified |
| DependenciesPathOrientation14 | 2048 | 20261016-112004-865600 | Final |
| DependenciesStats1 | 4096 | 20261016-210410-592785 | Final |
| DependenciesStatsFile15 | 2048 | 20261016-204159-608328 | Final |
| DependenciesStatsRadio7 | 4096 | 20261016-210458-348714 | Final |
| DependenciesStatsSex16 | 4096 | 20261016-224645-599852 | Final |
| DependenciesStatsUi8 | 8192 | 20261016-205425-970181 | Final |
| DependenciesTask11 | 4096 | 20261016-122844-529042 | Final |
| DependenciesTask12 | 2048 | 20261016-121545-583201 | Final |
| DependenciesTaskLeaf3 | 4096 | 20261016-121648-861433 | Final |
| DependenciesTaskPed5 | 8192 | 20261016-121753-445698 | Final |
| DependenciesTaskSprint16 | 3848 | 20261016-204102-478023 | Final |
| DependenciesVehicle12 | 2048 | 20261016-185539-956559 | Final |
| DependenciesVehicleLeaf5 | 5120 | 20261015-190928-798989 | Unified |
| DependenciesVehicleStats10 | 6144 | 20261016-205822-476807 | Final |
| DependenciesVehicleStats11 | 4096 | 20261016-210044-734372 | Final |
| DependenciesVehicleStats9 | 4096 | 20261016-205643-159616 | Final |
| DependenciesWorld10 | 4096 | 20261016-180918-861095 | Final |
| DependenciesWorld13 | 2048 | 20261015-190543-781737 | Unified |
| DependenciesWorld15 | 2048 | 20261016-123957-836268 | Final |
| DependenciesWorld9 | 4096 | 20261016-114922-960661 | Final |
| DependenciesWorldFxFont5 | 15360 | 20261016-115550-702922 | Final |
| DependenciesWorldLeaf3 | 8192 | 20261016-115854-749307 | Final |
| DependenciesWorldWaterPath6 | 24576 | 20261016-115018-015540 | Final |
| FixCore | 11776 | 20261016-113335-588078 | Final |
| FormationAssign355 | 3072 | 20261016-112914-957407 | Final |
| FormationGather355 | 2048 | 20261016-113004-255524 | Final |
| FormationLookup355 | 6144 | 20261016-113054-040985 | Final |
| GameIdle | 2048 | 20261016-195026-014337 | Final |
| GfxWave2 | 27616 | 20261016-114714-823169 | Final |
| HudCamFinal2 | 6528 | 20261016-185047-979689 | Final |
| InteriorBase | 25600 | 20261016-191147-095953 | Final |
| InteriorCommerce | 16384 | 20261016-190457-574435 | Final |
| InteriorHome | 9216 | 20261016-190352-720316 | Final |
| InteriorLifecycleCore | 6144 | 20261016-191300-124513 | Final |
| InteriorLifecycleRuntime | 4096 | 20261016-191446-734755 | Final |
| InteriorSpatial | 7168 | 20261016-191811-214318 | Final |
| MenuMgrD | 6784 | 20261016-201408-733744 | Final |
| Misc61b | 21504 | 20261015-191811-619392 | Unified |
| Mix11 | 19520 | 20261015-191125-020119 | Unified |
| PathFinal3 | 26880 | 20261016-112150-837656 | Final |
| Ped9 | 14720 | 20261016-112415-626518 | Final |
| Physical56 | 23808 | 20261016-113745-957664 | Final |
| PlaneAmbient355 | 1024 | 20261016-201505-803550 | Final |
| PlaneBlow355 | 1024 | 20261016-222739-736574 | Final |
| PlaneCreation355 | 1024 | 20261016-192331-031853 | Final |
| PlaneDamage355 | 1024 | 20261016-192243-363397 | Final |
| PlanePreRender355 | 16384 | 20261016-191955-038544 | Final |
| RadarFinal3 | 15040 | 20261016-115340-805011 | Final |
| RequestedMenuBatch-formation | 256 | 20261016-231642-236947 | Final |
| RequestedMenuBatch-formation-expanded | 4096 | 20261016-231919-438699 | Final |
| RequestedPadDistanceTrainNopHexV2 | 13952 | 20261016-224926-683909 | Final |
| Rs27 | 6464 | 20261016-223142-443670 | Final |
| ShadowPermanent355 | 8192 | 20261016-124611-578082 | Final |
| ShadowSectors355 | 6144 | 20261016-205337-013934 | Final |
| ShadowVehicle355 | 6144 | 20261016-125417-195948 | Final |
| ShadowXY355 | 6144 | 20261016-124736-314596 | Final |
| Stats32 | 13056 | 20261016-210300-035228 | Final |
| VehAudioFinal4 | 20024 | 20261016-193545-643431 | Final |
| VehWave2 | 12864 | 20261016-193410-180887 | Final |
| WaterNearest355 | 1024 | 20261016-115133-840887 | Final |
| WaterQuery355 | 8192 | 20261016-114620-689254 | Final |
| WaterRender355 | 1024 | 20261016-115230-496668 | Final |
| WldD | 6272 | 20261016-223018-769760 | Final |

## Límites

- Las cifras de `differences` y `null_comparison_records` del registro son 0 porque cada corrida registrada tiene `passed` y `all_equal` verdaderos en su `summary.json`; no se recontaron las comparaciones de `comparisons.jsonl.gz` (su formato cambia de un banco a otro).
- Los pares propios de cada función son los de `pairs_per_function` del resumen del banco. Los ayudantes sin nombre de clase se agrupan por archivo y se acreditan por el banco de estrés de su familia (`Automobile::`, `Bmx::`, `Cam::`, `Plane::`) o, si no lo hay, por los bancos de las funciones de la clase que los consumen; no hay un banco por ayudante en esos archivos.
- Los bancos de auditoría de bloqueos (`HookUnlock*`, `Unlock*`) no se usan como evidencia de ninguna función.
- Sólo se archivan los objetos «antes»; no hay objetos «después» en `artifacts`.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.
- Una corrida aprobada no cubre los cambios de fuente posteriores a ella: se aplicaron parches mientras corría la cola (`sexappeal`, `sprint`, `file15`, `misc3`, `HookUnlockAudit`) y los bancos afectados se repiten.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| AEVehicleAudioEntity.actual-before.obj | 6f7b35c856d410791fdf03a37cfc8f01168464231785f131f607c9a225b308f5 |
| Bmx.actual-before.obj | d5b726f3190add66fea1f42a983a6f656ce93f309c5fd06c7b40d9c3a990c24f |
| Matrix.actual-before.obj | 4b0d26514b88060f3de3bf17cf8bc76d630f1938fdbe40cf23b39685d5744472 |
| MemoryMgr.actual-before.obj | cf2a972a1afe6be84450d8837059cf2976d9343e7ba57e6c9519a1d0de43693e |
| Pad.actual-before.obj | fe7714a03cc8b85fe2146639e13c8c4367c7da3a7e30b6c66f8dc27660949625 |
| PathFind.actual-before.obj | 612a6a88f478c0bc0bb4bcebaf834c476247e54693e53a1e04c46c068963d451 |
| PedIK.actual-before.obj | 2925f1e7cc0b169a989c0ee18b232b8cea881045aaf77bc65c85d963aaa8c606 |
| Physical.actual-before.obj | 14a1f1ea562fd5e98f9e36e06ced8ec186962409878f785c3450b220558e6873 |
| Radar.actual-before.obj | 9b3adddf50c64d100d0fb71c78b3a72f641e3c1970b0e248c50eafef623f962b |
| TaskSimpleJetPack.actual-before.obj | 0d5141d280f9aeb2a8e0f463a49c7db1409bb4a5bd2dacd55d4f14e33d3d99f3 |
| TaskSimpleSwim.actual-before.obj | 85f34bf772a4a404f91c395458a41ad570f0a2ec3b91eb85d08fabb1fb20493e |
| WaterLevel.actual-before.obj | ef6abbf086b6b44ff509d048e1f04971cbd7c853e3b6095b5c25173573d90f10 |
| X87Math.actual-before.obj | 28c9a0269d4c7e1df13eed59ee87e7f8c44fad947aead7cbbc9b2a9f24652ac7 |
