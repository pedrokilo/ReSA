<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11732 con evidencia histórica y 709 pendientes de validación en 30 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn190A: 25 funciones de eventos, tareas y sombras de stencil del EXE (lote 190A)

Estado: **APROBADO dentro del juego** (15/11/2026, worktree `wv-5` desde main 9acc2209e0 + el commit del lote 190A sin validar 0775ca2977; plantilla `Fn171A`, con `Fn156A` y `Fn149A` para los `Clone` y destructores con borrado). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn190a/20261115-100238-388559` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn190A-20261115-100238-388559`, `comparisons.jsonl.gz` de unos 52 MB). 25 x 16384 = **409600 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `75092f79df3a1bfef779f1c36c3c18e6ea4397138d5fdcd7ccb150819a7cb2e7`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El banco no encontro ninguna diferencia entre el EXE y la DLL. Las correcciones de fuente de abajo no arreglan un resultado distinto: hacen que las llamadas pasen por la direccion enganchada del colaborador (para poder poner el mismo falso en el EXE y en el cuerpo de la DLL) y que el objeto clonado quede con la vtable del EXE.

| # | Funcion real | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CEventAcquaintancePedHate::GetLifeTime` | 0x420F00 | 3 | V |
| 1 | `CEventAcquaintancePedHate::GetSourceEntity` | 0x420F10 | 4 | V |
| 2 | `CEventAcquaintancePedHate::CanBeInterruptedBySameEvent` | 0x420F20 | 3 | V |
| 3 | `CEventAcquaintancePedHate` cuerpo del destructor | 0x420F30 | 11 | S |
| 4 | `CEventAcquaintancePedHate::GetEventType` | 0x420F40 | 6 | V |
| 5 | `CEventAcquaintancePedHate::CloneEditable` | 0x420F50 | 88 | V |
| 6 | `CEventAcquaintancePedHate` destructor con borrado | 0x420FB0 | 30 | S |
| 7 | `CEventAcquaintancePedHate::GetEventPriority` | 0x420FD0 | 6 | V |
| 8 | `CEventGunShot` destructor con borrado | 0x4B58E0 | 76 | S |
| 9 | `CEventVehicleCollision` destructor con borrado | 0x4B5930 | 76 | S |
| 10 | `CEventPedCollisionWithPed` destructor con borrado | 0x4B5980 | 76 | S |
| 11 | `CEventObjectCollision` destructor con borrado | 0x4B59D0 | 76 | S |
| 12 | `CEventBuildingCollision` destructor con borrado | 0x4B5A20 | 76 | S |
| 13 | `CEventDraggedOutCar` destructor con borrado | 0x4B5A70 | 76 | S |
| 14 | `CTaskComplexKillAllThreats` constructor | 0x622180 | 41 | S |
| 15 | `CTaskComplexKillAllThreats::GetTaskType` | 0x6221B0 | 6 | V |
| 16 | `CTaskComplexKillAllThreats` cuerpo del destructor | 0x6221C0 | 11 | S |
| 17 | `CTaskComplexKillAllThreats::ScanForThreat` | 0x6221D0 | 89 | S |
| 18 | `CTaskComplexKillAllThreats::ControlSubTask` | 0x622230 | 6 | V |
| 19 | `CTaskComplexKillAllThreats` destructor con borrado | 0x6293A0 | 30 | S |
| 20 | `CTaskComplexKillAllThreats::CreateNextSubTask` | 0x6293C0 | 178 | V |
| 21 | `CTaskComplexKillAllThreats::CreateFirstSubTask` | 0x629480 | 194 | V |
| 22 | `CTaskComplexKillPedGroupOnFoot` destructor con borrado | 0x629550 | 30 | S |
| 23 | `CStencilShadows` ayuda FindForOwner | 0x710BA0 | 30 | S |
| 24 | `CStencilShadowObject::Create` | 0x711280 | 134 | S |

## Funciones saltadas (sin cambios respecto al lote sin validar)
0x70FA70 (`StencilToggleEdge`, EDI de entrada), 0x711160 y 0x7111F0 (`StencilSignedDistSq`, ESI y EBX de entrada y retorno en ST0): `__usercall` con registros que C++ puro no puede leer; su cuerpo existe como ayudante interno sin hook y sigue cubierto por `Fn113A` y `StencilReg62`.

