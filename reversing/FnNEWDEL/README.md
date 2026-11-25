<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnNEWDEL: operator new/delete del CRT del EXE y rutinas internas del compilador de shaders de D3DX9 (21/11/2026)

Rama `new-delete-d3dx9` (sin subir; base `77c4422b1b`). Decisión del usuario: sin banco por función dentro del juego. Verificación: compilación, `audit_hook_catalog.py --strict`, `audit_cpp_assembly.py`, banco fuera del juego (new/delete) y prueba de humo. Sin `__asm` ni `naked`.

## 1. operator new / delete y manejador de new

Lo que hay en el EXE (MSVC 7.1, CRT estático) y su estado:

| Dirección | Función | Estado |
|---|---|---|
| 0x82119A | `operator new` | ya estaba (CRT1A); sin cambios |
| 0x821195 | `operator new[]` (`jmp 0x82119A`) | **nueva** `ExeCrt::OperatorNewArray`, gancho |
| 0x8214BD | `operator delete` (`jmp free` 0x82413F) | **nueva** `ExeCrt::OperatorDelete`, gancho |
| 0x8213AE | `operator delete[]` (`jmp 0x8214BD`) | **nueva** `ExeCrt::OperatorDeleteArray`, gancho |
| 0x826A57 | `_callnewh` | ya existía como ayudante interno; ahora `ExeCrt::CallNewHandler`, con gancho |
| 0x826A2D | `_set_new_handler` (bajo el cerrojo 4) | **nueva** `ExeCrt::SetNewHandler`, gancho |
| 0x826A51 | `_query_new_handler` | **nueva** `ExeCrt::QueryNewHandler`, gancho |
| 0x8212ED | lanza `std::bad_alloc` (`_CxxThrowException` con el ThrowInfo 0x8A1ED4) | **no reconstruida**: se sigue llamando por dirección desde `OperatorNew`. Depende de la maquinaria de excepciones del EXE (objeto estático en 0xC9ABEC, `atexit` 0x856C90 y `_CxxThrowException`); reescribirla con `throw` de la DLL cambiaría el ThrowInfo que ven los `catch` del EXE |

**No existen en este EXE:** variantes `nothrow` de `new`/`delete`, `_set_new_mode` / `_query_new_mode` (`_newmode` 0xC9ADCC solo se lee, nunca se escribe) ni `operator delete` con tamaño. Así que no hay nada que reconstruir de eso.

Cambios (todo en `source/game_sa/Crt/CrtHeap.{h,cpp}`, archivos de CRT1A; **no se ha tocado ningún otro archivo de `Crt/*`**, así que no colisiona con `crt4a-locale`; `CrtHeap.cpp` solo incluye `CrtState.h` para `ExeCrt::Lock/Unlock`):

- Opción CMake `GTASA_CRT_HEAP_HOOKS` (`${RE_PROJECT}_CRT_HEAP_HOOKS`, ON por defecto) respetada: los seis ganchos nuevos viven dentro del mismo `#ifdef GTASA_CRT_HEAP_HOOKS` de `CrtHeapInjectHooks` y solo se instalan con `__active_heap == 1`. Las funciones `ExeCrt::*` se compilan siempre.
- Coherencia del montón: `OperatorNew/Delete` se apoyan en `Malloc`/`Free`, que trabajan sobre el `_crtheap` (0xC9C2F4) del EXE; un bloque pedido por `new` del EXE, de `ExeCrt` o de `malloc` (hook `CrtMalloc` de VehicleRecording) se libera con cualquiera de los `free`/`delete` sin conversión.
- `source/game_sa/MemoryMgr.cpp`: los operadores **globales** del proyecto (los que usa todo el código con `new`/`delete`) llamaban por dirección al EXE (`plugin::Call 0x82119A/0x821195/0x8214BD/0x8213AE`). Ahora llaman directo a `ExeCrt::OperatorNew/NewArray/Delete/DeleteArray` cuando `CrtHeapIsSystemHeap()` (`__active_heap == 1`); antes de que el EXE inicialice su montón (constructores estáticos de la DLL) o si no fuese el montón del sistema caen a la llamada por dirección de siempre. Así no depende de que el gancho esté puesto (`--unhook`), y la ruta es la misma en los dos casos.

