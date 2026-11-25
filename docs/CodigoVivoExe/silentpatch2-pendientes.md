# SilentPatch, segunda tanda: viñetas pendientes (rama `silentpatch2-pend7a`)

Fecha: 25/11/2026. Base: `integracion-fase7b` (`01a26b9d5d`). Licencia y atribución: SilentPatch (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent)); ver [../THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md). Los arreglos son desviaciones deliberadas y opcionales: con `GTASA_SILENTPATCH=0` (o `SilentPatch=0` en `gta-reversed.ini`) se conserva el código del EXE; cada uno se apaga con `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0` o `SilentPatch_<Nombre>=0`. Ninguna de las tres opciones existe en `SilentPatchSA.ini`: nombres propios, activadas por defecto (no cambian el ritmo del juego).

Tabla fila a fila: [silentpatch-clasificacion/pendientes7.csv](silentpatch-clasificacion/pendientes7.csv). Filas en [../DesviacionesConocidas.md](../DesviacionesConocidas.md), sección 15.S.

| L | Viñeta | Estado |
| --- | --- | --- |
| 19 | Fallo ocasional junto a escaleras mecánicas | implementado + ejercitado (banco en proceso) |
| 20 | Fallo con el uso prolongado de la máquina expendedora | implementado + ejercitado (banco en proceso) |
| 82 | Animaciones del paracaídas | implementado (guarda) + ejercitada; efecto sobre el paracaídas de serie NO demostrado |
| 84 | Motos sin motor al generarse | bloqueado: sin código en la copia de SilentPatch |

## L19: escaleras mecánicas (`EscalatorsCrashFix`)

* **SilentPatch**: `CEscalator::SwitchOffNoRemove` (`GeneralSA.cpp:156`) encola los peldaños en vez de borrarlos y `UpdateEscalators` (`SilentPatchSA.cpp:1464`) los borra (`CWorld::Remove` + `delete`) al empezar `CEscalators::Update`. Ganchos: llamada en `0x71791F` y llamada de `CMovingThings::Update` en `0x7185B5`.
* **EXE (capstone sobre `_comparar/orig/gta_sa.exe`)**: `0x717900` es `CEntity::RemoveEscalatorsForEntity` y en `0x71791F` llama a `CEscalator::SwitchOff` (`0x717860`), que borra los peldaños (`CObject`) en el acto. Se invoca al descargar la entidad (edificio), en pleno recorrido de streaming/lista de objetos en movimiento, de ahí el cierre «ocasional».
* **Nuestro código**: `CEntity::RemoveEscalatorsForEntity` (`Entity/Entity.cpp`) llama a `CEscalator::RemoveDeferred` (nuevo, `Escalator.h/.cpp`), que anula los peldaños y los guarda en una cola estática; `CEscalators::Update` (`Escalators.cpp`) la vacía antes de actualizar (`CEscalator::FlushDeferredSteps`, con la misma bandera de depuración `0xC6E98C` del EXE alrededor del `delete`). `CEscalators::Shutdown` también la vacía para no dejar punteros colgando al reiniciar. Con la opción apagada se llama a `Remove()` literal.
* **Prueba** (banco `SilentPatchPend7aSelfTest.cpp`, `GTASA_SILENTPATCH_SELFTEST_PEND7A=1`; ejecuciones `reversing/FnJuego/ejecuciones/on3-*` y `off3-*`): se crea una escalera con un objeto como entidad y `CEscalators::Update` crea 14 peldaños (pool de objetos 98 a 112); `RemoveEscalatorsForEntity` deja el pool en 112 (14 pendientes) con la opción y en 98 sin ella; tras el siguiente `Update` vuelve a 98 en ambos casos.
* **No verificado**: que el cierre original deje de ocurrir (es raro y depende del orden de la lista de objetos en movimiento); se prueba el mecanismo diferido, no el fallo.

## L20: máquina expendedora (`TempObjectsFreeFix`)

* **SilentPatch**: reemplaza `CObject::TryToFreeUpTempObjects` (`0x5A1840`, salto) por `TryToFreeUpTempObjects_SilentPatch` (`GeneralSA.cpp:99`): primero libera objetos temporales no visibles (hasta el número pedido) y, si faltan, fuerza la liberación de temporales visibles (hasta `min(procesados, faltan)`). El comentario de SilentPatch lo une al fallo de la máquina expendedora (sus objetos temporales llenan el pool de objetos y `CObject::Create`, que llama a esta función con 5, no consigue hueco).
* **EXE**: `0x5A1840` solo libera temporales no visibles (capstone).
* **Nuestro código**: `CObject::TryToFreeUpTempObjects` (`Entity/Object/Object.cpp`) con la rama nueva y el código antiguo literal con la opción apagada.
* **Prueba** (mismas ejecuciones): 3 objetos temporales (2 visibles, 1 no), `TryToFreeUpTempObjects(3)`: con la opción se liberan los 3 (objetos 100 a 97); apagada solo 1 (100 a 99).
* **No verificado**: el uso prolongado de la máquina en una partida real.

