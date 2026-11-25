<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnD3D9Device: `D3D9System` (0x7F5F70) en C++ y retirada del parche `naked` 0x7F6781 (21/11/2026)

Rama `d3d9-device` (desde `origin/main` 4d39c9866b). Sin subir a origin ni a main. Lote **D3D9DEV**, todo `reversed=1, validated=0`.

## Qué era 0x7F6786

`0x7F6786` es el `jge` que sigue a la llamada a `IDirect3D9::CreateDevice` (`call [edx+0x40]` en 0x7F6781). El parche `naked` de `WindowedMode.cpp` (`HookDirect3DDeviceReplacerSA`, hook en 0x7F6781, retorno a 0x7F6786) estaba **dentro de una función grande**: el despachador del sistema del dispositivo D3D9 de RenderWare (`D3D9System`, SDK de RW 3.6/3.7), que empieza en **0x7F5F70**.

| | |
|---|---|
| Funcion contenedora | 0x7F5F70, cdecl, `(peticion, salida, inOut, entrada) -> RwBool`; `sub esp, 0x470`; `jmp [peticion*4 + 0x7F6A1C]` (23 casos); cuerpo disperso 0x7F5F70..0x7F6A1B |
| Llamantes | solo por puntero: el campo `fpSystem` (+4) de la estructura `RwDevice` en 0x8E2498 (se copia en la peticion REGISTER) y, a traves de ella, `_rwDeviceSystemRequest` (0x7F2AB0). Ninguna `call` ni `jmp` relativo apunta a 0x7F5F70 (comprobado buscando `E8/E9 rel32` y punteros de 4 bytes en todo el EXE) |
| Llamados | `_rwD3D9FindMSLevels` (0x7F6BF0), `_rwD3D9SetPresentParameters` (0x7F6CB0), `_rwD3D9StateCacheReset` (0x7F6B00), `_rwD3D9TransformCacheReset` (0x7F6B40), `_rwD3D9EnumerateDisplayModes` (0x7F7540), `_rwD3D9DeviceSystemStop` (0x7F7100), `_rwD3D9DeviceSystemStandards` (0x7F72E0), `_rwD3D9VertexBufferManagerOpen` (0x7F5CB0), `_rwD3D9Im2DRenderOpen` (0x7FB480), `_rwD3D9RenderStateOpen` (0x7FCAC0), `RasterListInit` (0x4CC150), deteccion de CPU (0x80DBE0, 0x80DC40, 0x80DCA0, 0x80DD00), `_rwMatrixSetMultFn` (0x7F12C0), `_rwVectorSetMultFn` (0x7ED5F0), `Direct3DCreate9` (import), `ShowWindow` y `GetClientRect` (imports) y la rutina x87 0x857404. Todas menos la ultima ya estaban en C++ |

**Decision: reemplazar la funcion completa, no un tramo.** El tramo de 0x7F6781 es el `case 2` (START) de un `switch` por tabla de saltos; no tiene entrada de funcion, comparte epilogo con los demas casos y los casos 7 y 16 comparten codigo con `_rwD3D9FindMSLevels`. No se puede sustituir como C++ "normal" sin sustituir la funcion, asi que se reconstruyen las 23 peticiones. Los saltos de unos casos a la cola de otros (0x7F60CD, 0x7F6A10...) son epilogos comunes del compilador.

## Qué cambió

- `source/game_sa/RenderWare/rw/rwcore.cpp`: nueva `_rwD3D9System` (grupo `RwD3D9`, `RH_ScopedGlobalInstall(_rwD3D9System, 0x7F5F70)` en `RwD3D9InjectHooksR26A`), con la tabla de peticiones y la descripcion de cada una en el comentario previo. Reutiliza los ayudantes y globales del lote R26A (`D3D9FindDepth`, `ComCall`, `D3D9Present`, ...).
- `source/app/platform/win/WindowedMode.cpp/.hpp`: **desaparecen** `HookDirect3DDeviceReplacerSA`, `STACK_PUSH`/`STACK_POP` y todo el `__asm`/`naked`; la logica del modo ventana pasa a dos funciones normales, `notsa::WindowedMode::BeforeCreateDevice()` (llama a `AdjustPresentParams(&Present)`: `Windowed`, intervalo de presentacion 0, formato A8R8G8B8/X8R8G8B8 segun el estado de juego, `SwapEffect` DISCARD con multimuestreo, tamano de cliente y `hDeviceWindow`) y `AfterCreateDevice()` (instala el proxy `Reset`/`SetViewport`, `D3D9ProxyDevice::Initialize`). `_rwD3D9System` las llama justo antes y justo despues de `CreateDevice`, en el mismo orden que el parche.
- Un fallo de `CreateDevice` con `NOTSA_WINDOWED_MODE` sigue cerrando el proceso (`NOTSA_LOG_ERR`, `Sleep(10)`, `exit(10)`), como el parche. Sin ese indicador la funcion sigue el camino del EXE (`0xC97C38 = 0` y devuelve FALSE).
- `docs/hooks.csv`: fila `RwD3D9,_rwD3D9System,0x007F5F70,1,0,0,S`. `docs/hooks_notsa.csv`: se retira la fila `HookDirect3DDeviceReplacerSA` (quedan 2 hooks de `WindowedModeHooks` y `NOTSA_WinMain`). `docs/HooksNOTSA.md`, `docs/DesviacionesConocidas.md`, `reversing/CppOnlyMigration/PROCESO.md` (punto 7: excepcion retirada) y los inventarios generados (`docs/implementaciones/*`, `reversing/CppOnlyMigration/*`) actualizados.
- Los comentarios de rwcore.cpp sobre 0x7F6781 y 0x7F6786 (lotes R26A y S33A) ahora dicen que quedaron resueltos.

