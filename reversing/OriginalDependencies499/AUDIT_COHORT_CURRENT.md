# Auditoría independiente de la cohorte admitida actual

Instantánea approved SHA256 `0449a71ec060db25e7ee81d546ab440f50cf98a29354ef5e673f3511a16704b2`. Gzip leído en streaming; no builds/juego ni cambios a fuentes, instrumentos o catálogo. Auditoría histórica de43 conservada.

**105 funciones / 48 bancos / 243392 pares; 0 hallazgos.**

| Banco | Funciones | Pares | Diferencias | NULL | Hallazgos |
|---|---:|---:|---:|---:|---:|
| DependenciesCarCtrl2 | 3 | 4352 | 0 | 0 | 0 |
| DependenciesCarCtrl4 | 2 | 2048 | 0 | 0 | 0 |
| DependenciesCollisionAdapters1 | 2 | 2048 | 0 | 0 | 0 |
| DependenciesDecisionPed7 | 4 | 8192 | 0 | 0 | 0 |
| DependenciesDecisionRelationship6 | 4 | 6272 | 0 | 0 | 0 |
| DependenciesFightLeaf2 | 3 | 6144 | 0 | 0 | 0 |
| DependenciesFormationMemory8 | 2 | 8256 | 0 | 0 | 0 |
| DependenciesGarage2 | 3 | 2560 | 0 | 0 | 0 |
| DependenciesGarageSphere15 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesGroupTasks14 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesLeaf2 | 2 | 2048 | 0 | 0 | 0 |
| DependenciesMathLeaf3 | 2 | 2048 | 0 | 0 | 0 |
| DependenciesMathSprite2 | 3 | 3072 | 0 | 0 | 0 |
| DependenciesMemory9 | 4 | 8192 | 0 | 0 | 0 |
| DependenciesPadCamera7 | 3 | 20480 | 0 | 0 | 0 |
| DependenciesPathDensity13 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesPathOrientation14 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesPedGroups1 | 1 | 1024 | 0 | 0 | 0 |
| DependenciesPedGroups2 | 4 | 13056 | 0 | 0 | 0 |
| DependenciesPointLights1 | 1 | 2560 | 0 | 0 | 0 |
| DependenciesPointLights2 | 2 | 1024 | 0 | 0 | 0 |
| DependenciesStats1 | 1 | 4096 | 0 | 0 | 0 |
| DependenciesStatsFile15 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesStatsRadio7 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesStatsUi8 | 4 | 8192 | 0 | 0 | 0 |
| DependenciesTask10 | 3 | 6144 | 0 | 0 | 0 |
| DependenciesTask11 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesTask12 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesTaskCtor4 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesTaskLeaf3 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesTaskPed5 | 4 | 8192 | 0 | 0 | 0 |
| DependenciesVehicle12 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesVehicleLeaf5 | 4 | 5120 | 0 | 0 | 0 |
| DependenciesVehicleStats10 | 3 | 6144 | 0 | 0 | 0 |
| DependenciesVehicleStats11 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesVehicleStats9 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesWorld10 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesWorld11 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesWorld12 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesWorld13 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesWorld14 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesWorld15 | 1 | 2048 | 0 | 0 | 0 |
| DependenciesWorld4 | 2 | 4608 | 0 | 0 | 0 |
| DependenciesWorld9 | 2 | 4096 | 0 | 0 | 0 |
| DependenciesWorldFxFont5 | 4 | 15360 | 0 | 0 | 0 |
| DependenciesWorldLeaf3 | 2 | 8192 | 0 | 0 | 0 |
| DependenciesWorldWaterPath6 | 4 | 24576 | 0 | 0 | 0 |
| DependenciesZones13 | 1 | 2048 | 0 | 0 | 0 |

Cada identidad approved se cruza una a una con hooks.csv (reversed/validated=1) y registry (identidad, evidencia y SHA). Pares y casos únicos/consecutivos; original/candidate dict completos iguales y accepted/equal true. Gates de ruta deben estar presentes; todos los gates de ABI/contrato/FP/restauración presentes se exigen verdaderos. Un begin registrado, resúmenes sin fallos y covered_functions verdadero (complete_paths también verdadero cuando existe; los bancos históricos no emiten ese campo) y un complete/restored. Huellas log/gzip/summary/result y manifiestos históricos verificadas. Partidas/configuraciones se comprueban por flags históricos de ejecución; no se reconstruyen snapshots posteriores inexistentes.

## Evolución de fuentes

Derivas de tested_sources respecto al estado actual son esperadas por tandas posteriores; no invalidan evidencia histórica. Instrumentación debe coincidir exactamente.

