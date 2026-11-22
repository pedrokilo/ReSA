# Ejecución de P1 (descartes V1–V3) y P2 (puentes por dirección)

Rama `vivo-p2` (desde `origin/main` 6224549cf8, 21/11/2026). Herramientas nuevas: `contrib/codigo_vivo_cierre.py` (P1) y `contrib/codigo_vivo_cierre_p2.py` (P2). El estado por lote queda en [cierre.csv](cierre.csv), que `contrib/codigo_vivo_plan.py` vuelve a volcar en la columna `estado` de [plan.csv](plan.csv) y en la tabla de [plan-lotes.md](plan-lotes.md); el estado por función en [cierre-p1.csv](cierre-p1.csv) (y la columna `cierre` de [funciones-vivas.csv](funciones-vivas.csv)) y por dirección en [cierre-p2.csv](cierre-p2.csv).

## 1. P1: verificación de los descartes

El informe original dejó las funciones como «DESCARTAR» por un análisis de grafo sobre las funciones de Ghidra. Aquí se vuelve a comprobar, de forma independiente, que ningún código que pueda ejecutarse las alcanza:

1. **Aristas directas** (`call`, `jmp`, `jcc`, inmediato) desde funciones vivas que no se descartan en la misma pasada.
2. **Búsqueda bruta de la dirección** como dword little-endian en toda la imagen del EXE (todas las secciones, cualquier alineación: punteros, `push`/`mov` inmediatos, tablas, inmediatos dentro de instrucciones mal decodificadas). Cada acierto se clasifica por la función o el hueco que lo contiene y por el estado de esa función (reemplazada, viva, descartada, dato).
3. **Referencias del C++** por literal (sin contar los enganches `RH_Scoped*`).
4. **Direcciones calculadas / llamadas por registro**: se listan, en las funciones vivas que se quedan, las llamadas indirectas por registro y las constantes cercanas a la base de imagen (0x400000–0x400FFF). No hay análisis de flujo de datos, así que es una cota residual, no una prueba.

Una función queda **cerrada** si ninguna de las cuatro comprobaciones la alcanza desde código vivo. Las referencias desde funciones reemplazadas o descartadas, desde envolturas de excepciones y desde mapas de excepciones (funclets de funciones reemplazadas) no cuentan. Quedan **condicionadas** las que solo tienen referencias desde huecos de código o tablas del `.text` por clasificar (parte 6) o cuyo dueño de excepciones sigue vivo, y **abiertas** las que alguien alcanza.

| Lote | Funciones | Cerradas | Abiertas | Condicionadas | Estado |
| --- | ---: | ---: | ---: | ---: | --- |
| V1 protector | 18 | 11 | 6 | 1 | parcial |
| V2-1 + V2-2 excluidas del juego | 85 | 53 | 3 | 29 | parcial |
| V3-1 … V3-6 funclets de excepciones | 3.504 | 3.502 | 1 | 1 | V3-2, V3-4, V3-5 y V3-6 cerrados; V3-1 y V3-3 parciales |
| **Total V1–V3** | **3.607** | **3.566 (51.337 bytes)** | 10 | 31 | |

Detalle de lo que no se cierra:

* **V1 (6 abiertas):** las cita `ExeProtectionStub.cpp` en la tabla `CALLBACK_STORES` (direcciones 0x4027CD, 0x402A36, 0x402DC5, 0x4032C5, 0x40335F, 0x403379). Mientras la DLL vaya sobre el EXE, el protector del EXE lee esa tabla; se retiran junto con el archivo entero al sustituir el protector (y su entrada en `hooks.csv`). No se tocó: borrar `ExeProtectionStub` cambia el arranque con la DLL.
* **V2 (3 abiertas):** 0x4A2BB0 (`FxEmitter.cpp`: `jmp` al cuerpo del destructor de `FxPrim_c`, lo llama el destructor del EXE con ABI propia), 0x85742B (`rwcore.cpp`: `fldcw [0xCAF004]`, hay que escribirlo con `_control87`) y 0x82C0B9 (la llama la entrada del CRT 0x824570, no es un descarte sino fase 8). Las 29 condicionadas son `usercall-cubierta` con una llamada desde un hueco de código sin función (parte 6) y `salto-puro`/`no-es-función` con un puntero en `.rdata` (vtables de clases ya en C++).
* **V3 (2):** 0x781A04 tiene un dueño de excepciones vivo (0x7819CA, D3DX9); se cierra con V4/E3. 0x83E2E8 (V3-3) la cita 0x8226D6, función del CRT que sigue viva.

