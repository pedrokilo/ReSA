<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 872 grupos / 9677 funciones reconstruidas; 9677 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn127A: veintidos destructores virtuales con borrado de 30 bytes (lote 127A)

Estado: **APROBADO dentro del juego** (08/11/2026, worktree `wv-1` sobre main 2aa6439dcf + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn127a/20261108-121623-388688` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn127A-20261108-121623-388688`; `comparisons.jsonl.gz` de 41,8 MB). 22 x 16384 = **360448 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). Sonda ejecutada SHA256 `55241ac2...9bd0`.

Modelo: `reversing/Fn126A` (mismo patron: miembro `ScalarDeletingDestructor`, hook S, falsos identicos del cuerpo del destructor y de `CTask::operator delete` 0x61A5B0). El cuerpo del destructor lleva `__declspec(noinline)` en cada `Destructor()` para que el banco pueda sustituirlo en la DLL.

| # | Clase real | Entrada | Cuerpo del destructor | vtable |
|---|---|---|---|---|
| 0 | `CTaskComplexLeaveAnyCar` | 0x421250 | 0x421270 (hook) | 0x858EB0 |
| 1 | `CTaskComplexScreamInCarThenLeave` | 0x6424F0 | 0x63C2C0 (hook) | 0x86E8D8 |
| 2 | `CTaskComplexLeaveCarAndFlee` | 0x642370 | 0x63C030 (hook) | 0x86E8AC |
| 3 | `CTaskComplexEnterBoatAsDriver` | 0x640E40 | 0x63B650 (hook) | 0x86E7D0 |
| 4 | `CTaskComplexEvasiveCower` | 0x6577D0 | 0x6554E0 (hook) | 0x86F3BC |
| 5 | `CTaskComplexKillCriminal` | 0x68DAB0 | 0x68BF30 (hook) | 0x870A00 |
| 6 | `CTaskSimpleCarSetTempAction` | 0x643760 | 0x63D730 (hook) | 0x86EA6C |
| 7 | `CTaskGangHassleVehicle` | 0x6637A0 | 0x65FB60 (hook) | 0x86F9D4 |
| 8 | `CTaskComplexUseScriptedAttractor` | 0x6390A0 | 0x633360 (hook) | 0x86E2DC |
| 9 | `CTaskComplexUseClosestFreeScriptedAttractorRun` | 0x4B8820 | 0x4B8840 (hook) | 0x85B904 |
| 10 | `CTaskComplexUseClosestFreeScriptedAttractorSprint` | 0x4B88E0 | 0x4B8900 (hook) | 0x85B930 |
| 11 | `CTaskComplexOnFire` | 0x639190 | 0x6333C0 (hook) | 0x86E308 |
| 12 | `CTaskComplexExtinguishFires` | 0x6980F0 | 0x6975F0 (hook) | 0x870DD0 |
| 13 | `CTaskComplexWalkRoundBuildingAttempt` | 0x656E50 | 0x654950 (hook) | 0x86F334 |
| 14 | `CTaskInteriorBeInOffice` | 0x676330 | 0x675260 (hook) | 0x870218 |
| 15 | `CTaskInteriorUseInfo` | 0x676860 | 0x675A90 (hook) | 0x8702E8 |
| 16 | `CTaskInteriorSitAtDesk` | 0x677760 | 0x676080 (hook) | 0x87035C |
| 17 | `CTaskInteriorGoToInfo` | 0x676620 | 0x675810 (hook) | 0x8702BC |
| 18 | `CTaskInteriorBeInHouse` | 0x6761E0 | 0x675000 (hook) | 0x8701E8 |
| 19 | `CTaskSimpleCarFallOut` | 0x64CC40 | 0x648DE0 (hook) | 0x86EFD0 |
| 20 | `CTaskSimpleCarOpenDoorFromOutside` | 0x64ACE0 | 0x645EE0 (hook) | 0x86EC9C |
| 21 | `CTaskSimpleCarCloseDoorFromOutside` | 0x64B000 | 0x646570 (hook) | 0x86ED2C |

Clases del listado corregidas por desensamblado y vtable (ver `scope_note.txt`; la ranura 0 de cada vtable es la entrada, comprobado en el EXE). Ninguna tenia hook en la entrada ni estaba en otro worktree; los veintidos cuerpos de destructor ya tenian hook RH y todas las clases tienen `.cpp`/`.h` propios (dos archivos por clase). Sin `__asm`, sin opciones de compilacion especiales (el contrato de registros no lo exige: los llamadores solo usan EAX = this).

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`): las 22 funciones fallan en ambas (`negative-runs/`, rondas `20261108-124549-730896` y `20261108-131435-389859`). Fuentes restauradas con SHA256 identico a las copias buenas (`mutate.py sha` antes y despues).

## Bancos repetidos
Ningun banco cita las veintidos direcciones de entrada (solo `TasksPendingEvidence/driveby-arrest-vtables.txt`, un dato de texto, cita 0x68DAB0): no hubo bancos que repetir ni adaptar.
