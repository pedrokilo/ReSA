<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10344 funciones reconstruidas; 10344 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn154A: 15 funciones diminutas del EXE (lote 154A)

Estado: **APROBADO dentro del juego** (10/11/2026, worktree `wv-1` desde main cef33b8425 + los cambios del lote; plantilla `Fn153A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn154a/20261110-144216-486902` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn154A-20261110-144216-486902`; `comparisons.jsonl.gz` de 29 MB, bajo el limite de GitHub). 15 x 16384 = **245760 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `bba1e0602a989aa9b55d69624ddeccea342ef7ac3a7c49cde8987c5da5d7d077`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 18 direcciones. **Tres ya tenian hook y se saltaron** (se comprobo el codigo y `docs/hooks.csv`): 0x638A40 (`CTaskComplexSequence::CreateNextSubTask`, V, el listado decia CTaskComplexWaitForBus), 0x4F21C0 (`CAEStreamingChannel::Stop`, V) y 0x59BB60 (`CMatrix::Update`, S, el listado decia CCompressedMatrixNotAligned). Las 15 restantes no tenian hook. Las clases del listado se atribuian por cercania y varias eran incorrectas: los nombres reales salen del desensamblado, de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CTaskComplexSignalAtPed` destructor con borrado (vtable 0x86FA8C; CTaskGangHasslePed, B) | 0x6644C0 | 30 | `Tasks/TaskTypes/TaskComplexSignalAtPed.cpp` | S |
| 1 | `CTaskGoToVehicleAndLean` destructor con borrado (vtable 0x86FAC8; CTaskGangHasslePed, B) | 0x6644E0 | 30 | `.../TaskGoToVehicleAndLean.cpp` | S |
| 2 | `CTaskComplexFallAndGetUp` destructor con borrado (vtable 0x870454; CTaskSimpleFall, B) | 0x67CB50 | 30 | `.../TaskComplexFallAndGetUp.cpp` | S |
| 3 | `CTaskSimpleFall::GetTaskType` (ranura 4; B) | 0x678300 | 6 | `.../TaskSimpleFall.cpp` (+ `.h`) | V |
| 4 | `CTaskSimpleFall` cuerpo del destructor (marco SEH; C) | 0x678310 | 93 | `.../TaskSimpleFall.cpp` | S |
| 5 | `CEventHandlerHistory::Flush` (el listado decia CEventHandler, D) | 0x4C3740 | 65 | `Events/EventHandlerHistory.cpp` | S |
| 6 | `CEventHandlerHistory::Flush`, copia byte a byte (sin llamadores directos; D) | 0x4C3E30 | 65 | idem (`FlushCopy`) | S |
| 7 | `CEventObjectCollision::GetEventType` (vtable 0x85ACA8, ranura 1; B) | 0x4ACDA0 | 6 | `Events/EventObjectCollision.cpp` (+ `.h`) | V |
| 8 | `CEventObjectCollision::GetEventPriority` (ranura 2; B) | 0x4ACDC0 | 6 | idem | V |
| 9 | `CEventObjectCollision::GetLifeTime` (ranura 3; B) | 0x4ACDB0 | 3 | idem | V |
| 10 | `CEventObjectCollision::TakesPriorityOver` (ranura 11; B) | 0x4ACEF0 | 5 | idem | V |
| 11 | `CEventObjectCollision::IsHeadOnCollision` (A) | 0x4ACE80 | 110 | idem | S |
| 12 | `CPedIntelligence::AddInterestingEntity` (sin llamadores directos; D) | 0x602240 | 93 | `PedIntelligence.cpp` (+ `.h`) | S |
| 13 | `CMatrix::AttachRW` (el listado decia CCompressedMatrixNotAligned, D) | 0x59BD90 | 57 | `Core/Matrix.cpp` (+ `.h`; en `InjectHooksAttachAssign`) | S |
| 14 | `CAEAudioEntity` cuerpo del destructor, copia de 0x5DE9E0 (el listado decia CAEGlobalWeaponAudioEntity, C) | 0x506CC0 | 14 | `Audio/Entities/AEAudioEntity.cpp` (+ `.h`) | S (`Destructor-3`) |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Tipo S = `RH_ScopedNamedGlobalInstall` (destructores con borrado, cuerpo de CTaskSimpleFall y copia de Flush), `RH_ScopedInstall` (miembros) y `RH_ScopedNamedInstall` (`Destructor-3`). Las 15 filas nuevas de `docs/hooks.csv` siguen el formato `Clase,Fn,0xADDR8digitos,1,0,1,S` (o V). Todas las clases ya tenian su `.cpp` y su `.h` (dos archivos por clase); no se crean archivos de produccion nuevos.

## Funciones saltadas
- Ya con hook: 0x638A40, 0x4F21C0 y 0x59BB60 (ver arriba).
- No inviable ninguna de las 15: todas son viables en C++ puro (sin `__asm`/`naked`, sin registros que C++ no pueda leer). `0x506CC0` termina en `jmp 0x4EF660` (cola de salto): en C++ es una llamada a `CAESound::Destructor` y el valor de EAX tras la llamada (el que deje ~CAESound) no lo lee nadie (el unico llamador es un funclet de desenrollado SEH en 0x83C5EA). No se toca `X87Math.cpp` ni `CMakeLists.txt`: `IsHeadOnCollision` usa la misma forma que `CEventBuildingCollision::IsHeadOnCollision` (0x4AD1E0, ya aprobada) con el umbral 0.9397f (0x3F70902E).

## Revision contra el EXE (desensamblado propio con `dumpbin /disasm`, busqueda de referencias en el PE y `docs/MemoryMap/binary_calls.csv`)
- **0x6644C0 / 0x6644E0 / 0x67CB50**: ranura 0 de las vtables 0x86FA8C (`CTaskComplexSignalAtPed`, cuerpo 0x660AB0 con hook, GetTaskType 0x4BA), 0x86FAC8 (`CTaskGoToVehicleAndLean`, cuerpo 0x660EE0 con hook, 0x4C0) y 0x870454 (`CTaskComplexFallAndGetUp`, cuerpo 0x678790 = `mov [ecx],870454h; jmp 61A3D0`, sin hook): `push esi; mov esi,ecx; call cuerpo; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`.
- **0x678300 / 0x678310**: `mov eax,0CFh` (ranura 4 de la vtable 0x870430 de `CTaskSimpleFall`) y su cuerpo del destructor: vtable 0x870430, `SetFinishCallback(0x4D1390, 0)` (0x4CEBE0) sobre `m_pAnim` (+0x14) si no es nulo y `~CTask` (0x61A3A0). Misma forma que `CTaskSimpleGiveCPR` (lote 150A).
- **0x4C3740 / 0x4C3E30**: copias de `CEventHandlerHistory::Flush` (el listado decia CEventHandler): `delete` virtual (`push 1; call [vtable]`) del evento temporal (+8), el no temporal (+4) y el guardado (+0xC), en ese orden, con la puesta a cero de +8 y +4 DESPUES de cada llamada previa y la de +0xC solo si habia evento; el banco observa el estado del historial dentro de cada destructor falso.
- **0x4ACDA0 / 0x4ACDC0 / 0x4ACDB0 / 0x4ACEF0**: ranuras 1 (5), 2 (0x39), 3 (0) y 11 (`mov al,1; ret 4`) de la vtable 0x85ACA8 de `CEventObjectCollision`. **0x4ACE80**: no virtual; `v = (normal.x, normal.y, 0)`, `Normalise` (0x59C910), `-((vy*fy + vz*fz) + vx*fx) > 0.9397f` con el avance de `ped->m_matrix` (+0x10); `ComputeObjectCollisionResponse` descarta el resultado (banco EvHB).
- **0x602240**: `m_apInterestingEntities` (+0x288, 3 casillas) desplazado: [2]=[1], [1]=[0], [0]=entidad; cada casilla libera su referencia anterior (0x571A00) y registra la nueva (0x571B70) solo si no son nulas, en el orden de casillas 2, 1 y 0. Sin llamadores directos (hermana de 0x602080, 0x6020A0 y 0x602320).
- **0x59BD90**: `CMatrix::AttachRW(RwMatrix*, bool)`, hermana de `Attach` (0x59BD10): bandera de propiedad leida y escrita como dword, `RwMatrixDestroy` (0x7F2A20) solo con matriz y bandera, y `UpdateRwMatrix` (0x59AD70) con la matriz nueva si no es nula.
- **0x506CC0**: copia byte a byte de 0x5DE9E0 y 0x4A9F60 (`mov [ecx],859988h; add ecx,8; jmp 4EF660`); solo la alcanza el funclet de 0x83C5EA.

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (A, B, C; ventana de 4 KiB con guardas de 0x40). Sin memoria global del juego. Plantilla `Fn153A`.
Falsos identicos en la direccion del EXE y en la funcion de la DLL (`F` / `H` / `U` / `E`): cuerpos de destructor 0x660AB0 y 0x660EE0 (con hook propio, `H`), 0x678790 (sin hook, `U`) y ~CTask 0x61A3A0 (`F`), `CTask::operator delete` 0x61A5B0, SetFinishCallback 0x4CEBE0, Normalise 0x59C910, CleanUpOldReference 0x571A00, RegisterReference 0x571B70, UpdateRwMatrix 0x59AD70, ~CAESound 0x4EF660 y RwMatrixDestroy 0x7F2A20 (`E`: sin hook, la DLL lo llama por esa direccion). Los destructores virtuales de Flush son entradas de una vtable falsa en la arena C.
Se observa: bytes cambiados de las tres arenas, eventos de los falsos con todos sus argumentos, valor devuelto (`this` como etiqueta de arena, EAX completo en constantes, AL en bool) y estado x87 final (vacio). **EAX, ECX, EDX y EFLAGS tras la llamada** quedan en `original_residue` / `candidate_residue` (informativos).
16 familias de 1024 casos por funcion (ver `Fixtures.hpp`): posicion de `this` x relleno de las arenas; banderas 0, 1, 2, 3, 0xFF, 0x100, bits altos y aleatorias; vtable previa; los tres eventos del historial nulos, propios o compartidos; en IsHeadOnCollision normales y avances con NaN, infinitos, ceros con signo, denormales, valores en el umbral 0.9397f +- unos ulp y magnitudes acotadas (|exponente| <= 32: la x87 de 24 bits tiene exponente ampliado y el SSE de la DLL no); casillas de entidades nulas, iguales y crudas; bandera de AttachRW con bits altos y matriz previa con propiedad distinta de cero con el byte bajo a cero.
`python -I fixture_selfcheck.py` (sin juego) pasa los 245760 casos por el oraculo y mide la cobertura de ramas (marcas de `KEYS`, minimo 50 por funcion).
Primera corrida (descartada): el EXE y la DLL coincidian en los 245760 pares, pero el oraculo fallaba en IsHeadOnCollision porque la constante del umbral del banco (0x3F70D6A0) estaba mal; la del EXE es 0x3F70902E (leida de 0x85AAE4). Corregida en `Fixtures.hpp` y `fixture_oracle.py` (el oraculo corregido valida los 245760 pares de esa primera corrida en ambos lados) y repetida entera: es la corrida aprobada.

## Oraculo independiente
`fixture_oracle.py` reimplementa solo lo observable desde el desensamblado; no usa ninguna constante ni rama del C++ de produccion. La aritmetica de IsHeadOnCollision se modela con fracciones exactas de Python (productos y sumas redondeados al par a 24 bits, NaN, infinitos, ceros con signo y comparacion estricta).

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE, magnitudes de IsHeadOnCollision con desbordamiento intermedio (x87 con exponente ampliado frente al SSE de la DLL), `Normalise` real (el banco usa un falso identico; tiene su propio banco, Fn96A) y la exactitud de los demas colaboradores falseados (tienen bancos propios).

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, una por funcion y ronda; se aplican a las fuentes, se recompila la sonda y se corre con `FN154A_CASES=2048` por funcion; la corrida aprobada usa 16384). Las mutaciones se condicionan a entradas que el juego no produce (`this` u objeto desalineado, banderas con el bit 1 o el 7) para que la partida no se caiga antes del banco. **Las 15 funciones fallan en ambas rondas** (`negative-runs/`: `ronda1-20261110-150023-119332` con 10931 de 30720 pares con diferencias y `ronda2-20261110-150827-251390` con 8653; el lado EXE coincide con el oraculo en todos los pares: "lado EXE fuera del oraculo 0"). Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, "restauradas 9 fuentes", comprobado tras cada ronda).

## Bancos repetidos
Citan direcciones del lote (grep de `0x...` con y sin ceros y de la forma `...h` en `reversing/`, sin contar este banco, `_handoff`, `artifacts`, la evidencia archivada ni los registros): 1 banco, `EvHB` (0x4ACE80, en un comentario y en la documentacion: `ComputeObjectCollisionResponse` descarta el resultado). Resultado en el informe del lote.