### V4–V8: verificados, no cerrados

Se ejecutó la misma comprobación sobre V4 a V8 (solo informe, estado `verificado sin cerrar` en [cierre.csv](cierre.csv)). **No se retiró nada de V5–V8**: aunque la parte estática sale limpia para 625 funciones de D3DX9 (V4), 162 del CRT (V5), 30 de QuickTime (V6), 14 de RenderWare (V7) y 14 de libpng (V8), no se pueden descartar con garantías las **114 llamadas por registro** que quedan en las ~285 funciones vivas que se conservan (el resto de V5 tiene 12 abiertas: las llaman D3DX9 o la entrada del CRT). V4 además depende de E3 (enlazar `d3dx9_43.dll`). Para cerrarlas hace falta revisar esas 114 llamadas (la lista sale con `codigo_vivo_cierre.py --lotes ...`) o probarlo con una traza del juego.

## 2. P2: puentes por dirección

`codigo_vivo_cierre_p2.py` vuelve a escanear `source/` con el mismo criterio que la auditoría (literales hexadecimales en 0x401000–0x857FFF fuera de comentarios, separando los enganches) y cuenta lo que queda por dirección.

| | Antes | Después |
| --- | ---: | ---: |
| Entradas de función reemplazadas o enganchadas que el C++ llama por dirección (B1–B18) | 434 | 271 sin cambio, 6 parciales |
| …sustituidas por la función C++ (sin ninguna llamada por dirección) | 0 | **92** |
| …que no eran puentes (tabla de instalación de enganches de `Cheat.cpp`, 65 filas) | 0 | 65 |
| Referencias que no son enganche (B1–B18) | 588 | 373 |

### Qué se sustituyó

Todo con la función C++ que ya tenía enganche en esa dirección; el mecanismo es el mismo que antes (el enganche enviaba la llamada a esa función), ahora sin pasar por la dirección:

