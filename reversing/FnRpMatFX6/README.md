<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnRpMatFX6: las 6 funciones «RpMatFX» que faltaban eran QuickTime (QTML), reconstruidas en C++ (21/11/2026)

Rama `rpmatfx6` (base origin/main 19b3624623). Nada subido a origin ni a main.

## Identificacion (el mapa se equivocaba: no son de RpMatFX)

La clasificacion por vecindad (confianza baja) las atribuia a RpMatFX. El desensamblado y las cadenas del EXE dicen otra cosa: son el arranque y la parada de la biblioteca
estatica **QTML (QuickTime Media Layer)** enlazada en el EXE. No hay fuentes RenderWare/librw equivalentes (es codigo de Apple); se identifico por las cadenas
`QuickTime.qts`, `theQuickTimeDispatcher`, `_CallComponent`, `_CallComponentFunctionWithStorage`, `QTMLInitTermMutex%lx`, `Software\Apple Computer, Inc.\QuickTime`, `QuickTime.qts folder`
y por la IAT (GetProcAddress, CreateMutexA, RegOpenKeyExA...).

| Direccion | Bytes | Nombre | Que hace |
|---|---|---|---|
| 0x8194B0 | 5 | QtmlInitializeThunk | `jmp 0x818650`: puente de QuickTime con id 0x1D0008 = InitializeQTML(flags) |
| 0x8194C0 | 5 | QtmlTerminateThunk | `jmp 0x818670`: puente con id 0x1D0009 = TerminateQTML() |
| 0x819520 | 148 | QtmlResolveEntryPoints | gancho 0x8E2A08 (que ejecutan los puentes antes de saltar): carga QuickTime.qts si hace falta y resuelve los 3 puntos de entrada; si la cargo ella, la libera (por eso «32 llamadas»: la ejecutan los 31 puentes por puntero) |
| 0x8195C0 | 319 | QtmlInitialize | InitializeQTML con cuenta de referencias (0xC9ABD8) y mutex del proceso; errores -2093/-2094 |
| 0x819770 | 168 | QtmlTerminate | TerminateQTML con el mismo mutex |
| 0x81DAE0 | 911 | QtmlLoadLibrary | busca y carga una DLL: modulo ya cargado, carpeta del EXE, registro HKLM/HKCU de QuickTime, directorio del sistema |

Estado global: 0xC9ABD8 contador, 0xC9ABDC HMODULE, 0x8E2A08 gancho, 0x8E2A0C/10/14 punteros del despachador (stub 0x8194D0 = `mov ax,0F7D1h; ret` si no hay QuickTime).

## Que se hizo

Las 6 funciones ya existieron (lote R10A, `QTMLRuntime`) y se retiraron en EXT1A (99d9745bf7) junto con el uso de QuickTime. Se restauran desde `99d9745bf7^` **tras revisarlas de nuevo
contra el desensamblado** (orden de GetProcAddress, rutas de error, limpieza, orden de la tabla de registro, lectura de 0xC9ABDC tras TerminateQTML...) y se re-enganchan:

- `source/app/platform/win/QTMLRuntime.cpp` / `.h` (dos archivos, C++ puro, sin `__asm`/`naked`; los puentes del EXE se llaman por su direccion como funciones cdecl).
- `source/InjectHooksMain.cpp`: include y `QTMLRuntimeInjectHooks()`.
- `docs/hooks.csv`: 6 filas `QTMLRuntime,...,reversed=1,locked=0,validated=0`.
- `CAEMFDecoder` NO cambia: sigue sin usar QuickTime (`InitLibrary` devuelve false, los .mp3 van a WMA). Los hooks solo evitan que el EXE ejecute su codigo de QTML si algo lo llamase.
- Ninguna es inexpresable: `no-reconstruibles.csv` no cambia.

## Banco fuera del juego (barato): `bench.cpp` + `build.cmd`

Proceso de 32 bits con el EXE original mapeado en 0x400000 (como FnLIB4A). Las 16 funciones de KERNEL32/ADVAPI32/USER32 se sustituyen por maquetas deterministas con registro de llamadas
(la IAT del EXE apunta a ellas; el C++ las llama por macros de nombre, compilando el `QTMLRuntime.cpp` real sin cambios con un `StdInc.h` minimo en `shim/`). Por escenario aleatorio
(estado inicial de los 6 globales, argumentos, respuestas de las maquetas) se ejecuta primero la funcion del EXE y luego la del C++ y se comparan: secuencia completa de llamadas con argumentos,
AX/EAX devuelto y los 6 globales.

- `resultados/corrida.txt`: 6 funciones x 20 000 escenarios (1,73 M eventos de maqueta): **0 diferencias**.
- `resultados/negativa.txt`: control negativo (el lado C++ invierte HKLM/HKCU): 4285 diferencias detectadas en 5000 x 6.
- Limites: con una cadena de registro vacia el EXE lee un byte anterior al bufer (indefinido; no se prueba); las maquetas de GetProcAddress no fallan en los puentes (el EXE saltaria a 0).

Compilar y correr: `build.cmd <salida>` y `bench.exe <gta_sa.exe original> [N] [neg]`.

## Compilacion, auditorias y humo

Ver la seccion final «Resultados» de este archivo.

## Resultados

- Compilacion (`_build_RPMATFX6.sh`, candado, build desde cero): `EXIT=0`, `gta_reversed.asi` generado.
- `python -B contrib/audit_hook_catalog.py --strict`: OK. `python -B contrib/audit_cpp_assembly.py`: 0 archivos con ensamblador o naked.
- Humo (DLL propia, `cal_mal.args`): `prueba_ropero.py` VEREDICTO BIEN (puerta True, marcador True); `prueba_agua.py` VEREDICTO BIEN (UnderWaterness 1.0, brillo medio 49.9).
- Mapa regenerado (`contrib/rw_function_map.py`, `contrib/rw_plan.py`): las 6 pasan a `reemplazada`; el plan queda con 0 lotes. Se quito de `docs/implementaciones/envolturas-pendientes.csv`
  la fila obsoleta de 0x819770 (apuntaba a una llamada de AEMFDecoder.cpp que EXT1A ya elimino).
- Sin banco por funcion dentro del juego (decision del usuario); filas `validated=0`.
