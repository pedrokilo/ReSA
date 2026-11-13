<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 902 grupos / 10558 funciones reconstruidas; 10558 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn128A: veintiun destructores virtuales con borrado de 30 bytes (lote 128A)

Estado: **APROBADO dentro del juego** (08/11/2026, worktree `wv-2` sobre main 2aa6439dcf + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn128a/20261108-134508-130484` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn128A-20261108-134508-130484`; `comparisons.jsonl.gz` de 39,9 MB). 21 x 16384 = **344064 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `79f05cc4e1e5dbfef9506c2fbfa4e82d2da2ec8292592bf41e96de57571d6a90`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Modelo: `reversing/Fn126A` (mismo patron: `ScalarDeletingDestructor`, hook S, falsos identicos del cuerpo del destructor y de `CTask::operator delete` 0x61A5B0).

| # | Clase real | Entrada | Cuerpo del destructor | vtable |
|---|---|---|---|---|
| 0 | `CTaskSimpleCarGetIn` | 0x64B2B0 | 0x646710 (hook) | 0x86ED50 |
| 1 | `CTaskComplexCarDrive` | 0x642F80 | 0x63CA40 (hook) | 0x86E934 |
| 2 | `CTaskSimpleCarDriveTimed` | 0x46F670 | 0x46F690 (hook) | 0x859E50 |
| 3 | `CTaskSimpleCarShuffle` | 0x64B3C0 | 0x646890 (hook) | 0x86ED74 |
| 4 | `CTaskSimpleCarOpenLockedDoorFromOutside` | 0x64ADE0 | 0x6460F0 (hook) | 0x86ECC0 |
| 5 | `CTaskSimplePickUpBike` | 0x64AE70 | 0x646230 (hook) | 0x86ECE4 |
| 6 | `CTaskSimpleCarGetOut` | 0x64BDC0 | 0x647260 (hook) | 0x86EE4C |
| 7 | `CTaskSimpleCarSetPedInAsDriver` | 0x64B930 | 0x647170 (hook, funcion global RH) | 0x86EE28 |
| 8 | `CTaskSimpleCarSlowBeDraggedOut` | 0x64C060 | 0x6482C0 (hook) | 0x86EF0C |
| 9 | `CTaskComplexEvasiveDiveAndGetUp` | 0x655FE0 | 0x653740 (hook) | 0x86F268 |
| 10 | `CTaskSimpleThrowProjectile` | 0x6259C0 | 0x61F700 (hook) | 0x86D790 |
| 11 | `CTaskComplexStuckInAir` | 0x67FA60 | 0x67BA70 (sin hook, funcion propia) | 0x870608 |
| 12 | `CTaskSimpleHurtPedWithCar` | 0x656AE0 | 0x653F30 (hook) | 0x86F2E4 |
| 13 | `CTaskSimpleNone` | 0x4635F0 | 0x463560 (sin hook propio, funcion propia; el EXE lo comparte con `CTaskSimpleLeaveGroup`, que ya lo engancha) | 0x859CFC |
| 14 | `CTaskSimpleCarCloseDoorFromInside` | 0x64AF30 | 0x646380 (hook) | 0x86ED08 |
| 15 | `CTaskSimpleIKChain` | 0x636F00 | 0x633A90 (hook) | 0x86DB48 |
| 16 | `CTaskSimpleSay` | 0x48E4C0 | 0x48E4E0 (hook) | 0x85A1DC |
| 17 | `CTaskSimpleFacial` | 0x692E30 | 0x690CB0 (sin hook, funcion propia) | 0x870AB8 |
| 18 | `CTaskSimpleSitIdle` | 0x637A40 | 0x6311B0 (hook) | 0x86DF18 |
| 19 | `CTaskSimpleStandUp` | 0x637D40 | 0x631320 (hook) | 0x86DF3C |
| 20 | `CTaskComplexSitDownThenIdleThenStandUp` | 0x637D90 | 0x6314A0 (hook) | 0x86DF60 |

Las clases del listado se corrigen por desensamblado y vtable (ver `scope_note.txt`; el listado las atribuia por cercania). Ninguna de las 21 entradas tenia hook ni estaba en otro worktree; todas tenian `.cpp`/`.h` propios. Los tres cuerpos sin hook (`mov dword [ecx],VTABLE; jmp CTask::Destructor`, 0x61A3A0 o 0x61A3D0) son funciones propias `noinline` que escriben la vtable del EXE y llaman al cuerpo base con hook; cada `.cpp` exporta su direccion (`Lote128A<Clase>DestructorBody`) para el banco. Las 17 envolturas `Destructor()` pasan a `__declspec(noinline)` (y `CarSetPedInAsDriverDestructorBody`) para que el falso del cuerpo de la DLL no quede expandido en linea.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`): las 21 funciones fallan en ambas (`negative-runs/`, rondas `20261108-130022-131130` y `20261108-132912-958583`). Fuentes restauradas con SHA256 identico, sonda recompilada y corrida final repetida con ella (la sonda de la corrida aprobada es la reconstruida despues de restaurar).

## Bancos repetidos
Ningun banco cita las 21 direcciones de entrada (solo los CSV de auditoria de cobertura binaria): no hubo bancos que repetir ni adaptar.
