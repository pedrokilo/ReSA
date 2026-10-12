<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de animaciones de conducción

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**16.640 comparaciones exactas aprobadas**, con el hook habilitado y sin bloqueo: 16.384 ProcessDrivingAnims y 256 GetVehicleAppearance. [Resumen verificable](validation-summary.json).

El caller original conserva ECX y EDX a través de GetVehicleAppearance. Un adaptador mantiene ambos registros, comprobados expresamente junto al retorno para combinaciones de flags de manejo. Se corrigieron orden de consultas de habilidad y aritmética x87 de velocidad y peso de animaciones.

El banco usa las tablas reales de grupos y la dependencia GetVehicleAppearance reparada. Controla asociaciones, consultas de habilidad y mezclas, comparando el orden de llamadas y todos los bytes de las asociaciones consultadas. Cubre vehículos bajos, barcos, kart, camiones, niveles de habilidad, umbral de velocidad, marcha atrás, dirección, disparo desde vehículo y cámara hacia atrás.

Se restauran cámara, flags, hooks, ASI e INI, con partidas verificadas por hash. Audio y SurfaceInfos excluidos. No cubre todas las animaciones reales, render, NaN/infinito, conducción prolongada ni estabilidad general. Los intentos fallidos previos no cuentan como aprobación.

Ejecución autónoma: `python reversing/UnlockDriving/run_local.py`.
