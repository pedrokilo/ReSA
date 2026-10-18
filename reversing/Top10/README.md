<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Diez clases completadas y validadas

> Informe de esta entrega, conservado como evidencia histórica. El estado instalado más reciente y las entregas posteriores se documentan en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente). Los hashes y pruebas de este documento corresponden a su propia entrega.

[Inicio](../../README.md) · [Progreso](../../docs/ReversedClasses.md) · [Prioridades restantes](../../docs/ReversingPriorities.md) · [Resumen verificable](validation-summary.json)

**ReSA, 23 de septiembre de 2026: 10 clases cerradas, 22 entradas nuevas habilitadas y 51.712 pares sin diferencias dentro del motor.** Las cuatro sondas finales se compilaron después de habilitar las diez clases; sus manifiestos coinciden con las fuentes usadas para la compilación normal.

| Clase | Revertidas / registradas | Pares | Evidencia y límites |
| --- | ---: | ---: | --- |
| `CTaskComplexGoToPointShooting` | **3/3** | 2.048 | [ShootingResources](../ShootingResources/README.md) |
| `CScriptResourceManager` | **4/4** | 14.336 | [ShootingResources](../ShootingResources/README.md) |
| `CTaskComplexLeaveCarAndWander` | **8/8** | 3.072 | [LeaveCarBus](../LeaveCarBus/README.md) |
| `CTaskComplexWaitForBus` | **7/7** | 2.560 | [LeaveCarBus](../LeaveCarBus/README.md) |
| `CPopCycle` | **15/15** | 8.192 | [PopulationFactory](../PopulationFactory/README.md) |
| `CCarCtrl` | **22/22** | 2.048 | [PopulationFactory](../PopulationFactory/README.md) |
| `CTaskAllocatorKillThreatsBasic` | **6/6** | 8.192 | [PopulationFactory](../PopulationFactory/README.md) |
| `CEntityScanner` | **4/4** | 4.096 | [ScannerDragFx](../ScannerDragFx/README.md) |
| `CTaskComplexDragPedFromCar` | **2/2** | 4.096 | [ScannerDragFx](../ScannerDragFx/README.md) |
| `FxPrimBP_c` | **2/2** | 1.024 | [ScannerDragFx](../ScannerDragFx/README.md) |

Los pares de recursos incluyen 2.048 de Initialise, ya registrada. Se añaden **2.048 del constructor Aiming**, corregido al validar Shooting: total 51.712 sobre 24 funciones. Aiming continúa 5/8. El 100% de estas diez clases se refiere a sus entradas registradas; no significa haber probado exhaustivamente cada función anterior.

## Qué se corrigió

Se contrastaron vtables, ABI, relleno, referencias, transiciones, selección aleatoria, despacho de constructores, asignación de amenazas, sectores y carga FX. La revisión encontró diferencias reales en candidatos que parecían pequeños. Shooting sí clona Shooting: el original construye primero Aiming y luego cambia la vtable. El registro virtual del escáner y el adaptador de Load evitan interceptar thunks recursivos.

## Repetir sin supervisión

```powershell
python reversing/run.py --case shooting-resources
python reversing/run.py --case leave-car-bus
python reversing/run.py --case population-factory
python reversing/run.py --case scanner-drag-fx
```

Cada comando termina solo, audita sus logs y restaura los archivos. También se incluyen en `PROBAR.cmd` y `--all`. Los binarios de sonda son artefactos locales archivados; no se descargan ni recompilan al ejecutar el caso.

## Instalación de esta entrega y límites

Al cerrar esta entrega se instaló el ASI normal sin instrumentación únicamente en `D:\JUEGOS\GTA San Andreas - CVehicle Test`. SHA-256: `4bfe388ecf97ee2ad8c7d4a296bdc2d720e69d29f7fda000312890f5276a9805`. Respaldo: `D:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline\before-top10-sdl-20260923`. Instalación personal intacta; partidas y ambos INI conservados.

Los bancos excluyen Audio y SurfaceInfos y usan dependencias controladas donde se especifica. El original de la fábrica también falla si no puede reservar una moto/BMX: se comprobó y preservó esa peculiaridad, no se reparó. No se revalidaron todos los constructores ni animaciones, rutas, render completo o juego prolongado.

FastLoader solicita la ranura 2 durante las pruebas; el log indica rechazo por versión del guardado y comienzo de partida nueva. La configuración normal vuelve al menú y deja elegir al usuario. Estas comparaciones **no garantizan estabilidad general**. La campaña integral continúa aplazada hasta completar todas las clases.

CSV actualizado después de validar; ReversedClasses regenerado y prioridades revisadas. Estado global: **614 completas, 85 parciales, 2 sin revertir; 7.485/8.014 entradas y 529 pendientes**. Sin commit ni push.

> **Nota de vigencia:** ese estado global corresponde al cierre histórico de esta entrega. El catálogo actual se amplió tras la auditoría de owners y se consulta en [ReversedClasses.md](../../docs/ReversedClasses.md); no reinterpretar los resultados del banco con los denominadores actuales.

## Comprobación histórica de inicio normal y reparación de SDL

Tras los bancos, el ASI normal se detenía en el estado del logo durante los 180 segundos de plazo. Una segunda prueba registró GTA en primer plano: no era falta de foco. La ruta SDL no reenviaba WM_GRAPHNOTIFY al manejador de DirectShow; se añadió ese reenvío para los estados de logo e introducción, conservando el manejador ya existente.

Con esa corrección, el ASI normal **sin argumentos ni exclusiones** recorrió los estados **0, 1, 2, 3, 4, 5, 7**, llegó al menú en **130.49 segundos** y permaneció allí diez segundos. No se enviaron teclas, no se saltaron pantallas por configuración y no se entró en los estados de carga/juego. [Evidencia de aquella ejecución](../InicioNormal/artifacts/validation-before-unlocks.json) · [Método y comprobación vigente](../InicioNormal/README.md).

Este ajuste de integración se comprobó después de los cuatro bancos; sus fuentes de reversing no cambiaron. Las sondas archivadas preceden al reenvío SDL. El chequeo de arranque cubre la compilación normal final, Debug x86, y no sustituye una partida prolongada.
