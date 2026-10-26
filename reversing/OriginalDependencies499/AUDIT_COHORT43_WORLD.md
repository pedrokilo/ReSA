<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de la cohorte aprobada (43 funciones)

Lectura directa de approved.json, 18 validation-summary.json, summary.json, approval-audit.json y gzip completos. No se compila, ejecuta GTA ni cambia fuente, bancos o catálogo. Instantánea de aprobación: 98de2654cb4bfc9112e547fa080b4ebb3c9e72a307a319b6fa51c47ee1ddc405.

**Resultado: 43 funciones, 18 bancos y 78336 pares. Hallazgos que afectan evidencia: 0.**

| Banco | Funciones | Pares | Diferencias | NULL | Auditoría |
|---|---:|---:|---:|---:|---|
| DependenciesCarCtrl2 | 3 | 4352 | 0 | 0 | OK |
| DependenciesCarCtrl4 | 2 | 2048 | 0 | 0 | OK |
| DependenciesFightLeaf2 | 3 | 6144 | 0 | 0 | OK |
| DependenciesGarage2 | 3 | 2560 | 0 | 0 | OK |
| DependenciesLeaf2 | 2 | 2048 | 0 | 0 | OK |
| DependenciesMathLeaf3 | 2 | 2048 | 0 | 0 | OK |
| DependenciesMathSprite2 | 3 | 3072 | 0 | 0 | OK |
| DependenciesPedGroups1 | 1 | 1024 | 0 | 0 | OK |
| DependenciesPedGroups2 | 4 | 13056 | 0 | 0 | OK |
| DependenciesPointLights1 | 1 | 2560 | 0 | 0 | OK |
| DependenciesPointLights2 | 2 | 1024 | 0 | 0 | OK |
| DependenciesStats1 | 1 | 4096 | 0 | 0 | OK |
| DependenciesTaskCtor4 | 2 | 4096 | 0 | 0 | OK |
| DependenciesTaskLeaf3 | 2 | 4096 | 0 | 0 | OK |
| DependenciesTaskPed5 | 4 | 8192 | 0 | 0 | OK |
| DependenciesVehicleLeaf5 | 4 | 5120 | 0 | 0 | OK |
| DependenciesWorld4 | 2 | 4608 | 0 | 0 | OK |
| DependenciesWorldLeaf3 | 2 | 8192 | 0 | 0 | OK |

Se verificaron casos únicos consecutivos por función y pares únicos, equality real de objetos, accepted/equal true, un único complete sin errores y restauración. SHA-256 se calcula sobre los bytes descomprimidos y se cruza contra los tres manifiestos; summary/RESULTADO se hashéan por bytes. Configuración y partidas están confirmadas por los flags históricos y hashes antes del run en summary; no se reconstruyen snapshots posteriores inexistentes ni se presenta como una comprobación independiente de los archivos después del run.

## Colaboradores nuevos

TaskPed5: begin registra cinco destinos 571B70/571A00/601C30/601BC0/5DF8F0 con JMP a DLL; no stubs. Todos los pares confirman ruta, pila, registros no volátiles, SEH y contrato. exception_unwind_tested=false: no se atribuye validación a desenrollado excepcional. El begin muestra rutas compartidas reconstruidas; no prueba independencia transitiva universal.
VehicleLeaf5: meta de helpers 542A50/46AA30 incluye símbolo crudo distinto de cuerpo resuelto, módulo ASI y ruta correcta; ambos colaboradores compartidos reconstruidos, sin stubs. Cada original/candidate reafirma helper_route_ok, ruta principal, ABI, efectos y estado protegido, y cada par state_restored. La relación de aprobación incluye exactamente sus cuatro identidades y el run 195234-703310.

## Manifiestos y evolución de fuente

