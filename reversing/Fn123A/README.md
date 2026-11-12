<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 888 grupos / 10227 funciones reconstruidas; 10227 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn123A: destructor de CTaskComplexPartner (lote 123A)

Estado: **APROBADO dentro del juego** (08/11/2026, worktree `wv-1` sobre main 1dd408a67c + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn123a/20261108-034013-483427` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn123A-20261108-034013-483427`; hubo una corrida anterior igual, `20261108-032808-873404`, previa a las rondas negativas, a la que se repitio con las fuentes finales). Banco par a par dentro del juego, 16384 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 igual, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).

Plantilla del banco: `Fn122A` (arenas propias, `Gate` de ABI, `HookRoute`, parches de entrada, `MakeCollab`: el mismo falso en la direccion del EXE y en el cuerpo de la DLL).

| Funcion | Direccion | Bytes | Que hace | Fuente | Listado de pendientes decia |
|---|---|---|---|---|---|
| `CTaskComplexPartner` cuerpo no virtual del destructor (`Destructor`) | 0x683A40 | 141 | marco SEH, vtable 0x870664, libera el companero, libera el bloque de animaciones, llama a `CTaskComplex::Destructor` 0x61A3D0 | `Tasks/TaskTypes/TaskComplexPartner.cpp` | `CTaskComplexPartnerGreet::Destructor` / `CTaskComplexBeInCouple` (**mal atribuida**: la vtable que escribe es la de `CTaskComplexPartner`) |

No tenia hook ni estaba en `docs/hooks.csv` (comprobado en el codigo y en el CSV; no estaba en otro worktree: `git worktree list` solo listaba main). La clase ya tenia `.cpp` y `.h` propios. La dejo pendiente el lote 115A (marco SEH y `repe cmps`) y el lote 122A dejo a 0x684200 llamandola por su direccion del EXE.

## Revision contra el EXE (desensamblado con `dumpbin /disasm` del EXE canonico)
0x683A40..0x683ACD, thiscall, sin argumentos de pila, `ret`:
- Marco SEH (`push -1; push 0x8468B8; mov eax,fs:[0]; push eax; mov fs:[0],esp; push ecx; push ebp`): la tabla de 0x8468B8 solo cubre la llamada final a la base (estado 0 durante el cuerpo, -1 al llegar a ella) y su funclet (0x846980: `mov ecx,[ebp-10h]; jmp 0x61A3D0`) solo desenrolla llamando a `CTaskComplex::Destructor`. Ninguna llamada del cuerpo lanza excepciones, asi que el comportamiento observable es el de una secuencia lineal: **no hace falta reproducir el SEH** en C++ puro (sin `try`/`__try`); el banco comprueba que `fs:[0]` se restaura.
- `mov [ebp],0x870664`: vtable de `CTaskComplexPartner` del EXE (la que registra `RH_ScopedVirtualClass`). El cuerpo de la DLL escribe la direccion literal (no la vtable del ASI), como el resto de cuerpos de destructor de los lotes 115A/122A.
- `[ebp+0x38]` (`m_partner`) no nulo: `push &m_partner; call 0x571A00` (`CEntity::CleanUpOldReference`, ECX = el companero; `ret 4`). No pone el campo a cero.
- Byte `[ebp+0x6E]` (`m_requiredAnimsStreamedIn`) distinto de cero (se prueba entero, `test al,al`, tambien con 2, 0x80, 0xFF...): `repe cmps byte [esi],[edi]` con `ecx = 1`, `esi = this+0x5E` (`m_animBlockName`), `edi = 0x858B54`: **compara un solo byte**, el primero del nombre, contra el byte de 0x858B54, que vale 0 (la `""` del `strcmp` del C++ anterior). Si difiere (nombre no vacio): `GetAnimationBlockIndex(const char*)` 0x4D3990 (cdecl) y `RemoveAnimBlockRef(int)` 0x4D3FD0 (cdecl) con el indice devuelto, `add esp,8`. El byte 0x6E se pone a 0 siempre que estaba distinto de cero.
- `mov ecx,ebp; call 0x61A3D0` (`CTaskComplex::Destructor`) y epilogo (`mov ecx,[esp+8]; pop ebp; mov fs:[0],ecx; add esp,10h; ret`).
- Llamadores: cuerpos de destructor de subclases (0x684206 `CTaskComplexPartnerDeal`; 0x684286 y 0x684756 con `jmp`), 0x684368 y 0x684F63 (`call`; despues solo usan ESI y `[esp+8]`) y el funclet 0x846983 (`jmp`): ninguno lee EAX, ECX ni EDX tras la llamada.

