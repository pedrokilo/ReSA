<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prompt: madurar ReIV con el método de ReSA

Pega el texto de la sección siguiente en la sesión que trabajará en `D:\GTA IV\ReIV`. Está escrito para quien no conoce ReSA: todo lo que necesita saber está aquí o apunta a archivos concretos.

---

## Prompt

Responde siempre en español. Trabajas en **ReIV** (`D:\GTA IV\ReIV`), un proyecto para reconstruir GTA IV PC 1.0.7.0 (x86, `GTAIV.exe`, SHA-256 `ab21c0d90d2ca1c697785aca9dcf44635e1f8d48aa0df1d737e9190bddf0cb4c`) en C++ y validar cada función contra el ejecutable original. Tu objetivo es llevar ReIV al mismo nivel de método y de rigor que **ReSA** (`D:\Original\gta-reversed-master`, GTA San Andreas), que ya tiene 8.670 funciones reconstruidas y validadas. ReSA es tu referencia de lectura: **no modifiques ReSA**. Lee sus documentos y scripts, y copia la idea adaptándola.

### 1. Qué es ReSA y cómo funciona (léelo primero)

- **Una DLL `.asi`** que el juego carga al arrancar. Cada función del juego tiene una dirección en el EXE. La DLL escribe un `JMP` en esa dirección hacia su versión en C++ (`RH_ScopedInstall(Func, 0xADDR)`, en `source/reversiblehooks/ReversibleHooks.h`). Los hooks son **reversibles** y están agrupados por categoría: se pueden apagar desde la línea de comandos (`--unhook-all`, `--unhook=A,B`, `--unhook-except=A,B`, ver `source/extensions/CommandLine.cpp`) y con el hook apagado se ejecuta el código original del EXE. Eso permite correr la misma entrada con el hook apagado (EXE) y encendido (DLL) **en el mismo proceso** y comparar. `--dump-hooks-to=archivo.csv` vuelca los hooks registrados para contrastarlos con el catálogo.
- **Envolturas:** una función C++ cuyo cuerpo solo reenvía al EXE (`plugin::Call<0xADDR>(...)`). Compila y el juego funciona, pero **no cuenta como reconstruida**. Reconstruir es escribir la lógica real y enganchar.
- **Catálogo:** `docs/hooks.csv` (clase, función, dirección, reversed, locked, validated, tipo). `docs/ReversedClasses.md` se genera con `contrib/hooks2md.py` (clases completas / parciales / sin revertir, barras de progreso). Una clase con envolturas pendientes **no** es completa.
- **Validación:** un banco por grupo de funciones en `reversing/<Banco>/` (`LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `README.md`). Dentro del juego real se ejecuta cada función con ≥1.024 casos de estrés (NaN, ±inf, ±0, subnormales, enteros extremos) con el hook apagado y encendido, y se compara **en bits**: retorno, memoria tocada, registros, ESP, traza de colaboradores, estado x87 completo. Los colaboradores se sustituyen por falsos en el EXE y en el cuerpo de la DLL (`EntryPatch`). El runner `reversing/CppOnlyMigration/herramientas/rerun_hist.py` construye y ejecuta bancos en serie; `reversing/OriginalDependencies499/HookRoute.hpp` gestiona el apagado/encendido.
- **Registro de evidencia:** `reversing/validation-registry.json` y `docs/hooks.csv` (`validated=1` solo con evidencia: `validation-summary.json` con `passed` y `all_equal`). Auditores en `contrib/` (`audit_hook_catalog.py --strict`, `audit_register_residue.py`, `audit_cpp_assembly.py`). El flujo de documentación: `contrib/build-memory-map.py`, `inventory_implementations.py`, `sync_validated_flags.py`, `sync_docs.py`, `sync_current_markdown.py`.
- **Reglas duras de ReSA:** todo en C++ puro (sin `__asm`/`naked` en `source/`; el ensamblador solo vale dentro del código de un banco); un `.h` y un `.cpp` por clase; helpers compartidos en un único archivo (`source/game_sa/X87Math.cpp/.h`); resultados idénticos al EXE bit a bit; nunca relajar ni quitar una compuerta de un banco (se arregla el código o la instrumentación del banco); ningún cambio de `source/` mientras corre una cola de bancos; una sola instancia del juego a la vez; en la copia de pruebas, nunca en la instalación original.

### 2. Estado actual de ReIV (comprobado el 18/10/2026 leyendo el repo; verifícalo tú)

- Base: `ReIV.asi` con **IV-SDK** (`contrib/IV SDK`, 120 cabeceras) y su `injector`; plugin-sdk (carpeta IV), IV-SDK.Net y patrones de bytes sirven para contrastar nombres y direcciones. **El EXE canónico decide cuando haya discrepancias.** Las direcciones del SDK son RVA (base real + RVA, no fijar `0x00400000`).
- 4.016 tipos RTTI con su cabecera generada en `source/game` y `source/rage` (`source/class-files.csv`), ~45.500 ranuras virtuales registradas como "pending hooks", `docs/MemoryMap` con 34.141 direcciones de código. Generador: `reversing/MemoryMap/generate-class-headers.py`.
- `docs/hooks.csv`: 15.995 entradas (destinos del EXE: llamadas al original y ranuras virtuales); solo **43 reconstruidas** (42 de `CPlayerPed` + `rage::atStringHash`), 0/4.016 clases completas, 1 parcial. `tools/ReIV.Tools` (C#) genera `docs/ReversedClasses.md`.
- Capa de llamadas al original: `source/core/OriginalCall.h/.cpp` (comprueba hash del EXE y bytes esperados por RVA). Bancos actuales: `reversing/AtStringHash`, `CPlayerPed`, `OriginalCalls`, `*Validation`. La validación es por función y semimanual.
- Consola de depuración propia (`core/DebugLog`). No he visto equivalente a hooks por categoría con apagado desde línea de comandos.
- Pendiente de lenguaje: 3 archivos con ensamblador en línea (59 bloques `__asm`, 20 `naked`) en `CPlayerPed.h/.cpp` y `hooks/PlayerPedProbe.cpp`.

### 3. Qué hay que madurar (en este orden)

**Fase A. Auditoría y línea base.** Antes de cambiar nada, comprueba que lo que dice el README es cierto: compila con `contrib/build-reiv.ps1`, verifica el hash del EXE, ejecuta lo existente en la copia de pruebas y anota cuántos bancos aprueban hoy. Entrega un informe breve con lo que funciona y lo que no. Pregúntame solo si algo bloquea de verdad.

**Fase B. Mecanismo de hooks reversibles y categorías.** Equivalente a `ReversibleHooks` de ReSA, sobre el `injector` de IV-SDK o con una librería de hooks propia: registro por clase/categoría, interruptor por función, trampolín al original, volcado `--dump-hooks-to` y argumentos de línea de comandos para apagar hooks. Sin esto no hay comparación EXE/DLL en el mismo proceso. Documenta las trampas: RVA relativos a la base real, que los hooks de código en `.text` necesitan las protecciones correctas, y que llamar al destino enganchado desde el propio hook provoca recursión.

**Fase C. Ranuras virtuales.** Son ~45.500. No se enganchan igual que una función estática: decide y documenta el mecanismo (parchear la vtable en memoria con un trampolín por ranura, o hook del cuerpo). Las ranuras compartidas entre clases (misma función en varias vtables) deben contarse una sola vez como cuerpo y varias como ranuras. Define cómo se cataloga cada una.

**Fase D. Banco por pares.** Porta a ReIV la maquinaria de ReSA: `LiveComparison.hpp` plantilla, `HookRoute` (con `RegisteredTarget`/`Body()` para resolver el cuerpo de la DLL **antes** de apagar nada), colaboradores falsos con `EntryPatch` en el EXE y en el cuerpo de la DLL (restaurados en orden inverso), `kCompareDeadX87Registers`, comparación de FNSAVE completo, restauración de todo el estado global, runner en serie y registro de evidencia. Empieza con `rage::atStringHash` y una función pequeña de `CPlayerPed` y exige ≥1.024 pares con casos de estrés por función. Lee los bancos recientes de ReSA como modelo: `reversing/Dependencies22A`, `23A`, `23B`, `24A`, `25B`, `25E`, `26A` y `UnlockFileMgr`.

**Fase E. Catálogo y porcentajes honestos.** Una "envoltura" (cuerpo que reenvía al EXE, `OriginalCall`) **no es una función reconstruida**, pero sí cuenta como pendiente. Replica el modelo de ReSA: `hooks.csv` con estado por entrada, `ReversedClasses.md` con completas/parciales/sin revertir, las funciones pendientes cuentan en los porcentajes de funciones y de clases, y una clase solo es completa con **inventario exhaustivo de métodos** acreditado (hoy 0 clases lo tienen). Define cómo obtener ese inventario para una clase: RTTI + vtable + llamadas directas desde el código desensamblado + referencias de IV-SDK; documenta qué falta cuando no se puede.

**Fase F. Ensamblador a C++.** Reescribe los 59 bloques `__asm` y las 20 funciones `naked` de `CPlayerPed` en C++ equivalente y repite sus bancos. Si el compilador no reproduce el residuo (x87, registros muertos), no uses ensamblador en `source/`: usa los patrones de ReSA (helpers en un único `X87Math.cpp/.h`, `ResidueRegsCpp`, plantado de registros muertos) o documenta con evidencia que no es viable.

**Fase G. Primera clase completa.** Lleva `CPlayerPed` de punta a punta: inventario de métodos, reconstrucción de lo que falta, banco para cada función, evidencia y catálogo. Ese será el modelo para el resto.

### 4. Reglas de trabajo

- Cada dirección, offset o firma debe corresponder al EXE canónico (hash). Contrasta siempre IV-SDK / plugin-sdk / IV-SDK.Net con el desensamblado: **los nombres y direcciones de los SDK a veces están mal**; no los copies sin comprobar. Documenta cada discrepancia.
- Nada se marca `validated` sin banco en el juego real aprobado y registrado; un wrapper no cuenta como reconstruido.
- No relajes ni elimines compuertas de un banco para que pase; arregla el código o la instrumentación. Si dudas de una comparación (por ejemplo la alineación de `malloc` depende del montón), justifícala con el código del EXE y déjala registrada, no ignorada.
- Pruebas solo en una copia de GTA IV (`D:\GTA IV\Grand Theft Auto IV - copia` u otra), nunca en la instalación original. Haz copia y restauración de configuración y partidas; no abras dos instancias; no mates procesos que no sean tuyos.
- No descargues ni instales nada sin pedírmelo. No hagas `git commit`/`push` salvo que te lo pida, y nunca añadas líneas de atribución en commits o PR.
- Cuando generes documentación con scripts, regenera siempre con los scripts, no a mano. Los errores `OSError [Errno 22]` de Windows al escribir archivos son transitorios: reintenta.
- Lee `D:\Original\gta-reversed-master\reversing\CppOnlyMigration\PROCESO.md` y `reversing\_handoff\README.md` para el proceso detallado.

### 5. Entregables y criterios de éxito

Por fase: código, bancos, evidencia vinculada, documentación regenerada y un informe corto en español con (a) qué hiciste, (b) qué comprobaste y cómo, (c) qué quedó pendiente y por qué. Éxito de esta etapa: hooks reversibles con categorías funcionando; el banco por pares portado y aprobando para `atStringHash` y varias funciones de `CPlayerPed`; `ReversedClasses.md` con porcentajes honestos (envolturas = pendientes); cero `__asm` en `source/`; y `CPlayerPed` como primera clase completa con inventario exhaustivo.

Empieza por la Fase A y dime el resultado antes de pasar a la B.
