# SilentPatch, segunda tanda: entrada, cámara, replay, guardado, streaming y heap (rama `silentpatch2-entrada`)

Base: `integracion-fase7b` (09d31673c6). Worktree `D:\Original\wv-spe`. Origen: SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz "Silent"; ver [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Los arreglos se aplican **cambiando nuestras funciones reconstruidas** (no se copian parches de bytes); cada sitio lleva `// SilentPatch (MIT, Silent): <arreglo>` y, con el interruptor apagado (`GTASA_SILENTPATCH=0`, o `GTASA_SILENTPATCH_<NOMBRE>=0` por arreglo), el código ejecutado es el del EXE.

* Clasificación viñeta a viñeta: [silentpatch-clasificacion/entrada.csv](silentpatch-clasificacion/entrada.csv) (columnas: L, viñeta, clase, función de SilentPatch, función nuestra, opción, estado).
* Tabla de desviaciones: sección 15.E de [DesviacionesConocidas.md](../DesviacionesConocidas.md).
* Ninguna opción tiene clave propia en `SilentPatchSA.ini` (en SilentPatch estos arreglos son fijos), así que todas van **activadas por defecto**.

## Solapes con otras ramas (retirados de esta)

Cinco arreglos que esta rama implementó y probó los implementan también otras ramas de la tanda; se retiraron en el segundo commit de esta rama (el primero, `56e52caebf`, los conserva) para no duplicarlos al fusionar:

| Viñeta | Opción propia (retirada) | Otra rama |
| --- | --- | --- |
| L29 temporizadores | `NewTimersFix` | `silentpatch2-graficos`: `TimersFix` (Timer.cpp, TheScripts.cpp; mismo código) |
| L200 pantallas de carga en bucle | `LoopLoadingScreens` | `silentpatch2-graficos`: misma opción y mismo arreglo (LoadingScreen.cpp) |
| L76 streaming con RC | `RCVehicleStreamingFix` | `silentpatch2-vehiculos`: `RCVehicleStreaming`; también la rama de peds (`RCStreaming`, sin confirmar su estado final) (Renderer.cpp) |
| L104 jetpack con teclado | `JetpackKeyboardHover` | rama de peds: `JetpackKeyboardControlsHover` (TaskSimpleJetPack.cpp) |
| L120 clones de CJ en controles de carretera | `RoadblockCJClonesFix` | rama de peds: `RoadblockNoCJClones` (RoadBlocks.cpp; mismo resultado, el hallazgo de que la reconstrucción previa ya no generaba clones aplica igual) |

Al contrario, **L23** (objetos sujetos por pandilleros, `DelayedHeldObjectDestruction`) sigue en esta rama porque el encargo lo asigna aquí, pero la rama de peds lo tiene también (`DelayHeldEntityDestroy`, mismo hunk): hay que quedarse con uno.

## Qué se portó (todo en C++ puro, sin `__asm`)

| Viñeta (L) | Opción | Dónde | Cómo se localizó |
| --- | --- | --- | --- |
| Deadlock de streaming (21) | `CdStreamSyncFix` | `CdStreamInfo.cpp` (`CdStreamSync`, `CdStreamThread`) | SilentPatch `CdStreamSync::Sema` (SilentPatchSA.cpp:1700-1740): sección crítica compartida. Es la misma idea que el commit `6c62d5e8f3` de la rama `carga-guardar-2` (que no estaba en la base): aquí con opción, y probada (ver abajo) |
| Fuga de memoria al hacer fotos (22) | `CameraMemoryLeakFix` | `app/platform/win/WinPs.cpp:psGrabScreen` | SilentPatch parchea las dos salidas de `psGrabScreen` (0x7453CE, 0x7453D6) con `UnlockRect` + `Release`; nuestra función termina en un único `return` |
| Replay: bandas con objetos (23) | `DelayedHeldObjectDestruction` | `Entity/Ped/Ped.cpp:DropEntityThatThisPedIsHolding` | `Patch(0x5E03D4, or [esi+1Ch], 800h)`: `CPed::DropEntityThatThisPedIsHolding+0x74`; el objeto sujeto se marca `bRemoveFromWorld` y lo borra `CWorld::Process` |
| Replay: ropa distinta (24) | `ReplayPlayerPedCrashFixes` | `Replay.cpp:FinishPlayback` | `InterceptCall(0x45F060)`: tras `RestoreStuffFromMem` se llama `CClothes::RebuildPlayer(FindPlayerPed(), false)` |
| Replay: cuerpo gordo/musculoso (25) | `ReplayPlayerPedCrashFixes` | `Replay.cpp:DealWithNewPedPacket` | `InterceptCall(0x45CF87)`: tras `RebuildPlayer(ped, true)` se piden los bloques `fat` y `muscular` (`RESOURCE_ID_IFP` + índice, flags 18) y `LoadAllRequestedModels(true)` |
| Cursor confinado a la ventana (27) | `ClipCursorToGameWindow` | `WndProc.cpp` (mensajes de activación/ventana y `SilentPatchCursorClipPerFrame`), `app_game.cpp:Idle` | SilentPatch `ClipCursorToGameWindow` (SilentPatchSA.cpp:4557-4662): `ClipCursor` al rectángulo cliente en juego, libre en el menú y sin foco; anula el `RsMouseSetPos` de centrado (0x53E9F1) |
| Corrupción de heap (30) | `HeapCorruptionFix` | `Fx/FxManager.cpp:LoadFxProject` | `Nop(0x5C25D3, 5)`: la llamada `FxMemoryPool_c::Optimise` (0x4A9CD0) al final de `LoadFxProject` (+0x1B3). Esa función hace `realloc` de la arena del pool al tamaño usado mientras los `FxSystemBP_c` ya apuntan dentro de ella; el resultado depende del gestor de memoria |
| Raton bloqueado al salir del menú (11) | `MouseReinitFix` | `Frontend/MenuManager_Input.cpp:CheckForMenuClosing` (3 sitios), `WinInput.cpp:diMouseInit` | SilentPatchSA.cpp:7122-7128: se salta `DIReleaseMouse` + `InitialiseMouse` (0x576CCC, 0x576EBA, 0x576F8A) y el dispositivo DirectInput es siempre no exclusivo (`al = 0` en 0x7469A0) |
| Latencia del teclado (79) | `KeyboardInputLatencyFix` | `Pad.cpp:CPad::UpdatePads` | `KeyboardInputFix`: Old = New, New = Temp se mueve del final de `UpdatePads` (0x541E1C) a justo antes de `ClearSimButtonPressCheckers` (0x541DE6) |
| Eje vertical del ratón en fundidos (46) | `MouseYAxisFadeInFix` | `Cam.cpp:Process_FollowPedWithMouse` (2 sitios), `Process_FollowPed_SA` | Parches 0x50FBB4, 0x510512 y 0x524071: se salta la condición de fundido que forzaba el ángulo vertical (y el rumbo del jugador) |
| Cámara de coche con el ratón (86) | `InCarCameraMouseLookFix` | `Cam.cpp:Process_FollowCar_SA` | 0x525615: la lectura de `m_bUseMouse3rdPerson` pasa a "ratón y `camLookDirection == 3` (mirando adelante)" |
| Checkpoints de carrera (97) | `RacingCheckpointsColourFix` | `3dMarkers.cpp:tDirectionArrow::Render` | 0x721520: se activa `rpGEOMETRYMODULATEMATERIALCOLOR` antes de dibujar la flecha |
| `FILE_FLAG_NO_BUFFERING` (71) | `RemoveNoBufferingFlag` | `CdStreamInfo.cpp:CdStreamInit` | 0x406BC6 |
| IMG de más de 4 GB (219) | `IMGsBiggerThan4GB` | `CdStreamInfo.cpp:CdStreamThread`/`CdStreamRead` | `CdStreamThreadHighSize`/`CdStreamSetFilePointer` (0x4065C2, 0x406620): desplazamiento de 64 bits (`OffsetHigh`, `SetFilePointer` con parte alta) |

## Prueba del deadlock de streaming (viñeta 21)

El bloqueo exige que el hilo de lectura compruebe `bLocked` (tras poner `nSectorsToRead = 0`) justo antes de que el principal ponga `bLocked = true`. En el juego aparece de forma esporádica (la nota de `docs/CodigoVivoExe/ejecucion-cargar-partidas.md` registra un único cuelgue en dos cargas de unos 100 s). No es práctico encadenar 50 cargas de partida de 100 s con y sin el arreglo y, aun así, un fallo esporádico no daría una comparación estadística, así que se hizo una **prueba dirigida en proceso** que ejecuta los mismos protocolos de `CdStreamRead`/`CdStreamSync`/`CdStreamThread` (misma disposición de campos y semáforos de cuenta máxima 2 y 5) sobre una lectura simulada:

* Programa: `reversing/FnFase8/cdstream_carrera/prueba_carrera.cpp` (`orig` = sin sección crítica como el EXE; `fix` = con ella). Se da por bloqueo reproducido si el hilo principal no avanza en 2 s.
* Resultados (`silentpatch-clasificacion/entrada-cdstream-carrera.txt`), 25 ejecuciones de 2.000.000 de lecturas+sincronizaciones por modo y por "tiempo de lectura" (giro 0 y 50 iteraciones vacías):

| Modo | Giro | Bloqueos | Completas |
| --- | --- | --- | --- |
| orig | 0 | 24 de 25 | 1 |
| fix | 0 | 0 de 25 | 25 |
| orig | 50 | 25 de 25 | 0 |
| fix | 50 | 0 de 25 | 25 |

El bloqueo del modo `orig` se detiene siempre con `sectorsToRead = 0` y `bLocked = 1`, es decir, el hilo principal espera un semáforo que nadie va a liberar (la carrera descrita). Esto **demuestra que la carrera existe y que la sección crítica la elimina en el protocolo**; **no demuestra** que sea la causa del cuelgue de ~120 s que se vio al cargar partidas (otros bloqueos del juego darían el mismo síntoma). En el juego real el arreglo se ejercitó recorriendo interiores y cargas (ver resultados del juego).

## Resultados en el juego (standalone `reversing/FnFase8/salida/spe/gta_sa_standalone.exe`, 25/11/2026)

Lanzado con `reversing/FnJuego/ejecutar_juego.ps1 -Exe <ese exe>` (candado `gta_lock`, carpeta `D:\Original\_juego_run`, partida 2 copiada) y el escenario nuevo `entrada` de `reversing/FnJuego/sonda_juego.py` (ratón y teclado reales con `SendInput`, memoria del proceso; capturas). Evidencias en `reversing/FnJuego/ejecuciones/spe_*` (sonda_juego.log, log.log, fase8_arranque.log).

| Ejecución | SilentPatch | Resultado |
| --- | --- | --- |
| `spe_on`, `spe_on2` | encendido (por defecto; binario con los cinco solapes todavía incluidos) | vivo todo el escenario; replay con F1 x3 y 12 pulsaciones seguidas sin caída; el log registra `HeapCorruptionFix: Optimise omitido`, `ReplayPlayerPedCrashFixes: cargando bloques de animacion fat y muscular` y `RebuildPlayer con la ropa actual al terminar la reproduccion` en cada replay; `excepciones=0` en `spe_on2` (130 s) y `excepciones=1` en `spe_on` (ver nota) |
| `spe_off`, `spe_off2` | `GTASA_SILENTPATCH=0` | vivo; el log dice `interruptor general = 0` y no lista opciones; `excepciones=1` (ver nota) |
| `spe_int` | encendido | `interiores` x3: 15 entradas y 15 salidas de interior (casa de CJ, gimnasio, tienda de ropa, Ammu-Nation, peluquería) en 440 s, todas con `entro` y `salio` verdaderos, sin NaN, sin bloqueo de streaming; 220 latidos con `excepciones=0` |

Medidas con la sonda (n = 60 pulsaciones de W por ejecución; tiempo desde `SendInput` hasta que el estado del mando 0 cambia):

| Medida | Encendido | Apagado |
| --- | --- | --- |
| Latencia del teclado (`KeyboardInputLatencyFix`), media / mediana / p90 | 17,4 / 18,3 / 30,5 ms (`spe_on2`); 15,7 / 14,7 / 30,3 ms (`spe_on`) | 68,3 / 67,5 / 86,5 ms (`spe_off2`); 64,6 / 62,3 / 84,6 ms (`spe_off`) |
| `GetClipCursor` (`ClipCursorToGameWindow`) | (512,227)-(1151,706) = el rectángulo cliente (640x480) | (0,0)-(1536,864) = toda la pantalla |
| Cursor tras empujar con ratón hacia el borde | queda en (1150,706) dentro de la ventana | llega a (832,467) / (1535,863) (fuera de la ventana) |
| Ratón (8 movimientos de 30,12 píxeles) | `New.mov` = (30,-12) por fotograma, la cámara gira 75,2 grados | igual (30,-12) y 75,2 grados: el ratón sigue funcionando |

La latencia baja unos 48-50 ms (algo más de un fotograma de 33 ms: el fotograma de menos más la espera del siguiente `UpdatePads`).

Nota sobre `excepciones=1`: en tres de las cinco ejecuciones el registro de latidos cuenta una única excepción, anotada al arrancar (`app_debug.cpp: UNHANDLED EXCEPTION 0xc0000005` con la pila `igd12um32xel` -> `d3d9on12` -> `Direct3DCreate9Ex` -> `GetVideoMemInfo` del independiente): es una violación de acceso interna del controlador gráfico Intel al enumerar el adaptador D3D9-sobre-D3D12, que el controlador captura él mismo. Aparece con SilentPatch apagado igual que encendido (`spe_off`, `spe_off2`) y no con ninguna función de esta rama (ocurre antes de cargar nada del juego); en `spe_on2` y `spe_int` no se produjo.

## Humo de la ASI de la rama

(ver abajo)

## Qué NO se verificó

* `CameraMemoryLeakFix`, `DelayedHeldObjectDestruction`, `InCarCameraMouseLookFix`, `MouseYAxisFadeInFix`, `RacingCheckpointsColourFix`, `IMGsBiggerThan4GB` (la rama de más de 4 GB) y `MouseReinitFix` (el reinicio solo existe en pantalla completa exclusiva): implementados y compilados, no ejercitados (requieren una foto con la cámara del juego, pandilleros con objetos, mirar atrás con el ratón en coche, un fundido de entrada con el ratón, una carrera, una imagen mayor de 4 GB o pantalla completa).
* Los fallos de replay que describe el changelog (ropa o cuerpo distintos) no se reprodujeron ni con ni sin el arreglo; solo se comprobó que los caminos nuevos se recorren sin caída.
* `HeapCorruptionFix`: se comprueba que `Optimise` se omite; no se observó la corrupción original (depende del gestor de memoria).
* El deadlock de streaming: la prueba aislada demuestra la carrera y su cura en el protocolo; no se probaron 50 cargas de partida de ~100 s con y sin el arreglo (≈ 3 horas) ni se demuestra que sea la causa del cuelgue de ~120 s observado en `ejecucion-cargar-partidas.md`.
* Temporizadores (L29): la prueba aritmética `silentpatch-clasificacion/entrada-temporizadores.txt` (a 2500 FPS el EXE avanza 0 ms en 10 s y con resto fraccionario 10000 ms; a 60 FPS el EXE atrasa un 4 %) se hizo con la fórmula, no con el juego; el arreglo en sí lo lleva la rama de gráficos.
* La clasificación recoge L10 ("More precise frame limiter") como no tomada: no está en mi lista y no la recoge mi búsqueda; sigue pendiente de asignar.
