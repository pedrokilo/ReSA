<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución D3DSTD: manejadores de raster de la tabla estándar de D3D9 y el instanciado genérico AllInOne (0x756960)

Rama `vivo-d3dstd` (desde `origin/main` d911da633b, 21/11/2026). Parte del hallazgo 2 de [ejecucion-v48.md](ejecucion-v48.md) (V7-1: cuatro ayudantes `usercall` vivos por una tabla) y del hueco `0x756958` de [ejecucion-p6.md](ejecucion-p6.md). Resultado por rutina en [cierre-d3dstd.csv](cierre-d3dstd.csv); banco y falsos en [reversing/FnD3DStd](../../reversing/FnD3DStd/README.md).
Decisión vigente del usuario: sin validar función por función dentro del juego; basta un banco fuera del juego y el humo.

## Resumen

| Bloque | Rutinas | Estado | Verificación |
| --- | ---: | --- | --- |
| 12 manejadores de la tabla `stdFunc` de `_rwD3D9DeviceSystemStandards` (huecos 0x4CA4E0, 0x4CAE40/60/80, 0x4CB4C0/E0, 0x4CB520, 0x4CBCB0, 0x4CBD40/50, 0x4CD4D0, 0x4CD820) | 12 | C++ puro en `D3D9RasterStandards.cpp`, **enganchados** (cdecl) y guardados por puntero C++ en la tabla | banco `bench_std`: 100.000 casos por manejador, 0 diferencias |
| Ayudantes `usercall` que por ellos seguían vivos (0x4CAD90, 0x4CAEA0, 0x4CA5A0, 0x4CAAE0) | 4 | funciones C++ con los registros como parámetros, **sin enganche** (ABI de registros; ya no tienen llamadores en el EXE) | los cubre `bench_std` a través de los manejadores |
| Instanciado genérico AllInOne (0x756960, 1.080 bytes) | 1 | `D3D9InstanceObject` en `rpworld_p6b.cpp`, **enganchada** (cdecl, 7 argumentos); la llaman los nodos de átomos y sectores (ya C++) sin pasar por la dirección | banco `bench_inst`: 200.000 casos, 0 diferencias |
| **Total** | **17** | | |

## Cómo se invocan (no hay puente con hook)

Los 12 manejadores son **cdecl** `(salida, entrada/salida, valor)`: son las entradas de la tabla `RwEngineInstance->stdFunc` (índices `rwSTANDARD*`), y el motor las llama por puntero. Las 12 eran direcciones del EXE guardadas por `_rwD3D9DeviceSystemStandards` (`rwcore.cpp`); ahora la tabla guarda las direcciones de las funciones C++
(`Fn(&D3D9RasterStandards::X)`) y cada dirección del EXE queda además enganchada con `RH_ScopedInstall` (por si algún código del EXE aún las llamara). Ningún llamador del EXE usa registros para llamarlas: el escaneo de todas las llamadas `E8/E9` y de las referencias absolutas al `.text` encuentra solo la tabla (0x7F73B9..0x7F74D7, que `_rwD3D9DeviceSystemStandards` sustituye)
y los saltos internos entre ellas. **No hace falta ningún puente**: los llamadores del motor son C++ (`RwRasterRender*`, `RwRasterClear*`, `RwRasterLock*`, `RwTextureStreamWrite`... usan `stdFunc[...]`).

Los ayudantes `usercall` solo los llamaban estos manejadores (escaneo igual: `0x4CAD90` desde 0x4CAE4C/6C/8F; `0x4CAEA0` desde 0x4CB4CA/B50A; `0x4CAAE0` y `0x4CA5A0` desde 0x4CAD90): al pasar los manejadores a C++ pierden todos sus llamadores y quedan como funciones C++ locales con parámetros (`RasterBlit`, `RasterFill`, `CopyBetweenSurfaces`, `RenderTexturedQuad`).
No llevan enganche porque su ABI usa registros (`EAX`, `EDX`, `ECX`, `ESI`, `EBX`); sus bytes siguen en el EXE pero ya nadie los ejecuta.

## Qué hace cada rutina (índice `rwSTANDARD*`)

