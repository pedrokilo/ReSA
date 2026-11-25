# SilentPatch, segunda tanda: garajes, coches guardados e impound

Rama `silentpatch2-garajes` (worktree `D:\Original\wv-spg`). SilentPatch es de Adrian Zdanowicz (Silent), licencia MIT (ver [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Todo lo de este documento son desviaciones deliberadas y opcionales del EXE original; con `GTASA_SILENTPATCH=0` el comportamiento es el del EXE. Fila a fila del changelog: [silentpatch-clasificacion/garajes.csv](silentpatch-clasificacion/garajes.csv). Registro de desviaciones: sección 15.4 de [DesviacionesConocidas.md](../DesviacionesConocidas.md).

## Resumen

| L | Arreglo | Opción | Estado |
| --- | --- | --- | --- |
| 28 | Trenes guardados en garajes | `TrainsInGarages` | implementado y ejercitado |
| 54 | Coches aleatorios en el impound | `ImpoundRandomVehiclesFix` | implementado y ejercitado |
| 55 | Impound solo coches y motos | `ImpoundOnlyCarsAndBikes` | implementado y ejercitado |
| 75 | Bombas de coches guardados | `StoredCarBombFix` | implementado y ejercitado |
| 83 | «Keep weapons» en Nueva Partida (no es de garajes) | `ResetVariablesOnNewGame` | primera tanda; no ejercitado |
| 185 | Impound recupera el último coche | `ImpoundGaragesFix` | implementado, ejercitado en parte |
| 186 | Impound guarda el coche al ser arrestado | `ImpoundAfterBustedFix` | implementado y ejercitado |
| 195 | Resprays gratis en Nueva Partida | `ResetVariablesOnNewGame` | primera tanda; no ejercitado |
| 203 | Pay 'n Spray limpia con la puerta cerrada | `PayNSprayCleanLate` | implementado y ejercitado |
| 204 | Pinturas no se pierden al guardar | `PaintjobsInGarageFix` | implementado y ejercitado |

Ninguna de estas viñetas tiene clave en `Config\SilentPatchSA.ini`, así que todas están activadas por defecto y ninguna cambia el ritmo ni la jugabilidad de forma notable (cambian qué coche vuelve o cómo se guarda).

## Lo que se cambió (código)

* `source/game_sa/Garage.cpp`
  * `CStoredCar::RestoreCar` (0x447E40): en la rama por defecto del `switch` (tipos 0, 5, 6 y 7, que el EXE construía con `CAutomobile`), si el modelo es un tren (`CModelInfo::IsTrainModel`, 0x4C5AD0) se construye un `CTrain` (0x6F6030, el mismo bloque de 0x988 bytes), se marca `bNotOnARailRoad` (descarrilado) y se le quitan `bDisableCollisionForce` y `bDisableSimpleCollision` (los dos bits que el constructor de `CTrain` fuerza). Es el `CAutomobileCtor_OrTrain` de SilentPatch (SilentPatchSA.cpp:4660), que se enganchaba en la llamada 0x4480D0.
  * `SpRestoreStoredBomb`, llamada desde `CGarage::RestoreCarsForThisHideOut` y `CGarage::RestoreCarsForThisImpoundingGarage` justo después de `RestoreCar`: si el coche guardado tiene bomba y el vehículo es coche o moto, se pone `m_nBombOnBoard` y `m_pWhoInstalledBombOnMe = FindPlayerPed()`. Es `RestoreCar_LoadBombOwnership` (VehicleSA.cpp:818) enganchado en 0x448550+0x1A y 0x4485C0+0x1B.
  * `CGarage::Update` (0x44AA50), Pay 'n Spray (tipo 5, estado 1 a 2): la suciedad (`+0x4B0`) ya no se pone a 0 al empezar a cerrar la puerta (SilentPatch hace `Nop(0x44ACDC, 6)`). El EXE la vuelve a poner a 0 con la puerta cerrada.
* `source/game_sa/GameLogic.cpp`, `CGameLogic::Update`, rama `GAMELOGIC_STATE_BUSTED`: no se llama a `CCarCtrl::ClearInterestingVehicleList` (SilentPatch hace `Nop(0x443292, 5)`).
* `source/game_sa/Entity/Vehicle/Vehicle.cpp`, `CVehicle::GetRemapIndex` (0x6D0B70): hunk mínimo; si la pintura cargada (`m_nPreviousRemapTxd`, +0x598) es -1 se usa la pendiente (`m_nRemapTxd`, +0x59A) y, si también es -1, devuelve -1. Es `CVehicle::GetRemapIndex` de SilentPatch (VehicleSA.cpp:393), que enganchaba 0x6D0B70. `CStoredCar::StoreCar` llama a esta función para guardar la pintura.

Cada sitio lleva el comentario `// SilentPatch (MIT, Silent): <arreglo>` y, con la opción apagada, ejecuta el código del EXE sin cambios.

## Offsets crudos verificados contra el desensamblado del EXE (1.0 US, `_comparar\orig\gta_sa.exe`)

`Garage.cpp` usa offsets crudos en `CStoredCar::StoreCar`/`RestoreCar` y en `CGarage::Update`. Los que se usan en esta tanda, comprobados con capstone:

| Offset | Significado | Evidencia en el EXE |
| --- | --- | --- |
| vehículo +0x590 | tipo de vehículo (`m_nVehicleType`) | 0x4498AB (`StoreCar`) y 0x4481B1 (`RestoreCar`) lo comparan con 0 y 9 |
| vehículo +0x4A8 (bits 0-2) | bomba a bordo | 0x4498BA (`StoreCar`), 0x4481D7/0x4481E9 (`RestoreCar`), 0x44B48B/0x44B69D (tienda de bombas) |
| vehículo +0x4CC | dueño de la bomba | 0x44B6C4: `mov [eax+0x4CC], edi` en la tienda de bombas (el jugador) |
| vehículo +0x4B0 | suciedad | 0x44ACDC: `mov [eax+0x4B0], ebx` (ebx = 0) tras `FindPlayerVehicle`; coincide con el sitio de SilentPatch |
| vehículo +0x598 / +0x59A | pintura cargada / pendiente | 0x6D0B93 compara con +0x598; 0x6D0BEB y 0x6D0BF2 escriben +0x598 y +0x59A en `SetRemapTexDictionary` |
| llamada 0x4480D0 | `CAutomobile::CAutomobile` dentro de `RestoreCar` | `call 0x6B0A90`; precedida de `push 0x988` / `call 0x6E2D50` |
| 0x6F6030 y 0x4C5AD0 | `CTrain::CTrain` y `CModelInfo::IsTrainModel` | prólogo del constructor y `cmp [ecx+0x3C], 6` |
| 0x425179, 0x425369, 0x425411 | llamadas a `IsPointWithinHideOutGarage` (0x448900) de `PossiblyRemoveVehicle` | primera tanda; coinciden con los tres sitios de `CarCtrl.cpp` |
| 0x566794 y 0x56A378 | `IsThisVehicleInteresting` seguido de `StoreCarInNearestImpoundingGarage` | los dos pasan por `CWorld::RemoveVehicleAndItsOccupants` en nuestro código; el filtro en `StoreCarInNearestImpoundingGarage` equivale a las dos intercepciones de SilentPatch |
| 0x442EE7, 0x443292, 0x44353D | las tres llamadas a `ClearInterestingVehicleList` (0x423F00) de `CGameLogic::Update` | muerto, arrestado y misión; SilentPatch anula la del medio, que es la de la rama `GAMELOGIC_STATE_BUSTED` (la que llama a `FindClosestPoliceRestartPoint`) |

La primera tanda dejó la viñeta 186 como «pendiente» porque no distinguió cuál de las tres llamadas era; la identificación de arriba la cierra.

## Pruebas (standalone, memoria + entrada real)

Ejecutor: `reversing\FnJuego\ejecutar_garajes.ps1` (copia de `ejecutar_juego.ps1` con la sonda de garajes y la variable `-Entorno "A=0;B=0"`), sonda `reversing\FnJuego\sonda_garajes.py`. Ambos toman el candado `D:\Original\gta_lock`. Partida 2 limpia en `D:\Original\_juego_run`, standalone enlazado desde esta rama (`reversing\FnFase8\salida\gta_sa_standalone.exe`). La sonda escribe coches guardados en la casa 0 (`aCarsInSafeHouse`, 0x96ABD8), acerca a CJ a la puerta de su garaje (`cjsafe`) para que el juego los restaure, lee el estado del vehículo y vuelve a alejarse para que el garaje los guarde. Cada prueba se repite con la opción apagada. Evidencia: las salidas de la sonda de las ejecuciones citadas (y las líneas `SilentPatch: opcion X = 0` del `log.log` de cada una) están copiadas en `reversing\FnJuego\evidencia\garajes\<etiqueta>-<fecha>.log`; las carpetas completas quedan sin versionar en `reversing\FnJuego\ejecuciones\`.

| Viñeta | Qué se hizo | Opción encendida | Opción apagada |
| --- | --- | --- | --- |
| L28 trenes | un Streak (538) en la casa 0, CJ junto a la puerta | `tren_on-20261125-043137`: restaurado como tipo 6 (tren), `m_nTrainFlags` = 0x4399 (bit 8, descarrilado), físicas 0x10000202 (sin los bits de masa y colisión forzados); el proceso sigue vivo | `tren_off-20261125-043314`: el proceso termina con 0xC0000005 al restaurar el tren |
| L75 bombas | Infernus con bomba 2 y Freeway con bomba 1 | `bom_on-20261125-042958`: los dos vuelven con la bomba y `+0x4CC` = el ped de CJ; al cerrar la puerta se guardan con bomba 2 y 1 | `bom_off-20261125-043059`: el coche con bomba 2 pero dueño 0; la moto sin bomba (0); guardados: 2 y 0 |
| L204 pintura | Elegy con pintura 1, restaurado y guardado sin dibujarlo (`m_nPreviousRemapTxd` = -1, `m_nRemapTxd` = 3278) | `pin_on-20261125-043215`: se guarda `pintura` = 1 | `pin_off-20261125-043353`: se guarda 255 (se pierde) |
| L54 apCarsToKeep | Infernus restaurado, puesto en `apCarsToKeep[0]` (con su marca de tiempo) y destruido por el garaje al cerrar | `keep_on-20261125-044545`: `apCarsToKeep[0]` = 0 | `keep_off-20261125-050012`: `apCarsToKeep[0]` conserva el puntero del coche ya destruido |
| L203 Pay 'n Spray | Infernus restaurado, CJ entra en él, el coche se coloca en el centro del Pay 'n Spray de Idlewood con suciedad 14, se muestrea (estado, puerta, suciedad) cada 10 ms | `pns_on-20261125-044734`: en estado 2 la suciedad se mantiene (11.0) mientras la puerta baja de 0.98 a 0.0 y se limpia después (0.0 ya en el estado 3) | `pns_off-20261125-044844`: en el primer muestreo del estado 2, con la puerta aún en 0.87, la suciedad ya es 0.0 |
| L186 impound al ser arrestado | Infernus y Maverick restaurados en la calle de CJ y puestos en `apCarsToKeep`; se escribe `PEDSTATE_ARRESTED` (63) en CJ | `bus_on-20261125-051243`: tras el arresto aparece un coche nuevo en las ranuras guardadas: casa 12, ranura 0, modelo 411 (Infernus); el Maverick no | `bus_off-20261125-052022` (`ImpoundAfterBustedFix=0`): no aparece ninguno |
| L55 solo coches y motos | misma prueba | `bus_on-20261125-051243`: el Maverick (helicóptero) no se guarda | `bus_all-20261125-052106` (`ImpoundOnlyCarsAndBikes=0`): se guardan los dos (casa 12, ranuras 0 y 1: 411 y 497) |
| L185 garajes de impound | Infernus restaurado dentro de `imp_la` (garaje de impound de Los Santos) y otro en una calle abierta, con la gravedad quitada para que no caigan (la colisión de esa zona no está cargada), CJ a ~1000 m | `impz_on-20261125-045312` y `impz_on-20261125-052400`: el Infernus del impound sigue vivo a los 60 s; el de la calle desaparece al instante | `impz_off-20261125-045749` y `impz_off-20261125-052150`: ninguno de los dos se llega a ver en muestreos de 20 ms (se eliminan casi al crearse) |

Notas de honestidad sobre las pruebas:

* Lo que se mide son efectos observables en memoria del proceso (estado de los vehículos, ranuras de coches guardados, lista `apCarsToKeep`), con CJ real, el juego real y las funciones C++ del standalone. Las posiciones y los coches guardados se escriben con la sonda para provocar el caso; la restauración, el cierre de la puerta, el arresto y el guardado los ejecuta el propio juego.
* L185: la prueba no reproduce «el jugador recupera su coche tras empezar una misión», solo la condición de la que depende (`PossiblyRemoveVehicle` no elimina un coche situado dentro de un garaje de impound). Con la opción apagada no se observó el instante de la eliminación (menos de 20 ms).
* L186: el arresto se provoca escribiendo el estado del ped; el resto del flujo (BUSTED, multa, resurrección en la comisaría, `ClearExcitingStuffFromArea`) es del juego.
* Varias ejecuciones sueltas (`bus_*`, `keep_*`, `impz_*` con fecha anterior a las citadas) son intentos con defectos de la propia sonda (coche demasiado lejos y eliminado, marca de tiempo de `apCarsToKeep` sin fijar, el coche cae al vacío); se conservan en `ejecuciones\` pero no son evidencia.
* En algunas ejecuciones (alrededor de un tercio) el proceso registra una excepción de primera oportunidad (0xC0000005, dentro de ntdll) a los 2 s de arrancar, antes de cargar el juego y antes de cualquier código de garajes (`excepciones=1` en `fase8_arranque.log`); aparece con la opción encendida y apagada y no impide el resto de la ejecución. No se ha investigado; no es de esta rama.

## Verificación final de la rama

* Compilado con `reversing\FnFase8\compilar_over.cmd` (bajo `compile_lock`): `Garage.cpp`, `GameLogic.cpp` y `Vehicle.cpp`, sin errores.
* Standalone enlazado con `enlazar.cmd D:\Original\wv-spg D:\Original\wv-spg\reversing\FnFase8\salida D:\Original\wv-int\build\source\gta_reversed.dir\Debug` (`link exit=0`, sin símbolos sin resolver).
* `python contrib/audit_hook_catalog.py --strict`: OK. `python contrib/auditoria_datos/tamanos_objetos.py lint`: OK (0 fallos).
* Humo con la ASI de esta rama (`AGUA_ASI=D:\Original\wv-spg\reversing\FnFase8\asi\gta_reversed.asi`, enlazada con los `.obj` de `wv-int\build` más los tres de `over\` de esta rama): `prueba_ropero.py cal_mal.args` y `prueba_marcadores.py cal_mal.args` dan `VEREDICTO BIEN` (ropero: etiqueta `spg3_prueba_ropero_1`; marcadores: `spg_prueba_marcadores_2`). Hubo varios intentos fallidos con «sin juego» / «el juego se cerró» antes: en los comprobados el motivo era que otro agente tenía `D:\Original\gta_lock` (el registro de `jugar.ps1` lo dice), no un fallo del juego.
eversing\FnFase8si\gta_reversed.asi`, enlazada con los `.obj` de `wv-intuild` más los tres de `over\` de esta rama): `prueba_ropero.py cal_mal.args` y `prueba_marcadores.py cal_mal.args` dan `VEREDICTO BIEN` (ropero: etiqueta `spg3_prueba_ropero_1`; marcadores: `spg_prueba_marcadores_2`). Hubo varios intentos fallidos con «sin juego» / «el juego se cerró» antes: en todos el motivo era que otro agente tenía `D:\Original\gta_lock` (el registro de `jugar.ps1` lo dice), no un fallo del juego.
* Ejecución del standalone de 120 s (`spg120-20261125-053211`, escenario `reposo120`): el proceso sigue vivo los 120 s y sale por el límite (`exit=0xDEAD0002`), 60 latidos con `excepciones=0`.

## No verificado

* Viñetas 83 y 195 (reinicio de variables en Nueva Partida): son de la primera tanda y exigen empezar una Nueva Partida desde el menú.
* Un tren no se puede guardar de nuevo en el garaje de CJ (mide más que el garaje: `EntityHasASphereWayOutsideGarage` lo descarta, igual que en el EXE y en SilentPatch); solo se probó la restauración, que es donde se caía el juego.
* Los arreglos de esta tanda se ejercitaron solo en el standalone. Con la ASI sobre el EXE original solo se pasó el humo (ropero y marcadores); no se repitieron las pruebas de garajes con la ASI ni se compararon con el EXE original puro.
