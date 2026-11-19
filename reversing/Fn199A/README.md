<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn199A: 30 funciones del EXE (lote 199A)

Estado: **APROBADO dentro del juego** (15/11/2026, worktree wv-6 desde main 9acc2209e0 + el lote 199A, cherry-pick de 16fc4bca8b). Evidencia: E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn199a/20261115-122946-179267 (archivada en reversing/CppTwelveLevel6Final/runs/Fn199A-20261115-122946-179267, comparisons.jsonl.gz de 67 MB). 30 x 16384 = **491520 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente. SHA256 de la sonda ejecutada: 6626f7ea0a91a9d7ecec04003e5043c099aacd378aa9fff99caa6d546d96ff49; del EXE original: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b.

## Funciones

El encargo listaba 32 direcciones atribuidas a cinco clases (CTaskSimpleBeHit, CEventVehicleHitAndRun, CPed, CCam). Las clases reales salen de las vtables y del desensamblado (`reversing/PendienteValidar/199A.md` en la rama `reconstruccion-sin-validar`, fuera de este arbol de validacion). Se reconstruyen y validan 30; 2 se saltan (ver abajo).

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CTaskComplexReactToAttack::MakeAbortable` (CTaskSimpleBeHit) | 0x620BF0 | 64 | S |
| 1 | `CTaskComplexReactToAttack::CreateSubTask` (antes llamada al EXE por direccion fija) | 0x620C30 | 451 | S |
| 2 | `CTaskComplexReactToAttack::ControlSubTask` | 0x620E00 | 41 | S |
| 3 | `CTaskComplexReactToAttack::Clone` | 0x623300 | 112 | S |
| 4 | `CTaskComplexKillPedOnFoot::Clone` | 0x623370 | 137 | V |
| 5 | `CEventVehicleHitAndRun::GetEventType` | 0x4AEA00 | 6 | V |
| 6 | `CEventVehicleHitAndRun::GetLifeTime` | 0x4AEA10 | 3 | V |
| 7 | `CEventVehicleHitAndRun::AffectsPed` | 0x4AEA20 | 12 | V |
| 8 | `CEventVehicleHitAndRun::GetLocalSoundLevel` (100.0f en ST0) | 0x4AEA30 | 7 | V |
| 9 | `CEventVehicleHitAndRun::GetEventPriority` | 0x4AEA40 | 6 | V |
| 10 | `CEventPedEnteredMyVehicle::AffectsPed` (CEventVehicleHitAndRun) | 0x4AEC00 | 65 | V |
| 11 | `CEventPedEnteredMyVehicle::CloneEditable` | 0x4B7170 | 105 | V |
| 12 | `CEventChatPartner::AffectsPed` | 0x4AEDC0 | 35 | V |
| 13 | `CEventChatPartner::Clone` | 0x4B7210 | 103 | V |
| 14 | `CEventSexyPed::AffectsPed` | 0x4AEF00 | 272 | V |
| 15 | `CEventSexyPed::CloneEditable` | 0x4B7280 | 97 | V |
| 16 | `CEventSexyVehicle::AffectsPed` | 0x4AF100 | 35 | V |
| 17 | `CEventSexyVehicle::Clone` | 0x4B72F0 | 97 | V |
| 18 | `CEventPedToChase::Clone` | 0x4B7360 | 97 | V |
| 19 | `CEventPedToFlee::Clone` | 0x4B73D0 | 97 | V |
| 20 | `CPool<CPed, CCopPed>::CreateAtRef` (CPedPool; CPed) | 0x5E0540 | 74 | S |
| 21 | `CPed::operator delete(void*, int)` | 0x5E47A0 | 49 | S |
| 22 | `CPed::CanWeRunAndFireWithWeapon` | 0x5E88E0 | 43 | S |
| 23 | `CPed` destructor con borrado | 0x5E8C80 | 76 | S |
| 24 | `CCamera` cuerpo del destructor (CCam) | 0x50A870 | 253 | S |
| 25 | `CCamera` destructor con borrado | 0x514010 | 27 | S |
| 26 | `TwoPlayerCameraVisible` (libre, stdcall, fuente por valor; CCam) | 0x513220 | 164 | S |
| 27 | `TwoPlayerCameraGeometry` (thiscall con la camara en ECX; CCam) | 0x5132D0 | 568 | S |
| 28 | `CHandShaker::Constructor` | 0x517740 | 23 | S |
| 29 | `CIdleCam::Constructor` | 0x517760 | 12 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`).

