<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 898 grupos / 10446 funciones reconstruidas; 10446 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn160A: 23 funciones diminutas del EXE (lote 160A)

Estado: **APROBADO dentro del juego** (10/11/2026, worktree `wv-1` creado con `git worktree add --detach` desde main 603d1e9bbc + los cambios del lote; plantilla `Fn159A`, que viene de `Fn156A`/`Fn155A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn160a/20261110-231503-889614` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn160A-20261110-231503-889614`; `comparisons.jsonl.gz` de 48,4 MB, bajo el limite de GitHub). 23 x 16384 = **376832 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `2b67db7036dc5a5d1310f1bf0ff4e1f2ce373b33d83052c826bb224990020cf8`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Las 23 direcciones del encargo estaban **sin hook** (se comprobo el codigo, `docs/hooks.csv` y `git worktree list`). Las 23 son viables en C++ puro (sin `__asm`/`naked`); no se salto ninguna. El listado atribuia las clases por cercania y **la mayoria era incorrecta**: los nombres reales salen del desensamblado (`dumpbin /disasm`), de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente | Tipo |
|---|---|---|---|---|---|
| 0 | `CTaskSimpleSwim::Clone` (ranura 1 de 0x870920; CTaskSimplePlayerOnFoot, B) | 0x68B050 | 100 | `Tasks/TaskTypes/TaskSimpleSwim.cpp` (+ `.h`) | V |
| 1 | `CTaskSimplePlayerOnFoot::ScalarDeletingDestructor` (vtable 0x8708EC; B) | 0x68B0C0 | 30 | `.../TaskSimplePlayerOnFoot.cpp` | S |
| 2 | `CTaskSimpleSwim::ScalarDeletingDestructor` (vtable 0x870920; el listado decia CTaskSimplePlayerOnFoot, B) | 0x68B0E0 | 30 | `.../TaskSimpleSwim.cpp` | S |
| 3 | `CTaskComplexGoToAttractor::GetTaskType` (0x393, ranura 4 de 0x86FF3C; B) | 0x66B690 | 6 | `.../TaskComplexGoToAttractor.cpp` (+ `.h`) | V |
| 4 | `CTaskComplexGoToAttractor::MakeAbortable` (ranura 6; B) | 0x66B6B0 | 8 | idem (ya reconstruida, sin hook) | V |
| 5 | `CTaskComplexGoToAttractor::ControlSubTask` (ranura 10; B) | 0x66B710 | 6 | idem (ya reconstruida, sin hook) | V |
| 6 | `CTaskComplexGoToPointAnyMeans::Clone` (ranura 1 de 0x86FF68; CTaskComplexGoToAttractor, B) | 0x66D1E0 | 112 | `.../TaskComplexGoToPointAnyMeans.cpp` (+ `.h`) | V |
| 7 | `CTaskComplexGoToPointAnyMeans::ScalarDeletingDestructor` (vtable 0x86FF68; idem, B) | 0x6705B0 | 30 | idem | S |
| 8 | `CTaskComplexJump::GetTaskType` (0xD3, ranura 4 de 0x870570; B) | 0x67A050 | 6 | `.../TaskComplexJump.cpp` (+ `.h`) | V |
| 9 | `CTaskComplexJump::ControlSubTask` (ranura 10; B) | 0x67A100 | 6 | idem | V |
| 10 | `CTaskSimpleClimb::Clone` (ranura 1 de 0x87059C; CTaskComplexJump, B) | 0x67C610 | 122 | `.../TaskSimpleClimb.cpp` (+ `.h`) | V |
| 11 | `CTaskSimpleClimb::ScalarDeletingDestructor` (vtable 0x87059C; idem, B) | 0x67DBC0 | 30 | idem | S |
| 12 | `CTaskComplexGangLeader::ScalarDeletingDestructor` (vtable 0x86F8FC; CTaskComplexProstituteSolicit, B) | 0x662350 | 30 | `.../TaskComplexGangLeader.cpp` | S |
| 13 | `CPathNode::GetNodeCoors(CVector*)` (CTaskComplexProstituteSolicit, A) | 0x667310 | 99 | `PathFind.cpp` (+ `.h`) | S |
| 14 | `CTaskSimpleRunNamedAnim::AnimNameIsEmpty` (comparacion con la cadena vacia; D) | 0x667460 | 37 | `.../TaskSimpleRunNamedAnim.cpp` | S |
| 15 | `CEventPedCollisionWithPlayer` cuerpo del destructor (C) | 0x5FED80 | 11 | `Events/EventPedCollisionWithPlayer.cpp` | S |
| 16 | `CEventPedCollisionWithPlayer::GetEventType` (3, ranura 1 de 0x86C970; B) | 0x5FED90 | 6 | idem (+ `.h`) | V |
| 17 | `CEventPedCollisionWithPlayer::Clone` (ranura 4; B) | 0x5FEDA0 | 120 | idem (+ `.h`) | V |
| 18 | `CEventPedCollisionWithPlayer::ScalarDeletingDestructor` (B) | 0x5FEE20 | 30 | idem | S |
| 19 | `CEventPlayerCollisionWithPed` cuerpo del destructor (C) | 0x5FEE80 | 11 | `Events/EventPlayerCollisionWithPed.cpp` | S |
| 20 | `CEventPlayerCollisionWithPed::GetEventType` (4, ranura 1 de 0x86C9B0; B) | 0x5FEE90 | 6 | idem (+ `.h`) | V |
| 21 | `CEventPlayerCollisionWithPed::Clone` (ranura 4; B) | 0x5FEEA0 | 120 | idem (+ `.h`) | V |
| 22 | `CEventPlayerCollisionWithPed::ScalarDeletingDestructor` (B) | 0x5FEF20 | 30 | idem | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Tipo S = `RH_ScopedNamedGlobalInstall` (destructores con borrado, cuerpos de destructor de evento, `AnimNameIsEmpty`) y `RH_ScopedInstall` (`CPathNode::GetNodeCoors`). Las 23 filas nuevas de `docs/hooks.csv` (al final del archivo) siguen el formato `Clase,Fn,0xADDR8digitos,1,0,1,S`.

## Funciones saltadas
Ninguna. `X87Math.cpp` no se toca (ninguna usa la x87 en la DLL) y no hay opciones de compilacion nuevas en `source/CMakeLists.txt`.

## Revision contra el EXE (desensamblado propio, busqueda de referencias en el PE y lectura de las vtables)
- **Destructores con borrado** (1, 2, 7, 11, 12, 18, 22): `push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je ..; push esi; call <liberacion>; add esp,4; mov eax,esi; pop esi; ret 4`. Cuerpos 0x6857D0 (con hook propio), 0x688A00 (`CTaskSimpleSwim`, **sin hook**: la DLL lo llama por una envoltura propia sin expansion), 0x66B830 y 0x65DF30 (con hook propio, envoltura propia), 0x67A1D0 (con hook, es la propia funcion `CTaskSimpleClimbDestructorBodyExe`), 0x5FED80 y 0x5FEE80 (nuevos en este lote). Liberacion: `CTask::operator delete` 0x61A5B0 o `CEvent::operator delete` 0x4B5630; solo cuenta el bit 0 del BYTE de banderas; devuelven `this`.
- **Constantes** (3, 8, 16, 20): `mov eax,393h`, `mov eax,0D3h`, `mov eax,3`, `mov eax,4`; no leen el objeto.
- **ControlSubTask** (5, 9): `mov eax,[ecx+8]; ret 4`. **MakeAbortable** (4): `mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]`, salto a la ranura 6 de la subtarea con los tres argumentos intactos.
- **Clone de tarea** (0, 6, 10; marco SEH que solo desenrolla): `CTask::operator new` (0x61A5A0) con 0x64/0x34/0x30 bytes; sin memoria devuelve 0; con memoria llama al constructor (0x688930 / 0x66B790 / 0x67A110) y devuelve **su EAX**. En la DLL: `CTask::operator new(sizeof)` y `Constructor(...)` (ahora `__declspec(noinline)` para que el banco lo sustituya). El constructor de Climb solo lee el byte de sus tres ultimos argumentos (`mov al,byte [esp+..]`); el de eventos solo las palabras de 16 bits de los tres enteros cortos: el falso anota solo esos bits.
- **Clone de evento** (17, 21): `CEvent::operator new(0x34)`; si no es nulo llama al constructor de `CEventPedCollisionWithPed` 0x4AC990 (7 argumentos), **ignora su valor devuelto**, escribe la vtable de la clase (0x86C970 / 0x86C9B0) y devuelve el objeto. `CEventPedCollisionWithPed::Constructor` pasa a `protected` y `noinline`.
- **Cuerpos de destructor de evento** (15, 19): `mov [ecx],<vtable>; jmp 0x4ACA70`. El cuerpo 0x4ACA70 (con hook, antes en espacio anonimo) pasa a `CEventBody::PedCollisionWithPedDestructorBody` para que las dos clases derivadas lo llamen.
- **0x667310** = `CPathNode::GetNodeCoors(CVector*)` (lo llaman 0x66F0CC, 0x66F19C, 0x66F262 y 0x66F313 con `m_pPathNodes[area] + 0x1C * id`): lee los tres enteros de 16 bits antes de escribir (el destino puede solaparse) y escribe entero * 0.125 (exacto). Devuelve void.
- **0x667460**: compara el primer byte de la cadena de ECX con el de la cadena vacia 0x858B54 (un solo byte) y devuelve AL = 1 si son iguales. No tiene llamadores ni referencias en el EXE; esta entre las comparaciones de nombre de animacion 0x6673B0/0x667440 y el constructor de `CTaskSimpleRunNamedAnim` 0x6674B0. **La clase propietaria no se pudo identificar** (se deja como funcion global enganchada, nombre `AnimNameIsEmpty` bajo `CTaskSimpleRunNamedAnim`).

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH (S o V) apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (A, B, C; ventana de 4 KiB con guardas de 0x40). Sin memoria global del juego.
Falsos identicos en la direccion del EXE y en la funcion de la DLL (`F` / `H` / `U`): cuerpos de destructor (0x6857D0, 0x66B830 y 0x65DF30 con hook propio `H`: el hook se apaga y el falso va en la envoltura de la DLL; 0x67A1D0, 0x5FED80 y 0x5FEE80 con hook y funcion propia `F`; 0x688A00 sin hook `U`), 0x4ACA70, `CTask::operator delete/new` (0x61A5B0 / 0x61A5A0), `CEvent::operator delete/new` (0x4B5630 / 0x4B5620) y los constructores 0x688930, 0x66B790, 0x67A110 y 0x4AC990. MakeAbortable no tiene colaboradores con hook: la subtarea es un objeto de la arena con una vtable falsa del banco (ranura 6 = funcion que anota ECX y los tres argumentos; las demas ranuras anotan un evento de error).
Se observa: bytes cambiados de las tres arenas, eventos de los falsos con sus argumentos, valor devuelto (this, objeto nuevo o subtarea como etiqueta de arena; EAX completo en constantes, MakeAbortable y la cadena vacia) y el estado x87 final (vacio). **EAX, ECX, EDX y EFLAGS tras la llamada** quedan en `original_residue` / `candidate_residue` (informativos). Los constructores y destructores escriben vtables: los cuerpos observan la vtable previa de `[ECX]` y los Clone de evento dejan la vtable de la clase del EXE.
16 familias de 1024 casos por funcion (ver `Fixtures.hpp`): posicion de `this` x relleno de las arenas; vtable previa; banderas; memoria disponible o no en los Clone; punteros crudos o de la arena; palabras con bytes y enteros de 16 bits con bit alto; subtarea en B, en C o el propio `this`; destino de `GetNodeCoors` en B, C, solapado con el nodo o en otro sitio de A; cadena vacia o no.
`python -I fixture_selfcheck.py` (sin juego) pasa los 376832 casos por el oraculo y mide la cobertura de ramas (marcas de `KEYS`, minimo 50 por marca).

## Oraculo independiente
`fixture_oracle.py` reimplementa solo lo observable desde el desensamblado; no usa ninguna constante ni rama del C++ de produccion (solo el relleno pseudoaleatorio de las arenas, para conocer su imagen inicial).

## Fuera de los fixtures
`this` nulo o crudo, subtarea nula en MakeAbortable (el EXE se cae), objetos reales construidos por el EXE, **NaN senalizador** en los campos float copiados por los Clone (el EXE copia la intensidad del evento con `fld/fstp`, que lo vuelve silencioso; el C++ lo copia tal cual: los fixtures solo usan floats sin NaN senalizador), y la exactitud de los colaboradores falseados (tienen sus propios bancos).

## Hallazgos
- Cero diferencias en la primera corrida completa (tras una de humo de 64 casos por funcion que solo fallo la cobertura minima de ramas, esperada con 64 casos). Ningun defecto de las funciones reconstruidas ni del oraculo.
- Un error propio de edicion (un NUL en lugar de `'\0'` en `AnimNameIsEmpty`) hizo fallar la compilacion de la base; corregido antes del banco.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, una por funcion y ronda: 23 mutaciones; se aplican a las fuentes, se recompila la sonda y se corre con `FN160A_CASES=2048` por funcion; la corrida aprobada usa 16384). Las mutaciones se condicionan a `this` o al puntero de cadena desalineado (el juego no lo produce) para que la partida no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
**Las 23 funciones fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261110-234546-412670` con 15855 de 47104 pares con diferencias; ronda 2 `20261111-000622-089868` con 15848 de 47104 (el lado EXE coincide con el oraculo en todos los pares: "lado EXE fuera del oraculo 0"). Fuentes restauradas con `mutate.py restore` ("restauradas 11 fuentes", SHA256 igual a las copias buenas).