### Banco fuera del juego (`bench_newdel.cpp`, `build_bench.cmd <dir>`, `neg_run.sh`)

Mismo método que CRT1A–5A: el `gta_sa.exe` original se mapea en 0x400000 en un proceso de 32 bits y se llama por dirección; cada operación se ejecuta con el código del EXE (montón A) y con el propio (montón B). Se compara nulo/no nulo, `HeapSize` del bloque, nº de llamadas al manejador de new, si la llamada lanza (el manejador lanza un `int` al agotar reintentos, para recorrer la ruta "sin memoria" sin `std::bad_alloc`), retorno de `_set_new_handler`/`_query`/`_callnewh`, estado de 0xC9AC68, validez de los montones y bloques sin liberar al final.

- Corrida (semilla 777): 50.000 secuencias (≈305.000 de cada `new`, `new[]`, `delete`, `delete[]`, `delete(nullptr)`) y 200.000 casos de manejador: **0 diferencias** (`resultados/corrida_completa.txt`).
- Prueba negativa (7 mutaciones de una línea en `CrtHeap.cpp`): **7 de 7 detectadas** (`resultados/prueba_negativa.txt`).
- Límite del banco: la ruta "sin memoria y sin manejador" (llama a 0x8212ED y lanza `std::bad_alloc`) no se prueba: necesita la maquinaria de excepciones C++ del EXE.

## 2. Rutinas internas del compilador de shaders de D3DX9 (13)

Son las 13 de `reversing/FnD3DX9B` (Plan B) más el contexto: el juego llama a `D3DXAssembleShader` del EXE (0x7652AE, solo desde 0x760C66) con `Flags = 4`; el envoltorio de la DLL del sistema sigue **desactivado** (`{.enabled = false}`), por lo que quien ensambla es el código del EXE, ahora con estas rutinas en C++. Hallazgo nuevo: en el ensamblador del EXE la bandera 4 significa **no usar el validador de `d3d9.dll`** (`Direct3DShaderValidatorCreate9`), y solo admite las banderas 1 y 4 (`test flags, 0xFFFFFFFA` -> `D3DERR_INVALIDCALL`); eso corrobora por qué la DLL moderna lo rechazaba (para ella 4 era `SKIPOPTIMIZATION`).

Cada cuerpo sigue el desensamblado instrucción a instrucción (`__thiscall`, `this` en ECX, mismos argumentos de pila). Los colaboradores internos que no están entre las 13 (lector de la fuente 0x76B567/0x76B818/0x76B60B/0x76B7B2/0x76BA24/0x76BA49/0x76B9E0/0x76E336, fases del ensamblado 0x77196B/0x770835/0x770767/0x77107F/0x77129F/0x7715F1/0x7725E3, 0x766088, mensajes 0x76EB84/0x76EC4D, cadena de bloques 0x76E390/0x76E396/0x76E3FD/0x76E41D, objeto 0x7716DD/0x77194F, destructores 0x76B87F/0x76B4FD/0x76B5EF, HLSL 0x77EB3C y, del CRT del EXE, `setlocale` 0x825A79, `_strdup` 0x82514C y `_controlfp` 0x825136) se llaman por dirección fija, como en lotes anteriores; `operator new/delete` y `free` van directo a `ExeCrt`.