## L82: paracaídas (`SkinBuildingPipelineFix`)

* La viñeta no cita código. Se buscó en `SilentPatchSA/*.cpp|.h`, `SilentPatch/*.cpp` y los `.ini` por `parach`, `chute`, `skydiv`, `ANIM_`, `AnimGroup`, `RtAnim`, `jump`: solo salen la munición del truco (`SilentPatchSA.cpp:7526`), `CPed::RenderWeapon` (`PedSA.cpp:57-105`, ya en `WeaponRenderingFixes` de la primera tanda), `GTARtAnimInterpolatorSetCurrentAnim` (L50, ya hecho) y **`SkinBuildingPipelineFix`** (`SilentPatchSA.cpp:2290`, hook `0x5D7F1E`), cuyo comentario es literalmente «Disable building pipeline for skinned objects (like parachute)». Es la única pista con la palabra «parachute» sobre un defecto de renderizado de animación (a un objeto skinned al que se le cambia el pipeline se le pierde la animación de piel), por eso se implementó; **es una deducción, no una cita**.
* **EXE**: `0x5D7F00` (`CCustomBuildingRenderer::AtomicSetup`) decide entre el pipeline de edificios con colores nocturnos (`0x5D71C0`, llamada en `0x5D7F1E`) y el normal (`0x5D7E50`); la primera rama sustituía `atomic->pipeline` también en átomos skinned.
* **Nuestro código**: `CCustomBuildingRenderer::AtomicSetup` (`Pipelines/CustomBuilding/CustomBuildingRenderer.cpp`): con la opción, si `atomic->pipeline->pluginId == rwID_SKINPLUGIN` (0x116) no se hace nada en la rama DN (como SilentPatch, que solo protege esa rama).
* **Prueba**: la guarda se ejercita sobre un clon del clump del jugador (átomos skinned) forzando la condición de la rama DN (colores nocturnos y prelit ficticios, restaurados): con la opción el pipeline sigue siendo el de piel (`0x116`); apagada pasa al de edificios (`0x53F20098`).
* **Lo que NO se demostró**: el modelo del paracaídas de serie (arma 371) cargado en el banco no es skinned ni tiene colores nocturnos/prelit (`IsCBPCPipelineAttached=0`, pipeline nulo), por lo que con los datos de serie la guarda no cambia nada observable; la mejora visual de «Fixed parachute animations» no se ha medido. Si la viñeta se refería a otra cosa, queda sin cubrir.

## L84: motos sin motor (clase E, bloqueado)

No existe código de SilentPatch que lo trate. Revisado: `SilentPatchSA.cpp` entero (palabras `bike`, `Bike`, `BMX`, `bicycle`, `engine`, `bEngineOn`, `IsBike`), `VehicleSA.cpp/.h`, `ModelInfoSA.cpp`, `ScriptSA.cpp`, `GeneralSA.cpp`, `PoolsSA.cpp`, `SilentPatch/Common.cpp`, `StoredCar.cpp`, `SVF.cpp`, `RWGTA.cpp` y los `.ini`. Los únicos «engine» son los rotores de componentes (L157) y la bandera de motor de `CTrain`. En nuestro código se leyeron los caminos que crean motos (`CCarGenerator::DoInternalProcessing`, `CCheat::VehicleCheat`, `CCarCtrl::CreateCarForScript`) sin encontrar una rama que genere una moto sin su componente de motor, y sin una causa no hay reproducción con la sonda. Haría falta el historial de git de SilentPatch (el commit de esa viñeta). No se ha inventado el arreglo.

## Archivos

* `source/game_sa/Escalator.h/.cpp`, `Escalators.cpp`, `Entity/Entity.cpp`, `Entity/Object/Object.cpp`, `Pipelines/CustomBuilding/CustomBuildingRenderer.cpp`: hunks mínimos con `// SilentPatch (MIT, Silent): ...`.
* `source/game_sa/SilentPatchPend7aSelfTest.h/.cpp` y una línea en `source/app/app_game.cpp` (`Idle`): banco aislado.
* `reversing/FnJuego/ejecutar_pend7a.ps1`: lanzador del banco (toma `gta_lock` solo).