## Bancos que citan estas direcciones
Grep de `0x...` con y sin ceros y de la forma `...h` en `reversing/` (sin este banco, `_handoff`, `artifacts`, evidencia archivada ni registros): **ningun banco ejecutable** cita ninguna de las 23 direcciones; `Fn157A` nombra 0x67DBC0 solo en el texto de su README (como funcion pendiente). No se repitio ni adapto ningun banco antiguo (pase final unico del coordinador). Cambios de produccion que pueden afectar a bancos antiguos de las mismas clases: `CEventPedCollisionWithPed::Constructor` ahora `protected`/`noinline`, `PedCollisionWithPedDestructorBody` movido a `CEventBody` (Fn107A), constructores de `CTaskSimpleSwim`, `CTaskComplexGoToPointAnyMeans` y `CTaskSimpleClimb` ahora `noinline` (Fn157A usa el cuerpo del destructor de Climb, sin cambios).

## Repeticion del 11/11/2026 (lote 163A)
El lote 163A engancha con RH_ScopedNamedGlobalInstall el cuerpo 0x688A00 (lote 163A lo engancha), que este banco trataba como colaborador sin hook. Se adapto solo el banco (colaborador con hook, `H`: falso en la direccion del EXE y en la funcion de la DLL; fila `True` en `STUBBED`; `DLL_BODY_WITHOUT_HOOK` vacio) y se repitio entero: **APROBADO**, cero diferencias. Evidencia: `reversing/CppTwelveLevel6Final/runs/Fn160A-20261111-153912-020103`.