| Banco | Fuente | SHA histórico | SHA actual |
|---|---|---|---|
| DependenciesCarCtrl2 | source/game_sa/CarCtrl.cpp | 1b1af5ea6ce4a5f647aa3d29bfe80b4e5a0569d7248726da286dd68271321c6c | d9236aeeb52b5bffbf53ccae1d1991b699e6e0ea7e0e4f6aaf7754b4b039693a |
| DependenciesCarCtrl2 | source/game_sa/CarCtrl.h | 47d983d82c1a2c029d8a8187b3e7758616af9c8aea606ec092fe7f71df41a21b | 839c30f512d0d453d3c5ede12522b5104f7788fab874bf9cf8802b98ee09b49d |
| DependenciesCarCtrl2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesCarCtrl2 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesCarCtrl4 | source/game_sa/CarCtrl.cpp | 004a136031fde60a1d357c9612b66b9dd45339805684f7b209db90c78bc84e6c | d9236aeeb52b5bffbf53ccae1d1991b699e6e0ea7e0e4f6aaf7754b4b039693a |
| DependenciesCarCtrl4 | source/game_sa/CarCtrl.h | 47d983d82c1a2c029d8a8187b3e7758616af9c8aea606ec092fe7f71df41a21b | 839c30f512d0d453d3c5ede12522b5104f7788fab874bf9cf8802b98ee09b49d |
| DependenciesCarCtrl4 | source/game_sa/Entity/Vehicle/Vehicle.h | fe438d94b2ab8867d9099eb938c1822f240555d5543ad299212ba3c86f125b6d | ef1ec3e5a298cebb325f63b31cacb1c5f6b1fb207838ebc96ecfa79285bd71f9 |
| DependenciesCarCtrl4 | source/game_sa/Entity/Entity.h | a81231fe40082b1bbd13995679e8032900fd3538eebea83c181346608ea906ed | 4434873dabecd750024d5a8d193c45b870aad9922ed5be53a4a8aa6324d5007e |
| DependenciesCarCtrl4 | source/game_sa/Entity/Physical.h | 3dc44efcc999a3ff8974e1a3da78963d0dfb00f3e9d3ee07d026efc01e275052 | 2be1902e933b8704da536c4f89fc7cf541083fb57aa1ca7cdc3c4ae1c719cea3 |
| DependenciesCarCtrl4 | source/InjectHooksMain.cpp | 399556be232dab15fe51f27b1a7a8273bd0b6374f060ee746bda26b2d1cef22e | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesCarCtrl4 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesCollisionAdapters1 | source/game_sa/Collision/Collision.cpp | d4a098fe034eac808a1c43ece7d1f1bc1557a2d3dfdc0b3d1aceecce25de1d97 | eb6fecc435da93db329cee2d9b41d732bfe10b1113e35ecabda9be4440db5bb8 |
| DependenciesCollisionAdapters1 | source/game_sa/CompressedVector.h | a590cacdace96671c1cbdbf88ff20c6b8ac82eb9b755e53ae2a2d23b54f532d6 | d8e7eba549a22d2a797a0ebcaa284e71477ef9dfdd388fd61d37443a6724bdac |
| DependenciesCollisionAdapters1 | source/game_sa/MemoryMgr.cpp | 30ec6a6b730b9160a9c61872b7810aaaa5ea3b5266d3695ba6ec7fb9e22f4ee0 | 1cdb60235c4f771740150646e524eb4986b87a69e8cbe45315de2de8c8ba432f |
| DependenciesCollisionAdapters1 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesDecisionRelationship6 | source/game_sa/FileMgr.cpp | a7a192213bcc8f9c445f080ea27e957fc58c27e8345fedd35be93bca37c51f68 | d3d1e045e5fe41895ee4bc624ba91d46134b57affa59ef79b1260f75a8ee55ea |
| DependenciesFightLeaf2 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp | a677c96ed78e89e6ba483783629136f62ea17cc17111df1fe4be9016a822ef21 | 6a3d2d98b4bce0aee97a08790ce20cd2abf453a37baf0137728115469b5a4ebd |
| DependenciesFightLeaf2 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h | 85e857dcb63b2c84b4dd31d19f1c0165fdd337606ae22405678ca0433633a18a | 1eab8612f9343d766debc1db79a405df84cc4654a4b2e2e5fb7748fb8d616981 |
| DependenciesFightLeaf2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesFightLeaf2 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesFormationMemory8 | source/game_sa/Formation.cpp | 6b2b77de218a0c3618277760cd794aded1487e6df5704752a4bb47a8b5188737 | d07a05ee721f0bafbcc1ff2c05ef0583c621c85acd49e82d168ec9e0ecd28c9d |
| DependenciesFormationMemory8 | source/app/platform/platform.cpp | 5593c41de5e8088cdc28a816f4c939a5b7f7c1620494cde7b79c1df9e52add19 | a00248ce14211ba1978539ce3d2a5e8c1f7c0cbd984f6401e9ef672e0cff26d3 |
| DependenciesFormationMemory8 | source/game_sa/RenderWare/rw/rwplcore.h | 1e4709dbbc43b0f14681dd53707bff4d3974e3215325af7cf1450684e8e55362 | b62b6c00f476d106ec59a7a9f4e3571eee5e4a50fad028efff5974e4c81e65fa |
| DependenciesFormationMemory8 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesGarageSphere15 | source/game_sa/Garage.cpp | d6cd84ad62468fb369005bbc82079e934fa75933dcf8604ecdec350445f8845a | 4498a718d91e34ddc7e093d171b8c7a8e1a0c54b699980b493deb2a65c1f3808 |
| DependenciesGarageSphere15 | source/game_sa/Entity/Entity.cpp | 2b3152ce25c8910ea3624da17bff6eb8b6c75ac1331c8596c1ff879300f11108 | c0e8b71dc61f80bbcb3ecf77b4b8e2d35e2781ac56f7c6dab89f998acaaa7c25 |
| DependenciesGarageSphere15 | source/game_sa/Entity/Entity.h | a81231fe40082b1bbd13995679e8032900fd3538eebea83c181346608ea906ed | 4434873dabecd750024d5a8d193c45b870aad9922ed5be53a4a8aa6324d5007e |
| DependenciesGarageSphere15 | source/game_sa/Entity/Placeable.cpp | bf57b97b53cd9e8326d97de870b9934dd487effde705a3462061d883e052e16a | 0030861304db9f417c8fd8b322a7be006ef4e5a099238c157e5dc0cf0d018e24 |
| DependenciesGarageSphere15 | source/game_sa/Entity/Vehicle/Vehicle.cpp | 1886f1f0360c46ab7fa481fa448b6afef2a6b0b96f1e06eb7285c8f4a1fb93e1 | 729368905b3759af53f39326bbc5b818183b8382004493757c346fe0c0a05056 |
| DependenciesGarageSphere15 | source/game_sa/Entity/Vehicle/Vehicle.h | fe438d94b2ab8867d9099eb938c1822f240555d5543ad299212ba3c86f125b6d | ef1ec3e5a298cebb325f63b31cacb1c5f6b1fb207838ebc96ecfa79285bd71f9 |
| DependenciesGarageSphere15 | source/game_sa/SimpleTransform.cpp | 9b37b26e261f5fcd84b0108d194e42ee29a47afc0134239e07a13663d1fbd7e2 | 91cfe9e3ac4ceb4a2d63bca9a897a4d62b3ff298a32576300771df16a259c8c4 |
| DependenciesGarageSphere15 | source/game_sa/Core/Matrix.cpp | f5c9d1f294ca4fdd5245eb23ff28148550d78e9a1615d3e1f4de6f814281d69b | e8eda28e69e0780cdf33668e8ca46aeada103855a83fc5b55f51283b11b602da |
| DependenciesGarageSphere15 | source/game_sa/Models/BaseModelInfo.cpp | 02519bd7682f909d265f24eb57545fdf8a38cfdfb48f63015ea6027d23d06bd2 | 54132cb455e72432516f20ea81b4a770b4e265797937963a9657c7fc0dba503d |
| DependenciesGarageSphere15 | source/game_sa/Pools/Pools.h | 5ea9374b3db5cfc399404dbab9907ada18691dc16ad4acc24a25472ef2333ebe | 1a38f6b00064b093f890e33c6889661d6d5a145333e124bc406057337c7bef93 |
| DependenciesGarageSphere15 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesGroupTasks14 | source/game_sa/PedGroups.cpp | 48a264aa3c4fc6124a4dd79f69534a8311dde415bdb8709ff0059863f74ffe68 | c1dea251248649681e752af39c6b73c8b8076a2b0f26c746fd42475e649dd0b1 |
| DependenciesGroupTasks14 | source/game_sa/PedGroupIntelligence.cpp | f41495a46bdf744cd4214092c9c2d334217ce042b571a9aa16f531a5ffb8811b | 365bd5c5872d8703d9b937aca759b8e7669e795c5f80b03566fe5406e41b1884 |
| DependenciesGroupTasks14 | source/game_sa/PedGroupIntelligence.h | 2b5f205418513d4ebbe9ca3b50b70b7536ecba530d812c81533b824f7a6caab8 | 1061837241e7990767704fadf56c29e674b98ae5eb36cbbc52957b5ad7f84344 |
| DependenciesGroupTasks14 | source/game_sa/Entity/Entity.cpp | 2b3152ce25c8910ea3624da17bff6eb8b6c75ac1331c8596c1ff879300f11108 | c0e8b71dc61f80bbcb3ecf77b4b8e2d35e2781ac56f7c6dab89f998acaaa7c25 |
| DependenciesGroupTasks14 | source/game_sa/Entity/Entity.h | a81231fe40082b1bbd13995679e8032900fd3538eebea83c181346608ea906ed | 4434873dabecd750024d5a8d193c45b870aad9922ed5be53a4a8aa6324d5007e |
| DependenciesGroupTasks14 | source/game_sa/Entity/Ped/Ped.cpp | 90d80d2d8b15f900cee469afc299eabd3e7494c0d04275699bec1fefe8b3f324 | 9276709c6c47677beccd39d2607a65945d14de5ca81cb5da1208ce94e4774b00 |
| DependenciesGroupTasks14 | source/game_sa/Entity/Ped/PlayerPed.cpp | 83e7a130b093f375c17f8028c0e150132d67ead232c317d6e078287eca7a17a0 | 68b89ae9eef986ebedd6a1c9b7ba1344e2e1c89d223b360af1ef364ee61fb413 |
| DependenciesGroupTasks14 | source/game_sa/Pools.cpp | 44be32e7886bc6a2ccfd35c5081a6f1970f4104bd1a7ea56a39126405a71c517 | 38b4dfcb66d2428ba80865b1e3d28f82ee464a8543e13443933715b92d8e07f6 |
| DependenciesGroupTasks14 | source/game_sa/Pools/Pools.h | 5ea9374b3db5cfc399404dbab9907ada18691dc16ad4acc24a25472ef2333ebe | 1a38f6b00064b093f890e33c6889661d6d5a145333e124bc406057337c7bef93 |
| DependenciesGroupTasks14 | source/game_sa/PedIntelligence.cpp | d4c28f40a0670cca99f1bbfb280f30d7e0bdda072fbb78a08820419f37528ff7 | 7b83d7dd91b60fb7b0a99946f2a819fbcb25ab28ca3520d911dc23d54fdf9e7e |
| DependenciesGroupTasks14 | source/game_sa/PedIntelligence.h | c18190fafe528ca4c7a185f12fac0f4ab348dee5aa411acd2e7349a13df55d91 | 3cdb1dfdd718c5dc92a273bb06a8db7642606f9c456d76bde75c16c02325abe6 |
| DependenciesGroupTasks14 | source/game_sa/Pools/PedPool.h | 00e211fcbd5c4b1fa276fe781725e11c1c90c2e18a9fa3e5bae5706d9b2cb05b | 0bcea55bae3a17870273a2091930390bc3f56edf335f25f5417bde1dce39cf36 |
| DependenciesGroupTasks14 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesLeaf2 | source/game_sa/Models/ModelInfo.cpp | 7b8d9dee38599e2751c662aa6c1be5022c6d4043781482ad523866ab847c406c | 7114af19efafabfa770d64bccc1bc88af6ffca255b2820101d828cdd57922134 |
| DependenciesLeaf2 | source/game_sa/MemoryMgr.cpp | 30ec6a6b730b9160a9c61872b7810aaaa5ea3b5266d3695ba6ec7fb9e22f4ee0 | 1cdb60235c4f771740150646e524eb4986b87a69e8cbe45315de2de8c8ba432f |
| DependenciesLeaf2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesLeaf2 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesMathLeaf3 | source/game_sa/Core/Quaternion.cpp | 3d60edffc11e7ecfaad4a841597721dfb5d864289ca44ee9ba26b1986e53a967 | 21caa19e14bb8f966bd7ad618e71df273f47470bad8a37b0b79a044fea5f2ca4 |
| DependenciesMathLeaf3 | source/InjectHooksMain.cpp | 991f931043a15bce44a1a7e1a2c6f08501aaa11211fd771669d0ba3c5c60f8e5 | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesMathLeaf3 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesMathSprite2 | source/game_sa/Sprite2d.cpp | 3cdc0b9f6730f17ff425712a1817c5b6cf570ce58fe67367f37de71a6946e8c8 | e348ab17734bdbb819d5684f5026629ac2fe994774ba4f7101cb2fd4b9c07d09 |
| DependenciesMathSprite2 | source/game_sa/common.cpp | 82ea6042072c5d5ec5583b4557d698115ee47ab7c7dd821e784a27d93c7b0357 | 3c9458212d4210e46d7d4f421cd832ab49fc1989961d0c1db72f5f3420a82cd7 |
| DependenciesMathSprite2 | source/game_sa/Core/Matrix.cpp | f5c9d1f294ca4fdd5245eb23ff28148550d78e9a1615d3e1f4de6f814281d69b | e8eda28e69e0780cdf33668e8ca46aeada103855a83fc5b55f51283b11b602da |
| DependenciesMathSprite2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesMathSprite2 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesMemory9 | source/game_sa/MemoryMgr.cpp | 645dd362627fb014a2acd4fdc95798b0caca8ae35c617762a5285a05053a162c | 1cdb60235c4f771740150646e524eb4986b87a69e8cbe45315de2de8c8ba432f |
| DependenciesMemory9 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesPadCamera7 | source/game_sa/X87Math.cpp | ff115d0bb000b3af1b792fb65e58c763392c2d4ca848e82bfb6bbcbab05e61fe | 8c095e5650de124975b0de0e36063681090f57d49837e6ce33b9b17bc7d8fe90 |
| DependenciesPadCamera7 | source/game_sa/X87Math.h | 7751a4e84ebe3f695e73174a8c006de9412c08aed0a27d8c37a7f6ba888a1e97 | 098e7579cf41d61519603bd3a133ddf00212a7006da14fa82a9866f7f1b83c90 |
| DependenciesPathDensity13 | source/game_sa/PathFind.cpp | b289ad134233f537732d2ff6555ae5d294e0d493bc9bc0e164a13f3b42bda5f2 | 10f68895e0166e581cfe17a4b9e5736ebf2c99985aec5cb72ff0c669dbf5b3e9 |
| DependenciesPathDensity13 | source/game_sa/CutsceneMgr.cpp | 4cddadb9affd99b4d1def2c1dfd65c218583e4315836c9aa1d6f8fb091a62332 | d05005b3a7ea935b9ec7cb07a1f9069e71aaa6a381913d87405c804d18030a1d |
| DependenciesPathDensity13 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesPedGroups1 | source/game_sa/PedGroups.cpp | 7541a7beb7db08f67e8ad6f3bf3d15e0d1800c524012073d7d7189dca0e74241 | c1dea251248649681e752af39c6b73c8b8076a2b0f26c746fd42475e649dd0b1 |
| DependenciesPedGroups1 | source/InjectHooksMain.cpp | d774c9f7572474d11096747ef2ff5884a696e92106f7ad40d40713712641c898 | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesPedGroups1 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesPedGroups2 | source/game_sa/PedGroups.cpp | 0b16b20509132bc774db2d7f300484d6882bbe85839808c5a07812cd2df4b6a3 | c1dea251248649681e752af39c6b73c8b8076a2b0f26c746fd42475e649dd0b1 |
| DependenciesPedGroups2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesPedGroups2 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesPointLights1 | source/game_sa/PointLights.cpp | 2a750dc198ac4f63a970aa02e117af45b4a2134ce3f34571ec3bec464746e084 | b10358b7b39fce1a8f33c2c0ae382c4fbe25657c64215d57a55653bcc5db6e1b |
| DependenciesPointLights1 | source/InjectHooksMain.cpp | d774c9f7572474d11096747ef2ff5884a696e92106f7ad40d40713712641c898 | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesPointLights1 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesPointLights2 | source/game_sa/PointLights.cpp | e4ce86341b6aa596300748094185f5d670f3fd3fdc63aeb2d798cf71ca6870ec | b10358b7b39fce1a8f33c2c0ae382c4fbe25657c64215d57a55653bcc5db6e1b |
| DependenciesPointLights2 | source/InjectHooksMain.cpp | 87b22b34070d1b503867720319876156317fe88595a424596c86ef4f2a43380f | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesPointLights2 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesStats1 | source/game_sa/Stats.cpp | b8e5e8bb24b9f527750cfcfccd00ecaf9b4b9b0c4f91b9633fb37a992bd106eb | 2ff660c3b1ba01d98275772ee83eb23786ae8ab605fa594a85114fe7b0551d58 |
| DependenciesStats1 | source/InjectHooksMain.cpp | d774c9f7572474d11096747ef2ff5884a696e92106f7ad40d40713712641c898 | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesStats1 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesStatsFile15 | source/game_sa/FileMgr.cpp | a7a192213bcc8f9c445f080ea27e957fc58c27e8345fedd35be93bca37c51f68 | d3d1e045e5fe41895ee4bc624ba91d46134b57affa59ef79b1260f75a8ee55ea |
| DependenciesStatsRadio7 | source/game_sa/Stats.cpp | e84d841dc097e7d00a610e9e2b6ab67ad841b49b13077d026c0b38e8ab35e55a | 2ff660c3b1ba01d98275772ee83eb23786ae8ab605fa594a85114fe7b0551d58 |
| DependenciesStatsRadio7 | source/game_sa/Audio/Managers/AERadioTrackManager.cpp | b0a7c3cc81326a37553706edfd97eaa3de6c1bfa5769d166953c2f6e5c4e48c8 | 8d3ff9190a73e6a39078dbf7714c5d413179b10fd24682eded01d28fd1b45e95 |
| DependenciesStatsRadio7 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesStatsUi8 | source/game_sa/Stats.cpp | 435ed0597eaa20ae2f951e3df137c14b88f6ecdf07ada7b3e7466a790b45f311 | 2ff660c3b1ba01d98275772ee83eb23786ae8ab605fa594a85114fe7b0551d58 |
| DependenciesStatsUi8 | source/game_sa/Hud.cpp | cb21ed7bb0d25797f52a106f5f5528ea695bf3da9f0f7e89fbf692845501d536 | f6f38b6d52096e15cc0cdbcec3b66eb7e81d9fc90ab89c33bfc1cbf5701cb0a8 |
| DependenciesStatsUi8 | source/game_sa/World.cpp | b7ff3a0962752810a3e1568ca6d66a53754d69e64491b9b8127c679e793d19ca | 0066108864b6c6c3aa13ac5a68da99749b177bd94605ebc3a2c14580018340bd |
| DependenciesStatsUi8 | source/game_sa/Pad.cpp | ce474e3e1a199a6b96b78a0c6811d61b1f09501786332ae76424377b2f04fc45 | 1c5dca807d19a1811b60fc19ef2e7a6d98e19788be56a7afac791c9691661981 |
| DependenciesStatsUi8 | source/game_sa/PlayerInfo.cpp | 37ac0439d6e185b455f05180386d6472a2784c8671bba1e7fe232497eda25752 | d12f8689d0214bb834079dabf54abe951e644aa6ffe290cca67ae199613679b2 |
| DependenciesStatsUi8 | source/game_sa/MenuManager.cpp | bdc00aa4b943a8515c0377f1f8613ba4457d08216b18f37f0745793bf67257c3 | d26b735c4abb642c0367df0b5afc75c443392e1161c0dd7609ebeb9dd5cb4e5b |
| DependenciesStatsUi8 | source/game_sa/CutsceneMgr.cpp | 4cddadb9affd99b4d1def2c1dfd65c218583e4315836c9aa1d6f8fb091a62332 | d05005b3a7ea935b9ec7cb07a1f9069e71aaa6a381913d87405c804d18030a1d |
| DependenciesStatsUi8 | source/game_sa/Garages.cpp | e735bbddabf34637c7717d15d7a2aeb44f92404af9c8a00085a380435cf5272c | b6d11644933778f0cd5919d3c59d55b1b88256e302a0476b00af8b4968ed439d |
| DependenciesStatsUi8 | source/game_sa/Garages.h | 6ef471e9a50620eab5f6d29927305d92979d7a79a7d0e15d935efda563ff02bc | f71fc96fd17d9d22cba76e913da868dea9b8359fe466eea9465241a3f093f658 |
| DependenciesStatsUi8 | source/game_sa/Replay.cpp | d341ccbe2e494a051a57f7172c28d81022ebd2283e9f0956c2f3fda4b917de1f | e8eab1443f86a11639d93d2cec1590d191e253e791cc79495635d5e6ab99c43b |
| DependenciesStatsUi8 | source/game_sa/common.cpp | 82ea6042072c5d5ec5583b4557d698115ee47ab7c7dd821e784a27d93c7b0357 | 3c9458212d4210e46d7d4f421cd832ab49fc1989961d0c1db72f5f3420a82cd7 |
| DependenciesStatsUi8 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesTask12 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp | 9745e2abb26aaf1cfc29b897f9ec17e8ffa82dbdb264b474ba5f934086cc87be | 6a3d2d98b4bce0aee97a08790ce20cd2abf453a37baf0137728115469b5a4ebd |
| DependenciesTask12 | source/game_sa/Pools.cpp | 44be32e7886bc6a2ccfd35c5081a6f1970f4104bd1a7ea56a39126405a71c517 | 38b4dfcb66d2428ba80865b1e3d28f82ee464a8543e13443933715b92d8e07f6 |
| DependenciesTask12 | source/game_sa/Pools/Pools.h | 5ea9374b3db5cfc399404dbab9907ada18691dc16ad4acc24a25472ef2333ebe | 1a38f6b00064b093f890e33c6889661d6d5a145333e124bc406057337c7bef93 |
| DependenciesTask12 | source/game_sa/Entity/Entity.cpp | 2b3152ce25c8910ea3624da17bff6eb8b6c75ac1331c8596c1ff879300f11108 | c0e8b71dc61f80bbcb3ecf77b4b8e2d35e2781ac56f7c6dab89f998acaaa7c25 |
| DependenciesTask12 | source/game_sa/Entity/Entity.h | a81231fe40082b1bbd13995679e8032900fd3538eebea83c181346608ea906ed | 4434873dabecd750024d5a8d193c45b870aad9922ed5be53a4a8aa6324d5007e |
| DependenciesTask12 | source/game_sa/Animation/AnimBlendNode.cpp | 60d67cc32c808c0db5249ff40e97a2d73b71904dd31d6573ff8999e43f64efd3 | 977f5f2ca32ab66b24e36d9fd2872b0a945d9432941f64c4593f123f27b2f2b6 |
| DependenciesTask12 | source/game_sa/Animation/AnimBlendNode.h | d7a70511ca249c8babb68dfda86ccf6ff7c322797f976c34199eb46dd6bf1028 | 3f921e75734288796aca38f759bc63771aa81c78ae7ffa54cc30c532b84423bf |
| DependenciesTask12 | source/game_sa/MemoryMgr.cpp | 645dd362627fb014a2acd4fdc95798b0caca8ae35c617762a5285a05053a162c | 1cdb60235c4f771740150646e524eb4986b87a69e8cbe45315de2de8c8ba432f |
| DependenciesTask12 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesTaskCtor4 | source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp | ea1da323f01379f7eba9087e6d6ba0570f9eca95c4343609e3bc6e6b03c22764 | 3cde6ed2e8d2e167384936ef0525ba511032c4b499ec6766196555276373ed82 |
| DependenciesTaskCtor4 | source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h | 7ded0f21b737bd7dafc5d3510845cdcbd9fe15a56eceb75c96d3e196568ca17e | 643cf049a52076114887fb8c2ed169384bec1acb332c5f34f494d4fabc3c198c |
| DependenciesTaskCtor4 | source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp | 5d44bf69ddd3f066ed601f7ba9e52a6843f143a942cbb0fb3aa1134989795d55 | 21955f3f218bdf7a40ee612d35671c6d7b0ea5395e9bdd2b690fa9f27e146136 |
| DependenciesTaskCtor4 | source/game_sa/Entity/Entity.cpp | 2b3152ce25c8910ea3624da17bff6eb8b6c75ac1331c8596c1ff879300f11108 | c0e8b71dc61f80bbcb3ecf77b4b8e2d35e2781ac56f7c6dab89f998acaaa7c25 |
| DependenciesTaskCtor4 | source/game_sa/Entity/Entity.h | a81231fe40082b1bbd13995679e8032900fd3538eebea83c181346608ea906ed | 4434873dabecd750024d5a8d193c45b870aad9922ed5be53a4a8aa6324d5007e |
| DependenciesTaskCtor4 | source/InjectHooksMain.cpp | 399556be232dab15fe51f27b1a7a8273bd0b6374f060ee746bda26b2d1cef22e | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesTaskCtor4 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesVehicle12 | source/game_sa/CutsceneMgr.cpp | 4cddadb9affd99b4d1def2c1dfd65c218583e4315836c9aa1d6f8fb091a62332 | d05005b3a7ea935b9ec7cb07a1f9069e71aaa6a381913d87405c804d18030a1d |
| DependenciesVehicle12 | source/game_sa/Entity/Vehicle/Vehicle.cpp | 1886f1f0360c46ab7fa481fa448b6afef2a6b0b96f1e06eb7285c8f4a1fb93e1 | 729368905b3759af53f39326bbc5b818183b8382004493757c346fe0c0a05056 |
| DependenciesVehicle12 | source/game_sa/Entity/Vehicle/Vehicle.h | fe438d94b2ab8867d9099eb938c1822f240555d5543ad299212ba3c86f125b6d | ef1ec3e5a298cebb325f63b31cacb1c5f6b1fb207838ebc96ecfa79285bd71f9 |
| DependenciesVehicle12 | source/game_sa/Pools.cpp | 44be32e7886bc6a2ccfd35c5081a6f1970f4104bd1a7ea56a39126405a71c517 | 38b4dfcb66d2428ba80865b1e3d28f82ee464a8543e13443933715b92d8e07f6 |
| DependenciesVehicle12 | source/game_sa/Pools/Pools.h | 5ea9374b3db5cfc399404dbab9907ada18691dc16ad4acc24a25472ef2333ebe | 1a38f6b00064b093f890e33c6889661d6d5a145333e124bc406057337c7bef93 |
| DependenciesVehicle12 | source/game_sa/MemoryMgr.cpp | 645dd362627fb014a2acd4fdc95798b0caca8ae35c617762a5285a05053a162c | 1cdb60235c4f771740150646e524eb4986b87a69e8cbe45315de2de8c8ba432f |
| DependenciesVehicle12 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesVehicleLeaf5 | source/game_sa/CarCtrl.cpp | 45e4e00f8def469fa1ec1692e1e3fe596bc01e54fb7c5db01163e2c5368e5ebc | d9236aeeb52b5bffbf53ccae1d1991b699e6e0ea7e0e4f6aaf7754b4b039693a |
| DependenciesVehicleLeaf5 | source/game_sa/CarCtrl.h | 47d983d82c1a2c029d8a8187b3e7758616af9c8aea606ec092fe7f71df41a21b | 839c30f512d0d453d3c5ede12522b5104f7788fab874bf9cf8802b98ee09b49d |
| DependenciesVehicleLeaf5 | source/game_sa/PathFind.cpp | 74740e839dd6617821671c9cddb7f42c650c4269a1575c9ab71648a8a3de338a | 10f68895e0166e581cfe17a4b9e5736ebf2c99985aec5cb72ff0c669dbf5b3e9 |
| DependenciesVehicleLeaf5 | source/game_sa/Entity/Physical.cpp | 8b177287690825af26d4ef767cec4c39f852c3a83014e8c747c27b3996445dd0 | 29d32a2d942f9ac0b04552be20c8e315a96a18f492d67fb0428f28bcaab41a6d |
| DependenciesVehicleLeaf5 | source/InjectHooksMain.cpp | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesVehicleLeaf5 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesVehicleStats11 | source/game_sa/Entity/Vehicle/Vehicle.cpp | 8f790f225e98a56259e29d9a946677072eaec3903a9d11e97a08166d3d6c9b9f | 729368905b3759af53f39326bbc5b818183b8382004493757c346fe0c0a05056 |
| DependenciesVehicleStats9 | source/game_sa/Entity/Vehicle/Vehicle.h | fe438d94b2ab8867d9099eb938c1822f240555d5543ad299212ba3c86f125b6d | ef1ec3e5a298cebb325f63b31cacb1c5f6b1fb207838ebc96ecfa79285bd71f9 |
| DependenciesVehicleStats9 | source/game_sa/Timer.cpp | b78d6cc3da32af05334e109f01853c5c29b9be5774e552f832d6213f9d518e3c | bb1e999ca6da7dbef1eee9d565c88dcd9bd213729bfdde861355fcf9e0127a5c |
| DependenciesVehicleStats9 | source/game_sa/Stats.cpp | 43c7ed5f6bf2c290b1f8b941764f4d15cbbb012aa8dacba32b6fcae733377360 | 2ff660c3b1ba01d98275772ee83eb23786ae8ab605fa594a85114fe7b0551d58 |
| DependenciesVehicleStats9 | source/game_sa/Hud.cpp | cb21ed7bb0d25797f52a106f5f5528ea695bf3da9f0f7e89fbf692845501d536 | f6f38b6d52096e15cc0cdbcec3b66eb7e81d9fc90ab89c33bfc1cbf5701cb0a8 |
| DependenciesVehicleStats9 | source/game_sa/World.cpp | b7ff3a0962752810a3e1568ca6d66a53754d69e64491b9b8127c679e793d19ca | 0066108864b6c6c3aa13ac5a68da99749b177bd94605ebc3a2c14580018340bd |
| DependenciesVehicleStats9 | source/game_sa/Pad.cpp | ce474e3e1a199a6b96b78a0c6811d61b1f09501786332ae76424377b2f04fc45 | 1c5dca807d19a1811b60fc19ef2e7a6d98e19788be56a7afac791c9691661981 |
| DependenciesVehicleStats9 | source/game_sa/PlayerInfo.cpp | 37ac0439d6e185b455f05180386d6472a2784c8671bba1e7fe232497eda25752 | d12f8689d0214bb834079dabf54abe951e644aa6ffe290cca67ae199613679b2 |
| DependenciesVehicleStats9 | source/game_sa/MenuManager.cpp | bdc00aa4b943a8515c0377f1f8613ba4457d08216b18f37f0745793bf67257c3 | d26b735c4abb642c0367df0b5afc75c443392e1161c0dd7609ebeb9dd5cb4e5b |
| DependenciesVehicleStats9 | source/game_sa/CutsceneMgr.cpp | 4cddadb9affd99b4d1def2c1dfd65c218583e4315836c9aa1d6f8fb091a62332 | d05005b3a7ea935b9ec7cb07a1f9069e71aaa6a381913d87405c804d18030a1d |
| DependenciesVehicleStats9 | source/game_sa/Garages.cpp | e735bbddabf34637c7717d15d7a2aeb44f92404af9c8a00085a380435cf5272c | b6d11644933778f0cd5919d3c59d55b1b88256e302a0476b00af8b4968ed439d |
| DependenciesVehicleStats9 | source/game_sa/Garages.h | 6ef471e9a50620eab5f6d29927305d92979d7a79a7d0e15d935efda563ff02bc | f71fc96fd17d9d22cba76e913da868dea9b8359fe466eea9465241a3f093f658 |
| DependenciesVehicleStats9 | source/game_sa/Replay.cpp | d341ccbe2e494a051a57f7172c28d81022ebd2283e9f0956c2f3fda4b917de1f | e8eab1443f86a11639d93d2cec1590d191e253e791cc79495635d5e6ab99c43b |
| DependenciesVehicleStats9 | source/game_sa/common.cpp | 82ea6042072c5d5ec5583b4557d698115ee47ab7c7dd821e784a27d93c7b0357 | 3c9458212d4210e46d7d4f421cd832ab49fc1989961d0c1db72f5f3420a82cd7 |
| DependenciesVehicleStats9 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesWorld10 | source/game_sa/X87Math.cpp | ff115d0bb000b3af1b792fb65e58c763392c2d4ca848e82bfb6bbcbab05e61fe | 8c095e5650de124975b0de0e36063681090f57d49837e6ce33b9b17bc7d8fe90 |
| DependenciesWorld10 | source/game_sa/X87Math.h | 7751a4e84ebe3f695e73174a8c006de9412c08aed0a27d8c37a7f6ba888a1e97 | 098e7579cf41d61519603bd3a133ddf00212a7006da14fa82a9866f7f1b83c90 |
| DependenciesWorld11 | source/game_sa/RwHelper.cpp | 6ea8a91c15a09c6d02babc0c5ccfa2261fb206bd1f7812d111edb94be2889ec9 | 769661aea9d37b15793e56e282c77ae27406e608576808f9a776a0de4808a94c |
| DependenciesWorld11 | source/game_sa/RwHelper.h | ef2f46800f300cee7ce7584dffebb5bd3f9f9c834598a30716ae387f686df735 | 37cb8ba332c4b3cfc3eaf639e293012d17f8083b9815d3348e62ace5fd792205 |
| DependenciesWorld11 | source/game_sa/RenderWare/rw/rwplcore.h | 1e4709dbbc43b0f14681dd53707bff4d3974e3215325af7cf1450684e8e55362 | b62b6c00f476d106ec59a7a9f4e3571eee5e4a50fad028efff5974e4c81e65fa |
| DependenciesWorld11 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesWorld13 | source/game_sa/PathFind.cpp | b289ad134233f537732d2ff6555ae5d294e0d493bc9bc0e164a13f3b42bda5f2 | 10f68895e0166e581cfe17a4b9e5736ebf2c99985aec5cb72ff0c669dbf5b3e9 |
| DependenciesWorld13 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesWorld14 | source/game_sa/CarCtrl.cpp | 6073bf49f30376e7e9175545b579c099418146b7c1ff2ec6d0b80bcd396ee93f | d9236aeeb52b5bffbf53ccae1d1991b699e6e0ea7e0e4f6aaf7754b4b039693a |
| DependenciesWorld14 | source/game_sa/PathFind.cpp | 92323d8ab4c18b41ddc23ec7ed275d6feb5ce68e5d44b7edd4f51cf43bf17496 | 10f68895e0166e581cfe17a4b9e5736ebf2c99985aec5cb72ff0c669dbf5b3e9 |
| DependenciesWorld14 | source/game_sa/Entity/Vehicle/Vehicle.h | fe438d94b2ab8867d9099eb938c1822f240555d5543ad299212ba3c86f125b6d | ef1ec3e5a298cebb325f63b31cacb1c5f6b1fb207838ebc96ecfa79285bd71f9 |
| DependenciesWorld14 | source/game_sa/Entity/Placeable.cpp | bf57b97b53cd9e8326d97de870b9934dd487effde705a3462061d883e052e16a | 0030861304db9f417c8fd8b322a7be006ef4e5a099238c157e5dc0cf0d018e24 |
| DependenciesWorld14 | source/game_sa/Pools/Pools.h | 5ea9374b3db5cfc399404dbab9907ada18691dc16ad4acc24a25472ef2333ebe | 1a38f6b00064b093f890e33c6889661d6d5a145333e124bc406057337c7bef93 |
| DependenciesWorld14 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesWorld4 | source/game_sa/Garages.cpp | e735bbddabf34637c7717d15d7a2aeb44f92404af9c8a00085a380435cf5272c | b6d11644933778f0cd5919d3c59d55b1b88256e302a0476b00af8b4968ed439d |
| DependenciesWorld4 | source/game_sa/Garages.h | 6ef471e9a50620eab5f6d29927305d92979d7a79a7d0e15d935efda563ff02bc | f71fc96fd17d9d22cba76e913da868dea9b8359fe466eea9465241a3f093f658 |
| DependenciesWorld4 | source/game_sa/Garage.cpp | 5f2c2afdb268e5a62c6d63165cead5ff495919195ae5b0e0f864e340fe48fba6 | 4498a718d91e34ddc7e093d171b8c7a8e1a0c54b699980b493deb2a65c1f3808 |
| DependenciesWorld4 | source/game_sa/RenderWare/D3DResourceSystem.cpp | 9950ee6ee636ce9fa8c112e10e3ad7e9539295341faf5f2933f069d55c011a7c | fc251c822d04e5d6adbf4eecd89cfdc54d8b9633a84a605b2e014f1b6b027647 |
| DependenciesWorld4 | source/game_sa/RenderWare/D3DResourceSystem.h | dc060d39860ea99ea7e75a7a473db78cfa59b7c30d50f6aa76291e666187a656 | 31666e8e366ecc5af1a0f006078e4d35c7cd71836e571ea1f776cedd8ee82b53 |
| DependenciesWorld4 | source/InjectHooksMain.cpp | 7a912880d0ff2128cb6654e679653c12ac91c8fa2e94784475ee43cb3ad9360c | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesWorld4 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesWorld9 | source/game_sa/X87Math.cpp | ff115d0bb000b3af1b792fb65e58c763392c2d4ca848e82bfb6bbcbab05e61fe | 8c095e5650de124975b0de0e36063681090f57d49837e6ce33b9b17bc7d8fe90 |
| DependenciesWorld9 | source/game_sa/X87Math.h | 7751a4e84ebe3f695e73174a8c006de9412c08aed0a27d8c37a7f6ba888a1e97 | 098e7579cf41d61519603bd3a133ddf00212a7006da14fa82a9866f7f1b83c90 |
| DependenciesWorldFxFont5 | source/game_sa/Fx/Fx.cpp | d7a80190b86bdbefd8fe91d86ecfac8b13a6dab4050a9916c307536d5f0ab0a0 | 833b5044b63f1df530117f8e7fe604b0e7a0e6be9e3a01c9abad68b2dc5117b8 |
| DependenciesWorldFxFont5 | source/game_sa/Font.cpp | 174b7aa63a0359e167c0903e82764ec3a2239868f767ac89dbbdc5428c340d66 | 20ffade8bc50bfaf800617ab6557554f52d2ccbd217de58cb18603f92d070545 |
| DependenciesWorldFxFont5 | source/game_sa/Camera.cpp | 6504b062b078a9abac940d720b6482894ef29275e139c49f12dfd14548f34557 | 69088a382c52d1f6215f64617f231abf1d665ff3374ca3b404455c4ddf55e6f0 |
| DependenciesWorldFxFont5 | source/game_sa/Radar.cpp | dcbd7cc2103c2739b9510c5b8b01e13a734916e41c4b39aa62079a028f71233a | 28897e1053b0ea1c33a281cfebaa5a53496d193ca6e1af5cefc1bab44562ccc6 |
| DependenciesWorldFxFont5 | source/game_sa/RenderWare/rw/rwplcore.h | 1e4709dbbc43b0f14681dd53707bff4d3974e3215325af7cf1450684e8e55362 | b62b6c00f476d106ec59a7a9f4e3571eee5e4a50fad028efff5974e4c81e65fa |
| DependenciesWorldFxFont5 | source/InjectHooksMain.cpp | 1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291 | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesWorldFxFont5 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesWorldLeaf3 | source/game_sa/Pad.cpp | e16080cdb71094e82dcd756c39cd347aea40f49911e22abe1553cb2ecce269d2 | 1c5dca807d19a1811b60fc19ef2e7a6d98e19788be56a7afac791c9691661981 |
| DependenciesWorldLeaf3 | source/game_sa/Pad.h | d18adb99148fe3fbdde2447a44699451f3afce97f29994bdf3bd29a3829889e2 | fc62f4d8e7190a6164e74c206262649d6f6e720ccb213d843f59edf9a440f52e |
| DependenciesWorldLeaf3 | source/game_sa/Fx/FxFrustumInfo.cpp | b43cc73aff8195e884a21fb759b0c3bf770247464d182e89c933017c452ca3b4 | 78fa799912c2069723f533494d029e7fee9a8aa7a7da36429d54e828b3b550bf |
| DependenciesWorldLeaf3 | source/game_sa/Fx/FxFrustumInfo.h | 51a05b5bc94ea9665bd0396dde2570a7bc729167f26782fcdd5428e2e0cd32aa | 3183d338720b3eeef8ddafbe963c8352ae847d7f6637739683f5ebbe484dfab0 |
| DependenciesWorldLeaf3 | source/InjectHooksMain.cpp | 9dfbb99233c6ca03c5aa68186aeb26d5ed8e2d3c394c747ac7e38b033975877e | 9120d33798aa23d337fdcfd3411d1e90fd9252eff4a24e0280490d6fd62db0b8 |
| DependenciesWorldLeaf3 | reversing/OriginalDependencies499/HookRoute.hpp | 82d7c39915d1eafcd5f964713a6c8a948dabb6275b16bafb514341f754029acf | 287b41ad22b989ed4acf86350d29f2e5f47d42e5878b19ec06e094dd31b6d377 |
| DependenciesWorldWaterPath6 | source/game_sa/X87Math.cpp | ff115d0bb000b3af1b792fb65e58c763392c2d4ca848e82bfb6bbcbab05e61fe | 8c095e5650de124975b0de0e36063681090f57d49837e6ce33b9b17bc7d8fe90 |
| DependenciesWorldWaterPath6 | source/game_sa/X87Math.h | 7751a4e84ebe3f695e73174a8c006de9412c08aed0a27d8c37a7f6ba888a1e97 | 098e7579cf41d61519603bd3a133ddf00212a7006da14fa82a9866f7f1b83c90 |

