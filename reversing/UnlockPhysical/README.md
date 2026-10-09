<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de CPhysical

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**3.072 comparaciones exactas aprobadas** con Add y RemoveAndAdd habilitados y sin bloqueo desde el arranque. [Resumen verificable](validation-summary.json).

Se compararon 1.024 altas y 2.048 redistribuciones: nodos reutilizados, listas vacías, retirada de sobrantes, peatones/objetos/vehículos, límites negativos del mapa y delegación de edificios grandes. El banco usa pools privados y compara listas, cursores, referencias y sectores; las llamadas base de entidad se controlan con dobles comunes.

La conversión de coordenadas conserva x87 y la máscara de sectores del original. No se modificó el helper global de sectores. El ejecutor restaura ASI, configuración, pools y mundo, y comprueba partidas por hash.

No cubre agotamiento de pools, colisiones prolongadas ni estabilidad general. Las aserciones y fallos RTC terminan automáticamente y quedan registrados. Audio y SurfaceInfos excluidos.

Ejecución autónoma: `python reversing/UnlockPhysical/run_local.py`.