## Cambios de produccion
- `TaskComplexPartner.cpp`: `CTaskComplexPartnerBody::Destructor` (`__fastcall`, `RH_ScopedNamedGlobalInstall(..., "Destructor", 0x683A40)`) escribe la vtable 0x870664, llama a `ReleasePartnerResources` y a `CTaskComplexBody::Destructor` (0x61A3D0; declaracion local como en `TaskComplexSequence.cpp`). `ReleasePartnerResources` (espacio anonimo) tiene el cuerpo que antes estaba en `~CTaskComplexPartner()`: ahora el destructor C++ (usado por las subclases reconstruidas) y el cuerpo enganchado comparten esa funcion (sin duplicados). Respecto al C++ anterior: el byte de bandera se prueba entero y el nombre se prueba por su primer byte (equivalente al `strcmp(nombre, "") != 0` anterior).
- `TaskComplexPartner.h`: declara `namespace CTaskComplexPartnerBody { void __fastcall Destructor(CTaskComplexPartner*, void*); }`.
- `TaskComplexPartnerDeal.cpp` (cuerpo 0x684200, lote 122A): la llamada `plugin::CallMethod<0x683A40>` pasa a llamada directa a `CTaskComplexPartnerBody::Destructor`; **0x684200 ya no depende del EXE** para su cola.
- No se toca `X87Math` ni las opciones de compilacion (no hay x87 ni contrato de registros: ningun llamador lee EAX/ECX/EDX; `TaskComplexPartner.cpp` ya se compilaba con `/O2 /Ob0 /Oi /arch:IA32 /fp:fast`, sin RTC ni PCH, por el lote 94A).
- Fila anadida a `docs/hooks.csv` (`CTaskComplexPartner,Destructor,0x00683A40,1,0,1,S`).

## Banco
`Fixtures.hpp`: 16 familias de 1024 casos (tipo = familia % 4: alineado, desalineado, borde de la ventana con el byte +0x6E entrando en la guarda, alineado con vtable previa interesante; relleno = familia / 4). `this` en una arena propia (guardas de 0x40, ventana de 0x180) con imagen previa conocida: vtable previa aleatoria / cero / unos / 0xDEADBEEF / la propia / la de otra clase / con uno o dos bytes distintos; companero nulo (25 %), dentro de la arena secundaria, el propio objeto, otro punto de la arena o crudo (1, 2, 0x7F000000, 0xFFFFFFFF, 0x80000000, 0x858B54, ...); byte de bandera 0 (35 %), 1, 2, 0x80, 0xFF o aleatorio; nombre de 16 bytes con primer byte 0 (35 %, con basura detras), 0x80, 0xFF, 1 o aleatorio; indice que devuelve el falso de `GetAnimationBlockIndex` (0, -1, 1, 0x7FFFFFFF, 0x80000000, 0x42, aleatorio).
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (`MakeCollab`; los cuatro tienen hook RH, apagado durante el banco): `CEntity::CleanUpOldReference` 0x571A00 (evento [dir, ECX, &m_partner, `[this]` al entrar]), `GetAnimationBlockIndex` por nombre 0x4D3990 (evento [dir, &nombre, FNV-1a de los 16 bytes del nombre]; devuelve el indice del caso), `RemoveAnimBlockRef` 0x4D3FD0 (evento [dir, indice]) y `CTaskComplex::Destructor` 0x61A3D0 (evento [dir, this, `[this]`, byte +0x6E, dword +0x38]). Los eventos fijan el orden de las cuatro llamadas, la vtable ya escrita al liberar el companero y el byte de bandera ya a cero al llegar a la base.
Se observa: bytes cambiados de las dos arenas y de la region global (`ms_aAnimBlocks`, 0x40 bytes desde 0xB5D4A0, sembrada y restaurada: ni el EXE ni la DLL la tocan porque `RemoveAnimBlockRef` esta falseado), el dword final en `[this]`, los eventos, EAX/ECX/EDX tras la llamada (informativos: ningun llamador los lee), pila, no volatiles, DF, SEH (`fs:[0]`), x87 y la palabra de control.
Cobertura de ramas (modelo del oraculo sobre las entradas, `fixture_selfcheck.py`; todas por encima de 50 casos): companero nulo / en arena / crudo / el propio objeto, bandera 0 / distinta de 0, nombre vacio / no vacio, con llamadas al bloque (6950) / sin ellas por bandera (5695) / sin ellas por nombre vacio (3739), indice 0 / negativo / otro, vtable previa igual / cero / unos / DEADBEEF / de otra clase / parcial / otra, de 0 a 5 bytes cambiados, this alineado / desalineado / con la escritura entrando en la guarda.

