# Sustitución de DirectDraw por D3D9

## Estado real encontrado (antes del cambio)
El plan (`docs/RutaExeIndependiente.md`: «DDRAW: solo lista modos de vídeo; reescribir con D3D9/DXGI») describía algo más grande que la realidad:

- **La lista de modos de vídeo, los adaptadores y su orden ya eran D3D9 en C++** desde antes: `_rwD3D9EnumerateDisplayModes` (rwcore.cpp, GetAdapterDisplayMode/GetAdapterModeCount/EnumAdapterModes), `RwEngineGetSubSystemInfo` (GetAdapterIdentifier), `GetBestRefreshRate` y `GetVideoModeList` (WinPs.cpp, VideoMode.cpp). DirectDraw no intervenía.
- DirectDraw lo usaban solo **tres cosas**, todas en `WinPs.cpp`/`DelayLoadThunks.cpp`:
  1. `GetVideoMemInfo` (0x7455E0): `DirectDrawCreateEx` + `IDirectDraw7::GetAvailableVidMem(DDSCAPS_VIDEOMEMORY)` -> `s_OSStatus.VRAM.Total/Avail` (0xC8CF80/0xC8CF84). Solo se lee el total (`VideoMode.cpp`: filtro de modos distintos de 640x480 por memoria libre).
  2. `GetDXVersion` (0x745650): `LoadLibrary("DDRAW.DLL")`, `DirectDrawCreateEx`, `Release`, y después D3D8.DLL / dpnhpast.dll / D3D9.DLL; devuelve 0x900 si todo está (comprobado en `psInitialize`: `< 0x900` -> mensaje WIN_DX y salir).
  3. El thunk de carga retardada `DirectDrawCreateExThunk` (0x81805C, ranura 0x8E3EBC), solo llamado desde 0x7455E0.
- Enlace: `ddraw` en `source/CMakeLists.txt` y `ddraw.lib` en `reversing/FnFase8/enlazar.cmd`. El ejecutable independiente **importaba DDRAW.dll de forma implícita** (`dumpbin /imports`: `DDRAW.dll  DirectDrawCreateEx`), así que ddraw.dll se cargaba al arrancar; el ASI también (import directo de `GetVideoMemInfo`).

## Mapeo de cada llamada DirectDraw -> D3D9

| Antes (DirectDraw 7) | Ahora (D3D9) |
|---|---|
| `DirectDrawCreateEx(NULL, &dd, IID_IDirectDraw7)` (adaptador primario) | `Direct3DCreate9(D3D_SDK_VERSION)` + `CreateDevice(D3DADAPTER_DEFAULT, HAL, ventana STATIC oculta, ventana, SOFTWARE_VERTEXPROCESSING \| FPU_PRESERVE \| NOWINDOWCHANGES)` |
| `GetAvailableVidMem(DDSCAPS_VIDEOMEMORY, &total, &libre)` | `IDirect3DDevice9::GetAvailableTextureMem()`; D3D9 da una sola cifra: total = libre = ese valor |
| `dd->Release()` | `device->Release(); DestroyWindow; d3d->Release()` |
| fallo de `DirectDrawCreateEx`: devuelve el HRESULT, no escribe las salidas | sin D3D9: `D3DERR_NOTAVAILABLE`; sin dispositivo HAL: HRESULT de `CreateDevice`; en ambos las salidas no se tocan (`psInitialize` no mira el resultado, como antes) |
| `GetDXVersion`: `LoadLibraryA("DDRAW.DLL")` (+ avisos «Couldn't LoadLibrary DDraw» / «GetProcAddress DirectDrawCreateEx» / «Couldn't DirectDrawCreateEx», retorno 0) y `DirectDrawCreateEx` | eliminado; se conservan D3D8.DLL (0x700), dpnhpast.dll (0x800), D3D9.DLL (0x801) con sus avisos y el 0x900 final |
| `DirectDrawCreateExThunk` 0x81805C (gancho) | retirado (y su fila en `docs/hooks.csv`): sin llamadores |
| `ddraw` / `ddraw.lib` | quitados del CMake y de `enlazar.cmd` |

`D3DCREATE_FPU_PRESERVE` es imprescindible: sin él D3D9 deja el x87 en 24 bits y rompe la aritmética del juego.

## Diferencias observables (honestas)
- **Memoria de vídeo**: valores medidos en esta máquina (Intel Iris Xe): DirectDraw `total=4294901760 libre=4286607360`; D3D9 `4293918720` para ambos (-0,02 % el total; el «libre» ya no se distingue). Ninguno afecta a la lista de modos (los dos son ~4 GB, muy por encima de los 45 MB que exige el filtro). Si otro equipo diera cifras cercanas al umbral, DirectDraw y D3D9 podrían diferir; no verificado en otro hardware.
- El juego ya no comprueba que DDRAW.DLL exista (en Windows actual siempre está; no era un requisito real del juego).
- `GetVideoMemInfo` crea un dispositivo D3D9 temporal en `psInitialize` (unas decenas de ms).

## Evidencia (`python reversing/FnFase8/probar_directdraw.py`, resultados en `reversing/FnFase8/ejecuciones/dd-*`)
Medido por memoria en el proceso y con `EnumProcessModulesEx`.

| Ejecución | ddraw.dll cargada | DxVer | VRAM total / libre | modos RW |
|---|---|---|---|---|
| `orig2`: gta_sa.exe original puro | sí | 0x900 | 4294901760 / 4286607360 | 52 |
| `stdbase`: standalone ANTES del cambio | sí | 0x900 | 4294901760 / 4286607360 | 35 |
| `std5`: standalone nuevo | **no** | 0x900 | 4293918720 / 4293918720 | 35 |
| `asi2`: gta_sa.exe + ASI nueva | **no** | 0x900 | 4293918720 / 4293918720 | 35 |

- `dumpbin /imports` del standalone nuevo y de la ASI: sin DDRAW.dll; `.map` y `link.rsp` sin ddraw.
- La tabla de modos del standalone nuevo es idéntica byte a byte (ancho, alto, frecuencia, formato, bandera, orden) a la del standalone anterior, y a la del original una vez quitados los modos R5G6B5 (35 = 52 - 17).
- **Observación previa a este cambio (no causada por él)**: el original puro enumera 52 modos (incluye R5G6B5, 16 bits) pero el standalone y el EXE+ASI enumeran 35 (`GetAdapterModeCount(R5G6B5)` da 0 en ellos). Se reproduce con el standalone anterior (`stdbase`), así que no depende de DirectDraw. Una ejecución (`std1`) dio 52 y no he sabido explicarla. Causa no investigada (hipótesis: SDL/ventana del proyecto).
- Menú de opciones de pantalla en el standalone (Pausa -> Opciones -> Config. pantalla -> Avanzadas -> Resolución): `ejecuciones/dd-std7/paso16_enter.png` y `dd-std8/paso25_right.png` (la resolución cambia al recorrer la lista: 800x600 -> 1280x768). La lista en memoria (`gVideoModes`, 0xC920D0) solo se pudo leer en el original (33 entradas); en el standalone y en el ASI esa dirección leyó 0 (no investigado por qué: puede ser que la variable viva en otra dirección del proceso propio), así que la lista del menú no se comparó textualmente; se compara la tabla de RenderWare de la que sale.
- Arranque en ventana: el standalone abre ventana (640x480) sin ddraw. **No probado**: pantalla completa exclusiva en el standalone (la ruta de vídeo no cambió; no se ha forzado).
