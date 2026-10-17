<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Proceso de reconstrucción en C++ (política vigente)

Vigente desde el 10/10/2026 por orden del propietario del proyecto. Resume las reglas, el flujo de cada lote y las técnicas que ya se probaron. Estado y cifras: [README.md](README.md). Revisión de lo anterior: [AUDITORIA_ARCHIVOS_AUXILIARES.md](AUDITORIA_ARCHIVOS_AUXILIARES.md). Plan: [PLAN_POR_LOTES.md](PLAN_POR_LOTES.md).

## Política

1. **Todo lo que se reconstruya se escribe en C++.** No se añade `__asm`, `_asm` ni `__declspec(naked)` en `source/game_sa`. El ensamblador existente se convierte a C++ conservando los resultados exactos del EXE.
2. **Una clase = dos archivos: un `.cpp` y un `.h`.** Nada de archivos auxiliares con sufijo `Cpp`, ni de métodos de una clase definidos en otra, ni de helpers sueltos por conversión.
3. **Sin duplicados.** No se copian funciones, variables globales con sus direcciones, declaraciones de `sin`/`cos`/`sqrt` ni listas de includes. Si algo lo usan varias clases (por ejemplo helpers trigonométricos x87), vive en un único módulo compartido: hoy `X87Math.cpp/.h`, y las clases lo usan; no lo copian. Una función suelta de una clase (o de su alias de dirección) que deba coincidir byte a byte con el EXE vive en el `.cpp` de esa clase.
4. **Resultados exactos del EXE, sin tolerancias añadidas.** No se mueve ensamblador a otro archivo de producción, no se esconden opcodes y no se delega el cuerpo al EXE para aparentar una conversión.
5. **Las opciones de compilación son por archivo, nunca globales.** Un archivo de clase puede compilarse con `/O2 /Oi /arch:IA32 /fp:fast /Zi` sin PCH ni unity cuando la clase necesita x87 exacto; entonces se revalida toda la clase, no sólo la función convertida.
6. **El 100 % del catálogo no es el 100 % en C++.** Catálogo, lenguaje e independencia del EXE se miden por separado. Nunca se declara «100 %» mezclándolos.
7. **Excepción documentada (16/10/2026).** `source/app/platform/win/WindowedMode.cpp` conserva `__asm` en `HookDirect3DDeviceReplacerSA`: es una extensión NOTSA (modo ventana), no código reconstruido del EXE, y está fuera de `source/game_sa`. Decisión del propietario (opción A): no se convierte y no cuenta en [FILES.md](FILES.md). Cualquier otro `__asm` o `naked` en `source/game_sa` sigue prohibido.

### Qué decían los documentos anteriores

`reversing/_handoff/BRIEF.md`, `WAVE_FINAL.md` y `CODEX_PROMPT.md` indicaban replicar con `__asm` el x87 de precisión completa (fsin, fcos, fpatan…) y usar envoltorios `naked` cuando el EXE pasa argumentos en registros o conserva EDX. Eso era correcto mientras el objetivo era validar y reconstruir; **queda superado por esta política**. Los tres documentos llevan ahora un aviso que apunta a este archivo.

## Flujo de cada lote

1. **Elegir** funciones de una misma clase o familia (ver el plan). Comprobar quién las llama y qué banco existe para esos llamadores; un banco histórico no basta, hay que derivar uno nuevo.
2. **Guardar el objeto «antes»** (`build/.../*.obj`) con su SHA256, sin publicarlo.
3. **Escribir el C++** en el `.cpp` de la clase. Opciones por archivo en `source/CMakeLists.txt` si hacen falta.
4. **Compilar el proyecto real** (`reversing/CVehicle/build-project.ps1`, con `CMAKE_BUILD_PARALLEL_LEVEL=2` y `_CL_=-MP2`) y **leer el objeto real**, no un estudio aislado: instrucciones, relocaciones, registros que escribe, orden de las operaciones x87.
5. **Comparar con el EXE** (bytes de la función en la dirección original) y con el objeto anterior (`reversing/CppTrigCoffAudit/compare_coff.py`) cuando el cuerpo de los consumidores debe ser idéntico salvo el destino de la llamada.
6. **Revisión independiente** (un subagente por lote, con el código y no con la conclusión) y **banco derivado** escrito por subagente sin compilar.
7. **Compilar la sonda y correr el banco**, siempre en serie, sólo en el clon `E:\JUEGOS\GTA San Andreas - CVehicle Test`. El coordinador es el único que compila y ejecuta GTA.
8. **Auditar** la corrida con el `audit_run.py` del banco (`--archive`): pares, cero diferencias, ningún registro nulo, estado, partidas y configuración restaurados, hashes de fuentes.
9. **Documentar una vez por lote:** `approvals.json`, `python -B contrib/audit_cpp_assembly.py`, `contrib/inventory_implementations.py`, textos de estado, y la evidencia del lote en su carpeta de estudio.
10. **Verificar** con `python -B contrib/audit_hook_catalog.py --strict` y las pruebas de `contrib/tests/test_inventory_implementations.py` antes de publicar. Nada se sube a git sin que se pida.
    - Si el lote usa el residuo de registros, ejecutar también `python -B contrib/audit_register_residue.py` (y añadir el objeto nuevo a `OBJECTS`) y sus pruebas en `contrib/tests/`.
    - `contrib/inventory_implementations.py` necesita `tree-sitter` y `tree-sitter-cpp` (`contrib/requirements-implementations.txt`); sin ellos falla con `ModuleNotFoundError`. En esta máquina están en `C:\Users\pedro\.codex\tmp\resa-source-inventory-parser-stable`: ejecutar con `PYTHONPATH` apuntando ahí.

