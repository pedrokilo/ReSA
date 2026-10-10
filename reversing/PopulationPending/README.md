<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# PopulationPending

Clase: `CPopulation`.

Validado: 2048 pares aceptados. Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-population-pending/20261007-193814-560382`.

| Función | Dirección EXE | Pares |

| --- | --- | --- |

| `ManageObject` | `0x615DC0` | 1024 |

| `AddToPopulation` | `0x614720` | 1024 |

## Evidencia y ejecución

La reconstrucción usa el pseudocódigo REA y el ensamblador conservados en `combo/`. El ensamblador determina convenciones de llamada, orden de efectos y conversiones. `build_probe.py` declara las fuentes contrastadas; `run_local.py` ejecuta la comparación mediante el banco compartido. Total esperado: 2048 pares.

Cada caso ejecuta EXE y DLL con los mismos fixtures, conmuta y verifica el hook, registra `pair`, `equal` y `accepted`, y procesa mensajes durante la prueba. Los snapshots globales y los parches temporales se restauran al finalizar. Se rechaza cualquier diferencia o gate de cobertura incompleto.

## Alcance

ManageObject compara trazas y hashes de objetos privados con eliminación, conversión y conservación. AddToPopulation compara creación, grupos, parejas, tareas, sol, disturbios, coordenadas y matrices; exige creación y ausencia de creación, grupos, parejas, sol y disturbios.

## Límites

Creación de entidades y tareas, cámara, colisión, modelos y scripts son dependencias controladas. Los constructores nativos usados para preservar ABI se sustituyen en ambos lados. No valida las dependencias ni la población completa del mundo.

Los hashes de ManageObject usan FNV-1a de 64 bits: valor inicial 1469598103934665603, XOR por byte y multiplicación por 1099511628211 con desbordamiento uint64. Los demás datos se comparan estructuralmente; los floats observados se registran por sus bits cuando corresponde. La igualdad dentro de fixtures controlados no certifica las dependencias sustituidas ni todas las situaciones posibles.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.048 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-population-pending\20261007-200446-913379/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
