<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
