<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Veinte archivos de nivel 2 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó el ensamblador de `Pickup.cpp`, `Entity/Object/Object.cpp`, `Entity/Vehicle/QuadBike.cpp`, `GangWars.cpp`, `PedGeometryAnalyser.cpp`, `Timer.cpp`, `Weapon.cpp`, `Weather.cpp`, `PedGroupPlacer.cpp`, `Audio/Entities/AEWeatherAudioEntity.cpp`, `Events/EventKnockOffBike.cpp`, `Hud.cpp`, `ProjectileInfo.cpp`, `CarEnterExit.cpp`, `Entity/Ped/PlayerPed.cpp`, `EntryExit.cpp`, `Entity/Vehicle/Train.cpp`, `Pickups.cpp` y `PostEffects.cpp` (86 funciones fuente), y se convirtió `NormaliseQuatExe` de `Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp`. Un subagente por archivo preparó el análisis; compilación y bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

Cada función se comparó con el desensamblado del EXE y se convirtió a float puro en el mismo orden (con `float_control(precise)` donde `/fp:fast` podía reasociar) o a ayudantes nuevos de `X87Math` (los transcendentales, en funciones aparte sin `precise`). Los antiguos envoltorios `naked` que reproducían el residuo de registros pasan a C++ con `ResidueRegsCpp`, `EntryEaxCpp`, `CarEnterExitEntryEdxCpp` y `PlayerPedResidueFloatCpp`, con `#pragma runtime_checks("", off)` y `safebuffers` donde hacía falta; `contrib/audit_register_residue.py` los verifica sobre el código objeto (se ampliaron sus objetos y patrones, sin relajarlo).

## Qué se encontró y cómo se resolvió

- `Data8b` falló al principio: la DLL llamaba directamente a `CCamera::IsSphereVisible` y a `CPathFind::FindNodeOrientationForCarPlacement` y el banco solo interceptaba las direcciones del EXE. Se añadieron en `reversing/Data8b/LiveComparison.hpp` las interceptaciones en la entrada de la DLL con el mismo Fake; no se quitó ninguna compuerta. Quedó en 17024 pares sin diferencias.
- `RpAnim43` se colgó en el primer intento («GTA no responde»: ventana sin progreso 30 s, abortado sin llegar a comparar) y aprobó en el segundo con todos los pares iguales; la causa del cuelgue no se investigó y la corrida fallida se conserva como evidencia.
- El auditor de residuo marcó correctamente `__RTC_CheckEsp` en el puente de `PlayerPed`; se corrigió el código, no el auditor.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
| AudioCompletion | 10086 |
| CarEnterWave4 | 3840 |
| CarQuit355 | 4096 |
| Data8b | 17024 |
| Fx43 | 15552 |
| FxPost1 | 15488 |
| GangEvent1 | 17792 |
| GangWarsPending | 3072 |
| GeometryCore355 | 4608 |
| GeometryRoutes355 | 3072 |
| GroupPlacerPending | 3072 |
| HookUnlockAudit | 2048 |
| HudCamFinal2 | 6528 |
| HudSubtitles355 | 1024 |
| Misc61a | 20992 |
| Misc61b | 21504 |
| MiscWave2 | 19264 |
| Object48 | 18368 |
| PickWeap1 | 11020 |
| PickupsPending | 2048 |
| PlayerPed51 | 11648 |
| ProjectileAdd355 | 16384 |
| ProjectileUpdate355 | 8192 |
| RequestedGeometryPlanesV2 | 2048 |
| RequestedGroupPlacerCorrectedV2 | 1536 |
| RpAnim43 | 13248 |
| Sys13 | 15872 |
| TrainPosition355 | 1024 |
| TrainWave3 | 1024 |
| WeaponDrive355 | 1024 |
| WeaponFinal2 | 7808 |
| WeaponInstant355 | 20480 |
| World11 | 16704 |
| World12 | 11392 |

## Límites

- `RpAnimBlend.cpp` conserva seis bloques de ensamblador: son puentes al EXE con el argumento en EAX (código compilado pero desactivado con `USE_COPY_PASTE_FRAME_UPDATE=0`) y dependen del EXE.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| AEWeatherAudioEntity.actual-before.obj | 73d0f63be44b0dc9036b83790f71872ee88fb757fe1832672158ef352f1e1bf8 |
| CarEnterExit.actual-before.obj | b511c0bf993d6ade1a58f20dbed6b71f6011ffd65d2f0b86655c6fc913f80cbe |
| EntryExit.actual-before.obj | 409df31e971d03323d29e25fcbb39e3b7e13d28eda23f204bb7aa1321d4d730d |
| EventKnockOffBike.actual-before.obj | 6337aa4a2cefbd435c422e0dc7b22ec9d02e6894fc1b7d4bcdd44c6db02d6b28 |
| GangWars.actual-before.obj | dd1f9fb0ff62a5db654a9234dd04454359dc618a3f789761a9869577bdf588ef |
| Hud.actual-before.obj | ad56394b7aa8403df1e545997749a37c51077bbb7db759a84f9bcdbdb175325b |
| Object.actual-before.obj | 233e4b663a1dfeb4b82246cf01373cee6a2ec83defa39bc55aca1f46772b5a68 |
| PedGeometryAnalyser.actual-before.obj | 20c7828d8ea7030fa2f867c25855899ba68101710766078e41f5feff8b4724de |
| PedGroupPlacer.actual-before.obj | eb252bee4e65cf1d0280a7db1e782e19a977b1a906bd462590018521c4d9afca |
| Pickup.actual-before.obj | e2c6b50e9447c35cd800f8933b82025278bd803c7c756a63c364797a3c745604 |
| Pickups.actual-before.obj | a584c420858d7c100d39758e6f5b3d3c03fc3ee753540f691373a2e2b372287a |
| PlayerPed.actual-before.obj | 76dd100f1a9e6f52a93590087193fc7533f3235cde07421e2434a5fd89cca055 |
| PostEffects.actual-before.obj | 44fd5c26827168b41a5eb87a03f424ccf87af90555664f6acb99b8aa7fe979a0 |
| ProjectileInfo.actual-before.obj | 8b9c3b3d2d9d4f4046584e1dc2b1389b79beb91dec4fc04eb4973b23ba56d157 |
| QuadBike.actual-before.obj | 331c364835fb859b08f31e267330c81512b822a4533846e8734f72c804a83074 |
| RpAnimBlend.actual-before.obj | 247cae18a1616753839ff234cd55a262c4cdf71cc9fe2f5887c05b309d97b9a1 |
| Timer.actual-before.obj | e52d1621a29b5b34574aa957fab2144f040aa41a6f2f46658b528950bc250ac1 |
| Train.actual-before.obj | 37799d2f219af27fc0215baf6eadf34d2b04127427bf46900b0e1f0e581c2c1c |
| Weapon.actual-before.obj | c36878de291a4fdd64a73803beadce31bcd9cd7b70ed36805d0108e2b733c599 |
| Weather.actual-before.obj | 18bd4141bcdef62791da75fad7c49e4a1d5c40332e19318db79abfc378132ac9 |
| X87Math.actual-before.obj | 58892371ba45aee6837c2053c7db9a8b16fbf84ca1fd43fa1d8bc04a394c5248 |