| Dirección | Función | Archivo | Estado |
|---|---|---|---|
| 0x76B89B | `D3dxShaderSource::Constructor` (guarda `setlocale(LC_NUMERIC)`, la fuerza a "C" y pone la precisión de coma flotante a 24 bits) | `D3dxShaderSource.{h,cpp}` | hecha, gancho activo |
| 0x76B940 | `D3dxShaderSource::Destructor` (restaura locale y palabra de control) | idem | hecha, gancho activo |
| 0x76DFD1 / 0x76DF03 / 0x76DF6C | `InitFromMemory` / `InitFromFile` / `InitFromResource` | idem | hechas, ganchos activos |
| 0x76E5FB | `D3dxErrorLog::ToBuffer` (texto de errores a un `ID3DXBuffer`) | `D3dxErrorLog.{h,cpp}` | hecha, gancho activo |
| 0x76E664 | `D3dxErrorLog::GetErrorCount` (`mov eax,[ecx+8]; ret`) | `D3dxErrorLog.h` | hecha **sin gancho**: son 4 bytes seguidos de otra función de 4 bytes (0x76E668); un `jmp` de 5 bytes la pisaría. Entra en `docs/JuegoExe/excluidas.csv` (`sin-espacio-para-gancho`) |
| 0x76FAEB | `D3dxShaderAssembler::Constructor` | `D3dxShaderAssembler.{h,cpp}` | hecha, gancho activo |
| 0x771F63 | `D3dxShaderAssembler::Destructor` | idem | hecha, gancho activo |
| 0x772BB1 | `D3dxShaderAssembler::Assemble` (valida banderas, ruta de versiones vs_1_x/vs_2_x/ps, validador de d3d9.dll si la bandera 4 está apagada, fases y limpieza común) | idem | hecha, gancho activo |
| 0x773022 / 0x77303B | `D3dxHlslCompiler::Constructor` / `Destructor` | `D3dxHlslCompiler.{h,cpp}` | hechas, ganchos activos (el juego no las usa) |
| 0x77EE77 | `D3dxCompileShaderEntry` (`__stdcall`, 8 argumentos; entrada de `D3DXCompileShader*`) | `D3dxHlslCompiler.cpp` | hecha, gancho activo (el juego no la usa) |

Todas con `RH_ScopedInstall`/`RH_ScopedGlobalInstall`, categoría `Plugins/D3DX9`, `validated=0`; 12 filas nuevas en `docs/hooks.csv` (clases `D3dxErrorLog`, `D3dxShaderSource`, `D3dxShaderAssembler`, `D3dxHlslCompiler` y una fila en `D3DX9`), registradas en `InjectHooksMain.cpp`. Ninguna se dejó fuera por inviable (salvo el gancho de 0x76E664, ver arriba).

Garantías y límites: son transcripciones de rutinas pequeñas y deterministas (sin coma flotante propia); la lógica pesada del ensamblado sigue siendo código del EXE, así que el resultado del ensamblado depende solo de que estas piezas de control de flujo estén bien. No hay banco por función dentro del juego (decisión del usuario) y no se montó banco fuera del juego para estas 13 porque necesitan el estado interno del lector/ensamblador del EXE (se verían solo con una fuente real y un ensamblado completo, que es justo lo que ejercita el juego al arrancar y generar los shaders de piel). La prueba de humo con el ropero (`cal_mal.args`) pasa por el ensamblador al crear los shaders de piel de CJ y peatones, y dio VEREDICTO BIEN.

## 3. Verificación

- Compilación: `_build_NEWDEL.sh` (candado `compile_lock`), EXIT=0; sin advertencias nuevas en los archivos tocados (`resultados/compilacion.txt`).
- `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 ensamblador, 0 `naked`. (No se regeneraron los bloques de estado de los fuentes con `hooks2md.py --update-source-status`: aborta en este árbol con `validated=1 (11801) no coincide con evidencia aprobada (0)` y antes de abortar reescribe cabeceras de ~50 fuentes ajenos; se descartó ese cambio.)
- Prueba de humo `prueba_humo.py` (copia de la de `FnS31AFILE` apuntando a `wv-nd\bin\Debug\gta_reversed.asi`) con `cal_mal.args`: **VEREDICTO BIEN** (puerta 2877 y marcador presentes, 0 NaN, partida sin cambios, copia de pruebas restaurada y verificada por hash). Una primera pasada se lanzó por error con la DLL de `wv-31` (ruta fija en la copia original); está descartada y la que cuenta es la segunda, con el SHA-256 de la DLL de este árbol registrado en la salida de `jugar.ps1`.
- No se pudo observar en el juego si los ganchos del montón quedaron instalados (el CSV de ganchos en ejecución de la carpeta de pruebas se restauró tras la prueba); su instalación depende de `__active_heap == 1`, igual que en CRT1A.