## Técnicas comprobadas

- **x87 exacto.** Compilar el archivo con `/O2 /Oi /arch:IA32 /fp:fast`. Con las opciones por defecto del proyecto (`/Od /ZI /RTC1`, SSE) cada `float` se guarda en memoria y `sqrt` llama a `__CIsqrt`. `#pragma optimize` dentro de un archivo Debug **no** sirve con `/ZI` y `/RTC1`: el compilador lo ignora.
- **Orden de operaciones.** Escribir las sumas en el orden del EXE (`(dz*dz + dy*dy) + dx*dx`); el compilador lo conserva. Comparaciones ordenadas dan `false` con NaN, igual que `FCOMP + FNSTSW + TEST AH,41h`.
- **`/fp:fast` y `sqrt`.** Insertado en línea, `/fp:fast` sustituyó `sqrt(s) > 4` por `s > 16` y eliminó el `FSQRT`. Las funciones que comparan una raíz van `static __declspec(noinline)`.
- **Envoltorios `naked` que sólo conservan EDX.** Se sustituyen por el hook directo si la función vive en un archivo compilado con `/O2` y el objeto real sólo escribe EAX, ECX y ESI (comprobarlo decodificando los bytes). Los índices de arrays van con `.data()[i]`: `array::operator[]` añade un assert cuya ruta llama a `__CrtDbgReport` y clobberaría EDX.
- **Clases con `naked` (transcripciones del EXE).** No se compilan completas con `/O2`: esas funciones escriben EBX/ESI/EDI sin guardarlos y los llamadores optimizados pierden variables guardadas en esos registros. El archivo de la clase se compila con `/Zi` (sin `/ZI`) y `BasicRuntimeChecks=Default`, y sólo la función que necesita código optimizado lleva `#pragma optimize("gt", on)` (y `#pragma float_control(precise, off, push)` si es x87) y `#pragma optimize("", on)` al terminar. Sin `std::bit_cast` ni otras plantillas dentro de esa función: sin `/Ob2` quedan como llamadas. Ejemplos: `Interior_c.cpp`, `CarCtrl.cpp`.
- **Producto escalar y retorno de `float`.** Con `/fp:fast` el compilador reasocia las sumas; se calculan en una función `static noinline` con `#pragma float_control(precise, on)`. Si devuelve `float`, el compilador lo guarda como float al retornar y un valor diminuto pasa a cero: devolver `double` conserva el signo. El resto de la función sigue en `/fp:fast` para que `fsqrt`, `fpatan` y `fsin` salgan en línea (con `precise` salen como llamadas a la CRT).
- **Palabra de control x87.** No hay intrínseco para `fldcw`: `_control87((anterior & _MCW_PC) | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM)` y su restauración reproducen `(anterior & 0x300) | 0x7F` del EXE.
- **Restos de registros que el banco exige.** Si el EXE deja EAX y EDX con valores que los bancos comparan (`CAttractorScanner::Clear` termina con `EAX = this+0x90` y `EDX = 10`), la función devuelve esos restos como `uint64` (EDX:EAX); el valor lo ignoran los llamadores en C++. ECX se conserva comprobando en el objeto que no se escribe.
- **Orden de evaluación de argumentos.** `f(a(), b())` evalúa `b()` antes que `a()` en MSVC x86; si el EXE llama primero a `a`, hay que leer los valores en sentencias separadas (los bancos comparan la secuencia de llamadas).
- **Helpers que devuelven float redondeado.** Si el original guarda el resultado con `fstp` y lo vuelve a cargar, el helper usa `volatile float` y lo devuelve: sin eso el compilador puede dejar el valor sin redondear en ST0, que sólo coincide con la precisión de 24 bits.
- **Residuo de registros del EXE (ECX/EDX/EAX).** Si un llamador del EXE o el banco exige los registros que deja una función, C++ puede reproducirlos terminando la función con una llamada a un ayudante trivial de `X87Math` (`ResidueRegsCpp`, `__fastcall`, o `ResidueEcx::Get`, `__thiscall`) que recibe los valores por la convención de llamada y no toca registros; el EDX de entrada se obtiene declarando la función `__fastcall`. Hay que desactivar `/RTC` en esa función (`#pragma runtime_checks("", off)`), no usar `__try/__finally` del compilador (su epílogo cambia ECX) y ejecutar `python contrib/audit_register_residue.py` tras cualquier cambio de opciones. Ver `reversing/CppFourNakedStudy/README.md`.
- **Argumentos en EAX o bits altos de EAX del EXE.** `EntryEaxCpp` (`X87Math`) devuelve el EAX de entrada si la función lo llama antes de ejecutar nada que escriba EAX (sin comprobaciones de ejecución ni /GS). Comportamiento sin definir vigilado por la comprobación de bytes de la carga y por `audit_register_residue.py`. Ver [CppFourteenFilesStudy](../CppFourteenFilesStudy/README.md).
- **Archivos sin PCH.** Basta `#include "StdInc.h"` entre `#pragma warning(push, 0)` y `pop`. Para eso se corrigieron en `Pools.h` (`PoolOf<T>`) y `CompressedVector.h` funciones definidas en cabecera sin `inline`, que duplicaban símbolos fuera del PCH.
- **Contrato de registros.** Los bancos estrictos siembran ECX y EDX, capturan EAX, ECX, EDX, ESI, EDI, EBX, EBP, ESP y las banderas definidas justo después de la llamada, y exigen la misma salida que el EXE.
- **Colaboradores.** Las llamadas a otras funciones quedan identificadas en el registro; probar un consumidor no valida a sus colaboradores.