* **Tests de colisión** (`Collision.cpp`): `CCollision::Tests` (bajo `#ifdef TEST_COLLISION_FUNCS`, comparaba contra el EXE) se retiró entera con su declaración (14 direcciones: 0x411E70, 0x411EC0, 0x4120C0, 0x412130, 0x412700, 0x412850, 0x412A30, 0x412AA0, 0x413070, 0x413080, 0x413AC0, 0x415730, 0x415A40, 0x4165B0, 0x417470).
* **Aritmética de vectores y matrices**: 0x59C890 (nueva `MatrixTimesPointExe`, envoltura sin `[[deprecated]]` de la misma función que engancha `operator*`, 17 llamadas), 0x59C790 (`Multiply3x3Exe`), 0x59C910 (`CVector::Normalise`), 0x54ECE0 (`TransformPoint`), 0x407180 (`CGeneral::GetRandomIntInRange`, solo en `Heli.cpp`).
* **CarCtrl y vehículos**: `StopCarIfNodesAreInvalid` (0x422590), `PickNextNodeAccordingStrategy` (0x432B10), `DragCarToPoint` (0x42EC90), `CPlaceable::GetRightVector` (0x41CC70), `CVehicle::operator new` (0x6E2D50), `GetRemapIndex`, `SetRemap`, `SetupUpgradesAfterLoad` (0x6D0B70, 0x6D0C00, 0x6E3400), `FireFixedMachineGuns` (0x73DF00), constructores de vehículo (`CAutomobile`, `CHeli`, `CPlane`, `CBoat`, `CBike`, `CBmx`, `CMonsterTruck`, `CQuadBike`, `CTrailer`: 0x6B0A90, 0x6C4190, 0x6C8E20, 0x6F2940, 0x6BF430, 0x6BF820, 0x6C8D60, 0x6CE370, 0x6D03A0) y `CTrainNode::GetLightingFromCollision` (0x6F5F80).
* **Streaming, tareas y eventos**: `HasVehicleUpgradeLoaded`, `RequestModel`, `RequestVehicleUpgrade` (0x407820, 0x4087E0, 0x408C70), `CTaskManager::SetTask`/`SetTaskSecondary` (0x681AF0, 0x681B60), constructores de `CTaskComplexSequence`, `LeaveCarAndWander`, `EnterCarAsDriver`, `EnterCarAsPassenger`, `SeekEntity`, `WanderCriminal`, `KillPedOnFoot`, `WaitForPed`, `CCivilianPed`, `CEventScriptCommand` y `CEventGroup::Add` (0x632BD0, 0x632D10, 0x63BDD0, 0x6402F0, 0x640340, 0x661DC0, 0x48E610, 0x620E30, 0x683340, 0x5DDB70, 0x4B0A00, 0x4B0A50, 0x4AB420), `CPedPool::GetRef` (0x4442D0), `ApplyTurnForceComponents` (0x65DA70), `CAttractorScanner::ScanEffect` (0x5FFFD0), `CPathFind` (0x44DA30, 0x44DF60), `CGeneral::GetRadianAngleBetweenPoints` (0x53CBE0), `CAERadioTrackManager::Add*IndexToHistory` (0x4E9720, 0x4E9760, 0x4E97B0), `cHandlingDataMgr::GetFlyingPointer` (0x6F52D0).
* **Cámara y juego**: `ProcessObbeCinemaCamera{Car,Train,Heli,Plane,Boat}` (0x5267C0, 0x526950, 0x526AE0, 0x526C80, 0x526E20), `WellBufferMe` (0x509AE0), `DefinedState` (0x734650), `Ps::GetDXVersion` (0x745650), `GetFirstAtomicCallback`/`SkinAtomicGetHAnimHierarchCB` (0x734810, 0x734A20; solo se pasan a `RpClumpForAllAtomics`, no se guardan).
* **RenderWare/D3D9**: `_rwerror` (0x8088D0, 9 llamadas), `_rwSqrt` (0x7EDB30), `RpGeometryAddRef`, `_rpD3D9PluginAttach`, `_rpWorldPipelinesAttach`, `_rpBuildStripListJoin` (0x74CCB0, 0x74EA40, 0x75E3E0, 0x75A660), `_rwD3D9Im2DRenderOpen/Close`, `_rwD3D9Im3DRenderOpen`, `_rwD3D9RenderStateReset`, `_rwD3D9DynamicVertexBufferRestore` (0x7FB480, 0x7FB5F0, 0x80E020, 0x7FD100, 0x7F58D0), `D3D9RasterFormat::VideoMemoryRasterListRestore` y `ConvertRasterToDisplayFormat` (0x4CC970, 0x4CD250) y las comprobaciones de formato y la lista de rasters del controlador (`RwD3D9RasterDriver::*Cpp`, 0x4CB530, 0x4CBB00, 0x4CBD80, 0x4CBDE0, 0x4CBE20, 0x4CBE80, 0x4CBF20, 0x4CBF80).

### Coordinación con las otras ramas

Se probó una fusión de prueba (`git merge-tree`) con `datos-exe`, `crt4a-locale` y `new-delete-d3dx9`. Tres líneas que tocaba esta rama también las cambia `datos-exe` (llevan `StaticRef<…>(0x…)`): `Cam.cpp` (la de «BUSTED», 0x6A0050 y 0x69F2B0, y la de `IsCoopGameGoingOn`, 0x441390) y `CarCtrl.cpp` (el sorteo de 0x407180). Se dejaron en su forma original y quedan como pendientes de esa rama. Sin ellas no hay conflictos con `datos-exe` ni con `crt4a-locale`; el único conflicto con `new-delete-d3dx9` (`docs/hooks.csv`) ya existe entre esa rama y `origin/main`, no lo causa esta. No se tocó `Crt/*`, `MemoryMgr`, `DatosExe/*` ni `new`/`delete`.

### Qué NO se tocó y por qué ([cierre-p2.csv](cierre-p2.csv), columna `motivo`)

