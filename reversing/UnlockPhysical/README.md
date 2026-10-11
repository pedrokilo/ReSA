<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de CPhysical

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**3.072 comparaciones exactas aprobadas** con Add y RemoveAndAdd habilitados y sin bloqueo desde el arranque. [Resumen verificable](validation-summary.json).

Se compararon 1.024 altas y 2.048 redistribuciones: nodos reutilizados, listas vacías, retirada de sobrantes, peatones/objetos/vehículos, límites negativos del mapa y delegación de edificios grandes. El banco usa pools privados y compara listas, cursores, referencias y sectores; las llamadas base de entidad se controlan con dobles comunes.

La conversión de coordenadas conserva x87 y la máscara de sectores del original. No se modificó el helper global de sectores. El ejecutor restaura ASI, configuración, pools y mundo, y comprueba partidas por hash.

No cubre agotamiento de pools, colisiones prolongadas ni estabilidad general. Las aserciones y fallos RTC terminan automáticamente y quedan registrados. Audio y SurfaceInfos excluidos.

Ejecución autónoma: `python reversing/UnlockPhysical/run_local.py`.