## Herramientas

En [herramientas/](herramientas/): `rerun_banks.py` (bancos con auditor propio), `rerun_int.py` (bancos de enteros), `rerun_hist.py` y `hist_wrap.py` (bancos históricos: redirigen el juego al clon, reapuntan `PROBE_HASH` y restauran la evidencia que reescriben) y `audit_generico.py` (auditor para bancos sin script propio).

## Trampas

- No tocar `E:\JUEGOS\GTA San Andreas`; el clon de pruebas es `E:\JUEGOS\GTA San Andreas - CVehicle Test` y el ejecutable canónico, `D:\Grand Theft Auto San Andreas\gta-sa.exe` (SHA256 `72ae59e4…ecc9b`).
- No editar `reversing/runtime_runner.py` ni los 28 documentos congelados.
- Cada sonda deja un `.pdb` de ~1,3 GB en `_baseline` del clon. Limpiar con `liberar-espacio-clon.ps1` cuando haga falta; se ejecuta a mano.
- Un banco aprobado con un archivo no vale para otro: si se renombra o se mueve una fuente, se repite el banco.
- No retirar gates de un banco para que pase. Separar el ruido del banco de las diferencias reales de la fuente.
- No matar procesos `gta_sa.exe` sin comprobar a qué banco pertenecen (sello de tiempo del `.asi` en `scripts\` del clon): un banco largo como `PowEquivalence` (unos 40 minutos de CPU) parece colgado y se mató por error el 16/10/2026. Tras un «GTA no responde» hay que buscar procesos huérfanos antes de lanzar el siguiente banco.
- Un arreglo global de `OriginalDependencies499/HookRoute.hpp` pisó el cuerpo C++ explícito del banco de Garage (`DependenciesWorld4`, `CppIntegerWorld4`): los arreglos de un banco se hacen locales al banco.
- Los bancos de ayudantes x87 (`Cpp*X87Stress1`) comparan cada ayudante de `X87Math` con la secuencia exacta del EXE en bits, con ESP, no volátiles, pila y palabra de control x87 y `errno`; sirven de evidencia por ayudante cuando los consumidores no tienen banco propio.
- Calcular `Body(dll)` (el cuerpo real de la función de la DLL) antes de crear los parches de entrada de un banco: el parche del falso se escribe sobre el cuerpo ya definitivo y, con `--unhook Audio`, un falso mal ordenado se autoparcheaba (bancos `Dependencies21A` a `Dependencies21G`, 17/10/2026).
- No comparar EAX ni el residuo de registros que ningún llamador lee: se desensamblan los llamadores antes de decidir y, si nadie los lee, ni el banco los exige ni la función los reproduce.
- Etiquetar los punteros de pila del llamador (por ejemplo `local`, relativos a la arena) en lugar de compararlos como números: cambian de una pasada a otra.
- Fijar el estado inicial de la memoria compartida (baseline) antes de la pasada original y de la candidata; si no, la segunda hereda restos de la primera. Las zonas vigiladas no deben solaparse con las modificables y el estado final se comprueba antes de `Restore`.
- El árbol queda con `approvals.json` válido: se escribe siempre con rutas con barras normales (`/`), nunca con barras invertidas, que JSON interpreta como escapes y lo dejan inválido.
