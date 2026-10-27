<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies24A (grupo g24a de BRIEF_21): Win::__MainWndProc

Procedimiento de ventana del juego (`WNDPROC`, EXE 0x747EB0). Pares EXE (hook RH apagado) frente a DLL (hook RH encendido), dentro del juego, con colaboradores sustituidos por los mismos falsos trazados.
**La corrida dentro del juego NO se ha ejecutado** (encargo de solo analisis). La evidencia previa es el ensayo sin juego de `offline/` (bytes reales del EXE frente al C++ de la DLL), que si se ejecuto.

| Funcion | Entrada | Convencion | Pares |
| --- | --- | --- | --- |
| Win::__MainWndProc | 0x747EB0 | `__stdcall` (HWND, UINT, WPARAM, LPARAM), `ret 0x10`, LRESULT en EAX | 19 rutas x 1024 = 19456 |

## Rutas (una por familia de mensajes; 1024 casos cada una, `Fixtures.hpp`)
WM_DESTROY, WM_CLOSE, WM_SIZE, WM_ACTIVATE, WM_SETFOCUS, WM_KILLFOCUS, WM_ACTIVATEAPP, WM_SETCURSOR, WM_KEYDOWN/WM_SYSKEYDOWN, WM_KEYUP/WM_SYSKEYUP, WM_MOUSEMOVE, WM_xBUTTONDOWN (3),
WM_xBUTTONUP (3), WM_MOUSEWHEEL, WM_TIMER, WM_SIZING, WM_DEVICECHANGE, WM_GRAPHNOTIFY y desconocidos (rangos bajos, tabla 0x20..0x105, 0x200..0x214, WM_USER+, 0x80000000.., nunca un mensaje manejado).
Los 24 mensajes que maneja el EXE salen de las tres tablas de salto (0x748464, 0x74848C, 0x748584) y de las comparaciones 0x113 / 0x219 / 0x40D (`original-disassembly.txt`).
Cada ruta varia: wParam/lParam (palabra baja/alta, 0, 0xFFFF0000, signos), el estado de ~25 globales del EXE (con bytes altos sucios para distinguir byte/dword), el comportamiento de los falsos (resultado de
`rsCAMERASIZE`, HRESULT de Run/Pause/GetState, que GetState escriba o no el estado, que el reproductor desaparezca a mitad, secuencia de CheckDVD/NoDiskInDriveMessage, etc.) y el FSW x87 inicial (8 variantes).

## Mensajes sinteticos y colaboradores
Los mensajes se entregan directamente a la entrada (no hay ventana de prueba: el procedimiento no necesita HWND valido porque TODA API de Win32 que llama esta sustituida). Colaboradores sustituidos en AMBOS lados
por el mismo falso trazado (`Core.hpp`): 18 funciones del juego (`EntryPatch` en la direccion del EXE y en el cuerpo de la DLL, resuelto con `Resolve()` ANTES de crear parches), 2 objetos COM falsos
(`IDirect3DDevice9::SetGammaRamp`, `IMediaControl::GetState/Run/Pause`, tabla de metodos con trampa para cualquier otro) y 13 API de Win32 por IAT (ClipCursor, PostQuitMessage, SetCursor, ShowCursor, SetCapture, ReleaseCapture,
GetWindowPlacement, SendMessageA, SetTimer, SetFocus, SetWindowPos, DefWindowProcA, OutputDebugStringA; solo el hilo principal, los demas hilos van a la API real). Por eso `WM_CLOSE`, `WM_DESTROY` (PostQuitMessage,
ClipCursor, `WinInput::Shutdown`) y el resto no pueden cerrar el juego ni cambiar su estado; las globales que el procedimiento escribe se guardan y se restauran alrededor de CADA lado y se vigilan las entradas parcheadas.
`_ftol2` (0x821B40, del CRT del EXE) y el 0x53BC60 (OnSetFocus del menu, en linea en la DLL) NO se sustituyen: sus efectos se comparan por memoria y por el estado x87 completo.

## Que se compara (en bits)
EAX (el LRESULT), FNSAVE completo tras la llamada (FCW, FSW con banderas acumuladas y C0..C3, FTW/TOP y los diez bytes de los ocho registros, VACIOS incluidos; FIP a cero y su modulo validado aparte), MXCSR (banderas), la traza
completa de colaboradores y de API con todos sus argumentos (incluido el valor de las globales en el instante de la llamada: fija el orden de las escrituras), las ventanas de memoria que el procedimiento y sus falsos pueden
escribir (9 ventanas, 440 bytes alrededor de ForegroundApp, CodePause, FEM, RsGlobal, gGameState, Windowed/RwInitialized, 0xC9246C...), la arena de lParam, ESP (`ret 0x10` balanceado) y EBX/ESI/EDI/EBP.
ECX/EDX al volver NO se exigen: los unicos llamadores son user32 y `CallWindowProcA` de `SDLWrapper` (ver `g24a-audit.txt`).