| Dirección | Función | Resumen |
| --- | --- | --- |
| 0x4CBD40 | `SetTextureRaster` (8) | `*salida = entrada` |
| 0x4CB520 | `SetRasterContext` (0xB) | destino actual (`0xB4E9E4`) = raster |
| 0x4CBD50 | `RasterSubRaster` (0xC) | copia paso, profundidad, tipo y formato del padre; pixeles a NULL |
| 0x4CB4C0 / 0x4CB4E0 | `RasterClearRect` (0xD) / `RasterClear` (0xE) | `RasterFill` del rectángulo o del destino entero |
| 0x4CAE40 / 0x4CAE80 / 0x4CAE60 | `RasterRender` (0x11) / `RasterRenderScaled` (0x12) / `RasterRenderFast` (0x13) | `RasterBlit` (ECX = escalado, pila = mezcla): (0, 1), (1, 1), (0, 0) |
| 0x4CA4E0 | `RasterLockPalette` (0x17) | solo tipos 0, 4 y 5; paleta de la extensión a `raster->palette`, `privateFlags |= 8` (|= 0x18 con escritura) |
| 0x4CBCB0 | `RasterGetMipLevels` (0x1C) | `GetLevelCount` de la textura del raster superior (o bits del lado mayor sin textura); 1 si no es tipo 0, 4 o 5 |
| 0x4CD4D0 | `NativeTextureWrite` (0x1B) | chunk 1 (versión 0x36003) + cabeceras de 0x48 y 0x10 bytes + paleta + niveles de cada cara |
| 0x4CD820 | `NativeTextureRead` (0x1A) | lo inverso: crea raster y recurso D3D según las banderas (comprimida, cubo, normal), verifica y lee paleta y niveles, crea la textura |
| 0x4CAD90 | `RasterBlit` | por tipos de destino y origen: `CopyBetweenSurfaces` (GPU a GPU), `RenderTexturedQuad` (origen sobre camara o textura de camara) o nada |
| 0x4CAAE0 | `CopyBetweenSurfaces` | `UpdateSurface`, `GetRenderTargetData` o `StretchRect` según los tipos; libera las superficies que no son de camara |
| 0x4CA5A0 | `RenderTexturedQuad` | quad Im2D sobre el destino de render con estados guardados y restaurados; sin destino de render, `StretchRect` del back buffer |
| 0x4CAEA0 | `RasterFill` | cámara: `Clear` (target); z-buffer: `Clear` (zbuffer); textura de cámara y textura en el pool por defecto: `ColorFill`; textura bloqueable: relleno por pixeles (32, 16 y 8 bits con la paleta más cercana) |
| 0x756960 | `D3D9InstanceObject` | crea la entrada de recursos (cabecera + datos de instancia), el búfer de índices (con la conversión tira a lista de triángulos si no compensa), rangos de vértices, `numPrimitives` y llama al callback de instanciado |

Particularidades reproducidas tal cual del EXE (cada una la cubre el banco): `UpdateSurface` usa como punto de destino el desplazamiento del **origen** (no el del destino); la rama «destino no renderizable» de `RenderTexturedQuad` copia con `StretchRect` el back buffer a la textura; `NativeTextureWrite` deja la cara de la extensión en 6 tras un cubo; los fallos de escritura devuelven 0 **sin**
desbloquear; el recorte a lista de triángulos de `D3D9InstanceObject` lo decide el byte de la tabla del plugin de mallas (`engine + [0xC9B8C0] + 8 + primType`, 4 = tira) y el nº de triángulos del objeto (sector `0xFF`: +0x84; átomo `1`: los de su geometría).

## Cambios en el código

* **Nuevos**: `source/game_sa/RenderWare/D3D9RasterStandards.h/.cpp` (clase `D3D9RasterStandards`; `InjectHooks` registrada en `InjectHooksMain.cpp`).
* `rwcore.cpp`: la tabla de `_rwD3D9DeviceSystemStandards` guarda las 12 funciones C++; entradas públicas mínimas (`D3D9RasterLockCpp`, `D3D9RasterUnlockCpp`, `RxD3D9ResEntryHeaderDestroyCpp`) para código fuera del espacio de nombres anónimo.
* `D3D9RasterDriver.h/.cpp`: `CheckCubeFormatUsage400Cpp` (el resto de las comprobaciones ya tenían entrada `*Cpp`).
* `rpworld.cpp`: entradas públicas `D3D9FindMinVertAndNumVerticesCpp`, `D3D9SortTrianglesCpp` y `D3D9TriStripToTriListCpp` (0x756710/0x756770/0x756830 siguen enganchadas).
* `rpworld_p6b.cpp`: `D3D9InstanceObject` (0x756960) en lugar de la llamada por dirección.
* `docs/hooks.csv`: 12 filas `D3D9RasterStandards` y `RpWorld,D3D9InstanceObject`.

## Bancos (reversing/FnD3DStd)

Método FnP3 (`bench_d3d`): el `gta_sa.exe` original se mapea en 0x400000 en un proceso de 32 bits, se llama por su dirección y se compara con el C++ compilado en el mismo proceso. Los falsos de las funciones de RenderWare y de los métodos COM de `IDirect3DDevice9`, texturas y superficies llevan traza (marca y argumentos) y resultados por hash; se compara el valor devuelto, la arena
completa (rasters, extensiones, objetos, entradas de recursos), el búfer de píxeles y de la paleta, el flujo escrito y la **traza completa de llamadas**. Resultados (SIN DIFERENCIAS): `bench_std` 100.000 casos por manejador (12 manejadores, ayudantes incluidos); `bench_inst` 200.000 casos (primitivas de los seis tipos, 13.139 tiras recortadas a lista de triángulos, rutas nativa y de pool).
Prueba negativa: ver [reversing/FnD3DStd/README.md](../../reversing/FnD3DStd/README.md) (`resultados/prueba_negativa*.txt`).

## Lo que no entra en este lote
* Otras tres direcciones del EXE que la misma tabla sigue guardando: `0x7F98D0` (`rwSTANDARDCAMERAENDUPDATE`), `0x7F99B0` (`rwSTANDARDRASTERSHOWRASTER`) y `0x7FFF00` (`rwSTANDARDIMAGEFINDRASTERFORMAT`); no estaban en el encargo (ver el informe de la rama).
* Humo con el juego (ropero y agua) y compilación: ver el informe de la rama.