## Peticiones (resumen; el detalle esta en el comentario de `_rwD3D9System`)

| Peticion | Que hace |
|---|---|
| 0 OPEN | `Direct3DCreate9(31)`, nombre "D3D9", primer adaptador con `GetDeviceCaps` correcto, recuento de modos de los formatos 0x17/0x16/0x23, modo del escritorio, `_rwD3D9FindMSLevels`, deteccion MMX/SSE/SSE2/3DNow! |
| 1 CLOSE, 3 STOP, 4 REGISTER, 11 STANDARDS | liberar tabla de modos y objeto D3D; llamar a `_rwD3D9DeviceSystemStop` / `Standards`; copiar `RwDevice` (0x8E2498, 0x38 bytes) |
| 2 START | `SetPresentParameters`, banderas de comportamiento (MULTITHREADED, SOFTWARE_VP / HARDWARE_VP / PUREDEVICE segun DevCaps), **`CreateDevice`** (con el gancho del modo ventana), tamano del cliente, render target y superficie de profundidad, cache de estado/matrices, luces, lista de rasters, Im2D, estados de render, gestor de buferes |
| 5, 6, 7, 10 | numero de modos, informacion del modo (RwVideoMode), usar modo (con `_rwD3D9FindMSLevels`), modo actual |
| 8 FOCUS | `ShowWindow(SW_RESTORE / SW_HIDE)` solo en pantalla completa |
| 12-16 | memoria de texturas, adaptadores (numero, identificador, actual, fijar adaptador) |
| 17 FINALIZESTART | palabra de control x87 (24 bits), FZ en MXCSR con SSE, `_rwMatrixSetMultFn`, `_rwVectorSetMultFn` con el juego de callbacks de la CPU |
| 19, 22 | maximo de MaxTextureWidth/Height; `*(uint16*)salida = 2` |
| 9, 18, 20, 21 y mayores de 22 | FALSE |

## Dependencias del EXE que quedan (datos y rutinas sin reconstruir)

- 0x857404 (fija la palabra x87 y guarda la anterior en 0xCAF004; la restaura 0x85742B). Mismo tratamiento que ya tenia `D3D9RestoreFpuControlWord`: no hay intrinseco para `fldcw`/`fnstcw` con el mismo almacenamiento.
- Callbacks de matriz y vectores entregados por direccion: `0x8572A0` (matriz) y los juegos `0x8570EF/0x857000/0x857160/0x85707F` (x87), `0x80D570/0x80D440/0x80D600/0x80D4E0` (SSE2) y `0x80D280/0x80D070/0x80D370/0x80D180` (SSE); los de 0x80Dxxx estan en C++ (lotes R33A/R34A) y se alcanzan por su direccion enganchada; los x87 de 0x857xxx no estan reconstruidos.
- Datos: la estructura `RwDevice` (0x8E2498) y la cadena "D3D9" (0x8E2510) se leen del EXE; la lista de formatos 0x884788 se sustituyo por constantes.
- `RasterListInit` (0x4CC150) se llama por su direccion enganchada (como ya hacia `_rwD3D9DeviceSystemStop` con 0x4CC170).

## Verificación

