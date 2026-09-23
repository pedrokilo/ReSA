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
