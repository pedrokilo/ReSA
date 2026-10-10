<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de tareas de combate

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**7.168 comparaciones aprobadas dentro del motor**, con los tres hooks habilitados y sin bloqueo desde el arranque. [Resumen verificable](validation-summary.json).

| Función | Pares |
| --- | ---: |
| CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges | 4.096 |
| CTaskComplexDestroyCarArmed::CreateSubTask | 1.024 |
| CTaskSimpleThrowControl::ProcessPed | 2.048 |

Se comparan campos, retornos, tipos de subtarea, referencias, ocupación del pool y rotación. Las fábricas se prueban con constructores originales y reversados; el banco detectó y corrigió el registro del destructor de SeekEntityStandard, que apuntaba al wrapper de liberación en lugar de al destructor. También se corrigió el orden de intervalos de su adaptador de construcción.

Las entidades y referencias son privadas. La información del arma se controla mediante un doble común; no se valida aquí la selección global de armas. Se restauran hooks, referencias y estado del jugador. El ejecutor restaura ASI, ambos INI y comprueba los hashes de las partidas. Audio y SurfaceInfos quedan excluidos en este banco.

No cubre combate prolongado, todas las animaciones, fallo de asignación ni estabilidad general. Los intentos previos fallidos se conservan como diagnóstico y no suman cobertura.

Ejecución autónoma: `python reversing/UnlockTasks/run_local.py`. Integrado como `unlock-tasks` en el lanzador común y en `--all`.