| Motivo | Direcciones |
| --- | ---: |
| ABI propia del EXE (registros residuales, bits de float) o puntero a función cuyo valor compara el EXE | 39 |
| RenderWare/D3D9: puntero guardado o función de uso interno sin API C++ pública | 80 |
| Vector de la STL del EXE llamado por dirección para que los bancos lo vieran (`PedAttractor.cpp`, `AEUserRadioTrackManager.cpp`: reescribir plantillas) | 55 |
| libjpeg: punteros a función guardados en estructuras que lee el EXE | 45 |
| CRT y arranque (ramas `crt4a-locale` y `new-delete-d3dx9`, lotes E2/F) | 32 |
| Constructores/destructores de tarea con ABI propia (`fastcall` con EDX, tamaño de reserva) o clases sin constructor C++ completo | 18 |
| Audio con dwords exactos (canal sin signo, bits de float) por la ABI del EXE | 3 |

Criterio común: **un puntero a función cuyo valor se guarda o se compara** (`renderCallBack == 0x7491C0`, `lpfnWndProc`, `fileFuncs`, las estructuras de libjpeg) no se sustituye mientras el EXE siga cargado, porque el código del EXE compara contra la dirección del EXE y no contra la de la DLL. Se retira con el EXE (fase 8). Tampoco se tocaron `Crt/*`, `MemoryMgr`, `DatosExe/*` ni los archivos de las otras ramas.

### B-R1 … B-R5 (183 direcciones que no son entrada de función)

Clasificadas, no cerradas: 50 son literales del protector (`ExeProtectionStub.cpp`), 59 de libjpeg y 58 de RenderWare son direcciones interiores o callbacks guardados, 6 son comentarios con la dirección que el escáner no reconoce (la línea lleva un separador de dígitos `10'000` o un literal de carácter), 1 es una constante numérica y 9 quedan para la parte 6. Ninguna es una llamada que haya que sustituir.

## 2b. P2b: segunda pasada sobre los puentes por dirección que quedaban

Rama `vivo-p2b` (desde `origin/main` 410d9f2d84, 21/11/2026). No cambia ninguna dirección de enganche: en cada caso la llamada por dirección se sustituye por la llamada a la función C++ que ya estaba enganchada en esa dirección (o por su puntero, cuando la dirección se guardaba en una estructura). `contrib/codigo_vivo_cierre_p2.py` lee ahora `motivos-p2b.csv`, que da el motivo concreto por dirección de lo que no se tocó.

| | Antes de P2b | Después de P2b |
| --- | ---: | ---: |
| Entradas de función B1–B18 sin llamada por dirección (sustituidas) | 92 | **300** |
| …pendientes o parciales | 277 | 69 (32 del CRT y arranque, que llevan otras ramas) |
| …que no eran puentes (tabla de `Cheat.cpp`) | 65 | 65 |
| Referencias que no son enganche (B1–B18) | 373 | **116** |
| *Tras fusionar `origin/main` a23f30550d (vivo-p3, que retira otras 19 por su lado, entre ellas 0x4CCE60 con `RasterCreateCpp` y los puentes de QuickTime):* sustituidas / pendientes o parciales / referencias | | 319 / 50 / 102 |

Todas las cifras de esta sección son de la rama sola, salvo la fila marcada. De las 316 entradas de función pendientes al empezar (los 271 puentes de B1–B18 más las entradas «vivas» del EXE que el C++ cita) se sustituyeron **209**; de las 256 que no eran del CRT ni del protector quedan 47, que se explican una a una abajo.

| Grupo (motivo de P2 original) | Direcciones | Sustituidas | Quedan |
| --- | ---: | ---: | ---: |
| Constructores/destructores de tarea con ABI propia | 18 | 18 | 0 |
| Plantillas del vector de la STL en `PedAttractor` y `AEUserRadioTrackManager` | 55 | 55 | 0 |
| RenderWare / D3D9 (punteros guardados o funciones internas) | 88 | 59 (60 con `RasterCreateCpp` de vivo-p3) | 29 (28 de libpng; tras vivo-p3 solo 2) |
| ABI propia del EXE | 46 | 31 | 15 |
| libjpeg | 45 | 43 | 2 (heap del CRT) |
| Audio con dwords exactos | 4 | 3 | 1 (sin C++) |
| CRT y arranque / protector | 60 | 0 | 60 (no son de esta parte) |

