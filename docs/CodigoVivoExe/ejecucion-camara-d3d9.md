<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución CAM: envolturas de RwCamera/RpWorld, funciones estándar de cámara de D3D9 y cola de «viva sin hook» (rama `vivo-cam`)

Rama `vivo-cam` (desde `origin/main` 4a4a7b7328, 21/11/2026). Parte del primer INT3 que dio el experimento de la fase 8 ([fase8-experimento.md](fase8-experimento.md) §3: rupturas 3, 4 y 5) y del hallazgo de [reclasificacion-raices-valor.md](reclasificacion-raices-valor.md) (una dirección que el C++ escribe como valor en una tabla de punteros es una raíz viva). Resultado por rutina en [cierre-cam.csv](cierre-cam.csv); bancos en [reversing/FnCAM](../../reversing/FnCAM/README.md).
Decisión vigente del usuario: sin validar función por función dentro del juego; basta un banco fuera del juego cuando el método ya existe, más el humo.

## Resumen

| Bloque | Rutinas | Estado | Verificación |
| --- | ---: | --- | --- |
| Siete funciones de la fase 8: 0x750430, 0x750460, 0x750480 (envolturas del plugin 0x509 de RwCamera), 0x750AC0 (luz, la que llamaba `FrameSyncObjects`), 0x7F98D0 (CAMERAENDUPDATE), 0x7F99B0 (RASTERSHOWRASTER) y 0x7FFF00 (IMAGEFINDRASTERFORMAT) | 7 | C++ puro y **enganchadas**; las tablas de punteros (`CameraWorldPluginConstruct`, `LightWorldPluginConstruct`, `_rwD3D9DeviceSystemStandards`) guardan ahora las funciones C++ | `bench_cam` (500.000 casos), `bench_d3d` (300.000 casos): 0 diferencias; pruebas negativas 31/31 y 25/25 |
| Cola de «viva sin hook» recalculada (ver abajo): 7507E0 (átomo), 751500/7515A0/751630 (retirada de un clump del mundo), 74F590 (render de sector), 74AE40/74B000/74AFA0/74AFD0/74AEA0/74AF20 (callbacks de `RpClumpStreamWrite`), 74CE60, 762440, 762040 (tamaños de flujo), 75C290 (arista de tira), 4CBEE0, 508EB0 y 856BC0 | 18 | C++ puro y **enganchadas**; las direcciones del EXE que el C++ pasaba como valor o por `CallMethod` se sustituyeron por la función C++ | `bench_cam` (7507E0), `bench_misc` (74CE60, 762440, 762040, 75C290; 400.000 casos, 18/18 mutaciones); el resto, humo |
| Dependencia nueva: 0x751E30 `RpLightStreamWrite` (la llama 0x74AEA0; no estaba en la cola) | 1 | C++ puro y enganchada | humo |
| **Total** | **26** | 26 filas nuevas en `docs/hooks.csv` | |

Además se retiran los tres shims del experimento (`StandaloneExe.cpp`: `ShimCamBegin/End/Sync`, que pisaban estas direcciones con su propio `jmp` justo después de instalar los ganchos): `--shims` queda aceptado y no hace nada.

## Cómo se invocan

* **Plugin 0x509 de RwCamera (RpWorld).** `CameraWorldPluginConstruct` (0x7503E0) guarda en los datos del plugin (camara + `[0xC925A4]`, 0x1C bytes) los tres punteros originales de la cámara (`+0x18` beginUpdate → datos+0x10, `+0x1C` endUpdate → datos+0x14, `+0x10` sincronización → datos+0x18) y los sustituye por las envolturas. Antes escribía las direcciones 0x750430/0x750460/0x750480; ahora escribe `&CameraWorldPluginBeginUpdate`, `&CameraWorldPluginEndUpdate` y `&CameraWorldPluginSync`
  (y cada dirección del EXE queda además enganchada). `RwCameraBeginUpdate`/`RwCameraEndUpdate` (rwcore.cpp) llaman por `camera->beginUpdate/endUpdate`; la sincronización la llama `FrameSyncObjects` por el campo `+0x10` del objeto.
* **Sincronización 0x750480 (650 B).** Primero la sincronización original (matriz de la cámara); con mundo, recorre su BSP (`rootSector`) y llena el arreglo `sectores en el frustum` de los datos de la cámara (la lee `RwCameraForAllSectorsInFrustum`, que usa `RpWorldRender`): una hoja entra si su caja exterior no cae del lado de afuera de ninguno de los 6 planos (por plano se toma la esquina que marcan `closestX/Y/Z`; fuera = el patrón de bits de `n·p − d` como entero con signo `> 0`, x87 a 24 bits). Los nodos de plano comparan la caja del frustum con `leftValue`/`rightValue`
  y, si tocan los dos hijos, el orden de visita lo decide la posición de la cámara contra el plano y `renderOrder` (BACK2FRONT = 2). La reserva crece de 50 en 50 sectores (`rwmalloc` la primera vez, `rwrealloc` después, pista 0x1030507; sin memoria: error 0x80000013 y se corta el recorrido). Al final llama a `stdFunc[rwSTANDARDHINTRENDERF2B](NULL, NULL, orden == FRONT2BACK)`.