## Funciones saltadas
- **0x5E3630 y 0x5E37C0** (CPed, particulas de pisada con lluvia y con polvo): usercall con el peaton en EBX, la posicion en EDI y el numero de particulas en EAX, sin argumentos de pila; C++ no puede leer esos registros y un envoltorio naked/__asm esta prohibido. Ya estan en C++ dentro de Ped.cpp (EmitFootRainParticles, EmitFootDustParticles) y las usa CPed::DoFootLanded; no hay nada que enganchar y no se validan.

## Aplicacion del lote sobre main
El cherry-pick dio conflicto en docs/hooks.csv (se conserva main y se anaden las 30 filas del lote), Ped.cpp, PedPool.h y TaskComplexReactToAttack.cpp: la rama traia ademas trabajo de los lotes 189A (CPool<CPed>::GetAt), 193A y 198A (destructor, constructor, GetTaskType, CreateNextSubTask y CreateFirstSubTask de CTaskComplexReactToAttack) y CPed::SetKindaStayInSamePlace (198A), que NO se incluyen. Consecuencias: el constructor 0x620B00 corre como codigo real del EXE y Clone lo llama por su direccion (plugin::CallMethodAndReturn); se registra CTaskComplexReactToAttack::InjectHooks() en InjectHooksMain.cpp (con el include); CTaskComplexFallAndGetUp::Constructor pasa a publico.

## Correcciones de fuente (antes de la primera corrida, por revision contra el desensamblado)
- CCamera cuerpo del destructor (0x50A870): el EXE pone a cero +0x14 (puntero de la matriz) ANTES de destruir los miembros (`mov dword [esi+14h],0` en 0x50A8AB); el lote y su informe decian lo contrario. Sin ello, con una matriz enlazada, ~CPlaceable llamaria a RemoveMatrix (hook real) y bajaria el contador 0xB74238. Ahora se pone a cero.
- CTaskComplexReactToAttack::CreateSubTask (0x620C30): las subtareas se construian con `new T(...)` (constructor de C++ directo, vtable de la DLL y EAX = la memoria). Ahora reservan con CTask::operator new y llaman a los cuerpos enganchados (Constructor de CTaskSimpleBeHit 0x620780 y de CTaskComplexFallAndGetUp 0x6786C0; CTaskSimpleBody::Constructor 0x61A390 mas la vtable 0x859CFC para CTaskSimpleNone) y devuelven su EAX, como el EXE. La conversion del dano usa static_cast<int32>(static_cast<int64>(f)) (el _ftol del EXE es de 64 bits: con |x| >= 2^31 el static_cast<int32>(float) daba 0x80000000).
- CPed destructor con borrado (0x5E8C80): llama a Destructor() (cuerpo 0x5E8620 con hook) en vez de ~CPed() directo, para que el cuerpo sea el que el EXE llama por direccion.

## Hallazgos del banco (ninguno requirio cambiar la fuente)
La primera corrida (110354-262135) dio diferencias en dos funciones, ambas por el banco y no por la fuente: CTaskComplexKillPedOnFoot::Clone (el ultimo argumento del constructor es un int8: el EXE lo empuja con movsx y la DLL con los bits altos sin ampliar; el constructor solo lee el byte bajo; el falso anota ahora el byte con signo ampliado) y CPed::CanWeRunAndFireWithWeapon (el falso de GetWeaponSkill devolvia basura en los bytes altos de un eWeaponSkill de 8 bits; ahora devuelve un byte limpio). Dos casos del oraculo coincidian con la pila del hilo (un EAX aleatorio dentro del rango de la pila): los valores aleatorios evitan 0xC00000-0x2000000. Las otras 28 funciones coincidian en los 16384 casos, incluida la geometria de la camara de dos jugadores (bit a bit con fcos/fsin del x87).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A de 0x3000 bytes, B, C y D de 0x1000) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL para los colaboradores con hook: CTask::operator new 0x61A5A0, CEventPool::New 0x4B5570, constructores 0x620780, 0x6786C0, 0x61A390, 0x620E30, 0x4AEAC0, 0x4AECD0, 0x4AEDF0, 0x4AF010, 0x4AF130 y 0x4AF240, GetWeaponSkill 0x5E3B60, GetWeaponInfo 0x743C60, cuerpo de CPed 0x5E8620, CMatrix::Destructor 0x59ACD0, ProcessLineOfSight 0x56BA00, Normalise 0x59C910 (el falso multiplica por 0.5f), SetDefaults 0x517330, CIdleCam::Init 0x50E6D0, IKChainManager_c 0x6181A0 y 0x6181D0, FindActiveTaskByType 0x681740 y el cuerpo 0x50A870 (otra funcion probada). Sin hook: 0x620B00 y operator delete del CRT 0x8214BD (la DLL los llama por su direccion del EXE: un solo parche). Reales (hook apagado): CPed::IsAlive 0x5E0170, ~CPlaceable 0x54F490 y _ftol 0x821B40. Vtables propias para las subtareas (ranuras 4, 6 y 9). Memoria global tocada y restaurada: 0xB74490, 0xB7CD98, 0xB7CF28, 0x8CCB80 y 0xB74238 (gchanges). Los pools son falsos en arenas (con una ranura libre ancla para que la busqueda del cursor termine). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 491520 casos por el oraculo. La corrida real se hizo con un envoltorio propio (candado de juego unico, timeouts largos).