### Qué se sustituyó

* **Tareas (18).** `CTaskSimpleFight` (constructor y destructor llaman a `Constructor()`/`Destructor()`), `CTaskComplexReactToAttack` (constructor 0x620B00 en `Clone`; `CVector2D::Normalise` 0x44E480 expuesta desde `Vector.cpp`), `CTaskComplexWaitForPed` (0x61A3D0), `CTaskComplexUseMobilePhone` (0x6348E0, 0x6348F0), `CTaskComplexAvoidEntity` (0x66AA20, 0x66AAE0, 0x66FD30), `CTaskComplexFollowPatrolRoute` (0x6709F0, 0x674930), `CTaskInteriorSitInChair` (0x675C30; el primer argumento del EXE es la duración; ya no se llama al constructor sin memoria) y `Population.cpp`: la plantilla `PopulationTask` reserva con `CTask::operator new` y construye con el constructor C++ de `CTaskComplexBeInCouple` (0x6836F0), `CTaskSimpleHoldEntity` (0x6913A0) y `CTaskComplexSunbathe` (0x631F80), y llama a `CanSunbathe` (0x632140). Corrección de un fallo latente: `PopulationSunbathe` llamaba a `SetHeading` (0x43E0C0) con la dirección de la **matriz** en lugar del peatón o la toalla; ahora usa `ped->SetHeading` y `towel->SetHeading`, como el EXE (0x6155C1, 0x615601).
* **Vector de la STL (55).** Los 47 ayudantes de `PedAttractor.cpp` (copias, rellenos, `uninitialized_copy`, `copy_backward`, `_Insert_n`, `insert`, `push_back`, `size`) se llaman entre sí por su nombre, sin direcciones; `vector::_Xlen` lo lanza `ExeVectorXlen` (cuerpo en `AEUserRadioTrackManager.cpp`). `UserTracksLegacyVector::Insert` (0x4F4320) reutiliza `SArrayInsertN4` (mismo cuerpo de plantilla) y los 8 ayudantes 0x4F39xx–0x4F3Dxx dejan de llamarse por dirección. `CPedAttractorManager` empuja con `PedAttractorListPushBack` (0x5EE400).
* **Con ABI propia (31).** `CTheScripts::MoveSearchLight*` (0x493360, 0x493420, 0x493480; las tres definiciones nuevas reenvían a las funciones con los mismos bits), `SetEngineOn` (0x41BDD0, en línea: con el motor roto lo apaga), `IsInvisibleOrRenderDamaged` (0x46A2F0), `DecisionMakerTypes` (0x4650F0), Fx (0x4A9F20 y 0x4A9F40 por `FxPrimConstructExe`/`FxPrimDestructExe`, 0x4A9D20 por `FxPrimBPDestroyBody`, 0x4A76C0), los 9 constructores de almacén de `ModelInfo.cpp` (0x4C5540…0x4C75E0, por la función `…ConstructExe` de cada clase), `CEventSource` (0x601C30, 0x601BC0, 0x5DF8F0: llamadas directas a `IsThreatenedBy`, `IsFriendlyWith` e `IsPlayer`), `CCutsceneObject`/`CHandObject` (0x5A1D10), `QuadBike` (0x6D0B30, destructor vacío), `X87Math` (0x7EE1D0 y 0x7EDB30 por `RwCameraSetNearClipPlaneExport`/`RwSqrtExport`), `CustomBuilding*` y `CustomCarEnvMap` (0x7F9F70, 0x7FA1C0, 0x7FE0A0) y los tres lanzadores `std::string`/`runtime_error` de `CrtException.cpp` (0x4F4170, 0x4F4090, 0x4F3BA0, por las funciones `ExeStd*` de `AEUserRadioTrackManager.cpp`).
* **RenderWare/D3D9 (59).** `rwcore.cpp`: destructor de la cabecera de instancia (0x74E9E0, 0x730D30, 0x7FAC10, 0x7F56A0, 0x7F5AE0), `RwIm3DSetRenderPipeline` (0x7EFB10 ×5), lista de rasters (0x4CB640, 0x4CC150, 0x4CC170 por `D3D9RasterList…Exe`), tabla de funciones estándar del dispositivo (ahora con las direcciones C++ de 0x7F8F20, 0x7F7730, 0x7FEE20, 0x7FF070, 0x8001E0, 0x7FF270, 0x4CBB00, 0x4C9F90, 0x4CA290, 0x4CA540 y 0x4CD360), `_rwerror` (0x8088D0), tablas de multiplicación de vectores SSE/SSE2 (0x80D070…0x80D600); `rwplcore.cpp` (0x7ED730, 0x7ED880, 0x80A240, 0x80A350, 0x7F5F60, 0x801D50); `rpworld.cpp` (callbacks de derechos 0x749120, 0x749160, 0x74D7D0, 0x74D810; atadores 0x74FFA0, 0x74FFF0; 0x760CF0; 0x74EEC0, que el EXE solo llama y no compara; 0x4C9990 como `destroyNotify`); `rpmatfx.cpp` (0x4C9EA0, 0x4C9F70, 0x4CD250, 0x7FC200, 0x7FE190, 0x7F5B10), `rpskin.cpp` (0x7FAA30), `rpanisot.cpp` (0x7FDE60), `D3D9RasterDriver.cpp` (0x7FDCD0, 0x7FDE50), `D3D9RasterFormat.cpp` (0x7FF270, 0x8001E0) y `D3DResourceSystem.cpp` (0x7304B0).
* **libjpeg (43).** Las llamadas entre archivos del módulo van por `LibJpegExports.h` (nuevo: funciones `LibJpegExport…` y `LibJpegFnAddress`) o por los miembros de `LibJpegCompress`; los 23 punteros a función guardados en las estructuras de libjpeg (`marker`, `master`, `inputctl`, `mem`, `err`, `up`, `cconvert`) llevan ahora la dirección de la función C++. Es viable porque esos valores solo aparecen en el EXE dentro de las rutinas de inicialización (0x5C35xx…0x5CF7xx), que ya están reemplazadas y no se ejecutan, y el EXE no los compara; las 27 funciones de libjpeg que el EXE conserva sin C++ están todas descartadas o condicionadas. Se mantiene el mecanismo `LIBJPEG_CB` (opción `GTASA_WITH_LIBJPEG`).
* **Audio (3).** `StartTrackPlayback` y `CheckForPause` (ambientación) y `StartTrackPlayback` y la pausa (cutscene) llaman a `SetChannelVolume`, `SetChannelFlags` y `SetChannelFrequencyScalingFactor` con el canal como `int16`: las tres funciones del EXE leen solo la palabra del canal (`mov ax, word [esp+4]`, `movsx`), así que extender a cero o con signo es indiferente; el volumen y el factor viajan con los mismos bits.

