<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 899 grupos / 10469 funciones reconstruidas; 10469 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn157A: 24 funciones diminutas del EXE (lote 157A)

Estado: **APROBADO dentro del juego** (10/11/2026, worktree `wv-1` desde main 9277fb4c56 + los cambios del lote; plantilla `Fn155A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn157a/20261110-184254-285397` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn157A-20261110-184254-285397`; `comparisons.jsonl.gz` de 49,9 MB, bajo el limite de GitHub). 24 x 16384 = **393216 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `b36635ea2a6ec6e034a04984196af151adfa70cd7fc0677e257820e96c74972e`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 24 direcciones y **ninguna tenia hook** (codigo, `docs/hooks.csv` y `git worktree list`). Las 24 son viables en C++ puro (sin `__asm`/`naked`); no se salto ninguna. Las clases del listado eran casi todas erroneas (por cercania de direcciones): los nombres reales salen del desensamblado, de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CTaskComplexFleeEntity::ScalarDeletingDestructor` (vtable 0x86F708; CTaskComplexFleePoint) | 0x65D240 | 30 | `Tasks/TaskTypes/TaskComplexFleeEntity.cpp` | S |
| 1 | `CTaskComplexSmartFleePoint::ScalarDeletingDestructor` (0x86F744) | 0x65D260 | 30 | `...TaskComplexSmartFleePoint.cpp` | S |
| 2 | `CTaskComplexSmartFleeEntity::ScalarDeletingDestructor` (0x86F784) | 0x65D280 | 30 | `...TaskComplexSmartFleeEntity.cpp` | S |
| 3 | `CTaskComplexFleeShooting::ScalarDeletingDestructor` (0x86F7C4) | 0x65D2A0 | 30 | `...TaskComplexFleeShooting.cpp` | S |
| 4 | `CTaskComplexFleeAnyMeans::ScalarDeletingDestructor` (0x86F7F0) | 0x65D2C0 | 30 | `...TaskComplexFleeAnyMeans.cpp` | S |
| 5 | `CTaskComplexObserveTrafficLights::ScalarDeletingDestructor` (0x86DF98; CTaskComplexSitDownThenIdleThenStandUp) | 0x637F60 | 30 | `...TaskComplexObserveTrafficLights.cpp` | S |
| 6 | `CTaskComplexObserveTrafficLightsAndAchieveHeading::ScalarDeletingDestructor` (0x86DFC4) | 0x637F80 | 30 | `...TaskComplexObserveTrafficLightsAndAchieveHeading.cpp` | S |
| 7 | `CTaskComplexCrossRoadLookAndAchieveHeading::ScalarDeletingDestructor` (0x86DFF0) | 0x637FA0 | 30 | `...TaskComplexCrossRoadLookAndAchieveHeading.cpp` | S |
| 8 | `CTaskSimpleTurn180::ScalarDeletingDestructor` (0x86E01C) | 0x637FC0 | 30 | `...TaskSimpleTurn180.cpp` | S |
| 9 | `CTaskComplexHitResponse::ScalarDeletingDestructor` (0x86E040) | 0x638020 | 30 | `...TaskComplexHitResponse.cpp` | S |
| 10 | `CTaskSimpleIKLookAt::ScalarDeletingDestructor` (0x86E37C; CTaskSimpleIKChain) | 0x639490 | 30 | `...TaskSimpleIKLookAt.cpp` | S |
| 11 | `CTaskSimpleIKPointArm::ScalarDeletingDestructor` (0x86E3A4) | 0x6394B0 | 30 | `...TaskSimpleIKPointArm.cpp` | S |
| 12 | `CTaskSimpleTriggerLookAt::ScalarDeletingDestructor` (0x86E3CC) | 0x6394D0 | 30 | `...TaskSimpleTriggerLookAt.cpp` | S |
| 13 | `CTaskSimpleClearLookAt::ScalarDeletingDestructor` (0x86E404) | 0x6394F0 | 30 | `...TaskSimpleClearLookAt.cpp` | S |
| 14 | `CTaskComplexUseClosestFreeScriptedAttractor::ScalarDeletingDestructor` (0x86E428) | 0x639510 | 30 | `...TaskComplexUseClosestFreeScriptedAttractor.cpp` | S |
| 15 | `CEventPlayerCommandToGroupAttack::GetEventType` (vtable 0x86C710, ranura 1) | 0x5F6360 | 6 | `Events/EventPlayerCommandToGroupAttack.cpp` (+ `.h`) | V |
| 16 | `CEventPlayerCommandToGroupAttack::GetEventPriority` (ranura 2) | 0x5F6400 | 6 | idem | V |
| 17 | `CPedGroupDefaultTaskAllocatorFollowAnyMeans::Constructor` (vtable 0x86C75C; listado: CEventPlayerCommandToGroupAttack, C) | 0x5F6450 | 9 | `Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.cpp` (nuevo, + `.h`) | S |
| 18 | `CEventPlayerCommandToGroupAttack` cuerpo del destructor | 0x5F63F0 | 11 | `Events/EventPlayerCommandToGroupAttack.cpp` | S |
| 19 | `CEventPlayerCommandToGroupAttack::ScalarDeletingDestructor` (ranura 0) | 0x5F63D0 | 30 | idem | S |
| 20 | `CEventPlayerCommandToGroupAttack::CloneEditable` (ranura 16) | 0x5F6370 | 89 | idem | V |
| 21 | `CTaskSimpleClimb::GetTaskType` (vtable 0x87059C, ranura 4) | 0x67A1C0 | 6 | `Tasks/TaskTypes/TaskSimpleClimb.cpp` (+ `.h`) | V |
| 22 | `CTaskSimpleJetPack::ScalarDeletingDestructor` (0x8705C4; listado: CTaskSimpleClimb) | 0x6801D0 | 30 | `...TaskSimpleJetPack.cpp` | S |
| 23 | `CTaskSimpleClimb` cuerpo del destructor (marco SEH) | 0x67A1D0 | 116 | `...TaskSimpleClimb.cpp` | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Tipo S = `RH_ScopedNamedGlobalInstall`. Las 24 filas nuevas de `docs/hooks.csv` siguen el formato `Clase,Fn,0xADDR8digitos,1,0,1,S|V`.

## Funciones saltadas
Ninguna. `X87Math.cpp` no se toca y no hay opciones de compilacion nuevas en `source/CMakeLists.txt`.

## Revision contra el EXE (desensamblado con `dumpbin /disasm` y busqueda de referencias en el PE)
- **Destructores con borrado (15 de tareas + 0x5F63D0)**: `push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je ..; push esi; call <liberacion>; add esp,4; mov eax,esi; pop esi; ret 4`, ranura 0 de cada vtable. Liberan con `CTask::operator delete` (0x61A5B0) o, el de evento, `CEvent::operator delete` (0x4B5630); solo cuenta el bit 0 del BYTE de banderas; devuelven `this`. Cuerpos: 0x65B9D0, 0x65BDB0, 0x65C4D0, 0x65C9A0, 0x65CD10, 0x6317C0, 0x631980, 0x631B50, 0x631D00, 0x631DA0, 0x633EF0, 0x634240, 0x634500, 0x6346A0, 0x634720, 0x67E600 y 0x5F63F0. Todos tienen hook salvo **0x631980** (`mov [ecx],86DFC4h; jmp 0x61A3D0`: la DLL usa el destructor generado de la clase, como dice su cabecera) y **0x634500** (`CTaskSimpleTriggerLookAt`, marco SEH con vtable 0x86E3CC, `CleanUpOldReference` de +8 y `~CTask`): la clase no define su constructor, asi que la DLL no emite su vtable; se engancha con `RH_ScopedClass` (sin `RH_ScopedVirtualClass`, que abortaria buscando la vtable), se activa su `InjectHooks` (estaba comentado en `InjectHooksMain.cpp`) y la DLL llama al cuerpo por su direccion del EXE con `plugin::CallMethod<0x634500>`. En el resto, una envoltura `__declspec(noinline)` llama al destructor de la clase (y se exporta `Lote157ABody_<Clase>` para el banco).
- **0x5F6360 / 0x5F6400 / 0x67A1C0**: `mov eax,47h` (EVENT_PLAYER_COMMAND_TO_GROUP), `mov eax,2Ch` (prioridad 44) y `mov eax,0FEh` (TASK_SIMPLE_CLIMB); pasan de inline en la cabecera a `.cpp`.
- **0x5F6370** (`CloneEditable`, 89 bytes, marco SEH): `CEvent::operator new(0x1C)`; si es nulo devuelve 0; si no, `0x4B23D0(ecx=nuevo, 0, [this+18h])`, `[nuevo]=0x86C710` y devuelve el nuevo. Para llamar al constructor se hizo publico `CEventPlayerCommandToGroup::Constructor`.
- **0x5F63F0**: `mov [ecx],86C710h; jmp 0x4B2470` (cuerpo de `~CEventPlayerCommandToGroup`, con hook); su cuerpo pasa de espacio anonimo a `CEventPlayerCommandBody` para poder llamarlo.
- **0x5F6450**: `mov eax,ecx; mov [eax],86C75Ch; ret`, constructor de `CPedGroupDefaultTaskAllocatorFollowAnyMeans` (clase solo con vtable; unico llamador 0x851275, inicializador estatico en 0xC09908). Hubo que crear su `.cpp`.
- **0x67A1D0**: vtable 0x87059C, `CleanUpOldReference(+24h)`, `SetDeleteCallback(+2Ch, DefaultAnimCB 0x4D1390, 0)` con `[+2Ch]=0`, `~CTask` 0x61A3A0. El destructor generado dejaria la vtable de la DLL; funcion libre `CTaskSimpleClimbDestructorBodyExe`. Su unico llamador es 0x67DBC3, el destructor con borrado de `CTaskSimpleClimb` (0x67DBC0), que **no** estaba en el encargo.

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (A, B, C; ventana de 4 KiB con guardas de 0x40). Sin memoria global del juego. Falsos identicos en el EXE y en la DLL (`F`/`H`/`Rt`/`U`): cuerpos de destructor, `CTask::operator delete`, `CEvent::operator delete`, `CleanUpOldReference` 0x571A00, `SetDeleteCallback` 0x4CEBC0, `~CTask` 0x61A3A0, `CEvent::operator new` 0x4B5620, constructor 0x4B23D0 (se anota orden y objetivo) y cuerpo 0x4B2470. Se observa: bytes cambiados de las arenas, eventos de los falsos con todos sus argumentos, valor devuelto (this/objeto nuevo como etiqueta; EAX completo en constantes y constructor) y x87 vacia; en constructores y cuerpos, la vtable escrita queda en los bytes cambiados y en el dword previo que ven los falsos. EAX/ECX/EDX tras la llamada en `original_residue`/`candidate_residue` (informativos). 16 familias de 1024 casos por funcion (ver `Fixtures.hpp`). `python -I fixture_selfcheck.py` (sin juego) pasa los 393216 casos por el oraculo.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, 24 por ronda, condicionadas a banderas con bit 1/7 o a `this` desalineado; 2048 casos por funcion). **Las 24 funciones fallan en ambas rondas** (`negative-runs/`: ronda 1 `20261110-190430-678460` con 12241 de 49152 pares con diferencias; ronda 2 `20261110-191025-428608` con 16598); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, "restauradas 19 fuentes", tras cada ronda).

