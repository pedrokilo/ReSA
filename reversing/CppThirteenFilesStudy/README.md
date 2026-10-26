<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ensamblador x87 en trece archivos de tareas (12/10/2026)

Conversión del único bloque de ensamblador de trece archivos de `Tasks/`. Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md). Continúa el [lote anterior](../CppTenFilesStudy/README.md).

## Qué se hizo

| Archivo / función | Fragmento original | Solución en C++ |
| --- | --- | --- |
| `TaskAllocatorKillThreatsBasic.cpp`, `ClosestThreatDistance` (`ComputeClosestPeds`, 0x69C850) | `(dz² + dx²) + dy²` sin redondear | `DistanceSquaredZXYCpp` (modo `precise`) |
| `TaskComplexDestroyCarMelee.cpp`, `CalculateSearchPositionAndRanges` (0x6289F0) | `fpatan` de (dx, -dy) | `TrigAtan2Cpp` |
| `TaskComplexFleePoint.cpp`, posición de huida (0x65B460) | `fcos` / `fsin` | `TrigSimpleCosCpp` / `TrigSimpleSinCpp` |
| `TaskSimpleAchieveHeading.cpp`, `SetUpIK` (0x667F20) | `fsin; fadd st,st; fadd pos` y `fcos` igual | `TrigCompoundSinMulAdd/CosMulAdd(h, 2.0f, pos)` |
| `TaskSimpleCarDrive.cpp`, `X87Sin` (`UpdateBopping`, 0x63C900) | `fsin` | `TrigSimpleSinCpp` |
| `TaskSimpleCarJumpOut.cpp`, `ProcessPed` (0x64DD60) | `call 0x822130 (_CIpow)`, `fmul`, `fst`, `fmul paso`, `fadd` | `PowDecaySpeedCpp` |
| `TaskSimpleChoking.cpp`, `ProcessPed` (0x620490) | `fpatan` de (-dx, dy) | `TrigAtan2Cpp` |
| `TaskSimpleGunControl.cpp`, `GunControlHeading` | `fchs; fpatan` | `TrigAtan2Cpp(-x, y)` |
| `TaskSimpleIKChain.cpp`, `ProcessPed` (0x633C80) | `fild; fidiv` | `IntRatioCpp` |
| `TaskSimpleJump.cpp`, `Launch` (0x679B80) | `fsin·v`, `fcos·v` | `TrigSimpleSinMulCpp` / `TrigSimpleCosMulCpp` |
| `TaskSimpleStealthKill.cpp`, `StealthKillAtan2` (`ProcessPed`, 0x62E540) | `fpatan` | `TrigAtan2Cpp` |
| `TaskSimpleThrowControl.cpp`, `ProcessPed` (0x61F9F0) | `fchs; fpatan` | `TrigAtan2Cpp(-x, y)` |
| `TaskSimpleThrowProjectile.cpp`, `ProcessPed` (0x62AF50) | `fild; fmul 0.05f` | `IntTimesFloatCpp` (ya existía) |

Cuatro helpers nuevos en el único módulo `X87Math.cpp/.h`: `TrigAtan2Cpp`, `DistanceSquaredZXYCpp`, `PowDecaySpeedCpp` e `IntRatioCpp`; el resto reutiliza los existentes.

## Comprobaciones en los objetos reales

Desensamblado de `X87Math.obj`: `TrigAtan2Cpp` es `fld y; fld x; fpatan` sin almacenamientos intermedios; `IntRatioCpp` es `fild; fidiv; fstp; fld`; `DistanceSquaredZXYCpp` es tres `fsub` con memoria, tres productos y las dos sumas en el orden (z² + x²) + y² antes de un único `fstp`; `PowDecaySpeedCpp` llama a `__CIpow` con la base como `double` y encadena `fmul; fst; fmul paso; fadd; fstp` (el compilador vuelca `paso` y la velocidad como `double` exactos a la pila).

## Revalidación

