<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Evidencia de tareas pendientes

Las 21 funciones del lote propio tienen combo con pseudocódigo REA/Ghidra y ensamblador del EXE 1.0 US SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Los bancos son independientes y sus README indican la validación; disponer de evidencia estática no acredita aprobación de un banco.

Las entradas que Ghidra no reconocía se definieron en una sesión aislada, limpiando únicamente el intervalo completo verificado del procedimiento. Se recuperó el pseudo con REA sin cambiar EXE, fuentes o puente REA compartido. Scripts de recuperación en `_handoff/rea/rea-x86/dump-armed-next.mjs` y `dump-tasks-final-pseudo.mjs`; puente aislado en `tasks-bridge`.

| Clase | Función | Dirección | Pseudo | Banco |
|---|---|---|---|---|
| CTaskComplexArrestPed | ControlSubTask | `0x68D350` | [TasksPending.Recovered_68D350.c](../_handoff/ghidra/TasksPending.Recovered_68D350.c) | [ArrestPending](../ArrestPending/README.md) |
| CTaskComplexArrestPed | CreateNextSubTask | `0x690220` | [TasksPending.Recovered_690220.c](../_handoff/ghidra/TasksPending.Recovered_690220.c) | [ArrestPending](../ArrestPending/README.md) |
| CTaskComplexArrestPed | CreateSubTask | `0x68CF80` | [CTaskComplexArrestPed.CreateSubTask_68CF80.c](../_handoff/ghidra/CTaskComplexArrestPed.CreateSubTask_68CF80.c) | [ArrestPending](../ArrestPending/README.md) |
| CTaskComplexGoToAttractor | Clone | `0x66D130` | [TasksPending.Recovered_66D130.c](../_handoff/ghidra/TasksPending.Recovered_66D130.c) | [AttractorTaskPending](../AttractorTaskPending/README.md) |
| CTaskComplexGoToAttractor | CreateFirstSubTask | `0x670420` | [TasksPending.Recovered_670420.c](../_handoff/ghidra/TasksPending.Recovered_670420.c) | [AttractorTaskPending](../AttractorTaskPending/README.md) |
| CTaskComplexGoToAttractor | CreateNextSubTask | `0x66B6C0` | [TasksPending.Recovered_66B6C0.c](../_handoff/ghidra/TasksPending.Recovered_66B6C0.c) | [AttractorTaskPending](../AttractorTaskPending/README.md) |
| CTaskComplexKillPedOnFootArmed | CreateNextSubTask | `0x62C190` | [TasksPending.Recovered_62C190.c](../_handoff/ghidra/TasksPending.Recovered_62C190.c) | [ArmedNextPending](../ArmedNextPending/README.md) |
| CTaskComplexKillPedOnFootArmed | CreateSubTask | `0x626FC0` | [CTaskComplexKillPedOnFootArmed.CreateSubTask_626FC0.c](../_handoff/ghidra/CTaskComplexKillPedOnFootArmed.CreateSubTask_626FC0.c) | [ArmedPending](../ArmedPending/README.md) |
| CTaskComplexProstituteSolicit | ControlSubTask | `0x6669D0` | [TasksPending.Recovered_6669D0.c](../_handoff/ghidra/TasksPending.Recovered_6669D0.c) | [ProstituteControlPending](../ProstituteControlPending/README.md) |
| CTaskComplexProstituteSolicit | CreateFirstSubTask | `0x6666A0` | [CTaskComplexProstituteSolicit.CreateFirstSubTask_6666A0.c](../_handoff/ghidra/CTaskComplexProstituteSolicit.CreateFirstSubTask_6666A0.c) | [ProstitutePending](../ProstitutePending/README.md) |
| CTaskComplexProstituteSolicit | CreateNextSubTask | `0x666780` | [CTaskComplexProstituteSolicit.CreateNextSubTask_666780.c](../_handoff/ghidra/CTaskComplexProstituteSolicit.CreateNextSubTask_666780.c) | [ProstitutePending](../ProstitutePending/README.md) |
| CTaskComplexProstituteSolicit | CreateSubTask | `0x666360` | [CTaskComplexProstituteSolicit.CreateSubTask_666360.c](../_handoff/ghidra/CTaskComplexProstituteSolicit.CreateSubTask_666360.c) | [ProstitutePending](../ProstitutePending/README.md) |
| CTaskGangHasslePed | ControlSubTask | `0x65FFE0` | [TasksPending.Recovered_65FFE0.c](../_handoff/ghidra/TasksPending.Recovered_65FFE0.c) | [GangHasslePending](../GangHasslePending/README.md) |
| CTaskGangHasslePed | CreateFirstSubTask | `0x664380` | [TasksPending.Recovered_664380.c](../_handoff/ghidra/TasksPending.Recovered_664380.c) | [GangHasslePending](../GangHasslePending/README.md) |
| CTaskGangHasslePed | CreateNextSubTask | `0x6642C0` | [TasksPending.Recovered_6642C0.c](../_handoff/ghidra/TasksPending.Recovered_6642C0.c) | [GangHasslePending](../GangHasslePending/README.md) |
| CTaskSimpleGangDriveBy | Clone | `0x6236D0` | [TasksPending.Recovered_6236D0.c](../_handoff/ghidra/TasksPending.Recovered_6236D0.c) | [DriveByPending](../DriveByPending/README.md) |
| CTaskSimpleGangDriveBy | MakeAbortable | `0x62D290` | [TasksPending.Recovered_62D290.c](../_handoff/ghidra/TasksPending.Recovered_62D290.c) | [DriveByPending](../DriveByPending/README.md) |
| CTaskSimpleGangDriveBy | ProcessPed | `0x62D3B0` | [TasksPending.Recovered_62D3B0.c](../_handoff/ghidra/TasksPending.Recovered_62D3B0.c) | [DriveByPending](../DriveByPending/README.md) |
| CTaskUtilityLineUpPedWithCar | GetLocalPositionToOpenCarDoor | `0x64FC10` | [CTaskUtilityLineUpPedWithCar.GetLocalPositionToOpenCarDoor_64FC10.c](../_handoff/ghidra/CTaskUtilityLineUpPedWithCar.GetLocalPositionToOpenCarDoor_64FC10.c) | [LineUpPending](../LineUpPending/README.md) |
| CTaskUtilityLineUpPedWithCar | GetPositionToOpenCarDoor | `0x650A80` | [CTaskUtilityLineUpPedWithCar.GetPositionToOpenCarDoor_650A80.c](../_handoff/ghidra/CTaskUtilityLineUpPedWithCar.GetPositionToOpenCarDoor_650A80.c) | [LineUpPending](../LineUpPending/README.md) |
| CTaskUtilityLineUpPedWithCar | ProcessPed | `0x6513A0` | [CTaskUtilityLineUpPedWithCar.ProcessPed_6513A0.c](../_handoff/ghidra/CTaskUtilityLineUpPedWithCar.ProcessPed_6513A0.c) | [LineUpProcessPending](../LineUpProcessPending/README.md) |
