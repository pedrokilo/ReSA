<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->


# Catorce archivos con un bloque de ensamblador y una función naked: once convertidos, tres que siguen naked (12/10/2026)

Lote pedido: `Models/BaseModelInfo.cpp`, `Models/ClumpModelInfo.cpp`, `PedGroupIntelligence.cpp`, `PedIntelligence.cpp`, `Plant/PlantLocTri.cpp`, `Pools.cpp`, `Ragdoll/IKChain.cpp`, `RwHelper.cpp`, `Interior/TaskInteriorSitInChair.cpp`, `SeekEntity/TaskComplexSeekEntityXYOffset.cpp`, `TaskComplexGoToCarDoorAndStandStill.cpp`, `TaskSimpleBeHit.cpp`, `TaskSimpleDead.cpp` y `TaskSimpleGoToPointFine.cpp`. Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md). Técnica del residuo de registros: [CppFourNakedStudy](../CppFourNakedStudy/README.md).

## Convertidos (11 funciones, sin relajar ningún banco)

| Función | Cómo |
| --- | --- |
| `SetBaseModelInfoFlags` / `SetClumpModelInfoFlags` (0x5B3AD0 / 0x5B3C30) | `__fastcall(void*, uint32 edxEntrada)` sin parámetros de pila; los argumentos se leen de la pila con `_AddressOfReturnAddress()`; termina en `ResidueRegsCpp(0, edxEntrada, 0)` |
| `CPedGroupIntelligence::ComputeScriptCommandTasks` (0x5F7800) | pares nuevos por plaza, borrado virtual de las tareas que quedan sin ped, copia de vuelta; `safebuffers` para evitar la comprobación de la cookie de /GS; residuo EAX/ECX/EDX con `ResidueRegsCpp` |
| `CPedPool::GetRef` (0x4442D0) | índice con la división con signo del EXE (imul 0x41EE7CA7, sar 9) y residuo de registros |
| `IKChain_c` ángulo de rotación (0x617CA0) | `IKChainRotationAngleCpp` en `X87Math` (acos del CRT del EXE en precisión completa) |
| `RwHelper::GetAnimHierarchyFromFrame` (0x734AB0) | C++ puro y un hook `__fastcall` con ECX/EDX de entrada y residuo; el banco DependenciesWorld12 se actualizó para referenciar ese hook |
| `CTaskInteriorSitInChair::Constructor` (0x675C30) | escritura de campos por desplazamiento y residuo |
| `CTaskComplexGoToCarDoorAndStandStill::IsVehicleInRange` (0x6458A0) | `__fastcall` con residuo ECX/EDX/EAX |
| `CTaskSimpleBeHit` (rumbo), `CTaskSimpleDead` (asin), `CTaskSimpleGoToPointFine::SetTargetPos` | `TrigAtan2Cpp`, `ExeAsinDoubleCpp` (nuevo en `X87Math`) y `ResidueEcx::Get` |

## Siguen naked (3) y por qué

- `CPedIntelligence::IncrementAngerAtPlayer` (0x421050): escribe sólo AL/DL; los bits altos de EAX/EDX que quedan al salir son los de entrada, y C++ no puede leer el EAX de entrada.
- `CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D` (0x4BC470): `fnstsw ax` deja la mitad alta de EAX con el valor de entrada.
- `GetPlantDensity` (0x5DC210): el argumento llega en EAX.

Los tres exigen leer o conservar un registro que el compilador no expone; ninguna técnica de C++ probada lo reproduce sin ensamblador.

## Revalidación

Bancos históricos completos con `herramientas/rerun_hist.py`; evidencia en `reversing/CppFourteenFilesUnified/runs/` y `CppFourteenFilesUnified2/runs/` (repetición de DependenciesGroupTasks14 y DependenciesWorld12).

| Banco | Pares | Resultado |
| --- | ---: | --- |
| Model34 | 13280 | aprobado |
| Clump24 | 12256 | aprobado |
| DependenciesGroupTasks14 | 2048 | aprobado en la segunda corrida (la primera falló por operar sobre los pares principales en vez de los de script) |
| DependenciesWorld12 | 2048 | aprobado en la segunda corrida (la primera falló sólo por `route_ok`: el banco registraba la función sin hook de registros) |
| DependenciesDecisionPed7 | 8192 | aprobado |
| GangWarsPending | 3072 | aprobado |
| DependenciesTaskCtor4 | 4096 | aprobado |
| STasks8a, UnlockTrackEntity | 12224, 9216 | aprobado |
| Mix11 | 19520 | aprobado |
| CTasks8c | 16896 | aprobado |
| IK20, IKDataWarnings | 11264, ver resumen | aprobado |
| FixHooks | — | falla en las comprobaciones estáticas con 16 hooks de vtable de otras clases (CTaskComplexGangFollower, CTaskGangHassleVehicle, CTaskGoToVehicleAndLean, CTaskSimpleDead::ProcessPed, CBike::ProcessAI) que el commit 0d0094be desbloqueó; el banco sigue esperándolos bloqueados. No depende de este lote y no se usa como evidencia |