## Prueba negativa
mutate.py (30 mutaciones por ronda, condicionadas a this/self/cam/data desalineado, que el juego no produce, salvo TwoPlayerCameraVisible, condicionada a una fuente con NaN). **Las 30 fallan en ambas rondas** (negative-runs/): ronda 1 20261115-131336-269900 (20533 de 61440 pares con diferencias) y ronda 2 20261115-151845-880434 (14373 de 61440); el lado EXE coincide con el oraculo en todos los pares. Incidencias: una primera ronda 2 (143548-562708, descartada y no archivada) no compilo la mutacion 22 y, corregida, la mutacion 21 (`(CPed*)data + 1`) tumbo la partida tras la funcion 20, asi que se sustituyo por una que sale antes y se repitio; las fuentes se restauraron con `mutate.py restore` (SHA256 igual a las copias buenas). La sonda buena no se recompilo: se restauraron su ASI y su manifiesto (copias en artifacts/good_probe).

## Fuera de los fixtures
this nulo o crudo, objetos reales construidos por el EXE (eventos, tareas, peds, camaras, pools reales), ranuras de pool fuera de rango, ocupadas (CreateAtRef) o ya libres (delete): la plantilla de la DLL tiene asserts que el EXE no tiene; memoria agotada en CTaskComplexKillPedOnFoot::Clone (el EXE desreferencia nulo y falla; la DLL devuelve nulo); el tamano que la DLL pasa a CEvent::operator new (el EXE no lo pasa); posiciones de jugador de mas de 2^60, con NaN, infinitos o denormales (la x87 tiene exponente ampliado) y angulos NaN, infinitos o de mas de 2^14 en TwoPlayerCameraGeometry; la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en reversing/ (sin este banco, _handoff, evidencia archivada, registros, PendienteValidar ni BinaryCoverageAudit): 0x5E88E0 -> CamControl355, World355Evidence; 0x50A870 -> CppIntegerWorld10, DependenciesPadCamera7; 0x514010 y 0x517740 -> RequestedPadSourceProposals; 0x513220 -> CamTwoPlayer355, CppIntegerWorld10, RequestedPadSourceProposals, Tasks355Evidence, World355Evidence; 0x5132D0 -> CamTwoPlayer355, CppIntegerWorld10, Tasks355Evidence, World355Evidence (los dos ayudantes pasan de codigo de la DLL sin hook a hook directo: repetir CamTwoPlayer355). El resto de direcciones no aparece citado.

## Repeticiones
- 2026-11-18, commit base 23c47ba081 (pase final B, riesgo ALTO): **APROBADO**, 491.520 pares (30 funciones x 16384) con 0 diferencias EXE/DLL; cada lado coincide con el oraculo independiente (`fixture_oracle.py`), con las compuertas y la cobertura de ramas intactas. Informe `resultados-fn199a/20261118-183157-391818`, archivado en `reversing/CppTwelveLevel6Final/runs/Fn199A-20261118-183157-391818` (comparisons.jsonl.xz, RESULTADO.txt, summary.json). SHA256 de la sonda: `fc7b83b5d308ced8a6f258ef2001f90a7f7ec99fddaaa97b2cb6776e54d843b5`.
- Cambios del banco para esta repeticion: (1) `run_local.py` con el analizador en flujo (`stream`/`stream_common`) y `stop_before_analysis=True`; (2) **0x620B00** (constructor de `CTaskComplexReactToAttack`, lote 198A) y **0x54F490** (`CPlaceable::DestructorBody`) tienen hook S en `docs/hooks.csv` desde entonces: 0x620B00 pasa de colaborador sin hook (`E`, falso solo en el EXE) a colaborador con hook (`F`, el mismo falso en la direccion del EXE y en el cuerpo de la DLL, `hooked=True`) y 0x54F490 de real sin hook (`N`) a real con hook apagado (`R`, `hooked=True`); `LiveComparison.hpp` y `run_local.py` actualizados. Los guardias de la sonda (`ready=false` si un colaborador `E`/`N` tiene hook) lo exigen.
- `TaskComplexReactToAttack.cpp` esta compilado con `/arch:IA32 /fp:fast` (B9 de `docs/DesviacionesConocidas.md`): esta repeticion lo corre con esa compilacion y la x87 sigue vacia en todos los pares. Ninguna fuente de produccion cambio.