### Qué NO se sustituyó, y por qué ([motivos-p2b.csv](motivos-p2b.csv))

| Direcciones | Motivo concreto |
| --- | --- |
| 0x407180, 0x441390, 0x69F2B0, 0x6A0050 | las tres líneas de `CarCtrl.cpp`/`Cam.cpp` que también cambia la rama `datos-exe` (`StaticRef`): `origin/main` todavía no la contiene; se hacen al fusionar `datos-exe` para no provocar conflicto |
| 0x705620, 0x7491C0 | `ShadowCamera.cpp` pasa/guarda esas direcciones del EXE y el EXE compara `renderCallBack` con 0x7491C0 (0x705626…0x705655, 0x705CD2, 0x705CF5, 0x4C919D, 0x7328A9, 0x7328BB, 0x749CAD) |
| 0x732B40 | `Renderer.cpp` compara con la dirección de `RenderEntity` que el EXE guarda en las listas (0x5539A6, 0x733DB1, 0x733E71, 0x73458E) |
| 0x747EB0 | `WinMain.cpp`: solo con `NOTSA_USE_SDL3` se registra como `lpfnWndProc` la dirección del EXE (sin SDL3 ya se usa `__MainWndProc`) |
| 0x72F420, 0x72F430, 0x72F440, 0x72F4C0, 0x72F4F0 | envolturas del heap del CRT (`MemoryMgr.cpp`, `free` de libjpeg): dependen del CRT propio y las tocan las ramas del CRT |
| 0x53E920, 0x541DD0 | no son puentes: `AutoSteal.cpp` parchea 5 bytes de `CGame::Idle` e `InjectHooksMain.cpp` instala el enganche de `CPad::UpdatePads`; la dirección es el destino del enganche |
| 0x4CCE60 | en la rama sola no tenía versión C++ (crear raster D3D9); `vivo-p3` la aporta (`RasterCreateCpp`) y tras la fusión `D3D9RasterFormat.cpp` y la tabla estándar de `rwcore.cpp` la llaman por nombre |
| 0x818650, 0x818670, 0x81A3C0 | en la rama sola: puentes de QuickTime y decodificador de medios del EXE sin C++ (lote V6/E); `vivo-p3` (QuickTime nulo) los retira al fusionar |
| 28 direcciones 0x7CF600…0x7DA870 (tras vivo-p3 quedan 0x7CF600 y 0x7CF9B0) | tabla de funciones de libpng de `rtpng.cpp`; decisión vigente: libpng + zlib se enlazan; el puntero desaparece al enlazar (rama `enl-png`) |