`python contrib/audit_register_residue.py` revisa ahora 11 objetos (0 violaciones).

## Límites

- Misma dependencia del código que genera el compilador que en CppFourNakedStudy (la vigilan la carga de la DLL, el auditor de objetos y los bancos).
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| BaseModelInfo.actual-after.obj | 0dbb24d6c11c6148fb1eb41f9ab1243c2ff2f7bb6dfa799d5049250d147dbcc6 |
| BaseModelInfo.actual-before.obj | 193e915943ef1dd60f44bd659052dba4f6b39d768ed3c360dfb9753ce7d6aa04 |
| ClumpModelInfo.actual-after.obj | 61136b8238d05115b1878a40bbdc62ff251918b42659ced20a71f49ddd8b5855 |
| ClumpModelInfo.actual-before.obj | 252c0fb766d2ce1393b46cd7da4a0685ab2275a123ae4e9d49e3a12ec2f9aec4 |
| IKChain.actual-after.obj | 15e34056bd7da6cda94f838480f569156b49557925fa1ce2d722bf6f495e02d6 |
| IKChain.actual-before.obj | b0c0f400c5a708fe2c0ff314c870dc7675c6cffa7b24274c09f637ecbb85975e |
| PedGroupIntelligence.actual-after.obj | 650ce50ff4ae0f38f8992e864d637d5ba16ba8ceac056ca8e218d2d1b0996980 |
| PedGroupIntelligence.actual-before.obj | 34239a6dea6c008aee05c4e294344dd552c1960e4d661c3a7d58d094bb386b96 |
| PedIntelligence.actual-before.obj | 715fbf6be0a9f217aa37eccacede9509ae17c0cb67433196fa0b26c98b9a8545 |
| PlantLocTri.actual-before.obj | 593b1d86246ddf543614b36c47420ed6b6f7119d0772d6604de10a6074dae40d |
| Pools.actual-after.obj | 817a0d5f78e501228c77c1e17f9692a82ba048a273d315b5ea1ac2e3b9007f37 |
| Pools.actual-before.obj | 427c82622c0495f418d62d3306d1c5df37f59394251e712baae3dbaf6040acc1 |
| RwHelper.actual-after.obj | 8e9dda3e8437ed9e673b2ade211e6ef1fc5e5e65f593ac5980d4f99f2a11bea9 |
| RwHelper.actual-before.obj | b88c8a7d4bf83a14b009dd70898bab820ed91dff51dd5d7536f7ef651eb837d5 |
| TaskComplexGoToCarDoorAndStandStill.actual-after.obj | ac75201ebc5820453788f9b69464cb63119411224865e9d9e5b05f9629fd16ec |
| TaskComplexGoToCarDoorAndStandStill.actual-before.obj | 89130e0f2107afcd425b6dcc665796d5b28f087e18feb992120b9c69d0773565 |
| TaskComplexSeekEntityXYOffset.actual-before.obj | 4b8ba073b0e8d3d701ac398c599d26e40e0ff55858da624c11f05e8377430e38 |
| TaskInteriorSitInChair.actual-after.obj | e5307967d11d6bea1bb63d6fe8fc5bec7a1b117cc89b7323bc84fbb4e6a49982 |
| TaskInteriorSitInChair.actual-before.obj | b49655c947a0d220912fb06744745c72152ed58b0abd53d46d7a57bd48549a29 |
| TaskSimpleBeHit.actual-after.obj | 0cadc7ed12c6526dff7d8227680e1ea36f77434328c47a7d1741b60d50837a2e |
| TaskSimpleBeHit.actual-before.obj | 791d0eb58e19e9013325ade8725b25b465ad361cbe0fc093c5e6ea3b1d05a272 |
| TaskSimpleDead.actual-after.obj | 154271dd6dd653f8f983e516d9ab0742985330fa2dcf0b10fc678d68f67e5b9d |
| TaskSimpleDead.actual-before.obj | 08135e472f7ac2a4a176240e72a4281cb71f15da28f7dc8c0eae3a9d73a50da7 |
| TaskSimpleGoToPointFine.actual-after.obj | e88a2500acaaf8c24f63fd33d2fa1460a16b07209d26559da84fe27b25c0e318 |
| TaskSimpleGoToPointFine.actual-before.obj | c0e1415e3981cd74a72e795755c381c9ba0097562ba331ce654d7899738ed91e |
| X87Math.actual-after.obj | 7021e83c2c1f7a5067280daa3f74f563f63aab3d8038c882230b954be6105708 |
| X87Math.actual-before.obj | 17a218497d41f975f73942fe8c4aeb93baf36e8f2440d18210ba437e7668474b |