## Estado x87
`kCompareDeadX87Registers = true` (no se relaja). El unico codigo x87 es `WM_ACTIVATE` con la ventana inactiva: `_ftol2(1000.0f / RsGlobal.frameLimit)` (`fild; fdivr; call _ftol2`). La DLL usa `WndProcTimerPeriodCpp`
(anexo `g24a-x87.*.txt` de X87Math: divide a la precision vigente, levanta las banderas PE/ZE/IE que levanta el `fistp` de `_ftol2` y planta T-2 = R, T-1 = fraccion con `Ftol2RegsCpp` / `X87Plant*`).
Prueba aislada (`offline/ftol2/`): 390585 casos (45 + 30000 valores de frameLimit x 13 FSW iniciales) contra los bytes reales de `_ftol2`, FNSAVE completo y EAX: 0 diferencias.

## Evidencia previa sin juego (`offline/`)
`offline/main.cpp` + `build.bat` mapean los bytes del EXE (proceso hijo suspendido, 0x400000) y llaman a la entrada 0x747EB0 de esa copia frente a `__MainWndProc` de `WndProc.cpp` compilado con las opciones del
proyecto (Debug, /RTC1), con los mismos falsos y las mismas fixtures (`Fixtures.hpp`, `Core.hpp` compartidos con el banco del juego). Resultado al entregar (sobre el arbol con `g24a.patch` y el anexo aplicados):
**19456 pares, 0 diferencias** (todas las compuertas: traza, memoria, arena, FNSAVE, EAX, ESP, no volatiles). `mutations.py`: 36 mutaciones del cuerpo de la DLL y 4 del ayudante x87, todas detectadas salvo una que
es equivalente (`& 31` en el desplazamiento: `shl` ya enmascara el contador). Las mutaciones cubren, entre otras, SWP_NOMOVE, WM_TIMER, LOWORD(wParam), orden de ForegroundApp, letra con signo, estado inicial de GetState,
`&&` frente a `||` de WM_SIZE, hWnd frente a PSGLOBAL(window), WM_MOUSEMOVE, la llamada inicial a NoDiskInDriveMessage, CodePause al salir sin disco, RwInitialized/Windowed como dword, tablas de gamma, indicadores
de volumen, `GTATranslateKey` por AL (el EXE devuelve `mov al, 1` con basura en EAX) y cada rama de la logica de vídeo. `run_local.py --analyze offline/obj/sim.jsonl` (ejecutando `h.exe <exe> 1024 <ruta>.jsonl`) acepta ese
registro con el analizador comun, lo que comprueba el formato de filas y las compuertas sin arrancar GTA.

## Limites declarados
- Fuera del banco y sin credito: `_ftol2`, el 0x53BC60 en linea, y los colaboradores sustituidos (VideoPlayer, CPad::Clear, audio, menu, cutscene, RsEventHandler, ...), que cuentan en sus propios bancos.
- El procedimiento no se ejecuta con una ventana real: user32 es el unico llamador y el banco entrega los mensajes con los mismos argumentos de pila; el despacho de user32 (SendMessage frente a Post, reentradas) no se valida.
- Dominio legal: en `WM_ACTIVATEAPP` con GAME_STATE_PLAYING_LOGO/INTRO el EXE llama a `GetState` sin comprobar que el reproductor exista (0x7480D6), asi que las fixtures nunca lo anulan en ese estado; las comprobaciones
  de nulo posteriores (Pause/Run) si se ejercitan (el falso GetState borra el puntero). `lParam` apunta a un RECT / DEV_BROADCAST_VOLUME valido en WM_SIZING y WM_DEVICECHANGE (0x8004).
- `GetWindowPlacement` y `RsKeyCodes` se pasan sin inicializar, como en el EXE: los falsos escriben siempre lo que lee el procedimiento (showCmd, ck).
- Si otro modulo intercepta la IAT del EXE (superposiciones, otro ASI) `run_local.py` aborta (`exe_iat_matches_real`).
- Con SDL3 (`build/` actual) el EXE registra 0x747EB0 como `lpfnWndProc` (WinMain.cpp) y `SDLWrapper` lo subclasa; con `g24a.patch` el hook sustituye a ese procedimiento tambien en SDL3, de modo que el juego usa la DLL.

## Ejecucion
```
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies24A
python -B reversing/Dependencies24A/build_probe.py
python -B reversing/Dependencies24A/run_local.py
python -B reversing/Dependencies24A/run_local.py --analyze <comparisons.jsonl>
```
Sin juego (no toca el directorio build\; solo lee build\source\gta_reversed.vcxproj):
```
reversing\Dependencies24A\offline\build.bat [raiz] [gta-sa.exe] [casos]      rem 19456 pares, 0 diferencias
python reversing\Dependencies24A\offline\mutations.py                       rem tras build.bat
reversing\Dependencies24A\offline\ftol2\build.bat [raiz]                    rem helper x87 frente a _ftol2
```
Antes: aplicar `g24a.patch`, anexar `g24a-x87.h.txt` / `g24a-x87.cpp.txt` al final de `X87Math.h` / `X87Math.cpp`, anadir la fila de `g24a-hooks.csv.txt` a `docs/hooks.csv` y compilar el proyecto.
