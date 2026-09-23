# Desbloqueo de animaciones de conducción

**16.640 comparaciones exactas aprobadas**, con el hook habilitado y sin bloqueo: 16.384 ProcessDrivingAnims y 256 GetVehicleAppearance. [Resumen verificable](validation-summary.json).

El caller original conserva ECX y EDX a través de GetVehicleAppearance. Un adaptador mantiene ambos registros, comprobados expresamente junto al retorno para combinaciones de flags de manejo. Se corrigieron orden de consultas de habilidad y aritmética x87 de velocidad y peso de animaciones.

El banco usa las tablas reales de grupos y la dependencia GetVehicleAppearance reparada. Controla asociaciones, consultas de habilidad y mezclas, comparando el orden de llamadas y todos los bytes de las asociaciones consultadas. Cubre vehículos bajos, barcos, kart, camiones, niveles de habilidad, umbral de velocidad, marcha atrás, dirección, disparo desde vehículo y cámara hacia atrás.

Se restauran cámara, flags, hooks, ASI e INI, con partidas verificadas por hash. Audio y SurfaceInfos excluidos. No cubre todas las animaciones reales, render, NaN/infinito, conducción prolongada ni estabilidad general. Los intentos fallidos previos no cuentan como aprobación.

Ejecución autónoma: `python reversing/UnlockDriving/run_local.py`.
