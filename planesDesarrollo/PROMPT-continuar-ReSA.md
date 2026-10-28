<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prompt: continuar lo pendiente de ReSA en un chat nuevo

Pega el texto de la sección siguiente en una sesión nueva abierta en `D:\Original\gta-reversed-master`. Las herramientas de trabajo que antes vivían en el directorio temporal de la sesión están copiadas en `planesDesarrollo/herramientas-lote27/`.

---

## Prompt

Responde siempre en español. Continúas el trabajo de **ReSA** (`D:\Original\gta-reversed-master`, rama `main`, remoto `https://github.com/pedrokilo/ReSA.git`): reconstruir GTA San Andreas 1.0 US en C++ puro y validar cada función con bancos por pares dentro del juego (EXE con el hook apagado frente a DLL con el hook encendido, comparados en bits). Último commit al escribir esto: `57da29c4a`. Todo lo hecho está subido y el árbol está limpio salvo carpetas antiguas sin versionar.

### 1. Estado

- `docs/hooks.csv`: 8.670 funciones, todas reconstruidas y validadas. `docs/ReversedClasses.md`: funciones reconstruidas 8.670/9.187 (94,37 %) y clases completas 754/818 (92,18 %), porque las **517 funciones pendientes** (envolturas al EXE en el código fuente, funciones del juego de las dos cohortes y bibliotecas del cohorte nuevo) cuentan en los totales. La lista está en `docs/implementaciones/envolturas-pendientes.csv`, la genera `contrib/pending_wrappers.py` y la regenera `contrib/sync_docs.py`.
- Cohorte 1 (`reversing/OriginalDependencies499/`): 131 aprobadas, 124 del juego pendientes. Cohorte 2 (`reversing/OriginalDependencies2026-10-17/`): 464 destinos (185 del juego, 279 de biblioteca); el nivel «muy fácil» está hecho salvo el puente QuickTime 0x819840 (recibe un argumento en EAX: no expresable sin ensamblador, queda como envoltura). La lista por dificultad está en `lista-por-dificultad.csv` y dentro de `ReversedClasses.md`.
- Auditorías en verde: `contrib/audit_hook_catalog.py --strict --runtime "E:/JUEGOS/GTA San Andreas - CVehicle Test/scripts/runtime-hooks.csv"`, `contrib/audit_register_residue.py`, `reversing/OriginalDependencies499/audit-cohort-current.py` (0 problemas) y las 63 pruebas de `contrib/tests`.

### 2. Tarea principal pendiente: quitar las envolturas de las 41 clases parciales

El usuario eligió **reconstruirlas** (no ocultarlas de las cuentas). Son **238 funciones** en 41 clases, repartidas en 23 lotes en `planesDesarrollo/herramientas-lote27/lote27-01.csv` … `lote27-23.csv` (columnas `class, function, address, bytes, source`):

- Lotes 01–14: clases de menos a más envolturas (`CAutoPilot`, `CCustomCarPlateMgr`, `CDecision`, `CDecisionMakerTypes`, `CFileLoader`, `CObject`, `CPedSaveStructure`, `CPedStats`, `CPickups`, `CPlantMgr`, `CQuaternion`, `CRadar`, `CRealTimeShadow`, `CSprite2d`, … hasta `CVehicle` 17 y `Fx_c` 17; incluye cerrar `FxEmitter_c::CreateParticle` 0x4A2580 y las `AddParticle` incrustadas).
- Lotes 15–23: las 81 de `CCarCtrl`, por tamaño (de 746 B a 29 KB por lote; las más grandes pasan de 3.500 B).
- La ola 1 (lotes 01–05, bancos `Dependencies27A`–`27E`) se lanzó y se detuvo a medias: **no hay ningún entregable aplicado**; el trabajo parcial está en el directorio temporal de la sesión anterior y se puede ignorar. Reempieza desde cero.

