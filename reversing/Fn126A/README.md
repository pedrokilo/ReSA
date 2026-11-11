<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 884 grupos / 9985 funciones reconstruidas; 9985 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn126A: trece destructores virtuales con borrado de 30 bytes (lote 126A)

Estado: **APROBADO dentro del juego** (08/11/2026, worktree `wv-2` sobre main 0512dd92cd + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn126a/20261108-112541-209748` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn126A-20261108-112541-209748`; `comparisons.jsonl.gz` de 24,6 MB). 13 x 16384 = **212992 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).

Modelo: `reversing/Fn125A` (mismo patron: `ScalarDeletingDestructor`, hook S, falsos identicos del cuerpo del destructor y de `CTask::operator delete` 0x61A5B0).

| # | Clase real | Entrada | Cuerpo del destructor | vtable |
|---|---|---|---|---|
| 0 | `CTaskComplexEnterCarAsDriverTimed` | 0x640590 | 0x63AE00 (hook) | 0x86E720 |
| 1 | `CTaskComplexStealCar` | 0x640F10 | 0x63B850 (hook) | 0x86E7FC |
| 2 | `CTaskComplexEnterCarAsPassengerTimed` | 0x640640 | 0x63B0C0 (hook) | 0x86E74C |
| 3 | `CTaskComplexEnterCarAsPassengerWait` | 0x640710 | 0x63B3C0 (hook) | 0x86E778 |
| 4 | `CTaskComplexLeaveCarAsPassengerWait` | 0x640BC0 | 0x63B4A0 (hook) | 0x86E7A4 |
| 5 | `CTaskComplexHitPedWithCar` | 0x6560C0 | 0x653A30 (hook) | 0x86F294 |
| 6 | `CTaskComplexInvestigateDeadPed` | 0x697820 | 0x696E80 (hook) | 0x870D3C |
| 7 | `CTaskComplexGangFollower` | 0x6629F0 | 0x65EBB0 (hook) | 0x86F938 |
| 8 | `CTaskSimpleCarAlign` | 0x64ABE0 | 0x645BE0 (hook) | 0x86EC78 |
| 9 | `CTaskComplexReactToGunAimedAt` | 0x697B50 | 0x697230 (hook) | 0x870D78 |
| 10 | `CTaskComplexCopInCar` | 0x68F9F0 | 0x68C8C0 (hook) | 0x870A2C |
| 11 | `CTaskSimpleStealthKill` | 0x6296B0 | 0x622680 (sin hook, funcion propia) | 0x86DA74 |
| 12 | `CTaskComplexKillPedOnFootArmed` | 0x626FA0 | 0x621250 (hook) | 0x86D918 |

Clases del listado corregidas por desensamblado y vtable (ver `scope_note.txt`). Ninguna tenia hook ni estaba en otro worktree; todas tenian `.cpp`/`.h` propios. El cuerpo 0x622680 (`~CTaskSimpleStealthKill`) no tiene hook: la DLL usa una funcion propia `noinline` exportada por el `.cpp` (`Lote126AStealthKillDestructorBody`).

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`): las 13 funciones fallan en ambas (`negative-runs/`, rondas `20261108-102723-344331` y `20261108-103606-686819`). Fuentes restauradas con SHA256 identico y sonda recompilada antes de la corrida final.

## Bancos repetidos
Ningun banco cita las trece direcciones (solo `TasksPendingEvidence/armed-vtable.txt`, un dato de texto): no hubo bancos que repetir ni adaptar.