- **Compilacion**: `reversing/FnD3D9Device/build_1.log` (primera, completa, `EXIT=0`, `bin/Debug/gta_reversed.asi`) y la final (`_build_D3D9DEV.log`). Script: `/d/Original/_build_D3D9DEV.sh` (candado `/d/Original/compile_lock`). Comprobacion de sintaxis rapida: `chk.bat`.
- `python -B contrib/audit_hook_catalog.py --strict`: **OK**. `python -B contrib/audit_cpp_assembly.py`: **0** ocurrencias de `__asm`/`naked` en `source/game_sa` (y WindowedMode.cpp ya tampoco tiene).
- **Banco fuera del juego** (`bench_d3d9sys.cpp`, `extract_bench.py`, `build_bench.cmd`; mismo metodo que FnLIB4A y FnS31AFILE): un proceso de 32 bits mapea el `gta_sa.exe` original en 0x400000 (solo lectura) y ejecuta la funcion **original** de 0x7F5F70 y despues la **C++** (el bloque extraido de rwcore.cpp, compilado en el banco) sobre el mismo estado. `IDirect3D9`, `IDirect3DDevice9` y las superficies son simulacros con registro de llamadas (ranura, argumentos, resultados, huella de `D3DPRESENT_PARAMETERS` y de `D3DCAPS9` en cada `CreateDevice`); `Direct3DCreate9`, `ShowWindow` y `GetClientRect` se simulan por la tabla de importaciones del EXE y por macros en el C++. Las funciones auxiliares pesadas (lista de rasters, Im2D, estados, gestor de buferes, cache de matrices, parada, estandares, parametros de presentacion, `_rwMatrixSetMultFn`, `_rwVectorSetMultFn`) son registradores parcheados en el EXE (los usan las dos ejecuciones); siguen reales la deteccion de CPU, 0x857404, la enumeracion de modos (0x7F7540) y los niveles de multimuestreo (0x7F6BF0). Cada escenario crea un mundo aleatorio (0-3 adaptadores, capacidades, formatos del escritorio, modos, fallos de `GetDeviceCaps`, `CreateDevice` y `Direct3DCreate9`) y una secuencia de peticiones 0..23 (incluidas las invalidas); se comparan valor devuelto, bytes de salida, registro completo y huella de los datos del EXE (y MXCSR y palabra x87) tras cada peticion y al final.
  - Resultado: **4 semillas x 5.000 escenarios + 300 + 5 = 20.305 escenarios, unas 312.000 peticiones, 0 diferencias.** Control negativo (con 0x7F6BF0 sustituida por `xor eax,eax; ret` solo en la ejecucion del EXE): diferencias en todos los escenarios probados (9 de 9).
  - Limites: sin NOTSA (el camino NOTSA, `BeforeCreateDevice/AfterCreateDevice` y `exit(10)`, se revisa por lectura y se prueba con el humo); no hay fallos de `GetRenderTarget/GetDepthStencilSurface` (el EXE tampoco los gestiona: desreferencia el puntero nulo); `SetPresentParameters` es un registrador (se prueba aparte, lote R26A).
- **Humo** (ver mas abajo).

## Humo (DLL propia `out/gta_reversed_d3d9dev.asi`, variante X de `jugar.ps1`; sin otro `gta_sa.exe` ni candados; cada script mato solo su PID)

Se lanzan en serie con `humo_todo.sh` (`AGUA_ASI` apunta a la DLL propia, argumentos `cal_mal.args`):

| Prueba | Resultado |
|---|---|
| `wv-8/reversing/FnBIS1/prueba_ropero.py` | **BIEN** (puerta 2877 y marcador del ropero; `humo_ropero.log`) |
| `wv-8/reversing/FnBIS1/prueba_agua.py` | **BIEN** (brillo medio 48,3; `humo_agua.log`) |
| `humo_ventana.py` (nuevo) | **BIEN** (`humo_ventana.log`): ForegroundApp (0x8D621C) = 1 escrito desde fuera, estado de juego 9 alcanzado; `D3DPRESENT_PARAMETERS` en modo ventana (`Windowed` 1, 800x600, formato X8R8G8B8 = 22, intervalo de presentacion 0, `hDeviceWindow` = ventana principal, 1 back buffer, multimuestreo 4, `SwapEffect` DISCARD (1), auto profundidad D24S8 = 75); dispositivo creado, `0xC97C38 = 1`, la ventana con barra de titulo (`WS_CAPTION`); las entradas `Reset` (16) y `SetViewport` (47) de la tabla virtual del dispositivo caen en `gta_reversed.asi` (el proxy de `AfterCreateDevice` esta instalado); 0x7F5F70 empieza por `E9` (hook de `_rwD3D9System`) |

Es decir, con el parche `naked` retirado el juego arranca en modo ventana, con VSync desactivado (intervalo 0, como el ajuste del parche), formato y multimuestreo como antes y con el mismo proxy de dispositivo. El humo no compara contra la DLL anterior; la equivalencia por funcion la da el banco de arriba.
