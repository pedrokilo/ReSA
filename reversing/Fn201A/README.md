<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn201A: 32 funciones del EXE (lote 201A, validacion del lote reconstruido sin validar)

Estado: **APROBADO dentro del juego** (16/11/2026, worktree `wv-6` desde origin/main 23938b22f1 + cherry-pick de bff1058af8 de la rama `reconstruccion-sin-validar` + correcciones de fuente; plantillas `Fn197A`, `Fn173A` y `Fn194A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn201a/20261116-095955-721144` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn201A-20261116-095955-721144`, `comparisons.jsonl.gz`). 32 x 16384 = **524288 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `b5728448ffbdaaebd925e3cdb61e70509ec1dd8bc5882baafbe41847bd1c9ee2`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Las 32 direcciones del lote estaban sin hook (`reversing/PendienteValidar/201A.md` en el commit del lote). Se validan las 32; no se excluye ninguna.

| # | Funcion real (el listado decia) | Entrada | Tipo |
|---|---|---|---|
| 0-9 | `ArrayPairReset` diez copias muertas de la plantilla de vaciado de un bloque de dos punteros (CPlaceable) | 0x54FC40, 0x54FC90, 0x54FCE0, 0x54FD30, 0x54FD80, 0x54FDD0, 0x54FE20, 0x54FE70, 0x54FEC0, 0x54FF10 | S |
| 10 | `CTaskComplexSeekEntityAnyMeansXYOffset` destructor con borrado | 0x6956B0 | S |
| 11 | `CTaskComplexSeekEntityAnyMeansXYOffset` cuerpo del destructor | 0x6956D0 | S |
| 12 | `CTaskComplexFollowPedFootsteps` destructor con borrado (el listado decia XYOffset) | 0x696260 | S |
| 13 | `CTaskComplexFollowLeaderInFormation::CreateSubTask` (el listado decia XYOffset) | 0x6962A0 | S |
| 14-17 | `CEventVehicleOnFire` GetEventType, GetLifeTime, GetLocalSoundLevel, GetEventPriority | 0x4B1130, 0x4B1140, 0x4B1150, 0x4B1160 | V |
| 18-29 | `Clone`/`CloneEditable` de HitByWaterCannon, InWater, StuckInAir, CopCarBeingStolen, GotKnockedOverByCar, CarUpsideDown, PotentialWalkIntoFire, FireNearby, GunShotWhizzedBy, SeenPanickedPed, AreaCodes, Danger | 0x4B77B0 .. 0x4B7C10 | V |
| 30 | `CEvent` destructor con borrado | 0x4B7C80 | S |
| 31 | `CEventVehicleCollision::GetEventType` | 0x4AC8F0 | V |

## Aplicacion del lote sobre main
El cherry-pick dio conflicto en `docs/hooks.csv` (se conserva main y se anaden las 32 filas) y en `Placeable.cpp`: main no tiene el lote 200A, asi que solo se conserva `PlaceableArrayPairResetBody` y las diez envolturas del 201A (sin los destructores 0x54F490/0x54F5D0 ni las siete copias 0x54FA10..0x54FBF0 del 200A).

## Correcciones de fuente impuestas por la revision y el banco
- `CTaskComplexFollowLeaderInFormation::CreateSubTask` (0x6962A0): usaba `::new (memoria) T(...)` (constructor directo de C++, que no pasa por el hook) y la resta de posiciones con `operator-`. El EXE llama a los cuerpos enganchados de los constructores (0x62F310, 0x48E750, 0x640340, 0x63B8C0, 0x63C340, 0x661DC0, 0x667E20) y devuelve SU EAX, y usa `VectorSubtract` (0x40FE60) con una copia local de la posicion del miembro. Ahora usa `Constructor` (friend de la clase en las siete cabeceras y en `CTaskComplexSeekEntity`), la 0x38B opera sobre lo que devuelve el constructor y la rama 0x386 llama a `VectorSubtract`. Corregido por revision estatica antes de la primera corrida.
- `CEventInWater::Clone` (0x4B7810): el EXE copia la aceleracion con `fld`/`fstp` y un NaN senalizante sale silenciado; la DLL con SSE lo copiaba tal cual. Ahora se pone el bit 22 en C++ puro. Corregido antes de la primera corrida; el banco lo comprueba (marca `snan`, miles de casos).
- Sin diferencias en las demas funciones. La primera corrida principal se detuvo por un fallo del oraculo/banco, no de la fuente: el valor devuelto por los constructores falsos (aleatorio) se etiquetaba como puntero de pila cuando coincidia con el rango de la pila. Se paso a comparar EAX crudo en `kCloneCtor` y `kFormSub`; la corrida abortada ya tenia 524288 pares con cero diferencias EXE/DLL.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: CEventPool::New 0x4B5570 (el EXE lo llama en linea desde cada Clone; la DLL por CEvent::operator new), los nueve constructores de eventos, CEntity::CleanUpOldReference 0x571A00, CTaskComplex::Destructor 0x61A3D0, cuerpos 0x695DC0 y 0x6956D0 (esta, otra funcion probada: `Rt`), CTask::operator new/delete, los siete constructores de tareas, CountMembers/GetMember/GetLeader, VectorSubtract, Normalise, GetRadianAngleBetweenPoints, LimitRadianAngle y rand() 0x821B1E (en la DLL CGeneral::GetRandomNumber); delete[] del CRT 0x8213AE solo en la direccion del EXE. Reales (hook apagado): 0x407180 y CEvent::operator delete 0x4B5630 (contra la liberacion en linea del EXE sobre un pool sintetico en 0xB744AC). Memoria global tocada y restaurada: el puntero del pool de eventos 0xB744AC. 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 524288 casos por el oraculo (cobertura minima 50 por marca y funcion). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn201A` (o la sesion unica de `D:\Original\session201A.sh`).

## Prueba negativa
`mutate.py` (32 mutaciones por ronda, condicionadas a `this`/`self` desalineado). **Las 32 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261116-103914-591396` (11848 de 32768 pares con diferencias, 1024 casos por funcion) y ronda 2 `20261116-104229-968975` (10775 de 32768); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y sonda buena recompilada.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos por el EXE (eventos, tareas, peds, grupos y pools reales), la rama 0x38B de `CreateSubTask` con `CTask::operator new` sin memoria (el EXE escribiria en nulo + 0x44), la aritmetica real de VectorSubtract, Normalise y los angulos (falseados) y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x6962A0 -> `FormationFollow355` (banco de CreateNextSubTask/CreateFirstSubTask/ControlSubTask de formacion; llama a CreateSubTask, ahora con hook y reescrita: **repetir**) y `Tasks355Evidence` (`make_formation_bank.py`, mismo banco). El resto de direcciones no aparece citado. Los bancos que usan como colaboradores los constructores de eventos (0x4B1490 ... 0x4B2600), 0x4B5570 o los constructores de tareas no cambian (no se tocaron esas funciones), salvo que los siete constructores de tareas pasan a tener `friend` nuevo (sin cambio de codigo).