## Hallazgos

Sin discrepancias detectadas.

## Logs verificados

- DependenciesCarCtrl2: `c1af42f0a22109819641b03c915febcadff5b5217a6197653dba37337b97ebd5`
- DependenciesCarCtrl4: `c5123d0f3ebb6d8e33d0649d019a69beb7f88349f6c8865bd6636a7efb243d76`
- DependenciesCollisionAdapters1: `91ea740f08b68b530c82d86e3bbf3aec6922a7eb3ba9bbe126742f2f4413162d`
- DependenciesDecisionPed7: `927670cd796abe89721277f3f93bd3b7439bd68cd1f3e5b9ab0cb2fc20e439d3`
- DependenciesDecisionRelationship6: `ca2fd4dbf5b7b012219952b8ed8a5add89471543251d755fc7b5f2aa9556cfd6`
- DependenciesFightLeaf2: `24a788f4a01c3d19a61022141626d18a108976abaf089173fe36cffa916c3dd1`
- DependenciesFormationMemory8: `e6a5950555d2d02265bdf3f8baa357cb24b4c93a9ddfd75febc5bc37ee887db9`
- DependenciesGarage2: `df0e9d07208b908a9534968c65c9a9f6e1907af98f53ca9f64b70dcece8775af`
- DependenciesGarageSphere15: `d3fae6b4129bad51ef489972e1065b8bc62c80919e1b6150df5e9a0994b85d1b`
- DependenciesGroupTasks14: `92136e376ac4aa3aafff49df56f0b5fc7049177c2eb36ba85669fc5ab20e462f`
- DependenciesLeaf2: `0777a00c0c1c301c14426c85a64a712c39082481fa3353c52203077420dfe439`
- DependenciesMathLeaf3: `89b9293858492f789c19fcbd80e5f2b07fae893028188938821bc805f5712daa`
- DependenciesMathSprite2: `8034887476f9bb9d8a0159ba4d5135ae3c97c8356d4d96d498c7d40f3b13094c`
- DependenciesMemory9: `1ee38e958e608ad4fb9419db8b8fcda35cfdfa8174112b5a4e926c8e93f7187d`
- DependenciesPadCamera7: `794ccaced29634e35862395dd61827b2e1dd212f1d1101f6c5f3cb6adc4d6f49`
- DependenciesPathDensity13: `940d436c34f116aa13293227867476a7989b7a4503e7331e347b4954e2816d23`
- DependenciesPathOrientation14: `2255b4002c70d1107811a06279719998a90ea19dbd18df19aff32a93cf6e77fd`
- DependenciesPedGroups1: `9f7a8587a905604e6b958af8c420d464cfad4019cea65a2334e261b357ded595`
- DependenciesPedGroups2: `067e8bf1421e4bd7df713e80295851dda0a0ca4df4ad50d6f0bea001a0f956b4`
- DependenciesPointLights1: `79bfb66a332e1a0c91cc473f0cc6f05ecd7d3b4e944c52393641250bec53a5bf`
- DependenciesPointLights2: `9a7ae11fb20ee6ba2cdbd6faf215b140df83d3d022615c344e8a50f7e0176530`
- DependenciesStats1: `a1108bffebe6ad09031acbeb75bcbcb5ea3b66f5fe353cf3a38b7ed1fc745333`
- DependenciesStatsFile15: `2be4a20b70b963d54b5fd86181d75fce0263aa8bd123750dfa8113e4c4a6aa79`
- DependenciesStatsRadio7: `677d56d37d83adf401e75a7fe02b292e12fab8c99c896e645ccfea8d6168b11f`
- DependenciesStatsUi8: `36580e799fa2346a4a9a8186e6f85a0266eb50fa4cd1f1f453a0df1aa4e792c8`
- DependenciesTask10: `d705f3d2e0dec69013a47e7c1059b8522cc601a2eeef9aae297854493c923404`
- DependenciesTask11: `69c2f98b7d0ce6ad0b2a5703903e9572f5a6eba0c4435173eb53f5d61e4ca166`
- DependenciesTask12: `6190fdf7dbbb3b5e2f06eb58a61eec7a5d07bb2a39eeeb2da9a8f1e807fd121f`
- DependenciesTaskCtor4: `2a568acfefefe8a714823dc7c8138c94d74508aa8eb72fc03d24a3ba7d39567d`
- DependenciesTaskLeaf3: `7a706eab70de6fa1d1c311e685abc8d7b6ac6a4128a9c6381950ca19f1b96950`
- DependenciesTaskPed5: `0cdb83d8717931570fdf64fd0454b99713e2d620113cb619e828375bfc34401c`
- DependenciesVehicle12: `37f427d8bc1849beedc8955773fd45d4707707a86c60a8830240ce1061f28a74`
- DependenciesVehicleLeaf5: `c68c3694be5b4a823ec1c50b8bf51dae996041cdbde0f3bbd4ea65f33f20b869`
- DependenciesVehicleStats10: `50544d8ba2914b5252ccb05a63499b0ec373d81ef9fdef0b115de8e79933dd29`
- DependenciesVehicleStats11: `ae39436bbea11f98d8c2721fd7c110902cfb1363502669581ffe5915ceec9693`
- DependenciesVehicleStats9: `d218e38f10bf24eba551286ac0f40a891d0706725a7b61c01ee80a8331b09725`
- DependenciesWorld10: `3953e6fa008286932da1e52a795aa78a3f8c04d6aa46fc6ae866fc1b4a41717c`
- DependenciesWorld11: `6b4a37ad81248b31488d389297df59814b305b7ae4992dc2bd927372b708c83d`
- DependenciesWorld12: `447134b8f72ccc034ba695366650c44194e23877b8ab7fabf488a335342d5e87`
- DependenciesWorld13: `3bc61396daf2022ecda49ebfd78f19bec0ccd71314b889c0c7e2b7882f2793ce`
- DependenciesWorld14: `09315c7f25873e6b184495907b58f301b304b242d20ffed90655409ff380e8e8`
- DependenciesWorld15: `608f5e8929e015028a289affef67b1b02ade722b6bf9b09f6a137b97eacbc5d5`
- DependenciesWorld4: `ab6b3595832e84b362842f0bfbee868d3d62bf4c35b797b21a8f043ab1b49c49`
- DependenciesWorld9: `c09858e3f36a96a0a40b6542fefa1572a78f782adccf9bbb3535ff631ca4bca1`
- DependenciesWorldFxFont5: `1f39571e6dc281aaa5cbd78c53e6a1944a9e476f6fe6f9370b57417feb5c5ccf`
- DependenciesWorldLeaf3: `a3625fcaeae6de3196cba11a4a819be671320bdf60ece622b9cff589485cebef`
- DependenciesWorldWaterPath6: `aa8894e625d3e78ecbd974f7524c44883dc9440d61e5ee05e9b5a678a802faa6`
- DependenciesZones13: `4ca06be5e3a60ee1403bab57747bee7f92e264356ddacbe533d91191f7959e15`
