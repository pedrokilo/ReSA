<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TrackEntity: cuatro hooks desbloqueados

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**4/4 activos y conmutables**, con **9.216 comparaciones aprobadas dentro del motor**. [Resumen verificable](validation-summary.json).

Se implementaron `CalcTargetPos` y `CalcMoveRatio`, antes llamadas al original. Se corrigieron la caducidad (inicio + intervalo), la rama `FollowNodeRoute` y el orden de consultas virtuales. El controlador original reutiliza `ECX` después de `GoToPointFine::SetTargetPos`; un adaptador preserva ese registro. El setter también debe conservar los cuatro bits altos y activar 0x20, en lugar de borrarlo por una máscara incorrecta.

| Función | Pares |
| --- | ---: |
| SetOffsetPos | 512 |
| CalcTargetPos | 2.048 |
| CalcMoveRatio | 4.096 |
| ControlSubTask | 2.048 |
| GoToPointFine::SetTargetPos, dependencia | 512 |

Se compararon bytes, velocidades, distancias, límites próximos a los umbrales, posición/rotación, tipos de entidad, temporizadores activos/reiniciados/caducados, desbordamiento de tiempo, devolución de subtarea, abortos, factory y flags del hijo. El controlador original llama al setter reconstruido, comprobando así el contrato de ECX. Los primeros intentos detectaron diferencias en consultas de tipo; se corrigieron antes de la pasada final. No se suman repeticiones como nueva cobertura.

Limitaciones: matrices preasignadas; subtarea y factory controladas. Se valida la decisión de transición y sus efectos observados, no rutas completas ni navegación prolongada. El constructor de TrackEntity queda fuera de esta tanda. Audio y SurfaceInfos excluidos. No garantiza estabilidad general.

Ejecutar `build_probe.py`, `archive_probe.py`, `run_local.py` desde la raíz. La sonda temporal restaura fuentes, hooks, reloj y estáticos de cálculo; el runner restaura ASI/INI y comprueba partidas. Sin instalación personal modificada. La [entrega parcial y su integración normal](../UnlockBatch2/README.md) agrupa estos cuatro desbloqueos con DataStream.

Se repitieron los 9.216 pares con los borradores de CFileMgr y Game fuera de las fuentes compilables; la pasada final del resumen corresponde a esa base aceptada. La repetición no aumenta la cobertura contabilizada.
