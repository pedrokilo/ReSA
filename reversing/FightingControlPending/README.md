<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CTaskSimpleFightingControl: ProcessPed validada

ProcessPed 0x62A0A0 revisado con combo.sh: no existe pseudocódigo Ghidra para esta dirección; evidencia disponible en combo-62A0A0.txt es ensamblador completo hasta ret 4 (0x62A378). **Validado el 07/10/2026: 1.024/1.024 pares exactos**, con cobertura positiva en las once ramas exigidas; estado, configuración y partidas restaurados. [Evidencia](validation-summary.json).

Se conservó la implementación existente y se habilitó su hook. Corrección: 0x62A263 llama a 0x407180, aleatorio entero exclusivo con factor 1/32768; se reemplazó la plantilla entera por GetRandomIntInRange. Los cálculos x87 existentes siguen el orden observado. La tabla 0x86D700 contiene nueve entradas y ProcessPed está en slot 7 (0x62A0A0).

Banco: 1024 pares original/candidato con conmutación verificada después de State(). Casos: terminación temprana, tarea ausente y creación, tarea de tipo distinto y aborto, ataque vencido, bloqueo aleatorio, decremento de bloqueo, distintos índices de combo (incluidos negativos), estilos, armas, tasa de disparo de estadísticas y misión/policía, ataque y movimiento. Compara retorno, memoria de tarea/peatón/combate y traza.

Dependencias controladas en ambas versiones: GiveWeaponAtStartOfFight, GetTaskSecondary, SetTaskSecondary, CTask::operator new, constructor de Fight, ControlFight, CalcMoveCommand y RNG base. Todas usan fixtures privados; la reserva devuelve un buffer privado, por lo que el banco no accede ni modifica el pool de tareas. Se restauran temporizador, parámetros de combate, tabla de combos y estado/bloqueo del hook. PumpMessages se invoca al escribir cada registro. La integración de las dependencias y la regresión de juego completo quedan fuera del banco.

Compilación con `/MP6` y validación dinámica aprobadas sobre las fuentes finales. Repetir con build_probe.py y run_local.py, con GTA cerrado.

Auditoría de cobertura: el primer banco usaba un literal incorrecto para el tipo de tarea y casi todas las ejecuciones abortaban una tarea distinta. Se sustituyó por TASK_SIMPLE_FIGHT y se distribuyeron 16 escenarios independientes de las 64 variaciones de combo/estadísticas. El resumen registra contadores de las ramas observadas y el banco exige presencia de terminación temprana, creación, aborto, ataque vencido, bloqueo aleatorio aceptado/rechazado, decremento restante/expirado, objetivo no peatón y CalcMoveCommand.

<!-- BEGIN PENDING BATCH REGRESSION -->

Regresión del 07/10/2026: **APROBADA**. 1.024 pares exactos. [Resumen actualizado](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fighting-control-pending\20261007-201515-804276/summary.json>).
<!-- END PENDING BATCH REGRESSION -->
