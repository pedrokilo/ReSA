<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PL1: ¿qué función reconstruida provoca `Pool.h:207 ... CEntryExit`, y está detrás del fallo del usuario?

Resultado corto (2026-11-18): **no hay función culpable de ese mensaje: no es un defecto.** Detalle y evidencia en el informe del lote; aquí queda el instrumental.

## Hechos medidos (sonda de solo lectura sobre el proceso, `sonda_pool.py`)

* El pool de `CEntryExit` tiene 400 huecos en el EXE y en la DLL (constructor 0x43F720 / `CEntryExitManager::Init`, `ENTRY_EXITS_POOL_SIZE = 400`).
* Tras cargar la partida hay **376** entradas ocupadas (los `enex` de los IPL) tanto con la DLL completa (A) como con `--unhook-all` (B, código original del EXE). Al cargarse el mundo exterior suben a 384-394 y vuelven a 376 (no hay fuga). B llegó a 394 de 400; A, a 393 (y a 400 en la partida del usuario). Mismo orden de magnitud: el EXE está a 6 huecos del límite.
* El mensaje `Pool.h:207` lo escribe `CPool<T>::New()` de la DLL (solo el primer fallo por tipo). Con `--unhook-all` no se ejecuta código C++ de la DLL y por eso B siempre da 0: **el 0 de B no demuestra que el EXE no llene el pool**, solo que no hay quien lo escriba.
* No es determinista: en 9 ejecuciones automáticas con la DLL completa el mensaje salió **0 veces**; solo salió en la partida jugada a mano por el usuario.
* Sin tocar el mando/teclado, CJ se queda en el interior de su casa (pos 2495.9,-1707.4,1014.7) y el pool no se mueve; la salida real de la casa sube el pool a 384-393.

## Lo que sí queda sin resolver

El fallo del usuario (peatones/vehículos que caen a z≈-100 en torno a Grove Street) solo apareció sin intervención en 1 de 8 ejecuciones automáticas con la DLL completa (a1) y no hay señal determinista: salir de la casa por el camino del juego (`-TpExit`) o a la calle (`-Tp`) no lo provoca. La bisección por clases (`bisect_pl1.py`) necesita una señal fiable; con una tasa de ~1/8 una ejecución negativa no descarta nada.

## Herramientas

* `jugar.ps1` (copia ampliada del de SIM1; mismas garantías de copia de seguridad/restauración y candado): variantes A, B, C, D, E (como SIM1), `F` (DLL corregida, `dll\Corregida.asi` + `Corregida.sha256`; **no existe: no hay corrección que probar**) y `X` (`-AsiPath`). Opciones automáticas: `-Auto <s>` (cierra solo SU juego), `-ParaAlCaer`, `-Sonda` (pools + posición cada 5 s), `-Tp`/`-TpExit` (mueven al jugador una vez a los 25 s), `-ArgsFile` (argumentos extra para gta_sa.exe).
* `correr.sh`: espera a que no haya gta_sa.exe/candado y lanza `jugar.ps1`.
* `catalogo_rutas.py` → `rutas.json`: 1007 rutas `Categoría/Clase` para `--unhook`/`--unhook-except` (las rutas son `Categoría/Clase`, no solo la clase).
* `bisect_pl1.py`: bisección por mitades con `--unhook-except` (reanudable). `clases_dump.csv`: volcado de hooks.