Todos los bank_sources deben coincidir con su hash histórico. Los hashes tested_sources coinciden entre validación y approval-audit. Un archivo fuente actual puede evolucionar por bancos posteriores: esto no invalida por sí mismo la evidencia histórica; especialmente InjectHooksMain actual no se exige igual. El SHA del ASI ejecutado concuerda entre las tres evidencias; artifacts no se requieren porque se excluyen de Git.

| Banco | Fuente con evolución desde su run | SHA histórico | SHA actual |
|---|---|---|---|
| DependenciesCarCtrl2 | source/game_sa/CarCtrl.cpp | 1b1af5ea6ce4a5f647aa3d29bfe80b4e5a0569d7248726da286dd68271321c6c | 45e4e00f8def469fa1ec1692e1e3fe596bc01e54fb7c5db01163e2c5368e5ebc |
| DependenciesCarCtrl2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesCarCtrl4 | source/game_sa/CarCtrl.cpp | 004a136031fde60a1d357c9612b66b9dd45339805684f7b209db90c78bc84e6c | 45e4e00f8def469fa1ec1692e1e3fe596bc01e54fb7c5db01163e2c5368e5ebc |
| DependenciesCarCtrl4 | source/game_sa/Entity/Physical.h | 3dc44efcc999a3ff8974e1a3da78963d0dfb00f3e9d3ee07d026efc01e275052 | 2be1902e933b8704da536c4f89fc7cf541083fb57aa1ca7cdc3c4ae1c719cea3 |
| DependenciesCarCtrl4 | source/InjectHooksMain.cpp | 399556be232dab15fe51f27b1a7a8273bd0b6374f060ee746bda26b2d1cef22e | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesFightLeaf2 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp | a677c96ed78e89e6ba483783629136f62ea17cc17111df1fe4be9016a822ef21 | 412b65ef1fc59fc1b0e1f59209bdff1ea5550f3b52d5c677173273ffaf14992c |
| DependenciesFightLeaf2 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h | 85e857dcb63b2c84b4dd31d19f1c0165fdd337606ae22405678ca0433633a18a | 1eab8612f9343d766debc1db79a405df84cc4654a4b2e2e5fb7748fb8d616981 |
| DependenciesFightLeaf2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesGarage2 | source/game_sa/Garages.cpp | f510ac836f1f5c1535daac20afd9bf0296cf54ae436f8bb4a9a98ccc3a3bef38 | e735bbddabf34637c7717d15d7a2aeb44f92404af9c8a00085a380435cf5272c |
| DependenciesGarage2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesLeaf2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesMathLeaf3 | source/InjectHooksMain.cpp | 991f931043a15bce44a1a7e1a2c6f08501aaa11211fd771669d0ba3c5c60f8e5 | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesMathSprite2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesPedGroups1 | source/game_sa/PedGroups.cpp | 7541a7beb7db08f67e8ad6f3bf3d15e0d1800c524012073d7d7189dca0e74241 | 0b16b20509132bc774db2d7f300484d6882bbe85839808c5a07812cd2df4b6a3 |
| DependenciesPedGroups1 | source/InjectHooksMain.cpp | d774c9f7572474d11096747ef2ff5884a696e92106f7ad40d40713712641c898 | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesPedGroups2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesPointLights1 | source/game_sa/PointLights.cpp | 2a750dc198ac4f63a970aa02e117af45b4a2134ce3f34571ec3bec464746e084 | e4ce86341b6aa596300748094185f5d670f3fd3fdc63aeb2d798cf71ca6870ec |
| DependenciesPointLights1 | source/InjectHooksMain.cpp | d774c9f7572474d11096747ef2ff5884a696e92106f7ad40d40713712641c898 | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesPointLights2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesStats1 | source/InjectHooksMain.cpp | d774c9f7572474d11096747ef2ff5884a696e92106f7ad40d40713712641c898 | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesTaskCtor4 | source/InjectHooksMain.cpp | 399556be232dab15fe51f27b1a7a8273bd0b6374f060ee746bda26b2d1cef22e | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesTaskLeaf3 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp | f5ad686f4647c6941aed5cf5627884042a168ab3b79ca77f2a9e0d0f5ed3e630 | 412b65ef1fc59fc1b0e1f59209bdff1ea5550f3b52d5c677173273ffaf14992c |
| DependenciesTaskLeaf3 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h | 85e857dcb63b2c84b4dd31d19f1c0165fdd337606ae22405678ca0433633a18a | 1eab8612f9343d766debc1db79a405df84cc4654a4b2e2e5fb7748fb8d616981 |
| DependenciesTaskLeaf3 | source/InjectHooksMain.cpp | 991f931043a15bce44a1a7e1a2c6f08501aaa11211fd771669d0ba3c5c60f8e5 | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesWorld4 | source/InjectHooksMain.cpp | 7a912880d0ff2128cb6654e679653c12ac91c8fa2e94784475ee43cb3ad9360c | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |
| DependenciesWorldLeaf3 | source/InjectHooksMain.cpp | 9dfbb99233c6ca03c5aa68186aeb26d5ed8e2d3c394c747ac7e38b033975877e | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 |

