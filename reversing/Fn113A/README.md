<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 07/11/2026 (Ciudad de México):** 865 grupos / 9350 funciones reconstruidas; 9350 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn113A: diecisiete funciones de tareas, eventos, pools y registro de relaciones (lote 113A)

Estado: **APROBADO dentro del juego** (06/11/2026, worktree `wv-3` sobre main 4275e5247 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn113a/20261106-161805-962955` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn113A-20261106-161805-962955`, `comparisons.jsonl.gz` de 21,8 MB). Banco par a par dentro del juego, 17 x 16384 = 278528 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 (pila vacia) igual, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).
Diecisiete funciones pequenas de codigo de juego (zona 0x600000-0x74FFFF de `docs/JuegoExe/pendientes-A.md`), ninguna con hook antes (comprobado en `docs/hooks.csv`, en el codigo, en `docs/JuegoExe/lotes-en-curso.csv` y en los worktrees `wv-1`/`wv-2`), reconstruidas en C++ puro y enganchadas (tipo S). Ninguna usa `__asm`/`naked` ni necesita opciones de compilacion por archivo. Los nombres salen del codigo existente y del desensamblado (el listado atribuye por cercania); donde el EXE no tiene simbolo el metodo es nuevo con nombre propio.

| Funcion | Direccion | Bytes | Fuente | Antes |
|---|---|---|---|---|
| `CTaskComplexEnterCar::ComputeTargetDoorOppositeToFlag` | 0x63AB70 | 22 | `TaskComplexEnterCar.cpp` | C++ sin hook |
| `CTaskComplexWander::RequestDir` (nombre propio) | 0x669D30 | 22 | `TaskComplexWander.cpp/.h` | codigo en linea de `CTaskComplexSmartFleePoint` |
| `CTaskComplexWander::RequestNodesFrom` (nombre propio) | 0x669D50 | 69 | `TaskComplexWander.cpp/.h` | codigo en linea de `CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir` |
| `CTaskComplexWanderStandard::SetMinNextScanTime` (nombre propio) | 0x66B150 | 10 | `TaskComplexWanderStandard.cpp/.h` | escritura en linea en `CTaskComplexPartner` |
| `CTaskComplexPartner::GetStateOfPartnerTask` (antes ayudante anonimo) | 0x6822B0 | 33 | `TaskComplexPartner.cpp/.h` | ayudante C++ sin hook |
| `GangDrinkAnimOfPed` (funcion de `TaskComplexGangLeader.cpp`) | 0x65E6F0 | 29 | `TaskComplexGangLeader.cpp` | lambda en linea (hermana no listada de 0x65E710) |
| `GangSmokeAnimOfPed` | 0x65E710 | 29 | `TaskComplexGangLeader.cpp` | lambda en linea |
| `CTaskAllocator::operator new` | 0x69D980 | 11 | `TaskAllocator.cpp` | C++ sin hook |
| `CTaskAllocator::operator delete` | 0x69D990 | 49 | `TaskAllocator.cpp` | C++ sin hook |
| `CPedIntelligence::operator new` | 0x6074A0 | 11 | `PedIntelligence.cpp` | C++ sin hook |
| `CPedIntelligence::operator delete` | 0x6074E0 | 49 | `PedIntelligence.cpp` | C++ sin hook |
| `CPlayerRelationshipRecorder::ClearRelationshipWithPlayer` | 0x61A150 | 35 | `PlayerRelationshipRecorder.cpp/.h` | C++ sin hook y distinto del EXE (ver abajo) |
| `CPlayerRelationshipRecorder::AddRelationship` | 0x61A180 | 18 | idem | idem |
| `CPlayerRelationshipRecorder::GetRelationshipWithPlayer` | 0x61A1A0 | 37 | idem | idem |
| `CPlayerRelationshipRecorder::Flush` | 0x61A2A0 | 20 | idem | idem |
| `CEventScanner::ScanForEventsNow` | 0x6053D0 | 69 | `EventScanner.cpp/.h` | C++ sin hook |
| `CTaskTimer::GetTimeLeft` (metodo nuevo) | 0x62EB80 | 24 | `TaskTimer.cpp/.h` | sin reconstruir |

Total 603 bytes; ninguna supera 1200 bytes. Dejadas: constructores y destructores con vtabla (0x6389F0, 0x61A6C0, 0x667D50, 0x67A060, 0x684200, 0x62FB40...), los saltos de `RwHelper` (0x734EA0 y 0x734EC0, `jmp [RwEngineInstance + N]`: dependencias de RenderWare), el constructor 0x61A130 y el destructor 0x61A2C0 del registro de relaciones (sin llamadores en el EXE; la inicializacion de miembros del C++ difiere), 0x699F00 y 0x667900 (clase dudosa), 0x6FBA20 y 0x70FA70 (registros propios).

