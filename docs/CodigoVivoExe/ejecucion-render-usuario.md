<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fallos de dibujo vistos por el usuario en el standalone (22/11/2026, rama `render-usuario`)

Base: `integracion-fase7b` 745cee8725. Herramientas: `reversing/FnFase8/render_correr.ps1` (lanza el standalone en `D:\Original\_ren_run`, con candado `gta_lock`), `render_shots.py` / `render_shots_walk.py` (capturas del cliente completo, proceso con DPI consciente) y el criterio de `reversing/FnBIS1/criterio_visual.py`. Registros y capturas en `D:\Original\sim_out\rn_*` (no se versionan).

## Resumen

| # | Síntoma del usuario | Causa | Estado |
| --- | --- | --- | --- |
| 2 | No se ven los marcadores 3D (tampoco con el ASI) | `D3D9AtomicAllInOneNodeInit` (0x757890) no fijaba el callback de render del nodo | **Arreglado y verificado con capturas** |
| 4 | Cinemática del aeropuerto: sin suelo ni edificios (blanco) | La misma: el entorno del aeropuerto usa la tubería de átomos por defecto (`atomic->pipeline == NULL`) | **Arreglado y verificado con capturas** |
| 1 | Sin tráfico | Los coches se dibujan (tubería env-map); en control libre el pool solo tiene ~5 vehículos: parece de generación de tráfico, no de dibujo | **Sin resolver** (sec. 2) |
| 3 | Sin nubes | Texturas y llamadas correctas; las volumétricas salen por el fader a z < 220 como el original y las bajas valen 0 con ExtraSunnyness = 1 | **Sin resolver**, falta control con el ASI (sec. 3) |

## 1. Causa común de (2) y (4): nodo AllInOne por defecto sin callback de render

El nodo `D3D9AtomicAllInOne` (definición estática 0x8D6338) guarda en sus datos privados cuatro punteros: instanciado (+0), reinstanciado (+4), iluminación (+8) y render (+0xC). Su inicializador, 0x757890, son 40 bytes:

```
8B 44 24 04 | 8B 40 14 | C7 00 C0 78 75 00 | C7 40 04 70 82 75 00 | C7 40 08 00 74 75 00 | C7 40 0C F0 6D 75 00 | B8 01 00 00 00 | C3
mov eax,[esp+4]; mov eax,[eax+0x14]; [eax]=0x7578C0; [eax+4]=0x758270; [eax+8]=0x757400; [eax+0xC]=0x756DF0; mov eax,1; ret
```

La reconstrucción (`rpworld_p6b.cpp`, lote P6B, «sin validar») rellenaba solo tres y decía en el comentario «no toca el callback de render (+0xC)»; además era `void` (el original devuelve TRUE). Igual el de sectores 0x75E540 (`mov [eax+0xC],0x756DF0`). Con el render a NULL, `D3D9AtomicAllInOneNode` solo instancia y no dibuja: **toda atómica con `pipeline == NULL`** (tubería por defecto del motor: `AtomicDefaultRenderCallBack` 0x7491C0 → `RxPipelineExecute`) quedaba invisible. Las tuberías propias (edificios DN, coches env-map, MatFX, skin) fijan su propio render después del init, por eso peatones, edificios exteriores y coches se veían y solo faltaban los objetos por defecto: los marcadores 3D (clon del átomo `cylinder`/`diamond_3`, `pipe=0x0` medido en el log) y el entorno del aeropuerto (`CJ_AIRP_S_*`, `ws_stationfloor`, `CJ_AIR_1`, átomo de 2.090 vértices y 10 materiales con `pipe=0x0`). Con el ASI pasaba lo mismo (los ganchos son los mismos), de ahí que el usuario tampoco viera los marcadores del ropero con el ASI.

Arreglo: fijar `data->render = D3D9DefaultRenderCallBack` (0x756DF0) y devolver `RwBool` TRUE en ambos inicializadores (`rpworld_p6b.cpp`, `rpworld.h`). Comprobado byte a byte con `rea read_bytes` 0x757890 y 0x75E540.

Verificación (capturas del cliente completo 800x600, standalone `render-usuario`):
* Marcador: `rn_c3_s01.png` y `rn_c4_s23.png` (control libre en el callejón de Grove): cono azul translúcido con degradado visible junto a la pared derecha. Antes (`rn_a1_12.png`, mismo sitio, mismo momento) no había nada.
* Aeropuerto: `rn_c5_s04.png`: techo con placas, pilar, panel «Arrivals/Llegadas», cinta de equipaje, maleta y CJ. Antes (`rn_a1_01.png`): fondo blanco liso con CJ y la maleta.

Capturas guardadas en `docs/CodigoVivoExe/render-usuario/` (`rn_a1_*` son anteriores al arreglo y salieron a 585x438 por la virtualización de DPI del script antiguo; `rn_c3_s01`, `rn_c4_s23`, `rn_c5_s04` son del cliente completo 800x600 con el arreglo).

## 2. Coches («no hay tráfico»): sin causa de dibujo hallada