## 3. Comprobaciones tras la parte

* Compilación completa de la DLL (`reversing/CVehicle/build-project.ps1` con el candado `compile_lock`): sin errores ni advertencias nuevas.
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 archivos con ensamblador o `naked`.
* Humo con la DLL propia (`AGUA_ASI`, `cal_mal.args`: todos los enganches activos): `prueba_ropero.py` → **BIEN** (puerta 2877 y marcador presentes); `prueba_agua.py` → **BIEN** (brillo medio 48,6, bajo el agua).
* Sin banco por función dentro del juego (decisión del usuario): la equivalencia de cada sustitución es la del enganche que ya redirigía esa dirección a la misma función C++.

Tras P2b (rama `vivo-p2b`): compilación completa de la DLL con el candado `compile_lock` (`_build_VIVOP2B.sh`), `EXIT=0` y sin advertencias nuevas; `audit_hook_catalog.py --strict` OK y 0 archivos con ensamblador; humo con la DLL de la rama (`cal_mal.args`): `prueba_ropero.py` **BIEN** (puerta y marcador presentes) y `prueba_agua.py` **BIEN** (brillo medio 48,5, bajo el agua). Después de fusionar `origin/main` a23f30550d (vivo-p3; un conflicto en `D3D9RasterFormat.cpp` resuelto a favor de `RasterCreateCpp` más `_rwD3D9RasterSetFromImage`, y la tabla estándar de `rwcore.cpp` pasa a `RasterCreateCpp`) se volvió a compilar (`EXIT=0`) y los dos humos dieron **BIEN** (ropero: puerta y marcador; agua: brillo medio 48,0). Sin banco por función (decisión del usuario). `contrib/sync_docs.py` no se pudo ejecutar en este árbol (la validación de `hooks.csv` exige antes `sync_validated_flags.py`, ajeno a esta parte); como no se añade ni se mueve ningún enganche, `hooks.csv` no cambia.

## 4. Reproducir

```
python -X utf8 contrib/codigo_vivo_cierre.py --lotes V1,V2-1,V2-2,V3-1,V3-2,V3-3,V3-4,V3-5,V3-6 --aplicar
python -X utf8 contrib/codigo_vivo_cierre.py --lotes V1,...,V8-1 --escribir-lotes V4-1,...,V8-1 --sin-garantia V4-1,...,V8-1 --aplicar
python -X utf8 contrib/codigo_vivo_cierre_p2.py --aplicar
python -X utf8 contrib/codigo_vivo_plan.py        # vuelca cierre.csv en plan.csv y plan-lotes.md
```

P2b usa además `docs/CodigoVivoExe/motivos-p2b.csv` (motivo concreto por dirección que no se sustituyó), que `codigo_vivo_cierre_p2.py` lee antes de caer en el motivo genérico.