Bancos históricos completos con `herramientas/rerun_hist.py`; evidencia en `reversing/CppThirteenFilesUnified/runs/` y, para los dos bancos corregidos, `reversing/CppBankFixes2/runs/`. Línea base (código de `HEAD`) en `reversing/CppThirteenFilesBaseline/runs/`.

| Banco | Función(es) afectadas | Pares | Resultado |
| --- | --- | ---: | --- |
| CTasks6a | `ComputeClosestPeds` (vía `ProcessGroup` y `AllocateTasks`) | 14784 | aprobado tras corregir el banco |
| CTasks10a | `CalculateSearchPositionAndRanges` | 15488 | aprobado |
| CTasks11 | posición de huida de `CTaskComplexFleePoint` | 12672 | aprobado tras corregir el banco |
| STasks43 | `SetUpIK` | 18880 | aprobado |
| CTasks61a | `UpdateBopping` | 26624 | aprobado |
| STasks10 | `CarJumpOut::ProcessPed` | 19136 | aprobado |
| CTasks61b | `Choking::ProcessPed` | 32256 | aprobado |
| STasks6b | `GunControl` | 18688 | aprobado |
| Ped9 | `IKChain::ProcessPed`, `ThrowProjectile` | 14720 | aprobado |
| STasks8b | `Jump::Launch` | 12608 | aprobado |
| STasks7 | `StealthKill::ProcessPed` | 19520 | aprobado |
| STasks6c | `ThrowControl`, `ThrowProjectile` | 15232 | aprobado |

## Dos bancos corregidos

CTasks6a y CTasks11 fallaban en la primera corrida y **igual con el código de `HEAD`** (línea base), con funciones que no toca este lote. Eran defectos de instrumentación: el banco interceptaba a los colaboradores sólo en las direcciones del EXE y el candidato, en C++, llama a las funciones de la DLL. No se retiró ni relajó ninguna compuerta; los colaboradores sustituidos no quedan validados por estos bancos.

- **CTasks6a** (`AllocateTasks` 846/1024, `ProcessGroup` 282/512, `CTaskComplexDieInCar::CreateFirstSubTask` 4/1024 y `PreparePedVehicleForPedDeath` 39/256): faltaban los puntos de entrada de la DLL de `CPedGroups::GetPedsGroup`, `GetGroupId`, `CInterestingEvents::Add` y `CCarCtrl::SwitchVehicleToRealPhysics`. Con ellos, 14784 pares y cero diferencias.
- **CTasks11** (error dentro de GTA, violación de acceso en `CEventSource::ComputeEventSourceType` → `CPedIntelligence::IsThreatenedBy`): el comentario del banco decía que `ComputeEventSourceType` y las dos `MakeDecision` de la DLL llamaban a la dirección del EXE, pero ya son código real, y la primera ejecutaba contra un ped falso. Ahora se interceptan también esas tres entradas de la DLL. 12672 pares, cero diferencias.

## Límites

