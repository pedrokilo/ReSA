<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Salir del coche y esperar el autobús

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

[Informe de las diez clases](../Top10/README.md) · [Resumen verificable](validation-summary.json) · [Catálogo de pruebas](../README.md)

**Resultado final: 5.632 pares exactos, cero diferencias.** Segunda pasada con los hooks objetivo habilitados y desbloqueados. Las primeras ejecuciones se usaron para corregir los candidatos y no se suman como cobertura distinta.

| Clase | Entradas registradas | Comparaciones |
| --- | ---: | ---: |
| `CTaskComplexLeaveCarAndWander` | 8/8 | 3.072 |
| `CTaskComplexWaitForBus` | 7/7 | 2.560 |

| ID en el log | Función comparada | Pares |
| ---: | --- | ---: |
| 0 | `Leave::CreateSubTask` | 512 |
| 1 | `Leave::Clone` | 512 |
| 2 | `Leave::GetTaskType` | 512 |
| 3 | `Leave::CreateNextSubTask` | 512 |
| 4 | `Leave::CreateFirstSubTask` | 512 |
| 5 | `Leave::ControlSubTask` | 512 |
| 6 | `Bus::Clone` | 512 |
| 7 | `Bus::GetTaskType` | 512 |
| 8 | `Bus::CreateNextSubTask` | 512 |
| 9 | `Bus::CreateFirstSubTask` | 512 |
| 10 | `Bus::ControlSubTask` | 512 |

## Correcciones y cobertura

LeaveCarAndWander devuelve nullptr para tipos no reconocidos; distingue entrada/salida del coche y subtareas None/LeaveCar. Al terminar asigna Wander también cuando la tarea primaria anterior es nula. Se comprueban Clone, tipo, control, creación inicial, siguiente subtarea y fábrica.

WaitForBus copia el autobús desde la tarea de espera, crea EnterCarAsPassenger y compara flags de retirada y cobro de cinco unidades si el jugador es pasajero o conductor. Se eliminó una aserción que añadía una condición ajena al original. El clon reinicia su autobús mediante el constructor, igual que el original.

Se usan constructores y destructores reales de las tareas del banco, listas privadas de referencias y estados de ped/vehículo preparados. La creación de Wander y su asignación se interceptan en ambos lados para comparar argumentos y orden; la búsqueda del jugador es controlada. IsPassenger/IsDriver y el contador de tarifas se comprueban dentro del motor. Cada entrada recibe 512 casos.

## Límites

Las trazas comprueban decisiones, objetos, referencias y efectos, no una ruta completa de autobús, animación de salida ni navegación de Wander. Las dependencias controladas no se presentan como revalidadas por este banco.

## Ejecución y trazabilidad

La comparación se ejecuta dentro de GTA contra la entrada original, cambiando temporalmente el hook. Cada par exige cero llamadas al candidato en el lado original y una en el lado candidato. Los hooks objetivo están habilitados y sin bloqueo al entrar al banco. El ejecutor comprueba cobertura exacta, ausencia de diferencias, restauración y hashes de los archivos.

Se excluyen Audio y SurfaceInfos durante estos bancos, como en el ejecutor común. FastLoader solicita temporalmente la ranura 2; en este entorno el log rechaza la versión del guardado y empieza una partida nueva. No se fuerza la carga ni se altera el archivo. ASI/PDB e INI se restauran y ambas copias de la partida conservan su hash.

Los artefactos locales archivados no se incluyen en Git. `build_probe.py` genera la instrumentación y restaura las fuentes en finally; `run_local.py` usa la sonda archivada de hash aprobado, sin recompilar. El comando siguiente termina solo y cierra únicamente el GTA que abrió. También está disponible en el único PROBAR.cmd y en --all.

```powershell
python reversing/run.py --case leave-car-bus
```

Evidencia final local: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-leave-car-bus\20260923-014914-529111`. El resumen conserva el SHA-256 del log, sonda, fuentes y dependencias; los archivos extensos quedan fuera de Git.
