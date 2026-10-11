<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# GangWarsPending

Clase: `CGangWars`.

Validado: 3072 pares aceptados. Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gang-wars-pending/20261007-194741-587537`.

| Función | Dirección EXE | Pares |

| --- | --- | --- |

| `AddKillToProvocation` | `0x443950` | 1024 |

| `CreateAttackWave` | `0x444810` | 1024 |

| `Update` | `0x446610` | 1024 |

## Evidencia y ejecución

La reconstrucción usa el pseudocódigo REA y el ensamblador conservados en `combo/`. El ensamblador determina convenciones de llamada, orden de efectos y conversiones. `build_probe.py` declara las fuentes contrastadas; `run_local.py` ejecuta la comparación mediante el banco compartido. Total esperado: 3072 pares.

Cada caso ejecuta EXE y DLL con los mismos fixtures, conmuta y verifica el hook, registra `pair`, `equal` y `accepted`, y procesa mensajes durante la prueba. Los snapshots globales y los parches temporales se restauran al finalizar. Se rechaza cualquier diferencia o gate de cobertura incompleto.

## Alcance

Provocación y zonas, transiciones ofensivas y defensivas, temporizadores, streaming, aparición de peatones y vehículos, armas y tareas. Compara estados observables y trazas ordenadas; exige ramas activas y vacías, peatones y vehículos.

## Límites

Entidades, tareas, grupos, radar y streaming son dependencias controladas. CalculateTimeTillNextAttack recibió una corrección de ABI a uint32 respaldada por el ensamblador, pero no es una función validada por este banco. No comprueba una guerra completa con entidades reales.

Los resultados y las trazas se comparan estructuralmente; los floats observados se registran por sus bits. La igualdad dentro de fixtures controlados no certifica las dependencias sustituidas ni todas las situaciones posibles.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 3.072 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-gang-wars-pending\20261007-200639-362824/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
