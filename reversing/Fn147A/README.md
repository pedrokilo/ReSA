<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 879 grupos / 9814 funciones reconstruidas; 9814 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn147A: 26 funciones diminutas del EXE (lote 147A)

Estado: **APROBADO dentro del juego** (09/11/2026, worktree `wv-2` sobre main 9969ef56cd + los cambios del lote; el encargo decia `wv-3`, ocupado, igual que `wv-1`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn147a/20261109-201222-630041` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn147A-20261109-201222-630041`; `comparisons.jsonl.gz` de 51,7 MB, bajo el limite de GitHub). 26 x 16384 = **425984 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `7b16d3288e06eb4a11b0803344673502ccdc291e4960fd1368474d8a59b31ae2`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 26 direcciones tenia hook (se comprobo el codigo, `docs/hooks.csv` y `git worktree list`). Las clases del listado se atribuian por cercania y **la mayoria eran incorrectas**: los nombres reales salen del desensamblado, de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CEventGroup` cuerpo del destructor (CEventGlobalGroup, A) | 0x4AB980 | 14 | `Events/EventGroup.cpp` | S |
| 1 | `CEventGlobalGroup` cuerpo del destructor (CEventGlobalGroup, C; solo lo llama el destructor con borrado 0x4ABAA0) | 0x4AB9B0 | 14 | `Events/EventGlobalGroup.cpp` | S |
| 2 | `CEventGroup` destructor con borrado (ranura 0 de 0x85AAB0; cuerpo en linea) | 0x4ABA20 | 35 | `Events/EventGroup.cpp` | S |
| 3 | `size()` de un vector de elementos de 20 bytes, disposicion STL de MSVC 6 como `SArray` (CPedShelterAttractor, A) | 0x5E9800 | 33 | `Attractors/PedShelterAttractor.cpp` | S |
| 4 | `size()` de un vector de elementos de 12 bytes (`SArray<CVector>`) (CPedShelterAttractor, A) | 0x5E9850 | 32 | idem | S |
| 5 | `CTaskComplexGoToPointShooting` cuerpo del destructor: `jmp 0x668870` (GoToPointAiming, C) | 0x66DFA0 | 5 | `Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp` | S |
| 6 | `CTaskComplexGoToPointShooting` destructor con borrado (vtable 0x86FE2C; GoToPointAiming, B) | 0x66DF80 | 30 | idem | S |
| 7 | `CTaskComplexGoToPointAndStandStillAndAchieveHeading` destructor con borrado (vtable 0x86FE58; GoToPointAiming, B) | 0x66DFB0 | 30 | `.../TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp` | S |
| 8 | `CTaskSimpleFinishBrain::Constructor` (CTaskComplexSequence, A) | 0x463640 | 18 | `.../TaskSimpleFinishBrain.cpp` | S |
| 9 | `CTaskSimpleWaitUntilLeaderAreaCodesMatch` cuerpo del destructor (vtable 0x86C7E8 + `jmp 0x6355E0`; Sequence, C) | 0x5F67E0 | 11 | `.../TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp` | S |
| 10 | `CTaskComplexSequence::GetTaskType` (244; ranura 4) | 0x632C60 | 6 | `Tasks/TaskComplexSequence.cpp` (+ `.h`) | V |
| 11 | `CTaskComplexBeInGroup` destructor con borrado (vtable 0x86E22C; Sequence, B) | 0x638A80 | 30 | `.../TaskComplexBeInGroup.cpp` | S |
| 12 | `CTaskComplexUseSequence::GetTaskType` (275; ranura 4) | 0x635490 | 6 | `Tasks/TaskComplexUseSequence.cpp` (+ `.h`) | V |
| 13 | `CTaskSimpleWaitUntilAreaCodesMatch` destructor con borrado (vtable 0x86E544; UseSequence, B) | 0x6397A0 | 30 | `.../TaskSimpleWaitUntilAreaCodesMatch.cpp` | S |
| 14 | `CTaskComplexUseScriptedBrain` destructor con borrado (vtable 0x86E568; UseSequence, B) | 0x6397C0 | 30 | `.../TaskComplexUseScriptedBrain.cpp` | S |
| 15 | salto `jmp 0x538900` = `CFileMgr::OpenFile` (cdecl; FxSystem_c, A) | 0x4AB240 | 5 | `Fx/FxSystem.cpp` | S |
| 16 | salto `jmp 0x5389B0` = `CFileMgr::ReadLine` (cdecl; valor bool) | 0x4AB250 | 5 | idem | S |
| 17 | salto `jmp 0x5389D0` = `CFileMgr::CloseFile` (cdecl) | 0x4AB260 | 5 | idem | S |
| 18 | `FxPrtMult_c::SetUp` (7 dwords de pila, `ret 1Ch`; FxSystem_c, A) | 0x4AB2D0 | 51 | `Fx/FxPrtMult.cpp` | S |
| 19 | `CTaskComplexDiveFromAttachedEntityAndGetUp` destructor con borrado (vtable 0x85A358; B) | 0x492EF0 | 30 | `.../TaskComplexDiveFromAttachedEntityAndGetUp.cpp` | S |
| 20 | `CEntitySeekPosCalculator` segunda copia del cuerpo del destructor (`mov [ecx],0x859DC4`; Dive, C) | 0x492F60 | 7 | `.../SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp` (fila `DestructorCopy`) | S |
| 21 | `CTaskComplexWalkRoundFire` destructor con borrado (vtable 0x86F3E8; Dive, B) | 0x6579F0 | 30 | `.../TaskComplexWalkRoundFire.cpp` | S |
| 22 | `CTaskSimpleRunAnim::GetTaskType` (`movzx eax,word [ecx+1Ch]`; comparten esta copia 15 vtables de tareas derivadas; Cower, B) | 0x48DEA0 | 5 | `.../TaskSimpleRunAnim.cpp` (+ `.h`) | V |
| 23 | `CTaskSimpleScratchHead` cuerpo del destructor: `jmp 0x61A6F0` (Cower, C) | 0x48E930 | 5 | `.../TaskSimpleScratchHead.cpp` | S |
| 24 | `CTaskSimpleScratchHead` destructor con borrado (vtable 0x85A100; Cower, B) | 0x48E910 | 30 | idem | S |
| 25 | `CTaskSimpleUseAtm` destructor con borrado (vtable 0x85A134; Cower, B) | 0x48E940 | 30 | `.../TaskSimpleUseAtm.cpp` | S |

Tipo V = `RH_ScopedVMTInstall` (ranura 4 de la vtable; fila `0x00000000` en `docs/hooks.csv`). Tipo S = `RH_ScopedNamedGlobalInstall` (destructores con borrado y cuerpos: `RH_ScopedVMTInstall` no puede nombrar la ranura 0) y `RH_ScopedInstall` (`SetUp`). Las 26 filas nuevas de `docs/hooks.csv` siguen el formato `Clase,Fn,0xADDR8digitos,1,0,1,S` (V con `0x00000000`). Los dos tamanos de vector van en la categoria `CPedShelterAttractor` (el listado la daba) con los nombres `SArraySizeStride20/12`: no son metodos de esa clase sino instancias de `size()`.

## Funciones saltadas
Ninguna: las 26 son viables en C++ puro (sin `__asm`/`naked`, sin registros que C++ no pueda leer). Los saltos puros (`jmp` a otra funcion con los mismos argumentos y convenio) se escriben como llamadas a la funcion de destino con hook (`CFileMgr`, `~CTaskComplexGoToPointAiming`, `~CTaskSimpleAnim`, `~CTaskSimpleWaitUntilAreaCodesMatch`), lo que cambia solo la profundidad de pila. `X87Math.cpp` no se toca; no hay opciones de compilacion nuevas.

## Revision contra el EXE (`dumpbin /disasm`)
- **0x4AB980 / 0x4AB9B0 / 0x4ABA20**: `push 0; mov [ecx],85AAB0h; call 4AB370 (CEventGroup::Flush(false), thiscall ret 4)`; el destructor con borrado de `CEventGroup` lleva ese cuerpo en linea y despues `operator delete` del CRT (0x8214BD) solo con el bit 0 del BYTE de banderas, y devuelve `this`. La vtable se escribe ANTES de `Flush`. El cuerpo de `CEventGlobalGroup` (que el lote 143A dejaba como funcion propia) pasa a escribir 0x85AAB0 y llamar a `Flush(false)` como el EXE (antes llamaba al destructor generado de la DLL).
- **0x5E9800 / 0x5E9850**: con `[ecx+4]` nulo devuelve 0; si no, `([ecx+8]-[ecx+4])` (palabra de 32 bits con signo) dividido por 20 / 12 truncando hacia cero (`imul 0x66666667; sar 3` / `imul 0x2AAAAAAB; sar 1`, mas el bit de signo).
- **0x66DFA0 / 0x48E930**: son `jmp 0x668870` (~CTaskComplexGoToPointAiming, que deja su vtable 0x86FE00) y `jmp 0x61A6F0` (~CTaskSimpleAnim): ninguno escribe vtable propia. **0x5F67E0**: `mov [ecx],86C7E8h; jmp 6355E0`. **0x492F60**: `mov [ecx],859DC4h; ret` (otra copia de 0x46A610, mismo vtable de `CEntitySeekPosCalculator`).
- **Destructores con borrado de tareas (0x66DF80, 0x66DFB0, 0x638A80, 0x6397A0, 0x6397C0, 0x492EF0, 0x6579F0, 0x48E910, 0x48E940)**: plantilla de 30 bytes de Fn145A (`call <cuerpo>`, bit 0 del byte de banderas, `CTask::operator delete` 0x61A5B0, devuelve `this`). Cuerpos: 0x66DFA0, 0x668D40, 0x632EA0, 0x6355E0, 0x635900, 0x492F10, 0x655780, 0x48E930, 0x48E960.
- **0x463640**: `push esi; mov esi,ecx; call 61A390 (CTaskSimple ctor); mov [esi],859D20h; mov eax,esi`.
- **0x632C60 / 0x635490**: `mov eax,0F4h` / `mov eax,113h`. **0x48DEA0**: `movzx eax,word [ecx+1Ch]` (`m_nTaskType`). Para poder mutarlos sin recompilar medio proyecto, las tres `GetTaskType` pasan de inline en la cabecera a declaradas en `.h` y definidas en `.cpp`.
- **0x4AB240 / 0x4AB250 / 0x4AB260**: `jmp` a `CFileMgr::OpenFile`, `ReadLine`, `CloseFile` (todas con hook); `ReadLine` termina en `setne al` (solo AL).
- **0x4AB2D0**: copia siete dwords de pila a +0x00..+0x18 con movimientos enteros (sin x87: un NaN senalizante conserva sus bits). Se escribe con `std::bit_cast<uint32>`; antes la funcion no estaba enganchada.

## Cambios de produccion
20 `.cpp` (y las cabeceras de las tres `GetTaskType`): envolturas `...DestructorBody` / `...BaseDestructorCall` (`__declspec(noinline)`), las funciones de cuerpo y de destructor con borrado, el enganche (`RH_ScopedNamedGlobalInstall` o `RH_ScopedVMTInstall`), funciones libres exportadas `Lote147A...()` que devuelven la direccion de una envoltura (solo las usa el banco) y `CEventGroup::Flush` marcada `noinline` (para que el banco pueda sustituirla). 26 filas en `docs/hooks.csv`; registro con `register_hook.py`. No se ejecuto `sync_docs` / `hooks2md`.

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (A, B, C; ventana de 4 KiB con guardas de 0x40; `this` alineado, desalineado, al borde o cerca del inicio). Sin memoria global ni IAT.
Falsos identicos en la direccion del EXE y en la funcion de la DLL (`MakeCollab` / `F` / `H` / `Rt` / `U` / `E`): `CEventGroup::Flush` (0x4AB370, con hook; registra [direccion, this, dword de [this], argumento] y deja un dword del caso en [this]: prueba que la vtable se escribe antes y que `delete` va despues), los cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la envoltura de la DLL; los cuerpos 0x66DFA0 y 0x48E930 son funciones probadas del lote: `Rt` reutiliza su ruta; `~CTaskSimpleAnim` 0x61A6F0 sin hook, `U`), el constructor de `CTaskSimple` 0x61A390, `CTask::operator delete` 0x61A5B0, `operator delete` del CRT 0x8214BD (`E`, sin hook, la DLL lo llama por esa direccion) y `CFileMgr::OpenFile/ReadLine/CloseFile` (anotan sus argumentos crudos y devuelven un EAX del caso).
Se observa: bytes cambiados de las tres arenas, eventos de los falsos con todos sus argumentos, valor devuelto (`this` como etiqueta en destructores con borrado y constructor; EAX completo en tamanos, constantes, `GetTaskType` y saltos; solo AL en `ReadLine`) y el estado x87 final (vacio). **EAX, ECX, EDX y EFLAGS tras la llamada** quedan en `original_residue` / `candidate_residue` (informativos).
16 familias de 1024 casos por funcion (ver `Fixtures.hpp`): posicion de `this` x relleno de las arenas; vtable previa igual, con otro bit o ajena; banderas 0, 1, 2, 3, 0xFF, 0x100, 0x101, bits altos y aleatorias; vectores con inicio nulo / extremos y diferencias exactas, inexactas, negativas, cero; palabra de `m_nTaskType` con la mitad alta puesta; argumentos de los saltos crudos y retornos 0, 1, 0x100, bits altos; `SetUp` con NaN silenciosos y senalizantes, infinitos, ceros con signo, denormales y extremos.
`python -I fixture_selfcheck.py` (sin juego) pasa los 425984 casos por el oraculo y mide la cobertura de ramas (marcas de `KEYS`, minimo 50 por funcion).

## Oraculo independiente
`fixture_oracle.py` reimplementa solo lo observable desde el desensamblado; no usa ninguna constante ni rama del C++ de produccion.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE (se usan objetos sinteticos), el cuerpo real de los colaboradores falseados (`Flush`, los destructores, los constructores base, `CFileMgr`; tienen sus propios bancos) y la exactitud de `CFileMgr` frente a `fopen/fgets/fclose` reales.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, una por funcion y ronda, 26 por ronda; se aplican a las fuentes de produccion, se recompila la sonda y se corre con `FN147A_CASES=2048` casos por funcion; el banco y `run_local.py` leen la misma variable y la corrida aprobada usa 16384). Las mutaciones se condicionan a entradas que el juego no produce (`this` desalineado, banderas con el bit 1 o el 7, punteros 0xFFFFFFFF / 0x80000000) porque los hooks de la DLL estan activos antes de que empiece el banco. **Las 26 funciones fallan en ambas rondas** (`negative-runs/`: `20261109-203746-158855` con 15438 de 53248 pares con diferencias y `20261109-213332-244008` con 16208; el lado EXE coincide con el oraculo en todos los pares: "lado EXE fuera del oraculo 0"). Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, comprobado: `restauradas 20 fuentes`).
- Ronda 1: `Flush` con argumento verdadero / vtable propia / segunda llamada con `this` desalineado; destructores con borrado borrando tambien con la bandera 2 (332 a 392 cada uno); tamanos +1 con inicio desalineado; `GetTaskType` de otro tipo con `this` desalineado; saltos con argumentos cambiados (`path` nulo, `num` 0x7FFFFFFF, `file` -1); `SetUp` con un bit del rojo invertido.
- Ronda 2: vtable alternativa, destruccion doble con la bandera 0x80 (indices pares) o retorno nulo con `this` desalineado (impares); tamano cero con el final de resto 2; `GetTaskType` con el bit 15 borrado; `SetUp` con verde y azul intercambiados; saltos con retorno invertido / sumado.

## Bancos repetidos
Citan direcciones del lote (grep de `0x...` con y sin ceros y de la forma `...h` en `reversing/`, sin contar este banco, `_handoff`, `artifacts`, la evidencia archivada, el registro ni las copias fuente de `CppIntegerWorld10/runs`): `Fn143A` (0x4AB9B0), `Fn142A` (0x5F67E0), `FxInfoLoad28` (0x4AB250) y `PartnerShoveSeq28` (0x632C60, solo en una constante de `Fixtures.hpp`). Resultados al final de este fichero (ver "Resultado de las repeticiones").

## Tiempo
Una corrida de los 425984 pares: unos 18 min (el `comparisons.jsonl` crudo ocupa 775 MB; se archiva como `.gz` y se borra el crudo). Compilacion de la sonda: unos 7 min; las dos rondas negativas de 2048 casos por funcion, unos 4 min de juego cada una (mas la espera de los candados compartidos).

## Resultado de las repeticiones
Repetidos con el codigo final, de uno en uno y bajo los candados compartidos (`refresh_bank.py` actualizo las filas del registro; evidencia archivada en `reversing/CppTwelveLevel6Final/runs/`):
- `Fn143A` (cita 0x4AB9B0, ahora con hook propio): el banco daba el cuerpo por sin hook (`U`); adaptado a `H` (el hook se apaga y el falso va en la funcion de la DLL, que ahora es tambien el cuerpo enganchado) y `run_local.py` espera `hooked=True`. **Aprobado**, `20261109-213912-879331` (22 filas del registro).
- `Fn142A` (cita 0x5F67E0, ahora con hook propio y vtable + salto a 0x6355E0): mismo cambio (`H` anadido al banco). **Aprobado**, `20261109-215630-716842` (23 filas).
- `FxInfoLoad28` (cita 0x4AB250, ahora con hook que salta a `CFileMgr::ReadLine`): **aprobado sin adaptar**, `20261109-221606-820846` (1 fila).
- `PartnerShoveSeq28` (0x632C60 solo como constante de `Fixtures.hpp`): **aprobado sin adaptar**, `20261109-222755-582120`.