Medido con el standalone (diagnóstico temporal, retirado):
* Los coches **sí se dibujan**: en la cinemática se ven el taxi, el coche de policía y el tren (`rn_c2_s05.png`, `rn_c4_s02.png`); el callback `RenderVehicleHiDetailCB` se invoca ~30 veces por fotograma y `CCustomCarEnvMapPipeline::CustomPipeRenderCB` ~600 veces/s con mallas y primitivas válidas (vertex/index buffer y declaración no nulos, ~800.000 primitivas cada 3 s). `ms_vehicleLod0Dist` = 9.800 (`TheCamera.m_fGenerationDistMultiplier` = 1), así que no hay descarte por distancia.
* Los coches del pool tienen `m_pRwObject`, bandera visible, `CColModel` (info de modelo con clump y col) y estado de streaming DFF/TXD = cargado.
* En control libre el pool tiene **5 vehículos** (BMX y poco más) y ninguno en pantalla: el caso es **de generación de tráfico** (`CCarCtrl`/`CarGenerator`), no de dibujo. El usuario tampoco choca con nada porque no hay coches cerca. Sin el ASI de control no se pudo comparar cuántos coches genera el original en el mismo punto y hora; la pista siguiente es medir `CCarCtrl::NumRandomCars/NumParkedCars/NumMissionCars` y las llamadas a `GenerateOneRandomCar` (y que `CEntryExit`/`CTheCarGenerators` no abortan por el fallo de pool de `CEntryExit` que sí se registra una vez).
* Efecto colateral del arreglo de la sec. 1: cualquier coche cuyo átomo use la tubería por defecto (`pipeline == NULL`) tampoco se dibujaba; ya sí.

## 3. Nubes: texturas bien, sin fallo de dibujo evidente

* `gpCloudTex`, `gpCloudMaskTex`, `gpMoonMask` y `ms_vc.texture` son texturas válidas con raster (se leen de la ranura «particle», 34 texturas con `cloud1`, `cloudhigh`, `cloudmasked`); `CClouds::Render` se ejecuta cada fotograma.
* Nubes bajas: 5-6 de 12 proyectadas por fotograma; color = color del timecycle × (1 − max(ExtraSunnyness, niebla, cobertura)): al amanecer (110,37,32) naranja (se ven como estelas finas en el horizonte); con `ExtraSunnyness = 1` el color sale (0,0,0) y no se ve nada (comportamiento del código validado).
* Nubes volumétricas (`VolumetricCloudsRender`, `cloudhigh`): solo hay 5 de 180 (`alphaSuma` 393) y desde el primer fotograma de exterior el `gfVolumetricCloudFader` sube hasta 255 por estar el jugador por debajo de z = 220 con `m_bVolumetricCloudHeightSwitch = 1` (valor del EXE en `0x8D538C`), con lo que el código sale antes de dibujar: es el comportamiento del original, no un fallo de dibujo. Para decidir si las nubes del ASI son otras (por ejemplo las de `MovingFog`/niebla o un estado de clima distinto en la partida) hace falta la captura de control con el ASI en el mismo punto y hora; sin ella no se toca código.

## 4. Fallo de pool de `CEntryExit` (`Pool.h:207`)

Una vez por partida (a los ~2 min). No se vincula a los cuatro síntomas: los marcadores y el aeropuerto se explican por la sec. 1 y el streaming de todos los modelos y TXD inspeccionados es correcto. El propio `Pool.h` documenta que el pool lleno es normal en el original.

## 5. Criterio visual de las pruebas

Las pruebas de humo (`prueba_marcadores.py`, `prueba_ropero.py`) daban BIEN solo por memoria. `reversing/FnBIS1/criterio_visual.py` aporta captura del cliente completo (DPI consciente; valida que el tamaño de imagen sea el del rectángulo cliente y al menos 640x480), conteo de píxeles del color del objeto esperado y `criterio_objeto`. Esos dos scripts viven sin versionar en `wv-8`; hay que importar `criterio_visual` allí y exigir `criterio_objeto(captura, color_del_marcador)`, además de mirar la imagen entera y compararla con el control (variante B, EXE original) en la misma posición, ángulo y hora.

## 6. Cómo reproducir

```
git worktree add --no-checkout -b render-usuario D:\Original\wv-ren 745cee8725    # sparse: source libs cmake conan contrib docs/CodigoVivoExe reversing/FnFase8 reversing/FnBIS1
bash /d/Original/_build_REN.sh                                                       # build-project.ps1 con candado de compilacion (EXIT=0)
reversing\FnFase8\enlazar.cmd D:\Original\wv-ren D:\Original\wv-ren\reversing\FnFase8\salida D:\Original\wv-ren\build\source\gta_reversed.dir\Debug
pwsh -File reversing/FnFase8/render_correr.ps1 -Etiqueta x -Limite 270 ; python reversing/FnFase8/render_shots.py x 255 8 5
```
(`cl.rsp` hay que reescribirlo con `sed` a las rutas del worktree sin commitearlo.)
