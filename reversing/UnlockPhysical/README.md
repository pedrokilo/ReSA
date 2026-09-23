# Desbloqueo de CPhysical

**3.072 comparaciones exactas aprobadas** con Add y RemoveAndAdd habilitados y sin bloqueo desde el arranque. [Resumen verificable](validation-summary.json).

Se compararon 1.024 altas y 2.048 redistribuciones: nodos reutilizados, listas vacías, retirada de sobrantes, peatones/objetos/vehículos, límites negativos del mapa y delegación de edificios grandes. El banco usa pools privados y compara listas, cursores, referencias y sectores; las llamadas base de entidad se controlan con dobles comunes.

La conversión de coordenadas conserva x87 y la máscara de sectores del original. No se modificó el helper global de sectores. El ejecutor restaura ASI, configuración, pools y mundo, y comprueba partidas por hash.

No cubre agotamiento de pools, colisiones prolongadas ni estabilidad general. Las aserciones y fallos RTC terminan automáticamente y quedan registrados. Audio y SurfaceInfos excluidos.

Ejecución autónoma: `python reversing/UnlockPhysical/run_local.py`.