Cómo hacerlo, lote a lote:
1. Lanza subagentes de **solo análisis** (no editan el repo, no ejecutan el juego, no compilan en `build\`): uno por lote, o dos por lote si son más de 12 funciones. Su brief es `planesDesarrollo/herramientas-lote27/BRIEF_21.md` (copia de `BRIEF_21.md`; léelo) más estas reglas: banco `reversing/Dependencies27X/`, prefijo de grupo `g27x`, funciones del CSV del lote. Cada subagente entrega en un directorio de trabajo: `<grupo>.patch`, anexos x87 `.cpp.txt/.h.txt`, `<grupo>-hooks.csv.txt`, `<grupo>-audit.txt`, `<grupo>-classification.md` y `<grupo>-bank/`. Pídeles verificar nombre, firma y convención con el desensamblado (los nombres y direcciones de plugin-sdk/gta-reversed upstream **fallan a menudo**) y mirar los llamadores para reproducir el residuo de registros solo si algún llamador lo lee.
2. Aplica **de uno en uno**: `git apply --check` y `git apply`, anexa los x87 al final de `source/game_sa/X87Math.cpp/.h`, añade las filas con `planesDesarrollo/herramientas-lote27/add_rows.py <archivo-de-filas>`, copia el banco a `reversing/`, compila con `powershell -File reversing/CVehicle/build-project.ps1`, ejecuta `python -B contrib/audit_register_residue.py` y corre los bancos **en serie** con `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <CarpetaEvidencia> <Banco...>` (copia antes `planesDesarrollo/herramientas-lote27/hist_wrap.py` a `%TEMP%\hist_wrap.py`: el runner lo usa). Si un banco falla, devuelve el diagnóstico real (evidencia en `comparisons.jsonl.gz` de la carpeta de corrida) al subagente y pídele un parche incremental; no relajes ninguna compuerta.
3. Cuando un lote apruebe: copia la corrida a `reversing/CppTwelveLevel6Final/runs/`, añade las funciones a `HOOKS` y `SOURCES` de `reversing/CppTwelveLevel6Study/register_hooks_nuevos.py` (hay ejemplos recientes: `reg_lote25.py` en la misma carpeta de herramientas) y ejecútalo, pon `validated=1` en `docs/hooks.csv` y regenera la documentación (sección 4).
4. Cuando una clase quede sin envolturas, pasa sola de Parciales a Completas. Revalida con sus bancos previos las clases ya validadas cuyo código cambie (los hooks nuevos pueden alterar bancos vecinos: ya ocurrió con 21G, CameraNewMode355, PlaneGeneration355, DependenciesWorld12 y CppIntegerNext/Hierarchy).

### 3. Otros pendientes (menores)

- **Bancos anidados:** `rerun_hist.py` no soporta rutas anidadas como `CppIntegerNext/Hierarchy`; se ejecutan a mano (`PYTHONPATH=reversing`, `build_probe.py`, luego `hist_wrap.py run_local.py`). Se puede arreglar el runner.
- **Hook sospechoso:** `FindSafeHouseIndexForGarageType` (0x4489F0) clobbera ECX y EDX en Debug mientras el original solo escribe EAX; `0x44A3C0` (aún en el EXE) lo llama con `xor ecx,ecx` y luego lee CX. Con el hook encendido podría contar mal. Verifícalo y, si es cierto, corrige con residuo (`ResidueRegsCpp`) y revalida su banco.
- **Dos hooks de `WinInput.cpp` con fallos reales y sin fila de catálogo:** `EnumDevicesCallback` (0x747020, invierte la lógica de éxito: un mando real no recibe formato ni nivel cooperativo) y `diPadSetRanges` (0x746D80, usa siempre `DIJOFS_Z` y marca el eje presente sin comprobar el resultado). Solo se instalan en compilaciones sin SDL3. Reconstruir también `_InputAddJoys` (0x7477C0) sería el paso natural.
- **Fallos del juego original** documentados en `source/game_sa/Conversations/BugsConversations.md` (por ejemplo el tema 7 de `CPedToPlayerConversations` es inalcanzable por `GetRandomNumberInRange(0, 7)` y la clave «sí» de `SetUpConversationNode` no se pasa a mayúsculas). Opcional: ofrecerlos como mejoras NOTSA desactivables, separadas de la reconstrucción exacta.
- `GetLightMultiplier` (0x6FFE70) no tiene llamadores en el EXE; se valida llamándola directamente (README de `reversing/Dependencies22A`).
- `reversing/BinaryCoverageAudit/summary.json` sigue con cifras del 10/10 (las regenera un script antiguo).
- Quedan sin versionar, y fuera de alcance salvo que el usuario lo pida: carpetas de estudio antiguas, `docs/Reorganizacion`, los reruns duplicados `CppLote1Runs`, `CppAuditRerun`, `CppLote2Runs`, `CppLote3Runs`, `CppUnlockRerun` (parte versionada) y `vc140.idb`.
- El `gta-reversed.ini` de la raíz de `E:\JUEGOS\GTA San Andreas - CVehicle Test` se reconstruyó tras un corte de luz copiando el de `scripts/`; los bancos lo reescriben y restauran.

### 4. Cómo se regenera la documentación y se verifica (orden exacto)

Con `PYTHONPATH=C:/Users/pedro/.codex/tmp/resa-source-inventory-parser-stable` (necesita `tree_sitter`):
1. `python -B contrib/build-memory-map.py`
2. `python -B contrib/audit_cpp_assembly.py`, `python -B contrib/inventory_implementations.py`, de nuevo `audit_cpp_assembly.py`
3. `python -B contrib/sync_validated_flags.py`
4. `python -B contrib/sync_docs.py` (regenera `envolturas-pendientes.csv`, `ReversedClasses.md` y `ProjectRoadmap.md`; **no** uses `--update-source-status` antes del último banco que registres: reescribe cabeceras de archivos fuente y cambia sus huellas)
5. `python -B contrib/sync_current_markdown.py --date <AAAA-MM-DD>` y luego `git checkout docs/ReversingPriorities.md docs/ValidationInventory.md`
6. `python -B contrib/audit_hook_catalog.py --strict --runtime "E:/JUEGOS/GTA San Andreas - CVehicle Test/scripts/runtime-hooks.csv"`, `python -B contrib/audit_register_residue.py`, `python -B reversing/OriginalDependencies499/audit-cohort-current.py` y `cd contrib/tests && PYTHONPATH="contrib;<parser>;.." python -B -m unittest discover -s . -p "test_*.py"`
Si un banco de la cohorte 1 cambia sus instrumentos o su fuente, repite su banco y ejecuta `python -B reversing/OriginalDependencies499/reregister_bank.py --rerun-root <carpeta>/runs <Banco>`.
Los errores `OSError [Errno 22]` de Windows al escribir archivos y un `.git/index.lock` huérfano (si no hay ningún `git` en ejecución, bórralo) son transitorios: reintenta.

### 5. Reglas duras (no negociables)

- Todo en C++ puro: **ningún `__asm`/`naked` en `source/`** (solo vale dentro del código de un banco bajo `reversing/`). Un `.h` y un `.cpp` por clase. Los helpers compartidos solo en `source/game_sa/X87Math.cpp/.h`. Resultados idénticos al EXE bit a bit; el x87 del EXE trabaja a 24 bits y la DLL (Debug) usa SSE: usa los patrones existentes (`X87Plant*`, `Ftol2RegsCore`, `ResidueRegsCpp`, `JpFixMemOp`).
- **Nunca relajes ni quites una compuerta de un banco**: se arregla el código o la instrumentación. `kCompareDeadX87Registers=true`. Si una comparación depende de algo no determinista del juego (p. ej. la alineación de `malloc` según las listas libres), justifícalo con el código del EXE, déjalo registrado y avisa al usuario.
- Trampas conocidas de los bancos: el juego arranca con `--unhook=Audio,Global/SurfaceInfos_c`, así que los hooks de esas categorías empiezan apagados y `Resolve(&f)` devuelve la dirección del EXE; calcula el cuerpo de la DLL (`Body()`, `HookRoute::RegisteredTarget{}`) **antes** de crear `HookRoute`, `CollabHooks` o `EntryPatch`, y restaura los parches en orden inverso. Los colaboradores que la DLL llama directamente necesitan `EntryPatch` en la dirección del EXE y en el cuerpo de la DLL. No compares EAX/ECX/EDX que ningún llamador lea. Las zonas vigiladas no deben solaparse con las modificadas, y las comprobaciones finales van antes de `Restore()`. Estudia los bancos recientes como modelo: `reversing/Dependencies22A`, `23A`, `23B`, `24A`, `24B`, `25B`, `25E`, `26A`, `UnlockFileMgr`.
- Pruebas dentro del juego **solo** en `E:\JUEGOS\GTA San Andreas - CVehicle Test`; una sola instancia del juego a la vez; no cambies `source/` mientras corre una cola de bancos; no edites `reversing/runtime_runner.py` ni los 28 documentos congelados; no toques otras tareas: **no mates procesos que no sean tuyos** (`cl.exe`, `link.exe`...). No descargues ni instales nada sin pedirlo (sí está autorizado `comtypes`/`capstone` en el entorno de herramientas del proyecto).
- No hagas `git commit` ni `git push` salvo que el usuario lo pida; los mensajes de commit **no llevan líneas de atribución** (ni `Co-Authored-By` ni «Generated with Claude Code»). Añade a Git rutas explícitas (nunca `git add -A`), sin `.pdb/.asi/.obj/.exe`.
- Mensajes y documentación en español. Reporta con honestidad: si algo falló, di cuál fue el resultado; si algo no se verificó, dilo.

### 6. Entregables y cierre

Por lote: código, banco aprobado, evidencia registrada, catálogo y documentación regenerados, auditorías en verde y (si el usuario lo pide) commit y push. Al final de cada tanda informa: funciones nuevas validadas y sus pares, cuántas clases pasaron a completas, nuevos porcentajes de `ReversedClasses.md`, y qué funciones no fueron viables sin ensamblador con su evidencia.

Empieza por el lote 27-01 y, antes de lanzar más de cinco subagentes a la vez, confirma con el usuario.
