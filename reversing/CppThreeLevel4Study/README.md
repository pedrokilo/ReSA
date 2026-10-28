<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tres archivos de nivel 4 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `cHandlingDataMgr.cpp`, `PedGroups.cpp` y `TheZones.cpp` (15 funciones fuente). Un subagente por archivo preparó el análisis; la compilación y los bancos corrieron en serie. Ningún banco se relajó. Los cinco bancos aprobaron a la primera; después se ampliaron las comprobaciones (registros y entradas del arcoseno) y se repitieron.

## Qué se hizo

- `cHandlingDataMgr`: tres ayudantes nuevos de `X87Math` (`HandlingAtofTimes04Cpp`, `HandlingReduceVelocityCpp`, `HandlingSinScaledCpp`) y `ExeAsinDegreesCpp` para el `naked` del arcoseno.
- `PedGroups`: C++ plano con un núcleo común (`AddGroupCore`, `InitCore`, `CleanUpForShutDownCore`) y tres envolturas `__fastcall` (`AddGroupExe`, `InitExe`, `CleanUpForShutDownExe`) registradas con los mismos nombres de hook, que terminan en `ResidueRegsCpp` con los EAX, ECX y EDX del EXE.
- `TheZones`: float puro con `_ftol2` emulado con enteros y la celda bajo `float_control(precise)`; la función termina en `ResidueRegsCpp` con los registros del EXE en los cuatro caminos.

## Seguimiento: los límites que se cerraron (15/10/2026)

Tras la primera versión quedaban tres límites escritos; los tres se resolvieron.

- **`_CIasin` con |x| ≥ 1.** Se desensambló `_CIasin` entero (0x821E70) y `ExeAsinDoubleCpp` cubre ahora todas sus ramas: ±1 exacto devuelve π/2 de 64 bits, el dominio (|x| > 1 e infinitos) devuelve el real indefinido y pone `errno` a EDOM, y NaN devuelve la propia x silenciada. La versión antigua daba 90,0 en lugar de 89,99999 para ±1. Se amplió el generador de entradas de `World11`, `Data23` y `WeaponFinal2` con ±1, ±1,5, ±2, ±1e30, NaN, ±inf, ±0 y subnormales (sin quitar compuertas). Los fixtures nuevos destaparon una diferencia ya existente en `ConvertBikeDataToWorldUnits`: `m_fFullAnimLean * constante` se multiplicaba en SSE y descartaba subnormales que el EXE conserva (6 de 256 pares); se corrigió con `X87TimesFloatCpp` (fmul x87), también en `ConvertBikeDataToGameUnits`. `World11`, `Data23`, `WeaponFinal2`, `CTaskDead`, `PickWeap1` y `WeaponInstant355` aprueban.
- **Residuo de registros de `PedGroups` y `TheZones`.** Ahora se reproducen EAX, ECX y EDX del EXE. Los bancos `DependenciesTask10` y `DependenciesZones13` los comparan (con semillas por caso, devolviendo a los colaboradores sus bytes del EXE durante cada llamada y con oráculos independientes) y el auditor `contrib/audit_register_residue.py` incluye ambos objetos, exige que toda salida de `TheZones` pase por el ayudante, que `PedGroups` conserve al menos tres llamadas y que no haya `/JMC` en las envolturas; también admite el `nop` que el compilador sin optimizar deja tras una llamada de resultado descartado (no toca registros). Los dos bancos aprobaron con la comparación de registros activa.
- **Finales de línea.** No era un problema: el repositorio guarda LF en el índice (`eol=lf`) y el directorio de trabajo está en CRLF por `autocrlf`; Git normaliza al confirmar. Los tres archivos que pasé a LF no producen diferencias.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
| CTaskDead | 1792 |
| Data23 | 13904 |
| DependenciesTask10 | 6144 |
| DependenciesZones13 | 2048 |
| Game22 | 5824 |
| GroupPlacerPending | 3072 |
| PickWeap1 | 11020 |
| ScriptsFinal3 | 16384 |
| WeaponFinal2 | 7808 |
| WeaponInstant355 | 20480 |
| World11 | 16704 |

## Límites

- `PedGroups`: el modelo del EDX supone nulos los eventos y asignadores `+0x34`, `+0x38`, `+0x2C0` y `+0x2C4` del grupo (el banco los deja nulos) y la rama de `RemoveMember` para un miembro que no es jugador está deducida del EXE sin ejercerse en el banco; supone además que todo destructor de tarea termina en `CTask::operator delete` (0x61A5B0).
- `_CIasin`: las banderas de estado x87 pegajosas (IE y PE) y el flag 0xC9ABFC (sin escritores en el EXE, supuesto 0 por análisis estático) no se reproducen.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| PedGroups.actual-after.obj | 8e44ea9939d0be07261bc2c6bcf608be80a2773ff42200e28f1d426d011982b4 |
| PedGroups.actual-before.obj | 75dcdc6fbef0cf2761a23c442e2d070592d12d03dc065d1e6e7ff259775249ff |
| TheZones.actual-after.obj | 3c432b3efc76e2f15f34ea0270b3c2b927b11b7822986585992fdc95f516d61e |
| TheZones.actual-before.obj | 0d73814a80a7eddae57bc2dc3ed4d2be089fdf8ccbe4e3a75322fd0dde58e4b1 |
| X87Math.actual-after.obj | b9a6c03cd0a62daa596c043bcb08c74e2f329c6b31ba714c62af4d4634d55c84 |
| X87Math.actual-before.obj | 01466ccb3f9b4e78f182a0321b3454b3598b94d02fadf434fad6c293067aef2e |
| cHandlingDataMgr.actual-after.obj | 8be3191580ed94d0b5d9a8cc6625e9eb6fd4107d3876a431957cdf02b8f3ab6c |
| cHandlingDataMgr.actual-before.obj | 9fa39ea85dbc85fb4563d41a3ea6b2b9b24714fbda6a497c8bfdd9ac6d2fa694 |
