<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
