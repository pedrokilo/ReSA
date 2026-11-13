<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10424 funciones reconstruidas; 10424 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn166A: 24 funciones diminutas del EXE (lote 166A)

Estado: **APROBADO dentro del juego** (11/11/2026, worktree `wv-5` desde main 284fb34374 + los cambios del lote; plantilla `Fn161A` con piezas de `Fn153A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn166a/20261111-142142-320083` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn166A-20261111-142142-320083`; `comparisons.jsonl.gz` de 54 MB). 24 x 16384 = **393216 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia (salvo `GetLocalSoundLevel`, que deja 55.0 en ST0) y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `d27c8adcb4ddd8859c7cbdabfb2d7b426557ab46f305339ef1d9474d120e10d3`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 24 direcciones y **ninguna tenia hook** (se comprobo el codigo, `docs/hooks.csv` y las vtables de las fuentes). Las 24 son viables en C++ puro; no se salto ninguna. Las clases del listado se atribuian por cercania y **muchas eran incorrectas**: los nombres reales salen del desensamblado, de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Tipo |
|---|---|---|---|
| 0 | `CTaskSimpleFight::Clone` (CTaskComplexSeekCoverUntilTargetDead) | 0x622E40 | V |
| 1 | `CTaskSimpleFight::ScalarDeletingDestructor` | 0x6239D0 | S |
| 2 | `CTaskSimpleFightingControl::ScalarDeletingDestructor` | 0x624B30 | S |
| 3 | `CTaskSimpleIKChain::ConstructorDefaultBody` (sin llamadores; vecina de GetTaskType 0x62EC30) | 0x62EC10 | S |
| 4 | `CTaskComplexFollowPedFootsteps::GetTaskType` | 0x694EC0 | V |
| 5 | `SetMaxEntityDist2D` (mismo cuerpo que 0x4BC470, `SetMaxEntityDist2DRegs`; el listado decia FollowPedFootsteps) | 0x6955D0 | S |
| 6 | `CTaskComplexSeekEntityAnyMeansXYOffset::ConstructorExe` (desplazamiento a cero; lo usan Clone 0x696030 y 0x6967E7) | 0x695600 | S |
| 7 | `GrowableArrayPushBack` (vector que crece, ampliacion 0x7304B0 sin hook) | 0x730B30 | S |
| 8 | `ScratchDirectoryInit` (dos `operator new` y `CDirectory(0x1388)`) | 0x7315A0 | S |
| 9 | `CTxdStore::PluginAttach` | 0x731650 | S |
| 10 | `FxPrimBP_c::ConstructorBody` (el listado decia FxMemoryPool_c) | 0x4A9CF0 | S |
| 11 | `FxPrimBP_c::DestructorBody` (destruye 3 de las 4 texturas, como el EXE) | 0x4A9D20 | S |
| 12 | `CEventHitByWaterCannon::GetEventType` | 0x4B12E0 | V |
| 13 | `CEventHitByWaterCannon::GetLifeTime` | 0x4B12F0 | V |
| 14 | `CEventHitByWaterCannon::GetLocalSoundLevel` (55.0f en ST0) | 0x4B1300 | V |
| 15 | `CEventHitByWaterCannon::GetEventPriority` | 0x4B1310 | V |
| 16 | `CEventInWater::AffectsPed` (el listado decia CEventHitByWaterCannon) | 0x4B13D0 | V |
| 17 | `CEventInWater::TakesPriorityOver` | 0x4B1420 | V |
| 18 | `CTaskSimpleRunNamedAnim::StartAnim` (el listado decia CTaskSimpleRunAnim) | 0x61BB10 | S |
| 19 | `CTaskSimpleRunTimedAnim::ScalarDeletingDestructor` | 0x61BBC0 | S |
| 20 | `CTaskComplexSeekEntityStandard::ScalarDeletingDestructor` | 0x46AD50 | S |
| 21 | `CMatrix::AddTranslation` (suma de un vector a la posicion, x87 24 bits con `FloatSumStoreCpp`) | 0x46AE00 | S |
| 22 | `CTheScripts::ScriptEntryListAdd` (lista global del EXE en 0xC18DB8, clase no identificada) | 0x46AE80 | S |
| 23 | `CTaskSimpleSetCharDecisionMaker::ScalarDeletingDestructor` | 0x46AEF0 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`); S = `RH_ScopedInstall` / `RH_ScopedNamedGlobalInstall` / `RH_ScopedNamedInstall`. Filas nuevas de `docs/hooks.csv`: 24, formato `Clase,Fn,0xADDR8digitos,1,0,1,S` (o V con 0x00000000).

## Funciones saltadas
Ninguna. `X87Math.cpp` no se toca (se reutiliza `FloatSumStoreCpp` de Fn153A) y no hay opciones de compilacion nuevas.