## Fuera de los fixtures
Objetos reales construidos por el EXE (funclets SEH y destructores con borrado ejecutados con el hook activo: se confirmo por desensamblado y por vtable, no con un banco de objetos reales), excepciones dentro del marco SEH, `this` nulo o crudo, el cuerpo real de los cuatro colaboradores (falseados; tienen sus propios bancos), el byte de 0x858B54 (se leyo del EXE: 0; el cuerpo de la DLL lo trata como constante) y los registros volatiles.

## Bancos repetidos
`grep` de 0x683A40 y 0x684200 (con y sin ceros, en `reversing/` sin Fn123A ni las evidencias archivadas): solo **`Fn122A`** las cita (los README de Fn111A, Fn113A y Fn115A solo las mencionan como funciones dejadas; no son bancos).
- **`Fn122A` adaptado y aprobado** (banco, no produccion): el falso de 0x683A40 iba solo en la direccion del EXE; el colaborador tiene ahora hook RH y `MakeCollab` lo parchea tambien en el cuerpo de la DLL (el cuerpo de 0x684200 llama directamente a `CTaskComplexPartnerBody::Destructor`). `run_local.py` espera ahora `(0x683A40, True)`. Aprobado con el codigo final: `resultados-fn122a/20261108-034147-968424` (163840 pares, cero diferencias; archivada en `reversing/CppTwelveLevel6Final/runs/Fn122A-20261108-034147-968424`; 10 filas del registro refrescadas con `refresh_bank.py`).
- Ningun otro banco cita esas direcciones.

## Prueba negativa
Copia buena (SHA256, `mutate.py` la guarda en `artifacts/good/` y la restaura comprobando el SHA256): `TaskComplexPartner.cpp` 79458c17.... Una mutacion por ronda (resultados en `negative-runs/`, recuentos en `pares-con-diferencias.txt`; ningun par "solo del oraculo"):
1. vtable + 1: 16384 / 16384 (`20261108-033000-709654`).
2. bandera probada solo en el bit 0 (`& 1`): 4088 / 16384 (`20261108-033125-911828`).
3. primer byte del nombre probado con signo (`> 0`): 3658 / 16384 (`20261108-033252-638276`).
4. vtable escrita despues de liberar el companero: 11087 / 16384 (`20261108-033421-353494`).
5. byte de bandera sin poner a cero: 10689 / 16384 (`20261108-033547-336390`).
6. companero == 1 sin liberar: 337 / 16384 (`20261108-033712-665299`).
7. destructor base antes de liberar (orden invertido de la cola): 14972 / 16384 (`20261108-033839-698061`).
Las siete mutaciones se detectan. Fuente restaurada (`cmp` por SHA256 contra la copia buena) y sonda recompilada con las fuentes buenas antes de la corrida aprobada final. Los `comparisons.jsonl` crudos de las rondas negativas se borraron del disco E: (los recuentos salen de `pares-con-diferencias.txt`).

## Tiempo
Una corrida de los 16384 pares: unos 1,5 min (mas la compilacion incremental de la sonda).