- Sin auditor propio por banco; sin cobertura exhaustiva de x87 más allá de sus fixtures.
- `PowDecaySpeedCpp` usa `ExeCompatiblePowCpp`. Con la base fija 0.96f, el banco [PowEquivalence](../PowEquivalence/README.md) comparó **los 2^32 exponentes float** bit a bit con el `_CIpow` del EXE (0x822130) y 2^26 casos de la función completa: cero diferencias de valor. Detalle del análisis, de las diferencias de C99 encontradas y corregidas, de las banderas de excepción y de sus límites en ese README.
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| TaskAllocatorKillThreatsBasic.actual-after.obj | f085fff68c80006aabf357d7ad372a84a8fb05deeffba1c7de46b652a0aaadc7 |
| TaskAllocatorKillThreatsBasic.actual-before.obj | 4c212736d771253350665cc1c9777481af48a893b64f5604b36e42fb3275398e |
| TaskComplexDestroyCarMelee.actual-after.obj | e774f3c473dea7cd691434c04f924be2b7b1ec63516328855e93743a0b87f4d3 |
| TaskComplexDestroyCarMelee.actual-before.obj | 7cb30994e4305f749bb5c7f5d929c50b58232b1fa38ea2bc7768bc80c7227dfc |
| TaskComplexFleePoint.actual-after.obj | f93bbf51b217a1b54a7532503e782a83e48b2b0bba5968ba3a995d547e1b0473 |
| TaskComplexFleePoint.actual-before.obj | 424e2a01beda03c0982cb64b1af10fdd2046a698b15b6eb3a3f1e253c355ff0c |
| TaskSimpleAchieveHeading.actual-after.obj | e2e63f13065e4c277976fcba92349e836237937439276a6156395db9756ba483 |
| TaskSimpleAchieveHeading.actual-before.obj | 1b80c838cc3f615cc4bc60bccfd5dc91819a5d83817c6985ee4e44197a704161 |
| TaskSimpleCarDrive.actual-after.obj | 54b1eafc694e2b47999735e8be74084b7eb350a490e1746b9ae8aabcbcb3f1f5 |
| TaskSimpleCarDrive.actual-before.obj | aef840693ec60611788519b0af4daef5d1f23652f3a9eaa2307876256da07c83 |
| TaskSimpleCarJumpOut.actual-after.obj | 804a1df00c27c9b3276aad96f3f6ab485d6bf03ef63adf6fc670c71fbfac819e |
| TaskSimpleCarJumpOut.actual-before.obj | 5ac58066e83c5b0fd7d51e47432fb6005651f0b195c24ac42c15fec6653c0973 |
| TaskSimpleChoking.actual-after.obj | edd1e62eb377ac2ec46513084482a34426e25d0541bf9b03fc0ff7dadbc9be32 |
| TaskSimpleChoking.actual-before.obj | 384a389a40344e71a20e6d52e3a94a3712e5e55204349ec144dbfcd7c939af60 |
| TaskSimpleGunControl.actual-after.obj | 6298d1e83105cf6e32fd7ac649a07fa46905a552a39f77953f7af70d907e0830 |
| TaskSimpleGunControl.actual-before.obj | c102e2b87722322245ff55afbbfea119cdc8a5f76991ba176ff7394989f08c9e |
| TaskSimpleIKChain.actual-after.obj | e6b939a2077f96c87ee447134389720b7e761479128a39c21faf775cbe8d8331 |
| TaskSimpleIKChain.actual-before.obj | 70b71533aed522514c10bf5bc55207f4b02c1b882ce7fb1ed83340ff568afd6b |
| TaskSimpleJump.actual-after.obj | 14b441ab33871ba4afd946ce52128b296c2124da070ffef49a84e9b7329a16d7 |
| TaskSimpleJump.actual-before.obj | d33c49d90a26ba47c97a6cd235b8da702f1def361ca7ad971d1eeb9ac3eb46c4 |
| TaskSimpleStealthKill.actual-after.obj | ec698da328e7eca9023541a491e7a4a8a0a22048c9d0c5e7e3630916e6acb36b |
| TaskSimpleStealthKill.actual-before.obj | 3758aed25d29161ed7e07757097b20bd42ffc09b90dd46be7d95a89704486ca4 |
| TaskSimpleThrowControl.actual-after.obj | 501996cce332ad1d07483847543c112f5f6936eb90653ea634deef96aab016bb |
| TaskSimpleThrowControl.actual-before.obj | b0bd765769c760350be3e7011ec66c5fcc6902dbbbe3d47ee6fa1b24edf684dc |
| TaskSimpleThrowProjectile.actual-after.obj | 1d790e0b9428b6ffb0cc28c7b2a3f41d8224a0f956c22fa151880bca23d5a545 |
| TaskSimpleThrowProjectile.actual-before.obj | 754e6e7d3352f746ae9d3e908dc18ee93c6382d64011051eb203f10c607721e0 |
| X87Math.actual-after.obj | 8a27a3ebed5a6aee83270250a597879c4cac2b4b1a0a561a5951b5c94251e854 |
| X87Math.actual-before.obj | 618cc9e1c574a419ee7c238b9ebf16eafd959d37788df1cb183663c0daf89ad4 |