## Bancos repetidos
Ningun banco cita por direccion las 24 funciones (grep de `0x...` con/sin ceros y `...h` en `reversing/`, sin contar este banco, `_handoff`, evidencia archivada ni registros). Como el lote cambia codigo que otros bancos recorren (`PlayerCommandToGroupDestructorBody` cambia de espacio de nombres, `Constructor` pasa a publico, `InjectHooks` de `CTaskSimpleTriggerLookAt` se activa, `GetTaskType` de Climb deja de ser inline), se repite una muestra de 8 bancos que citan los cuerpos que reutiliza: `Fn107A` (0x4B2470), `Events70` (0x4B23D0/0x5F6340), `CTasks8a`, `CTasks8c`, `STasks6b`, `CTasks14b`, `CTasks15` y `CTasks61c`. Los 8 quedan **aprobados sin adaptar** y su evidencia se refresca con `reversing/_handoff/refresh_bank.py`. No repetidos (cuerpos citados por otros bancos): `STasks8a`, `CTasks7a`, `STasks6a`, `STasks6c`, `CTasks61b`, `CTasks7b`, `CppTrigClimb`, `CppTrigJetPack`, `Fn151A`.

## Repeticion del 11/11/2026 (lote 163A)
El lote 163A engancha con RH_ScopedNamedGlobalInstall el cuerpo 0x634500 y 0x631980 (lote 163A los engancha), que este banco trataba como colaborador sin hook. Se adapto solo el banco (colaborador con hook, `H`: falso en la direccion del EXE y en la funcion de la DLL; fila `True` en `STUBBED`; `DLL_BODY_WITHOUT_HOOK` vacio) y se repitio entero: **APROBADO**, cero diferencias. Evidencia: `reversing/CppTwelveLevel6Final/runs/Fn157A-20261111-160003-145843`.