* **Luz y átomo (0x750AC0, 0x7507E0).** Mismo recorrido con la esfera (luz: posición de la LTM del marco ± radio, solo subTipo ≥ 0x80 con mundo y marco; átomo: esfera de mundo): desatan sus `RpLightTie`/`RpTie` previos y crean uno por hoja que toca la esfera (lista libre del plugin 0x509, pista 0x30507), enlazado en la lista del sector (`lightsInWorldSector` +0x40 / `collAtomicsInWorldSector` +0x38) y en la del objeto.
* **Funciones estándar de D3D9.** Son entradas de `RwEngineInstance->stdFunc` (índices `rwSTANDARD*`) que rellena `_rwD3D9DeviceSystemStandards` (0x7F72E0): 0x0A CAMERAENDUPDATE, 0x14 RASTERSHOWRASTER, 0x09 IMAGEFINDRASTERFORMAT. La tabla guarda ya las tres funciones C++ (no queda ninguna dirección del EXE en ella) y las direcciones del EXE están además enganchadas.

## El reinicio del dispositivo D3D9 (0x7F99B0)

`_rwD3D9RasterShowRaster(raster, dispositivo, banderas)` hace, en este orden (todo medido contra el EXE en `bench_d3d`, con un dispositivo falso que devuelve `OK`, `D3DERR_DEVICELOST` 0x88760868, `D3DERR_DEVICENOTRESET` 0x88760869, `D3DERR_INVALIDCALL` y un error genérico):

1. Si hay escena abierta (`0xC97C54`): `EndScene` y la cierra.
2. Sincronía vertical por el bit 0 de las banderas contra `D3DPRESENT_PARAMETERS::PresentationInterval` (`0xC9C074`): con el bit y el intervalo en IMMEDIATE (0x80000000) lo pone a ONE (1); sin el bit y en ONE lo pone a IMMEDIATE. **Cualquier cambio reinicia el dispositivo** (en los demás casos no se toca).
3. Reinicio = `_rwD3D9DeviceReleaseVideoMemory` (0x7F7F70, libera lo que no sobrevive al `Reset`) + `IDirect3DDevice9::Reset(&0xC9C040)` (resultado ignorado, como el EXE) + la cadena común de recuperación (`D3D9RestoreAfterReset` de `rwcore.cpp`: vuelve a anotar `GetRenderTarget`/`GetDepthStencilSurface`, restaura la lista de rasters (0x4CC970), los búferes de vértices dinámicos (0x7F58D0) y los estados de render (0x7FD100), reaplica las luces (`SetLight` + `LightEnable`), reabre Im2D/Im3D (0x7FB480, 0x80E020) y llama al callback de restauración `0xC980B0`).
4. `Present` de la cadena de intercambio del raster superior (cinco ceros) o, sin ella, `Present` del dispositivo (cuatro ceros).
5. **Pérdida de dispositivo**: con `DEVICELOST` se mira `TestCooperativeLevel`; con `DEVICENOTRESET` se repite el reinicio del punto 3. Con cualquier otro resultado negativo no se hace nada más (el dispositivo sigue perdido hasta el marco siguiente, que vuelve a pasar por aquí).
6. **Colas**: siempre, tras el `Present`, `_rwD3D9DynamicVertexBufferManagerForceDiscard` (0x7F5800: pone a cero las cuatro posiciones de los búferes dinámicos y avanza el índice módulo 4, para que el marco siguiente descarte en lugar de reescribir sobre lo que la GPU aún lee).
7. Devuelve TRUE si el último HRESULT (el del `Present`, o el de `TestCooperativeLevel` si hubo pérdida) no es negativo; tras una pérdida devuelve FALSE.

`_rwD3D9CameraEndUpdate` (0x7F98D0) copia (`StretchRect`, filtro POINT si `0xC9BFF4 & 0x1000100`, si no NONE) la superficie de color actual (`0xC97C30`) a la superficie nivel 0 (o cara del cubo) de la textura de una cámara de tipo «textura de cámara» (cType 5, sin cadena de intercambio, con copia pendiente `0xC97C34`) sobre el rectángulo de su raster de imagen, libera la superficie y anula la copia pendiente; siempre deja la cámara actual (`0xC9BCC0`) a NULL y devuelve TRUE.
`_rwD3D9StandardImageFindRasterFormat` (0x7FFF00) copia ancho y alto de la imagen al raster (profundidad 0) y, según `banderas & 7`, ajusta el tamaño a uno válido de textura (0x7FED70) y calcula el formato con `_rwD3D9ImageFindRasterFormat` (0x7FFFF0) y `SetRasterFormat` (0x4CC580); tipos 3, 6 y 7 dan el error 0x8000000D.