## Correcciones de fuente hechas al validar
- `CEventAcquaintancePedHate::CloneEditable` (0x420F50): llama al `Constructor` enganchado de `CEventAcquaintancePed` (0x4AF820) y escribe la vtable 0x858E68 del EXE (antes: constructor heredado en el sitio, que dejaba la vtable de la DLL y no pasaba por la direccion enganchada).
- `CTaskComplexKillAllThreats::CreateNextSubTask` y `CreateFirstSubTask`: llaman a los `Constructor` enganchados de `CTaskComplexKillPedOnFoot` (0x620E30) y `CTaskSimpleStandStill` (0x62F310) y devuelven su EAX, como el EXE (`friend class CTaskComplexKillAllThreats` en ambas cabeceras).
- `CTaskComplexKillPedGroupOnFoot::Destructor()`: `__declspec(noinline)`, para que el falso de la DLL intercepte la llamada del destructor con borrado.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (ver `LiveComparison.hpp`): cuerpos de destructores y constructores, `CEvent::operator new/delete`, `CTask::operator new/delete`, `ScanEntities` 0x607A90, `GetColModel` 0x535300, `Malloc` 0x72F420, `RegisterReference` 0x571B70 y `UpdateHierarchy` 0x710BC0, y las funciones propias del lote 0x420F30, 0x6221C0 y 0x6221D0. Reales en ambos lados: el constructor y el destructor base de `CTaskComplex` (0x61A3B0 y 0x61A3D0; subtarea sintetica con tabla virtual de falsos; la vtable resultante se clasifica porque la de la DLL no tiene la direccion de la del EXE) y, en los seis destructores con borrado de eventos, el `operator delete` de `CEvent` EN LINEA del EXE frente a `CEvent::operator delete` de la DLL sobre un pool de eventos sintetico (puntero 0xB744AC apuntando a una estructura de la arena D con el mapa de estados en la arena C; indice con signo, cursor). Memoria global tocada y restaurada: 0xB744AC y 0xC6A16C (cabeza de la lista de sombras activas). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 409600 casos por el oraculo y mide la cobertura de ramas (minimo 50 por marca). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn190A` (la corrida tarda unos 24 min de juego; se subieron los timeouts a 9000 s en `run_local.py` y en un envoltorio propio).

Incidencias del banco (no de la fuente): una primera corrida fallo en el oraculo por un artefacto del propio banco (los cambios globales se comparaban con el estado previo a preparar el caso, no con el posterior) y otra comprobacion marcaba como pila un dword crudo aleatorio; se corrigieron en `LiveComparison.hpp` (`SnapRegions`, `TagArena`) y se repitio la corrida completa, que es la aprobada.

## Prueba negativa
`mutate.py` (25 mutaciones por ronda, condicionadas a `this` desalineado o a un propietario desalineado, que el juego no produce). **Las 25 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261115-103009-636154` con 15044 de 51200 pares con diferencias y ronda 2 `20261115-102626-029274` con 6359; el lado EXE coincide con el oraculo en todos los pares. Incidencia: la primera compilacion de la ronda 1 fallo (mutaciones de los destructores con borrado de `CTaskComplexKillAllThreats` y `CTaskComplexKillPedGroupOnFoot` usaban `self` en una funcion miembro); se corrigio y se repitio. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y sonda buena reconstruida para el manifiesto.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE, pools reales de eventos y de tareas, listas de sombras reales, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x6221B0 -> `HooksActivados` (README, texto); 0x711280 -> `StencilReg62` (usa `Create` por direccion; **repetir si el coordinador lo considera**); las saltadas 0x70FA70 -> `Fn113A` y 0x711160 y 0x7111F0 -> `StencilReg62` (ayudantes internos sin hook, siguen cubiertos). El resto solo aparece en `BinaryCoverageAudit` (inventario, no banco). Ademas, los bancos que usan los eventos y tareas tocados (CarCtrl, eventos de colision, tareas de matar) no citan las direcciones; los hooks nuevos son enganches de las mismas entradas.