## Revision contra el EXE (desensamblado, `dumpbin /disasm` del EXE canonico)
- **0x63AB70 (thiscall, EAX)**: bit 0 del byte `+0x10` (`m_bAsDriver`) y dword `+0x14` (`m_TargetDoor`) == 8 -> 0xA; si no 0.
- **0x669D30 (thiscall, `ret 4`)**: si el **dword** del argumento es distinto del byte `+0x10` (`m_nDir`, ampliado a cero): byte = al y `+0x24 |= 2` (`m_bNewDir`). El metodo toma `uint32`.
- **0x669D50 (thiscall, `ret 4`)**: compara las cuatro palabras `+0x18`, `+0x1A`, `+0x1C`, `+0x1E` con las de la otra tarea; si alguna difiere copia los dwords `+0x18` y `+0x1C`, el byte `+0x10` y `+0x24 |= 4` (`m_bNewNodes`).
- **0x66B150 (thiscall, `ret 4`)**: dword `+0x34` (`m_nMinNextScanTime`) = argumento.
- **0x6822B0 (thiscall, EAX)**: `CTaskManager::FindActiveTaskByType` (0x681740, `ecx` = inteligencia del companero + 4, tipo = dword `+0x34`); nulo -> 0; si no `movsx` del byte `+0x59`.
- **0x65E6F0 / 0x65E710 (cdecl, EAX)**: palabra `+0x22` del ped == 0x67 (ballas2) o 0x69 (fam1) -> 0x12D / 0x12E; si no 0x12B / 0x12C (beber y fumar, femenina y masculina).
- **0x69D980 / 0x6074A0 (cdecl)**: `mov ecx,[pool global]; jmp New del pool` (0x69D8E0 / 0x605EC0). **0x69D990 / 0x6074E0**: indice = (obj - almacenamiento) / tamano, estado `|= 0x80`, y si indice < cursor (`+0xC`, con signo) cursor = indice.
- **0x61A150 / 0x61A180 / 0x61A1A0 / 0x61A2A0**: 16 entradas `{ped (dword), relacion (dword)}`, pero el EXE **solo escribe y lee el byte bajo de la relacion**: Clear borra la primera entrada cuyo ped coincide (un ped nulo coincide con entradas vacias), Add (solo si el ped de la entrada 0 es no nulo) guarda ped y el byte bajo del valor, Get devuelve AL, Flush borra las 16. **Hallazgo**: el C++ anterior escribia o devolvia el int32 completo (`Flush()` ponia a cero los cuatro bytes); se corrigio (`PlayerRelationship::Flush`, `AddRelationship`, `GetRelationshipWithPlayer`; el constructor sigue poniendo la relacion entera a 0). Es el mismo detalle que ya recogia `RecordRelationshipWithPlayerRegs` (0x61A1D0).
- **0x6053D0 (thiscall, `ret 8`)**: el argumento 2 se prueba como **dword** (`test esi,esi`): el metodo toma `uint32`; si es 0 y el byte `+0xC` (temporizador del escaner de vehiculos, en `+4`) es distinto de cero: inicio = tiempo del juego, intervalo = -1, byte = 1; siempre llama a `ScanForVehiclePotentialCollisionEvents` (0x603720; `ecx` = escaner + 4, ped, entidades de la inteligencia + 0xE0, 16).
- **0x62EB80 (thiscall, EAX)**: byte `+8` distinto de cero -> `(intervalo - tiempo) + inicio` en 32 bits; si no 0.
- Cambios de fuente (ademas de los `RH_ScopedInstall`): `CTaskComplexSmartFleePoint` llama a `RequestNodesFrom` / `RequestDir`; `CTaskComplexPartner` llama a `SetMinNextScanTime` y a `GetStateOfPartnerTask`; `TaskComplexGangLeader.cpp` sustituye las lambdas por `GangDrinkAnimOfPed` / `GangSmokeAnimOfPed` (declaradas antes de `InjectHooks`, como `IsDoorInUse`); `CEventScanner::ScanForEventsNow(const CPed&, uint32)`.

## Banco
17 funciones x 16384 casos, 16 familias de 1024 (ver `Fixtures.hpp`). Arenas con guardas de 0x40 bytes: tarea o registro (this), otra tarea, ped, inteligencia, companero, su inteligencia, su tarea, cabecera del pool, almacenamiento y estados del pool.
**Colaboradores con hook RH** (apagados durante el banco): `FindActiveTaskByType` 0x681740 (funcion 4) y `ScanForVehiclePotentialCollisionEvents` 0x603720 (funcion 15) llevan **falsos identicos** en la direccion del EXE y en el cuerpo de la DLL; `CTaskAllocatorPool::New` 0x69D8E0 (funcion 7) y `CPedIntelligencePool::New` 0x605EC0 (funcion 9) corren **de verdad** en ambos lados (el EXE con su hook apagado; se comprueba que ejecuta sus bytes originales, `real_with_hook_off`). Los operator delete, los bits de las tareas, el registro y la aritmetica de 32 bits de `GetTimeLeft` corren de verdad.
Memoria real del juego (se fija y se restaura tras cada lado; hash comparado): los punteros de pool `0xB744BC` / `0xB744C0` (apuntan a un pool de arenas de capacidad 1..8) y `CTimer::m_snTimeInMilliseconds` `0xB7CB84`.
Se observa: bytes cambiados de las diez arenas, eventos de los falsos, valor devuelto (EAX, AL o puntero etiquetado), hash de las regiones y estado x87 final.
Fuera de los fixtures: punteros invalidos distintos de nulo/arena, cursores de pool fuera de -1..capacidad-1 e indices fuera del pool (la DLL tiene `assert`), borrado de una ranura ya libre, tarea del companero nula en `GetStateOfPartnerTask` (el EXE falla igual) y relaciones de mas de un byte.
Nota de proceso: la primera corrida fallo por un error del oraculo (rama del cursor de `operator delete` invertida; EXE y DLL ya coincidian); se corrigio el oraculo y se repitio completa (la aprobada es la segunda).