## Cola de «viva sin hook» recalculada

`docs/CodigoVivoExe/cola-raices-valor-p6.csv` estaba desactualizada (47 filas; incluía lo que ya hizo D3DSTD, p. ej. 0x4CD820). Se regeneró sobre `main` actual con `python -X utf8 contrib/huecos_p6.py && python -X utf8 contrib/raices_valor_p6.py` (y `contrib/codigo_vivo_plan.py`):

| Momento | Filas «viva sin hook» | Bytes |
| --- | ---: | ---: |
| Cola del repositorio (anterior a D3DSTD) | 47 | 23.483 |
| Regenerada sobre `main` 4a4a7b7328 (antes de esta rama) | 34 | 20.064 |
| Tras esta rama (las 6 de la cola de la fase 8 que aparecían — 0x750480 estaba dentro de una pieza de hueco — y 18 más) | **10** | **15.747** |

Las 10 que quedan, con la razón de no hacerlas en esta rama (todas son RenderWare/juego citadas como valor):

| Dirección | Bytes | Qué es | Razón |
| --- | ---: | --- | --- |
| 0x75F0B0 | 7.153 | crea un vertex shader (`rpworld.cpp`) | grande: lote propio |
| 0x75A960 | 2.933 | núcleo común de los generadores de tiras exhaustivos de `RpBuildMesh` (se llama con indicador 0 y 1 desde `rpworld_s30a31a.cpp`) | grande, con listas libres propias; lote propio |
| 0x7624A0 | 1.205 | escritura del sector atómico raíz del mundo (`RpWorldStreamWrite`; su pareja de planos es 0x762740) | media, flujo y plugins; lote propio |
| 0x762F30 | 980 | lectura de un sector atómico del mundo (`RpWorldStreamRead`) | media, flujo y plugins; lote propio |
| 0x763310 | 663 | lectura de un sector de plano del mundo (`RpWorldStreamRead`) | igual |
| 0x7F40F0 | 966 | callback del módulo de texturas de RenderWare (campo +0x14 del módulo 0xC97B4C, `rwcore.cpp:13967`) | grande: lote propio |
| 0x7F5140 | 652 | ídem (campo +0x2C) | ídem |
| 0x7F53D0 | 292 | ídem (campo +0x18) | ídem |
| 0x74A770 | 669 | reúne las geometrías distintas de los átomos de un clump (`BuildGeometryList`) | media, con lista temporal y destrucción; lote propio |
| 0x7F2930 | 234 | eje unitario de una matriz de rotación (x87 con `RwSqrt`, usa una constante del `.rdata` 0x858624) | pequeña pero x87 con constante propia; necesita su banco de x87 |

## Verificación

* **Bancos fuera del juego** (método FnD3DStd/FnP3, [reversing/FnCAM](../../reversing/FnCAM/README.md)): `bench_cam` (0x750430, 0x750460, 0x750480, 0x750AC0, 0x7507E0), `bench_d3d` (0x7F98D0, 0x7F99B0, 0x7FFF00 con falsos de `IDirect3DDevice9`, cadena de intercambio, superficies y texturas con traza completa) y `bench_misc` (0x74CE60, 0x762440, 0x762040, 0x75C290): **0 diferencias** en 500.000, 300.000 y 400.000 casos; **pruebas negativas** (mutaciones de una línea en el texto C++ real): 31 de 31, 25 de 25 y 18 de 18 detectadas (la primera pasada dejó algunas sin detectar por un generador pobre y por mutaciones que caían en otra función: corregidas, ver el README).
* **Compilación**: `_build_VIVOCAM.sh`, EXIT=0 (build Debug del ASI, sin errores).
* **Auditorías**: `contrib/audit_hook_catalog.py --strict` OK; `contrib/audit_cpp_assembly.py`: 0 `__asm`/`naked`.
* **Humo** (`D:\Original\wv-8\reversing\FnBIS1`, con la DLL de esta rama): `prueba_ropero.py cal_mal.args cam1`: **BIEN** (puerta True, marcador True); `prueba_agua.py cal_mal.args cam1`: **BIEN** (bajo el agua, `UnderWaterness` 1,0 y brillo medio 50,2); `prueba_marcadores.py cal_mal.args cam1`: **BIEN** (12 marcadores en memoria y los 12 cambian en 1 s). El juego carga la ventana, crea la cámara con las envolturas del plugin 0x509 y dibuja con `RwCameraShowRaster`/`CAMERAENDUPDATE` de C++.
* **No cubierto por banco**: los callbacks finos de la tabla de abajo (humo): el contador y los tamaños de flujo del clump, la escritura de luz y cámara, `RpLightStreamWrite`, los tres callbacks de retirada de un clump del mundo, el render de un sector, `CheckCubeTextureFormat`, `BugstarWriteBlock` (sin llamadores en el juego) y el destructor de `atexit` de la matriz de `AddWeaponStreak`.
