<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# GroupPlacerPending

Clase: `CPedGroupPlacer`.

Validado: 3072 pares aceptados. Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-group-placer-pending/20261007-191444-157371`.

| Función | Dirección EXE | Pares |

| --- | --- | --- |

| `PlaceFormationGroup` | `0x5FC9B0` | 1024 |

| `PlaceChatGroup` | `0x5FCE80` | 1024 |

| `PlaceRandomGroup` | `0x5FD330` | 1024 |

## Evidencia y ejecución

La reconstrucción usa el pseudocódigo REA y el ensamblador conservados en `combo/`. El ensamblador determina convenciones de llamada, orden de efectos y conversiones. `build_probe.py` declara las fuentes contrastadas; `run_local.py` ejecuta la comparación mediante el banco compartido. Total esperado: 3072 pares.

Cada caso ejecuta EXE y DLL con los mismos fixtures, conmuta y verifica el hook, registra `pair`, `equal` y `accepted`, y procesa mensajes durante la prueba. Los snapshots globales y los parches temporales se restauran al finalizar. Se rechaza cualquier diferencia o gate de cobertura incompleto.

## Alcance

Grupos y peatones privados, colocación, densidad, cámara, alturas, formación y tareas. Compara retorno, estado y trazas; exige éxito y fallo de cada función.

## Límites

Creación, agrupación, tareas y consultas del mundo usan dependencias controladas. No valida su ejecución real ni grupos ya existentes en el mapa.

Los hashes de estado del banco usan FNV-1a de 64 bits: valor inicial 1469598103934665603, XOR por byte y multiplicación por 1099511628211 con desbordamiento uint64. Los demás datos se comparan estructuralmente; los floats observados se registran por sus bits cuando corresponde. La igualdad dentro de fixtures controlados no certifica las dependencias sustituidas ni todas las situaciones posibles.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 3.072 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-group-placer-pending\20261007-200600-010291/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
