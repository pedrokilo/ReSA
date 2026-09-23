# Pruebas y evidencias de reconstrucción

Esta carpeta sí se utiliza. Ejecutar `PROBAR.cmd` desde la raíz para elegir una prueba; `PROBAR.cmd --list` muestra los casos disponibles. No se necesita un CMD por función.

`PROBAR.cmd --all` ejecuta en serie todos los bancos dentro de GTA. Para evitar la pausa del CMD, usar `python reversing/run.py --all`. Abre y cierra la copia de pruebas para cada banco, vuelve a analizar sus logs nuevos y comprueba la restauración del ASI/PDB y los hashes de las partidas antes de continuar. Los informes conjuntos quedan en `reversing/artifacts/suite-*/summary.json`, con salidas por caso y rutas/hashes de la evidencia. Se omite `pcsave-isolated` porque la misma función ya se prueba dentro del motor.

Esta serie usa las sondas archivadas de cada trabajo. No es una prueba de integración de todos los cambios en una única compilación actual ni sustituye la campaña de estabilidad futura.

- `run.py` y `cases.json`: menú y catálogo común.
- `CVehicle`, `C_PcSave`, `CPlayerPed`, `CPed`, `CWeaponEffects`: pruebas repetibles y evidencia de las funciones trabajadas.
- `RpAnimBlend`: actualización de animaciones, validada mediante 652 comparaciones dentro del motor.
- `CBaseModelInfo`: retorno de SwaysInWind, validado con 65.536 combinaciones de flags.
- `FxManager`: destrucción de sistemas y recorrido de Update, validados con 720 comparaciones dentro del motor.
- `CClouds`: RenderBottomFromHeight, validada con 1.170 comparaciones exactas de geometría, proyección, sprites y estados dentro del motor.
- `CMenuSystem`: FillGridWithCarColours, validada con 784 comparaciones dentro del motor, 196 paletas y 50.176 índices de color.
- `CTaskSimpleUseGun`: ProcessPed, validada con 3.232 comparaciones dentro del motor: 64 con dependencias reales y 3.168 con dependencias de animación/apuntado controladas; consultar su alcance específico.
- `runtime_runner.py`: ejecución, restauración y registro compartidos; utilizado inicialmente por RpAnimBlend.
- `MEJORAS_DEL_PROCESO.md`: método de trabajo y criterios de validación.

La caché local `CVehicle/conan-cache` sigue referenciada por CMake y las dependencias. No es una carpeta abandonada y no se debe borrar mientras el build dependa de ella. Los binarios, registros extensos y cachés quedan fuera del commit mediante las reglas de Git existentes.

Se eliminaron los siete lanzadores específicos de la raíz y tres copias temporales de fuentes ya restauradas. No se eliminaron informes de pruebas ni código necesario para repetirlas.

- `CPed`: 145/145 entradas registradas; banco `cped-completion` con 28.672 pares de las seis ultimas funciones. Consultar su README y validation-summary.json para dependencias controladas y limites.

- CConversations: 11/11; 2.048 pares de DoneSettingUpConversation. Configuracion de tablas y referencias, sin reproduccion de dialogos.
- CPedList: 7/7; 4.096 pares de sus cuatro funciones pendientes. Fixtures de grupos/pool y consultas de inteligencia controladas.

- `CPlayerInfo`: tres funciones, 16.896 pares dentro del motor; 28/28 registradas y habilitadas; constructor corregido con otros 4.224 pares. Ver README y validation-summary.json.

- `FurnitureSubGroup_c`: 3/3, seleccion por ID/riqueza y almacenamiento; 20.480 comparaciones dentro del motor, --case furniture.


Banco añadido: [CClothes, CScripted2dEffects y Sprint](ClothesScriptedSprint/README.md), 8.448 pares; `python reversing/run.py --case clothes-scripted-sprint`. Integrado en `--all`.

Banco añadido: [CGeneral: RNG float](CGeneral/README.md), 65.536 pares exactos y desbloqueo; `python reversing/run.py --case general`, incluido en `--all`.

[CAESound: copia y referencias](CAESound/README.md): 1.024 pares exactos, Audio activo, caso `sound-copy`. Los ejecutores compartidos activan la carga rápida solo durante las pruebas y restauran ambos INI. [Inicio normal y diagnóstico del cierre](InicioNormal/README.md).

## Diez clases del Top 10 completadas

[Informe conjunto y límites](Top10/README.md): **51.712 pares exactos**, 22 entradas habilitadas y dos funciones auxiliares revalidadas. Los cuatro bancos se repitieron con las diez clases habilitadas antes de instalar el ASI normal.

| Caso autónomo | Banco | Pares |
| --- | --- | ---: |
| `shooting-resources` | [Shooting y recursos](ShootingResources/README.md) | 18.432 |
| `leave-car-bus` | [Salir del coche y autobús](LeaveCarBus/README.md) | 5.632 |
| `population-factory` | [Población, fábrica y amenazas](PopulationFactory/README.md) | 18.432 |
| `scanner-drag-fx` | [Escáner, expulsión y FX](ScannerDragFx/README.md) | 9.216 |

Ejemplo: `python reversing/run.py --case scanner-drag-fx`. Integrados en el catálogo y `--all`, conservando un solo PROBAR.cmd. Esta tarea no repitió toda la suite histórica.


## Desbloqueos y plataforma conmutable

[Informe conjunto](Unlocks/README.md): **24 candados retirados**, 50.432 pares en siete bancos y nueve arranques de plataforma (324 pares repetidos, 18 rutas de entrada). CGeneral conserva su desbloqueo anterior. Incluye correcciones de ABI, pools, física, audio, radio, conducción y SDL/ImGui.

Casos nuevos: `unlock-core`, `unlock-tasks`, `unlock-physical`, `unlock-audio`, `unlock-radio`, `unlock-driving`, `unlock-input` y `unlock-platform`. Todos disponibles en el único PROBAR.cmd y `--all`. Para las nueve combinaciones: `python reversing/UnlockPlatform/run_matrix.py`.

Se conserva la configuración de inicio normal. Los bancos usan carga rápida temporal, restauran ASI/PDB e INI y verifican partidas. Los binarios históricos de bancos no prueban conjuntamente toda la compilación actual; la campaña integral sigue aplazada.


## Entrega parcial: DataStream y TrackEntity

[Informe y trabajo aplazado](UnlockBatch2/README.md): diez candados retirados, 15.360 pares y 64 combinaciones de DataStream. Casos `unlock-data-stream` y `unlock-track-entity` añadidos a cases.json, PROBAR.cmd y `--all`. Los borradores de CFileMgr/Game quedan archivados fuera de la compilación; no se añaden como bancos aprobados. El catálogo conserva los otros 86 bloqueos solicitados para después.


## CEntryExit: reserva agotada

[Diagnóstico y corrección](CEntryExitPool/README.md): reserva ampliada deliberadamente de 400 a 512. Banco de 1.854 comprobaciones, con 21 diferencias conocidas de FIX_BUGS; reproducción de las mismas 19 altas fallidas en original y candidato a 400, ninguna a 512. ASI normal: 120,24 s y 3.404 fotogramas sin fallos de CEntryExit; otros avisos documentados. Caso `entryexit-pool` disponible en PROBAR.cmd y `--all`: 39 casos, 38 dentro del motor y uno aislado. No cambia ningún indicador del CSV.