## Prueba negativa
Copias buenas en `D:/Original/w113_good` (SHA256: `TaskComplexEnterCar.cpp` 49f0dd99..., `TaskComplexWander.cpp` e158c368..., `TaskComplexWanderStandard.cpp` 08cdb4d0..., `TaskComplexPartner.cpp` dd9235f6..., `TaskComplexGangLeader.cpp` 63faa783..., `TaskAllocator.cpp` a56543ec..., `PedIntelligence.cpp` ee2dc4fa..., `PlayerRelationshipRecorder.cpp` 3e1adf49... y `.h` a6ad6964..., `EventScanner.cpp` 48092dc3..., `TaskTimer.cpp` ccd11ed1...). Cada ronda aplica las mutaciones, compila la sonda, corre el banco completo con `negative_run.py` y restaura las fuentes comprobando SHA256 (con fecha de modificacion nueva). Informes en `negative-runs/`.
- Ronda 1 (16 mutaciones): diferencias en 16 de las 17 funciones (`20261106-163834-022172`); `GetRelationshipWithPlayer` salio igual (devolver el dword completo es invisible en AL).
- Ronda 2 (13 mutaciones distintas): diferencias en 13 funciones (`20261106-165445-390224`); no mutaba `operator new` de ambos pools, `Clear` ni `Get`.
- Ronda 3: `operator new` de TaskAllocator devuelve nulo (15437 pares), el de PedIntelligence suma 4 (16384), Clear no borra la relacion (11940) y Get devuelve el byte bajo + 1 (12003) (`20261106-172121-606770`).
**Las diecisiete funciones fallan con alguna mutacion.** Las tres rondas imprimen "APROBADO: Fn113A-negativa": es solo el aviso de que el analizador especial dejo correr todos los pares.

## Bancos repetidos (con el codigo final; evidencia archivada con `_handoff/refresh_bank.py` en `reversing/CppTwelveLevel6Final/runs/`)
Citan alguna de las direcciones (busqueda en `reversing/`): `PedIntel52` (0x61A150), `STasks6a` (0x6053D0), `CPedD` (0x6074A0 / 0x6074E0), `GangEvent1` (0x69D980 / 0x69D990) y `PartnerNext60` (0x66B150 / 0x6822B0). Ademas, por llamar a las funciones nuevas o compartir fuente modificada: `SmartFlee355` (llama a 0x669D30 / 0x669D50), `CTasks13` (0x65E730 llama a 0x65E6F0 / 0x65E710), `PartnerControl60` y `PartnerStream60` (`TaskComplexPartner.cpp` cambio). `Tasks355Evidence` solo son borradores de texto.
- **Aprobados sin cambios en el banco (7)**: `PedIntel52` (`20261106-174433-790328`), `STasks6a` (`20261106-180337-523488`), `CPedD` (`20261106-181949-187432`), `SmartFlee355` (`20261106-183445-585276`), `CTasks13` (`20261106-183709-265357`), `PartnerControl60` (`20261106-184043-279475`) y `PartnerStream60` (`20261106-185047-859616`).
- **Adaptados y despues aprobados (2)**: `PartnerNext60` daba 0x6822B0 y 0x66B150 por ayudantes del EXE sin hook (`kExeHelpers`) y fallaba la compuerta (error dentro de GTA, 0 pares): ahora se apagan con `HookRoute` (`stateRoute`, `scanRoute`) -> `20261106-185312-468567`. `GangEvent1`: ver su README (falsos de 0x44F460, 0x44F8C0 y 0x5DDB70 tambien en el cuerpo de la DLL; el fallo de `CreateDefendingGroup` era del lote 93A, no de este) -> `20261106-190439-736392`.
- No repetidos: `PartnerFirstSub31`, `PartnerDealFirst31`, `PartnerChatSeq30`, `PartnerDealSeq31`, `PartnerShoveSeq28`, `EventsTasks56`, `GangFollower355`, `CppTrigFollowPointRoute` (comparten `TaskComplexPartner.cpp` o la funcion 0x65E730 pero no citan las direcciones; el cambio en esos archivos es solo mover el cuerpo de un ayudante a un metodo y llamar a `SetMinNextScanTime`).

## Tiempo
Una corrida de los 278528 pares: unos 7 minutos (mas la compilacion incremental de la sonda, de 1 a 10 minutos segun el reparto de los candados).