## Hallazgos

Ninguna discrepancia de integridad, relación de aprobación, cifras, casos, NULL, igualdad, restauración o instrumentación detectada en esta instantánea.

## Logs verificados

- DependenciesCarCtrl2: `c1af42f0a22109819641b03c915febcadff5b5217a6197653dba37337b97ebd5`
- DependenciesCarCtrl4: `c5123d0f3ebb6d8e33d0649d019a69beb7f88349f6c8865bd6636a7efb243d76`
- DependenciesFightLeaf2: `24a788f4a01c3d19a61022141626d18a108976abaf089173fe36cffa916c3dd1`
- DependenciesGarage2: `44994745714170b1aeac0dc00aa354d53db0b339e24f84cee3028d11cb9f4b1e`
- DependenciesLeaf2: `0777a00c0c1c301c14426c85a64a712c39082481fa3353c52203077420dfe439`
- DependenciesMathLeaf3: `89b9293858492f789c19fcbd80e5f2b07fae893028188938821bc805f5712daa`
- DependenciesMathSprite2: `8034887476f9bb9d8a0159ba4d5135ae3c97c8356d4d96d498c7d40f3b13094c`
- DependenciesPedGroups1: `9f7a8587a905604e6b958af8c420d464cfad4019cea65a2334e261b357ded595`
- DependenciesPedGroups2: `067e8bf1421e4bd7df713e80295851dda0a0ca4df4ad50d6f0bea001a0f956b4`
- DependenciesPointLights1: `79bfb66a332e1a0c91cc473f0cc6f05ecd7d3b4e944c52393641250bec53a5bf`
- DependenciesPointLights2: `9a7ae11fb20ee6ba2cdbd6faf215b140df83d3d022615c344e8a50f7e0176530`
- DependenciesStats1: `a1108bffebe6ad09031acbeb75bcbcb5ea3b66f5fe353cf3a38b7ed1fc745333`
- DependenciesTaskCtor4: `2a568acfefefe8a714823dc7c8138c94d74508aa8eb72fc03d24a3ba7d39567d`
- DependenciesTaskLeaf3: `241490d8b145fac9d5d22555431cd83086867476d84d16f03ebcbd5f28e0eb35`
- DependenciesTaskPed5: `2bc358b65e8a04890f91bb52dce4c9c3f1336fe8112e1424f2560b4a2e2b3e6f`
- DependenciesVehicleLeaf5: `c68c3694be5b4a823ec1c50b8bf51dae996041cdbde0f3bbd4ea65f33f20b869`
- DependenciesWorld4: `ab6b3595832e84b362842f0bfbee868d3d62bf4c35b797b21a8f043ab1b49c49`
- DependenciesWorldLeaf3: `a3625fcaeae6de3196cba11a4a819be671320bdf60ece622b9cff589485cebef`