## Correcciones respecto al codigo anterior
- `CEventInWater::TakesPriorityOver`: la DLL pedia aceleracion > 1.0 en los tres tipos (8, 9 y 0x5D); el EXE devuelve verdadero sin mirarla en 8 y 9, y solo en 0x5D la exige (con NaN no). Llama hasta tres veces a `GetEventType` del otro evento y consulta su prioridad ANTES que la propia. Reescrita segun el EXE.
- `CTaskSimpleRunNamedAnim::StartAnim`: el clump se lee crudo (`m_pRwObject`) en lugar de `GetRpClump()`, que desreferencia el objeto de RenderWare.
- `CTaskComplexSeekEntityStandard`: reutiliza `ScalarDeletingDestructorExe` de la plantilla (que llama a `DestructorBodyExe`).
- `TxdParent*` (TxdStore_Plugin.cpp) pasan de `static` a externas (el banco toma sus direcciones para normalizar los callbacks).

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (ventana de 4 KiB con guardas de 0x40). Falsos identicos en la direccion del EXE y en la funcion de la DLL: `CTask::operator new/delete` (0x61A5A0, 0x61A5B0), constructor y destructor de `CTaskSimpleFight` (0x61C470, 0x61C530), 0x61DCA0, 0x48E090, 0x46AF10 (el cuerpo del EXE es un `jmp` a `CTask::Destructor` 0x61A3A0, que tambien se apaga), 0x46AD70 (la DLL llama a `DestructorBodyExe`), 0x61A390, 0x61A3B0, `RegisterReference` 0x571B70, `CDirectory` 0x5322A0, `RwTexDictionaryRegisterPlugin` 0x7F3C10, `GetActiveTask` 0x681720, `IsPlayer` 0x5DF8F0, `IsAlive` 0x5E0170, `BlendAnimation` 0x4D4410 y `SetFinish/SetDeleteCallback` 0x4CEBE0 / 0x4CEBC0. Sin hook (un solo parche en la direccion del EXE): ampliacion del vector 0x7304B0, `operator new` 0x82119A (en otros hilos reenvia a `malloc` 0x824257) y `RwTextureDestroy` 0x7F3820. Las direcciones de callbacks de la DLL (`FinishRunAnimMovePedCB`, `FinishRunAnimCB`, `TxdParent*`) se normalizan en los falsos a las del EXE (0x61AAA0, 0x61A8A0, 0x731600, 0x731620, 0x731630); los eventos virtuales (`GetTaskType`, `GetEventType`, prioridades) usan vtables falsas estaticas. Memoria global vigilada: `ms_txdPluginOffset` (0xC88018); `CTimer::m_snTimeInMilliseconds` (0xB7CB84) se lee y se anota en la entrada del caso. Los falsos de funciones `bool` devuelven solo 0 o 1 en AL (el EXE normaliza con `setne`; la DLL no).
16 familias de 1024 casos por funcion (ver `Fixtures.hpp`). `python -I fixture_selfcheck.py` (sin juego) pasa los 393216 casos por el oraculo y mide la cobertura de ramas.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE, listas con cadenas sin terminador que se salen de la arena, capacidad cero o negativa con numero mayor en el vector que crece (bucle sin fin tambien en el EXE), y la exactitud de los colaboradores falseados (tienen sus propios bancos).

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, una por funcion y ronda; 24 por ronda; 2048 casos por funcion con `FN166A_CASES=2048`; las mutaciones se condicionan a entradas que el juego no produce: `this` desalineado, o valores crudos del banco). **Las 24 funciones fallan en ambas rondas** (`negative-runs/`: `ronda1-20261111-155701-971954` con 15570 de 49152 pares con diferencias y `ronda2-20261111-161859-685201` con 12972; el lado EXE coincide con el oraculo en todos los pares). Fuentes restauradas con `mutate.py restore` ("restauradas 17 fuentes", SHA256 igual a las copias buenas). Copias buenas (SHA256): Matrix.cpp aa1bc1ba, EventHitByWaterCannon.cpp 2bacccd7, EventInWater.cpp bd91438e, FxPrimBP.cpp 900c7ef8, D3DResourceSystem.cpp e20969ae, TheScripts.cpp ed5afaf1, TaskComplexSeekEntity.h f23559f7, TaskComplexSeekEntityAnyMeansXYOffset.cpp 6ae416ff, TaskComplexSeekEntityXYOffset.cpp 94685078, TaskComplexFollowPedFootsteps.cpp 573077b6, TaskSimpleFight.cpp b802140c, TaskSimpleFightingControl.cpp 719e0bf3, TaskSimpleIKChain.cpp 3081802b, TaskSimpleRunNamedAnim.cpp 519b3557, TaskSimpleRunTimedAnim.cpp 8ce217cb, TaskSimpleSetCharDecisionMaker.cpp 725966ca, TxdStore_Plugin.cpp a7f54fbb.

## Bancos que citan las direcciones (no se repiten: el coordinador hace un unico pase final)
Busqueda de `0x...` con y sin ceros y de la forma `...h` en `reversing/` (sin contar este banco, `_handoff`, `artifacts`, la evidencia archivada ni los registros): `Tasks355Evidence` (0x6955D0), `CTasks43` (0x695600), `AppBatch` (0x731650, como funcion llamada por el arranque), `RequestedFxEmitterTwo` (0x4A9CF0 y 0x4A9D20), `CTasks61c` (0x61BB10) y `MEJORAS_DEL_PROCESO.md` (0x46AD50, solo texto). Los otros 17 no aparecen en ningun banco. Ademas el lote cambia codigo que otros bancos recorren (`CEventInWater::TakesPriorityOver`, `StartAnim`, `CTaskSimpleFight`, los destructores con borrado): el coordinador los cubre en el pase final.
