<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11770 con evidencia histórica y 671 pendientes de validación en 24 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn202A: 32 funciones del EXE (lote 202A)

Estado: **APROBADO dentro del juego** (16/11/2026, worktree `wv-5` desde main 23938b22f1 + el commit del lote 202A (cherry-pick de 7d697593e5 de `reconstruccion-sin-validar`) + la correccion de fuente de abajo; plantillas `Fn179A`, `Fn173A` y `Fn171A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn202a/20261116-121546-093994` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn202A-20261116-121546-093994`). 32 x 16384 = **524288 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, con una FPU x87 por software en `x87soft.py`). SHA256 de la sonda ejecutada: `14594745fc7cb1de3b73d88097c9a325108d210124acc4cbf6fb561f488e57db`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Detalle de las funciones y riesgos: `reversing/PendienteValidar/202A.md` (commit del lote). El lote no salta ninguna funcion. Las clases del listado de entrada eran en su mayoria incorrectas: los nombres reales salen del desensamblado y de las vtables.

| # | Funcion real (el listado decia) | Entrada | Tipo |
|---|---|---|---|
| 0 | `CEventVehicleCollision::GetLifeTime` (0) | 0x4AC900 | V |
| 1 | `CEventVehicleCollision::GetEventPriority` (58) | 0x4AC910 | V |
| 2 | `CEventVehicleCollision::TakesPriorityOver` (true) | 0x4AC980 | V |
| 3-13 | `Clone`/`CloneEditable` de `CEventPedCollisionWithPed`, `CEventObjectCollision`, `CEventBuildingCollision`, `CEventDraggedOutCar`, `CEventDeath` (en linea), `CEventDeadPed`, `CEventGroupEvent` (Clone virtual del evento interno), `CEventPotentialGetRunOver`, `CEventPotentialWalkIntoVehicle`, `CEventPotentialWalkIntoObject`, `CEventPotentialWalkIntoPed` (todas decian CEventVehicleCollision) | 0x4B6C40..0x4B7090 | V |
| 14-20 | tarea de la vtable 0x86D594 (TASK_SIMPLE_ANIM_LOOPED_MIDDLE; decian CTaskSimpleRunTimedAnim): constructor por identificador, por nombre, StartAnim, Clone, destructor con borrado, cuerpo del destructor y ProcessPed | 0x61AC90, 0x61AD10, 0x61ADF0, 0x61B890, 0x61BC80, 0x61BCA0, 0x61BCB0 | S |
| 21-25 | `CTaskSimplePlayHandSignalAnim`: GetTaskType (V), cuerpo del destructor (S), MakeAbortable (V), Clone (V), destructor con borrado (S) | 0x61AEA0, 0x61AEB0, 0x61AF50, 0x61B980, 0x61BD80 | V/S |
| 26, 27 | `Interior_c::Constructor` e `InteriorGroup_c::Constructor` (decia Interior_c) | 0x5921D0, 0x597FE0 | S |
| 28, 29 | `CMatrix::ConvertToEulerAngles` y `ConvertFromEulerAngles` (decian Interior_c) | 0x59A840, 0x59AA40 | S |
| 30, 31 | `CAttractorScanner::ScanEffect` y `GetBestEffect` | 0x5FFFD0, 0x600180 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Las 32 filas pasan a `validated=1`.

## Lo que encontro el banco y se corrigio
- **`TASK_SIMPLE_ANIM_LOOPED_MIDDLE::ProcessPed` (0x61BCB0)**: con tiempos denormales la DLL daba 0 donde el EXE guarda el denormal (el producto `tiempoTotal * fraccion` y las comparaciones se compilaban con SSE, y D3D deja FTZ/DAZ en MXCSR). Corregido con dos ayudantes x87 de `X87Math.cpp`: `FloatTimesFloatCpp` (ya existia) para los dos productos y `LoopedMiddleCrossedLoopEndCpp` (nuevo, declarado en `X87Math.h`) para `actual > fin` y `(actual - paso) <= fin`. Un primer intento, compilar todo el archivo con `/arch:IA32`, se descarto: la x87 silencia las NaN senalizantes al pasar `float` por pila y `StartAnim` pasaba a diferir del EXE en la mezcla.
- Sin diferencias en las otras 31 funciones.

## Notas de alcance
- El destructor de la matriz local de `ScanEffect`: el EXE llama a 0x59ACD0 y la DLL a `~CMatrix` (que no es el cuerpo enganchado); no se falsea ninguno (corren reales, la matriz local esta a cero).
- `StartAnim` pasa a `SetFinishCallback` 0x61A8A0 en el EXE; la DLL pasa su reconstruccion de `CTaskSimpleAnim::FinishRunAnimCB` (0x61A8A0 no tiene hook). El banco anota la misma etiqueta para ambas.
- Pool de eventos: el EXE reserva con la llamada directa a `CEventPool::New` (0x4B5570) y la DLL por `CEvent::operator new` (0x4B5620); el falso de 0x4B5570 (EXE) y el del cuerpo de 0x4B5620 (DLL) anotan el mismo evento.
- Oraculo: en `ConvertToEulerAngles` (838 filas) y `ConvertFromEulerAngles` (2384 filas) las filas con NaN, infinitos o angulos de mas de 2^20 no tienen modelo exacto (fsin/fcos fuera de rango, NaN de dos origenes): solo se comparan EXE contra DLL.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (constructores de los eventos, `CTask::operator new/delete`, constructor base 0x61A390, cuerpo 0x61A6F0, `CAnimManager`, `SetFinishCallback`, `RpAnimBlendClumpGetAssociation`, `CTaskTimer::IsOutOfTime`, `SetCurrentTime`, `CWorld::Remove`, `MakeAbortable` (V), constructor 0x61AE50, y para el escaner `TransformFromObjectSpace`, `GetIndex`, `GetPedAttractorManager`, `HasEmptySlot`, `operator=`, `SetScale`, `IsApproachable`); vtables falsas en una arena para el evento interno de `CEventGroupEvent` y las manos de la tarea. Memoria global tocada y restaurada: pool de eventos 0xB744AC, contador 0xBB4A70, reloj 0xB7CB84, lluvia 0xC81324 y radios 0xC39EE0. 16 familias de 1024 casos por funcion (`Fixtures.hpp`); `python -I selfcheck_per_fn.py` pasa los 524288 casos por el oraculo y cuenta la cobertura de ramas (cada marca de `KEYS` aparece al menos 50 veces). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn202A` (con timeouts largos: la corrida principal tarda unos 45 minutos).

## Prueba negativa
`mutate.py` (32 mutaciones por ronda, condicionadas a `this` desalineado a 4 bytes, algo que el juego real no produce). **Las 32 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261116-125924-782372` (23732 de 65536 pares con diferencias) y ronda 2 `20261116-131328-738354` (24817 de 65536); el lado EXE coincide con el oraculo en todos los pares. Incidencias: una primera ronda 2 (`20261116-130426-097825`, descartada) dejo tres funciones sin diferencias porque su mutacion cambiaba un dato que los falsos no anotan (tamano de `CEvent::operator new`, argumento EDX, FLT_MAX inicial); se sustituyeron por mutaciones observables y se repitio la ronda. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas). Una primera corrida principal fallo solo en el analisis del oraculo (un dword crudo coincidia con una direccion de la pila); se corrigio el oraculo y se repitio la corrida completa.

## Fuera de los fixtures
`this` nulo o crudo, eventos y tareas reales construidos por el EXE, el pool de eventos real, animaciones, asociaciones, jerarquias y manos reales, escaneres con tipo de atractor >= 10, indices de radio fuera de 0..63, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x61AC90, 0x61AD10, 0x61ADF0, 0x61AEB0, 0x61B890, 0x61B980, 0x61BC80, 0x61BCA0, 0x61BCB0 y 0x61BD80 -> `Tasks355Evidence`; 0x5921D0 -> `InteriorAbi`, `InteriorEvidence`; 0x59A840 y 0x59AA40 -> `CamSphere76`, `CppMatrixExact1`, `Fn162A`, `MatrixOps76`; 0x5FFFD0 y 0x600180 -> `Fn92C`. Sus colaboradores pasan de codigo del EXE a hook (Tasks355Evidence, InteriorAbi/InteriorEvidence, CMatrix Euler, el escaner de atractores): candidatos a repetir en el pase final. El resto de direcciones no aparece citado.
